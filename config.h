#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

#define DEBUG

#define BANK_NUM		4
#define BLOCK_NUM		128
#define PAGE_NUM		128

#define PAGE_SIZE			1024	// Byte
#define SECTOR_SIZE			8		// Byte

#define MAX_SECTORS_PER_PAGE		(PAGE_SIZE / SECTOR_SIZE)			// 1024/8 = 128					: 한 page에 들어갈 수 있는 최대 sector 개수

#define TOTAL_BLOCK			(BANK_NUM * BLOCK_NUM)
#define TOTAL_PAGE			(TOTAL_BLOCK * PAGE_NUM)
#define TOTAL_BYTES			(TOTAL_PAGE * PAGE_SIZE)
#define TOTAL_SECTORS		(TOTAL_BYTES / SECTOR_SIZE)

// 구조체 선언 및 type 정의
#define MAX_VALID_PER_BLOCK 	(MAX_SECTORS_PER_PAGE * PAGE_NUM)		// 128 * 2000 = 16384개(bit)		: 한 block당 나올 수 있는 최대 valid 개수
#define BITMAP_BYTES_PER_BLOCK	(MAX_VALID_PER_BLOCK / 8)			// 16384 / 8 = 2048 bytes(2KB)	: 한 block 기준 bitmap의 byte크기
#define BITMAP_BYTES_PER_PAGE	(BITMAP_BYTES_PER_BLOCK / PAGE_NUM)	// 2048 / 128 = 16 Bytes
typedef struct {
	UINT32 BlockState;
	UINT8  validBitmap[BITMAP_BYTES_PER_BLOCK];
} BLOCK_META;
typedef UINT16 MAP_ADDR;
typedef UINT8 BLOCK_CURSOR;

// 메모리 선언
extern UINT8 G_MEM[];
extern BLOCK_META*		const g_Meta;
extern MAP_ADDR*		const g_Map;
extern BLOCK_CURSOR*	const g_Cursor;