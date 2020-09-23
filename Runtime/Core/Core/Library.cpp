#include "PCH.h"
#include "Library.h"
#include "Platform/Win32/Win32.h"

namespace XE
{
    FLibrary::FLibrary() :
        Handle(NULL)
    {

    }

    FLibrary::FLibrary(const FString & ModulePathIn)
    {
        Load(ModulePathIn);
    }

    FLibrary::~FLibrary()
    {
        Free();
    }

    EError FLibrary::Load(FStringView ModulePathIn, bool bUserDir)
    {
        Free();
        if (!ModulePathIn.IsEmpty())
        {
            FWStringView PathW = Strings::ToWide(ModulePathIn);
            if (bUserDir)
                Handle = ::LoadLibraryExW(PathW.GetData(), NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);
            else
                Handle = ::LoadLibraryW(PathW.GetData());
            //if (!Handle)
            //    core::war() << __FUNCTION__" LoadLibrary [" << pathw << "] failed " << win32::winerr_str(GetLastError());
            //else
            {
                wchar_t ModuleFilePathW[1024] = {};
                uint32_t Length = GetModuleFileNameW((HMODULE)Handle, ModuleFilePathW, 1024);
                ModulePath = Strings::FromWide(ModuleFilePathW, Length);
            }
        }
        return Handle ? EError::OK : EError::Inner;
    }

    void FLibrary::Free()
    {
        if (Handle)
        {
            ::FreeLibrary((HMODULE)Handle);
            Handle = NULL;
        }
    }

    void * FLibrary::GetProc(const char * name)
    {
        if (Handle)
            return static_cast<void *>(::GetProcAddress((HMODULE)Handle, name));
        else
            return nullptr;
    }
}
