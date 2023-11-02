#ifndef GET_INPUT_IMPLEMENTATION
#define GET_INPUT_IMPLEMENTATION
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;

int init_input(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON);
    return tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int get_input(void) {
    int input = getchar();
    if (input == 0x7f) {
        input = '\b';
    }
    return input;
}

int restore_input(void) {
    return tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

#endif