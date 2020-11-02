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
unordered_map<int, set<int>> first;

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
  bool operator<(const I& o) const {
    return pi != o.pi ? pi < o.pi :
           dot != o.dot ? dot < o.dot : ahead < o.ahead;
  }
};

/* For debug */
string to_string(const I& t) {
  int i, n = prod[t.pi].size();
  string res;
  for (i = 0; i < n; i++) {
    if (i == t.dot) res += '.';
    res += symb[prod[t.pi][i]].s;
  }
  if (i == t.dot) res += '.';
  res += " ahead: " + symb[t.ahead].s;
  return res;
}

void addprod(const vector<int>& p) {
  phead[p[0]].push_back(prod.size());
  prod.push_back(p);
}

// symb.term, prod, phead -> symb.nullable
void init_nullable() {
  unordered_map<int, int> vis;
  function<int(int)> dfs = [&](int s) {
    vis[s] = 1;
    symb[s].nullable = 0;
    // debug(s, symb[s]);
    if (!symb[s].term) {
      debug(phead[s]);
      for (int p: phead[s]) {
        int allnull = 1;
        for (auto t: prod[p])
          if (!vis[t]) allnull &= dfs(t);
        if (allnull) symb[s].nullable = 1;
      }
    }
    return symb[s].nullable;
  };
  dfs(prod[0][0]);
  cerr << "--- init_nullable finish.\n";
}

void init_first() {
  unordered_map<int, int> vis;
  function<void(int)> dfs = [&](int u) {
    vis[u] = 1;
    if (symb[u].term) first[u] = {u};
    else
      for (int p: phead[u]) {
        int allnull = 1;
        for (int t: prod[p]) {
          if (!vis[t]) dfs(t);
          if (allnull) first[u].insert(first[t].begin(), first[t].end());
          allnull &= symb[t].nullable;
        }
      }
  };
  dfs(prod[0][0]);
  cerr << "--- init_first finish.\n";
}

// dep: nullable
set<int> firsts(const vector<int>& s) {
  set<int> res;
  int allnull = 1;
  for (int t: s) {
    if (!allnull) break;
    res.insert(first[t].begin(), first[t].end());
    allnull &= symb[t].nullable;
  }
  return res;
}

set<I> closure(const set<I>& s) {
  set<I> res;
  function<void(const I&)> dfs = [&](const I& t) {
    auto& p = prod[t.pi];
    if (t.dot < p.size()) {
      vector<int> r(p.begin() + t.dot + 1, p.end());
      r.push_back(t.ahead);
      for (int q: phead[p[t.dot]]) {
        for (auto u: firsts(r)) {
          I ni = {q, 1, u};
          if (res.find(ni) == res.end())
            res.insert(ni), dfs(ni);
        }
      }
    }
  };
  for (auto x: s) res.insert(x), dfs(x);
  return res;
}

set<I> gotox(const set<I>& items, int x) {
  set<I> res;
  for (auto& t: items)
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

void build() {
  vector<vector<A>> table;

  int nstates = 0;
  unordered_map<int, set<I>> state;
  map<set<I>, int> statei;

  queue<int> q;

  auto getstate = [&](const set<I>& s) {
    int& u = statei[s];
    if (!u) {
      q.push(u = ++nstates);
      state[u] = s;
      table.resize(u+1);
      table[u] = vector<A>(nsymbs+1, {EMPTY, 0});
    }
    return u;
  };

  debug(prod), debug(phead), debug(symb);

  int u0 = getstate(closure({{0, 1, 1}}));
  debug(state[u0]);

  for (; q.size(); q.pop()) {
    int u = q.front();
    auto& s = state[u];
    debug(u, s);
    for (auto& i: s) {
      auto& p = prod[i.pi];
      // debug(i, p.size());
      if (i.dot < p.size()) {
        int x = p[i.dot];
        if (symb[x].s == "$") continue;

        int v = getstate(gotox(s, x));
        A a = {symb[x].term ? SHIFT: GOTO, v};

        // debug(symb[x].s, state[v]);
        assert(1 <= x && x <= nsymbs);
        auto& t = table[u][x];
        if (t.type == REDUCE && a.type == SHIFT) {
          cerr << "Shift-reduce conflict found.\n";
        } else if (!(table[u][x].type == EMPTY || table[u][x] == a)) {
          debug(table[u][x], a);
          assert(table[u][x].type == EMPTY || table[u][x] == a);
        }
        t = a;
      } else {
        A a = {REDUCE, i.pi};

        assert(1 <= i.ahead && i.ahead <= nsymbs);
        auto& t = table[u][i.ahead];

        /* Resolves shift-reduce conflicts by shifting. */
        if (t.type == SHIFT) {
          cerr << "Shift-reduce conflict found.\n";
          continue;
        }

        if (!(t.type == EMPTY || t == a)) {
          debug(u, i.ahead, symb[u], symb[i.ahead], t, a);
          assert(table[u][i.ahead].type == EMPTY || table[u][i.ahead] == a);
        }

        table[u][i.ahead] = a;
      }
    }
  }
  debug(nsymbs, prod.size(), nstates);
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
   * - Legal token should not contain '`' and '$' (without quote),
   *   which is guaranteed by the lexer above. 
   * - The third token is the first symbol provided by user.
   */
  vector<int> p = {addsymb("S`"), 3, addsymb("$")};
  addprod(p);
  assert(symbi["S`"] == 1);
  symb[1].term = 0;
  p.resize(0);

  for (const char *s = str.c_str(); *s; ) {
    int n = nxt(s), stop = 0;
    string t;
    switch (token) {
    case ';': symb[p[0]].term = 0, addprod(p), p.resize(0); break;
    case '|': addprod(p), p.resize(1); break;

    case ID:
    case CHAR:
      t = str.substr(s - str.c_str(), n);
      p.push_back(addsymb(t));
      break;

    case SPLIT:
      n += s - str.c_str();
      cout << str.substr(n, str.size() - n);
      stop = 1;
      break;

    case ':':
    case SPACE: break;
    default: assert(0);
    }

    if (stop) break;
    if (token != SPACE)
      cerr << "find token " << token << " of length " << n << ": " << str.substr(s - str.c_str(), n) << '\n';
    s += n;
  }
  init_nullable();
  init_first();
  build();
  return 0;
}
