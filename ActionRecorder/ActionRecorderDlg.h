
// ActionRecorderDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CActionRecorderDlg dialog
class CActionRecorderDlg : public CDialogEx
{
// Construction
public:
	CActionRecorderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ACTIONRECORDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
//	afx_msg void OnBnClickedbtn();
	afx_msg void OnBnClickedloadbtn();
	afx_msg void OnBnClickedsavebtn();
	afx_msg void OnBnClickedplaybtn();
	afx_msg void OnBnClickedrecordbtn();
	void enable_act_btn(void);
	void disable_act_btn(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedloadbtn2();
	afx_msg void OnLbnSelchangejobsq();
	CListBox jobLst;

	
	/*
	add add
	*/
	char s_input_buffer[1024];
	int now_jobs_index;
	int now_jobs_click;
	int now_sub_job_index;
	int now_sub_job_click;
	double play_speed;
	afx_msg void OnBnClickedRmbtn();
	CListBox sub_jobList;
	afx_msg void OnBnClickedRmbtn2();
	afx_msg void OnSubSelChange();
	void save_one_RCrhing(void);
	void enable_subLoad(void);
	void disable_subLoad(void);
	EventTaskTree eventStorage;
	void enable_subEdit(void);
	void disable_subEdit(void);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit1();
	void rm1_enable(void);
	void rm2_enable(void);
	void rm1_disable(void);
	void rm2_disable(void);
	afx_msg void OnEnChangeEdit2();
	void set_speeder(void);
	afx_msg void OnEnChangeEdit3();
	void enable_RandNum(void);
	void disable_RandNum(void);
	afx_msg void OnBnClickedsavebtn2();
	void CleanJobQueue(void);
	afx_msg void OnBnClickedCleanbtn();
	void init_var(void);
	afx_msg void OnBnClickedRetype();
};
