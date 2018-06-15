#ifndef __MOUSE_H
#define __MOUSE_H

#ifdef  __cplusplus
extern "C" {
#endif

void interrupt_mouse();
void mouse_draw_mousepointerwoxy();
int mouse_getx();
int mouse_gety();
void mouse_setposition(int x, int y);
int mouse_update();
int mouse_getbutton();

#ifdef  __cplusplus
}
#endif

#endif
