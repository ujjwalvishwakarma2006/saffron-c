#include "common.h"
#include "tui.h"

void tui_init() {
    initscr();
    cbreak();
    noecho();

    setlocale(LC_ALL, "");

    int win_rows, win_cols, input_rows = 5;
    getmaxyx(stdscr, win_rows, win_cols);

    // window boxes
    WINDOW *log_box = newwin(win_rows - input_rows, win_cols, 0, 0);
    WINDOW *input_box = newwin(input_rows, win_cols, win_rows - 5, 0);
    box(log_box, 0, 0);
    box(input_box, 0, 0);

    // actual windows; the trick is to use actual window dimension - 2 for row and cols
    log_win = derwin(log_box, win_rows - input_rows - 2, win_cols - 4, 1, 2);
    input_win = derwin(input_box, input_rows - 2, win_cols - 4, 1, 2);

    scrollok(log_win, TRUE);
    scrollok(input_win, TRUE);
    keypad(log_win, TRUE);
    keypad(input_win, TRUE);
    idlok(input_win, TRUE);

    wrefresh(log_box);
    wrefresh(input_box);
}