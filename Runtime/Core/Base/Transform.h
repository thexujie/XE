#pragma once

#include "Matrix.h"
#include "Quat.h"

namespace XE
{
	/**
	 * RTS
	 */
	struct CORE_API FTransform
	{
		FQuat Rotation;
		FVector3 Translation;
		FVector3 Scaling;

		FTransform()
		{
			Rotation = FQuat::Zero;
			Translation = FVector3::Zero;
			Scaling = FVector3::Zero;
		}

		FTransform(const FQuat & RotationIn, const FVector3 & TranslationIn, const FVector3 & ScalingIn)
		{
			Rotation = RotationIn;
			Translation = TranslationIn;
			Scaling = ScalingIn;
		}

		FTransform(const FTransform &) = default;
		FTransform(FTransform &&) = default;
		FTransform & operator=(const FTransform & That) = default;

		FMatrix ToMatrix() const;
		FTransform Inverse() const;

		FTransform operator*(const FTransform & Another) const;

	public:
		static const FTransform Identity;

		static FTransform Rotate(FQuat Rotation)
		{
			FTransform Transform = {};
			Transform.Rotation = Rotation;
			Transform.Scaling = FVector3::One;
			return Transform;
		}

		static FTransform Scale(FVector3 Scaling)
		{
			FTransform Transform = {};
			Transform.Rotation = FQuat::Identity;
			Transform.Scaling = Scaling;
			return Transform;
		}

		static FTransform Translate(FVector3 Translation)
		{
			FTransform Transform = {};
			Transform.Rotation = FQuat::Identity;
			Transform.Translation = Translation;
			Transform.Scaling = FVector3::One;
			return Transform;
		}
	};
}
