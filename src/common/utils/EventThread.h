//
// Created on 2024/8/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "eventpp/eventqueue.h"
#include "eventpp/callbacklist.h"
#include "Namespace.h"
#include <thread>

NAMESPACE_DEFAULT

typedef std::function<void()> Runnable;
typedef std::function<void(int)> EventHandler;

typedef int ListenerID;

/**
 * 实现类似 Android 的 HandlerThread
 */
class EventThread {
public:
    explicit EventThread(const char *name = "_event_thread") : m_name(name) {
        m_event_queue.appendListener(NORM_EVENT, [&](const Runnable &func) { func(); });
        m_thread = std::thread(&EventThread::threadLoop, this);
        m_thread.detach();
    }

    ~EventThread() { quit(); }

    ListenerID listenEvent(int event, const EventHandler &handler) {
        auto handle = m_handlers.appendListener(event, handler);
        std::lock_guard<std::mutex> lock(m_handle_mutex);
        ListenerID id = m_handle_id;
        m_handler_map[id] = handle;
        m_handle_id += 1;
        return id;
    }

    void removeListener(int event, ListenerID id) {
        std::lock_guard<std::mutex> lock(m_handle_mutex);
        auto it = m_handler_map.find(id);
        if (it != m_handler_map.end()) {
            m_handlers.removeListener(event, it->second);
            m_handler_map.erase(it);
        }
    }

    bool send(int event) {
        _WARN_RETURN_IF(!isRunning(), false, "thread has quit, failed to send event: %d", event);
        m_event_queue.enqueue(NORM_EVENT, [event, this]() { m_handlers.dispatch(event, event); });
        return true;
    }

    bool post(const Runnable &func) {
        _WARN_RETURN_IF(!isRunning(), false, "thread has quit, failed to post!");
        m_event_queue.enqueue(NORM_EVENT, func);
        return true;
    }
    
    void sync(const Runnable &func, int timeoutMs = -1) {
        _WARN_RETURN_IF(!isRunning(), void(), "thread has quit, failed to sync!");
        if (std::this_thread::get_id() == m_thread.get_id()) {
            // 当前已经在线程里面
            func();
        } else {
            std::unique_lock<std::mutex> lock(m_sync_mutex);
            Runnable proxy = [this, func]() {
                func();
                m_syncer.notify_all();
            };
            post(proxy);
            
            if (timeoutMs > 0) {
                std::cv_status status = m_syncer.wait_for(lock, std::chrono::milliseconds(timeoutMs));
                if (status == std::cv_status::timeout) {
                    _ERROR("sync timeout, timeMs: %d", timeoutMs);
                }
            } else {
                m_syncer.wait(lock);
            }
        }
    }

    bool isRunning() const { return m_running_flag; }

    void quit() {
        if (!m_running_flag) {
            return;
        }
        _INFO("thread(%s) start quit!", m_name.c_str());
        m_running_flag = false;
        m_event_queue.emptyQueue();
        m_event_queue.enqueue(EXIT_EVENT, []() {});
    }

private:
    void threadLoop() {
        std::string name = m_name;
        _INFO("thread(%s) start!", name.c_str());
        while (m_running_flag) {
            m_event_queue.process();
            if (m_running_flag) {
                m_event_queue.wait();
            }
        }
        _INFO("thread(%s) end!", name.c_str());
    }

private:
    std::string m_name;
    std::thread m_thread;
    volatile bool m_running_flag = true;
    
    std::mutex m_sync_mutex;
    std::condition_variable m_syncer;

    const int NORM_EVENT = 0;
    const int EXIT_EVENT = -1;

    int m_handle_id = 0;
    std::mutex m_handle_mutex;
    using Handle = eventpp::internal_::CallbackListBase<void(int), eventpp::DefaultPolicies>::Handle;
    std::unordered_map<int, Handle> m_handler_map;

    eventpp::EventDispatcher<int, void(int)> m_handlers;
    eventpp::EventQueue<int, void(const Runnable &)> m_event_queue;
};

NAMESPACE_END
