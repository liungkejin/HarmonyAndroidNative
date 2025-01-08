//
// Created on 2024/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "Namespace.h"
#include <cstdio>
#include <iostream>
#include <memory>

NAMESPACE_DEFAULT
    // 可以进行引用计数的对象

class Object {
public:
    bool no_reference() const { return m_ref_ptr.use_count() <= 1; }
    
    long reference_count() const { return m_ref_ptr.use_count(); }

protected:
    void reset_reference() { m_ref_ptr = std::make_shared<int>(); }

    std::shared_ptr<int> m_ref_ptr = std::make_shared<int>();
};

NAMESPACE_END