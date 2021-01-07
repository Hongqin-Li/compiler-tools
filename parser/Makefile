obj/parser: obj/parser.cc
	g++ --std=c++17 -O2 -I. $< -o $@ -DLALR
	# g++ --std=c++17 -O2 -I. $< -o $@

obj/parser.cc: main.cc
	mkdir -p obj
	make -C ../slex
	cat $< | ../slex/obj/slex > $@

test%.y: obj/parser
	yacc tests/test$*.y -o /dev/null 2> /dev/null

test%.cc: obj/parser
	cat tests/test$*.cc | $< > /dev/null 2> /dev/null

test: obj/parser
	# cat tests/test2.cc | $<
	# cat tests/test3.cc | $<
	# cat tests/test4.cc | $<
	# cat tests/test5.cc | $<
	# cat tests/test-c.cc | $< 1> /dev/null
	# cat tests/test-pascal.cc | $< 1> /dev/null
	# cat tests/test-rust.cc | $< 1> /dev/null
	cat tests/test-js.cc | $< 1> /dev/null
	# cat tests/test-my.cc | $<
	# cat tests/test-minic.cc | $<
	# cat tests/test-int.cc | $< > obj/test-parser.cc
	# g++ --std=c++17 obj/test-parser.cc -o obj/test-parser

bench:
	sh scripts/bench.sh

clean:
	rm -rf obj/

.PHONY: test bench clean
