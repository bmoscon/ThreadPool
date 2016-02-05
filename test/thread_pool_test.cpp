/*
 * thread_pool_test.cpp
 *
 *
 * Thread Pool Test Program
 *
 *
 * Copyright (C) 2012-2016  Bryant Moscon - bmoscon@gmail.com
 * 
 * Please see the LICENSE file for the terms and conditions 
 * associated with this software.
 *
 */


//todo: finish. This is just a placeholder


#include <iostream>
#include <cassert>
#include <unistd.h>
#include <thread>

#include "thread_pool.hpp"


void work() {
    for(int i = 0; i < 100; ++i) {
	if (i == 56) {
	    std::cout << "count is 56!\n\n";
	    std::cout.flush();
	}
    }
    sleep(1);
}

int main() {
    
    ThreadPool pool;
    
    pool.add_work(&work);
    pool.start();
    
    sleep(3);
    return(0);
}
