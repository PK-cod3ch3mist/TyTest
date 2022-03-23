#ifndef WINDOW_UTIL_H
#define WINDOW_UTIL_H

#include <curses.h>
#include <menu.h>
#include <string.h>
#include <stdlib.h>

void print_in_middle_para(WINDOW*, int, int, int, char*, chtype);
void print_in_middle(WINDOW*, int, int, int, char*, chtype);
void renew_win(WINDOW*, int, int, char*, chtype);
WINDOW* create_window(int, int, int, int, char*, chtype);
void destroy_win(WINDOW*);


void print_in_middle_para(WINDOW* win, int sy, int sx, int width, char* msg, chtype color) {
    int len, x, y;
    float temp;
    int i, c = 0;
    int printLen = 0;
    int nextStart = 0;

    if (win == NULL) {
        win = stdscr;
    }
    getyx(win, y, x);
    if (sx) x = sx;
    if (sy) y = sy;
    if (!width) width = 80;

    len = strlen(msg);
    wattron(win, color);
    for (int i = 0; i < len; i++) {
        c++;
        if (msg[i] == ' ') {
            printLen = i - nextStart;
        }
        if (c == (width - 4)) {
            // on this line, print only till prev_space
            temp = (width - printLen)/2;
            x = sx + (int)temp;
            mvwprintw(win, y++, x, "%.*s", printLen, msg + nextStart);
            nextStart += printLen + 1;
            c = 0;
        }
    }
    temp = (width - (len - nextStart))/2;
    x = sx + (int)temp;
    mvwprintw(win, y, x, "%s", msg + nextStart);
    wattroff(win, color);
    wrefresh(win);
}

void print_in_middle(WINDOW* win, int starty, int startx, int width, char* msg, chtype color) {
    int length, x, y;
    float temp;

    if (win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if (startx)
        x = startx;
    if (starty)
        y = starty;
    if (width == 0)
        width = 80;

    length = strlen(msg);
    temp = (width - length)/2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", msg);
    wattroff(win, color);
    refresh();
}

void renew_win(WINDOW* win, int rows, int cols, char *title, chtype color) {
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattron(win, A_BOLD);
    print_in_middle(win, 1, 0, cols, title, color);
    wattroff(win, A_BOLD);
    mvwaddch(win, 2, 0, '+');
    mvwhline(win, 2, 1, '-', cols - 2);
    mvwaddch(win, 2, cols - 1, '+');
    wrefresh(win);
}

WINDOW* create_window(int startx, int starty, int rows, int cols, char *title, chtype color) {
    WINDOW *win;
    win = newwin(rows, cols, starty, startx);
    renew_win(win, rows, cols, title, color);
    return win;
}

void destroy_win(WINDOW *local_win) {
    wclear(local_win);
    /* box(local_win, ' ', ' '); : This won't produce the desired
     * result of erasing the window. It will leave it's four corners
     * and so an ugly remnant of window.
     */
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    /* The parameters taken are
     * 1. win: the window on which to operate
     * 2. ls: character to be used for the left side of the window
     * 3. rs: character to be used for the right side of the window
     * 4. ts: character to be used for the top side of the window
     * 5. bs: character to be used for the bottom side of the window
     * 6. tl: character to be used for the top left corner of the window
     * 7. tr: character to be used for the top right corner of the window
     * 8. bl: character to be used for the bottom left corner of the window
     * 9. br: character to be used for the bottom right corner of the window
     */
    wrefresh(local_win);
    delwin(local_win);
}
#endif