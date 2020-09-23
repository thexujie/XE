#pragma once

#include "Types.h"
#include "xm/xmm.h"

namespace XE
{
    using FVector1 = float32_t;
    using float1 = FVector1;

    struct CORE_API FVector2
    {
        FVector2() = default;
        FVector2(const FVector2 &) = default;
        FVector2 & operator=(const FVector2 &) = default;
        ~FVector2() = default;
    	auto operator <=> (const FVector2 & Another) const { return Compare(X, Another.X, Y, Another.Y); }
    	
        constexpr FVector2(const xm::xmf & v) : X(xm::xmv_x(v)), Y(xm::xmv_y(v)) { }
        constexpr FVector2(float32_t InX, float32_t InY) : X(InX), Y(InY) { }
        constexpr FVector2(const float32_t(&FloatsIn)[2]) : X(FloatsIn[0]), Y(FloatsIn[1]) { }

        inline FVector2 & operator=(const float32_t(&FloatsIn)[2])
        {
            X = FloatsIn[0];
            Y = FloatsIn[1];
            return *this;
        }

        inline FVector2 & operator =(xm::xmf v)
        {
            X = xm::xmv_x(v);
            Y = xm::xmv_y(v);
            return *this;
        }

        inline float32_t & operator[](int32_t Index) { return Floats[Index]; }
        inline const float32_t & operator[](int32_t Index) const { return Floats[Index]; }

        inline operator xm::xmf() const
        {
            return xm::xmf(X, Y, 0.0f, 0.0f);
        }

        float32_t Dot(const FVector2 & Another) const { return X * Another.X + Y * Another.Y; }
        float32_t Length() const { return sqrt(X * X + Y * Y); }
        float32_t LengthSquare() const { return X * X + Y * Y; }
    	
        FVector2 operator +() const;
        FVector2 operator -() const;

        FVector2 operator +(const FVector2 & Another) const;
        FVector2 & operator +=(const FVector2 & Another);

        FVector2 operator -(const FVector2 & Another) const;
        FVector2 & operator -=(const FVector2 & Another);

        FVector2 operator *(const FVector2 & Another) const;
        FVector2 & operator *=(float32_t Rate);
        FVector2 & operator *=(const FVector2 & Another);

        FVector2 operator /(const FVector2 & Another) const;
        FVector2 & operator /=(float32_t Rate);
        FVector2 & operator /=(const FVector2 & Another);

        friend FArchive & operator >> (FArchive & Serializer, FVector2 & Vector2)
        {
            Serializer >> Vector2.X;
            Serializer >> Vector2.Y;
            return Serializer;
        }

    public:
        union
        {
            struct
            {
                float32_t X;
                float32_t Y;
            };
        	
            float32_t Floats[2];
            int32_t Ints[2];
            uint32_t UInts[2];
        };

    public:
        FVector2 Lerp(const FVector2 & VectorMin, const FVector2 & VectorMax, float32_t Inter);
    };

    using float2 = FVector2;

    inline FVector2 operator *(float32_t Rate, FVector2 Vector)
    {
        return FVector2(Vector.X * Rate, Vector.Y * Rate);
    }

    inline FVector2 operator /(float32_t Rate, FVector2 Vector)
    {
        return FVector2(Vector.X * Rate, Vector.Y * Rate);
    }
}
