#include "read.h"
#include "file_sys_util.h"
#include "map_address.h"

UINT64 find_sector(UINT32 target_lba, const char* page_buf) {
	for (size_t off = 0; off + 8 <= PAGE_SIZE; off += 8) {
		UINT32 lba;
		memcpy(&lba, page_buf + off, 4);

		if (lba == target_lba) {
			UINT64 sector_data;
			memcpy(&sector_data, page_buf + off, 8);
			return sector_data;
		}
	}

	return 0;
}

BOOL read_page(UINT32 start_lba, UINT32 sector_cnt, char* buf)
{
	// lba 값으로 pba를 얻은 다음
	// 해당 lba 부터 sector_cnt 만큼의 sector data 읽어서 buf에 담기

	char page_buf[PAGE_SIZE];
	char path[MAX_PATH_SIZE];

	UINT32 lba = start_lba;
	char* cursor = buf;   // buf에서 현재 써야 할 위치를 가리키는 포인터

	for (UINT32 cnt = 0; cnt < sector_cnt; cnt++) {
		
		UINT64 sector_data;

		// PBA 받아오기
		UINT16 pba = get_pba(g_Map, lba);

		// PBA에 해당하는 Path 받아오기
		get_page_path(pba, path);

		// 해당 Path 파일 읽기
		if (!read_file(path, page_buf)) {
#ifdef DEBUG
			printf("File Read Failed...\n");
#endif
			return FALSE;
		}

		// 읽은 Page에서 LBA에 해당하는 sector 찾기
		sector_data = find_sector(lba, page_buf);
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

		lba++;
	}

	return TRUE;
}
