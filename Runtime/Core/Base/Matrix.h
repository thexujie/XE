#pragma once

#include "Vector4.h"
#include "Quat.h"

namespace XE
{
    struct CORE_API FMatrix
    {
        FMatrix() = default;
        FMatrix(const FMatrix &) = default;
        FMatrix & operator=(const FMatrix &) = default;
        ~FMatrix() = default;
    	
        FMatrix(const FVector4 & Row0In, const FVector4 & Row1In, const FVector4 & Row2In, const FVector4 & Row3In) : Row0(Row0In), Row1(Row1In), Row2(Row2In), Row3(Row3In) { }
        FMatrix(FVector4 Rows[4]) : Row0(Rows[0]), Row1(Rows[1]), Row2(Rows[2]), Row3(Rows[3]) { }
        FMatrix(float32_t Floats[16])
            : Row0(Floats[0], Floats[1], Floats[2], Floats[3])
            , Row1(Floats[4], Floats[5], Floats[6], Floats[7])
            , Row2(Floats[8], Floats[9], Floats[10], Floats[11])
            , Row3(Floats[12], Floats[13], Floats[14], Floats[15])
    	{}
        FMatrix(float32_t Float4s[4][4]) : Row0(Float4s[0]), Row1(Float4s[1]), Row2(Float4s[2]), Row3(Float4s[3]) { }
        FMatrix(float32_t F00, float32_t F01, float32_t F02, float32_t F03,
            float32_t F10, float32_t F11, float32_t F12, float32_t F13,
            float32_t F20, float32_t F21, float32_t F22, float32_t F23,
            float32_t F30, float32_t F31, float32_t F32, float32_t F33) :
            Row0(F00, F01, F02, F03), Row1(F10, F11, F12, F13), Row2(F20, F21, F22, F23), Row3(F30, F31, F32, F33) { }
        FMatrix(const xm::xmm & matrix) : Row0(matrix.row0), Row1(matrix.row1), Row2(matrix.row2), Row3(matrix.row3) { }

        FVector4 & operator[](int32_t RowIndex) { return Rows[RowIndex]; }
        const FVector4 & operator[](int32_t RowIndex) const { return Rows[RowIndex]; }

        FMatrix & operator=(const xm::xmm & matrix)
        {
            Row0 = matrix.row0;
            Row1 = matrix.row1;
            Row2 = matrix.row2;
            Row3 = matrix.row3;
            return *this;
        }

        operator xm::xmm() const
        {
            xm::xmm matrix;
            matrix.row0 = Row0;
            matrix.row1 = Row1;
            matrix.row2 = Row2;
            matrix.row3 = Row3;
            return matrix;
        }

    public:
        FMatrix & Inverse(float32_t * Determinant = nullptr);
        FMatrix & Transpose();
        FMatrix & Multiply(const FMatrix & Another);

        FVector3 TransformPoint(const FVector3 & Point) const;
        FVector4 TransformPoint(const FVector4 & Point) const;
        FVector4 TransformCoord(const FVector4 & Coord) const;
        FQuat TransformQuat(FQuat & Quat) const;

        FMatrix operator *(const FMatrix & Another) const;
        FMatrix & operator *=(const FMatrix & Another);

        void InverseTo(FMatrix & Result) const;
        void TransposeTo(FMatrix & Result) const;

        friend FArchive & operator >> (FArchive & Serializer, FMatrix & Matrix)
        {
            Serializer >> Matrix.Row0;
            Serializer >> Matrix.Row1;
            Serializer >> Matrix.Row2;
            Serializer >> Matrix.Row3;
            return Serializer;
        }
    public:
        union
        {
            FVector1 Floats[16];

            struct
            {
                FVector4 Rows[4];
            };

            struct
            {
                FVector4 Row0;
                FVector4 Row1;
                FVector4 Row2;
                FVector4 Row3;
            };

            struct
            {
                float32_t _11, _12, _13, _14;
                float32_t _21, _22, _23, _24;
                float32_t _31, _32, _33, _34;
                float32_t _41, _42, _43, _44;
            };
        };

    public:
        static const FMatrix Identity;

        static FMatrix Transform(const FVector3 & Translation, const FVector3 & Rotation, const FVector3 & Scaling);
        static FMatrix Transform(const FVector3 & Translation, const FQuat & Rotation, const FVector3 & Scaling);

        static FMatrix Translate(const FVector3 & Translation);

        static FMatrix Rotate(const FVector3 & Rotation);
        static FMatrix Rotate(const FQuat & Quatation);
        static FMatrix RotateAxis(const FVector4 & Axis, float32_t Angle);

        static FMatrix Scale(const FVector3 & Scale);

        static FMatrix PerspectiveRH(float32_t FovY, float32_t Aspect, float32_t NearZ, float32_t FarZ);
        static FMatrix PerspectiveLH(float32_t FovY, float32_t Aspect, float32_t NearZ, float32_t FarZ);

        static FMatrix OrthogonaLH(float32_t Width, float32_t Height, float32_t NearZ, float32_t FarZ);
        static FMatrix OrthogonaRH(float32_t Width, float32_t Height, float32_t NearZ, float32_t FarZ);

        static FMatrix LookAtLH(const FVector3 & Position, const FVector3 & Center, const FVector3 & Upward);
        static FMatrix LookAtRH(const FVector3 & Position, const FVector3 & Center, const FVector3 & Upward);

        static FMatrix LookToLH(const FVector3 & Position, const FVector3 & Forward, const FVector3 & Upward);
        static FMatrix LookToRH(const FVector3 & Position, const FVector3 & Forward, const FVector3 & Upward);
    };

    using float4x4 = FMatrix;
	
    inline FVector3 operator * (const FVector3 & Point, const FMatrix & Matrix)
    {
        return Matrix.TransformPoint(Point);
    }

    inline FVector3 operator * (const FMatrix & Matrix, const FVector3 & Point)
    {
        return Matrix.TransformPoint(Point);
    }

    inline FVector4 operator * (const FVector4 & Point, const FMatrix & Matrix)
    {
        return Matrix.TransformCoord(Point);
    }

    inline FVector4 operator * (const FMatrix & Matrix, const FVector4 & Point)
    {
        return Matrix.TransformCoord(Point);
    }
}
