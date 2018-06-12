#pragma once
#include "windef.h"
#include "SkySheet.h"

#define MAX_SHEETS		256
#define SHEET_USE		1

class SkySheetController
{
public:
	SkySheetController();
	virtual ~SkySheetController();

	virtual bool Initialize(unsigned char *vram, int xsize, int ysize) = 0;
	virtual void RefreshSub(int vx0, int vy0, int vx1, int vy1, int h0, int h1) = 0;
	virtual unsigned char* AllocBuffer(int width, int height) = 0;

	SkySheet *Alloc();
	int GetTop() { return m_sheetTop; }
	void SetTop(int top) { m_sheetTop = top; }

	void UpdateSheets(int old, int height, SkySheet* sheet);

	void RefreshMap(int vx0, int vy0, int vx1, int vy1, int h0);	

	SkySheet* FindSheet(int x, int y);
	SkySheet* FindSheetById(int processId);
	

protected:
	unsigned char* m_map;
	int m_xsize, m_ysize, m_sheetTop;
	
	SkySheet *m_pSheets[MAX_SHEETS];
	SkySheet sheets0[MAX_SHEETS];
};

