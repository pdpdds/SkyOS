#include "SkySheetController.h"
#include "memory.h"

SkySheetController::SkySheetController()
{
	memset(m_pSheets, 0, sizeof(SkySheet*) * MAX_SHEETS);
}


SkySheetController::~SkySheetController()
{
}

SkySheet *SkySheetController::Alloc()
{
	SkySheet* sheet;
	int i;
	for (i = 0; i < MAX_SHEETS; i++)
	{
		if (sheets0[i].m_flags == 0)
		{
			sheet = &sheets0[i];
			sheet->m_flags = SHEET_USE; /* 사용중 마크 */
			sheet->m_height = -1; /* 비표시중 */
			sheet->SetOwner(this);

			return sheet;
		}
	}
	return nullptr;	//사용할 수 있는 쉬트가 없음
}

SkySheet* SkySheetController::FindSheet(int x, int y)
{
	for (int h = m_sheetTop; h >= 0; h--)
	{
		if (m_pSheets[h] == nullptr)
			continue;

		if (m_pSheets[h]->m_movable == false)
			continue;

		if (m_pSheets[h]->InRange(x, y))
			return m_pSheets[h];
	}

	return nullptr;
}

SkySheet* SkySheetController::FindSheetById(int processId)
{
	for (int h = m_sheetTop; h >= 0; h--)
	{
		if (m_pSheets[h] == nullptr)
			continue;

		if (m_pSheets[h]->m_ownerProcess == processId)
			return m_pSheets[h];
	}

	return nullptr;
}

void SkySheetController::UpdateSheets(int old, int height, SkySheet* sheet)
{
	if (sheet == nullptr || height >= MAX_SHEETS)
		return;

	/* 이하는 주로 sheets[]를 늘어놓고 대체 */
	if (old > height)  /* 이전보다 낮아진다 */
	{
		if (height >= 0) {
			/* 사이의 것을 끌어올린다 */
			for (int h = old; h > height; h--)
			{
				m_pSheets[h] = m_pSheets[h - 1];
				m_pSheets[h]->m_height = h;
			}

			m_pSheets[height] = sheet;
			RefreshMap(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height + 1);
			RefreshSub(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height + 1, old);
		}
		else
		{	/* 비표시화 */
			if (m_sheetTop > old) {
				/* 위로 되어 있는 것을 내린다 */
				for (int h = old; h < m_sheetTop; h++) {
					m_pSheets[h] = m_pSheets[h + 1];
					m_pSheets[h]->m_height = h;
				}
			}
			m_sheetTop--;

			RefreshMap(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, 0);
			RefreshSub(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, 0, old - 1);
		}
	}
	else if (old < height) /* 이전보다 높아진다 */
	{
		
		if (old >= 0) {
			/* 사이의 것을 눌러 내린다 */
			for (int h = old; h < height; h++) {

				if (m_pSheets[h + 1])
				{
					m_pSheets[h] = m_pSheets[h + 1];
					m_pSheets[h]->m_height = h;
				}
				
			}
			m_pSheets[height] = sheet;
		}
		else {	/* 비표시 상태에서 표시 상태로 */
				/* 위로 되어 있는 것을 들어 올린다 */
			for (int h = m_sheetTop; h >= height; h--)
			{
				if (m_pSheets[h])
				{
					m_pSheets[h + 1] = m_pSheets[h];
					m_pSheets[h + 1]->m_height = h + 1;
				}
				
			}

			m_pSheets[height] = sheet;

			m_sheetTop++;/* 표시중의 레이어가 1개 증가하므로, 맨 위의 높이가 증가한다 */
		}
		RefreshMap(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height);
		RefreshSub(sheet->m_vx0, sheet->m_vy0, sheet->m_vx0 + sheet->m_bxsize, sheet->m_vy0 + sheet->m_bysize, height, height);
	}
}


void SkySheetController::RefreshMap(int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, sid;
	SkySheet *sht;

	if (vx0 < 0)
	{
		vx0 = 0;
	}

	if (vy0 < 0)
	{
		vy0 = 0;
	}

	if (vx1 > m_xsize - 1)
	{
		vx1 = m_xsize - 1;
	}

	if (vy1 > m_ysize)
	{
		vy1 = m_ysize;
	}

	for (h = h0; h <= m_sheetTop; h++)
	{
		sht = m_pSheets[h];

		if (sht == nullptr)
			continue;

		sid = sht - sheets0; /* 번지를 빼서 그것을 레이어 번호로 이용 */
		buf = sht->m_buf;

		if (buf == nullptr)
			continue;


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
				if (buf[by * sht->m_bxsize + bx] != sht->m_col_inv)
				{
					//if (vy * m_xsize + vx < 200000)
						m_map[vy * m_xsize + vx] = sid;
				}
			}
		}
	}
	return;
}