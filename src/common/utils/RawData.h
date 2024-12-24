//
// Created by LiangKeJin on 2024/11/10.
//

#pragma once

#include "Base.h"
#include "common/Object.h"

NAMESPACE_DEFAULT

class RawData : Object {
public:
    RawData() = default;

    explicit RawData(size_t size) : m_size(size), m_owned(true) {
        m_data = new uint8_t[size];
    }

    RawData(uint8_t *data, size_t size, bool owned = false) : m_data(data), m_size(size), m_owned(owned) {}

    RawData(const RawData& o) : m_size(o.m_size), m_data(o.m_data), m_owned(o.m_owned), Object(o) {}

    ~RawData() {
        if (m_owned && no_reference() && m_data) {
            DELETE_ARR_TO_NULL(m_data)
        }
    }

    inline const uint8_t *data() const { return m_data; }

    inline uint8_t *data() { return m_data; }

    inline size_t size() const { return m_size; }

    std::string toString() {
        return {(const char *) m_data, m_size};
    }
private:
    uint8_t * m_data = nullptr;
    size_t m_size = 0;
    bool m_owned = true;
};

NAMESPACE_END