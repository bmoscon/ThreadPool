# Makefile for thread pool test program
# 
# Feb 2013 - Bryant Moscon


thread_pool_test: ../thread_pool.hpp thread_pool_test.cpp
	g++ -ggdb3 -Wall -Wextra -I../src -o thread_pool_test thread_pool_test.cpp -std=c++11 -pthread

clean:
	rm -f thread_pool_test
