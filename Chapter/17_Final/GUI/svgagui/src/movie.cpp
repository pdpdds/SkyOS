#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "local.h"
#include <defs.h>
#include <inttypes.h>
#include <mpeg2.h>
#include <mpeg2convert.h>
#include <isofs.h>
#include <multiboot.h>
#include <vesa.h>
#include <task.h>

GuiObject *movieobj;

extern GuiWinThread *win_thread;

void displayImage(GuiWindow *winning, int x1, int y1, int width, int height, unsigned char *buf) {
		unsigned char r,g,b;
		unsigned long i,j,x,y;
		unsigned long *lfb = getlfb();
		GuiWindow *win;
		
		i=0;
		for (y=0; y<height; y++) {
			i=width*y*3;
			for (x=0; x<width; x++) {
				r=buf[i];
				g=buf[i+1];
				b=buf[i+2];
				i+=3;	

				if (winning != win_thread->focuswin) {
					win = win_thread->first;
					for (j=0;j<win_thread->number;j++) {
						if (winning != win)
							if ((x1+x >= win->x) && (x+x1 <= win->x+win->width) && (y1+y >= win->y) && (y+y1 <= win->y+win->height)) break;
						win=win->next;
					}
				}
				
				lfb[SCREENWIDTH*(y+y1)+(x+x1)] = (r<<16) | (g<<8) | b;
			}
		}
}

void run_movie() {

/* New vfs - Movie Playing removed.
   Rewrite if needed */
	
#if 0
#define BUFFER_SIZE 4096
    mpeg2dec_t * decoder;
    const mpeg2_info_t * info;
    const mpeg2_sequence_t * sequence;
    mpeg2_state_t state;
    size_t size;
	unsigned char *video;
    unsigned char *p;
	unsigned long videofsize=cdiso_getfilesize("VIDEO.MPG");
  	struct __cdiso_directory *file = kalloc(sizeof(struct __cdiso_directory));
	unsigned int sector=0;
  	
	cdiso_getfile("VIDEO.MPG", file);
	
	video =(unsigned char*) kalloc(BUFFER_SIZE);

for(;;) {
	sector=0;
    decoder = mpeg2_init ();
    if (decoder == NULL) {
	return;
    }
    info = mpeg2_info (decoder);

    size = (size_t)-1;
   	size = BUFFER_SIZE;
   	   	
    do {
	state = mpeg2_parse (decoder);
	sequence = info->sequence;
	switch (state) {
	case STATE_BUFFER:
	    p=video;
	    cdiso_readsector(file, video, sector);
	    sector++;
	    video+=2048;
	    cdiso_readsector(file, video, sector);
	    sector++;
	    video=p;	    
	    mpeg2_buffer(decoder, video, video + size);
	   	if (sector*2048>videofsize) size=0;
	case STATE_SEQUENCE:
	    mpeg2_convert (decoder, mpeg2convert_rgb24, NULL);
	    break;	    
	case STATE_SLICE:
	case STATE_END:
	case STATE_INVALID_END:
	    if (info->display_fbuf)
		displayImage(movieobj->win, movieobj->win->x+movieobj->x+5, movieobj->win->y+movieobj->y+20, info->sequence->width,
					 info->sequence->height, info->display_fbuf->buf[0]);
	    break;
	default:
	    break;
	}
    } while (size);

    mpeg2_close (decoder);
}
#endif
}

GuiObject *add_movie(GuiWindow * win, int x, int y, char *filename)
{
	GuiObject *obj;
	int width, height;	
	check_window(win, "add_movie");

	obj = (GuiObject *) malloc(sizeof(GuiObject));
	if (obj == NULL)
		error("Cannot allocate memory for movie.");

	width=0;
	height=0;
	
	obj->win = win;
	obj->x = obj->x_min = x;
	obj->y = obj->y_min = y;
	obj->width = width;
	obj->height = height;
	obj->pressed = FALSE;
	obj->active = TRUE;
	obj->hide = FALSE;
	obj->wait_for_mouse = FALSE;
	obj->objclass = MOVIE;
	obj->u_data = 0;
	obj->align = ALIGN_CENTER;
	obj->x_max = x + width - 1;
	obj->y_max = y + height - 1;
	obj->bg_col1 = WIN_BACK;
	obj->info[0] = '\0';	/* make string length zero */
	set_object_callback(obj, object_cb);

	obj->data[0] = (char *) malloc(obj->width * obj->height);
	if (obj->data[0] == NULL)
		error("Unable to allocate data array in add_image().");
	obj->data[1] = NULL;

	add_object(obj);  	
    movieobj = obj;
    ProcessCreate(run_movie,"MOVIEPLAYER");
    
	return obj;
}

void update_movie(GuiObject * obj)
{
	check_object(obj, "obj", "update_movie");
	win_object(obj);
}

void show_movie(GuiObject * obj)
{
	check_object(obj, "obj", "show_movie");
	check_window(obj->win, "show_movie");

	update_movie(obj);
	show_window(obj->win);
}
