#pragma once

#include "../Base/Base.h"

namespace XE
{
	enum class ELockFlags
	{
		None = 0,
		Read = 0x0001,
		Write = 0x0002,
		ReadWrite = Read | Write,
	};
	template struct TFlag<ELockFlags>;
}
