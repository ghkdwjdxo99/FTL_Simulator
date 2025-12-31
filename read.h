#pragma once
#include "config.h"

#define MAX_PATH_SIZE	100

// read 버퍼에 있는 값을 출력하는 함수
void view_read_buf(char* buf, UINT32 sector_cnt);

// read 버퍼에 있는 값을 검증하는 함수
BOOL verify_page_buf(char* buf, UINT32 sector_cnt, UINT16 pba);

UINT64 find_sector(UINT32 target_lba, const char* page_buf, size_t size);

// 한 페이지 읽기
size_t ftl_read(UINT32 start_lba, UINT32 sector_cnt, char* buf);
