#include "PCH.h"
#include "Panel.h"


namespace XE::UI
{
	void UPanel::AddPanelControl(TReferPtr<UElement> Control)
	{
		PannelChildren.Add(Control);
	}

	void UPanel::InitializeUI()
	{
		for (const TReferPtr<UElement> & PanelChild : PannelChildren)
			AddChild(PanelChild);
		UControl::InitializeUI();
	}
}
