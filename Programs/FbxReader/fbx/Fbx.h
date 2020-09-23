#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <sstream>
#include <future>
#include <limits>
#include <assert.h>
#include <stdint.h>
#include <xmmintrin.h>

#ifdef __UNREAL__
#include <WindowsPlatform.h>
#else
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#undef X

#include "fbxsdk.h"

namespace fbx
{
	template<class _Ty,
		class _Pr>
		_NODISCARD constexpr const _Ty& clamp(const _Ty& _Val, const _Ty& _Min_val, const _Ty& _Max_val, _Pr _Pred)
	{	// returns _Val constrained to [_Min_val, _Max_val] ordered by _Pred
#if _ITERATOR_DEBUG_LEVEL == 2
		if (_DEBUG_LT_PRED(_Pred, _Max_val, _Min_val))
		{
			_STL_REPORT_ERROR("invalid bounds arguments passed to fbx::clamp");
			return (_Val);
		}
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */

		return (_DEBUG_LT_PRED(_Pred, _Max_val, _Val)
			? _Max_val
			: _DEBUG_LT_PRED(_Pred, _Val, _Min_val)
			? _Min_val
			: _Val);
	}

	template<class _Ty>
	_NODISCARD constexpr const _Ty& clamp(const _Ty& _Val, const _Ty& _Min_val, const _Ty& _Max_val)
	{	// returns _Val constrained to [_Min_val, _Max_val]
		return (fbx::clamp(_Val, _Min_val, _Max_val, std::less<>()));
	}
	
	enum error_e : int32_t
	{
		error_generic = std::numeric_limits<int32_t>::min(),
		error_inner,
		error_eof,
		error_nullptr,
		error_unreachable,
		error_outofmemory,
		error_args,
		error_exists,
		error_access,
		error_state,
		error_io,
		error_broken,

		error_timeout,
		error_cancel,

		error_bad_data,
		error_bad_format,

		error_not_now,
		error_not_supported,
		error_not_found,
		error_not_implemented,

		error_ok = 0,
		error_true,
		error_false,
		error_pendding,
		error_done,
	};
	inline void format_helper(std::ostringstream & stream) {}

	template<typename Head, typename ...Tail>
	void format_helper(std::ostringstream & stream, const Head & head, Tail && ...tail)
	{
		stream << head;
		return format_helper(stream, std::forward<Tail>(tail)...);
	}

	template<typename ...Args>
	std::string format(Args && ...args)
	{
		std::ostringstream stream;
		format_helper(stream, std::forward<Args>(args)...);
		return stream.str();
	}

	void log_inf(const char *);
	void log_war(const char *);
	
    const int FBX_MAX_TEXCOORDS = 4;
    const float FBX_SMALL_NUMBER = 1.e-8f;
    const float FBX_KINDA_SMALL_NUMBER = 1.e-4f;
    const float FBX_DELTA = 0.00001f;
    const int FBX_MAX_GPU_SKIN_BONES = 256;
    const int FBX_INDEX_NONE = -1;
    const int FBX_MAX_TOTAL_INFLUENCES = 8;
    const float FBX_THRESH_POINTS_ARE_SAME = 0.00002f;
    const float FBX_THRESH_NORMALS_ARE_SAME = 0.00002f;
    const float FBX_THRESH_UVS_ARE_SAME = 0.0009765625f;
    const int FBX_MAX_INFLUENCES_PER_STREAM = 4;
    const float FBX_THRESH_VECTORS_ARE_NEAR = 0.0004f;
    const float FBX_THRESH_POINTS_ARE_NEAR = 0.015f;
    const float FBX_DEFAULT_SAMPLERATE = 30.f;
    enum class EAxis
    {
        None = 0,
        X,
        Y,
        Z,
    };

    enum EFbxCreator
    {
        Blender,
        Unknow
    };

	inline float InvSqrt(float F)
	{
		// Performs two passes of Newton-Raphson iteration on the hardware estimate
		//    v^-0.5 = x
		// => x^2 = v^-1
		// => 1/(x^2) = v
		// => F(x) = x^-2 - v
		//    F'(x) = -2x^-3

		//    x1 = x0 - F(x0)/F'(x0)
		// => x1 = x0 + 0.5 * (x0^-2 - Vec) * x0^3
		// => x1 = x0 + 0.5 * (x0 - Vec * x0^3)
		// => x1 = x0 + x0 * (0.5 - 0.5 * Vec * x0^2)
		//
		// This final form has one more operation than the legacy factorization (X1 = 0.5*X0*(3-(Y*X0)*X0)
		// but retains better accuracy (namely InvSqrt(1) = 1 exactly).

		const __m128 fOneHalf = _mm_set_ss(0.5f);
		__m128 Y0, X0, X1, X2, FOver2;
		float temp;

		Y0 = _mm_set_ss(F);
		X0 = _mm_rsqrt_ss(Y0);	// 1/sqrt estimate (12 bits)
		FOver2 = _mm_mul_ss(Y0, fOneHalf);

		// 1st Newton-Raphson iteration
		X1 = _mm_mul_ss(X0, X0);
		X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
		X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

		// 2nd Newton-Raphson iteration
		X2 = _mm_mul_ss(X1, X1);
		X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
		X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

		_mm_store_ss(&temp, X2);
		return temp;
	}
	
    inline int TruncToInt(float F) { return (int)F; }
    inline int TruncToFloat(float F) { return (float)(int)F;}
    inline float Fractional(float Value) { return Value - TruncToFloat(Value); }

    inline int RoundToInt(float f) { return (int)(f + 0.5f); }
    inline bool IsNearlyZero(float Value, float ErrorTolerance) { return std::abs(Value) <= ErrorTolerance; }
    inline bool IsNearlyZero(float A, float B, float ErrorTolerance) { return std::abs(A - B) <= ErrorTolerance; }

    inline int GreatestCommonDivisor(int a, int b)
    {
        while (b != 0)
        {
            int t = b;
            b = a % b;
            a = t;
        }
        return a;
    }
    inline int LeastCommonMultiplier(int a, int b)
    {
        int CurrentGcd = GreatestCommonDivisor(a, b);
        return CurrentGcd == 0 ? 0 : (a / CurrentGcd) * b;
    }

    struct Float2
    {
        float X = 0;
        float Y = 0;

        bool operator==(const Float2 & another) const { return X == another.X && Y == another.Y; }
        bool operator!=(const Float2 & another) const { return !operator==(another); }
        inline const float & operator[](int index) const { return ((float *)this)[index]; }
        inline float & operator[](int index) { return ((float *)this)[index]; }
    };

    struct Int2
    {
        int X = 0;
        int Y = 0;

        bool operator==(const Int2 & another) const { return X == another.X && Y == another.Y; }
        bool operator!=(const Int2 & another) const { return !operator==(another); }
        inline const int & operator[](int index) const { return ((int *)this)[index]; }
        inline int & operator[](int index) { return ((int *)this)[index]; }
    };

    struct Float3
    {
        float X = 0;
        float Y = 0;
        float Z = 0;

        Float3() = default;
        //Float3(const FbxVector4 & fv) :X(fv[0]) , Y(fv[1]) , Z(fv[2]) {}
        Float3(float X_, float Y_, float Z_) :X(X_), Y(Y_), Z(Z_) {}

        bool operator==(const Float3 & another) const { return X == another.X && Y == another.Y && Z == another.Z; }
        bool operator!=(const Float3 & another) const { return !operator==(another); }
        inline const float & operator[](int index) const { return ((float *)this)[index]; }
        inline float & operator[](int index) { return ((float *)this)[index]; }
        Float3 operator-() const { return { -X, -Y, -Z }; }
        Float3 operator+(const Float3 & another) const { return { X + another.X, Y + another.Y, Z + another.Z }; }
        Float3 operator-(const Float3 & another) const { return { X - another.X, Y - another.Y, Z - another.Z }; }
        Float3 operator*(const Float3 & another) const { return { X * another.X, Y * another.Y, Z * another.Z }; }
        Float3 operator/(const Float3 & another) const { return { X / another.X, Y / another.Y, Z / another.Z }; }
        Float3 operator*(float value) const { return { X * value, Y * value, Z * value }; }
        Float3 operator/(float value) const { return { X / value, Y / value, Z / value }; }

        float operator|(const Float3 & V) const
        {
            return X * V.X + Y * V.Y + Z * V.Z;
        }

        Float3 operator^(const Float3 & V) const
        {
            return 
            {
                Y * V.Z - Z * V.Y,
                Z * V.X - X * V.Z,
                X * V.Y - Y * V.X
            };
        }

        Float3 & operator +=(const Float3 & V)
        {
            X += V.X;
            Y += V.Y;
            Z += V.Z;
            return *this;
        }

        Float3 & operator -=(const Float3 & V)
        {
            X -= V.X;
            Y -= V.Y;
            Z -= V.Z;
            return *this;
        }

        bool normalize(float tolerance = 1.e-8f)
        {
            const float SquareSum = X * X + Y * Y + Z * Z;
            if (SquareSum > tolerance)
            {
                const float Scale = InvSqrt(SquareSum);
                X *= Scale; Y *= Scale; Z *= Scale;
                return true;
            }
            return false;
        }

        Float3 GetSafeNormal(float Tolerance = FBX_SMALL_NUMBER) const
        {
            const float SquareSum = X * X + Y * Y + Z * Z;

            // Not sure if it's safe to add tolerance in there. Might introduce too many errors
            if (SquareSum == 1.f)
            {
                return *this;
            }
            else if (SquareSum < Tolerance)
            {
                return {0, 0, 0};
            }
            const float Scale = InvSqrt(SquareSum);
            return { X * Scale, Y * Scale, Z * Scale };
        }

        bool IsNearlyZero(float Tolerance = FBX_KINDA_SMALL_NUMBER) const
        {
            return std::abs(X) <= Tolerance && std::abs(Y) <= Tolerance && std::abs(Z) <= Tolerance;
        }
        float SizeSquared() const
        {
            return X * X + Y * Y + Z * Z;
        }

        bool Normalize(float Tolerance = FBX_SMALL_NUMBER)
        {
            const float SquareSum = X * X + Y * Y + Z * Z;
            if (SquareSum > Tolerance)
            {
                const float Scale = InvSqrt(SquareSum);
                X *= Scale; Y *= Scale; Z *= Scale;
                return true;
            }
            return false;
        }

        bool IsZero() const
        {
            return X == 0.f && Y == 0.f && Z == 0.f;
        }

        bool ContainsNaN() const
        {
            return !_finite(X) || !_finite(Y) || !_finite(Z);
        }

        static void CreateOrthonormalBasis(Float3 & XAxis, Float3 & YAxis, Float3 & ZAxis)
        {
            // Project the X and Y axes onto the plane perpendicular to the Z axis.
            XAxis -= ZAxis * ((XAxis | ZAxis) / (ZAxis | ZAxis));
            YAxis -= ZAxis * ((YAxis | ZAxis) / (ZAxis | ZAxis));

            // If the X axis was parallel to the Z axis, choose a vector which is orthogonal to the Y and Z axes.
            if (XAxis.SizeSquared() < FBX_DELTA * FBX_DELTA)
            {
                XAxis = YAxis ^ ZAxis;
            }

            // If the Y axis was parallel to the Z axis, choose a vector which is orthogonal to the X and Z axes.
            if (YAxis.SizeSquared() < FBX_DELTA * FBX_DELTA)
            {
                YAxis = XAxis ^ ZAxis;
            }

            // Normalize the basis vectors.
            XAxis.Normalize();
            YAxis.Normalize();
            ZAxis.Normalize();
        }
    };
    inline Float3 operator *(float value, const Float3 & another) { return another * value; }
    const Float3 Float3_Zero = Float3(0.0f, 0.f, 0.0f);

    struct Int3
    {
        int X = 0;
        int Y = 0;
        int Z = 0;

        bool operator==(const Int3 & another) const { return X == another.X && Y == another.Y && Z == another.Z; }
        bool operator!=(const Int3 & another) const { return !operator==(another); }
        inline const int & operator[](int index) const { return ((int *)this)[index]; }
        inline int & operator[](int index) { return ((int *)this)[index]; }
    };

    struct Float4
    {
        float X = 0;
        float Y = 0;
        float Z = 0;
        float W = 0;

        Float4() = default;
        Float4(float X_, float Y_, float Z_, float W_) :X(X_), Y(Y_), Z(Z_), W(W_) {}

        bool operator==(const Float4 & another) const { return X == another.X && Y == another.Y && Z == another.Z && W == another.W; }
        bool operator!=(const Float4 & another) const { return !operator==(another); }
        inline const float & operator[](int index) const { return ((float *)this)[index]; }
        inline float & operator[](int index) { return ((float *)this)[index]; }

        Float4 operator+(const Float4 & another) const { return { X + another.X, Y + another.Y, Z + another.Z, W + another.W }; }
        Float4 operator-(const Float4 & another) const { return { X - another.X, Y - another.Y, Z - another.Z, W - another.W }; }
    };

    const Float4 Float4_Zero = { 0, 0, 0, 0 };

    struct Plane : Float4
    {
        Plane() = default;

        Plane(float X_, float Y_, float Z_, float W_) :Float4(X_, Y_, Z_, W_)
        {
            
        }

        Plane(const Float3 & A, const Float3 & B, const Float3 & C)
        {
            Float3 XYZ = ((B - A) ^ (C - A)).GetSafeNormal();
            X = XYZ.X;
            Y = XYZ.Y;
            Z = XYZ.Z;
            W = A | XYZ;
        }
    };

    struct Quat
    {
        float X = 0;
        float Y = 0;
        float Z = 0;
        float W = 0;

        bool operator==(const Quat & another) const { return X == another.X && Y == another.Y && Z == another.Z && W == another.W; }
        bool operator!=(const Quat & another) const { return !operator==(another); }
        inline const float & operator[](int index) const { return ((float *)this)[index]; }
        inline float & operator[](int index) { return ((float *)this)[index]; }
        Quat operator+(const Quat & another) const { return { X + another.X, Y + another.Y, Z + another.Z, W + another.W }; }
        Quat operator-(const Quat & another) const { return { X - another.X, Y - another.Y, Z - another.Z, W - another.W }; }
        Quat operator*(const Quat & another) const { return { X * another.X, Y * another.Y, Z * another.Z, W * another.W }; }
        Quat operator/(const Quat & another) const { return { X / another.X, Y / another.Y, Z / another.Z, W / another.W }; }
    };

    struct Int4
    {
        int X = 0;
        int Y = 0;
        int Z = 0;
        int W = 0;

        bool operator==(const Int4 & another) const { return X == another.X && Y == another.Y && Z == another.Z && W == another.W; }
        bool operator!=(const Int4 & another) const { return !operator==(another); }
        inline const int & operator[](int index) const { return ((int *)this)[index]; }
        inline int & operator[](int index) { return ((int *)this)[index]; }
    };

    struct Color32
    {
        uint8_t A = 0;
        uint8_t R = 0;
        uint8_t G = 0;
        uint8_t B = 0;

        bool operator==(const Color32 & another) const { return A == another.A && R == another.R && G == another.G && B == another.B; }
        bool operator!=(const Color32 & another) const { return !operator==(another); }
        inline const uint8_t & operator[](int index) const { return ((uint8_t *)this)[index]; }
        inline uint8_t & operator[](int index) { return ((uint8_t *)this)[index]; }
    };

    struct Matrix
    {
        Float4 row0;
        Float4 row1;
        Float4 row2;
        Float4 row3;

        Matrix() = default;
        Matrix(const Float4 & r0, const Float4 & r1, const Float4 & r2, const Float4 & r3) : row0(r0), row1(r1), row2(r2), row3(r3) {}
        explicit Matrix(const FbxMatrix & fm)
        {
            row0.X = fm[0][0]; row0.Y = fm[0][1]; row0.Z = fm[0][2]; row0.W = fm[0][3];
            row1.X = fm[1][0]; row1.Y = fm[1][1]; row1.Z = fm[1][2]; row1.W = fm[1][3];
            row2.X = fm[2][0]; row2.Y = fm[2][1]; row2.Z = fm[2][2]; row2.W = fm[2][3];
            row3.X = fm[3][0]; row3.Y = fm[3][1]; row3.Z = fm[3][2]; row3.W = fm[3][3];
        }

        FbxMatrix fbx() const
        {
            return FbxMatrix
            (
                row0.X, row0.Y, row0.Z, row0.W,
                row1.X, row1.Y, row1.Z, row1.W,
                row2.X, row2.Y, row2.Z, row2.W,
                row3.X, row3.Y, row3.Z, row3.W
            );
        }

        inline const Float4 & operator[](int index) const { return ((Float4 *)this)[index]; }
        inline Float4 & operator[](int index) { return ((Float4 *)this)[index]; }

        Float4 * M() const { return (Float4 *)this; }

        static const Matrix Identity;

        Matrix Inverse() const
        {
            FbxMatrix fm2 = fbx().Inverse();
            return Matrix(fm2);
        }

        Float3 TransformVector(const Float3 & v) const
        {
            FbxVector4 v4 = fbx().MultNormalize(FbxVector4(v.X, v.Y, v.Z, 1));
            return Float3(v4[0], v4[1], v4[2]);
        }

        Matrix operator * (const Matrix & another) const
        {
            return Matrix(fbx() * another.fbx());
        }
    };

    struct Transform
    {
        Float3 Translation;
        Quat Rotation;
        Float3 Scale;
    };

    struct JointPos
    {
        FbxAMatrix Transform;

        // For collision testing / debug drawing...
        float Length;
        float XSize;
        float YSize;
        float ZSize;
    };

    struct Bone
    {
        std::string Name;
        //@ todo FBX - Flags unused?
        uint32_t Flags; // reserved / 0x02 = bone where skin is to be attached...
        int32_t NumChildren; // children // only needed in animation ?
        int32_t ParentIndex; // 0/NULL if this is the root bone.
        JointPos BonePos; // reference position
    };

    struct Wedge
    {
        uint32_t PointIndex = 0; // Index to a vertex.
        Float2 UVs[FBX_MAX_TEXCOORDS]; // Scaled to BYTES, rather...-> Done in digestion phase, on-disk size doesn't matter here.
        Color32 Color; // Wedge colors
        uint8_t MatIndex = 0; // At runtime, this one will be implied by the face that's pointing to us.
        uint8_t Reserved = 0; // Top secret.
        bool operator==(const Wedge & Other) const
        {
            bool Equal = true;

            Equal &= (PointIndex == Other.PointIndex);
            Equal &= (MatIndex == Other.MatIndex);
            Equal &= (Color == Other.Color);
            Equal &= (Reserved == Other.Reserved);

            bool bUVsEqual = true;
            for (uint32_t UVIdx = 0; UVIdx < FBX_MAX_TEXCOORDS; ++UVIdx)
            {
                if (UVs[UVIdx] != Other.UVs[UVIdx])
                {
                    bUVsEqual = false;
                    break;
                }
            }

            Equal &= bUVsEqual;

            return Equal;
        }
    };

    struct Triangle
    {
        // Point to three vertices in the vertex list.
        uint32_t WedgeIndex[3];
        // Materials can be anything.
        uint8_t MatIndex;
        // Second material from exporter (unused)
        uint8_t AuxMatIndex;
        // 32-bit flag for smoothing groups.
        uint32_t SmoothingGroups;

        Float3 TangentX[3];
        Float3 TangentY[3];
        Float3 TangentZ[3];


        Triangle & operator=(const Triangle & Other)
        {
            this->AuxMatIndex = Other.AuxMatIndex;
            this->MatIndex = Other.MatIndex;
            this->SmoothingGroups = Other.SmoothingGroups;
            this->WedgeIndex[0] = Other.WedgeIndex[0];
            this->WedgeIndex[1] = Other.WedgeIndex[1];
            this->WedgeIndex[2] = Other.WedgeIndex[2];
            this->TangentX[0] = Other.TangentX[0];
            this->TangentX[1] = Other.TangentX[1];
            this->TangentX[2] = Other.TangentX[2];

            this->TangentY[0] = Other.TangentY[0];
            this->TangentY[1] = Other.TangentY[1];
            this->TangentY[2] = Other.TangentY[2];

            this->TangentZ[0] = Other.TangentZ[0];
            this->TangentZ[1] = Other.TangentZ[1];
            this->TangentZ[2] = Other.TangentZ[2];

            return *this;
        }
    };

    struct BoneInfluence // just weight, vertex, and Bone, sorted later....
    {
        float Weight = 0.0f;
        int32_t PointIndex;
        uint16_t BoneIndex;
    };

    struct BoundingBox
    {
        /** Holds the box's minimum point. */
        Float3 Min;

        /** Holds the box's maximum point. */
        Float3 Max;

        bool valid = false;

        BoundingBox & operator+=(const Float3 & point)
        {
            if (!valid)
            {
                valid = true;
                Min = Max = point;
            }
            else
            {
                Min.X = std::min(Min.X, point.X);
                Min.Y = std::min(Min.Y, point.Y);
                Min.Z = std::min(Min.Z, point.Z);

                Max.X = std::max(Max.X, point.X);
                Max.Y = std::max(Max.Y, point.Y);
                Max.Z = std::max(Max.Z, point.Z);
            }
            return *this;
        }

        Float3 size() const { return Max - Min; }
    };

    struct Material
    {
        std::string name;
    };

    template<typename T>
    void fbx_object_deleter(T * ptr)
    {
        if (ptr)
            ptr->Destroy();
    }

    struct Vertex
    {
        Float3 Position;
        Float3 TangentX; // Tangent, U-direction
        Float3 TangentY; // Binormal, V-direction
        Float3 TangentZ; // Normal

        //Int4 TangentX; // Tangent, U-direction
        //Int4 TangentY; // Binormal, V-direction
        //Int4 TangentZ; // Normal
        Float2 UVs[FBX_MAX_TEXCOORDS]; // UVs
        Color32 Color; // VertexColor

    	//  这里可能超过 255，不能用 uint8_t
        uint16_t InfluenceBones[FBX_MAX_TOTAL_INFLUENCES];
        uint8_t InfluenceWeights[FBX_MAX_TOTAL_INFLUENCES];
        uint32_t PointWedgeIdx = 0;
    };

	
    struct Section
    {
        int32_t MaterialIndex = 0;
        int32_t OriginalSectionIndex = 0;

        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
        std::vector<uint16_t> BoneMap;
    };

    template<typename T>
    inline size_t Contains(std::vector<T> & v, const T & t)
    {
        auto iter = std::find(v.begin(), v.end(), t);
        return iter != v.end();
    }

    template<typename T>
    inline bool AddUnique(std::vector<T> & v, const T & t)
    {
        auto iter = std::find(v.begin(), v.end(), t);
        if (iter == v.end())
        {
            v.push_back(t);
            return true;
        }
        return false;
    }

    template<typename KeyT, typename ValueT>
    inline bool AddUnique(std::map<KeyT, ValueT> & map, const KeyT & key, const ValueT & value)
    {
        auto range = map.equal_range(key);
        if (std::find_if(range.first, range.second, [&](auto iter) { return iter.second == value; }) == range.second)
        {
            map.insert({ key, value });
            return true;
        }
        return false;
    }

    template<typename KeyT, typename ValueT>
    inline bool AddUnique(std::multimap<KeyT, ValueT> & map, const KeyT & key, const ValueT & value)
    {
        auto range = map.equal_range(key);
        if (std::find_if(range.first, range.second, [&](auto iter) { return iter.second == value; }) == range.second)
        {
            map.insert({ key, value });
            return true;
        }
        return false;
    }

    struct IndexAndZ
    {
        int32_t Index;
        float Z;

        bool operator<(IndexAndZ const & B) const { return Z < B.Z; }
    };

    struct BoneKeys
    {
        std::string boneName;
        std::vector<FbxAMatrix> keys;
    };

    struct Anim
    {
        std::string name;
        FbxTime duration;
        std::vector<BoneKeys> boneKeys;
    };
}
