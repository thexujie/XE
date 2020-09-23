#include "PCH.h"
#include "Matrix.h"
#include "xm/xmm.h"

namespace XE
{
    using namespace XE::xm;

    const FMatrix FMatrix::Identity(FVector4::V1000, FVector4::V0100, FVector4::V0010, FVector4::V0001);

    FMatrix & FMatrix::Inverse(float32_t * Determinant)
    {
        *this = xm_matr_invert(*this, nullptr);
        return *this;
    }

    FMatrix & FMatrix::Transpose()
    {
        *this = xm_matr_transpose(*this);
        return *this;
    }

    FMatrix & FMatrix::Multiply(const FMatrix & Another)
    {
        *this = xm_matr_mul(*this, Another);
        return *this;
    }

    FVector3 FMatrix::TransformPoint(const FVector3 & Point) const
    {
	    return xm_matr_transform_point(*this, Point);
    }

    FVector4 FMatrix::TransformPoint(const FVector4 & Point) const
    {
        return FVector4(TransformPoint(Point.XYZ), 1.0f);
    }

	FVector4 FMatrix::TransformCoord(const FVector4 & Coord) const
    {
        return xm_matr_transform_coord(*this, Coord);
    }
	
    FQuat FMatrix::TransformQuat(FQuat & Quat) const
    {
        return xm_matr_transform_coord(*this, Quat);
    }

    FMatrix FMatrix::operator *(const FMatrix & Another) const
    {
        return xm_matr_mul(*this, Another);
    }

    FMatrix & FMatrix::operator *=(const FMatrix & Another)
    {
        *this = xm_matr_mul(*this, Another);
        return *this;
    }

    void FMatrix::InverseTo(FMatrix & Result) const
    {
        Result = xm_matr_invert(*this, nullptr);
    }

    void FMatrix::TransposeTo(FMatrix & Result) const
    {
        Result = xm_matr_transpose(*this);
    }


    FMatrix FMatrix::Transform(const FVector3 & Translation, const FVector3 & Rotation, const FVector3 & Scaling)
    {
        xmf angles(Rotation.X, Rotation.Y, Rotation.Z, 0.0f);
        xmf Quatation = xmf_quat_rot_from_angles(angles);
        return xm_matr_transform(Translation, Quatation, Scaling);
    }

    FMatrix FMatrix::Transform(const FVector3 & Translation, const FQuat & Rotation, const FVector3 & Scaling)
    {
        return xm_matr_transform(Translation, Rotation, Scaling);
    }

    FMatrix FMatrix::Translate(const FVector3 & Translation)
    {
        return xm_matr_translate(Translation);
    }

    FMatrix FMatrix::Rotate(const FVector3 & Rotation)
    {
        return xm_matr_rotate(Rotation);
    }

    FMatrix FMatrix::Rotate(const FQuat & Quatation)
    {
        return xm_matr_rotate_quat(Quatation);
    }

    FMatrix FMatrix::RotateAxis(const FVector4 & Axis, float32_t Angle)
    {
        return xm_matr_rotate_axis(Axis, Angle);
    }

    FMatrix FMatrix::Scale(const FVector3 & Scale)
    {
        return xm_matr_scale(Scale);
    }

    FMatrix FMatrix::PerspectiveLH(float32_t FovY, float32_t Aspect, float32_t NearZ, float32_t FarZ)
    {
        return xmm_matr_perspective_lh(FovY, Aspect, NearZ, FarZ);
    }

    FMatrix FMatrix::PerspectiveRH(float32_t FovY, float32_t Aspect, float32_t NearZ, float32_t FarZ)
    {
        return xmm_matr_perspective_rh(FovY, Aspect, NearZ, FarZ);
    }

    FMatrix FMatrix::OrthogonaLH(float32_t Width, float32_t Height, float32_t NearZ, float32_t FarZ)
    {
        return xm::xm_matr_orthogona_lh(Width, Height, NearZ, FarZ);
    }

    FMatrix FMatrix::OrthogonaRH(float32_t Width, float32_t Height, float32_t NearZ, float32_t FarZ)
    {
        return xm::xm_matr_orthogona_rh(Width, Height, NearZ, FarZ);
    }

    FMatrix FMatrix::LookAtLH(const FVector3 & Position, const FVector3 & Center, const FVector3 & Upward)
    {
        return xm::xm_matr_lookat_lh(Position, Center, Upward);
    }

    FMatrix FMatrix::LookAtRH(const FVector3 & Position, const FVector3 & Center, const FVector3 & Upward)
    {
        return xm::xm_matr_lookat_rh(Position, Center, Upward);
    }
    FMatrix FMatrix::LookToLH(const FVector3 & Position, const FVector3 & Forward, const FVector3 & Upward)
    {
        return xm::xm_matr_lookto_lh(Position, Forward, Upward);
    }

    FMatrix FMatrix::LookToRH(const FVector3 & Position, const FVector3 & Forward, const FVector3 & Upward)
    {
        return xm::xm_matr_lookto_rh(Position, Forward, Upward);
    }
	
}
