

#include <stdio.h>

#include <memory>
#include <stdexcept>

#include <winsock2.h>

#include "../Sock/Sock.h"
#include "../Sock/Udp.h"

#include "Setting.h"
#include "Sender.h"


namespace NET {

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
        unsigned char by[Setting::max_buff];
        int nRcv = -1;

        while (0 != nRcv)
        {
            std::memcpy(&by[m_pktSeqOffset], &m_ulPktSeq, sizeof(m_ulPktSeq));
            std::memcpy(&by[m_pktSentOffset], &m_ulPktSent, sizeof(m_ulPktSent));
            std::memcpy(&by[m_dataOffset], "\xAA\xBB\xCC", 3);

            if ((nRcv = m_pTcp->snd(by, Setting::max_buff)) > 0)
            {
                addPktSent();
            }
        }
    };

}