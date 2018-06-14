#pragma once

class I_MapFileReader
{
public:
	virtual int readFile(char* fileName) = 0;
	virtual void setLoadAddress(DWORD	loadAddress) = 0;
	virtual unsigned int getPreferredLoadAddress() = 0;
	virtual int getAddressInfo(DWORD	address, char* module, char* fileName, int		&lineNumber, char* function, DWORD		&resultAddress) = 0;
	virtual bool SimpleUndecorateCPP(char* in, char* out, int outLen) = 0;
	virtual bool UndecorateCPP(char* in, char* out, int outLen) = 0;

	
};