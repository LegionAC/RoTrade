#include "feature_utils.h"
#include <iostream>
#include <termios.h>
#include <csignal>

termios orig_termios;

void enable_raw_mode() {
    std::cout << "\033[?25l";
    tcgetattr(STDIN_FILENO, &orig_termios);
    termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(int) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    std::cout << "\033[?25h" << "\033[2J\033[H";
    std::exit(0);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int main() {
    signal(SIGINT, disable_raw_mode);

    enable_raw_mode();

    menu_nav();

    disable_raw_mode(0);
    return 0;
}