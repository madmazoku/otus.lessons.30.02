#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <list>

class ThreadLoop {
public:
    std::thread _thread;
    std::mutex _mutex;
    std::condition_variable _cv;

    bool _active;
    bool _terminate;

public:
    ThreadLoop(const ThreadLoop&) = delete;
    ThreadLoop(ThreadLoop&&) = delete;
    ThreadLoop(std::function<void (void)> lambda) : _active(false), _terminate(false) {
        _thread = std::thread([this, lambda]{
            while(true) {
                std::unique_lock<std::mutex> lock(_mutex);
                _cv.wait(lock, [&](){ return _active || _terminate; });
                if(_terminate)
                    break;
                lock.unlock();

                lambda();

                lock.lock();
                _active = false;
                _cv.notify_one();
            }
        });
    }

    void next() {
        std::unique_lock<std::mutex> lock(_mutex);
        _active = true;
        _cv.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [&](){ return !_active; });
    }

    void join() {
        std::unique_lock<std::mutex> lock(_mutex);
        _terminate = true;
        _cv.notify_one();
        lock.unlock();
        _thread.join();
    }
};

using ThreadLoopPtr = std::unique_ptr<ThreadLoop>;
using ThreadLoopPtrs = std::list<ThreadLoopPtr>;
