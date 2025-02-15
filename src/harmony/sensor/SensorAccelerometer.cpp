//
// Created on 2025/2/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "SensorAccelerometer.h"
#include "SensorMgr.h"
#include "common/Log.h"
#include "common/utils/Base.h"
#include "common/utils/EventThread.h"
#include <cstdint>


NAMESPACE_DEFAULT

static volatile float g_acc_x = 0, g_acc_y = 0, g_acc_z = 0;
static int64_t g_shake_ptms = -1;
static int g_shake_threshold = 10;

static void onSensorEvent(Sensor_Event *e) {
    SensorEvent event(e);
    uint32_t length;
    float *data = event.getData(length);
    if (length < 3) {
        return;
    }
    g_acc_x = data[0];
    g_acc_y = data[1];
    g_acc_z = data[2];
    double q = sqrt(g_acc_x*g_acc_x + g_acc_y*g_acc_y + g_acc_z*g_acc_z);
//    _INFO("acc %.2f, %.2f, %.2f, %.2f", g_acc_x, g_acc_y, g_acc_z, q);
    if (g_shake_threshold > 0 && q > g_shake_threshold) {
//        _INFO("on shake detect!!");
        g_shake_ptms = TimeUtils::nowMs();
    }
}

static int64_t g_subscription_id = 0;
static volatile bool g_stop_flag = false;
static EventThread* g_listen_thread = nullptr;
static std::mutex g_mutex;

void SensorAccelerometer::startListen(float shakeThreshold) {
    std::lock_guard<std::mutex> lock(g_mutex);
    _WARN_RETURN_IF(g_listen_thread != nullptr, void(), "SensorAccelerometer already start!");
    if (g_listen_thread == nullptr) {
        g_listen_thread = new EventThread("accelerometer_sensor");
    }
    _INFO("_SensorAccelerometer start listen");
    g_acc_x = g_acc_y = g_acc_z = 0;
    g_shake_threshold = shakeThreshold;
    g_stop_flag = false;
    g_listen_thread->post([]() {
        if (g_subscription_id != 0) {
            return;
        }
        int tryCount = 0;
        while (!g_stop_flag && g_subscription_id == 0 && tryCount < 10) {
            g_subscription_id = SensorMgr::subscribe(
                Sensor_Type::SENSOR_TYPE_ACCELEROMETER, -1, onSensorEvent);
            tryCount += 1;
            if (g_subscription_id == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                _WARN("_SensorAccelerometer subscribe failed, try again: %d", tryCount);
            }
        }
        
        if (g_subscription_id != 0) {
            _INFO("_SensorAccelerometer start listen success! id = %d", g_subscription_id);
        } else {
            _WARN("_SensorAccelerometer subscribe failed!");
        }
    });
}

void SensorAccelerometer::getCurAcc(float &ox, float &oy, float &oz) {
    ox = g_acc_x;
    oy = g_acc_y;
    oz = g_acc_z;
}

bool SensorAccelerometer::hasShake(int durMs) {
    if (TimeUtils::nowMs() - g_shake_ptms < durMs) {
        return true;
    }
    return false;
}

void SensorAccelerometer::stopListen() {
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
                _INFO("_SensorAccelerometer stop listen");
            }
        });
        g_listen_thread->quit();
        DELETE_TO_NULL(g_listen_thread);
    }
}
NAMESPACE_END