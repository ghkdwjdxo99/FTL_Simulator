#pragma once
#include "config.h"

#define DATA_WRITE_SIZE		8		// byte
#define DATA_PER_PAGE		(PAGE_SIZE / DATA_WRITE_SIZE)	// 1024 / 8 = 128 : Page에 쓸 수 있는 최대 data 개수

/* =================== Base function ==========================*/
// make directory
BOOL make_dir(const char* path);

// make .bin file
BOOL make_bin_file(const char* path);

// write
BOOL write_file(const char* path, const void* data_buf, UINT32 size);

// read
BOOL read_file(const char* path, void* buf);


/* ======== Application Function ======== */

