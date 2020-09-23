#include "PCH.h"

using namespace XE;

static TTuple<int, float> GetValues()
{
	return { 12, 13.0f };
}

void TestTuple()
{
	auto [IntV, FLoatV] = GetValues();
	int iii = 0;
	float fff = 0;
	Tie(iii, Ignore) = GetValues();
	Assert(iii == 12);
}
