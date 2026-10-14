#include "ThreadPool.hpp"

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>


int main()
{
    std::cout
        << "\n============================================\n"
        << "          THREAD POOL TEST\n"
        << "============================================\n";


    // --------------------------------------------------------
    // Create a thread pool with 3 worker threads
    // --------------------------------------------------------

    ThreadPool pool(3);


    std::mutex output_mutex;

    std::vector<int> completed_tasks;

    std::mutex completed_mutex;


    // --------------------------------------------------------
    // Submit 10 tasks
    // --------------------------------------------------------

    for (int task_id = 1; task_id <= 10; ++task_id)
    {
        pool.enqueue(
            [task_id,
             &output_mutex,
             &completed_tasks,
             &completed_mutex]()
            {
                {
                    std::lock_guard<std::mutex> lock(
                        output_mutex
                    );

                    std::cout
                        << "[Worker] Executing Task #"
                        << task_id
                        << " | Thread ID: "
                        << std::this_thread::get_id()
                        << '\n';
                }


                // Simulate some work
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );


                {
                    std::lock_guard<std::mutex> lock(
                        completed_mutex
                    );

                    completed_tasks.push_back(
                        task_id
                    );
                }


                {
                    std::lock_guard<std::mutex> lock(
                        output_mutex
                    );

                    std::cout
                        << "[Worker] Completed Task #"
                        << task_id
                        << '\n';
                }
            }
        );
    }


    // --------------------------------------------------------
    // Give workers time to finish
    // --------------------------------------------------------

    std::this_thread::sleep_for(
        std::chrono::milliseconds(1500)
    );


    // --------------------------------------------------------
    // Shutdown Thread Pool
    // --------------------------------------------------------

    pool.shutdown();


    // --------------------------------------------------------
    // Validate results
    // --------------------------------------------------------

    std::cout
        << "\n--------------------------------------------\n";

    std::cout
        << "Tasks submitted : 10\n";

    std::cout
        << "Tasks completed : "
        << completed_tasks.size()
        << '\n';


    if (completed_tasks.size() == 10)
    {
        std::cout
            << "\n[PASS] All tasks completed successfully.\n";
    }
    else
    {
        std::cout
            << "\n[FAIL] Some tasks were not completed.\n";

        return 1;
    }


    std::cout
        << "============================================\n";

    std::cout
        << "       THREAD POOL TEST PASSED\n";

    std::cout
        << "============================================\n";


    return 0;
}