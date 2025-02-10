//
// Created by LiangKeJin on 2024/11/15.
//

#pragma once

#include "ZNamespace.h"
#include "common/Object.h"

#define DELETE_TO_NULL(ptr)  if (ptr) { delete ptr; ptr = nullptr; }
#define DELETE_ARR_TO_NULL(ptr)  if (ptr) { delete [] ptr; ptr = nullptr; }

#define LOCK_MUTEX(m) std::lock_guard<std::mutex> lock__(m)
#define LOCK_MUTEX_BEGIN(m) { std::lock_guard<std::mutex> lock__(m);
#define LOCK_MUTEX_END }

NAMESPACE_DEFAULT

class ZUtils {
public:

};

NAMESPACE_END