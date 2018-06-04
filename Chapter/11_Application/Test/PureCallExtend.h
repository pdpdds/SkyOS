#pragma once

struct PureCallBase
{
	PureCallBase() { mf(); }
	virtual ~PureCallBase() {}
	void mf()
	{
		pvf();
	}
	virtual void pvf() = 0;
};

struct PureCallExtend : public PureCallBase
{
	PureCallExtend() {}
	virtual ~PureCallExtend(){}
	virtual void pvf() {}
};

