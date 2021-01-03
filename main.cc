#include <bits/stdc++.h>
#include "../slex/debug.h"
using namespace std;

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

/* For debug */
string to_string(const S& s) {
  return "{ s: " + s.s + ", term: " + to_string(s.term) + ", nullable: " + to_string(s.nullable) + " }\n";
}

/*
 * Let X be any non-terminal symbol, and s be any symbol, then
 * - first[s] is the set of terminals that can begin strings derived from s.
 * - follow[X] is the set of terminals that can immediately follow X.
 */

int addsymb(const string& s) {
  auto& si = symbi[s];
  if (!si) symb.resize((si = ++nsymbs) + 1), symb[si] = {s, 1, 0};
  // if (!si) si = ++nsymbs, symb[si] = {s, 1, 0};
  return si;
}

// Productions
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
  // bool operator<(const I& o) const {
  //   return pi < o.pi || (pi == o.pi && (dot < o.dot || (
  //     dot == o.dot && ahead < o.ahead
  //   )));
  // }
};

/* Hashing */
namespace std {
  template<typename T1, typename T2>
  struct hash<pair<T1, T2>> {
    size_t operator()(pair<T1, T2> const &p) const {
      return hash<T1>()(p.first) ^ (hash<T2>()(p.second) << 10);
    }
  };
  template <>
  struct hash<I> {
    size_t operator()(const I& i) const {
      // FIXME: 10 -> max prod len
      return i.ahead * prod.size() * 10  + i.dot * prod.size() + i.pi;
    }
  };

}

// LR State Node
struct N {
  unordered_map<pair<int, int>, set<int>> ahead;
  size_t hashv, hashp;
  N() : ahead(), hashv(0), hashp(0) {}
  bool insert(const I& o) {
    pair<int, int> p = {o.pi, o.dot};
    bool chg = ahead[p].insert(o.ahead).second;
    if (chg) hashv += hash<I>()(o), hashp += hash<pair<int, int>>()(p);
    return chg;
  }
  void foreach(function<void(const I&)> f) const {
    for (const auto& p: ahead) {
      int pi = p.first.first, dot = p.first.second;
      for (int a: p.second)
        f({pi, dot, a});
    }
  }
  bool merge(const N& o) {
    assert(hashp == o.hashp);
    bool chg = false;
    for (auto& p: o.ahead) {
      for (int a: p.second) {
        chg |= ahead[p.first].insert(a).second;
      }
    }
    return chg;
  }
  bool operator==(const N& o) const {
    return ahead == o.ahead;
  }
};

namespace std {
  template <>
  struct hash<N> {
    size_t operator()(const N& o) const {
      return o.hashv;
    }
  };
}

/* For debug */
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
  n.foreach([&](const I& i) { s.push_back(i); });
  return to_string(s);
}

void addprod(const vector<int>& p, string f) {
  phead[p[0]].push_back(prod.size());
  prod.push_back(p);
  if (f.empty()) {
    if (p.size() == 2) f = "{ return $1; }";
    else f = "{ Node d; return d; }";
  }
  prod_action.push_back(f);
}

/* Init nullable and first set. */
void init_nf() {
  cerr << "--- init_nf begin.\n";
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
  cerr << "--- init_nf end.\n";
}

int closure_us = 0;
int nclosure = 0;

void closure(N& res) {
  chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  static vector<int> added(nsymbs + 1, 0);
  static unordered_map<I, vector<int>> fmap;

  queue<I> q;
  res.foreach([&](const I& i) { q.push(i); });

  while (!q.empty()) {
    I t = q.front();
    q.pop();

    auto& p = prod[t.pi];
    if (t.dot < p.size()) {

      vector<int>& fr = fmap[t];
      if (fr.empty()) {
        p.push_back(t.ahead);
        for (int i = t.dot + 1; i < p.size(); i++) {
          int si = p[i];
          for (int i: first[si]) if (!added[i]) added[i] = 1, fr.push_back(i);
          if (!symb[si].nullable) break;
        }
        p.pop_back();
        for (int i: fr) added[i] = 0;
      }

      for (int pi: phead[p[t.dot]]) {
        for (int u: fr) {
          I ni = {pi, 1, u};
          if (res.insert(ni)) q.push(ni);
        }
      }
    }
  }

  chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  closure_us += chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
  nclosure++;
}

int gotox_us = 0;
int ngotox = 0;

auto gotox(const N& items, int x) {

  chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  N res;
  items.foreach([&](const I& t) {
    if (t.dot < prod[t.pi].size() && prod[t.pi][t.dot] == x)
      res.insert({t.pi, t.dot+1, t.ahead});
  });

  chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  gotox_us += chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
  ngotox++;
  closure(res);
  return res;
}

enum { EMPTY, SHIFT, GOTO, REDUCE, ACCEPT }; // Should be identical to output()
struct A {
  int type, i;
  bool operator==(const A& o) const {
    return i == o.i && type == o.type;
  }
};
// shift/goto { type, si; }
// reduce { type, pi; }
string to_string(const A& a) {
  return "{ .type = " + to_string(a.type) + ", .i = " + to_string(a.i) + " }";
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

void build() {
  int nterms = 0;
  assert(nsymbs + 1 == symb.size());
  for (int i = 1; i <= nsymbs; i++) if (symb[i].term) nterms++;
  debug(nterms, nsymbs, prod.size());

  vector<vector<A>> table;

  int nstates = 0;
  unordered_map<int, N> state;
  unordered_map<N, int> statei;

  queue<int> q;

  int getstate_us = 0, ngetstate = 0;

  auto getstate = [&](const N& s) {
    // cerr << "1";
    chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int& u = statei[s];
    if (!u) {
      q.push(u = ++nstates);
      state[u] = s;
      table.resize(u+1, vector<A>(nsymbs+1, {EMPTY, 0}));
      // table[u] = 
      // debug(u, s);
    }

    chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    getstate_us += chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    ngetstate++;

    return u;
  };

  debug(prod), debug(phead), debug(symb);

  N s0;
  assert(s0.hashv == 0);
  s0.insert({0, 1, 1});
  closure(s0);
  int u0 = getstate(s0);
  debug(state[u0]);


  for (; q.size(); q.pop()) {
    int u = q.front();
    const auto& s = state[u];

    s.foreach([&](const I& i) {
      auto& p = prod[i.pi];
      if (i.dot < p.size()) {
        int x = p[i.dot];
        if (symb[x].s == "$end") {
          auto& ent = table[u][symbi["$end"]];
          assert(ent.type == EMPTY || ent.type == ACCEPT);
          ent = { ACCEPT, 0 };
          return;
        }

        int v = getstate(gotox(s, x));
        A a = {symb[x].term ? SHIFT: GOTO, v};

        // debug(symb[x].s, state[v]);
        assert(1 <= x && x <= nsymbs);
        auto& t = table[u][x];
        if (t.type == REDUCE && a.type == SHIFT) {
          cerr << "Shift-reduce conflict found on table[" << u << "][" << x << "].\n";
          // assert(0);
        } else if (!(table[u][x].type == EMPTY || table[u][x] == a)) {
          // debug(table[u][x], a);
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
          return;
        }

        if (!(t.type == EMPTY || t == a)) {
          assert(table[u][i.ahead].type == EMPTY || table[u][i.ahead] == a);
        }
        table[u][i.ahead] = a;
      }
    });

  }
  assert(table.size() == nstates + 1 && table[0].size() == nsymbs + 1);
  debug(nsymbs, nterms, prod.size(), nstates);
  debug(closure_us, gotox_us, getstate_us);
  debug(nclosure, ngotox, ngetstate);
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
      // do not support '\n'
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
      // init_nullable();
      // init_first();
      init_nf();
      build();

      n += s - str.c_str();
      cout << str.substr(n, str.size() - n);
      return 0;

    default: assert(0);
    }

    if (token != SPACE)
      cerr << "find token " << token << " of length " << n << ": " << str.substr(s - str.c_str(), n) << '\n';
    s += n;
  }
  return 0;
}
