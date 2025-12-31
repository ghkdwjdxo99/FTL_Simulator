#include "metadata.h"
#include "map_address.h"
#include "cursor.h"

// type, valid count 초기화 함수 : bank 0 block 0 만 meta, 나머지는 free
BOOL init_BlockState(BLOCK_META* metadata_base)
{
	if (metadata_base == NULL)
		return FALSE;

	for (UINT16 block_idx = 0; block_idx < TOTAL_BLOCK; block_idx++)
	{
		// 1) block type 초기화
		UINT32 block_type = 0;
		if(block_idx == 0)
			block_type = BT_META_DATA;
		else
			block_type = BT_FREE_DATA;

		// 2) valid count 초기화
		UINT16 valid_cnt = 0;

		// 3) type, valid count를 BlockState에 대입하기
		(metadata_base + block_idx)->BlockState = block_type | (valid_cnt & 0x0000FFFF);
	}
	return TRUE;
}

// bitmap 초기화 함수
BOOL init_validBitmap(BLOCK_META* metadata_base)
{
	if (metadata_base == NULL)
		return FALSE;

	for (UINT16 block_idx = 0; block_idx < TOTAL_BLOCK; block_idx++)
	{
		memset((metadata_base+block_idx)->validBitmap, 0, BITMAP_BYTES_PER_BLOCK);
	}
	return TRUE;
}

BOOL init_metadata(BLOCK_META* metadata_base)
{
	if (metadata_base == NULL)
		return FALSE;

	init_BlockState(metadata_base);
	init_validBitmap(metadata_base);
	return TRUE;
}

//---------------------------------------------------------------------
// block 기준 bitmap에 들어있는 1비트 개수 세기
UINT16 count_valid_from_bitmap(BLOCK_META* target_metadata)
{
	//UINT8* target_Bitmap = target_metadata->validBitmap;
	UINT16 valid_cnt = 0;
	for (UINT8 page = 0; page < PAGE_NUM; page++)
	{
		//BOOL is_page_free = TRUE;
		for (UINT16 byte_offset = 0; byte_offset < BITMAP_BYTES_PER_PAGE; byte_offset++)
		{
			UINT8* target_Bitmap = target_metadata->validBitmap + (page * BITMAP_BYTES_PER_PAGE) + byte_offset;		// 비트맵을 Byte단위로 이동
			for (UINT8 bit_offset = 0; bit_offset < 8; bit_offset++)
			{
				UINT8 mask = (UINT8)(1u << bit_offset);
				if (*target_Bitmap & mask)		// Byte단위 비트맵을 마스크를 이용해서 비트단위로 1이 있는지 확인
				{
					valid_cnt++;
					//is_page_free = FALSE;
				}
			}
		}
		//if (is_page_free == FALSE)
		//{
		//	(*free_page_cnt)--;
		//}
	}

	return valid_cnt;
}

// type, valid count 변경 함수
BOOL update_BlockState(BLOCK_META* target_metadata, UINT32 target_LBA)
{
	if (target_metadata == NULL)
		return FALSE;

	// 1. block type 변경 : cursor값을 보고 write한 page 여부를 확인
	UINT8 target_bank = get_bank(g_Map, target_LBA);
	UINT8 target_block = get_block(g_Map, target_LBA);
	UINT8 target_page = get_page(g_Map, target_LBA);

	BLOCK_CURSOR* target_cursor = g_Cursor + target_bank * BLOCK_NUM + target_block;
	BLOCK_CURSOR next_enable_page_idx = get_block_write_page_idx(target_cursor);

	UINT32 block_type = 0;
	if (next_enable_page_idx == 0)				// (1) free로 변경 : 현재 block에 write한 page가 없는 경우
		block_type = BT_FREE_DATA;

	else if (next_enable_page_idx == PAGE_FULL)	// (2) real host로 변경 : 모든 page가 다 써졌을 때
		block_type = BT_REALHOST_DATA;

	else if(next_enable_page_idx > 0)			// (3) host로 변경 (기존에 free였을 때) : page의 일부가 써졌을 때
		block_type = BT_HOST_DATA;


	// 2. validBitmap 변경
	UINT16 valid_cnt = count_valid_from_bitmap(target_metadata);		// bitmap에서 세어주기

#if 0
	UINT32 block_type = 0;
	UINT8 free_page_cnt = PAGE_NUM;
	UINT16 valid_cnt = count_valid_from_bitmap(target_metadata, &free_page_cnt);		// bitmap에서 세어주기
	

	// (1) free로 변경 : 현재 block에 write한 page가 없는 경우
	if (free_page_cnt == PAGE_NUM)
		block_type = BT_FREE_DATA;

	// (2) host로 변경 (기존에 free였을 때) : page의 일부가 써졌을 때
	else if (free_page_cnt > 0)
		block_type = BT_HOST_DATA;

	// (3) real host로 변경 : 모든 page가 다 써졌을 때
	else if (free_page_cnt == 0)
		block_type = BT_REALHOST_DATA;
#endif

	// 변경한 1, 2번을 BlockState로 합치기
	target_metadata->BlockState = (block_type & 0xF0000000) | (valid_cnt & 0x0000FFFF);

	return TRUE;
}

// lba에 해당하는 특정 bitmap을 sector_cnt만큼 bitmap을 1로 변경하는 함수 (page 하나 기준)
BOOL update_validBitmap_one(BLOCK_META* target_metadata, UINT32 target_LBA, UINT32 sectors_in_page)
{
	if (target_metadata == NULL)
		return FALSE;

	UINT8 target_page = get_page(g_Map, target_LBA);

	UINT8* target_Bitmap = ((target_metadata->validBitmap) + (target_page * BITMAP_BYTES_PER_PAGE));
	UINT32 set_sectors = 0;
	for (UINT8 byte_offset = 0; byte_offset < BITMAP_BYTES_PER_PAGE; byte_offset++)
	{
		UINT8* target_Bitmap_Byte = target_Bitmap + byte_offset;
		for (UINT8 bit_offset = 0; bit_offset < 8; bit_offset++)
		{
			UINT8 mask = 1u << bit_offset;
			*target_Bitmap_Byte = *target_Bitmap_Byte | mask;
			set_sectors++;
			if (set_sectors == sectors_in_page)
				return TRUE;
		}
	}
	return FALSE;
}

// lba에 해당하는 특정 bitmap을 sector_cnt만큼 bitmap을 0으로 변경하는 함수 (page 하나 기준)
BOOL update_validBitmap_zero(BLOCK_META* metadata_base, UINT32 target_PBA)
{
	if (metadata_base == NULL)
		return FALSE;

	UINT8 target_bank = get_bank_from_pba(target_PBA);
	UINT8 target_block = get_block_from_pba(target_PBA);
	UINT8 target_page = get_page_from_pba(target_PBA);

	UINT16 block_offset = target_bank * BLOCK_NUM + target_block;

	BLOCK_META* target_metadata = metadata_base + block_offset;

	UINT8* target_Bitmap = ((target_metadata->validBitmap) + (target_page * BITMAP_BYTES_PER_PAGE));

	for (UINT16 byte_offset = 0; byte_offset < BITMAP_BYTES_PER_PAGE; byte_offset++)
	{
		UINT8* target_Bitmap_Byte = target_Bitmap + byte_offset;
		for (UINT8 bit_offset = 0; bit_offset < 8; bit_offset++)
		{
			UINT8 mask = (UINT8)(1u << bit_offset);
			if (*target_Bitmap_Byte & mask)		// Byte단위 비트맵을 마스크를 이용해서 비트단위로 1이 있는지 확인
			{
				*target_Bitmap_Byte &= (UINT8)~mask;
				return TRUE;
			}
			else
				continue;
		}
	}

	return FALSE;
	//UINT32 set_sectors = 0;
	//for (UINT8 byte_offset = 0; byte_offset < BITMAP_BYTES_PER_PAGE; byte_offset++)
	//{
	//	UINT8* target_Bitmap_Byte = target_Bitmap + byte_offset;
	//	for (UINT8 bit_offset = 0; bit_offset < 8; bit_offset++)
	//	{
	//		UINT8 mask = 1u << bit_offset;
	//		*target_Bitmap_Byte = *target_Bitmap_Byte | mask;
	//		set_sectors++;
	//		if (set_sectors == sectors_in_page)
	//			return;
	//	}
	//}
}

// metadata 변경 함수
BOOL update_metadata(BLOCK_META* target_metadata, UINT32 target_LBA, UINT32 sectors_in_page)
{
	if (target_metadata == NULL)
		return FALSE;

	update_validBitmap_one(target_metadata, target_LBA, sectors_in_page);
	update_BlockState(target_metadata, target_LBA);
	return TRUE;
}

// .bin 파일로 save
BOOL save_metadata(const char* filename, BLOCK_META const* metadata_base)
{
	FILE* fp = fopen(filename, "wb"); 
	if (!fp)
		return FALSE;

	// g_Meta에 있는 것을 TOTAL_META_SIZE 만큼 file로 write
	size_t n = fwrite(metadata_base, TOTAL_META_SIZE, 1, fp);

	fclose(fp);

	return (n == 1) ? TRUE : FALSE;
}

// .bin 파일로 load
BOOL load_metadata(const char* filename, BLOCK_META* metadata_base)
{
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		return FALSE;

	// fp에서 TOTAL_META_SIZE만큼을 read해서 g_Meta에 저장
	size_t n = fread(metadata_base, TOTAL_META_SIZE, 1, fp);

	fclose(fp);

	return (n == 1) ? TRUE : FALSE;
}

