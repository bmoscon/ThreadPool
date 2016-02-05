/*
 * thread_pool.hpp
 *
 *
 * Thread Pool Definitions
 *
 *
 * Copyright (C) 2012-2016  Bryant Moscon - bmoscon@gmail.com
 * 
 * Please see the LICENSE file for the terms and conditions 
 * associated with this software.
 *
 */

#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <vector>
#include <queue>
#include <cassert>
#include <cstring>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <thread>



typedef void (*work_fn)();



class ThreadPool {
    
public:
    ThreadPool() : _mon(), _stop(true), _thread_count(0) {}
    ThreadPool(uint32_t t) : _stop(true), _thread_count(t) {}
    ~ThreadPool();
    
    void start();
    void stop();
    void add_work(work_fn task);
    
private:
    void thread_entry();
    
    std::vector<std::thread> _thread_list;
    std::queue<work_fn> _task_list;
    std::mutex _lock;
    std::condition_variable _mon;
    bool _stop;
    uint32_t _thread_count;
    
};


#endif

