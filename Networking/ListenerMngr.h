#pragma once


namespace NET
{
	class ListenerMngr
	{
	private:

		std::unique_ptr<Udp> m_pTcp{};
		std::unique_ptr<unsigned char[]> m_byRcvBuff{};

		const  Setting&	m_Settings;
	public:

		explicit ListenerMngr(Setting &settings);

		bool create() const;
		void start() const;

		bool check(const unsigned char *pRcv) const;

	};


};
