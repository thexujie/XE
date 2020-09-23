#include "PCH.h"
#include <future>
#include <iostream>
#include <queue>
#include <windows.h>

#include <set>


#include "Test.h"
#include "EASTL/map.h"
#include "EASTL/set.h"
#include "EASTL/tuple.h"

using namespace XE;

#include <crtdbg.h>
void * operator new[](size_t size, const char * pName, int flags, unsigned debugFlags, const char * file, int line)
{
	return _malloc_dbg(size, _NORMAL_BLOCK, pName, line);
}

void * operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char * pName, int flags, unsigned debugFlags, const char * file, int line)
{
	return _aligned_malloc_dbg(size, alignment, pName, line);
	
}

void Output(FDataStream & DataStream)
{
	int IntValue = 0;
	DataStream >> IntValue;
	DataStream << 12;
}

struct SSerializerTest
{
	int IntValue = 0;
};

inline FArchive & operator >> (FArchive & Serializer, SSerializerTest & ST)
{
	Serializer >> ST.IntValue;
	return Serializer;
}

#include <bitset>


void TestTuple();

template<const char * VS, const char * HS, const char * DS, const char * GS>
class FHHVertexFactory
{
public:
	
};


template<typename KeyT, typename ValueT>
struct SSSS
{
	int a = 12;
	KeyT Key;
	ValueT Value;
};

struct ClassA
{
	virtual ~ClassA() = default;
	int a;
};
struct ClassB : ClassA { int b; };
struct ClassC : ClassA { int c; };
struct ClassD : ClassB { int d; };


//_NODISCARD inline bool operator==(TCoroutineHandle<> _Left, TCoroutineHandle<> _Right) noexcept {
//	return _Left.address() == _Right.address();
//}
//
//_NODISCARD inline bool operator<(TCoroutineHandle<> _Left, TCoroutineHandle<> _Right) noexcept {
//	return less<void *>()(_Left.address(), _Right.address());
//}
//
//_NODISCARD inline bool operator!=(TCoroutineHandle<> _Left, TCoroutineHandle<> _Right) noexcept {
//	return !(_Left == _Right);
//}
//
//_NODISCARD inline bool operator>(TCoroutineHandle<> _Left, TCoroutineHandle<> _Right) noexcept {
//	return _Right < _Left;
//}
//
//_NODISCARD inline bool operator<=(TCoroutineHandle<> _Left, TCoroutineHandle<> _Right) noexcept {
//	return !(_Left > _Right);
//}
//
//_NODISCARD inline bool operator>=(TCoroutineHandle<> _Left, TCoroutineHandle<> _Right) noexcept {
//	return !(_Left < _Right);
//}

#include <d3d12.h>

class FUnkownBase
{
public:
	FUnkownBase() = default;
	FUnkownBase(const FUnkownBase &) = default;
	FUnkownBase(FUnkownBase &&) noexcept = default;
	FUnkownBase & operator = (const FUnkownBase &) = default;
	virtual ~FUnkownBase() = default;

	virtual HRESULT QueryInterface(const IID & riid, void ** ppvObject)
	{
		return E_NOINTERFACE;
	}

	virtual ULONG AddRef()
	{
		return Atomics::IncFetch(ReferCount);
	}

	virtual ULONG Release()
	{
		if (ReferCount == 0)
			throw EError::State;

		return Atomics::DecFetch(ReferCount);
	}
public:
	ULONG ReferCount = 1;
};

int _tmain(int argc, const char *args[])
{
	XE::TView<char>::Empty;

	XE::TestFun();
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(335);
	FApplication Application;

	{
	}
	{
		enum class EKey
		{
			A,
			B,
			C,
		};

		FVariant KeyVar = EKey::B;
		KeyVar.EnumType->name();

		auto Key = KeyVar.Get<EKey>();
		KeyVar.Reset();
	}
	{
		FStringView SA = Str("ABCD");
		FStringView SB = Str("abcd");
		bool BE = SA == SB;
		BE = false;
	}
	for (FStringView Entry : FDirectory::GetEntries(Str("C:\\Windows")))
	{
		FSystem::DebugOutput(Entry);
		std::wcout << Strings::ToStdWString(Entry) << std::endl;
	}
	{
		FVariant Variant;
		Vec2F A;
		Vec2F B;
		auto C = A <=> B;
		int value = TypeOf<int>().Parse(StrView("234")).Get<int>();
		Assert(value == 234);
		auto Str = TypeOf<int>().Format(FVariant(value));
		Str.IsEmpty();
	}
	{
		TUniquePtr<ClassA> PA = new ClassA();
		TUniquePtr<ClassB> PB = new ClassB();
		TUniquePtr<ClassC> PC = new ClassC();
		TUniquePtr<ClassD> PD = new ClassD();
		TPointer<ClassA> PAA = PD.Get();
		auto ta = typeid(*PA).name();
		auto tb = typeid(*PB).name();
		auto tc = typeid(*PC).name();
		auto taa = typeid(*PAA).name();
		auto ee = ta == taa;
	}
	{
		FGuid GUid = FGuid::Generate();
		GUid = FGuid::Generate();
		GUid = FGuid::Generate();
		GUid = FGuid::Generate();
		GUid = FGuid::Generate();
	}
	
	{
		auto Path = FString::Empty;
		Path = FPath::LexicallyNormal(Str("../Content/UI/ControlTemplates/"));
		Path = FPath::LexicallyNormal(Str("./.././Content/UI/ControlTemplates/./../"));
		Path = FPath::LexicallyNormal(Str("a/../.././Content/UI/ControlTemplates/./../"));
		//auto Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\.svn\\file.ext"));
		Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\..\\.\\.\\.svn\\file.ext"));
		Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\..\\.\\.\\abc.svn\\file.ext"));
		Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\..\\.\\.\\abc..svn\\file.ex.."));
		Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\..\\.\\.\\abc.\\file."));
		Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\..\\.\\.\\abc..\\file.."));
		Path = FPath::LexicallyNormal(Str("C:\\Windows\\System32\\Drivers\\..\\../../../../..\\.\\.\\abc..\\file.."));
		Path.IsEmpty();
	}
	
	FGlobalLogger GlobalLogger(Str("../Data/Local/Log.log"), ELogLevel::LogLevel_Dbg);
	LogInfo(Str("Enter Main"));

	std::set<int> sets;
	std::unique_ptr<int> upa;
	std::unique_ptr<int> upb;

	std::map<FGuid, std::unique_ptr<int>> pp;
	FGuid GG = __uuidof(ID3D12Device);
	const FGuid & GGF = GG;
	//pp.insert_or_assign(GGF, upb);
	TPair<int, TUniquePtr<int>> pp0;
	TPair<int, TUniquePtr<int>> ppppp = Move(pp0);


	{
		FVariant V1 = 0.125f;
		FVariant V2 = 0.125;
		FVariant V3 = 125;
		FVariant V4 = "fegwg";
		FVariant V5 = TVector<int>({ 12, 13 });
		FVariant S22 = FString(Str("xujie"));
		FVariant VaVec2F = Vec2F();
		FVariant VaVec3F = Vec3F();
		FVariant VaVec4F = Vec4F();
		FVariant VaVec2I = Vec2I();
		FVariant VaVec3I = Vec3I();
		FVariant VaVec4I = Vec4I();
		FVariant VaVec2U = Vec2U();
		FVariant VaVec3U = Vec3U();
		FVariant VaVec4U = Vec4U();
		FVariant VaVector2 = FVector2();
		FVariant VaVector3 = FVector3();
		FVariant VaVector4 = FVector4();
		int a = 1;
	}
	
	{
		
		auto WD = FPath::GetDirectory(FPath::GetWorkingDirectory());
		WD = {};
	}

	{
		auto Comp = 0 <=> 0;
		auto CompA = Compare(0, 0);
		auto CompB = Compare(EFormat::None, EFormat::B32G32R32F);
		auto CompC = Compare(FString(), FString());
	}
	
	{
		std::bitset<64> bbb;
		bbb.set(12, true);
		
		FBitSet Bitset;
		Bitset.Add(true);
		Bitset.Add(false);
		Bitset.Add(false);
		for (int index = 0; index < 100; ++index)
			Bitset.Add((index % 2 == 1) && index < 80);

		Bitset.Add(false);

		Assert(Bitset[0] == true);
		Assert(Bitset[1] == false);

		Bitset.Resize(100, false);
		Bitset.SetAt(10, 12, true);
		Bitset.SetAt(12, 2, false);
		Bitset.SetAt(13, 2, true);

		Bitset.SetAt(0, 100, false);
		Bitset.SetAt(60, 16, true);


		Bitset.Resize(200, true);
		Bitset.SetAt(60, 4, false);

		Bitset.SetAt(100, 64, false);
	}
	{
		FGuid Guid1 = "{2D66CF94-7E10-4BF0-A3FD-DD517159753C}";
		FGuid Guid2 = { 0x2d66cf94, 0x7e10, 0x4bf0, 0xa3, 0xfd, 0xdd, 0x51, 0x71, 0x59, 0x75, 0x3c };
		FGuid Guid3 = "2D66CF94-7E10-4BF0-A3FD-DD517159753C";
		auto str = Guid1.Format();
		auto ee = Guid1 == Guid3;
		auto eee = Guid1 <=> Guid3;

		//auto Str = Format(Str("{{{:X}-{:X}-{:X}-{:X}-{:X}{:X}{:X}}}"), Guid3.Data1, Guid3.Data2, Guid3.Data3,
		//	uint16_t(((Guid3.Data4[0] << 8) | Guid3.Data4[1])),
		//	uint16_t(((Guid3.Data4[2] << 8) | Guid3.Data4[3])),
		//	uint16_t(((Guid3.Data4[4] << 8) | Guid3.Data4[5])),
		//	uint16_t(((Guid3.Data4[6] << 8) | Guid3.Data4[7])));
		//auto ggg = TFormater<FGuid>()(Guid2);
		//Guid2.ToString();

		auto Str = Guid1.Format();
		auto RRR1 = Format(Str("{}"), 123);
		auto RRR = Format(Str("{}"), Str);
		auto RRR2 = Format(Str("{}"), Guid1);
	}

	//{
	//	TPointer<int> Int0;
	//	
	//	TPointer<int> IntP = new int(12);
	//	auto bb = Int0 < IntP;
	//}
	
	TestTuple();
	//
	//std::vector<int> arr = {1232,4124,4214,424,42142};
	////std::tuple<> tt;
	//XE2::TDelegate<int(int, int)> AddDelegate;
	////AddDelegate.Bind(&STest::Add, &Test, 45, Placer2);
	//AddDelegate.Invoke(40);
	//
	//auto Fun = &STest::Add;
	//auto re = Apply(Fun, MakeTuple(&Test, 12, 11));
	
	//AddDelegate.Bind([](int a, int b) {return a + b; });
	//AddDelegate.Bind(&STest::Add, &Test);
	//int Result = AddDelegate.Invoke(12, 13);
	
	//_CrtSetBreakAlloc(330);

	bool IV1 = IsSignedV<int>;
	IV1 = IsSignedV<float>;
	IV1 = IsSignedV<char>;
	IV1 = IsSignedV<bool>;
	IV1 = IsSignedV<FString>;
	IV1 = IsSignedV<uint8_t>;
	IV1 = IsSignedV<uint16_t>;
	IV1 = IsSignedV<uint32_t>;
	IV1 = IsSignedV<float>;
	IV1 = IsSignedV<float1>;
	char Memory[128] = {};

	struct FTest
	{
		int a;
	};
	FTest Haha = { 46 };
	
	new (Memory) FTest(Haha);
	new (Memory) FTest(Move(Haha));
	new (Memory) FTest(Move(Haha));

	bool E1 = FPath::Exists(Str("111.tx"));
	bool E2 = FPath::Exists(Str("Logger.h"));
	bool IsFile = FPath::IsFile(Str("Logger.h"));
	bool IsDirectory = FPath::IsDirectory(Str("Logger.h"));
	FString WorkingDirectory = FPath::GetWorkingDirectory();
	TVector<FString> PathsAll;
	TVector<FString> Folders = { FString() };

	auto FunEnumEntries = [&](FStringView Path, auto && FuncSelf)->void
	{
		for (auto & Entry : FDirectory::GetEntries(Path))
		{
			PathsAll.Add(Entry);
			if (FPath::IsDirectory(Entry))
				FuncSelf(Entry, FuncSelf);
		}
	};

	FunEnumEntries(nullptr, FunEnumEntries);

	FString StringA = Str("0123456789");
	StringA = Str("123");
	StringA = Str("12fegwg");
	StringA = Str("01234567890123456789");
	StringA = Str("0123456789");
	StringA.Append(Str("abc"));

	int a = 12;
	int & A = a;
	
	//TVector<FString> Files = FDirectory::GetFiles(Str(""));
	//TVector<FString> Directories = FDirectory::GetDirectories(Str(""));
	
	//FGlobalLogger::Stop();
	
	auto fff = XE::Format("{} {}!", "Hello", "world", "something");
	char c = 120;
	auto s0 = XE::Format("{:6}", 42);    // s0 的值为 "    42"
	auto s1 = XE::Format("{:6}", 'x');   // s1 的值为 "x     "
	auto s2 = XE::Format("{:*<6}", 'x'); // s2 的值为 "x*****"
	auto s3 = XE::Format("{:*>6}", 'x'); // s3 的值为 "*****x"
	auto s4 = XE::Format("{:*^6}", 'x'); // s4 的值为 "**x***"
	auto s5 = XE::Format("{:6d}", c);    // s5 的值为 "   120"
	auto s6 = XE::Format("{:6}", true);  // s6 的值为 "true  "
	
	SSerializerTest ST;
	FString String;
	TVector<int> Vector;
	int IntValue;
	TVector<FVector3> Positions;
	TVector<FMatrix> Matries;

	FXmlDocument Xml;
	Xml.LoadFromFile(Str("../Core/Core.vcxproj"));
	TVector<FXmlNode> GetChildren = Xml.GetRootNode().GetChildren();
	TVector<FXmlNode> ItemGroups = Xml.GetRootNode().GetChildren(Str("ItemGroup"));
	TVector<FString> ClIncludeFiles;
	for (auto & ItemGroup : ItemGroups)
	{
		auto ClIncludes = ItemGroup.GetChildren(Str("ClInclude"));
		for (auto & ClInclude : ClIncludes)
			ClIncludeFiles.Add(ClInclude.GetAttribute(Str("Include")));
	}
	
	FMemoryStream MemoryStream;
	{
		String = Str(R"(CoreTest.exe”(Win32): 已加载“C:\Windows\System32\KernelBase.dll”。包含/排除设置已禁用符号加载)");
		String = Str(R"(CoreTest.exe”(Win32): 已加载“C:\Windows\System32\KernelBase.dll”。包含/排除设置已禁用符号加载)");
		Vector = {12, 13, 14, 15, 16, 17, 18, 19, 20};
		ST.IntValue = 1234;
		IntValue = 8676;
		Positions = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
		Matries =
		{
			FMatrix::LookAtLH({ 0, 0, 1 }, {}, { 0, 1, 0 }),
			FMatrix::Identity,
			FMatrix::OrthogonaLH(100, 200, 0.001f, 1.0f)
		};
		
		//FFileStream FileStream(u8"D:\\SerializerTest.data", EFileMode::Out | EFileMode::CreateAlways);
		FDataStream DataStream(MemoryStream);
		FArchive SerializerOutput(DataStream, EArchiveFlags::Output);
		SerializerOutput >> ST;
		SerializerOutput >> String;
		SerializerOutput >> Vector;
		SerializerOutput >> IntValue;
		SerializerOutput >> Positions;
		SerializerOutput >> Matries;

		String.Clear();
		Vector.Clear();
		Positions.Clear();
		Matries.Clear();
		IntValue = 0;
	}

	MemoryStream.Reset();
	{
		//FFileStream FileStream(u8"D:\\SerializerTest.data", EFileMode::In);
		FDataStream DataStream(MemoryStream);
		FArchive SerializerInput(DataStream, EArchiveFlags::Input);
		SerializerInput >> ST;
		SerializerInput >> String;
		SerializerInput >> Vector;
		SerializerInput >> IntValue;
		SerializerInput >> Positions;
		SerializerInput >> Matries;
	}
	
	
	//_CrtSetBreakAlloc(236);
	//std::set<std::string> stdsets;
	//stdsets.insert("Key00");

	//std::map < std::string, std::string> stdmap;
	//stdmap.insert_or_assign("Key00", "Value00");
	//stdmap.find("");
	
	//eastl::set<std::string> sets;
	//auto iter = sets.insert("abc");

	std::multimap<std::string, std::string> mulmap;
	mulmap.insert({ std::string("abc"), "abc_00" });
	mulmap.insert({ std::string("abc"), "abc_01" });
	mulmap.insert({ std::string("abc"), "abc_02" });
	mulmap.insert({ std::string("AAA"), "AAA__00" });
	mulmap.insert({ std::string("AAA"), "AAA__01" });
	mulmap.insert({ std::string("AAA"), "AAA__02" });

	auto iters = mulmap.find(std::string("AAA"));
	//eastl::map<std::string, std::string> strmap;
	//strmap.insert_or_assign("Key00", "Value00");
	//strmap.insert_or_assign("Key01", "Value01");

	TMap<FString, FString> StrMap;
	StrMap.InsertOrAssign(Str("Key00"), Str("Value00"));
	StrMap.InsertOrAssign(Str("Key01"), Str("Value01"));
	StrMap.Find(Str("Key01"))->Value = Str("3123");

	THashMap<FString, FString> StrHashMap;
	StrHashMap.InsertOrAssign(Str("Key00"), Str("Value00"));
	StrHashMap.InsertOrAssign(Str("Key01"), Str("Value01"));
	auto Iter2 = StrHashMap.Find(Str("Key00"));
	StrHashMap.Find(Str("Key01"))->Value = Str("3123");

	TSet<FString> Set;
	Set.Insert(Str("Key00"));
	Set.Insert(Str("Key01"));
	Set.Insert(Str("Key02"));
	bool C = Set.Contains(Str("Key01"));

	TSet<int> IntSet;
	TMap<int, FString> IntMap;
	std::map<int, FString> StdIntMap;
	TMultiMap<int, FString> MultiIntMap;

	int Count = 1000;
	int values[] = { 1, 10, 9, 2, 8, 6 };
	for (int cnt = 0; cnt < Count; ++cnt)
	{
		int val = FRandom::Rand() % 256;
		IntSet.Insert(val);
		IntMap.InsertOrAssign(val % 10, Strings::Printf(u8"String %d", val));
		IntMap.InsertOrAssign(cnt, Strings::Printf(u8"String %d", val));
		MultiIntMap.Insert(cnt % 10, Strings::Printf(u8"String %d", cnt));
		assert(IntSet.IsRBTree());
		assert(IntMap.IsRBTree());
		assert(MultiIntMap.IsRBTree());
		
		StdIntMap.insert_or_assign(val % 10, Strings::Printf(u8"String %d", val));
	}
	IntMap.InsertOrAssign(0, Str("XXX"));
	IntMap.InsertOrAssign(0, Str("YYYY"));

	assert(IntSet.IsRBTree());
	assert(IntMap.IsRBTree());
	
	TVector<FString> Strings;
	auto Iter = IntMap.Find(1);
	auto StdIter = StdIntMap.find(1);
	auto LB = MultiIntMap.LowerBound(1);
	auto UB = MultiIntMap.UpperBound(1);
	auto Iters = MultiIntMap.Find(9);
	for (auto StrIter = Iters.Key; StrIter != Iters.Value; ++StrIter)
	{
		Strings.Add(StrIter->Value);
	}

	TMap<int, TSharedPtr<FString>> StringPtrMap;
	StringPtrMap.InsertOrAssign(12, MakeShared<FString>(Str("XXX")));
	StringPtrMap.InsertOrAssign(112, MakeShared<FString>(Str("XXX")));
	StringPtrMap.InsertOrAssign(1112, MakeShared<FString>(Str("XXX")));
	assert(StringPtrMap.IsRBTree());
	return 0;
	//auto v11 = eastl::get<0>(i32c1);
	//auto [iva, cval, fval] = i32c1;
	
	{
		THashMap<FStringView, FString> StringMap;
		StringMap.InsertOrAssign(Str("Number12"), Str("Number 12"));
		StringMap.InsertOrAssign(Str("Number13"), Str("Number 13"));
		StringMap.InsertOrAssign(Str("Number14"), Str("Number 14"));
		StringMap.InsertOrAssign(Str("Number15"), Str("Number 15"));
		auto Str13 = StringMap[Str("Number13")];
		auto Str17 = StringMap[Str("Number17")];
	}
	
	return 0;
	//FMutex Mutex;
	FMutex Lock;
	FConditionalVariable ConditionalVariable;
	std::thread T1 = std::thread([&]()
		{
			ConditionalVariable.Wait(Lock);
			std::cout << "Waited !!!!" << std::endl;
		});

	std::thread T2 = std::thread([&]()
		{
		for (int cnt = 0; cnt < 3; ++cnt)
		{
			std::cout << "Delay " << cnt << " time"<< std::endl;
			Sleep(3000);
		}
		ConditionalVariable.NotifyAll();
		});

	T2.join();
	T1.join();
	
	int count = 0;
	std::vector<std::thread> threads;
	for (int tindex = 0; tindex < 100; ++tindex)
	{
		threads.push_back(std::thread([&]()
			{
				for (int index = 0; index < 10000; ++index)
				{
					TUniqueLock L(Lock);
					++count;
				}
			}));
	}

	for (auto & th : threads)
		th.join();
	
	//float3 z = float3::Zero;
	
	FString Name(Str("My name is "));
	Name += Str("hello");

	TVector<FString> Names;
	Names.Add(Str("fff"));
	Names.Add(Str("eee"));
	Names.Add(Str("ddd"));
	Names.Add(Str("ccc"));
	Names.Add(Str("bbb"));
	Names.Add(Str("aaa"));
	Names.Add(Str("999"));
	Names.Add(Str("888"));
	Names.Add(Str("777"));
	Names.Add(Str("666"));
	Names.Add(Str("555"));
	Names.Add(Str("444"));
	Names.Add(Str("333"));
	Names.Add(Str("222"));
	Names.Add(Str("111"));
	Names.Add(Str("000"));
	
	TMap<int, int> Map;
	Map[9] = 999;
	Map[8] = 888;
	Map[7] = 777;
	//Map[6] = 666;
	//Map[5] = 555;
	Map[4] = 444;
	//Map[3] = 333;
	Map[2] = 222;
	Map[1] = 111;
	auto iter9 = Map.Find(9);
	auto iter8 = Map.Find(8);
	auto iter7 = Map.Find(7);
	auto iter6 = Map.Find(6);
	auto iter5 = Map.Find(5);
	auto iter4 = Map.Find(4);
	auto iter3 = Map.Find(3);
	auto iter2 = Map.Find(2);
	auto iter1 = Map.Find(1);
	
		//	TMap<FStr1ing, int> Map;
	//Map[Str("ABC")] = 111;
	//Map[Str("feg")] = 777;
	//Map[Str("wagegw")] = 111;
	//Map[Str("gggg")] = 333;
	//Map[Str("123")] = 655;
	//Map[Str("yiuo")] = 45435;
	//Map[Str("123")] = 980;
	//Map[Str("8768768")] = 456;
	//assert(Map.IsRBTree());

	//TMap<FString, int>::Iterator iter = Map.Find(Str("123"));
	
	//std::map<FString, int> Map2;
	//Map2[Str("ABC")] = 111;
	//Map2[Str("feg")] = 777;
	//Map2[Str("wagegw")] = 111;
	//Map2[Str("gggg")] = 333;

	//std::unordered_map<FString, int> Map3;
	//Map3[Str("ABC")] = 111;
	//Map3[Str("feg")] = 777;
	//Map3[Str("wagegw")] = 111;
	//Map3[Str("gggg")] = 333;

	//eastl::map<FString, int> Map4;
	//Map4[Str("ABC")] = 111;
	//Map4[Str("feg")] = 777;
	//Map4[Str("wagegw")] = 111;
	//Map4[Str("gggg")] = 333;
	
	return 0;
}
