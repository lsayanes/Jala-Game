#pragma once


#if _WIN32
#include <malloc.h>
#endif

namespace draw
{
	template<typename T>
	class Memory
	{
	public:
		static constexpr T const M1G{ 1ULL << 30 };
		static constexpr T const M512M{ 1ULL << 29 };
		static constexpr T const M256M{ 1ULL << 28 };
		static constexpr T const M128M{ 1ULL << 27 };

		inline explicit Memory(T size) :
#if _WIN32
			m_pbyMem{ reinterpret_cast<uint8_t*>(_aligned_malloc(size, sizeof(T))) },
#endif
			m_used{ 0 },
			m_total{ size },
			m_alignment{ sizeof(T) }
		{

#if !_WIN32
			posix_memalign(reinterpret_cast<void**>(&m_pbyMem), m_alignment, size);
#endif

			m_pyCurr = const_cast<uint8_t*>(m_pbyMem);
		}


		Memory(const Memory&) = delete;
		Memory(Memory&&) = delete;

		Memory& operator=(const Memory&) = delete;
		Memory& operator=(Memory&& o) = delete;

		inline virtual ~Memory()
		{
#if _WIN32
			_aligned_free(reinterpret_cast<void*>(const_cast<uint8_t*>((m_pbyMem))));
#else
			free(m_pbyMem);
#endif
		}


		inline void* alloc(T size)
		{
			uint8_t* pRet{ nullptr };

			if (m_used + size < m_total)
			{
				pRet = m_pyCurr;
				m_pyCurr += size;
				m_used += size;
			}

			return pRet;
		}

		inline T total() const
		{
			return m_total;
		}

		inline T used() const
		{
			return m_used;
		}

		inline void reset()
		{
			m_used = 0;
			m_pyCurr = const_cast<uint8_t*>(m_pbyMem);
		}

	private:
		const uint8_t* m_pbyMem;
		T				m_used;
		T				m_total;
		uint8_t* m_pyCurr;
		T				m_alignment;
	};

}//draw