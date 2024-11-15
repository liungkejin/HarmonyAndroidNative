//
// Created on 2024/8/15.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "AudioReader.h"

NAMESPACE_DEFAULT

AudioReader *AudioReaderBuilder::simpleCreate() {
    AudioReaderBuilder builder;
    builder.setSamplingRate(48000);
    builder.setChannelCount(2);
    builder.setSampleFormat(AUDIOSTREAM_SAMPLE_S16LE);
    builder.setEncodingType(AUDIOSTREAM_ENCODING_TYPE_RAW);
    builder.setCapturerSource(AUDIOSTREAM_SOURCE_TYPE_MIC);

    return builder.createReader();
}


/**
 * This function pointer will point to the callback function that
 * is used to read audio data.
 */
int32_t _OnReadData(OH_AudioCapturer *capturer, void *userData, void *buffer, int32_t length) { // NOLINT(*-reserved-identifier)
    auto reader = (AudioReader *)userData;
    if (reader) {
        reader->onReadData(buffer, length);
    } else {
        _ERROR("_OnReadData userData == nullptr");
    }
    return 0;
}

/**
 * This function pointer will point to the callback function that
 * is used to handle audio capturer stream events.
 *
 * @since 10
 */
int32_t _OnStreamEvent(OH_AudioCapturer *capturer, void *userData, OH_AudioStream_Event event) { // NOLINT(*-reserved-identifier)
    auto reader = (AudioReader *)userData;
    if (reader) {
        reader->onStreamEvent(event);
    } else {
        _ERROR("_OnStreamEvent userData == nullptr");
    }
    return 0;
}

/**
 * This function pointer will point to the callback function that
 * is used to handle audio interrupt events.
 *
 * @since 10
 */
int32_t _OnInterruptEvent(OH_AudioCapturer *capturer, void *userData, OH_AudioInterrupt_ForceType type, // NOLINT(*-reserved-identifier)
                          OH_AudioInterrupt_Hint hint) {
    auto reader = (AudioReader *)userData;
    if (reader) {
        reader->onInterruptEvent(type, hint);
    } else {
        _ERROR("_OnInterruptEvent userData == nullptr");
    }
    return 0;
}

/**
 * This function pointer will point to the callback function that
 * is used to handle audio error result.
 *
 * @since 10
 */
int32_t _OnError(OH_AudioCapturer *capturer, void *userData, OH_AudioStream_Result error) { // NOLINT(*-reserved-identifier)
    auto reader = (AudioReader *)userData;
    if (reader) {
        reader->onError(error);
    } else {
        _ERROR("_OnError userData == nullptr");
    }
    return 0;
}

AudioReader *AudioReaderBuilder::createReader() {
    auto reader = new AudioReader();

    OH_AudioCapturer_Callbacks callbacks;
    callbacks.OH_AudioCapturer_OnReadData = _OnReadData;
    callbacks.OH_AudioCapturer_OnStreamEvent = _OnStreamEvent;
    callbacks.OH_AudioCapturer_OnInterruptEvent = _OnInterruptEvent;
    callbacks.OH_AudioCapturer_OnError = _OnError;
    this->setCapturerCallback(callbacks, reader);

    AudioCapturer *capturer = this->createCapturer();
    _ERROR_RETURN_IF(!capturer, nullptr, "create failed!");

    reader->setCapturer(capturer);
    return reader;
}

NAMESPACE_END