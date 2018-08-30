#include "ZetPlane.h"


ZetPlane::ZetPlane()
{
	m_rotation = 1;
}


ZetPlane::~ZetPlane()
{
}

bool ZetPlane::IsRotate()
{
	return m_rotation != 0;
}
