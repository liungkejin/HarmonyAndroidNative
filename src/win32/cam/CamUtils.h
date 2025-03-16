//
// Created by wutacam on 2025/1/13.
//

#pragma once

#include <windows.h>
#include <uuids.h>
#include <common/Common.h>
#include <local/win32/WinUtils.h>

NAMESPACE_DEFAULT
enum CamFmt {
    CAM_FMT_UNSUPPORTED = -1,
    CAM_FMT_CLPL,
    CAM_FMT_YUYV,
    CAM_FMT_IYUV,
    CAM_FMT_YVU9,
    CAM_FMT_Y411,
    CAM_FMT_Y41P,
    CAM_FMT_YUY2,
    CAM_FMT_YVYU,
    CAM_FMT_UYVY,
    CAM_FMT_Y211,
    CAM_FMT_CLJR,
    CAM_FMT_IF09,
    CAM_FMT_CPLA,
    CAM_FMT_MJPG,
    CAM_FMT_TVMJ,
    CAM_FMT_WAKE,
    CAM_FMT_CFCC,
    CAM_FMT_IJPG,
    CAM_FMT_Plum,
    CAM_FMT_DVCS,
    CAM_FMT_DVSD,
    CAM_FMT_MDVF,
    CAM_FMT_RGB1,
    CAM_FMT_RGB4,
    CAM_FMT_RGB8,
    CAM_FMT_RGB565,
    CAM_FMT_RGB555,
    CAM_FMT_RGB24,
    CAM_FMT_RGB32,
    CAM_FMT_ARGB1555,
    CAM_FMT_ARGB4444,
    CAM_FMT_ARGB32,
    CAM_FMT_A2R10G10B10,
    CAM_FMT_A2B10G10R10,
    CAM_FMT_AYUV,
    CAM_FMT_AI44,
    CAM_FMT_IA44,
    CAM_FMT_RGB32_D3D_DX7_RT,
    CAM_FMT_RGB16_D3D_DX7_RT,
    CAM_FMT_ARGB32_D3D_DX7_RT,
    CAM_FMT_ARGB4444_D3D_DX7_RT,
    CAM_FMT_ARGB1555_D3D_DX7_RT,
    CAM_FMT_RGB32_D3D_DX9_RT,
    CAM_FMT_RGB16_D3D_DX9_RT,
    CAM_FMT_ARGB32_D3D_DX9_RT,
    CAM_FMT_ARGB4444_D3D_DX9_RT,
    CAM_FMT_ARGB1555_D3D_DX9_RT,
    CAM_FMT_YV12,
    CAM_FMT_NV12,
    CAM_FMT_IMC1,
    CAM_FMT_IMC2,
    CAM_FMT_IMC3,
    CAM_FMT_IMC4,
    CAM_FMT_S340,
    CAM_FMT_S342,
};

struct CamSize {
    int width;
    int height;

    CamSize(const int width, const int height) : width(width), height(height) {
    }
};

class CamUtils {
public:
    static CamFmt videoTypeToFmt(const GUID &guid) {
        if (guid == MEDIASUBTYPE_None) {
            return CAM_FMT_UNSUPPORTED;
        }
        if (guid == MEDIASUBTYPE_CLPL) {
            return CAM_FMT_CLPL;
        }
        if (guid == MEDIASUBTYPE_YUYV) {
            return CAM_FMT_YUYV;
        }
        if (guid == MEDIASUBTYPE_IYUV) {
            return CAM_FMT_IYUV;
        }
        if (guid == MEDIASUBTYPE_YVU9) {
            return CAM_FMT_YVU9;
        }
        if (guid == MEDIASUBTYPE_Y411) {
            return CAM_FMT_Y411;
        }
        if (guid == MEDIASUBTYPE_Y41P) {
            return CAM_FMT_Y41P;
        }
        if (guid == MEDIASUBTYPE_YUY2) {
            return CAM_FMT_YUY2;
        }
        if (guid == MEDIASUBTYPE_YVYU) {
            return CAM_FMT_YVYU;
        }
        if (guid == MEDIASUBTYPE_UYVY) {
            return CAM_FMT_UYVY;
        }
        if (guid == MEDIASUBTYPE_Y211) {
            return CAM_FMT_Y211;
        }
        if (guid == MEDIASUBTYPE_CLJR) {
            return CAM_FMT_CLJR;
        }
        if (guid == MEDIASUBTYPE_IF09) {
            return CAM_FMT_IF09;
        }
        if (guid == MEDIASUBTYPE_CPLA) {
            return CAM_FMT_CPLA;
        }
        if (guid == MEDIASUBTYPE_MJPG) {
            return CAM_FMT_MJPG;
        }
        if (guid == MEDIASUBTYPE_TVMJ) {
            return CAM_FMT_TVMJ;
        }
        if (guid == MEDIASUBTYPE_WAKE) {
            return CAM_FMT_WAKE;
        }
        if (guid == MEDIASUBTYPE_CFCC) {
            return CAM_FMT_CFCC;
        }
        if (guid == MEDIASUBTYPE_IJPG) {
            return CAM_FMT_IJPG;
        }
        if (guid == MEDIASUBTYPE_Plum) {
            return CAM_FMT_Plum;
        }
        if (guid == MEDIASUBTYPE_DVCS) {
            return CAM_FMT_DVCS;
        }
        if (guid == MEDIASUBTYPE_DVSD) {
            return CAM_FMT_DVSD;
        }
        if (guid == MEDIASUBTYPE_MDVF) {
            return CAM_FMT_MDVF;
        }
        if (guid == MEDIASUBTYPE_RGB1) {
            return CAM_FMT_RGB1;
        }
        if (guid == MEDIASUBTYPE_RGB4) {
            return CAM_FMT_RGB4;
        }
        if (guid == MEDIASUBTYPE_RGB8) {
            return CAM_FMT_RGB8;
        }
        if (guid == MEDIASUBTYPE_RGB565) {
            return CAM_FMT_RGB565;
        }
        if (guid == MEDIASUBTYPE_RGB555) {
            return CAM_FMT_RGB555;
        }
        if (guid == MEDIASUBTYPE_RGB24) {
            return CAM_FMT_RGB24;
        }
        if (guid == MEDIASUBTYPE_RGB32) {
            return CAM_FMT_RGB32;
        }
        if (guid == MEDIASUBTYPE_ARGB1555) {
            return CAM_FMT_ARGB1555;
        }
        if (guid == MEDIASUBTYPE_ARGB4444) {
            return CAM_FMT_ARGB4444;
        }
        if (guid == MEDIASUBTYPE_ARGB32) {
            return CAM_FMT_ARGB32;
        }
        if (guid == MEDIASUBTYPE_A2R10G10B10) {
            return CAM_FMT_A2R10G10B10;
        }
        if (guid == MEDIASUBTYPE_A2B10G10R10) {
            return CAM_FMT_A2B10G10R10;
        }
        if (guid == MEDIASUBTYPE_AYUV) {
            return CAM_FMT_AYUV;
        }
        if (guid == MEDIASUBTYPE_AI44) {
            return CAM_FMT_AI44;
        }
        if (guid == MEDIASUBTYPE_IA44) {
            return CAM_FMT_IA44;
        }
        if (guid == MEDIASUBTYPE_RGB32_D3D_DX7_RT) {
            return CAM_FMT_RGB32_D3D_DX7_RT;
        }
        if (guid == MEDIASUBTYPE_RGB16_D3D_DX7_RT) {
            return CAM_FMT_RGB16_D3D_DX7_RT;
        }
        if (guid == MEDIASUBTYPE_ARGB32_D3D_DX7_RT) {
            return CAM_FMT_ARGB32_D3D_DX7_RT;
        }
        if (guid == MEDIASUBTYPE_ARGB4444_D3D_DX7_RT) {
            return CAM_FMT_ARGB4444_D3D_DX7_RT;
        }
        if (guid == MEDIASUBTYPE_ARGB1555_D3D_DX7_RT) {
            return CAM_FMT_ARGB1555_D3D_DX7_RT;
        }
        if (guid == MEDIASUBTYPE_RGB32_D3D_DX9_RT) {
            return CAM_FMT_RGB32_D3D_DX9_RT;
        }
        if (guid == MEDIASUBTYPE_RGB16_D3D_DX9_RT) {
            return CAM_FMT_RGB16_D3D_DX9_RT;
        }
        if (guid == MEDIASUBTYPE_ARGB32_D3D_DX9_RT) {
            return CAM_FMT_ARGB32_D3D_DX9_RT;
        }
        if (guid == MEDIASUBTYPE_ARGB4444_D3D_DX9_RT) {
            return CAM_FMT_ARGB4444_D3D_DX9_RT;
        }
        if (guid == MEDIASUBTYPE_ARGB1555_D3D_DX9_RT) {
            return CAM_FMT_ARGB1555_D3D_DX9_RT;
        }
        if (guid == MEDIASUBTYPE_YV12) {
            return CAM_FMT_YV12;
        }
        if (guid == MEDIASUBTYPE_NV12) {
            return CAM_FMT_NV12;
        }
        if (guid == MEDIASUBTYPE_IMC1) {
            return CAM_FMT_IMC1;
        }
        if (guid == MEDIASUBTYPE_IMC2) {
            return CAM_FMT_IMC2;
        }
        if (guid == MEDIASUBTYPE_IMC3) {
            return CAM_FMT_IMC3;
        }
        if (guid == MEDIASUBTYPE_IMC4) {
            return CAM_FMT_IMC4;
        }
        if (guid == MEDIASUBTYPE_S340) {
            return CAM_FMT_S340;
        }
        if (guid == MEDIASUBTYPE_S342) {
            return CAM_FMT_S342;
        }
        return CAM_FMT_UNSUPPORTED;
    }

    static std::string camFmtString(const int fmt) {
        switch (fmt) {
            case CAM_FMT_UNSUPPORTED:
                return "CAM_FMT_UNSUPPORTED";
            case CAM_FMT_CLPL:
                return "CAM_FMT_CLPL";
            case CAM_FMT_YUYV:
                return "CAM_FMT_YUYV";
            case CAM_FMT_IYUV:
                return "CAM_FMT_IYUV";
            case CAM_FMT_YVU9:
                return "CAM_FMT_YVU9";
            case CAM_FMT_Y411:
                return "CAM_FMT_Y411";
            case CAM_FMT_Y41P:
                return "CAM_FMT_Y41P";
            case CAM_FMT_YUY2:
                return "CAM_FMT_YUY2";
            case CAM_FMT_YVYU:
                return "CAM_FMT_YVYU";
            case CAM_FMT_UYVY:
                return "CAM_FMT_UYVY";
            case CAM_FMT_Y211:
                return "CAM_FMT_Y211";
            case CAM_FMT_CLJR:
                return "CAM_FMT_CLJR";
            case CAM_FMT_IF09:
                return "CAM_FMT_IF09";
            case CAM_FMT_CPLA:
                return "CAM_FMT_CPLA";
            case CAM_FMT_MJPG:
                return "CAM_FMT_MJPG";
            case CAM_FMT_TVMJ:
                return "CAM_FMT_TVMJ";
            case CAM_FMT_WAKE:
                return "CAM_FMT_WAKE";
            case CAM_FMT_CFCC:
                return "CAM_FMT_CFCC";
            case CAM_FMT_IJPG:
                return "CAM_FMT_IJPG";
            case CAM_FMT_Plum:
                return "CAM_FMT_Plum";
            case CAM_FMT_DVCS:
                return "CAM_FMT_DVCS";
            case CAM_FMT_DVSD:
                return "CAM_FMT_DVSD";
            case CAM_FMT_MDVF:
                return "CAM_FMT_MDVF";
            case CAM_FMT_RGB1:
                return "CAM_FMT_RGB1";
            case CAM_FMT_RGB4:
                return "CAM_FMT_RGB4";
            case CAM_FMT_RGB8:
                return "CAM_FMT_RGB8";
            case CAM_FMT_RGB565:
                return "CAM_FMT_RGB565";
            case CAM_FMT_RGB555:
                return "CAM_FMT_RGB555";
            case CAM_FMT_RGB24:
                return "CAM_FMT_RGB24";
            case CAM_FMT_RGB32:
                return "CAM_FMT_RGB32";
            case CAM_FMT_ARGB1555:
                return "CAM_FMT_ARGB1555";
            case CAM_FMT_ARGB4444:
                return "CAM_FMT_ARGB4444";
            case CAM_FMT_ARGB32:
                return "CAM_FMT_ARGB32";
            case CAM_FMT_A2R10G10B10:
                return "CAM_FMT_A2R10G10B10";
            case CAM_FMT_A2B10G10R10:
                return "CAM_FMT_A2B10G10R10";
            case CAM_FMT_AYUV:
                return "CAM_FMT_AYUV";
            case CAM_FMT_AI44:
                return "CAM_FMT_AI44";
            case CAM_FMT_IA44:
                return "CAM_FMT_IA44";
            case CAM_FMT_RGB32_D3D_DX7_RT:
                return "CAM_FMT_RGB32_D3D_DX7_RT";
            case CAM_FMT_RGB16_D3D_DX7_RT:
                return "CAM_FMT_RGB16_D3D_DX7_RT";
            case CAM_FMT_ARGB32_D3D_DX7_RT:
                return "CAM_FMT_ARGB32_D3D_DX7_RT";
            case CAM_FMT_ARGB4444_D3D_DX7_RT:
                return "CAM_FMT_ARGB4444_D3D_DX7_RT";
            case CAM_FMT_ARGB1555_D3D_DX7_RT:
                return "CAM_FMT_ARGB1555_D3D_DX7_RT";
            case CAM_FMT_RGB32_D3D_DX9_RT:
                return "CAM_FMT_RGB32_D3D_DX9_RT";
            case CAM_FMT_RGB16_D3D_DX9_RT:
                return "CAM_FMT_RGB16_D3D_DX9_RT";
            case CAM_FMT_ARGB32_D3D_DX9_RT:
                return "CAM_FMT_ARGB32_D3D_DX9_RT";
            case CAM_FMT_ARGB4444_D3D_DX9_RT:
                return "CAM_FMT_ARGB4444_D3D_DX9_RT";
            case CAM_FMT_ARGB1555_D3D_DX9_RT:
                return "CAM_FMT_ARGB1555_D3D_DX9_RT";
            case CAM_FMT_YV12:
                return "CAM_FMT_YV12";
            case CAM_FMT_NV12:
                return "CAM_FMT_NV12";
            case CAM_FMT_IMC1:
                return "CAM_FMT_IMC1";
            case CAM_FMT_IMC2:
                return "CAM_FMT_IMC2";
            case CAM_FMT_IMC3:
                return "CAM_FMT_IMC3";
            case CAM_FMT_IMC4:
                return "CAM_FMT_IMC4";
            case CAM_FMT_S340:
                return "CAM_FMT_S340";
            case CAM_FMT_S342:
                return "CAM_FMT_S342";

            default:
                return "CAM_FMT_UNKNOWN_" + std::to_string(fmt);
        }
    }
};

NAMESPACE_END
