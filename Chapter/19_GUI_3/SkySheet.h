#pragma once

class SkySheetController;

class SkySheet
{
	friend class SkySheetController;
public:
	SkySheet();
	~SkySheet();

	void SetBuf(unsigned char *buf, int xsize, int ysize, int col_inv);
	void Updown(int height);
	void Refresh(int bx0, int by0, int bx1, int by1);
	void Slide(int vx0, int vy0);
	void Free();

	SkySheetController* GetOwner() { return m_ctl; }
	void SetOwner(SkySheetController* ctl) { m_ctl = ctl; }
	unsigned char* GetBuf() { return m_buf; }
	int GetXSize() { return m_bxsize; }
	int GetYSize() { return m_bysize; }

protected:

private:
	unsigned char *m_buf;
	int m_bxsize, m_bysize; 
	int m_vx0, m_vy0;
	int m_col_inv, m_height, m_flags;

	SkySheetController* m_ctl;
};

