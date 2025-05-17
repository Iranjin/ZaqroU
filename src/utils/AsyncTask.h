#pragma once

#include <future>
#include <atomic>
#include <functional>
#include <optional>


template<typename T>
class AsyncTask
{
    std::future<T> m_future;
    std::atomic<bool> m_running = false;
    std::optional<T> m_result;

public:
    void run(std::function<T()> func)
    {
        if (m_running.load())
            return;
        m_running = true;
        m_future = std::async(std::launch::async, [this, func]() {
            T result = func();
            m_result = result;
            m_running = false;
            return result;
        });
    }

    std::optional<T> get_result()
    {
        if (!m_running && m_result.has_value())
        {
            std::optional<T> result = m_result;
            m_result.reset();
            return result;
        }
        return std::nullopt;
    }

    bool is_running() const { return m_running.load(); }
};

template<>
class AsyncTask<void>
{
    std::future<void> m_future;
    std::atomic<bool> m_running = false;
    std::atomic<bool> m_done = false;

public:
    void run(std::function<void()> func)
    {
        if (m_running.load())
            return;
        m_running = true;
        m_done = false;
        m_future = std::async(std::launch::async, [this, func]() {
            func();
            m_done = true;
            m_running = false;
        });
    }

    bool get_result()
    {
        if (!m_running && m_done.load())
        {
            m_done = false;
            return true;
        }
        return false;
    }

    bool is_running() const { return m_running.load(); }
    bool is_done() const { return m_done.load(); }
};
