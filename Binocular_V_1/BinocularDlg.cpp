// BinocularDlg.cpp : ʵ���ļ�
#include "stdafx.h"
#include "Binocular_V_1.h"
#include "BinocularDlg.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

//CAboutDlg �Ի���
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};
//���ڽ���
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBinocularDlg �Ի���

CBinocularDlg::CBinocularDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BINOCULAR_V_1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	cam_online_num = 0;
}

CBinocularDlg::~CBinocularDlg()
{
	//�رն�ʱ��
	KillTimer(CAM_FINDER_TIMER_ID);
	KillTimer(CAM_FRAMERATE_GET_ID);
	//�ر���������
	DeleteCriticalSection(&Log_Protection);
}

void CBinocularDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_TRIGGER, _triger_type);
	DDX_Text(pDX, IDC_EDT_EXPORE, _setted_exp_val);
	DDX_IPAddress(pDX, IDC_IPADD_LEFT, _left_ip);
	DDX_IPAddress(pDX, IDC_IPADD_RIGHT, _right_ip);
	DDX_Control(pDX, IDC_EDITLOG, _log_ctrl);
	DDX_Control(pDX, IDC_STCRFRMRT, _right_frate);
	DDX_Control(pDX, IDC_STCLFRMRT, _left_frate);
	DDX_Control(pDX, IDC_EDT_EXPOREL, _left_expose_value);
	DDX_Control(pDX, IDC_EDT_EXPORER, _right_expose_value);
	DDX_Control(pDX, IDC_EDT_FRAMESETL, _frame_set_l);
	DDX_Control(pDX, IDC_EDT_FRAMESETR, _frame_set_r);
}

BEGIN_MESSAGE_MAP(CBinocularDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CMB_TRIGGER, &CBinocularDlg::OnSelchangeCmbTrigger)
	ON_BN_CLICKED(IDC_BTN_SETLEFT, &CBinocularDlg::OnBnClickedBtnopenleft)
	ON_BN_CLICKED(IDC_BTNCLOSELEFT, &CBinocularDlg::OnBnClickedBtncloseleft)
	ON_BN_CLICKED(IDC_BTN_SETRIGHT, &CBinocularDlg::OnBnClickedBtnopenright)
	ON_BN_CLICKED(IDC_BTNCLOSERIGHT, &CBinocularDlg::OnBnClickedBtncloseright)
	ON_BN_CLICKED(IDC_BTN_STARTACQ, &CBinocularDlg::OnBnClickedBtnStartacq)
	ON_BN_CLICKED(IDC_BTN_STOPACQ, &CBinocularDlg::OnBnClickedBtnStopacq)
	ON_BN_CLICKED(IDC_BTN_SETEXPOSEL, &CBinocularDlg::OnBnClickedBtnSetexposel)
	ON_BN_CLICKED(IDC_BTN_SETEXPOSER, &CBinocularDlg::OnBnClickedBtnSetexposer)
END_MESSAGE_MAP()


// CBinocularDlg ���ɵĴ���

BOOL CBinocularDlg::OnInitDialog()
{
	//About����Ĳ���
	CDialogEx::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	//���ɵĴ���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//����Pv������ʹ��ǰ ��Ҫ����PvInitialize()
	PvInitialize();

	_triger_type.SetCurSel(0);
	_frame_set_l.SetWindowTextA("30");
	_frame_set_r.SetWindowTextA("30");
	_left_expose_value.SetWindowTextA("5000");
	_right_expose_value.SetWindowTextA("5000");

	_left_ip = ntohl(inet_addr("192.168.1.3"));		//IP��ַת�� ���ĸ����ʮ����ת��2������ ������ʾ���ڿ������
	_right_ip = ntohl(inet_addr("192.168.1.2"));
	_setted_exp_val.Format("%d",5000);
	UpdateData(FALSE);

	//�����ؼ�����
	InitializeCriticalSection(&Log_Protection);

	//������ʱ��
	SetTimer(CAM_FINDER_TIMER_ID, 1000, 0);
	SetTimer(CAM_FRAMERATE_GET_ID, 1000, 0);
	append_log(string("Enviroment Init Finished.\r\n"));
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CBinocularDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CBinocularDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CBinocularDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�Լ��ĳ���
void CBinocularDlg::OnSelchangeCmbTrigger()
{
	//�ı䴥��ģʽ ֱ���趨���������
	//�Ȼ��
	int triger_mode = _triger_type.GetCurSel();
	CString fixed_frame_rate_s;
	if (left_Camera)
	{
		if (triger_mode == 3)
		{
			_frame_set_l.GetWindowTextA(fixed_frame_rate_s);
			int fixed_frame_rate = _ttoi(fixed_frame_rate_s);
			left_Camera->FrameRate = fixed_frame_rate;
		}
		if (!left_Camera->ChangeTrigerMode(triger_mode))
		{
			append_log(string("Wrnning: Left Camera changed trigger failed."));
		}
		append_log(left_Camera->outLog);
	}
	if (right_Camera)
	{
		if (triger_mode == 3)
		{
			_frame_set_r.GetWindowTextA(fixed_frame_rate_s);
			int fixed_frame_rate = _ttoi(fixed_frame_rate_s);
			right_Camera->FrameRate = fixed_frame_rate;
		}
		if (!right_Camera->ChangeTrigerMode(triger_mode))
		{
			append_log(string("Wrnning: Right Camera changed trigger failed."));
		}
		append_log(right_Camera->outLog);
	}
}

//check
void CBinocularDlg::OnBnClickedBtnopenleft()
{
	//�����½�һ����������ʵ�� ����ͼ ������Ҫ�������Ƿ����
	UpdateData(TRUE);
	if (!left_Camera)	//������������
	{
		left_Camera = new Camera(htonl(_left_ip), "Left");
		
		if (!left_Camera->Open())
		{
			append_log(left_Camera->outLog);
			delete left_Camera;
			left_Camera = NULL;
		}
		else
		{
			append_log(left_Camera->outLog);
			OnSelchangeCmbTrigger();
		}
			
	}
	else if (_left_ip != left_Camera->IpAddr)	//���������� ��IP���� ��ôֹͣ��ȥ����� �����µ����
	{
		OnBnClickedBtncloseright();
		left_Camera = new Camera(htonl(_left_ip), "Left");
		if (!left_Camera->Open())
		{
			append_log(left_Camera->outLog);
			delete left_Camera;
			left_Camera = NULL;
		}
		else
		{
			append_log(left_Camera->outLog);
			OnSelchangeCmbTrigger();
		}
			
	}
	//IP��Ӧ�Ļ� ��ť��Ч
}

//check
void CBinocularDlg::OnBnClickedBtncloseleft()
{
	//����һ�������ʵ��	�������ڲ�ͼ ��������������� ��Ӧ�ر�
	if (left_Camera == NULL)
	{
		append_log(string("Left Camera has already been removed or not Open."));
	}
	else
	{
		left_Camera->Close();
		append_log(left_Camera->outLog);
		delete left_Camera;
		left_Camera = NULL;
	}
}

//check
void CBinocularDlg::OnBnClickedBtnopenright()
{
	//�����½�һ����������ʵ�� ����ͼ ������Ҫ�������Ƿ����
	UpdateData(TRUE);
	if (!right_Camera)	//������������
	{
		right_Camera = new Camera(htonl(_right_ip), "Right");
		if (!right_Camera->Open())
		{
			append_log(right_Camera->outLog);
			delete right_Camera;
			right_Camera = NULL;
		}
		else
		{
			append_log(right_Camera->outLog);
			OnSelchangeCmbTrigger();
		}
	}
	else if (_right_ip != right_Camera->IpAddr)	//���������� ��IP���� ��ôֹͣ��ȥ����� �����µ����
	{
		OnBnClickedBtncloseright();
		right_Camera = new Camera(htonl(_right_ip), "Right");
		if (!right_Camera->Open())
		{
			append_log(right_Camera->outLog);
			delete right_Camera;
			right_Camera = NULL;
		}
		else
		{
			append_log(right_Camera->outLog);
			OnSelchangeCmbTrigger();
		}
	}
	//IP��Ӧ�Ļ� ��ť��Ч 
}

//check
void CBinocularDlg::OnBnClickedBtncloseright()
{
	//����һ�������ʵ��	�������ڲ�ͼ ��������������� ��Ӧ�ر�
	if (right_Camera == NULL)
	{
		append_log(string("Right Camera has already been removed or not Open."));
	}
	else
	{
		right_Camera->Close();	//�����Closeһ��Ҫ�ͷ�������Դ
		append_log(right_Camera->outLog);
		delete right_Camera;
		right_Camera = NULL;
	}
}

//ͬʱ�����е��߳����޸�Log������־
void CBinocularDlg::append_log(string & log_data)
{
	EnterCriticalSection(&Log_Protection);
	//������־
	CString log;
	_log_ctrl.GetWindowText(log);
	log += log_data.c_str();
	log += "\r\n";							//�Դ��س�

	int iCount = log.GetLength();
	_log_ctrl.SetRedraw(FALSE);
	_log_ctrl.SetWindowText(log);
	int iLine = _log_ctrl.GetLineCount();
	_log_ctrl.LineScroll(iLine, 0);
	_log_ctrl.SetSel(iCount, iCount);
	_log_ctrl.SetRedraw(TRUE);
	LeaveCriticalSection(&Log_Protection);
}

unsigned long FindCamAddLog(string& Out)
{
	Out.clear();
	//�����
	tPvCameraInfo list[MAX_CAMERA_NUM];
	unsigned long numCams;
	numCams = PvCameraList(list, MAX_CAMERA_NUM, NULL);
	if (numCams < 10)
	{
		numCams += PvCameraListUnreachable(&list[numCams],MAX_CAMERA_NUM - numCams,NULL);
	}
	tPvIpSettings ipSetting;
	tPvErr Err;
	struct in_addr address;

	for (unsigned int i = 0; i < numCams; i++)
	{
		Err = PvCameraIpSettingsGet(list[i].UniqueId, &ipSetting);
		if (Err == ePvErrSuccess)
		{
			address.S_un.S_addr = ipSetting.CurrentIpAddress;
			Out += "Camera " + to_string(i) + ": IP(" + inet_ntoa(address)+")\r\n";
		}
		else
		{
			Out += "Camera" + to_string(i) + ": IP unavailiable";
		}
	}
	Out += "Have " + to_string(numCams) + " Cameras Founded.";
	return numCams;
}

void CBinocularDlg::OnTimer(UINT_PTR uId)
{
	switch (uId)
	{
	case CAM_FINDER_TIMER_ID:
	{
		unsigned long number_now = FindCamAddLog(log_buffer);
		if (cam_online_num != number_now)
		{
			append_log(log_buffer);
			log_buffer.clear();
			cam_online_num = number_now;
		}
		break;
	}
	case CAM_FRAMERATE_GET_ID: //������������֡��
	{
		int fram_rate = 0;
		if (left_Camera != NULL)
		{
			fram_rate = left_Camera->FrameCount - frmcnt_buffer_l;
			frmcnt_buffer_l = left_Camera->FrameCount;
			_left_frate.SetWindowTextA(to_string(fram_rate).c_str());
		}
		else
		{
			frmcnt_buffer_l = 0;
			_left_frate.SetWindowTextA("N/A");
		}
		if (right_Camera != NULL)
		{
			int fram_rate = right_Camera->FrameCount - frmcnt_buffer_r;
			frmcnt_buffer_r = right_Camera->FrameCount;
			_right_frate.SetWindowTextA(to_string(fram_rate).c_str());
		}
		else
		{
			frmcnt_buffer_r = 0;
			_right_frate.SetWindowTextA("N/A");
		}
		break;
	}
	/*
		��������������ʱ�Ӻ���
	*/
	default:
		break;
	}
}


//��ʼ�ɼ�ͼ��
void CBinocularDlg::OnBnClickedBtnStartacq()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	//Ϊ�������һ���Լ����߳�
	if (left_Camera)
	{
		left_thread = AfxBeginThread(Left_ThreadCapture, this);
	}
	if (right_Camera)
	{
		right_thread = AfxBeginThread(Right_ThreadCapture, this);
	}
}
//ֹͣ�ɼ�ͼ��
void CBinocularDlg::OnBnClickedBtnStopacq()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (left_Camera)
	{
		if (left_Camera->isStreaming)
		{
			left_Camera->CloseCapture();
			append_log(left_Camera->outLog);
		}
	}
	if (right_Camera)
	{
		if (right_Camera->isStreaming)
		{
			right_Camera->CloseCapture();
			append_log(right_Camera->outLog);
		}
	}
}

UINT Left_ThreadCapture(LPVOID lpParam)
{
	CBinocularDlg *dlg = (CBinocularDlg*)lpParam;
	bool bCapture;
	bCapture = dlg->left_Camera->StartCapture();//���ÿ�ʼ�ɼ�����
	dlg->append_log(dlg->left_Camera->outLog);
	return bCapture;
}

UINT Right_ThreadCapture(LPVOID lpParam)
{
	CBinocularDlg *dlg = (CBinocularDlg*)lpParam;
	bool bCapture;
	bCapture = dlg->right_Camera->StartCapture();//���ÿ�ʼ�ɼ�����
	dlg->append_log(dlg->right_Camera->outLog);
	return bCapture;
}

//�ı��ع�ֵ
void CBinocularDlg::OnBnClickedBtnSetexposel()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (left_Camera)
	{
		CString expose_s;
		int expose_v;
		_left_expose_value.GetWindowTextA(expose_s);
		expose_v = _ttoi(expose_s);
		if (!left_Camera->ChangeExposeValue(expose_v))
		{
			append_log(string("Wrnning: Left Camera changed expose val failed."));
		};
		append_log(left_Camera->outLog);
	}
}

void CBinocularDlg::OnBnClickedBtnSetexposer()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if (right_Camera)
	{
		CString expose_s;
		int expose_v;
		_right_expose_value.GetWindowTextA(expose_s);
		expose_v = _ttoi(expose_s);
		if (!right_Camera->ChangeExposeValue(expose_v))
		{
			append_log(string("Wrnning: Right Camera changed expose val failed."));
		};
		append_log(right_Camera->outLog);
	}
}