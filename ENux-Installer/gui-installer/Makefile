#
# makefile -- build system
#
# This builds the GUI installer, and provides a few utilities to help install the rest
#

CFILES   := gui.c main.c
OFILES   := $(CFILES:.c=.o)
TARG     := enux-installer-gui
CC       := gcc
CFLAGS   := -std=c99 -Wall -Werror `pkg-config --cflags gtk+-3.0` -g
CPPFLAGS := -DIN_ENUX_INSTALLER -D_POSIX_C_SOURCE=200809L
LDFLAGS  := `pkg-config --libs gtk+-3.0` -lui

.SUFFIXES: .c .o
.PHONY: gui-clean install

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(TARG): $(OFILES)
	$(CC) $(OFILES) -o $(TARG) $(LDFLAGS)

clean:
	rm -f $(OFILES) $(TARG)

install:
	install -Dsm 755 ${TARG} /usr/bin/enux-installer-gui

