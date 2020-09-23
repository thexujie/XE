#include "PCH.h"
#include "Vector3.h"
#include "xm/xmm.h"

namespace XE
{
    using namespace xm;

   FVector3 FVector3::Zero(0.0f, 0.0f, 0.0f);
   FVector3 FVector3::One(1.0f, 1.0f, 1.0f);
	
    float32_t FVector3::Length() const
    {
        return xmv_x(xm_vec3_length(*this));
    }

    float32_t FVector3::LengthSquare() const
    {
        return xmv_x(xm_vec3_length_sq(*this));
    }

    FVector3 FVector3::Cross(const FVector3 & Another) const
    {
        return xm_vec3_cross(*this, Another);
    }

    FVector3 FVector3::Abs() const
    {
        return xm_vec_abs(*this);
    }

    FVector3 FVector3::Negate() const
    {
        return xm_vec_negate(*this);
    }
	
    FVector3 FVector3::Reciprocal() const
    {
        return xm_vec_reciprocal(*this);
    }

    FVector3 & FVector3::Normalize()
    {
        *this = xm_vec3_normalize(*this);
        return *this;
    }

    FVector3 & FVector3::Add(const FVector3 & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FVector3 & FVector3::Subtract(const FVector3 & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FVector3 & FVector3::Multiply(const FVector3 & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FVector3 & FVector3::Divide(const FVector3 & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    FVector3 FVector3::operator +() const
    {
        return *this;
    }

    FVector3 FVector3::operator -() const
    {
        return xm_vec_negate(*this);
    }

    FVector3 FVector3::operator +(const FVector3 & Another) const
    {
        return xm_vec_add(*this, Another);
    }

    FVector3 & FVector3::operator +=(const FVector3 & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FVector3 FVector3::operator -(const FVector3 & Another) const
    {
        return xm_vec_sub(*this, Another);
    }

    FVector3 & FVector3::operator -=(const FVector3 & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FVector3 FVector3::operator *(float32_t Rate) const
    {
        return xm_vec_mul(*this, FVector3(Rate, Rate, Rate));
    }

    FVector3 FVector3::operator *(const FVector3 & Another) const
    {
        return xm_vec_mul(*this, Another);
    }

    FVector3 & FVector3::operator *=(float32_t Rate)
    {
        *this = xm_vec_mul(*this, FVector3(Rate, Rate, Rate));
        return *this;
    }

    FVector3 & FVector3::operator *=(const FVector3 & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FVector3 FVector3::operator /(float32_t Rate) const
    {
        return xm_vec_div(*this, xm_vec_replicate(Rate));
    }

    FVector3 FVector3::operator /(const FVector3 & Another) const
    {
        return xm_vec_div(*this, Another);
    }

    FVector3 & FVector3::operator /=(float32_t Rate)
    {
        *this = xm_vec_div(*this, xm_vec_replicate(Rate));
        return *this;
    }

    FVector3 & FVector3::operator /=(const FVector3 & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    float32_t FVector3::operator % (const FVector3 & Another) const
    {
        return xmv_x(xm_vec3_dot(*this, Another));
    }

    FVector3 FVector3::operator ^(const FVector3 & Another) const
    {
        return xm_vec3_cross(*this, Another);
    }

    FVector3 FVector3::Lerp(const FVector3 & VectorMin, const FVector3 & VectorMax, float32_t Inter)
    {
        return xm_vec_lerp(VectorMin, VectorMax, Inter);
    }
}
