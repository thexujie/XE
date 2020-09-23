#pragma once

#include "RHID3D12Core.h"
#include "RHID3D12Device.h"
#include "RHID3D12RootSignature.h"

namespace XE::RHI::D3D12
{
	class FRHID3D12PipelineState : public FRHIPipelineState
	{
	public:
		virtual ID3D12PipelineState * PipelineState() const = 0;
	};
	
	class FRHID3D12GraphicsPipelineState : public FRHID3D12PipelineState
	{
	public:
		FRHID3D12GraphicsPipelineState(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}

		EError Create(FRHIRootSignature * RHIRootSignature, const FGraphicsPipelistStateDesc & GraphicsPipelistStateArgsIn);
		void SetName(FStringView Name) override;

	public:
		const FGraphicsPipelistStateDesc & Args() const { return PipelineStateArgs; }
		ID3D12PipelineState * PipelineState() const override { return D3D12PipelineState.Get(); }
		
	private:
		FRHID3D12Device * RHID3D12Device = nullptr;

		FGraphicsPipelistStateDesc PipelineStateArgs;
		TReferPtr<ID3D12PipelineState> D3D12PipelineState;

		ECommandType CommandType = ECommandType::Direct;
		ECommandQueueFlags CommandQueueFlags = ECommandQueueFlags::None;
	};

	class FRHID3D12ComputePipelineState : public FRHID3D12PipelineState
	{
	public:
		FRHID3D12ComputePipelineState(FRHID3D12Device * RHID3D12DeviceIn) : RHID3D12Device(RHID3D12DeviceIn) {}

		EError Create(FRHIRootSignature * RHIRootSignature, const FComputePipelineStateDesc & ComputePipelineStateIn);
		void SetName(FStringView Name) override;

	public:
		const FComputePipelineStateDesc & Args() const { return ComputePipelineState; }
		ID3D12PipelineState * PipelineState() const override { return D3D12PipelineState.Get(); }

	private:
		FRHID3D12Device * RHID3D12Device = nullptr;

		FComputePipelineStateDesc ComputePipelineState;
		TReferPtr<ID3D12PipelineState> D3D12PipelineState;

		ECommandType CommandType = ECommandType::Direct;
		ECommandQueueFlags CommandQueueFlags = ECommandQueueFlags::None;
	};
}
