#pragma once

class Udp : public Sock
{
private:
	//struct sockaddr_in	m_Server;	
	//struct sockaddr_in	m_Client;	


	struct sockaddr_in	m_SockAddrBind;
	struct sockaddr_in	m_SockAddrSndRcv;

public:
	Udp(SOCKET sk);
	Udp();
	~Udp();

	SOCKET		create(const char *szHost, int nPort);
	int			snd(void *pBuff, int nLen, SOCKET sk = -1);
	int			rcv(void *pBuff, int nLen, SOCKET sk = -1);

	void		peerName(char *strHostIp, SOCKET sk = -1);

	
	SOCKET	create() 
	{
		return SOCKET_ERROR;
	}

	bool connect()
	{
		return true;
	}
	
	bool	connect(const char *szHost, int nPort) 
	{
		(void)szHost;
		(void)nPort;
		return true;
	}


};

