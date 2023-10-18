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
#define DISPLAY_LENGTH (LINE_LENGTH) + 2

HANDLE hStdin;

void set_display_line(char* file_line, char* display_line) {
    strncpy_s(display_line, DISPLAY_LENGTH, file_line, LINE_LENGTH);
    int newline_index = strcspn(display_line, "\n"); 
    // display_line[newline_index] = '\0';
    // strncat_s(display_line, DISPLAY_LENGTH, "\\n\n", 4);
    strncpy_s(display_line + newline_index, DISPLAY_LENGTH, "\\n\n", 4);
}

int main() {
    FILE *fptr = fopen("D:\\agats", "r");
    char file_line[LINE_LENGTH], display_line[DISPLAY_LENGTH];
    int correct[LINE_LENGTH];
    int count = 0;
    int idx = 0;
    int is_correct = 0;
    DWORD buf;
    INPUT_RECORD input;

    hStdin = GetStdHandle(STD_INPUT_HANDLE);


    while (fgets(file_line, LINE_LENGTH, fptr)) {
        set_display_line(file_line, display_line);
        printf(CLEAR "%s", display_line);
        if (is_correct) {
            printf(GREEN);
        } else {
            printf(RED);
        }
        while(idx != strlen(file_line)) {
            ReadConsoleInput(hStdin, &input, 1, &buf);
            if (input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown) continue;
            char player_in = input.Event.KeyEvent.uChar.AsciiChar;
            if (player_in == 0) continue;
            if (player_in == '\b') {
                if (idx > 0) {
                    idx--;
                    printf("\b");
                }
                continue;
            }
            correct[idx] = file_line[idx] == player_in;
            if (correct[idx] && !is_correct) {
                is_correct = 1;
                printf(GREEN);
            } else if (!correct[idx] && is_correct) {
                is_correct = 0;
                printf(RED);
            }
            printf("%c", player_in);
            ++idx;
        }
        idx = 0;
        printf(CLEAR);
    }
    printf(RESET);
}
