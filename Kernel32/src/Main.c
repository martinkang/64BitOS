#include "Types.h"

void printStringToScreen( int aX, int aY, const char * aPcString );

void Main( void )
{
	printStringToScreen( 0, 3, "C Language Kernel Start~!!!" );

	while( 1 );
}

void printStringToScreen( int aX, int aY, const char * aString )
{
	CHARACTER * sScreen = (CHARACTER*) 0xB8000; // 비디오 메모리 어드레스
	int i = 0;

	sScreen += ( aY * 80 ) + aX;
	while( aString[i] != 0 )
	{
		sScreen[i].bCharactor = aString[i];
		i++;
	}
}

