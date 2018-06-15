#ifndef __KEYB_H
#define __KEYB_H

#ifdef  __cplusplus
extern "C" {
#endif
void interrupt_keyboard();
char vga_getkey();
#ifdef  __cplusplus
}
#endif

#endif
