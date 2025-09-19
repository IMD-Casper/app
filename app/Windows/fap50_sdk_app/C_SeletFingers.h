#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "InterfaceImdFap50Method.h"
#include <vector>
using namespace std;

// C_SeletFingers dialog

class C_SeletFingers : public CDialogEx
{
	DECLARE_DYNAMIC(C_SeletFingers)

public:
	C_SeletFingers(CWnd* pParent = nullptr);   // standard constructor
	virtual ~C_SeletFingers();
	vector<E_FINGER_POSITION> select_finger;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SEL_FINGERS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int show_en;
	void show_window(BOOL enable);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedChk();
	afx_msg void OnBnClickedChkrightfull();
	afx_msg void OnBnClickedChkleftfull();
};
