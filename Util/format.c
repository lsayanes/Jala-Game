

#include <Windows.h>
#include <stdlib.h>

#if defined(LINUX)
#include <errno.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "format.h"

#define MAX_STR_FORMAT		1024*8
#define MAX_WSTR_FORMAT		1024*8


#if defined(WIN32)
static wchar_t		s_wString_uni_s[MAX_WSTR_FORMAT];
static char		s_strMsg_error_s[MAX_STR_FORMAT];
static char		s_st_str_tochar_[MAX_STR_FORMAT];
#endif

char upchar(char ch)
{
	return (ch >= 'a' && ch <= 'z') ? ch - 32 : ch;
}

char downchar(char ch)
{
	return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch;
}

int toHexInt(char ch)
{
	ch = upchar(ch);
	return ch > '9' ? ch - 55 : ch - 48;
}

char toHexChar(unsigned char by)
{
	return by > 9 ? by + 55 : by + 48;
}


long asciiToHex(const char *szAscii, unsigned char *pbyOut)
{
	long lLen = (long)strlen(szAscii);
	long lRet = 0, i;

	if(0 != (lLen % 2))
		lLen--;

	for(i = 0; i < lLen; i += 2)
		pbyOut[lRet++] = (((unsigned char)toHexInt(szAscii[i])) << 4) | ((unsigned char)toHexInt(szAscii[i + 1]));

	return lRet;
}

size_t asciiToAsciiHex(const char *szAscii, char *strOut)
{
	long lLen = (long)strlen(szAscii);
	long i;

	for(i = 0; i < lLen; i++)
		sprintf(&strOut[i*2], "%02X", szAscii[i]);

	strOut[i*2] = 0;

	return strlen(strOut);
}



long hexToAscii(const unsigned char *pbyHex, long lSize, char *strAsciiOut)
{

	long lRet = 0, i;

	for(i = 0; i < lSize; i++)
	{
		strAsciiOut[lRet++] = toHexChar(pbyHex[i] >> 4);
		strAsciiOut[lRet++] = toHexChar(pbyHex[i]&0x0F);
	}

	strAsciiOut[lRet] = 0;

	return lRet;
}

long bufferToPrintAscii(unsigned char *pbyBuff, long lSize, char *strPrintableOut)
{
	long i;
	for(i = 0; i < lSize; i++)
		strPrintableOut[i] = isprint(pbyBuff[i])?pbyBuff[i]:'.';
	
	strPrintableOut[i] = 0;
	return i;
}


void uLongToBuffer(unsigned long ul, unsigned char *pOut, unsigned char bLittleEndian)
{
	if(bLittleEndian)
	{
		pOut[3] = (unsigned char)ul;
		pOut[2] = (unsigned char)(ul>>8);
		pOut[1] = (unsigned char)(ul>>16);
		pOut[0] = (unsigned char)(ul>>24);
	}
	else
	{
		pOut[0] = (unsigned char)ul;
		pOut[1] = (unsigned char)(ul>>8);
		pOut[2] = (unsigned char)(ul>>16);
		pOut[3] = (unsigned char)(ul>>24);
	}
}

void uLongLongToBuffer(unsigned long long ull, unsigned char *pOut, unsigned char bLittleEndian)
{

	//0 a las iquierda es big
	if(bLittleEndian)
	{
		pOut[0] = (unsigned char)ull;
		pOut[1] = (unsigned char)(ull >> 8);
		pOut[2] = (unsigned char)(ull >> 16);
		pOut[3] = (unsigned char)(ull >> 24);
		pOut[4] = (unsigned char)(ull >> 32);
		pOut[5] = (unsigned char)(ull >> 40);
		pOut[6] = (unsigned char)(ull >> 48);
		pOut[7] = (unsigned char)(ull >> 56);
	}
	else
	{
		pOut[7] = (unsigned char)ull;
		pOut[6] = (unsigned char)(ull >> 8);
		pOut[5] = (unsigned char)(ull >> 16);
		pOut[4] = (unsigned char)(ull >> 24);
		pOut[3] = (unsigned char)(ull >> 32);
		pOut[2] = (unsigned char)(ull >> 40);
		pOut[1] = (unsigned char)(ull >> 48);
		pOut[0] = (unsigned char)(ull >> 56);
	}
}

void uIntToBuffer(short int w, unsigned char *pOut, unsigned char bLittleEndian)
{
	if (bLittleEndian)
	{
		pOut[1] = (unsigned char)w;
		pOut[0] = (unsigned char)(w >> 8);
	}
	else
	{
		pOut[0] = (unsigned char)w;
		pOut[1] = (unsigned char)(w >> 8);
	}
}

unsigned long toLittleEndian(const unsigned char *pbyHex)
{
	//AA,BB,CC,DD
	//AABBCCDD
	unsigned long ul = 0;
	
	ul = 0xFF & pbyHex[0];
    ul |= (0xFF & pbyHex[1]) << 8;
    ul |= (0xFF & pbyHex[2]) << 16;
    ul |= (unsigned long)(0xFF & pbyHex[3]) << 24;

	return ul;
}

unsigned long toBigEndian(const unsigned char *pbyHex)
{
	//AA,BB,CC,DD
	//DDCCBBAA
    unsigned long ul = 0;

	ul = 0xFF & pbyHex[3];
    ul |= (0xFF & pbyHex[2]) << 8;
    ul |= (0xFF & pbyHex[1]) << 16;
    ul |= (unsigned long)(0xFF & pbyHex[0]) << 24;

    return ul;
}

const char *lastError()
{
#if defined(WIN32)
    return win_strerror(GetLastError());
#else
    return strerror(errno);
#endif
}

#if defined(WIN32)
wchar_t *towchar_t(const char* charArray)
{
	size_t stLen = strlen(charArray);
	s_wString_uni_s[stLen] = 0;
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, s_wString_uni_s, (int)stLen);
	return s_wString_uni_s;
}

char *toChar(wchar_t *wstr)
{
	s_st_str_tochar_[wcstombs(s_st_str_tochar_, wstr, sizeof(s_st_str_tochar_))] = 0;
	return s_st_str_tochar_;
}

wchar_t *toWChar(char *str)
{
	return towchar_t(str);
}

char *win_strerror(unsigned long dwError) 
{
	
	LPVOID	lpMsgBuf = NULL;		
//	int length;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR)&lpMsgBuf, 0, NULL );

	//length = WideCharToMultiByte(CP_ACP, 0, lpMsgBuf, -1, 0, 0, NULL, NULL);
	
	sprintf(s_strMsg_error_s, "%s", (LPCTSTR)lpMsgBuf);
	
	///wcstombs(s_strMsg_error_s, lpMsgBuf, length);
	
	LocalFree(lpMsgBuf);

	return s_strMsg_error_s;
};

unsigned char charToBin(char ch, char *strBin)
{
	int i;
	unsigned char byHex = (unsigned char)ch;

	for (i = 0; i < 8; i++)
		strBin[i] = (0 != (byHex&(0x80 >> i))) ? '1' : '0';

	strBin[i] = 0;

	return byHex;
}

char binToChar(const char *szBin)
{
	int i = 0;
	unsigned char byRet = 0;
	
	for (; i < 8; i++)
	{
		if ('1' == szBin[i])
			byRet |= (0x80 >> i);
	}

	return byRet;
}

#endif


const char *stringCpy(char *strDest, size_t stSize, const char *szSrc)
{
	size_t stSrc = strlen(szSrc);

	if(stSrc > stSize)
	{
		strncpy(strDest, szSrc, stSize);
		strDest[stSize - 1] = 0;
	}
	else
		strcpy(strDest, szSrc);

	return strDest;
}

void lowstr(char *str)
{
	char *p = str;
	int i = 0;
	while(*p)
		str[i++] = downchar(*p++);
}

void upstr(char *str)
{
	char *p = str;
	int i = 0;
	while(*p)
		str[i++] = upchar(*p++);
}


unsigned long long atoull(const char *psz)
{
	unsigned long long ull = 0;
	int i = 0;

	for(i; psz[i] != '\0'; ++i)
		ull = ull * 10 + psz[i] - '0';

	return ull;
}


unsigned long long hexaToUlonglong(const char *pszHex)
{
	int len = (int)strlen(pszHex);

	int base = 1;

	unsigned long long dec_val = 0;

	for(int i = len - 1; i >= 0; i--)
	{
		if(pszHex[i] >= '0' && pszHex[i] <= '9')
		{
			dec_val += (pszHex[i] - 48)*base;
			base = base * 16;
		}

		else if(pszHex[i] >= 'A' && pszHex[i] <= 'F')
		{
			dec_val += (pszHex[i] - 55)*base;
			base = base * 16;
		}
	}

	return dec_val;
}