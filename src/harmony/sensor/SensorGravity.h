//
// Created on 2024/11/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "ZNamespace.h"
#include "SensorMgr.h"

NAMESPACE_DEFAULT

class SensorGravity {
public:
    static void startListen();

    static float getCurDegree();

    /**
     * 返回 0 90 180 270 角度
     *  V 0 度 
     *  > 90 度
     *  ^ 180
     *  < 270
     * 反方向
     * @return
     */
    static int getOrientation() {
        float degree = getCurDegree();
        if (degree >= 45 && degree <= 135) { // Use ROTATION_90 when degree range is [60, 120]
            return 90;
        } else if (degree >= 135 && degree <= 225) { // Use ROTATION_180 when degree range is [150, 210]
            return 180;
        } else if (degree >= 225 && degree <= 315) { // Use ROTATION_270 when degree range is [240, 300]
            return 270;
        }
        return 0;
    }

    static void stopListen();
};

NAMESPACE_END
