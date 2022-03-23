#include "window-util.h"
#include <time.h>

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
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);

    int curr_choice = 0;
    do {
        curr_choice = main_menu();

        if (curr_choice == 0) {
            test_func();
        }
        else if (curr_choice == 1) {
            about_func();
        }
    } while(curr_choice != 2);
    endwin();
}

void test_func() {
    int c;
    WINDOW *wbw_win;
    int starty = (LINES - 20) / 2;
    int startx = (COLS - 100) / 2;
    wbw_win = create_window(startx, starty, 20, 100, "No Error Allowed Test", COLOR_PAIR(1));

    c = 0;

    do {
        print_in_middle_para(wbw_win, 5, 15, 70, "In this test, you will be required to type word by word. In the end you will be given an error score as well. You will not be able to see the whole text though, only the current word highlighted. The test will start as soon you press the key.", COLOR_PAIR(2));
        wattron(wbw_win, A_BOLD);
        print_in_middle(wbw_win, 16, 0, 100, "Press Enter to start", COLOR_PAIR(4));
        print_in_middle(wbw_win, 17, 0, 100, "Enter character 'x' to exit", COLOR_PAIR(3));
        wattroff(wbw_win, A_BOLD);
        wrefresh(wbw_win);
        if (c == 10) break; /* Enter key is pressed */
        else if (c == (int)'x') {
            destroy_win(wbw_win);
            return;
        }
    } while((c = wgetch(wbw_win)));

    renew_win(wbw_win, 20, 100, "No Error Allowed Test", COLOR_PAIR(1));
    mvwhline(wbw_win, 17, 2, '_', 97);
    wrefresh(wbw_win);

    FILE *wbwfile = fopen("wbw-short.txt", "r");
    int count = 0, check, loopExit = 0;
    long double tm = 0;
    time_t tic = (time_t)0;

    if (wbwfile == NULL) {
        print_in_middle(wbw_win, 3, 0, 100, "FILE FAILED TO OPEN", COLOR_PAIR(3));
    }
    else {
        echo();
        char word[100];
        char user_input[100] = "\0";
        char message[100] = "\0";
        int index;

        while (fscanf(wbwfile, "%99s", word) == 1) {
            print_in_middle(wbw_win, 4, 0, 100, word, COLOR_PAIR(2));
            sprintf(message, "Time taken in previous word : %.2lf", (double)tic);
            print_in_middle(wbw_win, 8, 0, 100, message, COLOR_PAIR(4));
            print_in_middle(wbw_win, 9, 0, 100, "Type quit() at any time to quit", COLOR_PAIR(3));
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
            print_in_middle(wbw_win, 4, 0, 100, "                         ", COLOR_PAIR(2));
            strcpy(word, "\0");
            strcpy(user_input, "\0");
            count += index;
            tm = tm + (long double)(tic);
        }
    }
    noecho();

    renew_win(wbw_win, 20, 100, "No Error Allowed Test", COLOR_PAIR(1));
    mvwhline(wbw_win, 17, 2, '_', 97);
    wrefresh(wbw_win);

    mvwprintw(wbw_win, 6, 2, "Time Taken : %Lf seconds", tm);
    mvwprintw(wbw_win, 7, 2, "Speed : %Lf characters per second", (long double)count / tm);
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

    /* Set menu mark to the string " > " */
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
    about_win = create_window(startx, starty, 20, 100, "About TyTest", COLOR_PAIR(1));

    print_in_middle_para(about_win, 4, 0, 100, "TyTest (v0.1.0) is a terminal based typing test application. It is made as a hobbyist project to demonstrate use of ncurses library in C. It currently supports only a word by word test. I plan to add different types of tests to it in the future, stay tuned!", COLOR_PAIR(2));
    print_in_middle(about_win, 17, 0, 100, "Press any key to exit", COLOR_PAIR(3));

    c = wgetch(about_win);
    destroy_win(about_win);
}