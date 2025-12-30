#include "read.h"
#include "file_sys_util.h"
#include "map_address.h"

UINT64 find_sector(UINT32 target_lba, const char* page_buf, size_t size)
{
	const size_t sector_size = SECTOR_SIZE;
	const size_t lba_offset = 4;

	// size는 항상 8의 배수라고 가정
	size_t sector_cnt = size / sector_size;

	for (size_t i = 0; i < sector_cnt; i++)
	{
		size_t off = i * sector_size;

		UINT32 lba;
		memcpy(&lba, page_buf + off + lba_offset, 4);

		if (lba == target_lba)
		{
			UINT64 sector_data;
			memcpy(&sector_data, page_buf + off, 8);
			return sector_data;
		}
	}

	return 0;
}




size_t ftl_read(UINT32 start_lba, UINT32 sector_cnt, char* buf)
{
	// lba 값으로 pba를 얻은 다음
	// 해당 lba 부터 sector_cnt 만큼의 sector data 읽어서 buf에 담기

	char page_buf[PAGE_SIZE];
	char path[MAX_PATH_SIZE];
	size_t sector_data_num = 0;

	UINT32 lba = start_lba;
	char* cursor = buf;   // buf에서 현재 써야 할 위치를 가리키는 포인터

	for (UINT32 cnt = 0; cnt < sector_cnt; cnt++) {
		
		UINT64 sector_data;

		// PBA 받아오기
		UINT16 pba = get_pba(g_Map, lba);

		// PBA에 해당하는 Path 받아오기
		get_page_path(pba, path);

		// 해당 Path 파일 읽기
		printf("%s %d\n", path, cnt);
		size_t page_size = read_file(path, page_buf);
		if (page_size == FALSE) {
#ifdef DEBUG
			printf("File Read Failed...\n");
#endif
			return FALSE;
		}

		// 읽은 Page에서 LBA에 해당하는 sector 찾기
		sector_data = find_sector(lba, page_buf, page_size);
		if (sector_data == 0) {
#ifdef DEBUG
			printf("Find Sector Failed...\n");
			printf("There is no data corresponding to the LBA.\n");
#endif
			return FALSE;
		}

		// buf에 8바이트 sector data 이어서 쓰기
		memcpy(cursor, &sector_data, sizeof(UINT64));
		cursor += sizeof(UINT64);   // 다음 위치로 이동

		sector_data_num++;
		lba++;
	}

	return sector_data_num;	// sector data 개수 반환
}
