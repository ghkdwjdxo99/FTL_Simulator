#include "erase.h"

void find_MinMax_one(BLOCK_META* start_metadata, UINT8 scan_size, BOOL is_max, UINT32 target_type)
{
	UINT8 scan = 0;
	UINT16 best_valid = 0;
	UINT16 best_block = 0;
	for (UINT8 block_offset = 0; block_offset < scan_size; block_offset++)
	{
		BLOCK_META* target_metadata = start_metadata + block_offset;
		UINT32 block_type = (target_metadata->BlockState & 0xF0000000);
		if (block_type != target_type)		// target type과 다를 때는 넘어감
			continue;

		UINT16 cur_valid = (UINT16)(target_metadata->BlockState & 0x0000FFFF);
		if (scan == 0)
		{
			best_valid = cur_valid;
			best_block = block_offset;
			scan++;
		}
		else
		{
			if (is_max == 1 ? (cur_valid > best_valid) : (cur_valid < best_valid))
			{
				best_valid = cur_valid;
				best_block = block_offset;
				scan++;
			}
		}
	}

	if (scan == scan_size)
		return TRUE;
}

BOOL erase_single_block(void)
{

}