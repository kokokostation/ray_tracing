#ifndef CONTINUOUS_PERFORMER
#define CONTINUOUS_PERFORMER

#include <vector>
#include <future>

namespace ray_tracing
{

class Semaphore
{
private:
    std::mutex mutex;
    std::condition_variable condition_variable;
    size_t count;

public:
    Semaphore(size_t count = 0)
        : count(count)
    {}
    void decrease();
    void increase();
};

class Continuous_performer
{
private:
    const size_t WORKERS_NUM;

public:
    Continuous_performer(size_t workers_num = 4)
        : WORKERS_NUM(workers_num)
    {}

    void continuous_perform(std::vector<std::future<void>>& tasks);
};

}

#endif // CONTINUOUS_PERFORMER

