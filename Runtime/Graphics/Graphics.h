#pragma once

#include "GraphicsInc.h"
#include "Typeface.h"
#include "TextLayout.h"
#include "FormattedText.h"
#include "Path.h"
#include "Pixmap.h"
#include "Brush.h"
#include "Surface.h"
#include "Imaging/Image.h"

namespace XE
{
	class GRAPHICS_API FGraphics : public FObject
	{
	public:
		FGraphics() = default;
		~FGraphics() = default;

		virtual void Clear(FColor Color) = 0;
		virtual void Flush() = 0;
		virtual void Resize(SizeU Size) = 0;

		virtual void PushTranslation(const PointF & Translation) = 0;
		virtual void Pop() = 0;
		
		virtual void DrawLine(const PointF & Start, const PointF & End, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f) = 0;
		virtual void DrawBezier(const TArray<PointF, 4> & BezierPoints, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f) = 0;
		virtual void DrawRectangle(const RectF & Rect, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f) = 0;
		virtual void DrawString(FStringView String, const PointF & Point, const FTextFormat & TextFormat, const FBrush & Brush = FSolidBrush(Colors::Black)) = 0;
		virtual void DrawString(FStringView String, const RectF & Rect, const FTextFormat & TextFormat, const FBrush & Brush = FSolidBrush(Colors::Black)) = 0;

		virtual void DrawCircle(const PointF & Center, float32_t Radius, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f) = 0;
		virtual void DrawEllipse(const PointF & Center, const SizeF & Radius, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f) = 0;
		virtual void DrawPath(TView<FPathPoint> PathPoints, const PointF & Point, const FBrush & Brush = FSolidBrush(Colors::Black), float32_t LineWidth = 1.0f) = 0;

		virtual void FillRectangle(const RectF & Rect, const FBrush & Brush = FSolidBrush(Colors::Black)) = 0;

		virtual void FillCircle(const PointF & Center, float32_t Radius, const FBrush & Brush = FSolidBrush(Colors::Black)) = 0;
		virtual void FillEllipse(const PointF & Center, const SizeF & Radius, const FBrush & Brush = FSolidBrush(Colors::Black)) = 0;
		virtual void FillPath(TView<FPathPoint> PathPoints, const PointF & Point, const FBrush & Brush = FSolidBrush(Colors::Black)) = 0;
	};

	class GRAPHICS_API FGraphicsService : public IService
	{
	public:

		virtual EError Initialize() = 0;

		virtual TReferPtr<ITextLayout> CreateTextLayout(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture) = 0;

		virtual TSharedPtr<FPixmap> CreateBitmap(const SizeU & Size, EFormat Format = EFormat::BGRA) = 0;

		virtual TSharedPtr<FGraphics> CreateGraphics(ptr_t WindowId) = 0;
		virtual TSharedPtr<FGraphics> CreateGraphics(FPixmap & GrpBitmap) = 0;

		virtual TVector<FPathPoint> StringToPath(FStringView String, const SizeF & LayoutSize, const FTextFormat & TextFormat) = 0;
	};
	CLASS_UUID(FGraphicsService, "4177AE52-8BD9-4009-B4E1-ADB31CA1520C");
}
