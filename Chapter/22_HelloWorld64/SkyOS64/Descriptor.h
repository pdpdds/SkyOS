#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

void LoadGDTR(QWORD _GdtrAddress);
void LoadTR(WORD _TssSegmentOffset);
void LoadIDTR(QWORD _IDTRAddress);

#endif /*__DESCRIPTOR_H__*/