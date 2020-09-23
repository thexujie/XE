#include "PCH.h"
#include "PipelineState.h"

#include "InputLayout.h"
#include "Material.h"
#include "MeshAssembler.h"
#include "RenderFactory.h"
#include "RenderTarget.h"
#include "Shader.h"


namespace XE::Render
{
	void FPipelineState::InitializeRHI(RHI::FRHIDevice & RHIDevice, RHI::FRHIRootSignature * RHIRootSignature)
	{
		RHI::FGraphicsPipelistStateDesc GraphicsPipelistStateDesc = {};
		GraphicsPipelistStateDesc.TopologyType = MeshAssembler.GetGeometry();
		GraphicsPipelistStateDesc.VSCode = MeshAssembler.GetVertexShader() ? MeshAssembler.GetVertexShader()->GetCode() : TView<byte_t>();
		GraphicsPipelistStateDesc.HSCode = MeshAssembler.GetHullShader() ? MeshAssembler.GetHullShader()->GetCode() : TView<byte_t>();
		GraphicsPipelistStateDesc.DSCode = MeshAssembler.GetDomainShader() ? MeshAssembler.GetDomainShader()->GetCode() : TView<byte_t>();
		GraphicsPipelistStateDesc.GSCode = MeshAssembler.GetGeometryShader() ? MeshAssembler.GetGeometryShader()->GetCode() : TView<byte_t>();
		GraphicsPipelistStateDesc.PSCode = Material.GetPixelShader() ? Material.GetPixelShader()->GetCode() : TView<byte_t>();
		
		for (const FVertexElement & VertexElement : InputLayout.GetVertexElements())
		{
			RHI::FInputElement InputElement;
			InputElement.Name = VertexElement.Name;
			InputElement.Index = VertexElement.StreamIndex;
			InputElement.Format = VertexElement.Format;
			InputElement.Offset = VertexElement.Offset;
			GraphicsPipelistStateDesc.Elements.Add(InputElement);
		}

		GraphicsPipelistStateDesc.RenderTarget.NumRenderTargets = (uint32_t)OutputLayout.GetRenderTargetForamts().Size;
		GraphicsPipelistStateDesc.RenderTarget.Formats = OutputLayout.GetRenderTargetForamts();
		GraphicsPipelistStateDesc.RenderTarget.MSAA = OutputLayout.GetMSAA();

		GraphicsPipelistStateDesc.DepthStencil.Format = OutputLayout.GetDepthStencilFormat();
		GraphicsPipelistStateDesc.DepthStencil.DepthTest = OutputLayout.GetDepthTest();
		GraphicsPipelistStateDesc.DepthStencil.DepthWrite = OutputLayout.GetDepthWrite();
		GraphicsPipelistStateDesc.DepthStencil.DepthComparison = OutputLayout.GetDepthComparison();
		
		GraphicsPipelistStateDesc.Rasterize.Wireframe = Material.IsWireFrame();
		GraphicsPipelistStateDesc.Rasterize.CullMode = Material.GetCullMode();
		GraphicsPipelistStateDesc.Rasterize.DepthClip = Material.IsDepthClip();
		
		RHIPipelineState = RHIDevice.CreateGraphicsPipelineState(RHIRootSignature, GraphicsPipelistStateDesc);
	}
}
