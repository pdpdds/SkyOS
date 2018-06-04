#pragma once
#include "stdint.h"

template <typename T>
class SkyWindow
{
public:
	SkyWindow(){}
	virtual ~SkyWindow(){}

	bool Initialize(void* _pVideoRamPtr, int _width, int _height, int _bpp, uint8_t buffertype);
	bool Run();
	bool Clear();
	bool Print(char* pMsg);


	T m_guiSys;
};

template <typename T>
bool SkyWindow<T>::Initialize(void* _pVideoRamPtr, int _width, int _height, int _bpp, uint8_t buffertype)
{
	return m_guiSys.Initialize(_pVideoRamPtr, _width, _height, _bpp, buffertype);
}

template <typename T>
bool SkyWindow<T>::Run()
{
	return m_guiSys.Run();
}

template <typename T>
bool SkyWindow<T>::Print(char* pMsg)
{
	return m_guiSys.Print(pMsg);
}

template <typename T>
bool SkyWindow<T>::Clear()
{
	return m_guiSys.Clear();
}


