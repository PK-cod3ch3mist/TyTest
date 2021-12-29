#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

char *choices[] = {
    "Word by word test",
    "About",
    "Exit",
    (char *)NULL,
};

char *desc = "";

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
void destroy_win(WINDOW *local_win);
void test_func();
int main_menu();
void about_func();

int main() {
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_RED);

    int curr_choice = main_menu();

    if (curr_choice == 0) {
        test_func();
    }
    else if (curr_choice == 1) {
        about_func();
    }
    endwin();
}

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

void test_func() {
    int c;
    WINDOW *wbw_win;
    int starty = (LINES - 20) / 2;
    int startx = (COLS - 100) / 2;
    wbw_win = newwin(20, 100, starty, startx);
    wborder(wbw_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattron(wbw_win, A_BOLD);
    print_in_middle(wbw_win, 1, 0, 100, "Word by Word Test", COLOR_PAIR(1));
    wattroff(wbw_win, A_BOLD);
    mvwaddch(wbw_win, 2, 0, '+');
    mvwhline(wbw_win, 2, 1, '-', 98);
    mvwaddch(wbw_win, 2, 99, '+');

    c = 0;

    do {
        print_in_middle(wbw_win, 3, 0, 100, "In this test, you will be required to type word by word.", COLOR_PAIR(2));
        print_in_middle(wbw_win, 4, 0, 100, "In the end you will be given an error score as well.", COLOR_PAIR(2));
        print_in_middle(wbw_win, 5, 0, 100, "You will not be able to see the whole text though, only the current word highlighted.", COLOR_PAIR(2));
        print_in_middle(wbw_win, 6, 0, 100, "The test will start as soon you press the key.", COLOR_PAIR(2));
        wattron(wbw_win, A_BLINK);
        print_in_middle(wbw_win, 17, 0, 100, "Press Enter to start", COLOR_PAIR(3));
        wattroff(wbw_win, A_BLINK);
        wrefresh(wbw_win);
        if (c == 10) break; /* Enter key is pressed */
    } while((c = wgetch(wbw_win)));

    wclear(wbw_win);
    wborder(wbw_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattron(wbw_win, A_BOLD);
    print_in_middle(wbw_win, 1, 0, 100, "Word by Word Test", COLOR_PAIR(1));
    wattroff(wbw_win, A_BOLD);
    mvwaddch(wbw_win, 2, 0, '+');
    mvwhline(wbw_win, 2, 1, '-', 98);
    mvwaddch(wbw_win, 2, 99, '+');
    mvwhline(wbw_win, 17, 2, '_', 97);
    wrefresh(wbw_win);

    FILE *wbwfile = fopen("wbw.txt", "r");
    int inaccuracy = 0;

    if (wbwfile == NULL) {
        print_in_middle(wbw_win, 3, 0, 100, "FILE FAILED TO OPEN", COLOR_PAIR(3));
    }
    else {
        echo();
        char word[100];
        char user_input[100] = "\0";
        int index;

        while (fscanf(wbwfile, "%99s", word) == 1) {
            print_in_middle(wbw_win, 4, 0, 100, word, COLOR_PAIR(2));
            wrefresh(wbw_win);
            mvwgetstr(wbw_win, 16, 2, user_input);
            for (index = 0; word[index] != '\0'; index++) {
                if (word[index] != user_input[index]) inaccuracy++;
            }
            print_in_middle(wbw_win, 4, 0, 100, "                         ", COLOR_PAIR(2));
            mvwprintw(wbw_win, 16, 2, "                         ");
            strcpy(word, "\0");
            strcpy(user_input, "\0");
        }
    }
    noecho();

    wclear(wbw_win);
    wborder(wbw_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattron(wbw_win, A_BOLD);
    print_in_middle(wbw_win, 1, 0, 100, "Word by Word Test", COLOR_PAIR(1));
    wattroff(wbw_win, A_BOLD);
    mvwaddch(wbw_win, 2, 0, '+');
    mvwhline(wbw_win, 2, 1, '-', 98);
    mvwaddch(wbw_win, 2, 99, '+');
    mvwhline(wbw_win, 17, 2, '_', 97);
    wrefresh(wbw_win);

    mvwprintw(wbw_win, 6, 2, "Accuracy : %f", (float)((float)(499 - inaccuracy)/499)*100);
    print_in_middle(wbw_win, 16, 0, 100, "Press any key to exit", COLOR_PAIR(3));
    c = wgetch(wbw_win);

    destroy_win(wbw_win);
}

int main_menu() {
    ITEM **my_items;
    int c;
    MENU *my_menu;
    WINDOW *my_menu_win;
    int n_choices, i;

    /* Counters to keep track of user choices */
    int curr_choice, menu_choice;
    menu_choice = curr_choice = 0;

    /* Create items */
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
    for(i = 0; i < n_choices; ++i)
            my_items[i] = new_item(choices[i], desc);

    /* Create menu */
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
    wborder(my_menu_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattron(my_menu_win, A_BOLD);
    print_in_middle(my_menu_win, 1, 0, 60, "TyTest", COLOR_PAIR(1));
    wattroff(my_menu_win, A_BOLD);
    mvwaddch(my_menu_win, 2, 0, '+');
    mvwhline(my_menu_win, 2, 1, '-', 58);
    mvwaddch(my_menu_win, 2, 59, '+');
    refresh();

    /* Post the menu */
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while((c = wgetch(my_menu_win))) {       
        switch(c)
        {       
            case KEY_DOWN:
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

    return curr_choice;
}

void about_func() {
    int c;
    WINDOW *about_win;
    int starty = (LINES - 20) / 2;
    int startx = (COLS - 100) / 2;
    about_win = newwin(20, 100, starty, startx);
    wborder(about_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattron(about_win, A_BOLD);
    print_in_middle(about_win, 1, 0, 100, "About TyTest", COLOR_PAIR(1));
    wattroff(about_win, A_BOLD);
    mvwaddch(about_win, 2, 0, '+');
    mvwhline(about_win, 2, 1, '-', 98);
    mvwaddch(about_win, 2, 99, '+');

    print_in_middle(about_win, 4, 0, 100, "TyTest (v0.1.0) is a terminal based typing test application.", COLOR_PAIR(2));
    print_in_middle(about_win, 5, 0, 100, "It is made as a hobbyist project to demonstrate use of ncurses library in C.", COLOR_PAIR(2));
    print_in_middle(about_win, 6, 0, 100, "It currently supports only a word by word test.", COLOR_PAIR(2));
    print_in_middle(about_win, 7, 0, 100, "I plan to add different types of tests to it in the future, stay tuned!", COLOR_PAIR(2));
    print_in_middle(about_win, 17, 0, 100, "Press any key to exit", COLOR_PAIR(3));

    c = wgetch(about_win);
}