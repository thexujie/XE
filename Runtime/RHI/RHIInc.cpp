#include "PCH.h"
#include "RHIInc.h"

namespace XE::RHI
{
	TVector<FRootSignatureDesc> VA;
	TVector<FRootSignatureDesc> VB;
	std::strong_ordering Comp = 0 <=> 0;
	std::strong_ordering CompA = Compare(0, 0);
	std::strong_ordering CompB = Compare(EFormat::None, EFormat::B32G32R32F);
	std::strong_ordering CompC = Compare(FString(), FString());
	std::strong_ordering CompD = Compare(VA, VB);
}
