#include "write.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"
#include <time.h>


/* =========================== FTL ============================*/
// Page에 8 Byte 데이터 쓰기
BOOL write_page(UINT32 start_lba, UINT32 size, UINT32 timestamp)
{
	char* buf = 0;
	put_page_data(buf, start_lba, size, timestamp);

	char* path = 0;
	write_file(path, buf, size);

}


BOOL write(UINT32 startLBA, UINT32 sector_cnt)
{
	UINT8 bank, block, page;
	BLOCK_CURSOR* cur_cursor = g_Cursor;
	UINT32 block_offset;

	for (UINT32 sector_offset = 0; sector_offset < sector_cnt; sector_offset++)
	{
		UINT32 targetLBA = startLBA + sector_offset;

		/* ========== (0) LBA와 매칭되는 pba 찾기 ========== */
		UINT16 PBA = 0;
		get_pba(g_Map, targetLBA, &PBA);

		/* ========== (1) PBA를 bank, block, page로 분리 ========== */
		bank = (PBA >> 14) & 0x03;
		block = (PBA >> 7) & 0x7F;
		page = (PBA) & 0x7F;
		
		/* ========== (2) write 할 수 있는 page 찾기 ========== */
		while (1)
		{
			block_offset = bank * BLOCK_NUM + block;

			cur_cursor = g_Cursor + block_offset;

			UINT8 cur_page = get_block_write_page_idx(cur_cursor);
			if (cur_page == PAGE_FULL)		// 현재 block의 모든 page가 가득 차있으면 다음 block으로 변경
			{
				block += 1;
				if (block >= BLOCK_NUM)		// block개수 넘어가면 다음 bank로 이동
					bank += 1;
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
		PBA |= (bank & 0x03) << 14;			// 상위 2비트
		PBA |= (block & 0x7F) << 7;			// 중간 7비트
		PBA |= (page & 0x7F);				// 하위 7비트

		/* ========== (4) map 업데이트 ========== */
		set_pba(g_Map, targetLBA, PBA);
	}

	/* ========== (5) page에 write ========== */
	UINT8 page_buf[PAGE_SIZE] = { 0 };
	UINT32 timestamp = GetTickCount64();

	put_page_data(page_buf, startLBA, sector_cnt, timestamp);

	/* ========== (6) metadata 업데이트 ========== */
	//UINT32 block_offset = bank * BLOCK_NUM + block;

	BLOCK_META* target_metadata = g_Meta + block_offset;
	update_validBitmap_one(target_metadata, page, sector_cnt);
	update_BlockState(target_metadata);

	/* ========== (7) cursor 업데이트 ========== */
	set_cursor_next_page(cur_cursor);

	return TRUE;
}