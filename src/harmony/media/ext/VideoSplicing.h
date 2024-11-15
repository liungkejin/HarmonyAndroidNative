//
// Created on 2024/8/27.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once

#include "common/Common.h"
#include "harmony/media/Demuxer.h"
#include "harmony/media/Muxer.h"

NAMESPACE_DEFAULT

enum VSplicingError { OK = 0, ERR_SRC_FILE_INVALID = -1000, ERR_TRACK_MISMATCH, ERR_OUTPUT_FILE_INVALID };

/**
 * 视频拼接工具
 * 以音频为基准，如果视频比音频长，那则需要缩放视频的时间戳，强制对齐音频
 * 如果音频比视频长，那则不需要对齐，因为视频修改第一帧时间戳不会有什么影响
 */
class VideoSplicing {
public:
    static int splicing(std::string fileLines, std::string output) {
        std::vector<std::string> files;
        // split the lines
        std::istringstream iss(fileLines);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.empty()) {
                continue;
            }
            files.push_back(line);
        }

        return splicing(files, output);
    }

    static int splicing(std::vector<std::string> &files, std::string output) {
        int64_t startMs = TimeUtils::nowMs();

        Muxer muxer;
        if (!muxer.create(output)) {
            _ERROR("create muxer failed: %s", output);
            return ERR_OUTPUT_FILE_INVALID;
        }

        int64_t basePtUs = 0;
        int errRet = OK;
        bool anySuccess = false;
        for (auto &file : files) {
            int ret = splicing(muxer, file, basePtUs);
            if (ret != OK) {
                errRet = ret;
                _WARN("splicing failed: %d, file: %s", ret, file);
            } else {
                anySuccess = true;
            }
        }
        if (muxer.isStarted()) {
            muxer.stop();
        }
        int64_t costMs = TimeUtils::nowMs() - startMs;
        _INFO("splicing cost time: %lld ms", costMs);
        if (!anySuccess) {
            _ERROR("all video file splice failed! final error: %d", errRet);
            return errRet;
        }
        return OK;
    }

private:
    static int splicing(Muxer &muxer, std::string &file, int64_t &basePtUs) {
        Demuxer *demuxer = Demuxer::open(file);
        _ERROR_RETURN_IF(!demuxer, ERR_SRC_FILE_INVALID, "file invalid: %s", file);
        _INFO("splicing basePtUs(%lld): %s", basePtUs, demuxer->toString());

        int ret = splicing(muxer, demuxer, basePtUs);
        _ERROR_IF(ret, "splicing failed: %d, file: %s", ret, file);
        DELETE_TO_NULL(demuxer);

        return ret;
    }

    static int splicing(Muxer &muxer, Demuxer *demuxer, int64_t &basePtUs) {
        AVSource &source = demuxer->source();
        if (!muxer.isStarted()) {
            if (source.hasVideoTrack()) {
                int trackId;
                muxer.addTrack(source.videoTrackFormat(), trackId);
            }
            if (source.hasAudioTrack()) {
                int trackId;
                muxer.addTrack(source.audioTrackFormat(), trackId);
            }

            muxer.start();
        }
        if (muxer.hasAudioTrack()) {
            if (!source.hasAudioTrack()) {
                _ERROR("source track mismatch, no audio track!");
                return ERR_TRACK_MISMATCH;
            }
            demuxer->selectAudioTrack();
        }
        // 必须

        // 以音频为基准，如果视频轨道比音频轨道长，则需要缩放视频轨道
        double videoScale = 1;
#if 1
        if (muxer.hasVideoTrack() && muxer.hasAudioTrack()) {
            // 这个 unselect 是必须的，不然读取会报 NO_PERMIT 错误
            demuxer->unselectTrack(source.videoTrackIndex());
            demuxer->seekToTime(source.durationUs() / 1000);
            AVBuffer *buffer = demuxer->readAudioSample();
            if (buffer) {
                double audioDurationUs = buffer->ptUs();
                if (audioDurationUs + 30000 < source.durationUs()) {
                    videoScale = audioDurationUs / source.durationUs();
                    _INFO("video track(%lld) is longer than audio track(%lld) scale: %f", source.durationUs(),
                          audioDurationUs, videoScale);
                }
            }
            demuxer->seekToTime(0);
        }
#endif

        if (muxer.hasVideoTrack()) {
            if (!source.hasVideoTrack()) {
                _ERROR("source track mismatch, no video track!");
                return ERR_TRACK_MISMATCH;
            }
            demuxer->selectVideoTrack();
        }

        demuxer->retrieve([&](AVBuffer *vbuffer, AVBuffer *abuffer) {
            if (muxer.hasVideoTrack() && vbuffer) {
                int64_t ptsUs = basePtUs + (int64_t)(vbuffer->ptUs() * videoScale);
                uint32_t flags = vbuffer->flags();
                if (flags & AVCODEC_BUFFER_FLAGS_EOS) {
                    flags ^= AVCODEC_BUFFER_FLAGS_EOS;
                }
                //                _INFO("video read buffer: %s", vbuffer->toString());
                if (vbuffer->size() > 0) {
                    vbuffer->setAttr(ptsUs, vbuffer->size(), vbuffer->offset(), flags);
                    //                    _INFO("video write buffer: %s", vbuffer->toString());
                    muxer.writeVideoPacket(*vbuffer);
                }
            }
            if (muxer.hasAudioTrack() && abuffer) {
                int64_t ptsUs = basePtUs + abuffer->ptUs();
                uint32_t flags = abuffer->flags();
                if (flags & AVCODEC_BUFFER_FLAGS_EOS) {
                    flags ^= AVCODEC_BUFFER_FLAGS_EOS;
                }
                //                _INFO("audio read buffer: %s", abuffer->toString());
                if (abuffer->size() > 0) {
                    abuffer->setAttr(ptsUs, abuffer->size(), abuffer->offset(), flags);
                    //                    _INFO("audio write buffer: %s", abuffer->toString());
                    muxer.writeAudioPacket(*abuffer);
                }
            }
        });

        // +1000 为了防止意外情况
        basePtUs += source.durationUs() + 1000;
        return OK;
    }
};

NAMESPACE_END
