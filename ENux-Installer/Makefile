#
# makefile -- build system
#
# This builds the GUI installer, and provides a few utilities to help install the rest
#

GUI_CFILES   := enux-installer-gui.c
GUI_OFILES   := $(GUI_CFILES:.c=.o)
GUI_TARG     := enux-installer-gui
GUI_CC       := gcc
GUI_CFLAGS   := -std=c99 -Wall -Werror `pkg-config --cflags gtk+-3.0`
GUI_CPPFLAGS := -DIN_ENUX_INSTALLER -D_POSIX_C_SOURCE=200809L
GUI_LDFLAGS  := `pkg-config --libs gtk+-3.0` -lui

.SUFFIXES: .c .o
.PHONY: gui-clean install

.c.o:
	$(GUI_CC) $(GUI_CFLAGS) $(GUI_CPPFLAGS) -c $< -o $@

$(GUI_TARG): $(GUI_OFILES)
	$(GUI_CC) $(GUI_OFILES) -o $(GUI_TARG) $(GUI_LDFLAGS)

gui-clean:
	rm -f $(GUI_OFILES) $(GUI_TARG)

install:
	install -Dm 755 enux-installer /usr/bin/enux-installer
	install -Dm 755 enux-installer-tui /usr/bin/enux-installer-tui
	install -Dm 755 ${GUI_TARG} /usr/bin/enux-installer-gui

