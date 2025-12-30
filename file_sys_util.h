#pragma once
#include "config.h"

#define DATA_WRITE_SIZE		8		// byte
#define DATA_PER_PAGE		(PAGE_SIZE / DATA_WRITE_SIZE)	// 1024 / 8 = 128 : Page에 쓸 수 있는 최대 data 개수

/* =================== Make Path Function =================== */
void pba_to_bank_block_page(UINT32 pba, UINT32* bank, UINT32* block, UINT32* page);
void get_bank_path(UINT32 pba, char* buf);
void get_block_path(UINT32 pba, char* buf);
void get_page_path(UINT32 pba, char* buf);

/* =================== Base function ==========================*/
// make directory
BOOL make_dir(const char* path);

// make .bin file
BOOL make_bin_file(const char* path);

// reset .bin file
BOOL reset_bin_file(const char* path);

// write
BOOL write_file(const char* path, const void* data_buf, UINT32 size);

// read
size_t read_file(const char* path, void* buf);


/* ======== Application Function ======== */

