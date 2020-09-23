#pragma once

#include "../RHI/RHI.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <d3d12.h>

namespace XE::RHI::D3D12
{
	class FRHID3D12Factory;
	class FRHID3D12Device;
	class FRHID3D12Resource;
	class FRHID3D12DescriptorHeap;

	inline void SetD3D12ObjectName(ID3D12Object * D3D12Object, LPCWSTR Name)
	{
		D3D12Object->SetName(Name);
	}

	inline void SetD3D12ObjectName(ID3D12Object * D3D12Object, FStringView Name)
	{
		D3D12Object->SetName(Strings::ToWide(Name).Data);
	}
	
	inline void SetD3D12ObjectName(ID3D12Object * D3D12Object, FWStringView Name)
	{
		D3D12Object->SetName(Name.Data);
	}

	inline FStringView D3D12BlobMessage(ID3DBlob * D3DBlob)
	{
		if (!D3DBlob || D3DBlob->GetBufferSize() < 2)
			return FStringView();

		return D3DBlob ? Strings::ToAuto(FAStringView(static_cast<const char *>(D3DBlob->GetBufferPointer()), D3DBlob->GetBufferSize() - 1)) : FStringView();
	}

	inline FStringView D3D12BlobMessage(TReferPtr<ID3DBlob> D3DBlob)
	{
		if (!D3DBlob || D3DBlob->GetBufferSize() < 2)
			return FStringView();

		return D3DBlob ? Strings::ToAuto(FAStringView(static_cast<const char *>(D3DBlob->GetBufferPointer()), D3DBlob->GetBufferSize() - 1 )) : FStringView();
	}
	
	inline DXGI_SWAP_EFFECT FromSwapEffect(ESwapEffect SwapEffect)
	{
		switch(SwapEffect)
		{
		case ESwapEffect::Discard:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		case ESwapEffect::Sequential:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_SEQUENTIAL;
		case ESwapEffect::FlipSequential:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		case ESwapEffect::FlipDiscard:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
		default:
			return DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
		}
	}

	inline DXGI_FORMAT FromFormat(EFormat Format)
	{
		switch (Format)
		{
		case EFormat::BGRX: return DXGI_FORMAT_B8G8R8X8_UNORM;
		case EFormat::BGRA: return DXGI_FORMAT_B8G8R8A8_UNORM;
		case EFormat::NV12: return DXGI_FORMAT_NV12;
		case EFormat::YUY2: return DXGI_FORMAT_420_OPAQUE;
		case EFormat::P010: return DXGI_FORMAT_P010;
		case EFormat::Float1: return DXGI_FORMAT_R32_FLOAT;
		case EFormat::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case EFormat::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case EFormat::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case EFormat::Int1: return DXGI_FORMAT_R32_SINT;
		case EFormat::Int2: return DXGI_FORMAT_R32G32_SINT;
		case EFormat::Int3: return DXGI_FORMAT_R32G32B32_SINT;
		case EFormat::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
		case EFormat::UInt1: return DXGI_FORMAT_R32_UINT;
		case EFormat::UInt2: return DXGI_FORMAT_R32G32_UINT;
		case EFormat::UInt3: return DXGI_FORMAT_R32G32B32_UINT;
		case EFormat::UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;
		case EFormat::D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		default: return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline EFormat ToFormat(DXGI_FORMAT Format)
	{
		switch (Format)
		{
		case DXGI_FORMAT_B8G8R8X8_UNORM: return EFormat::BGRX;
		case DXGI_FORMAT_B8G8R8A8_UNORM: return EFormat::BGRA;
		case DXGI_FORMAT_NV12: return EFormat::NV12;
		case DXGI_FORMAT_420_OPAQUE: return EFormat::YUY2;
		case DXGI_FORMAT_P010: return EFormat::P010;
		case DXGI_FORMAT_R32_FLOAT: return EFormat::Float1;
		case DXGI_FORMAT_R32G32_FLOAT: return EFormat::Float2;
		case DXGI_FORMAT_R32G32B32_FLOAT: return EFormat::Float3;
		case DXGI_FORMAT_R32G32B32A32_FLOAT: return EFormat::Float4;
		case DXGI_FORMAT_R32_SINT: return EFormat::Int1;
		case DXGI_FORMAT_R32G32_SINT: return EFormat::Int2;
		case DXGI_FORMAT_R32G32B32_SINT: return EFormat::Int3;
		case DXGI_FORMAT_R32G32B32A32_SINT: return EFormat::Int4;
		case DXGI_FORMAT_R32_UINT: return EFormat::UInt1;
		case DXGI_FORMAT_R32G32_UINT: return EFormat::UInt2;
		case DXGI_FORMAT_R32G32B32_UINT: return EFormat::UInt3;
		case DXGI_FORMAT_R32G32B32A32_UINT: return EFormat::UInt4;
		default: return EFormat::None;
		}
	}

	inline D3D12_HEAP_TYPE FromHeapType(EHeapType HeapType)
	{
		switch (HeapType)
		{
		case EHeapType::Default:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		case EHeapType::Upload:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD;
		case EHeapType::Readback:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK;
		default:
			return D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT;
		}
	}

	inline EHeapType ToHeapType(D3D12_HEAP_TYPE HeapType)
	{
		switch (HeapType)
		{
		case D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_DEFAULT:
			return EHeapType::Default;
		case D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD:
			return EHeapType::Upload;
		case D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK:
			return EHeapType::Readback;
		default:
			return EHeapType::Default;
		}
	}

	inline D3D12_COMMAND_LIST_TYPE FromCommandType(ECommandType CommandType)
	{
		switch (CommandType)
		{
		case ECommandType::Direct:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		case ECommandType::Bundle:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case ECommandType::Compute:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case ECommandType::Copy:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}

	inline D3D12_RESOURCE_DIMENSION FromResourceDimension(EResourceDimension ResourceDimension)
	{
		switch (ResourceDimension)
		{
		case EResourceDimension::None:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
		case EResourceDimension::Buffer:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER;
		case EResourceDimension::Texture1D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case EResourceDimension::Texture2D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case EResourceDimension::Texture3D:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default:
			return D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN;
		}
	}

	inline EResourceDimension ToResourceDimension(D3D12_RESOURCE_DIMENSION ResourceDimension)
	{
		switch (ResourceDimension)
		{
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_UNKNOWN:
			return EResourceDimension::None;
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER:
			return EResourceDimension::Buffer;
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			return EResourceDimension::Texture1D;
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			return EResourceDimension::Texture2D;
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			return EResourceDimension::Texture3D;
		default:
			return EResourceDimension::None;
		}
	}

	inline D3D12_RESOURCE_FLAGS FromResourceUsages(EResourceUsages ResourceUsages)
	{
		TFlag<D3D12_RESOURCE_FLAGS> Result;
		Result.Set(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, ResourceUsages.Any(EResourceUsage::RenderTarget));
		Result.Set(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, ResourceUsages.Any(EResourceUsage::DepthStencial));
		Result.Set(D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, ResourceUsages.Any(EResourceUsage::UnorderdResource));
		Result.Set(D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER, ResourceUsages.Any(EResourceUsage::CrossAdapter));
		//Result.Set(D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE, !ResourceUsages.Any(EResourceUsage::ConstBuffer | EResourceUsage::ShaderResource));
		Result.Set(D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE, ResourceUsages.Any(EResourceUsage::DepthStencial));
		return Result;
	}

	inline D3D12_HEAP_FLAGS FromHeapFlags(EHeapFlags HeapFlags)
	{
		TFlag<D3D12_HEAP_FLAGS> Result;
		Result.Set(D3D12_HEAP_FLAG_SHARED, HeapFlags.Any(EHeapFlag::Shader));
		Result.Set(D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH, HeapFlags.Any(EHeapFlag::WriteWatch));
		Result.Set(D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER, HeapFlags.Any(EHeapFlag::CrossAdapter));
		Result.Set(D3D12_HEAP_FLAG_DENY_BUFFERS, HeapFlags.Any(EHeapFlag::DenyBuffers));
		return Result;
	}

	inline D3D12_CPU_PAGE_PROPERTY FromCPUPageProperty(ECPUPageProperty CPUPageProperty)
	{
		switch(CPUPageProperty)
		{
		case ECPUPageProperty::None:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		case ECPUPageProperty::NA:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
		case ECPUPageProperty::WriteCombine:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
		case ECPUPageProperty::WriteBack:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		default:
			return D3D12_CPU_PAGE_PROPERTY::D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		}
	}

	inline D3D12_MEMORY_POOL FromMemoryPool(EMemoryPool MemoryPool)
	{
		switch(MemoryPool)
		{
		case EMemoryPool::None:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		case EMemoryPool::L0:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L0;
		case EMemoryPool::L1:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_L1;
		default:
			return D3D12_MEMORY_POOL::D3D12_MEMORY_POOL_UNKNOWN;
		}
	}

	inline D3D12_RESOURCE_STATES FromResourceStates(EResourceStates ResourceStates)
	{
		if (ResourceStates == EResourceStates::GenericRead)
			return D3D12_RESOURCE_STATE_GENERIC_READ;
		
		TFlag<D3D12_RESOURCE_STATES> Result;
		Result.Set(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ResourceStates * (EResourceStates::VertexBuffer | EResourceStates::ConstBuffer));
		Result.Set(D3D12_RESOURCE_STATE_INDEX_BUFFER, ResourceStates * EResourceStates::IndexBuffer);
		Result.Set(D3D12_RESOURCE_STATE_RENDER_TARGET, ResourceStates * EResourceStates::RenderTarget);
		Result.Set(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, ResourceStates * EResourceStates::UnorderedAccess);
		Result.Set(D3D12_RESOURCE_STATE_DEPTH_WRITE, ResourceStates * EResourceStates::DepthWrite);
		Result.Set(D3D12_RESOURCE_STATE_DEPTH_READ, ResourceStates * EResourceStates::DepthRead);
		Result.Set(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, ResourceStates * (
			EResourceStates::VertexShaderResource |
			EResourceStates::HullShaderResource |
			EResourceStates::DomainShaderResource |
			EResourceStates::GeometryShaderResource |
			EResourceStates::ComputerShaderResource));
		Result.Set(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, ResourceStates * EResourceStates::PixelShaderResource);
		Result.Set(D3D12_RESOURCE_STATE_COPY_SOURCE, ResourceStates * EResourceStates::CopySource);
		Result.Set(D3D12_RESOURCE_STATE_COPY_DEST, ResourceStates * EResourceStates::CopyDest);
		Result.Set(D3D12_RESOURCE_STATE_RESOLVE_SOURCE, ResourceStates * EResourceStates::ResolveSource);
		Result.Set(D3D12_RESOURCE_STATE_RESOLVE_DEST, ResourceStates * EResourceStates::ResolveDest);
		Result.Set(D3D12_RESOURCE_STATE_PRESENT, ResourceStates * EResourceStates::Present);
		return Result;
	}

	inline D3D12_SRV_DIMENSION FromResourceViewDimension(EResourceViewDimension ResourceViewDimension, bool MSAA = false)
	{
		switch(ResourceViewDimension)
		{
		case EResourceViewDimension::None: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN;
		case EResourceViewDimension::Buffer: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER;
		case EResourceViewDimension::Texture1D: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D;
		case EResourceViewDimension::Texture1DArray: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
		case EResourceViewDimension::Texture2D: 
			return MSAA ? D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMS : D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D;
		case EResourceViewDimension::Texture2DArray:
			return MSAA ? D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		case EResourceViewDimension::Texture3D: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D;
		case EResourceViewDimension::TextureCube: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBE;
		case EResourceViewDimension::TextureCubeArray: 
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
		default:
			return D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN;
		}
	}

	inline EResourceViewDimension ToResourceViewDimension(D3D12_SRV_DIMENSION ResourceViewDimension, bool MSAA = false)
	{
		switch (ResourceViewDimension)
		{
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_UNKNOWN:
			return EResourceViewDimension::None;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_BUFFER:
			return EResourceViewDimension::Buffer;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1D:
			return EResourceViewDimension::Texture1D;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
			return EResourceViewDimension::Texture1DArray;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2D:
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMS:
			return EResourceViewDimension::Texture2D;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
			return EResourceViewDimension::Texture2DArray;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURE3D:
			return EResourceViewDimension::Texture3D;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBE:
			return EResourceViewDimension::TextureCube;
		case D3D12_SRV_DIMENSION::D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
			return EResourceViewDimension::TextureCubeArray;
		default:
			return EResourceViewDimension::None;
		}
	}

	inline D3D12_RTV_DIMENSION FromResourceViewDimension_RenderTargetView(EResourceViewDimension ResourceViewDimension, bool MSAA = false)
	{
		switch (ResourceViewDimension)
		{
		case EResourceViewDimension::None:
			return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_UNKNOWN;
		case EResourceViewDimension::Buffer:
			return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_BUFFER;
		case EResourceViewDimension::Texture1D:
			return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE1D;
		case EResourceViewDimension::Texture1DArray:
			return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
		case EResourceViewDimension::Texture2D:
			return MSAA ? D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DMS : D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DMS;
		case EResourceViewDimension::Texture2DArray:
			return MSAA ? D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
		case EResourceViewDimension::Texture3D:
			return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_TEXTURE3D;
		default:
			return D3D12_RTV_DIMENSION::D3D12_RTV_DIMENSION_UNKNOWN;
		}
	}

	inline D3D12_DSV_DIMENSION FromResourceViewDimension_DepthStencilView(EResourceViewDimension ResourceViewDimension, bool MSAA = false)
	{
		switch (ResourceViewDimension)
		{
		case EResourceViewDimension::None:
			return D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_UNKNOWN;
		case EResourceViewDimension::Texture1D:
			return D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE1D;
		case EResourceViewDimension::Texture1DArray:
			return D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
		case EResourceViewDimension::Texture2D:
			return MSAA ? D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2D : D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2DMS;
		case EResourceViewDimension::Texture2DArray:
			return MSAA ? D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2DARRAY : D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
		default:
			return D3D12_DSV_DIMENSION::D3D12_DSV_DIMENSION_UNKNOWN;
		}
	}

	inline D3D12_UAV_DIMENSION FromResourceViewDimension_UAV(EResourceViewDimension ResourceViewDimension)
	{
		switch (ResourceViewDimension)
		{
		case EResourceViewDimension::None:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_UNKNOWN;
		case EResourceViewDimension::Buffer:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_BUFFER;
		case EResourceViewDimension::Texture1D:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE1D;
		case EResourceViewDimension::Texture1DArray:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
		case EResourceViewDimension::Texture2D:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2D;
		case EResourceViewDimension::Texture2DArray:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
		case EResourceViewDimension::Texture3D:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_TEXTURE3D;
		default:
			return D3D12_UAV_DIMENSION::D3D12_UAV_DIMENSION_UNKNOWN;
		}
	}

	inline D3D12_DESCRIPTOR_HEAP_FLAGS FromDescriptorHeapFlags(EDescriptorHeapFlags ResourcePacketFlags)
	{
		TFlag<D3D12_DESCRIPTOR_HEAP_FLAGS> Result;
		Result.Set(D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, ResourcePacketFlags * EDescriptorHeapFlags::ShaderInvisible);
		return Result;
	}

	inline D3D12_DESCRIPTOR_RANGE_TYPE FromDescriptorRangeType(EDescriptorType DescriptorType)
	{
		switch(DescriptorType)
		{
		case EDescriptorType::ConstBuffer:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case EDescriptorType::ShaderResource:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case EDescriptorType::UnorderedAccess:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case EDescriptorType::Sampler:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default:
			return D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		}
	}

	inline D3D12_ROOT_PARAMETER_TYPE FromRootPipelineParameterType(EDescriptorType DescriptorType)
	{
		switch (DescriptorType)
		{
		case EDescriptorType::DescriptorTable:
			return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		case EDescriptorType::UInt32Constant:
			return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		case EDescriptorType::ConstBuffer:
			return D3D12_ROOT_PARAMETER_TYPE_CBV;
		case EDescriptorType::ShaderResource:
			return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EDescriptorType::UnorderedAccess:
			return D3D12_ROOT_PARAMETER_TYPE_UAV;
		default:
			return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		}
	}

	inline D3D12_SHADER_VISIBILITY FromShaderType(EShaderType Shader)
	{
		switch (Shader)
		{
		case EShaderType::All:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
		case EShaderType::Vertex:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_VERTEX;
		case EShaderType::Hull:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_HULL;
		case EShaderType::Domain:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_DOMAIN;
		case EShaderType::Geoetry:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_GEOMETRY;
		case EShaderType::Pixel:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_PIXEL;
		default:
			return D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
		}
	}

	inline D3D12_SHADER_VISIBILITY FromShaderMask(EShaderMasks ShaderMasks)
	{
		if (GetFlagAll(ShaderMasks, EShaderMasks::All))
			return D3D12_SHADER_VISIBILITY_ALL;
		
		TFlag<D3D12_SHADER_VISIBILITY> D3D12ShaderVisibility;
		D3D12ShaderVisibility.Set(D3D12_SHADER_VISIBILITY_VERTEX, ShaderMasks * EShaderMasks::Vertx);
		D3D12ShaderVisibility.Set(D3D12_SHADER_VISIBILITY_HULL, ShaderMasks * EShaderMasks::Hull);
		D3D12ShaderVisibility.Set(D3D12_SHADER_VISIBILITY_DOMAIN, ShaderMasks * EShaderMasks::Domain);
		D3D12ShaderVisibility.Set(D3D12_SHADER_VISIBILITY_GEOMETRY, ShaderMasks * EShaderMasks::Geoetry);
		D3D12ShaderVisibility.Set(D3D12_SHADER_VISIBILITY_PIXEL, ShaderMasks * EShaderMasks::Pixel);
		return D3D12ShaderVisibility;
	}

	inline D3D12_BLEND FromBlend(EBlend Blend)
	{
		switch(Blend)
		{
		case EBlend::None:
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		case EBlend::Zero:
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		case EBlend::One:
			return D3D12_BLEND::D3D12_BLEND_ONE;
		case EBlend::SrcColor:
			return D3D12_BLEND::D3D12_BLEND_SRC_COLOR;
		case EBlend::SrcColorInv:
			return D3D12_BLEND::D3D12_BLEND_INV_SRC_COLOR;
		case EBlend::SrcAlpha:
			return D3D12_BLEND::D3D12_BLEND_SRC_ALPHA;
		case EBlend::SrcAlphaInv:
			return D3D12_BLEND::D3D12_BLEND_INV_SRC_ALPHA;
		case EBlend::DestAlpha:
			return D3D12_BLEND::D3D12_BLEND_DEST_ALPHA;
		case EBlend::DestAlphaInv:
			return D3D12_BLEND::D3D12_BLEND_INV_DEST_ALPHA;
		case EBlend::DestColor:
			return D3D12_BLEND::D3D12_BLEND_DEST_COLOR;
		case EBlend::DestColorInv:
			return D3D12_BLEND::D3D12_BLEND_INV_DEST_COLOR;
		case EBlend::BlendFactor:
			return D3D12_BLEND::D3D12_BLEND_BLEND_FACTOR;
		case EBlend::BlendFactorInv:
			return D3D12_BLEND::D3D12_BLEND_INV_BLEND_FACTOR;
		default:
			return D3D12_BLEND::D3D12_BLEND_ZERO;
		}
	}

	inline D3D12_BLEND_OP FromBlendOP(EBlendOP BlendOP)
	{
		switch(BlendOP)
		{
		case EBlendOP::Add:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		case EBlendOP::Subtract:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_SUBTRACT;
		case EBlendOP::Min:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_MIN;
		case EBlendOP::Max:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_MAX;
		default:
			return D3D12_BLEND_OP::D3D12_BLEND_OP_ADD;
		}
	}

	inline D3D12_CULL_MODE FromCullMode(ECullMode CullMode)
	{
		switch(CullMode)
		{
		case ECullMode::None:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		case ECullMode::Front:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_FRONT;
		case ECullMode::Back:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_BACK;
		default:
			return D3D12_CULL_MODE::D3D12_CULL_MODE_NONE;
		}
	}

	inline D3D12_COLOR_WRITE_ENABLE FromWriteMasks(EWriteMasks WriteMasks)
	{
		TFlag<D3D12_COLOR_WRITE_ENABLE> Result;
		Result.Set(D3D12_COLOR_WRITE_ENABLE_RED, WriteMasks * EWriteMasks::Red);
		Result.Set(D3D12_COLOR_WRITE_ENABLE_GREEN, WriteMasks * EWriteMasks::Green);
		Result.Set(D3D12_COLOR_WRITE_ENABLE_BLUE, WriteMasks * EWriteMasks::Blue);
		Result.Set(D3D12_COLOR_WRITE_ENABLE_ALPHA, WriteMasks * EWriteMasks::Alpha);
		return Result;
	}

	inline D3D12_PRIMITIVE_TOPOLOGY FromTopology(ETopology Topology)
	{
		switch(Topology)
		{
		case ETopology::None: 
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case ETopology::PointList:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case ETopology::LineList:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case ETopology::LineStrip:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case ETopology::TriangleList: 
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case ETopology::TriangleStrip:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case ETopology::Point1PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
		case ETopology::Point2PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
		case ETopology::Point3PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		case ETopology::Point4PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		case ETopology::Point5PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
		case ETopology::Point6PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
		case ETopology::Point7PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
		case ETopology::Point8PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
		case ETopology::Point9PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
		case ETopology::Point10PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
		case ETopology::Point11PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
		case ETopology::Point12PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
		case ETopology::Point13PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
		case ETopology::Point14PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
		case ETopology::Point15PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
		case ETopology::Point16PatchList: return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
		default:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}
	
	inline D3D12_PRIMITIVE_TOPOLOGY_TYPE FromGeometry(EGeometry Geometry)
	{
		switch(Geometry)
		{
		case EGeometry::None:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		case EGeometry::Point:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case EGeometry::Line:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case EGeometry::Triangle:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case EGeometry::Patch:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE::D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}
	}

	inline D3D12_FILTER FromFilter(EFilter Filter)
	{
		switch (Filter)
		{
		case EFilter::Point:
			return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case EFilter::Bilinear:
			return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		default:
			return D3D12_FILTER::D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		}
	}

	inline D3D12_TEXTURE_ADDRESS_MODE FromAddressMode(EAddressMode AddressMode)
	{
		switch(AddressMode)
		{
		case EAddressMode::Clamp: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case EAddressMode::Wrap: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case EAddressMode::Mirror: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case EAddressMode::Border: 
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		default:
			return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		}
	}

	inline D3D12_COMPARISON_FUNC FromComparison(EComparison Comparison)
	{
		switch(Comparison)
		{
		case EComparison::None: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		case EComparison::Always: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
		case EComparison::Less: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
		case EComparison::LessEqual: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case EComparison::Equal: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
		case EComparison::GreaterEqual: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case EComparison::Greater: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
		case EComparison::NotEqual: 
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default:
			return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
		}
	}

	inline D3D12_ROOT_DESCRIPTOR_FLAGS FromRootDescriptorFlags(EDescriptorFlags DescriptorFlags)
	{
		TFlag<D3D12_ROOT_DESCRIPTOR_FLAGS> Result;
		Result.Set(D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE, DescriptorFlags * EDescriptorFlags::Volatile);
		Result.Set(D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, DescriptorFlags * EDescriptorFlags::Static);
		return Result;
	}
	
	inline D3D12_DESCRIPTOR_RANGE_FLAGS FromDescriptorFlags(EDescriptorFlags DescriptorFlags)
	{
		TFlag<D3D12_DESCRIPTOR_RANGE_FLAGS> Result;
		Result.Set(D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE, DescriptorFlags * EDescriptorFlags::DescriptorVolatile);
		Result.Set(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE, DescriptorFlags * EDescriptorFlags::Volatile);
		Result.Set(D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC, DescriptorFlags * EDescriptorFlags::Static);
		return Result;
	}

	inline D3D12_FENCE_FLAGS FromFenceFlags(EFenceFlags FenceFlags)
	{
		TFlag<D3D12_FENCE_FLAGS> Result;
		Result.Set(D3D12_FENCE_FLAG_SHARED, FenceFlags * EFenceFlags::Shared);
		return Result;
	}
}

#define RHID3D12_SETNAME(x) XE::RHI::D3D12::SetD3D12ObjectName((x).Get(), L#x)
