all: foobar qsort

foobar: 
	clang -O0 -g0 -emit-llvm -o foobar.bc -c foobar.c
	llvm-dis foobar.bc
	llc -march=cpp foobar.bc -o foobar.cpp
	clang -o foobar foobar.bc
	
qsort:
	clang -O0 -g0 -emit-llvm -o qsort.bc -c qsort.c
	llvm-dis qsort.bc
	llc -march=cpp qsort.bc -o qsort.cpp
	clang -o qsort qsort.bc

clean:
	rm -f *.cpp *.bc *.ll foobar qsort 

