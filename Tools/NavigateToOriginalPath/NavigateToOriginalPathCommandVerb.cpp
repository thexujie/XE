#include "PCH.h"
#include "NavigateToOriginalPathCommandVerb.h"

#pragma warning(disable: 4090)

__inline HRESULT GetItemAt(IShellItemArray * psia, DWORD i, REFIID riid, void ** ppv)
{
	*ppv = NULL;
	IShellItem * psi = NULL; // avoid error C4701
	HRESULT hr = psia ? psia->GetItemAt(i, &psi) : E_NOINTERFACE;
	if (SUCCEEDED(hr))
	{
		hr = psi->QueryInterface(riid, ppv);
		psi->Release();
	}
	return hr;
}

static TCoMemPtr<wchar_t> IsShellItemLinked(IShellItem * ShellItem)
{
	if (!ShellItem)
		return nullptr;
	
	TCoMemPtr<wchar_t> CurrShellItemFilePath;
	ShellItem->GetDisplayName(SIGDN_FILESYSPATH, CurrShellItemFilePath.GetPP());

	WIN32_FILE_ATTRIBUTE_DATA FileAttributeData = {};
	::GetFileAttributesExW(CurrShellItemFilePath.Get(), GetFileExInfoStandard, &FileAttributeData);
	return (FileAttributeData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ? Move(CurrShellItemFilePath) : nullptr;
}

template<typename CallableT>
class CDelayExcutor : public FObject
{
public:
	CDelayExcutor(CallableT && Callable_, uint32_t Elapse = 0, uint32_t MaxRetry_ = UInt32Max) : Callable(Move(Callable_)), MaxRetry(MaxRetry_)
	{
		XE::Atomics::IncFetch(DllRefCount);
		hWnd = CreateWindowExW(0L, L"STATIC", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		::SetTimer(hWnd, (UINT_PTR)this, Elapse, [](HWND, UINT, UINT_PTR EventId, DWORD)
			{
				static_cast<CDelayExcutor *>(reinterpret_cast<void*>(EventId))->Excute();
			});
	}

	~CDelayExcutor()
	{
		Atomics::DecFetch(DllRefCount);
	}

	void Excute()
	{
		if (!Callable() && ++Retry < MaxRetry)
			return;
		
		::KillTimer(hWnd, (UINT_PTR)this);
		::DestroyWindow(hWnd);

		hWnd = NULL;
		delete this;
	}
private:
	HWND hWnd = NULL;
	CallableT Callable;
	uint32_t Retry = 0;
	uint32_t MaxRetry = UInt32Max;
};

IFACEMETHODIMP CNavigateToOriginalPathCommandVerb::GetState(IShellItemArray * ShellItemArray, BOOL fOkToBeSlow, EXPCMDSTATE * pCmdState)
{
	if (!ShellItemArray)
		return E_INVALIDARG;

	DWORD ShellItemCount = 0;
	ShellItemArray->GetCount(&ShellItemCount);

	if (ShellItemCount > 1)
		return E_INVALIDARG;

	TReferPtr<IShellItem> CurrShellItem;
	HRESULT Result = GetItemAt(ShellItemArray, 0, __uuidof(IShellItem), CurrShellItem.GetVV());
	if (FAILED(Result))
		return Result;

	// This does not work for a lot of cases.
	//SFGAOF IsLink = {};
	//CurrShellItem->GetAttributes(SFGAO_LINK, &IsLink);
	//if (IsLink)
	
	*pCmdState = ECS_HIDDEN;
	while (CurrShellItem)
	{
		if (IsShellItemLinked(CurrShellItem.Get()))
		{
			*pCmdState = ECS_ENABLED;
			break;
		}
		
		TReferPtr<IShellItem> ShellItemNext;
		CurrShellItem->GetParent(ShellItemNext.GetPP());
		CurrShellItem = ShellItemNext;
	}

	return S_OK;
}

IFACEMETHODIMP CNavigateToOriginalPathCommandVerb::Invoke(IShellItemArray * ShellItemArray, IBindCtx * pbc)
{
	if (!ShellItemArray)
		return E_INVALIDARG;
	
	//TReferPtr<IStream> ShellItemArray;
	//HRESULT Result = CoMarshalInterThreadInterfaceInStream(__uuidof(psiItemArray), psiItemArray, ShellItemArray.GetPP());
	//if (FAILED(Result))
	//	return Result;

	//IShellItemArray * psia;
	//HRESULT hr = CoGetInterfaceAndReleaseStream(_pstmShellItemArray, IID_PPV_ARGS(&psia));

	//HWND hWnd = nullptr;
	//IUnknown_GetWindow(UnknownSite.Get(), &hWnd);

	DWORD ShellItemCount = 0;
	ShellItemArray->GetCount(&ShellItemCount);

	if (ShellItemCount > 1)
		return E_INVALIDARG;

	TReferPtr<IShellItem> ShellItem;
	HRESULT Result = GetItemAt(ShellItemArray, 0, __uuidof(IShellItem), ShellItem.GetVV());
	if (FAILED(Result))
		return Result;

	TCoMemPtr<ITEMIDLIST> ShellItemIdList;
	SHGetIDListFromObject(ShellItem.Get(), &ShellItemIdList);
	TReferPtr<IShellItem> CurrShellItem = ShellItem;

	TCoMemPtr<ITEMIDLIST> FinalIdList;
	while (CurrShellItem)
	{
		if (TCoMemPtr<wchar_t> ShellItemFilePath = IsShellItemLinked(CurrShellItem.Get()))
		{
			TCoMemPtr<ITEMIDLIST> LinkItemIdList;
			SHGetIDListFromObject(CurrShellItem.Get(), &LinkItemIdList);

			THandle hDirectory(CreateFileW(ShellItemFilePath.Get(), 0,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, nullptr));
			if (hDirectory == INVALID_HANDLE_VALUE)
				return E_FAIL;

			DWORD RequiredSize = ::GetFinalPathNameByHandleW(hDirectory, nullptr, 0, FILE_NAME_NORMALIZED);
			if (RequiredSize == 0)
				return E_FAIL;

			FWString FinalDirectoryPath(RequiredSize - 1);
			::GetFinalPathNameByHandleW(hDirectory, FinalDirectoryPath.Data, FinalDirectoryPath.Capacity, FILE_NAME_NORMALIZED);
			
			// "\\?\"
			TReferPtr<IShellItem> FinalDirectoryShellItem;
			SHCreateItemFromParsingName(FinalDirectoryPath.Data + 4, nullptr, __uuidof(IShellItem), FinalDirectoryShellItem.GetVVSafe());

			TCoMemPtr<ITEMIDLIST> FinalDirectoryIdList;
			SHGetIDListFromObject(FinalDirectoryShellItem.Get(), &FinalDirectoryIdList);


			ITEMIDLIST * ItemIdList = ShellItemIdList.Get();
			ITEMIDLIST * LinkIdList = LinkItemIdList.Get();
			while (!ILIsEmpty(LinkIdList))
			{
				ItemIdList = ILNext(ItemIdList);
				LinkIdList = ILNext(LinkIdList);
			}
			FinalIdList.Reset(ILCombine(FinalDirectoryIdList.Get(), ItemIdList));
			SHCreateItemFromIDList(FinalIdList.Get(), __uuidof(IShellItem), CurrShellItem.GetVVSafe());
		}
		else
		{
			TReferPtr<IShellItem> ShellItemNext;
			CurrShellItem->GetParent(ShellItemNext.GetPP());
			CurrShellItem = ShellItemNext;
		}
	}

	if (!FinalIdList)
		return S_OK;

	TReferPtr<IServiceProvider> ServiceProvider = UnknownSite.QueryInterface<IServiceProvider>();
	if (!ServiceProvider)
		return E_FAIL;

#ifdef XE_DEBUG
	wchar_t FinalShellItemPath[512];
	SHGetPathFromIDListEx(FinalIdList.Get(), FinalShellItemPath, 512, GPFIDL_DEFAULT);
	Debug::WriteLineF(L"Navigate to: {}", FinalShellItemPath);
#endif

	// Use IShellBrowser
	TReferPtr<IShellBrowser> ShellBrowser;
	if (FAILED(ServiceProvider->QueryService(SID_SInPlaceBrowser, ShellBrowser.GetPP())))
		return E_FAIL;
	
	TCoMemPtr<ITEMIDLIST> FolderIdList(ILClone(FinalIdList.Get()));
	ILRemoveLastID(FolderIdList.Get());
	
	Result = ShellBrowser->BrowseObject(FolderIdList.Get(), SBSP_DEFBROWSER);
	if (FAILED(Result))
		return E_FAIL;

	new CDelayExcutor([ShellBrowser, FinalIdList = Move(FinalIdList), FolderIdList = Move(FolderIdList)]()
		{
			//const ITEMIDLIST * ItemId = ILFindLastID(FinalIdList.Get());
			//SHOpenFolderAndSelectItems(FolderIdList.Get(), 1, &ItemId, 0);
			TReferPtr<IShellView> ShellView;
			ShellBrowser->QueryActiveShellView(ShellView.GetPP());
			if (ShellView)
			{
				TReferPtr<IFolderView> FolderView = ShellView.QueryInterface<IFolderView>();
				TReferPtr<IPersistFolder2> PersistFolder;
				FolderView->GetFolder(__uuidof(IPersistFolder2), PersistFolder.GetVV());
				if (PersistFolder)
				{
					TCoMemPtr<ITEMIDLIST> CurrIdList;
					PersistFolder->GetCurFolder(CurrIdList.GetPP());
					if (CurrIdList && ILIsEqual(CurrIdList.Get(), FolderIdList.Get()))
					{
						ShellView->SelectItem(ILFindLastID(FinalIdList.Get()), SVSI_SELECT | SVSI_ENSUREVISIBLE | SVSI_DESELECTOTHERS | SVSI_FOCUSED | SVSI_POSITIONITEM);
						return true;
					}
				}
			}
			return false;
		}, 100, 40);
	return S_OK;
}
