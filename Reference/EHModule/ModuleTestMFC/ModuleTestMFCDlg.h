// ModuleTestMFCDlg.h : header file
//

#pragma once
#include "EHModuleManager.h"

struct PureCallBase 
{ 
	PureCallBase() { mf(); } 
	void mf() 
	{ 
		pvf(); 
	} 
	virtual void pvf() = 0; 
};

struct PureCallExtend : public PureCallBase
{ 
	PureCallExtend() {}
	virtual void pvf() {}
}; 

// CModuleTestMFCDlg dialog
class CModuleTestMFCDlg : public CDialog
{
// Construction
public:
	CModuleTestMFCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MODULETESTMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedGeneralerror();

	afx_msg void OnBnClickedStackoverflow();
	afx_msg void OnBnClickedHeapdamage();
	afx_msg void OnBnClickedCrterror();
	afx_msg void OnBnClickedPurefunctioncall();

private:
	CEHModuleManager ModuleManger;
public:
	afx_msg void OnBnClickedHeapdamage2();
	afx_msg void OnBnClickedOutofmemory();
};
