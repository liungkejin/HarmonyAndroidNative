//
// Created on 2024/6/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "Namespace.h"
#include "common/Object.h"
#include <cstdint>

NAMESPACE_DEFAULT

class Array : Object {
public:
    Array() {}

    Array(const Array &other) : m_capacity(other.m_capacity),
                                m_data_size(other.m_data_size),
                                m_data(other.m_data), Object(other) {}

    ~Array() {
        if (m_data && no_reference()) {
            delete[] m_data;
            m_data = nullptr;
        }
    }

public:

    /**
     * @return 当前分配的内存大小
     */
    size_t capacity() const { return m_capacity; }

    const uint8_t *bytes(int index = 0) { return m_data + index; }

    /**
     * @param index 偏移量
     * @return 内存地址
     */
    template<typename T>
    T *data(int index = 0) {
        size_t unitSize = sizeof(T);
        size_t bi = index * unitSize;
        _FATAL_IF(bi < 0 || bi > m_capacity - unitSize, "Invalid index(%d) array bytes size: %d, unit size: %d", index,
                  m_capacity, unitSize);
        T *d = (T *) m_data;
        return d + index;
    }

    template<typename T>
    T at(int index) { return *data<T>(index); }

    template<typename T>
    T *obtain(size_t size, bool strict = false) {
        size_t unitSize = sizeof(T);
        return (T *) obtainBytes(size * unitSize, strict);
    }

    /**
     * @return 返回实际写入的元素个数
     */
    template<typename T>
    int put(const T *src, size_t size, bool strict = false) {
        size_t unitSize = sizeof(T);
        uint8_t *dst = obtainBytes(size * unitSize, strict);
        memcpy(dst, src, size * unitSize);
        m_data_size = size * unitSize;

        return m_data_size / sizeof(T);
    }

    void free() {
        delete[] m_data;
        m_data = nullptr;
        m_capacity = 0;
    }

private:
    /**
     * @param strict 严格模式下，只要 size 与当前容量同时就会重新分配内存
     * @return 新的内存地址
     */
    uint8_t *obtainBytes(size_t size, bool strict = false) {
        bool needReallocate = size > m_capacity;
        if (strict) {
            needReallocate = size != m_capacity;
        }
        if (needReallocate) {
            delete[] m_data;
            if (size > 0) {
                m_data = new uint8_t[size];
            } else {
                m_data = nullptr;
            }
            m_capacity = size;
        }
        return m_data;
    }

private:
    uint8_t *m_data = nullptr;
    size_t m_capacity = 0;
    size_t m_data_size = 0;
};

NAMESPACE_END
