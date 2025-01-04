//
// Created by bigheadson on 2025/1/4.
//

#pragma once

#include <common/utils/YuvUtils.h>

#include "Namespace.h"
#include "common/Object.h"
#include "common/Log.h"
#ifdef __OPENCV__
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#endif

NAMESPACE_DEFAULT
enum ZImgFormat {
    F_UNKNOWN = 0,
    F_RGBA = 1,
    F_BGRA = 2,
    F_RGB = 3,
    F_BGR = 4,
    F_YUV_NV21 = 5,
    F_GRAY = 6
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
            case F_YUV_NV21:
                return cv::Mat(m_height * 3 / 2, m_width, CV_8UC1, m_data);
            case F_GRAY:
                return cv::Mat(m_height, m_width, CV_8UC1, m_data);
            default:
                _FATAL("unknown format: %d", m_format);
        }
    }

    cv::Mat resize(int w, int h) {
        if (m_format == F_YUV_NV21) {
            cv::Mat dst(h*3/2, w, CV_8UC1);
            YuvUtils::scaleNV21(m_data, m_width, m_height, dst.data, w, h, nullptr, 1);
            return dst;
        }
        cv::Mat src = this->mat();
        cv::Mat dst;
        cv::resize(src, dst, cv::Size(w, h));
        return dst;
    }

    cv::Mat convertTo(ZImgFormat dstFmt) {
        cv::Mat src = this->mat();
        cv::Mat dst;
        if (dstFmt == F_RGBA) {
            switch (this->m_format) {
                case F_RGBA:
                    dst = src.clone();
                    break;
                case F_BGRA:
                    cv::cvtColor(src, dst, cv::COLOR_BGRA2RGBA);
                    break;
                case F_RGB:
                    cv::cvtColor(src, dst, cv::COLOR_RGB2RGBA);
                    break;
                case F_BGR:
                    cv::cvtColor(src, dst, cv::COLOR_BGR2RGBA);
                    break;
                case F_YUV_NV21:
                    cv::cvtColor(src, dst, cv::COLOR_YUV2RGBA_NV21);
                    break;
                case F_GRAY:
                    cv::cvtColor(src, dst, cv::COLOR_GRAY2RGBA);
                    break;
                default:
                    _FATAL("unknown format: %d", m_format);
            }
        } else if (dstFmt == F_BGRA) {
            switch (this->m_format) {
                case F_RGBA:
                    cv::cvtColor(src, dst, cv::COLOR_RGBA2BGRA);
                    break;
                case F_BGRA:
                    dst = src.clone();
                    break;
                case F_RGB:
                    cv::cvtColor(src, dst, cv::COLOR_RGB2BGRA);
                    break;
                case F_BGR:
                    cv::cvtColor(src, dst, cv::COLOR_BGR2BGRA);
                    break;
                case F_YUV_NV21:
                    cv::cvtColor(src, dst, cv::COLOR_YUV2BGRA_NV21);
                    break;
                case F_GRAY:
                    cv::cvtColor(src, dst, cv::COLOR_GRAY2BGRA);
                default:
                    _FATAL("unknown format: %d", m_format);
            }
        } else if (dstFmt == F_BGR) {
            switch (this->m_format) {
                case F_RGBA:
                    cv::cvtColor(src, dst, cv::COLOR_RGBA2BGR);
                    break;
                case F_BGRA:
                    cv::cvtColor(src, dst, cv::COLOR_RGBA2BGR);
                    break;
                case F_RGB:
                    cv::cvtColor(src, dst, cv::COLOR_RGB2BGR);
                    break;
                case F_BGR:
                    dst = src.clone();
                    break;
                case F_YUV_NV21:
                    cv::cvtColor(src, dst, cv::COLOR_YUV2BGR_NV21);
                    break;
                case F_GRAY:
                    cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);
                default:
                    _FATAL("unknown format: %d", m_format);
            }
        } else if (dstFmt == F_RGB) {
            switch (this->m_format) {
                case F_RGBA:
                    cv::cvtColor(src, dst, cv::COLOR_RGBA2RGB);
                    break;
                case F_BGRA:
                    cv::cvtColor(src, dst, cv::COLOR_RGBA2RGB);
                    break;
                case F_RGB:
                    dst = src.clone();
                    break;
                case F_BGR:
                    cv::cvtColor(src, dst, cv::COLOR_BGR2RGB);
                    break;
                case F_YUV_NV21:
                    cv::cvtColor(src, dst, cv::COLOR_YUV2RGB_NV21);
                    break;
                case F_GRAY:
                    cv::cvtColor(src, dst, cv::COLOR_GRAY2RGB);
                default:
                    _FATAL("unknown format: %d", m_format);
            }
        } else if (dstFmt == F_YUV_NV21) {
            dst = cv::Mat(m_height * 3 / 2, m_width, CV_8UC1);
            switch (this->m_format) {
                case F_RGBA:
                    YuvUtils::rgbaToNV21(m_data, m_width, m_height, dst.data);
                    break;
                case F_BGRA:
                    YuvUtils::bgraToNV21(m_data, m_width, m_height, dst.data);
                    break;
                case F_RGB:
                    YuvUtils::rgbToNV21(m_data, m_width, m_height, dst.data);
                    break;
                case F_BGR:
                    YuvUtils::bgrToNV21(m_data, m_width, m_height, dst.data);
                    break;
                case F_YUV_NV21:
                    dst = src.clone();
                    break;
                case F_GRAY:
                    dst.setTo(128);
                    memcpy(dst.data, m_data, m_width * m_height);
                    break;
                default:
                    _FATAL("unknown format: %d", m_format);
            }
        } else if (dstFmt == F_GRAY) {
            switch (this->m_format) {
                case F_RGBA:
                    cv::cvtColor(src, dst, cv::COLOR_RGBA2GRAY);
                    break;
                case F_BGRA:
                    cv::cvtColor(src, dst, cv::COLOR_BGRA2GRAY);
                    break;
                case F_RGB:
                    cv::cvtColor(src, dst, cv::COLOR_RGB2GRAY);
                    break;
                case F_BGR:
                    cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
                    break;
                case F_YUV_NV21:
                    dst = cv::Mat(m_height, m_width, CV_8UC1);
                    memcpy(dst.data, m_data, m_width * m_height);
                    break;
                case F_GRAY:
                    dst = src.clone();
                    break;
                default:
                    _FATAL("unknown format: %d", m_format);
            }
        } else {
            _FATAL("unknown dst format: %d", dstFmt);
        }
        return dst;
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