#pragma once

class SkyDebugger
{
public:
	~SkyDebugger();

	static SkyDebugger* GetInstance()
	{
		if (m_pDebugger == 0)
			m_pDebugger = new SkyDebugger();

		return m_pDebugger;
	}

	void TraceStack(unsigned int maxFrames = 20);

private:
	SkyDebugger();
	static SkyDebugger* m_pDebugger;
};