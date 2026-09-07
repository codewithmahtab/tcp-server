#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

template <typename T>
class ThreadSafeQueue {

private:

    std::queue<T> queue;

    mutable std::mutex mutex;

    std::condition_variable condition;

    bool stopped = false;

public:

    bool push(T value) {

        {
            std::lock_guard<std::mutex> lock(mutex);

            if (stopped) {
                return false;
            }

            queue.push(std::move(value));
        }

        condition.notify_one();

        return true;
    }


    bool waitAndPop(T& value) {

        std::unique_lock<std::mutex> lock(mutex);

        condition.wait(lock, [this] {
            return stopped || !queue.empty();
        });


        if (queue.empty()) {
            return false;
        }


        value = std::move(queue.front());

        queue.pop();

        return true;
    }


    void shutdown() {

        {
            std::lock_guard<std::mutex> lock(mutex);

            stopped = true;
        }

        condition.notify_all();
    }
};