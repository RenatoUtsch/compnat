/*
 * Copyright 2017 Renato Utsch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COMPNAT_TP1_THREADING_HPP
#define COMPNAT_TP1_THREADING_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include <glog/logging.h>

namespace threading {

/**
 * Super simple and efficient thread pool for embarrassingly parallel tasks.
 */
class ThreadPool {
public:
  /**
   * Constructs the thread pool.
   * By default, the number of threads is the hardware concurrency.
   */
  ThreadPool(size_t numThreads = std::thread::hardware_concurrency())
      : finishedCount_(0), counter_(0), end_(0), shouldJoin_(false) {
    for (size_t i = 0; i < numThreads; ++i) {
      threads_.emplace_back([this]() { worker_(); });
    }
  }

  ~ThreadPool() { join(); }

  // Non-copyable/non-movable
  ThreadPool(ThreadPool &other) = delete;
  ThreadPool(ThreadPool &&other) = delete;
  ThreadPool &operator=(ThreadPool &other) = delete;
  ThreadPool &operator=(ThreadPool &&other) = delete;

  /// Sets the given task to run, from begin to end, and waits.
  void run(size_t begin, size_t end, std::function<void(size_t)> taskFn) {
    taskFn_ = taskFn;
    finishedCount_ = 0;
    counter_ = begin;
    end_ = end;
    waitingCondition_.notify_all();

    std::unique_lock<std::mutex> lock(finishedMutex_);
    finishedCondition_.wait(lock);
  }

  /// Joins with all threads. Pool unusable after calling this.
  void join() {
    shouldJoin_ = true;
    waitingCondition_.notify_all();
    for (auto &t : threads_) {
      t.join();
    }
  }

private:
  /// Function ran by a single worker.
  void worker_() {
    while (true) {
      if (shouldJoin_) {
        return;
      }

      const size_t index = counter_++;
      if (index < end_) {
        taskFn_(index);
      } else {
        const size_t numFinishedThreads = ++finishedCount_;
        if (numFinishedThreads == threads_.size()) {
          finishedCondition_.notify_one();
        }

        std::unique_lock<std::mutex> lock(waitingMutex_);
        waitingCondition_.wait(lock);
      }
    }
  }

  std::vector<std::thread> threads_;
  std::function<void(size_t)> taskFn_;

  std::atomic_size_t finishedCount_;
  std::atomic_size_t counter_;
  size_t end_;

  bool shouldJoin_;
  std::condition_variable finishedCondition_;
  std::mutex finishedMutex_;
  std::condition_variable waitingCondition_;
  std::mutex waitingMutex_;
};

} // namespace threading

#endif // !COMPNAT_TP1_THREADING_HPP
