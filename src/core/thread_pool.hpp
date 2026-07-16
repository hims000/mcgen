#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

namespace mcgen {

// Thread pool for parallel chunk generation
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = 0);  // 0 = auto-detect
    ~ThreadPool();

    // Submit a task and get future result
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using return_type = decltype(f(args...));

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queueMutex_);

            if (stop_) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks_.emplace([task](){ (*task)(); });
        }
        condition_.notify_one();
        return res;
    }

    // Wait for all tasks to complete
    void waitAll();

    // Get number of threads
    size_t size() const { return workers_.size(); }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_{false};
    std::atomic<size_t> activeTasks_{0};
};

} // namespace mcgen
