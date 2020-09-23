#include "PCH.h"
#include "Directory.h"

#include <filesystem>

namespace XE
{
	static TEnumerable<WIN32_FIND_DATAW> EnumPath(FWStringView PathPattern)
	{
		WIN32_FIND_DATAW FindData = {};
		HANDLE hFind = ::FindFirstFileExW(PathPattern.Data, FindExInfoBasic, &FindData, FindExSearchNameMatch, nullptr, 0);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == 0)
					continue;

				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == L'.' && FindData.cFileName[2] == 0)
					continue;
				
				co_yield FindData;
			}
			while (::FindNextFileW(hFind, &FindData));
			::FindClose(hFind);
		}
	}

	bool FDirectory::Exists(FStringView DirectoryPath)
	{
		DWORD FileAttribute = GetFileAttributesW(Strings::ToWide(DirectoryPath).Data);
		return (FileAttribute != INVALID_FILE_ATTRIBUTES) && ((FileAttribute & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	}

	bool FDirectory::Create(FStringView DirectoryPath)
	{
		FStringView DirectoryParentPath = FPath::GetDirectory(DirectoryPath);
		if (!Exists(DirectoryParentPath))
			Create(DirectoryParentPath);
		
		FWString DirectoryPathW = Strings::ToWide(DirectoryPath);
		return !!::CreateDirectoryW(DirectoryPathW.Data, NULL);
	}

	bool FDirectory::Delete(FStringView DirectoryPath)
	{
		FWStringView DirectoryPathW = Strings::ToWide(DirectoryPath);
		auto FunDelete = [](FWStringView Path, auto && Self)->bool
		{
			if (Path.IsEmpty())
				return true;

			if (Path.Last() == L'\\' || Path.Last() == L'/')
				Path = Path.Slice(0, Path.Size - 1);
			
			FWString PathPattern = Path + TView(L"\\*.*");
			for (const WIN32_FIND_DATAW & FindData : EnumPath(PathPattern))
			{
				FWString SubPath = Path + L"\\" + FindData.cFileName;
				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					bool IsFileDeleted = !!::DeleteFileW(SubPath.Data);
					if (!IsFileDeleted)
						return false;
				}
				else if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					bool IsSubFolderDeleted = Self(SubPath, Self);
					if (!IsSubFolderDeleted)
						return false;
				}
				else {}
			}
			return !!::RemoveDirectoryW(Path.Data);
		};
		
		return FunDelete(DirectoryPathW.Data, FunDelete);
	}

	bool FDirectory::HasEntries(FStringView DirectoryPath)
	{
		FWString DirectoryPathW = Strings::ToWide(FPath::Combine(DirectoryPath, Str("*.*")));
		WIN32_FIND_DATAW FindData = {};
		HANDLE hFind = ::FindFirstFileExW(DirectoryPathW.Data, FindExInfoBasic, &FindData, FindExSearchNameMatch, nullptr, 0);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == 0)
					continue;

				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == L'.' && FindData.cFileName[2] == 0)
					continue;

				if (FindData.dwFileAttributes & (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_DIRECTORY))
				{
					::FindClose(hFind);
					return true;
				}
			}
			while (::FindNextFileW(hFind, &FindData));
			::FindClose(hFind);
		}
		return false;
	}
	
	bool FDirectory::HasFiles(FStringView DirectoryPath, bool Recursively)
	{
		FWString DirectoryPathW = Strings::ToWide(FPath::Combine(DirectoryPath, Str("*.*")));
		WIN32_FIND_DATAW FindData = {};
		HANDLE hFind = ::FindFirstFileExW(DirectoryPathW.Data, FindExInfoBasic, &FindData, FindExSearchNameMatch, nullptr, 0);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == 0)
					continue;

				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == L'.' && FindData.cFileName[2] == 0)
					continue;

				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					::FindClose(hFind);
					return true;
				}

				if (Recursively && (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if (HasFiles(Strings::ToAuto(FWStringView(FindData.cFileName))))
					{
						::FindClose(hFind);
						return true;
					}
				}
			}
			while (::FindNextFileW(hFind, &FindData));
			::FindClose(hFind);
		}
		return false;
	}
	
	bool FDirectory::HasDirectories(FStringView DirectoryPath)
	{

		FWString DirectoryPathW = Strings::ToWide(FPath::Combine(DirectoryPath, Str("*.*")));
		WIN32_FIND_DATAW FindData = {};
		HANDLE hFind = ::FindFirstFileExW(DirectoryPathW.Data, FindExInfoBasic, &FindData, FindExSearchNameMatch, nullptr, 0);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == 0)
					continue;

				if (FindData.cFileName[0] == L'.' && FindData.cFileName[1] == L'.' && FindData.cFileName[2] == 0)
					continue;

				if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					::FindClose(hFind);
					return true;
				}
			}
			while (::FindNextFileW(hFind, &FindData));
			::FindClose(hFind);
		}
		return false;
	}
	
	TEnumerable<FStringView> FDirectory::GetEntries(FStringView FilePath)
	{
		FWString FilePathW = Strings::ToWide(FPath::Combine(FilePath, Str("*.*")));
		for (const WIN32_FIND_DATAW & FindData : EnumPath(FilePathW))
		{
			FString Entry;
			if (FindData.dwFileAttributes & (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_DIRECTORY))
			{
				Entry = FPath::Combine(FilePath, Strings::FromWide(FindData.cFileName));
				co_yield Entry;
			}
		}
	}
	
	TEnumerable<FStringView> FDirectory::GetFiles(FStringView FilePath)
	{
		FWString FilePathW = Strings::ToWide(FPath::Combine(FilePath, Str("*.*")));
		for (const WIN32_FIND_DATAW & FindData : EnumPath(FilePathW))
		{
			FString Entry;
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			{
				Entry = FPath::Combine(FilePath, Strings::FromWide(FindData.cFileName));
				co_yield Entry;
			}
		}
	}
	
	TEnumerable<FStringView> FDirectory::GetDirectories(FStringView FilePath)
	{
		FWString FilePathW = Strings::ToWide(FPath::Combine(FilePath, Str("*.*")));
		for (const WIN32_FIND_DATAW & FindData : EnumPath(FilePathW))
		{
			FString Entry;
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				Entry = FPath::Combine(FilePath, Strings::FromWide(FindData.cFileName));
				co_yield Entry;
			}
		}
	}
	
	bool FDirectory::Move(FStringView SourcePath, FStringView DestinationPath)
	{
		FWStringView SourcePathW = Strings::ToWide(SourcePath);
		FWStringView DestinationPathW = Strings::ToWide(DestinationPath);
		return !!::MoveFileW(SourcePathW.Data, DestinationPathW.Data);
	}
}
