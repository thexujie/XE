#pragma once

#include "ElementRouter.h"
#include "UIInc.h"
#include "Visual.h"
#include "Style.h"

namespace XE::UI
{
	enum class EVisibility
	{
		Visible = 0,
		Collapsed,
		Hidden,
	};
	
	enum class EElemntFlag
	{
		
	};

	class UI_API FMouse
	{
	public:
		static FRoutedEvent PreviewMouseDown;
		static FRoutedEvent MouseDown;
		static FRoutedEvent PreviewMouseUp;
		static FRoutedEvent MouseUp;
		static FRoutedEvent PreviewMouseMove;
		static FRoutedEvent MouseMove;

		static FRoutedEvent PreviewKeyDown;
		static FRoutedEvent KeyDown;
		static FRoutedEvent PreviewKeyUp;
		static FRoutedEvent KeyUp;
	};

	class UI_API FMouseEventArgs : public FRoutedEventArgs
	{
	public:
		FMouseEventArgs(const FRoutedEvent & RoutedEvent_) : FRoutedEventArgs(RoutedEvent_) {}

		PointF Position = {};
	};

	class UI_API FKeyboardEventArgs : public FRoutedEventArgs
	{
	public:
		FKeyboardEventArgs(const FRoutedEvent & RoutedEvent_) : FRoutedEventArgs(RoutedEvent_) {}

		EKeyCode KeyCode = EKeyCode::None;
	};
	
	class UI_API FMouseButtonEventArgs : public FMouseEventArgs
	{
	public:
		FMouseButtonEventArgs(const FRoutedEvent & RoutedEvent_) : FMouseEventArgs(RoutedEvent_) {}
	};
	
	class UI_API UElement : public UVisual, public IElementRouter
	{
	public:
		UElement() = default;

		TReferPtr<FObject> GetParent() const { return TReferPtr(ReferAddRef(Parent.Get())); }
		TReferPtr<FObject> FindChild(FType Type, bool IsRecursively) const override;
		PointF GlobalToLocal(const PointF & Point) const;
		
		void PrintChildren(uint32_t Depth = 0);

	public:
		void RaiseEvent(FRoutedEventArgs & Args) override;
		IElementRouter * GetRouterParent() const override { return Parent.Get(); }
		
	public:
		TReferPtr<UElement> GetElementParent() const { return TReferPtr(ReferAddRef(Parent.Get())); }
		void AddChild(TReferPtr<UElement> Child) { Children.Add(Child); Child->Parent = this; }
		TReferPtr<UElement> FindElementAt(const PointF & Point) const;
		
		void SetPosition(const PointF & Position_) { Position = Position_; }
		PointF GetPosition() const { return Position; }

		void SetSize(const SizeF & Size_) { Size = Size_; }
		SizeF GetSize() const { return Size; }

	public:
		virtual void Draw(FGraphics & Graphics) const;
		virtual void OnRender(FGraphics & Graphics) const {}
		
	public:
		virtual void Layout();
		
		virtual void LayoutMeasure(SizeF AvailableSize);
		virtual SizeF Measure(SizeF ConstrainedSize) const;
		
		virtual void LayoutArrange(RectF FinalRect);
		virtual void Arrange(SizeF ArrangedSize);
		
	public:
		virtual void InitializeUI();
		virtual void ApplyStyle();

	public:
		void OnPropertyChanged(FProperty Property, FVariant & PropertyNewValue, FVariant & PropertyOldValue) override;
		
	public:
		virtual void PreviewMouseDown(FMouseButtonEventArgs & Args);
		virtual void PreviewMouseUp(FMouseButtonEventArgs & Args);
		virtual void PreviewMouseMove(FMouseEventArgs & Args);
		
		virtual void MouseDown(FMouseButtonEventArgs & Args);
		virtual void MouseUp(FMouseButtonEventArgs & Args);
		virtual void MouseMove(FMouseEventArgs & Args);

		virtual void PreviewKeyDown(FKeyboardEventArgs & Args) {}
		virtual void KeyDown(FKeyboardEventArgs & Args) {}

		virtual void PreviewKeyUp(FKeyboardEventArgs & Args) {}
		virtual void KeyUp(FKeyboardEventArgs & Args) {}
		
	public:
		TPointer<UElement> Parent;
		TVector<TReferPtr<UElement>> Children;

		TReferPtr<FStyle> Style;
		
		EVisibility Visibility = EVisibility::Visible;
		PointF Position = { NanF, NanF };
		SizeF Size = { NanF, NanF };
		SizeF MinSize = { NanF, NanF };
		SizeF MaxSize = { NanF, NanF };
		SizeF DesiredSize = { NanF, NanF };
		ThicknessF Margin = {};

		bool Selected = false;
		bool Focused = false;
		
	public:
		static FProperty StyleProperty;
		static FProperty PositionProperty;
		static FProperty SizeProperty;
		static FProperty MarginProperty;
		friend UI_API void Reflection(TNativeType<UElement> & NativeType);
	};
}

namespace XE
{
	template<>
	class UI_API TDataConverter<UI::UElement> : public TRefer<IDataConverter>
	{
	public:
		FVariant Convert(const IDataProvider * ValueProvider) const;
	};
}