#ifndef GET_INPUT_IMPLEMENTATION
#define GET_INPUT_IMPLEMENTATION
#include "get_input.h"
#include "windows.h"
#include "stdlib.h"
#define BUFFER_LEN 128

static HANDLE hStdin;
static INPUT_RECORD input[BUFFER_LEN];

int init_input(void) {
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    return hStdin == INVALID_HANDLE_VALUE;
}

int get_input(void) {
    DWORD chars_read;
    ReadConsoleInput(hStdin, input, BUFFER_LEN, &chars_read);
    for (DWORD i = 0; i < chars_read; i++) {
        if (input[i].EventType != KEY_EVENT || !input[i].Event.KeyEvent.bKeyDown) {
            continue;
        }
        char player_in = input[i].Event.KeyEvent.uChar.AsciiChar;
        if (player_in == 0) {
            continue;
        } else if (player_in == 13) {
            return 10;
        }
        return player_in;
    }
    return 0;
}

int restore_input(void) { return 0; }


#endif