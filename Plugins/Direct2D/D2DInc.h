#pragma once

#include "Core/Core.h"
#include "Graphics/Graphics.h"
#include "Core/Platform/Win32/Win32.h"

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>

#if XE_WINDOWS
#ifdef XEngine_Direct2D_Module
#define DIRECT2D_API __declspec(dllexport)
#else
#define DIRECT2D_API __declspec(dllimport)
#endif
#endif

namespace XE::Direct2D
{
	using namespace Win32;

	class FD2DDevice;
	class FD2DGraphics;
	class FD2DBitmap;

	inline DWRITE_WORD_WRAPPING FromTextWrap(ETextFlags TextFlags)
	{
		if (TextFlags * ETextFlags::WrapDefault)
			return DWRITE_WORD_WRAPPING_WRAP;
		else if (TextFlags * ETextFlags::WrapWord)
			return DWRITE_WORD_WRAPPING_WHOLE_WORD;
		else if (TextFlags * ETextFlags::WrapChar)
			return DWRITE_WORD_WRAPPING_CHARACTER;
		else
			return DWRITE_WORD_WRAPPING_NO_WRAP;
	}

	inline DWRITE_TEXT_ALIGNMENT FromTextAlignment(ETextFlags TextFlags)
	{
		if (TextFlags * ETextFlags::AlignCenter)
			return DWRITE_TEXT_ALIGNMENT_CENTER;
		else if (TextFlags * ETextFlags::AlignTrailing)
			return DWRITE_TEXT_ALIGNMENT_TRAILING;
		else if (TextFlags * ETextFlags::AlignLeadingJustify)
			return DWRITE_TEXT_ALIGNMENT_JUSTIFIED;
		else
			return DWRITE_TEXT_ALIGNMENT_LEADING;
	}

	inline DWRITE_PARAGRAPH_ALIGNMENT FromParagraphAlignment(ETextFlags TextFlags)
	{
		if (TextFlags * ETextFlags::LineAlignFar)
			return DWRITE_PARAGRAPH_ALIGNMENT_FAR;
		else if (TextFlags * ETextFlags::LineAlignCenter)
			return DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
		else
			return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
	}
}
