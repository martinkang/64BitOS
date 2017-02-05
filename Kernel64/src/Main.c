#include "Types.h"

void printStringToScreen( int 		   aX,
				  		  int		   aY,
				 		  const char * aStr );

void Main( void )
{
	printStringToScreen( 0, 10, "Switch To IA-32e Mode Success~!!" );
	printStringToScreen( 0, 11, "IA-32e C Language Kernel Start...........[    ]" );
}

void printStringToScreen( int 		   aX,
				  		  int		   aY,
				 		  const char * aStr )
{
	int i = 0;
	CHARACTER * sScreen = (CHARACTER*)0xB8000;
	
	// 문자열 출력할 어드레스를 계산
	sScreen += ( aY * 80 ) + aX;

	for ( i = 0; aStr[i] != 0; i++ )
	{
		sScreen[i].mCharactor = aStr[i];
	}
}


