#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

HANDLE hStdin;

int main(int argc, char **argv) {
    DWORD player_in, cNumRead;
    INPUT_RECORD input[128];

    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    while (1) {
        ReadConsoleInput(hStdin, input, 1, &cNumRead);
        printf("num read: %ld\n", cNumRead);
        for (int i = 0; i < cNumRead; i++) {
            if (input[i].EventType != KEY_EVENT || !input[i].Event.KeyEvent.bKeyDown) {
                continue;
            }
            printf("%d", input[i].Event.KeyEvent.uChar.AsciiChar);
        }
        printf("\n\n");
    }

//     for (int i = 0; i < argc; i++) {
//         printf("%d %s\n", i, argv[i]);
//     }
}
