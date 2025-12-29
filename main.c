#include "config.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"
#include "write.h"
#include "read.h"

// 메모리 정의
static UINT8 G_MEM[1024 * 1024 * 18];

// 메모리 포인터 선언
BLOCK_META*		const g_Meta	= (BLOCK_META*)	  G_MEM;
MAP_ADDR*		const g_Map		= (MAP_ADDR*)	 (G_MEM + TOTAL_META_SIZE);
BLOCK_CURSOR*	const g_Cursor	= (BLOCK_CURSOR*)(G_MEM + TOTAL_META_SIZE + TOTAL_MAP_SIZE);

int main(void)
{
	
	printf("sizeof(g_Meta) : %llu\n", TOTAL_META_SIZE);
	printf("sizeof(g_Map) : %llu\n", TOTAL_MAP_SIZE);
	printf("sizeof(g_Cursor) : %llu\n", TOTAL_CURSOR_SIZE);
	printf("Total G_MEM size : %llu\n", TOTAL_META_SIZE+ TOTAL_MAP_SIZE +TOTAL_CURSOR_SIZE);
	
	if (!load_metadata("metadata.bin", g_Meta))
	{
		printf("[INFO] \"metadata.bin\" not found.\n");
		init_metadata(g_Meta);
	}
	if (!load_map_address("map.bin", g_Map))
	{
		printf("[INFO] \"map.bin\" not found.\n");
		init_map_addr_zero(g_Map);
	}
	if (!load_cursor("cursor.bin", g_Cursor))
	{
		printf("[INFO] \"cursor.bin\" not found.\n");
		init_cursor(g_Cursor);
	}

	UINT32 startLBA = 0;
	UINT32 sector_cnt = 100;

	//write(startLBA, sector_cnt);



	return 0;
}