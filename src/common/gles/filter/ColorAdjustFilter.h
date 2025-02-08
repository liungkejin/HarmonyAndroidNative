//
// Created by LiangKeJin on 2024/12/28.
//

#pragma once

#include "ZNamespace.h"
#include "common/Object.h"
#include "BaseFilter.h"

NAMESPACE_DEFAULT

/**
 * 支持调节：
 * 亮度、对比度、饱和度、色温、色调、曝光、高光、阴影、胶片颗粒感
 *
 * 参考: https://github.com/yuki-koyama/enhancer/blob/master/shaders/enhancer.fs
 */
class ColorAdjustFilter : public BaseFilter {
public:
    ColorAdjustFilter() : BaseFilter("color_adjust") {
        defAttribute("position", DataType::FLOAT_POINTER)->bind(vertexCoord());
        defAttribute("inputTextureCoordinate", DataType::FLOAT_POINTER)->bind(textureCoord());
        defUniform("inputImageTexture", DataType::SAMPLER_2D);
        defUniform("brightness_strength", DataType::FLOAT)->set(0.0f);
        defUniform("contrast_strength", DataType::FLOAT)->set(0.0f);
        defUniform("saturation_strength", DataType::FLOAT)->set(0.0f);
        defUniform("wb_temperature_strength", DataType::FLOAT)->set(0.0f);
        defUniform("wb_tint_strength", DataType::FLOAT)->set(0.0f);
        defUniform("exposure_strength", DataType::FLOAT)->set(0.0f);
        defUniform("highlights_strength", DataType::FLOAT)->set(1.0f);
        defUniform("highlight_tint_color", DataType::FVEC3)->set(0.0f, 0.0f, 0.0f);
        defUniform("shadows_strength", DataType::FLOAT)->set(0.0f);
        defUniform("shadow_tint_color", DataType::FVEC3)->set(1.0f, 1.0f, 1.0f);
        defUniform("film_grain_strength", DataType::FLOAT)->set(0.0f);
    }

    /**
     * 设置亮度正常范围 [-0.5, 0.5]
     * @param brightness 0.0为原始值
     * @param normalized 是否是归一化的值 [-1, 1]
     */
    void setBrightness(float brightness, bool normalized = true) {
        if (normalized) {
            brightness = brightness * 0.5f;
        }
        uniform("brightness_strength")->set(brightness);
    }

    /**
     * 设置对比度, 正常范围 [-1, 1]
     * @param contrast 0 为原始值
     * @param normalized 是否是归一化的值 [-1, 1]
     */
    void setContrast(float contrast, bool normalized = true) {
        if (normalized) {
            // 转换为 0 - 4
            contrast = contrast * 0.6f;
        }
        uniform("contrast_strength")->set(contrast);
    }

    /**
     * 设置饱和度
     * @param saturation [-1.0, 1.0], 0.0为原始值
     */
    void setSaturation(float saturation) {
        uniform("saturation_strength")->set(saturation);
    }

    /**
     * 设置色温
     * @param brightness 3000 - 8000, 5000.0为原始值
     * @param normalized 是否是归一化的值 [-1, 1]
     */
    void setWhiteBalanceTemperature(float temperature, bool normalized = true) {
        if (normalized) {
            temperature = temperature < 0 ? 5000.0f+temperature*2000.0f : 5000.0f + temperature*3000.0f;
        }
        temperature = temperature < 5000.0f ? 0.0004f * (temperature - 5000.0f) : 0.00006f * (temperature - 5000.0f);
        uniform("wb_temperature_strength")->set(temperature);
    }

    /**
     * 设置色调
     * @param tint -200 ~ 200, 0.0为原始值
     * @param normalized 是否是归一化的值 [-1, 1]
     */
    void setWhiteBalanceTint(float tint, bool normalized = true) {
        if (normalized) {
            tint = tint * 200.0f;
        }
        tint = tint / 100.0f;
        uniform("wb_tint_strength")->set(tint);
    }

    /**
     * 设置曝光
     * @param exposure -2.0 ~ 2.0, 0.0为原始值
     * @param normalized 是否是归一化的值 [-1, 1]
     */
    void setExposure(float exposure, bool normalized = true) {
        if (normalized) {
            exposure = exposure * 2.f;
        }
        uniform("exposure_strength")->set(exposure);
    }

    /**
     * 设置高光 [-0.3, 1.0]
     * @param highlights -1.0 ~ 1.0, 0.0为原始值
     */
    void setHighlights(float highlights, bool normalized = true) {
        if (normalized) {
            highlights = highlights < 0 ? highlights * 0.3f : highlights * 3.0f;
        }
        highlights *= -1.0f;
        uniform("highlights_strength")->set(highlights);
    }

    void setHighlightTintColor(float r, float g, float b) {
        uniform("highlight_tint_color")->set(r, g, b);
    }

    /**
     * 设置阴影
     * @param shadows -1.0 ~ 1.0, 0.0为原始值
     */
    void setShadows(float shadows, bool normalized = true) {
        if (normalized) {
            shadows = shadows * 0.3f;
        }

        shadows *= -1.0f;
        uniform("shadows_strength")->set(shadows);
    }

    void setShadowTintColor(float r, float g, float b) {
        uniform("shadow_tint_color")->set(r, g, b);
    }

    /**
     * 设置胶片颗粒感
     * @param film_grain 0.0 ~ 1.0, 0.0为原始值
     */
    void setFilmGrain(float film_grain, bool normalized = true) {
        if (normalized) {
            film_grain = film_grain * 0.3f;
        }
        uniform("film_grain_strength")->set(film_grain);
    }

    void inputTexture(int id) {
        uniform("inputImageTexture")->set(id);
    }

    void inputTexture(Texture2D &texture) {
        uniform("inputImageTexture")->set((int)texture.id());
    }

protected:
    std::string vertexShader() override {
        std::string vs = R"(
attribute vec4 position;
attribute vec2 inputTextureCoordinate;
varying highp vec2 textureCoordinate;
void main() {
    gl_Position = position;
    textureCoordinate = inputTextureCoordinate;
})";
        return CORRECT_VERTEX_SHADER(vs);
    }

    std::string fragmentShader() override {
        std::string fs = R"(
precision highp float;
varying highp vec2 textureCoordinate;
uniform sampler2D inputImageTexture;

vec4 convert_to_linear(const vec4 color){
    return vec4(pow(color.rgb, vec3(2.2)), color.a);
}

vec4 convert_to_normal(const vec4 color){
    return vec4(pow(color.rgb, vec3(1.0 / 2.2)), color.a);
}

/// 亮度：-1.0 ~ 1.0, 0.0为原始值
uniform lowp float brightness_strength;
vec4 adjust_brightness(vec4 input_color, float strength) {
    vec3 rgb = pow(input_color.rgb, vec3(1.0 / (1.0 + 1.5 * strength)));
    return vec4(rgb, input_color.a);
}

/// 对比度：-1.0 - 1.0, 0.0为原始值
uniform lowp float contrast_strength;
vec4 adjust_contrast(vec4 input_color, float strength) {
    const float pi_4 = 3.14159265358979 * 0.25;
    float contrast_coef = tan((strength + 1.0) * pi_4);
    return vec4(((input_color.rgb - vec3(0.5)) * contrast_coef + vec3(0.5)), input_color.a);
}

/// 饱和度: -1.0 - 1.0, 0.0为原始值
uniform lowp float saturation_strength;
vec4 adjust_saturation(vec4 input_color, float strength) {
    strength = strength + 1.0;
    lowp float luminance = dot(input_color.rgb, vec3(0.2125, 0.7154, 0.0721));
    lowp vec3 greyScaleColor = vec3(luminance);
    return vec4(mix(greyScaleColor, input_color.rgb, strength), input_color.a);
}

/// 色温: 4000k ~ 7000k, 5000为原始值
/// 色调: -200 ~ 200, 0为原始值
uniform lowp float wb_temperature_strength;
uniform lowp float wb_tint_strength;

const lowp vec3 warmFilter = vec3(0.93, 0.54, 0.0);
const mediump mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
const mediump mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);
vec4 adjust_white_balance(vec4 input_color, float temperature, float tint) {
    mediump vec3 yiq = RGBtoYIQ * input_color.rgb;
    yiq.b = clamp(yiq.b + tint * 0.5226 * 0.1, -0.5226, 0.5226);
    lowp vec3 rgb = YIQtoRGB * yiq;

    lowp vec3 processed = vec3(
            (rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))),
            (rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))),
            (rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b)))
    );

    return vec4(clamp(mix(rgb, processed, temperature), 0.0, 1.0), input_color.a);
}

/// 曝光: -2.0 ~ 2.0, 0.0为原始值
uniform lowp float exposure_strength;
vec4 adjust_exposure(vec4 input_color, float strength) {
    return vec4(input_color.rgb * pow(2.0, strength), input_color.a);
}

/// 高光: -1.0 ~ 1.0, 0.0为原始值
/// 阴影: -1.0 ~ 1.0, 0.0为原始值
uniform lowp float highlights_strength;
uniform highp vec3 highlight_tint_color;
uniform lowp float shadows_strength;
uniform highp vec3 shadow_tint_color;
vec4 adjust_highlights_shadows(vec4 input_color, float highlightTintIntensity, vec3 highlightTintColor, float shadowTintIntensity, vec3 shadowTintColor) {
    highp float luminance = dot(input_color.rgb, vec3(0.2125, 0.7154, 0.0721));

    highp vec4 shadowResult = mix(input_color, max(input_color, vec4( mix(shadowTintColor, input_color.rgb, luminance), input_color.a)), shadowTintIntensity);
    highp vec4 highlightResult = mix(input_color, min(shadowResult, vec4( mix(shadowResult.rgb, highlightTintColor, luminance), input_color.a)), highlightTintIntensity);

    return vec4( mix(shadowResult.rgb, highlightResult.rgb, luminance), input_color.a);
}

/// 调节胶片颗粒感: 0.0 ~ 0.5, 0.0为原始值
uniform lowp float film_grain_strength;
vec4 apply_film_grain(vec4 input_color, float grain) {
    vec3 rgb = convert_to_normal(input_color).rgb;
    float noise = (fract(sin(dot(textureCoordinate, vec2(12.9898, 78.233) * 2.0)) * 43758.5453));
    return convert_to_linear(vec4(rgb - noise * grain, input_color.a));
}

void main() {
    lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
    vec4 result_color = convert_to_linear(textureColor);

    if (abs(wb_temperature_strength) > 0.00001 || abs(wb_tint_strength) > 0.00001) {
        result_color = adjust_white_balance(result_color, wb_temperature_strength, wb_tint_strength);
    }

    if (abs(brightness_strength) > 0.00001) {
        result_color = adjust_brightness(result_color, brightness_strength);
    }

    if (abs(contrast_strength) > 0.00001) {
        result_color = adjust_contrast(result_color, contrast_strength);
    }

    if (abs(exposure_strength) > 0.00001) {
        result_color = adjust_exposure(result_color, exposure_strength);
    }

    if (abs(saturation_strength) > 0.00001) {
        result_color = adjust_saturation(result_color, saturation_strength);
    }

    if (abs(highlights_strength) > 0.00001 || abs(shadows_strength) > 0.00001) {
        result_color = adjust_highlights_shadows(result_color, highlights_strength, highlight_tint_color, shadows_strength, shadow_tint_color);
    }

    if (abs(film_grain_strength) > 0.00001) {
        result_color = apply_film_grain(result_color, film_grain_strength);
    }

    gl_FragColor = clamp(convert_to_normal(result_color), 0.0, 1.0);
}
        )";
        return CORRECT_FRAGMENT_SHADER(fs);
    }
};

NAMESPACE_END
