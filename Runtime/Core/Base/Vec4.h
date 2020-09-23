#pragma once

#include "Types.h"
#include "Vec2.h"

namespace XE
{
    template<typename T>
    class TVec4
    {
    public:
        TVec4() = default;
        constexpr TVec4(const TVec4 &) = default;
        TVec4 & operator=(const TVec4 &) = default;
        ~TVec4() = default;
        auto operator <=>(const TVec4 & Another) const noexcept { return Compare(X, Another.X, Y, Another.Y, Z, Another.Z, W, Another.W); }
    	
        constexpr TVec4(T InX, T InY, T InZ, T InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
        constexpr TVec4(const TVec2<T> & PositionIn, const TVec2<T> & SizeIn) : X(PositionIn.X), Y(PositionIn.Y), Z(SizeIn.X), W(SizeIn.Y) { }
 
        TVec2<T> LeftTop() const { return TVec2<T>(X, Y); }
        TVec2<T> RightTop() const { return TVec2<T>(X + Width, Y); }
        TVec2<T> LeftBottom() const { return TVec2<T>(X, Y + Height); }
        TVec2<T> RightBottom() const { return TVec2<T>(X + Width, Y + Height); }
        TVec2<T> LeftCenter() const { return TVec2<T>(X, Y + Height / 2); }
        TVec2<T> TopCenter() const { return TVec2<T>(X + Width / 2, Y); }
        TVec2<T> RightCenter() const { return TVec2<T>(X + Width, Y + Height / 2); }
        TVec2<T> BottomCenter() const { return TVec2<T>(X + Width / 2, Y + Height); }
        TVec2<T> Center() const { return TVec2<T>(X + Width / 2, Y + Height / 2); }
    	
        T ThicknessWidth() const { return Left + Right; }
        T ThicknessHeight() const { return Top + Bottom; }
        TVec2<T> ThicknessSize() const { return TVec2<T>(Left + Right, Top + Bottom); }
    	
        bool Contains(const TVec2<T> & Point) const
        {
            return Point.X >= X && Point.Y >= Y && Point.X < X + Width && Point.Y < Y + Height;
        }

        bool IsZero() const
        {
            if constexpr (!IsArithmeticV<T>)
                return false;
            else if constexpr (IsFloatingPointV<T>)
                return Abs(X) <= FloatEpslion<T> && Abs(Y) <= FloatEpslion<T> &&
                Abs(Width) <= FloatEpslion<T> && Abs(Height) <= FloatEpslion<T>;
            else
                return X == 0 && Y == 0 && Width == 0 && Height == 0;
        }

        bool IsEmpty() const
        {
            if constexpr (!IsArithmeticV<T>)
                return false;
            else if constexpr (IsFloatingPointV<T>)
                return Abs(Width) <= FloatEpslion<T> || Abs(Height) <= FloatEpslion<T>;
            else
                return Width == 0 || Height == 0;
        }

        bool HasNan() const
        {
            if constexpr (!IsArithmeticV<T> || !IsFloatingPointV<T>)
                return false;
            else
				return IsNan(X) || IsNan(Y) || IsNan(Width) || IsNan(Height);
        }

        bool IsAllNan() const
        {
            if constexpr (!IsArithmeticV<T> || !IsFloatingPointV<T>)
                return false;
            else
				return IsNan(X) && IsNan(Y) && IsNan(Width) && IsNan(Height);
        }

        void Clear()
        {
            X = 0;
            Y = 0;
            Width = 0;
            Height = 0;
        }

        template<typename = decltype(DeclVal<T>() - DeclVal<T>())>
        TVec4 operator +(const TVec4 & Another) const
        {
            return TVec4(X + Another.X, Y + Another.Y, Z + Another.Z, W + Another.W);
        }

        template<typename = decltype(DeclVal<T>() - DeclVal<T>())>
        TVec4 operator -(const TVec4 & Another) const
        {
            return TVec4(X - Another.X, Y - Another.Y, Z - Another.Z, W - Another.W);
        }

        template<typename = decltype(DeclVal<T>() * DeclVal<T>())>
        TVec4 operator *(const TVec4 & Another) const
        {
            return TVec4(X * Another.X, Y * Another.Y, Z * Another.Z, W * Another.W);
        }

        template<typename = decltype(DeclVal<T>() / DeclVal<T>())>
        TVec4 operator /(const TVec4 & Another) const
        {
            return TVec4(X / Another.X, Y / Another.Y, Z / Another.Z, W / Another.W);
        }

        template<typename = EnableIfT<!IsSameV<T, float32_t>>>
        TVec4 operator * (float32_t Rate) const
        {
            return TVec4(static_cast<T>(X * Rate), static_cast<T>(Y * Rate), static_cast<T>(Width * Rate), static_cast<T>(Height * Rate));
        }

        template<typename = EnableIfT<!IsSameV<T, float32_t>>>
        TVec4 operator / (float32_t Rate) const
        {
            return TVec4(static_cast<T>(X / Rate), static_cast<T>(Y / Rate), static_cast<T>(Width / Rate), static_cast<T>(Height / Rate));
        }

        TVec4 & operator +=(const TVec4 & Another)
        {
            X += Another.X;
            Y += Another.Y;
            Z += Another.Z;
            W += Another.W;
            return *this;
        }

        TVec4 & operator -=(const TVec4 & Another)
        {
            X -= Another.X;
            Y -= Another.Y;
            Z -= Another.Z;
            W -= Another.W;
            return *this;
        }

        TVec4 & operator *=(const TVec4 & Another)
        {
            X *= Another.X;
            Y *= Another.Y;
            Z *= Another.Z;
            W *= Another.W;
            return *this;
        }

        TVec4 & operator /=(const TVec4 & Another)
        {
            X /= Another.X;
            Y /= Another.Y;
            Z /= Another.Z;
            W /= Another.W;
            return *this;
        }

        TVec4 operator *(const T & Rate) const
        {
            return TVec4(X * Rate, Y * Rate, Z * Rate, W * Rate);
        }

        TVec4 operator /(const T & Rate)
        {
            return TVec4(X / Rate, Y / Rate, Z / Rate, W / Rate);
        }

        TVec4 & operator *=(const T & Rate)
        {
            X *= Rate;
            Y *= Rate;
            Z *= Rate;
            W *= Rate;
            return *this;
        }

        TVec4 & operator /=(const T & Rate)
        {
            X /= Rate;
            Y /= Rate;
            Z /= Rate;
            W /= Rate;
            return *this;
        }

        TVec4 operator -() const requires IsSignedV<T>
        {
            return TVec4(-X, -Y, -Z, -W);
        }

        TVec4 operator +() const
        {
            return *this;
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() + DeclVal<T>())>>>
        TVec4 operator +(const TVec2<T> & Another) const
        {
            return TVec4(X + Another.X, Y + Another.Y, Z + Another.Z, W + Another.W);
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() += DeclVal<T>())>>>
        TVec4 & operator +=(const TVec2<T> & Another)
        {
            X += Another.X;
            Y += Another.Y;
            Z += Another.Z;
            W += Another.W;
            return *this;
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() - DeclVal<T>())>>>
        TVec4 operator -(const TVec2<T> & Another) const
        {
            return TVec4(X - Another.X, Y - Another.Y, Z - Another.Z, W - Another.W);
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() -= DeclVal<T>())>>>
        TVec4 & operator -=(const TVec2<T> & Another)
        {
            X -= Another.X;
            Y -= Another.Y;
            Z -= Another.Z;
            W -= Another.W;
            return *this;
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() & DeclVal<T>())>>>
        TVec4 operator &(const TVec4 & Another) const
        {
            return TVec4(X & Another.X, Y & Another.Y, Z & Another.Z, W & Another.W);
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() &= DeclVal<T>())>>>
        TVec4 & operator &=(const TVec4 & Another)
        {
            X &= Another.X;
            Y &= Another.Y;
            Z &= Another.Z;
            W &= Another.W;
            return *this;
        }

        template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() | DeclVal<T>())>>>
        TVec4 operator |(const TVec4 & Another) const
        {
            return TVec4(X | Another.X, Y | Another.Y, Z | Another.Z, W | Another.W);
        }

    	template<typename = EnableIfT<IsSameV<T, decltype(DeclVal<T>() |= DeclVal<T>())>>>
        TVec4 & operator |=(const TVec4 & Another)
        {
            X |= Another.X;
            Y |= Another.Y;
            Z |= Another.Z;
            W |= Another.W;
            return *this;
        }

        bool operator ==(const TVec4 & Another) const
        {
            if constexpr (IsFloatingPointV<T>)
                return Abs(X - Another.X) < FloatEpslion<T> &&
                Abs(Y - Another.Y) < FloatEpslion<T> &&
                Abs(Width - Another.Width) < FloatEpslion<T> &&
                Abs(Height - Another.Height) < FloatEpslion<T>;
            else
                return X == Another.X && Y == Another.Y && Width == Another.Width && Height == Another.Height;
        }
        bool operator !=(const TVec4 & Another) const { return !operator==(Another); }

        const T & operator[](int32_t Index) const { return Values[Index % 4]; }
        T & operator[](int32_t Index) { return Values[Index % 4]; }

        template<typename T2>
        TVec4<T2> To() const
        {
            return { static_cast<T2>(X), static_cast<T2>(Y), static_cast<T2>(Width), static_cast<T2>(Height) };
        }

        template<class ConvertT>
        auto Convert(ConvertT convert) const
        {
            return TVec4(convert(X), convert(Y), convert(Width), convert(Height));
        }

        friend FArchive & operator >>(FArchive & Archive, TVec4 & Vec4)
        {
            Archive >> Vec4.X;
            Archive >> Vec4.Y;
            Archive >> Vec4.Z;
            Archive >> Vec4.W;
            return Archive;
        }
    public:
        union
        {
            struct
            {
                T X;
                T Y;
                T Z;
                T W;
            };
        	
            struct
            {
                T __X;
                T __Y;
                T Width;
                T Height;
            };

            struct
            {
                T Left;
                T Top;
                T Right;
                T Bottom;
            };
        	
            struct
            {
                T R;
                T G;
                T B;
                T A;
            };

            struct
            {
                TVec2<T> XY;
                TVec2<T> ZW;
            };

            struct
            {
                TVec2<T> XYZ;
                TVec2<T> __W;
            };
            
            struct
            {
                TVec2<T> Position;
                TVec2<T> Size;
            };

            T Values[4];
        };
    };

    template CORE_API class TVec4<int32_t>;
    template CORE_API class TVec4<uint32_t>;
    template CORE_API class TVec4<float32_t>;
	
    using Int4 = TVec4<int32_t>;
    using UInt4 = TVec4<uint32_t>;

    using RectI = TVec4<int32_t>;
    using RectU = TVec4<uint32_t>;
    using RectF = TVec4<float32_t>;

    using Vec4I = TVec4<int32_t>;
    using Vec4U = TVec4<uint32_t>;
    using Vec4F = TVec4<float32_t>;
	
    using ThicknessF = TVec4<float32_t>;
}
