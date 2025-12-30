#include "config.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"
#include "write.h"
#include "init.h"
#include "read.h"

// 메모리 정의
static UINT8 G_MEM[1024 * 1024 * 18];

// 메모리 포인터 선언
BLOCK_META*		const g_Meta	= (BLOCK_META*)	  G_MEM;
MAP_ADDR*		const g_Map		= (MAP_ADDR*)	 (G_MEM + TOTAL_META_SIZE);
BLOCK_CURSOR*	const g_Cursor	= (BLOCK_CURSOR*)(G_MEM + TOTAL_META_SIZE + TOTAL_MAP_SIZE);

static void flush_stdin_line(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}
}

static void wait_enter(void)
{
	int c;
	printf("\nPress ENTER to continue...");
	fflush(stdout);

	// 엔터(\n)가 나올 때까지 무조건 대기
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

UINT8 select_menu(UINT8 cmd)					/* 2. 메뉴 선택 */
{
	printf("\n---------------------- Menu -----------------------------\n");
	printf("| 1) Make METADATA (RESET)				|\n");
	printf("| 2) View METADATA					|\n");
	printf("| 3) View Map Address					|\n");
	printf("| 4) View Cursor					|\n");
	printf("| 5) seq WRITE						|\n");
	printf("| 6) seq READ						|\n");
	printf("| 7) random WRITE / READ				|\n");
	printf("| 8) flush (metadata store)				|\n");
	printf("| 0) EXIT						|\n");
	printf("---------------------------------------------------------\n");
 	printf("\n> Select menu number : "), scanf("%u", &cmd);
	return cmd;
}

int main(void)
{
/*	printf("sizeof(g_Meta) : %llu bytes\n", TOTAL_META_SIZE);
	printf("sizeof(g_Map) : %llu bytes\n", TOTAL_MAP_SIZE);
	printf("sizeof(g_Cursor) : %llu bytes\n", TOTAL_CURSOR_SIZE);
	printf("Total G_MEM size : %llu bytes\n", TOTAL_META_SIZE+ TOTAL_MAP_SIZE +TOTAL_CURSOR_SIZE);
	*/	
	load_bin_files(g_Meta, g_Map, g_Cursor);
	create_nand_dirs();

	UINT8 cmd = 0;

	char read_buffer[PAGE_NUM * PAGE_SIZE];
	while (1)
	{
		cmd = select_menu(cmd);
		if (cmd == 1)
		{
			if (reset_bin_files(g_Meta, g_Map, g_Cursor))
			{
				save_bin_files(g_Meta, g_Map, g_Cursor);
				reset_nand_dirs();
				printf("RESET SUCCESS (metadata.bin , map.bin, cursor.bin)\n");
			}
		}
		else if (cmd == 2)
		{
			UINT8 option = 0;
			printf("> Select option ( 0 : total bank, 1 : one bank ) : "), scanf("%u", &option);
			flush_stdin_line();

			if (option == 0)		// 전체 bank 출력
			{
				for (UINT8 bank = 0; bank < BANK_NUM; bank++)
				{
					BLOCK_META* target_meta = g_Meta + bank * BLOCK_NUM;
					printf("\n===================================================================\n");
					printf("[Meta Summary] bank = %u\n", bank);
					show_metadata(target_meta);
					printf("===================================================================\n");

					wait_enter();
				}

			}
			else if (option == 1)		//  특정 bank만 출력
			{
				UINT8 bank = 0;
				printf("> Input BANK NUMBER (0 ~ 3) : "), scanf("%u", &bank);							
				flush_stdin_line();

				BLOCK_META* target_meta = g_Meta + bank * BLOCK_NUM;
				printf("\n===================================================================\n");
				printf("[Meta Summary] bank = %u\n", bank);
				show_metadata(target_meta);
				printf("===================================================================\n");
			}
		}
		else if (cmd == 3)
		{

		}
		else if (cmd == 4)
		{

		}
		else if (cmd == 5)		// seq Write
		{
			UINT32 startLBA;
			UINT32 sector_cnt;
			printf("> 'Start LBA' (Max 8,388,608) : ");		// 몇번 LBA부터 write
			scanf("%u", &startLBA);
			printf("> Total Write 'Sector_cnt' : ");
			scanf("%u", &sector_cnt);

			ftl_write(startLBA, sector_cnt);
			printf("===================================================================\n");
		}
		else if (cmd == 6)		// seq Read
		{
			UINT32 startLBA;
			UINT32 sector_cnt;
			printf("> 'Start LBA' (Max 8,388,608) : ");	// 몇번 LBA부터 Read
			scanf("%u", &startLBA);
			printf("> Total Write 'Sector_cnt' : ");
			scanf("%u", &sector_cnt);

			if (ftl_read(startLBA, sector_cnt, read_buffer) == FALSE) {
				printf("ftl_read failed...\n");
			}
			/*else {

			}*/
		}
		else if (cmd == 7)		// random Write / Read
		{

		}
		else if (cmd == 8)		// flush
		{
			if (save_bin_files(g_Meta, g_Map, g_Cursor))
			{
				printf("SAVE SUCCESS (metadata.bin , map.bin, cursor.bin)\n");
			}

		}

		else if (cmd == 0)
		{
			break;
		}
	}


	return 0;
}