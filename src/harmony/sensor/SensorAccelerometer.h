//
// Created on 2025/2/13.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#pragma once

#ifndef WUTACAM_SENSORACCELEROMETER_H
#define WUTACAM_SENSORACCELEROMETER_H

#include <ZNamespace.h>
#include <common/Common.h>

NAMESPACE_DEFAULT

class SensorAccelerometer {
public:
    static void startListen(float shakeThreshold = 10);
    
    static void getCurAcc(float &ox, float &oy, float &oz);
    
    /**
     * 判断一段时间内是否有shake发生
     * @param durMs 默认 100ms
     */
    static bool hasShake(int durMs = 100);
    
    static void stopListen();
};

NAMESPACE_END

#endif //WUTACAM_SENSORACCELEROMETER_H
