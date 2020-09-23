#include "PCH.h"
#include "Vector4.h"
#include "xm/xmm.h"

namespace XE
{
    using namespace xm;

    const FVector4 FVector4::V0000(0.0f, 0.0f, 0.0f, 0.0f);
    const FVector4 FVector4::V1111(1.0f, 1.0f, 1.0f, 1.0f);
    const FVector4 FVector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
    const FVector4 FVector4::One(1.0f, 1.0f, 1.0f, 1.0f);

    const FVector4 FVector4::V1000(1.0f, 0.0f, 0.0f, 0.0f);
    const FVector4 FVector4::V0100(0.0f, 1.0f, 0.0f, 0.0f);
    const FVector4 FVector4::V0010(0.0f, 0.0f, 1.0f, 0.0f);
    const FVector4 FVector4::V0001(0.0f, 0.0f, 0.0f, 1.0f);

    float32_t FVector4::Length3() const
    {
        return xmv_x(xm_vec_length(*this));
    }

    float32_t FVector4::Length3Square() const
    {
        return xmv_x(xm_vec_length_sq(*this));
    }

    float32_t FVector4::Dot(const FVector4 & Another) const
    {
        return xmv_x(xm_vec_dot(*this, Another));
    }

    float32_t FVector4::Cross(const FVector4 & Another, const FVector4 & Another2) const
    {
        return xmv_x(xm_vec_cross(*this, Another, Another2));
    }

    FVector4 & FVector4::Negate()
    {
        *this = xm_vec_negate(*this);
        return *this;
    }

    FVector4 & FVector4::Normalize()
    {
        *this = xm_vec_normalize(*this);
        return *this;
    }

    FVector4 & FVector4::Add(const FVector4 & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FVector4 & FVector4::Subtract(const FVector4 & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FVector4 & FVector4::Multiply(const FVector4 & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FVector4 & FVector4::Divide(const FVector4 & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    FVector4 FVector4::operator +() const
    {
        return *this;
    }

    FVector4 FVector4::operator -() const
    {
        return xm_vec_negate(*this);
    }

    FVector4 FVector4::operator +(const FVector4 & Another) const
    {
        return xm_vec_add(*this, Another);
    }

    FVector4 & FVector4::operator +=(const FVector4 & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FVector4 FVector4::operator -(const FVector4 & Another) const
    {
        return xm_vec_sub(*this, Another);
    }

    FVector4 & FVector4::operator -=(const FVector4 & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FVector4 FVector4::operator *(const FVector4 & Another) const
    {
        return xm_vec_mul(*this, Another);
    }

    FVector4 & FVector4::operator *=(const FVector4 & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FVector4 FVector4::operator /(const FVector4 & Another) const
    {
        return xm_vec_div(*this, Another);
    }

    FVector4 & FVector4::operator /=(const FVector4 & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    FVector4 FVector4::operator *(float32_t rate) const
    {
        return xm_vec_mul(*this, xm_vec_replicate(rate));
    }

    FVector4 & FVector4::operator *=(float32_t rate)
    {
        *this = xm_vec_mul(*this, xm_vec_replicate(rate));
        return *this;
    }

    FVector4 FVector4::operator /(float32_t rate) const
    {
        return xm_vec_div(*this, xm_vec_replicate(rate));
    }

    FVector4 & FVector4::operator /=(float32_t rate)
    {
        *this = xm_vec_div(*this, xm_vec_replicate(rate));
        return *this;
    }

    FVector4 FVector4::Lerp(const FVector4 & VectorMin, const FVector4 & VectorMax, float32_t Inter)
    {
        return xm_vec_lerp(VectorMin, VectorMax, Inter);
    }
}
