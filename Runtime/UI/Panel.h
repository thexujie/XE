#pragma once

#include "UIInc.h"
#include "Control.h"

namespace XE::UI
{
	class UI_API UPanel : public UControl
	{
	public:
		UPanel() = default;

		void AddPanelControl(TReferPtr<UElement> Control);
		const TVector<TReferPtr<UElement>> & GetChildren() const { return Children; }

	public:
		void InitializeUI() override;
		
	public:
		TVector<TReferPtr<UElement>> PannelChildren;
	};
}
