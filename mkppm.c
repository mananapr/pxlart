#include <stdio.h>
#include <stdlib.h>
#include "dangen.h"
#include "clut.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define GLYPHROW 16
#define BITMAP_WIDTH 128 /* FONT_WIDTH * GLYPH_ROW */
#define BITMAP_SKIP 2048 /* BITMAP_WIDTH * FONT_HEIGHT */

static void draw_glyph(unsigned char *data,
                       int cx, int cy,
                       int w, int h,
                       unsigned char c,
                       unsigned char fg, unsigned char bg)
{
    unsigned int x, y;
    unsigned char clr;
    unsigned int offset;
    unsigned int letter_offset;

    letter_offset = c - 32;
    letter_offset =
        (letter_offset % GLYPHROW)*FONT_WIDTH +
        BITMAP_SKIP*(letter_offset/GLYPHROW);

    for(y = 0; y < FONT_HEIGHT; y++) {
        for(x = 0; x < FONT_WIDTH; x++) {
            offset = letter_offset + x + y * BITMAP_WIDTH;
            if(dangen_pgm[offset]) {
                clr = bg;
            } else {
                clr = fg;
            }
            data[((cx * FONT_WIDTH) + x) + w * (y + (cy * FONT_HEIGHT))] = clr;
        }
    }
}

int hex2num(unsigned char c)
{
    switch(c) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': return 10;
        case 'b': return 11;
        case 'c': return 12;
        case 'd': return 13;
        case 'e': return 14;
        case 'f': return 15;
    }
    return 0;
}

void index2clr(int pos, unsigned char *clr)
{
    const char *rgb;
    unsigned char tmp;


    rgb = CLUT[pos];

    /* r */
    tmp = hex2num(rgb[0]) * 16;
    tmp += hex2num(rgb[1]);
    clr[0] = tmp;

    /* g */
    tmp = hex2num(rgb[3]);
    tmp += hex2num(rgb[2]) * 16;
    clr[1] = tmp;

    /* b */
    tmp = hex2num(rgb[5]);
    tmp += hex2num(rgb[4]) * 16;
    clr[2] = tmp;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    unsigned char *data;
    unsigned int sz;
    unsigned int i;
    unsigned char clr[3];
    int w, h;
    int cols, rows;
    int c, r;
    FILE *df;
    int glyph;
    int fg, bg;

    if(argc < 3) {
        fprintf(stderr, "Usage: %s file.data out.ppm\n", argv[0]);
        return 1;
    }

    df = fopen(argv[1], "r");


    fscanf(df, "%d %d\n", &cols, &rows);
    w = cols * FONT_WIDTH;
    h = rows * FONT_HEIGHT;
    sz = w * h;

    fp = fopen(argv[2], "w");
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", w, h);
    fprintf(fp, "255\n");
    data = calloc(1, sz);

    for(r = 0; r < rows; r++) {
        for(c = 0; c < cols; c++) {
            fscanf(df, "%d %d %d\n", &fg, &bg, &glyph);
            draw_glyph(data, c, r, w, h, (unsigned char)glyph, fg, bg);
        }
    }

    for(i = 0; i < sz; i++) {
        index2clr(data[i], clr);
        fwrite(clr, 1, 3, fp);
    }

    free(data);
    fclose(fp);
    fclose(df);
    return 0;
}
