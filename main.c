#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define RESET "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define CLEAR "\033c"

#define LINE_LENGTH 80

HANDLE hStdin;

int main() {
    FILE *fptr = fopen("D:\\agats", "r");
    char file_line[LINE_LENGTH];
    int correct[LINE_LENGTH];
    int count = 0;
    int idx = 0;
    DWORD player_in, cNumRead;
    INPUT_RECORD input;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);


    while (fgets(file_line, LINE_LENGTH, fptr)) {
        printf(CLEAR RESET "%s", file_line);
        while(idx != strlen(file_line) - 1) {
            ReadConsoleInput(hStdin, &input, 1, &cNumRead);
            if (input.EventType != KEY_EVENT || input.Event.KeyEvent.uChar.AsciiChar == '\n') {
                // if (idx == strlen(file_line)) {
                //     printf("%s", "done");
                //     break;
                // } else {
                continue;
                // }
            }

            correct[idx] = file_line[idx] == input.Event.KeyEvent.uChar.AsciiChar;
            // printf("%s", file_line[idx]);
            ++idx;
            printf("%s", CLEAR);
            for (int i = 0; i < idx; i++) {
                if (correct[i]) {
                    printf(GREEN "%c", file_line[i]);
                } else {
                    printf(RED "%c", file_line[i]);
                }
            }
            printf(RESET "%s", &(file_line[idx]));
            
        }
        idx = 0;
    }
    printf(RESET);
}
