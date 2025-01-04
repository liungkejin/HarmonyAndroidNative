//
// Created by bigheadson on 2025/1/4.
//

#pragma once

#include "Namespace.h"
#include "common/Object.h"
#include "common/Log.h"
#ifdef __OPENCV__
#include <opencv2/core/mat.hpp>
#endif

NAMESPACE_DEFAULT

enum ZImgFormat {
    F_UNKNOWN = 0,
    F_RGBA = 1,
    F_BGRA = 2,
    F_RGB = 3,
    F_BGR = 4,
    F_YUV_NV21 = 5,
    F_YUV_NV12 = 6,
    F_GRAY = 7
};

class ZImage : public Object {
public:
    ZImage() {
    }

    ZImage(uint8_t *data, int w, int h, int fmt, bool owner)
        : m_data(data), m_width(w), m_height(h), m_format(fmt), m_owner(owner) {
    }

    ZImage(const ZImage &o)
        : Object(o), m_data(o.m_data), m_width(o.m_width), m_height(o.m_height), m_format(o.m_format),
          m_owner(o.m_owner) {
    }

    ZImage &operator=(const ZImage &o) {
        if (this != &o) {
            this->release();

            m_data = o.m_data;
            m_width = o.m_width;
            m_height = o.m_height;
            m_format = o.m_format;
            m_owner = o.m_owner;
            m_ref_ptr = o.m_ref_ptr;
        }
        return *this;
    }

    ~ZImage() { this->release(); }

public:
    void create(int w, int h, int fmt) {
        this->release();
        m_format = fmt;
        m_width = w;
        m_height = h;
        m_owner = true;

        int s = this->size();
        m_data = new uint8_t[s];
    }

    void put(const uint8_t *d, int w, int h, int fmt) {
        this->release();
        m_format = fmt;
        m_width = w;
        m_height = h;
        m_owner = true;

        int s = this->size();
        m_data = new uint8_t[s];
        memcpy(m_data, d, s);
    }

    void wrap(uint8_t *d, int w, int h, int fmt, bool owner) {
        this->release();
        m_format = fmt;
        m_width = w;
        m_height = h;
        m_owner = owner;
        m_data = d;
    }

public:
    uint8_t *data() { return m_data; }

    int size() const {
        switch (m_format) {
            case F_RGBA:
            case F_BGRA:
                return m_width * m_height * 4;
            case F_BGR:
            case F_RGB:
                return m_width * m_height * 3;
            case F_YUV_NV12:
            case F_YUV_NV21:
                return m_width * m_height * 3 / 2;
            case F_GRAY:
                return m_width * m_height;
            default:
                _ERROR("unknown format: %d", m_format);
                return 0;
        }
    }

    int width() const { return m_width; }

    int height() const { return m_height; }

    int format() const { return m_format; }

    #ifdef __OPENCV__
    cv::Mat mat() {
        switch (m_format) {
            case F_RGBA:
            case F_BGRA:
                return cv::Mat(m_height, m_width, CV_8UC4, m_data);
            case F_BGR:
            case F_RGB:
                return cv::Mat(m_height, m_width, CV_8UC3, m_data);
            case F_YUV_NV12:
            case F_YUV_NV21:
                return cv::Mat(m_height * 3 / 2, m_width, CV_8UC1, m_data);
            case F_GRAY:
                return cv::Mat(m_height, m_width, CV_8UC1, m_data);
            default:
                _FATAL("unknown format: %d", m_format);
        }
    }
    #endif

private:
    void release() {
        if (m_owner && no_reference() && m_data) {
            delete[] m_data;
            m_data = nullptr;
        }
        this->reset_reference();
    }

private:
    uint8_t *m_data = nullptr;

    int m_width = 0;
    int m_height = 0;
    int m_format = 0;

    bool m_owner = true;
};

NAMESPACE_END