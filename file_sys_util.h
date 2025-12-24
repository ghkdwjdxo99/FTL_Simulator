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
/* write */
// 8 Byte 짜리 데이터 만들기
BOOL put_sector_data(UINT8* data_buf, UINT32 lba, UINT32 timestamp);

// 8 Byte 데이터 붙여서 Page buf 만들기
BOOL put_page_data(UINT8* data_buf, UINT32 start_lba, UINT32 size, UINT32 timestamp);


/* ==================== 이건 FTL안에서 하는 거인듯..? ===================*/
// -> readwrite.c로 옮겼음
// start LBA, size, timestamp 받아서 write
BOOL write_page(UINT32 start_lba, UINT32 size, UINT32 timestamp);

/* read */
BOOL read_page(UINT32 start_lba, UINT32 size, UINT32 timestamp);
