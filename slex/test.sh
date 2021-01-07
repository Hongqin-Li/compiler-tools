make

cat lex.cc | ./obj/slex > obj/lex.cc
g++ obj/lex.cc -o obj/lex
cat prog.c | ./obj/lex

# cat example.cc | ./obj/slex > obj/g.dot 
# dot -Tpdf obj/g.dot -o obj/g.dot.pdf

# echo $1 | ./a.out | dot -Tpdf -o g.dot.pdf
