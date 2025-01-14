//
// Created by wutacam on 2025/1/13.
//

#pragma once

#include <windows.h>
#include <common/Common.h>

NAMESPACE_DEFAULT
class DSUtils {
public:
    static std::string mediaTypeString(const GUID &guid) {
        if (guid == MEDIATYPE_Video) {
            return "MEDIATYPE_Video";
        }
        if (guid == MEDIATYPE_Audio) {
            return "MEDIATYPE_Audio";
        }
        if (guid == MEDIATYPE_Text) {
            return "MEDIATYPE_Text";
        }
        if (guid == MEDIATYPE_Midi) {
            return "MEDIATYPE_Midi";
        }
        if (guid == MEDIATYPE_Stream) {
            return "MEDIATYPE_Stream";
        }
        if (guid == MEDIATYPE_Interleaved) {
            return "Interleaved";
        }
        if (guid == MEDIATYPE_File) {
            return "MEDIATYPE_File";
        }
        if (guid == MEDIATYPE_ScriptCommand) {
            return "MEDIATYPE_ScriptCommand";
        }
        if (guid == MEDIATYPE_AUXLine21Data) {
            return "MEDIATYPE_AuxLine21Data";
        }
        if (guid == MEDIATYPE_VBI) {
            return "MEDIATYPE_VBI";
        }
        if (guid == MEDIATYPE_Timecode) {
            return "MEDIATYPE_Timecode";
        }
        if (guid == MEDIATYPE_LMRT) {
            return "MEDIATYPE_LMRT";
        }
        if (guid == MEDIATYPE_URL_STREAM) {
            return "MEDIATYPE_URLStream";
        }
        return "MEDIATYPE_Unknown";
    }

    static std::string videoTypeString(const GUID &guid) {
        if (guid == MEDIASUBTYPE_None) {
            return "None";
        }
        if (guid == MEDIASUBTYPE_CLPL) {
            return "CLPL";
        }
        if (guid == MEDIASUBTYPE_YUYV) {
            return "YUYV";
        }
        if (guid == MEDIASUBTYPE_IYUV) {
            return "IYUV";
        }
        if (guid == MEDIASUBTYPE_YVU9) {
            return "YVU9";
        }
        if (guid == MEDIASUBTYPE_Y411) {
            return "Y411";
        }
        if (guid == MEDIASUBTYPE_Y41P) {
            return "Y41P";
        }
        if (guid == MEDIASUBTYPE_YUY2) {
            return "YUY2";
        }
        if (guid == MEDIASUBTYPE_YVYU) {
            return "YVYU";
        }
        if (guid == MEDIASUBTYPE_UYVY) {
            return "UYVY";
        }
        if (guid == MEDIASUBTYPE_Y211) {
            return "Y211";
        }
        if (guid == MEDIASUBTYPE_CLJR) {
            return "CLJR";
        }
        if (guid == MEDIASUBTYPE_IF09) {
            return "IF09";
        }
        if (guid == MEDIASUBTYPE_CPLA) {
            return "CPLA";
        }
        if (guid == MEDIASUBTYPE_MJPG) {
            return "MJPG";
        }
        if (guid == MEDIASUBTYPE_TVMJ) {
            return "TVMJ";
        }
        if (guid == MEDIASUBTYPE_WAKE) {
            return "WAKE";
        }
        if (guid == MEDIASUBTYPE_CFCC) {
            return "CFCC";
        }
        if (guid == MEDIASUBTYPE_IJPG) {
            return "IJPG";
        }
        if (guid == MEDIASUBTYPE_Plum) {
            return "Plum";
        }
        if (guid == MEDIASUBTYPE_DVCS) {
            return "DVCS";
        }
        if (guid == MEDIASUBTYPE_DVSD) {
            return "DVSD";
        }
        if (guid == MEDIASUBTYPE_MDVF) {
            return "MDVF";
        }
        if (guid == MEDIASUBTYPE_RGB1) {
            return "RGB1";
        }
        if (guid == MEDIASUBTYPE_RGB4) {
            return "RGB4";
        }
        if (guid == MEDIASUBTYPE_RGB8) {
            return "RGB8";
        }
        if (guid == MEDIASUBTYPE_RGB565) {
            return "RGB565";
        }
        if (guid == MEDIASUBTYPE_RGB555) {
            return "RGB555";
        }
        if (guid == MEDIASUBTYPE_RGB24) {
            return "RGB24";
        }
        if (guid == MEDIASUBTYPE_RGB32) {
            return "RGB32";
        }
        if (guid == MEDIASUBTYPE_ARGB1555) {
            return "ARGB1555";
        }
        if (guid == MEDIASUBTYPE_ARGB4444) {
            return "ARGB4444";
        }
        if (guid == MEDIASUBTYPE_ARGB32) {
            return "ARGB32";
        }
        if (guid == MEDIASUBTYPE_A2R10G10B10) {
            return "A2R10G10B10";
        }
        if (guid == MEDIASUBTYPE_A2B10G10R10) {
            return "A2B10G10R10";
        }
        if (guid == MEDIASUBTYPE_AYUV) {
            return "AYUV";
        }
        if (guid == MEDIASUBTYPE_AI44) {
            return "AI44";
        }
        if (guid == MEDIASUBTYPE_IA44) {
            return "IA44";
        }
        if (guid == MEDIASUBTYPE_RGB32_D3D_DX7_RT) {
            return "RGB32_D3D_DX7_RT";
        }
        if (guid == MEDIASUBTYPE_RGB16_D3D_DX7_RT) {
            return "RGB16_D3D_DX7_RT";
        }
        if (guid == MEDIASUBTYPE_ARGB32_D3D_DX7_RT) {
            return "ARGB32_D3D_DX7_RT";
        }
        if (guid == MEDIASUBTYPE_ARGB4444_D3D_DX7_RT) {
            return "ARGB4444_D3D_DX7_RT";
        }
        if (guid == MEDIASUBTYPE_ARGB1555_D3D_DX7_RT) {
            return "ARGB1555_D3D_DX7_RT";
        }
        if (guid == MEDIASUBTYPE_RGB32_D3D_DX9_RT) {
            return "RGB32_D3D_DX9_RT";
        }
        if (guid == MEDIASUBTYPE_RGB16_D3D_DX9_RT) {
            return "RGB16_D3D_DX9_RT";
        }
        if (guid == MEDIASUBTYPE_ARGB32_D3D_DX9_RT) {
            return "ARGB32_D3D_DX9_RT";
        }
        if (guid == MEDIASUBTYPE_ARGB4444_D3D_DX9_RT) {
            return "ARGB4444_D3D_DX9_RT";
        }
        if (guid == MEDIASUBTYPE_ARGB1555_D3D_DX9_RT) {
            return "ARGB1555_D3D_DX9_RT";
        }
        if (guid == MEDIASUBTYPE_YV12) {
            return "YV12";
        }
        if (guid == MEDIASUBTYPE_NV12) {
            return "NV12";
        }
        if (guid == MEDIASUBTYPE_IMC1) {
            return "IMC1";
        }
        if (guid == MEDIASUBTYPE_IMC2) {
            return "IMC2";
        }
        if (guid == MEDIASUBTYPE_IMC3) {
            return "IMC3";
        }
        if (guid == MEDIASUBTYPE_IMC4) {
            return "IMC4";
        }
        if (guid == MEDIASUBTYPE_S340) {
            return "S340";
        }
        if (guid == MEDIASUBTYPE_S342) {
            return "S342";
        }
        return "UNKNOWN_" + WinUtils::guidToString(guid);
    }
};

NAMESPACE_END
