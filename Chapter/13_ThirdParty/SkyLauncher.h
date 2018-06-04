#pragma once

class Thread;

class SkyLauncher
{
public:
	SkyLauncher();
	~SkyLauncher();
	
	virtual void Launch() = 0;

protected:
	void Jump(Thread* pThread);

private:
	void JumpToNewKernelEntry(int entryPoint, unsigned int procStack);
};