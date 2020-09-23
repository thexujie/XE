#pragma once

#include "CoreWindowsInc.h"

namespace XE::Windows
{
	template<typename T>
	class TCoMemPtr
	{
	public:
        TCoMemPtr() = default;
        TCoMemPtr(NullPtrT) : Pointer(nullptr) {}
		explicit TCoMemPtr(T * Pointer_) : Pointer(Pointer_) {}
        TCoMemPtr(TCoMemPtr && Another) noexcept
		{
            Pointer = Another.Pointer;
            Another.Pointer = nullptr;
		}

        ~TCoMemPtr()
        {
            if (Pointer)
            {
                CoTaskMemFree(Pointer);
                Pointer = nullptr;
            }
        }

        T * operator ->() const
        {
            if (!Pointer)
                throw std::exception("null com ptr");
            return Pointer;
        }
        T & operator * () { return *Pointer; }
        T ** operator & () { return &Pointer; }

        const T & operator * ()const { return *Pointer; }
        explicit operator bool() const { return !!Pointer; }

        TCoMemPtr<T> & operator=(TCoMemPtr<T> && Another) noexcept
        {
            if (Pointer == Another.Pointer)
                throw EError::InvalidOperation;

            if (Pointer)
            {
                CoTaskMemFree(Pointer);
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
                CoTaskMemFree(Pointer);
                Pointer = nullptr;
            }
        }

        void Reset(T * Another)
        {
            if (Pointer == Another)
                return;

            if (Pointer)
            {
                CoTaskMemFree(Pointer);
                Pointer = nullptr;
            }
            Pointer = Another;
        }

        T * Get() const { return Pointer; }
        T ** GetPP() const { if (Pointer) throw 1;  return const_cast<T **>(&Pointer); }
        T ** GetPPSafe() const
		{
			if (Pointer)
			{
                CoTaskMemFree(const_cast<TCoMemPtr *>(this)->Pointer);
                const_cast<TCoMemPtr *>(this)->Pointer = nullptr;
			}
			return const_cast<T **>(&Pointer);
        }
        void ** GetVV() const { if (Pointer) throw 1;  return reinterpret_cast<void **>(const_cast<T **>(&Pointer)); }
        template<typename T2>
        T2 ** GetTT() const { if (Pointer) throw 1;  return reinterpret_cast<T2 **>(GetVV()); }
		
	public:
		T * Pointer = nullptr;
	};
}

