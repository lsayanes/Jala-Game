

#if defined(_WINDOWS)
#include <Windows.h>
#include <stdint.h>
#include "format.h"

#include "debug.h"

void dbg(const char *msg, ...)
{
	OutputDebugString(format("\nDEBUG\tFile:%s Line:%d::", __FILE__, __LINE__));
	OutputDebugString(format(msg));
}


#endif