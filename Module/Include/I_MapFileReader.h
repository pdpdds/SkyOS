#pragma once

class I_SampleDLLInterface
{
public:
	virtual int AddNumbers(int a, int b) = 0;
	virtual void MyMessageBox(const char* message) {}
};

class I_MapFileReader
{
public:
	virtual int readFile(char* fileName) = 0;
	virtual void setLoadAddress(DWORD	loadAddress) = 0;
	virtual int getAddressInfo(DWORD	address, char* module, char* fileName, int		&lineNumber, char* function, DWORD		&resultAddress) = 0;
};