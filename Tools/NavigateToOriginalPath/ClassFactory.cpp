#include "PCH.h"
#include "ClassFactory.h"

#include "NavigateToOriginalPathCommandVerb.h"

CClassFactory::~CClassFactory()
{
	XE::Atomics::DecFetch(DllRefCount);
}

HRESULT CClassFactory::QueryInterface(const IID & riid, void ** ppvObject)
{
    static const QITAB qit[] =
    {
        QITABENT(CClassFactory, IClassFactory),
        { 0 }
    };
    return QISearch(this, qit, riid, ppvObject);
}


HRESULT CClassFactory::CreateInstance(IUnknown * pUnkOuter, const IID & riid, void ** ppvObject)
{
    return pUnkOuter ? CLASS_E_NOAGGREGATION : CreateVerbInstance(riid, ppvObject);
}

HRESULT CClassFactory::LockServer(BOOL fLock)
{
    if (fLock) XE::Atomics::IncFetch(DllRefCount);
    else XE::Atomics::DecFetch(DllRefCount);
    return S_OK;
}

