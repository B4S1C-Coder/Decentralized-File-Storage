#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace fsn {

class ThreadPool {
public:
  explicit ThreadPool(size_t threadCount) : m_stopFlag(false) { start(threadCount); }
  ~ThreadPool() { stop(); }
  void enqueueTask(const std::function<void()>& task);

private:
  std::vector<std::thread> m_workers;             // Worker threads stored here
  std::queue<std::function<void()>> m_taskQueue;  // Tasks stored here
  std::mutex m_taskMutex;                         // Synchronization primitives (for thread safe access to 
  std::condition_variable m_taskCondition;        // shared resources)
  std::atomic<bool> m_stopFlag;                   // Shutdown flag for graceful shutdown

  void start(size_t threadCount);
  void stop();
};

}