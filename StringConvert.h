/*********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************
Author:		Aries
Date:		2018/05/05
Class:		�ַ�ת����
Detail:		�� USING_STL_CODECVT ,Ϊ0ʱ��ʾʹ��iconv�����ת��,Ϊ1ʱ��ʾʹ��C++11������stl�Դ��ӿ�ת��
iconv��ɿ�ƽ̨ʹ�ã�������Ҫ���б����Ӧƽ̨��(ios�Դ��ÿ�,ֱ�����ü���,android��Ҫ���б����,windowҲ��Ҫ���б���,
��Ȼcocos2d�Դ���iconv��,���ǰ汾̫��(v1.09),�޷�֧��wchar_tתchar,���Ա������߰汾,Ŀǰʹ�õ�iconv���°汾Ϊv1.15.
C++11 stlת����stl�ṩ�Ľӿ�,����Ŀǰcocos2d�汾��Ȼ֧��C++11,���Ǹð汾��Ӧ��NDK�汾(r9d,r10e)ȴ��֧��,����android
���޷�����ͨ��,����Ҫʹ�ø��߰汾��NDK(r12������)����,�����ʱ���ò�����ʹ�ø÷���,����iconv��ƽ̨ת��.
��� USING_STL_CODECVT Ĭ��Ϊ 0

ע: iconv������wstringת����صĽӿ�ֻ֧��pcƽ̨,ios��android��string����Ĭ��Ϊutf8,ֱ�ӿ�����������;pc�Ļ�Ҫ��������
����Ҫ��ת��Ϊwstring,�����޷�������������
**********************************************************************************************************************
**********************************************************************************************************************
**********************************************************************************************************************/

#ifndef __STRINGCONVERT_H__
#define __STRINGCONVERT_H__

// 1:����C++11��STL������ת��ӿ�(�߰汾ndk��֧��,��������Ĭ�Ϲر�)
// 0:����iconvת��(��Ҫ���б���iconv��)
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

	// �ڴ��С�˴洢ģʽ
	enum eBIGSMALLSTORAGE
	{
		BES_NONE = 0,		// ��
		BER_BIG,			// ���
		BES_SMALL,			// С��
		BES_AUTO			// �Զ�����ڴ�ģʽ
	};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::wstring Utf8TOWstring(const std::string &str);						// utf8 ת unicode
	std::string WstringToUtf8(const std::wstring &str);						// unicode ת utf8
	std::wstring GbkToWstring(const std::string &str);						// gbk ת unicode
	std::string WstringToGbk(const std::wstring &str);						// unicode ת gbk
	std::string WstringToAscii(const std::wstring& str);					// unicode ת ascii
	std::wstring AsciiToWstring(const std::string& str);					// ascii ת unicode
#endif

	std::string Utf8ToGbk(const std::string &str);							// utf8 ת gbk
	std::string GbkToUtf8(const std::string &str);							// gbk ת utf8
	std::string Utf8ToAscii(const std::string &str);						// utf8 ת ascii
	std::string AsciiToUtf8(const std::string &str);						// ascii ת utf8

	bool CheckBigSmallStorage();											// ����ڴ��С�˴洢ģʽ(����true-���,false-С��)
	bool MatchRegular(std::string& strIn, std::string strReg);				// �����ַ��� ����ƥ��
	bool MatchRegular(std::wstring& wstrIn, std::wstring wstrReg);			// unicode ����ƥ��

private:

#if USING_STL_CODECVT
	// C++11 STL ��
	std::wstring _codecvt_utf8_to_wstring(const std::string &str);
	std::string _codecvt_wstring_to_utf8(const std::wstring &str);
	std::wstring _codecvt_gbk_to_wstring(const std::string &str);
	std::string _codecvt_wstring_to_gbk(const std::wstring &str);
	std::string _codecvt_utf8_to_gbk(const std::string &str);
	std::string _codecvt_gbk_to_utf8(const std::string &str);
	std::string _codecvt_wstring_to_ascii(const std::wstring& str);
	std::wstring _codecvt_ascii_to_wstring(const std::string& str);
#else
	// iconv��ƽ̨�汾(��Ҫ���б�������libiconv.so��)
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