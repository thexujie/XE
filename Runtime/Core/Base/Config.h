#pragma once

#pragma warning(disable: 4251)

#define __XE__ 1

#ifdef _DEBUG
#define XE_DEBUG 1
#define XE_TEST 1
#endif

#if defined(_WIN32) || defined(_WINDLL)
#define XE_WINDOWS 1
#else
#define XE_WINDOWS 0
#endif

#ifdef _M_X64
#define XE_X86 0
#define XE_X64 1
#define XE_BITS 64
#else
#define XE_X86 1
#define XE_X64 0
#define XE_BITS 32
#endif

#define STDCALL		__stdcall
#define INLINE inline
#define FORCEINLINE __forceinline
#define FORCENOINLINE __declspec(noinline)

#if XE_WINDOWS
#ifdef XEngine_Core_Module
#define CORE_API __declspec(dllexport)
#define CORE_VAR __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#define CORE_VAR
#endif
#endif

#define ENUM_STRING_BEGIN(Value) switch(Value) {
#define ENUM_STRING_CASE(Enum, Name) case Enum::Name: return Str( #Name );
#define ENUM_STRING_END default: return {}; }

#pragma warning(disable: 4201)
#pragma warning(disable: 4100)

/**
 * ²»ÐèÒª define min max
 */
#define NOMINMAX 1

#include <locale.h>
#include <tchar.h>
#include <memory.h>
#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <intrin.h>

#include <cstring>
#include <cmath>

#include <locale>
#include <sstream>
#include <iomanip>
#include <type_traits>
#include <limits>
#include <exception>
#include <array>
#include <future>
#include <fstream>
#include <compare>

#include <numeric>
#include <vector>
#include <map>

#include <random>

namespace XE
{
    template<typename T>
    using TInitializerList = std::initializer_list<T>;
}
