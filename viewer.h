#pragma once

#include "config.h"
#include "file_sys_util.h"
#include "util.h"
#include "map_address.h"
#include "metadata.h"

void view_block_type_for_lba_range(UINT32 start_lba, UINT32 sector_cnt);

// metadata 보여주기
void show_metadata(BLOCK_META* target_meta);
static const char* block_type_to_str(UINT32 block_type);

// map 출력 함수
//void squencial_view_map(MAP_ADDR* map_addr_base);
void range_view_map(const MAP_ADDR* map_addr_base, UINT32 start_lba, UINT32 sector_cnt);

// cursor 출력 함수
void show_cursor(BLOCK_CURSOR* target_cursor);

// random w/r log 출력 함수
void log_rand_wr(BOOL wr, UINT32 num, UINT32 start_lba, UINT32 end_lba, UINT16 pba, BOOL read_result);