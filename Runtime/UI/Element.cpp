#include "PCH.h"
#include "Element.h"

namespace XE::UI
{
	FProperty UElement::StyleProperty(Str("Style"), &UElement::Style);
	FProperty UElement::PositionProperty(Str("Position"), &UElement::Position);
	FProperty UElement::SizeProperty(Str("Size"), &UElement::Size);
	FProperty UElement::MarginProperty(Str("Margin"), &UElement::Margin);

	void Reflection(TNativeType<UElement> & NativeType)
	{
		NativeType.RegisterProperty(UElement::StyleProperty);
		NativeType.RegisterProperty(UElement::PositionProperty);
		NativeType.RegisterProperty(UElement::SizeProperty);
		NativeType.RegisterProperty(UElement::MarginProperty);
	}

	FRoutedEvent FMouse::PreviewMouseDown(Str("PreviewMouseDown"));
	FRoutedEvent FMouse::MouseDown(Str("MouseDown"));
	FRoutedEvent FMouse::PreviewMouseUp(Str("PreviewMouseUp"));
	FRoutedEvent FMouse::MouseUp(Str("MouseUp"));
	FRoutedEvent FMouse::PreviewMouseMove(Str("PreviewMouseMove"));
	FRoutedEvent FMouse::MouseMove(Str("MouseMove"));

	FRoutedEvent FMouse::PreviewKeyDown(Str("PreviewKeyDown"));
	FRoutedEvent FMouse::KeyDown(Str("KeyDown"));
	FRoutedEvent FMouse::PreviewKeyUp(Str("PreviewKeyUp"));
	FRoutedEvent FMouse::KeyUp(Str("KeyUp"));
	
	TReferPtr<FObject> UElement::FindChild(FType Type, bool IsRecursively) const
	{
		for (const TReferPtr<UVisual> & Child : Children)
		{
			if (Child->GetObjectType() == Type)
				return Child;

			if (IsRecursively)
			{
				TReferPtr<UVisual> Visual = Child->TFindChild<UVisual>(IsRecursively);
				if (Visual)
					return Visual;
			}
		}
		return nullptr;
	}

	PointF UElement::GlobalToLocal(const PointF & Point) const
	{
		PointF Result = Point;
		TReferPtr<UElement> Current = ReferAs<UElement>();
		TReferPtr<UElement> CurrentParent = Current->TGetParent<UElement>();
		while(CurrentParent)
		{
			Result -= Current->RenderRect.Position;
			Current = CurrentParent;
			CurrentParent = Current->TGetParent<UElement>();
		}
		return Result;
	}
	
	void UElement::PrintChildren(uint32_t Depth)
	{
		static_assert(IsFormatAbleV<RectF>);
		FString & Buffer = GetThreadLocalStringBuffer<Char>();
		for (uint32_t Index = 0; Index < Depth; ++Index)
			Buffer.Append(Str("--"));
		Buffer.Append(ToString());
		FSystem::DebugOutput(Buffer);
		for (const TReferPtr<UElement> & Child : Children)
			Child->PrintChildren(Depth + 1);
	}
	
	void UElement::RaiseEvent(FRoutedEventArgs & Args)
	{
		if (Args.Event == FMouse::MouseDown)
		{
			MouseDown(static_cast<FMouseButtonEventArgs &>(Args));
		}
		else if (Args.Event == FMouse::MouseMove)
		{
			MouseMove(static_cast<FMouseEventArgs &>(Args));
		}
		else if (Args.Event == FMouse::KeyDown)
		{
			KeyDown(static_cast<FKeyboardEventArgs &>(Args));
		}
		else if (Args.Event == FMouse::KeyUp)
		{
			KeyUp(static_cast<FKeyboardEventArgs &>(Args));
		}
		
		if (IElementRouter * Parent = GetRouterParent())
			Parent->RaiseEvent(Args);

		if (Args.Handled)
			return;

		if (Args.Event == FMouse::PreviewMouseDown)
		{
			PreviewMouseDown(static_cast<FMouseButtonEventArgs &>(Args));
		}
		else if (Args.Event == FMouse::PreviewMouseMove)
		{
			PreviewMouseMove(static_cast<FMouseEventArgs &>(Args));
		}
		else if (Args.Event == FMouse::PreviewKeyDown)
		{
			PreviewKeyDown(static_cast<FKeyboardEventArgs &>(Args));
		}
		else if (Args.Event == FMouse::PreviewKeyUp)
		{
			PreviewKeyUp(static_cast<FKeyboardEventArgs &>(Args));
		}
		
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

		if (Args.Handled)
			return;
		
		if (Args.Owner == this)
		{
			if (Args.Event == FMouse::PreviewMouseDown)
			{
				FMouseButtonEventArgs & PreviewMouseDownEventArgs = static_cast<FMouseButtonEventArgs &>(Args);
				FMouseButtonEventArgs MouseDownEventArgs(FMouse::MouseDown);
				MouseDownEventArgs.Owner = PreviewMouseDownEventArgs.Owner;
				MouseDownEventArgs.Position = PreviewMouseDownEventArgs.Position;
				RaiseEvent(MouseDownEventArgs);
			}
			else if (Args.Event == FMouse::PreviewMouseMove)
			{
				FMouseButtonEventArgs & PreviewMouseDownEventArgs = static_cast<FMouseButtonEventArgs &>(Args);
				FMouseButtonEventArgs MouseDownEventArgs(FMouse::MouseMove);
				MouseDownEventArgs.Owner = PreviewMouseDownEventArgs.Owner;
				MouseDownEventArgs.Position = PreviewMouseDownEventArgs.Position;
				RaiseEvent(MouseDownEventArgs);
			}
		}
	}
	
	TReferPtr<UElement> UElement::FindElementAt(const PointF & Point) const
	{
		for (const TReferPtr<UElement> & Child : Children)
		{
			if (Child->RenderRect.Contains(Point))
			{
				TReferPtr<UElement> ChildChildElement = Child->FindElementAt(Point - Margin.XY - Child->RenderRect.Position);
				return ChildChildElement ? ChildChildElement : Child;
			}
		}
		return nullptr;
	}
	
	void UElement::Draw(FGraphics & Graphics) const
	{
		Graphics.PushTranslation(RenderRect.Position);
		OnRender(Graphics);

		for (const TReferPtr<UElement> & Child : Children)
			Child->Draw(Graphics);

		Graphics.Pop();
	}
	
	void UElement::Layout()
	{
	}

	static float32_t SizeComponentMinMax(float32_t Value, float32_t MinValue, float32_t MaxValue)
	{
		if (!IsNan(MinValue)) Value = Max(Value, MinValue);
		if (!IsNan(MaxValue)) Value = Min(Value, MaxValue);
		return Value;
	}
	
	void UElement::LayoutMeasure(SizeF AvailableSize)
	{
		if (Visibility == EVisibility::Collapsed)
			return;

		SizeF ConstrainedSize = AvailableSize - SizeF(Margin.Left + Margin.Right, Margin.Top + Margin.Bottom);
		
		if (IsNan(Size.Width))
			ConstrainedSize.Width = SizeComponentMinMax(ConstrainedSize.Width, MinSize.Width, MaxSize.Width);
		else
			ConstrainedSize.Width = SizeComponentMinMax(Size.Width, MinSize.Width, MaxSize.Width);

		if (IsNan(Size.Height))
			ConstrainedSize.Height = SizeComponentMinMax(ConstrainedSize.Height, MinSize.Height, MaxSize.Height);
		else
			ConstrainedSize.Height = SizeComponentMinMax(Size.Height, MinSize.Height, MaxSize.Height);
		
		SizeF MeasuredSize = Measure(ConstrainedSize);

		if (IsNan(Size.Width))
			MeasuredSize.Width = SizeComponentMinMax(MeasuredSize.Width, MinSize.Width, MaxSize.Width);
		else
			MeasuredSize.Width = SizeComponentMinMax(Size.Width, MinSize.Width, MaxSize.Width);

		if (IsNan(Size.Height))
			MeasuredSize.Height = SizeComponentMinMax(MeasuredSize.Height, MinSize.Height, MaxSize.Height);
		else
			MeasuredSize.Height = SizeComponentMinMax(Size.Height, MinSize.Height, MaxSize.Height);

		MeasuredSize += SizeF(Margin.Left + Margin.Right, Margin.Top + Margin.Bottom);
		
		DesiredSize = SizeF(Min(MeasuredSize.Width, AvailableSize.Width), Min(MeasuredSize.Height, AvailableSize.Height));
	}

	SizeF UElement::Measure(SizeF ConstrainedSize) const
	{
		SizeF MeasuredSize = {};
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutMeasure(ConstrainedSize);
			MeasuredSize.Width = Max(MeasuredSize.Width, Child->DesiredSize.Width);
			MeasuredSize.Height = Max(MeasuredSize.Height, Child->DesiredSize.Height);
		}
		return MeasuredSize;
	}

	void UElement::LayoutArrange(RectF FinalRect)
	{
		RenderRect = RectF(FinalRect.Position + Margin.Position, FinalRect.Size - Margin.ThicknessSize());
		Arrange(RenderRect.Size);
	}
	
	void UElement::Arrange(SizeF ArrangedSize)
	{
	}
	
	void UElement::InitializeUI()
	{
		ApplyStyle();
		for (const TReferPtr<UElement> & Child : Children)
			Child->InitializeUI();
		SetUIObjectFlag(EUIObjectFlags::Initialized);
	}

	void UElement::ApplyStyle()
	{
		if (auto StylePropertyIter = Properties.Find(StyleProperty); StylePropertyIter != Properties.End())
			Style = StylePropertyIter->Value.Get<TReferPtr<FStyle>>();
		else
		{
			FType ThisType = GetObjectType();
			Style = GApplication->FindResource_(GetObjectType()).Get<TReferPtr<FStyle>>();
		}

		for (const auto & PropertyPair : Properties)
			PropertyPair.Key.SetValue(this, PropertyPair.Value);

		if (Style)
		{
			for (const auto & PropertyPair : Style->Properties)
			{
				if (!Properties.Contains(PropertyPair.Key))
					PropertyPair.Key.SetValue(this, PropertyPair.Value);
			}
		}
	}
	
	void UElement::OnPropertyChanged(FProperty Property, FVariant & PropertyNewValue, FVariant & PropertyOldValue)
	{
		if (!IsUIObjectFlag(EUIObjectFlags::Initialized))
			return;
		
		FSystem::DebugOutput(Format(Str("Property {} changed"), Property.GetName()));
	}
	
	void UElement::PreviewMouseDown(FMouseButtonEventArgs & Args)
	{
		FSystem::DebugOutput(Format(Str("PreviewMouseDown {} Point = {}"), GetObjectType().GetShortName(), Args.Position));
	}
	
	void UElement::PreviewMouseUp(FMouseButtonEventArgs & Args)
	{
		
	}
	
	void UElement::PreviewMouseMove(FMouseEventArgs & Args)
	{
		//FSystem::DebugOutput(Format(Str("{} Point = {}"), *this, Point));
	}

	void UElement::MouseDown(FMouseButtonEventArgs & Args)
	{
		FSystem::DebugOutput(Format(Str("MouseDown {} Point = {}"), GetObjectType().GetShortName(), Args.Position));
	}
	
	void UElement::MouseUp(FMouseButtonEventArgs & Args)
	{
		FSystem::DebugOutput(Format(Str("MouseUp {} Point = {}"), GetObjectType().GetShortName(), Args.Position));
	}

	void UElement::MouseMove(FMouseEventArgs & Args)
	{

	}
}
