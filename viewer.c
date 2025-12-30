#include "viewer.h"

/* 사용자 입력 관련 함수 */
int get_any_key() {
	return _getch();
}

int wait_key_continue() {
	int ch = get_any_key();

	if (ch == 'q' || ch == 'Q')
		return 0;
	return 1;
}

/* 출력 헬퍼 함수 */


/*
// map 출력 함수
void sequential_view_map(const MAP_ADDR* map_addr_base) {
	// 처음부터 끝까지 모든 map addr의 LBA, PBA를 출력
	// row_cnt 단위로 출력 여부를 확인한다.
	UINT32 entry_cnt = (UINT32)(TOTAL_MAP_SIZE / sizeof(MAP_ADDR));

	const UINT32 col_cnt = 4;
	const UINT32 row_cnt = col_cnt * 10;

	UINT32 printed = 0;

	for (UINT32 lba = 0; lba < entry_cnt; lba++) {
		UINT16 pba = *(map_addr_base + lba);

		UINT32 bank, block, page;
		pba_to_bank_block_page(pba, &bank, &block, &page);

		printf("LBA: %3u, PBA: 0x%04X (%u %u %u)|",
			(unsigned)lba,
			(unsigned)pba,
			bank, block, page);

		printed++;

		// col_cnt개씩 한 줄에 찍고 개행
		if ((printed % col_cnt) == 0)
			printf("\n");

		if ((printed % row_cnt) == 0) {
			printf("Press any key if you want to continue...\n");
			printf("Press 'q' to exit.\n");
			if (wait_key_continue() == 0)
				return;	// 'q'면 출력 종료
		}
	}
}
*/

void range_view_map(const MAP_ADDR* map_addr_base, UINT32 start_lba, UINT32 sector_cnt) {
	// 특정 범위의 map addr의 LBA, PBA를 출력
	UINT32 entry_cnt = (UINT32)(TOTAL_MAP_SIZE / sizeof(MAP_ADDR));

	// 범위 보정
	if (start_lba >= entry_cnt)
		return;

	UINT32 end_lba = start_lba + sector_cnt;
	if (end_lba > entry_cnt)
		end_lba = entry_cnt;

	const UINT32 col_cnt = 4;
	const UINT32 row_cnt = col_cnt * 10;

	UINT32 printed = 0;

	for (UINT32 lba = start_lba; lba < end_lba; lba++)
	{
		UINT16 pba = *(map_addr_base + lba);

		UINT32 bank, block, page;
		pba_to_bank_block_page(pba, &bank, &block, &page);

		printf("LBA: %3u, PBA: 0x%04X (%u %u %u)|",
			(unsigned)lba,
			(unsigned)pba,
			bank, block, page);

		printed++;

		// col_cnt개씩 한 줄
		if ((printed % col_cnt) == 0)
			printf("\n");

		// row_cnt개 출력마다 pause
		if ((printed % row_cnt) == 0)
		{
			printf("Press any key if you want to continue...\n");
			printf("Press 'q' to exit.\n");
			if (wait_key_continue() == 0)
				return; // 'q'
		}
	}

	// 마지막 줄 개행 보정
	if ((printed % col_cnt) != 0)
		printf("\n");
}