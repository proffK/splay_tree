.PHONY: all static dynamic test clean
-include *.d

TEST_TARGET=splay_test

AR=ar
AR_FLAGS=rcs

INCLUDES=./include

CFLAGS=-Wall -Werror -fPIC -g --coverage -MD -I$(INCLUDES)

all: static test 

static: libsplay.a 

dynamic: libsplay.so

test: test.o static
	$(CC) -o $(TEST_TARGET) test.o -L. -lsplay -lgcov
	./$(TEST_TARGET)

libsplay.a: splay_tree.o
	$(CC) -shared -o $@ $^ -lgcov

libsplay.so: splay_tree.o
	$(CC) -shared -o $@ $^ -lgcov

clean:
	rm *.o *.d
