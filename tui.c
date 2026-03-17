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

    // Initialize colors
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

    int header_h = HEADER_HEIGHT;
    int hint_h   = 1;
    int chat_h   = rows - header_h - hint_h;

    // ======================== HEADER (no border) ========================
    WINDOW *header_win = newwin(header_h, cols, 0, 0);

    // Logo — cyan bold
    wattron(header_win, COLOR_PAIR(CP_CYAN) | A_BOLD);
    for (int i = 0; i < LOGO_LINES; i++) {
        mvwprintw(header_win, i + 1, 2, "%s", logo[i]);
    }
    wattroff(header_win, COLOR_PAIR(CP_CYAN) | A_BOLD);

    // Tagline — dim, left side
    wattron(header_win, A_DIM);
    mvwprintw(header_win, 8, 2, "Encrypted P2P Chat");
    wattroff(header_win, A_DIM);

    // Connection info — green, right-aligned
    const char *mode_str = (app_mode == SERVER) ? "SERVER" : "CLIENT";

    char info_line[256];
    snprintf(info_line, sizeof(info_line),
             "Connected to: %s  |  Mode: %s", server_ip, mode_str);
    wattron(header_win, COLOR_PAIR(CP_GREEN));
    mvwprintw(header_win, 8, cols - (int)strlen(info_line) - 2, "%s", info_line);
    wattroff(header_win, COLOR_PAIR(CP_GREEN));

    char peer_line[128];
    snprintf(peer_line, sizeof(peer_line), "Peer: %s", display_name);
    wattron(header_win, COLOR_PAIR(CP_GREEN));
    mvwprintw(header_win, 9, cols - (int)strlen(peer_line) - 2, "%s", peer_line);
    wattroff(header_win, COLOR_PAIR(CP_GREEN));

    wrefresh(header_win);

    // ============= CHAT BOX (log + input, single bordered box) =============
    WINDOW *chat_box = newwin(chat_h, cols, header_h, 0);
    box(chat_box, 0, 0);

    // Horizontal divider between log area and input area
    int divider_row = chat_h - 4;
    mvwhline(chat_box, divider_row, 1, ACS_HLINE, cols - 2);
    mvwaddch(chat_box, divider_row, 0, ACS_LTEE);         // ├
    mvwaddch(chat_box, divider_row, cols - 1, ACS_RTEE);   // ┤

    // >> prompt — cyan bold, drawn on the box
    wattron(chat_box, COLOR_PAIR(CP_CYAN) | A_BOLD);
    mvwprintw(chat_box, divider_row + 1, 2, ">>");
    wattroff(chat_box, COLOR_PAIR(CP_CYAN) | A_BOLD);

    wrefresh(chat_box);

    // Log window — inner area above the divider
    int log_rows = divider_row - 1;
    log_win = derwin(chat_box, log_rows, cols - 4, 1, 2);
    scrollok(log_win, TRUE);
    keypad(log_win, TRUE);
    wrefresh(log_win);

    // Input window — inner area after >>, below divider
    input_win = derwin(chat_box, 1, cols - 7, divider_row + 1, 5);
    scrollok(input_win, TRUE);
    keypad(input_win, TRUE);
    idlok(input_win, TRUE);
    wrefresh(input_win);

    // ======================== HINT BAR (no border) ========================
    WINDOW *hint_win = newwin(hint_h, cols, rows - 1, 0);

    const char *hint_text = "/f <filepath> send file  |  /q quit";
    wattron(hint_win, A_DIM);
    mvwprintw(hint_win, 0, cols - (int)strlen(hint_text) - 2, "%s", hint_text);
    wattroff(hint_win, A_DIM);
    wrefresh(hint_win);
}