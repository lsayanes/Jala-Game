
#pragma once

struct Png
{

	explicit Png() 
	{
	}

	explicit Png(uint32_t w, unsigned h, unsigned char bpp, void* pData) :
		ulWidth{w},
		ulHeight{h},
		byBpp{bpp},
		pRawData {pData},
		ulSize { (ulWidth * ulHeight * (byBpp >> 3)) }
	{
	}

	virtual ~Png();

	uint32_t		ulWidth{0};
	uint32_t		ulHeight{0};
	unsigned char	byBpp{0};
	void*			pRawData{nullptr}; //GBRA
	uint32_t		ulSize{0};

	[[nodiscard]] bool load(const char* szFile, unsigned char byBpp = 32, unsigned char byBitdepth = 8);
};



