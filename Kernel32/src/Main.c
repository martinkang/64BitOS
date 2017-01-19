#include "Types.h"

void printStringToScreen( int aX, int aY, const char * aPcString );
BOOL initializeKernel64Area( void );

void Main( void )
{
	printStringToScreen( 0, 3, "C Language Kernel Start~!!!" );

	printStringToScreen( 0, 4, "" );

	if ( initializeKernel64Area() == TRUE )
	{
		printStringToScreen( 0, 4, "IA-32e Kernel Area Initialization Complete" );
	}
	else
	{
		printStringToScreen( 0, 4, "" );
	}


	while( 1 );
}

void printStringToScreen( int aX, int aY, const char * aString )
{
	int i = 0;
	CHARACTER * sScreen = (CHARACTER*) 0xB8000; // 비디오 메모리 어드레스

	sScreen += ( aY * 80 ) + aX;
	while( aString[i] != 0 )
	{
		sScreen[i].bCharactor = aString[i];
		i++;
	}
}

BOOL initializeKernel64Area( void )
{
	DWORD * sCurAddr;

	sCurAddr = (DWORD*) 0x100000;

	while( (DWORD)sCurAddr < 0x600000 )
	{
		*sCurAddr = 0x00;

		if ( *sCurAddr != 0 )
		{	
			return FALSE;
		}

		sCurAddr++;
	}

	return TRUE;
}

