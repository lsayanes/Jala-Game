#pragma once

class Udp : public Sock
{
private:
	struct sockaddr_in	m_SockAddrBind;
	struct sockaddr_in	m_SockAddrSndRcv;

public:
	Udp();
	~Udp();

	SOCKET		create(const char *szHost, int nPort) override;
	int			snd(void *pBuff, int nLen, SOCKET sk = -1) const override;
	int			rcv(void *pBuff, int nLen, SOCKET sk = -1) const override;

	void		peerName(char *strHostIp, SOCKET sk = -1);

	
	SOCKET create() override
	{
		return SOCKET_ERROR;
	}

	bool connect() override
	{
		return true;
	}
	
	bool connect(const char *szHost, int nPort)  override
	{
		(void)szHost;
		(void)nPort;
		return true;
	}


};

