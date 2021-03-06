
#include <stdio.h>

#include <memory>
#if !defined(_PART_OFF_GAME_)        
#include <stdexcept>
#endif
#include <string>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

#include <winsock2.h>

#include "../Util/format.h"

#include "../Sock/Sock.h"
#include "../Sock/Udp.h"

#include "Setting.h"
#include "ListenerMngr.h"

namespace net
{

    ListenerMngr::ListenerMngr(Setting& settings) :
        m_bListening{false},
        m_Settings{ settings },
        m_pTcp{ std::make_unique<Udp>() },
        m_byRcvBuff { std::make_unique<unsigned char[]>(Setting::max_buff) }
    {
    }

    ListenerMngr::~ListenerMngr()
    {
        m_bListening = false;
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

    void ListenerMngr::start(std::mutex *pmSc, std::vector<unsigned char> *pvOut)
    {
        if (pmSc && pvOut)
        {
            int nRcv = -1, i;
            unsigned char* pby = m_byRcvBuff.get();

            m_bListening = true;

            while (m_bListening)
            {
                if ((nRcv = m_pTcp->rcv(pby, Setting::max_buff)) > 0)
                {
                    pmSc->lock();

                    if (check(pby))
                    {
                        for (i = 0; i < nRcv; i++)
                            pvOut->push_back(pby[i]);
                    }

                    pmSc->unlock();
                }
                else
                {
                    //   std::this_thread::sleep_for(std::chrono::milliseconds(1));
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