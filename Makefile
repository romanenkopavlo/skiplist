CC := gcc
CFLAGS := -Wall -Werror -pedantic -g -O0
# CFLAGS := -Wall -Werror -O0 -fprofile-arcs -ftest-coverage # Aussi ajouter -lgcov
LFLAGS := 
EXECUTABLES := skiplist.test

CFLAGS := -Wall -O0 -fprofile-arcs -ftest-coverage
LIBS := -lgcov

all: $(EXECUTABLES)

clean:
	rm -f *.o *.so $(EXECUTABLES) *.gcno *.gcov *.gcda

skiplist.test: skiplist.test.o skiplist.o
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -I. -c -o $@ $^

archive:
	tar -cvzf skiplist.tar.gz Makefile skiplist.h skiplist.c skiplist.test.c

memcheck:
	valgrind --tool=memcheck ./skiplist.test

coverage:
	mkdir -p lcov-report
	lcov --capture --directory . --output-file lcov-report/coverage.info
	genhtml lcov-report/coverage.info --output-directory lcov-report
