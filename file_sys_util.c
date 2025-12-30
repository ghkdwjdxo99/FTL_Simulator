#include "file_sys_util.h"

void pba_to_bank_block_page(UINT16 pba, UINT32* bank, UINT32* block, UINT32* page)
{
	UINT32 b	= (pba >> 14) & 0x3;
	UINT32 bl	= (pba >> 7) & 0x7F;
	UINT32 p	= (pba >> 0) & 0x7F;

	if (bank)  *bank = b;
	if (block) *block = bl;
	if (page)  *page = p;
}

void get_bank_path(UINT16 pba, char* buf)
{
	UINT32 bank;
	pba_to_bank_block_page(pba, &bank, NULL, NULL);

	// 현재 디렉터리 기준: ./A
	snprintf(buf, 64, "%c", (char)('A' + bank));
}

void get_block_path(UINT16 pba, char* buf)
{
	UINT32 bank, block;
	pba_to_bank_block_page(pba, &bank, &block, NULL);

	// 현재 디렉터리 기준: ./A/0
	snprintf(buf, 64, "%c\\%u", (char)('A' + bank), block);
}

void get_page_path(UINT16 pba, char* buf)
{
	UINT32 bank, block, page;
	pba_to_bank_block_page(pba, &bank, &block, &page);

	// 현재 디렉터리 기준: ./A/0/P0.bin
	snprintf(buf, 128, "%c\\%u\\P%u.bin", 'A' + bank, block, page);
}

BOOL make_dir(const char* path)
{
	if (CreateDirectoryA(path, NULL))
		return TRUE;
}

BOOL make_bin_file(const char* path)
{
	FILE* fp = fopen(path, "rb");	// 파일 존재 여부 확인
	if (fp != NULL)
	{
		fclose(fp);
		return TRUE;
	}

	fp = fopen(path, "w+b");	// 파일이 없을 때만 읽기쓰기용으로 새로 생성
	if (fp == NULL)
	{
		printf("fopen failed : %s\n", path);
		return FALSE;
	}

	fclose(fp);
	return TRUE;
}

BOOL reset_bin_file(const char* path)
{
	FILE* fp = fopen(path, "w+b");		// 기존 파일 덮어쓰고 새로 생성
	if (fp == NULL)
	{
		printf("fopen failed : %s\n", path);
		return FALSE;
	}

	fclose(fp);
	return TRUE;
}


// data_buf에 있는 data(8바이트 data 뭉치)를 path에 있는 file에 write
BOOL write_file(const char* path, const void* data_buf, UINT32 sector_cnt)
{
	FILE* fp = fopen(path, "wb");
	if (fp == NULL)
	{
		printf("fopen failed : %s\n", path);
		return FALSE;
	}

	// page에 write를 한다 = page가 비워져있다 = page의 처음위치부터 쓰면 된다는 의미
	if (fseek(fp, 0, SEEK_SET) != 0)
	{
		printf("fseek failed : %s\n", path);
		fclose(fp);
		return FALSE;
	}

	if (fwrite(data_buf, DATA_WRITE_SIZE, sector_cnt, fp) != sector_cnt)		// 	fwrite(data_buf, size * DATA_WRITE_SIZE, 1, fp) 같은 의미
	{
		printf("fwrite failed : %s \n", path);
	}

	fclose(fp);
	return TRUE;
}

// path에 있는 file을 처음부터 1KB 읽어서 buf에 저장
size_t read_file(const char* path, void* buf)
{
	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
	{
		printf("fopen failed : %s\n", path);
		perror("fopen");
		return FALSE;
	}

	size_t size = fread(buf, 1, PAGE_SIZE, fp);

	if (size < PAGE_SIZE) {
		if (ferror(fp)) {
			// 에러
			printf("fread failed : %s \n", path);
			perror("fread");
			fclose(fp);
			return FALSE;
		}
	}

	fclose(fp);
	return size;
}


