#pragma once
#include "config.h"
#include <string.h>

#define PAGE_FULL	0xFF
#define TOTAL_CURSOR_SIZE		( sizeof(BLOCK_CURSOR) * TOTAL_BLOCK )


/*
* Block 당 하나씩 있음
*	- 초기값		: 0
*	- write 불가	: 0xFF
*/
//// type 정의
//typedef UINT8 BLOCK_CURSOR;

//// 메모리 포인터 선언
//extern BLOCK_CURSOR* const g_Cursor;

/* =================== function ==========================*/
// 각 Block 기준 Write 할 수 있는 Page Index가 어디인지 초기화
BOOL init_cursor(BLOCK_CURSOR* cursor_base);

// Block 내 Cursor 위치 이동 (Next Page)
BOOL set_cursor_next_page(BLOCK_CURSOR* cur_cursor);

// 현재 Cursor 위치 반환 (Page Index 반환)
BLOCK_CURSOR get_block_write_page_idx(BLOCK_CURSOR* cur_cursor);

// .bin 파일로 save
BOOL save_cursor(const char* path, const BLOCK_CURSOR* cursor_base);

// .bin 파일로 load
BOOL load_cursor(const char* path, BLOCK_CURSOR* cursor_base);