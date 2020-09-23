#pragma once

#include "Types.h"

namespace XE
{
    template<typename T>
    concept ReferAble = requires(T & Value)
    {
        Value.AddRef();
        Value.Release();
    };

    template<typename T>
    inline constexpr bool IsReferAbleV = false;

    template<ReferAble T>
    inline constexpr bool IsReferAbleV<T> = true;
	
	template<typename T>
    concept ComReferAble = requires(T & Value, void ** VV)
    {
        __uuidof(T);
        { Value.QueryInterface(__uuidof(T), VV) } ->ConvertibleTo<long_t>;
        { Value.AddRef() } -> ConvertibleTo<uintx_t>;
        { Value.Release() } -> ConvertibleTo<uintx_t>;
    };
	
    template<ReferAble T>
    T * ReferAddRef(T * Pointer)
    {
        if (!Pointer)
            return Pointer;
        Pointer->AddRef();
        return Pointer;
    }

    template<ReferAble T>
    void ReferRelease(T * Pointer)
    {
        if (!Pointer)
            return;
        Pointer->Release();
    }

    template</*ReferAble*/typename T>
    class TReferPtr
    {
    public:
        using ValueType = T;

    public:
        TReferPtr() {}
        TReferPtr(std::nullptr_t)
        {
        }

        TReferPtr(const TReferPtr<T> & That)
        {
            if (That.Pointer)
            {
                Pointer = That.Pointer;
                if (Pointer)
                    Pointer->AddRef();
            }
        }

        TReferPtr(TReferPtr<T> && That) noexcept
		{
            if (That.Pointer)
            {
                Pointer = That.Pointer;
                That.Pointer = nullptr;
            }
        }

        template<ReferAble T2> requires IsConvertibleToV<T2 *, T *>
        TReferPtr(const TReferPtr<T2> & That)
        {
            if (static_cast<void *>(Pointer) != static_cast<void *>(That.Get()))
            {
                Pointer = static_cast<T *>(That.Pointer);
                if (Pointer)
                    Pointer->AddRef();
            }
        }

        explicit TReferPtr(T * PointerIn) :Pointer(PointerIn)
        {

        }
    	
        ~TReferPtr()
        {
            if (Pointer)
            {
                Pointer->Release();
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

        TReferPtr<T> & operator=(const TReferPtr<T> & Another)
        {
            if (Pointer == Another.Pointer)
                return *this;

            if (Pointer)
            {
                Pointer->Release();
                Pointer = nullptr;
            }

            if (Another.Pointer)
            {
                Another.Pointer->AddRef();
                Pointer = Another.Pointer;
            }
            return *this;
        }

        TReferPtr<T> & operator=(TReferPtr<T> && Another) noexcept
        {
            if (Pointer == Another.Pointer)
            {
            	if(Another.Pointer)
            	{
                    Another.Pointer->Release();
                    Another.Pointer = nullptr;
            	}
                return *this;
            }

            if (Pointer)
            {
                Pointer->Release();
                Pointer = nullptr;
            }

            if (Another.Pointer)
            {
                Pointer = Another.Pointer;
                Another.Pointer = nullptr;
            }
            return *this;
        }

        void Reset()
        {
            if (Pointer)
            {
                Pointer->Release();
                Pointer = nullptr;
            }
        }

        void Reset(T * ptr)
        {
            if (Pointer == ptr)
                return;

            if (Pointer)
            {
                Pointer->Release();
                Pointer = nullptr;
            }
            if (ptr)
                ptr->AddRef();
            Pointer = ptr;
        }

        T * Get() const { return Pointer; }
        T ** GetPP() const { if (Pointer) throw 1;  return const_cast<T **>(&Pointer); }
        T ** GetPPSafe() const
        {
            if (Pointer)
            {
                const_cast<TReferPtr *>(this)->Pointer->Release();
                const_cast<TReferPtr *>(this)->Pointer = nullptr;
            }
            return const_cast<T **>(&Pointer);
        }
        void ** GetVV() const { if (Pointer) throw 1;  return reinterpret_cast<void **>(const_cast<T **>(&Pointer)); }
        void ** GetVVSafe() const
        {
            if (Pointer)
            {
                const_cast<TReferPtr *>(this)->Pointer->Release();
                const_cast<TReferPtr *>(this)->Pointer = nullptr;
            }
            return reinterpret_cast<void **>(const_cast<T **>(&Pointer));
        }
        template<typename UT2>
        UT2 ** GetTT() const { if (Pointer) throw 1;  return reinterpret_cast<UT2 **>(GetVV()); }

        T * Ref() const
        {
            if (Pointer)
                Pointer->AddRef();
            return Pointer;
        }

        bool operator == (const TReferPtr<T> & another) const
        {
            return Pointer == another.Pointer;
        }

        bool operator != (const TReferPtr<T> & another) const
        {
            return Pointer != another.Pointer;
        }

    public:
        template<ComReferAble T2> requires ComReferAble<T>
    	TReferPtr<T2> QueryInterface()
        {
            if (!Pointer)
                return nullptr;
        	
            TReferPtr<T2> Result;
            Pointer->QueryInterface(__uuidof(T2), Result.GetVV());
            return Result;
        }
    	
    public:
        T * Pointer = nullptr;
    };

	template<ReferAble T, typename ...ArgsT>
	TReferPtr<T> MakeRefer(ArgsT &&... Args)
	{
        return TReferPtr<T>(New<T>(Forward<ArgsT>(Args)...));
	}

    template<ReferAble T2, ReferAble T>
    TReferPtr<T2> StaticPointerCast(const TReferPtr<T> & Pointer)
    {
        return TReferPtr<T2>(static_cast<T2 *>(ReferAddRef(Pointer.Get())));
    }
	
    template<typename T>
    class TRefer : public T
    {
    public:
        TRefer() = default;
        TRefer(const TRefer &) = default;
        TRefer(TRefer &&) noexcept = default;
        TRefer & operator = (const TRefer &) = default;
        virtual ~TRefer() = default;

        uintx_t AddRef()
        {
            return Atomics::IncFetch(ReferCount);
        }

        uintx_t Release()
        {
            if (ReferCount == 0)
                throw EError::State;

            uintx_t Result = Atomics::DecFetch(ReferCount);
            if (Result == 0)
                DeleteUnSafe(this);
            return Result;
        }

        template<ReferAble T2>
        TReferPtr<T2> Refer() const
        {
            TRefer * Pointer = const_cast<TRefer *>(this);
            Pointer->AddRef();
            return TReferPtr<T>(static_cast<T *>(Pointer));
        }
		
    public:
        uintx_t ReferCount = 1;
    };


    template<typename T>
    struct IsReferPointer : FalseType {};
	
    template<typename T>
    struct IsReferPointer<TReferPtr<T>> : TrueType { using Type = T; };

	template<typename T>
    inline constexpr bool IsReferPointerV = IsReferPointer<T>::Value;

}
