#pragma once
#include "config.h"

BOOL load_bin_files(BLOCK_META* metadata_base, MAP_ADDR* map_addr_base, BLOCK_CURSOR* cursor_base);
BOOL save_bin_files(BLOCK_META* metadata_base, MAP_ADDR* map_addr_base, BLOCK_CURSOR* cursor_base);
BOOL reset_bin_files(BLOCK_META* metadata_base, MAP_ADDR* map_addr_base, BLOCK_CURSOR* cursor_base);

void create_nand_dirs(void);
