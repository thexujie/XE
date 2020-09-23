#include "PCH.h"
#include "Quat.h"

namespace XE
{
    using namespace XE::xm;

    const FQuat FQuat::Zero(0.0f, 0.0f, 0.0f, 0.0f);
    const FQuat FQuat::Identity(0.0f, 0.0f, 0.0f, 1.0f);
	
    float32_t FQuat::Length() const
    {
        return xmv_x(xm_vec_length(*this));
    }

    float32_t FQuat::LengthSquare() const
    {
        return xmv_x(xm_vec_length_sq(*this));
    }

    float32_t FQuat::Dot(const FQuat & Another) const
    {
        return xmv_x(xm_vec_dot(*this, Another));
    }

    FQuat FQuat::Cross(const FQuat & Another, const FQuat & Another2) const
    {
        return xm_vec_cross(*this, Another, Another2);
    }

    FQuat & FQuat::Negate()
    {
        *this = xm_vec_negate(*this);
        return *this;
    }

    FQuat & FQuat::Normalize()
    {
        *this = xm_vec_normalize(*this);
        return *this;
    }


    FQuat & FQuat::Add(const FQuat & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FQuat & FQuat::Subtract(const FQuat & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FQuat & FQuat::Multiply(const FQuat & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FQuat & FQuat::Divide(const FQuat & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    FQuat FQuat::operator +() const
    {
        return *this;
    }

    FQuat FQuat::operator -() const
    {
        return xm_vec_negate(*this);
    }

    FQuat FQuat::operator +(const FQuat & Another) const
    {
        return xm_vec_add(*this, Another);
    }

    FQuat & FQuat::operator +=(const FQuat & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FQuat FQuat::operator -(const FQuat & Another) const
    {
        return xm_vec_sub(*this, Another);
    }

    FQuat & FQuat::operator -=(const FQuat & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FQuat FQuat::operator *(const FQuat & Another) const
    {
        return xm_vec_mul(*this, Another);
    }

    FQuat & FQuat::operator *=(const FQuat & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FQuat FQuat::operator /(const FQuat & Another) const
    {
        return xm_vec_div(*this, Another);
    }

    FQuat & FQuat::operator /=(const FQuat & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    FQuat FQuat::operator *(float32_t rate) const
    {
        return xm_vec_mul(*this, xm_vec_replicate(rate));
    }

    FQuat & FQuat::operator *=(float32_t rate)
    {
        *this = xm_vec_mul(*this, xm_vec_replicate(rate));
        return *this;
    }

    FQuat FQuat::operator /(float32_t rate) const
    {
        return xm_vec_div(*this, xm_vec_replicate(rate));
    }

    FQuat & FQuat::operator /=(float32_t rate)
    {
        *this = xm_vec_div(*this, xm_vec_replicate(rate));
        return *this;
    }

    FQuat FQuat::Lerp(const FQuat & QuatMin, const FQuat & QuatMax, float32_t Inter)
    {
        return xm_quat_lerp(QuatMin, QuatMax, Inter);
    }

    FQuat FQuat::Rotate(const FVector3 & Rotation)
    {
        return xm_quat_rotation_from_vec(Rotation);
    }

    FVector4 operator * (const FVector4 & vector, const FQuat & rotation)
    {
        xm::xmf QW = xm::xm_vec_swizzle<xm::xm_swizzle_z, xm::xm_swizzle_z, xm::xm_swizzle_z, xm::xm_swizzle_z>(rotation);
        xm::xmf T = xm::xm_vec3_cross(QW, vector);
        T = xm::xm_vec_add(T, T);
        xm::xmf VTemp0 = xm::xm_vec_mul_add(QW, T, vector);
        xm::xmf VTemp1 = xm::xm_vec3_cross(rotation, T);
        xm::xmf VTempRotated = xm::xm_vec_add(VTemp0, VTemp1);
        return VTempRotated;
    }
}
