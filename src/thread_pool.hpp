/*
 * thread_pool.hpp
 *
 *
 * Thread Pool Definitions
 *
 *
 * Copyright (C) 2012-2013  Bryant Moscon - bmoscon@gmail.com
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


typedef struct task_st {
  void (*fp)();
} task_st;


class ThreadPool {

public:
  ThreadPool() : mon_(), stop_(true), thread_count_(0) {}
  ThreadPool(uint32_t t) : stop_(true), thread_count_(t) {}
  ~ThreadPool()
  {
    stop();
  }
  
  void start()
  {
    lock_.lock();
    stop_ = false;
    
    if (!thread_count_) {
      thread_count_ = std::thread::hardware_concurrency();
      assert(thread_count_ > 0);
    }

    for (unsigned int i = 0; i < thread_count_; ++i) {
      thread_list_.push_back(std::thread(&ThreadPool::threadEntry, this));
      thread_list_[i].detach();
    }

    lock_.unlock();
  }

  void stop()
  {
    lock_.lock();

    stop_ = true;

    lock_.unlock();
  }
  
  void addWork(task_st& task)
  {
    lock_.lock();

    task_list_.push(task);
    mon_.notify_one();
    
    lock_.unlock();
  }
  
  void addWork(void (*fp)())
  {
    task_st task;
    task.fp = fp;

    addWork(task);
  }
  
private:
  
  void threadEntry()
  {
    task_st work;
    
    while (true) {
      lock_.lock();
      std::unique_lock<std::mutex> u_lck(lock_, std::adopt_lock);
      
      while(task_list_.empty()) {
	if (stop_) {
	  u_lck.unlock();
	  return;
	}
	
	mon_.wait(u_lck);
      }
      
      work = task_list_.front();
      task_list_.pop();
      
      u_lck.unlock();
      work.fp();
    }
  }
  
  std::vector<std::thread> thread_list_;
  std::queue<task_st> task_list_;
  std::mutex lock_;
  std::condition_variable mon_;
  bool stop_;
  uint32_t thread_count_;

};


#endif

