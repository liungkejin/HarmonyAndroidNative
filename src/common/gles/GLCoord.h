//
// Created on 2024/6/28.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "GLUtil.h"
#include "common/utils/MathUtils.h"
#include <mutex>

NAMESPACE_DEFAULT

class GLRect {
public:
    // 用于顶点坐标
    static GLRect fitCenter(float tw, float th, float vw, float vh) {
        GLRect rect(0, 0, tw, th);
        float scale = std::min(vw / tw, vh / th);
        float dx = (vw - tw * scale) / 2;
        float dy = (vh - th * scale) / 2;
        rect.scale(scale, scale);
        rect.translate(dx, dy);
        return rect;
    }

    // 用于定点坐标
    static GLRect centerCrop(float tw, float th, float vw, float vh) {
        GLRect rect(0, 0, tw, th);
        float scale = std::max(vw / tw, vh / th);
        float dx = (vw - tw * scale) / 2;
        float dy = (vh - th * scale) / 2;
        rect.scale(scale, scale);
        rect.translate(dx, dy);
        return rect;
    }
    
    // 从纹理的尺寸中抠出 vw, vh 比例的中心区域
    static GLRect texCenter(float tw, float th, float vw, float vh) {
        float scale = std::min(tw/vw, th/vh);
        float vtw = vw * scale;
        float vth = vh * scale;
        return GLRect((tw - vtw)/2, (th - vth)/2, vtw, vth);
    }

public:
    GLRect() {}

    GLRect(float _w, float _h) : x(0), y(0), width(_w), height(_h) {};

    GLRect(float _x, float _y, float _w, float _h) : x(_x), y(_y), width(_w), height(_h) {};

    GLRect(const GLRect &o)
            : x(o.x), y(o.y), width(o.width), height(o.height), rotation(o.rotation), flipH(o.flipH), flipV(o.flipV) {}

    GLRect &operator=(const GLRect &o) {
        x = o.x;
        y = o.y;
        width = o.width;
        height = o.height;
        rotation = o.rotation;
        flipH = o.flipH;
        flipV = o.flipV;
        return *this;
    }
    
    inline float _width() { return this->width; }
    inline float _height() { return this->height; }

    bool empty() const { return width <= 0 && height <= 0; }

    void reset() {
        x = 0.0f;
        y = 0.0f;
        width = 0.0f;
        height = 0.0f;
        rotation = 0.0f;
        flipH = false;
        flipV = false;
    }

    GLRect &setRect(float _x, float _y, float _w, float _h) {
        x = _x;
        y = _y;
        width = _w;
        height = _h;
        return *this;
    }

    GLRect &setRotation(float angle) {
        return setRotation((x + width)/2.0f, (y + height)/2.0f, angle);
    }

    GLRect &setRotation(float cx, float cy, float angle) {
        rcx = cx;
        rcy = cy;
        rotation = angle;
        rotation = fmod(rotation, 360.0f);
        return *this;
    }

    GLRect &setFlipH(bool h) {
        flipH = h;
        return *this;
    }

    GLRect &setFlipV(bool v) {
        flipV = v;
        return *this;
    }

    GLRect &setFlip(bool h, bool v) {
        flipH = h;
        flipV = v;
        return *this;
    }

    GLRect &translate(float _x, float _y) {
        this->x += _x;
        this->y += _y;
        return *this;
    }

    GLRect &scale(float scaleX, float scaleY) {
        this->width *= scaleX;
        this->height *= scaleY;
        return *this;
    }

    GLRect &centerScale(float sx, float sy) {
        float cx = (x + width) / 2.0f;
        float cy = (y + height) / 2.0f;
        float w = width * sx;
        float h = height * sy;
        x = cx - w / 2.0f;
        y = cy - h / 2.0f;
        width = w;
        height = h;
        return *this;
    }

    /**
     * 计算顶点坐标
     * @param viewW 视图宽
     * @param viewH
     * @param coords 输出的坐标
     */
    void toVertexCoords(float viewW, float viewH, float *coords) const {
        toGLCoords(viewW, viewH, coords);
        for (int i = 0; i < 4; i++) {
            coords[i * 2] = coords[i * 2] * 2.0f - 1.0f;
            coords[i * 2 + 1] = 1.0f - coords[i * 2 + 1] * 2.0f;
        }
    }

    /**
     * 计算纹理坐标
     * @param texWidth 纹理宽
     * @param texHeight
     * @param coords 输出的坐标
     */
    void toTextureCoords(float texWidth, float texHeight, float *coords) const {
        toGLCoords(texWidth, texHeight, coords);
    }

private:
    void toGLCoords(float outW, float outH, float *coords) const {
        // 按照default的顺序，依次是 坐下，右下，左上，右上
        float x1 = x, y1 = y + height;
        float x2 = x + width, y2 = y + height;
        float x3 = x, y3 = y;
        float x4 = x2, y4 = y;

        if (rotation != 0) {
            float cosTheta = (float) cos(rotation * M_PI / 180.0f);
            float sinTheta = (float) sin(rotation * M_PI / 180.0f);
            MathUtils::rotatePoint(x1, y1, rcx, rcy, sinTheta, cosTheta);
            MathUtils::rotatePoint(x2, y2, rcx, rcy, sinTheta, cosTheta);
            MathUtils::rotatePoint(x3, y3, rcx, rcy, sinTheta, cosTheta);
            MathUtils::rotatePoint(x4, y4, rcx, rcy, sinTheta, cosTheta);
        }

        // 必须先旋转再归一化，不然旋转后的尺寸就变了
        x1 /= outW, y1 /= outH;
        x2 /= outW, y2 /= outH;
        x3 /= outW, y3 /= outH;
        x4 /= outW, y4 /= outH;

        coords[0] = x1, coords[1] = y1;
        coords[2] = x2, coords[3] = y2;
        coords[4] = x3, coords[5] = y3;
        coords[6] = x4, coords[7] = y4;
        if (flipH) {
            std::swap(coords[0], coords[2]);
            std::swap(coords[1], coords[3]);
            std::swap(coords[4], coords[6]);
            std::swap(coords[5], coords[7]);
        }
        if (flipV) {
            std::swap(coords[0], coords[6]);
            std::swap(coords[1], coords[7]);
            std::swap(coords[2], coords[4]);
            std::swap(coords[3], coords[5]);
        }
    }

private:
    float x = 0, y = 0, width = 0, height = 0;
    float rcx = 0, rcy = 0, rotation = 0; // 旋转角度
    bool flipH = false, flipV = false;    // 翻转
};

class GLCoord {
public:
    virtual ~GLCoord() {
        delete[] m_coords;
        m_coords = nullptr;
        m_size = 0;
        m_cap = 0;
    }

    void setToDefault() {
        set(nullptr, 0, GL_TRIANGLE_STRIP, 4);
    }

    void set(const float *coord, int size, GLenum drawMode, int drawCount) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        if (coord == nullptr) {
            drawMode = GL_TRIANGLE_STRIP;
            drawCount = 4;
            coord = getDefault(size);
        }
        float *dst = obtainCoords(size);
        memcpy(dst, coord, sizeof(float) * size);
        m_size = size;
        m_draw_mode = drawMode;
        m_draw_count = drawCount;
    }

    const float *get(int &size) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        if (m_coords == nullptr) {
            return getDefault(size);
        }
        size = m_size;
        return m_coords;
    }

    GLenum drawMode() const {
        return m_draw_mode;
    }

    int drawCount() const {
        return m_draw_count;
    }

protected:
    virtual const float *getDefault(int &size) = 0;

    float *obtainCoords(int size) {
        if (m_coords == nullptr || m_cap < size) {
            delete[] m_coords;
            m_coords = new float[size];
            m_cap = size;
        }
        return m_coords;
    }

    std::mutex m_update_mutex;
protected:
    float *m_coords = nullptr;
    int m_cap = 0;

    int m_size = 0;
    GLenum m_draw_mode = GL_TRIANGLE_STRIP;
    int m_draw_count = 4;
};

class TextureCoord : public GLCoord {
#define TEX_COORD_SIZE 8
public:
    const float *setFullCoord(int rot, bool flipH, bool flipV) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        int size = TEX_COORD_SIZE;
        float *coords = obtainCoords(size);
        const float *srcCoords;
        rot = rot % 360;
        if (rot == 90) {
            srcCoords = ROTATED_90;
        } else if (rot == 180) {
            srcCoords = ROTATED_180;
        } else if (rot == 270) {
            srcCoords = ROTATED_270;
        } else {
            _FATAL_IF(rot != 0, "setFullCoord() rotation value must be one of { 0, 90, 180, 270 }");
            srcCoords = ROTATED_0;
        }
        memcpy(coords, srcCoords, size * sizeof(float));

        if (flipH) {
            coords[0] = flip(coords[0]);
            coords[2] = flip(coords[2]);
            coords[4] = flip(coords[4]);
            coords[6] = flip(coords[6]);
        }
        if (flipV) {
            coords[1] = flip(coords[1]);
            coords[3] = flip(coords[3]);
            coords[5] = flip(coords[5]);
            coords[7] = flip(coords[7]);
        }
        m_size = size;
        m_draw_mode = GL_TRIANGLE_STRIP;
        m_draw_count = 4;
        return coords;
    }

    const float *centerCrop(float texW, float texH, float viewW, float viewH, bool flipH, bool flipV) {
        float x, y, w, h;
        float vratio = viewW / viewH;
        if (texW * viewH > texH * viewW) {
            h = viewH;
            w = h * vratio;
        } else {
            w = viewW;
            h = w / vratio;
        }

        x = (texW - w) / 2.0f;
        y = (texH - h) / 2.0f;
        return setRect(x, y, w, h, texW, texH, flipH, flipV);
    }

    /**
     * 计算 x, y , width, height 区域在 texWidth, textHeight 下经过 flipH, flipV 之后的纹理坐标
     * @return 纹理坐标
     */
    const float *setRect(float x, float y, float w, float h, float texW, float texH, bool flipH, bool flipV) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        int size = TEX_COORD_SIZE;
        float *coords = obtainCoords(size);
        coords[0] = x / texW;
        coords[1] = y / texH;
        coords[6] = (x + w) / texW;
        coords[7] = (y + h) / texH;
        coords[2] = coords[6];
        coords[3] = coords[1];
        coords[4] = coords[0];
        coords[5] = coords[7];

        if (flipH) {
            std::swap(coords[0], coords[2]);
            std::swap(coords[1], coords[3]);
            std::swap(coords[4], coords[6]);
            std::swap(coords[5], coords[7]);
        }
        if (flipV) {
            std::swap(coords[0], coords[6]);
            std::swap(coords[1], coords[7]);
            std::swap(coords[2], coords[4]);
            std::swap(coords[3], coords[5]);
        }

        m_size = size;
        m_draw_mode = GL_TRIANGLE_STRIP;
        m_draw_count = 4;
        return coords;
    }

    const float *setByGLRect(float texW, float texH, const GLRect &rect) {
        int size = TEX_COORD_SIZE;
        m_draw_mode = GL_TRIANGLE_STRIP;
        m_draw_count = 4;
        if (rect.empty()) {
            const float *d = GLCoord::get(size);
            m_size = size;
            return d;
        }
        std::lock_guard<std::mutex> lock(m_update_mutex);
        float *coords = obtainCoords(size);
        rect.toTextureCoords(texW, texH, coords);
        m_size = size;
        return coords;
    }

protected:
    const float *getDefault(int &size) override {
        size = TEX_COORD_SIZE;
        return ROTATED_0;
    }

private:
    float flip(float i) { return 1.0f - i; }

    const float ROTATED_0[TEX_COORD_SIZE] = {
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    };

    const float ROTATED_90[TEX_COORD_SIZE] = {
            1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    const float ROTATED_180[TEX_COORD_SIZE] = {
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    const float ROTATED_270[TEX_COORD_SIZE] = {
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    };
};

class VertexCoord : public GLCoord {
#define VERTEX_COORD_SIZE 8
public:
    /**
     * 矩形区域 x, y, w, h 在 viewW, viewH 坐标系下 flipH, flipV , rotate 之后的坐标
     */
    const float *setByGLRect(float viewW, float viewH, const GLRect &rect) {
        int size = VERTEX_COORD_SIZE;
        m_draw_mode = GL_TRIANGLE_STRIP;
        m_draw_count = 4;
        if (rect.empty()) {
            const float *d = GLCoord::get(size);
            m_size = size;
            return d;
        }
        std::lock_guard<std::mutex> lock(m_update_mutex);
        float *coords = obtainCoords(size);
        rect.toVertexCoords(viewW, viewH, coords);
        m_size = size;
        return coords;
    }

protected:
    const float *getDefault(int &size) override {
        size = VERTEX_COORD_SIZE;
        return DEFAULT_COORDS;
    }

private:
    const float DEFAULT_COORDS[VERTEX_COORD_SIZE] = {
            -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    };
};

NAMESPACE_END
