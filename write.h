#pragma once
#include "config.h"

// seq : start lba, sector size 입력받기
// random : start lba, sector size를 랜덤으로

// start lba와 매칭되는 pba찾기
// 그 pba가 0이라면 그 block의 cursor값으로 pba를 변경
	// 그 cursor값이 128이라면 다음 block의 cursor값으로 pba를 변경
// map 업데이트
// page에 write
// metadata 업데이트
	// bitmap 업데이트
	// blockstate 업데이트
// cursor 업데이트

BOOL write(UINT32 startLBA, UINT32 sector_cnt);
