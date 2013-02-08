# Makefile for thread pool test program
# 
# Feb 2013 - Bryant Moscon


thread_pool_test: thread_pool_test.o
	g++ -o thread_pool_test thread_pool_test.o -lpthread

thread_pool_test.o: thread_pool_test.cpp ../src/thread_pool.hpp ../src/thread.hpp ../src/lock.hpp ../src/monitor.hpp
	g++ -c thread_pool_test.cpp

clean:
	rm thread_pool_test thread_pool_test.o
