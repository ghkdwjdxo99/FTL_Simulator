#include "config.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"
#include "write.h"
#include "init.h"
#include "read.h"
#include "viewer.h"
#include "util.h"

#include <stdlib.h>
#include <time.h>

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
	srand((unsigned int)time(NULL));

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
		else if (cmd == 2)		// meta
		{
			UINT8 option;
			printf("> Select option ( 0 : all bank, 1 : one bank ) : "), scanf("%u", &option);
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

					printf("Press any key if you want to continue...\n");
					printf("Press 'q' to exit.\n");
					if (wait_key_continue() == 0)
						continue; // 'q'
				}

			}
			else if (option == 1)		//  특정 bank만 출력
			{
				UINT8 bank;
				printf("> Input BANK NUMBER (0 ~ 3) : "), scanf("%u", &bank);							
				flush_stdin_line();

				BLOCK_META* target_meta = g_Meta + bank * BLOCK_NUM;
				printf("\n===================================================================\n");
				printf("[Meta Summary] bank = %u\n", bank);
				show_metadata(target_meta);
				printf("===================================================================\n");
			}
		}
		else if (cmd == 3)		// map 
		{
			//squencial_view_map(g_Map);
			UINT32 start_lba;
			UINT32 sector_cnt;
			printf("> 'Start LBA' (Max 8,388,608) : ");
			scanf("%u", &start_lba);
			printf("> Total print 'Sector_cnt' : ");
			scanf("%u", &sector_cnt);
			range_view_map(g_Map, start_lba, sector_cnt);
		}
		else if (cmd == 4)		// cursor
		{
			UINT8 option = 0;
			printf("> Select option ( 0 : all bank, 1 : selected one bank ) : "), scanf("%u", &option);
			flush_stdin_line();

			if (option == 0)		// 전체 bank 출력
			{
				for (UINT8 bank = 0; bank < BANK_NUM; bank++)
				{
					BLOCK_CURSOR* target_cursor = g_Cursor + bank * BLOCK_NUM;
					printf("\n===================================================================\n");
					printf("[CURSOR Summary] bank = %u \n", bank);
					show_cursor(target_cursor);
					printf("===================================================================\n");

					printf("Press any key if you want to continue...\n");
					printf("Press 'q' to exit.\n");
					if (wait_key_continue() == 0)
						continue; // 'q'
				}
			}

			else if (option == 1)		//  특정 bank 만 출력
			{
				UINT8 bank;
				printf("> Input BANK NUMBER (0 ~ 3) : "), scanf("%u", &bank);
				flush_stdin_line();

				BLOCK_CURSOR* target_cursor = g_Cursor + bank * BLOCK_NUM;
				printf("\n===================================================================\n");
				printf("[CURSOR Summary] bank = %u\n", bank);
				show_cursor(target_cursor);
				printf("===================================================================\n");
			}
		}
		else if (cmd == 5)		// seq Write
		{
			UINT32 start_lba;
			UINT32 sector_cnt;
			printf("> 'Start LBA' (Max 8,388,608) : ");		// 몇번 LBA부터 write
			scanf("%u", &start_lba);
			printf("> Total Write 'Sector_cnt' : ");
			scanf("%u", &sector_cnt);

			ftl_write(start_lba, sector_cnt);
			printf("===================================================================\n");
		}
		else if (cmd == 6)		// seq Read
		{
			UINT32 start_lba;
			UINT32 sector_cnt;
			printf("> 'Start LBA' (Max 8,388,608) : ");	// 몇번 LBA부터 Read
			scanf("%u", &start_lba);
			printf("> Total Write 'Sector_cnt' : ");
			scanf("%u", &sector_cnt);

			if (ftl_read(start_lba, sector_cnt, read_buffer) == FALSE) {
				printf("ftl_read failed...\n");
			}
			else {
				view_read_buf(read_buffer, sector_cnt);
			}
		}
		else if (cmd == 7)		// random Write / Read
		{
			UINT32 wr_count = 0;

			while (1) {
				// User Input (q)이 들어오면 random test 종료
				if (is_user_interrupt())
					break;

				// 랜덤 write 시작 LBA 선택
				UINT32 rand_start_lba = rand() % TOTAL_SECTORS;
				
				// 주소 범위 기준 최대 sector 개수 계산
				// start_lba부터 끝(TOTAL_SECTORS)까지 범위를 넘지 않도록 제한
				UINT32 max_cnt = TOTAL_SECTORS - rand_start_lba;

				// read_buffer 용량 기준 최대 sector 개수
				// read_buffer 크기 = PAGE_NUM * PAGE_SIZE (bytes)
				// sector 1개 = 8 bytes
				UINT32 buf_max_cnt = (PAGE_NUM * PAGE_SIZE) / 8;

				// 실제로 사용할 sector_cnt 최대치 결정
				// - 주소 범위 제한
				// - 버퍼 용량 제한
				// 위 두 조건 중 더 작은 값으로 제한
				UINT32 limit = (max_cnt < buf_max_cnt) ? max_cnt : buf_max_cnt;
				if (limit == 0)
					limit = 1;

				// 랜덤 sector 개수 결정 (범위: 1 ~ limit)
				UINT32 rand_sector_cnt = (rand() % limit) + 1;	// random sector count
				UINT32 end_lba = rand_start_lba + rand_sector_cnt - 1;	// 출력용 마지막 LBA
				UINT32 read_verify_result = TRUE;	// 출력용 read 검증 결과

				printf("start_lba : %u, sector_cnt : %u\n", rand_start_lba, rand_sector_cnt);
				ftl_random_write(rand_start_lba, rand_sector_cnt, wr_count);

				UINT32 loop_cnt = (rand_sector_cnt + MAX_SECTORS_PER_PAGE - 1) / MAX_SECTORS_PER_PAGE;
				for (UINT32 cnt = 0; cnt < loop_cnt; cnt++) {
					UINT32 start_lba_loop = rand_start_lba + MAX_SECTORS_PER_PAGE * cnt;
					UINT32 sector_cnt_loop = rand_sector_cnt - (MAX_SECTORS_PER_PAGE * cnt);
					if (sector_cnt_loop > MAX_SECTORS_PER_PAGE)
						sector_cnt_loop = MAX_SECTORS_PER_PAGE;
					if(ftl_read(start_lba_loop, sector_cnt_loop, read_buffer) == FALSE)
						read_verify_result = FALSE;
					else {
						UINT32 new_PBA = get_pba(g_Map, rand_start_lba);
						read_verify_result = verify_page_buf(read_buffer, sector_cnt_loop, new_PBA);
					}
				}

				UINT32 new_PBA = get_pba(g_Map, rand_start_lba);
				log_rand_wr(READ, wr_count, rand_start_lba, end_lba, new_PBA, read_verify_result);

				wr_count++;
			}
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