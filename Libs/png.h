
#pragma once

struct Png
{

	explicit Png() 
	{
	}

	explicit Png(unsigned long w, unsigned h, unsigned char bpp, void* pData) :
		ulWidth{w},
		ulHeight{h},
		byBpp{bpp},
		pRawData {pData},
		ulSize { (ulWidth * ulHeight * (byBpp >> 3)) }
	{
	}

	virtual ~Png();

	unsigned long		ulWidth{0};
	unsigned long		ulHeight{0};
	unsigned char		byBpp{0};
	void*				pRawData{nullptr}; //GBRA
	unsigned long		ulSize{0};

	bool load(const char* szFile, unsigned char byBpp = 32, unsigned char byBitdepth = 8);
};



