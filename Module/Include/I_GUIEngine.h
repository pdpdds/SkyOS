#pragma once

class I_GUIEngine
{
public:
	virtual bool Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void SetLinearBuffer(unsigned long* pBuffer, unsigned long width, unsigned long height, unsigned long depth, unsigned type) = 0;
	virtual void ProcessKeyboard(int value) = 0;
	virtual void ProcessMouse(int value) = 0;

};