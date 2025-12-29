#pragma once
#include "config.h"
#include <string.h>

#define TOTAL_MAP_SIZE		( sizeof(MAP_ADDR) * TOTAL_SECTORS )

//// type 정의
//typedef UINT16 MAP_ADDR;

//// 메모리 포인터 선언
//extern MAP_ADDR* const g_Map;

/* =================== function ==========================*/
// PBA 값 초기화 (모두 0)
BOOL init_map_addr_zero(MAP_ADDR* map_addr_base);

// PBA로 Map 변경
void set_pba(MAP_ADDR* map_addr_base, UINT32 target_lba, MAP_ADDR target_pba);

// PBA 값 불러오기
//void get_pba(MAP_ADDR* map_addr_base, UINT32 target_lba, UINT16* target_pba);
UINT16 get_pba(MAP_ADDR* map_addr_base, UINT32 target_lba);
UINT8 get_bank(MAP_ADDR* map_addr_base, UINT32 target_lba);
UINT8 get_block(MAP_ADDR* map_addr_base, UINT32 target_lba);
UINT8 get_page(MAP_ADDR* map_addr_base, UINT32 target_lba);


// .bin 파일로 Save
BOOL save_map_address(const char* path, const MAP_ADDR* map_addr_base);

// .bin 파일로 load
BOOL load_map_address(const char* path, MAP_ADDR* map_addr_base);