/*
 * thread_pool.hpp
 *
 *
 * Thread Pool
 *
 *
 * Copyright (C) 2012-2013  Bryant Moscon - bmoscon@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution, and in the same 
 *    place and form as other copyright,
 *    license and disclaimer information.
 *
 * 3. The end-user documentation included with the redistribution, if any, must 
 *    include the following acknowledgment: "This product includes software 
 *    developed by Bryant Moscon (http://www.bryantmoscon.org/)", in the same 
 *    place and form as other third-party acknowledgments. Alternately, this 
 *    acknowledgment may appear in the software itself, in the same form and 
 *    location as other such third-party acknowledgments.
 *
 * 4. Except as contained in this notice, the name of the author, Bryant Moscon,
 *    shall not be used in advertising or otherwise to promote the sale, use or 
 *    other dealings in this Software without prior written authorization from 
 *    the author.
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
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

