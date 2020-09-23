#pragma once

#include "RHID3D12Inc.h"

namespace XE::RHI::D3D12
{
	class RHID3D12_API FRHID3D12Factory : public FRHIFactory
	{
	public:
		FRHID3D12Factory() = default;
		virtual ~FRHID3D12Factory() = default;

		EError Load() override;
		TVector<FRHIAdapterDesc> GetAdapterDescs() const override;
		TSharedPtr<FRHIDevice> CreateDevice(uint64_t Id) const override;

	public:
		TReferPtr<IDXGIFactory3> GetFactory() const { return DXGIFactory; }
		
	private:
		FLibrary DXGILibrary;
		TReferPtr<IDXGIFactory3> DXGIFactory;
	};
}

