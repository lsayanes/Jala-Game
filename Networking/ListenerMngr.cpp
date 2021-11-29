
#include <stdio.h>

#include <memory>
#if !defined(_PART_OFF_GAME_)        
#include <stdexcept>
#endif
#include <string>

#include <winsock2.h>

#include "../Util/format.h"

#include "../Sock/Sock.h"
#include "../Sock/Udp.h"

#include "Setting.h"
#include "ListenerMngr.h"

namespace net
{

    ListenerMngr::ListenerMngr(Setting& settings) :
        m_Settings{ settings },
        m_pTcp{ std::make_unique<Udp>() },
        m_byRcvBuff { std::make_unique<unsigned char[]>(Setting::max_buff) }
    {
    }

    bool ListenerMngr::create() const
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

    void ListenerMngr::start() const
    {
        int nRcv = -1;
        unsigned char* pby = m_byRcvBuff.get();
        
        while (0 != nRcv)
        {
            if ((nRcv = m_pTcp->rcv(pby, Setting::max_buff)) > 0)
            {
                if (check(pby))
                {
                    //TODO create a new thread 
                    char* s{ new char[nRcv * 3] };
                    nRcv = hexToAscii(pby, nRcv, s);
                    s[nRcv] = 0;
                    OutputDebugString(s);
                    OutputDebugString("\n");
                    delete[] s;
                }
            }
        }
    };

    bool ListenerMngr::check(const unsigned char* pRcv) const
    {
        //TODO check protocol
        return (nullptr != pRcv);
    };

}