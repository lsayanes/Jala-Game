
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
#include <condition_variable>

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
        m_byRcvBuff { std::make_unique<unsigned char[]>(Setting::max_buff) },
        m_Packets{},
        m_mxPacks{},
        m_cvPackets{}
    {
        m_Packets.reserve(Setting::max_buff * 10);
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


    void ListenerMngr::listen()
    {
        int nRcv = -1, i;
        unsigned char* pby = m_byRcvBuff.get();

        m_bListening = true;

        while (m_bListening)
        {
            if ((nRcv = m_pTcp->rcv(pby, Setting::max_buff)) > 0)
            {
                m_mxPacks.lock();

                if (check(pby))
                {
                    for (i = 0; i < nRcv; i++)
                        m_Packets.push_back(pby[i]);
                }

                m_mxPacks.unlock();
                m_cvPackets.notify_one();
            }
            else
            {
                //   std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        m_cvPackets.notify_all();

    }
    
    void ListenerMngr::start()
    {
        std::thread t(&ListenerMngr::listen, this);
        t.detach();
    };

    const bool ListenerMngr::waitPacketsNotify() 
    {
        std::unique_lock lk(m_mxPacks);
        m_cvPackets.wait(lk);
        return m_bListening;
    };

    std::vector<unsigned char> ListenerMngr::cpyPackets()
    {
        if (m_bListening)
        {
            m_mxPacks.lock();

            std::vector<unsigned char> ret{};
            ret.reserve(m_Packets.size());
            //std::copy(std::begin(m_Packets), std::end(m_Packets), std::begin(ret));
            std::vector<unsigned char>::iterator it = m_Packets.begin();
            
            while (it != m_Packets.end())
            {
                ret.push_back(*it);
                it++;
            }
            
            m_Packets.clear();

            m_mxPacks.unlock();
            return ret;
        }

        return {};
    }
  
    bool ListenerMngr::check(const unsigned char* pRcv) const
    {
        //TODO check protocol
        return (nullptr != pRcv);
    };

}