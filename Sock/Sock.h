#pragma once

#define MAX_CHAR_HOST	255

class Sock
{
private:

	static WSADATA *m_stpWSockData;
	
	typedef void(*TPACCEPTFUNCTION)(SOCKET *, char *strFrom, void *pParam);
protected:
	SOCKET		m_sck;

	char		m_strIP[MAX_CHAR_HOST];
	char		m_strDSN[MAX_CHAR_HOST];
	char		m_strLocal[MAX_CHAR_HOST];

private:
	void			startup();

protected:
	bool			debugLastError(const char *szTag);
public:
	
	explicit Sock();

	Sock(const Sock&) = delete;
	Sock(Sock&&) = delete;
	Sock& operator=(const Sock&) = delete;
	Sock& operator=(Sock&&) = delete;
	
	virtual ~Sock();

	static void cleanup();

	const char		*toIP(const char *szDSN);	
	const char		*toDNS(const char *szIP);	
	const char		*local();

	bool			localIp(char* pstr);

	void			close(SOCKET sck = -1);
	bool			setTimeoutRcv(int nMillisec, SOCKET sck = -1);
	bool			setTimeoutSnd(int nMillisec, SOCKET sck = -1);
	bool			down(bool bSnd, bool bRcv, SOCKET sck = -1);	
	
	
	/* Retorna error=-1 error tipo de interface=0 ok=6*/
	int				getMac( 
							DWORD dwIndexInterface,		//numero de interface
							unsigned char *pbyMAC		//arreglo de 6 bytes donde copia la MAC 
							); 

	virtual SOCKET		create() = 0;
	virtual SOCKET		create(const char *szHost, int nPort) = 0;
	virtual bool		connect(const char *szHost, int nPort) = 0;
	virtual bool		connect() = 0;
	virtual int			snd(void *pBuff, int nLen, SOCKET sk = -1) const = 0;
	virtual int			rcv(void *pBuff, int nLen, SOCKET sk = -1) const = 0;

	bool			listen(TPACCEPTFUNCTION acceptFn, int nPort, const char *szIP, void *pParamAcceptFn);

	static  WSADATA *WSData() { return m_stpWSockData; }
	static  unsigned long lastError();
	static  bool local(char* str);
	static	bool getLocalIp(char *str);

	
	inline SOCKET sock() const { return m_sck; };

};

