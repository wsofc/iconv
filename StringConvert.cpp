#include "StringConvert.h"

StringConvert::StringConvert()
{

}

StringConvert::~StringConvert()
{

}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

std::wstring StringConvert::Utf8TOWstring(const std::string &str)
{
#if USING_STL_CODECVT
	return _codecvt_utf8_to_wstring(str);
#else
	return _iconv_utf8_to_wstring(str);
#endif
}

std::string StringConvert::WstringToUtf8(const std::wstring &str)
{
#if USING_STL_CODECVT
	return _codecvt_wstring_to_utf8(str);
#else
	return _iconv_wstring_to_utf8(str);
#endif
}

std::wstring StringConvert::GbkToWstring(const std::string &str)
{
#if USING_STL_CODECVT
	return _codecvt_gbk_to_wstring(str);
#else
	return _iconv_gbk_to_wstring(str);
#endif
}

std::string StringConvert::WstringToGbk(const std::wstring &str)
{
#if USING_STL_CODECVT
	return _codecvt_wstring_to_gbk(str);
#else
	return _iconv_wstring_to_gbk(str);
#endif
}

std::string StringConvert::WstringToAscii(const std::wstring& str)
{
#if USING_STL_CODECVT
	return _codecvt_wstring_to_ascii(str);
#else
	return _iconv_wstring_to_ascii(str);
#endif
}

std::wstring StringConvert::AsciiToWstring(const std::string& str)
{
#if USING_STL_CODECVT
	return _codecvt_ascii_to_wstring(str);
#else
	return _iconv_ascii_to_wstring(str);
#endif
}

#endif

std::string StringConvert::Utf8ToGbk(const std::string &str)
{
#if USING_STL_CODECVT
	return _codecvt_utf8_to_gbk(str);
#else
	return _iconv_utf8_to_gbk(str);
#endif
}

std::string StringConvert::GbkToUtf8(const std::string &str)
{
#if USING_STL_CODECVT
	return _codecvt_gbk_to_utf8(str);
#else
	return _iconv_gbk_to_utf8(str);
#endif
}

std::string StringConvert::Utf8ToAscii(const std::string &str)
{
#if USING_STL_CODECVT
	std::wstring wstr = _codecvt_utf8_to_wstring(str);
	return _codecvt_wstring_to_ascii(wstr);
#else
	return _iconv_utf8_to_ascii(str);
#endif
}

std::string StringConvert::AsciiToUtf8(const std::string &str)
{
#if USING_STL_CODECVT
	std::wstring wstr = _codecvt_ascii_to_wstring(str);
	return _codecvt_wstring_to_utf8(wstr);
#else
	return _iconv_ascii_to_utf8(str);
#endif
}

bool StringConvert::CheckBigSmallStorage()
{
	int i = 0x12345678;
	char *c = (char *)&i;
	return(*c == 0x12);
}

bool StringConvert::MatchRegular(std::string& strIn, std::string strReg)
{
	std::regex pattern(strReg);
	return std::regex_match(strIn, pattern);
}

bool StringConvert::MatchRegular(std::wstring& wstrIn, std::wstring wstrReg)
{
	std::wregex wpattern(wstrReg);
	return std::regex_match(wstrIn, wpattern);
}

#if USING_STL_CODECVT

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// C++11 STL 版
std::wstring StringConvert::_codecvt_utf8_to_wstring(const std::string &str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>>utf8_cvt;
	return utf8_cvt.from_bytes(str);
}

std::string StringConvert::_codecvt_wstring_to_utf8(const std::wstring &str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_cvt;
	return utf8_cvt.to_bytes(str);
}

std::wstring StringConvert::_codecvt_gbk_to_wstring(const std::string &str)
{
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> gbk_cvt(new std::codecvt<wchar_t, char, std::mbstate_t>("chs"));
	return gbk_cvt.from_bytes(str);
}

std::string StringConvert::_codecvt_wstring_to_gbk(const std::wstring &str)
{
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> gbk_cvt(new std::codecvt<wchar_t, char, std::mbstate_t>("chs"));
	return gbk_cvt.to_bytes(str);
}

std::string StringConvert::_codecvt_utf8_to_gbk(const std::string &str)
{
	std::wstring wstr = this->Utf8TOWstring(str);
	return this->WstringToGbk(wstr);
}

std::string StringConvert::_codecvt_gbk_to_utf8(const std::string &str)
{
	std::wstring wstr = this->GbkToWstring(str);
	return this->WstringToUtf8(wstr);
}

std::string StringConvert::_codecvt_wstring_to_ascii(const std::wstring& str)
{
	std::string dst("");
	std::locale sys_locale("");

	const wchar_t* data_from = str.c_str();
	const wchar_t* data_from_end = str.c_str() + str.size();
	const wchar_t* data_from_next = 0;

	int wchar_size = 4;
	char* data_to = new char[(str.size() + 1) * wchar_size];
	char* data_to_end = data_to + (str.size() + 1) * wchar_size;
	char* data_to_next = 0;

	::memset(data_to, 0, (str.size() + 1) * wchar_size);

	typedef std::codecvt<wchar_t, char, std::mbstate_t> convert_facet;
	std::mbstate_t out_state = 0;
	auto result = std::use_facet<convert_facet>(sys_locale).out(
		out_state, data_from, data_from_end, data_from_next,
		data_to, data_to_end, data_to_next);
	if (result == convert_facet::ok)
	{
		dst = data_to;
	}
	else
	{
		assert(false, "StringConvert wstring convert to string error!\n");
	}

	delete[] data_to;
	return dst;
}

std::wstring StringConvert::_codecvt_ascii_to_wstring(const std::string& str)
{
	std::wstring dst(L"");
	std::locale sys_locale("");

	const char* data_from = str.c_str();
	const char* data_from_end = str.c_str() + str.size();
	const char* data_from_next = 0;

	wchar_t* data_to = new wchar_t[str.size() + 1];
	wchar_t* data_to_end = data_to + str.size() + 1;
	wchar_t* data_to_next = 0;

	::wmemset(data_to, 0, str.size() + 1);

	typedef std::codecvt<wchar_t, char, std::mbstate_t> convert_facet;
	std::mbstate_t in_state = 0;
	auto result = std::use_facet<convert_facet>(sys_locale).in(
		in_state, data_from, data_from_end, data_from_next,
		data_to, data_to_end, data_to_next);
	if (result == convert_facet::ok)
	{
		dst = data_to;
	}
	else
	{
		assert(false, "StringConvert string convert to wstring error!\n");
	}

	delete[] data_to;
	return dst;
}

#else

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// iconv跨平台版本(需要自行编译生成libiconv.so库)

std::string StringConvert::_iconv_utf8_to_gbk(const std::string &str)
{
	int inlen = str.length() + 1;
	int outlen = 4 * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("utf-8", "gb2312", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_gbk_to_utf8(const std::string &str)
{
	int inlen = str.length() + 1;
	int outlen = 4 * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("gb2312", "utf-8", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_utf8_to_ascii(const std::string &str)
{
	int inlen = str.length() + 1;
	int outlen = 4 * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("utf-8", "ascii", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_ascii_to_utf8(const std::string &str)
{
	int inlen = str.length() + 1;
	int outlen = 4 * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("ascii", "utf-8", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_utf16_to_utf8(const std::wstring &str, eBIGSMALLSTORAGE ebm)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(wchar_t) * inlen;
	size_t insize = sizeof(wchar_t) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	const char* chekc_code = "utf-16";
	switch (ebm)
	{
		case StringConvert::BES_NONE:
			break;

		case StringConvert::BER_BIG:
		{
			chekc_code = "utf-16be";
		}break;

		case StringConvert::BES_SMALL:
		{
			chekc_code = "utf-16le";
		}break;

		case StringConvert::BES_AUTO:
		{
			chekc_code = this->CheckBigSmallStorage() ? "utf-16be" : "utf-16le";
		}break;

		default:
			break;
	}

	int ires = this->_iconv_convert(chekc_code, "utf-8", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::wstring StringConvert::_iconv_utf8_to_utf16(const std::string &str, eBIGSMALLSTORAGE ebm)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(char) * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(wchar_t) * outlen;

	wchar_t* outstr = new wchar_t[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	const char* chekc_code = "utf-16";
	switch (ebm)
	{
		case StringConvert::BES_NONE:
			break;

		case StringConvert::BER_BIG:
		{
			chekc_code = "utf-16be";
		}break;

		case StringConvert::BES_SMALL:
		{
			chekc_code = "utf-16le";
		}break;

		case StringConvert::BES_AUTO:
		{
			chekc_code = this->CheckBigSmallStorage() ? "utf-16be" : "utf-16le";
		}break;

		default:
			break;
	}

	int ires = this->_iconv_convert("utf-8", chekc_code, inptr, insize, outptr, outsize);

	std::wstring outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

/*
	@brief 对字符串进行语言编码转换
	@param from		原始编码(比如"GB2312",的按照iconv支持的写)
	@param to		转换后的编码
	@param inptr    原始需要转换的地址
	@param insize	原始需要转换的内存大小
	@param outptr	转换后的数据存储地址(需要在外部分配内存)
	@param outsize	转换后的数据内存大小
	*/
int StringConvert::_iconv_convert(const char* from, const char* to, char* inptr, size_t insize, char* outptr, size_t outsize)
{
	char *inptr_tmp = inptr;
	char *outptr_tmp = outptr;

	size_t insize_tmp = insize;
	size_t outsize_tmp = outsize;

	int ires = 0;
	if (insize_tmp == 0 || outsize_tmp == 0)
	{
		goto done;
	}

	iconv_t env;
	env = iconv_open(to, from);
	if ((iconv_t)-1 == env)
	{
		goto done;
	}

	if ((size_t)-1 == iconv(env, (char**)&inptr_tmp, (size_t*)&insize_tmp, (char**)&outptr_tmp, (size_t*)&outsize_tmp))
	{
		goto done;
	}

	ires = strlen(outptr);

done:
	iconv_close(env);
	return ires;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

std::wstring StringConvert::_iconv_utf8_to_wstring(const std::string &str)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(char) * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(wchar_t) * outlen;

	wchar_t* outstr = new wchar_t[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("utf-8", "wchar_t", inptr, insize, outptr, outsize);

	std::wstring outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_wstring_to_utf8(const std::wstring &str)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(wchar_t) * inlen;
	size_t insize = sizeof(wchar_t) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("wchar_t", "utf-8", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::wstring StringConvert::_iconv_gbk_to_wstring(const std::string &str)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(char) * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(wchar_t) * outlen;

	wchar_t* outstr = new wchar_t[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("gb2312", "wchar_t", inptr, insize, outptr, outsize);

	std::wstring outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_wstring_to_gbk(const std::wstring &str)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(wchar_t) * inlen;
	size_t insize = sizeof(wchar_t) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("wchar_t", "gb2312", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::string StringConvert::_iconv_wstring_to_ascii(const std::wstring& str)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(wchar_t) * inlen;
	size_t insize = sizeof(wchar_t) * inlen;
	size_t outsize = sizeof(char) * outlen;

	char* outstr = new char[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("wchar_t", "char", inptr, insize, outptr, outsize);

	std::string outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

std::wstring StringConvert::_iconv_ascii_to_wstring(const std::string& str)
{
	int inlen = str.length() + 1;
	int outlen = sizeof(char) * inlen;
	size_t insize = sizeof(char) * inlen;
	size_t outsize = sizeof(wchar_t) * outlen;

	wchar_t* outstr = new wchar_t[outlen];
	memset(outstr, 0, outsize);

	char* inptr = (char*)str.c_str();
	char* outptr = (char*)outstr;

	int ires = this->_iconv_convert("char", "wchar_t", inptr, insize, outptr, outsize);

	std::wstring outtmp = outstr;
	delete[] outstr;

	return outtmp;
}

#endif

#endif