#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

char *brush;
int x, y;
int maxy, maxx;
int fg;
int bg;
int count = 2;
WINDOW *draw_win;
WINDOW *status_win;


/*
   Creates a new window with dimensions `height` and `width` starting at `starty` and `startx`
*/
WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    return local_win;
}


/*
   Creates current_win, preview_win and status_win
*/
void init_windows()
{
    getmaxyx(stdscr, maxy, maxx);
    draw_win = create_newwin(maxy-1, maxx, 0, 0);
    status_win = create_newwin(1, maxx, maxy-1, 0);
    keypad(draw_win, TRUE);
}


/*
    Initializes ncurses
*/
void init_curses()
{
    initscr();
    noecho();
    start_color();
    fg = 1;
    bg = 0;
    init_pair(1, 0, 0);
    init_pair(2, fg, bg);
}


/*
    Prints status line
*/
void printStatus()
{
    werase(status_win);
    wprintw(status_win, "(b)rush: ");
    wattron(status_win, COLOR_PAIR(count));
    wprintw(status_win, "%s",brush);
    wattroff(status_win, COLOR_PAIR(count));
    wprintw(status_win, "  (c)olor: %d  ", fg);
    wprintw(status_win, "(B)ackground: %d", bg);
    //wprintw(status_win, "  (s)ave  (l)oad");
    wrefresh(status_win);
}


/*
    Sets brush to some character
*/
void setBrush(char *newBrush)
{
    if( strcmp(newBrush,"default") == 0 )
    {
        setBrush("█");
        return;
    }
    int allocSize = snprintf(NULL, 0, "%s", newBrush);
    brush = malloc(allocSize+1);
    snprintf(brush, allocSize+1, "%s", newBrush);
}


/*
    Initializes the program
*/
void init()
{
    setlocale(LC_ALL, "");
    setBrush("█");
    init_curses();
    init_windows();
}


int main()
{
    init();
    int a;
    char temp[10];
    do
    {
        printStatus();
        a = wgetch(draw_win);
        switch(a)
        {
            case KEY_UP:
            case 'k':
                getyx(draw_win, y, x);
                wmove(draw_win, y-1, x);
                break;

            case KEY_DOWN:
            case 'j':
                getyx(draw_win, y, x);
                wmove(draw_win, y+1, x);
                break;

            case KEY_LEFT:
            case 'h':
                getyx(draw_win, y, x);
                wmove(draw_win, y, x-1);
                break;

            case KEY_RIGHT:
            case 'l':
                getyx(draw_win, y, x);
                wmove(draw_win, y, x+1);
                break;

            case 'd':
                getyx(draw_win, y, x);
                wmove(draw_win, y, x);
                wattron(draw_win, COLOR_PAIR(count));
                wprintw(draw_win, "%s", brush);
                wattroff(draw_win, COLOR_PAIR(count));
                wmove(draw_win, y, x+1);
                break;

            case 'e':
                getyx(draw_win, y, x);
                wmove(draw_win, y, x);
                wprintw(draw_win, "%s", " ");
                wmove(draw_win, y, x+1);
                break;

            case 'b':
                werase(status_win);
                wprintw(status_win, "Enter New Brush Symbol: ");
                echo();
                wgetnstr(status_win, temp, 10);
                noecho();
                free(brush);
                setBrush(temp);
                break;

            case 'c':
                werase(status_win);
                wprintw(status_win, "Enter Shell Color (0-255): ");
                echo();
                wscanw(status_win, "%d", &fg);
                init_pair(++count, fg, bg);
                noecho();
                break;

            case 'B':
                werase(status_win);
                wprintw(status_win, "Enter Shell Color (0-255): ");
                echo();
                wscanw(status_win, "%d", &bg);
                init_pair(++count, fg, bg);
                noecho();
                break;

            case 's':
                scr_dump("save.out");
                break;

            case 'o':
                wclear(draw_win);
                scr_restore("save.out");
                doupdate();
                refresh();
                break;
        }
    }while(a != 'q');
    endwin();
    free(brush);
    return 0;
}
