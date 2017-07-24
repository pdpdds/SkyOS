#pragma once
#include "kheap.h"
#include "stdint.h"
#include "hal.h"

class Scheduler
{
public:
	static Scheduler* GetInstance()
	{
		if (m_pScheduler == 0)
			m_pScheduler = new Scheduler();

		return m_pScheduler;
	}

	bool  DoSchedule(int tick, registers_t& registers);

private:
	Scheduler();
	~Scheduler();

	static Scheduler* m_pScheduler;
};

