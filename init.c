#include "init.h"
#include "metadata.h"
#include "map_address.h"
#include "cursor.h"
#include "file_sys_util.h"


// 전체 bin 파일 불러오기
BOOL load_bin_files(BLOCK_META* metadata_base, MAP_ADDR* map_addr_base, BLOCK_CURSOR* cursor_base)
{
    if (metadata_base == NULL || map_addr_base == NULL || cursor_base == NULL)
        return FALSE;

    if (!load_metadata("metadata.bin", metadata_base))
    {
        printf("[INFO] \"metadata.bin\" not found.\n");
        init_metadata(metadata_base);
    }
    
    if (!load_map_address("map.bin", map_addr_base))
    {
        printf("[INFO] \"map.bin\" not found.\n");
        init_map_addr_zero(map_addr_base);
    }
    
    if (!load_cursor("cursor.bin", cursor_base))
    {
        printf("[INFO] \"cursor.bin\" not found.\n");
        init_cursor(cursor_base);
    }
    
    return TRUE;
}

// 전체 bin 파일 저장하기
BOOL save_bin_files(BLOCK_META* metadata_base, MAP_ADDR* map_addr_base, BLOCK_CURSOR* cursor_base)
{
    save_metadata("metadata.bin", metadata_base);
    save_map_address("map.bin", map_addr_base);
    save_cursor("cursor.bin", cursor_base);

    return TRUE;
}

// 메모리 초기화 및 bin 파일 생성
BOOL reset_bin_files(BLOCK_META* metadata_base, MAP_ADDR* map_addr_base, BLOCK_CURSOR* cursor_base)
{
    init_metadata(metadata_base);
    init_map_addr_zero(map_addr_base);
    init_cursor(cursor_base);

    return TRUE;
}

// bank, block 디렉토리 & page bin 파일 만들기
void create_nand_dirs(void)
{
    for(UINT8 bank = 0; bank < BANK_NUM; bank++)
    {
        char bank_path[16];
        sprintf(bank_path, "%c", 'A' + bank);
        make_dir(bank_path);

        for(UINT8 blk = 0; blk < BLOCK_NUM; blk++)
        {
            char block_path[16];
            sprintf(block_path, "%s\\%u", bank_path, blk);
            make_dir(block_path);
            
            // bin 파일 생성
            for (UINT8 page = 0; page < PAGE_NUM; page++)
            {
                char page_path[16];
                sprintf(page_path, "%s\\P%u.bin", block_path, page);
                make_bin_file(page_path);
            }
        }
    }
}

// bank, block 디렉토리 & page bin 파일 초기화
void reset_nand_dirs(void)
{
    for (UINT8 bank = 0; bank < BANK_NUM; bank++)
    {
        char bank_path[16];
        sprintf(bank_path, "%c", 'A' + bank);
        make_dir(bank_path);

        for (UINT8 blk = 0; blk < BLOCK_NUM; blk++)
        {
            char block_path[16];
            sprintf(block_path, "%s\\%u", bank_path, blk);
            make_dir(block_path);

            // bin 파일 생성
            for (UINT8 page = 0; page < PAGE_NUM; page++)
            {
                char page_path[16];
                sprintf(page_path, "%s\\P%u.bin", block_path, page);
                reset_bin_file(page_path);
            }
        }
    }
}
