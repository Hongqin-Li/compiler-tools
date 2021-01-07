#include <cassert>
#include <cstdio>
#include <iostream>
using namespace std;

enum { SPACE = 128, IF, ELSE, INT, OP_EQ, ASSIGN, RETURN, ID, NUM, REAL, STRING };

int token;

---
if
  token = IF;

int
  token = INT;

return
  token = RETURN;

==
  token = OP_EQ;

=
  token = '=';
;
  token = ';';
\(
  token = '(';
\)
  token = ')';
\[
  token = '[';
\]
  token = ']';
{
  token = '{';
}
  token = '}';

[ \t\n]+
  token = SPACE;

[a-z][a-z0-9]*
  token = ID;

[0-9][0-9]*
  token = NUM;

([0-9][0-9]*\.[0-9]*)|([0-9]*\.[0-9][0-9]*)
  token = REAL;

"(\\[^]|[^\\"\n])*"
  token = STRING;
---

int main() {
  string str;
  for (string s; getline(cin, s); )
    str += s + '\n';
  cout << str;
  for (char *s = &str[0]; *s; ) {
    int n = nxt(s);
    if (token != SPACE)
      cout << "find token of type " << token << ": " << str.substr(s - &str[0], n) << '\n';
    s += n;
    cout << "n: " << n << '\n';
  }
  return 0;
}
