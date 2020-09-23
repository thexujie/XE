#pragma once

#include "UIInc.h"

namespace XE::UI
{
	class UI_API FRoutedEvent : public FEvent
	{
	public:
		FRoutedEvent() = default;
		FRoutedEvent(FStringView Name_) : FEvent(Name_) {}
		auto operator<=>(const FRoutedEvent & Another) const = default;
		bool operator==(const FRoutedEvent & Another) const = default;
	};

	class UI_API FRoutedEventArgs : public FEventArgs
	{
	public:
		FRoutedEventArgs(const FRoutedEvent & RoutedEvent_) : FEventArgs(RoutedEvent_) {}

		const class IElementRouter * Owner = nullptr;
	};

	class UI_API IElementRouter
	{
	public:
		IElementRouter() = default;
		virtual ~IElementRouter() = default;

		virtual void RaiseEvent(FRoutedEventArgs & Args);
		virtual void AddHandler(const FEvent & RoutedEvent, const FDelegate & Delegate);
		virtual void RemoveHandler(const FEvent & RoutedEvent, const FDelegate & Delegate);

		virtual IElementRouter * GetRouterParent() const { return nullptr; }
		
	protected:
		TMultiMap<FEvent, FDelegate> Handlers;
	};
}
