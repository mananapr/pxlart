CFLAGS = -I. -Wall -g

OS = $(shell uname -s)

ifeq ($(OS), Darwin)
# OSX Flags (using Homebrew ncurses)
LIBS += -L/usr/local/opt/ncurses/lib
CFLAGS += -I/usr/local/opt/ncurses/include
LIBS += -lncurses
endif

ifeq ($(OS), Linux)
LIBS += -lncursesw
endif

SRCS = pxlart.c
OBJS = $(SRCS:.c=.o)
PROG = pxlart
DEST = /usr/local/bin

all: pxlart mkppm

pxlart: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG) $(LIBS)

mkppm: mkppm.o
	$(CC) $(CFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJS)
	$(RM) mkpm
	$(RM) pxlart

install:
	cp -v $(PROG) $(DEST)

uninstall:
	rm -v "$(DEST)/$(PROG)"
