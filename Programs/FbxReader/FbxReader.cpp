#include "stdafx.h"

#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")

#include "fbx/Importer.h"

namespace fbx
{
	void log_inf(const char * str)
	{
		puts(str);
	}
	
	void log_war(const char * str)
	{
		puts(str);
	}
}
int _tmain(int argc, const char args[])
{
	fbx::Importer Importer;
	Importer.LoadFromFile("E:\\files\\models\\33-fbx\\fbx\\Wolf_with_Animations2.fbx");
 	return 0;
}
