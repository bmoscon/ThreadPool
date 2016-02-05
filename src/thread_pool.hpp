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
    ThreadPool(uint32_t t) : stop_(true), _thread_count(t) {}
    ~ThreadPool()
    {
	stop();
	for (uint32_t i = 0; i < _thread_count; ++i) {
	    _thread_list[i].join();
	}
    }
    
    void start()
    {
	_lock.lock();
	_stop = false;
	
	if (!_thread_count) {
	    _thread_count = std::thread::hardware_concurrency();
	    assert(_thread_count > 0);
	}
	
	for (unsigned int i = 0; i < _thread_count; ++i) {
	    _thread_list.push_back(std::thread(&ThreadPool::threadEntry, this));
	}
	
	_lock.unlock();
    }
    
    void stop()
    {
	_lock.lock();
	
	stop_ = true;
	
	_lock.unlock();
    }
    
    void addWork(work_fn task)
    {
	_lock.lock();
	
	_task_list.push(task);
	_mon.notify_one();
	
	_lock.unlock();
    }
    
private:
  
    void threadEntry()
    {
	work_fn work;
	
	while (true) {
	    _lock.lock();
	    std::unique_lock<std::mutex> u_lck(_lock, std::adopt_lock);
	    
	    while(_task_list.empty()) {
		if (_stop) {
		    u_lck.unlock();
		    return;
		}
		
		_mon.wait(u_lck);
	    }
	    
	    work = _task_list.front();
	    _task_list.pop();
	    
	    u_lck.unlock();
	    work();
	}
    }
    
    std::vector<std::thread> _thread_list;
    std::queue<work_fn> _task_list;
    std::mutex _lock;
    std::condition_variable _mon;
    bool _stop;
    uint32_t _thread_count;
    
};


#endif

