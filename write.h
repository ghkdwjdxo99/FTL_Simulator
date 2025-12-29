#pragma once
#include "config.h"

#define MAX_SECTOR_PER_PAGE		(PAGE_SIZE / SECTOR_SIZE)
//#define MAX_SECTOR_PER_PAGE		16
// seq : start lba, sector size 입력받기
// random : start lba, sector size를 랜덤으로

// start lba와 매칭되는 pba찾기
// 그 pba가 0이라면 그 block의 cursor값으로 pba를 변경
	// 그 cursor값이 128이라면 다음 block의 cursor값으로 pba를 변경
// map 업데이트
// page에 write
// metadata 업데이트
	// bitmap 업데이트
	// blockstate 업데이트
// cursor 업데이트


// 8 Byte 짜리 데이터 1개 만들기
BOOL put_sector_data(UINT8* data_buf, UINT32 lba, UINT32 timestamp);

// 8 Byte 데이터 붙여서 Page buf 만들기
BOOL put_total_data(UINT8* data_buf, UINT32 start_lba, UINT32 size, UINT32 timestamp);

// lba를 넣어주면 그에 해당하는 pba에 해당하는 파일 경로 구하는 함수
void get_page_path_from_lba(UINT32 targetLBA, char* out_path);

// page 하나를 write하는 함수
BOOL write_single_page(UINT32 firstLBA, UINT32 sectors_in_page);		

// sector_cnt에 따른 write해야하는 page 개수를 구해서 write하는 process
BOOL process_write_pages(UINT32 startLBA, UINT32 sector_cnt);		

UINT16 find_enable_pba(MAP_ADDR* map_addr_base, UINT32 target_lba);

BOOL ftl_write(UINT32 startLBA, UINT32 sector_cnt);
