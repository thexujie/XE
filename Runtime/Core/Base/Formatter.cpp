#include "PCH.h"
#include "Atomic.h"
#include "Formatter.h"

namespace XE
{
	static const size_t StringBufferCount = 32;
	enum EFormatBuffer
	{
		EFormatBuffer_Ansi = 0,
		EFormatBuffer_Utf8,
		EFormatBuffer_Wide,
		EFormatBuffer_Count,
	};

	template<typename CharT>
	static int32_t GFormatBufferIndex;
	
	template<typename CharT>
	static fmt::basic_memory_buffer<CharT> GFormatBuffers[StringBufferCount];
	
	template<typename CharT>
	fmt::basic_memory_buffer<CharT> & GetThreadLocalFormatBuffer()
	{
		int32_t Index = Atomics::IncFetch(GFormatBufferIndex<CharT>) - 1;
		auto & ThreadLocalFormatBuffer = GFormatBuffers<CharT>[Index % StringBufferCount];
		ThreadLocalFormatBuffer.clear();
		return ThreadLocalFormatBuffer;
	}

	template fmt::basic_memory_buffer<char> & GetThreadLocalFormatBuffer<char>();
	template fmt::basic_memory_buffer<char8_t> & GetThreadLocalFormatBuffer<char8_t>();
	template fmt::basic_memory_buffer<wchar_t> & GetThreadLocalFormatBuffer<wchar_t>();
}
