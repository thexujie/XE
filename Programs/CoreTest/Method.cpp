#include "PCH.h"
#include <iostream>

namespace XE
{
#ifdef XE_TEST
	class FTestMethod : public FObject
	{
	public:
		int Add(int a, int b) const
		{
			return a + b + c;
		}

		int Sub(int a, int b)
		{
			return a - b + c;
		}

		int Callback(FEventArgs & RoutedEventArgs)
		{
			RoutedEventArgs.Handled = true;
			return c;
		}
		
		int c = 100;

	};
	
	void Test_Method()
	{
		auto & ti = typeid(&FTestMethod::Add);
		FTestMethod TestMethod;
		FMethod AddMethod(Str("Add"), &FTestMethod::Add);
		int Result = AddMethod.Invoke(&TestMethod, { 12, 13 }).Get<int>();

		FMethod SubMethodAccessor(Str("Sub"), &FTestMethod::Sub);
		Result = SubMethodAccessor.Invoke(&TestMethod, { 12, 13 }).Get<int>();
		std::cout << Result;
	}

	class FTestMethod_Test
	{
	public:
		FTestMethod_Test()
		{
			Test_Method();
		}
	}TestMethod_Test_Instance;
#endif
}
