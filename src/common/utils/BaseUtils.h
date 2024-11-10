//
// Created by LiangKeJin on 2024/11/10.
//

#pragma once

#define DELETE_TO_NULL(ptr)  if (ptr) { delete ptr; ptr = nullptr; }
#define DELETE_ARR_TO_NULL(ptr)  if (ptr) { delete [] ptr; ptr = nullptr; }
