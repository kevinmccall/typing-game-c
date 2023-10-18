#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

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
#define INPUT_FILE "./lyrics.txt"

HANDLE hStdin;
FILE *fptr;
char file_line[LINE_LENGTH], display_line[DISPLAY_LENGTH], *file_name;
int count, idx, is_correct, num_correct, num_chars;
DWORD buf;
time_t start_time, end_time;
INPUT_RECORD input;

void do_display_line(char* file_line, char* display_line) {
    strncpy_s(display_line, DISPLAY_LENGTH, file_line, LINE_LENGTH);
    int newline_index = strcspn(display_line, "\n"); 
    strncpy_s(display_line + newline_index, DISPLAY_LENGTH, "\\n\n", 4);
    printf(CLEAR "%s", display_line);
    if (is_correct) {
        printf(GREEN);
    } else {
        printf(RED);
    }
}

void display_stats(void) {
    printf(RESET CLEAR);
    end_time = time(NULL);
    float accuracy = 100.0 * num_correct / num_chars;
    float time_taken = (end_time - start_time) / 60.0;
    int minutes_taken = (end_time - start_time) / 60;
    int seconds_taken = (end_time - start_time) % 60;
    float gross_wpm = num_chars / 5.0 / time_taken;

    printf("Accuracy: %.2f%%\n", accuracy);
    printf("Time Taken: %dm %ds\n", minutes_taken, seconds_taken);
    printf("WPM Gross: %.0f\n", gross_wpm);
    fclose(fptr);
}

void init(int argc, char** argv) {
    if (argc > 1) {
        file_name = argv[1];

    } else {
        file_name = INPUT_FILE;
    }

    fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        fprintf(stderr, "could not open %s", file_name);
        exit(1);
    }
    start_time = time(NULL);
    count = idx = is_correct = 0;
    hStdin = GetStdHandle(STD_INPUT_HANDLE);

    atexit(display_stats);
}

int main(int argc, char** argv) {
    init(argc, argv);
    
    while (fgets(file_line, LINE_LENGTH, fptr)) {
        do_display_line(file_line, display_line);
        while(idx != strlen(file_line)) {
            // get input
            ReadConsoleInput(hStdin, &input, 1, &buf);
            if (input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown) continue;
            char player_in = input.Event.KeyEvent.uChar.AsciiChar;
            if (player_in == 0) continue;

            // handle special characters
            if (player_in == '\b') {
                if (idx > 0) {
                    idx--;
                    printf("\b");
                }
                continue;
            } else if (player_in == 13 && file_line[idx] != 10) {
                continue;
            }

            // update correctness            
            if (file_line[idx] == player_in || player_in == 13 && file_line[idx] == 10) {
                if (!is_correct) {
                    printf(GREEN);
                }
                is_correct = 1;
                num_correct++;
            } else {
                if (is_correct) {
                    printf(RED);
                }
                is_correct = 0;
            }
            num_chars++;

            // display player input
            printf("%c", player_in);
            ++idx;
        }
        idx = 0;
        printf(CLEAR);
    }
    return 0;
}
