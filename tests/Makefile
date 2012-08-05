TESTS := $(patsubst %.in, %.result, $(wildcard *.in))

check: ../chttp $(TESTS)

.SUFFIXES: .result .in .out
%.result: %.in %.out

$(TESTS):
	@echo testing $*
	-cat $*.in | ../chttp &>$@
	-diff -i -B --suppress-common-lines -y $*.out $@

.PHONY: clean
clean:
	-rm -f *.result
