#include "PCH.h"
#include "D2DGraphics.h"

#include "D2DBitmap.h"
#include "D2DDevice.h"


namespace XE::Direct2D
{
	static TReferPtr<ID2D1PathGeometry> D2DPathToGeometry(ID2D1Factory & D2D1Factory, TView<FPathPoint> PathPoints, const PointF & BasePoint)
	{
		TReferPtr<ID2D1PathGeometry> D2D1PathGeometry;
		HRESULT Result = D2D1Factory.CreatePathGeometry(D2D1PathGeometry.GetPP());
		if (Win32::Failed(Result))
		{
			LogWarning(Str("CreateTextLayout Failed: {}"), ResultStr(Result));
			return nullptr;
		}
		TReferPtr<ID2D1GeometrySink> D2D1GeometrySink;
		D2D1PathGeometry->Open(D2D1GeometrySink.GetPP());

		bool Closed = true;
		for (size_t PathPointIndex = 0; PathPointIndex < PathPoints.Size; ++PathPointIndex)
		{
			switch (PathPoints[PathPointIndex].Command)
			{
			case EPathCommand::FillWinding:
				D2D1GeometrySink->SetFillMode(D2D1_FILL_MODE_WINDING);
				break;
			case EPathCommand::FillAlternate:
				D2D1GeometrySink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
				break;

			case EPathCommand::MoveTo:
				D2D1GeometrySink->BeginFigure({ BasePoint.X + PathPoints[PathPointIndex].Point.X, BasePoint.Y + PathPoints[PathPointIndex].Point.Y }, D2D1_FIGURE_BEGIN_FILLED);
				Closed = false;
				break;

			case EPathCommand::LineTo:
				D2D1GeometrySink->AddLine({ BasePoint.X + PathPoints[PathPointIndex].Point.X, BasePoint.Y + PathPoints[PathPointIndex].Point.Y });
				break;
			case EPathCommand::Bezier3To:
				D2D1GeometrySink->AddQuadraticBezier(
					{
						BasePoint.X + PathPoints[PathPointIndex + 0].Point.X, BasePoint.Y + PathPoints[PathPointIndex + 0].Point.Y,
						BasePoint.X + PathPoints[PathPointIndex + 1].Point.X, BasePoint.Y + PathPoints[PathPointIndex + 1].Point.Y,
					});
				break;
			case EPathCommand::Bezier4To:
				D2D1GeometrySink->AddBezier(
					{
						BasePoint.X + PathPoints[PathPointIndex - 2].Point.X, BasePoint.Y + PathPoints[PathPointIndex - 2].Point.Y,
						BasePoint.X + PathPoints[PathPointIndex - 1].Point.X, BasePoint.Y + PathPoints[PathPointIndex - 1].Point.Y,
						BasePoint.X + PathPoints[PathPointIndex - 0].Point.X, BasePoint.Y + PathPoints[PathPointIndex - 0].Point.Y,
					});
				break;
			case EPathCommand::ArcTo:
				break;
			case EPathCommand::Close:
				D2D1GeometrySink->EndFigure(D2D1_FIGURE_END_CLOSED);
				Closed = true;
				break;
			default:
				break;
			}
		}

		if (!Closed)
			D2D1GeometrySink->EndFigure(D2D1_FIGURE_END_OPEN);
		D2D1GeometrySink->Close();
		return D2D1PathGeometry;
	}

	FD2DGraphics::FD2DGraphics(FD2DDevice & D2DDeviceIn, HWND hWnd) : D2DDevice(D2DDeviceIn)
	{
		SizeU WindowSize = FWindowUtils::GetWindowSize(hWnd);
		D2D1_RENDER_TARGET_PROPERTIES RenderTargetProperties = D2D1::RenderTargetProperties();
		RenderTargetProperties.dpiX = 96;
		RenderTargetProperties.dpiY = 96;
		TReferPtr<ID2D1HwndRenderTarget> D2D1HwndRenderTarget;
		HRESULT Result = D2DDevice.GetD2D1Factory()->CreateHwndRenderTarget(
			RenderTargetProperties,
			D2D1::HwndRenderTargetProperties(hWnd, { WindowSize.Width, WindowSize.Height }, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
			D2D1HwndRenderTarget.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("CreateHwndRenderTarget Failed: {}"), ResultStr(Result));
		}
		D2D1RenderTarget = D2D1HwndRenderTarget;
	}

	FD2DGraphics::FD2DGraphics(FD2DDevice & D2DDeviceIn, FSurface & Surface) : D2DDevice(D2DDeviceIn)
	{
		IDXGISurface * DXGISurface = (IDXGISurface*)Surface.GetPointer();
		TReferPtr<ID2D1RenderTarget> D2D1DXGIRenderTarget;
		HRESULT Result = D2DDevice.GetD2D1Factory()->CreateDxgiSurfaceRenderTarget(
			DXGISurface,
			D2D1::RenderTargetProperties(),
			D2D1DXGIRenderTarget.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("CreateDxgiSurfaceRenderTarget Failed: {}"), ResultStr(Result));
		}
		D2D1RenderTarget = D2D1DXGIRenderTarget;
	}

	FD2DGraphics::FD2DGraphics(FD2DDevice & D2DDeviceIn, FD2DBitmap & Bitmap) : D2DDevice(D2DDeviceIn)
	{
		TReferPtr<ID2D1RenderTarget> D2D1WICBitmapRenderTarget;
		HRESULT Result = D2DDevice.GetD2D1Factory()->CreateWicBitmapRenderTarget(
			Bitmap.GetWICBitmap(),
			D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED }),
			D2D1WICBitmapRenderTarget.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("CreateWicBitmapRenderTarget Failed: {}"), ResultStr(Result));
		}
		D2D1RenderTarget = D2D1WICBitmapRenderTarget;
	}

	FD2DGraphics::~FD2DGraphics()
	{
		SolidBrushs.Reset();
		D2D1RenderTarget.Reset();
	}

	void FD2DGraphics::Clear(FColor Color)
	{
		D2D1RenderTarget->BeginDraw();
		D2D1_COLOR_F D2DColor = { Color.R, Color.G, Color.B, Color.A };
		D2D1RenderTarget->Clear(D2DColor);
	}

	void FD2DGraphics::Flush()
	{
		//HRESULT Result = D2D1RenderTarget->Flush();
		HRESULT Result = D2D1RenderTarget->EndDraw();
		if (Win32::Failed(Result))
		{
			LogError(Str("Flush Failed: {}"), ResultStr(Result));
		}
	}

	void FD2DGraphics::Resize(SizeU Size)
	{
		SolidBrushs.Reset();
		TReferPtr<ID2D1HwndRenderTarget> D2D1HwndRenderTarget = StaticPointerCast<ID2D1HwndRenderTarget>(D2D1RenderTarget);
		D2D1HwndRenderTarget->Resize(D2D1_SIZE_U{ Size.Width, Size.Height });
	}

	void FD2DGraphics::PushTranslation(const PointF & Translation)
	{
		Translations.Add(CurrentTranslation);
		CurrentTranslation += Translation;
	}
	
	void FD2DGraphics::Pop()
	{
		CurrentTranslation = Translations.IsEmpty() ? PointF() : Translations.Back();
		Translations.Reduce(1);
	}

	void FD2DGraphics::DrawLine(const PointF & Start, const PointF & End, const FBrush & Brush, float32_t LineWidth)
	{
		D2D1RenderTarget->DrawLine(
			{ CurrentTranslation.X + Start.X, CurrentTranslation.Y + Start.Y }, 
			{ CurrentTranslation.X + End.X, CurrentTranslation.Y + End.Y }, GetBrush(Brush), LineWidth);
	}

	void FD2DGraphics::DrawBezier(const TArray<PointF, 4> & BezierPoints, const FBrush & Brush, float32_t LineWidth)
	{
		TReferPtr<ID2D1PathGeometry> D2D1PathGeometry;
		HRESULT Result = D2DDevice.GetD2D1Factory()->CreatePathGeometry(D2D1PathGeometry.GetPP());
		if (Win32::Failed(Result))
		{
			LogWarning(Str("CreateTextLayout Failed: {}"), Result);
			return;
		}
		TReferPtr<ID2D1GeometrySink> D2D1GeometrySink;
		D2D1PathGeometry->Open(D2D1GeometrySink.GetPP());

		D2D1GeometrySink->BeginFigure({ BezierPoints[0].X, BezierPoints[0].Y }, D2D1_FIGURE_BEGIN_FILLED);
		D2D1GeometrySink->AddBezier(
			{
				BezierPoints[1].X, BezierPoints[1].Y,
				BezierPoints[2].X, BezierPoints[2].Y,
				BezierPoints[3].X, BezierPoints[3].Y,
			});
		D2D1GeometrySink->EndFigure(D2D1_FIGURE_END_OPEN);

		D2D1GeometrySink->Close();

		D2D1RenderTarget->DrawGeometry(D2D1PathGeometry.Get(), GetBrush(Brush), LineWidth);
	}

	void FD2DGraphics::DrawRectangle(const RectF & Rect, const FBrush & Brush, float32_t LineWidth)
	{
		D2D1RenderTarget->DrawRectangle({ 
			CurrentTranslation.X + Rect.X,
			CurrentTranslation.Y + Rect.Y,
			CurrentTranslation.X + Rect.X + Rect.Width,
			CurrentTranslation.Y + Rect.Y + Rect.Height }, GetBrush(Brush), LineWidth);
	}

	void FD2DGraphics::DrawString(FStringView String, const PointF & Point, const FTextFormat & TextFormat, const FBrush & Brush)
	{
		FWString StringW = Strings::ToWide(String);
		IDWriteTextFormat * DWriteTextFormat = GetDWriteTextFormat(TextFormat);
		DWriteTextFormat->SetWordWrapping(FromTextWrap(TextFormat.Flags));

		//TReferPtr<IDWriteTextLayout> DWriteTextLayout;
		//HRESULT Result = D2DDevice.GetDWriteFactory3()->CreateTextLayout(StringW.Data, (uint32_t)StringW.Size, DWriteTextFormat, Float32Max, Float32Max, DWriteTextLayout.GetPP());
		//if (Win32::Failed(Result))
		//{
		//	LogWarning(Str("CreateTextLayout Failed: {}"), ResultStr(Result));
		//	return;
		//}

		//DWRITE_TEXT_METRICS DWriteTextMetrics = {};
		//DWriteTextLayout->GetMetrics(&DWriteTextMetrics);

		//D2D_POINT_2F D2DPoint = { Point.X, Point.Y };
		//if (TextFormat.Flags & ETextFlag::AlignCenter)
		//	D2DPoint.x -= DWriteTextMetrics.width * 0.5f;
		//else if (TextFormat.Flags & ETextFlag::AlignTrailing)
		//	D2DPoint.x -= DWriteTextMetrics.width;
		//else if (TextFormat.Flags & ETextFlag::AlignLeadingJustify)
		//	throw EError::Args;
		//else {}

		//if (TextFormat.Flags & ETextFlag::LineAlignFar)
		//	D2DPoint.y -= DWriteTextMetrics.height;
		//else if (TextFormat.Flags & ETextFlag::LineAlignCenter)
		//	throw EError::Args;
		//else {}

		//DWriteTextLayout->SetTextAlignment(FromTextAlignment(TextFormat.Flags));
		////DWriteTextLayout->SetParagraphAlignment(FromParagraphAlignment(TextFormat.Flags));
		//D2D1RenderTarget->DrawTextLayout(D2DPoint, DWriteTextLayout.Get(), GetBrush(Brush), D2D1_DRAW_TEXT_OPTIONS_NO_SNAP);

		//D2D1RenderTarget->DrawTextW(StringW.Data, StringW.Size, DWriteTextFormat, D2D1_RECT_F{ Point.X, Point.Y, Float32Max, Float32Max }, GetBrush(Brush));
	}

	void FD2DGraphics::DrawString(FStringView String, const RectF & Rect, const FTextFormat & TextFormat, const FBrush & Brush)
	{
		FWString StringW = Strings::ToWide(String);
		IDWriteTextFormat * DWriteTextFormat = GetDWriteTextFormat(TextFormat);
		DWriteTextFormat->SetWordWrapping(FromTextWrap(TextFormat.Flags));
		DWriteTextFormat->SetTextAlignment(FromTextAlignment(TextFormat.Flags));
		DWriteTextFormat->SetParagraphAlignment(FromParagraphAlignment(TextFormat.Flags));

		//TReferPtr<IDWriteTextLayout> DWriteTextLayout;
		//HRESULT Result = D2DDevice.GetDWriteFactory3()->CreateTextLayout(StringW.Data, (uint32_t)StringW.Size, DWriteTextFormat, Rect.Width, Rect.Height, DWriteTextLayout.GetPP());
		//if (Win32::Failed(Result))
		//{
		//	LogWarning(Str("CreateTextLayout Failed: {}"), ResultStr(Result));
		//	return;
		//}

		//DWRITE_TEXT_METRICS DWriteTextMetrics = {};
		//DWriteTextLayout->GetMetrics(&DWriteTextMetrics);

		//DWriteTextLayout->SetFontSize(50, { 0, 4 });
		//DWRITE_TEXT_METRICS DWriteTextMetrics2 = {};
		//DWriteTextLayout->GetMetrics(&DWriteTextMetrics2);

		//D2D_POINT_2F D2DPoint = { Rect.X, Rect.Y };

		//D2D1RenderTarget->DrawTextLayout(D2DPoint, DWriteTextLayout.Get(), GetBrush(Brush), D2D1_DRAW_TEXT_OPTIONS_NO_SNAP);

		D2D1RenderTarget->DrawText(StringW.Data, StringW.Size, DWriteTextFormat, D2D1_RECT_F{ CurrentTranslation.X + Rect.X, CurrentTranslation.Y + Rect.Y, Float32Max, Float32Max }, GetBrush(Brush));
	}

	void FD2DGraphics::DrawCircle(const PointF & Center, float32_t Radius, const FBrush & Brush, float32_t LineWidth)
	{
		DrawEllipse(Center, { Radius, Radius }, Brush, LineWidth);
	}

	void FD2DGraphics::DrawEllipse(const PointF & Center, const SizeF & Radius, const FBrush & Brush, float32_t LineWidth)
	{
		D2D1RenderTarget->DrawEllipse({ { Center.X, Center.Y }, Radius.Width, Radius.Height }, GetBrush(Brush), LineWidth);
	}

	void FD2DGraphics::DrawPath(TView<FPathPoint> PathPoints, const PointF & Point, const FBrush & Brush, float32_t LineWidth)
	{
		if (PathPoints.IsEmpty())
			return;

		TReferPtr<ID2D1PathGeometry> D2D1PathGeometry = D2DPathToGeometry(*D2DDevice.GetD2D1Factory(), PathPoints, Point);
		D2D1RenderTarget->DrawGeometry(D2D1PathGeometry.Get(), GetBrush(Brush), LineWidth);
	}

	void FD2DGraphics::FillRectangle(const RectF & Rect, const FBrush & Brush)
	{
		D2D1RenderTarget->FillRectangle({ 
			CurrentTranslation.X + Rect.X,
			CurrentTranslation.Y + Rect.Y,
			CurrentTranslation.X + Rect.X + Rect.Width,
			CurrentTranslation.Y + Rect.Y + Rect.Height }, GetBrush(Brush));
	}

	void FD2DGraphics::FillCircle(const PointF & Center, float32_t Radius, const FBrush & Brush)
	{
		FillEllipse(Center, { Radius, Radius }, Brush);
	}

	void FD2DGraphics::FillEllipse(const PointF & Center, const SizeF & Radius, const FBrush & Brush)
	{
		D2D1RenderTarget->FillEllipse({ { Center.X, Center.Y }, Radius.Width, Radius.Height }, GetBrush(Brush));
	}

	void FD2DGraphics::FillPath(TView<FPathPoint> PathPoints, const PointF & Point, const FBrush & Brush)
	{
		if (PathPoints.IsEmpty())
			return;

		TReferPtr<ID2D1PathGeometry> D2D1PathGeometry = D2DPathToGeometry(*D2DDevice.GetD2D1Factory(), PathPoints, Point);
		D2D1RenderTarget->FillGeometry(D2D1PathGeometry.Get(), GetBrush(Brush));
	}


	ID2D1SolidColorBrush * FD2DGraphics::GetSolidBrush(FColor Color)
	{
		auto Iter = SolidBrushs.Find(Color);
		if (Iter != SolidBrushs.End())
			return Iter->Value.Get();

		TReferPtr<ID2D1SolidColorBrush> D2D1SolidColorBrush;
		HRESULT Result = D2D1RenderTarget->CreateSolidColorBrush({ Color.R, Color.G, Color.B, Color.A }, D2D1SolidColorBrush.GetPP());
		if (Win32::Failed(Result))
		{
			LogWarning(Str("CreateSolidColorBrush Failed: {}"), ResultStr(Result));
			return nullptr;
		}

		SolidBrushs.InsertOrAssign(Color, D2D1SolidColorBrush);
		return D2D1SolidColorBrush.Get();
	}

	ID2D1SolidColorBrush * FD2DGraphics::GetBrush(const FBrush & Brush)
	{
		if (Brush.GetBrushType() == EBrushType::Solid)
		{
			const FSolidBrush & SolidBrush = static_cast<const FSolidBrush&>(Brush);

			auto Iter = SolidBrushs.Find(SolidBrush.Color);
			if (Iter != SolidBrushs.End())
				return Iter->Value.Get();

			TReferPtr<ID2D1SolidColorBrush> D2D1SolidColorBrush;
			FColor Color = SolidBrush.Color;
			HRESULT Result = D2D1RenderTarget->CreateSolidColorBrush({ Color.R, Color.G, Color.B, Color.A }, D2D1SolidColorBrush.GetPP());
			if (Win32::Failed(Result))
			{
				LogWarning(Str("CreateSolidColorBrush Failed: {}"), ResultStr(Result));
				return nullptr;
			}

			SolidBrushs.InsertOrAssign(SolidBrush.Color, D2D1SolidColorBrush);
			return D2D1SolidColorBrush.Get();
		}

		throw EError::NotImplemented;
	}

	IDWriteTextFormat * FD2DGraphics::GetDWriteTextFormat(const FTextFormat & TextFormat)
	{
		return D2DDevice.GetDWriteTextFormat(TextFormat);
	}
}
