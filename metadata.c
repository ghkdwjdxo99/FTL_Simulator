#include "metadata.h"
#include "map_address.h"

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
UINT16 count_valid_from_bitmap(BLOCK_META* target_metadata, UINT8* free_page_cnt)
{
	//UINT8* target_Bitmap = target_metadata->validBitmap;
	UINT16 valid_cnt = 0;
	for (UINT8 page = 0; page < PAGE_NUM; page++)
	{
		BOOL is_page_free = TRUE;
		for (UINT16 byte_offset = 0; byte_offset < BITMAP_BYTES_PER_PAGE; byte_offset++)
		{
			UINT8* target_Bitmap = target_metadata->validBitmap + (page * BITMAP_BYTES_PER_PAGE) + byte_offset;		// 비트맵을 Byte단위로 이동
			for (UINT8 bit_offset = 0; bit_offset < 8; bit_offset++)
			{
				UINT8 mask = (UINT8)(1u << bit_offset);
				if (*target_Bitmap & mask)		// Byte단위 비트맵을 마스크를 이용해서 비트단위로 1이 있는지 확인
				{
					valid_cnt++;
					is_page_free = FALSE;
				}
			}
		}
		if (is_page_free == FALSE)
		{
			(*free_page_cnt)--;
		}
	}

	return valid_cnt;
}

// type, valid count 변경 함수
void update_BlockState(BLOCK_META* target_metadata)
{
	UINT32 block_type = 0;
	UINT8 free_page_cnt = PAGE_NUM;
	UINT16 valid_cnt = count_valid_from_bitmap(target_metadata, &free_page_cnt);		// bitmap에서 세어주기
	

	// (1) free로 변경 : 현재 block에 valid한 데이터가 없을 경우
	if (free_page_cnt == PAGE_NUM)
		block_type = BT_FREE_DATA;

	// (2) host로 변경 (기존에 free였을 때) : page의 일부가 써졌을 때
	else if (free_page_cnt > 0)
		block_type = BT_HOST_DATA;

	// (3) real host로 변경 : 모든 page가 다 써졌을 때
	else if (free_page_cnt == 0)
		block_type = BT_REALHOST_DATA;

	target_metadata->BlockState = (block_type & 0xF0000000) | (valid_cnt & 0x0000FFFF);
}

// lba에 해당하는 특정 bitmap을 sector_cnt만큼 bitmap을 1로 변경하는 함수 (page 하나 기준)
void update_validBitmap_one(BLOCK_META* target_metadata, UINT32 target_LBA, UINT32 sectors_in_page)
{
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
				return;
		}
	}
}

#if 0
void update_validBitmap_one(BLOCK_META* target_metadata, UINT8 target_page, UINT32 sector_cnt)
{
	UINT32 page_offset = target_page * BITMAP_BYTES_PER_PAGE;
	UINT8* target_Bitmap = target_metadata->validBitmap + page_offset;

	UINT32 set = 0;
	for (UINT8 byte_offset = 0; byte_offset < BITMAP_BYTES_PER_PAGE && set < sector_cnt; byte_offset++)
	{
		UINT8* target_Bitmap_Byte = target_Bitmap + byte_offset;
		for (UINT8 bit_offset = 0; bit_offset < 8 && set < sector_cnt; bit_offset++)		// write하는 sector 개수만큼 반복
		{
			UINT8 mask = 1u << bit_offset;
			*target_Bitmap_Byte = *target_Bitmap_Byte | mask;		// 해당 비트를 1로 만들어줌
			set++;
		}
	}
}
#endif

// bitmap 변경 함수
void update_validBitmap(BLOCK_META* metadata_base, UINT32 target_LBA, UINT16 target_PBA)
{
	UINT8 page = target_PBA % 0x7F;

	UINT32 sector_in_page = target_LBA % MAX_SECTORS_PER_PAGE;				// target sector가 한 페이지 기준 어느 위치에 있는지 ( 0 ~ 127)
	UINT32 sector_in_block = page * MAX_SECTORS_PER_PAGE + sector_in_page;	// target sector가 한 block기준 어느 위치에 있는지

	UINT32 byte_offset = sector_in_block / 8;	// 그 위치가 byte기준으로 몇번째 byte인지
	UINT32 bit_offset = sector_in_block % 8;	// 그 byte위치 기준으로 몇번재 비트에 위치한지
	
	UINT8* pBitmap = (metadata_base->validBitmap) + byte_offset;
	UINT8 mask = 1u << bit_offset;

	*pBitmap = *pBitmap | mask;		// 해당 비트를 1로 만들어줌
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