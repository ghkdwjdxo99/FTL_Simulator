#include "util.h"

/* 사용자 입력 관련 함수 */
int is_user_interrupt() {
	if (_kbhit()) {
		int ch = get_any_key();
		if (ch == 'q' || ch == 'Q')
			return 0;
	}
	return 0;
}

int get_any_key() {
	return _getch();
}

int wait_key_continue() {
	int ch = get_any_key();

	if (ch == 'q' || ch == 'Q')
		return 0;
	return 1;
}