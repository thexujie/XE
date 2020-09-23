#pragma once

#include "CoreInc.h"
#include "Delegate.h"
#include "NativeType.h"

namespace XE
{
	class CORE_API FObject : public IRefer
	{
	public:
		FObject() = default;
		FObject(const FObject &) = default;
		FObject(FObject &&) noexcept = default;
		FObject & operator = (const FObject &) = default;
		
		uintx_t AddRef()
		{
			return Atomics::IncFetch(ReferCount);
		}

		uintx_t Release()
		{
			if (ReferCount == 0)
				throw EError::State;

			uintx_t Result = Atomics::DecFetch(ReferCount);
			if (Result == 0)
				DeleteUnSafe(this);
			return Result;
		}

	public:
		template<typename T>
		TReferPtr<T> ReferAs() const
		{
			T * Pointer = static_cast<T *>(const_cast<FObject*>(this));
			return TReferPtr<T>(ReferAddRef(Pointer));
		}
		
		virtual TReferPtr<FObject> GetParent() const { return nullptr; }
		template<typename T>
		TReferPtr<T> TGetParent() const { return StaticPointerCast<T>(GetParent()); }
		virtual TReferPtr<FObject> FindChild(FType ChildType, bool IsRecursively) const { return nullptr; }
		template<typename T>
		TReferPtr<T> TFindChild(bool IsRecursively = true) const { return StaticPointerCast<T>(FindChild(TypeOf<T>(), IsRecursively)); }
		TReferPtr<FObject> FindAncestor(FType AncestorType) const
		{
			TReferPtr<FObject> Current = GetParent();
			while (Current && !Current->GetObjectType().IsChildOf(AncestorType))
				Current = Current->GetParent();
			return Current;
		}
		template<typename T>
		TReferPtr<T> TFindAncestor() const { return StaticPointerCast<T>(FindAncestor(TypeOf<T>())); }

	public:
		uintx_t ReferCount = 1;
	};
	
	class CORE_API FChangable : public FObject
	{
	public:
		FChangable() = default;
		FChangable(const FChangable &) = default;
		FChangable(FChangable &&) noexcept = default;
		FChangable & operator = (const FChangable &) = default;

	public:
		void SetProperty(FProperty Property, FVariant PropertyValue);
		virtual void OnPropertyChanging(FProperty Property, FVariant & PropertyNewValue, FVariant & PropertyOldValue) {}
		virtual void OnPropertyChanged(FProperty Property, FVariant & PropertyNewValue, FVariant & PropertyOldValue) {}

	protected:
		TLinearUnorderedMap<FProperty, FVariant> Properties;
		
	public:
		TDelegate<void(const FProperty &)> PropertyChanged;
	};
}
