
// ActionRecorderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActionRecorder.h"
#include "ActionRecorderDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CActionRecorderDlg dialog



CActionRecorderDlg::CActionRecorderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CActionRecorderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CActionRecorderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, jobsQ, jobLst);
	DDX_Control(pDX, sub_jobsQ, sub_jobList);
}

BEGIN_MESSAGE_MAP(CActionRecorderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CActionRecorderDlg::OnBnClickedCancel)
//	ON_BN_CLICKED(load_btn, &CActionRecorderDlg::OnBnClickedbtn)
ON_BN_CLICKED(loadbtn, &CActionRecorderDlg::OnBnClickedloadbtn)
ON_BN_CLICKED(savebtn, &CActionRecorderDlg::OnBnClickedsavebtn)
ON_BN_CLICKED(playbtn, &CActionRecorderDlg::OnBnClickedplaybtn)
ON_BN_CLICKED(recordbtn, &CActionRecorderDlg::OnBnClickedrecordbtn)
ON_WM_TIMER()
ON_BN_CLICKED(loadbtn2, &CActionRecorderDlg::OnBnClickedloadbtn2)
ON_LBN_SELCHANGE(jobsQ, &CActionRecorderDlg::OnLbnSelchangejobsq)
ON_BN_CLICKED(RMbtn, &CActionRecorderDlg::OnBnClickedRmbtn)
ON_BN_CLICKED(RMbtn2, &CActionRecorderDlg::OnBnClickedRmbtn2)
ON_LBN_SELCHANGE(sub_jobsQ, &CActionRecorderDlg::OnSubSelChange)
ON_BN_CLICKED(IDC_BUTTON1, &CActionRecorderDlg::OnBnClickedButton1)
ON_EN_CHANGE(IDC_EDIT1, &CActionRecorderDlg::OnEnChangeEdit1)
ON_EN_CHANGE(IDC_EDIT2, &CActionRecorderDlg::OnEnChangeEdit2)
ON_EN_CHANGE(IDC_EDIT3, &CActionRecorderDlg::OnEnChangeEdit3)
ON_BN_CLICKED(savebtn2, &CActionRecorderDlg::OnBnClickedsavebtn2)
ON_BN_CLICKED(CLEANBTN, &CActionRecorderDlg::OnBnClickedCleanbtn)
ON_BN_CLICKED(IDC_Retype, &CActionRecorderDlg::OnBnClickedRetype)
END_MESSAGE_MAP()


// CActionRecorderDlg message handlers

BOOL CActionRecorderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	srand(time(NULL));
	hookCtrlCmd();
	SetTimer(1, 25, NULL);
	
	init_var();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CActionRecorderDlg::init_var(void)
{
	disable_act_btn();
	disable_subLoad();
	disable_RandNum();
	disable_subEdit();
	rm1_disable();
	rm2_disable();
	now_jobs_click = now_sub_job_click = now_jobs_index = now_sub_job_index = 0;
	play_speed = 1;
	set_speeder();

	
	// //for debug to print the message on the console
	//AllocConsole();
	//freopen( "CONOUT$","w",stdout);
	//printf("XXX\n");
	//FreeConsole();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CActionRecorderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CActionRecorderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CActionRecorderDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}



void CActionRecorderDlg::OnBnClickedloadbtn()
{
	CString now_strr , strr;
	EventTaskTree* toapd;
	int t_id = now_jobs_index+now_sub_job_index;
	if(now_jobs_click)
	{
		jobLst.GetText(now_jobs_click-1, now_strr);
		toapd = eventStorage.find( string((CW2A)now_strr.GetBuffer(0)) );
	}
	else toapd = &eventStorage;


	if(MessageBox(TEXT("YES to load a job\r\nNO to add random queue"),TEXT("Type"),MB_YESNO) == 6)
	{
		CString szFilter = L"Simple Record File|*.srcf|all file|*.*||";
		CFileDialog fd(true, L"srcf", L"*.srcf", OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter, this);
		
		TCHAR str_Files[MAX_PATH * 260] = L"";
		fd.m_ofn.lpstrFile = str_Files;
		
		if(fd.DoModal() == IDOK)
		{
			POSITION fileNamePos = fd.GetStartPosition();
			while(fileNamePos)
			{
				int t_id = now_jobs_index+now_sub_job_index;
				CString dlgfName = fd.GetNextPathName(fileNamePos);
				CString thisFileName = dlgfName.Mid(dlgfName.ReverseFind('\\')+1);
				strr.Format(_T("%d-%s"),t_id,thisFileName);
				string apdname = (CW2A)strr.GetBuffer(0);
				string LdFileName = (CW2A)dlgfName.GetBuffer(0);
			
				if(ReadDataQ(LdFileName))
				{
					toapd->add(t_id,act_OneJob,-1,1,1,0,apdname,LdFileName,toapd->p_nxt);
					enable_act_btn();
					jobLst.InsertString(now_jobs_click++, strr );
					now_jobs_index++;

					jobLst.GetText(now_jobs_click-1, now_strr);
					toapd = eventStorage.find( string((CW2A)now_strr.GetBuffer(0)) );
				}
				else MessageBox(TEXT("Error file format."), TEXT("Load Error"), MB_OK);
			}
		}
	}
	else
	{
		strr.Format(_T("%d-%s"),t_id,L"Random Queue");
		string apdname = (CW2A)strr.GetBuffer(0);
		toapd->add(t_id,act_RandJob,-1,1,1,0,apdname,"-",toapd->p_nxt);
		jobLst.InsertString(now_jobs_click++, strr);
		now_jobs_index++;
	}
}

void CActionRecorderDlg::OnBnClickedloadbtn2()
{
	int parend_ID , spnpos = 0;
	CString strr;
	jobLst.GetText(now_jobs_click, strr);
	string get_id_s = (CW2A)(strr.Tokenize(_T("-"),spnpos));
	parend_ID = atoi(get_id_s.c_str());

	CString szFilter = L"Simple Record File|*.srcf|all file|*.*||";
	CFileDialog fd(true, L"srcf", L"*.srcf", OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter, this);
	TCHAR str_Files[MAX_PATH * 260] = L"";
	fd.m_ofn.lpstrFile = str_Files;
	if(fd.DoModal() == IDOK)
	{
		POSITION fileNamePos = fd.GetStartPosition();
		while(fileNamePos)
		{
			int t_id = now_jobs_index+now_sub_job_index;
			CString dlgfName = fd.GetNextPathName(fileNamePos);
			CString thisFileName = dlgfName.Mid(dlgfName.ReverseFind('\\')+1);
			strr.Format(_T("%d-%s"),t_id,thisFileName);
			string apdname = (CW2A)strr.GetBuffer(0);
			std::string LdFileName = (CW2A)dlgfName.GetBuffer(0);
		
			if(ReadDataQ(LdFileName))
			{
				eventStorage.add(t_id,act_OneJob,parend_ID,1,1,0,apdname,LdFileName,eventStorage.p_nxt);
				enable_act_btn();
				sub_jobList.InsertString(now_sub_job_click++, strr );
				now_sub_job_index++;
			}
			else MessageBox(TEXT("Error file format."), TEXT("Load Error"), MB_OK);
		}
	}
}


void CActionRecorderDlg::OnBnClickedplaybtn()
{
	toggle_playing();
}


void CActionRecorderDlg::OnBnClickedrecordbtn()
{
	if(toggle_recording())save_one_RCrhing();
	enable_act_btn();
}


void CActionRecorderDlg::enable_act_btn(void)
{
	GetDlgItem(playbtn)->EnableWindow(true);
	GetDlgItem(savebtn)->EnableWindow(true);
	GetDlgItem(savebtn2)->EnableWindow(true);
}


void CActionRecorderDlg::disable_act_btn(void)
{
	GetDlgItem(playbtn)->EnableWindow(false);
	GetDlgItem(savebtn)->EnableWindow(false);
	GetDlgItem(savebtn2)->EnableWindow(false);
}

void CActionRecorderDlg::enable_subLoad(void)
{
	GetDlgItem(loadbtn2)->EnableWindow(true);
}


void CActionRecorderDlg::disable_subLoad(void)
{
	GetDlgItem(loadbtn2)->EnableWindow(false);
}

void CActionRecorderDlg::enable_subEdit(void)
{
	GetDlgItem(IDC_EDIT1)->EnableWindow(true);
}

void CActionRecorderDlg::disable_subEdit(void)
{
	GetDlgItem(IDC_EDIT1)->EnableWindow(false);
}

void CActionRecorderDlg::rm1_enable(void)
{
	GetDlgItem(RMbtn)->EnableWindow(true);
}

void CActionRecorderDlg::rm2_enable(void)
{
	GetDlgItem(RMbtn2)->EnableWindow(true);
}

void CActionRecorderDlg::rm1_disable(void)
{
	GetDlgItem(RMbtn)->EnableWindow(false);
}

void CActionRecorderDlg::rm2_disable(void)
{
	GetDlgItem(RMbtn2)->EnableWindow(false);
}

void CActionRecorderDlg::enable_RandNum(void)
{
	GetDlgItem(IDC_EDIT3)->EnableWindow(true);
	GetDlgItem(IDC_Retype)->EnableWindow(true);
}

void CActionRecorderDlg::disable_RandNum(void)
{
	GetDlgItem(IDC_EDIT3)->EnableWindow(false);
	GetDlgItem(IDC_Retype)->EnableWindow(false);
}




void CActionRecorderDlg::OnTimer(UINT_PTR nIDEvent)
{
	contact_eventQ(eventStorage,play_speed);
	peek_an_msg();
	if(ExcuCMD())save_one_RCrhing();
	if(isbtn_status())enable_act_btn();
	CDialogEx::OnTimer(nIDEvent);
}

void CActionRecorderDlg::OnBnClickedRetype()
{
	CString strr;
	jobLst.GetText(now_jobs_click, strr);
	int spnpos = 0;
	int get_id = atoi((string((CW2A)(strr.Tokenize(_T("-"),spnpos)))).c_str());

	if(BST_CHECKED == ((CButton*)GetDlgItem(IDC_Retype))->GetCheck())
		eventStorage.find( get_id )->replaytype = 1;
	else
		eventStorage.find( get_id )->replaytype = 0;
}

void CActionRecorderDlg::OnLbnSelchangejobsq()
{
	while(sub_jobList.GetCount())sub_jobList.DeleteString(0);
	EventTaskTree *ev_ptr;
	now_jobs_click = jobLst.GetCurSel();
	CString strr;
	jobLst.GetText(now_jobs_click, strr);
	int spnpos = 0;
	int get_id = atoi((string((CW2A)(strr.Tokenize(_T("-"),spnpos)))).c_str());

	ev_ptr = eventStorage.find( get_id );
	if(ev_ptr->type == act_RandJob)
	{//string((CW2A)strr.GetBuffer(0))
		enable_subLoad();
		// Load the children
		now_sub_job_click = 0;
		std::vector<int> ch_s = eventStorage.find_children(get_id);

		EventTaskTree* ett_ptr;
		for(int i=0;i<ch_s.size();i++)
		{
			ett_ptr = eventStorage.find(ch_s[i]);
			CString tmps(ett_ptr->idName.c_str());
			sub_jobList.InsertString(0, tmps);
		}
		CString rnumC;
		rnumC.Format(_T("%d"), ev_ptr->rand_size);
		GetDlgItem(IDC_EDIT3)->SetWindowTextW(rnumC);
		if(ev_ptr->replaytype)((CButton*)GetDlgItem(IDC_Retype))->SetCheck(true);
		else ((CButton*)GetDlgItem(IDC_Retype))->SetCheck(false);
		enable_RandNum();
	}
	else
	{
		disable_subLoad();
		disable_RandNum();
	}
	rm1_enable();
	rm2_disable();
	disable_subEdit();
}


void CActionRecorderDlg::OnSubSelChange()
{
	rm1_disable();
	rm2_enable();

	now_sub_job_click = sub_jobList.GetCurSel();
	CString strr , weigg;
	sub_jobList.GetText(now_sub_job_click, strr);
	weigg.Format(_T("%d"),eventStorage.find( string((CW2A)strr.GetBuffer(0)) )->weight);
	
	enable_subEdit();
	GetDlgItem(IDC_EDIT1)->SetWindowTextW(weigg);
}



void CActionRecorderDlg::OnBnClickedRmbtn()
{
	CString strr;
	jobLst.GetText(jobLst.GetCurSel(), strr);
	int spnpos = 0;
	int get_id = atoi(((string)(CW2A)(strr.Tokenize(_T("-"),spnpos))).c_str());
	eventStorage.Delete(get_id);
	jobLst.DeleteString(jobLst.GetCurSel());
	while(sub_jobList.GetCount())sub_jobList.DeleteString(0);
}


void CActionRecorderDlg::OnBnClickedRmbtn2()
{
	CString strr;
	sub_jobList.GetText(sub_jobList.GetCurSel(), strr);
	int spnpos = 0;
	int get_id = atoi(((string)(CW2A)(strr.Tokenize(_T("-"),spnpos))).c_str());
	eventStorage.Delete(get_id);
	sub_jobList.DeleteString(sub_jobList.GetCurSel());
}



void CActionRecorderDlg::save_one_RCrhing(void)
{
	CString szFilter = L"Simple Record File|*.srcf||";
	CFileDialog fd(false, L".srcf", L"*.srcf", OFN_HIDEREADONLY, szFilter, this);
	if(fd.DoModal() == IDOK)
	{
		string svFileName = (CW2A)fd.GetPathName().GetBuffer(0);
		if(FileExistCheck(svFileName))
			if(MessageBox(TEXT("A file with the specified name already exists. Overwrite?"), TEXT("File exists"), MB_YESNO) != 6) // user clicked NO (do not overwrite file) (6 is OK)
				return;

		WriteDataQ(svFileName);
	}
}


void CActionRecorderDlg::OnBnClickedsavebtn2()
{
	// TODO: Add your control notification handler code here
	CombineEvents();
	save_one_RCrhing();
}


void CActionRecorderDlg::OnBnClickedsavebtn()
{
	CString szFilter = L"MultiTask Record|*.mtrc||";
	CFileDialog fd(false, L".mtrc", L"*.mtrc", OFN_HIDEREADONLY, szFilter, this);
	if(fd.DoModal() == IDOK)
	{
		string svFileName = (CW2A)fd.GetPathName().GetBuffer(0);
		if(FileExistCheck(svFileName))
			if(MessageBox(TEXT("A file with the specified name already exists. Overwrite?"), TEXT("File exists"), MB_YESNO) != 6) // user clicked NO (do not overwrite file) (6 is OK)
				return;

		ofstream fo(svFileName.c_str());
		fo << now_jobs_index << ' ' << now_sub_job_index <<endl;
		fo << play_speed <<endl<<endl;
		EventTaskTree *et_ptr = eventStorage.p_nxt;
		while(et_ptr!= NULL)
		{
			fo << et_ptr->id << endl
				<< et_ptr->type << endl
				<< et_ptr->parent << endl
				<< et_ptr->weight << endl
				<< et_ptr->rand_size << endl
				<< et_ptr->replaytype << endl
				<< et_ptr->idName << endl
				<< et_ptr->Nam_path << endl << endl;
			et_ptr = et_ptr->p_nxt;
		}
		fo.close();
	}
}

void CActionRecorderDlg::OnBnClickedButton1()
{
	CString szFilter = L"MultiTask Record|*.mtrc||";
	CFileDialog fd(true, L".mtrc", L"*.mtrc", OFN_HIDEREADONLY, szFilter, this);
	if(fd.DoModal() == IDOK)
	{
		CString dlgfName = fd.GetPathName();
		std::string LdFileName = (CW2A)dlgfName.GetBuffer(0);
		
		CleanJobQueue();
		enable_act_btn();

		int Did;
		int Dtype;
		int Dparent;
		int Dweight;
		int Drandsize;
		int Dretype;
		string DidName;
		string DNam_path;
		EventTaskTree* ee_ptr = &eventStorage;
		ifstream fi(LdFileName);
		fi >> now_jobs_index >> now_sub_job_index >> play_speed;
		while(fi>>Did>>Dtype>>Dparent>>Dweight>>Drandsize>>Dretype)
		{
			fi.get();
			fi.getline(s_input_buffer,1024);
			DidName = s_input_buffer;
			fi.getline(s_input_buffer,1024);
			DNam_path = s_input_buffer;
			if(Dtype == act_OneJob)
				ReadDataQ(DNam_path);
			ee_ptr->add(Did,Dtype,Dparent,Dweight,Drandsize,Dretype,DidName,DNam_path,ee_ptr->p_nxt);
			ee_ptr = ee_ptr->p_nxt;

			if(Dparent == -1)
			{
				CString strr(DidName.c_str());
				jobLst.InsertString(now_jobs_click++, strr );
			}
		}
		fi.close();
		set_speeder();
	}
}

void CActionRecorderDlg::OnEnChangeEdit1()
{
	CString strr;
	sub_jobList.GetText(now_sub_job_click, strr);

	CString edit_weigC;
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(edit_weigC);
	string edit_weig = (CW2A)edit_weigC.GetBuffer(0);
	int key_weight;
	std::stringstream(edit_weig) >> key_weight;
	if(key_weight >= 0)
	{
		eventStorage.find( string((CW2A)strr.GetBuffer(0)) )->weight = key_weight;
	}
}



void CActionRecorderDlg::OnEnChangeEdit2()
{
	CString speedC;
	GetDlgItem(IDC_EDIT2)->GetWindowTextW(speedC);
	string edit_speed = (CW2A)speedC.GetBuffer(0);
	double key_speed;
	std::stringstream(edit_speed) >> key_speed;
	if(key_speed > 0)play_speed = key_speed;
}


void CActionRecorderDlg::set_speeder(void)
{
	CString speed_str;
	speed_str.Format(_T("%.3f"),play_speed);
	GetDlgItem(IDC_EDIT2)->SetWindowTextW(speed_str);
}


void CActionRecorderDlg::OnEnChangeEdit3()
{
	CString strr;
	jobLst.GetText(now_jobs_click, strr);
	int spnpos = 0;
	int get_id = atoi((string((CW2A)(strr.Tokenize(_T("-"),spnpos)))).c_str());

	CString rnumC;
	GetDlgItem(IDC_EDIT3)->GetWindowTextW(rnumC);
	string edit_rnum = (CW2A)rnumC.GetBuffer(0);
	double key_rnum;
	std::stringstream(edit_rnum) >> key_rnum;
	if(key_rnum > 0 && key_rnum <= eventStorage.find_children(get_id).size())
		eventStorage.find( get_id )->rand_size = key_rnum;
}



void CActionRecorderDlg::CleanJobQueue(void)
{
	while(jobLst.GetCount())jobLst.DeleteString(0);
	while(sub_jobList.GetCount())sub_jobList.DeleteString(0);
	delete eventStorage.p_nxt;
	eventStorage.p_nxt = NULL;
	
	init_var();
}


void CActionRecorderDlg::OnBnClickedCleanbtn()
{
	CleanJobQueue();
}


