#include "Types.h"

void printStringToScreen( int aX, int aY, const char * aPcString );
BOOL initializeKernel64Area( void );
BOOL isMemoryEnough( void );

void Main( void )
{
	printStringToScreen( 0, 3, "C Language Kernel Start...................[Pass]" );

	//최소 메모리 크기를 만족하는 지 검사
	printStringToScreen( 0, 4, "Minimum Memory Size Check.................[    ]" );
	if ( isMemoryEnough() == TRUE )
	{
		printStringToScreen( 43, 4, "Pass" );
	}
	else
	{
		printStringToScreen( 43, 4, "Fail" );
		printStringToScreen( 0, 5, "Not Enough Memory~!! MINT64 OS Requires Over 64MByte Memory!" );
		while( 1 );
	}

	// IA-32e 모드의 커널 영역을 초기화
	printStringToScreen( 0, 5, "IA-32e Kernel Area Initialize.............[    ]" );
	if ( initializeKernel64Area() == TRUE )
	{
		printStringToScreen( 43, 5, "Pass" );
	}
	else
	{
		printStringToScreen( 43, 5, "Fail" );
		printStringToScreen( 0, 6, "Kernel Area Initialization Fail!" );
		while( 1 );
	}

	// IA-32e 모드 커널을 위한 페이지 테이블 생성
	printStringToScreen( 0, 6, "IA-32e Page Tables Initialize.............[    ]" );
	initializePageTables();
	printStringToScreen( 43, 6, "Pass" );

	while( 1 );
}

void printStringToScreen( int aX, int aY, const char * aString )
{
	int i = 0;
	CHARACTER * sScreen = (CHARACTER*) 0xB8000; // 비디오 메모리 어드레스

	sScreen += ( aY * 80 ) + aX;
	while( aString[i] != 0 )
	{
		sScreen[i].mCharactor = aString[i];
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

BOOL isMemoryEnough( void )
{
	DWORD * sCurAddr;
	sCurAddr = (DWORD*) 0x100000;

	while( (DWORD) sCurAddr < 0x4000000 )
	{
		*sCurAddr = 0x12345678;

		//0x12345678 로 저장한 후 다시 읽었을 때 같은 값이 나오지 않으면
		// 해당 어드레스를 사용하는 데 문제가 생긴 것이므로 더 이상 진행하지 않고 종료;
		if ( *sCurAddr != 0x12345678 )
		{
			return FALSE;
		}

		// 1MB 씩 이동
		sCurAddr += ( 0x100000 / 4 );
	}

	return TRUE;
}

