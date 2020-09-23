#include "PCH.h"

struct STest
{
	int c = 100;
	int Add(int a, int b) const
	{
		return a + b + c;
	}

	int Add2(int a, int b, int cc, int, int, int, int, int, int, int) const
	{
		return a + b + c + cc;
	}
};


int AddFun(int a, int b)
{
	return a * b;
}

using namespace XE;

void Test()
{
	TEvent<int, int> Calculate_Add;
	FEventHandle Handle1 = Calculate_Add.AddBind([](int a, int b)
	{
		return EReply::None;
	});
	FEventHandle Handle2 = Calculate_Add.AddBind([](int a, int b) { return EReply::Handled; });
	//TFunction<int(int, int)> Calculate_Add;
	Calculate_Add.Excute(3, 7);
	Calculate_Add.UnBind(Handle1);
	Calculate_Add.Excute(3, 7);
	
	auto stdff = std::function<int(int, int)>(AddFun);
	//auto xeff = TFunction<int(int, int)>(AddFun);
	auto xeff = TFunction<int(int, int)>(AddFun);
	
	STest Test;

	{
		auto Fun = TFunction<int(int, int)>(&STest::Add, &Test);
		auto Fun2 = TFunction<int(int, int)>([](int a, int b) { return a - b; });
		auto Fun3 = TFunction<int(int, int)>(AddFun);
		int Val1 = Fun(2, 33);
		int Val2 = Fun2(2, 33);
		int Val3 = Fun3(2, 33);

		auto Exp = Bind([](int a, int b, int c) {return a + b + c; }, Placer1, Placer2, Placer3);
		int Gval = 0;
		auto res22 = Exp(1, 2, 3);

		//auto BB = Bind([](int & val, int val2) { val = val2; }, Placer1, 123);
		//BB(Gval);
		//Gval = 2;

		auto BB = Bind([](int & val, int val2) { val = val2; }, Placer1, Placer2);
		BB(Gval, 999);
		Gval = 2;
	}
	{
		auto bbb = std::bind([](int,int){}, std::placeholders::_1, std::placeholders::_2);
		bbb(1, 13);

		{
			auto fun = [](FString && String)
			{
				FString Str2(String);
			};

			FString String = Str("fwgeg");
			fun(Move(String));
		}
		{
			int c = 13;
			FString String = Str("fwgeg");
			FString String2 = Str("1234");
			auto Binded = Bind([](FString && A, FString & B, int C)
				{
					LogInfo(Str(" C = {}"), C);
					B = Move(A);
				}, Placer1, Placer2, Placer3);

			Binded(Move(String), String2, c);

			int a = 12;
		}
	}
}
