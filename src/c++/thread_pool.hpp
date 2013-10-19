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



typedef struct task_st {
  void     (*fp)(void *);
  void      *opaque;
  uint32_t   run_count;

  task_st() : fp(NULL), opaque(NULL), run_count(1) {}
  
  task_st(const task_st &s) {
    fp = s.fp;
    opaque = s.opaque;
    run_count = s.run_count;
  }

} task_st;


template <class T, class M, class L>
class ThreadPool {

public:
  ThreadPool(const uint32_t num_threads) : thread_list_(num_threads), stop_(true) {}

  ThreadPool() : thread_list_(num_cores()), stop_(true) {}

  ~ThreadPool()
  {
    stop();
    while (task_list_.size()) {
      delete task_list_.front();
      task_list_.pop();
    }
  }

  void addWork(const task_st *work) 
  {
    addWork(work->fp, work->opaque, work->run_count);    
  }
    
  void addWork(void (*fp)(void *), void *opaque, uint32_t run_count = 1)
  {
    task_st *new_work = new task_st();
    new_work->fp = fp;
    new_work->opaque = opaque;
    new_work->run_count = run_count;
    
    if (stop_) {
      task_list_.push(new_work);
    } else {
      lock_.lock();
      
      task_list_.push(new_work);
      mon_.signal();
      
      lock_.unlock();
    }
  }
  
  void stop()
  {
    lock_.lock();
    stop_ = true;
    lock_.unlock();
  }
  
  void start() 
  { 
    assert(lock_.init() == 0);
    assert(mon_.init() == 0);
    stop_ = false;
    
    for (uint32_t i = 0; i < thread_list_.size(); ++i) {
      assert(thread_list_[i].spawn("Worker Thread", threadEntry, this) == 0);
    } 
    
  }
  
  
private:
  
  static void *threadEntry(void *opaque)
  {
    static_cast<ThreadPool *>(opaque)->doWork();
    
    return (NULL);
  }
  
  void doWork() 
  {
    task_st *work = NULL;
    while (true) {
      
      lock_.lock();
      
      while (task_list_.empty()) {
	if (stop_) {
	  lock_.unlock();
	  T::exit();
	} 
	
        mon_.wait(lock_);
      }
      
      
      work = task_list_.front();
      task_list_.pop();
      
      lock_.unlock();
      
      
      if (work) { 
	while (work->run_count) {
	  work->fp(work->opaque);
	  --work->run_count;
	}
        
        delete work;
	work = NULL;
	
      } else {
	//TODO: Log some error
      }
    }
  }
  
  inline void cpuid(uint32_t &eax, uint32_t &ebx, uint32_t &ecx, uint32_t &edx) const
  {
    asm ( "cpuid;"
	  :  "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
	  : "a"   (eax), "c"  (0)
	);
  }
  
  uint32_t num_cores() const
  {
    uint32_t eax, ebx, ecx, edx;
    char vendor[13];
    uint32_t cores;

    eax = 0;
    cpuid(eax, ebx, ecx, edx);
    
    *((uint32_t *)vendor) = ebx;
    *(((uint32_t *)vendor) + 1) = edx;
    *(((uint32_t *)vendor) + 2) = ecx;
    vendor[12] = '\0';
    
    eax = 1;
    cpuid(eax, ebx, ecx, edx);
    cores = (ebx >> 16) & 0xff;
    
    
    if (strcmp(vendor, "GenuineIntel") == 0) {
      eax = 4;
      cpuid(eax, ebx, ecx, edx);
      cores = ((eax >> 26) & 0x3f) + 1;
    } else if (strcmp(vendor, "AuthenticAMD") == 0) {
      eax = 0x80000008;
      cpuid(eax, ebx, ecx, edx);
      cores = ((unsigned)(ecx & 0xff)) + 1;
    }
    
    return (cores);
  }
  


  std::vector<T> thread_list_;
  std::queue<task_st *> task_list_;
  L lock_;
  M mon_;
  bool stop_;

};


#endif

