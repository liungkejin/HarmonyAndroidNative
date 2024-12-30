//
// Created by LiangKeJin on 2024/12/27.
//

#pragma once

#include "Namespace.h"
#include "common/Object.h"
#include "BaseFilter.h"

NAMESPACE_DEFAULT
/**
 * 锐化滤镜
 */
class SharpenFilter : public BaseFilter {
public:
    SharpenFilter() : BaseFilter("sharpen") {
        defAttribute("position", DataType::FLOAT_POINTER)->bind(vertexCoord());
        defUniform("resolution", DataType::FVEC4)->set(1.0f, 1.0f, 1.0f, 1.0f);
        defUniform("strength", DataType::FLOAT)->set(0.0f);
        defUniform("pin0", DataType::SAMPLER_2D);
        // identity matrix
        float array[16] = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
        defUniform("pin0Transform", DataType::FMAT4)->put(array);
    }

    void setStrength(float strength) {
        uniform("strength")->set(strength);
    }

    void setResolution(float w, float h) {
        uniform("resolution")->set(w, h, 1.f, 1.f);
    }

    void inputTexture(int id) {
        uniform("pin0")->set(id);
    }

    void inputTexture(Texture2D &texture) {
        uniform("pin0")->set((int) texture.id());
    }

protected:
    std::string vertexShader() override {
        std::string vs = R"(
attribute highp vec4 position;
varying highp vec4 screenCoordinate;
void main() {
  screenCoordinate = position;
  gl_Position = position;
})";
        return CORRECT_VERTEX_SHADER(vs);
    }

    std::string fragmentShader() override {
        std::string fs = R"(
lowp vec4 blend(lowp vec4 src, lowp vec4 append) {
    return vec4(src.rgb * (1.0 - append.a) + append.rgb,
                1.0 - (1.0 - src.a) * (1.0 - append.a));
}

lowp vec3 screen(lowp vec3 s, lowp vec3 d) { return s + d - s * d; }

lowp vec4 screen(lowp vec4 s, lowp vec4 d) { return vec4(clamp(screen(s.rgb, d.rgb), 0., s.a), s.a); }

lowp vec3 lighten(lowp vec3 s, lowp vec3 d) { return max(s, d); }

lowp float hardLight(lowp float s,lowp float d )
{
    return (s < 0.5) ? 2.0 * s * d : 1.0 - 2.0 * (1.0 - s) * (1.0 - d);
}

lowp vec3 hardLight(lowp vec3 s,lowp vec3 d )
{
    lowp vec3 c;
    c.x = hardLight(s.x,d.x);
    c.y = hardLight(s.y,d.y);
    c.z = hardLight(s.z,d.z);
    return c;
}

lowp float overlay(lowp float s, lowp float d) {
    return (d < 0.5) ? 2.0 * s * d : 1.0 - 2.0 * (1.0 - s) * (1.0 - d);
}

lowp vec3 overlay(lowp vec3 s, lowp vec3 d) {
    lowp vec3 c;
    c.x = overlay(s.x, d.x);
    c.y = overlay(s.y, d.y);
    c.z = overlay(s.z, d.z);
    return c;
}

highp vec3 rgb(highp vec4 color) {
    highp float sign_value = sign(color.a);
    highp float sign_value_squared = sign_value*sign_value;
    highp float invertA =  sign_value_squared / ( color.a + sign_value_squared - 1.0);
    return color.rgb * invertA;
}

lowp float insideBox(highp vec2 v, highp vec2 bottomLeft, highp vec2 topRight) {
    lowp vec2 s = step(bottomLeft, v) - step(topRight, v);
    return s.x * s.y;
}

uniform highp mat4 pin0Transform;
uniform sampler2D pin0;

lowp vec4 getPin0(highp vec4 textureCoordinate) {
    highp vec2 pos = (pin0Transform * textureCoordinate).xy;
    lowp vec4 color = texture2D(pin0, pos);
    lowp float alpha = insideBox(pos, vec2(0.), vec2(1.));

    color.a = min(color.a, alpha);
    color.r = min(color.r, alpha);
    color.g = min(color.g, alpha);
    color.b = min(color.b, alpha);
    return color;
}

lowp vec4 getPin0(highp vec2 uv) {
    return getPin0(vec4(uv, vec2(1.0)));
}

varying highp vec4 screenCoordinate;
uniform highp vec4 resolution;
uniform highp float strength;

highp float rand(highp vec2 n)
{
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

lowp vec4 getPin0NoLimit(highp vec2 uv) {
    highp vec2 pos = (pin0Transform * vec4(uv, vec2(1.0))).xy;
    return texture2D(pin0, clamp(pos, 0., 1.));
}

highp vec4 sharpen(highp vec2 uv, highp vec4 center, highp float sharpness) {
    highp vec2 imgStep = 1. / resolution.xy;

    highp vec3 sum = vec3(0.);
    highp vec3 leftTop = rgb(getPin0NoLimit(uv + vec2(-imgStep.x, -imgStep.y) * 1.5));
    highp vec3 rightTop = rgb(getPin0NoLimit(uv + vec2(imgStep.x, -imgStep.y) * 1.5));
    highp vec3 leftBottom = rgb(getPin0NoLimit(uv + vec2(-imgStep.x, imgStep.y) * 1.5));
    highp vec3 rightBottom = rgb(getPin0NoLimit(uv + vec2(imgStep.x, imgStep.y) * 1.5));
    sum = (leftTop.rgb + rightTop.rgb + leftBottom.rgb + rightBottom.rgb) /4.;

    return clamp(vec4(center.rgb + (center.rgb - sum * center.a) * sharpness, center.a), 0., center.a);
}

void main() {
    highp vec2 uv = screenCoordinate.xy * 0.5 + 0.5;
    lowp vec4 result = sharpen(uv, getPin0(uv), strength);

    gl_FragColor = result;
}
)";
        return CORRECT_FRAGMENT_SHADER(fs);
    }
};

NAMESPACE_END
