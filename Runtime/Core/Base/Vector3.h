#pragma once

#include "Types.h"
#include "Vector2.h"
#include "xm/xmm.h"

namespace XE
{
    struct CORE_API FVector3
    {
        FVector3() = default;
        FVector3(const FVector3 &) = default;
        FVector3 & operator=(const FVector3 &) = default;
        ~FVector3() = default;
        auto operator <=> (const FVector3 & Another) const { return Compare(X, Another.X, Y, Another.Y, Z, Another.Z); }
    	
        constexpr FVector3(const xm::xmf & m128) : X(xmv_x(m128)), Y(xmv_y(m128)), Z(xmv_z(m128)) { }
        constexpr FVector3(float32_t InX, float32_t InY, float32_t InZ) : X(InX), Y(InY), Z(InZ) { }
        constexpr FVector3(const float32_t(&FloatsIn)[3]) : X(FloatsIn[0]), Y(FloatsIn[1]), Z(FloatsIn[2]) { }
        constexpr FVector3(const FVector2 & InXY, float32_t InZ) : X(InXY.X), Y(InXY.Y), Z(InZ) { }

        inline FVector3 & operator=(const float32_t(&FloatsIn)[3])
        {
            X = FloatsIn[0];
            Y = FloatsIn[1];
            Z = FloatsIn[2];
            return *this;
        }
    	
        inline FVector3 & operator =(const xm::xmf & v)
        {
            X = xmv_x(v);
            Y = xmv_y(v);
            Z = xmv_z(v);
            return *this;
        }

        inline float32_t & operator[](int32_t Index) { return Floats[Index]; }
        inline const float32_t & operator[](int32_t Index) const { return Floats[Index]; }

        inline operator xm::xmf() const { return xm::xmf(X, Y, Z, 1.0f); }

        // -----------------------------------------------------------
        float32_t Length() const;
        float32_t LengthSquare() const;
        float32_t Dot(const FVector3 & Another) const { return X * Another.X + Y * Another.Y + Z * Another.Z; }
        FVector3 Cross(const FVector3 & Another) const;
        FVector3 Abs() const;

        FVector3 Negate() const;
        FVector3 Reciprocal() const;
        FVector3 & Normalize();
        FVector3 & Add(const FVector3 & Another);
        FVector3 & Subtract(const FVector3 & Another);
        FVector3 & Multiply(const FVector3 & Another);
        FVector3 & Divide(const FVector3 & Another);

        FVector3 operator +() const;
        FVector3 operator -() const;

        FVector3 operator +(const FVector3 & Another) const;
        FVector3 & operator +=(const FVector3 & Another);

        FVector3 operator -(const FVector3 & Another) const;
        FVector3 & operator -=(const FVector3 & Another);

        FVector3 operator *(float32_t Rate) const;
        FVector3 operator *(const FVector3 & Another) const;
        FVector3 & operator *=(float32_t Rate);
        FVector3 & operator *=(const FVector3 & Another);

        FVector3 operator /(float32_t Rate) const;
        FVector3 operator /(const FVector3 & Another) const;
        FVector3 & operator /=(float32_t Rate);
        FVector3 & operator /=(const FVector3 & Another);

        // dot
        float32_t operator % (const FVector3 & Another) const;
        // cross
        FVector3 operator ^(const FVector3 & Another) const;

        inline bool operator ==(const FVector3 & Another) const
        {
            xm::xmf diff = xm::xm_vec_abs(xm::xm_vec_sub(*this, Another));
            return !!xm::xmi_x(xm_vec_less(diff, xm::xmf_epsilon));
        }

        inline bool operator !=(const FVector3 & Another) const
        {
            return !operator==(Another);
        }

        friend FArchive & operator >> (FArchive & Serializer, FVector3 & Vector3)
        {
            Serializer >> Vector3.X;
            Serializer >> Vector3.Y;
            Serializer >> Vector3.Z;
            return Serializer;
        }
    public:
        union
        {
            struct
            {
                float32_t X;
                float32_t Y;
                float32_t Z;
            };

            struct
            {
                FVector2 XY;
                float32_t __Z;
            };

            float32_t Floats[3];
            int32_t Ints[3];
            uint32_t UInts[3];
        };

    public:
        static FVector3 Zero;
        static FVector3 One;
    	
    public:
        static FVector3 Lerp(const FVector3 & VectorMin, const FVector3 & VectorMax, float32_t Inter);
    };

    using float3 = FVector3;

    inline FVector3 operator *(FVector1 Rate, FVector3 Vector)
    {
        return Vector * Rate;
    }

    inline FVector3 operator /(FVector1 Rate, FVector3 Vector)
    {
        return Vector / Rate;
    }
}
