#include "write.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"
#include <time.h>

// 8 Byte 데이터 만들어서 data_buf에 저장
BOOL put_sector_data(UINT8* buf, UINT32 lba, UINT32 timestamp)
{
	UINT64 data = ((UINT64)lba << 32) | ((UINT64)timestamp);
	memcpy(buf, &data, 8);

	return TRUE;
}

// 8 Byte 데이터를 size만큼 만들어서 data_buf에 저장
BOOL put_total_data(UINT8* data_buf, UINT32 start_lba, UINT32 sector_cnt, UINT32 timestamp)
{
	for (UINT8 s = 0; s < sector_cnt; s++)
	{
		UINT32 offset = DATA_WRITE_SIZE * s;
		UINT32 lba = start_lba + s;

		put_sector_data(data_buf + offset, lba, timestamp);
	}
	return TRUE;
}

// page path 찾기
void get_page_path_from_lba(UINT32 targetLBA, char* out_path)
{
	UINT16 PBA = get_pba(g_Map, targetLBA);

	UINT8 bank = (PBA >> 14) & 0x03;
	UINT8 block = (PBA >> 7) & 0x7F;
	UINT8 page = (PBA) & 0x7F;

	sprintf(out_path, "%c\\%u\\P%u.bin", 'A' + bank, block, page);
}

// 한 Page에 8 Byte 데이터 쓰기
BOOL write_single_page(UINT32 firstLBA, UINT32 sectors_in_page)		// firstLBA : 한 page에 write할 가장 첫번째 lba 번호, sectors_in_page : 한 page에 write할 sector의 개수
{
	UINT8 data_buf[PAGE_SIZE] = { 0 };

	UINT32 timestamp = GetTickCount64();
	put_total_data(data_buf, firstLBA, sectors_in_page, timestamp);		// sector_cnt만큼의 write할 8바이트 데이터를 buf에 만들기

	char page_path[16];
	get_page_path_from_lba(firstLBA, &page_path);		// 현재 LBA에 해당하는 page의 path 가져오기

	write_file(page_path, data_buf, sectors_in_page);	// 해당 path에 data_buf에 있는 데이터를 쓰기
}

//// sector_cnt에 따른 page 개수만큼 write 
//BOOL process_write_pages(UINT32 startLBA, UINT32 sector_cnt)
//{
//	UINT8 page_cnt = (sector_cnt + MAX_SECTOR_PER_PAGE - 1) / MAX_SECTOR_PER_PAGE;		// 써야하는 page 개수
//
//	for (UINT8 page_offset = 0; page_offset < page_cnt; page_offset++)
//	{
//		UINT32 firstLBA_in_page = startLBA + MAX_SECTOR_PER_PAGE * page_offset;			// 한 page 기준 첫번째 lba
//		UINT32 sectors_in_page = (sector_cnt > MAX_SECTOR_PER_PAGE ? MAX_SECTOR_PER_PAGE : sector_cnt);		// 한 page 기준 write하는 sector 개수
//
//		if (firstLBA_in_page > sector_cnt)
//		{
//			firstLBA_in_page = sector_cnt - 1;
//		}
//
//		write_single_page(firstLBA_in_page, sectors_in_page);
//	}
//#if 0
//	// (1) sector_cnt를 128보다 크게 write할 때 (page 개수가 1보다 클때)
//	if (page_cnt > 1)
//	{
//		for (UINT8 page_offset = 0; page_offset < page_cnt; page_offset++)
//		{
//			UINT32 firstLBA_of_page = startLBA + MAX_SECTOR_PER_PAGE * page_offset;
//			if (firstLBA_of_page > sector_cnt)
//				firstLBA_of_page = sector_cnt;
//
//
//			UINT8 data_buf[PAGE_SIZE] = { 0 };
//			UINT32 timestamp = GetTickCount64();
//			put_total_data(data_buf, firstLBA_of_page, sector_cnt, timestamp);		// sector_cnt만큼의 write할 8바이트 데이터를 buf에 만들기
//
//			char page_path[16];
//			get_page_path_from_lba(firstLBA_of_page, &page_path);		// 현재 LBA에 해당하는 page의 path 가져오기
//
//			write_file(page_path, data_buf, sector_cnt);	// 해당 path에 data_buf에 있는 데이터를 쓰기
//		}
//
//	}
//
//	// (2) sector_cnt를 128까지 write할 때 (page 개수가 1개 일때)
//	else if (page_cnt == 1)
//	{
//		write_single_page(startLBA, sector_cnt);
//	}
//#endif
//}

// sector단위로 write할 수 있는 pba를 찾는 함수
UINT16 find_enable_pba(MAP_ADDR* map_addr_base, UINT32 target_lba)
{
	///* ========== (0) LBA와 매칭되는 pba 찾기 ========== */
	//UINT16 origin_pba = get_pba(map_addr_base, target_lba);

	/* ========== (1) PBA를 bank, block, page로 분리 ========== */
	UINT8 bank = get_bank(map_addr_base, target_lba);
	UINT8 block = get_block(map_addr_base, target_lba);
	UINT8 page = get_page(map_addr_base, target_lba);

	//UINT8 bank = (origin_pba >> 14) & 0x03;
	//UINT8 block = (origin_pba >> 7) & 0x7F;
	//UINT8 page = (origin_pba) & 0x7F;

	/* ========== (2) write 할 수 있는 page 찾기 ========== */
	while (1)
	{
		//UINT32 block_offset = bank * BLOCK_NUM + block;
		UINT32 block_offset = get_block_offset(map_addr_base, target_lba);

		BLOCK_CURSOR* target_cursor = g_Cursor + block_offset;				// g_Cursor에서 현재 cursor를 구할 위치
		UINT8 cur_page = get_block_write_page_idx(target_cursor);	// 그 위치에 저장되어있는 cursor 값 = write할 수 있는 page

		if (cur_page == PAGE_FULL)		// 현재 block의 모든 page가 가득 차있으면 다음 block으로 변경
		{
			block++;
			page = 0;

			if (block >= BLOCK_NUM)		// block개수 넘어가면 다음 bank로 이동
			{
				bank++;
				block = 0;
			}
			if (bank >= BANK_NUM)		// bank 개수가 최대 넘어가는 경우 (용량 초과 = 쓸 수 있는 page 없음)
			{
				bank = BANK_NUM - 1;
				return FALSE;
			}
		}

		else                 // 현재 block에 write할 수 있는 page가 있는 경우
		{
			page = cur_page;		// page 변경
			break;
		}
	}
	/* ========== (3) bank, block, page를 PBA로 합치기 ========== */
	UINT16 new_pba =	((bank & 0x03) << 14) |			// 상위 2비트
						((block & 0x7F) << 7) |			// 중간 7비트
						((page & 0x7F));				// 하위 7비트
	return new_pba;
}

BOOL ftl_write(UINT32 startLBA, UINT32 sector_cnt)
{
	//UINT8 page_cnt = (sector_cnt + MAX_SECTOR_PER_PAGE - 1) / MAX_SECTOR_PER_PAGE;		// 써야하는 page 개수
	UINT32 set_page = 0;						// 현재 write 완료한 page 개수
	for (UINT32 sector_offset = 0; sector_offset < sector_cnt; sector_offset++)
	{
		UINT32 targetLBA = startLBA + sector_offset;

		/* ========== (0) sector 단위로 write할 수 있는 위치 찾기 ========== */
		UINT16 new_PBA = find_enable_pba(g_Map, targetLBA);		

		/* ========== (1) map 업데이트 ========== */
		set_pba(g_Map, targetLBA, new_PBA);

		//UINT8 bank	= get_bank(g_Map, targetLBA);
		//UINT8 block = get_block(g_Map, targetLBA);
		//UINT8 page	= get_page(g_Map, targetLBA);

		/* ========== (2) cursor 및 metadata 업데이트 -> "한 페이지 단위" ========== */
		// sector_offset이 마지막 sector 일 때  : sector_offset = sector_cnt-1
		// sector_offfset + 1이 128배수일 때 : (sector_offset + 1) % 128 == 0
		if ( sector_offset == (sector_cnt - 1) || (sector_offset + 1) % MAX_SECTORS_PER_PAGE == 0 )
		{
			UINT32 block_offset = get_block_offset(g_Map, targetLBA);

			/* ========== (2-1) cursor 업데이트 ========== */
			BLOCK_CURSOR* target_cursor = g_Cursor + block_offset;
			set_cursor_next_page(target_cursor);

			/* ========== (2-2) metadata 업데이트 ========== */	
			UINT32 firstLBA_in_page = 0;				// 현재 페이지 1개에 들어가는 첫번째 LBA
			UINT32 sectors_in_page = 0;					// 현재 페이지 1개에 들어가는 sector의 개수
			if (set_page == 0)		// 첫번째 page
			{
				firstLBA_in_page = startLBA;
				sectors_in_page = (sector_offset == sector_cnt - 1) ? sector_cnt : MAX_SECTORS_PER_PAGE;

				set_page++;
			}
			else    // 두번째 page 이상부터
			{
				firstLBA_in_page = set_page * MAX_SECTORS_PER_PAGE;		
				sectors_in_page = (sector_offset == sector_cnt - 1) ? (sector_cnt - (set_page * MAX_SECTORS_PER_PAGE)) : MAX_SECTORS_PER_PAGE;

				set_page++;
			}

			BLOCK_META* target_metadata = g_Meta + block_offset;
			update_validBitmap_one(target_metadata, firstLBA_in_page, sectors_in_page);
			update_BlockState(target_metadata);

			/* ========== (4) Page.bin에 write ========== */
			write_single_page(firstLBA_in_page, sectors_in_page);
		}
	}

	return TRUE;
}