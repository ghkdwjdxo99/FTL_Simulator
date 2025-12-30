#pragma once
#include <stdio.h>
#include <conio.h>

#include "config.h"
#include "map_address.h"
#include "file_sys_util.h"

/* 사용자 입력 관련 함수 */
int get_any_key();
int wait_key_continue();

/* 출력 헬퍼 함수 */


// map 출력 함수
//void squencial_view_map(MAP_ADDR* map_addr_base);
void range_view_map(const MAP_ADDR* map_addr_base, UINT32 start_lba, UINT32 sector_cnt);

// cursor 출력 함수
void show_cursor(BLOCK_CURSOR* target_cursor);
