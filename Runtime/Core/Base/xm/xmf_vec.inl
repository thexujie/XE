#pragma once

#define xm_permute_ps(v, ctrl) _mm_shuffle_ps(v, v, ctrl)
#define xm_shuffle_ps(v1, v2, ctrl) _mm_shuffle_ps(v1, v2, ctrl)

namespace XE::xm
{
	inline xmf xm_vec_equal_int(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		__m128i v = _mm_cmpeq_epi32(_mm_castps_si128(v1), _mm_castps_si128(v2));
		return _mm_castsi128_ps(v);
#else
		xmf result;
		result.aui[0] = (v1.aui[0] == v2.aui[0]) ? 0xFFFFFFFF : 0;
		result.aui[1] = (v1.aui[1] == v2.aui[1]) ? 0xFFFFFFFF : 0;
		result.aui[2] = (v1.aui[2] == v2.aui[2]) ? 0xFFFFFFFF : 0;
		result.aui[3] = (v1.aui[3] == v2.aui[3]) ? 0xFFFFFFFF : 0;
		return result;
#endif
	}

	inline xmf xm_vec_or_int(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		__m128i v = _mm_or_si128(_mm_castps_si128(v1), _mm_castps_si128(v2));
		return _mm_castsi128_ps(v);
#else
		xmf result;
		result.aui[0] = v1.aui[0] | v2.aui[0];
		result.aui[1] = v1.aui[1] | v2.aui[1];
		result.aui[2] = v1.aui[2] | v2.aui[2];
		result.aui[3] = v1.aui[3] | v2.aui[3];
		return result;
#endif
	}

	inline xmf xm_vec_is_infinite(xmf v)
	{
#ifdef VXM_SSE
		xmf vTemp = _mm_and_ps(v, xmf_abs_mask);
		return _mm_cmpeq_ps(vTemp, xmf_infinity);
#else
		xmf result;
		result.aui[0] = float_is_inf(v.af[0]) ? 0xFFFFFFFFU : 0;
		result.aui[1] = float_is_inf(v.af[1]) ? 0xFFFFFFFFU : 0;
		result.aui[2] = float_is_inf(v.af[2]) ? 0xFFFFFFFFU : 0;
		result.aui[3] = float_is_inf(v.af[3]) ? 0xFFFFFFFFU : 0;
		return result;
#endif
	}

	// ------------------------------------------------------------ 重复某一成员

	inline xmf xm_vec_splat_x(xmf v)
	{
#ifdef VXM_SSE
		return xm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
#else
		return xmf(v.fx, v.fx, v.fx, v.fx);
#endif
	}

	inline xmf xm_vec_splat_y(xmf v)
	{
#ifdef VXM_SSE
		return xm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
#else
		return xmf(v.fy, v.fy, v.fy, v.fy);
#endif
	}

	inline xmf xm_vec_splat_z(xmf v)
	{
#ifdef VXM_SSE
		return xm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
#else
		return xmf(v.fz, v.fz, v.fz, v.fz);
#endif
	}

	inline xmf xm_vec_splat_w(xmf v)
	{
#ifdef VXM_SSE
		return xm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
#else
		return xmf(v.fw, v.fw, v.fw, v.fw);
#endif
	}

	// ------------------------------------------------------------ 基本操作
	inline xmf xm_vec_zero()
	{
#ifdef VXM_SSE
		return _mm_setzero_ps();
#else
		return xmf_zero;
#endif
	}

	inline xmf xm_vec_true_int()
	{
#ifdef VXM_SSE
		__m128i V = _mm_set1_epi32(-1);
		return _mm_castsi128_ps(V);
#else
		return xmf(0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu);
#endif
	}

	inline xmf xm_vec_set(float32_t x, float32_t y, float32_t z, float32_t w)
	{
#ifdef VXM_SSE
		return _mm_set_ps(w, z, y, x);
#else
		return xmf(x, y, z, w);
#endif
	}

	// 复制
	inline xmf xm_vec_replicate(float32_t value)
	{
#ifdef VXM_SSE
		return _mm_set_ps1(value);
#else
		return xmf(value, value, value, value);
#endif
	}

	inline xmf xm_vec_negate(xmf v)
	{
#ifdef VXM_SSE
		return _mm_sub_ps(xmf_zero, v);
#else
		return xmf(-v.x, -v.y, -v.z, -v.w);
#endif
	}

	inline xmf xm_vec_permute(xmf v1, xmf v2, uint32_t pmx, uint32_t pmy, uint32_t pmz, uint32_t pmw)
	{
		const uint32_t * aPtr[2];
		aPtr[0] = (const uint32_t *)(&v1);
		aPtr[1] = (const uint32_t *)(&v2);

		xmf Result;
		uint32_t * pWork = (uint32_t *)(&Result);

		const uint32_t i0 = pmx & 3;
		const uint32_t vi0 = pmx >> 2;
		pWork[0] = aPtr[vi0][i0];

		const uint32_t i1 = pmy & 3;
		const uint32_t vi1 = pmy >> 2;
		pWork[1] = aPtr[vi1][i1];

		const uint32_t i2 = pmz & 3;
		const uint32_t vi2 = pmz >> 2;
		pWork[2] = aPtr[vi2][i2];

		const uint32_t i3 = pmw & 3;
		const uint32_t vi3 = pmw >> 2;
		pWork[3] = aPtr[vi3][i3];
		return Result;
	}

	// General permute template
	template <uint32_t PermuteX, uint32_t PermuteY, uint32_t PermuteZ, uint32_t PermuteW>
	inline xmf xm_vec_permute(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		const uint32_t Shuffle = _MM_SHUFFLE(PermuteW & 3, PermuteZ & 3, PermuteY & 3, PermuteX & 3);

		const bool WhichX = PermuteX > 3;
		const bool WhichY = PermuteY > 3;
		const bool WhichZ = PermuteZ > 3;
		const bool WhichW = PermuteW > 3;
		static const xmf selectMask = xmf(
			WhichX ? 0xFFFFFFFFu : 0u,
			WhichY ? 0xFFFFFFFFu : 0u,
			WhichZ ? 0xFFFFFFFFu : 0u,
			WhichW ? 0xFFFFFFFFu : 0u);

		xmf shuffled1 = xm_permute_ps(v1, Shuffle);
		xmf shuffled2 = xm_permute_ps(v2, Shuffle);

		xmf masked1 = _mm_andnot_ps(selectMask, shuffled1);
		xmf masked2 = _mm_and_ps(selectMask, shuffled2);

		return _mm_or_ps(masked1, masked2);
#else
		return xm_vec_permute(v1, v2, PermuteX, PermuteY, PermuteZ, PermuteW);
#endif
	}

	inline xmf xm_vec_swizzle(xmf v, uint32_t E0, uint32_t E1, uint32_t E2, uint32_t E3)
	{
		const uint32_t * aPtr = (const uint32_t *)(&v);

		xmf result;
		uint32_t * pWork = (uint32_t *)(&result);

		pWork[0] = aPtr[E0];
		pWork[1] = aPtr[E1];
		pWork[2] = aPtr[E2];
		pWork[3] = aPtr[E3];

		return result;
	}

	template <uint32_t SwizzleX, uint32_t SwizzleY, uint32_t SwizzleZ, uint32_t SwizzleW>
	inline xmf xm_vec_swizzle(xmf v)
	{
#ifdef VXM_SSE
		return _mm_shuffle_ps(v, v, _MM_SHUFFLE(SwizzleW, SwizzleZ, SwizzleY, SwizzleX));
#else
		return xm_vec_swizzle(v, SwizzleX, SwizzleY, SwizzleZ, SwizzleW);
#endif
	}

	inline xmf xm_vec_select(xmf v1, xmf v2, xmf ctrl)
	{
#ifdef VXM_SSE
		xmf vTemp1 = _mm_andnot_ps(ctrl, v1);
		xmf vTemp2 = _mm_and_ps(v2, ctrl);
		return _mm_or_ps(vTemp1, vTemp2);
#else
		xmf result;
		result.aui[0] = (v1.aui[0] & ~ctrl.aui[0]) | (v2.aui[0] & ctrl.aui[0]);
		result.aui[1] = (v1.aui[1] & ~ctrl.aui[1]) | (v2.aui[1] & ctrl.aui[1]);
		result.aui[2] = (v1.aui[2] & ~ctrl.aui[2]) | (v2.aui[2] & ctrl.aui[2]);
		result.aui[3] = (v1.aui[3] & ~ctrl.aui[3]) | (v2.aui[3] & ctrl.aui[3]);
		return result;
#endif
	}

	inline xmf xm_vec_abs(xmf v)
	{
#ifdef VXM_SSE
		xmf vResult = _mm_setzero_ps();
		vResult = _mm_sub_ps(vResult, v);
		vResult = _mm_max_ps(vResult, v);
		return vResult;
#else
		xmf result;
		result.af[0] = float_abs(v.af[0]);
		result.af[1] = float_abs(v.af[1]);
		result.af[2] = float_abs(v.af[2]);
		result.af[3] = float_abs(v.af[3]);
		return result;
#endif
	}

	inline xmf xm_vec_round(xmf v)
	{
#ifdef VXM_SSE
		static const xmf magic = { 0x4B000000u, 0x4B000000u, 0x4B000000u, 0x4B000000u };
		xmf sign = _mm_and_ps(v, xmf_negative_zero);
		xmf sMagic = _mm_or_ps(magic, sign);
		xmf vResult = _mm_add_ps(v, sMagic);
		vResult = _mm_sub_ps(vResult, sMagic);
		return vResult;
#else#else
		xmf result;
		result.af[0] = float_round(v.af[0]);
		result.af[1] = float_round(v.af[1]);
		result.af[2] = float_round(v.af[2]);
		result.af[3] = float_round(v.af[3]);
		return result;
#endif
	}

	// ------------------------------------------------------------ 逻辑运算
	inline xmf xm_vec_and(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_and_ps(v1, v2);
#else
		xmf result;
		result.aui[0] = v1.aui[0] & v2.aui[0];
		result.aui[1] = v1.aui[1] & v2.aui[1];
		result.aui[2] = v1.aui[2] & v2.aui[2];
		result.aui[3] = v1.aui[3] & v2.aui[3];
		return result;
#endif
	}

	inline xmf xm_vec_xor(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_xor_ps(v1, v2);
#else
		xmf result;
		result.aui[0] = v1.aui[0] ^ v2.aui[0];
		result.aui[1] = v1.aui[1] ^ v2.aui[1];
		result.aui[2] = v1.aui[2] ^ v2.aui[2];
		result.aui[3] = v1.aui[3] ^ v2.aui[3];
		return result;
#endif
	}

	inline xmf xm_vec_xor_int(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		__m128i v = _mm_xor_si128(_mm_castps_si128(v1), _mm_castps_si128(v2));
		return _mm_castsi128_ps(v);
#else
		xmf result;
		result.aui[0] = v1.aui[0] ^ v2.aui[0];
		result.aui[1] = v1.aui[1] ^ v2.aui[1];
		result.aui[2] = v1.aui[2] ^ v2.aui[2];
		result.aui[3] = v1.aui[3] ^ v2.aui[3];
		return result;
#endif
	}

	inline xmf xm_vec_shift_left(xmf v1, xmf v2, uint32_t elements)
	{
		return xm_vec_permute(v1, v2, elements, elements + 1, elements + 2, elements + 3);
	}


	// ------------------------------------------------------------ 比较运算
	inline xmf xm_vec_equal(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_cmpeq_ps(v1, v2);
#else
		xmf result;
		result.aui[0] = (v1.af[0] == v2.af[0]) ? 0xFFFFFFFF : 0;
		result.aui[1] = (v1.af[1] == v2.af[1]) ? 0xFFFFFFFF : 0;
		result.aui[2] = (v1.af[2] == v2.af[2]) ? 0xFFFFFFFF : 0;
		result.aui[3] = (v1.af[3] == v2.af[3]) ? 0xFFFFFFFF : 0;
		return result;
#endif
	}

	inline xmf xm_vec_less(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_cmplt_ps(v1, v2);
#else
		xmf result;
		result.aui[0] = (v1.af[0] < v2.af[0]) ? 0xFFFFFFFF : 0;
		result.aui[1] = (v1.af[1] < v2.af[1]) ? 0xFFFFFFFF : 0;
		result.aui[2] = (v1.af[2] < v2.af[2]) ? 0xFFFFFFFF : 0;
		result.aui[3] = (v1.af[3] < v2.af[3]) ? 0xFFFFFFFF : 0;
		return result;
#endif
	}

	// ------------------------------------------------------------ 四则运算

	//! 求倒数
	inline xmf xm_vec_reciprocal(xmf v)
	{
#ifdef VXM_SSE
		return _mm_div_ps(xmf_one, v);
#else
		xmf result;
		result.af[0] = 1.f / v.af[0];
		result.af[1] = 1.f / v.af[1];
		result.af[2] = 1.f / v.af[2];
		result.af[3] = 1.f / v.af[3];
		return result;
#endif
	}

	inline xmf xm_vec_add(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_add_ps(v1, v2);
#else
		return xmf(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
#endif
	}

	inline xmf xm_vec_sub(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_sub_ps(v1, v2);
#else
		return xmf(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
#endif
	}

	inline xmf xm_vec_mul(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_mul_ps(v1, v2);
#else
		return xmf(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
#endif
	}

	inline xmf xm_vec_div(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		return _mm_div_ps(v1, v2);
#else
		return xmf(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
#endif
	}

	//! v3 - (v1 * v2)
	inline xmf xm_vec_negative_mul_sub(xmf v1, xmf v2, xmf v3)
	{
#ifdef VXM_SSE
		xmf R = _mm_mul_ps(v1, v2);
		return _mm_sub_ps(v3, R);
#else
		xmf result = {
			v3.af[0] - (v1.af[0] * v2.af[0]),
			v3.af[1] - (v1.af[1] * v2.af[1]),
			v3.af[2] - (v1.af[2] * v2.af[2]),
			v3.af[3] - (v1.af[3] * v2.af[3])
		};
		return result;
#endif
	}

	//! (v1 * v2) + v3
	inline xmf xm_vec_mul_add(xmf v1, xmf v2, xmf v3)
	{
#ifdef VXM_SSE
		xmf vResult = _mm_mul_ps(v1, v2);
		return _mm_add_ps(vResult, v3);
#else
		xmf vResult = xmf(
			(v1.af[0] * v2.af[0]) + v3.af[0],
			(v1.af[1] * v2.af[1]) + v3.af[1],
			(v1.af[2] * v2.af[2]) + v3.af[2],
			(v1.af[3] * v2.af[3]) + v3.af[3]);
		return vResult;
#endif
	}

	// ------------------------------------------------------------ 三角函数
	inline xmf xm_vec_mod_angles(xmf angles)
	{
#ifdef VXM_SSE
		// Modulo the range of the given angles such that -XM_PI <= Angles < XM_PI
		xmf vResult = _mm_mul_ps(angles, xmf_1_of_2pi);
		// Use the inline function due to complexity for rounding
		vResult = xm_vec_round(vResult);
		vResult = _mm_mul_ps(vResult, xmf_2pi);
		vResult = _mm_sub_ps(angles, vResult);
		return vResult;
#else
		xmf V;
		xmf Result;

		// Modulo the range of the given angles such that -XM_PI <= Angles < XM_PI
		V = xm_vec_mul(angles, xmf_1_of_2pi);
		V = xm_vec_round(V);
		Result = xm_vec_negative_mul_sub(xmf_2pi, V, angles);
		return Result;
#endif
	}

	inline xmf xm_vec_sin(xmf v)
	{
#ifdef VXM_SSE
		// Force the value within the bounds of pi
		xmf x = xm_vec_mod_angles(v);

		// Map in [-pi/2,pi/2] with sin(y) = sin(x).
		xmf sign = _mm_and_ps(x, xmf_negative_zero);
		xmf c = _mm_or_ps(xmf_pi, sign);  // pi when x >= 0, -pi when x < 0
		xmf absx = _mm_andnot_ps(sign, x);  // |x|
		xmf rflx = _mm_sub_ps(c, x);
		xmf comp = _mm_cmple_ps(absx, xmf_1pi_of_2);
		xmf select0 = _mm_and_ps(comp, x);
		xmf select1 = _mm_andnot_ps(comp, rflx);
		x = _mm_or_ps(select0, select1);

		xmf x2 = _mm_mul_ps(x, x);

		// Compute polynomial approximation
		const xmf SC1 = xmf_sin_coefficients1;
		xmf vConstants = xm_shuffle_ps(SC1, SC1, _MM_SHUFFLE(0, 0, 0, 0));
		xmf Result = _mm_mul_ps(vConstants, x2);

		const xmf SC0 = xmf_sin_coefficients0;
		vConstants = xm_shuffle_ps(SC0, SC0, _MM_SHUFFLE(3, 3, 3, 3));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(SC0, SC0, _MM_SHUFFLE(2, 2, 2, 2));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(SC0, SC0, _MM_SHUFFLE(1, 1, 1, 1));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(SC0, SC0, _MM_SHUFFLE(0, 0, 0, 0));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);
		Result = _mm_add_ps(Result, xmf_one);
		Result = _mm_mul_ps(Result, x);
		return Result;
#else
		xmf Result;
		Result.af[0] = xm_scalar_sin(v.af[0]);
		Result.af[1] = xm_scalar_sin(v.af[1]);
		Result.af[2] = xm_scalar_sin(v.af[2]);
		Result.af[3] = xm_scalar_sin(v.af[3]);
		return Result;
#endif
	}

	inline xmf xm_vec_atan(xmf v)
	{
#ifdef VXM_SSE
		// 17-degree minimax approximation
		xmf absV = xm_vec_abs(v);
		xmf invV = _mm_div_ps(xmf_one, v);
		xmf comp = _mm_cmpgt_ps(v, xmf_one);
		xmf select0 = _mm_and_ps(comp, xmf_one);
		xmf select1 = _mm_andnot_ps(comp, xmf_negative_one);
		xmf sign = _mm_or_ps(select0, select1);
		comp = _mm_cmple_ps(absV, xmf_one);
		select0 = _mm_and_ps(comp, xmf_zero);
		select1 = _mm_andnot_ps(comp, sign);
		sign = _mm_or_ps(select0, select1);
		select0 = _mm_and_ps(comp, v);
		select1 = _mm_andnot_ps(comp, invV);
		xmf x = _mm_or_ps(select0, select1);

		xmf x2 = _mm_mul_ps(x, x);

		// Compute polynomial approximation
		const xmf TC1 = xmf_atan_coefficients1;
		xmf vConstants = xm_shuffle_ps(TC1, TC1, _MM_SHUFFLE(3, 3, 3, 3));
		xmf Result = _mm_mul_ps(vConstants, x2);

		vConstants = xm_shuffle_ps(TC1, TC1, _MM_SHUFFLE(2, 2, 2, 2));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(TC1, TC1, _MM_SHUFFLE(1, 1, 1, 1));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(TC1, TC1, _MM_SHUFFLE(0, 0, 0, 0));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		const xmf TC0 = xmf_atan_coefficients1;
		vConstants = xm_shuffle_ps(TC0, TC0, _MM_SHUFFLE(3, 3, 3, 3));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(TC0, TC0, _MM_SHUFFLE(2, 2, 2, 2));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(TC0, TC0, _MM_SHUFFLE(1, 1, 1, 1));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_shuffle_ps(TC0, TC0, _MM_SHUFFLE(0, 0, 0, 0));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);
		Result = _mm_add_ps(Result, xmf_one);
		Result = _mm_mul_ps(Result, x);
		xmf result1 = _mm_mul_ps(sign, xmf_1pi_of_2);
		result1 = _mm_sub_ps(result1, Result);

		comp = _mm_cmpeq_ps(sign, xmf_zero);
		select0 = _mm_and_ps(comp, Result);
		select1 = _mm_andnot_ps(comp, result1);
		Result = _mm_or_ps(select0, select1);
		return Result;
#else
		xmf Result;
		Result.af[0] = float_atan(v.af[0]);
		Result.af[1] = float_atan(v.af[1]);
		Result.af[2] = float_atan(v.af[2]);
		Result.af[3] = float_atan(v.af[3]);
		return Result;
#endif
	}

	inline xmf xm_vec_atan2(xmf y, xmf x)
	{
		// Return the inverse tangent of Y / X in the range of -Pi to Pi with the following exceptions:

		//     Y == 0 and X is Negative         -> Pi with the sign of Y
		//     y == 0 and x is positive         -> 0 with the sign of y
		//     Y != 0 and X == 0                -> Pi / 2 with the sign of Y
		//     Y != 0 and X is Negative         -> atan(y/x) + (PI with the sign of Y)
		//     X == -Infinity and Finite Y      -> Pi with the sign of Y
		//     X == +Infinity and Finite Y      -> 0 with the sign of Y
		//     Y == Infinity and X is Finite    -> Pi / 2 with the sign of Y
		//     Y == Infinity and X == -Infinity -> 3Pi / 4 with the sign of Y
		//     Y == Infinity and X == +Infinity -> Pi / 4 with the sign of Y

		const xmf ATan2Constants = { xm_pi, xm_1pi_of_2, xm_1pi_of_4, xm_pi * 3.0f / 4.0f };

		xmf ATanResultValid = xm_vec_true_int();

		xmf Pi = xm_vec_splat_x(ATan2Constants);
		xmf PiOverTwo = xm_vec_splat_y(ATan2Constants);
		xmf PiOverFour = xm_vec_splat_z(ATan2Constants);
		xmf ThreePiOverFour = xm_vec_splat_w(ATan2Constants);

		xmf YEqualsZero = xm_vec_equal(y, xmf_zero);
		xmf XEqualsZero = xm_vec_equal(x, xmf_zero);
		xmf XIsPositive = xm_vec_and(x, xmf_negative_zero);
		XIsPositive = xm_vec_equal_int(XIsPositive, xmf_zero);
		xmf YEqualsInfinity = xm_vec_is_infinite(y);
		xmf XEqualsInfinity = xm_vec_is_infinite(x);

		xmf YSign = xm_vec_and(y, xmf_negative_zero);
		Pi = xm_vec_or_int(Pi, YSign);
		PiOverTwo = xm_vec_or_int(PiOverTwo, YSign);
		PiOverFour = xm_vec_or_int(PiOverFour, YSign);
		ThreePiOverFour = xm_vec_or_int(ThreePiOverFour, YSign);

		xmf R1 = xm_vec_select(Pi, YSign, XIsPositive);
		xmf R2 = xm_vec_select(ATanResultValid, PiOverTwo, XEqualsZero);
		xmf R3 = xm_vec_select(R2, R1, YEqualsZero);
		xmf R4 = xm_vec_select(ThreePiOverFour, PiOverFour, XIsPositive);
		xmf R5 = xm_vec_select(PiOverTwo, R4, XEqualsInfinity);
		xmf Result = xm_vec_select(R3, R5, YEqualsInfinity);
		ATanResultValid = xm_vec_equal_int(Result, ATanResultValid);

		xmf V = xm_vec_div(y, x);

		xmf R0 = xm_vec_atan(V);

		R1 = xm_vec_select(Pi, xmf_zero, XIsPositive);
		R2 = xm_vec_add(R0, R1);

		return xm_vec_select(Result, R2, ATanResultValid);
	}

	inline void xm_vec_sin_cos(xmf * pSin, xmf * pCos, xmf V)
	{
		// 11/10-degree minimax approximation
#ifdef VXM_SSE
	// Force the value within the bounds of pi
		xmf x = xm_vec_mod_angles(V);

		// Map in [-pi/2,pi/2] with sin(y) = sin(x), cos(y) = sign*cos(x).
		xmf sign = _mm_and_ps(x, xmf_negative_zero);
		xmf c = _mm_or_ps(xmf_pi, sign);  // pi when x >= 0, -pi when x < 0
		xmf absx = _mm_andnot_ps(sign, x);  // |x|
		xmf rflx = _mm_sub_ps(c, x);
		xmf comp = _mm_cmple_ps(absx, xmf_1pi_of_2);
		xmf select0 = _mm_and_ps(comp, x);
		xmf select1 = _mm_andnot_ps(comp, rflx);
		x = _mm_or_ps(select0, select1);
		select0 = _mm_and_ps(comp, xmf_one);
		select1 = _mm_andnot_ps(comp, xmf_negative_one);
		sign = _mm_or_ps(select0, select1);

		xmf x2 = _mm_mul_ps(x, x);

		// Compute polynomial approximation of sine
		const xmf SC1 = xmf_sin_coefficients1;
		xmf vConstants = xm_permute_ps(SC1, _MM_SHUFFLE(0, 0, 0, 0));
		xmf Result = _mm_mul_ps(vConstants, x2);

		const xmf SC0 = xmf_sin_coefficients0;
		vConstants = xm_permute_ps(SC0, _MM_SHUFFLE(3, 3, 3, 3));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_permute_ps(SC0, _MM_SHUFFLE(2, 2, 2, 2));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_permute_ps(SC0, _MM_SHUFFLE(1, 1, 1, 1));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_permute_ps(SC0, _MM_SHUFFLE(0, 0, 0, 0));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);
		Result = _mm_add_ps(Result, xmf_one);
		Result = _mm_mul_ps(Result, x);
		*pSin = Result;

		// Compute polynomial approximation of cosine
		const xmf CC1 = xmf_cos_coefficients1;
		vConstants = xm_permute_ps(CC1, _MM_SHUFFLE(0, 0, 0, 0));
		Result = _mm_mul_ps(vConstants, x2);

		const xmf CC0 = xmf_cos_coefficients0;
		vConstants = xm_permute_ps(CC0, _MM_SHUFFLE(3, 3, 3, 3));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_permute_ps(CC0, _MM_SHUFFLE(2, 2, 2, 2));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_permute_ps(CC0, _MM_SHUFFLE(1, 1, 1, 1));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);

		vConstants = xm_permute_ps(CC0, _MM_SHUFFLE(0, 0, 0, 0));
		Result = _mm_add_ps(Result, vConstants);
		Result = _mm_mul_ps(Result, x2);
		Result = _mm_add_ps(Result, xmf_one);
		Result = _mm_mul_ps(Result, sign);
		*pCos = Result;
#else
		xmf Sin;
		xmf Cos;

		xm_scalar_sin_cos(&Sin.af[0], &Cos.af[0], V.af[0]);
		xm_scalar_sin_cos(&Sin.af[1], &Cos.af[1], V.af[1]);
		xm_scalar_sin_cos(&Sin.af[2], &Cos.af[2], V.af[2]);
		xm_scalar_sin_cos(&Sin.af[3], &Cos.af[3], V.af[3]);

		*pSin = Sin;
		*pCos = Cos;
#endif
	}

	// ------------------------------------------------------------ 开方
	inline xmf xm_vec_sqrt(xmf v)
	{
#ifdef VXM_SSE
		return _mm_sqrt_ps(v);
#else
		xmf Result;
		Result.af[0] = float_sqrt(v.af[0]);
		Result.af[1] = float_sqrt(v.af[1]);
		Result.af[2] = float_sqrt(v.af[2]);
		Result.af[3] = float_sqrt(v.af[3]);
		return Result;
#endif
	}

	inline xmf xm_vec_lerp(xmf min, xmf max, float32_t lerp)
	{
#ifdef VXM_SSE
		xmf mLerp = _mm_set_ps1(lerp);
		xmf diff = _mm_sub_ps(max, min);
		diff = _mm_mul_ps(diff, mLerp);
		return _mm_add_ps(min, diff);
#else
		xmf scale = xm_vec_replicate(lerp);
		xmf length = xm_vec_sub(max, min);
		return xm_vec_mul_add(length, scale, min);
#endif
	}

	inline xmf xm_vec_lerp(xmf min, xmf max, xmf lerp)
	{
#ifdef VXM_SSE
		xmf diff = _mm_sub_ps(max, min);
		diff = _mm_mul_ps(diff, lerp);
		return _mm_add_ps(min, diff);
#else
		xmf length = xm_vec_sub(max, min);
		return xm_vec_mul_add(length, lerp, min);
#endif
	}

	inline xmf xm_vec_dot(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		v1 = _mm_mul_ps(v1, v2);
		v2 = xm_shuffle_ps(v2, v1, _MM_SHUFFLE(1, 0, 0, 0));
		v2 = _mm_add_ps(v2, v1);
		v1 = xm_shuffle_ps(v1, v2, _MM_SHUFFLE(0, 3, 0, 0));
		v1 = _mm_add_ps(v1, v2);
		return xm_shuffle_ps(v1, v1, _MM_SHUFFLE(2, 2, 2, 2));
#else
		float32_t val = v1.af[0] * v2.af[0] + v1.af[1] * v2.af[1] + v1.af[2] * v2.af[2] + v1.af[3] * v2.af[3];
		return xmf(val, val, val, val);
#endif
	}

	inline xmf xm_vec_cross(xmf v1, xmf v2, xmf v3)
	{
#ifdef VXM_SSE
		// V2zwyz * V3wzwy
		xmf vResult = xm_permute_ps(v2, _MM_SHUFFLE(2, 1, 3, 2));
		xmf vTemp3 = xm_permute_ps(v3, _MM_SHUFFLE(1, 3, 2, 3));
		vResult = _mm_mul_ps(vResult, vTemp3);
		// - V2wzwy * V3zwyz
		xmf vTemp2 = xm_permute_ps(v2, _MM_SHUFFLE(1, 3, 2, 3));
		vTemp3 = xm_permute_ps(vTemp3, _MM_SHUFFLE(1, 3, 0, 1));
		vTemp2 = _mm_mul_ps(vTemp2, vTemp3);
		vResult = _mm_sub_ps(vResult, vTemp2);
		// term1 * V1yxxx
		xmf vTemp1 = xm_permute_ps(v1, _MM_SHUFFLE(0, 0, 0, 1));
		vResult = _mm_mul_ps(vResult, vTemp1);

		// V2ywxz * V3wxwx
		vTemp2 = xm_permute_ps(v2, _MM_SHUFFLE(2, 0, 3, 1));
		vTemp3 = xm_permute_ps(v3, _MM_SHUFFLE(0, 3, 0, 3));
		vTemp3 = _mm_mul_ps(vTemp3, vTemp2);
		// - V2wxwx * V3ywxz
		vTemp2 = xm_permute_ps(vTemp2, _MM_SHUFFLE(2, 1, 2, 1));
		vTemp1 = xm_permute_ps(v3, _MM_SHUFFLE(2, 0, 3, 1));
		vTemp2 = _mm_mul_ps(vTemp2, vTemp1);
		vTemp3 = _mm_sub_ps(vTemp3, vTemp2);
		// vResult - temp * V1zzyy
		vTemp1 = xm_permute_ps(v1, _MM_SHUFFLE(1, 1, 2, 2));
		vTemp1 = _mm_mul_ps(vTemp1, vTemp3);
		vResult = _mm_sub_ps(vResult, vTemp1);

		// V2yzxy * V3zxyx
		vTemp2 = xm_permute_ps(v2, _MM_SHUFFLE(1, 0, 2, 1));
		vTemp3 = xm_permute_ps(v3, _MM_SHUFFLE(0, 1, 0, 2));
		vTemp3 = _mm_mul_ps(vTemp3, vTemp2);
		// - V2zxyx * V3yzxy
		vTemp2 = xm_permute_ps(vTemp2, _MM_SHUFFLE(2, 0, 2, 1));
		vTemp1 = xm_permute_ps(v3, _MM_SHUFFLE(1, 0, 2, 1));
		vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
		vTemp3 = _mm_sub_ps(vTemp3, vTemp1);
		// vResult + term * V1wwwz
		vTemp1 = xm_permute_ps(v1, _MM_SHUFFLE(2, 3, 3, 3));
		vTemp3 = _mm_mul_ps(vTemp3, vTemp1);
		vResult = _mm_add_ps(vResult, vTemp3);
		return vResult;
#else
		xmf result;
		result.af[0] =
			(((v2.af[2] * v3.af[3]) - (v2.af[3] * v3.af[2])) * v1.af[1]) -
			(((v2.af[1] * v3.af[3]) - (v2.af[3] * v3.af[1])) * v1.af[2]) +
			(((v2.af[1] * v3.af[2]) - (v2.af[2] * v3.af[1])) * v1.af[3]);
		result.af[1] =
			(((v2.af[3] * v3.af[2]) - (v2.af[2] * v3.af[3])) * v1.af[0]) -
			(((v2.af[3] * v3.af[0]) - (v2.af[0] * v3.af[3])) * v1.af[2]) +
			(((v2.af[2] * v3.af[0]) - (v2.af[0] * v3.af[2])) * v1.af[3]);
		result.af[2] =
			(((v2.af[1] * v3.af[3]) - (v2.af[3] * v3.af[1])) * v1.af[0]) -
			(((v2.af[0] * v3.af[3]) - (v2.af[3] * v3.af[0])) * v1.af[1]) +
			(((v2.af[0] * v3.af[1]) - (v2.af[1] * v3.af[0])) * v1.af[3]);
		result.af[3] =
			(((v2.af[2] * v3.af[1]) - (v2.af[1] * v3.af[2])) * v1.af[0]) -
			(((v2.af[2] * v3.af[0]) - (v2.af[0] * v3.af[2])) * v1.af[1]) +
			(((v2.af[1] * v3.af[0]) - (v2.af[0] * v3.af[1])) * v1.af[2]);
		return result;
#endif
	}

	inline xmf xm_vec_length_sq(xmf v)
	{
		return xm_vec_dot(v, v);
	}

	inline xmf xm_vec_length(xmf v)
	{
		xmf temp = xm_vec_dot(v, v);
		return xm_vec_sqrt(temp);
	}

	inline xmf xm_vec_normalize(xmf v)
	{
#ifdef VXM_SSE
		xmf vLengthSq = _mm_mul_ps(v, v);
		// vTemp has z and w
		xmf vTemp = xm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
		// x+z, y+w
		vLengthSq = _mm_add_ps(vLengthSq, vTemp);
		// x+z,x+z,x+z,y+w
		vLengthSq = xm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
		// ??,??,y+w,y+w
		vTemp = xm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
		// ??,??,x+z+y+w,??
		vLengthSq = _mm_add_ps(vLengthSq, vTemp);
		// Splat v length
		vLengthSq = xm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
		// Prepare for v division
		xmf vResult = _mm_sqrt_ps(vLengthSq);
		// Create zero with a single instruction
		xmf vZeroMask = _mm_setzero_ps();
		// Test for a divide by zero (Must be FP to detect -0.0)
		vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
		// Failsafe on zero (Or epsilon) length planes
		// If the length is infinity, set the elements to zero
		vLengthSq = _mm_cmpneq_ps(vLengthSq, xmf_infinity);
		// Divide to perform the normalization
		vResult = _mm_div_ps(v, vResult);
		// Any that are infinity, set to zero
		vResult = _mm_and_ps(vResult, vZeroMask);
		// Select qnan or result based on infinite length
		xmf vTemp1 = _mm_andnot_ps(vLengthSq, xmf_qnan);
		xmf vTemp2 = _mm_and_ps(vResult, vLengthSq);
		vResult = _mm_or_ps(vTemp1, vTemp2);
		return vResult;
#else
		xmf vResult;

		vResult = xm_vec_length_sq(v);
		float32_t fLength = vResult.af[0];

		// Prevent divide by zero
		if (fLength > 0)
		{
			fLength = 1.0f / fLength;
		}

		vResult.af[0] = v.af[0] * fLength;
		vResult.af[1] = v.af[1] * fLength;
		vResult.af[2] = v.af[2] * fLength;
		vResult.af[3] = v.af[3] * fLength;
		return vResult;
#endif
	}

	// ------------------------------------------------------------ vector 3
	inline xmf xm_vec3_dot(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		// Perform the dot product
		xmf vDot = _mm_mul_ps(v1, v2);
		// x=Dot.vector4_f32[1], y=Dot.vector4_f32[2]
		xmf vTemp = xm_shuffle_ps(vDot, vDot, _MM_SHUFFLE(2, 1, 2, 1));
		// Result.vector4_f32[0] = x+y
		vDot = _mm_add_ss(vDot, vTemp);
		// x=Dot.vector4_f32[2]
		vTemp = xm_shuffle_ps(vTemp, vTemp, _MM_SHUFFLE(1, 1, 1, 1));
		// Result.vector4_f32[0] = (x+y)+z
		vDot = _mm_add_ss(vDot, vTemp);
		// Splat x
		return xm_shuffle_ps(vDot, vDot, _MM_SHUFFLE(0, 0, 0, 0));
#else
		float32_t fValue = v1.af[0] * v2.af[0] + v1.af[1] * v2.af[1] + v1.af[2] * v2.af[2];
		return xm_vec_replicate(fValue);
#endif
	}

	inline xmf xm_vec3_cross(xmf v1, xmf v2)
	{
#ifdef VXM_SSE
		// y1,z1,x1,w1
		xmf vTemp1 = xm_permute_ps(v1, _MM_SHUFFLE(3, 0, 2, 1));
		// z2,x2,y2,w2
		xmf vTemp2 = xm_permute_ps(v2, _MM_SHUFFLE(3, 1, 0, 2));
		// Perform the left operation
		xmf result = _mm_mul_ps(vTemp1, vTemp2);
		// z1,x1,y1,w1
		vTemp1 = xm_permute_ps(vTemp1, _MM_SHUFFLE(3, 0, 2, 1));
		// y2,z2,x2,w2
		vTemp2 = xm_permute_ps(vTemp2, _MM_SHUFFLE(3, 1, 0, 2));
		// Perform the right operation
		vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
		// Subract the right from left, and return answer
		result = _mm_sub_ps(result, vTemp1);
		// Set w to zero
		return _mm_and_ps(result, xmf_mask_3);
#else
		xmf result = {
			(v1.af[1] * v2.af[2]) - (v1.af[2] * v2.af[1]),
			(v1.af[2] * v2.af[0]) - (v1.af[0] * v2.af[2]),
			(v1.af[0] * v2.af[1]) - (v1.af[1] * v2.af[0]),
			0.0f
		};
		return result;
#endif
	}

	inline xmf xm_vec3_length_sq(xmf v)
	{
		return xm_vec3_dot(v, v);
	}

	inline xmf xm_vec3_length(xmf v)
	{
		xmf temp = xm_vec3_length_sq(v);
		return xm_vec_sqrt(temp);
	}

	inline xmf xm_vec3_normalize(xmf v)
	{
#ifdef VXM_SSE
		xmf vLengthSq = _mm_mul_ps(v, v);
		xmf vTemp = xm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(2, 1, 2, 1));
		vLengthSq = _mm_add_ss(vLengthSq, vTemp);
		vTemp = xm_shuffle_ps(vTemp, vTemp, _MM_SHUFFLE(1, 1, 1, 1));
		vLengthSq = _mm_add_ss(vLengthSq, vTemp);
		vLengthSq = xm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
		// Prepare for the division
		xmf vResult = _mm_sqrt_ps(vLengthSq);
		// Create zero with a single instruction
		xmf vZeroMask = _mm_setzero_ps();
		// Test for a divide by zero (Must be FP to detect -0.0)
		vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
		// Failsafe on zero (Or epsilon) length planes
		// If the length is infinity, set the elements to zero
		vLengthSq = _mm_cmpneq_ps(vLengthSq, xmf_infinity);
		// Divide to perform the normalization
		vResult = _mm_div_ps(v, vResult);
		// Any that are infinity, set to zero
		vResult = _mm_and_ps(vResult, vZeroMask);
		// Select qnan or result based on infinite length
		xmf vTemp1 = _mm_andnot_ps(vLengthSq, xmf_qnan);
		xmf vTemp2 = _mm_and_ps(vResult, vLengthSq);
		vResult = _mm_or_ps(vTemp1, vTemp2);
		return vResult;
#else
		xmf vResult = xm_vec3_length(v);
		float32_t fLength = vResult.af[0];

		// Prevent divide by zero
		if (fLength > 0)
		{
			fLength = 1.0f / fLength;
		}

		vResult.af[0] = v.af[0] * fLength;
		vResult.af[1] = v.af[1] * fLength;
		vResult.af[2] = v.af[2] * fLength;
		vResult.af[3] = v.af[3] * fLength;
		return vResult;
#endif
	}

	inline xmf xmf_quat_rot_from_angles(xmf angles)
	{
		static const xmf sign = { 1.0f, -1.0f, -1.0f, 1.0f };

		xmf angles_half = xm_vec_mul(angles, xmf_1of2);

		xmf angles_sin, angles_cos;
		xm_vec_sin_cos(&angles_sin, &angles_cos, angles_half);

		xmf P0 = xm_vec_permute<xm_permute_0x, xm_permute_1x, xm_permute_1x, xm_permute_1x>(angles_sin, angles_cos);
		xmf Y0 = xm_vec_permute<xm_permute_1y, xm_permute_0y, xm_permute_1y, xm_permute_1y>(angles_sin, angles_cos);
		xmf R0 = xm_vec_permute<xm_permute_1z, xm_permute_1z, xm_permute_0z, xm_permute_1z>(angles_sin, angles_cos);
		xmf P1 = xm_vec_permute<xm_permute_0x, xm_permute_1x, xm_permute_1x, xm_permute_1x>(angles_cos, angles_sin);
		xmf Y1 = xm_vec_permute<xm_permute_1y, xm_permute_0y, xm_permute_1y, xm_permute_1y>(angles_cos, angles_sin);
		xmf R1 = xm_vec_permute<xm_permute_1z, xm_permute_1z, xm_permute_0z, xm_permute_1z>(angles_cos, angles_sin);

		xmf Q1 = xm_vec_mul(P1, sign);
		xmf Q0 = xm_vec_mul(P0, Y0);
		Q1 = xm_vec_mul(Q1, Y1);
		Q0 = xm_vec_mul(Q0, R0);
		xmf Q = xm_vec_mul_add(Q1, R1, Q0);

		return Q;
	}

}