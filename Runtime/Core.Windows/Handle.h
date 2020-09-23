#pragma once

#include "CoreWindowsInc.h"

namespace XE::Windows
{
	template<typename HandleT = HANDLE>
	class THandle
	{
	public:
        THandle() = default;
        THandle(NullPtrT) : Pointer(nullptr) {}
		explicit THandle(HandleT Pointer_) : Pointer(Pointer_) {}
        THandle(THandle && Another) noexcept
		{
            Pointer = Another.Pointer;
            Another.Pointer = nullptr;
		}

        ~THandle()
        {
            if (Pointer)
            {
                CloseHandle(Pointer);
                Pointer = nullptr;
            }
        }

        operator HandleT() { return Pointer; }
        HandleT * operator & () { return &Pointer; }

        explicit operator bool() const { return !!Pointer; }

        THandle & operator=(THandle && Another) noexcept
        {
            if (Pointer == Another.Pointer)
                throw EError::InvalidOperation;

            if (Pointer)
            {
                CloseHandle(Pointer);
                Pointer = nullptr;
            }

            Pointer = Another.Pointer;
            Another.Pointer = nullptr;
            return *this;
        }

        void Reset()
        {
            if (Pointer)
            {
                CloseHandle(Pointer);
                Pointer = nullptr;
            }
        }

        void Reset(HandleT Another)
        {
            if (Pointer == Another)
                return;

            if (Pointer)
            {
                CloseHandle(Pointer);
                Pointer = nullptr;
            }
            Pointer = Another;
        }

        HandleT Get() const { return Pointer; }
        HandleT * GetP() const { if (Pointer) throw 1;  return &Pointer; }
        HandleT * GetPSafe() const
		{
			if (Pointer)
			{
                CloseHandle(const_cast<THandle *>(this)->Pointer);
                const_cast<THandle *>(this)->Pointer = nullptr;
			}
            return &Pointer;
        }
		
	public:
        HANDLE Pointer = nullptr;
	};
}

