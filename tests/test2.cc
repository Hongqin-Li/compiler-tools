#include <stdio.h>
using namespace std;

---
S: V '=' E | E;
E: V ;
V: 'x' | '*' E;
---

int main() {
  return 0;
}
