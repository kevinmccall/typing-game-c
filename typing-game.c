#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define BRIGHT_BLACK "\033[0;90m"
#define SET_CURSOR_POS "\033[%d;%dH"

#if defined(__linux__) || defined(TARGET_OS_MAC) || defined(__APPLE__)
#include "get_input_linux.h"
#elif defined(_WIN32)
#include "get_input_windows.h"
#endif

#define LINE_LENGTH 80
#define NUM_LINES_DISPLAYED 6
#define DISPLAY_LENGTH (LINE_LENGTH) + 2
#define INPUT_FILE "./lyrics.txt"

struct InputFile {
    char *text;
    size_t text_size;
};

struct GameData {
    int was_correct, *correct;
    size_t idx;
};

struct TypingData {
    int num_skipped, num_correct_adjusted, num_chars_adjusted;
    time_t start_time, end_time;
};

FILE *fptr;
struct TypingData typing_data;
struct GameData game_data;

void error_exit(char *message);
void do_display_line(char *file_line, char *display_line);
void display_stats();
void ctrlc_handler();
void end_game();
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
    if (game_data.was_correct) {
        printf(GREEN);
    } else {
        printf(RED);
    }
}

void display_stats() {
    printf(RESET CLEAR);
    typing_data.end_time = time(NULL);
    float accuracy = 100.0 * typing_data.num_correct_adjusted /
                     typing_data.num_chars_adjusted;
    float time_taken_minutes =
        (typing_data.end_time - typing_data.start_time) / 60.0;
    int minutes_taken = (typing_data.end_time - typing_data.start_time) / 60;
    int seconds_taken = (typing_data.end_time - typing_data.start_time) % 60;
    float gross_wpm = typing_data.num_chars_adjusted / 5.0 / time_taken_minutes;
    float net_wpm = typing_data.num_correct_adjusted / 5.0 / time_taken_minutes;

    printf("Characters Typed: %d\n", typing_data.num_chars_adjusted);
    printf("Accuracy: %.2f%%\n", accuracy);
    printf("Time Taken: %dm %ds\n", minutes_taken, seconds_taken);
    printf("WPM Gross: %.0f\n", gross_wpm);
    printf("WPM Net: %.0f\n", net_wpm);
    fflush(stdout);
}

void ctrlc_handler() { exit(0); }

void end_game() {
    display_stats();
    restore_input();
    fclose(fptr);
}

int is_whitespace(char character) {
    assert(character != '\r');
    return character == '\t' || character == ' ';
}

void read_file(FILE *file, struct InputFile *in_file) {
    fseek(file, 0, SEEK_END);
    in_file->text_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    in_file->text = (char *)malloc(sizeof(char) * in_file->text_size);
    fread(in_file->text, sizeof(char), in_file->text_size, file);
}

void free_file(struct InputFile *in_file) { free(in_file->text); }

int main(int argc, char **argv) {
    struct InputFile input_file;
    char *file_name;
    typing_data.start_time = time(NULL);

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
    read_file(fptr, &input_file);

    if (init_input())
        error_exit("cannot initialize input");
    signal(SIGTERM, ctrlc_handler);
    signal(SIGINT, ctrlc_handler);
    atexit(end_game);

    game_data.correct = (int *)malloc(input_file.text_size * sizeof(int));
    game_data.idx = 0;
    game_data.was_correct = 0;

    printf(CLEAR RESET);
    size_t display_accum = 0;
    for (size_t i = 0; i < NUM_LINES_DISPLAYED; i++) {
        for (size_t j = 0; j < LINE_LENGTH; j++) {
            if (input_file.text[display_accum] == '\n') {
                printf(YELLOW "\\n\n" RESET);
                display_accum++;
                break;
            }
            putchar(input_file.text[display_accum]);
            display_accum++;
        }
    }
    printf(SET_CURSOR_POS, 0, 0);

    while (game_data.idx != input_file.text_size) {
        // get input
        int player_in = get_input();
        if (!player_in)
            continue;

        // handle special characters
        if (player_in == '\b') {
            if (game_data.idx > 0 &&
                input_file.text[game_data.idx - 1] != '\n') {
                game_data.idx--;
                typing_data.num_chars_adjusted--;
                if (game_data.correct[game_data.idx]) {
                    typing_data.num_correct_adjusted--;
                    game_data.correct[game_data.idx] = 0;
                }
                putchar('\b');
            }
            continue;
        } else if (player_in == '\n' &&
                   input_file.text[game_data.idx] != '\n') {
            continue;
        } else if (player_in == '\t') {
            while (game_data.idx < input_file.text_size &&
                   !is_whitespace(input_file.text[game_data.idx])) {
                game_data.idx += 1;
                typing_data.num_skipped += 1;
                putchar(' ');
            }
            while (game_data.idx < input_file.text_size &&
                   is_whitespace(input_file.text[game_data.idx])) {
                game_data.idx += 1;
                typing_data.num_skipped += 1;
                putchar(' ');
            }
            continue;
        }

        // update correctness
        game_data.correct[game_data.idx] =
            input_file.text[game_data.idx] == player_in;
        if (game_data.correct[game_data.idx]) {
            if (!game_data.was_correct) {
                printf(GREEN);
            }
            game_data.was_correct = 1;
            typing_data.num_correct_adjusted++;
        } else {
            if (game_data.was_correct) {
                printf(RED);
            }
            game_data.was_correct = 0;
        }
        typing_data.num_chars_adjusted++;

        // display player input
        putchar(player_in);
        ++game_data.idx;
    }
    free_file(&input_file);
    free(game_data.correct);
    return 0;
}
