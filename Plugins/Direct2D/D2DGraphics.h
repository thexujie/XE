#pragma once

#include "D2DInc.h"

namespace XE::Direct2D
{
	class FD2DGraphics : public FGraphics
	{
	public:
		FD2DGraphics(FD2DDevice & D2DDeviceIn, HWND hWnd);
		FD2DGraphics(FD2DDevice & D2DDeviceIn, FSurface & Surface);
		FD2DGraphics(FD2DDevice & D2DDeviceIn, FD2DBitmap & Bitmap);
		~FD2DGraphics();

	public:
		void Clear(FColor Color);
		void Flush();
		void Resize(SizeU Size);

		void PushTranslation(const PointF & Translation);
		void Pop();
		
		void DrawLine(const PointF & Start, const PointF & End, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f);
		void DrawBezier(const TArray<PointF, 4> & BezierPoints, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f);
		void DrawRectangle(const RectF & Rect, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f);
		void DrawString(FStringView String, const PointF & Point, const FTextFormat & TextFormat, const FBrush & Brush = FSolidBrush(Colors::Black));
		void DrawString(FStringView String, const RectF & Rect, const FTextFormat & TextFormat, const FBrush & Brush = FSolidBrush(Colors::Black));

		void DrawCircle(const PointF & Center, float32_t Radius, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f);
		void DrawEllipse(const PointF & Center, const SizeF & Radius, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f);
		void DrawPath(TView<FPathPoint> PathPoints, const PointF & Point, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f);

		void FillRectangle(const RectF & Rect, const FBrush & Brush = FSolidBrush(Colors::Black));

		void FillCircle(const PointF & Center, float32_t Radius, const FBrush & Brush = FSolidBrush(Colors::Black));
		void FillEllipse(const PointF & Center, const SizeF & Radius, const FBrush & Brush = FSolidBrush(Colors::Black));
		void FillPath(TView<FPathPoint> PathPoints, const PointF & Point, const FBrush & Brush = FSolidBrush(Colors::Black));
		
	private:
		ID2D1SolidColorBrush * GetSolidBrush(FColor Color);
		ID2D1SolidColorBrush * GetBrush(const FBrush & Brush);
		IDWriteTextFormat * GetDWriteTextFormat(const FTextFormat & TextFormat);
		
	public:
		FD2DDevice & D2DDevice;

	private:
		TReferPtr<ID2D1RenderTarget> D2D1RenderTarget;
		TMap<FColor, TReferPtr<ID2D1SolidColorBrush>> SolidBrushs;
		TVector<PointF> Translations;
		PointF CurrentTranslation = {};
	};
}

