#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <type_traits>

class thread_pool 
{
private:
    struct thread_stop{};
public:
    explicit thread_pool(uint32_t thread_count = 1)
        : m_stop(false), m_count(std::clamp<uint32_t>(thread_count, 1, std::thread::hardware_concurrency()))
    {
         for (uint32_t i = 0; i < m_count; ++i)
            m_workers.emplace_back([this]() { worker_loop(); });
    }

    ~thread_pool() 
    {
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_stop = true;
        }
        m_condition.notify_all();

        for (auto& worker : m_workers)
        {
            if (worker.joinable())
                worker.join();
        }
    }

    void set_thread_count(uint32_t thread_count)
    {
        uint32_t new_count = std::clamp<uint32_t>(thread_count, 1, std::thread::hardware_concurrency());
        if (new_count == m_count)
            return;

        if (new_count > m_count)
        {
            for (uint32_t i = m_count; i < new_count; ++i)
            {
                m_workers.emplace_back([this]() { worker_loop(); });
            }
        }
        else
        {
            uint32_t threads_to_stop = m_count - new_count;

            for (uint32_t i = 0; i < threads_to_stop; ++i)
            {
                enqueue([this]() {
                    throw thread_stop{};
                    });
            }
        }

        m_count = new_count;
    }


    uint32_t thread_count() const
    {
        return m_count;
    }

    static uint32_t max_thread_count()
    {
        return std::thread::hardware_concurrency();
    }

    template<typename Func, typename... Args>
    auto enqueue(Func&& func, Args&&... args)-> std::future<std::invoke_result_t<Func, Args...>>
    {
        using return_type = std::invoke_result_t<Func, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);

            if (m_stop)
                throw std::runtime_error("thread_pool stopped");

            m_tasks.emplace([task]() { (*task)(); });
        }
        m_condition.notify_one();
        return res;
    }

    template<typename Func, typename InputContainer>
    auto enqueue_bulk(Func&& func, const InputContainer& inputs)-> std::vector<std::invoke_result_t<Func, typename InputContainer::value_type>>
    {
        using return_type = std::invoke_result_t<Func, typename InputContainer::value_type>;
        std::vector<std::future<return_type>> futures;
        futures.reserve(inputs.size());

        for (const auto& item : inputs) 
            futures.push_back(enqueue(func, item));

        std::vector<return_type> results;
        results.reserve(inputs.size());
        for (auto& fut : futures) 
            results.push_back(fut.get());
        return results;
    }

private:
    void worker_loop()
    {
        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_condition.wait(lock, [this]() { return m_stop || !m_tasks.empty(); });

                if (m_stop && m_tasks.empty())
                    return;

                task = std::move(m_tasks.front());
                m_tasks.pop();
            }

            try
            {
                task();
            }
            catch (thread_stop)
            {
                return;
            }
        }
    }


private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queue_mutex;
    std::condition_variable m_condition;
    uint32_t m_count;
    bool m_stop;
};