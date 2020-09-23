#pragma once

#include "xm.h"

namespace XE::xm
{
    const float32_t float_epsilon = 1.192092896e-07f;

    inline bool float_is_zero(float32_t val) { return abs(val) < float_epsilon; }
    inline bool float_equal(const float32_t & val1, const float32_t & val2) { return abs(val1 - val2) < float_epsilon; }
    inline float32_t float_reciprocal(float32_t val) { return 1.0f / val; }
    inline float32_t float_sqrt_reciprocal(float32_t val) { return 1.0f / sqrt(val); }

    inline bool float_is_inf(float32_t val) { return ((*(uint32_t *)&val) & 0x7FFFFFFF) == 0x7F800000; }
    inline float32_t float_abs(float32_t val) { return abs(val); }
    inline float32_t float_round(float32_t val) { return roundf(val); }
    inline float32_t float_sqrt(float32_t val) { return sqrt(val); }
    inline float32_t float_sin(float32_t val) { return sin(val); }
    inline float32_t float_cos(float32_t val) { return cos(val); }
    inline float32_t float_tan(float32_t val) { return tan(val); }
    inline float32_t float_cot(float32_t val) { return 1.0f / tan(val); }
    inline float32_t float_atan(float32_t val) { return atan(val); }

    float32_t float_lerp(float32_t min, float32_t max, float32_t determine);

    const uint32_t xm_permute_0x = 0;
    const uint32_t xm_permute_0y = 1;
    const uint32_t xm_permute_0z = 2;
    const uint32_t xm_permute_0w = 3;
    const uint32_t xm_permute_1x = 4;
    const uint32_t xm_permute_1y = 5;
    const uint32_t xm_permute_1z = 6;
    const uint32_t xm_permute_1w = 7;

    const uint32_t xm_swizzle_x = 0;
    const uint32_t xm_swizzle_y = 1;
    const uint32_t xm_swizzle_z = 2;
    const uint32_t xm_swizzle_w = 3;

    const float32_t xm_pi = 3.141592654f;
    const float32_t xm_eplison = 1.0e-12f;
    const float32_t xm_2pi = xm_pi * 2;
    const float32_t xm_1_of_1pi = 0.318309886f;
    const float32_t xm_1_of_2pi = 0.159154943f;
    const float32_t xm_1pi_of_2 = 1.570796327f;
    const float32_t xm_1pi_of_4 = 0.785398163f;

    const uint32_t xm_select_0 = 0x00000000;
    const uint32_t xm_select_1 = 0xFFFFFFFF;

    const xmf xmf_zero = { 0.0f, 0.0f, 0.0f, 0.0f };
    const xmf xmf_one = { 1.0f, 1.0f, 1.0f, 1.0f };

    const xmf xmf_1of2 = { 0.5f, 0.5f, 0.5f, 0.5f };

    const xmui xmf_negative_zero = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
    const xmf xmf_negative_one = { -1.0f, -1.0f, -1.0f, -1.0f };

    const xmui xmf_abs_mask = { 0x7FFFFFFFu, 0x7FFFFFFFu, 0x7FFFFFFFu, 0x7FFFFFFFu };
    const xmui xmf_infinity = { 0x7F800000u, 0x7F800000u, 0x7F800000u, 0x7F800000u };
    const xmui xmf_mask_3 = { 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u };
    const xmui xmf_mask_x = { 0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u };
    const xmui xmf_mask_y = { 0x00000000u, 0xFFFFFFFFu, 0x00000000u, 0x00000000u };
    const xmui xmf_mask_z = { 0x00000000u, 0x00000000u, 0xFFFFFFFFu, 0x00000000u };
    const xmui xmf_mask_w = { 0x00000000u, 0x00000000u, 0x00000000u, 0xFFFFFFFFu };
    const xmui xmf_qnan = { 0x7FC00000u, 0x7FC00000u, 0x7FC00000u, 0x7FC00000u };

    const xmf xmf_pi = { xm_pi, xm_pi, xm_pi, xm_pi };
    const xmf xmf_2pi = { xm_2pi, xm_2pi, xm_2pi, xm_2pi };
    const xmf xmf_1pi_of_2 = { xm_1pi_of_2, xm_1pi_of_2, xm_1pi_of_2, xm_1pi_of_2 };
    const xmf xmf_1_of_2pi = { xm_1_of_2pi, xm_1_of_2pi, xm_1_of_2pi, xm_1_of_2pi };

    const xmf xmf_row0 = { 1.0f, 0.0f, 0.0f, 0.0f };
    const xmf xmf_row1 = { 0.0f, 1.0f, 0.0f, 0.0f };
    const xmf xmf_row2 = { 0.0f, 0.0f, 1.0f, 0.0f };
    const xmf xmf_row3 = { 0.0f, 0.0f, 0.0f, 1.0f };
    const xmf xmf_row3_neg = { 0.0f, 0.0f, 0.0f, -1.0f };

    const xmf xmf_0000 = { 0.0f, 0.0f, 0.0f, 0.0f };
    const xmf xmf_1000 = { 1.0f, 0.0f, 0.0f, 0.0f };
    const xmf xmf_1110 = { 1.0f, 1.0f, 1.0f, 0.0f };
    const xmf xmf_1111 = { 1.0f, 1.0f, 1.0f, 1.0f };

    const xmui xmf_select_1110 = { xm_select_1, xm_select_1, xm_select_1, xm_select_0 };
    const xmui xmf_select_0101 = { xm_select_0, xm_select_1, xm_select_0, xm_select_1 };

    const xmf xmf_epsilon = { 1.192092896e-07f, 1.192092896e-07f, 1.192092896e-07f, 1.192092896e-07f };
    // atan ÏµÊý
    const xmf xmf_atan_coefficients1 = { -0.0752896400f, +0.0429096138f, -0.0161657367f, +0.0028662257f };

    const xmf xmf_sin_coefficients0 = { -0.16666667f, +0.0083333310f, -0.00019840874f, +2.7525562e-06f };
    const xmf xmf_sin_coefficients1 = { -2.3889859e-08f, -0.16665852f, +0.0083139502f, -0.00018524670f };

    const xmf xmf_cos_coefficients0 = { -0.5f, +0.041666638f, -0.0013888378f, +2.4760495e-05f };
    const xmf xmf_cos_coefficients1 = { -2.6051615e-07f, -0.49992746f, +0.041493919f, -0.0012712436f };
}
