#include "window-util.h"
#include <time.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define WIN_SIZE_ROWS 20
#define WIN_SIZE_COLS 70

char *choices[] = {
    "Word by word test",
    "About",
    "Exit",
    (char *)NULL,
};

char *desc = "";

void test_func();
int main_menu();
void about_func();

int main() {
    /* Initialize curses */
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);

    int curr_choice = 0;
    do {
        curr_choice = main_menu();

        if (curr_choice == 0) {
            test_func();
        } else if (curr_choice == 1) {
            about_func();
        }
    } while(curr_choice != 2);
    endwin();
}

/**
 * @brief Function to implement the no error allowed typing test on a separate window
 */
void test_func() {
    int c;
    WINDOW *wbw_win;
    int starty = (LINES - WIN_SIZE_ROWS) / 2;
    int startx = (COLS - WIN_SIZE_COLS) / 2;
    wbw_win = create_window(startx, starty, WIN_SIZE_ROWS, WIN_SIZE_COLS, "No Error Allowed Test", COLOR_PAIR(1));

    c = 0;

    do {
        int startX = 3;
        print_in_middle_para(wbw_win, 5, startX, WIN_SIZE_COLS - (startX * 2), "In this test, you will be required to type word by word (enter to go to next word). In the end you will be given an error score as well. You will not be able to see the whole text though, only the current word highlighted. The test will start as soon you press the key.", COLOR_PAIR(2));
        wattron(wbw_win, A_BOLD);
        print_in_middle(wbw_win, 16, 0, WIN_SIZE_COLS, "Press Enter to start", COLOR_PAIR(4));
        print_in_middle(wbw_win, 17, 0, WIN_SIZE_COLS, "Enter character 'x' to exit", COLOR_PAIR(3));
        wattroff(wbw_win, A_BOLD);
        wrefresh(wbw_win);
        if (c == 10) break; /* Enter key is pressed */
        else if (c == (int)'x') {
            destroy_win(wbw_win);
            return;
        }
    } while((c = wgetch(wbw_win)));

    wclear(wbw_win);
    renew_win(wbw_win, WIN_SIZE_ROWS, WIN_SIZE_COLS, "No Error Allowed Test", COLOR_PAIR(1));
    mvwhline(wbw_win, 17, 2, ACS_S1, WIN_SIZE_COLS - 3);
    wrefresh(wbw_win);

    FILE *wbwfile = fopen("./assets/wbw-short.txt", "r");
    int count = 0, check, loopExit = 0;
    long double tm = 0;
    time_t tic = (time_t)0;

    if (wbwfile == NULL) {
        print_in_middle(wbw_win, 3, 0, WIN_SIZE_COLS, "FILE FAILED TO OPEN", COLOR_PAIR(3));
    } else {
        echo();
        char word[WIN_SIZE_COLS];
        char user_input[WIN_SIZE_COLS] = "\0";
        char message[WIN_SIZE_COLS] = "\0";
        int index;

        while (fscanf(wbwfile, "%99s", word) == 1) {
            print_in_middle(wbw_win, 4, 0, WIN_SIZE_COLS, word, COLOR_PAIR(2));
            sprintf(message, "Time taken in previous word : %.2lf", (double)tic);
            print_in_middle(wbw_win, 8, 0, WIN_SIZE_COLS, message, COLOR_PAIR(4));
            print_in_middle(wbw_win, 9, 0, WIN_SIZE_COLS, "Type quit() at any time to quit", COLOR_PAIR(3));
            wrefresh(wbw_win);
            tic = time(NULL);
            do {
                check = 1;
                mvwgetstr(wbw_win, 16, 2, user_input);
                index = strlen(word);
                check = (strcmp(word, user_input)) ? 0 : 1;
                loopExit = (strcmp(user_input, "quit()")) ? 0 : 1;
                mvwprintw(wbw_win, 16, 2, "                         ");
            } while (!check && !loopExit);
            if (loopExit) break;
            tic = time(NULL) - tic;
            print_in_middle(wbw_win, 4, 0, WIN_SIZE_COLS, "                         ", COLOR_PAIR(2));
            strcpy(word, "\0");
            strcpy(user_input, "\0");
            count += index;
            tm = tm + (long double)(tic);
        }
    }
    noecho();

    wclear(wbw_win);
    renew_win(wbw_win, WIN_SIZE_ROWS, WIN_SIZE_COLS, "No Error Allowed Test", COLOR_PAIR(1));
    wrefresh(wbw_win);

    mvwprintw(wbw_win, 6, 2, "Time Taken : %Lf seconds", tm);
    mvwprintw(wbw_win, 7, 2, "Speed : %Lf characters per second", (long double)count / tm);
    print_in_middle(wbw_win, 16, 0, WIN_SIZE_COLS, "Press any key to exit", COLOR_PAIR(3));
    c = wgetch(wbw_win);

    destroy_win(wbw_win);
}

/**
 * @brief Creates a menu list of items using the choices array and waits for user choice
 * @return An integer representing the user choice/menu item
 */
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

    /* Set menu mark to the string " > " */
    set_menu_mark(my_menu, " > ");

    /* Print a border around the main window and print a title */
    renew_win(my_menu_win, 10, 60, "TyTest", COLOR_PAIR(1));
    refresh();

    /* Post the menu */
    post_menu(my_menu);
    wrefresh(my_menu_win);

    while((c = wgetch(my_menu_win))) {
        switch(c) {
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

/**
 * @brief Displays 'about software' information on a separate window
 */
void about_func() {
    int c;
    WINDOW *about_win;
    int starty = (LINES - WIN_SIZE_ROWS) / 2;
    int startx = (COLS - WIN_SIZE_COLS) / 2;
    about_win = create_window(startx, starty, WIN_SIZE_ROWS, WIN_SIZE_COLS, "About TyTest", COLOR_PAIR(1));

    print_in_middle_para(about_win, 4, 0, WIN_SIZE_COLS, "TyTest (v0.1.0) is a terminal based typing test application. It is made as a hobbyist project to demonstrate use of ncurses library in C. It currently supports only a word by word test. I plan to add different types of tests to it in the future, stay tuned!", COLOR_PAIR(2));
    print_in_middle(about_win, 17, 0, WIN_SIZE_COLS, "Press any key to exit", COLOR_PAIR(3));

    c = wgetch(about_win);
    destroy_win(about_win);
}
