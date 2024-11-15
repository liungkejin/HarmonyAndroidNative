//
// Created by LiangKeJin on 2024/11/15.
//

#pragma once

#include "Namespace.h"
#include "common/Object.h"

#define DELETE_TO_NULL(ptr)  if (ptr) { delete ptr; ptr = nullptr; }
#define DELETE_ARR_TO_NULL(ptr)  if (ptr) { delete [] ptr; ptr = nullptr; }