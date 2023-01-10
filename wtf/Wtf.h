#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include "WinConsole.h"

class Wtf : public  WinConsole
{
public:
	explicit Wtf(const char *szPathFile);
	
	virtual ~Wtf();


	static constexpr const char* m_szTitle{ "Watching the file!" };
private:
	bool			m_bCtrlC;
	const char		*m_szPathFile;
private:
	void ctrlC() final;


	
	void error() const;


public:

	void read();

	static void pressIntro()
	{
		std::cout << "Press intro" << std::endl;
		std::cin.get();
	}
	
	static void help()
	{
		std::cout << m_szTitle << std::endl << "use: wtf file" << std::endl;
		pressIntro();
	}

	static void sleep(unsigned long ulMill)  
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ulMill));
	}

};

