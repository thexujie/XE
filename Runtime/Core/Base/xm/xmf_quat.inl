#pragma once

namespace XE::xm
{
	// <Pitch, Yaw, Roll, 0>
	inline xmf xm_quat_rotation_from_vec(xmf Angles)
	{
		static const xmf Sign = { 1.0f, -1.0f, -1.0f, 1.0f };

		xmf HalfAngles = xm_vec_mul(Angles, xmf_1of2);

		xmf SinAngles, CosAngles;
		xm_vec_sin_cos(&SinAngles, &CosAngles, HalfAngles);

		xmf P0 = xm_vec_permute<xm_permute_0x, xm_permute_1x, xm_permute_1x, xm_permute_1x>(SinAngles, CosAngles);
		xmf Y0 = xm_vec_permute<xm_permute_1y, xm_permute_0y, xm_permute_1y, xm_permute_1y>(SinAngles, CosAngles);
		xmf R0 = xm_vec_permute<xm_permute_1z, xm_permute_1z, xm_permute_0z, xm_permute_1z>(SinAngles, CosAngles);
		xmf P1 = xm_vec_permute<xm_permute_0x, xm_permute_1x, xm_permute_1x, xm_permute_1x>(CosAngles, SinAngles);
		xmf Y1 = xm_vec_permute<xm_permute_1y, xm_permute_0y, xm_permute_1y, xm_permute_1y>(CosAngles, SinAngles);
		xmf R1 = xm_vec_permute<xm_permute_1z, xm_permute_1z, xm_permute_0z, xm_permute_1z>(CosAngles, SinAngles);

		xmf Q1 = xm_vec_mul(P1, Sign);
		xmf Q0 = xm_vec_mul(P0, Y0);
		Q1 = xm_vec_mul(Q1, Y1);
		Q0 = xm_vec_mul(Q0, R0);
		xmf Q = xm_vec_mul_add(Q1, R1, Q0);

		return Q;
	}


	inline xmf xm_quat_dot(xmf v1, xmf v2)
	{
		return xm_vec_dot(v1, v2);
	}

	inline xmf xm_quat_lerp(xmf min, xmf max, xmf lerp)
	{
		//min = xm_vec_set(0.707053423f, 0.707053781f, -0.00867260713f, 0.00867185090f);
		//max = xm_vec_set(0.687174797f, 0.725747228, -0.0241940953f, 0.0222795364f);
		//lerp = xm_vec_replicate(0.326562494f);
#ifdef VXM_SSE
		const xmf OneMinusEpsilon = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };
		const xmf SignMask2 = xmf(0x80000000u, 0x00000000u, 0x00000000u, 0x00000000u);
		const xmf MaskXY = xmf(0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u);

		xmf CosOmega = xm_vec_dot(min, max);

		xmf Control = _mm_cmplt_ps(CosOmega, xmf_zero);
		xmf Sign = xm_vec_select(xmf_one, xmf_negative_one, Control);

		CosOmega = _mm_mul_ps(CosOmega, Sign);

		Control = _mm_cmplt_ps(CosOmega, OneMinusEpsilon);

		xmf SinOmega = _mm_mul_ps(CosOmega, CosOmega);
		SinOmega = _mm_sub_ps(xmf_one, SinOmega);
		SinOmega = _mm_sqrt_ps(SinOmega);

		xmf Omega = xm_vec_atan2(SinOmega, CosOmega);

		xmf V01 = xm_shuffle_ps(lerp, lerp, _MM_SHUFFLE(2, 3, 0, 1));
		V01 = _mm_and_ps(V01, MaskXY);
		V01 = _mm_xor_ps(V01, SignMask2);
		V01 = _mm_add_ps(xmf_row0, V01);

		xmf S0 = _mm_mul_ps(V01, Omega);
		S0 = xm_vec_sin(S0);
		S0 = _mm_div_ps(S0, SinOmega);

		S0 = xm_vec_select(V01, S0, Control);

		xmf S1 = xm_vec_splat_y(S0);
		S0 = xm_vec_splat_x(S0);

		S1 = _mm_mul_ps(S1, Sign);
		xmf result = _mm_mul_ps(min, S0);
		S1 = _mm_mul_ps(S1, max);
		result = _mm_add_ps(result, S1);
		return result;
#elif defined VXM_DX9
		D3DXQUATERNION quat;
		D3DXQuaternionSlerp(&quat, (const D3DXQUATERNION *)& min, (const D3DXQUATERNION *)& max, lerp.x);
		return *(xmf *)& quat;
#elif defined VXM_XMATH
		XMVECTORF32 xMin = { min.x, min.y, min.z, min.w };
		XMVECTORF32 xMax = { max.x, max.y, max.z, max.w };
		XMVECTORF32 xLerp = { lerp.x, lerp.y, lerp.z, lerp.w };
		XMVECTOR xmmv = XMQuaternionSlerpV2(xMin, xMax, xLerp);
		return *(xmf *)& xmmv;
#else
		const xmf OneMinusEpsilon = { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f };

		xmf CosOmega = xm_quat_dot(min, max);

		xmf Control = xm_vec_less(CosOmega, xmf_zero);
		xmf Sign = xm_vec_select(xmf_one, xmf_negative_one, Control);

		CosOmega = xm_vec_mul(CosOmega, Sign);

		Control = xm_vec_less(CosOmega, OneMinusEpsilon);

		xmf SinOmega = xm_vec_negative_mul_sub(CosOmega, CosOmega, xmf_one);
		SinOmega = xm_vec_sqrt(SinOmega);

		xmf Omega = xm_vec_atan2(SinOmega, CosOmega);

		xmf SignMask = xmf_negative_zero;
		xmf V01 = xm_vec_shift_left(lerp, xmf_zero, 2);
		SignMask = xm_vec_shift_left(SignMask, xmf_zero, 3);
		V01 = xm_vec_xor_int(V01, SignMask);
		V01 = xm_vec_add(xmf_row0, V01);

		xmf InvSinOmega = xm_vec_reciprocal(SinOmega);

		xmf S0 = xm_vec_mul(V01, Omega);
		S0 = xm_vec_sin(S0);
		S0 = xm_vec_mul(S0, InvSinOmega);

		S0 = xm_vec_select(V01, S0, Control);

		xmf S1 = xm_vec_splat_y(S0);
		S0 = xm_vec_splat_x(S0);

		S1 = xm_vec_mul(S1, Sign);

		xmf Result = xm_vec_mul(min, S0);
		Result = xm_vec_mul_add(max, S1, Result);

		return Result;
#endif
	}

	inline xmf xm_quat_lerp(xmf min, xmf max, float32_t lerp)
	{
		return xm_quat_lerp(min, max, xm_vec_replicate(lerp));
	}
}