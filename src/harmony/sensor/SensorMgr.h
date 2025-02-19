//
// Created on 2024/11/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "ZNamespace.h"
#include "common/Object.h"
#include "common/Log.h"
#include <cstdint>
#include <sensors/oh_sensor.h>
#include <sensors/oh_sensor_type.h>

NAMESPACE_DEFAULT

class SensorMgr;

class SubscriptionId : Object {
    friend class SensorMgr;

public:
    SubscriptionId() {
        m_id = OH_Sensor_CreateSubscriptionId();
        _FATAL_IF(m_id == nullptr, "OH_Sensor_CreateSubscriptionId failed")
    }

    SubscriptionId(const SubscriptionId &o) : Object(o), m_type(o.m_type), m_id(o.m_id) {}

    ~SubscriptionId() {
        if (no_reference() && m_id) {
            OH_Sensor_DestroySubscriptionId(m_id);
            m_id = nullptr;
        }
    }

public:
    void setType(const Sensor_Type type) {
        m_type = type;
        auto error = OH_SensorSubscriptionId_SetType(m_id, type);
        _ERROR_IF(error, "OH_SensorSubscriptionId_SetType(%d) failed, error: %d", type, error);
    }

    inline int type() const { return m_type; }

private:
    int m_type = -1;
    Sensor_SubscriptionId *m_id;
};

class SubscriptionAttribute : Object {
    friend class SensorMgr;

public:
    SubscriptionAttribute() {
        m_id = OH_Sensor_CreateSubscriptionAttribute();
        _FATAL_IF(m_id == nullptr, "OH_Sensor_CreateSubscriptionAttribute failed")
    }

    SubscriptionAttribute(const SubscriptionAttribute &o) : Object(o), m_id(o.m_id) {}

    ~SubscriptionAttribute() {
        if (no_reference() && m_id) {
            OH_Sensor_DestroySubscriptionAttribute(m_id);
            m_id = nullptr;
        }
    }

public:
    void setSamplingInterval(const int64_t samplingInterval) {
        auto error = OH_SensorSubscriptionAttribute_SetSamplingInterval(m_id, samplingInterval);
        _ERROR_IF(error, "OH_SensorSubscriptionAttribute_SetSamplingInterval(%lld) failed, error: %d", samplingInterval,
                  error);
    }

private:
    Sensor_SubscriptionAttribute *m_id;
};

class SensorEvent : Object {
    friend class SensorMgr;

public:
    explicit SensorEvent(Sensor_Event *e) : m_event(e) {}

    SensorEvent(const SensorEvent &o) : Object(o), m_event(o.m_event) {}

public:
    Sensor_Type type() {
        Sensor_Type t = Sensor_Type::SENSOR_TYPE_ACCELEROMETER;
        auto error = OH_SensorEvent_GetType(m_event, &t);
        _ERROR_IF(error, "OH_SensorEvent_GetType() failed, error: %d", error);
        return t;
    }

    int64_t timestamp() {
        int64_t t = 0;
        auto error = OH_SensorEvent_GetTimestamp(m_event, &t);
        _ERROR_RETURN_IF(error, 0, "OH_SensorEvent_GetTimestamp() failed, error: %d", error);
        return t;
    }

    Sensor_Accuracy accuracy() {
        Sensor_Accuracy a = Sensor_Accuracy::SENSOR_ACCURACY_UNRELIABLE;
        auto error = OH_SensorEvent_GetAccuracy(m_event, &a);
        _ERROR_IF(error, "OH_SensorEvent_GetAccuracy() failed, error: %d", error);
        return a;
    }

    float *getData(uint32_t &length) {
        float *data = nullptr;
        auto error = OH_SensorEvent_GetData(m_event, &data, &length);
        _ERROR_RETURN_IF(error, nullptr, "OH_SensorEvent_GetData() failed, error: %d", error);
        return data;
    }

    /**
     * @return Gravity Sensor 计算的手机角度, 保证 0 - 360, 如果出现 < 0 的情况则表示无效值
     */
    float calGravityDegree() {
        _ERROR_RETURN_IF(type() != Sensor_Type::SENSOR_TYPE_GRAVITY, -1,
                         "SensorEvent.type != GRAVITY, calGravityDegree failed!");

        uint32_t length = 0;
        float *data = getData(length);
        if (data == nullptr || length < 3) {
            return -1;
        }

        float x = data[0], y = data[1], z = data[2];
//        _INFO("gravity: x: %.2lf, y: %.2lf, z: %.2lf", x, y, z);
        if ((x * x + y * y) * 3 < z * z) {
            return -1; // 不可信
        }
        double degree = atan2(y, x) / M_PI * 180.0 - 90;
        int id = (int) degree;
        auto d = (float) (degree - id);
        id = ((id + 360) % 360 + 360) % 360;
        return (float) id + d;
    }

private:
    Sensor_Event *m_event;
};

class SensorSubscriber : Object {
    friend class SensorMgr;

public:
    SensorSubscriber() {
        m_id = OH_Sensor_CreateSubscriber();
        _FATAL_IF(m_id == nullptr, "OH_Sensor_CreateSubscriptionAttribute failed")
    }

    SensorSubscriber(const SensorSubscriber &o) : m_id(o.m_id), Object(o) {}

    ~SensorSubscriber() {
        if (no_reference() && m_id) {
            OH_Sensor_DestroySubscriber(m_id);
            m_id = nullptr;
        }
    }

public:
    void setCallback(const Sensor_EventCallback callback) {
        auto error = OH_SensorSubscriber_SetCallback(m_id, callback);
        _ERROR_IF(error, "OH_SensorSubscriber_SetCallback() failed, error: %d", error);
    }

private:
    Sensor_Subscriber *m_id;
};

class SensorInfoWrap : Object {
    friend class SensorMgr;

public:
    explicit SensorInfoWrap(Sensor_Info *info) : m_info(info) {}

    SensorInfoWrap(const SensorInfoWrap &o) : m_info(o.m_info), Object(o) {}

public:
    std::string name() {
        char name[64] = {0};
        uint32_t length = 64;
        auto error = OH_SensorInfo_GetName(m_info, name, &length);
        _ERROR_RETURN_IF(error, "", "OH_SensorInfo_GetName() failed, error: %d", error);
        return {name, strlen(name)};
    }

    std::string vendorName() {
        char name[64] = {0};
        uint32_t length = 64;
        auto error = OH_SensorInfo_GetVendorName(m_info, name, &length);
        _ERROR_RETURN_IF(error, "", "OH_SensorInfo_GetVendorName() failed, error: %d", error);
        return {name, strlen(name)};
    }

    Sensor_Type type() {
        Sensor_Type t = Sensor_Type::SENSOR_TYPE_ACCELEROMETER;
        auto error = OH_SensorInfo_GetType(m_info, &t);
        _ERROR_IF(error, "OH_SensorInfo_GetType() failed, error: %d", error);
        return t;
    }

    float resolution() {
        float r = 0;
        auto error = OH_SensorInfo_GetResolution(m_info, &r);
        _ERROR_RETURN_IF(error, 0, "OH_SensorInfo_GetResolution() failed, error: %d", error);
        return r;
    }

    int64_t minSamplingInterval() {
        int64_t interval = 0;
        auto error = OH_SensorInfo_GetMinSamplingInterval(m_info, &interval);
        _ERROR_RETURN_IF(error, interval, "OH_SensorInfo_GetMinSamplingInterval() failed, error: %d", error);
        return interval;
    }

    int64_t maxSamplingInterval() {
        int64_t interval = 0;
        auto error = OH_SensorInfo_GetMaxSamplingInterval(m_info, &interval);
        _ERROR_RETURN_IF(error, interval, "OH_SensorInfo_GetMaxSamplingInterval() failed, error: %d", error);
        return interval;
    }

public:
    std::string toString() {
        std::stringstream ss;
        auto n = name();
        auto vn = vendorName();
        ss << "(name=" << n << ", vendorName=" << vn
           << ", type=" << type() << ", resolution=" << resolution()
           << ", samplingInterval[" << minSamplingInterval() << "," << maxSamplingInterval() << ")";
        return ss.str();
    }

private:
    Sensor_Info *m_info;
};

class SensorInfo {
public:
    explicit SensorInfo(SensorInfoWrap wrap)
            : m_name(wrap.name()), m_vendor_name(wrap.vendorName()), m_type(wrap.type()),
              m_resolution(wrap.resolution()),
              m_min_interval(wrap.minSamplingInterval()), m_max_interval(wrap.maxSamplingInterval()) {}

    SensorInfo(std::string &n, std::string &vn, Sensor_Type t, float res, int64_t mini, int64_t maxi)
            : m_name(n), m_vendor_name(vn), m_type(t), m_resolution(res), m_min_interval(mini), m_max_interval(maxi) {}

    SensorInfo(const SensorInfo &o)
            : m_name(o.m_name), m_vendor_name(o.m_vendor_name), m_type(o.m_type), m_resolution(o.m_resolution),
              m_min_interval(o.m_min_interval), m_max_interval(o.m_max_interval) {}

public:
    inline std::string name() const { return m_name; }

    inline std::string vendorName() const { return m_vendor_name; }

    inline Sensor_Type type() const { return m_type; }

    inline float resolution() const { return m_resolution; }

    inline int64_t minSamplingInterval() const { return m_min_interval; }

    inline int64_t maxSamplingInterval() const { return m_max_interval; }

public:
    std::string toString() {
        std::stringstream ss;
        ss << "(name=" << m_name << ", vendorName=" << m_vendor_name
           << ", type=" << m_type << ", resolution=" << m_resolution
           << ", samplingInterval[" << m_min_interval << "," << m_max_interval << "])";
        return ss.str();
    }

private:
    const std::string m_name;
    const std::string m_vendor_name;
    const Sensor_Type m_type;
    const float m_resolution;
    const int64_t m_min_interval;
    const int64_t m_max_interval;
};

class SensorMgr {
public:
    static const SensorInfo *findSensor(Sensor_Type type);

    /**
     * 返回一个 id
     * @param type
     * @param samplingInterval 如果传入 -1 表示使用最大间隔, 其他 >= 0 的值会被规范至 [min, max] 范围
     * @param callback
     * @return 一个 subscribe id, 根据这个 id 来取消订阅, 返回 0 表示错误
     */
    static int64_t subscribe(Sensor_Type type, int64_t samplingInterval, Sensor_EventCallback callback);

    static void unsubscribe(int64_t subId);
};

NAMESPACE_END
