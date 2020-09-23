#pragma once

namespace XE::Win32
{
	enum EWin32Resource
	{
		// "FILE"
		File = 0,
		Count
	};

	constexpr wchar_t WINDOW_CLASS_NAME[] = L"[Default:29a0f390-f5ab-4617-9ac0-c41fe276fb64]";
	constexpr wchar_t WINDOW_PROP_THIS_PTR[] = L"C8C8BD2D-46A7-4DFB-BB5D-EE6A25E83368";
	constexpr wchar_t WINDOW_PROP_DLG_RESULT[] = L"99358F8F-E19F-4337-8192-A088F1CA41D1";
	constexpr wchar_t WINDOW_PROP_OLD_WNDPROC[] = L"FCAC0730-350D-4D79-B28E-D137C22648EA";

	CORE_API FStringView ErrorStr(int32_t err);
	CORE_API FStringView ResultStr(HRESULT Result);

	std::tuple<std::shared_ptr<byte_t>, int32_t> LoadResource(uint32_t id, EWin32Resource type = File);

	struct winversion_t
	{
		uint32_t major;
		uint32_t minor;
		uint32_t pack;
		uint32_t build;

		int32_t diff(const winversion_t & another) const
		{
			int32_t ret = 0;
			if (major > another.major) ret += 1000;
			else if (major < another.major) ret -= 1000;
			else {}
			if (minor > another.minor) ret += 100;
			else if (minor < another.minor) ret -= 100;
			else {}
			if (pack > another.pack) ret += 10;
			else if (pack < another.pack) ret -= 10;
			else {}
			if (build > another.build) ret += 1;
			else if (build < another.build) ret -= 1;
			else {}
			return ret;
		}
		bool operator > (const winversion_t & another) const { return diff(another) > 0; }
		bool operator < (const winversion_t & another) const { return diff(another) < 0; }
		bool operator >= (const winversion_t & another) const { return diff(another) >= 0; }
		bool operator <= (const winversion_t & another) const { return diff(another) <= 0; }
		bool operator == (const winversion_t & another) const { return diff(another) == 0; }
		bool operator != (const winversion_t & another) const { return diff(another) != 0; }
	};

	constexpr winversion_t winversion_10 = { 10, 0, 0, 0 };
	constexpr winversion_t winversion_8_1 = { 6, 3, 0, 0 };
	constexpr winversion_t winversion_8 = { 6, 2, 0, 0 };
	constexpr winversion_t winversion_7 = { 6, 1, 0, 0 };
	constexpr winversion_t winversion_vista = { 6, 0, 0, 0 };
	constexpr winversion_t winversion_server_2003 = { 5, 2, 0, 0 };
	constexpr winversion_t winversion_xp_sp3 = { 5, 1, 3, 0 };
	constexpr winversion_t winversion_xp_sp2 = { 5, 1, 2, 0 };
	constexpr winversion_t winversion_xp_sp1 = { 5, 1, 1, 0 };
	constexpr winversion_t winversion_xp = { 5, 1, 0, 0 };
	constexpr winversion_t winversion_2000 = { 5, 0, 0, 0 };
	winversion_t version();
	std::u8string version_str(const winversion_t & ver);

	inline bool Succeeded(HRESULT Result)
	{
		return SUCCEEDED(Result);
	}

	inline bool Failed(HRESULT Result)
	{
		return FAILED(Result);
	}

	inline std::string hr_str(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}

	class hr_error : public std::runtime_error
	{
	public:
		hr_error(HRESULT hr) : std::runtime_error(hr_str(hr)), _hr(hr) {}
		HRESULT error() const { return _hr; }
	private:
		const HRESULT _hr;
	};

	inline void throw_if_failed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw hr_error(hr);
		}
	}

#if 0
	inline std::u8ostream & operator << (std::u8ostream & ost, const winversion_t & ver)
	{
		return ost << "Windows " << version_str(ver) << " [" << ver.major << "." << ver.minor << "." << ver.pack << "." << ver.build << "]";
	}
	std::u8string GUID2String(const GUID & guid);
	std::u8string GUID2String(std::array<uint8_t, 16> data);

	struct winrect_t : public RECT
	{
		winrect_t() {}
		winrect_t(int32_t x, int32_t y, int32_t w, int32_t h)
		{
			left = (int32_t)x; top = (int32_t)y;
			right = (int32_t)(x + w); bottom = (int32_t)(y + h);
		}
		winrect_t(const core::recti & rect)
		{
			left = (int32_t)rect.x; top = (int32_t)rect.y;
			right = (int32_t)rect.right(); bottom = (int32_t)rect.bottom();
		}

		int32_t width() const { return right - left; }
		int32_t height() const { return bottom - top; }
	};

	struct guid
	{
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t  Data4[8];
	};
#endif
}
