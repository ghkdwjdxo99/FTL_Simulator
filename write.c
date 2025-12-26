#include "write.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"
#include <time.h>


/* =========================== FTL ============================*/
// page path 찾기
void get_page_path_from_lba(UINT32 targetLBA, char* out_path)
{
	UINT8 bank, block, page = 0;

	UINT16 PBA = get_pba(g_Map, targetLBA);
	bank = (PBA >> 14) & 0x03;
	block = (PBA >> 7) & 0x7F;
	page = (PBA) & 0x7F;

	//char page_path[16];
	sprintf(out_path, "%c\\%u\\P%u.bin", 'A' + bank, block, page);
}

// Page에 8 Byte 데이터 쓰기
BOOL write_page(UINT32 startLBA, UINT32 sector_cnt)
{
	UINT8 bank, block, page = 0;

	if (sector_cnt < DATA_PER_PAGE)
	{
		UINT8 data_buf[PAGE_SIZE] = { 0 };
		UINT32 timestamp = GetTickCount64();
		put_total_data(data_buf, startLBA, sector_cnt, timestamp);		// sector_cnt만큼의 write할 8바이트 데이터를 buf에 만들기

		char page_path[16];
		get_page_path_from_lba(startLBA, &page_path);		// 현재 LBA에 해당하는 page의 path 가져오기

		write_file(page_path, data_buf, sector_cnt);	// 해당 path에 data_buf에 있는 데이터를 쓰기
	}
}

BOOL write(UINT32 startLBA, UINT32 sector_cnt)
{
	UINT8 bank, block, page = 0;
	BLOCK_CURSOR* cur_cursor = NULL;
	UINT8 cur_page = NULL; 
	UINT32 block_offset = NULL;

	for (UINT32 sector_offset = 0; sector_offset < sector_cnt; sector_offset++)
	{
		UINT32 targetLBA = startLBA + sector_offset;

		/* ========== (0) LBA와 매칭되는 pba 찾기 ========== */
		UINT16 PBA = get_pba(g_Map, targetLBA);

		/* ========== (1) PBA를 bank, block, page로 분리 ========== */
		bank = (PBA >> 14) & 0x03;
		block = (PBA >> 7) & 0x7F;
		page = (PBA) & 0x7F;
		
		/* ========== (2) write 할 수 있는 page 찾기 ========== */
		while (1)
		{
			block_offset = bank * BLOCK_NUM + block;

			cur_cursor = g_Cursor + block_offset;

			cur_page = get_block_write_page_idx(cur_cursor);
			if (cur_page == PAGE_FULL)		// 현재 block의 모든 page가 가득 차있으면 다음 block으로 변경
			{
				block ++;

				if (block >= BLOCK_NUM)		// block개수 넘어가면 다음 bank로 이동
				{
					bank ++;
					block = 0;
				}
				if (bank >= BANK_NUM)		// bank 개수가 최대 넘어가는 경우
					bank = BANK_NUM - 1;
			}

			else                 // 현재 block에 write할 수 있는 page가 있는 경우
			{
				page = cur_page;		// page 변경
				break;
			}
		}

		/* ========== (3) bank, block, page를 PBA로 합치기 ========== */
		PBA =	((bank & 0x03) << 14) |			// 상위 2비트
				((block & 0x7F) << 7) |			// 중간 7비트
				((page & 0x7F));				// 하위 7비트

		/* ========== (4) map 업데이트 ========== */
		set_pba(g_Map, targetLBA, PBA);
	}

	/* ========== (5) Page.bin에 write ========== */
	write_page(startLBA, sector_cnt);

	/* ========== (6) metadata 업데이트 ========== */
	BLOCK_META* target_metadata = g_Meta + block_offset;
	update_validBitmap_one(target_metadata, page, sector_cnt);
	update_BlockState(target_metadata);

	/* ========== (7) cursor 업데이트 ========== */
	set_cursor_next_page(cur_cursor);

	return TRUE;
}