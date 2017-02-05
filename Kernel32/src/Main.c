#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void printStringToScreen( int aX, int aY, const char * aPcString );
BOOL initializeKernel64Area( void );
BOOL isMemoryEnough( void );
void copyKernel64ImageTo2Mbyte( void );

// 아래 함수는 C 언어 커널의 시작 부분임
// 반드시 다른 함수들 보다 가장 앞쪽에 존재해야 함
void Main( void )
{
	DWORD sDwEAX, sDwEBX, sDwECX, sDwEDX;
	char sVendorStr[13] = { 0, };

	printStringToScreen( 0, 3, "Protected Mode C Language Kernel Start....[Pass]" );

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

	// 프로세서 제조사 정보 읽기
	readCPUID( 0x00,
			   &sDwEAX,
			   &sDwEBX,
			   &sDwECX,
			   &sDwEDX );
	*(DWORD*)sVendorStr = sDwEBX;
	*((DWORD*)sVendorStr + 1 ) = sDwEDX;
	*((DWORD*)sVendorStr + 2 ) = sDwECX;
	printStringToScreen( 0, 7, "Processor Vendor String...................[              ]" );
	printStringToScreen( 43, 8, sVendorStr );

	readCPUID( 0x80000001,
			   &sDwEAX,
			   &sDwEBX,
			   &sDwECX,
			   &sDwEDX );
	printStringToScreen( 0, 8, "64Bit Mode Support Check..................[    ]" );
	if ( sDwEDX & ( 1 << 29 ) )
	{
		printStringToScreen( 43, 8, "Pass" );
	}
	else
	{
		printStringToScreen( 43, 8, "Fail" );
		printStringToScreen( 0, 9, "This processor does not support 64bit mode" );
		while(1);
	}


	// IA-32e 모드 커널을 0x200000 (2Mbyte) 어드레스로 이동
	printStringToScreen( 0, 9, "Copy IA-32e Kernel To 2M Address..........[    ]" );
	copyKernel64ImageTo2Mbyte();
	printStringToScreen( 43, 9, "Pass" );
	printStringToScreen( 0, 10, "Switch To IA-32e Mode" );

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

void copyKernel64ImageTo2Mbyte()
{
	int i = 0;
	WORD sKernel32SectorCnt = 0;
	WORD sTotalKernelSectorCnt = 0;
	DWORD * sSrcAddr = NULL;
	DWORD * sDestAddr = NULL;

	// 0x7C05 에 총 커널 섹터 수, 0x7C07 에 보호 모드 커널 섹터 수가 들어 있음
	sTotalKernelSectorCnt = *((WORD*)0x7C05);
	sKernel32SectorCnt = *((WORD*)0x7C07);

	sSrcAddr = (DWORD*)( 0x10000 + ( sKernel32SectorCnt * 512 ) );
	sDestAddr = (DWORD*)0x200000;

	// IA-32e 모드 커널 섹터 크기만큼 복사
	for( i = 0; i < 512 * ( sTotalKernelSectorCnt - sKernel32SectorCnt ) / 4; i++ )
	{
		sDestAddr = sSrcAddr;
		sDestAddr++;
		sSrcAddr++;
	}

}

