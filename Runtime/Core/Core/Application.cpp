#include "PCH.h"
#include "Application.h"

namespace XE
{
	FApplication * GApplication = nullptr;
	const FType & TypeBy(const Char * Name)
	{
		Assert(GApplication);
		return GApplication->GetType(Name);
	}
	
	const FType & TypeBy(FStringView Name)
	{
		Assert(GApplication);
		return GApplication->GetType(Name);
	}
	
	FApplication::FApplication()
	{
		GApplication = this;
		RegisterType<NullPtrT>();
		RegisterType<FType>();
		
		RegisterType<char8_t>();
		RegisterType<char16_t>();
		RegisterType<char32_t>();
		
		RegisterType<int8_t>();
		RegisterType<uint8_t>();
		RegisterType<int16_t>();
		RegisterType<uint16_t>();
		RegisterType<int32_t>();
		RegisterType<uint32_t>();
		RegisterType<int64_t>();
		RegisterType<uint64_t>();
		
		RegisterType<float32_t>();
		RegisterType<float64_t>();

		RegisterType<FColor32>();
		RegisterType<FStringView>();
		RegisterType<FString>();

		RegisterType<TVec2<int16_t>>();
		RegisterType<TVec2<uint16_t>>();
		RegisterType<TVec2<int32_t>>();
		RegisterType<TVec2<uint32_t>>();
		RegisterType<TVec2<int64_t>>();
		RegisterType<TVec2<uint64_t>>();
		RegisterType<TVec2<float32_t>>();
		RegisterType<TVec2<float64_t>>();

		RegisterType<TVec4<int16_t>>();
		RegisterType<TVec4<uint16_t>>();
		RegisterType<TVec4<int32_t>>();
		RegisterType<TVec4<uint32_t>>();
		RegisterType<TVec4<int64_t>>();
		RegisterType<TVec4<uint64_t>>();
		RegisterType<TVec4<float32_t>>();
		RegisterType<TVec4<float64_t>>();

		RegisterType<IObject>();
		RegisterType<IRefer, IObject>();
		RegisterType<FObject, IRefer>();

		RegisterType<FResource, FObject>();

		RegisterType<IService, FObject>();
		
		RegisterType<FVariant>();
	}

	IService * FApplication::GetService(const FType & Type)
	{
		auto ServiceIter = Services.Find(Type);
		if (ServiceIter != Services.End())
			return ServiceIter->Value.Get();

		auto ServiceTypeIter = ServiceTypes.Find(Type);
		if (ServiceTypeIter == ServiceTypes.End())
			return nullptr;

		FType InstanceType = ServiceTypeIter->Value.Front();
		IService * Pointer = (IService *)Malloc(InstanceType.GetSize(), InstanceType.GetAlignment());
		InstanceType.Construct(Pointer);
		Pointer->Initialize();
		TSharedPtr<IService> Service = TSharedPtr<IService>(Pointer, [](IService * Service) {Service->Finalize(); Delete(Service); });
		Services[Type] = Service;
		return Service.Get();
	}


	void FApplication::RegisterType(FType Type, TView<FType> ParentTypes)
	{
		if (Types.Contains(Type.GetName()))
			throw EError::Exists;

		if (!ParentTypes.IsEmpty())
			Type.GetNativeType()->SetParent(ParentTypes[0].GetNativeType());

		FStringView TypeName = Type.GetName();
		Types[TypeName] = Type;

		if ((TypeName.StartsWith(Str("class")) || TypeName.StartsWith(Str("struct"))) && TypeName.FindFirst('<') == NullIndex)
		{
			if (size_t NamespaceLastIndex = TypeName.FindLast(Str("::")); NamespaceLastIndex != NullIndex)
			{
				FStringView SimpleTypeName = TypeName.Slice(NamespaceLastIndex + 2);
				if (SimpleTypeName[0] == 'F' || SimpleTypeName[0] == 'U')
					SimpleTypeName = SimpleTypeName.Slice(1);
				AliasTypes[SimpleTypeName] = Type;
			}
			else if (size_t SpaceLastIndex = TypeName.FindLast(Str(" ")); SpaceLastIndex != NullIndex)
			{
				FStringView SimpleTypeName = TypeName.Slice(SpaceLastIndex + 1);
				if (SimpleTypeName[0] == 'F' || SimpleTypeName[0] == 'U')
					SimpleTypeName = SimpleTypeName.Slice(1);
				AliasTypes[SimpleTypeName] = Type;
			}

		}
		for (FType ParentType : ParentTypes)
			ParentType.GetNativeType()->AppendChild(Type.GetNativeType());
	}

	FVariant FApplication::FindResource_(FVariant ResourceKey) const
	{
		auto ResourceIter = ResourceDirectory.Resources.Find(ResourceKey);
		return ResourceIter == ResourceDirectory.Resources.End() ? nullptr : ResourceIter->Value;
	}

	const FType & FApplication::GetType(FStringView TypeName) const
	{
		auto TypeIter = Types.Find(TypeName);
		if (TypeIter != Types.End())
			return TypeIter->Value;
		
		auto AliasTypeIter = AliasTypes.Find(TypeName);
		if (AliasTypeIter != AliasTypes.End())
			return AliasTypeIter->Value;
		
		return FType::Unkown;
	}
}
