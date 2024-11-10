//
// Created on 2024/5/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include "ZNative.h"

NAMESPACE_DEFAULT

// 可以进行引用计数的对象

class Object {
public:
    inline bool no_reference() { return m_ref_ptr.use_count() <= 1; }
    
    inline int reference_count() { return m_ref_ptr.use_count(); }
    
private:
    std::shared_ptr<int> m_ref_ptr = std::make_shared<int>();
};

NAMESPACE_END