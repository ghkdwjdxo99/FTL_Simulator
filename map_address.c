#include "map_address.h"

// PBA 값 초기화 (모두 0)
BOOL init_map_addr_zero(MAP_ADDR* map_addr_base) {
	if (map_addr_base == NULL)
		return FALSE;
	
	memset(map_addr_base, 0, TOTAL_MAP_SIZE);
	return TRUE;
}

// PBA로 Map 변경
void set_pba(MAP_ADDR* map_addr_base, UINT32 target_lba, MAP_ADDR target_pba) {
	*(map_addr_base + target_lba) = target_pba;
}

// PBA 값 가져오기
UINT16 get_pba(MAP_ADDR* map_addr_base, UINT32 target_lba)
{
	//*target_pba = *(map_addr_base + target_lba);
	return *(map_addr_base + target_lba);
}

// .bin 파일로 Save
BOOL save_map_address(const char* path, const MAP_ADDR* map_addr_base) {
	FILE* fp = fopen(path, "wb");

	if (fp == NULL)
		return FALSE;

	size_t n = fwrite(map_addr_base, TOTAL_MAP_SIZE, 1, fp);

	fclose(fp);

	return (n == 1) ? TRUE : FALSE;
}

// .bin 파일로 load
BOOL load_map_address(const char* path, MAP_ADDR* map_addr_base) {
	FILE* fp = fopen(path, "rb");

	if (fp == NULL)
		return FALSE;

	size_t n = fread(map_addr_base, TOTAL_MAP_SIZE, 1, fp);

	fclose(fp);

	return (n == 1) ? TRUE : FALSE;
}