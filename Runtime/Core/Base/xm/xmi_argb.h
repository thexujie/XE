#pragma once

#include "xm.h"

namespace XE::xm
{
    inline xmi argb_mul(xmi c1, xmi c2)
    {
        xmi result0 = _mm_mullo_epi32(c1, c2);

        xmi mask1 = _mm_cmpgt_epi32(result0, xmi_255);
        xmi result1 = xmi_select(result0, xmi_255, mask1);

        xmi mask2 = _mm_cmplt_epi32(result1, xmi_0);
        xmi result2 = xmi_select(result1, xmi_0, mask2);

        return result2;
    }


    inline xmi argb_blend(xmi src, xmi dst)
    {
        // 先计算 rgb
        xmi src_alpha = _mm_shuffle_epi32(src, xm_shuffle(0, 0, 0, 0));
        xmi src_argb = _mm_mullo_epi16(src, src_alpha);
        xmi dst_alpha = _mm_sub_epi16(xmi_255, src_alpha);
        xmi dst_argb = _mm_mullo_epi16(dst, dst_alpha);

        xmi result_x255 = _mm_add_epi32(src_argb, dst_argb);
        xmi result = _mm_srli_epi32(result_x255, 8);

        result = _mm_min_epi32(result, xmi_255);

        // 计算 alpha
        dst_alpha = _mm_shuffle_epi32(dst, xm_shuffle(0, 0, 0, 0));
        src_alpha = _mm_sub_epi16(xmi_255, dst_alpha);
        src_alpha = _mm_mullo_epi16(src, src_alpha);
        src_alpha = _mm_srli_epi32(src_alpha, 8);
        dst_alpha = _mm_add_epi32(src_alpha, dst_alpha);

        result = xmi_select(result, dst_alpha, xmi_select_1000);
        return result;
    }

    // sse2 的 intrin 不建议再使用
    // int res = 0;
    // res = argb_blend(0, 0x80FFFFFF); // 0x80ffffff
    // res = argb_blend(0x80FFFFFF, 0); // 0x40808080
    // res = argb_blend(0x80000000, 0x80604020); // 0xc0302010
    // res = argb_blend(0x80604020, 0x80604020); // 0xc0604020
    // res = argb_blend(0x80604020, 0x20604020); // 0x60604020
    // res = argb_blend(0x80604020, 0xC0A08060); // 0xff806040
    // res = argb_blend(0x80FFFFFF, 0x80FFFFFF); // 0xc0ffffff
    // res = argb_blend(0xFFFFFFFF, 0xFFFFFFFF); // 0xffffffff
    // res = 0;
    //inline unsigned int argb_blend(unsigned int src, unsigned int dst)
    //{
    //	unsigned char alpha = src >> 24;
    //	unsigned int alpha_xxxx = alpha | (alpha << 8) | (alpha << 16) | (alpha << 24);
    //	_m_empty();
    //
    //	// 弥补 /255 和 /256(>>8) 的误差，结果为四舍五入，先 >>1 的，只需要 0x80
    //	__m64 tolerance = _m_from_int(0x01010101);
    //	tolerance = _m_punpcklbw(tolerance, tolerance);
    //	tolerance = _m_psllwi(tolerance, 8);
    //	// 0xff - alpha
    //	__m64 ffff = _m_from_int(0xffffffff);
    //	ffff = _m_punpcklbw(ffff, ffff);
    //	ffff = _m_psrlwi(ffff, 8);
    //
    //	// alpha = src >> 24
    //	__m64 src_aaaa = _m_from_int(alpha_xxxx);
    //	src_aaaa = _m_punpcklbw(src_aaaa, src_aaaa);
    //	//src_aaaa = _m_psrlwi(src_aaaa, 8);
    //
    //	//---------------------------------------------------------------- -src
    //	// argb = src
    //	__m64 src_argb = _m_from_int(src);
    //	src_argb = _m_punpcklbw(src_argb, src_argb);
    //	src_argb = _m_psrlwi(src_argb, 8);
    //
    //	// 计算乘积
    //	// res_src.argb = (src_argb.argb * src_argb.aaaa) + 0x100;
    //	__m64 res_src = _m_pmullw(src_argb, src_aaaa);
    //
    //	// -----------------------------------------------------------------dst
    //	__m64 dst_aaaa = _m_psubb(ffff, src_aaaa);
    //
    //	// argb = dst
    //	__m64 dst_argb = _m_from_int(dst);
    //	dst_argb = _m_punpcklbw(dst_argb, dst_argb);
    //	dst_argb = _m_psrlwi(dst_argb, 8);
    //
    //	// res_dst.argb = (dst.argb * (0xff -src.aaaa))
    //	__m64 res_dst = _m_pmullw(dst_argb, dst_aaaa);
    //
    //	// -----------------------------------------------------------------src + dst
    //	__m64 res_total = _m_paddw(res_src, res_dst);
    //	res_total = _m_paddw(res_total, tolerance);
    //	res_total = _m_psrlwi(res_total, 8);
    //
    //	res_total = _m_packuswb(res_total, res_total);
    //	unsigned int res_xrgb = (unsigned int)_m_to_int(res_total);
    //
    //	// ----------------------------------------------------------------- dst.a = dst.a + src.a * src.a
    //	dst_argb = _m_from_int(dst);
    //	dst_argb = _m_punpcklbw(dst_argb, dst_argb);
    //
    //	src_argb = _m_psrlwi(res_src, 8);
    //	dst_argb = _m_psrlwi(dst_argb, 8);
    //
    //	res_total = _m_paddusb(src_argb, dst_argb);
    //	res_total = _m_packuswb(res_total, res_total);
    //	unsigned int res_alpha = (unsigned int)_m_to_int(res_total);
    //
    //	_m_empty();
    //	return (res_alpha & 0xFF000000) | (res_xrgb & 0x00FFFFFF);
    //}
}
