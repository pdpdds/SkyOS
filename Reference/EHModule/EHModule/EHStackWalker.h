#pragma once
#include "StackWalker.h"

class CEHStackWalker : public StackWalker
{
public:
	CEHStackWalker(void);
	virtual ~CEHStackWalker(void);

	virtual void OnOutput(LPCSTR szText) override;
	void SetLogFilePath(void);

protected:

private:
	TCHAR m_szDumpPath[MAX_PATH];
};
