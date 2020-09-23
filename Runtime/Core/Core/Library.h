#pragma once

#include "CoreInc.h"
#include "String.h"

namespace XE
{
    class CORE_API FLibrary
    {
    public:
        FLibrary();
        FLibrary(const FString & ModulePath);
        ~FLibrary();

        EError Load(FStringView ModulePathIn, bool bUserDir = false);
        void Free();
        explicit operator bool() const { return !!Handle; }

        void * GetProc(const char * name);

        template<typename T>
        T GetProc(const char * name)
        {
            void * ptr = GetProc(name);
            return static_cast<T>(ptr);
        }

        template<typename T>
        T GetProc(const std::u8string & name)
        {
            return GetProc<T>(name.c_str());
        }

        void * GetHandle() const { return Handle; }

    protected:
        void * Handle = nullptr;
        FString ModulePath;
    };
}

