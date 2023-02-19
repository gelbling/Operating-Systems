# Class      : Operating Systems
# Assignment : Project 1
# Authors    : Gabriel Amodeo Elbling & Felixander Kery
# File       : Makefile -> creates executables for both treecopy.c and filecopy.c

CC = gcc
CFLAGS = -std=c99 -Wall

all: treecopy filecopy

treecopy: treecopy.c
	$(CC) $(CFLAGS) -o $@ $<

filecopy: filecopy.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f treecopy filecopy
