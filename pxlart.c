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
int brushsize = 0;

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

typedef struct {
    int fg;
    int bg;
    char c;
} pxl_cell;

pxl_cell *cells = NULL;

void write_cell(int x, int y, char c);
void erase_cell(int x, int y);
void store_cell(int x, int y, char c, int w, int h, int fg, int bg);

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
void init_windows(void)
{
    getmaxyx(stdscr, maxy, maxx);
    draw_win = create_newwin(maxy-1, maxx, 0, 0);
    status_win = create_newwin(1, maxx, maxy-1, 0);
    keypad(draw_win, TRUE);
}

/*
    Initializes ncurses
*/
void init_curses(void)
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
void printStatus(void)
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
    brushsize = allocSize;
    brush = malloc(allocSize+1);
    snprintf(brush, allocSize+1, "%s", newBrush);
}


/*
    Moves the cursor up
*/
void goUp(void)
{
    getyx(draw_win, y, x);
    y -= 1;
    if(y < 0) y = 0;
    wmove(draw_win, y, x);
}


/*
    Moves the cursor down
*/
void goDown(void)
{
    getyx(draw_win, y, x);
    y += 1;
    if(y >= (maxy - 1)) y = maxy - 2;
    wmove(draw_win, y, x);
}


/*
    Moves the cursor left
*/
void goLeft(void)
{
    getyx(draw_win, y, x);
    x -= 1;
    if(x < 0) x = 0;
    wmove(draw_win, y, x);
}


/*
    Moves the cursor right
*/
void goRight(void)
{
    getyx(draw_win, y, x);
    x += 1;
    if(x >= maxx) x = maxx - 1;
    wmove(draw_win, y, x);
}


/*
    Draw
*/
void draw(void)
{
    int i;
    getyx(draw_win, y, x);
    wmove(draw_win, y, x);
    wattron(draw_win, COLOR_PAIR(count));
    wprintw(draw_win, "%s", brush);
    wattroff(draw_win, COLOR_PAIR(count));
    wmove(draw_win, y, x+1);
    for(i = 0; i < brushsize; i++) {
        write_cell(x + i, y, brush[i]);
    }
}

/*
    Erase
*/
void eraser(void)
{
    getyx(draw_win, y, x);
    wmove(draw_win, y, x);
    wprintw(draw_win, "%s", " ");
    erase_cell(x, y);
    wmove(draw_win, y, x+1);
}

/*
    Initializes the program
*/
void init(void)
{
    int sz;
    int i;
    setlocale(LC_ALL, "");
    setBrush("#");
    init_curses();
    init_windows();
    sz = maxx * maxy;
    cells = calloc(1, sizeof(pxl_cell) * sz);
    for(i = 0; i < sz; i++) {
        cells[i].c = ' ';
        cells[i].fg = 0;
        cells[i].bg = 0;
    }
}

/*
    Cleanup the program
*/
void cleanup(void)
{
    endwin();
    free(brush);
    free(cells);
}

/*
    Save Project
*/
void save(void)
{
    // For input (stdin & file)
    char *buf = NULL;
    // Stores Filename of save file
    char *filename = NULL;
    // Stores Filename of colors file
    char *colorsfile = NULL;
    // Stores Cell Data
    char *datafile = NULL;
    // Stores amount to allocate
    int allocSize;
    // Write all colors in `colorsfile`
    FILE *f;
    // Write data in `datafile`
    FILE *df;
    int sz;
    int i;

    werase(status_win);
    echo();
    wprintw(status_win, "Enter Name of Project: ");

    // Read filename and appropriately set `filename` and `colorsfile`
    buf = malloc(250);
    wgetnstr(status_win, buf, 250);
    allocSize = snprintf(NULL,0,"%s.save",buf);
    filename = malloc(allocSize+1);
    snprintf(filename,allocSize+1,"%s.save",buf);
    colorsfile = malloc(allocSize+1);
    snprintf(colorsfile,allocSize+1,"%s.cols",buf);
    datafile = malloc(allocSize+1);
    snprintf(datafile ,allocSize+1,"%s.data",buf);

    // Save screen output in ``filename
    scr_dump(filename);

    // Write all colors in `colorsfile`
    f = fopen(colorsfile,"w");
    df = fopen(datafile,"w");
    for(int i=1; i<=count; i++)
    {
        short int *a, *b;
        short int x, y;
        a = &x;
        b = &y;
        pair_content(i, a, b);
        fprintf(f,"%d %hd %hd\n", i, x, y);
    }

    fprintf(df, "%d %d\n", maxx, maxy);
    sz = maxx * maxy;

    for(i = 0; i < sz; i++) {
        fprintf(df, "%d %d %d\n",
                cells[i].fg,
                cells[i].bg,
                (unsigned char)cells[i].c);
    }

    // Close file and free memory
    fclose(f);
    fclose(df);
    free(filename);
    free(colorsfile);
    free(buf);
    noecho();
}

/*
    Load Project
*/
void load(void)
{
    // For input (stdin & file)
    char *buf = NULL;
    // Stores Filename of save file
    char *filename = NULL;
    // Stores Filename of colors file
    char *colorsfile = NULL;
    // Stores Filename of data file
    char *datafile = NULL;
    // Stores amount to allocate
    int allocSize;
    FILE *f;
    int w, h;
    int x, y;
    int the_fg, the_bg;
    int c;

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
    datafile = malloc(allocSize+1);
    snprintf(datafile,allocSize+1,"%s.data",buf);

    // If colorsfile doesn't exists, free memory and break
    f = fopen(colorsfile,"r");
    if(f == NULL)
    {
        noecho();
        free(filename);
        free(colorsfile);
        free(buf);
        free(datafile);
        return;
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
    fclose(f);
    // Restore screen layout
    scr_restore(filename);
    doupdate();


    f = fopen(datafile,"r");

    if(f != NULL) {
        fscanf(f, "%d %d\n", &w, &h);
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                fscanf(f, "%d %d %d\n", &the_fg, &the_bg, (int *)&c);
                store_cell(x, y, (char)c, w, h, the_fg, the_bg);
            }
        }
        fclose(f);
    }
    // free memory
    noecho();
    free(filename);
    free(colorsfile);
    free(datafile);
    free(buf);
}


/*
    Stores cell (using arbitrary width/height)
*/
void store_cell(int x, int y, char c, int w, int h, int fg, int bg)
{
    int pos;
    if(x < 0 || x >= w) return;
    if(y < 0 || y >= h) return;
    pos = y * w + x;
    cells[pos].c = c;
    cells[pos].fg = fg;
    cells[pos].bg = bg;
}

/*
    Write a cell
*/
void write_cell(int x, int y, char c)
{
    store_cell(x, y, c, maxx, maxy, fg, bg);
}

/*
    Erase a cell
*/
void erase_cell(int x, int y)
{
    write_cell(x, y, ' ');
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
                draw();
                break;

            // Erase the brush character
            case 'e':
                eraser();
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
                save();
                break;

            // Load
            case 'o':
                load();
                break;
        }
    } while(a != 'q');
    cleanup();
    return 0;
}
