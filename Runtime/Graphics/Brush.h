#pragma once

#include "GraphicsInc.h"

namespace XE
{
	enum class EBrushType
	{
		Solid = 0,
	};

	class GRAPHICS_API FBrush : public FChangable
	{
	public:
		virtual EBrushType GetBrushType() const = 0;
	};

	class GRAPHICS_API FSolidBrush : public FBrush
	{
	public:
		constexpr FSolidBrush() = default;
		constexpr FSolidBrush(FColor32 ColorIn) : Color(ColorIn) {}
		EBrushType GetBrushType() const { return EBrushType::Solid; }

		FColor32 Color;
	};

	template<>
	inline constexpr bool IsRawVariableV<FSolidBrush> = true;

	template<>
	class GRAPHICS_API TDataConverter<FBrush> : public TRefer<IDataConverter>
	{
	public:
		FVariant Convert(const IDataProvider * ValueProvider) const;
	};

	namespace Brushs
	{
		inline FSolidBrush None = FSolidBrush(Colors::None);
		inline FSolidBrush AliceBlue = FSolidBrush(Colors::AliceBlue);
		inline FSolidBrush AntiqueWhite = FSolidBrush(Colors::AntiqueWhite);
		inline FSolidBrush Aqua = FSolidBrush(Colors::Aqua);
		inline FSolidBrush Aquamarine = FSolidBrush(Colors::Aquamarine);
		inline FSolidBrush Azure = FSolidBrush(Colors::Azure);
		inline FSolidBrush Beige = FSolidBrush(Colors::Beige);
		inline FSolidBrush Bisque = FSolidBrush(Colors::Bisque);
		inline FSolidBrush Black = FSolidBrush(Colors::Black);
		inline FSolidBrush BlanchedAlmond = FSolidBrush(Colors::BlanchedAlmond);
		inline FSolidBrush Blue = FSolidBrush(Colors::Blue);
		inline FSolidBrush BlueViolet = FSolidBrush(Colors::BlueViolet);
		inline FSolidBrush Brown = FSolidBrush(Colors::Brown);
		inline FSolidBrush BurlyWood = FSolidBrush(Colors::BurlyWood);
		inline FSolidBrush CadetBlue = FSolidBrush(Colors::CadetBlue);
		inline FSolidBrush Chartreuse = FSolidBrush(Colors::Chartreuse);
		inline FSolidBrush Chocolate = FSolidBrush(Colors::Chocolate);
		inline FSolidBrush Coral = FSolidBrush(Colors::Coral);
		inline FSolidBrush CornflowerBlue = FSolidBrush(Colors::CornflowerBlue);
		inline FSolidBrush Cornsilk = FSolidBrush(Colors::Cornsilk);
		inline FSolidBrush Crimson = FSolidBrush(Colors::Crimson);
		inline FSolidBrush Cyan = FSolidBrush(Colors::Cyan);
		inline FSolidBrush DarkBlue = FSolidBrush(Colors::DarkBlue);
		inline FSolidBrush DarkCyan = FSolidBrush(Colors::DarkCyan);
		inline FSolidBrush DarkGoldenRod = FSolidBrush(Colors::DarkGoldenRod);
		inline FSolidBrush DarkGray = FSolidBrush(Colors::DarkGray);
		inline FSolidBrush DarkGreen = FSolidBrush(Colors::DarkGreen);
		inline FSolidBrush DarkKhaki = FSolidBrush(Colors::DarkKhaki);
		inline FSolidBrush DarkMagenta = FSolidBrush(Colors::DarkMagenta);
		inline FSolidBrush DarkOliveGreen = FSolidBrush(Colors::DarkOliveGreen);
		inline FSolidBrush DarkOrange = FSolidBrush(Colors::DarkOrange);
		inline FSolidBrush DarkOrchid = FSolidBrush(Colors::DarkOrchid);
		inline FSolidBrush DarkRed = FSolidBrush(Colors::DarkRed);
		inline FSolidBrush DarkSalmon = FSolidBrush(Colors::DarkSalmon);
		inline FSolidBrush DarkSeaGreen = FSolidBrush(Colors::DarkSeaGreen);
		inline FSolidBrush DarkSlateBlue = FSolidBrush(Colors::DarkSlateBlue);
		inline FSolidBrush DarkSlateGray = FSolidBrush(Colors::DarkSlateGray);
		inline FSolidBrush DarkTurquoise = FSolidBrush(Colors::DarkTurquoise);
		inline FSolidBrush DarkViolet = FSolidBrush(Colors::DarkViolet);
		inline FSolidBrush DeepPink = FSolidBrush(Colors::DeepPink);
		inline FSolidBrush DeepSkyBlue = FSolidBrush(Colors::DeepSkyBlue);
		inline FSolidBrush DimGray = FSolidBrush(Colors::DimGray);
		inline FSolidBrush DodgerBlue = FSolidBrush(Colors::DodgerBlue);
		inline FSolidBrush FireBrick = FSolidBrush(Colors::FireBrick);
		inline FSolidBrush FloralWhite = FSolidBrush(Colors::FloralWhite);
		inline FSolidBrush ForestGreen = FSolidBrush(Colors::ForestGreen);
		inline FSolidBrush Fuchsia = FSolidBrush(Colors::Fuchsia);
		inline FSolidBrush Gainsboro = FSolidBrush(Colors::Gainsboro);
		inline FSolidBrush GhostWhite = FSolidBrush(Colors::GhostWhite);
		inline FSolidBrush Gold = FSolidBrush(Colors::Gold);
		inline FSolidBrush GoldenRod = FSolidBrush(Colors::GoldenRod);
		inline FSolidBrush Gray = FSolidBrush(Colors::Gray);
		inline FSolidBrush Green = FSolidBrush(Colors::Green);
		inline FSolidBrush GreenYellow = FSolidBrush(Colors::GreenYellow);
		inline FSolidBrush HoneyDew = FSolidBrush(Colors::HoneyDew);
		inline FSolidBrush HotPink = FSolidBrush(Colors::HotPink);
		inline FSolidBrush IndianRed = FSolidBrush(Colors::IndianRed);
		inline FSolidBrush Indigo = FSolidBrush(Colors::Indigo);
		inline FSolidBrush Ivory = FSolidBrush(Colors::Ivory);
		inline FSolidBrush Khaki = FSolidBrush(Colors::Khaki);
		inline FSolidBrush Lavender = FSolidBrush(Colors::Lavender);
		inline FSolidBrush LavenderBlush = FSolidBrush(Colors::LavenderBlush);
		inline FSolidBrush LawnGreen = FSolidBrush(Colors::LawnGreen);
		inline FSolidBrush LemonChiffon = FSolidBrush(Colors::LemonChiffon);
		inline FSolidBrush LightBlue = FSolidBrush(Colors::LightBlue);
		inline FSolidBrush LightCoral = FSolidBrush(Colors::LightCoral);
		inline FSolidBrush LightCyan = FSolidBrush(Colors::LightCyan);
		inline FSolidBrush LightGoldenRodYellow = FSolidBrush(Colors::LightGoldenRodYellow);
		inline FSolidBrush LightGray = FSolidBrush(Colors::LightGray);
		inline FSolidBrush LightGreen = FSolidBrush(Colors::LightGreen);
		inline FSolidBrush LightPink = FSolidBrush(Colors::LightPink);
		inline FSolidBrush LightSalmon = FSolidBrush(Colors::LightSalmon);
		inline FSolidBrush LightSeaGreen = FSolidBrush(Colors::LightSeaGreen);
		inline FSolidBrush LightSkyBlue = FSolidBrush(Colors::LightSkyBlue);
		inline FSolidBrush LightSlateGray = FSolidBrush(Colors::LightSlateGray);
		inline FSolidBrush LightSteelBlue = FSolidBrush(Colors::LightSteelBlue);
		inline FSolidBrush LightYellow = FSolidBrush(Colors::LightYellow);
		inline FSolidBrush Lime = FSolidBrush(Colors::Lime);
		inline FSolidBrush LimeGreen = FSolidBrush(Colors::LimeGreen);
		inline FSolidBrush Linen = FSolidBrush(Colors::Linen);
		inline FSolidBrush Magenta = FSolidBrush(Colors::Magenta);
		inline FSolidBrush Maroon = FSolidBrush(Colors::Maroon);
		inline FSolidBrush MediumAquaMarine = FSolidBrush(Colors::MediumAquaMarine);
		inline FSolidBrush MediumBlue = FSolidBrush(Colors::MediumBlue);
		inline FSolidBrush MediumOrchid = FSolidBrush(Colors::MediumOrchid);
		inline FSolidBrush MediumPurple = FSolidBrush(Colors::MediumPurple);
		inline FSolidBrush MediumSeaGreen = FSolidBrush(Colors::MediumSeaGreen);
		inline FSolidBrush MediumSlateBlue = FSolidBrush(Colors::MediumSlateBlue);
		inline FSolidBrush MediumSpringGreen = FSolidBrush(Colors::MediumSpringGreen);
		inline FSolidBrush MediumTurquoise = FSolidBrush(Colors::MediumTurquoise);
		inline FSolidBrush MediumVioletRed = FSolidBrush(Colors::MediumVioletRed);
		inline FSolidBrush MidnightBlue = FSolidBrush(Colors::MidnightBlue);
		inline FSolidBrush MintCream = FSolidBrush(Colors::MintCream);
		inline FSolidBrush MistyRose = FSolidBrush(Colors::MistyRose);
		inline FSolidBrush Moccasin = FSolidBrush(Colors::Moccasin);
		inline FSolidBrush NavajoWhite = FSolidBrush(Colors::NavajoWhite);
		inline FSolidBrush Navy = FSolidBrush(Colors::Navy);
		inline FSolidBrush OldLace = FSolidBrush(Colors::OldLace);
		inline FSolidBrush Olive = FSolidBrush(Colors::Olive);
		inline FSolidBrush OliveDrab = FSolidBrush(Colors::OliveDrab);
		inline FSolidBrush Orange = FSolidBrush(Colors::Orange);
		inline FSolidBrush OrangeRed = FSolidBrush(Colors::OrangeRed);
		inline FSolidBrush Orchid = FSolidBrush(Colors::Orchid);
		inline FSolidBrush PaleGoldenRod = FSolidBrush(Colors::PaleGoldenRod);
		inline FSolidBrush PaleGreen = FSolidBrush(Colors::PaleGreen);
		inline FSolidBrush PaleTurquoise = FSolidBrush(Colors::PaleTurquoise);
		inline FSolidBrush PaleVioletRed = FSolidBrush(Colors::PaleVioletRed);
		inline FSolidBrush PapayaWhip = FSolidBrush(Colors::PapayaWhip);
		inline FSolidBrush PeachPuff = FSolidBrush(Colors::PeachPuff);
		inline FSolidBrush Peru = FSolidBrush(Colors::Peru);
		inline FSolidBrush Pink = FSolidBrush(Colors::Pink);
		inline FSolidBrush Plum = FSolidBrush(Colors::Plum);
		inline FSolidBrush PowderBlue = FSolidBrush(Colors::PowderBlue);
		inline FSolidBrush Purple = FSolidBrush(Colors::Purple);
		inline FSolidBrush Red = FSolidBrush(Colors::Red);
		inline FSolidBrush RosyBrown = FSolidBrush(Colors::RosyBrown);
		inline FSolidBrush RoyalBlue = FSolidBrush(Colors::RoyalBlue);
		inline FSolidBrush SaddleBrown = FSolidBrush(Colors::SaddleBrown);
		inline FSolidBrush Salmon = FSolidBrush(Colors::Salmon);
		inline FSolidBrush SandyBrown = FSolidBrush(Colors::SandyBrown);
		inline FSolidBrush SeaGreen = FSolidBrush(Colors::SeaGreen);
		inline FSolidBrush SeaShell = FSolidBrush(Colors::SeaShell);
		inline FSolidBrush Sienna = FSolidBrush(Colors::Sienna);
		inline FSolidBrush Silver = FSolidBrush(Colors::Silver);
		inline FSolidBrush SkyBlue = FSolidBrush(Colors::SkyBlue);
		inline FSolidBrush SlateBlue = FSolidBrush(Colors::SlateBlue);
		inline FSolidBrush SlateGray = FSolidBrush(Colors::SlateGray);
		inline FSolidBrush Snow = FSolidBrush(Colors::Snow);
		inline FSolidBrush SpringGreen = FSolidBrush(Colors::SpringGreen);
		inline FSolidBrush SteelBlue = FSolidBrush(Colors::SteelBlue);
		inline FSolidBrush Tan = FSolidBrush(Colors::Tan);
		inline FSolidBrush Teal = FSolidBrush(Colors::Teal);
		inline FSolidBrush Thistle = FSolidBrush(Colors::Thistle);
		inline FSolidBrush Tomato = FSolidBrush(Colors::Tomato);
		inline FSolidBrush Turquoise = FSolidBrush(Colors::Turquoise);
		inline FSolidBrush Violet = FSolidBrush(Colors::Violet);
		inline FSolidBrush Wheat = FSolidBrush(Colors::Wheat);
		inline FSolidBrush White = FSolidBrush(Colors::White);
		inline FSolidBrush WhiteSmoke = FSolidBrush(Colors::WhiteSmoke);
		inline FSolidBrush Yellow = FSolidBrush(Colors::Yellow);
		inline FSolidBrush YellowGreen = FSolidBrush(Colors::YellowGreen);
	}
}
