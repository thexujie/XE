#include "PCH.h"
#include "Transform.h"

namespace XE
{
	const FTransform FTransform::Identity(FQuat::Identity, FVector3::Zero, FVector3::One);
	
	FMatrix FTransform::ToMatrix() const
	{
		return FMatrix::Transform(Translation, Rotation, Scaling);
	}

	FTransform FTransform::Inverse() const
	{
		FQuat RotationInv = Rotation.Inverse();
		FVector3 ScaleInv = Scaling.Reciprocal();

		FVector3 ScaledTranslation = ScaleInv * Translation;
		FVector3 t2 = ScaledTranslation * RotationInv;
		FVector3 TranslationInv = t2.Negate();
		return FTransform(RotationInv, TranslationInv, ScaleInv);
	}

	FTransform FTransform::operator*(const FTransform & Another) const
	{
		FQuat RotationOut = Rotation * Another.Rotation;
		FVector3 ScaledTransA = Translation * Another.Scaling;
		FVector3 RotatedTranslate = ScaledTransA * Another.Rotation;
		FVector3 TranslationOut = RotatedTranslate + Another.Translation;
		FVector3 ScaleOut = Scaling * Another.Scaling;
		return FTransform(RotationOut, TranslationOut, ScaleOut);
	}
}
