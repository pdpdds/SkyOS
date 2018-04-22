#pragma once
class ConsoleManager
{
public:
	ConsoleManager();
	virtual ~ConsoleManager();

	bool RunCommand(char* buf);
};

