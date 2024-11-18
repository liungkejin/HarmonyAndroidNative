//
// Created on 2024/11/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "SensorMgr.h"
#include <vector>
#include <map>

NAMESPACE_DEFAULT

static std::vector<SensorInfo> g_all_sensors;

static std::vector<SensorInfo> &getAllSensors() {
    if (!g_all_sensors.empty()) {
        return g_all_sensors;
    }

    uint32_t count = 0;
    int32_t error = OH_Sensor_GetInfos(nullptr, &count); // 获取设备上所有传感器的个数。 
    _ERROR_RETURN_IF(error, g_all_sensors, "OH_Sensor_GetInfos() failed, error: %d", error);

    Sensor_Info **sensors = OH_Sensor_CreateInfos(count);
    _ERROR_RETURN_IF(sensors == nullptr, g_all_sensors, "OH_Sensor_CreateInfos(%d) failed", count);

    uint32_t realCount = count;
    error = OH_Sensor_GetInfos(sensors, &realCount);
    if (error) {
        OH_Sensor_DestroyInfos(sensors, count);
        _ERROR("OH_Sensor_GetInfos failed: %d", error);
        return g_all_sensors;
    }

    for (int i = 0; i < realCount; ++i) {
        SensorInfoWrap wrap(sensors[i]);
        std::string s = wrap.toString();
        _INFO("sensor[%d]: %s", i, s);
        g_all_sensors.emplace_back(wrap);
    }

    OH_Sensor_DestroyInfos(sensors, count);
    return g_all_sensors;
}

const SensorInfo *SensorMgr::findSensor(Sensor_Type type) {
    auto sensors = getAllSensors();
    for (auto &i: sensors) {
        if (i.type() == type) {
            return &i;
        }
    }
    return nullptr;
}

class SubscribedBox {
public:
    SubscribedBox(SubscriptionId &_id, SubscriptionAttribute &_attr, SensorSubscriber &_sub) : id(_id), attr(_attr),
                                                                                               subscriber(_sub) {}

    SubscribedBox(const SubscribedBox &o) : id(o.id), attr(o.attr), subscriber(o.subscriber) {}

public:
    SubscriptionId id;
    SubscriptionAttribute attr;
    SensorSubscriber subscriber;
};

static std::unordered_map<int64_t, SubscribedBox> g_subscription_map;
static std::mutex g_mutex;

int64_t SensorMgr::subscribe(Sensor_Type type, int64_t samplingInterval, Sensor_EventCallback callback) {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto info = findSensor(type);
    if (info == nullptr) {
        _ERROR("Not find sensor type of %d", type);
        return 0;
    }
    SubscriptionId id;
    id.setType(type);
    SubscriptionAttribute attr;
    if (samplingInterval < 0) { // NOLINT(*-branch-clone)
        samplingInterval = info->maxSamplingInterval();
    } else if (samplingInterval < info->minSamplingInterval()) {
        samplingInterval = info->minSamplingInterval();
    } else if (samplingInterval > info->maxSamplingInterval()) {
        samplingInterval = info->maxSamplingInterval();
    }
    attr.setSamplingInterval(samplingInterval);
    SensorSubscriber subscriber;
    subscriber.setCallback(callback);
    auto error = OH_Sensor_Subscribe(id.m_id, attr.m_id, subscriber.m_id);
    _WARN_RETURN_IF(error, 0, "OH_Sensor_Subscribe() failed, error: %d", error);

    auto subId = (int64_t) subscriber.m_id;
    g_subscription_map.insert(std::make_pair(subId, SubscribedBox(id, attr, subscriber)));
    _INFO("SensorMgr subscribe sensor type: %d, sampling interval: %lld, subscription id: %lld", type, samplingInterval,
          subId);
    return subId;
}

void SensorMgr::unsubscribe(int64_t subId) {
    std::lock_guard<std::mutex> lock(g_mutex);
    
    auto it = g_subscription_map.find(subId);
    if (it != g_subscription_map.end()) {
        auto &box = it->second;
        auto error = OH_Sensor_Unsubscribe(box.id.m_id, box.subscriber.m_id);
        _WARN_IF(error, "OH_Sensor_Subscribe failed: %d", error);
        _INFO("SensorMgr unsubscribe sensor type: %d, subscription id: %lld", box.id.type(), subId);
        g_subscription_map.erase(it);
    } else {
        _WARN("SensorMgr unsubscribe not found subscription id: %lld", subId);
    }
}

NAMESPACE_END