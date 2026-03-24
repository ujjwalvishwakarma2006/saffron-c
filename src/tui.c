#include "common.h"
#include "tui.h"

static const char *logo[] = {
    "███████╗ █████╗ ███████╗███████╗██████╗  ██████╗ ███╗   ██╗",
    "██╔════╝██╔══██╗██╔════╝██╔════╝██╔══██╗██╔═══██╗████╗  ██║",
    "███████╗███████║█████╗  █████╗  ██████╔╝██║   ██║██╔██╗ ██║",
    "╚════██║██╔══██║██╔══╝  ██╔══╝  ██╔══██╗██║   ██║██║╚██╗██║",
    "███████║██║  ██║██║     ██║     ██║  ██║╚██████╔╝██║ ╚████║",
    "╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝",
};

#define LOGO_LINES 6
#define HEADER_HEIGHT 10

void tui_init() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    curs_set(1);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(CP_CYAN,    COLOR_CYAN,    -1);
        init_pair(CP_GREEN,   COLOR_GREEN,   -1);
        init_pair(CP_MAGENTA, COLOR_MAGENTA, -1);
        init_pair(CP_YELLOW,  COLOR_YELLOW,  -1);
        init_pair(CP_RED,     COLOR_RED,     -1);
        init_pair(CP_DIM,     COLOR_WHITE,   -1);
    }

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int header_height = HEADER_HEIGHT;
    int hint_height = 1;
    int chat_height = rows - header_height - hint_height;

    WINDOW *header_window = newwin(header_height, cols, 0, 0);

    wattron(header_window, COLOR_PAIR(CP_CYAN) | A_BOLD);
    for (int i = 0; i < LOGO_LINES; i++) {
        mvwprintw(header_window, i + 1, 2, "%s", logo[i]);
    }
    wattroff(header_window, COLOR_PAIR(CP_CYAN) | A_BOLD);

    wattron(header_window, A_DIM);
    mvwprintw(header_window, 8, 2, "Encrypted P2P Chat");
    wattroff(header_window, A_DIM);

    const char *mode_name = (app_mode == SERVER) ? "SERVER" : "CLIENT";

    char connection_info_buffer[256];
    snprintf(connection_info_buffer, sizeof(connection_info_buffer),
             "Connected to: %s  |  Mode: %s", server_ip, mode_name);
    wattron(header_window, COLOR_PAIR(CP_GREEN));
    mvwprintw(header_window, 8, cols - (int)strlen(connection_info_buffer) - 2, "%s", connection_info_buffer);
    wattroff(header_window, COLOR_PAIR(CP_GREEN));

    char peer_info_buffer[128];
    snprintf(peer_info_buffer, sizeof(peer_info_buffer), "Peer: %s", peer_display_name);
    wattron(header_window, COLOR_PAIR(CP_GREEN));
    mvwprintw(header_window, 9, cols - (int)strlen(peer_info_buffer) - 2, "%s", peer_info_buffer);
    wattroff(header_window, COLOR_PAIR(CP_GREEN));

    wrefresh(header_window);

    WINDOW *chat_box_window = newwin(chat_height, cols, header_height, 0);
    box(chat_box_window, 0, 0);

    /* The divider reserves one input row and one spacer row at the bottom. */
    int divider_row = chat_height - 4;
    mvwhline(chat_box_window, divider_row, 1, ACS_HLINE, cols - 2);
    mvwaddch(chat_box_window, divider_row, 0, ACS_LTEE);
    mvwaddch(chat_box_window, divider_row, cols - 1, ACS_RTEE);

    wattron(chat_box_window, COLOR_PAIR(CP_CYAN) | A_BOLD);
    mvwprintw(chat_box_window, divider_row + 1, 2, ">>");
    wattroff(chat_box_window, COLOR_PAIR(CP_CYAN) | A_BOLD);

    wrefresh(chat_box_window);

    int log_rows = divider_row - 1;
    log_win = derwin(chat_box_window, log_rows, cols - 4, 1, 2);
    scrollok(log_win, TRUE);
    keypad(log_win, TRUE);
    wrefresh(log_win);

    input_win = derwin(chat_box_window, 1, cols - 7, divider_row + 1, 5);
    scrollok(input_win, TRUE);
    keypad(input_win, TRUE);
    idlok(input_win, TRUE);
    wrefresh(input_win);

    WINDOW *hint_window = newwin(hint_height, cols, rows - 1, 0);

    const char *hint_text = "/f <filepath> send file  |  /q quit";
    wattron(hint_window, A_DIM);
    mvwprintw(hint_window, 0, cols - (int)strlen(hint_text) - 2, "%s", hint_text);
    wattroff(hint_window, A_DIM);
    wrefresh(hint_window);
}