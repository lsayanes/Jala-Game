
#define TOLITTLEENDIANSHORT(w) (unsigned short)((w << 8) | (w >> 8))

#define INVERT_UINT(ui)	(unsigned int)((((ui>>24) & 0x000000ff) | ((ui>>8) & 0x0000ff00) | ((ui<<8) & 0x00ff0000) | ((ui<<24) & 0xff000000)))
#define INVERT_INT(n)	INVERT_UINT((unsigned int)n)
#define INVERT_WORD(w) 	(unsigned short)((unsigned short)(w << 8)|(unsigned short)(w >> 8)) 


#if defined(__cplusplus)
extern "C" {
#endif

char 	upchar(char ch);		/*retorna la mayuscula de ch*/
char 	downchar(char ch);		/*retorna la minuscula de ch*/
int 	toHexInt(char ch);		/*retorna el valor entero de un char-hex ('A' 'B' 'C' 'd' 'e' etc)*/
char 	toHexChar(unsigned char by);	/*retorna el ascii de by*/

/*convierte una cadena hex en un buffer, retorna la cantidad de bytes del pbyHexOut*/
long 	asciiToHex(const char *szHexAscii, unsigned char *pbyHexOut);	
long 	hexToAscii(const unsigned char *pbyHex, long lSize, char *strAsciiOut);
long	bufferToPrintAscii(unsigned char *pbyBuff, long lSize, char *strPrintableOut);

size_t	asciiToAsciiHex(const char *szAscii, char *strOut);

void	uLongToBuffer(unsigned long ul, unsigned char *pOut, unsigned char bLittleEndian);
void	uIntToBuffer(short int w, unsigned char *pOut, unsigned char bLittleEndian);

void	uLongLongToBuffer(unsigned long long ull, unsigned char *pOut, unsigned char bLittleEndian);

//unsigned short	toLittleEndianShort(unsigned short w);
unsigned long	toBigEndian(const unsigned char *pbyHex);
unsigned long	toLittleEndian(const unsigned char *pbyHex);


unsigned char	charToBin(char ch, char *strBin);
char			binToChar(const char *szBin);

const char *lastError();

#if defined(WIN32)
wchar_t *towchar_t(const char* charArray);
char	*win_strerror(unsigned long dwError);
char	*toChar(wchar_t *wstr);
wchar_t *toWChar(char *str);
#else
#define win_strerror(n)	strerror(n)
#endif


const char *stringCpy(char *strDest, size_t stSize, const char *szSrc);

void lowstr(char *str);
void upstr(char *str);
unsigned long long	atoull(const char *psz);
unsigned long long hexaToUlonglong(const char *pszHex);

#if defined(__cplusplus)
}
#endif

