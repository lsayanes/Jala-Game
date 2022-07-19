#pragma once


namespace net
{
	class ListenerMngr
	{
	private:

		bool	m_bListening;

		std::unique_ptr<Udp>				m_pTcp{};
		std::unique_ptr<unsigned char[]>	m_byRcvBuff{};

		const  Setting&	m_Settings;
	public:

		explicit ListenerMngr(Setting &settings);
		~ListenerMngr();

		bool create() const;
		void start(std::mutex *pmSc, std::vector<unsigned char> *pvOut);

		bool check(const unsigned char *pRcv) const;
	
	};
};
