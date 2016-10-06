#################################################################################
#
# Makefile:
#	dashboard-motion-detect
#       July 2016, Torbjørn Kristoffersen <tk@mezzanineware.com>
#
#################################################################################

DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LDLIBS    = -lwiringPi -lsqlite3

SRC	=       dashboard-motion-detect.c	

OBJ	=	$(SRC:.c=.o)

BINS	=	$(SRC:.c=)

dashboard-motion-detect:	dashboard-motion-detect.o
	$(CC) -o $@ dashboard-motion-detect.o $(LDFLAGS) $(LDLIBS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(BINS)
