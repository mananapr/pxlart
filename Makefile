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
	install -Dm 755 $(PROG) $(DEST)/$(PROG)
	install -Dm 755 mkppm $(DEST)/mkppm
	install -Dm 644 LICENSE /usr/share/licenses/$(PROG)/LICENSE
	install -Dm 644 pxlart.1 /usr/local/man/man1/pxlart.1

uninstall:
	rm -v $(DEST)/$(PROG)
	rm -v $(DEST)/mkppm
	rm -v /usr/share/licenses/$(PROG)/LICENSE
	rm -v /usr/local/man/man1/pxlart.1
