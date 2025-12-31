#pragma once
#include "config.h"

#define MAX_PATH_SIZE	100

// read 버퍼에 있는 값을 출력하는 함수
void view_read_buf(char* buf, UINT32 start_lba, UINT32 sector_cnt);

// read 버퍼에 있는 값을 검증하는 함수
BOOL verify_page_buf(char* buf, UINT32 sector_cnt, UINT16 pba);

UINT64 find_sector(UINT32 target_lba, const char* page_buf, size_t size);

// 한 페이지 읽기
size_t ftl_read(UINT32 start_lba, UINT32 sector_cnt, char* buf);


/* read modify 관련 */
// pba에 있는 데이터를 buf로 가져오는 함수
void get_data_from_page(UINT16 pba, char* page_buf);

// 8바이트 sector data에서 lba만 가져오는 함수
UINT32 get_lba_from_sector_data(UINT8* buf);

// page_buf에서 특정 sector data가 쓰여져있는 을 찾는 함수
void find_buf_idx(UINT8* page_buf, UINT16 target_lba);
void modify_data_from_buf(UINT8* page_buf, UINT16 target_lba);

