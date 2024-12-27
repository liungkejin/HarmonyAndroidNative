//
// Created on 2024/11/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "SensorGravity.h"
#include "common/Log.h"
#include "common/utils/Base.h"
#include "common/utils/EventThread.h"

NAMESPACE_DEFAULT

static volatile float g_cur_degree = 0;

static void onSensorEvent(Sensor_Event *e) {
    SensorEvent event(e);
    float degree = event.calGravityDegree();
    if (degree >= 0) {
//        _INFO("sensor degree: %.2lf", degree);
        g_cur_degree = degree;
    }
}

static int64_t g_subscription_id = 0;
static volatile bool g_stop_flag = false;
static EventThread* g_listen_thread = nullptr;
static std::mutex g_mutex;

void SensorGravity::startListen() {
    std::lock_guard<std::mutex> lock(g_mutex);
    _WARN_RETURN_IF(g_listen_thread != nullptr, void(), "SensorGravity already start!");
    if (g_listen_thread == nullptr) {
        g_listen_thread = new EventThread("gravity_sensor");
    }
    _INFO("SensorGravity start listen");
    g_cur_degree = 0;
    g_stop_flag = false;
    g_listen_thread->post([]() {
        if (g_subscription_id != 0) {
            return;
        }
        int tryCount = 0;
        while (!g_stop_flag && g_subscription_id == 0 && tryCount < 10) {
            g_subscription_id = SensorMgr::subscribe(
                Sensor_Type::SENSOR_TYPE_GRAVITY, -1, onSensorEvent);
            tryCount += 1;
            if (g_subscription_id == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                _WARN("SensorGravity subscribe failed, try again: %d", tryCount);
            }
        }
        
        if (g_subscription_id != 0) {
            _INFO("SensorGravity start listen success! id = %d", g_subscription_id);
        } else {
            _WARN("SensorGravity subscribe failed!");
        }
    });
}

float SensorGravity::getCurDegree() {
    if (g_stop_flag) {
        return 0;
    }
    return g_cur_degree;
}

void SensorGravity::stopListen() {
    if (g_stop_flag) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    g_stop_flag = true;
    if (g_listen_thread) {
        g_listen_thread->post([]() {
            if (g_subscription_id != 0) {
                SensorMgr::unsubscribe(g_subscription_id);
                g_subscription_id = 0;
                _INFO("SensorGravity stop listen");
            }
            g_cur_degree = 0;
        });
        g_listen_thread->quit();
        DELETE_TO_NULL(g_listen_thread);
    }
}
NAMESPACE_END