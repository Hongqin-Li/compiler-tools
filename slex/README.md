# Simple Lexical Analyzer Generator in C++

This is a simple lexical analyzer (lexer) generator in C++. It's similar to flex, but is mainly written and used for teaching, as a guide to the lexical analysis part of compilers. Although it is minimal, it supports exactly what we need to create a real lexer for C language.

## Getting started

To start with, make sure you understand the mechanism behind a lexer and lexer generator. Great introduction can be found in the tiger book, *Appel, Andrew W. Modern compiler implementation in C*. Basically, slex works as follows:

1. Read in a lexer code with a special block surrounded by `---`. Contents inside this block consist of token descriptions (regular expressions) and corresponding actions (code snippet to execute) when a token is found.
2. Parsing token descriptions and actions from the special block and convert them into a NFA.
3. Convert the NFA to DFA using the algorithm provided in the tiger book.
4. Generate C code of the DFA, replace the special block by these generated code, and output.

### Compilation

Run `make` to compile the lexer generator at `obj/slex`.

### Syntax

Only non-empty lines inside the first block surrounded by `---` will be parsed by slex. Every two lines of them represents a pair of description and action for certain token. The action is just a line of code to be executed, and description is a regular expression with minimal syntax supported as follows:

- Support charset syntax like `[abc]`, `[^123]` and `[0-9a-z]`.
- Support expression or syntax like `apple|orange`.
- `| [ ] ( ) * ? + -` are all of the special characters and should be escaped by `\` whenever they are representing an actual character(even inside charset, like `[\*\?\(\+]`).
- Among all non-printing ascii characters, we only support `\n \t` for now.
- The Dot `.` syntax, representing "all" characters, is not supported since it's useless for slex to argue about whether characters like `\n` should be dotted. Instead, try something like `[^]` or `[^\n]`.

### Example

A simple example can be found in `test.sh`.

It first inputs `lex.cc` into `slex` to generate the lexer source code `obj/lex.cc`, which is then used to parse a simple C program `prog.c`.
