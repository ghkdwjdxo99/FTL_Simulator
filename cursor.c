#include "cursor.h"

// 각 Block 기준 Write 할 수 있는 Page Index가 어디인지 초기화
BOOL init_cursor(BLOCK_CURSOR* cursor_base) {
	if (cursor_base == NULL)
		return FALSE;

	*cursor_base = PAGE_FULL;						// 첫번째 block은 cursor값 max (metadata라서 쓰지 못하도록 처리)
	memset(cursor_base+1, 0, TOTAL_CURSOR_SIZE-1);	// 나머지 block은 cursor값 0
	return TRUE;
}

// Block 내 Cursor 위치 이동 (Next Page)
BOOL set_cursor_next_page(BLOCK_CURSOR* cur_cursor) {
	if (cur_cursor == NULL)
		return FALSE;

	if (*cur_cursor == MAX_SECTORS_PER_PAGE - 1)
		*cur_cursor = PAGE_FULL;
	else
		*cur_cursor += 1;

	return TRUE;
}

// 현재 Cursor 위치 반환 (Page Index 반환)
BLOCK_CURSOR get_block_write_page_idx(BLOCK_CURSOR* cur_cursor) {
	return *cur_cursor;
}

// .bin 파일로 save
BOOL save_cursor(const char* path, const BLOCK_CURSOR* cursor_base) {
	FILE* fp = fopen(path, "wb");

	if (fp == NULL)
		return FALSE;

	size_t n = fwrite(cursor_base, TOTAL_CURSOR_SIZE, 1, fp);

	fclose(fp);

	return (n == 1) ? TRUE : FALSE;
}

// .bin 파일로 load
BOOL load_cursor(const char* path, BLOCK_CURSOR* cursor_base) {
	FILE* fp = fopen(path, "rb");

	if (fp == NULL)
		return FALSE;

	size_t n = fread(cursor_base, TOTAL_CURSOR_SIZE, 1, fp);

	fclose(fp);

	return (n == 1) ? TRUE : FALSE;
}