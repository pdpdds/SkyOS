#pragma once
#include "SkySheet.h"

#define MAX_SHEETS		256
#define SHEET_USE		1

class SkySheetController
{
public:
	SkySheetController();
	~SkySheetController();

	SkySheet *Alloc();
	bool Initialize(unsigned char *vram, int xsize, int ysize);
	int GetTop() { return m_top; }
	void SetTop(int top) { m_top = top; }

	void UpdateSheets(int old, int height, SkySheet* sheet);

	void RefreshMap(int vx0, int vy0, int vx1, int vy1, int h0);
	void RefreshSub(int vx0, int vy0, int vx1, int vy1, int h0, int h1);

private:
	unsigned char* m_vram;
	unsigned char* m_map;
	int m_xsize, m_ysize, m_top;
	
	SkySheet *sheets[MAX_SHEETS];
	SkySheet sheets0[MAX_SHEETS];
};

