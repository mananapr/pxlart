/*
           _            _
 _ ____  _| | __ _ _ __| |
| '_ \ \/ / |/ _` | '__| __|
| |_) >  <| | (_| | |  | |
| .__/_/\_\_|\__,_|_|   \__|
|_|

*/

/////////////
// HEADERS //
/////////////
#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>


//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// Stores the brush
char *brush = NULL;

// Stores the current coordinates for the cursor
int x, y;

// Stores the dimensions of the window
int maxy, maxx;

// Stores current foreground color
int fg;

// Stores current background color
int bg;

// Stores the current color count
int count = 2;

// Window for the draw area
WINDOW *draw_win;

// Window for status line
WINDOW *status_win;


//////////////////////
// HELPER FUNCTIONS //
//////////////////////

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
    draw_win = create_newwin(maxy-2, maxx, 0, 0);
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
    free(brush);
    int allocSize = snprintf(NULL, 0, "%s", newBrush);
    brush = malloc(allocSize+1);
    snprintf(brush, allocSize+1, "%s", newBrush);
}


/*
    Moves the cursor up
*/
void goUp()
{
    getyx(draw_win, y, x);
    wmove(draw_win, y-1, x);
}


/*
    Moves the cursor down
*/
void goDown()
{
    getyx(draw_win, y, x);
    wmove(draw_win, y+1, x);
}


/*
    Moves the cursor left
*/
void goLeft()
{
    getyx(draw_win, y, x);
    wmove(draw_win, y, x-1);
}


/*
    Moves the cursor right
*/
void goRight()
{
    getyx(draw_win, y, x);
    wmove(draw_win, y, x+1);
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


///////////////////
// MAIN FUNCTION //
///////////////////

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
                goUp();
                break;

            case KEY_DOWN:
            case 'j':
                goDown();
                break;

            case KEY_LEFT:
            case 'h':
                goLeft();
                break;

            case KEY_RIGHT:
            case 'l':
                goRight();
                break;

            // Draw the brush character
            case ' ':
            case 'd':
                getyx(draw_win, y, x);
                wmove(draw_win, y, x);
                wattron(draw_win, COLOR_PAIR(count));
                wprintw(draw_win, "%s", brush);
                wattroff(draw_win, COLOR_PAIR(count));
                wmove(draw_win, y, x+1);
                break;

            // Erase the brush character
            case 'e':
                getyx(draw_win, y, x);
                wmove(draw_win, y, x);
                wprintw(draw_win, "%s", " ");
                wmove(draw_win, y, x+1);
                break;

            // Set new brush character
            case 'b':
                werase(status_win);
                wprintw(status_win, "Enter New Brush Symbol: ");
                echo();
                wgetnstr(status_win, temp, 10);
                noecho();
                setBrush(temp);
                break;

            // Set new foreground color
            case 'c':
                werase(status_win);
                wprintw(status_win, "Enter Shell Color (0-255): ");
                echo();
                wscanw(status_win, "%d", &fg);
                init_pair(++count, fg, bg);
                noecho();
                break;

            // Set new background color
            case 'B':
                werase(status_win);
                wprintw(status_win, "Enter Shell Color (0-255): ");
                echo();
                wscanw(status_win, "%d", &bg);
                init_pair(++count, fg, bg);
                noecho();
                break;

            // Save
            case 's':
                werase(status_win);
                echo();
                wprintw(status_win, "Enter Name of Project: ");

                // For input (stdin & file)
                char *buf = NULL;
                // Stores Filename of save file
                char *filename = NULL;
                // Stores Filename of colors file
                char *colorsfile = NULL;
                // Stores amount to allocate
                int allocSize;

                // Read filename and appropriately set `filename` and `colorsfile`
                buf = malloc(250);
                wgetnstr(status_win, buf, 250);
                allocSize = snprintf(NULL,0,"%s.save",buf);
                filename = malloc(allocSize+1);
                snprintf(filename,allocSize+1,"%s.save",buf);
                colorsfile = malloc(allocSize+1);
                snprintf(colorsfile,allocSize+1,"%s.cols",buf);

                // Save screen output in ``filename
                scr_dump(filename);

                // Write all colors in `colorsfile`
                FILE *f = fopen(colorsfile,"a+");
                for(int i=1; i<=count; i++)
                {
                    short int *a, *b;
                    short int x, y;
                    a = &x;
                    b = &y;
                    pair_content(i, a, b);
                    fprintf(f,"%d %hd %hd\n", i, x, y);
                }

                // Close file and free memory
                fclose(f);
                free(filename);
                free(colorsfile);
                free(buf);
                noecho();
                break;

            // Load
            case 'o':
                werase(status_win);
                echo();
                wprintw(status_win, "Enter Name of Project: ");

                // Take filename input and appropriately set `filename` and `colorsfile`
                buf = malloc(250);
                wgetnstr(status_win, buf, 250);
                allocSize = snprintf(NULL,0,"%s.save",buf);
                filename = malloc(allocSize+1);
                snprintf(filename,allocSize+1,"%s.save",buf);
                colorsfile = malloc(allocSize+1);
                snprintf(colorsfile,allocSize+1,"%s.cols",buf);

                // If colorsfile doesn't exists, free memory and break
                f = fopen(colorsfile,"r");
                if(f == NULL)
                {
                    noecho();
                    free(filename);
                    free(colorsfile);
                    free(buf);
                    break;
                }

                // Remove existing color pairs
                reset_color_pairs();

                // Load color pairs from `colorsfile`
                int i=1;
                while(fgets(buf, 250, (FILE*) f))
                {
                    int n, f, b;
                    sscanf(buf,"%d %d %d", &n, &f, &b);
                    init_pair(n, f, b);
                    count = i++;
                }

                // Restore screen layout
                scr_restore(filename);
                doupdate();

                // CLose file and free memory
                fclose(f);
                noecho();
                free(filename);
                free(colorsfile);
                free(buf);
                break;
        }
    } while(a != 'q');
    endwin();
    free(brush);
    return 0;
}
