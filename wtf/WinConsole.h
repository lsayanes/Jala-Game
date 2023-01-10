

#pragma once

#include <Windows.h>

class WinConsole
{
public:
	

	explicit WinConsole();
	virtual ~WinConsole();

    void printLastError() const;
	void consoleEvent(DWORD signal);

	virtual void ctrlC() = 0;

private:

	void	*m_pInternalFnt;

};