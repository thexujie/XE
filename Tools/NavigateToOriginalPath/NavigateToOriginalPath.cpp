#include "PCH.h"

#include "ClassFactory.h"
#include "NavigateToOriginalPathCommandVerb.h"

#include <new>

XE::uintx_t DllRefCount = 0;

template<typename ClassT>
HRESULT CreateClassFactoryInstance(const IID & riid, void ** ppvObject)
{
    TReferPtr<IClassFactory> pClassFactory(new (std::nothrow) CClassFactory(XE::TDummy<ClassT>()));
    HRESULT hResult = pClassFactory ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hResult))
        hResult = pClassFactory->QueryInterface(riid, ppvObject);
    return hResult;
}

#pragma comment(linker, "/export:DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/export:DllGetClassObject,PRIVATE")
#pragma comment(linker, "/export:DllRegisterServer,PRIVATE")
#pragma comment(linker, "/export:DllUnregisterServer,PRIVATE")

STDAPI DllCanUnloadNow()
{
    return (DllRefCount == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void ** ppv)
{
    if (clsid == __uuidof(CNavigateToOriginalPathCommandVerb))
        return CreateClassFactoryInstance<CNavigateToOriginalPathCommandVerb>(riid, ppv);
    return CLASS_E_CLASSNOTAVAILABLE;
}

const wchar_t GProgID[] = L"*";
const wchar_t GVerbName[] = L"Sample.NavigateToOriginalPath";

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
__inline HINSTANCE GetModuleHINSTANCE() { return (HINSTANCE)&__ImageBase; }
HRESULT RegisterInProcServer(HKEY RootKEY, PCWSTR FriendlyName, XE::FWStringView ThreadingModel, const CLSID & clsid)
{
    wchar_t GuidStr[39];
    int GuidStrLength = StringFromGUID2(clsid, GuidStr, ARRAYSIZE(GuidStr));
    HRESULT Result = FRegkey::SetKeyValue(RootKEY, XE::Format(L"Software\\Classes\\CLSID\\{}", GuidStr), L"", FriendlyName);
    if (SUCCEEDED(Result))
    {
        FWString & Buffer = GetThreadLocalStringBuffer<wchar_t>();
        int ModuleFileNameLength = GetModuleFileNameW(GetModuleHINSTANCE(), Buffer.Data, Buffer.GetCapacityWithoutNull());
        while (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    	{
            Buffer.Resize(0, Max<size_t>(Buffer.Capacity, 1) * 2);
            ModuleFileNameLength = GetModuleFileNameW(GetModuleHINSTANCE(), Buffer.Data, Buffer.GetCapacityWithoutNull());
        }
        Buffer.Resize(ModuleFileNameLength);
    	
        Result = FRegkey::SetKeyValue(RootKEY, XE::Format(L"Software\\Classes\\CLSID\\{}\\InProcServer32", GuidStr), L"", Buffer);
        if (SUCCEEDED(Result))
        {
            Result = FRegkey::SetKeyValue(RootKEY, XE::Format(L"Software\\Classes\\CLSID\\{}\\InProcServer32", GuidStr), L"ThreadingModel", ThreadingModel);
        }
    }
    return Result;
}

HRESULT RegisterExplorerCommandVerb(HKEY RootKey, FWStringView ProgID, FWStringView VerbName, FWStringView VerbDisplayName, const GUID & ClassId)
{
    wchar_t GuidStr[39];
    int GuidStrLength = StringFromGUID2(ClassId, GuidStr, ARRAYSIZE(GuidStr));
	
    FWString VerbRegkeyPath = Format(L"Software\\Classes\\{}\\Shell\\{}", ProgID, VerbName);
    FRegkey::DeleteKey(RootKey, VerbRegkeyPath);

    HRESULT Result = FRegkey::SetKeyValue(RootKey, VerbRegkeyPath, L"ExplorerCommandHandler", GuidStr);
    if (SUCCEEDED(Result))
        Result = FRegkey::SetKeyValue(RootKey, VerbRegkeyPath, L"", VerbDisplayName);

    if (SUCCEEDED(Result))
		Result = FRegkey::SetKeyValue(RootKey, VerbRegkeyPath, L"NeverDefault", FWStringView::Empty);
    return Result;
}

STDAPI DllRegisterServer()
{
    HRESULT Result = RegisterInProcServer(HKEY_CURRENT_USER, L"Navigate To Original Path", L"Apartment", __uuidof(CNavigateToOriginalPathCommandVerb));
    if (SUCCEEDED(Result))
        Result = RegisterExplorerCommandVerb(HKEY_CURRENT_USER, GProgID, GVerbName, L"≤‚ ‘ø¥ø¥", __uuidof(CNavigateToOriginalPathCommandVerb));
    return Result;
}

HRESULT UnRegisterExplorerCommandVerb(HKEY RootKey, FWStringView ProgID, FWStringView VerbName, const GUID & ClassId)
{
    wchar_t GuidStr[39];
    int GuidStrLength = StringFromGUID2(ClassId, GuidStr, ARRAYSIZE(GuidStr));

    FWString VerbRegkeyPath = Format(L"Software\\Classes\\{}\\Shell\\{}", ProgID, VerbName);
    FRegkey::DeleteKey(RootKey, VerbRegkeyPath);

    FWString ClassRegkeyPath = Format(L"Software\\Classes\\{}", GuidStr);
    FRegkey::DeleteKey(RootKey, ClassRegkeyPath);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    return UnRegisterExplorerCommandVerb(HKEY_CURRENT_USER, GProgID, GVerbName, __uuidof(CNavigateToOriginalPathCommandVerb));
}


int _tmain(int argc, const char * args[])
{
    return UnRegisterExplorerCommandVerb(HKEY_CURRENT_USER, GProgID, GVerbName, __uuidof(CNavigateToOriginalPathCommandVerb));
}