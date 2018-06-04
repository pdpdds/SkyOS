OBJS =	main.o color.o

all:: svgagui

svgagui: $(OBJS)
	$(CC) -o svgagui $(OBJS) $(LIBDIR) $(LIBS)

install: svgagui
	cp svgagui /usr/local/bin/svgagui

$(OBJS) : ../svgagui.h main.h

clean:
	rm -f $(PROJECT) *.o *~ *% core

indent:
	indent -kr -i8 *.c
