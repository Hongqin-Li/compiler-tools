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

[a-zA-Z][a-zA-Z0-9_]*
  token = ID;

'(\\[^]|[^\\'\n])'
  token = CHAR;

[ \t\n]+
  token = SPACE;

\-\-\-[^\n]*
  token = SPLIT;
---

int nsymbs;
struct S { string s; int term, nullable; };
unordered_map<string, int> symbi;
vector<S> symb;
vector<set<int>> first;

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
unordered_map<int, vector<int>> phead;  // phead[i]: Indices of production whose lhs symbol is i.
struct I {
  int pi, dot, ahead;  // dot: Index of symbol after dot
  bool operator==(const I& o) const {
    return pi == o.pi && dot == o.dot && ahead == o.ahead;
  }
};

/* Hashing */
namespace std {
  template <>
  struct hash<I> {
    size_t operator()(const I& i) const {
      return (((hash<int>()(i.pi)
             ^ (hash<int>()(i.dot)) << 1)) >> 1)
             ^ (hash<int>()(i.ahead) << 1);
    }
  };

  template <>
  struct hash<unordered_set<I>> {
    size_t operator()(const unordered_set<I>& s) const {
      size_t res = 0;
      for (const auto& s: s) res ^= hash<I>()(s);
      return res;
    }
  };
}

/* For debug */
string to_string(const I& t) {
  int i, n = prod[t.pi].size();
  string res;
  for (i = 0; i < n; i++) {
    if (i == t.dot) res += ". ";
    res += symb[prod[t.pi][i]].s + " ";
    if (i == 0) res += "=> ";
  }
  if (i == t.dot) res += ". ";
  res += ", ahead: " + symb[t.ahead].s;
  return res + '\n';
}

void addprod(const vector<int>& p) {
  phead[p[0]].push_back(prod.size());
  prod.push_back(p);
}

/* Init nullable and first set. */
void init_nf() {
  cerr << "--- init_nf begin.\n";
  first = vector<set<int>>(nsymbs + 1, set<int>());
  for (int i = 1; i <= nsymbs; i++) {
    symb[i].nullable = 0;
    if (symb[i].term) first[i].insert(i);
  }
  
  for (int stop = 0; !stop; ) {
    stop = 1;
    for (auto& p: prod) {
      int u = p[0], allnull = 1;
      int fs = first[u].size(), ns = symb[u].nullable;
      for (int i = 1; i < p.size() && allnull; i++) {
        int v = p[i];
        if (allnull) first[u].insert(first[v].begin(), first[v].end());
        if (!symb[v].nullable) allnull = 0;
      }
      symb[u].nullable |= allnull;

      if (fs != first[u].size() || ns != symb[u].nullable) stop = 0;
    }
  }

  for (int i = 1; i <= nsymbs; i++) {
      vector<string> firstset;
      for (auto u: first[i]) firstset.push_back(symb[u].s);
      debug(symb[i].s, firstset);
  }
  cerr << "--- init_nf end.\n";
}

// dep: nullable
set<int> firsts(const vector<int>& s) {
  set<int> res;
  for (int t: s) {
    res.insert(first[t].begin(), first[t].end());
    if (!symb[t].nullable) break;
  }
  return res;
}

auto closure(const unordered_set<I>& s) {
  unordered_set<I> res;
  function<void(const I&)> dfs = [&](const I& t) {
    auto& p = prod[t.pi];
    if (t.dot < p.size()) {
      vector<int> r(p.begin() + t.dot + 1, p.end());
      r.push_back(t.ahead);
      auto fr = firsts(r);
      for (int q: phead[p[t.dot]]) {
        for (int u: fr) {
          I ni = {q, 1, u};
          if (res.insert(ni).second) dfs(ni);
        }
      }
    }
  };
  for (const auto& x: s) res.insert(x), dfs(x);
  return res;
}

auto gotox(const unordered_set<I>& items, int x) {
  unordered_set<I> res;
  for (const auto& t: items)
    if (t.dot < prod[t.pi].size() && prod[t.pi][t.dot] == x)
      res.insert({t.pi, t.dot + 1, t.ahead});
  return closure(res);
}


enum { EMPTY, SHIFT, GOTO, REDUCE, ACCEPT };
struct A {
  int type, i;
  bool operator==(const A& o) const {
    return i == o.i && type == o.type;
  }
};
// shift/goto { type, si; }
// reduce { type, pi; }
string to_string(const A& a) {
  return "{ type: " + to_string(a.type) + ", i: " + to_string(a.i) + " }";
}

void output(const vector<vector<A>>& table) {
  struct B { pair<int, int> i; A a; };
  vector<B> t;
  for (int i = 0; i < table.size(); i++) {
    for (int j = 0; j < table[0].size(); j++) {
      if (table[i][j].type != EMPTY)
        t.push_back({{i, j}, table[i][j]});
    }
  }
  debug(t.size());
}

void build() {
  int nterms = 0;
  assert(nsymbs + 1 == symb.size());
  for (int i = 1; i <= nsymbs; i++) if (symb[i].term) nterms++;
  debug(nterms, nsymbs, prod.size());

  vector<vector<A>> table;

  int nstates = 0;
  unordered_map<int, unordered_set<I>> state;
  unordered_map<unordered_set<I>, int> statei;

  queue<int> q;

  auto getstate = [&](const unordered_set<I>& s) {
    // cerr << "1";
    int& u = statei[s];
    if (!u) {
      q.push(u = ++nstates);
      state[u] = s;
      table.resize(u+1, vector<A>(nsymbs+1, {EMPTY, 0}));
      // table[u] = 
    }
    return u;
  };

  debug(prod), debug(phead), debug(symb);

  int u0 = getstate(closure({{0, 1, 1}}));
  debug(state[u0]);

  for (; q.size(); q.pop()) {
    int u = q.front();
    auto& s = state[u];
    // debug(u, s);
    for (auto& i: s) {
      auto& p = prod[i.pi];
      // debug(i, p.size());
      if (i.dot < p.size()) {
        int x = p[i.dot];
        if (symb[x].s == "$end") continue;

        int v = getstate(gotox(s, x));
        A a = {symb[x].term ? SHIFT: GOTO, v};

        // debug(symb[x].s, state[v]);
        assert(1 <= x && x <= nsymbs);
        auto& t = table[u][x];
        if (t.type == REDUCE && a.type == SHIFT) {
          cerr << "Shift-reduce conflict found on table[" << u << "][" << x << "].\n";
        } else if (!(table[u][x].type == EMPTY || table[u][x] == a)) {
          // debug(table[u][x], a);
          assert(table[u][x].type == EMPTY || table[u][x] == a);
        }
        t = a;
      } else {
        A a = {REDUCE, i.pi};

        assert(1 <= i.ahead && i.ahead <= nsymbs);
        auto& t = table[u][i.ahead];

        /* Resolves shift-reduce conflicts by shifting. */
        if (t.type == SHIFT) {
          cerr << "Shift-reduce conflict found on table[" << u << "][" << i.ahead << "].\n";
          continue;
        }

        if (!(t.type == EMPTY || t == a)) {
          // debug(u, i.ahead, symb[u], symb[i.ahead], t, a);
          assert(table[u][i.ahead].type == EMPTY || table[u][i.ahead] == a);
        }
        table[u][i.ahead] = a;
      }
    }
  }
  assert(table.size() == nstates + 1 && table[0].size() == nsymbs + 1);
  debug(nsymbs, nterms, prod.size(), nstates);
  output(table);
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
  addprod(p);
  assert(symbi["$start"] == 1);
  symb[1].term = 0;
  p.resize(0);

  for (const char *s = str.c_str(); *s; ) {
    int n = nxt(s);
    string t;
    switch (token) {
    case ';': symb[p[0]].term = 0, addprod(p), p.resize(0); break;
    case '|': addprod(p), p.resize(1); break;

    case ID:
    case CHAR:
      t = str.substr(s - str.c_str(), n);
      p.push_back(addsymb(t));
      break;

    case ':':
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
