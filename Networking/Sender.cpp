

#include <stdio.h>

#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>

#include <winsock2.h>

#include "../Sock/Sock.h"
#include "../Sock/Udp.h"

#include "Setting.h"
#include "Sender.h"


namespace net {

    Sender::Sender(const Setting& settings) :
        m_Settings{ settings },
        m_pTcp{ std::make_unique<Udp>() },
        m_ulPktSent{ 0 }, m_ulPktSeq{ 0 }
    {

    }

    bool Sender::create() const
    {
        bool bRet = false;
#if !defined(_PART_OFF_GAME_)        
        try
#endif
        {
            bRet = SOCKET_ERROR != m_pTcp->create(m_Settings.m_szHost, m_Settings.m_nPort);
        }
#if !defined(_PART_OFF_GAME_)
        catch (std::exception& e)
        {
            OutputDebugString(e.what());
        }
#endif

        return bRet;
    }

    void Sender::start()
    {
        //char str[1024];
        unsigned char by[Setting::max_buff];
        int nRcv = -1;

        while (0 != nRcv)
        {
            std::memcpy(&by[m_pktSeqOffset], &m_ulPktSeq, sizeof(m_ulPktSeq));
            std::memcpy(&by[m_pktSentOffset], &m_ulPktSent, sizeof(m_ulPktSent));
            std::memcpy(&by[m_dataOffset], "\xAA\xBB\xCC", 3);

            if ((nRcv = m_pTcp->snd(by, m_dataOffset + 3)) > 0)
            {
                addPktSent();
                m_ullByteSet += (m_dataOffset + 3);
                //sprintf(str, "Sent { Seq :%lu Pkt :%lu }\n ", m_ulPktSeq, m_ulPktSent);
                //OutputDebugString(str);

                //std::this_thread::sleep_for(std::chrono::milliseconds(50000));
            }
        }
    };

}