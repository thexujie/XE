#pragma once

#include "Types.h"

namespace XE
{
    template<typename T>
    class TVec3
    {
    public:
        TVec3() = default;
        TVec3(const TVec3 &) = default;
        TVec3 & operator=(const TVec3 &) = default;
        ~TVec3() = default;
        auto operator <=>(const TVec3 & Another) const { return Compare(X, Another.X, Y, Another.Y, Z, Another.Z); }
    	
        constexpr TVec3(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ) { }

        TVec3 operator +(const TVec3 & Another) const
        {
            return TVec3(X + Another.X, Y + Another.Y, Z + Another.Z);
        }

        TVec3 operator -(const TVec3 & Another) const
        {
            return TVec3(X - Another.X, Y - Another.Y, Z - Another.Z);
        }

        TVec3 operator *(const TVec3 & Another) const
        {
            return TVec3(X * Another.X, Y * Another.Y, Z * Another.Z);
        }

        TVec3 operator /(const TVec3 & Another) const
        {
            return TVec3(X / Another.X, Y / Another.Y, Z / Another.Z);
        }

        TVec3 & operator +=(const TVec3 & Another)
        {
            X += Another.X;
            Y += Another.Y;
            Z += Another.Z;
            return *this;
        }

        TVec3 & operator -=(const TVec3 & Another)
        {
            X -= Another.X;
            Y -= Another.Y;
            Z -= Another.Z;
            return *this;
        }

        TVec3 & operator *=(const TVec3 & Another)
        {
            X *= Another.X;
            Y *= Another.Y;
            Z *= Another.Z;
            return *this;
        }

        TVec3 & operator /=(const TVec3 & Another)
        {
            X /= Another.X;
            Y /= Another.Y;
            Z /= Another.Z;
            return *this;
        }

        TVec3 operator *(const T & Rate) const
        {
            return TVec3(X * Rate, Y * Rate, Z * Rate);
        }

        TVec3 operator /(const T & Rate)
        {
            return TVec3(X / Rate, Y / Rate, Z / Rate);
        }

        TVec3 & operator *=(const T & Rate)
        {
            X *= Rate;
            Y *= Rate;
            Z *= Rate;
            return *this;
        }

        TVec3 & operator /=(const T & Rate)
        {
            X /= Rate;
            Y /= Rate;
            Z /= Rate;
            return *this;
        }

        TVec3 operator -() const requires IsSignedV<T>
        {
            return TVec3(-X, -Y, -Z);
        }
    	
        TVec3 operator +() const
        {
            return *this;
        }

        bool operator ==(const TVec3 & Another) const
        {
            return X == Another.X && Y == Another.Y && Z == Another.Z;
        }

        bool operator !=(const TVec3 & Another) const
        {
            return X != Another.X || Y != Another.Y || Z != Another.Z;
        }

        bool operator <(const TVec3 & Another) const
        {
            return X < Another.X && Y < Another.Y && Z < Another.Z;
        }

        bool operator >(const TVec3 & Another) const
        {
            return X > Another.X && Y > Another.Y && Z > Another.Z;
        }

        bool operator <=(const TVec3 & Another) const
        {
            return X <= Another.X && Y <= Another.Y && Z <= Another.Z;
        }

        bool operator >=(const TVec3 & Another) const
        {
            return X >= Another.X && Y >= Another.Y && Z >= Another.Z;
        }

        friend FArchive & operator >>(FArchive & Archive, TVec3 & Vec3)
        {
            Archive >> Vec3.X;
            Archive >> Vec3.Y;
            Archive >> Vec3.Z;
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
    		};

            struct
            {
                T R;
                T G;
                T B;
            };
    	};
    };

    template CORE_API class TVec3<int32_t>;
    template CORE_API class TVec3<uint32_t>;
    template CORE_API class TVec3<float32_t>;
	
    using int3 = TVec3<int32_t>;
    using uint3 = TVec3<uint32_t>;
    using Int3 = TVec3<int32_t>;
    using Uint3 = TVec3<uint32_t>;

    using Vec3I = TVec3<int32_t>;
    using Vec3U = TVec3<uint32_t>;
    using Vec3F = TVec3<float32_t>;
}
