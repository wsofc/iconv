/*********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************
Author:		Aries
Date:		2018/05/05
Class:		字符转码类
Detail:		宏 USING_STL_CODECVT ,为0时表示使用iconv库进行转码,为1时表示使用C++11新特性stl自带接口转码
iconv库可跨平台使用，但是需要自行编译对应平台库(ios自带该库,直接引用即可,android需要自行编译成,window也需要自行编译,
虽然cocos2d自带了iconv库,但是版本太低(v1.09),无法支持wchar_t转char,所以必须编译高版本,目前使用的iconv最新版本为v1.15.
C++11 stl转码是stl提供的接口,但是目前cocos2d版本虽然支持C++11,但是该版本对应的NDK版本(r9d,r10e)却不支持,导致android
上无法编译通过,必须要使用更高版本的NDK(r12及以上)编译,因此暂时不得不放弃使用该方法,改用iconv跨平台转码.
因此 USING_STL_CODECVT 默认为 0

注: iconv库中与wstring转换相关的接口只支持pc平台,ios和android中string编码默认为utf8,直接可以正则中文;pc的话要正则中文
必须要先转换为wstring,否则无法正常正则中文
**********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************/

#ifndef __STRINGCONVERT_H__
#define __STRINGCONVERT_H__

// 1:采用C++11中STL新特性转码接口(高版本ndk才支持,所以这里默认关闭)
// 0:采用iconv转码(需要自行编译iconv库)
#define USING_STL_CODECVT 0

#include "SEEngine.h"
#include <regex>

#if USING_STL_CODECVT
#include <codecvt>
#else
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "../cocos2d/external/win32-specific/icon/include/iconv.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "../libs/iconv/android/src/include/iconv.h"
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include <iconv.h>
#endif
#endif

class StringConvert : public SESingleton < StringConvert >
{
public:
	StringConvert();
	virtual ~StringConvert();

	// 内存大小端存储模式
	enum eBIGSMALLSTORAGE
	{
		BES_NONE = 0,		// 无
		BER_BIG,			// 大端
		BES_SMALL,			// 小端
		BES_AUTO			// 自动检测内存模式
	};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::wstring Utf8TOWstring(const std::string &str);						// utf8 转 unicode
	std::string WstringToUtf8(const std::wstring &str);						// unicode 转 utf8
	std::wstring GbkToWstring(const std::string &str);						// gbk 转 unicode
	std::string WstringToGbk(const std::wstring &str);						// unicode 转 gbk
	std::string WstringToAscii(const std::wstring& str);					// unicode 转 ascii
	std::wstring AsciiToWstring(const std::string& str);					// ascii 转 unicode
#endif

	std::string Utf8ToGbk(const std::string &str);							// utf8 转 gbk
	std::string GbkToUtf8(const std::string &str);							// gbk 转 utf8
	std::string Utf8ToAscii(const std::string &str);						// utf8 转 ascii
	std::string AsciiToUtf8(const std::string &str);						// ascii 转 utf8

	bool CheckBigSmallStorage();											// 检测内存大小端存储模式(返回true-大端,false-小端)
	bool MatchRegular(std::string& strIn, std::string strReg);				// 多字字符集 正则匹配
	bool MatchRegular(std::wstring& wstrIn, std::wstring wstrReg);			// unicode 正则匹配

private:

#if USING_STL_CODECVT
	// C++11 STL 版
	std::wstring _codecvt_utf8_to_wstring(const std::string &str);
	std::string _codecvt_wstring_to_utf8(const std::wstring &str);
	std::wstring _codecvt_gbk_to_wstring(const std::string &str);
	std::string _codecvt_wstring_to_gbk(const std::wstring &str);
	std::string _codecvt_utf8_to_gbk(const std::string &str);
	std::string _codecvt_gbk_to_utf8(const std::string &str);
	std::string _codecvt_wstring_to_ascii(const std::wstring& str);
	std::wstring _codecvt_ascii_to_wstring(const std::string& str);
#else
	// iconv跨平台版本(需要自行编译生成libiconv.so库)
	std::string _iconv_utf8_to_gbk(const std::string &str);
	std::string _iconv_gbk_to_utf8(const std::string &str);
	std::string _iconv_utf8_to_ascii(const std::string &str);
	std::string _iconv_ascii_to_utf8(const std::string &str);
	std::string _iconv_utf16_to_utf8(const std::wstring &str, eBIGSMALLSTORAGE ebm);
	std::wstring _iconv_utf8_to_utf16(const std::string &str, eBIGSMALLSTORAGE ebm);
	int _iconv_convert(const char* from, const char* to, char* inptr, size_t insize, char* outptr, size_t outsize);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::wstring _iconv_utf8_to_wstring(const std::string &str);
	std::string _iconv_wstring_to_utf8(const std::wstring &str);
	std::wstring _iconv_gbk_to_wstring(const std::string &str);
	std::string _iconv_wstring_to_gbk(const std::wstring &str);
	std::string _iconv_wstring_to_ascii(const std::wstring& str);
	std::wstring _iconv_ascii_to_wstring(const std::string& str);
#endif

#endif
};

#define g_pStringConvert StringConvert::GetSingletonPtr()

#endif  //__STRINGCONVERT_H__