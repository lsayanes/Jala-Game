
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>

#include <winsock2.h>
#include <Windows.h>
#pragma warning (disable : 4786)
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace std;



#include <string.h>

#include "../Util/format.h"

#include "Sock.h"

#include "Udp.h"


Udp::Udp():Sock()
{
}

Udp::Udp(SOCKET sk):Sock(sk)
{
}

Udp::~Udp()
{
}

SOCKET Udp::create(const char *szHost, int nPort)
{
	m_sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int nBind = -1;

	if(SOCKET_ERROR != m_sck)
	{
		char strServerIp[256];
		char strLocalIp[256];
		struct hostent *pServerName;
		struct hostent *pLocalName;

		int optval = 1;
		setsockopt(m_sck, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);

		memset((void *)&m_SockAddrSndRcv, '\0', sizeof(struct sockaddr_in));
		m_SockAddrSndRcv.sin_addr.s_addr = htonl(INADDR_ANY);
		
		if (szHost)
		{
			pServerName = gethostbyname(szHost);

			if (NULL != pServerName)
				strcpy(strServerIp, inet_ntoa(*((struct in_addr *)pServerName->h_addr)));
			else
				strcpy(strServerIp, szHost);
			
			m_SockAddrSndRcv.sin_addr.s_addr = inet_addr(strServerIp);
		}


		m_SockAddrSndRcv.sin_family = AF_INET;
		m_SockAddrSndRcv.sin_port = htons(nPort);


		memset((void *)&m_SockAddrBind, '\0', sizeof(struct sockaddr_in));

		m_SockAddrBind.sin_family = AF_INET;
		m_SockAddrBind.sin_port = htons(nPort);

		m_SockAddrBind.sin_addr.s_addr = INADDR_ANY;

		strcpy(strLocalIp, "0.0.0.0");
		pLocalName = gethostbyname(strLocalIp);

		if(pLocalName)
		{
			strcpy(strLocalIp, inet_ntoa(*((struct in_addr *)pLocalName->h_addr)));
			m_SockAddrBind.sin_addr.s_addr = inet_addr(strLocalIp);
			
			if (-1 == (nBind = bind(m_sck, (struct sockaddr *)&m_SockAddrBind, sizeof(struct sockaddr_in))))
			{

#if !defined(_PART_OFF_GAME_)
				int nErr = WSAGetLastError();
#endif
				close();

#if !defined(_PART_OFF_GAME_)
				throw std::exception(win_strerror(nErr));
#endif
			}
		}
	}

	return m_sck;

}

int	Udp::snd(void *pBuff, int nLen, SOCKET sk) const
{
	return sendto(-1==sk?m_sck:sk, (char*)pBuff, nLen, 0, (struct sockaddr *)&m_SockAddrSndRcv, sizeof(struct sockaddr_in));
}

void Udp::peerName(char *strHostIp, SOCKET sk)
{
	SOCKET nSk = sk;
	if (nSk < 0)
		nSk = (int)m_sck;
	
	strHostIp[0] = 0;
	if (nSk > 0)
	{
		int nSockLen = sizeof(struct sockaddr_in);
		getpeername(nSk, (SOCKADDR *)&m_SockAddrSndRcv, &nSockLen);
		strcpy(strHostIp, inet_ntoa(m_SockAddrSndRcv.sin_addr)); //htons(SenderAddr.sin_port)
	}
}

int	Udp::rcv(void *pBuff, int nLen, SOCKET sk) const 
{
	int nSockLen = sizeof(struct sockaddr_in);

	SOCKET nSk = sk;
	if(nSk < 0)
		nSk = (int)m_sck;

	int nRet = 0;

	if (nSk > 0)
		nRet = recvfrom(nSk, (char*)pBuff, nLen, 0, (struct sockaddr *)&m_SockAddrSndRcv, &nSockLen);

	return nRet;
}

