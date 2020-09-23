#pragma once

#include "Types.h"

namespace XE
{
    template<typename T>
    class TPointer
    {
    public:
        TPointer() = default;
        TPointer(const TPointer<T> & That) = default;
        TPointer(TPointer<T> && That) noexcept : Pointer(That.Pointer) { That.Pointer = nullptr; }
    	
        TPointer(std::nullptr_t) { }
        auto operator <=> (const TPointer<T> &) const = default;

        template<typename T2, typename = EnableIfT<IsConvertibleToV<T2 *, T *>>>
        TPointer(const TPointer<T2> & That) : Pointer(That.Pointer) {}

        template<typename T2, typename = EnableIfT<IsConvertibleToV<T2 *, T *>>>
        TPointer(TPointer<T2> && That) noexcept : Pointer(That.Pointer) { That.Pointer = nullptr; }

        TPointer(T * PointerIn) :Pointer(PointerIn) { }

        template<typename T2, typename = EnableIfT<IsConvertibleToV<T2 *, T *>>>
        TPointer(T2 * PointerIn) :Pointer(PointerIn) { }
    	
        ~TPointer()
        {
            if (Pointer)
            {
                Pointer = nullptr;
            }
        }

        T * operator ->() const
        {
            if (!Pointer)
                throw std::exception("null com ptr");
            return Pointer;
        }

        T & operator *()
        {
            if (!Pointer)
                throw EError::Nullptr;
            return *Pointer;
        }

        const T & operator *() const
        {
            if (!Pointer)
                throw EError::Nullptr;
            return *Pointer;
        }
    	
        explicit operator bool() const { return !!Pointer; }

        TPointer<T> & operator=(const TPointer<T> & another)
        {
            Pointer = another.Pointer;
            return *this;
        }

        template<typename T2, typename = EnableIfT<IsConvertibleToV<T2 *, T *>>>
        TPointer<T> & operator=(const TPointer<T2> & Another)
        {
            Pointer == Another.Pointer;
            return *this;
        }

        template<typename T2, typename = EnableIfT<IsConvertibleToV<T *, T2 *>>>
        TPointer<T2> As() const
        {
            if (!Pointer)
                return nullptr;
            return TPointer<T2>(Pointer);
        }

        void Reset()
        {
            if (Pointer)
            {
                Pointer = nullptr;
            }
        }

        void Reset(T * Another)
        {
            Pointer = Another;
        }

        T * Get() const { return Pointer; }
        T ** GetPP() const { if (Pointer) throw EError::State;  return const_cast<T **>(&Pointer); }
        void ** GetVV() const { if (Pointer) throw EError::State;  return reinterpret_cast<void **>(const_cast<T **>(&Pointer)); }
        template<typename T2>
        T2 ** GetTT() const { if (Pointer) throw EError::State;  return reinterpret_cast<T2 **>(GetVV()); }

    public:
        T * Pointer = nullptr;
    };

    template<typename T2, typename T>
    TPointer<T2> StaticPointerCast(const TPointer<T> & Pointer)
    {
        return TPointer<T2>(static_cast<T2 *>(Pointer.Get()));
    }
}
