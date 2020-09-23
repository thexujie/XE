#pragma once

#include "UIInc.h"
#include "Control.h"

namespace XE::UI
{
	class UI_API UWindow : public FUIObject
	{
	public:
		virtual EError Attatch(TReferPtr<UControl> Control) = 0;
		virtual ptr_t GetHandle() const = 0;
		//virtual std::shared_ptr<IImeContext> imeContext() const = 0;
		//virtual std::shared_ptr<ICursorContext> cursorContext() const = 0;
		virtual void Move(const PointF & Position) = 0;
		virtual void Resize(const SizeU & Position) = 0;
		virtual void Show(EShowMode ShowMode = EShowMode::Default) = 0;
	};
}