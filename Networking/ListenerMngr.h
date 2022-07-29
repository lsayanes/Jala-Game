#pragma once


namespace net
{
	class ListenerMngr
	{
	private:

		bool	m_bListening;

		std::unique_ptr<Udp>				m_pTcp{};
		std::unique_ptr<unsigned char[]>	m_byRcvBuff{};

		std::vector<unsigned char>	m_Packets;
		std::mutex					m_mxPacks;
		std::condition_variable		m_cvPackets;

		const  Setting&	m_Settings;
	private:
		void listen();
	public:

		explicit ListenerMngr(Setting &settings);
		~ListenerMngr();

		bool create() const;
		//void start(std::mutex *pmSc, std::vector<unsigned char> *pvOut);
		//void listen();
		void start();

		const bool waitPacketsNotify();
		std::vector<unsigned char> cpyPackets();

		bool check(const unsigned char *pRcv) const;
	
	};
};
