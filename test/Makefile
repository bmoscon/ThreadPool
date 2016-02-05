# Makefile for thread pool test program
# 
# Feb 2013 - Bryant Moscon


thread_pool_test: ../src/thread_pool.hpp ../src/thread_pool.cc thread_pool_test.cpp ../libthreadpool.a
	g++ -ggdb3 -Wall -Wextra -I../src -L../ -o thread_pool_test thread_pool_test.cpp -std=c++11 -pthread -lthreadpool

clean:
	rm -f thread_pool_test
