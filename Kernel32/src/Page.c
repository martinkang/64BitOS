#include "Page.h"

// IA-32e 모드 커널을 위한 페이지 테이블 생성
void initializePageTables( void )
{
	int i = 0;

	PML4TblEntry       * sPML4TblEntry; // PML4 Entry
	PageDirPtrTblEntry * sPageDirPtrTblEntry;  // Page Directory Pointer Table Entry
	PageDirEntry       * sPageDirEntry;    // Page Directory Entry
	DWORD		         sDwMappingAddr; // 

	// PML4 테이블 생성
	// 첫 번째 엔트리 외에 나머지는 모두 0 으로 초기화
	sPML4TblEntry = (PML4TblEntry*) 0x100000;
	setPageEntryData( &( sPML4TblEntry[0] ),  // Page Table Entry
						 0x00, 				  // UpperBaseAddress
						 0x101000, 			  // LowerBaseAddress
						 PAGE_FLAGS_DEFAULT,  // LowerFlags
						 0 );				  // Upperflags

	for ( i = 1; i < PAGE_MAXENTRYCOUNT; i++ )
	{
		setPageEntryData( &( sPML4TblEntry[i] ),
						  0,
						  0,
						  0,
						  0 );
	}

	// 페이지 디렉터리 포인터 테이블 생성
	// 하나의 PDPT 로 512GB 까지 매핑 가능하므로 하나로 충분함
	// 64 개의 엔트리를 설정하여 64GB 까지 매핑함
	sPageDirPtrTblEntry = (PageDirPtrTblEntry*) 0x101000;
	for( i = 0; i < 64; i++ )
	{
		setPageEntryData( &( sPageDirPtrTblEntry[i] ),
						  0,
						  0x102000 + ( i * PAGE_TABLESIZE ),
						  PAGE_FLAGS_DEFAULT,
						  0 );
	}


	for ( i = 64; i < PAGE_MAXENTRYCOUNT; i++ )
	{
		setPageEntryData( &( sPageDirPtrTblEntry[i] ),
						  0,
						  0,
						  0,
						  0 );
	}

	/* 페이지 디렉터리 테이블 생성
	 * 하나의 페이지 디렉터리가 1GB 가지 매핑 가능
	 * 여유있게 64 개의 페이지 디렉터리를 생성하여 총 64GB 가지 지원 */
	sPageDirEntry = (PageDirEntry*) 0x102000;
	sDwMappingAddr = 0;
	for( i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++ )
	{
		// 32 비트로는 사우이 어드레스를 표현할 수 없으므로, MB 단위로 계산한 다음
		// 최종 결과를 다시 4KB 로 나누어 32 비트 이상의 어드레스를 계산함
		setPageEntryData( &( sPageDirEntry[i] ),
			     		   ( i * ( PAGE_DEFAULTSIZE >> 20 ) ) >> 12,
						   sDwMappingAddr,
						   PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );
		sDwMappingAddr += PAGE_DEFAULTSIZE;
	}
}
	
// 페이지 엔트리에 기준 주소와 속성 플래그를 설정
void setPageEntryData( PageTblEntry * aPageTblEntry, 
				       DWORD          aDwUpperBaseAddr,
					   DWORD          aDwLowerBaseAddr,
	   				   DWORD          aDwLowerFlags,
					   DWORD          aDwUpperFlags )
{
	aPageTblEntry->mDwAttributeAndLowerBaseAddr = aDwLowerBaseAddr | aDwLowerFlags;
	aPageTblEntry->mDwUpperBaseAddrAndEXB = ( aDwUpperBaseAddr & 0xFF ) | aDwUpperFlags;
}

