#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <assert.h>

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

#ifdef __linux__
#include "get_input_linux.h"
#elif _WIN32
#include "get_input_windows.h"
#endif

#define LINE_LENGTH 80
#define DISPLAY_LENGTH (LINE_LENGTH) + 2
#define INPUT_FILE "./lyrics.txt"

FILE *fptr;
char file_line[LINE_LENGTH], display_line[DISPLAY_LENGTH], *file_name;
int count, num_skipped, is_correct, num_correct, num_chars, num_correct_adjusted, num_chars_adjusted, correct[LINE_LENGTH];
size_t idx;
time_t start_time, end_time;

void error_exit(char *message);
void do_display_line(char *file_line, char *display_line);
void display_stats();
void ctrlc_handler();
void end_game();
void init(int argc, char **argv);
int is_whitespace(char);

void error_exit(char *message) {
    fprintf(stdout, "%s", message);
    exit(1);
}

void do_display_line(char *file_line, char *display_line) {
    strncpy(display_line, file_line, LINE_LENGTH);
    int newline_index = strcspn(display_line, "\n"); 
    strncpy(display_line + newline_index, "\\n\n", 4);
    printf(CLEAR RESET "%s", display_line);
    if (is_correct) {
        printf(GREEN);
    } else {
        printf(RED);
    }
}

void display_stats() {
    printf(RESET CLEAR);
    end_time = time(NULL);
    float accuracy = 100.0 * num_correct / num_chars;
    float time_taken = (end_time - start_time) / 60.0;
    int minutes_taken = (end_time - start_time) / 60;
    int seconds_taken = (end_time - start_time) % 60;
    float gross_wpm = num_chars / 5.0 / time_taken;
    float net_wpm = (num_chars / 5 - (num_chars_adjusted - num_correct_adjusted)) / time_taken;

    printf("Accuracy: %.2f%%\n", accuracy);
    printf("Time Taken: %dm %ds\n", minutes_taken, seconds_taken);
    printf("WPM Gross: %.0f\n", gross_wpm);
    printf("WPM Net: %.0f\n", net_wpm);
    fflush(stdout);
}

void ctrlc_handler() {
    exit(0);
}

void end_game() {
    display_stats();
    restore_input();
    fclose(fptr);
}

void init(int argc, char **argv) {
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
    count = num_skipped = idx = is_correct = num_chars_adjusted = num_correct_adjusted = num_correct = num_chars = 0;

    if (init_input()) error_exit("cannot initialize input");
    signal(SIGTERM, ctrlc_handler);
    signal(SIGINT, ctrlc_handler);
    atexit(end_game);
}

int is_whitespace(char character) {
    assert(character != '\r');
    return character == '\t' || character == ' ';
}

int main(int argc, char **argv) {
    init(argc, argv);
    while (fgets(file_line, LINE_LENGTH, fptr)) {
        do_display_line(file_line, display_line);
        size_t line_length = strlen(file_line);
        while(idx != line_length) {
            // get input
            int player_in = get_input();
            if (!player_in) continue;

            // handle special characters
            if (player_in == '\b') {
                if (idx > 0) {
                    idx--;
                    putchar('\b');
                }
                continue;
            } else if (player_in == '\n' && file_line[idx] != '\n') {
                continue;
            } else if (player_in == '\t') {
                while (idx < line_length && !is_whitespace(file_line[idx])) {
                    idx += 1;
                    putchar(' ');
                }
                while (idx < line_length && is_whitespace(file_line[idx])) {
                    idx += 1;
                    putchar(' ');
                }
                continue;
            }

            // update correctness
            correct[idx] = file_line[idx] == player_in;
            if (correct[idx]) {
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
            putchar(player_in);
            ++idx;
        }
        num_chars_adjusted += idx;
        for (int i = 0; i < idx; i++) {
            if (correct[i]) num_correct_adjusted++;
        }
        idx = 0;
    }
    return 0;
}
