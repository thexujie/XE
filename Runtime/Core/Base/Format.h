#pragma once

#include "Types.h"

namespace XE
{
    FORCEINLINE constexpr unsigned int MakeFormatValue_RGB(unsigned short index)
    {
        return (static_cast<unsigned int>(index)) | 0x7eee0000;
    }

    enum class EFormat
    {
        None = 0,
        Y8  = MakeFourCC("Y8  "),
        RGB = MakeFourCC("24RG"),
        BGR = MakeFourCC("24BG"),
        ARGB = MakeFourCC("ARGB"),
        ABGR = MakeFourCC("ABGR"),
        BGRA = MakeFourCC("BGRA"),
        RGBA = MakeFourCC("RGBA"),
        XRGB = MakeFourCC("XRGB"),
        XBGR = MakeFourCC("XBGR"),
        BGRX = MakeFourCC("BGRX"),
        RGBX = MakeFourCC("RGBX"),

        I420 = MakeFourCC("I420"),
        I422 = MakeFourCC("I422"),
        I444 = MakeFourCC("I444"),
        NV12 = MakeFourCC("NV12"),
        NV21 = MakeFourCC("NV21"),

        YU12 = MakeFourCC("YU12"),
        YUV2 = MakeFourCC("YUV2"),
        YUY2 = MakeFourCC("YUY2"),
        YUYV = MakeFourCC("YUYV"),
        YV12 = MakeFourCC("YV12"),
        YVYU = MakeFourCC("YVYU"),
        UYUV = MakeFourCC("UYUV"),
        UYVY = MakeFourCC("UYVY"),
        VYVU = MakeFourCC("VYVU"),
        VUVY = MakeFourCC("VUVY"),

        CMYK = MakeFourCC("CMYK"),
        YCCK = MakeFourCC("YCCK"),

        PNG = MakeFourCC("PNG "),
        JPEG = MakeFourCC("JPEG"),
        MJPG = MakeFourCC("MJPG"),
        H264 = MakeFourCC("H264"),
        H265 = MakeFourCC("H265"),
        HEVC = MakeFourCC("HEVC"),
        X264 = MakeFourCC("X264"),
        Y420 = MakeFourCC("Y420"),

        P010 = MakeFourCC("P010"),

        // PACKAGED
        Half1 = MakeFourCC("00H1"),
        Half2 = MakeFourCC("00H2"),
        Half3 = MakeFourCC("00H3"),
        Half4 = MakeFourCC("00H4"),
        Float1 = MakeFourCC("00F1"),
        Float2 = MakeFourCC("00F2"),
        Float3 = MakeFourCC("00F3"),
        Float4 = MakeFourCC("00F4"),
        Int1 = MakeFourCC("00I1"),
        Int2 = MakeFourCC("00I2"),
        Int3 = MakeFourCC("00I3"),
        Int4 = MakeFourCC("00I4"),
        UInt1 = MakeFourCC("0UI1"),
        UInt2 = MakeFourCC("0UI2"),
        UInt3 = MakeFourCC("0UI3"),
        UInt4 = MakeFourCC("0UI4"),

        RAW = MakeFourCC("RAW "),
        FOURCC_I420 = MakeFourCC("I420"),
        FOURCC_I422 = MakeFourCC("I422"),
        FOURCC_I444 = MakeFourCC("I444"),
        FOURCC_I400 = MakeFourCC("I400"),
        FOURCC_NV21 = MakeFourCC("NV21"),
        FOURCC_NV12 = MakeFourCC("NV12"),
        FOURCC_YUY2 = MakeFourCC("YUY2"),
        FOURCC_UYVY = MakeFourCC("UYVY"),
        FOURCC_M420 = MakeFourCC("M420"),
        FOURCC_ARGB = MakeFourCC("ARGB"),
        FOURCC_ABGR = MakeFourCC("ABGR"),
        FOURCC_BGRA = MakeFourCC("BGRA"),
        FOURCC_RGBA = MakeFourCC("RGBA"),
        FOURCC_AR30 = MakeFourCC("AR30"), // 10 BIT PER CHANNEL. 2101010.
        FOURCC_AB30 = MakeFourCC("AB30"), // ABGR VERSION OF 10 BIT
        FOURCC_RGBP = MakeFourCC("RGBP"), // RGB565 LE.
        FOURCC_RGB0 = MakeFourCC("RGB0"), // ARGB1555 LE.
        FOURCC_R444 = MakeFourCC("R444"), // ARGB4444 LE.
        FOURCC_MJPG = MakeFourCC("MJPG"),
        FOURCC_YV12 = MakeFourCC("YV12"),
        FOURCC_YV16 = MakeFourCC("YV16"),
        FOURCC_YV24 = MakeFourCC("YV24"),
        FOURCC_YU12 = MakeFourCC("YU12"),
        FOURCC_YU16 = MakeFourCC("YU16"), // ALIAS FOR I422.
        FOURCC_YU24 = MakeFourCC("YU24"), // ALIAS FOR I444.
        FOURCC_J420 = MakeFourCC("J420"),
        FOURCC_J400 = MakeFourCC("J400"),
        FOURCC_H420 = MakeFourCC("H420"),
        FOURCC_H422 = MakeFourCC("H422"),
        FOURCC_IYUV = MakeFourCC("IYUV"), // ALIAS FOR I420.
        FOURCC_YUYV = MakeFourCC("YUYV"), // ALIAS FOR YUY2.
        FOURCC_YUVS = MakeFourCC("YUVS"), // ALIAS FOR YUY2 ON MAC.
        FOURCC_HDYC = MakeFourCC("HDYV"), // ALIAS FOR UYVY.
        FOURCC_JPEG = MakeFourCC("JPEG"), // ALIAS FOR JPEG.
        FOURCC_DMB1 = MakeFourCC("DMB1"), // ALIAS FOR MJPG ON MAC.
        FOURCC_BA81 = MakeFourCC("BA81"), // ALIAS FOR BGGR.
        FOURCC_BGB3 = MakeFourCC("BGB3"), // ALIAS FOR RAW.
        FOURCC_BGR3 = MakeFourCC("BGR3"), // ALIAS FOR 24BG.
        FOURCC_CM32 = MakeFourCC(0, 0, 0, 32), // ALIAS FOR BGRA KCMPIXELFORMAT_32ARGB
        FOURCC_CM24 = MakeFourCC(0, 0, 0, 24), // ALIAS FOR RAW KCMPIXELFORMAT_24RGB
        FOURCC_L555 = MakeFourCC("L555"), // ALIAS FOR RGBO.
        FOURCC_L565 = MakeFourCC("L565"), // ALIAS FOR RGBP.
        FOURCC_L551 = MakeFourCC("L551"), // ALIAS FOR RGB0.

        GRAY1 = MakeFormatValue_RGB(0),
        GRAY2,
        GRAY4,
        GRAY8,

        R3G3B2,

        //---------------------------------16 BITS
        R8G8,
        R5G6B5,
        B5G6R5,
        A1R5G5B5,
        A1B5G5R5,
        X1R5G5B5,
        X1B5G5R5,
        A4R4G4B4,
        X4R4G4B4,
        A8R3G3B2,

        //---------------------------------24 BITS
        R8G8B8 = RGB,
        B8G8R8 = BGR,
        A8R5G6B5,
        A8B5G6R5,
        A8X1R5G5B5,
        A8X1B5G5R5,

        //---------------------------------32 BITS
        A8R8G8B8 = ARGB,
        X8R8G8B8 = XRGB,
        A8B8G8R8 = ABGR,
        X8B8G8R8 = XBGR,
        R8G8B8A8 = RGBA,
        R8G8B8X8 = RGBX,
        B8G8R8A8 = BGRA,
        B8G8R8X8 = BGRX,

        G16R16,

        A2R10G10B10,
        A2B10G10R10,

        //---------------------------------
        A16B16G16R16,

        R16F,
        G16R16F,
        A16B16G16R16F,
        R32F,
        G32R32F,
        B32G32R32F,
        A32B32G32R32F,

        X32Y32Z32F,

        A4R5G6B5,

        // INDEXED
        INDEX4_A8R8G8B8,
        INDEX8_A8R8G8B8,

    	// DepthStencil
    	D24S8,
    };
}
