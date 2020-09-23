#pragma once

#include "CoreInc.h"

namespace XE
{
	class CORE_API FBlob
	{
	public:
		virtual ~FBlob() = default;
		virtual TView<byte_t> GetData() const = 0;
	};

	class CORE_API FFileBlob : public FBlob
	{
	public:
		FFileBlob(FStringView Path);
		TView<byte_t> GetData() const override { return Data; }

	private:
		TVector<byte_t> Data;
	};
}
