#pragma once

template<class T>
HRESULT SetInterface(T ** ppT, IUnknown * punk)
{
    SafeRelease(ppT);
    return punk ? punk->QueryInterface(ppT) : E_NOINTERFACE;
}

class __declspec(uuid("FDB44147-0C74-4D1A-8E48-D2F8F42378EF")) CNavigateToOriginalPathCommandVerb;

class CNavigateToOriginalPathCommandVerb :
    public XE::Win32::TUnkown<
    IExplorerCommand,
    IInitializeCommand,
    IObjectWithSite
    >
{
public:
    HRESULT QueryInterface(const IID & riid, void ** ppvObject) override
    {
        static const QITAB qit[] =
        {
            QITABENT(CNavigateToOriginalPathCommandVerb, IExplorerCommand),       // required
            QITABENT(CNavigateToOriginalPathCommandVerb, IInitializeCommand),     // optional
            QITABENT(CNavigateToOriginalPathCommandVerb, IObjectWithSite),        // optional
            { 0 },
        };
        return QISearch(this, qit, riid, ppvObject);
    }


    // IExplorerCommand
    IFACEMETHODIMP GetTitle(IShellItemArray * /* psiItemArray */, LPWSTR * ppszName)
    {
        // the verb name can be computed here, in this example it is static
        return SHStrDupW(L"Navigate To Original Path", ppszName);
    }

    IFACEMETHODIMP GetIcon(IShellItemArray * /* psiItemArray */, LPWSTR * ppszIcon)
    {
        // the icon ref ("dll,-<resid>") is provied here, in this case none is provieded
        *ppszIcon = NULL;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetToolTip(IShellItemArray * /* psiItemArray */, LPWSTR * ppszInfotip)
    {
        // tooltip provided here, in this case none is provieded
        *ppszInfotip = NULL;
        return E_NOTIMPL;
    }

    IFACEMETHODIMP GetCanonicalName(GUID * pguidCommandName)
    {
        *pguidCommandName = __uuidof(this);
        return S_OK;
    }

    // compute the visibility of the verb here, respect "fOkToBeSlow" if this is slow (does IO for example)
    // when called with fOkToBeSlow == FALSE return E_PENDING and this object will be called
    // back on a background thread with fOkToBeSlow == TRUE
    IFACEMETHODIMP GetState(IShellItemArray * ShellItemArray, BOOL fOkToBeSlow, EXPCMDSTATE * pCmdState);

    IFACEMETHODIMP Invoke(IShellItemArray * psiItemArray, IBindCtx * pbc);

    IFACEMETHODIMP GetFlags(EXPCMDFLAGS * pFlags)
    {
        *pFlags = ECF_DEFAULT;
        return S_OK;
    }

    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand ** ppEnum)
    {
        *ppEnum = NULL;
        return E_NOTIMPL;
    }

    // IInitializeCommand
    IFACEMETHODIMP Initialize(PCWSTR /* pszCommandName */, IPropertyBag * /* ppb */)
    {
        // the verb name is in pszCommandName, this handler can vary its behavior
        // based on the command name (implementing different verbs) or the
        // data stored under that verb in the registry can be read via ppb
        return S_OK;
    }

    // IObjectWithSite
    IFACEMETHODIMP SetSite(IUnknown * punkSite)
    {
        UnknownSite.Reset(punkSite);
        return S_OK;
    }

    IFACEMETHODIMP GetSite(REFIID riid, void ** ppv)
    {
        *ppv = NULL;
        return UnknownSite ? UnknownSite->QueryInterface(riid, ppv) : E_FAIL;
    }

	~CNavigateToOriginalPathCommandVerb()
    {
	    
    }
private:
	XE::TReferPtr<IUnknown> UnknownSite;
};

