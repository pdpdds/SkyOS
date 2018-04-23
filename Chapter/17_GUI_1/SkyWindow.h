#pragma once

class SkyWindow
{
public:
	SkyWindow();
	virtual ~SkyWindow();

	virtual bool Initialize(void* _pVideoRamPtr, int _width, int _height, int _bpp) = 0;
	virtual bool Run() = 0;
};

