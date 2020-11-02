obj/parser: obj/parser.cc
	g++ --std=c++17 -O2 -I. $< -o $@

obj/parser.cc: main.cc
	mkdir -p obj
	make -C ../slex
	cat $< | ../slex/obj/slex > $@

test: obj/parser
	cat test2.cc | $<
	# cat test.cc | $<

.PHONY: test
