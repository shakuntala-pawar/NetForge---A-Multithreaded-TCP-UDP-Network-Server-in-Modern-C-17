#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


class ThreadPool
{
public:

    explicit ThreadPool(
        std::size_t thread_count
    );

    ~ThreadPool();


    // Prevent copying
    ThreadPool(
        const ThreadPool&
    ) = delete;

    ThreadPool& operator=(
        const ThreadPool&
    ) = delete;


    // Add a task to the queue
    void enqueue(
        std::function<void()> task
    );


    // Stop all worker threads
    void shutdown();


private:

    // Worker thread function
    void worker();


private:

    std::vector<std::thread> workers_;

    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;

    std::condition_variable condition_;

    bool stopping_;
};