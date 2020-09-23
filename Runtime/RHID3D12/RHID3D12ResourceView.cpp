#include "PCH.h"
#include "RHID3D12ResourceView.h"
#include "RHID3D12Resource.h"
#include "RHID3D12Device.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12ResourceView::Create(FRHIResource * RHIResourceIn, const FResourceViewArgs & ResourceViewArgsIn)
	{
		TReferPtr<ID3D12Device> D3D12Device = RHID3D12Device->GetDevice();
		assert(D3D12Device);

		FAddress Address = {};
		if (ResourceViewArgsIn.Type == EResourceType::ConstBuffer)
		{
			Address = RHID3D12Device->AllocateDescriptor(EDescriptorHeapType::ShaderResource);
			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc = {};
			ConstantBufferViewDesc.BufferLocation = static_cast<FRHID3D12Resource *>(RHIResourceIn)->GetD3D12GPUVirtualAddress();
			ConstantBufferViewDesc.SizeInBytes = (RHIResourceIn->GetSize().Width + 255) & ~255;
			//viewDesc.SizeInBytes = resource->Size().cx;

			D3D12Device->CreateConstantBufferView(&ConstantBufferViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ Address.CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::ShaderResource)
		{
			Address = RHID3D12Device->AllocateDescriptor(EDescriptorHeapType::ShaderResource);
			D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc = {};
			ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			ShaderResourceViewDesc.ViewDimension = FromResourceViewDimension(ResourceViewArgsIn.Resource.ViewDimension, RHIResourceIn->GetResourceArgs().MSAA.Level > 1);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Buffer)
			{
				ShaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				ShaderResourceViewDesc.Buffer.FirstElement = ResourceViewArgsIn.Resource.Buffer.FirstElement;
				ShaderResourceViewDesc.Buffer.NumElements = ResourceViewArgsIn.Resource.Buffer.NumElements;
				ShaderResourceViewDesc.Buffer.StructureByteStride = ResourceViewArgsIn.Resource.Buffer.Stride;
				ShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			}
			else if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				ShaderResourceViewDesc.Format = FromFormat(ResourceViewArgsIn.Resource.Format);
				ShaderResourceViewDesc.Texture2D.MipLevels = 1;
			}
			else
			{

			}
			D3D12Device->CreateShaderResourceView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), &ShaderResourceViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ Address.CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::UnorderedAccess)
		{
			Address = RHID3D12Device->AllocateDescriptor(EDescriptorHeapType::ShaderResource);
			D3D12_UNORDERED_ACCESS_VIEW_DESC UnorderedAccessViewDesc = {};
			UnorderedAccessViewDesc.Format = /*FromFormat(args.resource.format)*/DXGI_FORMAT_UNKNOWN;
			UnorderedAccessViewDesc.ViewDimension = FromResourceViewDimension_UAV(ResourceViewArgsIn.Resource.ViewDimension);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Buffer)
			{
				UnorderedAccessViewDesc.Buffer.FirstElement = ResourceViewArgsIn.Resource.Buffer.FirstElement;
				UnorderedAccessViewDesc.Buffer.NumElements = ResourceViewArgsIn.Resource.Buffer.NumElements;
				UnorderedAccessViewDesc.Buffer.StructureByteStride = ResourceViewArgsIn.Resource.Buffer.Stride;
				UnorderedAccessViewDesc.Buffer.CounterOffsetInBytes = 0;
				UnorderedAccessViewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			}
			else if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				UnorderedAccessViewDesc.Texture2D.MipSlice = 0;
				UnorderedAccessViewDesc.Texture2D.PlaneSlice = 0;
			}
			else
			{

			}

			D3D12Device->CreateUnorderedAccessView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), nullptr, &UnorderedAccessViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ Address.CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::RenderTarget)
		{
			Address = RHID3D12Device->AllocateDescriptor(EDescriptorHeapType::RenderTarget);
			D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
			RenderTargetViewDesc.ViewDimension = FromResourceViewDimension_RenderTargetView(ResourceViewArgsIn.Resource.ViewDimension, RHIResourceIn->GetResourceArgs().MSAA.Level > 1);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Buffer)
			{
				RenderTargetViewDesc.Format = DXGI_FORMAT_UNKNOWN;
				RenderTargetViewDesc.Buffer.FirstElement = ResourceViewArgsIn.Resource.Buffer.FirstElement;
				RenderTargetViewDesc.Buffer.NumElements = ResourceViewArgsIn.Resource.Buffer.NumElements;
			}
			else if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				RenderTargetViewDesc.Format = FromFormat(ResourceViewArgsIn.Resource.Format);
				RenderTargetViewDesc.Texture2D.MipSlice = 0;
				RenderTargetViewDesc.Texture2D.PlaneSlice = 0;
			}
			else
			{

			}
			D3D12Device->CreateRenderTargetView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), &RenderTargetViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ Address.CPUAddress });
		}
		else if (ResourceViewArgsIn.Type == EResourceType::DepthStencil)
		{
			Address = RHID3D12Device->AllocateDescriptor(EDescriptorHeapType::DepthStencil);
			D3D12_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
			DepthStencilViewDesc.ViewDimension = FromResourceViewDimension_DepthStencilView(ResourceViewArgsIn.Resource.ViewDimension, RHIResourceIn->GetResourceArgs().MSAA.Level > 1);
			if (ResourceViewArgsIn.Resource.ViewDimension == EResourceViewDimension::Texture2D)
			{
				DepthStencilViewDesc.Format = FromFormat(ResourceViewArgsIn.Resource.Format);
				DepthStencilViewDesc.Texture2D.MipSlice = 0;
			}
			else
			{
				throw EError::BadFormat;
			}
			D3D12Device->CreateDepthStencilView(static_cast<FRHID3D12Resource *>(RHIResourceIn)->Resource(), &DepthStencilViewDesc, D3D12_CPU_DESCRIPTOR_HANDLE{ Address.CPUAddress });
		}
		else
		{
			throw EError::BadFormat;
		}

		ResourceViewArgs = ResourceViewArgsIn;
		RHIResource = RHIResourceIn;
		D3D12CPUDescriptorHandle = D3D12_CPU_DESCRIPTOR_HANDLE{ Address.CPUAddress };
		D3D12GPUDescriptorHandle = D3D12_GPU_DESCRIPTOR_HANDLE{ Address.GPUAddress };
		return EError::OK;
	}
}
