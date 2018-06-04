#pragma once
#include "SkyLauncher.h"

class SkyConsoleLauncher : public SkyLauncher
{
public:
	SkyConsoleLauncher();
	~SkyConsoleLauncher();

	virtual void Launch() override;
};