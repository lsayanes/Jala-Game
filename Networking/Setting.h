#pragma once

namespace NET
{

	struct Setting
	{
		explicit Setting(int nPort, unsigned long ulMaxConnections, const char* szHost = nullptr) :
			m_nPort {nPort},
			m_ulMaxConnections{ ulMaxConnections },
			m_szHost { szHost }

		{

		}

		Setting(const Setting&) = delete;
		Setting(Setting&&) = delete;
		Setting& operator=(const Setting&) = delete;
		Setting& operator=(Setting&&) = delete;


		const unsigned long m_ulMaxConnections{};
		const char* m_szHost;
		const int	m_nPort;


		static constexpr unsigned long max_buff{ 1024 };

	};

}

