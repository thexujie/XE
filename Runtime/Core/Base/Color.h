#pragma once

#include "Vec4.h"

namespace XE
{
    class CORE_API FColor32 : public TVec4<uint8_t>
    {
	public:
		FColor32() = default;
		constexpr FColor32(const FColor32 &) = default;
        FColor32 & operator=(const FColor32 &) = default;
        ~FColor32() = default;
		bool operator==(const FColor32 &) const noexcept = default;
		auto operator <=>(const FColor32 & Another) const = default; // { return TVec4<uint8_t>::operator<=>(Another); }
    	
        constexpr FColor32(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA = 0xFF) : TVec4(InR, InG, InB, InA) {}
        constexpr FColor32(uint32_t ARGB) :
            FColor32(
                uint8_t((ARGB >> 16) & 0xFF), 
                uint8_t((ARGB >> 8) & 0xFF),
                uint8_t((ARGB >> 0) & 0xFF), 
                uint8_t((ARGB >> 24) & 0xFF)) {}
    	
        constexpr uint32_t ToARGB() const
        {
            return (W << 24) | (X << 16) | (Y << 8) | Z;
        }

		explicit operator bool() const { return A > 0; }
    	
        static FColor32 FromARGB(uint32_t ARGB)
        {
            FColor32 Result;
            Result.A = (ARGB >> 24) & 0xFF;
            Result.R = (ARGB >> 16) & 0xFF;
            Result.G = (ARGB >> 8) & 0xFF;
            Result.B = (ARGB >> 0) & 0xFF;
            return Result;
        }
    };

    namespace Colors
    {
		constexpr FColor32 None = 0x00000000;
		constexpr FColor32 AliceBlue(0xFFF0F8FF);
		constexpr FColor32 AntiqueWhite = 0xFFFAEBD7;
		constexpr FColor32 Aqua = 0xFF00FFFF;
		constexpr FColor32 Aquamarine = 0xFF7FFFD4;
		constexpr FColor32 Azure = 0xFFF0FFFF;
		constexpr FColor32 Beige = 0xFFF5F5DC;
		constexpr FColor32 Bisque = 0xFFFFE4C4;
		constexpr FColor32 Black = 0xFF000000;
		constexpr FColor32 BlanchedAlmond = 0xFFFFEBCD;
		constexpr FColor32 Blue = 0xFF0000FF;
		constexpr FColor32 BlueViolet = 0xFF8A2BE2;
		constexpr FColor32 Brown = 0xFFA52A2A;
		constexpr FColor32 BurlyWood = 0xFFDEB887;
		constexpr FColor32 CadetBlue = 0xFF5F9EA0;
		constexpr FColor32 Chartreuse = 0xFF7FFF00;
		constexpr FColor32 Chocolate = 0xFFD2691E;
		constexpr FColor32 Coral = 0xFFFF7F50;
		constexpr FColor32 CornflowerBlue = 0xFF6495ED;
		constexpr FColor32 Cornsilk = 0xFFFFF8DC;
		constexpr FColor32 Crimson = 0xFFDC143C;
		constexpr FColor32 Cyan = 0xFF00FFFF;
		constexpr FColor32 DarkBlue = 0xFF00008B;
		constexpr FColor32 DarkCyan = 0xFF008B8B;
		constexpr FColor32 DarkGoldenRod = 0xFFB8860B;
		constexpr FColor32 DarkGray = 0xFFA9A9A9;
		constexpr FColor32 DarkGreen = 0xFF006400;
		constexpr FColor32 DarkKhaki = 0xFFBDB76B;
		constexpr FColor32 DarkMagenta = 0xFF8B008B;
		constexpr FColor32 DarkOliveGreen = 0xFF556B2F;
		constexpr FColor32 DarkOrange = 0xFFFF8C00;
		constexpr FColor32 DarkOrchid = 0xFF9932CC;
		constexpr FColor32 DarkRed = 0xFF8B0000;
		constexpr FColor32 DarkSalmon = 0xFFE9967A;
		constexpr FColor32 DarkSeaGreen = 0xFF8FBC8F;
		constexpr FColor32 DarkSlateBlue = 0xFF483D8B;
		constexpr FColor32 DarkSlateGray = 0xFF2F4F4F;
		constexpr FColor32 DarkTurquoise = 0xFF00CED1;
		constexpr FColor32 DarkViolet = 0xFF9400D3;
		constexpr FColor32 DeepPink = 0xFFFF1493;
		constexpr FColor32 DeepSkyBlue = 0xFF00BFFF;
		constexpr FColor32 DimGray = 0xFF696969;
		constexpr FColor32 DodgerBlue = 0xFF1E90FF;
		constexpr FColor32 FireBrick = 0xFFB22222;
		constexpr FColor32 FloralWhite = 0xFFFFFAF0;
		constexpr FColor32 ForestGreen = 0xFF228B22;
		constexpr FColor32 Fuchsia = 0xFFFF00FF;
		constexpr FColor32 Gainsboro = 0xFFDCDCDC;
		constexpr FColor32 GhostWhite = 0xFFF8F8FF;
		constexpr FColor32 Gold = 0xFFFFD700;
		constexpr FColor32 GoldenRod = 0xFFDAA520;
		constexpr FColor32 Gray = 0xFF808080;
		constexpr FColor32 Green = 0xFF008000;
		constexpr FColor32 GreenYellow = 0xFFADFF2F;
		constexpr FColor32 HoneyDew = 0xFFF0FFF0;
		constexpr FColor32 HotPink = 0xFFFF69B4;
		constexpr FColor32 IndianRed = 0xFFCD5C5C;
		constexpr FColor32 Indigo = 0xFF4B0082;
		constexpr FColor32 Ivory = 0xFFFFFFF0;
		constexpr FColor32 Khaki = 0xFFF0E68C;
		constexpr FColor32 Lavender = 0xFFE6E6FA;
		constexpr FColor32 LavenderBlush = 0xFFFFF0F5;
		constexpr FColor32 LawnGreen = 0xFF7CFC00;
		constexpr FColor32 LemonChiffon = 0xFFFFFACD;
		constexpr FColor32 LightBlue = 0xFFADD8E6;
		constexpr FColor32 LightCoral = 0xFFF08080;
		constexpr FColor32 LightCyan = 0xFFE0FFFF;
		constexpr FColor32 LightGoldenRodYellow = 0xFFFAFAD2;
		constexpr FColor32 LightGray = 0xFFD3D3D3;
		constexpr FColor32 LightGreen = 0xFF90EE90;
		constexpr FColor32 LightPink = 0xFFFFB6C1;
		constexpr FColor32 LightSalmon = 0xFFFFA07A;
		constexpr FColor32 LightSeaGreen = 0xFF20B2AA;
		constexpr FColor32 LightSkyBlue = 0xFF87CEFA;
		constexpr FColor32 LightSlateGray = 0xFF778899;
		constexpr FColor32 LightSteelBlue = 0xFFB0C4DE;
		constexpr FColor32 LightYellow = 0xFFFFFFE0;
		constexpr FColor32 Lime = 0xFF00FF00;
		constexpr FColor32 LimeGreen = 0xFF32CD32;
		constexpr FColor32 Linen = 0xFFFAF0E6;
		constexpr FColor32 Magenta = 0xFFFF00FF;
		constexpr FColor32 Maroon = 0xFF800000;
		constexpr FColor32 MediumAquaMarine = 0xFF66CDAA;
		constexpr FColor32 MediumBlue = 0xFF0000CD;
		constexpr FColor32 MediumOrchid = 0xFFBA55D3;
		constexpr FColor32 MediumPurple = 0xFF9370DB;
		constexpr FColor32 MediumSeaGreen = 0xFF3CB371;
		constexpr FColor32 MediumSlateBlue = 0xFF7B68EE;
		constexpr FColor32 MediumSpringGreen = 0xFF00FA9A;
		constexpr FColor32 MediumTurquoise = 0xFF48D1CC;
		constexpr FColor32 MediumVioletRed = 0xFFC71585;
		constexpr FColor32 MidnightBlue = 0xFF191970;
		constexpr FColor32 MintCream = 0xFFF5FFFA;
		constexpr FColor32 MistyRose = 0xFFFFE4E1;
		constexpr FColor32 Moccasin = 0xFFFFE4B5;
		constexpr FColor32 NavajoWhite = 0xFFFFDEAD;
		constexpr FColor32 Navy = 0xFF000080;
		constexpr FColor32 OldLace = 0xFFFDF5E6;
		constexpr FColor32 Olive = 0xFF808000;
		constexpr FColor32 OliveDrab = 0xFF6B8E23;
		constexpr FColor32 Orange = 0xFFFFA500;
		constexpr FColor32 OrangeRed = 0xFFFF4500;
		constexpr FColor32 Orchid = 0xFFDA70D6;
		constexpr FColor32 PaleGoldenRod = 0xFFEEE8AA;
		constexpr FColor32 PaleGreen = 0xFF98FB98;
		constexpr FColor32 PaleTurquoise = 0xFFAFEEEE;
		constexpr FColor32 PaleVioletRed = 0xFFDB7093;
		constexpr FColor32 PapayaWhip = 0xFFFFEFD5;
		constexpr FColor32 PeachPuff = 0xFFFFDAB9;
		constexpr FColor32 Peru = 0xFFCD853F;
		constexpr FColor32 Pink = 0xFFFFC0CB;
		constexpr FColor32 Plum = 0xFFDDA0DD;
		constexpr FColor32 PowderBlue = 0xFFB0E0E6;
		constexpr FColor32 Purple = 0xFF800080;
		constexpr FColor32 Red = 0xFFFF0000;
		constexpr FColor32 RosyBrown = 0xFFBC8F8F;
		constexpr FColor32 RoyalBlue = 0xFF4169E1;
		constexpr FColor32 SaddleBrown = 0xFF8B4513;
		constexpr FColor32 Salmon = 0xFFFA8072;
		constexpr FColor32 SandyBrown = 0xFFF4A460;
		constexpr FColor32 SeaGreen = 0xFF2E8B57;
		constexpr FColor32 SeaShell = 0xFFFFF5EE;
		constexpr FColor32 Sienna = 0xFFA0522D;
		constexpr FColor32 Silver = 0xFFC0C0C0;
		constexpr FColor32 SkyBlue = 0xFF87CEEB;
		constexpr FColor32 SlateBlue = 0xFF6A5ACD;
		constexpr FColor32 SlateGray = 0xFF708090;
		constexpr FColor32 Snow = 0xFFFFFAFA;
		constexpr FColor32 SpringGreen = 0xFF00FF7F;
		constexpr FColor32 SteelBlue = 0xFF4682B4;
		constexpr FColor32 Tan = 0xFFD2B48C;
		constexpr FColor32 Teal = 0xFF008080;
		constexpr FColor32 Thistle = 0xFFD8BFD8;
		constexpr FColor32 Tomato = 0xFFFF6347;
		constexpr FColor32 Turquoise = 0xFF40E0D0;
		constexpr FColor32 Violet = 0xFFEE82EE;
		constexpr FColor32 Wheat = 0xFFF5DEB3;
		constexpr FColor32 White = 0xFFFFFFFF;
		constexpr FColor32 WhiteSmoke = 0xFFF5F5F5;
		constexpr FColor32 Yellow = 0xFFFFFF00;
		constexpr FColor32 YellowGreen = 0xFF9ACD32;

		CORE_API FStringView ColorName(FColor32 ColorValue);
		CORE_API FColor32 ColorValue(FStringView ColorName, bool * IsSystemColor = nullptr);

    	struct FSystemColor
    	{
			FColor32 Color;
			FStringView Name;
    	};

		TView<FSystemColor> SystemColors();
	}

    class CORE_API FColor : public TVec4<float32_t>
    {
	public:
        FColor() = default;
        FColor(const FColor &) = default;
        FColor & operator=(const FColor &) = default;
        ~FColor() = default;
		friend bool operator == (const FColor &, const FColor &) = default;
		friend auto operator <=> (const FColor &, const FColor &) = default;
        constexpr FColor(float32_t InR, float32_t InG, float32_t InB, float32_t InA = 1.0f) : TVec4(InR, InG, InB, InA) {}
        constexpr FColor(uint32_t ARGB) :
            FColor(((ARGB >> 16) & 0xFF) / 255.0f,
                ((ARGB >> 8) & 0xFF) / 255.0f,
                ((ARGB >> 0) & 0xFF) / 255.0f,
                ((ARGB >> 24) & 0xFF) / 255.0f) {}
        constexpr FColor(const FColor32 & Color32) :TVec4(Color32.X / 255.0f, Color32.Y / 255.0f, Color32.Z / 255.0f, Color32.W / 255.0f) {}

        FColor32 ToColor32()
        {
            return FColor32(uint8_t(RoundU8(R * 255)), uint8_t(RoundU8(G * 255)), uint8_t(RoundU8(B * 255)), uint8_t(RoundU8(A * 255)));
        }

		explicit operator bool() const { return A > 0.0f; }
    	
        friend FArchive & operator >>(FArchive & Serializer, FColor & Color)
        {
            Serializer >> Color.R;
            Serializer >> Color.G;
            Serializer >> Color.B;
            Serializer >> Color.A;
            return Serializer;
        }
    };
}
