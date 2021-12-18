#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 4

char *choices[] = {
    "Word by Word Test",
    "No Correction Test",
    "About",
    "Exit",
    (char *)NULL,
};

char *desc = "";

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color) {
    int length, x, y;
    float temp;

    if (win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if (startx != 0)
        x = startx;
    if (starty != 0)
        y = starty;
    if (width == 0)
        width = 80;

    length = strlen(string);
    temp = (width - length)/2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
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

int main() {
    ITEM **my_items;
    int c;
    MENU *my_menu;
    WINDOW *my_menu_win;
    int n_choices, i;

    /* Counters to keep track of user choices */
    int curr_choice, menu_choice;
    menu_choice = curr_choice = 0;

    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);

    /* Create items */
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    for(i = 0; i < n_choices; ++i)
            my_items[i] = new_item(choices[i], desc);

    /* Crate menu */
    my_menu = new_menu((ITEM **)my_items);

    /* Create the window to be associated with the menu */
    my_menu_win = newwin(10, 60, 4, COLS/2 - 30);
    keypad(my_menu_win, TRUE);

    /* Set main window and sub window */
    set_menu_win(my_menu, my_menu_win);
    set_menu_sub(my_menu, derwin(my_menu_win, 6, 58, 3, 1));

    /* Set menu mark to the string " * " */
    set_menu_mark(my_menu, " > ");

    /* Print a border around the main window and print a title */
    box(my_menu_win, 0, 0);
    wattron(my_menu_win, A_BOLD);
    print_in_middle(my_menu_win, 1, 0, 60, "TyTest", COLOR_PAIR(1));
    wattroff(my_menu_win, A_BOLD);
    mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
    mvwhline(my_menu_win, 2, 1, ACS_HLINE, 58);
    mvwaddch(my_menu_win, 2, 59, ACS_RTEE);
    mvprintw(LINES - 2, 0, "F1 to exit");
    refresh();

    /* Post the menu */
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while((c = wgetch(my_menu_win)) != KEY_F(1))
    {       switch(c)
            {       case KEY_DOWN:
                            curr_choice++;
                            menu_driver(my_menu, REQ_DOWN_ITEM);
                            break;
                    case KEY_UP:
                            curr_choice--;
                            menu_driver(my_menu, REQ_UP_ITEM);
                            break;
                    case 10:
                            menu_choice = 1;
                            break;
            }
            if (menu_choice) break;
            wrefresh(my_menu_win);
    }

    /* Unpost the menu and free memory */
    unpost_menu(my_menu);
    free_menu(my_menu);
    for (i = 0; i < n_choices; ++i)
        free_item(my_items[i]);

    destroy_win(my_menu_win);

    if (curr_choice == 0) {
        WINDOW *wbw_win;
        int starty = (LINES - 20) / 2;
        int startx = (COLS - 100) / 2;
        wbw_win = newwin(20, 100, starty, startx);
        box(wbw_win, 0, 0);
        wattron(wbw_win, A_BOLD);
        print_in_middle(wbw_win, 1, 0, 100, "Word by Word Test", COLOR_PAIR(1));
        wattroff(wbw_win, A_BOLD);
        mvwaddch(wbw_win, 2, 0, ACS_LTEE);
        mvwhline(wbw_win, 2, 1, ACS_HLINE, 98);
        mvwaddch(wbw_win, 2, 99, ACS_RTEE);
        mvprintw(LINES - 2, 0, "F1 to exit");
        wrefresh(wbw_win);
        while((c = wgetch(my_menu_win)) != KEY_F(1));
    }
    endwin();
}