#pragma once
#include "config.h"

#define MAX_PATH_SIZE	100

// 한 페이지 읽기
size_t ftl_read(UINT32 start_lba, UINT32 sector_cnt, char* buf);