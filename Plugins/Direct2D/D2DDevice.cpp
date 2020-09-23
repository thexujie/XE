#include "PCH.h"
#include "D2DDevice.h"

#include "D2DBitmap.h"
#include "D2DGraphics.h"
#include "D2DTextLayout.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "imm32.lib")

namespace XE::Direct2D
{
	// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
	struct FDWriteTextRenderer : TUnkown<IDWriteTextRenderer>
	{
		FDWriteTextRenderer(TVector<FPathPoint> & PathPointsIn) : PathPoints(PathPointsIn) {}
		TVector<FPathPoint> & PathPoints;

		HRESULT IsPixelSnappingDisabled(void * clientDrawingContext, BOOL * isDisabled) override
		{
			// Enable pixel snapping of the text baselines,
			// since we're not animating and don't want blurry text.
			*isDisabled = FALSE;
			return S_OK;
		}
		HRESULT GetCurrentTransform(void * clientDrawingContext, DWRITE_MATRIX * transform) override
		{
			return S_OK;
		}
		HRESULT GetPixelsPerDip(void * clientDrawingContext, FLOAT * pixelsPerDip) override
		{
			// Any scaling will be combined into matrix transforms rather than an
			// additional DPI scaling. This simplifies the logic for rendering
			// and hit-testing. If an application does not use matrices, then
			// using the scaling factor directly is simpler.
			*pixelsPerDip = 1;
			return S_OK;
		}
		HRESULT DrawGlyphRun(void * clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const * glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION const * glyphRunDescription, IUnknown * clientDrawingEffect) override
		{
			struct FDWriteGeometrySink : public TUnkown<IDWriteGeometrySink>
			{
				FDWriteGeometrySink(TVector<FPathPoint> & PathPointsIn, FLOAT baselineOriginYIn) : PathPoints(PathPointsIn), BaseLineY(baselineOriginYIn) {}
				TVector<FPathPoint> & PathPoints;
				FLOAT BaseLineY;

				void SetFillMode(D2D1_FILL_MODE fillMode) override
				{
					switch (fillMode)
					{
					case D2D1_FILL_MODE_ALTERNATE:
						PathPoints.Add({ EPathCommand::FillAlternate, {} });
						break;
					case D2D1_FILL_MODE_WINDING:
						PathPoints.Add({ EPathCommand::FillWinding, {} });
						break;
					default:
						break;
					}
				}
				void SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags) override
				{

				}
				void BeginFigure(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin) override
				{
					PathPoints.Add({ EPathCommand::MoveTo, { startPoint.x, BaseLineY + startPoint.y } });
				}
				void AddLines(const D2D1_POINT_2F * points, UINT32 pointsCount) override
				{
					for (auto Index = 0u; Index < pointsCount; ++Index)
					{
						PathPoints.Add({ EPathCommand::LineTo, { points[Index].x, BaseLineY + points[Index].y } });
					}
				}
				void AddBeziers(const D2D1_BEZIER_SEGMENT * beziers, UINT32 beziersCount) override
				{
					for (auto Index = 0u; Index < beziersCount; ++Index)
					{
						PathPoints.Add({ EPathCommand::Point, { beziers[Index].point1.x, BaseLineY + beziers[Index].point1.y } });
						PathPoints.Add({ EPathCommand::Point, { beziers[Index].point2.x, BaseLineY + beziers[Index].point2.y } });
						PathPoints.Add({ EPathCommand::Bezier4To, { beziers[Index].point3.x, BaseLineY + beziers[Index].point3.y } });
					}
				}
				void EndFigure(D2D1_FIGURE_END figureEnd) override
				{
					PathPoints.Add({ EPathCommand::Close, {} });
				}
				HRESULT Close() override
				{
					return S_OK;
				}
			};

			FDWriteGeometrySink DWriteGeometrySink(PathPoints, baselineOriginY);
			glyphRun->fontFace->GetGlyphRunOutline(glyphRun->fontEmSize, glyphRun->glyphIndices, glyphRun->glyphAdvances, glyphRun->glyphOffsets, glyphRun->glyphCount, glyphRun->isSideways,
				!!(glyphRun->bidiLevel % 2), &DWriteGeometrySink);
			return S_OK;
		}

		HRESULT DrawUnderline(void * clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_UNDERLINE const * underline, IUnknown * clientDrawingEffect) override
		{
			return S_OK;
		}

		HRESULT DrawStrikethrough(void * clientDrawingContext, FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_STRIKETHROUGH const * strikethrough, IUnknown * clientDrawingEffect) override
		{
			return S_OK;

		}
		HRESULT DrawInlineObject(void * clientDrawingContext, FLOAT originX, FLOAT originY, IDWriteInlineObject * inlineObject, BOOL isSideways, BOOL isRightToLeft, IUnknown * clientDrawingEffect) override
		{

			return S_OK;
		}
	};

	
	static FWStringView GetDefaultFontName()
	{
		NONCLIENTMETRICS metrics;
		metrics.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
		FWString & FontName = GetThreadLocalStringBuffer<wchar_t>();
		FontName = metrics.lfMessageFont.lfFaceName;
		return FontName;
	}

	static float32_t GetDefaultFontSize()
	{
		NONCLIENTMETRICS metrics;
		metrics.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
		if (metrics.lfMessageFont.lfHeight < 0)
		{
			return -static_cast<float32_t>(metrics.lfMessageFont.lfHeight);
			//HDC hdcScreen = GetDC(NULL);
			//int Height = -MulDiv(metrics.lfMessageFont.lfHeight, 72, GetDeviceCaps(hdcScreen, LOGPIXELSY));
			//ReleaseDC(NULL, hdcScreen);
			//return Height;
		}
		return static_cast<float32_t>(metrics.lfMessageFont.lfHeight);
	}

	EError FD2DDevice::Initialize()
	{
		HRESULT Result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, D2D1Factory.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("D2D1CreateFactory Failed: {}"), Result);
			return EError::Inner;
		}

		Result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), DWriteFactory3.GetTT<IUnknown>());
		if (Win32::Failed(Result))
		{
			LogError(Str("DWriteCreateFactory Failed: {}"), Result);
			return EError::Inner;
		}

		Result = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), WICImagingFactory.GetVV());
		if (Win32::Failed(Result))
		{
			LogError(Str("CoCreateInstance(CLSID_WICImagingFactory) Failed: {}"), Result);
			return EError::Inner;
		}

		return EError::OK;
	}
	
	void FD2DDevice::Finalize()
	{
		DWriteTextFormats.Reset();
		WICImagingFactory.Reset();
		DWriteFactory3.Reset();
		D2D1Factory.Reset();
	}
	
	TVector<FPathPoint> FD2DDevice::StringToPath(FStringView String, const SizeF & LayoutSize, const FTextFormat & TextFormat)
	{
		TVector<FPathPoint> PathPoints;
		FWString StringW = Strings::ToWide(String);
		IDWriteTextFormat * DWriteTextFormat = GetDWriteTextFormat(TextFormat);
		DWriteTextFormat->SetWordWrapping(FromTextWrap(TextFormat.Flags));
		DWriteTextFormat->SetTextAlignment(FromTextAlignment(TextFormat.Flags));
		DWriteTextFormat->SetParagraphAlignment(FromParagraphAlignment(TextFormat.Flags));

		TReferPtr<IDWriteTextLayout> DWriteTextLayout;
		HRESULT Result = DWriteFactory3->CreateTextLayout(StringW.Data, (uint32_t)StringW.Size, DWriteTextFormat, LayoutSize.Width, LayoutSize.Height, DWriteTextLayout.GetPP());
		if (Win32::Failed(Result))
		{
			LogWarning(Str("CreateTextLayout Failed: {}"), Result);
			return PathPoints;
		}

		FDWriteTextRenderer DWriteTextRenderer = { PathPoints };
		DWriteTextLayout->Draw(0, &DWriteTextRenderer, 0, 0);
		return PathPoints;
	}

	TReferPtr<ITextLayout> FD2DDevice::CreateTextLayout(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture)
	{
		TReferPtr<FD2DTextLayout> D2DTextLayout = MakeRefer<FD2DTextLayout>(*this);
		D2DTextLayout->Load(Text, TextFormat, Culture);
		return D2DTextLayout;
	}
	
	TSharedPtr<FPixmap> FD2DDevice::CreateBitmap(const SizeU & Size, EFormat Format)
	{
		return MakeShared<FD2DBitmap>(*this, Size, Format);
	}

	TSharedPtr<FGraphics> FD2DDevice::CreateGraphics(ptr_t WindowId)
	{
		return MakeShared<FD2DGraphics>(*this, static_cast<HWND>(WindowId));
	}
	
	TSharedPtr<FGraphics> FD2DDevice::CreateGraphics(FPixmap & GrpBitmap)
	{
		FD2DBitmap & D2DBitmap = static_cast<FD2DBitmap &>(GrpBitmap);
		return MakeShared<FD2DGraphics>(*this, D2DBitmap);
	}
	
	IDWriteTextFormat * FD2DDevice::GetDWriteTextFormat(const FTextFormat & TextFormat)
	{
		auto Iter = DWriteTextFormats.Find(TextFormat);
		if (Iter != DWriteTextFormats.End())
			return Iter->Value.Get();

		IDWriteFontCollection * DWriteFontCollection = nullptr;

		TReferPtr<IDWriteTextFormat> DWriteTextFormat;
		FWString FontFamilyW = TextFormat.Typeface.FontFamily.IsEmpty() ? GetDefaultFontName() : Strings::ToWide(TextFormat.Typeface.FontFamily);
		HRESULT Result = DWriteFactory3->CreateTextFormat(FontFamilyW.Data, DWriteFontCollection,
			(DWRITE_FONT_WEIGHT)TextFormat.Typeface.FontWeight,
			(TextFormat.Typeface.FontFlags * EFontFlags::Italic) ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_ULTRA_CONDENSED,
			TextFormat.FontSize > 0 ? TextFormat.FontSize : GetDefaultFontSize(),
			L"",
			DWriteTextFormat.GetPP());
		if (Win32::Failed(Result))
		{
			LogWarning(Str("CreateTextFormat Failed: {}"), Result);
			return nullptr;
		}

		DWriteTextFormats.InsertOrAssign(TextFormat, DWriteTextFormat);
		return DWriteTextFormat.Get();
	}
}
