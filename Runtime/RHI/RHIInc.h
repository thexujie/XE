#pragma once

#include "../Core/Core.h"

#if XE_WINDOWS
#ifdef XEngine_RHI_Module
#define RHI_API __declspec(dllexport)
#else
#define RHI_API __declspec(dllimport)
#endif
#endif

namespace XE::RHI
{
	using FGPUAddress = addr_t;
	using FCPUAddress = addr_t;
	using FDescriptorIndex = uintx_t;

	struct FAddress
	{
		FCPUAddress CPUAddress = NullAddress;
		FGPUAddress GPUAddress = NullAddress;
		size_t AddressUnit = 0;
	};

	struct FViewPort
	{
		float Left = 0;
		float Top = 0;
		float Width = 0;
		float Height = 0;
		float NearZ = 0;
		float FarZ = 0;
	};
	
	class RHI_API FRHIObject
	{
	public:
		FRHIObject() = default;
		virtual ~FRHIObject() = default;
		virtual void SetName(FStringView name) {}
	};

	enum class ECommandQueueFlags
	{
		None = 0,
		DisableGPUTimeout = 0x0001,
	};

	template struct TFlag<ECommandQueueFlags>;


	enum class EShaderType
	{
		Vertex = 0,
		Hull,
		Domain,
		Geoetry,
		Pixel,
		Compute,
		All
	};
	constexpr size_t ShaderTypeCount = size_t(EShaderType::All) + 1;

	enum class EShaderMasks
	{
		None = 0,
		Vertx = 0x0001,
		Hull = 0x0002,
		Domain = 0x0004,
		Geoetry = 0x0008,
		Pixel = 0x0010,
		Compute = 0x0020,
		All = 0x003f
	};
	constexpr size_t ShaderMaskCount = size_t(EShaderType::All) + 1;
	template struct TFlag<EShaderMasks>;

	inline EShaderMasks ShaderMasksFromShaderType(EShaderType ShaderType)
	{
		switch (ShaderType)
		{
		case EShaderType::All: return EShaderMasks::All;
		case EShaderType::Vertex: return EShaderMasks::Vertx;
		case EShaderType::Hull: return EShaderMasks::Hull;
		case EShaderType::Domain: return EShaderMasks::Domain;
		case EShaderType::Geoetry: return EShaderMasks::Geoetry;
		case EShaderType::Pixel: return EShaderMasks::Pixel;
		case EShaderType::Compute: return EShaderMasks::Compute;
		default: return EShaderMasks::None;
		}
	}

	enum class EDescriptorType
	{
		UInt32Constant = 0,
		ConstBuffer,
		ShaderResource,
		UnorderedAccess,
		Sampler,
		DescriptorTable,
	};
	constexpr size_t DescriptorTypeCount = size_t(EDescriptorType::DescriptorTable) + 1;

	enum class EDescriptorFlags
	{
		None = 0,
		Static = 0x0001,
		Volatile = 0x0002,
		DescriptorVolatile = 0x0004,
	};
	template struct TFlag<EDescriptorFlags>;

	enum class EFilter
	{
		Point = 0,
		Bilinear,
		Trilinear,
		Anisotropic,
	};

	enum class EAddressMode
	{
		Clamp = 0,
		Wrap,
		Mirror,
		Border,
	};

	enum class EComparison
	{
		None = 0,
		Always,
		Less,
		LessEqual,
		Equal,
		GreaterEqual,
		Greater,
		NotEqual,
	};

	enum class EBlend
	{
		None = 0,
		Zero,
		One,
		SrcColor,
		SrcColorInv,
		SrcAlpha,
		SrcAlphaInv,
		DestColor,
		DestColorInv,
		DestAlpha,
		DestAlphaInv,
		BlendFactor,
		BlendFactorInv,
	};

	enum class EBlendOP
	{
		Add = 0,
		Subtract,
		Min,
		Max,
	};

	enum class ECullMode
	{
		None = 0,
		Front,
		Back
	};
	enum class ETopology
	{
		None = 0,
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		Point1PatchList,
		Point2PatchList,
		Point3PatchList,
		Point4PatchList,
		Point5PatchList,
		Point6PatchList,
		Point7PatchList,
		Point8PatchList,
		Point9PatchList,
		Point10PatchList,
		Point11PatchList,
		Point12PatchList,
		Point13PatchList,
		Point14PatchList,
		Point15PatchList,
		Point16PatchList,
	};

	enum class EGeometry
	{
		None = 0,
		Point,
		Line,
		Triangle,
		Patch,
	};


	enum class EResourceViewDimension
	{
		None = 0,
		Buffer,
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
		TextureCubeArray,
	};

	enum class EDescriptorHeapType
	{
		ShaderResource = 0,
		RenderTarget,
		DepthStencil,
		Sampler,
	};
	constexpr size_t DescriptorHeapTypeCount = size_t(EDescriptorHeapType::Sampler) + 1;

	enum class EResourceType
	{
		None = 0,
		ConstBuffer,
		ShaderResource,
		UnorderedAccess,
		RenderTarget,
		DepthStencil,
	};

	enum class ECommandType
	{
		Direct = 0,
		Bundle,
		Compute,
		Copy,
	};
	constexpr size_t ECommandTypeCount = size_t(ECommandType::Copy) + 1;

	enum class ESwapEffect
	{
		Discard = 0,
		Sequential,
		FlipSequential,
		FlipDiscard,
	};

	enum class EHeapType
	{
		Default = 0,
		Upload,
		Readback,
	};

	enum class ECPUPageProperty
	{
		None = 0,
		NA,
		WriteCombine,
		WriteBack,
	};

	enum class EMemoryPool
	{
		None,
		L0,
		L1,
	};

	enum class EResourceDimension
	{
		None = 0,
		Buffer,
		Texture1D,
		Texture2D,
		Texture3D,
	};

	enum class EResourceUsage
	{
		None = 0,
		RenderTarget = 0x1,
		DepthStencial = 0x2,
		CrossAdapter = 0x8,

		ConstBuffer = 0x10,
		ShaderResource = 0x20,
		UnorderdResource = 0x40,

		VertexBuffer = 0x100,
		IndexBuffer = 0x200,
	};
	using EResourceUsages = TFlag<EResourceUsage>;

	enum class EHeapFlag
	{
		None = 0,
		Shader = 0x1,
		WriteWatch = 0x2,
		CrossAdapter = 0x4,
		DenyBuffers = 0x1000,
	};
	using EHeapFlags = TFlag<EHeapFlag>;

	enum class ERHIAdapterFlags
	{
		None = 0,
		CacheCoherentUMA = 0x1,
		TileBasedRender = 0x2,
		Software = 0x40,
	};
	template struct TFlag<ERHIAdapterFlags>;

	enum class EResourceStates
	{
		None = 0,
		VertexBuffer = 0x1,
		IndexBuffer = 0x2,
		ConstBuffer = 0x4,
		RenderTarget = 0x8,
		UnorderedAccess = 0x10,
		DepthWrite = 0x20,
		DepthRead = 0x40,

		VertexShaderResource = 0x100,
		HullShaderResource = 0x200,
		DomainShaderResource = 0x400,
		GeometryShaderResource = 0x800,
		PixelShaderResource = 0x1000,
		ComputerShaderResource = 0x2000,

		NonePixelShaderResource = VertexShaderResource | HullShaderResource | DomainShaderResource | GeometryShaderResource | ComputerShaderResource,

		CopySource = 0x10000,
		CopyDest = 0x20000,
		ResolveSource = 0x40000,
		ResolveDest = 0x80000,

		Present = 0x100000,
		GenericRead = 0x200000,
	};

	template struct TFlag<EResourceStates>;

	enum class EWriteMasks
	{
		None = 0,
		Red = 0x1,
		Green = 0x2,
		Blue = 0x4,
		Alpha = 0x8,
		All = Red | Green | Blue | Alpha,
	};
	template struct TFlag<EWriteMasks>;


	union FResourceClearValue
	{
		FColor Color = {};
		struct
		{
			float32_t Depth;
			uint8_t Stencial;
		};
	};

	struct FMSAA
	{
		uint32_t Level = 1;
		uint32_t Quality = 0;

		auto operator <=>(const FMSAA &) const noexcept = default;
		uintx_t Hash() const noexcept
		{
			return HashJoin(Level, Quality);
		}
	};
	
	struct FRHIAdapterDesc
	{
		uint64_t DeviceId = 0;
		FString Name;
		ERHIAdapterFlags Flags = ERHIAdapterFlags::None;
	};


	class RHI_API FRHIDeviceObject : public FRHIObject
	{
	public:
		FRHIDeviceObject() = default;
		virtual ~FRHIDeviceObject() = default;
	};
	
	struct FResourceDesc
	{
		struct
		{
			EHeapType HeapType = EHeapType::Default;
			EHeapFlags HeapFlags = nullptr;
			ECPUPageProperty CPUPageProperty = ECPUPageProperty::None;
			EMemoryPool MemoryPool = EMemoryPool::None;
		}Heap;

		EResourceDimension Dimension = EResourceDimension::None;
		EResourceUsages Usages = nullptr;
		EFormat Format = EFormat::None;

		uint32_t Alignment = 0;
		SizeU Size = {0, 1};
		uint16_t Depth = 1;
		uint16_t MipLevels = 1;
		FMSAA MSAA;

		EResourceStates States = EResourceStates::None;

		FColor ClearColor = {};
		float32_t ClearDepth = 0.0f;
		uint8_t CleanStencial = 0;

		static FResourceDesc Buffer(EHeapType HeapType, size_t Size)
		{
			FResourceDesc ResourceArgs = {};
			ResourceArgs.Heap.HeapType = HeapType;
			ResourceArgs.Dimension = EResourceDimension::Buffer;
			ResourceArgs.Usages = EResourceUsage::ConstBuffer;
			ResourceArgs.Size = { (uint32_t)Size, 1 };
			ResourceArgs.States = EResourceStates::GenericRead;
			return ResourceArgs;
		}
	};
	
	enum class EDescriptorHeapFlags
	{
		None = 0,
		ShaderInvisible = 0x0001,
	};
	template struct TFlag<EDescriptorHeapFlags>;

	struct FResourceViewArgs
	{
		EResourceType Type = EResourceType::None;
		struct
		{
			EFormat Format = EFormat::None;
			EResourceViewDimension ViewDimension = EResourceViewDimension::None;
			uint32_t MipLevels = 1;
			struct
			{
				uint32_t FirstElement = 0;
				uint32_t NumElements = 0;
				uint32_t Stride = 0;
			}Buffer;
		}Resource;

		static FResourceViewArgs CBuffer()
		{
			FResourceViewArgs ResourceViewArgs = {};
			ResourceViewArgs.Type = EResourceType::ConstBuffer;
			return ResourceViewArgs;
		}
		
		static FResourceViewArgs Unordered(uint32_t Stride, uint32_t NumElements, EFormat Format)
		{
			FResourceViewArgs ResourceViewArgs = {};
			ResourceViewArgs.Type = EResourceType::UnorderedAccess;
			ResourceViewArgs.Resource.Format = Format;
			ResourceViewArgs.Resource.ViewDimension = EResourceViewDimension::Buffer;
			ResourceViewArgs.Resource.Buffer.NumElements = NumElements;
			ResourceViewArgs.Resource.Buffer.Stride = Stride;
			return ResourceViewArgs;
		}

		static FResourceViewArgs Texture2D(EFormat Format)
		{
			FResourceViewArgs ResourceViewArgs = {};
			ResourceViewArgs.Type = EResourceType::ShaderResource;
			ResourceViewArgs.Resource.Format = Format;
			ResourceViewArgs.Resource.ViewDimension = EResourceViewDimension::Texture2D;
			return ResourceViewArgs;
		}

		static FResourceViewArgs Shader(uint32_t Stride, uint32_t NumElements, EFormat Format)
		{
			FResourceViewArgs ResourceViewArgs = {};
			ResourceViewArgs.Type = EResourceType::ShaderResource;
			ResourceViewArgs.Resource.Format = Format;
			ResourceViewArgs.Resource.ViewDimension = EResourceViewDimension::Buffer;
			ResourceViewArgs.Resource.Buffer.NumElements = NumElements;
			ResourceViewArgs.Resource.Buffer.Stride = Stride;
			return ResourceViewArgs;
		}

		static FResourceViewArgs RenderTarget(EFormat Format, EResourceViewDimension ResourceViewDimension = EResourceViewDimension::Texture2D)
		{
			FResourceViewArgs ResourceViewArgs = {};
			ResourceViewArgs.Type = EResourceType::RenderTarget;
			ResourceViewArgs.Resource.Format = Format;
			ResourceViewArgs.Resource.ViewDimension = ResourceViewDimension;
			return ResourceViewArgs;
		}

		static FResourceViewArgs DepthStencil(EFormat Format, EResourceViewDimension ResourceViewDimension = EResourceViewDimension::Texture2D)
		{
			FResourceViewArgs ResourceViewArgs = {};
			ResourceViewArgs.Type = EResourceType::DepthStencil;
			ResourceViewArgs.Resource.Format = Format;
			ResourceViewArgs.Resource.ViewDimension = ResourceViewDimension;
			return ResourceViewArgs;
		}
	};
	
	struct FDescriptorHeapDesc
	{
		EDescriptorHeapType DescriptorHeapType = EDescriptorHeapType::ShaderResource;
		uint32_t NumDescriptors = 0;
		EDescriptorHeapFlags HeapFlags = EDescriptorHeapFlags::None;
	};

	struct FSwapChainDesc
	{
		void * WindowHandle = nullptr;
		uint32_t NumBuffers = 2;
		
		EFormat Format = EFormat::BGRA;
		SizeU Size;
		ESwapEffect SwapEffect = ESwapEffect::FlipDiscard;
		FMSAA MSAA = { 1, 0 };
		
		ECommandQueueFlags CommandQueueFlags;
	};
	
	struct FPipelineSamplerDesc
	{
		EShaderMasks ShaderMask = EShaderMasks::All;
		uint32_t RegisterIndex = 0;
		uint32_t RegisterSpace = 0;

		EFilter Filter = EFilter::Point;
		EAddressMode AddressModeU = EAddressMode::Clamp;
		EAddressMode AddressModeV = EAddressMode::Clamp;
		EAddressMode AddressModeW = EAddressMode::Clamp;
		Fraction MipLODBias = 0;
		uint32_t Anisotropy = 0;
		EComparison Comparison = EComparison::None;
		Fraction MinLOD = 0;
		Fraction MaxLOD = 0;

		auto operator<=>(const FPipelineSamplerDesc &) const noexcept = default;
		uintx_t Hash() const noexcept
		{
			return HashJoin(ShaderMask,
				RegisterIndex,
				RegisterSpace,
				Filter,
				AddressModeU,
				AddressModeV,
				AddressModeW,
				MipLODBias,
				Anisotropy,
				Comparison,
				MinLOD,
				MaxLOD
			);
		}
	};

	struct FUInt32ConstantDesc
	{
		EShaderType Shader = EShaderType::All;
		uint32_t RegisterIndex = 0;
		uint32_t RegisterSpace = 0;
		uint32_t NumValues = 0;

		auto operator<=>(const FUInt32ConstantDesc &) const noexcept = default;
		uintx_t Hash() const noexcept
		{
			return HashJoin(Shader,
				RegisterIndex,
				RegisterSpace,
				NumValues);
		}
	};

	struct FDescriptorDesc
	{
		EShaderType Shader = EShaderType::All;
		EDescriptorType DescriptorType = EDescriptorType::ConstBuffer;
		uint32_t RegisterIndex = 0;
		uint32_t RegisterSpace = 0;
		EDescriptorFlags DescriptorFlags = EDescriptorFlags::None;

		auto operator<=>(const FDescriptorDesc &) const noexcept = default;
		uintx_t Hash() const noexcept
		{
			return HashJoin(Shader,
				DescriptorType,
				RegisterIndex,
				RegisterSpace,
				DescriptorFlags);
		}
	};

	struct FDescriptorRangeDesc
	{
		EDescriptorType DescriptorType = EDescriptorType::ConstBuffer;
		uint32_t RegisterIndex = 0;
		uint32_t RegisterSpace = 0;
		EDescriptorFlags DescriptorFlags = EDescriptorFlags::None;
		
		uint32_t NumDescriptors = 1;
		uint32_t PacketOffset = UInt32Max;

		auto operator<=>(const FDescriptorRangeDesc &) const noexcept = default;
		uintx_t Hash() const noexcept
		{
			return HashJoin(DescriptorType,
				RegisterIndex,
				RegisterSpace,
				DescriptorFlags,
				NumDescriptors,
				PacketOffset
			);
		}
	};

	struct FDescriptorTableDesc
	{
		EShaderType Shader = EShaderType::All;
		TVector<FDescriptorRangeDesc> DescriptorRanges;

		auto operator<=>(const FDescriptorTableDesc &) const noexcept = default;
		uintx_t Hash() const
		{
			return HashJoin(Shader, DescriptorRanges);
		}
	};

	constexpr size_t NumRenderTargetsMax = 8;

	struct FInputElement
	{
		FString Name;
		uint32_t Index;
		EFormat Format = EFormat::BGRA;
		uint32_t Offset = 0;
		uint32_t SlotIndex = 0;

		uintx_t Hash() const noexcept
		{
			return HashJoin(Name,
				Index,
				Format,
				Offset,
				SlotIndex
			);
		}
	};

	struct FRootSignatureDesc
	{
		TVector<FUInt32ConstantDesc> UInt32Constants;
		TVector<FDescriptorDesc> Descriptors;
		TVector<FPipelineSamplerDesc> Samplers;
		TVector<FDescriptorTableDesc> DescriptorTables;

		auto operator<=>(const FRootSignatureDesc &) const noexcept = default;
		uintx_t Hash() const noexcept
		{
			return HashJoin(DescriptorTables,
				UInt32Constants,
				Descriptors,
				Samplers);
		}

		uintx_t GetUnitCount() const
		{
			return UInt32Constants.Size + Descriptors.Size * 2 + DescriptorTables.Size;
		}
	};

	struct FGraphicsPipelistStateDesc
	{
		EGeometry TopologyType = EGeometry::Triangle;
		
		TView<byte_t> VSCode;
		TView<byte_t> HSCode;
		TView<byte_t> DSCode;
		TView<byte_t> GSCode;
		TView<byte_t> PSCode;

		TVector<FInputElement> Elements;

		struct
		{
			uint32_t NumRenderTargets = 1;
			TArray<EFormat, NumRenderTargetsMax> Formats = { EInitializeMode::Zero };
			FMSAA MSAA = {};
		}RenderTarget;

		struct
		{
			EFormat Format = EFormat::None;
			bool DepthTest = true;
			bool DepthWrite = true;
			EComparison DepthComparison = EComparison::Less;
			bool StencilTest = false;
			uint32_t StencilRef = 0;
		}DepthStencil;
		
		struct
		{
			bool AlphaToCoverage = false;
			bool IndependentBlend = false;
			float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			struct
			{
				bool Enable = false;
				EBlend SrcColor = EBlend::One;
				EBlend DstColor = EBlend::Zero;
				EBlendOP ColorOP = EBlendOP::Add;
				EBlend SrcAlpha = EBlend::One;
				EBlend DstAlpha = EBlend::Zero;
				EBlendOP AlphaOP = EBlendOP::Add;
				EWriteMasks WriteMasks = EWriteMasks::All;
			}RenderTargets[NumRenderTargetsMax];
		}Blend;

		struct
		{
			bool Wireframe = false;
			ECullMode CullMode = ECullMode::None;
			bool CCW = false;
			int DepthBias = 0;
			bool DepthClip = true;
			bool MSAA = false;
		}Rasterize;


		uint32_t SampleMask = 0xffffffff;


		uintx_t Hash() const noexcept
		{
			uintx_t HashValue = HashJoin(
				TopologyType,
				VSCode,
				HSCode,
				DSCode,
				GSCode,
				PSCode,
				Elements,
				RenderTarget.NumRenderTargets,
				RenderTarget.Formats,
				RenderTarget.MSAA.Level,
				RenderTarget.MSAA.Quality,
				Blend.AlphaToCoverage,
				Blend.IndependentBlend,
				Blend.BlendFactor
			);

			for (uint32_t RenderTargetIndex = 0; RenderTargetIndex < RenderTarget.NumRenderTargets; ++RenderTargetIndex)
			{
				HashValue = HashCombine(HashValue,
					Blend.RenderTargets[RenderTargetIndex].Enable,
					Blend.RenderTargets[RenderTargetIndex].SrcColor,
					Blend.RenderTargets[RenderTargetIndex].DstColor,
					Blend.RenderTargets[RenderTargetIndex].ColorOP,
					Blend.RenderTargets[RenderTargetIndex].SrcAlpha,
					Blend.RenderTargets[RenderTargetIndex].DstAlpha,
					Blend.RenderTargets[RenderTargetIndex].AlphaOP,
					Blend.RenderTargets[RenderTargetIndex].WriteMasks
				);
			}

			HashValue = HashCombine(HashValue,
				Rasterize.Wireframe,
				Rasterize.CullMode,
				Rasterize.CCW,
				Rasterize.DepthBias,
				Rasterize.DepthClip,
				Rasterize.MSAA
			);

			HashValue = HashCombine(HashValue,
				DepthStencil.Format,
				DepthStencil.DepthTest,
				DepthStencil.DepthWrite,
				DepthStencil.DepthComparison,
				DepthStencil.StencilTest,
				DepthStencil.StencilRef
			);

			HashValue = HashCombine(HashValue, SampleMask);
			return HashValue;
		}
	};

	struct FPipelineStateArgs
	{
		RHI::FRootSignatureDesc RootSignatureArgs = {};
		RHI::FGraphicsPipelistStateDesc GraphicsPipelistStateArgs = {};
	};

	struct FComputePipelineStateDesc
	{
		TView<byte_t> CSCode;
	};


	enum class EFenceFlags
	{
		None = 0,
		Shared = 0x1
	};

	template struct TFlag<EFenceFlags>;

	struct FCopyableFootprint
	{
		uint32_t NumRows = 0;
		uint32_t Offset = 0;
		uint32_t AlignMask = 0;
		EFormat Format = EFormat::None;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Depth = 0;
		uint32_t RowPitch = 0;
	};
}
