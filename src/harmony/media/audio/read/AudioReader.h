//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "ZNamespace.h"
#include "common/utils/Base.h"
#include "AudioCapturer.h"
#include "AudioStreamBuilder.h"

NAMESPACE_DEFAULT

class AudioReaderBuilder;

class AudioReader {
    friend class AudioReaderBuilder;
public:
    explicit AudioReader(AudioCapturer *capturer) : m_capturer(capturer) {}
    
    ~AudioReader() {
        DELETE_TO_NULL(m_capturer);
        setOutputListener(nullptr);
    }
    
public:
    OH_AudioStream_Result start() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        if (m_capturer->curState() == AUDIOSTREAM_STATE_RUNNING) {
            _WARN("AudioCapturer already started");
            return OH_AudioStream_Result::AUDIOSTREAM_SUCCESS;
        }
        OH_AudioStream_Result result = m_capturer->start();
        _INFO("start result: %s", ASUtils::resultString(result));
        return result;
    }
    
    OH_AudioStream_Result pause() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        OH_AudioStream_Result result = m_capturer->pause();
        _INFO("pause result: %s", ASUtils::resultString(result));
        return result;
    }
    
    OH_AudioStream_Result stop() {
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        OH_AudioStream_Result result = m_capturer->stop();
        _INFO("stop result: %s", ASUtils::resultString(result));
        return result;
    }
    
    const AudioCapturer &capturer() const { 
        _FATAL_IF(!m_capturer, "m_capturer == nullptr");
        return *m_capturer;
    }
    
    void setOutputListener(const std::function<void(AudioReader &reader, void *buffer, int length)>& listener) {
        m_output_listener = listener;
    }
    
public:
    void onReadData(void *buffer, int length) {
//        _INFO("onReadData: %p, length: %d", buffer, length);
        if (m_output_listener) {
            m_output_listener(*this, buffer, length);
        }
    }
    
    void onStreamEvent(OH_AudioStream_Event event) { // NOLINT(*-convert-member-functions-to-static)
        _INFO("onStreamEvent: %d", event);
    }
    
    void onInterruptEvent(OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint) { // NOLINT(*-convert-member-functions-to-static)
        _INFO("onInterruptEvent: type: %d, hint: %d", type, hint);
    }
    
    void onError(OH_AudioStream_Result result) { // NOLINT(*-convert-member-functions-to-static)
        _INFO("onErrorEvent: %d", ASUtils::resultString(result));
    }
    
private:
    AudioReader() {}
    
    void setCapturer(AudioCapturer *capturer) {
        m_capturer = capturer;
    }
    
private:
    bool m_start_flag = false;
    AudioCapturer *m_capturer = nullptr;
    std::function<void(AudioReader &reader, void *buffer, int length)> m_output_listener;
};

class AudioReaderBuilder : public AudioStreamBuilder {
public:
    static AudioReader * simpleCreate();
    
public:
    AudioReaderBuilder() : AudioStreamBuilder(AUDIOSTREAM_TYPE_CAPTURER) {}
    
    AudioReaderBuilder(const AudioReaderBuilder &other) : AudioStreamBuilder(other) {}
    
public:
    AudioReader *createReader();
};

NAMESPACE_END
