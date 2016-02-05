/*
 * thread_pool.hpp
 *
 *
 * Thread Pool Methods
 *
 *
 * Copyright (C) 2012-2016  Bryant Moscon - bmoscon@gmail.com
 * 
 * Please see the LICENSE file for the terms and conditions 
 * associated with this software.
 *
 */


#include "thread_pool.hpp"


ThreadPool::~ThreadPool()
{
    stop();
    for (uint32_t i = 0; i < _thread_count; ++i) {
	_thread_list[i].join();
    }
}


void ThreadPool::start()
{
    _lock.lock();
    _stop = false;
    
    if (!_thread_count) {
	_thread_count = std::thread::hardware_concurrency();
	assert(_thread_count > 0);
    }
    
    for (uint32_t i = 0; i < _thread_count; ++i) {
	_thread_list.push_back(std::thread(&ThreadPool::thread_entry, this));
    }
    
    _lock.unlock();
}


void ThreadPool::stop()
{
    _lock.lock();
    
    _stop = true;
    _mon.notify_all();
    
    _lock.unlock();
}


void ThreadPool::add_work(work_fn task)
{
    _lock.lock();
    
    _task_list.push(task);
    _mon.notify_one();
    
    _lock.unlock();
}


void ThreadPool::thread_entry()
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
