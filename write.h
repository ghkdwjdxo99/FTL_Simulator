#pragma once
#include "config.h"

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

// 8 Byte 데이터 1개 만들어서 buf에 저장
BOOL put_sector_data(UINT8* buf, UINT32 lba, UINT32 timestamp);

// 8 Byte 데이터를 sector_cnt개 만큼 만들어서 data_buf에 저장
BOOL put_total_data(UINT8* data_buf, UINT32 start_lba, UINT32 sector_cnt, UINT32 timestamp);

// lba를 넣어주면 그에 해당하는 pba에 해당하는 "page.bin 파일의 path" 구하는 함수
void get_page_path_from_lba(UINT32 targetLBA, char* out_path);

// page 하나에 해당하는 sector data를 만들고 page.bin 파일에 write하는 함수
BOOL write_single_page(UINT32 firstLBA, UINT32 sectors_in_page);		

UINT16 find_enable_pba(MAP_ADDR* map_addr_base, UINT32 target_lba);

/*
BOOL ftl_write(UINT32 startLBA, UINT32 sector_cnt);
*/

BOOL ftl_random_write(UINT32 startLBA, UINT32 sector_cnt, UINT32 count);

/* read modify 관련 */
// pba에 있는 데이터를 buf로 가져오는 함수
size_t get_data_from_page(UINT16 pba, char* page_buf);

// 8바이트 sector data에서 lba만 가져오는 함수
UINT32 get_lba_from_sector_data(UINT8* buf);

/*
UINT32 modify_data_from_buf(UINT8* page_buf, size_t page_buf_size, UINT16 target_lba, UINT8* new_page_buf, UINT16* new_PBA);

// 기존에 쓰여져있던 page에서 modify할 data만 뽑아서 buf에 담고 그 data에 포함된 sector 개수를 반환
UINT32 read_and_modify_1(UINT16 target_lba, UINT16 origin_pba, char* new_page_buf);
BOOL read_and_modify_2(MAP_ADDR* map_addr_base, UINT16 target_lba, UINT32 new_sector_cnt, char* new_page_buf);
*/