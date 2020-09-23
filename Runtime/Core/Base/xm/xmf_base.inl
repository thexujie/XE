#pragma once

namespace XE::xm
{
	inline float32_t xm_scalar_sin(float32_t angle)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float32_t quotient = xm_1_of_2pi * angle;
		if (angle >= 0.0f)
		{
			quotient = (float32_t)((int32_t)(quotient + 0.5f));
		}
		else
		{
			quotient = (float32_t)((int32_t)(quotient - 0.5f));
		}
		float32_t y = angle - xm_2pi * quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(angle).
		if (y > xm_1pi_of_2)
		{
			y = xm_pi - y;
		}
		else if (y < -xm_1pi_of_2)
		{
			y = -xm_pi - y;
		}
		else
		{
		}

		// 11-degree minimax approximation
		float32_t y2 = y * y;
		return (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f)
			* y2 + 1.0f) * y;
	}

	inline void xm_scalar_sin_cos(float32_t * sin, float32_t * cos, float32_t value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float32_t quotient = xm_1_of_2pi * value;
		if (value >= 0.0f)
			quotient = (float32_t)((int32_t)(quotient + 0.5f));
		else
			quotient = (float32_t)((int32_t)(quotient - 0.5f));
		float32_t y = value - xm_2pi * quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float32_t sign = 0.0f;
		if (y > xm_1pi_of_2)
		{
			y = xm_pi - y;
			sign = -1.0f;
		}
		else if (y < -xm_1pi_of_2)
		{
			y = -xm_pi - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float32_t y2 = y * y;

		// 11-degree minimax approximation
		*sin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f)
			* y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float32_t p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 +
			1.0f;
		*cos = sign * p;
	}
}
