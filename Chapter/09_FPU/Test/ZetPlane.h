#pragma once
#include "Plane.h"

class ZetPlane : public Plane
{
public:
	ZetPlane();
	virtual ~ZetPlane();

	virtual bool IsRotate() override;

	int m_rotation;
};

