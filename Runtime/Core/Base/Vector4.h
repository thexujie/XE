#pragma once

#include "Types.h"
#include "Vector3.h"

namespace XE
{
    struct FMatrix;

    struct CORE_API FVector4
    {
        FVector4() = default;
        FVector4(const FVector4 &) = default;
        FVector4 & operator=(const FVector4 &) = default;
        ~FVector4() = default;
        auto operator <=> (const FVector4 & Another) const { return Compare(X, Another.X, Y, Another.Y, Z, Another.Z, W, Another.W); }
    	
        constexpr FVector4(float32_t InX, float32_t InY, float32_t InZ, float32_t InW = 1.0f) : X(InX), Y(InY), Z(InZ), W(InW) { }
        constexpr FVector4(const FVector3 & Vector3) : X(Vector3.X), Y(Vector3.Y), Z(Vector3.Z), W(0) { }
        constexpr FVector4(const FVector3 & Vector3, float32_t InW) : X(Vector3.X), Y(Vector3.Y), Z(Vector3.Z), W(InW) { }
        constexpr FVector4(const xm::xmf & v) : X(xmv_x(v)), Y(xmv_y(v)), Z(xmv_z(v)), W(xmv_w(v)) { }
        constexpr FVector4(const float32_t(&FloatsIn)[4]) : X(FloatsIn[0]), Y(FloatsIn[1]), Z(FloatsIn[2]), W(FloatsIn[3]) { }

        FVector4 & operator =(const xm::xmf & v)
        {
            X = xmv_x(v);
            Y = xmv_y(v);
            Z = xmv_z(v);
            W = xmv_w(v);
            return *this;
        }

        FVector4 & operator =(const float32_t(&FloatsIn)[4])
        {
            X = FloatsIn[0];
            Y = FloatsIn[1];
            Z = FloatsIn[2];
            W = FloatsIn[3];
            return *this;
        }

        inline float32_t & operator[](int32_t Index) { return Floats[Index]; }
        inline const float32_t & operator[](int32_t Index) const { return Floats[Index]; }
    	
        operator xm::xmf() const { return xm::xm_vec_set(X, Y, Z, W); }
        operator FVector3() const { return FVector3(X, Y, Z); }

        float32_t Length3() const;
        float32_t Length3Square() const;
        float32_t Dot(const FVector4 & Another) const;
        float32_t Cross(const FVector4 & Another, const FVector4 & Another2) const;

        FVector4 & Negate();
        FVector4 & Normalize();

        FVector4 & Add(const FVector4 & Another);
        FVector4 & Subtract(const FVector4 & Another);
        FVector4 & Multiply(const FVector4 & Another);
        FVector4 & Divide(const FVector4 & Another);

        FVector4 operator +() const;
        FVector4 operator -() const;

        FVector4 operator +(const FVector4 & Another) const;
        FVector4 & operator +=(const FVector4 & Another);

        FVector4 operator -(const FVector4 & Another) const;
        FVector4 & operator -=(const FVector4 & Another);

        FVector4 operator *(const FVector4 & Another) const;
        FVector4 & operator *=(const FVector4 & Another);

        FVector4 operator /(const FVector4 & Another) const;
        FVector4 & operator /=(const FVector4 & Another);

        FVector4 operator *(float32_t rate) const;
        FVector4 & operator *=(float32_t rate);
        FVector4 operator /(float32_t rate) const;
        FVector4 & operator /=(float32_t rate);

        friend FArchive & operator >> (FArchive & Serializer, FVector4 & Vector4)
        {
            Serializer >> Vector4.X;
            Serializer >> Vector4.Y;
            Serializer >> Vector4.Z;
            Serializer >> Vector4.W;
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
                float32_t W;
            };

            struct
            {
                FVector3 XYZ;
                float32_t __W;
            };
        	
            struct
            {
                FVector2 XY;
                FVector2 ZW;
            };

            float32_t Floats[4];
            int32_t Ints[4];
            uint32_t UInts[4];
        };

    public:
        static const FVector4 Zero;
        static const FVector4 One;
        static const FVector4 V0000;
        static const FVector4 V1111;

        static const FVector4 V1000;
        static const FVector4 V0100;
        static const FVector4 V0010;
        static const FVector4 V0001;

    public:
        static FVector4 Lerp(const FVector4 & VectorMin, const FVector4 & VectorMax, float32_t Inter);
    };

    using float4 = FVector4;

    inline FVector4 operator *(FVector1 Rate, FVector4 Vector4)
    {
        return FVector4(Vector4.X * Rate, Vector4.Y * Rate, Vector4.Z * Rate, Vector4.W * Rate);
    }

    inline FVector4 operator /(FVector1 Rate, FVector4 Vector4)
    {
        return FVector4(Vector4.X * Rate, Vector4.Y * Rate, Vector4.Z * Rate, Vector4.W * Rate);
    }
}
