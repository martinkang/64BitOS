#ifndef __PAGE_H__
#define __PAGE_H__ 1

#include "Types.h"

#define PAGE_FLAGS_P       0x00000001 // Present
#define PAGE_FLAGS_RW      0x00000002 // Read/Write
#define PAGE_FLAGS_US      0x00000004 // User/Supervisor ( 플래그 설정 시 유저 레벨 )
#define PAGE_FLAGS_PWT     0x00000008 // Page Level Write-through
#define PAGE_FLAGS_PCD     0x00000010 // Page Lebel Cache Disable
#define PAGE_FLAGS_A       0x00000020 // Accessed
#define PAGE_FLAGS_D       0x00000040 // Dirty
#define PAGE_FLAGS_PS      0x00000080 // Page Size
#define PAGE_FLAGS_G       0x00000100 // Global
#define PAGE_FLAGS_PAT     0x00001000 // Page Attribute Table Index
#define PAGE_FLAGS_EXB     0x80000000 // Execute Disable bit
#define PAGE_FLAGS_DEFAULT ( PAGE_FLAGS_P | PAGE_FLAGS_RW )
#define PAGE_TABLESIZE     0x1000
#define PAGE_MAXENTRYCOUNT 512
#define PAGE_DEFAULTSIZE   0x200000

#pragma pack( push, 1 )

// 페이지 엔트리에 대한 자료구조
typedef struct pageTableEntryStruct
{
	/* PML4T 와 PDPTE 의 경우
	 *  - 1 비트 P, RW, US, PWT, PCD, A, 3 비트 Reserbed, 3 비트 Avail, 20 비트 Base Address
	 * PDE 의 경우
	 *  - 1 비트 P, RW, US, PWT, PCD, A, D, 1, G, 3 비트 Avail, 1 비트 PAT, 8 비트 Avail, 11 비트 Base Address */
	DWORD mDwAttributeAndLowerBaseAddr;
	// 8 비트 Upper BaseAddress, 12 비트 Reserved, 11 비트 Avail, 1 비트 EXB 
	DWORD mDwUpperBaseAddrAndEXB;
} PML4TblEntry, PageDirPtrTblEntry, PageDirEntry, PageTblEntry;
#pragma pack( pop )

// 함수
void initializePageTables( void );
void setPageEntryData( PageTblEntry * aPageTblEntry, 
					   DWORD     	  aDwUpperBaseAddr,
					   DWORD 		  aDwLowerBaseAddr,
					   DWORD 	 	  aDwLowerFlags,
					   DWORD	 	  aDwUpperFlags );

#endif
