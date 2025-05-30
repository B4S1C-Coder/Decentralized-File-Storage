#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include "thread_pool.hh"

void fsn::ThreadPool::enqueueTask(const std::function<void()>& task) {
  {
    std::unique_lock<std::mutex> lock(m_taskMutex);
    m_taskQueue.push(task);
  }

  m_taskCondition.notify_one();
}

void fsn::ThreadPool::start(size_t threadCount) {
  for (size_t i = 0; i < threadCount; i++) {
    m_workers.emplace_back([this]() {
      while (true) {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(m_taskMutex);
          m_taskCondition.wait(lock, [this]() {
            return m_stopFlag || !m_taskQueue.empty();
          });

          if (m_stopFlag && m_taskQueue.empty()) { break; }

          task = std::move(m_taskQueue.front());
          m_taskQueue.pop();
        }

        // Execute the task
        task();

      }
    });
  }
}

void fsn::ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(m_taskMutex);
    m_stopFlag = true;
  }

  m_taskCondition.notify_all();

  for (std::thread& worker : m_workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}