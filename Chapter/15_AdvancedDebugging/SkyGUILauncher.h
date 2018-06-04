#pragma once
#include "SkyLauncher.h"

class SkyGUILauncher : public SkyLauncher
{
public:
	SkyGUILauncher();
	~SkyGUILauncher();

	virtual void Launch() override;
};