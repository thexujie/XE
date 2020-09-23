#include "PCH.h"
#include "RHID3D12DescriptorTable.h"
#include "RHID3D12DescriptorHeap.h"
#include "RHID3D12Resource.h"

namespace XE::RHI::D3D12
{
	EError FRHID3D12DescriptorTable::Create(uint32_t NumDescriptors)
	{
		if (!RHID3D12Device)
			return EError::State;

		DescriptorsIndexBase = RHID3D12DescriptorHeap->AllocDescriptors(NumDescriptors);
		DescriptorTableCPUAddress = RHID3D12DescriptorHeap->GetCPUAddress(DescriptorsIndexBase);
		DescriptorTableGPUAddress = RHID3D12DescriptorHeap->GetGPUAddress(DescriptorsIndexBase);
		DescriptorViewArgs.Resize(NumDescriptors, NumDescriptors, EInitializeMode::Default);
		return EError::OK;
	}

	void FRHID3D12DescriptorTable::SetResource(size_t Index, FRHIResource * RHIResource, const FResourceViewArgs & ResourceViewArgsIn)
	{
		if (!RHID3D12Device)
			return;

		HRESULT HR = S_OK;

		TReferPtr<ID3D12Device> Device = RHID3D12Device->GetDevice();
		assert(Device);

		ID3D12DescriptorHeap * D3D12DescriptorHeap = RHID3D12DescriptorHeap->GetD3D12DescriptorHeap();
		assert(D3D12DescriptorHeap);
		
		D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptorHandle = D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptorHandle = D3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		CPUDescriptorHandle.ptr += (DescriptorsIndexBase + Index )* DescriptorsIndexBase;
		GPUDescriptorHandle.ptr += (DescriptorsIndexBase + Index) * DescriptorsIndexBase;
		
		if (ResourceViewArgsIn.Type == EResourceType::ConstBuffer)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferViewDesc = {};
			ConstantBufferViewDesc.BufferLocation = static_cast<FRHID3D12Resource *>(RHIResource)->GetD3D12GPUVirtualAddress();
			ConstantBufferViewDesc.SizeInBytes = (RHIResource->GetSize().Width + 255) & ~255;
			//viewDesc.SizeInBytes = resource->Size().cx;

			Device->CreateConstantBufferView(&ConstantBufferViewDesc, CPUDescriptorHandle);
		}
		else if(ResourceViewArgsIn.Type == EResourceType::ShaderResource)
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc = {};
			ShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			ShaderResourceViewDesc.ViewDimension = FromResourceViewDimension(ResourceViewArgsIn.Resource.ViewDimension, RHIResource->GetResourceArgs().MSAA.Level > 1);
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
			Device->CreateShaderResourceView(static_cast<FRHID3D12Resource *>(RHIResource)->Resource(), &ShaderResourceViewDesc, CPUDescriptorHandle);
		}
		else if (ResourceViewArgsIn.Type == EResourceType::UnorderedAccess)
		{
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

			Device->CreateUnorderedAccessView(static_cast<FRHID3D12Resource *>(RHIResource)->Resource(), nullptr, &UnorderedAccessViewDesc, CPUDescriptorHandle);
		}
		else if (ResourceViewArgsIn.Type == EResourceType::RenderTarget)
		{
			D3D12_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc = {};
			RenderTargetViewDesc.ViewDimension = FromResourceViewDimension_RenderTargetView(ResourceViewArgsIn.Resource.ViewDimension, RHIResource->GetResourceArgs().MSAA.Level > 1);
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
			Device->CreateRenderTargetView(static_cast<FRHID3D12Resource *>(RHIResource)->Resource(), &RenderTargetViewDesc, CPUDescriptorHandle);
		}
		else
		{
		}

		DescriptorViewArgs[Index] = ResourceViewArgsIn;
	}
}
