#ifndef __FONT_H
#define __FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_WIDTH 9
#define FONT_HEIGHT 15
#define FONTPICTURE_WIDTH 145

void putstring(char *s,
			   int x,
			   int y,
			   unsigned long *offscreen);
void putstringc(char *s,
				int x,
				int y,
				unsigned char r,
				unsigned char g,
				unsigned char b,
				unsigned long *offscreen);
void font_init();

#ifdef __cplusplus
}
#endif

#endif
