#include "PCH.h"
#include "File.h"
#include "../Platform/Win32/Win32.h"

namespace XE
{
	FFile::FFile(FStringView FilePath)
	{
		FWStringView FilePathW = Strings::ToWide(FilePath);
		HANDLE hFile = CreateFileW(FilePathW.Data, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			Handle = hFile;
	}

	FFile::~FFile()
	{
		if (Handle)
		{
			CloseHandle(Handle);
			Handle = nullptr;
		}
	}

	size_t FFile::GetSize() const
	{
		if (!Handle)
			return 0;
		
		LARGE_INTEGER FileSize;
		BOOL bGetFileSizeEx = GetFileSizeEx(Handle, &FileSize);
		assert(bGetFileSizeEx);
		return size_t(FileSize.QuadPart);
	}

	bool FFile::Exists(FStringView FilePath)
	{
		DWORD FileAttribute = GetFileAttributesW(Strings::ToWide(FilePath).Data);
		return (FileAttribute != INVALID_FILE_ATTRIBUTES) && ((FileAttribute & FILE_ATTRIBUTE_ARCHIVE) == FILE_ATTRIBUTE_ARCHIVE);
	}

	bool FFile::Delete(FStringView FilePath)
	{
		FWStringView FilePathW = Strings::ToWide(FilePath);
		return !!::DeleteFileW(FilePathW.Data);
	}
	bool FFile::Move(FStringView SourcePath, FStringView DestinationPath)
	{
		FWStringView SourcePathW = Strings::ToWide(SourcePath);
		FWStringView DestinationPathW = Strings::ToWide(DestinationPath);
		return !!::MoveFileW(SourcePathW.Data, DestinationPathW.Data);
	}
	
	FString FFile::ReadToString(FStringView FilePath, EFileBOM * FileBOM)
	{
		FWStringView FilePathW = Strings::ToWide(FilePath);
		HANDLE hFile = CreateFileW(FilePathW.Data, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FString();

		LARGE_INTEGER FileSize;
		BOOL bGetFileSizeEx = GetFileSizeEx(hFile, &FileSize);
		assert(bGetFileSizeEx);

		if (FileSize.QuadPart > UInt32Max)
		{
			CloseHandle(hFile);
			return FString();
		}

		// BOM
		uint32_t BOMSize = 0;
		{
			uint8_t BOM[3] = {};
			DWORD NumberOfBytesReadBOM = 0;
			BOOL bReadFileBOM = ReadFile(hFile, BOM, 3, &NumberOfBytesReadBOM, NULL);
			if (!bReadFileBOM)
			{
				CloseHandle(hFile);
				return FString();
			}

			// UTF16BE
			if (NumberOfBytesReadBOM >= 2 && BOM[0] == 0xFE && BOM[1] == 0xFF)
			{
				if (FileBOM) *FileBOM = EFileBOM::UTF16BE;
				BOMSize = 2;
			}
			// UTF16LE
			else if (NumberOfBytesReadBOM >= 2 && BOM[0] == 0xFF && BOM[1] == 0xFE)
			{
				if (FileBOM) *FileBOM = EFileBOM::UTF16LE;
				BOMSize = 2;
			}
			// UTF8
			else if (NumberOfBytesReadBOM == 3 && BOM[0] == 0xEF && BOM[1] == 0xBB && BOM[2] == 0xBF)
			{
				if (FileBOM) *FileBOM = EFileBOM::UTF8;
				BOMSize = 3;
			}
			// ANSI
			else
			{
				if (FileBOM) *FileBOM = EFileBOM::None;
				BOMSize = 0;
			}
		}

		SetFilePointer(hFile, BOMSize, 0, FILE_BEGIN);
		FString String(FileSize.LowPart - BOMSize);
		DWORD NumberOfBytesRead = 0;
		BOOL bReadFile = ReadFile(hFile, String.GetData(), FileSize.LowPart - BOMSize, &NumberOfBytesRead, NULL);
		CloseHandle(hFile);

		if (!bReadFile || NumberOfBytesRead != FileSize.LowPart - BOMSize)
			return FString();
		return String;
	}

	bool FFile::SaveToFile(FStringView FilePath, FStringView String)
	{
		FWStringView FilePathW = Strings::ToWide(FilePath);
		HANDLE hFile = CreateFileW(FilePathW.Data, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		DWORD NumberOfBytesWriten = 0;
		[[maybe_unused]] BOOL bReadFile = WriteFile(hFile, String.GetData(), DWORD(String.GetSize()) * sizeof(Char), &NumberOfBytesWriten, NULL);
		CloseHandle(hFile);
		
		return NumberOfBytesWriten == String.GetSize() * sizeof(Char);
	}

	TVector<byte_t> FFile::ReadToBytes(FStringView FilePath)
	{
		FWStringView FilePathW = Strings::ToWide(FilePath);
		HANDLE hFile = CreateFileW(FilePathW.Data, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return TVector<byte_t>();

		LARGE_INTEGER FileSize;
		BOOL bGetFileSizeEx = GetFileSizeEx(hFile, &FileSize);
		assert(bGetFileSizeEx);

		if (FileSize.QuadPart > UInt32Max)
		{
			CloseHandle(hFile);
			return TVector<byte_t>();
		}

		TVector<byte_t> Vector(FileSize.LowPart, FileSize.LowPart, EInitializeMode::None);

		DWORD NumberOfBytesRead = 0;
		BOOL bReadFile = ReadFile(hFile, Vector.GetData(), FileSize.LowPart, &NumberOfBytesRead, NULL);
		CloseHandle(hFile);
		
		if (!bReadFile || NumberOfBytesRead != FileSize.LowPart)
			return TVector<byte_t>();
		return Vector;
	}
}