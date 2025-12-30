#pragma once
#include "config.h"

#define BT_META_DATA	 0x60000000
#define BT_FREE_DATA	 0x70000000
#define BT_HOST_DATA	 0x10000000
#define BT_REALHOST_DATA 0x20000000
#define BT_BAD_BLOCK 	 0xF0000000

#define TOTAL_META_SIZE		( sizeof(BLOCK_META) * TOTAL_BLOCK )

//// 구조체 정의 (+ 필수 매크로)
//#define MAX_VALID_PER_BLOCK 	(SECTORS_PER_PAGE * PAGE_NUM)		// 128 * 2000 = 16384개(bit)		: 한 block당 나올 수 있는 최대 valid 개수
//#define BITMAP_BYTES_PER_BLOCK	(MAX_VALID_PER_BLOCK / 8)			// 16384 / 8 = 2048 Bytes(2KB)	: 한 block 기준 bitmap의 byte크기
//#define BITMAP_BYTES_PER_PAGE	(BITMAP_BYTES_PER_BLOCK / PAGE_NUM)	// 2048 / 128 = 16 Bytes
//typedef struct {
//	UINT32 BlockState;
//	UINT8  validBitmap[BITMAP_BYTES_PER_BLOCK];
//} BLOCK_META;

//// 메모리 포인터 선언
//extern BLOCK_META* const g_Meta;

/* =================== function ==========================*/
// 초기화
BOOL init_metadata(BLOCK_META* metadata_base);
BOOL init_BlockState(BLOCK_META* metadata_base);
BOOL init_validBitmap(BLOCK_META* metadata_base);

// 변경
BOOL update_metadata(BLOCK_META* target_metadata, UINT32 target_LBA, UINT32 sectors_in_page);
BOOL update_BlockState(BLOCK_META* target_metadata);
BOOL update_validBitmap_one(BLOCK_META* target_metadata, UINT32 target_LBA, UINT32 sectors_in_page);
UINT16 count_valid_from_bitmap(BLOCK_META* target_metadata, UINT8* free_page_cnt);


// 저장 및 불러오기
BOOL save_metadata(const char* filename, BLOCK_META const* metadata_base);
BOOL load_metadata(const char* filename, BLOCK_META* metadata_base);

// 보여주기
void show_metadata(BLOCK_META* target_meta);

