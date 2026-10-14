#include "ThreadPool.hpp"

#include <iostream>
#include <stdexcept>

ThreadPool::ThreadPool(
    std::size_t thread_count
)
    : stopping_(false)
{
    if (thread_count == 0)
    {
        thread_count = 1;
    }


    workers_.reserve(
        thread_count
    );


    for (
        std::size_t i = 0;
        i < thread_count;
        ++i
    )
    {
        workers_.emplace_back(
            &ThreadPool::worker,
            this
        );
    }
}


ThreadPool::~ThreadPool()
{
    shutdown();
}


void ThreadPool::enqueue(
    std::function<void()> task
)
{
    {
        std::lock_guard<std::mutex> lock(
            mutex_
        );


        if (stopping_)
        {
            throw std::runtime_error(
                "Cannot enqueue task: "
                "ThreadPool is shutting down."
            );
        }


        tasks_.push(
            std::move(task)
        );
    }


    condition_.notify_one();
}


void ThreadPool::worker()
{
    while (true)
    {
        std::function<void()> task;


        {
            std::unique_lock<std::mutex> lock(
                mutex_
            );


            condition_.wait(
                lock,
                [this]()
                {
                    return stopping_ ||
                           !tasks_.empty();
                }
            );


            // If shutdown was requested and
            // there are no remaining tasks,
            // this worker can exit.
            if (
                stopping_ &&
                tasks_.empty()
            )
            {
                return;
            }


            task =
                std::move(
                    tasks_.front()
                );

            tasks_.pop();
        }


        try
        {
            task();
        }
        catch (const std::exception& exception)
        {
            std::cerr
                << "[ThreadPool] Task exception: "
                << exception.what()
                << '\n';
        }
        catch (...)
        {
            std::cerr
                << "[ThreadPool] Unknown task exception.\n";
        }
    }
}


void ThreadPool::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(
            mutex_
        );

        if (stopping_)
        {
            return;
        }

        stopping_ = true;
    }


    condition_.notify_all();


    for (
        std::thread& worker_thread :
        workers_
    )
    {
        if (worker_thread.joinable())
        {
            worker_thread.join();
        }
    }


    workers_.clear();
}