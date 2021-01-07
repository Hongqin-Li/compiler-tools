#include <bits/stdc++.h>
#include "debug.h"
using namespace std;

inline int SPECIAL(int i) { return -i; }

struct Node { int head, rule; };
struct Edge { int to, nxt, w; };
unordered_map<int, Node> node;
unordered_map<int, Edge> edge;
int nnodes, nedges;
int nalloc() { return ++nnodes; }
void addedge(int u, int v, int w) {
  edge[++nedges] = {v, node[u].head, w};
  node[u].head = nedges;
}

/* Print the graph with given root r in dot format. */
static void gstat(int r) {
  unordered_map<int, int> vis;

  function<void(int)> stat1 = [&](int u) {
    if (vis[u]) return;
    vis[u] = 1;
    if (node[u].rule) printf("n%d [label=r%d];\n", u, node[u].rule);
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to, w = edge[i].w;
      printf("n%d -> n%d ", u, v);
      if (w == 0) ;
      else if (w > 0) {
        if (w == '"' || w == '\\') printf("[label=\"\\%c\"]", w);
        else printf("[label=\"%c\"]", w);
      }
      else printf("[label=\"%c(s)\"]", -w);
      printf(";\n");
      if (!vis[v]) stat1(v);
    }
  };

  printf("digraph {\n");
  stat1(r);
  printf("}\n");
}

/* Special characters are negative. */
int eat(char **ps) {
  int c = **ps;
  if (**ps == '\\') {
    ++*ps;
    switch (**ps) {
    case 'n': c = '\n'; break;
    case 't': c = '\t'; break;
    default: c = **ps;
    }
  } else {
    switch (**ps) {
    case '[': case ']': case '(': case ')': case '*': case '?': case '-':
      c = -c;
      break;
    }
  }
  ++*ps;
  return c;
}

struct NFA { int head, tail; };

NFA nfa_init() {
  return {nalloc(), nalloc()};
}

NFA nfa_empty() {
  int h = nalloc(), t = nalloc();
  addedge(h, t, 0);
  return {h, t};
}

NFA nfa_cat(NFA a, NFA b) {
  addedge(a.tail, b.head, 0);
  return {a.head, b.tail};
}

NFA nfa_alt(NFA a, NFA b) {
  addedge(a.head, b.head, 0);
  addedge(b.tail, a.tail, 0);
  return a;
}

NFA charset(char **ps) {
  assert(**ps != '\0');
  int h = nalloc(), t = nalloc(), comp = 0, c;
  if ((c = eat(ps)) == SPECIAL('[')) {
    if (**ps == '^') {
      ++*ps;
      comp = 1;
    }
    int valid[128] = {0};
    for (int i = 1; i < 128; i++)
      valid[i] = comp;

    for (int start, span = 0; (c = eat(ps)) != SPECIAL(']') && c != '\0'; ) {
      if (c == SPECIAL('-'))
        span = 1;
      else if (!span)
        start = c, valid[c] = !comp;
      else {
        assert(1 < start && start < c && c < 128);
        for (int i = start + 1; i <= c; i++)
          valid[i] = !comp;
        span = 0;
      }
    }
    for (int i = 0; i < 128; i++)
      if (valid[i]) addedge(h, t, i);
  } else addedge(h, t, c);
  return {h, t};
}

/* Repetion of the NFA constructed by span [begin, *ps). */
NFA repete(NFA a, char **ps, char *begin) {
  switch (**ps) {
    case '*':
      addedge(a.tail, a.head, 0);
    case '?':
      addedge(a.head, a.tail, 0);
      ++*ps;
      return a;
    case '+':
      **ps = '*';
      *ps = begin;
      return a;
  }
  return a;
}

/* Parse regular expression to NFA. */
NFA parse(char **ps) {
  NFA res = {nalloc(), nalloc()}, cur = nfa_empty();
  for (int stop = 0; !stop; ) {
    NFA a;
    char *begin;
    switch (**ps) {
    case '\0':
    case ')':
      nfa_alt(res, cur);
      stop = 1;
      break;
    case '|':
      nfa_alt(res, cur);
      cur = nfa_empty();
      ++*ps;
      break;
    case '(':
      begin = (*ps)++;
      a = parse(ps);
      assert(**ps == ')');
      ++*ps;
      cur = nfa_cat(cur, repete(a, ps, begin));
      break;
    default:
      begin = *ps;
      a = charset(ps);
      cur = nfa_cat(cur, repete(a, ps, begin));
    }
  }
  return res;
}

/* Compute e-closure of states in s. */
set<int> closure(const set<int>& s) {
  set<int> res;
  queue<int> q;
  for (auto s: s) q.push(s);
  for (; q.size(); q.pop()) {
    int u = q.front();
    if (res.count(u)) continue;
    res.insert(u);
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to, w = edge[i].w;
      if (!w && !res.count(v)) q.push(v);
    }
  }
  return res;
}

/* Compte adjacent states of states s with edge c. */
set<int> dfaedge(const set<int>& s, int c) {
  set<int> res;
  for (auto u: s) {
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to;
      if (edge[i].w == c && !res.count(v))
        res.insert(v);
    }
  }
  return closure(res);
}


/*
 * A state d is final in the DFA if any NFA-state in states[d]
 * is final in the NFA. And we label it with the first regular
 * expressions that constitute the lexical specification.
 * This is how rule priority is implement.
 */
int prule(const set<int>& s) {
  int r = 0;
  for (auto s: s)
    if (node[s].rule)
      r = r ? min(r, node[s].rule) : node[s].rule;
  return r;
}

/*
 * Convert NFA to DFA.
 * Return DFA nodes in a order of BFS from the head node.
 */
vector<int> convert(NFA a) {
  vector<int> alpha;
  for (int i = 1; i < 128; i++) alpha.push_back(i);

  map<int, set<int>> ss;
  map<set<int>, int> ni;

  auto s0 = closure({a.head});
  auto& u0 = ni[s0];
  ss[u0 = nalloc()] = s0;
  node[u0].rule = prule(s0);

  vector<int> q;
  q.push_back(u0);
  for (int front = 0; front < q.size(); front++) {
    int u = q[front];
    for (auto c: alpha) {
      auto s = dfaedge(ss[u], c);
      if (s.size()) {
        auto& v = ni[s];
        if (!v) {
          ss[v = nalloc()] = s;
          node[v].rule = prule(s);
          q.push_back(v);
        }
        addedge(u, v, c);
      }
    }
  }
  return q;
}

/* Dump c code of NFA. */
string dump(const vector<int>& ns, vector<string> act) {
  int n = ns.size();
  vector<vector<int>> trans(n+1, vector<int>(128, 0));

  map<int, int> ni;
  for (int i = 0; i < n; i++) ni[ns[i]] = i+1;
  for (auto u: ns) {
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to, w = edge[i].w;
      assert(0 < w && w < 128);
      trans[ni[u]][w] = ni[v];
    }
  }

  string code;
  code += "/* Transition table. */\n";
  code += "int trans[][128] = {\n";
  for (auto v: trans) {
    code += "  {";
    for (auto x: v) code += to_string(x) + ", ";
    code += "},\n";
  }
  code += "};\n";

  code += "\n/* Action functions. */\n";
  for (int i = 0; i < act.size(); i++)
    code += "void act" + to_string(i+1) + "() {" + act[i] + "}\n";

  code += "\n/* Action table. */\n";
  code += "void (*action[])() = {\n  0,\n";
  for (auto u: ns) {
    if (node[u].rule) code += "  act" + to_string(node[u].rule) + ",\n";
    else code += "  0,\n";
  }
  code += "};\n";

  code += R"(
/*
 * Feed in the chars of beginning at s, call the
 * action function of matched final state.
 * Return the length of the next token.
 */
int nxt(char *s) {
  int u = 1, preu = 0, prei;
  for (int i = 0; s[i] && (u = trans[u][s[i]]); i++)
    if (action[u]) preu = u, prei = i;
  assert(preu);
  action[preu]();
  return prei + 1;
}
  )";

  return code;
}

int main() {
  string s;
  bool graph_only = false;
  while (getline(cin, s))
    if (s.size() >= 3 && s.substr(0, 3) == "---") break;
    else if (!graph_only) cout << s << '\n';

  /* Read the next non-empty line from stdin. */
  auto get1 = [](string& s) -> int {
    while (getline(cin, s)) {
      if (s.size() == 0) continue;
      if (s.size() >= 3 && s.substr(0, 3) == "---") return 0;
      else return s.length();
    };
    return s.length();
  };
 
  int h = nalloc(), t = nalloc();
  vector<string> rule, action;
  for (string sr, sa; get1(sr) && get1(sa); ) {
    rule.push_back(sr);
    action.push_back(sa);
    char *s = &sr[0];
    NFA a = parse(&s);
    node[a.tail].rule = rule.size();
    addedge(h, a.head, 0);
    addedge(a.tail, t, 0);
  }
  debug(rule, action);
  if (graph_only) gstat(convert({h, t})[0]);
  else {
    cout << dump(convert({h, t}), action);
    while (getline(cin, s)) cout << s << '\n';
  }
  return 0;
}
