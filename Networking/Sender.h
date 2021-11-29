#pragma once

namespace net
{
	class Sender
	{
	private:

		unsigned long m_ulPktSent{};  //packet sent
		unsigned long m_ulPktSeq{};  //sequence of packet sent

		static constexpr unsigned long m_pktSeqOffset{ 0 };
		static constexpr unsigned long m_pktSentOffset{ sizeof(m_ulPktSeq) };
		static constexpr unsigned long m_dataOffset{ sizeof(m_ulPktSent) + sizeof(m_ulPktSeq) };

		std::unique_ptr<Udp> m_pTcp{};
		const  Setting& m_Settings;
	public:
		explicit Sender() = default;
		explicit Sender(const Setting& settings);

		bool create() const;
		void start();

		inline void addPktSent() 
		{
			m_ulPktSent++;
			if (m_ulPktSent > (ULONG_MAX - 2))
			{
				m_ulPktSent = 0;
				m_ulPktSeq++;
			}
		}
	};

}

