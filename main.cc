#include <bits/stdc++.h>
#include "../slex/debug.h"
using namespace std;

// hash set: phmap > pbds > std
#include "parallel_hashmap/phmap.h"
using N = phmap::flat_hash_set<int>;
// #include <ext/pb_ds/assoc_container.hpp>
// using namespace __gnu_pbds;
// using N = gp_hash_table<int, null_type>;
// using N = unordered_set<int>;

struct HashN {
  inline size_t operator()(const N& x) const {
    size_t h = x.size();
    for (auto& i: x) h ^= hash<int>()(i);
    return h;
  }
};

struct EqualN {
  inline bool operator()(const N& a, const N& b) const {
    if (a.size() != b.size()) return false;
    for (auto& i: a) if (b.find(i) == b.end()) return false;
    return true;
  }
};

enum { SPACE = 128, ID, CHAR, SPLIT };

int token;

// A simple lexer for parsing grammar.
---
:
  token = ':';
;
  token = ';';
\|
  token = '|';
{
  token = '{';

[a-zA-Z][a-zA-Z0-9_]*
  token = ID;

'(\\[^]|[^\\'\n])'
  token = CHAR;

[ \t\n]+
  token = SPACE;

\-\-\-[^\n]*
  token = SPLIT;
---

string char_symb(char c) {
  return "_CHAR_" + to_string((int)c);
}

string unchar(string s) {
  if (s.rfind("_CHAR_", 0) == 0) {
    return string() + "'" + (char)stoi(s.substr(6, s.size() - 6)) + "'";
  }
  return s;
}

int nsymbs;
struct S { string s; int term, nullable; };
unordered_map<string, int> symbi;
vector<S> symb;
vector<vector<int>> first;

/*
 * Let X be any non-terminal symbol, and s be any symbol, then
 * - first[s] is the set of terminals that can begin strings derived from s.
 * - follow[X] is the set of terminals that can immediately follow X.
 */
int addsymb(const string& s) {
  auto& si = symbi[s];
  if (!si) symb.resize((si = ++nsymbs) + 1), symb[si] = {s, 1, 0};
  return si;
}

// Productions
int max_dot;
vector<vector<int>> prod;
vector<string> prod_action;
unordered_map<int, vector<int>> phead;  // phead[i]: Indices of production whose lhs symbol is i.
struct I {
  int pi, dot; // dot: Index of symbol in production after dot
  int ahead;
  // set<int> ahead;
  bool operator==(const I& o) const {
    return pi == o.pi && dot == o.dot && ahead == o.ahead;
  }
};

void addprod(const vector<int>& p, string f) {
  phead[p[0]].push_back(prod.size());
  prod.push_back(p);
  max_dot = max(max_dot, (int)p.size() + 1);
  if (f.empty()) {
    if (p.size() == 2) f = "{ return $1; }";
    else f = "{ Node d; return d; }";
  }
  prod_action.push_back(f);
}

int initnf_us = 0;
/* Init nullable and first set. */
void init_nf() {
  cerr << "--- init_nf begin.\n";

  auto begin = chrono::steady_clock::now();

  auto firsts = vector<set<int>>(nsymbs + 1, set<int>());
  for (int i = 1; i <= nsymbs; i++) {
    symb[i].nullable = 0;
    if (symb[i].term) firsts[i].insert(i);
  }
  
  for (int stop = 0; !stop; ) {
    stop = 1;
    for (auto& p: prod) {
      int u = p[0], allnull = 1;
      int fs = firsts[u].size(), ns = symb[u].nullable;
      for (int i = 1; i < p.size() && allnull; i++) {
        int v = p[i];
        if (allnull) firsts[u].insert(firsts[v].begin(), firsts[v].end());
        if (!symb[v].nullable) allnull = 0;
      }
      symb[u].nullable |= allnull;

      if (fs != firsts[u].size() || ns != symb[u].nullable) stop = 0;
    }
  }
  first.resize(nsymbs + 1);
  for (int i = 0; i <= nsymbs; i++)
    first[i] = vector<int>(firsts[i].begin(), firsts[i].end());

  for (int i = 1; i <= nsymbs; i++) {
      vector<string> firstset;
      for (auto u: first[i]) firstset.push_back(symb[u].s);
      debug(symb[i].s, firstset);
  }
  auto end = chrono::steady_clock::now();
  initnf_us += chrono::duration_cast<chrono::microseconds>(end - begin).count();
  cerr << "--- init_nf end.\n";
}

struct C {
  int pi, dot, ahead;
  vector<int> adj;
};
vector<C> node;

inline int encode(int pi, int dot, int a) {
  return pi * (max_dot + 1) * (nsymbs+1) + dot * (nsymbs+1) + a;
}

int cgraph_us;
void init_cgraph() {
  auto begin = chrono::steady_clock::now();

  vector<bool> added(nsymbs + 1, false);
  node = vector<C>(prod.size() * (max_dot + 1) * (nsymbs + 1));

  for (int pi = 0; pi < prod.size(); pi++) {
    for (int a = 1; a <= nsymbs; a++) if (symb[a].term) {
      for (int dot = 1; dot <= prod[pi].size(); dot++) {
        int i = encode(pi, dot, a);
        node[i] = {pi, dot, a, vector<int>()};

        if (dot < prod[pi].size()) {
          vector<int>& p = prod[pi];
          vector<int> fr;
          p.push_back(a);
          for (int i = dot + 1; i < p.size(); i++) {
            int si = p[i];
            for (int i: first[si]) if (!added[i]) added[i] = true, fr.push_back(i);
            if (!symb[si].nullable) break;
          }
          p.pop_back();
          for (int i: fr) added[i] = false;

          for (int pj: phead[prod[pi][dot]])
            for (int u: fr)
              node[i].adj.push_back(encode(pj, 1, u));
        }
      }
    }
  }
  for (auto& c: node) {
    sort(c.adj.begin(), c.adj.end());
    c.adj.resize(unique(c.adj.begin(), c.adj.end()) - c.adj.begin());
  }
  auto end = chrono::steady_clock::now();
  cgraph_us += chrono::duration_cast<chrono::microseconds>(end - begin).count();
}

/* Hashing */
// namespace std {
//   template<typename T1, typename T2>
//   struct hash<pair<T1, T2>> {
//     size_t operator()(const pair<T1, T2>& p) const {
//       return hash<T1>()(p.first) ^ (hash<T2>()(p.second) << 10);
//     }
//   };
// }

enum { EMPTY, SHIFT, GOTO, REDUCE, ACCEPT }; // Should be identical to output()
struct A {
  int type, i;
  bool operator==(const A& o) const {
    return i == o.i && type == o.type;
  }
};

/* For debug. */

// shift/goto { type, si; }
// reduce { type, pi; }
string to_string(const A& a) {
  return "{ .type = " + to_string(a.type) + ", .i = " + to_string(a.i) + " }";
}

string to_string(const S& s) {
  return "{ s: " + s.s + ", term: " + to_string(s.term) + ", nullable: " + to_string(s.nullable) + " }\n";
}

string to_string(const I& t) {
  int i, n = prod[t.pi].size();
  string res;
  for (i = 0; i < n; i++) {
    if (i == t.dot) res += ". ";
    res += unchar(symb[prod[t.pi][i]].s) + " ";
    if (i == 0) res += "=> ";
  }
  if (i == t.dot) res += ". ";
  res += ", ahead: " + unchar(symb[t.ahead].s);
  return res + '\n';
}

string to_string(const N& n) {
  vector<I> s;
  for (auto i: n) { s.push_back({node[i].pi, node[i].dot, node[i].ahead}); }
  return to_string(s);
}

void output(const vector<vector<A>>& table) {
  struct B { pair<int, int> i; A a; };
  vector<B> t;

  string code;

  // Headers.
  code += R"(
#include <iostream>
#include <vector>
#include <cassert>

)";

  // Export symbol index, symb[0] is EOF.
  // Add 128 to distinguish from char.
  for (int i = 1; i < symb.size(); i++)
    if (symb[i].term)
      code += "const int " + symb[i].s + " = " + to_string(i + 128) + ";\n";

  // Table for hacking char symbols.
  vector<int> char_map;
  char_map.push_back(symbi["$end"]); // '\0' is $end.
  for (int i = 1; i < 128; i++) {
    auto it = symbi.find(char_symb(i));
    char_map.push_back(it != symbi.end() ? it->second: 0);
  }
  code += "static int char_map[] = " + to_string(char_map) + ";\n";

  // Table entry struct.
  // Should be identical to those in parser source code.
  code += R"(
enum { EMPTY, SHIFT, GOTO, REDUCE, ACCEPT };
struct A { int type, i; };
)";
  code += "static int nsymbs = " + to_string(table[0].size()) + ";\n";
  code += "static int nstats = " + to_string(table.size()) + ";\n";

  // The LR table.
  code += "static struct A table[][" + to_string(table[0].size()) + "] = " + to_string(table) + ';';

  // Production rules len and first symbol(target symbol).
  vector<int> prod_first, prod_len;
  int max_prod_len = 0;
  for (auto& p: prod) {
    prod_first.push_back(p[0]);
    prod_len.push_back(p.size());
    max_prod_len = max(max_prod_len, (int)p.size());
  }
  code += "static int prod_len[] = " + to_string(prod_len) + ";\n";
  code += "static int prod_first[] = " + to_string(prod_first) + ";\n";

  // Production action functions.
  for (int i = 0; i < prod.size(); i++) {
    code += "Node prod_" + to_string(i) + "(";
    for (int j = 1; j < prod[i].size(); j++) {
      code += "Node $" + to_string(j);
      if (j != prod[i].size() - 1) code += ",";
    }
    code += ") " + prod_action[i] + "\n";
  }

  // Helper functions.
  code += R"(
/* Node is provided by user. */
struct I {
  int state;
  Node data;
};

static std::vector<I> stk;

void init_parser() {
  stk.push_back({1});
}

int parse(int symb_raw, Node data) {
  int symb;
  if (symb_raw < 128) symb = char_map[symb_raw];
  else symb = symb_raw - 128;

  assert(symb < nsymbs);
  // cerr << "symb: " << symb << '\n';

  struct A a = table[stk.back().state][symb];
  if (a.type == SHIFT) {
    stk.push_back({a.i, data});
    cerr << "parser: shift to state " << a.i << "\n";
    return 0;
  } else if (a.type == REDUCE) {
    cerr << "parser: reduce ";
    int stklen;
    Node ret;
    switch(a.i) {
)";
  for (int i = 0; i < prod.size(); i++) {
    code +=
"   case " + to_string(i) + ": " +
      // For debug
      "cerr << \"prod \" << " + to_string(i) + " << \": \" << " + to_string(unchar(symb[prod[i][0]].s)) + " << \" -> \";";
    for (int j = 1; j < prod[i].size(); j++) code += "cerr << " + to_string(unchar(symb[prod[i][j]].s)) + " << ' ';";
    code += "cerr << '\\n';";
      
    code += "stklen = stk.size(); ret = prod_" + to_string(i) + "(";

    for (int j = 1; j < prod[i].size(); j++) {
      int jj = prod[i].size() - j;
      code += "stk[stklen-" + to_string(jj) + "].data";
      if (j < prod[i].size() - 1) code += ", ";
    }

    code += "); stk.resize(stklen - " + to_string(prod[i].size() - 1) + "); break;\n";
  }

  code += R"(
    default: assert(0);
    }
    assert(!stk.empty());
    a = table[stk.back().state][prod_first[a.i]];
    assert(a.type == GOTO);
    stk.push_back({a.i, ret});
    cerr << "parser: goto state " << a.i << "\n";
    return parse(symb_raw, data);

  } else if (a.type == ACCEPT) {
    cerr << "parser: accept\n";
    return 1;
  } else {
    cerr << "unexpected transition. ";
    assert(0);
  }
  return 0;
}


)";

  cout << code;
  debug(t.size());
}

int closure_us = 0, nclosure = 0, nbfs = 0;
inline void closure(N& s) {
  static unordered_map<N, N, HashN, EqualN> cache;
  auto begin = chrono::steady_clock::now();
  auto& t = cache[s];
  if (t.size()) s = t;
  else {
    queue<int> q;
    for (int i: s) q.push(i);
    for (; !q.empty(); q.pop()) {
      nbfs++;
      for (int v: node[q.front()].adj)
        if (s.insert(v).second)
          q.push(v);
    }
    t = s;
  }
  closure_us += chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - begin).count();
  nclosure++;
}

int gotox_us = 0, ngotox = 0, ncore = 0, ncores = 0;
auto gotox(const N& s, int x) {
  auto begin = chrono::steady_clock::now();
  assert(s.size());

  N t;
  for (int u: s) {
    int pi = node[u].pi, dot = node[u].dot;
    if (dot < prod[pi].size() && prod[pi][dot] == x) {
      t.insert(encode(pi, dot+1, node[u].ahead));
      ncore++;
    }
    ncores++;
  }

  gotox_us += chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - begin).count();
  ngotox++;

  closure(t);
  assert(t.size());
  return t;
}

int getstate_us = 0, ngetstate = 0;
void build() {
  int nterms = 0;
  assert(nsymbs + 1 == symb.size());
  for (int i = 1; i <= nsymbs; i++) if (symb[i].term) nterms++;
  debug(nterms, nsymbs, prod.size());

  vector<vector<A>> table;

  int nstates = 0;
  unordered_map<int, N> state;
  unordered_map<N, int, HashN, EqualN> statei;

  queue<int> q;

  auto getstate = [&](const N& s) {
    auto begin = std::chrono::steady_clock::now();

    int& u = statei[s];
    if (!u) {
      q.push(u = ++nstates);
      state[u] = s;
      ngetstate++;
      table.resize(u+1, vector<A>(nsymbs+1, {EMPTY, 0}));
    }

    auto end = std::chrono::steady_clock::now();
    getstate_us += chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    return u;
  };

  debug(prod), debug(phead), debug(symb);

  assert(symb[2].s == "$end" && symb[2].term); // look ahead of start node should be terminal.

  N s0;
  s0.insert(encode(0, 1, 2));
  closure(s0);
  int u0 = getstate(s0);
  assert(u0 == 1);
  debug(state[u0]);

  for (; q.size(); q.pop()) {
    int u = q.front();
    const auto& s = state[u];

    vector<bool> vis(nsymbs + 1, false);
    for (auto& ns: s) {
      I i = {node[ns].pi, node[ns].dot, node[ns].ahead};
      auto& p = prod[i.pi];
      if (i.dot < p.size()) {
        int x = p[i.dot];

        if (vis[x]) continue;
        vis[x] = true;

        if (symb[x].s == "$end") {
          auto& ent = table[u][symbi["$end"]];
          assert(ent.type == EMPTY || ent.type == ACCEPT);
          ent = { ACCEPT, 0 };
          continue;
        }
        int v = getstate(gotox(s, x));
        A a = {symb[x].term ? SHIFT: GOTO, v};

        assert(1 <= x && x <= nsymbs);
        auto& t = table[u][x];
        if (t.type == REDUCE && a.type == SHIFT) {
          cerr << "Shift-reduce conflict found on table[" << u << "][" << x << "].\n";
          // assert(0);
        } else if (!(table[u][x].type == EMPTY || table[u][x] == a)) {
          assert(table[u][x].type == EMPTY || table[u][x] == a);
        }
        t = a;
      } else {
        assert(i.dot == p.size() && 1 <= i.ahead && i.ahead <= nsymbs);
        A a = {REDUCE, i.pi};
        auto& t = table[u][i.ahead];

        /* Resolves shift-reduce conflicts by shifting. */
        if (t.type == SHIFT) {
          cerr << "Shift-reduce conflict found on table[" << u << "][" << i.ahead << "].\n";
          // assert(0);
          continue;
        }

        if (!(t.type == EMPTY || t == a)) {
          assert(table[u][i.ahead].type == EMPTY || table[u][i.ahead] == a);
        }
        table[u][i.ahead] = a;
      }
    };
  }
  assert(table.size() == nstates + 1 && table[0].size() == nsymbs + 1);
  debug(nsymbs, nterms, prod.size(), max_dot, nstates, state.size());
  // output(table);
}

int main() {
  for (string s; getline(cin, s); )
    if (s.size() >= 3 && s.substr(0, 3) == "---") break;
    else cout << s << '\n';

  string str;
  for (string s; getline(cin, s); )
    str += s + '\n';

  /*
   * Add the start symbol and production
   * NOTE:
   * - Legal token should not contain '$' (without quote),
   *   which is guaranteed by the lexer above. 
   * - The third token is the first symbol provided by user.
   */
  vector<int> p = {addsymb("$start"), 3, addsymb("$end")};
  string action = "";
  addprod(p, action);
  assert(prod.size() == 1); // The first production is special and will "accept" when eating "$end".
  assert(symbi["$start"] == 1 && symbi["$end"] == 2);
  symb[1].term = 0;
  p.resize(0);

  for (const char *s = str.c_str(); *s; ) {
    int n = nxt((char *)s);
    string t;
    switch (token) {
    // TODO: Currently only support single line action function.
    case '{':
      assert(n == 1);
      for (; *s != '\n'; s++) action += *s;
      break;

    case ';': symb[p[0]].term = 0, addprod(p, action), p.resize(0), action = ""; break;
    case '|': addprod(p, action), p.resize(1), action = ""; break;

    case CHAR:
      // TODO: Do not support length > 1 (e.g '\n') for now.
      assert(n == 3);
      p.push_back(addsymb(char_symb(s[1])));
      break;
    case ID:
      t = str.substr(s - str.c_str(), n);
      p.push_back(addsymb(t));
      break;

    case ':':
      assert(p.size() == 1 && "unexpected target");
    case SPACE: break;

    case SPLIT:
      init_nf();
      init_cgraph();
      build();
      debug(initnf_us, cgraph_us, closure_us, gotox_us, getstate_us);
      debug(nbfs, nclosure, ncore, ncores, ngotox, ngetstate);

      n += s - str.c_str();
      cout << str.substr(n, str.size() - n);
      return 0;

    default: assert(0 && "unexpected token");
    }
    if (token != SPACE)
      cerr << "find token " << token << " of length " << n << ": " << str.substr(s - str.c_str(), n) << '\n';
    s += n;
  }
  return 0;
}
