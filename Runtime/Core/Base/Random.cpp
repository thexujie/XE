#include "PCH.h"
#include "Random.h"

namespace XE
{
	static struct FRandomInitializer
	{
		FRandomInitializer()
		{
			
		}
	}RandomInitializer;
	
	int32_t FRandom::Rand()
	{
		std::default_random_engine generator((uint32_t)std::chrono::steady_clock::now().time_since_epoch().count());
		return generator();
	}
}
