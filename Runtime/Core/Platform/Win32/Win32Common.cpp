#include "PCH.h"
#include "Win32.h"
#include <shlobj.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace XE::Win32
{
	FStringView ErrorStr(int32_t err)
	{
		TString<wchar_t> & Buffer = GetThreadLocalStringBuffer<wchar_t>();
		int Size = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Buffer.GetData(), 512, NULL);
		if (!Size)
			return Strings::Printf(Str("Error %d"), err);

		Buffer.Resize(Size - 2);
		return Strings::FromWide(Buffer);
	}

	FStringView ResultStr(HRESULT Result)
	{
		if (High(Result) == FACILITY_WIN32)
			return ErrorStr(Low(Result));

		return Strings::Printf(Str("HRESULT %x"), Result);
	}
	
#if 0
	std::u8string GUID2String(const GUID & guid)
	{
		std::u8string output(40, 0);
		snprintf(reinterpret_cast<char *>(output.data()), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
			guid.Data4[7]);
		return output;
	}

	std::u8string GUID2String(std::array<uint8_t, 16> data)
	{
		const GUID & guid = *(const GUID *)data.data();
		std::u8string output(40, 0);
		snprintf(reinterpret_cast<char *>(output.data()), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
			guid.Data4[7]);
		return output;
	}


	std::tuple<std::shared_ptr<byte_t>, int32_t> LoadResource(uint32_t id, res_e type)
	{
		static const char RES_TYPES[res_count][16] = { "FILE" };

		int index = std::clamp((int)type, 0, res_count - 1);
		HRSRC hRes = FindResourceA(NULL, (const char *)(uintmax_t)id, RES_TYPES[index]);
		if (hRes)
		{
			HGLOBAL hData = LoadResource(NULL, hRes);
			if (hData)
			{
				byte_t * ptr = (byte_t *)LockResource(hData);
				uint32_t nbRes = SizeofResource(NULL, hRes);

				std::shared_ptr<byte_t> data(ptr, [hData](byte_t * ptr)
					{
						UnlockResource(hData);
						FreeResource(hData);
					});
				return { data, (int32_t)nbRes };
			}
		}
		return {};
	}

	winversion_t version()
	{
		static winversion_t version = {};
		if (version.major == 0)
		{
			typedef LONG(WINAPI * RtlGetVersionFuncT)(RTL_OSVERSIONINFOEXW *);
			typedef void (WINAPI * NtVersionNumbersFuncT)(DWORD *, DWORD *, DWORD *);

			RTL_OSVERSIONINFOEXW rtlOSVersion = { sizeof(RTL_OSVERSIONINFOEXW) };
			RtlGetVersionFuncT pfnRtlGetVersion = (RtlGetVersionFuncT)(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
			if (pfnRtlGetVersion)
			{
				pfnRtlGetVersion(&rtlOSVersion);
			}

			if (rtlOSVersion.dwMajorVersion > 5)
			{
				version.major = rtlOSVersion.dwMajorVersion;
				version.minor = rtlOSVersion.dwMinorVersion;
				version.pack = rtlOSVersion.wServicePackMajor;
				version.build = rtlOSVersion.dwBuildNumber;
			}
			else
			{
				NtVersionNumbersFuncT pfnRtlGetNtVersionNumbers = (NtVersionNumbersFuncT)(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
				if (pfnRtlGetNtVersionNumbers)
				{
					DWORD dwMajor = 0;
					DWORD dwMinor = 0;
					DWORD dwBuild = 0;
					pfnRtlGetNtVersionNumbers(&dwMajor, &dwMinor, &dwBuild);
					dwBuild &= 0xffff;
					version.major = dwMajor;
					version.minor = dwMinor;
					version.pack = 0;
					version.build = dwBuild;
				}
				else
				{
					version.major = rtlOSVersion.dwMajorVersion;
					version.minor = rtlOSVersion.dwMinorVersion;
					version.pack = rtlOSVersion.dwMajorVersion;
					version.build = rtlOSVersion.dwBuildNumber;
				}
			}
		}

		return version;
	}

	std::u8string version_str(const winversion_t & ver)
	{
		if (ver >= winversion_10)
			return u8"10";
		else if (ver >= winversion_8_1)
			return u8"8.1";
		if (ver >= winversion_8)
			return u8"8";
		if (ver >= winversion_7)
			return u8"7";
		if (ver >= winversion_vista)
			return u8"Vista";
		if (ver >= winversion_server_2003)
			return u8"Server 2003";
		if (ver >= winversion_xp_sp3)
			return u8"XP SP3";
		if (ver >= winversion_xp_sp2)
			return u8"XP SP2";
		if (ver >= winversion_xp_sp1)
			return u8"XP SP1";
		if (ver >= winversion_xp)
			return u8"XP";
		if (ver >= winversion_2000)
			return u8"2000";
		return u8"Unknown";
	}
#endif
}
