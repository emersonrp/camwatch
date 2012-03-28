# Makefile for camwatch
PREFIX = /usr/local
VERSION = 0.2
DEBUG = 1


CC = gcc -g -Wall

IFLAGS = -I/usr/X11R6/include -I/usr/local/include -I/usr/include
LFLAGS = -L/usr/X11R6/lib -L/usr/local/lib -lX11 -lXext -ljpeg -lpng -ltiff -lz -lgif -lm -lImlib -lcurl -lcurses

CFLAGS += $(IFLAGS)

OBJS = url.o get.o camwatch.o

all: camwatch

camwatch: $(OBJS)
  $(CC) -o camwatch $(OBJS) $(IFLAGS) $(LFLAGS)

wmhints: wmhints.o
  $(CC) -o wmhints wmhints.o $(IFLAGS) $(LFLAGS)

install: camwatch
  strip camwatch
  install -m755 camwatch $(PREFIX)/bin/camwatch
  install -m644 camwatch.1 $(PREFIX)/man/man1/camwatch.1

uninstall:
  rm -f $(PREFIX)/bin/camwatch
  rm -f $(PREFIX)/bin/man/man1/camwatch.1*

dist: clean
  cp -a . camwatch-$(VERSION)
  tar zcf camwatch-$(VERSION).tgz camwatch-$(VERSION)
  rm -rf camwatch-$(VERSION)
  @echo "camwatch-$(VERSION).tgz is ready for distribution."

clean: 
  rm -f *.o *~ camwatch core 
