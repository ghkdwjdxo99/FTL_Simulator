#pragma once

#include "config.h"
#include "file_sys_util.h"
#include "util.h"
#include "map_address.h"
#include "metadata.h"


// metadata 보여주기
void show_metadata(BLOCK_META* target_meta);

// map 출력 함수
//void squencial_view_map(MAP_ADDR* map_addr_base);
void range_view_map(const MAP_ADDR* map_addr_base, UINT32 start_lba, UINT32 sector_cnt);
