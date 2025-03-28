//
// Created on 2024/6/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "ZNamespace.h"
#include "common/Object.h"
#include "common/Log.h"

#include <cstdint>

NAMESPACE_DEFAULT

// TODO 这个 Array 需要优化
class Array : Object {
public:
    Array() {}

    Array(const Array &other) : Object(other),
                                m_data(other.m_data),
                                m_capacity(other.m_capacity), m_put_size(other.m_put_size) {}

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

    const uint8_t *bytes(int index = 0) const { return m_data + index; }

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
        if (src) {
            memcpy(dst, src, size * unitSize);
        }
        m_put_size = size * unitSize;

        return m_put_size / sizeof(T);
    }

    /**
     * 注意： 这里只有在 put 之后才能使用 size() 方法
     */
    template<typename T>
    int size() {
        return m_put_size / sizeof(T);
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
    size_t m_put_size = 0;
};

template <typename T>
class FlexArray {
public:
    FlexArray() {}
    FlexArray(const FlexArray &other) : m_data(other.m_data),
                                        m_data_size(other.m_data_size), m_capacity(other.m_capacity) {}

    FlexArray& operator=(const FlexArray &other) {
        m_data = other.m_data;
        m_data_size = other.m_data_size;
        m_capacity = other.m_capacity;
        return *this;
    }

    bool operator == (const T *other) {
        return *m_data == other;
    }

    T &operator[](size_t index) {
        _FATAL_IF(index >= m_data_size, "Invalid index(%d) array size: %d", index, m_data_size);
        return m_data[index];
    }

public:
    size_t size() const { return m_data_size; }
    size_t capacity() const { return m_capacity; }

    T* obtain(size_t size) {
        if (size > m_capacity) {
            m_capacity = size;
            m_data = std::make_shared<T*>(new T[size]);
        }
        m_data_size = size;
        return *m_data;
    }

    T * data() { return *m_data; }

    void free() {
        m_data = nullptr;
        m_data_size = 0;
        m_capacity = 0;
    }

private:
    std::shared_ptr<T*> m_data = nullptr;
    size_t m_data_size = 0;
    size_t m_capacity = 0;
};

NAMESPACE_END
