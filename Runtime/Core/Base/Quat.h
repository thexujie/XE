#pragma once

#include "Types.h"
#include "xm/xmm.h"
#include "Vector3.h"
#include "Vector4.h"

namespace XE
{
    struct CORE_API FQuat
    {
        FQuat() = default;
        FQuat & operator =(const FQuat & That) = default;

        FQuat(float32_t InX, float32_t InY, float32_t InZ, float32_t InW) : X(InX), Y(InY), Z(InZ), W(InW) { }

        FQuat(const FQuat & That) : X(That.X), Y(That.Y), Z(That.Z), W(That.W) { }

        FQuat(const xm::xmf & v) : X(xmv_x(v)), Y(xmv_y(v)), Z(xmv_z(v)), W(xmv_w(v)) { }

        FQuat(const float32_t (&FloatsIn)[4]) : X(FloatsIn[0]), Y(FloatsIn[1]), Z(FloatsIn[2]), W(FloatsIn[3]) { }

        float32_t & operator[](int32_t Index) { return Floats[Index]; }
        const float32_t & operator[](int32_t Index) const { return Floats[Index]; }

        FQuat & operator =(const xm::xmf & v)
        {
            X = xm::xmv_x(v);
            Y = xm::xmv_y(v);
            Z = xm::xmv_z(v);
            W = xm::xmv_w(v);
            return *this;
        }

        FQuat & operator =(const float32_t(&FloatsIn)[4])
        {
            X = FloatsIn[0];
            Y = FloatsIn[1];
            Z = FloatsIn[2];
            W = FloatsIn[3];
            return *this;
        }

        operator xm::xmf() const { return xm::xm_vec_set(X, Y, Z, W); }

        float32_t Length() const;
        float32_t LengthSquare() const;
        float32_t Dot(const FQuat & Another) const;
        FQuat Cross(const FQuat & Another, const FQuat & Another2) const;

        FQuat & Negate();
        FQuat & Normalize();
        FQuat Inverse() const
    	{
            return xm::xm_vec_mul(*this, xm::xmf(-1.0f, -1.0f, -1.0f, 1.0f));
        }
    	

        FQuat & Add(const FQuat & Another);
        FQuat & Subtract(const FQuat & Another);
        FQuat & Multiply(const FQuat & Another);
        FQuat & Divide(const FQuat & Another);

        FQuat operator +() const;
        FQuat operator -() const;

        FQuat operator +(const FQuat & Another) const;
        FQuat & operator +=(const FQuat & Another);

        FQuat operator -(const FQuat & Another) const;
        FQuat & operator -=(const FQuat & Another);

        FQuat operator *(const FQuat & Another) const;
        FQuat & operator *=(const FQuat & Another);

        FQuat operator /(const FQuat & Another) const;
        FQuat & operator /=(const FQuat & Another);

        FQuat operator *(float32_t rate) const;
        FQuat & operator *=(float32_t rate);
        FQuat operator /(float32_t rate) const;
        FQuat & operator /=(float32_t rate);

    public:
        union
        {
            float32_t Floats[4];
            struct
            {
                float32_t X;
                float32_t Y;
                float32_t Z;
                float32_t W;
            };
        };

    public:
        static const FQuat Zero;
        static const FQuat Identity;

    public:
        static FQuat Lerp(const FQuat & QuatMin, const FQuat & QuatMax, float32_t Inter);
        static FQuat Rotate(const FVector3 & Rotation);
    };

    using quat4 = FQuat;

    CORE_API FVector4 operator * (const FVector4 & vector, const FQuat & rotation);
}
