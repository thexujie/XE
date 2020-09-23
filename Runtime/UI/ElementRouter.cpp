#include "PCH.h"
#include "ElementRouter.h"

namespace XE::UI
{
	void IElementRouter::RaiseEvent(FRoutedEventArgs & Args)
	{
		if (IElementRouter * Parent = GetRouterParent())
			Parent->RaiseEvent(Args);

		if (!Args.Handled)
		{
			auto [DelegateIter, DelegateIterEnd] = Handlers.Find(Args.Event);
			while (DelegateIter != DelegateIterEnd)
			{
				DelegateIter->Value.Invoke({ Ref(Args) });
				if (Args.Handled)
					break;
				++DelegateIter;
			}
		}
	}

	void IElementRouter::AddHandler(const FEvent & RoutedEvent, const FDelegate & Delegate)
	{
		Handlers.Insert(RoutedEvent, Delegate);
	}

	void IElementRouter::RemoveHandler(const FEvent & RoutedEvent, const FDelegate & Delegate)
	{
		
	}
}
