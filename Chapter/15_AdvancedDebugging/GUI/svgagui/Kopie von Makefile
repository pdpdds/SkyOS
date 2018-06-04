.EXPORT_ALL_VARIABLES:

# Source directories:
LIBDIR = -L.. -L/usr/X11/lib
INCDIR = -I..
LIBS = -lsvgagui -lX11 -lunix

# Compilation:
CFLAGS = -D_USE_VGALIB -DUSE_VGALIB -Wall -O3 -fomit-frame-pointer $(INCDIR) -nostdlib -nodefaultlibs -nostdinc -fno-builtin -static -ffreestanding -fno-exceptions -Wall -I../include -Ic:/LikeNew/Like/src/src/include/ -Ic:/LikeNew/Like/src/src/include/c++/ -Ic:/LikeNew/Like/src/src/lib/unix/include/unix/ -Wl,-Lc:/LikeNew/Like/src/src/lib/  -Ic:/LikeNew/Like/src/src/lib/unix/include/unix/ -Ic:/LikeNew/Like/src/src/apps -Ic:/LikeNew/Like/src/src/. -Ic:/LikeNew/Like/src/src/ports/mpeg2dec/include
CC = gcc

PROJECT = libsvgagui.a
VERSION = 1.2
#DIRS = src example
DIRS = src

all:
	for d in $(DIRS); do $(MAKE) -C $$d || exit 1; done;

clean:
	for d in $(DIRS); do $(MAKE) -C $$d clean || exit 1; done;
	rm -f libsvgagui.a *~ *%

tar:
	rm -f *~ *%
	cd src ; rm -f *~ *%
	cd example ; rm -f *~ *% core
	cd .. ; tar cfz svgagui-$(VERSION).tgz svgagui-$(VERSION)/*
