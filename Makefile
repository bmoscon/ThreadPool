all: src/thread_pool.hpp src/thread_pool.cc
	g++ -O3 -ggdb3 -Wall -Wextra -I./src -c src/thread_pool.cc -o src/thread_pool.o -std=c++11 -pthread
	ar rcs libthreadpool.a src/thread_pool.o

clean:
	rm -f src/thread_pool.o libthreadpool.a

