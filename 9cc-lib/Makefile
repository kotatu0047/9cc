CFLAGS=-std=c++17 -g -static -Wall -O2
CC = g++

# 9cc: 9cc.c
# 	$(CC) $(CFLAGS) $^ -o $@

# foo: foo.c
# 	$(CC) $(CFLAGS) $^ -o $@

# test: 9cc
# 	./test.sh

# clean: 
# 	rm -f 9cc *.o *~ tmp*

# .PHONY: test clean

# CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean