//
// Created on 2024/4/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "common/utils/Log.h"

// 为了防止缩进，IDE 无法修改namespace的缩进
#define NAMESPACE_BEG(ns) namespace ns {
#define NAMESPACE_END }

#define DELETE_TO_NULL(ptr)  if (ptr) { delete ptr; ptr = nullptr; }
#define DELETE_ARR_TO_NULL(ptr)  if (ptr) { delete [] ptr; ptr = nullptr; }

#define NAMESPACE_DEFAULT NAMESPACE_BEG(znative)