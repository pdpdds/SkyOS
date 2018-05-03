#pragma once

class CBaseClass;

#define MAX_ARRAY_SIZE 1000000

class CExtensionClass : public CBaseClass
{
public:
	CExtensionClass(void);
	virtual ~CExtensionClass(void);

	int GetExtensionVar(){return m_iExtensionVar;}
	void SetExtensionVar(int iExtensionVar){m_iExtensionVar = iExtensionVar;}
	void InitArray();

	virtual int ProcessTask() override;
	int ProcessStackOverFlow();
	int ProcessHeapCorruption();
	void ProcessOutofMemory();


protected:

private:
	int m_iExtensionVar;
	int m_aArray[MAX_ARRAY_SIZE];
};
