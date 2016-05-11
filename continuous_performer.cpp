#include <vector>
#include <thread>
#include <future>
#include <algorithm>

#include "continuous_performer.h"

void ray_tracing::Semaphore::decrease()
{
    std::unique_lock<std::mutex> lock(mutex);
    while(count == 0)
        condition_variable.wait(lock);

    --count;
}

void ray_tracing::Semaphore::increase()
{
    mutex.lock();
    ++count;
    condition_variable.notify_one();
    mutex.unlock();
}

void helper(std::future<void>& task, ray_tracing::Semaphore& semaphore)
{
    task.get();
    semaphore.increase();
}

void ray_tracing::continuous_perform(std::vector<std::future<void>>& tasks)
{
    Semaphore semaphore(WORKERS_NUM);

    std::vector<std::thread> threads;

    for(std::future<void>& future : tasks)
    {
        semaphore.decrease();
        threads.emplace_back(helper, std::ref(future), std::ref(semaphore));
    }

    std::for_each(threads.begin(), threads.end(), [](std::thread& thread) {thread.join();});
}
