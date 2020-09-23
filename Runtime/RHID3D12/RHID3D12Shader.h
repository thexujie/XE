#pragma once

#include "RHID3D12Core.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12Shader : public FRHIFence
	{
	public:
		FRHID3D12Shader(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}
		~FRHID3D12Shader();

	private:
		FRHID3D12Device * RHID3D12Device = nullptr;
		FResourceDesc ResourceArgs;
		TReferPtr<ID3DBlob> D3DBlob;
		void * Pointer = nullptr;
	};
}
