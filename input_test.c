#include "get_input_linux.h"
#include <stdio.h>

int main() {
    while (1) {
        init_input();
        int character = get_input();
        printf("char: %d\n", character);
    }
}