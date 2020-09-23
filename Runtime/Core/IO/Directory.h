#pragma once

#include "IOInc.h"
#include "Path.h"

namespace XE
{
	class CORE_API FDirectory
	{
	public:


	public:
		static bool Exists(FStringView DirectoryPath);
		static bool Create(FStringView DirectoryPath);
		static bool Delete(FStringView DirectoryPath);
		static bool HasEntries(FStringView DirectoryPath);
		static bool HasFiles(FStringView DirectoryPath, bool Recursively = false);
		static bool HasDirectories(FStringView DirectoryPath);
		
		static TEnumerable<FStringView> GetEntries(FStringView FilePath);
		static TEnumerable<FStringView> GetFiles(FStringView FilePath);
		static TEnumerable<FStringView> GetDirectories(FStringView FilePath);
		static bool Move(FStringView SourcePath, FStringView DestinationPath);
	};
}
