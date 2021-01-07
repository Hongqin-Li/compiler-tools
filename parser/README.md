<h1 align="center">Simple LR Parser Generator in C++</h1>

<div align="right">Hongqin Li<br/>Department of Computer Science<br/>Fudan University</div>



## 1 Introduction

Compiler, a magic bridge connecting programmer with executable programs, accurate and efficient translator from code to binary, allows programming like prosing, without which thriving of programming is impossible. Parser, eyes of compiler, follows the elegant rules created by programmers and parses plain text into fancy data structures. The most famous theory behind it is the LR theory, introduced by Donald Knuth in 1965. And here we are, after half of a century, revisiting his great works with admiration and passion, studying recent practical approaches, providing our thoughts, detailing optimization tricks and making thorough comparison with Yacc. It's an efficient implementation as well as review of Knuth's algorithm, striving through textbook towards real world.



### 1.1 LR(k) Parser

The idea behind LR(k) parser is quite straightforward. Simply by keeping track of all possible configurations, each of which is of form $A \rightarrow a.Xb,z$ with a dot indicating current parsing state of this production and k lookahead symbols $z$ indicating the next k symbols immediately following this production, LR(k) parser moves between states, consisting of set of configurations, by eating possible $X$, yielding another state, another set of configurations, which is actually a closure of all eaten configurations with dot moving right by exactly one position.

The choice of k is restricted by the fact that LR(0) can hardly represent any complicate languages, while LR(k) with k greater than 1 may consume large amount of memory. So in practice, most compilers use a LR(1) parser with possibly some shift-reduce conflicts.



### 1.2 LALR

Even though LR(1) parser is capable of constructing most computer programming languages, the performance of the generator is not good enough on poor hardware in old days. The limitation of memory and speed leads to the invention of LALR parser by DeRemer<sup>[2]</sup>, which accelerate by merging states with identical set of configurations except for the difference of lookaheads on generation. For some grammars, LALR(1) table may contain mysterious reduce-reduce conflict while LR(1) may not, but in practice, such cases are pretty rare.



## 2 Related Algorithms

Many algorithms have been designed to practically implement a LR(1) parser, including

1. Knuth's canonical LR(1) algorithm<sup>[1]</sup>, which can be found in many textbooks such as the tiger book<sup>[3]</sup>.
2. The practical general method (PGM) by Pager<sup>[4]</sup> is a well-known LR(1) parser generation algorithm using merging process, which is conceptually simple, and has been widely implemented.
3. The lane-tracing algorithm by Pager<sup>[5]\[6]</sup> is another LR(1) parser generation algorithm. It uses a splitting approach and is more complex than the PGM method. Recent implementations can be found in [Hyacc](http://hyacc.sourceforge.net/).



## 3 Implementation

In general, we follow the pseudo code of Knuth's algorithm in the tiger book<sup>[3]</sup>, but translate it into programmable codes. After that, in order to optimize our implementation, we first measure the running time by injecting timing code snippets around functions to identify critical paths of our program, and then try our best to optimize them using common tricks like pre-computation, eliminating duplicated computation and caching results of expensive routines.



### 3.1 Modified Knuth's algorithm

The original statement of Knuth's algorithm in the textbook is not quite suitable for implementation, as shown below.

```pseudocode
Closure(I) = 
  repeat
    for any item (A -> a.Xb, z) in I
      for any production X -> y
        for any w in FIRST(bz)
          add item (X -> .y, w) to I
  until I does not change
  return I
  
Goto(I, X) =
  J = {}
  for any item (A -> a.Xb, z) in I
    add item (A -> aX.b, z) to J
  return Closure(J)
```

In `Closure(I)`, it's quite easy to observe that each item `(X -> .y, w)` is determined or generated only by item `(A -> a.Xb, z)`. Thus with the inspiration of directional graph, we can view each item as nodes, and teat the generation procedure as visiting it's child nodes, leading to our modified version:

```pseudocode
ClosureModified(I) = 
  repeat
    for any node u in I
      add all child nodes of u to I 
  until I does not change
  return I
```

Well, that's pretty clear. If you are familiar with graph algorithms, the function above is basically a DFS or BFS. We choose the BFS approach since it's slightly faster and can avoid stack overflow, as follows.

```pseudocode
ClosureBFS(I) = 
  add each item in I into queue q
  repeat
  	pop the front item in q as u
  	for each child nodes v of u
  	  if v is not in I
  	    add v to I
  	    push v to the back of q
  until q is empty
  return I
```



### 3.2 Critical Path

Before optimization, we should first locate which function is the bottle neck of the performance issue. The critical path is determined by measuring the longest sub-routine inside a program. Once figuring out a critical path, we can optimize it, which will effectively reduce the running time by the definition critical path. 

Our experiments show that the critical path is exactly those two functions `Closure(I)` and `Goto(I, X)`. 

Inside the loop of `Goto(I, X)` is the insertion of items into `J`, with restriction that the symbol following dot should be `X`. Our experiments on C99 grammar show that the number of loops is about 40 times more than the number of actual insertions (24290099/632434). Seemly, optimizing this loop by building index on symbol after dot may works. However, we found it worse since the maintenance of such index outweighs the improvement. Instead of indexing, we just cache it by precomputing a lookup table `nxtx[I][X]` once for each `I` and `X` when a new state I is generated. Since the precomputation is linear to the number of states(1800+ for C99), which is usually smaller than the number of invocations of `Goto` (17745 for C99), it achieves much more faster speed.

As for `Closure` function, caching is quite simple but powerful. Each invocation of `Closure(I)` will first check if `I` has been visited before and return the cached result immediately if possible. This can reduce half the overall running time of this function.

Together, we can reduce the running time greatly as shown by the following table, which is our experiments on C99 using our LR(1) generator.

| Function | Number of invocations | Time (sec) | After optimization (sec)         |
| -------- | --------------------- | ---------- | -------------------------------- |
| Closure  | 17746                 | 0.218      | 0.119                            |
| Goto     | 17745                 | 0.201      | 0.015 + some precomputation time |



### 3.3 Optimization

Here, we present some practical tricks of optimization and example application in our implementation.

- Precomputation of the whole graph first, which is mainly used for `ClosureBFS`.
- Caching the result of `Closure(I)` 
- Maintaining set of elements by hash table instead of B-tree for approximately O(1) query and insertion.
- Eliminating duplicated computation by BFS instead of tracing changes inside a for loop.
- Use faster hash map library such as [parallel-hashmap](https://github.com/greg7mdp/parallel-hashmap) other than `std::unorderd_map`



### 3.4 LALR(1)

After finishing LR(1), we also implement a LALR(1) generator since they are quite similar and we can change between them without many changes. To support LALR(1), we must provide a function to merge two "similar" states. By stating two states are "similar", we mean the only difference between them is looks ahead symbols. In other words, both states are identical if we omit the look ahead symbols of all items inside the states and group them by their production and dot position. For example, item `A -> a.Xb, a` and `A -> a.Xb, b` will be merged into a single item after grouping.

Such merging process can be done during the generation of new states, when we apply `Goto(I, X)` to expand current state with symbol X. And once we go to a new state, we first try to merge it with some similar state if exists, otherwise, a new state is generated.

As expected, LALR(1) can greatly reduce the number of states, lessen the execution time of parser generator and the memory usage of the generated parser. Since for most of the programming languages, LALR(1) won't introduce conflicts and behaves as it is LR(1), this faster approach is widely used in many famous parser generator such as [Yacc](http://dinosaur.compilertools.net/yacc/) and [Bison](https://www.gnu.org/software/bison/).



## 4 Performance

To measure the performance of this parser generator, we compared it with Yacc, a LALR(1) parser generator written in C, on four kinds of programming languages, SysY(a subset of C), [Pascal](https://github.com/FANMixco/minipascal-simplified), [ECMAScript 1](http://www.ecma-international.org/publications/files/ECMA-ST-ARCH/ECMA-262,%201st%20edition,%20June%201997.pdf), [C99](http://www.quut.com/c/ANSI-C-grammar-y-1999.html), shown as below.

| Language     | Our LR(1) (sec) | Our LALR(1) (sec) | Performance of Yacc (sec) |
| ------------ | --------------- | ----------------- | ------------------------- |
| SysY         | 0.026           | **0.020**         | 0.087                     |
| Pascal       | 0.030           | **0.020**         | 0.073                     |
| ECMAScript 1 | 0.216           | 0.139             | **0.092**                 |
| C99          | 0.368           | 0.173             | **0.115**                 |


Experiments show that our LR(1) generator is 4 times faster than Yacc on SysY, but 3.6 times slower on C99. And our implementation is faster on small grammar while slower on large grammar, which means that its asymptotic computational complexity is worse than Yacc.  Possible reasons we think are listed as follows:

- Constant factors between C++'s STL and pure C, especially the string manipulation used to output the generated table.
- Different number of states between LR(1) and LALR(1).
- The difference of algorithms used internally.

Slow though it is, the performance of the generated parser may be the same, since generated code is simply a two-dimensional table plus some helper functions to drive the state machine based on this table.



## 5 Conclusion

To sum up, our work is that we provide a simple implementation of LR(1) and LALR(1) parser generator, which performs much better than Yacc on small grammars and behaves not too bad on large grammars. Inspired by graph theory, we transform the problem using the notion of node and child nodes to represent the relationship incurs when computing closure. Also, extreme and effective optimization are detailed in this paper and successfully applied to make our progress from textbook towards modern implementations. The implementation consolidate our understanding of LR parser theorem and further our interest in modern implementation of compilers.



## 6 Future Works

It a pity that we do not have enough time to try the lane-tracing approach, which is considered to be more efficient than ordinary Knuth's algorithm, and has been widely used in recent parser generators such as Hyacc.

Besides, we are quite eager to develop a practical algorithm that can accept any unambiguous grammar, no matter what k is for LR(k). The algorithm itself need to be able to further look ahead dynamically, which may challenging if we want to simultaneously obtain decent usage of memory as well as high performance.



## References

References are presented in MLA format.

[1] Knuth, Donald E. "On the translation of languages from left to right." *Information and control* 8.6 (1965): 607-639.

[2] DeRemer, Franklin Lewis. *Practical translators for LR (k) languages*. Diss. Massachusetts Institute of Technology, 1969.

[3] Appel, Andrew W. *Modern compiler implementation in C*. Cambridge university press, 2004.

[4] Pager, David. "A practical general method for constructing LR (k) parsers." *Acta Informatica* 7.3 (1977): 249-268.

[5] Pager, David. "The lane tracing algorithm for constructing LR (k) parsers." *Proceedings of the fifth annual ACM symposium on Theory of computing*. 1973.

[6] Pager, David. "The lane-tracing algorithm for constructing LR (k) parsers and ways of enhancing its efficiency." *Information Sciences* 12.1 (1977): 19-42.



## Appendix A

### A.1 Syntax

The syntax of ours is similar to but not compatible with Yacc's. Compared to Yacc' syntax, our's are clean and convention-based. The main differences and trade-offs are as follows.

- We don't need to declare tokens. Since the generator can easily distinguish between terminals and non-terminal by checking whether it acts as a target of some productions or not. Explicit declaration of terminal tokens can help checking the grammar provided by user, while implicit derivation allows neater grammar rules.
- We use the first production as the starting production since almost every grammar starts with it's first production by conventional.
- Code snippet used for constructing abstract syntax tree is allowed for each production but **must be in one line**, surrounded by curly braces `{}`. It's actually a function with prerequisites of production (right-hand side symbols of the production) as its parameters and target symbol (left-hand side symbol) as its return value, all of which are of type `Node`. The parameters is `$1` for the first prerequisite, `$2` for the second one and so on. Thus you need to provide a definition of `Node` type above the grammar. This allows more customization than just using `union` in Yacc. For instance, we can use `std::variance` in C++.



Here is a example.

```c++
#include <cstdio>
using namespace std;

struct Node { int i; };

---
S
  : V '=' E { return $1; }
  | E
  ;

E: V ;
V: 'x' | '*' E ;
---

int main() {
  return 0;
}
```



### A.2 Usage

First build it by `make`. We can also modify Makefile to use the LALR(1) implementation.

After building, the parser generator compiled at obj/parser can be use to generate parser. Input grammar with legal format to its standard input. Then the corresponding generated parser code in C++ can be found in its standard output. Example commands can be found in Makefile.



### A.3 Benchmark

Run `make bench` to reproduce the result in this paper, which run Yacc and ours several times on different grammar, and output the average time as result.

