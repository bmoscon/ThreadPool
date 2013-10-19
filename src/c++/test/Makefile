# Makefile for thread pool test program
# 
# Feb 2013 - Bryant Moscon


thread_pool_test: thread_pool_test.o
	g++ -Wall -Wextra -I../ -o thread_pool_test thread_pool_test.o -lpthread

thread_pool_test.o: thread_pool_test.cpp ../thread_pool.hpp ../thread.hpp ../lock.hpp ../monitor.hpp
	g++ -c -Wall -Wextra -I../ thread_pool_test.cpp

clean:
	rm thread_pool_test thread_pool_test.o
