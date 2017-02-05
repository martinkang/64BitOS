#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

#define EXCEPTION_END __EXCEPTION_END:

#define TEST( __condition ) \
	if ( __condition == 1 ) \
		goto __EXCEPTION_END;


int openFile( char * aPath, int aFlags, int aMode );
int adjustInSectorSize( int aFd, int aSrcSize );
int writeKernelInformation( int aTargetFd, 
							int aTotalKernelSectorCnt,
							int aKernel32SectoerCnt);
int copyFile( int aSrcFd, int aTargetFd );

int main( int argc, char * argv[] )
{
	int sSrcFd             = -1;
	int sTargetFd          = -1;
	int sBootLoaderSize    = 0;
	int sKernel32SectorCnt = 0;	
	int sKernel64SectorCnt = 0;
	int sSrcSize           = 0;
	int sTargetOpen        = 0;
	int sSrcOpen           = 0;

	if ( argc < 4 )
	{
		fprintf( stderr, "[ERROR] ImageMaker.exe BootLoader.bin Kernel32.bin Kernel64.bin\n" );
		exit( -1 );
	}
	else
	{
		/* noting to do */
	}

	// Disk.img 파일을 생성
	sTargetFd = openFile( "Disk.img", O_RDWR | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE );
	TEST( sTargetFd == -1 );
	sTargetOpen = 1;

	printf( "[INFO] Copy boot loader to image file\n" );
	sSrcFd = openFile( argv[1], O_RDONLY | O_BINARY, -1 );
	TEST( sSrcFd == -1 );
	sSrcOpen = 1;

	sSrcSize = copyFile( sSrcFd, sTargetFd );
	sSrcOpen = 0;
	close( sSrcFd );


	sBootLoaderSize = adjustInSectorSize( sTargetFd, sSrcSize );
	printf( "[INFO] %s Size = [%d] and sector count =  [%d]\n",
			argv[1], sSrcSize, sBootLoaderSize );

	printf( "[INFO] Copy protected mode kernel to image file\n" );
	sSrcFd = openFile( argv[2], O_RDONLY | O_BINARY, -1 );
	TEST( sSrcFd == -1 );
	sSrcOpen = 1;

	sSrcSize = copyFile( sSrcFd, sTargetFd );
	sSrcOpen = 0;
	close( sSrcFd );

	// 파일 크기를 섹터 크기인 512 바이트로 밪추기 위해 나머지 부분을 0x00 으로 채움
	sKernel32SectorCnt = adjustInSectorSize( sTargetFd, sSrcSize );
	TEST( sKernel32SectorCnt == -1 );
	printf( "[INFO] %s size = [%d] and sector count =  [%d]\n",
			argv[2], sSrcSize, sKernel32SectorCnt );

	
	//------------------------------------------------------------------
	// 64 비트 커널 파일을 열어서 모든 내용을 디스크 이미지 파일로 복사
	//------------------------------------------------------------------
	sTargetFd = open( "Disk.img", O_RDWR | O_CREATE | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE );
	TEST( sTargetFd == -1 );
	sSrcOpen = 1;

	sSrcSize = copyFile( sSrcFd, sTargetFd );
	sSrcOpen = 0;
	close( sSrcFd );
	
	// 파일 크기를 섹터 크기인 512 바이트로 밪추기 위해 나머지 부분을 0x00 으로 채움
	sKernel64SectorCnt = adjustInSectorSize( sTargetFd, sSrcSize );
	TEST( sKernel32SectorCnt == -1 );
	printf( "[INFO] %s size = [%d] and sector count =  [%d]\n",
			argv[3], sSrcSize, sKernel64SectorCnt );

	//------------------------------------------------------------------
	// 디스크 이미지에 커널 정보를 갱신
	//------------------------------------------------------------------
	printf( "[INFO] Start to write kernel information\n" );
	TEST( writeKernelInformation( sTargetFd, 
								  sKernel32SectorCnt + sKernel64SectorCnt,
								  sKernel32SectorCnt ) == -1 );
	printf( "[INFO] Image file create complete\n" );

	sTargetOpen = 0;
	close( sTargetFd );

	return 0;

	EXCEPTION_END;

	if ( sTargetOpen == 1 )
	{
		close( sTargetFd );
		sTargetOpen = 0;
	}
	else
	{
		/* nothing to do */
	}

	if ( sSrcOpen == 1 )
	{
		close( sSrcFd );
		sSrcOpen = 0;
	}
	else
	{
		/* nothing to do */
	}

	return -1;
}

int openFile( char * aPath, int aFlag, int aMode )
{
	int sFd = -1;

	if ( aMode == -1 )
	{
		sFd = open( aPath, aFlag );
	}
	else
	{
		sFd = open( aPath, aFlag, aMode );
	}

	if ( sFd == -1 )
	{
		fprintf( stderr, "[ERROR] %s open fail\n", aPath );
	}
	else
	{
		/* nothing to do */
	}

	return sFd;
}

int adjustInSectorSize( int aFd, int aSrcSize )
{
	int i = 0;
	int sAdjustSizeToSector = 0;
	int sSectorCnt = 0;
	char sCh = 0;

	sAdjustSizeToSector = aSrcSize % BYTESOFSECTOR;
	sCh = 0x00;

	if ( sAdjustSizeToSector != 0 )
	{
		sAdjustSizeToSector = 512 - sAdjustSizeToSector;
		printf( "[INFO] File size [%lu] and fill [%u] byte\n", 
				aSrcSize, sAdjustSizeToSector );

		for ( i = 0; i < sAdjustSizeToSector; i++ )
		{
			write( aFd, &sCh, 1 );
		}
	}
	else
	{
		printf( "[INFO] File size is aligned 512 byte\n" );
	}

	sSectorCnt = ( aSrcSize + sAdjustSizeToSector ) / BYTESOFSECTOR;
	return sSectorCnt;
}

int writeKernelInformation( int aTargetFd, 
						    int aTotalKernelSectorCnt,
						    int aKernel32SectorCnt)
{
	unsigned short sData = 0;
	long sPosition = 0;
	int sRC = 0;

	sPosition = lseek( aTargetFd, 5, SEEK_SET );
	if ( sPosition == -1 )
	{
		fprintf( stderr, "lseek fail. Return value = %d, errno = %d, %d\n",
				sPosition, errno, SEEK_SET );
		sRC = -1;
	}
	else
	{
		sData = (unsigned short) aKernelSectorCnt;
		write( aTargetFd, &sData, 2 );

		printf( "[INFO] Total sector count except boot loader [%d]\n",
				aTotalKernelSectorCnt );

		sRC = 0;
	}

	sData = (unsigned short)aTotalKernelSectorCnt;
	write( aTargetFd, &sData, 2 );
	sData = (unsigned short)aKernel32SectorCnt;
	write( aTargetFd, &sData, 2 );
	printf( "[INFO] Total sector count of protected mode kernel [%d]\n",
				aKernel32SectorCnt );

	return sRC;
}

int copyFile( int aSrcFd, int aTargetFd )
{
	int sSrcFileSize = 0;
	int sRead = 0;
	int sWrite = 0;
	char sBuf[BYTESOFSECTOR] = {0, };

	sSrcFileSize = 0;
	while( 1 )
	{
		sRead = read( aSrcFd, sBuf, sizeof( sBuf ) );
		sWrite = write( aTargetFd, sBuf, sRead );

		if ( sRead != sWrite )
		{
			fprintf( stderr, "[ERROR] sRead != sWrite..\n" );
			sSrcFileSize = -1;
			break;
		}
		else
		{
			/* noting to do */
		}

		sSrcFileSize += sRead;

		if ( sRead != sizeof( sBuf ) )
		{
			break;
		}
		else
		{
			/* nothing to do */
		}
	}
	return sSrcFileSize;
}
