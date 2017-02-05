#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Types.h"

void readCPUID( DWORD   aDwEAX,
				DWORD * aPdwEAX,
				DWORD * aPdwEBX,
				DWORD * aPdwECX,
				DWORD * aPdwEDX );
void switchAndExecute64BitKernel( void );

#endif
