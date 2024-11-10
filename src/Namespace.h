//
// Created by LiangKeJin on 2024/11/10.
//

#pragma once

#include "common/utils/Log.h"

// 为了防止缩进，IDE 无法修改namespace的缩进
#define NAMESPACE_BEG(ns) namespace ns {
#define NAMESPACE_END }

#ifndef NAMESPACE_DEFAULT
#define NAMESPACE_DEFAULT NAMESPACE_BEG(znative)
#endif