//
// Created by LiangKeJin on 2024/12/10.
//

#include "ImageReader.h"

#include <utility>

NAMESPACE_DEFAULT

static void gOnImageAvailable(void *context, AImageReader *reader) {
    auto ir = ImageReader(reader, false);
    auto callback = (ImageCallback *) context;
    if (callback) {
        (*callback)(ir);
    } else {
        _WARN("ImageReader callback is null");
    }
}

static AImageReader_ImageListener gListener = {
    .context = nullptr,
    .onImageAvailable = gOnImageAvailable
};

void ImageReader::setImageAvailableCallback(ImageCallback callback) {
    this->m_callback = std::move(callback);
    gListener.context = &this->m_callback;
    auto ret = AImageReader_setImageListener(this->m_reader, &gListener);
    _FATAL_IF(ret, "AImageReader_setImageListener failed: %d", ret);
}

NAMESPACE_END