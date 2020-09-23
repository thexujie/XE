#pragma once

namespace XE::xm
{
	//! ×ªÖÃ
	inline xmm xm_matr_transpose(const xmm & matrix)
	{
#ifdef VXM_SSE
		// x.x,x.y,y.x,y.y
		xmf vTemp1 = xm_shuffle_ps(matrix.row0, matrix.row1, _MM_SHUFFLE(1, 0, 1, 0));
		// x.z,x.w,y.z,y.w
		xmf vTemp3 = xm_shuffle_ps(matrix.row0, matrix.row1, _MM_SHUFFLE(3, 2, 3, 2));
		// z.x,z.y,w.x,w.y
		xmf vTemp2 = xm_shuffle_ps(matrix.row2, matrix.row3, _MM_SHUFFLE(1, 0, 1, 0));
		// z.z,z.w,w.z,w.w
		xmf vTemp4 = xm_shuffle_ps(matrix.row2, matrix.row3, _MM_SHUFFLE(3, 2, 3, 2));

		xmm result;
		// x.x,y.x,z.x,w.x
		result[0] = xm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));
		// x.y,y.y,z.y,w.y
		result[1] = xm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));
		// x.z,y.z,z.z,w.z
		result[2] = xm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));
		// x.w,y.w,z.w,w.w
		result[3] = xm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(3, 1, 3, 1));
		return result;
#else
		xmf row0 = matrix[0];
		xmf row1 = matrix[1];
		xmf row2 = matrix[2];
		xmf row3 = matrix[3];

		xmm result;
		result[0] = { row0[0], row1[0], row2[0], row3[0] };
		result[1] = { row0[1], row1[1], row2[1], row3[1] };
		result[2] = { row0[2], row1[2], row2[2], row3[2] };
		result[3] = { row0[3], row1[3], row2[3], row3[3] };
		return result;
#endif
	}

	//! ³Ë·¨
	inline xmm xm_matr_mul(const xmm & matr1, const xmm & matr2)
	{
		xmm result;
#ifdef VXM_SSE
		xmf row10 = matr1[0];
		xmf row11 = matr1[1];
		xmf row12 = matr1[2];
		xmf row13 = matr1[3];

		xmf row20 = matr2[0];
		xmf row21 = matr2[1];
		xmf row22 = matr2[2];
		xmf row23 = matr2[3];

		xmf vX = _mm_shuffle_ps(row10, row10, _MM_SHUFFLE(0, 0, 0, 0));
		xmf vY = _mm_shuffle_ps(row10, row10, _MM_SHUFFLE(1, 1, 1, 1));
		xmf vZ = _mm_shuffle_ps(row10, row10, _MM_SHUFFLE(2, 2, 2, 2));
		xmf vW = _mm_shuffle_ps(row10, row10, _MM_SHUFFLE(3, 3, 3, 3));
		vX = _mm_mul_ps(vX, row20);
		vY = _mm_mul_ps(vY, row21);
		vZ = _mm_mul_ps(vZ, row22);
		vW = _mm_mul_ps(vW, row23);
		vX = _mm_add_ps(vX, vZ);
		vY = _mm_add_ps(vY, vW);
		vX = _mm_add_ps(vX, vY);
		result[0] = vX;

		vX = _mm_shuffle_ps(row11, row11, _MM_SHUFFLE(0, 0, 0, 0));
		vY = _mm_shuffle_ps(row11, row11, _MM_SHUFFLE(1, 1, 1, 1));
		vZ = _mm_shuffle_ps(row11, row11, _MM_SHUFFLE(2, 2, 2, 2));
		vW = _mm_shuffle_ps(row11, row11, _MM_SHUFFLE(3, 3, 3, 3));
		vX = _mm_mul_ps(vX, row20);
		vY = _mm_mul_ps(vY, row21);
		vZ = _mm_mul_ps(vZ, row22);
		vW = _mm_mul_ps(vW, row23);
		vX = _mm_add_ps(vX, vZ);
		vY = _mm_add_ps(vY, vW);
		vX = _mm_add_ps(vX, vY);
		result[1] = vX;

		vX = _mm_shuffle_ps(row12, row12, _MM_SHUFFLE(0, 0, 0, 0));
		vY = _mm_shuffle_ps(row12, row12, _MM_SHUFFLE(1, 1, 1, 1));
		vZ = _mm_shuffle_ps(row12, row12, _MM_SHUFFLE(2, 2, 2, 2));
		vW = _mm_shuffle_ps(row12, row12, _MM_SHUFFLE(3, 3, 3, 3));
		vX = _mm_mul_ps(vX, row20);
		vY = _mm_mul_ps(vY, row21);
		vZ = _mm_mul_ps(vZ, row22);
		vW = _mm_mul_ps(vW, row23);
		vX = _mm_add_ps(vX, vZ);
		vY = _mm_add_ps(vY, vW);
		vX = _mm_add_ps(vX, vY);
		result[2] = vX;

		vX = _mm_shuffle_ps(row13, row13, _MM_SHUFFLE(0, 0, 0, 0));
		vY = _mm_shuffle_ps(row13, row13, _MM_SHUFFLE(1, 1, 1, 1));
		vZ = _mm_shuffle_ps(row13, row13, _MM_SHUFFLE(2, 2, 2, 2));
		vW = _mm_shuffle_ps(row13, row13, _MM_SHUFFLE(3, 3, 3, 3));
		vX = _mm_mul_ps(vX, row20);
		vY = _mm_mul_ps(vY, row21);
		vZ = _mm_mul_ps(vZ, row22);
		vW = _mm_mul_ps(vW, row23);
		vX = _mm_add_ps(vX, vZ);
		vY = _mm_add_ps(vY, vW);
		vX = _mm_add_ps(vX, vY);
		result[3] = vX;

#elif defined VXM_DX9
		D3DXMatrixMultiply((D3DXMATRIX *)& result, (const D3DXMATRIX *)& matr1, (const D3DXMATRIX *)& matr2);
#else
		float32_t x = matr1[0][0];
		float32_t y = matr1[0][1];
		float32_t z = matr1[0][2];
		float32_t w = matr1[0][3];
		result[0][0] = (matr2[0][0] * x) + (matr2[1][0] * y) + (matr2[2][0] * z) + (matr2[3][0] * w);
		result[0][1] = (matr2[0][1] * x) + (matr2[1][1] * y) + (matr2[2][1] * z) + (matr2[3][1] * w);
		result[0][2] = (matr2[0][2] * x) + (matr2[1][2] * y) + (matr2[2][2] * z) + (matr2[3][2] * w);
		result[0][3] = (matr2[0][3] * x) + (matr2[1][3] * y) + (matr2[2][3] * z) + (matr2[3][3] * w);

		x = matr1[1][0];
		y = matr1[1][1];
		z = matr1[1][2];
		w = matr1[1][3];
		result[1][0] = (matr2[0][0] * x) + (matr2[1][0] * y) + (matr2[2][0] * z) + (matr2[3][0] * w);
		result[1][1] = (matr2[0][1] * x) + (matr2[1][1] * y) + (matr2[2][1] * z) + (matr2[3][1] * w);
		result[1][2] = (matr2[0][2] * x) + (matr2[1][2] * y) + (matr2[2][2] * z) + (matr2[3][2] * w);
		result[1][3] = (matr2[0][3] * x) + (matr2[1][3] * y) + (matr2[2][3] * z) + (matr2[3][3] * w);

		x = matr1[2][0];
		y = matr1[2][1];
		z = matr1[2][2];
		w = matr1[2][3];
		result[2][0] = (matr2[0][0] * x) + (matr2[1][0] * y) + (matr2[2][0] * z) + (matr2[3][0] * w);
		result[2][1] = (matr2[0][1] * x) + (matr2[1][1] * y) + (matr2[2][1] * z) + (matr2[3][1] * w);
		result[2][2] = (matr2[0][2] * x) + (matr2[1][2] * y) + (matr2[2][2] * z) + (matr2[3][2] * w);
		result[2][3] = (matr2[0][3] * x) + (matr2[1][3] * y) + (matr2[2][3] * z) + (matr2[3][3] * w);

		x = matr1[3][0];
		y = matr1[3][1];
		z = matr1[3][2];
		w = matr1[3][3];
		result[3][0] = (matr2[0][0] * x) + (matr2[1][0] * y) + (matr2[2][0] * z) + (matr2[3][0] * w);
		result[3][1] = (matr2[0][1] * x) + (matr2[1][1] * y) + (matr2[2][1] * z) + (matr2[3][1] * w);
		result[3][2] = (matr2[0][2] * x) + (matr2[1][2] * y) + (matr2[2][2] * z) + (matr2[3][2] * w);
		result[3][3] = (matr2[0][3] * x) + (matr2[1][3] * y) + (matr2[2][3] * z) + (matr2[3][3] * w);
#endif
		return result;
	}

	//! ÇóÄæ¾ØÕó
	inline xmm xm_matr_invert(const xmm & matrix, xmf * pDeterminant)
	{
#ifdef VXM_SSE
		xmm temp = xm_matr_transpose(matrix);

		xmf row0 = temp[0];
		xmf row1 = temp[1];
		xmf row2 = temp[2];
		xmf row3 = temp[3];

		xmf V00 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 1, 0, 0));
		xmf V10 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(3, 2, 3, 2));
		xmf V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 1, 0, 0));
		xmf V11 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 2, 3, 2));
		xmf V02 = _mm_shuffle_ps(row2, row0, _MM_SHUFFLE(2, 0, 2, 0));
		xmf V12 = _mm_shuffle_ps(row3, row1, _MM_SHUFFLE(3, 1, 3, 1));

		xmf D0 = _mm_mul_ps(V00, V10);
		xmf D1 = _mm_mul_ps(V01, V11);
		xmf D2 = _mm_mul_ps(V02, V12);

		V00 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 2, 3, 2));
		V10 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(1, 1, 0, 0));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 2, 3, 2));
		V11 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 1, 0, 0));
		V02 = _mm_shuffle_ps(row2, row0, _MM_SHUFFLE(3, 1, 3, 1));
		V12 = _mm_shuffle_ps(row3, row1, _MM_SHUFFLE(2, 0, 2, 0));

		V00 = _mm_mul_ps(V00, V10);
		V01 = _mm_mul_ps(V01, V11);
		V02 = _mm_mul_ps(V02, V12);
		D0 = _mm_sub_ps(D0, V00);
		D1 = _mm_sub_ps(D1, V01);
		D2 = _mm_sub_ps(D2, V02);
		// V11 = D0Y,D0W,D2Y,D2Y
		V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
		V00 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 0, 2, 1));
		V10 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(0, 3, 0, 2));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(0, 1, 0, 2));
		V11 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(2, 1, 2, 1));
		// V13 = D1Y,D1W,D2W,D2W
		xmf V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
		V02 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(1, 0, 2, 1));
		V12 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(0, 3, 0, 2));
		xmf V03 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(0, 1, 0, 2));
		V13 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(2, 1, 2, 1));

		xmf C0 = _mm_mul_ps(V00, V10);
		xmf C2 = _mm_mul_ps(V01, V11);
		xmf C4 = _mm_mul_ps(V02, V12);
		xmf C6 = _mm_mul_ps(V03, V13);

		// V11 = D0X,D0Y,D2X,D2X
		V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
		V00 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(2, 1, 3, 2));
		V10 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(2, 1, 0, 3));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 3, 2, 3));
		V11 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(0, 2, 1, 2));
		// V13 = D1X,D1Y,D2Z,D2Z
		V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
		V02 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(2, 1, 3, 2));
		V12 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(2, 1, 0, 3));
		V03 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 3, 2, 3));
		V13 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(0, 2, 1, 2));

		V00 = _mm_mul_ps(V00, V10);
		V01 = _mm_mul_ps(V01, V11);
		V02 = _mm_mul_ps(V02, V12);
		V03 = _mm_mul_ps(V03, V13);
		C0 = _mm_sub_ps(C0, V00);
		C2 = _mm_sub_ps(C2, V01);
		C4 = _mm_sub_ps(C4, V02);
		C6 = _mm_sub_ps(C6, V03);

		V00 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(0, 3, 0, 3));
		// V10 = D0Z,D0Z,D2X,D2Y
		V10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
		V10 = _mm_shuffle_ps(V10, V10, _MM_SHUFFLE(0, 2, 3, 0));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(2, 0, 3, 1));
		// V11 = D0X,D0W,D2X,D2Y
		V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
		V11 = _mm_shuffle_ps(V11, V11, _MM_SHUFFLE(2, 1, 0, 3));
		V02 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(0, 3, 0, 3));
		// V12 = D1Z,D1Z,D2Z,D2W
		V12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
		V12 = _mm_shuffle_ps(V12, V12, _MM_SHUFFLE(0, 2, 3, 0));
		V03 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 0, 3, 1));
		// V13 = D1X,D1W,D2Z,D2W
		V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
		V13 = _mm_shuffle_ps(V13, V13, _MM_SHUFFLE(2, 1, 0, 3));

		V00 = _mm_mul_ps(V00, V10);
		V01 = _mm_mul_ps(V01, V11);
		V02 = _mm_mul_ps(V02, V12);
		V03 = _mm_mul_ps(V03, V13);
		xmf C1 = _mm_sub_ps(C0, V00);
		C0 = _mm_add_ps(C0, V00);
		xmf C3 = _mm_add_ps(C2, V01);
		C2 = _mm_sub_ps(C2, V01);
		xmf C5 = _mm_sub_ps(C4, V02);
		C4 = _mm_add_ps(C4, V02);
		xmf C7 = _mm_add_ps(C6, V03);
		C6 = _mm_sub_ps(C6, V03);

		C0 = _mm_shuffle_ps(C0, C1, _MM_SHUFFLE(3, 1, 2, 0));
		C2 = _mm_shuffle_ps(C2, C3, _MM_SHUFFLE(3, 1, 2, 0));
		C4 = _mm_shuffle_ps(C4, C5, _MM_SHUFFLE(3, 1, 2, 0));
		C6 = _mm_shuffle_ps(C6, C7, _MM_SHUFFLE(3, 1, 2, 0));
		C0 = _mm_shuffle_ps(C0, C0, _MM_SHUFFLE(3, 1, 2, 0));
		C2 = _mm_shuffle_ps(C2, C2, _MM_SHUFFLE(3, 1, 2, 0));
		C4 = _mm_shuffle_ps(C4, C4, _MM_SHUFFLE(3, 1, 2, 0));
		C6 = _mm_shuffle_ps(C6, C6, _MM_SHUFFLE(3, 1, 2, 0));
		// Get the determinate
		xmf vTemp = xm_vec_dot(C0, row0);
		if (pDeterminant != nullptr)
			* pDeterminant = vTemp;
		vTemp = _mm_div_ps(xmf_one, vTemp);

		xmm result;
		result[0] = _mm_mul_ps(C0, vTemp);
		result[1] = _mm_mul_ps(C2, vTemp);
		result[2] = _mm_mul_ps(C4, vTemp);
		result[3] = _mm_mul_ps(C6, vTemp);
		return result;
#elif defined VXM_DX9
		D3DXMatrixInverse((D3DXMATRIX *)& result, nullptr, (const D3DXMATRIX *)& matrix);
#else

		xmm MT = xm_matr_transpose(matrix);

		xmf V0[4], V1[4];
		V0[0] = xm_vec_swizzle<xm_swizzle_x, xm_swizzle_x, xm_swizzle_y, xm_swizzle_y>(MT[2]);
		V1[0] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_w, xm_swizzle_z, xm_swizzle_w>(MT[3]);
		V0[1] = xm_vec_swizzle<xm_swizzle_x, xm_swizzle_x, xm_swizzle_y, xm_swizzle_y>(MT[0]);
		V1[1] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_w, xm_swizzle_z, xm_swizzle_w>(MT[1]);
		V0[2] = xm_vec_permute<xm_permute_0x, xm_permute_0z, xm_permute_1x, xm_permute_1z>(MT[2], MT[0]);
		V1[2] = xm_vec_permute<xm_permute_0y, xm_permute_0w, xm_permute_1y, xm_permute_1w>(MT[3], MT[1]);

		xmf D0 = xm_vec_mul(V0[0], V1[0]);
		xmf D1 = xm_vec_mul(V0[1], V1[1]);
		xmf D2 = xm_vec_mul(V0[2], V1[2]);

		V0[0] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_w, xm_swizzle_z, xm_swizzle_w>(MT[2]);
		V1[0] = xm_vec_swizzle<xm_swizzle_x, xm_swizzle_x, xm_swizzle_y, xm_swizzle_y>(MT[3]);
		V0[1] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_w, xm_swizzle_z, xm_swizzle_w>(MT[0]);
		V1[1] = xm_vec_swizzle<xm_swizzle_x, xm_swizzle_x, xm_swizzle_y, xm_swizzle_y>(MT[1]);
		V0[2] = xm_vec_permute<xm_permute_0y, xm_permute_0w, xm_permute_1y, xm_permute_1w>(MT[2], MT[0]);
		V1[2] = xm_vec_permute<xm_permute_0x, xm_permute_0z, xm_permute_1x, xm_permute_1z>(MT[3], MT[1]);

		D0 = xm_vec_negative_mul_sub(V0[0], V1[0], D0);
		D1 = xm_vec_negative_mul_sub(V0[1], V1[1], D1);
		D2 = xm_vec_negative_mul_sub(V0[2], V1[2], D2);

		V0[0] = xm_vec_swizzle<xm_swizzle_y, xm_swizzle_z, xm_swizzle_x, xm_swizzle_y>(MT[1]);
		V1[0] = xm_vec_permute<xm_permute_1y, xm_permute_0y, xm_permute_0w, xm_permute_0x>(D0, D2);
		V0[1] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_x, xm_swizzle_y, xm_swizzle_x>(MT[0]);
		V1[1] = xm_vec_permute<xm_permute_0w, xm_permute_1y, xm_permute_0y, xm_permute_0z>(D0, D2);
		V0[2] = xm_vec_swizzle<xm_swizzle_y, xm_swizzle_z, xm_swizzle_x, xm_swizzle_y>(MT[3]);
		V1[2] = xm_vec_permute<xm_permute_1w, xm_permute_0y, xm_permute_0w, xm_permute_0x>(D1, D2);
		V0[3] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_x, xm_swizzle_y, xm_swizzle_x>(MT[2]);
		V1[3] = xm_vec_permute<xm_permute_0w, xm_permute_1w, xm_permute_0y, xm_permute_0z>(D1, D2);

		xmf C0 = xm_vec_mul(V0[0], V1[0]);
		xmf C2 = xm_vec_mul(V0[1], V1[1]);
		xmf C4 = xm_vec_mul(V0[2], V1[2]);
		xmf C6 = xm_vec_mul(V0[3], V1[3]);

		V0[0] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_w, xm_swizzle_y, xm_swizzle_z>(MT[1]);
		V1[0] = xm_vec_permute<xm_permute_0w, xm_permute_0x, xm_permute_0y, xm_permute_1x>(D0, D2);
		V0[1] = xm_vec_swizzle<xm_swizzle_w, xm_swizzle_z, xm_swizzle_w, xm_swizzle_y>(MT[0]);
		V1[1] = xm_vec_permute<xm_permute_0z, xm_permute_0y, xm_permute_1x, xm_permute_0x>(D0, D2);
		V0[2] = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_w, xm_swizzle_y, xm_swizzle_z>(MT[3]);
		V1[2] = xm_vec_permute<xm_permute_0w, xm_permute_0x, xm_permute_0y, xm_permute_1z>(D1, D2);
		V0[3] = xm_vec_swizzle<xm_swizzle_w, xm_swizzle_z, xm_swizzle_w, xm_swizzle_y>(MT[2]);
		V1[3] = xm_vec_permute<xm_permute_0z, xm_permute_0y, xm_permute_1z, xm_permute_0x>(D1, D2);

		C0 = xm_vec_negative_mul_sub(V0[0], V1[0], C0);
		C2 = xm_vec_negative_mul_sub(V0[1], V1[1], C2);
		C4 = xm_vec_negative_mul_sub(V0[2], V1[2], C4);
		C6 = xm_vec_negative_mul_sub(V0[3], V1[3], C6);

		V0[0] = xm_vec_swizzle<xm_swizzle_w, xm_swizzle_x, xm_swizzle_w, xm_swizzle_x>(MT[1]);
		V1[0] = xm_vec_permute<xm_permute_0z, xm_permute_1y, xm_permute_1x, xm_permute_0z>(D0, D2);
		V0[1] = xm_vec_swizzle<xm_swizzle_y, xm_swizzle_w, xm_swizzle_x, xm_swizzle_z>(MT[0]);
		V1[1] = xm_vec_permute<xm_permute_1y, xm_permute_0x, xm_permute_0w, xm_permute_1x>(D0, D2);
		V0[2] = xm_vec_swizzle<xm_swizzle_w, xm_swizzle_x, xm_swizzle_w, xm_swizzle_x>(MT[3]);
		V1[2] = xm_vec_permute<xm_permute_0z, xm_permute_1w, xm_permute_1z, xm_permute_0z>(D1, D2);
		V0[3] = xm_vec_swizzle<xm_swizzle_y, xm_swizzle_w, xm_swizzle_x, xm_swizzle_z>(MT[2]);
		V1[3] = xm_vec_permute<xm_permute_1w, xm_permute_0x, xm_permute_0w, xm_permute_1z>(D1, D2);

		xmf C1 = xm_vec_negative_mul_sub(V0[0], V1[0], C0);
		C0 = xm_vec_mul_add(V0[0], V1[0], C0);
		xmf C3 = xm_vec_mul_add(V0[1], V1[1], C2);
		C2 = xm_vec_negative_mul_sub(V0[1], V1[1], C2);
		xmf C5 = xm_vec_negative_mul_sub(V0[2], V1[2], C4);
		C4 = xm_vec_mul_add(V0[2], V1[2], C4);
		xmf C7 = xm_vec_mul_add(V0[3], V1[3], C6);
		C6 = xm_vec_negative_mul_sub(V0[3], V1[3], C6);

		xmm R;
		R[0] = xm_vec_select(C0, C1, xmf_select_0101);
		R[1] = xm_vec_select(C2, C3, xmf_select_0101);
		R[2] = xm_vec_select(C4, C5, xmf_select_0101);
		R[3] = xm_vec_select(C6, C7, xmf_select_0101);

		xmf Determinant = xm_vec_dot(R[0], MT[0]);

		if (pDeterminant != nullptr)
			* pDeterminant = Determinant;

		xmf Reciprocal = xm_vec_reciprocal(Determinant);

		xmm Result;
		Result[0] = xm_vec_mul(R[0], Reciprocal);
		Result[1] = xm_vec_mul(R[1], Reciprocal);
		Result[2] = xm_vec_mul(R[2], Reciprocal);
		Result[3] = xm_vec_mul(R[3], Reciprocal);
		return Result;
#endif
	}

	//! Æ½ÒÆ
	inline xmm xm_matr_translate(const xmf & vec)
	{
		xmm result;
		result[0] = xmf_row0;
		result[1] = xmf_row1;
		result[2] = xmf_row2;
		//result[3] = xmvf(vec.x, vec.y, vec.z, 1.0f);
		result[3] = xm_vec_select(xmf_row3, vec, xmf_select_1110);
		return result;
	}

	inline xmm xm_matr_rotate_quat(const xmf & quat)
	{
#ifdef VXM_SSE
		// xmm the = _mm_set_ps(quat.x, quat.y, quat.z, quat.w);
		xmf Q0 = _mm_add_ps(quat, quat);
		xmf Q1 = _mm_mul_ps(quat, Q0);

		xmf V0 = xm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(3, 0, 0, 1));
		V0 = _mm_and_ps(V0, xmf_mask_3);
		xmf V1 = xm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(3, 1, 2, 2));
		V1 = _mm_and_ps(V1, xmf_mask_3);
		xmf R0 = _mm_sub_ps(xmf_1110, V0);
		R0 = _mm_sub_ps(R0, V1);

		V0 = xm_shuffle_ps(quat, quat, _MM_SHUFFLE(3, 1, 0, 0));
		V1 = xm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(3, 2, 1, 2));
		V0 = _mm_mul_ps(V0, V1);

		V1 = xm_shuffle_ps(quat, quat, _MM_SHUFFLE(3, 3, 3, 3));
		xmf V2 = xm_shuffle_ps(Q0, Q0, _MM_SHUFFLE(3, 0, 2, 1));
		V1 = _mm_mul_ps(V1, V2);

		xmf R1 = _mm_add_ps(V0, V1);
		xmf R2 = _mm_sub_ps(V0, V1);

		V0 = xm_shuffle_ps(R1, R2, _MM_SHUFFLE(1, 0, 2, 1));
		V0 = xm_shuffle_ps(V0, V0, _MM_SHUFFLE(1, 3, 2, 0));
		V1 = xm_shuffle_ps(R1, R2, _MM_SHUFFLE(2, 2, 0, 0));
		V1 = xm_shuffle_ps(V1, V1, _MM_SHUFFLE(2, 0, 2, 0));

		Q1 = xm_shuffle_ps(R0, V0, _MM_SHUFFLE(1, 0, 3, 0));
		Q1 = xm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(1, 3, 2, 0));

		xmm result;
		result[0] = Q1;

		Q1 = xm_shuffle_ps(R0, V0, _MM_SHUFFLE(3, 2, 3, 1));
		Q1 = xm_shuffle_ps(Q1, Q1, _MM_SHUFFLE(1, 3, 0, 2));
		result[1] = Q1;

		Q1 = xm_shuffle_ps(V1, R0, _MM_SHUFFLE(3, 2, 1, 0));
		result[2] = Q1;
		result[3] = xmf_row3;
		return result;
#elif defined VXM_DX9
		D3DXMatrixRotationQuaternion((D3DXMATRIX *)& result, (const D3DXQUATERNION *)& quat);
#else
		xmf Q0 = xm_vec_add(quat, quat);
		xmf Q1 = xm_vec_mul(quat, Q0);

		xmf V0 = xm_vec_permute<xm_permute_0y, xm_permute_0x, xm_permute_0x, xm_permute_1w>(Q1, xmf_1110);
		xmf V1 = xm_vec_permute<xm_permute_0z, xm_permute_0z, xm_permute_0y, xm_permute_1w>(Q1, xmf_1110);
		xmf R0 = xm_vec_sub(xmf_1110, V0);
		R0 = xm_vec_sub(R0, V1);

		V0 = xm_vec_swizzle<xm_swizzle_x, xm_swizzle_x, xm_swizzle_y, xm_swizzle_w>(quat);
		V1 = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_y, xm_swizzle_z, xm_swizzle_w>(Q0);
		V0 = xm_vec_mul(V0, V1);

		V1 = xm_vec_splat_w(quat);
		xmf V2 = xm_vec_swizzle<xm_swizzle_y, xm_swizzle_z, xm_swizzle_x, xm_swizzle_w>(Q0);
		V1 = xm_vec_mul(V1, V2);

		xmf R1 = xm_vec_add(V0, V1);
		xmf R2 = xm_vec_sub(V0, V1);

		V0 = xm_vec_permute<xm_permute_0y, xm_permute_1x, xm_permute_1y, xm_permute_0z>(R1, R2);
		V1 = xm_vec_permute<xm_permute_0x, xm_permute_1z, xm_permute_0x, xm_permute_1z>(R1, R2);

		xmm M;
		M[0] = xm_vec_permute<xm_permute_0x, xm_permute_1x, xm_permute_1y, xm_permute_0w>(R0, V0);
		M[1] = xm_vec_permute<xm_permute_1z, xm_permute_0y, xm_permute_1w, xm_permute_0w>(R0, V0);
		M[2] = xm_vec_permute<xm_permute_1x, xm_permute_1y, xm_permute_0z, xm_permute_0w>(R0, V1);
		M[3] = xmf_row3;
		return M;
#endif
	}

	// Ðý×ª
	inline xmm xm_matr_rotate(const xmf & rot)
	{
#ifdef VXM_SSE
		xmf quat = xm_quat_rotation_from_vec(rot);
		return xm_matr_rotate_quat(quat);
#else
		float32_t cr = float_cos(rot.x);
		float32_t sr = float_sin(rot.x);
		float32_t cp = float_cos(rot.y);
		float32_t sp = float_sin(rot.y);
		float32_t cy = float_cos(rot.z);
		float32_t sy = float_sin(rot.z);

		float32_t srsp = sr * sp;
		float32_t crsp = cr * sp;

		xmm result;
		result[0] = xm_vec_set(cp * cy, cp * sy, -sp, 0.0f);
		result[1] = xm_vec_set(srsp * cy - cr * sy, srsp * sy + cr * cy, sr * cp, 0.0f);
		result[2] = xm_vec_set(crsp * cy + sr * sy, crsp * sy - sr * cy, cr * cp, 0.0f);
		result[3] = xmf_row3;
		return result;
#endif
	}


	// ÈÆ¹Ì¶¨ÖáÐý×ª
	inline xmm xm_matr_rotate_axis_normal(const xmf & axis, float32_t angle)
	{
#ifdef VXM_SSE
		float32_t fSinAngle;
		float32_t fCosAngle;
		xm_scalar_sin_cos(&fSinAngle, &fCosAngle, angle);

		xmf C2 = _mm_set_ps1(1.0f - fCosAngle);
		xmf C1 = _mm_set_ps1(fCosAngle);
		xmf C0 = _mm_set_ps1(fSinAngle);

		xmf N0 = xm_permute_ps(axis, _MM_SHUFFLE(3, 0, 2, 1));
		xmf N1 = xm_permute_ps(axis, _MM_SHUFFLE(3, 1, 0, 2));

		xmf V0 = _mm_mul_ps(C2, N0);
		V0 = _mm_mul_ps(V0, N1);

		xmf R0 = _mm_mul_ps(C2, axis);
		R0 = _mm_mul_ps(R0, axis);
		R0 = _mm_add_ps(R0, C1);

		xmf R1 = _mm_mul_ps(C0, axis);
		R1 = _mm_add_ps(R1, V0);
		xmf R2 = _mm_mul_ps(C0, axis);
		R2 = _mm_sub_ps(V0, R2);

		V0 = _mm_and_ps(R0, xmf_mask_3);
		xmf V1 = xm_shuffle_ps(R1, R2, _MM_SHUFFLE(2, 1, 2, 0));
		V1 = xm_permute_ps(V1, _MM_SHUFFLE(0, 3, 2, 1));
		xmf V2 = xm_shuffle_ps(R1, R2, _MM_SHUFFLE(0, 0, 1, 1));
		V2 = xm_permute_ps(V2, _MM_SHUFFLE(2, 0, 2, 0));

		R2 = xm_shuffle_ps(V0, V1, _MM_SHUFFLE(1, 0, 3, 0));
		R2 = xm_permute_ps(R2, _MM_SHUFFLE(1, 3, 2, 0));

		xmm result;
		result[0] = R2;

		R2 = xm_shuffle_ps(V0, V1, _MM_SHUFFLE(3, 2, 3, 1));
		R2 = xm_permute_ps(R2, _MM_SHUFFLE(1, 3, 0, 2));
		result[1] = R2;

		V2 = xm_shuffle_ps(V2, V0, _MM_SHUFFLE(3, 2, 1, 0));
		result[2] = V2;
		result[3] = xmf_row3;
		return result;
#else
		float32_t fSinAngle;
		float32_t fCosAngle;
		xm_scalar_sin_cos(&fSinAngle, &fCosAngle, angle);

		xmf A = xm_vec_set(fSinAngle, fCosAngle, 1.0f - fCosAngle, 0.0f);

		xmf C2 = xm_vec_splat_z(A);
		xmf C1 = xm_vec_splat_y(A);
		xmf C0 = xm_vec_splat_x(A);

		xmf N0 = xm_vec_swizzle<xm_swizzle_y, xm_swizzle_z, xm_swizzle_x, xm_swizzle_w>(axis);
		xmf N1 = xm_vec_swizzle<xm_swizzle_z, xm_swizzle_x, xm_swizzle_y, xm_swizzle_w>(axis);

		xmf V0 = xm_vec_mul(C2, N0);
		V0 = xm_vec_mul(V0, N1);

		xmf R0 = xm_vec_mul(C2, axis);
		R0 = xm_vec_mul_add(R0, axis, C1);

		xmf R1 = xm_vec_mul_add(C0, axis, V0);
		xmf R2 = xm_vec_negative_mul_sub(C0, axis, V0);

		V0 = xm_vec_select(A, R0, xmf_select_1110);
		xmf V1 = xm_vec_permute<xm_permute_0z, xm_permute_1y, xm_permute_1z, xm_permute_0x>(R1, R2);
		xmf V2 = xm_vec_permute<xm_permute_0y, xm_permute_1x, xm_permute_0y, xm_permute_1x>(R1, R2);

		xmm M;
		M[0] = xm_vec_permute<xm_permute_0x, xm_permute_1x, xm_permute_1y, xm_permute_0w>(V0, V1);
		M[1] = xm_vec_permute<xm_permute_1z, xm_permute_0y, xm_permute_1w, xm_permute_0w>(V0, V1);
		M[2] = xm_vec_permute<xm_permute_1x, xm_permute_1y, xm_permute_0z, xm_permute_0w>(V0, V2);
		M[3] = xmf_row3;
		return M;
#endif
	}

	inline xmm xm_matr_rotate_axis(const xmf & axis, float32_t angle)
	{
#ifdef VXM_SSE
		xmf Normal = xm_vec3_normalize(axis);
		return xm_matr_rotate_axis_normal(Normal, angle);
#else
		float32_t c = float_cos(angle);
		float32_t s = float_sin(angle);
		float32_t t = 1 - c;

		float32_t tx = t * axis.x;
		float32_t ty = t * axis.y;
		float32_t tz = t * axis.z;

		float32_t sx = s * axis.x;
		float32_t sy = s * axis.y;
		float32_t sz = s * axis.z;

		xmm result;
		result[0] = xm_vec_set(tx * axis.x + c, tx * axis.y + sz, tx * axis.z - sy, 0.0f);
		result[1] = xm_vec_set(ty * axis.x - sz, ty * axis.y + c, ty * axis.z + sx, 0.0f);
		result[2] = xm_vec_set(tz * axis.x + sy, tz * axis.y - sx, tz * axis.z + c, 0.0f);
		result[3] = xmf_row3;
		return result;
#endif
	}

	inline xmm xm_matr_scale(const xmf & scl)
	{
#ifdef VXM_SSE
		xmm result;
		result[0] = _mm_and_ps(scl, xmf_mask_x);
		result[1] = _mm_and_ps(scl, xmf_mask_y);
		result[2] = _mm_and_ps(scl, xmf_mask_z);
		result[3] = xmf_row3;
		return result;
#elif defined VXM_DX9
		D3DXMatrixScaling((D3DXMATRIX *)& result, scl.x, scl.y, scl.z);
#else
		xmm result;
		result[0] = xm_vec_set(scl.x, 0.0f, 0.0f, 0.0f);
		result[1] = xm_vec_set(0.0f, scl.y, 0.0f, 0.0f);
		result[2] = xm_vec_set(0.0f, 0.0f, scl.z, 0.0f);
		result[3] = xmf_row3;
		return result;
#endif
	}

	inline xmm xmm_matr_perspective_lh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
	{
#ifdef VXM_SSE
		float32_t SinFov;
		float32_t CosFov;

		xm_scalar_sin_cos(&SinFov, &CosFov, 0.5f * fovy);

		float32_t fRange = z_far / (z_far - z_near);
		// Note: This is recorded on the stack
		float32_t fHeight = CosFov / SinFov;
		// Copy from memory to SSE register
		xmf vValues = { fHeight / aspect, fHeight, fRange, -fRange * z_near };
		xmf vTemp = _mm_setzero_ps();
		// Copy x only
		vTemp = _mm_move_ss(vTemp, vValues);

		xmm result;
		// CosFov / SinFov,0,0,0
		result[0] = vTemp;
		// 0,Height / aspect,0,0
		vTemp = vValues;
		vTemp = _mm_and_ps(vTemp, xmf_mask_y);
		result[1] = vTemp;
		// x=fRange,y=-fRange * NearZ,0,1.0f
		vTemp = _mm_setzero_ps();
		vValues = xm_shuffle_ps(vValues, xmf_row3, _MM_SHUFFLE(3, 2, 3, 2));
		// 0,0,fRange,1.0f
		vTemp = xm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
		result[2] = vTemp;
		// 0,0,-fRange * NearZ,0.0f
		vTemp = xm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
		result[3] = vTemp;
		return result;
#elif defined VXM_XMATH
		XMMATRIX matrix = XMMatrixPerspectiveFovLH(fovy, aspect, z_near, z_far);
		result = *(float4x4 *)& matrix;
#else
		xmm result;
		float32_t h = 1 / (float_tan(fovy * 0.5f));
		float32_t w = h / aspect;
		float32_t z = z_far / (z_far - z_near);
		float32_t f = -z_near * z_far / (z_far - z_near);

		result[0] = xm_vec_set(w, 0, 0, 0);
		result[1] = xm_vec_set(0, h, 0, 0);
		result[2] = xm_vec_set(0, 0, z, 1);
		result[3] = xm_vec_set(0, 0, f, 0);
		return result;
#endif
	}

	inline xmm xmm_matr_perspective_rh(float32_t fovy, float32_t aspect, float32_t z_near, float32_t z_far)
	{
#ifdef VXM_SSE
		float32_t SinFov;
		float32_t CosFov;
		xm_scalar_sin_cos(&SinFov, &CosFov, 0.5f * fovy);
		float32_t fRange = z_far / (z_near - z_far);
		// Note: This is recorded on the stack
		float32_t Height = CosFov / SinFov;
		xmf rMem = { Height / aspect, Height, fRange, fRange * z_near };
		// Copy from memory to SSE register
		xmf vValues = rMem;
		xmf vTemp = _mm_setzero_ps();
		// Copy x only
		vTemp = _mm_move_ss(vTemp, vValues);
		// CosFov / SinFov,0,0,0
		xmm result;
		result[0] = vTemp;
		// 0,Height / AspectHByW,0,0
		vTemp = vValues;
		vTemp = _mm_and_ps(vTemp, xmf_mask_y);
		result[1] = vTemp;
		// x=fRange,y=-fRange * NearZ,0,-1.0f
		vTemp = _mm_setzero_ps();
		vValues = xm_shuffle_ps(vValues, xmf_row3_neg, _MM_SHUFFLE(3, 2, 3, 2));
		// 0,0,fRange,-1.0f
		vTemp = xm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
		result[2] = vTemp;
		// 0,0,fRange * NearZ,0.0f
		vTemp = xm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
		result[3] = vTemp;
		return result;
#else
		float32_t h = 1 / (float_tan(fovy * 0.5f));
		float32_t w = h / aspect;
		float32_t z = z_far / (z_far - z_near);
		float32_t f = z_near * z_far / (z_near - z_far);
		xmm result;
		result.row0 = xm_vec_set(w, 0, 0, 0);
		result.row1 = xm_vec_set(0, h, 0, 0);
		result.row2 = xm_vec_set(0, 0, z, -1);
		result.row3 = xm_vec_set(0, 0, f, 0);
		return result;
#endif
	}


	inline xmm xm_matr_orthogona_lh(float32_t width, float32_t height, float32_t z_near, float32_t z_far)
	{
		xmm result;
		result.row0 = xmf(2.0f / width, 0.0f, 0.0f, 0.0f);
		result.row1 = xmf(0.0f, 2.0f / height, 0.0f, 0.0f);
		result.row2 = xmf(0.0f, 0.0f, 1.0f / (z_far - z_near), 0.0f);
		result.row3 = xmf(0.0f, 0.0f, z_near / (z_far - z_near), 1.0f);
		return result;
	}

	inline xmm xm_matr_orthogona_rh(float32_t width, float32_t height, float32_t z_near, float32_t z_far)
	{
#ifdef VXM_SSE
		float32_t fRange = 1.0f / (z_far - z_near);
		// Note: This is recorded on the stack
		xmf rMem = { 2.0f / width, 2.0f / height, fRange, -fRange * z_near };
		// Copy from memory to SSE register
		xmf vValues = rMem;
		xmf vTemp = _mm_setzero_ps();
		// Copy x only
		vTemp = _mm_move_ss(vTemp, vValues);

		xmm result;
		// 2.0f / ViewWidth,0,0,0
		result[0] = vTemp;
		// 0,2.0f / ViewHeight,0,0
		vTemp = vValues;
		vTemp = _mm_and_ps(vTemp, xmf_mask_y);
		result[1] = vTemp;
		// x=fRange,y=-fRange * NearZ,0,1.0f
		vTemp = _mm_setzero_ps();
		vValues = xm_shuffle_ps(vValues, xmf_row3, _MM_SHUFFLE(3, 2, 3, 2));
		// 0,0,fRange,0.0f
		vTemp = xm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 0, 0, 0));
		result[2] = vTemp;
		// 0,0,-fRange * NearZ,1.0f
		vTemp = xm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 1, 0, 0));
		result[3] = vTemp;
		return result;
#else
		xmm result;
		result.row0 = xm_vec_set(2 / width, 0, 0, 0);
		result.row1 = xm_vec_set(0, 2 / height, 0, 0);
		result.row2 = xm_vec_set(0, 0, 1 / (z_far - z_near), 0);
		result.row3 = xm_vec_set(0, 0, z_near / (z_far - z_near), -1);
		return result;
#endif
	}


	inline xmm xm_matr_lookto_lh(const xmf & eye, const xmf & direction, const xmf & up)
	{
		xmf R2 = xm_vec3_normalize(direction);

		xmf R0 = xm_vec3_cross(up, R2);
		R0 = xm_vec3_normalize(R0);

		xmf R1 = xm_vec3_cross(R2, R0);

		xmf NegEyePosition = xm_vec_negate(eye);

		xmf D0 = xm_vec3_dot(R0, NegEyePosition);
		xmf D1 = xm_vec3_dot(R1, NegEyePosition);
		xmf D2 = xm_vec3_dot(R2, NegEyePosition);

		xmm result;
		result[0] = xm_vec_select(D0, R0, xmf_select_1110);
		result[1] = xm_vec_select(D1, R1, xmf_select_1110);
		result[2] = xm_vec_select(D2, R2, xmf_select_1110);
		result[3] = xmf_row3;
		return xm_matr_transpose(result);
	}

	inline xmm xm_matr_lookto_rh(const xmf & eye, const xmf & direction, const xmf & up)
	{
		xmf eye_negate = xm_vec_negate(eye);
		return xm_matr_lookto_lh(eye_negate, direction, up);
	}

	inline xmm xm_matr_lookat_lh(const xmf & eye, const xmf & at, const xmf & up)
	{
		xmf direction = xm_vec_sub(at, eye);
		return xm_matr_lookto_lh(eye, direction, up);
	}

	inline xmm xm_matr_lookat_rh(const xmf & eye, const xmf & at, const xmf & up)
	{
		xmf direction = xm_vec_sub(at, eye);
		return xm_matr_lookto_rh(eye, direction, up);
	}

	inline xmm xm_matr_transform(const xmf & pos, const xmf & rot, const xmf & scl)
	{
		xmm mPos = xm_matr_translate(pos);
		xmm mRot = xm_matr_rotate_quat(rot);
		xmm mScl = xm_matr_scale(scl);
		xmm mRotScl = xm_matr_mul(mRot, mScl);
		xmm result = xm_matr_mul(mRotScl, mPos);
		return result;
	}

	inline xmm float4x4_transform(
		const xmf & ScalingOrigin,
		const xmf & ScalingOrientationQuaternion,
		const xmf & Scaling,
		const xmf & RotationOrigin,
		const xmf & RotationQuaternion,
		const xmf & trans
	)
	{
		xmf VScalingOrigin = xm_vec_select(xmf_select_1110, ScalingOrigin, xmf_select_1110);
		xmf NegScalingOrigin = xm_vec_negate(ScalingOrigin);

		xmm MScalingOriginI = xm_matr_translate(NegScalingOrigin);
		xmm MScalingOrientation = xm_matr_rotate_quat(ScalingOrientationQuaternion);
		xmm MScalingOrientationT = xm_matr_transpose(MScalingOrientation);
		xmm MScaling = xm_matr_scale(Scaling);
		xmf VRotationOrigin = xm_vec_select(xmf_select_1110, RotationOrigin, xmf_select_1110);
		xmm MRotation = xm_matr_rotate_quat(RotationQuaternion);
		xmf VTranslation = xm_vec_select(xmf_select_1110, trans, xmf_select_1110);

		xmm result;
		result = xm_matr_mul(MScalingOriginI, MScalingOrientationT);
		result = xm_matr_mul(result, MScaling);
		result = xm_matr_mul(result, MScalingOrientation);
		result[3] = xm_vec_add(result[3], VScalingOrigin);
		result[3] = xm_vec_sub(result[3], VRotationOrigin);
		result = xm_matr_mul(result, MRotation);
		result[3] = xm_vec_add(result[3], VRotationOrigin);
		result[3] = xm_vec_add(result[3], VTranslation);
		return result;
	}

	inline xmf xm_matr_transform_coord(const xmm & matrix, const xmf & vec)
	{
#ifdef VXM_SSE
		xmf V = vec;
		xmf X = xm_vec_splat_x(V);
		xmf Y = xm_vec_splat_y(V);
		xmf Z = xm_vec_splat_z(V);

		xmf Result = xm_vec_mul_add(Z, matrix.row2, matrix.row3);
		Result = xm_vec_mul_add(Y, matrix.row1, Result);
		Result = xm_vec_mul_add(X, matrix.row0, Result);
		return Result;
#elif defined VXM_XMATH
#else
		const xmf & row0 = matrix[0];
		const xmf & row1 = matrix[1];
		const xmf & row2 = matrix[2];
		const xmf & row3 = matrix[3];

		float32_t x = vec.x * row0[0] + vec.y * row1[0] + vec.z * row2[0] + vec.w * row3[0];
		float32_t y = vec.x * row0[1] + vec.y * row1[1] + vec.z * row2[1] + vec.w * row3[1];
		float32_t z = vec.x * row0[2] + vec.y * row1[2] + vec.z * row2[2] + vec.w * row3[2];
		xmf result = xm_vec_set(x, y, z, vec.w);
		return result;
#endif
	}

	inline xmf xm_matr_transform_point(const xmm & matrix, const xmf & vec)
	{
#ifdef VXM_SSE
		xmf V = vec;
		xmf X = xm_vec_splat_x(V);
		xmf Y = xm_vec_splat_y(V);
		xmf Z = xm_vec_splat_z(V);

		xmf Result = xm_vec_mul_add(Z, matrix.row2, matrix.row3);
		Result = xm_vec_mul_add(Y, matrix.row1, Result);
		Result = xm_vec_mul_add(X, matrix.row0, Result);

		xmf W = xm_vec_splat_w(Result);
		return xm_vec_div(Result, W);
#elif defined VXM_XMATH
		XMVector4Transform()
#else
		const xmf & row0 = matrix[0];
		const xmf & row1 = matrix[1];
		const xmf & row2 = matrix[2];
		const xmf & row3 = matrix[3];

		float32_t x = vec.x * row0[0] + vec.y * row1[0] + vec.z * row2[0] + vec.w * row3[0];
		float32_t y = vec.x * row0[1] + vec.y * row1[1] + vec.z * row2[1] + vec.w * row3[1];
		float32_t z = vec.x * row0[2] + vec.y * row1[2] + vec.z * row2[2] + vec.w * row3[2];
		xmf result = xm_vec_set(x / vec.w, y / vec.w, z / vec.w, 1.0f);
		return result;
#endif
	}
}