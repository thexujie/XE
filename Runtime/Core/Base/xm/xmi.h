#pragma once

#include "xm.h"

namespace XE::xm
{
    const xmi xmi_0 = { 0, 0, 0, 0 };
    const xmi xmi_255 = { 255, 255, 255, 255 };
    const xmi xmi_select_1000 = { 0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u };

    inline xmi xmi_select(xmi v1, xmi v2, xmi ctrl)
    {
#ifdef VXM_SSE
        xmi vTemp1 = _mm_andnot_si128(ctrl, v1);
        xmi vTemp2 = _mm_and_si128(v2, ctrl);
        return _mm_or_si128(vTemp1, vTemp2);
#else
                xmi result;
                result.aui[0] = (v1.aui[0] & ~ctrl.aui[0]) | (v2.aui[0] & ctrl.aui[0]);
                result.aui[1] = (v1.aui[1] & ~ctrl.aui[1]) | (v2.aui[1] & ctrl.aui[1]);
                result.aui[2] = (v1.aui[2] & ~ctrl.aui[2]) | (v2.aui[2] & ctrl.aui[2]);
                result.aui[3] = (v1.aui[3] & ~ctrl.aui[3]) | (v2.aui[3] & ctrl.aui[3]);
                return result;
#endif
    }

    inline xmi xmi_qualify(xmi i, xmi min, xmi max)
    {
        //xmi mask = _mm_cmplt_epi32(i, min);
        //i = xmi_select(i, min, mask);
        //mask = _mm_cmpgt_epi32(i, max);
        //i = xmi_select(i, max, mask);
        i = _mm_min_epi32(i, min);
        i = _mm_max_epi32(i, max);
        return i;
    }
}
