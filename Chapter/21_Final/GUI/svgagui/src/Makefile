OBJS =	window_thread.o window.o window_draw.o screen.o mouse.o \
	object.o button.o slider.o text.o number.o input.o info.o \
	progress.o dialog.o image.o movie.o browser.o \
	pulldown.o choice.o listbox.o iconlist.o filedialog.o \
	general.o svgalib.o check.o sscanf.o
#	svgalib.o

all:: $(PROJECT) ../svgagui.h local.h

libsvgagui.a: $(OBJS)
	rm -f libsvgagui.a
	ar cq libsvgagui.a $(OBJS)
	ranlib libsvgagui.a

$(OBJS) : ../svgagui.h local.h
text.o : font.h
number.o : bigfont.h

clean:
	rm -f $(PROJECT) *.o *~ *%

indent:
	indent -kr -i8 *.c
