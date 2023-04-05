


#include <stdio.h>

#if defined(WIN32)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#endif
#include <sys/socket.h>
#include <bits/socket.h>
#include <linux/socket.h>
#include "Sock.h"

#if defined(WIN32)
#pragma comment(lib, "iphlpapi")
#pragma comment(lib, "ws2_32.lib")

WSADATA *Sock::m_stpWSockData = NULL;
#else
WSADATA *Sock::m_stpWSockData = NULL;
#endif

Sock::Sock():
	m_strIP{0},
	m_strDSN{0},
	m_strLocal{0},
	m_sck{INVALID_SOCKET}

{
	startup();
}

void Sock::startup()
{
	if(!m_stpWSockData)
	{
		m_stpWSockData = new WSADATA;
		if(WSAStartup(MAKEWORD(2, 2), m_stpWSockData))
		{
			delete m_stpWSockData;
			m_stpWSockData = NULL;
		}
	}


	m_strIP[0] = 0;
	m_strDSN[0] = 0;
}

void Sock::cleanup()
{
	if(m_stpWSockData)
	{
		WSACleanup();
		delete m_stpWSockData;
		m_stpWSockData = NULL;
	}
}


Sock::~Sock()
{
	Sock::cleanup();
}

bool Sock::listen(TPACCEPTFUNCTION acceptFn, int nPort, const char *szIP, void *pParamAcceptFn)
{
    struct sockaddr_in		me;		
	int						nYes = 1;
	ULONG					ulIP;

	if(NULL == szIP)
		ulIP = INADDR_ANY;
	else
		ulIP = inet_addr(szIP);

	if(-1 != setsockopt(m_sck,  SOL_SOCKET, SO_REUSEADDR|SO_DONTLINGER, (char*)&nYes, sizeof(int))) 
	{
		me.sin_family = AF_INET;  
		me.sin_port = htons(nPort); 
		me.sin_addr.s_addr = htonl(ulIP); 
		memset(&(me.sin_zero), '\0', 8); 
	
		if(-1 != bind(m_sck, (struct sockaddr *)&me, sizeof(struct sockaddr))) 
		{
			if(0 == ::listen(m_sck, SOMAXCONN))
			{
				if(acceptFn)
				{
					while(1)
					{
						int						nSize = sizeof(struct sockaddr_in);
						SOCKET					sckNew = SOCKET_ERROR;
						struct sockaddr_in		Their;	

						if(m_sck == SOCKET_ERROR)
							return true;	//cerraron el socket
						
						sckNew = ::accept(m_sck, (struct sockaddr *)&Their, &nSize); 
						if(SOCKET_ERROR != sckNew)
							acceptFn(&sckNew, inet_ntoa(Their.sin_addr), pParamAcceptFn);
					}
				}
			}
			else
				debugLastError("listen");
		}
		else 
			debugLastError("bind");
	}
	else 
		debugLastError("setsockopt");


	return false;
}


unsigned long Sock::lastError()
{
	return WSAGetLastError();
}

bool Sock::debugLastError(const char *szTag)
{

	LPVOID	lpMsgBuf = NULL;		
	char	error_s[2048];

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			lastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR)&lpMsgBuf, 0, NULL );

	
	sprintf(error_s, "Sock::Err %s %s\n", szTag, (char*)lpMsgBuf);

	OutputDebugString(error_s);

	LocalFree(lpMsgBuf);
		
	return false; //para usar en los return
}



const char *Sock::toIP(const char *szDNS) 
{
	struct			hostent *Host;

	Host = gethostbyname(szDNS);
	
	m_strIP[0] = 0;
	if(NULL != Host) 
		sprintf(m_strIP, "%s", inet_ntoa(*((struct in_addr *)Host->h_addr)));
	else 
	{
		debugLastError("gethostbyname");
		return NULL;
	}
	
	return m_strIP;
};

const char *Sock::toDNS(const char *szIP) 
{

	struct			hostent *Host;
	long			ulAddress;
	
	ulAddress = inet_addr(szIP);
	Host = gethostbyaddr((char*)&ulAddress, sizeof(ulAddress), AF_INET);

	if(NULL != Host) 
	{
		sprintf(m_strDSN, "%s", Host->h_name);
	}
	else 
	{
		debugLastError("gethostbyname");
		return NULL;
	}
	
	return m_strDSN;
};

const char *Sock::local() 
{
	m_strLocal[0] = 0;
	WSASetLastError(0);			
		
	if(SOCKET_ERROR == gethostname(m_strLocal, sizeof(m_strLocal))) 
		debugLastError("gethostname");

	return m_strLocal;
};

bool Sock::local(char* str)
{
	str[0] = 0;
	return (SOCKET_ERROR == gethostname(str, sizeof(str)));
}

bool Sock::getLocalIp(char* str)
{

	struct			hostent* Host;

	if (Sock::local(str))
	{
		Host = gethostbyname(str);
		str[0] = 0;
		if (NULL != Host)
		{
			sprintf(str, "%s", inet_ntoa(*((struct in_addr*)Host->h_addr)));
			return true;
		}
	}


	return false;
}

bool Sock::localIp(char* pstr)
{
	if (toIP(local()))
	{
		strcpy(pstr, m_strIP);
		return true;
	}
	return false;
}


bool Sock::setTimeoutRcv(int nMillisec, SOCKET sck)
{
	return SOCKET_ERROR != setsockopt(-1==sck?m_sck:sck, SOL_SOCKET, SO_RCVTIMEO, (char*)&nMillisec, sizeof(nMillisec))?true:false;
}

bool Sock::setTimeoutSnd(int nMillisec, SOCKET sck)
{
	return SOCKET_ERROR != setsockopt(-1==sck?m_sck:sck, SOL_SOCKET, SO_SNDTIMEO, (char*)&nMillisec, sizeof(nMillisec))?true:false;
}

void Sock::close(SOCKET sck)
{
	closesocket(INVALID_SOCKET==sck?m_sck:sck);
	m_sck = SOCKET_ERROR;
}

bool Sock::down(bool bSnd, bool bRcv, SOCKET sck)
{

	int		nHow = -1;
	bool	bRet = false;

	if(true == bSnd && true == bRcv)
		nHow = SD_BOTH;
	if(true == bSnd && false == bRcv)
		nHow = SD_SEND;
	if(false == bSnd && true == bRcv)
		nHow = SD_RECEIVE;

	if(-1 != nHow) 
	{
		if(SOCKET_ERROR == shutdown(INVALID_SOCKET==sck?m_sck:sck, nHow))
			debugLastError("down");
		else
			bRet = true;
	}
	
	return bRet;
};

void print_adapter( PIP_ADAPTER_ADDRESSES aa )
{
	char buf[ BUFSIZ ];
	char str[ 1024 ];

	memset( buf, 0, BUFSIZ );
	WideCharToMultiByte( CP_ACP, 0, aa->FriendlyName, static_cast<int>(wcslen( aa->FriendlyName)), buf, (BUFSIZ), NULL, NULL );
	sprintf(str,  "adapter_name:%s\n", buf );

	OutputDebugString( str );
}

void print_addr( PIP_ADAPTER_UNICAST_ADDRESS ua )
{
	char buf[ BUFSIZ ];
	char str[ 1024 ];

	int family = ua->Address.lpSockaddr->sa_family;
	sprintf(str, "\t%s ", family == AF_INET ? "IPv4" : "IPv6" );

	memset( buf, 0, BUFSIZ );
	getnameinfo( ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof( buf ), NULL, 0, NI_NUMERICHOST );
	strcat( str, buf );
	//printf( "%s\n", buf );
	OutputDebugString( str );
}


int Sock::getMac( DWORD dwIndexInterface, unsigned char *pbyMAC )
{
	int nRet = -1;
	
	ULONG ulSize = 0;
	MIB_IPADDRTABLE *pIPTable = NULL;

	if ( pbyMAC )
	{
		memset( pbyMAC, 0, 6 );

		if ( ERROR_INSUFFICIENT_BUFFER == GetIpAddrTable( pIPTable, &ulSize, 0 ) )
		{
			if ( NULL != ( pIPTable = (MIB_IPADDRTABLE *)malloc( ulSize ) ) )
			{
				if ( NO_ERROR == GetIpAddrTable( pIPTable, &ulSize, 0 )  )
				{
					nRet = 0;
					for (DWORD i = 0; i < pIPTable->dwNumEntries && 0 == nRet; i++)
					{
						if ( dwIndexInterface == pIPTable->table[ i ].dwIndex )
						{
							MIB_IFROW iInfo;
							memset( &iInfo, 0, sizeof( MIB_IFROW ) );

							iInfo.dwIndex = pIPTable->table[ i ].dwIndex;

							GetIfEntry( &iInfo );

							if ( MIB_IF_TYPE_ETHERNET == iInfo.dwType )
							{
								memcpy( pbyMAC, iInfo.bPhysAddr, iInfo.dwPhysAddrLen );
								nRet = 6;
							}
						}

					}
				}
			}

			free( pIPTable );
		}
	}

	return nRet;
}