#include "PCH.h"
#include "RHID3D12PipelineState.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12GraphicsPipelineState::Create(FRHIRootSignature * RHIRootSignature, const  FGraphicsPipelistStateDesc & GraphicsPipelistStateArgsIn)
	{
		if (!RHIRootSignature)
			return EError::Args;
		
		if (!RHID3D12Device)
			return EError::State;
		
		auto Device = RHID3D12Device->GetDevice();
		assert(Device);

		HRESULT HR = S_OK;
		FRHID3D12RootSignature * RHID3D12RootSignature = static_cast<FRHID3D12RootSignature*>(RHIRootSignature);
		ID3D12RootSignature * D3D12RootSignature = RHID3D12RootSignature->RootSignature();

		TVector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs(GraphicsPipelistStateArgsIn.Elements.GetSize());
		for (size_t ElementIndex = 0; ElementIndex < GraphicsPipelistStateArgsIn.Elements.GetSize(); ++ElementIndex)
		{
			InputElementDescs[ElementIndex].SemanticName = (LPCSTR)GraphicsPipelistStateArgsIn.Elements[ElementIndex].Name.GetData();
			InputElementDescs[ElementIndex].SemanticIndex = GraphicsPipelistStateArgsIn.Elements[ElementIndex].Index;
			InputElementDescs[ElementIndex].Format = FromFormat(GraphicsPipelistStateArgsIn.Elements[ElementIndex].Format);
			InputElementDescs[ElementIndex].InputSlot = GraphicsPipelistStateArgsIn.Elements[ElementIndex].SlotIndex;
			InputElementDescs[ElementIndex].AlignedByteOffset = GraphicsPipelistStateArgsIn.Elements[ElementIndex].Offset;
			InputElementDescs[ElementIndex].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			InputElementDescs[ElementIndex].InstanceDataStepRate = 0;
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPipelineStateDesc = {};
		GraphicsPipelineStateDesc.pRootSignature = D3D12RootSignature;
		GraphicsPipelineStateDesc.VS = D3D12_SHADER_BYTECODE{ GraphicsPipelistStateArgsIn.VSCode.Data, GraphicsPipelistStateArgsIn.VSCode.Size };
		GraphicsPipelineStateDesc.HS = D3D12_SHADER_BYTECODE{ GraphicsPipelistStateArgsIn.HSCode.Data, GraphicsPipelistStateArgsIn.HSCode.Size };
		GraphicsPipelineStateDesc.DS = D3D12_SHADER_BYTECODE{ GraphicsPipelistStateArgsIn.DSCode.Data, GraphicsPipelistStateArgsIn.DSCode.Size };
		GraphicsPipelineStateDesc.GS = D3D12_SHADER_BYTECODE{ GraphicsPipelistStateArgsIn.GSCode.Data, GraphicsPipelistStateArgsIn.GSCode.Size };
		GraphicsPipelineStateDesc.PS = D3D12_SHADER_BYTECODE{ GraphicsPipelistStateArgsIn.PSCode.Data, GraphicsPipelistStateArgsIn.PSCode.Size };

		GraphicsPipelineStateDesc.BlendState.AlphaToCoverageEnable = GraphicsPipelistStateArgsIn.Blend.AlphaToCoverage;
		GraphicsPipelineStateDesc.BlendState.IndependentBlendEnable = GraphicsPipelistStateArgsIn.Blend.IndependentBlend;
		for (uint32_t itarget = 0; itarget < GraphicsPipelistStateArgsIn.RenderTarget.NumRenderTargets; ++itarget)
		{
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].BlendEnable = GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].Enable;
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].SrcBlend = FromBlend(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].SrcColor);
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].DestBlend = FromBlend(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].DstColor);
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].BlendOp = FromBlendOP(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].ColorOP);
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].SrcBlendAlpha = FromBlend(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].SrcAlpha);
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].DestBlendAlpha = FromBlend(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].DstAlpha);
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].BlendOpAlpha = FromBlendOP(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].AlphaOP);
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].LogicOp = D3D12_LOGIC_OP_NOOP;
			GraphicsPipelineStateDesc.BlendState.RenderTarget[itarget].RenderTargetWriteMask = FromWriteMasks(GraphicsPipelistStateArgsIn.Blend.RenderTargets[itarget].WriteMasks);
		}

		GraphicsPipelineStateDesc.DSVFormat = FromFormat(GraphicsPipelistStateArgsIn.DepthStencil.Format);
		GraphicsPipelineStateDesc.DepthStencilState.DepthEnable = GraphicsPipelistStateArgsIn.DepthStencil.DepthTest;
		GraphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = GraphicsPipelistStateArgsIn.DepthStencil.DepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = FromComparison(GraphicsPipelistStateArgsIn.DepthStencil.DepthComparison);
		GraphicsPipelineStateDesc.DepthStencilState.DepthFunc = FromComparison(EComparison::Less);
		GraphicsPipelineStateDesc.DepthStencilState.StencilEnable = GraphicsPipelistStateArgsIn.DepthStencil.StencilTest;

		GraphicsPipelineStateDesc.InputLayout = { InputElementDescs.GetData(), (uint32_t)InputElementDescs.GetSize() };
		GraphicsPipelineStateDesc.PrimitiveTopologyType = FromGeometry(GraphicsPipelistStateArgsIn.TopologyType);

		GraphicsPipelineStateDesc.NumRenderTargets = GraphicsPipelistStateArgsIn.RenderTarget.NumRenderTargets;
		for (uint32_t itarget = 0; itarget < GraphicsPipelistStateArgsIn.RenderTarget.NumRenderTargets; ++itarget)
			GraphicsPipelineStateDesc.RTVFormats[itarget] = FromFormat(GraphicsPipelistStateArgsIn.RenderTarget.Formats[itarget]);

		GraphicsPipelineStateDesc.SampleMask = GraphicsPipelistStateArgsIn.SampleMask;
		GraphicsPipelineStateDesc.SampleDesc.Count = GraphicsPipelistStateArgsIn.RenderTarget.MSAA.Level;
		GraphicsPipelineStateDesc.SampleDesc.Quality = GraphicsPipelistStateArgsIn.RenderTarget.MSAA.Quality;

		GraphicsPipelineStateDesc.RasterizerState.FillMode = GraphicsPipelistStateArgsIn.Rasterize.Wireframe ? D3D12_FILL_MODE::D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE::D3D12_FILL_MODE_SOLID;
		GraphicsPipelineStateDesc.RasterizerState.CullMode = FromCullMode(GraphicsPipelistStateArgsIn.Rasterize.CullMode);
		GraphicsPipelineStateDesc.RasterizerState.FrontCounterClockwise = GraphicsPipelistStateArgsIn.Rasterize.CCW;
		GraphicsPipelineStateDesc.RasterizerState.DepthBias = GraphicsPipelistStateArgsIn.Rasterize.DepthBias;
		GraphicsPipelineStateDesc.RasterizerState.DepthBiasClamp = 0;
		GraphicsPipelineStateDesc.RasterizerState.DepthClipEnable = GraphicsPipelistStateArgsIn.Rasterize.DepthClip;
		GraphicsPipelineStateDesc.RasterizerState.MultisampleEnable = GraphicsPipelistStateArgsIn.Rasterize.MSAA;

		TReferPtr<ID3D12PipelineState> TempD3D12PipelineState;
		HR = Device->CreateGraphicsPipelineState(&GraphicsPipelineStateDesc, __uuidof(ID3D12PipelineState), TempD3D12PipelineState.GetVV());
		if (FAILED(HR))
		{
			LogWarning(Str( " CreateGraphicsPipelineState failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}

		D3D12PipelineState = TempD3D12PipelineState;
		PipelineStateArgs = GraphicsPipelistStateArgsIn;
		return EError::OK;
	}

	void FRHID3D12GraphicsPipelineState::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12PipelineState.Get(), Name);
	}


	EError FRHID3D12ComputePipelineState::Create(FRHIRootSignature * RHIRootSignature, const  FComputePipelineStateDesc & ComputePipelineStateIn)
	{
		if (!RHIRootSignature)
			return EError::Args;

		if (!RHID3D12Device)
			return EError::State;

		auto Device = RHID3D12Device->GetDevice();
		assert(Device);

		HRESULT HR = S_OK;
		FRHID3D12RootSignature * RHID3D12RootSignature = static_cast<FRHID3D12RootSignature *>(RHIRootSignature);
		ID3D12RootSignature * D3D12RootSignatureIn = RHID3D12RootSignature->RootSignature();
		
		TReferPtr<ID3D12PipelineState> TempD3D12PipelineState;
		D3D12_COMPUTE_PIPELINE_STATE_DESC ComputerPipelineStateDesc = {};
		ComputerPipelineStateDesc.pRootSignature = D3D12RootSignatureIn;
		ComputerPipelineStateDesc.CS = D3D12_SHADER_BYTECODE{ ComputePipelineStateIn.CSCode.Data, ComputePipelineStateIn.CSCode.Size };

		HR = Device->CreateComputePipelineState(&ComputerPipelineStateDesc, __uuidof(ID3D12PipelineState), TempD3D12PipelineState.GetVV());
		if (FAILED(HR))
		{
			LogWarning(Str( " CreateComputePipelineState failed, {}"), Win32::ErrorStr(HR & 0xFFFF));
			return EError::Inner;
		}

		D3D12PipelineState = TempD3D12PipelineState;
		ComputePipelineState = ComputePipelineStateIn;
		return EError::OK;
	}

	void FRHID3D12ComputePipelineState::SetName(FStringView Name)
	{
		SetD3D12ObjectName(D3D12PipelineState.Get(), Name);
	}
}
