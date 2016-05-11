#ifndef CONTINUOUS_PERFORMER
#define CONTINUOUS_PERFORMER

#include <vector>
#include <future>

namespace ray_tracing
{

const size_t WORKERS_NUM = 4;

class Semaphore
{
private:
    std::mutex mutex;
    std::condition_variable condition_variable;
    size_t count;

public:
    Semaphore(size_t count = 0) : count(count) {}
    void decrease();
    void increase();
};

void continuous_perform(std::vector<std::future<void>>& tasks);

}

#endif // CONTINUOUS_PERFORMER

