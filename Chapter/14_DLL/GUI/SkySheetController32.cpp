#include "SkySheetController32.h"



SkySheetController32::SkySheetController32()
{
}


SkySheetController32::~SkySheetController32()
{
}

bool SkySheetController32::Initialize(unsigned char *vram, int xsize, int ysize)
{
	m_map = (unsigned char *) new unsigned char[xsize * ysize];

	if (m_map == nullptr)
	{
		return false;
	}
	m_vram = (ULONG*)vram;
	m_xsize = xsize;
	m_ysize = ysize;
	m_top = -1; /* 시트는 한 장도 없다 */

	for (int i = 0; i < MAX_SHEETS; i++)
	{
		sheets0[i].m_flags = 0; /* 미사용 마크 */
	}

	return true;
}

void SkySheetController32::RefreshSub(int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	ULONG* buf;
	unsigned char sid;
	SkySheet *sht;

	/* refresh 범위가 화면외에 있으면 보정 */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > m_xsize)
	{
		vx1 = m_xsize;
	}

	if (vy1 > m_ysize)
	{
		vy1 = m_ysize;
	}

	for (h = h0; h <= h1; h++)
	{
		sht = m_pSheets[h];
		buf = (ULONG*)sht->m_buf;
		sid = sht - sheets0;

		/* vx0~vy1를 사용해, bx0~by1를 역산한다 */
		bx0 = vx0 - sht->m_vx0;
		by0 = vy0 - sht->m_vy0;
		bx1 = vx1 - sht->m_vx0;
		by1 = vy1 - sht->m_vy0;

		if (bx0 < 0)
		{
			bx0 = 0;
		}

		if (by0 < 0)
		{
			by0 = 0;
		}

		if (bx1 > sht->m_bxsize)
		{
			bx1 = sht->m_bxsize;
		}

		if (by1 > sht->m_bysize)
		{
			by1 = sht->m_bysize;
		}

		for (by = by0; by < by1; by++)
		{
			vy = sht->m_vy0 + by;
			for (bx = bx0; bx < bx1; bx++)
			{
				vx = sht->m_vx0 + bx;
				if (m_map[vy * m_xsize + vx] == sid)
				{
					m_vram[vy * m_xsize + vx] = buf[by * sht->m_bxsize + bx];
				}
			}
		}
	}
	return;
}

unsigned char* SkySheetController32::AllocBuffer(int width, int height)
{
	if (width == 0 || height == 0)
		return nullptr;

	return new unsigned char[width * height * 4];
}
