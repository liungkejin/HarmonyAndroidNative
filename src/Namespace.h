//
// Created by LiangKeJin on 2024/11/10.
//

#pragma once

// 为了防止缩进，IDE 无法修改namespace的缩进
#define NAMESPACE_BEG(ns) namespace ns {
#define NAMESPACE_END }

// 可以使用 -DDEFAULT_NAMESPACE=xxx 来修改默认的命名空间
#ifndef DEFAULT_NAMESPACE
#define DEFAULT_NAMESPACE znative
#endif

#ifndef NAMESPACE_DEFAULT
#define NAMESPACE_DEFAULT NAMESPACE_BEG(DEFAULT_NAMESPACE)
#endif