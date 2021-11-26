
#include <stdio.h>

#include <memory>
#include <stdexcept>

#include <winsock2.h>

#include "../Sock/Sock.h"
#include "../Sock/Udp.h"

#include "Setting.h"
#include "ListenerMngr.h"

namespace NET
{

    ListenerMngr::ListenerMngr(Setting& settings) :
        m_Settings {settings},
        m_pTcp{ std::make_unique<Udp>() }

    {
    }

    bool ListenerMngr::create() const
    {
        bool bRet = false;
        try
        {
            bRet = SOCKET_ERROR != m_pTcp->create(m_Settings.m_szHost, m_Settings.m_nPort);
        }
        catch (std::exception& e)
        {
            OutputDebugString(e.what());
        }

        return bRet;
    }

    void ListenerMngr::start() const
    {
        unsigned char by[Setting::max_buff];
        int nRcv = -1;
        while (0 != nRcv)
        {
            if ((nRcv = m_pTcp->rcv(by, Setting::max_buff)) > 0)
            {
                if (check(by))
                {
                    //TODO create a new thread 
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