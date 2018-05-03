#pragma once

class CBaseClass
{
public:
	CBaseClass(void);
	virtual ~CBaseClass(void);	

	virtual int ProcessTask() {return 0;};

protected:

private:
	int m_iBaseVar;	
};
