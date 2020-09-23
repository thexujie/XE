#pragma once

#include "../../Base/Types.h"

//! ∆Ù”√ SSE
#define VXM_SSE
//#define VXM_DX9
//#define VXM_XMATH

#ifdef VXM_SSE
#include <intrin.h>
#elif defined VXM_DX9
#pragma push_macro("new")
#undef new
#include <d3dx9.h>
#pragma pop_macro("new")
#elif defined VXM_XMATH
#include <DirectXMath.h>
using namespace DirectX;
#endif

#define xm_shuffle(fp0, fp1, fp2, fp3) (((fp0) << 0) | ((fp1) << 2) | ((fp2) << 4) | ((fp3 << 6)))

namespace XE::xm
{
    union xmt
    {
        xmt() { }

        xmt(float32_t _x, float32_t _y, float32_t _z, float32_t _w) : fx(_x), fy(_y), fz(_z), fw(_w) { }

        xmt(int32_t _x, int32_t _y, int32_t _z, int32_t _w) : ix(_x), iy(_y), iz(_z), iw(_w) { }

        xmt(uint32_t _x, uint32_t _y, uint32_t _z, uint32_t _w) : uix(_x), uiy(_y), uiz(_z), uiw(_w) { }

        float32_t af[4];

        struct
        {
            float32_t fx;
            float32_t fy;
            float32_t fz;
            float32_t fw;
        };

        int32_t ai[4];

        struct
        {
            int32_t ix;
            int32_t iy;
            int32_t iz;
            int32_t iw;
        };

        uint32_t aui[4];

        struct
        {
            uint32_t uix;
            uint32_t uiy;
            uint32_t uiz;
            uint32_t uiw;
        };

        float32_t & operator[](int32_t iIndex) { return af[iIndex]; }
        const float32_t & operator[](int32_t iIndex) const { return af[iIndex]; }

#ifdef VXM_SSE
#ifdef BIT64
        __m128 m128;
        xmt(__m128 _m128) : m128(_m128) {}
        operator const __m128 &() const { return m128; }
        operator __m128 &() { return m128; }

        xmt & operator =(__m128 _m128)
        {
            m128 = _m128;
            return *this;
        }

        __m128i m128i;
        xmt(__m128i _m128i) : m128i(_m128i) {}
        operator const __m128i &() const { return m128i; }
        operator __m128i &() { return m128i; }

        xmt & operator =(__m128i _m128i)
        {
            m128i = _m128i;
            return *this;
        }
#else // BIT64
        xmt(__m128 _m128) : xmt(_m128.m128_f32[0], _m128.m128_f32[1], _m128.m128_f32[2], _m128.m128_f32[3])
        {
        }

        operator __m128() const { return _mm_load_ps(af); }

        xmt & operator =(__m128 _m128)
        {
            _mm_store_ps(af, _m128);
            return *this;
        }

        xmt(__m128i _m128i) : xmt(_m128i.m128i_i32[0], _m128i.m128i_i32[1], _m128i.m128i_i32[2], _m128i.m128i_i32[3])
        {
        }

        operator __m128i() const { return _mm_castps_si128(*(const __m128 *)af); };

        xmt & operator =(__m128i _m128i)
        {
            _mm_store_si128((__m128i *)af, _m128i);
            return *this;
        }
#endif // BIT64
#endif // VXM_SSE
    };

    typedef xmt xmf;
    typedef xmt xmi;
    typedef xmt xmui;

    struct xmm
    {
        xmm() { }

        xmm(xmf _row0, xmf _row1, xmf _row2, xmf _row3) : row0(_row0), row1(_row1), row2(_row2), row3(_row3) { }

        xmf row0;
        xmf row1;
        xmf row2;
        xmf row3;

        xmf & operator[](int32_t iIndex) { return ((xmf *)this)[iIndex]; }
        const xmf & operator[](int32_t iIndex) const { return ((xmf *)this)[iIndex]; }
    };

    constexpr float32_t xmv_x(const xmf & v) { return v.fx; }
    constexpr float32_t xmv_y(const xmf & v) { return v.fy; }
    constexpr float32_t xmv_z(const xmf & v) { return v.fz; }
    constexpr float32_t xmv_w(const xmf & v) { return v.fw; }
    constexpr int32_t xmi_x(const xmf & v) { return v.ix; }
    constexpr int32_t xmi_y(const xmf & v) { return v.iy; }
    constexpr int32_t xmi_z(const xmf & v) { return v.iz; }
    constexpr int32_t xmi_w(const xmf & v) { return v.iw; }
}
