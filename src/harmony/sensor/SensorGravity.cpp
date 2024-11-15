//
// Created on 2024/11/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "SensorGravity.h"

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

void SensorGravity::startListen() {
    _WARN_RETURN_IF(g_subscription_id, void(), "SensorGravity already start!");
    g_subscription_id = SensorMgr::subscribe(Sensor_Type::SENSOR_TYPE_GRAVITY, -1, onSensorEvent);
    _INFO("SensorGravity start listen, subscription id: %lld", g_subscription_id);
}

float SensorGravity::getCurDegree() {
    return g_cur_degree;
}

void SensorGravity::stopListen() {
    if (g_subscription_id != 0) {
        SensorMgr::unsubscribe(g_subscription_id);
        g_subscription_id = 0;
        _INFO("SensorGravity stop listen");
    }
}
NAMESPACE_END