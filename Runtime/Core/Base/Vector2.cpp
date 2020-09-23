#include "PCH.h"
#include "Vector2.h"
#include "xm/xmm.h"

namespace XE
{
    using namespace xm;

    FVector2 FVector2::operator +() const
    {
        return *this;
    }

    FVector2 FVector2::operator -() const
    {
        return xm_vec_negate(*this);
    }

    FVector2 FVector2::operator +(const FVector2 & Another) const
    {
        return xm_vec_add(*this, Another);
    }

    FVector2 & FVector2::operator +=(const FVector2 & Another)
    {
        *this = xm_vec_add(*this, Another);
        return *this;
    }

    FVector2 FVector2::operator -(const FVector2 & Another) const
    {
        return xm_vec_sub(*this, Another);
    }

    FVector2 & FVector2::operator -=(const FVector2 & Another)
    {
        *this = xm_vec_sub(*this, Another);
        return *this;
    }

    FVector2 FVector2::operator *(const FVector2 & Another) const
    {
        return xm_vec_mul(*this, Another);
    }

    FVector2 & FVector2::operator *=(float32_t Rate)
    {
        *this = xm_vec_mul(*this, FVector2(Rate, Rate));
        return *this;
    }

    FVector2 & FVector2::operator *=(const FVector2 & Another)
    {
        *this = xm_vec_mul(*this, Another);
        return *this;
    }

    FVector2 FVector2::operator /(const FVector2 & Another) const
    {
        return xm_vec_div(*this, Another);
    }

    FVector2 & FVector2::operator /=(float32_t Rate)
    {
        *this = xm_vec_div(*this, xm_vec_replicate(Rate));
        return *this;
    }

    FVector2 & FVector2::operator /=(const FVector2 & Another)
    {
        *this = xm_vec_div(*this, Another);
        return *this;
    }

    FVector2 FVector2::Lerp(const FVector2 & VectorMin, const FVector2 & VectorMax, float32_t Inter)
    {
        return xm_vec_lerp(VectorMin, VectorMax, Inter);
    }
}
