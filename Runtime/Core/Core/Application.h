#pragma once

#include "CoreInc.h"
#include "Name.h"
#include "Object.h"
#include "NativeType.h"
#include "ResourceDictionary.h"

namespace XE
{
	class CORE_API IService : public FObject
	{
	public:
		virtual EError Initialize() = 0;
		virtual void Finalize() = 0;
	};
	
	class CORE_API FApplication
	{
	public:
		FApplication();

		void RegisterService(const FType & ServiceType, const FType & InstanceType)
		{
			ServiceTypes[ServiceType].Add(InstanceType);
		}
		template<typename ServiceT, typename InstanceT>
		void RegisterService()
		{
			RegisterService(TypeOf<ServiceT>(), TypeOf<InstanceT>());
		}
		IService * GetService(const FType & Type);
		template<typename ServiceT>
		ServiceT * GetService() { return static_cast<ServiceT *>(GetService(TypeOf<ServiceT>())); }
		
		const TMap<FStringView, FType> & GetTypes() { return Types; }

		const FType & GetType(FStringView Name) const;
		
		void RegisterType(FType Type, TView<FType> ParentTypes);

		template<typename T, typename ...ParentsT>
		void RegisterType()
		{
			static_assert(!IsReferPointerV<T>);
			static_assert(ConjunctionV<IsBaseOf<ParentsT, T>...>);
			static TNativeType<T> NativeType;
			Reflection(NativeType);
			RegisterType(FType(&NativeType), { ((TypeOf<ParentsT>()))... });

			if constexpr(IsReferAbleV<T>)
			{
				static TNativeType<TReferPtr<T>> NativeReferType;
				NativeReferType.SetUnderlyingType(&NativeType);
				RegisterType(FType(&NativeReferType), {});
			}

			if constexpr(IsCompleteV<TDataConverter<T>>)
			{
				static TDataConverter<T> DataConverter;
				NativeType.SetDataConverter(&DataConverter);
			}
		}

		FVariant FindResource_(FVariant ResourceKey) const;

	private:
		TMap<FType, TVector<FType>> ServiceTypes;
		TMap<FType, TSharedPtr<IService>> Services;
		TMap<FType, TPointer<const IDataConverter>> DataConverters;

		// static reflection, FStringView as Key by name pointer is OK
		TMap<FStringView, FType> Types;
		TMap<FStringView, FType> AliasTypes;

	public:
		FResourceDictionary ResourceDirectory;
	};

	CORE_API extern FApplication * GApplication;
}
