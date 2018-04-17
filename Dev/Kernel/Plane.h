#pragma once

class Plane
{
public:
	Plane();
	virtual ~Plane();

	void SetX(int x){ m_x = x; }
	void SetY(int y){ m_y = y; }

	int GetX(){ return m_x; }
	int GetY(){ return m_y; }

	virtual bool IsRotate() = 0;

private:
	int m_x;
	int m_y;
};

