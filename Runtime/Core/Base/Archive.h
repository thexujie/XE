#pragma once

#include "Types.h"
#include "Flag.h"
#include "Stream.h"

namespace XE
{
	enum class EArchiveFlags
	{
		None = 0,
		Input= 0x0001,
		Output = 0x0002,
	};
	template struct TFlag<EArchiveFlags>;
	
	class CORE_API FArchive : public FStream
	{
	public:
		FArchive(FDataStream & DataStreamIn, EArchiveFlags ArchiveFlagsIn = EArchiveFlags::None) : DataStream(DataStreamIn), ArchiveFlags(ArchiveFlagsIn) {}
		~FArchive() = default;

		uintx_t Read(void * Data, uintx_t Size) override { return DataStream.Read(Data, Size); }
		uintx_t Write(const void * Data, uintx_t Size) override { return DataStream.Write(Data, Size); }
		
		template<typename T>
		void Write(const T & Value)
		{
			if (!IsInput())
				DataStream << Value;
		}

		template<typename T>
		T Read()
		{
			if (IsInput())
			{
				T Value;
				Read(&Value, sizeof(T));
				return Value;
			}
			else
				throw EError::InvalidOperation;
		}
		
		template<typename T>
		void operator >>(T & Value)
		{
			if (IsInput())
				DataStream >> Value;
			else
				DataStream << Value;
		}

		template<typename T>
		void operator >>(const T & Value)
		{
			if (!IsInput())
				DataStream << Value;
		}


		template<typename T>
		void operator >>(TFlag<T> & Value)
		{
			if (IsInput())
				DataStream >> Value.Get();
			else
				DataStream << Value.Get();
		}

		template<typename T>
		void operator >>(const TFlag<T> & Value)
		{
			if (!IsInput())
				DataStream << Value.Get();
		}

		bool IsInput() const { return GetFlag(ArchiveFlags, EArchiveFlags::Input); }
		
	protected:
		FDataStream & DataStream;
		EArchiveFlags ArchiveFlags = EArchiveFlags::None;
	};
}
