#pragma once

#include "IOInc.h"

namespace XE
{
	class CORE_API FPath
	{
	public:
		FPath() = default;
		FPath(const FPath &) = default;
		FPath(FStringView FilePathIn) : FilePath(FilePathIn) {}

		FString FilePath;

	public:
		static Char DirectorySeparatorChar;
		static FStringView Combine(FStringView BasePath, FStringView Path);
		static FStringView GetAbsolutePath(FStringView Path);
		static FStringView GetAbsolutePath(FStringView BasePath, FStringView Path);
		
		static FStringView GetWorkingDirectory();

		static bool IsAbsolute(FStringView Path);
		static bool Exists(FStringView Path);
		static bool IsFile(FStringView Path);
		static bool IsDirectory(FStringView Path);

		static FStringView GetDirectory(FStringView Path);
		static FStringView GetFileName(FStringView Path);
		static FStringView GetFileNameWithoutExtention(FStringView Path);
		static FStringView GetExtention(FStringView Path);

		static FStringView LexicallyNormal(FStringView Path);

		static bool Move(FStringView SourcePath, FStringView DestinationPath);
	};
}
