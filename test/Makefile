# Makefile for thread pool test program
# 
# Feb 2013 - Bryant Moscon


thread_pool_test: ../thread_pool.hpp thread_pool_test.cpp
	g++ -ggdb3 -Wall -Wextra -I../ -o thread_pool_test thread_pool_test.cpp -std=c++11 -pthread -Wl,--no-as-needed

clean:
	rm thread_pool_test
