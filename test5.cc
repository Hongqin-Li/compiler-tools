#include <stdio.h>
using namespace std;

---
stm
  : ID '=' exp
  ;

exp
  : ID
  | exp OP exp
  ;
---

int main() {
  return 0;
}
