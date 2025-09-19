#include "stdafx.h"
#include "framework.h"
#include "fap50_sdk_app.h"
#include "fap50_sdk_appDlg.h"
#include "imd_framework.h"
//
//Add for powershell
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <atlstr.h>  // CString
#pragma comment(lib, "shlwapi.lib")

#define PANEL_IP "192.168.100.10"
#define PANEL_PORT  1812

enum TIMER_EVENT {
	TIMER_EVENT_SHOW_IMAGE,
	TIMER_EVENT_SHOW_PANEL,
	TIMER_EVENT_STANDBY_VIDEO,
	TIME_DEBOUNCE_BRIGHTNESS,

	TIMER_SHOW_IMAGE = 40, // (1000 / 30),
	TIMER_SHOW_PANEL = 40, //(1000/66),
	//TIMER_SHOW_SIGNATURE = 1, //(1000/40),
	TIMER_SHOW_VIDEO = 10 * 1000,
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LIST_SCALE 0.31

#define GRAY_COLOR	RGB(128, 128, 128)
#define	BLACK_COLOR RGB(0, 0, 0)
#define WHITE_COLOR RGB(255, 255, 255)
#define YELLOW_COLOR RGB(255, 255, 0)

#define _1_COLOR	RGB(22, 19, 104)
#define _2_COLOR	RGB(51, 52, 144)
#define _3_COLOR	WHITE_COLOR
#define _4_COLOR	RGB(232, 73, 53)
#define GREEN_COLOR _4_COLOR//GRAY_COLOR //RGB(51, 204, 51) //RGB(146, 208, 80)

#define FIGURE_BORDER_SIZE 5
#define FINGER_TEXT_X_OFFSET  25 
#define FOUR_FINGER_X_OFFSET 400 
#define ROLL_FINGER_TEXT_Y_OFFSET  725
#define FLAT_FINGER_TEXT_Y_OFFSET  975 
#define SCORE_OFFSET	85 
#define THICKNESS  7 

#define USER_DEFINE_TEXT L"User Define"
#define SCALE_PANEL_TO_GUIDE 0.56 // guid/panel(480*800)

//#define SIGN_RECODE	

void ShowImageOnControl(Mat& image, CStatic& staticControl) 
{
	CClientDC dc(&staticControl);  // 獲取控制項的 DC
	RECT rect;
	staticControl.GetClientRect(&rect);

	// 配置 BITMAPINFO 結構體
	BITMAPINFO bitmapInfo;
	memset(&bitmapInfo, 0, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = image.cols;
	bitmapInfo.bmiHeader.biHeight = -image.rows;  // 負值表示從上到下
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 24;         // 假設 BGR 圖像
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	// 使用 StretchDIBits 將影像繪製到控制項
	StretchDIBits(dc.GetSafeHdc(),
		0, 0, rect.right, rect.bottom,  // 目標區域大小
		0, 0, image.cols, image.rows,   // 來源圖像大小
		image.data,
		&bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}

static mutex mu;
void C_FAP50_SDK_Dlg::my_imshow(string id, Mat img)
{
	lock_guard<mutex> lock(mu);

	CString str_id (id.c_str());
	if (str_id != L"IDC_GUIDE")
	{
		imshow(id, img);
		return;
	}
	CStatic& com = *((CStatic*)this->GetDlgItem(IDC_GUIDE));
	ShowImageOnControl(img, com);
}

//#define imshow my_imshow


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedDisableAllUSB();
	afx_msg void OnBnClickedEnableAudioUSB();
	afx_msg void OnBnClickedEnableUSB2USB3();
	afx_msg void OnBnClickedBurnCode();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_PW, &CAboutDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BTN_DISABLE_ALL_USB, &CAboutDlg::OnBnClickedDisableAllUSB)
	ON_BN_CLICKED(IDC_BTN_AUDIO_USB, &CAboutDlg::OnBnClickedEnableAudioUSB)
	ON_BN_CLICKED(IDC_BTN_ENABLE_USB23, &CAboutDlg::OnBnClickedEnableUSB2USB3)
	ON_BN_CLICKED(IDC_BTN_BURN_CODE, &CAboutDlg::OnBnClickedBurnCode)
END_MESSAGE_MAP()

// Cfap50sdkappDlg dialog
static C_FAP50_SDK_Dlg* dlg;

C_FAP50_SDK_Dlg::C_FAP50_SDK_Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FAP50_SDK_APP_DIALOG, pParent)
    , msg_window(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nPreviorBrightnessValue = 0;
	dlg = this;
}

C_FAP50_SDK_Dlg::~C_FAP50_SDK_Dlg()
{

} 

void C_FAP50_SDK_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_msg_window, msg_window);
	DDX_Control(pDX, IDC_CB_FINGER_POS, cb_finger_position);
	DDX_Control(pDX, IDC_CB_NFIQ_VER, cb_nfiq_ver);
	DDX_Control(pDX, IDC_CB_SAMPLING_TYPE, cb_sampling_sequence);
	DDX_Control(pDX, IDC_FINGER_IMAGES, finger_images);
	DDX_Control(pDX, IDC_MFCBTN_START, mfc_btn_start);
	DDX_Control(pDX, IDC_MFCBTN_SAVE_FILE, mfc_btn_save_file);
	DDX_Control(pDX, IDC_CB_LANG, cb_language);
	DDX_Control(pDX, IDC_CB_VOL, cb_volume);
	DDX_Control(pDX, IDC_CB_back_light, cb_back_light);
}

void C_FAP50_SDK_Dlg::message_bar(CString msg, BOOL clear_msg_en=FALSE)
{
	if (clear_msg_en)
		clear_msg();

	CString org;
	GetDlgItem(IDC_EDIT1)->GetWindowText(org);
	org += L" " + msg;
	GetDlgItem(IDC_EDIT1)->SetWindowText(org);
	GetDlgItem(IDC_EDIT1)->Invalidate();
	GetDlgItem(IDC_EDIT1)->UpdateWindow();
}

void C_FAP50_SDK_Dlg::clear_msg()
{
	GetDlgItem(IDC_EDIT1)->SetWindowText(L"");
}

void WINAPI shell_callback_fap50_event(IMD_RESULT e)
{
	if (dlg->m_hWnd)
		dlg->callback_fap50_event(e);
}

void C_FAP50_SDK_Dlg::callback_fap50_event(IMD_RESULT prompt)
{
	CString str;

	switch (prompt) {
	case IMD_RLT_CLAI_TIMEOUT:
		str.Format(L"Calibration timeout! ");
		break;
	case IMD_RLT_SCAN_THREAD_START:
		//str.Format(L"Start scanning ... ");
		//break;
		return;
	case IMD_RLT_SCAN_THREAD_END:
		//str.Format(L"Scan thread end. ");
		//break;
		return;
	case IMD_RLT_CANT_FIND_ANY_DEVICE:
		str.Format(L"Can not find any device. Need to re-plugin USB. ");
		break;
	case IMD_RLT_SCAN_THREAD_IDLE_TIMEOUT:
		str.Format(L"Scan idle timeout. ");
		break;
	case IMD_RLT_DCMI_IS_STUCK:
		str.Format(L"DCMI has stopped with no response. ");
		break;
	case IMD_RLT_USB_TOO_SLOW:
		str.Format(L"USB is too slow. ");
		break;
	case IMD_RLT_USB_READ_IMAGE_EXCEPTION:
		str.Format(L"USB read image exception. ");
		break;
	case IMD_RLT_USB_READ_IMAGE_TIMEOUT:
		str.Format(L"USB read image timeout. ");
		break;
	case IMD_RLT_RESET_DEVICE_FAIL:
		str.Format(L"Reset device fail. ");
		break;
	case IMD_RLT_NO_AVAILABLE_IMAGE:		
		str.Format(L"No images available. ");
		break;
	case IMD_RLT_CHIP_ID_FAIL:
		str.Format(L"Chip ID is wrong. Need to re-plugin USB.");
		break;
	default:
		str.Format(L"Something happened! 0x%X ", prompt);
	}

	message_bar(str);

	switch (prompt) {
	case IMD_RLT_USB_READ_IMAGE_EXCEPTION:
	case IMD_RLT_SCAN_THREAD_IDLE_TIMEOUT:
		cancel_sampling = true;
	}
}

BEGIN_MESSAGE_MAP(C_FAP50_SDK_Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_RESET, &C_FAP50_SDK_Dlg::OnBnClickedBtnReset)
	ON_WM_TIMER()
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MFCBTN_START, &C_FAP50_SDK_Dlg::OnBnClickedBtnStart)
	ON_CBN_SELCHANGE(IDC_CB_SAMPLING_TYPE, &C_FAP50_SDK_Dlg::OnCbnSelchangeCbSamplingSequence)
	ON_BN_CLICKED(IDC_MFCBTN_SAVE_FILE, &C_FAP50_SDK_Dlg::OnBnClickedBtnSaveFile)
	ON_CBN_SELCHANGE(IDC_CB_NFIQ_VER, &C_FAP50_SDK_Dlg::OnCbnSelchangeCbNfiqVer)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &C_FAP50_SDK_Dlg::OnBnClickedUserSpaceTest)
	ON_CBN_SELCHANGE(IDC_CB_LANG, &C_FAP50_SDK_Dlg::OnCbnSelchangeCbLang)
	ON_CBN_SELCHANGE(IDC_CB_VOL, &C_FAP50_SDK_Dlg::OnCbnSelchangeCbVol)
	ON_EN_CHANGE(IDC_EDIT1, &C_FAP50_SDK_Dlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BTN_R4, &C_FAP50_SDK_Dlg::OnBnClickedBtnR4)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_ERASE_SING, &C_FAP50_SDK_Dlg::OnBnClickedBtnEraseSing)
	ON_BN_CLICKED(IDC_BTN_SIGN_DONE, &C_FAP50_SDK_Dlg::OnBnClickedBtnSignDone)
	ON_BN_CLICKED(IDC_BTN_show_video, &C_FAP50_SDK_Dlg::OnBnClickedBtnshowvideo)
	ON_BN_CLICKED(IDC_BTN_test, &C_FAP50_SDK_Dlg::OnBnClickedBtntest)
	ON_CBN_SELCHANGE(IDC_CB_back_light, &C_FAP50_SDK_Dlg::OnCbnSelchangeCbbacklight)
END_MESSAGE_MAP()

void C_FAP50_SDK_Dlg::resample()
{
	message_bar(L"Resampling fingerprint ... \n");

	if (!reset(false))
		goto func_end;

	set_gui(true);

	sampling_finger(resample_mode, resample_pos);
	if (cancel_sampling)
		goto func_end;

	message_bar(L"Sampling Done.\n", true);

func_end:
	set_gui(false);
}

BOOL C_FAP50_SDK_Dlg::get_sampling_mode(int x, int y) 
{
	E_GUI_SHOW_MODE mode = GUI_SHOW_MODE_ROLL;
	E_FINGER_POSITION pos = FINTER_POSITION_UNKNOW_FINGER;
	CString msg = L"Do you want to ";

	switch (sampling_sequence) {
	case SS_FLAT_442:
		mode = GUI_SHOW_MODE_FLAT;
		if (y > 467)
		{
			if (x > 750)
			{
				msg += L"sample right four fingers";
				pos = FINGER_POSITION_RIGHT_FOUR;
			}
			else if (x > 500)
			{
				msg += L"sample two thumbs";
				pos = FINGER_POSITION_BOTH_THUMBS;
			}
			else
			{
				msg += L"sample left four fingers";
				pos = FINGER_POSITION_LEFT_FOUR;
			}
		}
		else if (y > 235)
		{
			if (x > 250)
			{
				msg += L"sample left four fingers";
				pos = FINGER_POSITION_LEFT_FOUR;
			}
			else
			{
				msg += L"sample two thumbs";
				pos = FINGER_POSITION_BOTH_THUMBS;
			}
		}
		else //x>0
		{
			if (x > 250)
			{
				msg += L"sample right four fingers";
				pos = FINGER_POSITION_RIGHT_FOUR;
			}
			else
			{
				msg += L"sample two thumbs";
				pos = FINGER_POSITION_BOTH_THUMBS;
			}
		}
		break;

	case SS_FLAT_442R:
		if (y > 467)
		{
			mode = GUI_SHOW_MODE_FLAT;
			if (x > 750)
			{
				msg += L"sample right four fingers";
				pos = FINGER_POSITION_RIGHT_FOUR;
			}
			else if (x > 497)
			{
				msg += L"sample two thumbs";
				pos = FINGER_POSITION_BOTH_THUMBS;
			}
			else
			{
				msg += L"sample left four fingers";
				pos = FINGER_POSITION_LEFT_FOUR;
			}
		}
		else if (y > 235)
		{
			if (x > 1000)
			{
				msg += L"roll left little finger";
				pos = FINGER_POSITION_LEFT_LITTLE;
			}
			else if (x > 750)
			{
				msg += L"roll left ring finger";
				pos = FINGER_POSITION_LEFT_RING;
			}
			else if (x > 500)
			{
				msg += L"roll left middle finger";
				pos = FINGER_POSITION_LEFT_MIDDLE;
			}
			else if (x > 250)
			{
				msg += L"roll left index finger";
				pos = FINGER_POSITION_LEFT_INDEX;
			}
			else
			{
				msg += L"roll left thumb";
				pos = FINGER_POSITION_LEFT_THUMB;
			}
		}
		else //x>0
		{
			if (x > 1000)
			{
				msg += L"roll right little finger";
				pos = FINGER_POSITION_RIGHT_LITTLE;
			}
			else if (x > 750)
			{
				msg += L"roll right ring finger";
				pos = FINGER_POSITION_RIGHT_RING;
			}
			else if (x > 500)
			{
				msg += L"roll right middle finger";
				pos = FINGER_POSITION_RIGHT_MIDDLE;
			}
			else if (x > 250)
			{
				msg += L"roll right index finger";
				pos = FINGER_POSITION_RIGHT_INDEX;
			}
			else
			{
				msg += L"roll right thumb";
				pos = FINGER_POSITION_RIGHT_THUMB;
			}
		}
		break;

	default:
		return false;
	}

	msg += L"?";

	if (disable_prompts == false)
	{
		int result = dlg->MessageBox(msg, L"Sample finger", MB_YESNO);
		if (result != IDYES)
			return false;
	}

	resample_mode = mode;
	resample_pos = pos;
	return true;
}

void my_MouseCallback(int event, int x, int y, int flags, void* userdata)
{
	static UINT timeout = timeGetTime();

	if (flags != 2)
		return;
	
	//dbg(L"x=%d y=%d \n", x, y);

	if(dlg->get_sampling_mode(x, y) == false)
		return;

	if (timeGetTime() > timeout)
	{
		timeout = timeGetTime()+1500;
		//dbg("my_MouseCallback\n");
		thread_class_member(dlg, C_FAP50_SDK_Dlg::resample);
	}
}

LRESULT CALLBACK HiddenWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Cfap50sdkappDlg message handlers
BOOL C_FAP50_SDK_Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LoadPNG(IDB_iMD_Logo, IDC_iMDLogo, 0, 0, 185, 81);

	//--- set scan timeout
	SystemProperty pty;
	get_system_property(&pty);
	pty.scan_timeout_ms = 60 * 1000;

	//pty.life_check_en = 0; //Notice: if debug the prj, need to set 0.
	pty.life_check_en |= atoi((CT2A)ini.get_value(L"life_check_en", L"SystemProperty"));
	pty.nfiq_score_minimum_acceptable = atoi((CT2A)ini.get_value(L"nfiq_score_minimum_acceptable", L"SystemProperty"));
	pty.nfiq2_score_minimum_acceptable = atoi((CT2A)ini.get_value(L"nfiq2_score_minimum_acceptable", L"SystemProperty"));
	pty.speech_en = atoi((CT2A)ini.get_value(L"speech_en", L"SystemProperty"));
	//is_scan_by_manual = true;
	pty.scan_by_manual = is_scan_by_manual;
	set_system_property(&pty);

	disable_prompts |= atoi((CT2A)ini.get_value(L"disable_prompts", L"Main"));
{
	cb_finger_position.AddString(L"UNKNOW_FINGER");
	cb_finger_position.AddString(L"RIGHT_THUMB");
	cb_finger_position.AddString(L"RIGHT_INDEX");
	cb_finger_position.AddString(L"RIGHT_MIDDLE");
	cb_finger_position.AddString(L"RIGHT_RING");
	cb_finger_position.AddString(L"RIGHT_LITTLE");
	cb_finger_position.AddString(L"LEFT_THUMB");
	cb_finger_position.AddString(L"LEFT_INDEX");
	cb_finger_position.AddString(L"LEFT_MIDDLE");
	cb_finger_position.AddString(L"LEFT_RING");
	cb_finger_position.AddString(L"LEFT_LITTLE");
	cb_finger_position.AddString(L"---");
	cb_finger_position.AddString(L"---");
	cb_finger_position.AddString(L"RIGHT_FOUR");
	cb_finger_position.AddString(L"LEFT_FOUR");
	cb_finger_position.AddString(L"BOTH_THUMBS");
	cb_finger_position.SetCurSel(0);
	cb_finger_position.EnableWindow(FALSE);


	cb_nfiq_ver.AddString(L"NFIQ");
	cb_nfiq_ver.AddString(L"NFIQ2");
	cb_nfiq_ver.SetCurSel(0);


	cb_language.AddString(L"English");
	cb_language.AddString(L"Chinese");
	cb_language.AddString(L"Spanish");
	cb_language.AddString(USER_DEFINE_TEXT);
	cb_language.SetCurSel(0);
	OnCbnSelchangeCbLang();


	cb_volume.AddString(L"Low");
	cb_volume.AddString(L"Medium");
	cb_volume.AddString(L"High");
	cb_volume.AddString(L"Mute");
	cb_volume.SetCurSel(1);
	OnCbnSelchangeCbVol();


	cb_back_light.AddString(L"Disable");
	cb_back_light.AddString(L"Low");
	CString str;
	for(int i=2; i<=9; i++)
	{
		str.Format(L"%d", i);
		cb_back_light.AddString(str);
	}
	cb_back_light.AddString(L"High");
	cb_back_light.SetCurSel(8);
	OnCbnSelchangeCbbacklight();


	cb_sampling_sequence.AddString(L"Flat 4-4-2");
	cb_sampling_sequence.AddString(L"Flat 4-4-2 Roll");
	cb_sampling_sequence.AddString(L"Flat Some Fingers");
	cb_sampling_sequence.AddString(L"Sign by sensor");
	cb_sampling_sequence.AddString(L"Sign by pen");
	sampling_sequence = 0;//1,4
	cb_sampling_sequence.SetCurSel(sampling_sequence);
}

	set_event(shell_callback_fap50_event);

	//IDC_FINGER_IMAGES fingerprint list
	namedWindow("IDC_FINGER_IMAGES", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("IDC_FINGER_IMAGES");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_FINGER_IMAGES)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	GetDlgItem(IDC_FINGER_IMAGES)->MoveWindow(6, 95, 1248, 780);

	//IDC_SAMPLING
	namedWindow("IDC_SAMPLING", WINDOW_AUTOSIZE);
	hWnd = (HWND)cvGetWindowHandle("IDC_SAMPLING");
	hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_SAMPLING)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	GetDlgItem(IDC_SAMPLING)->MoveWindow(470, 96, 780, 780);

	//IDC_GUIDE
	namedWindow("IDC_GUIDE", WINDOW_AUTOSIZE);
	hWnd = (HWND)cvGetWindowHandle("IDC_GUIDE");
	hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_GUIDE)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	GetDlgItem(IDC_GUIDE)->MoveWindow(10, 96, 448, 268);//W/H = 1.6666

	Mat init_img{ 1000, 1600, CV_8UC1, Scalar(255) };
	imshow("IDC_SAMPLING", init_img);
	imshow("IDC_GUIDE", init_img);

	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	str_dir = szPath;

	CString str_bg_path;
	str_bg_path = str_dir + L"/panel/RightHandPanel/RightHandPanel-Main.png";
	scanning_img[FINGER_POSITION_RIGHT_FOUR] = imread((char*)(CT2A)str_bg_path);
	str_bg_path = str_dir + L"/panel/LeftHandPanel/LeftHandPanel-Main.png";
	scanning_img[FINGER_POSITION_LEFT_FOUR] = imread((char*)(CT2A)str_bg_path);
	str_bg_path = str_dir + L"/panel/ThumbsPanel/ThumbPanel-Main.png";
	scanning_img[FINGER_POSITION_BOTH_THUMBS] = imread((char*)(CT2A)str_bg_path);

	str_bg_path = str_dir + L"/panel/RightRollingFingers/RightRolling-MainPanel.png";
	scanning_img[FINGER_POSITION_RIGHT_THUMB] = imread((char*)(CT2A)str_bg_path);
	scanning_img[FINGER_POSITION_RIGHT_INDEX] = scanning_img[FINGER_POSITION_RIGHT_THUMB];
	scanning_img[FINGER_POSITION_RIGHT_MIDDLE] = scanning_img[FINGER_POSITION_RIGHT_THUMB];
	scanning_img[FINGER_POSITION_RIGHT_RING] = scanning_img[FINGER_POSITION_RIGHT_THUMB];
	scanning_img[FINGER_POSITION_RIGHT_LITTLE] = scanning_img[FINGER_POSITION_RIGHT_THUMB];

	str_bg_path = str_dir + L"/panel/LeftRollingFingers/LeftRolling-MainPanel.png";
	scanning_img[FINGER_POSITION_LEFT_THUMB] = imread((char*)(CT2A)str_bg_path);
	scanning_img[FINGER_POSITION_LEFT_INDEX] = scanning_img[FINGER_POSITION_LEFT_THUMB];
	scanning_img[FINGER_POSITION_LEFT_MIDDLE] = scanning_img[FINGER_POSITION_LEFT_THUMB];
	scanning_img[FINGER_POSITION_LEFT_RING] = scanning_img[FINGER_POSITION_LEFT_THUMB];
	scanning_img[FINGER_POSITION_LEFT_LITTLE] = scanning_img[FINGER_POSITION_LEFT_THUMB];

	str_bg_path = str_dir + L"/panel/SignaturePanel/SignatureScreen-MainPanel-NoButton.png";
	scanning_img[FINGER_POSITION_SIGNATURE] = imread((char*)(CT2A)str_bg_path);

	str_bg_path = str_dir + L"/panel/LoadingScreen/WithButton/ScanningFrame-1.png";
	loading_img[1] = imread((char*)(CT2A)str_bg_path);
	str_bg_path = str_dir + L"/panel/LoadingScreen/WithButton/ScanningFrame-2.png";
	loading_img[2] = imread((char*)(CT2A)str_bg_path);
	str_bg_path = str_dir + L"/panel/LoadingScreen/WithButton/ScanningFrame-3.png";
	loading_img[3] = imread((char*)(CT2A)str_bg_path);

	init_list();
	setMouseCallback("IDC_FINGER_IMAGES", my_MouseCallback);

	//set font size and style.
	CFont* text_font = new CFont;
	text_font->CreateFont(
		16, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_MEDIUM,//FW_BOLD, // nWeight 
		FALSE,//TRUE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("Arial")); // lpszFac 

	GetDlgItem(IDC_STATIC1)->SetFont(text_font);
	GetDlgItem(IDC_STATIC2)->SetFont(text_font);
	GetDlgItem(IDC_STATIC3)->SetFont(text_font);
	GetDlgItem(IDC_STATIC4)->SetFont(text_font);
	GetDlgItem(IDC_STATIC5)->SetFont(text_font);
	GetDlgItem(IDC_CB_NFIQ_VER)->SetFont(text_font);
	GetDlgItem(IDC_CB_SAMPLING_TYPE)->SetFont(text_font);
	GetDlgItem(IDC_CB_LANG)->SetFont(text_font);
	GetDlgItem(IDC_CB_VOL)->SetFont(text_font);
	GetDlgItem(IDC_STATIC_BackLight)->SetFont(text_font);

	CFont* f = new CFont;
	f->CreateFont(
		24, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_MEDIUM,//FW_BOLD, // nWeight 
		FALSE,//TRUE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("Arial")); // lpszFac 

	mfc_btn_start.SetFaceColor(WHITE_COLOR);
	mfc_btn_start.SetTextColor(BLACK_COLOR);
	mfc_btn_start.m_bDontUseWinXPTheme = TRUE;
	mfc_btn_start.SetFont(f);

	mfc_btn_save_file.SetFaceColor(WHITE_COLOR);
	mfc_btn_save_file.SetTextColor(GRAY_COLOR);
	mfc_btn_save_file.m_bDontUseWinXPTheme = TRUE;
	mfc_btn_save_file.SetFont(f);

	f = new CFont;
	f->CreateFont(
		20, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_THIN,//FW_BOLD, // nWeight 
		FALSE,//TRUE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("Arial")); // lpszFac 
	GetDlgItem(IDC_msg_window)->SetFont(f);
	GetDlgItem(IDC_msg_window)->ModifyStyleEx(0, WS_EX_TRANSPARENT);
	
	GetDlgItem(IDC_EDIT1)->SetWindowText(L"Hi :) \n");
	//GetDlgItem(IDC_EDIT1)->EnableWindow(0);
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetReadOnly();
	GetDlgItem(IDC_EDIT1)->SetFont(f);

	m_edit_bg_color.CreateSolidBrush(_1_COLOR);//211, 222, 241));

	sampling_ui_ids = { IDC_SAMPLING, 
		//IDC_BTN_L4, IDC_BTN_R4, IDC_BTN_TWO_TB,
		//IDC_BTN_L_1, IDC_BTN_L_2, IDC_BTN_L_3, IDC_BTN_L_4, IDC_BTN_L_5,
		//IDC_BTN_R_1, IDC_BTN_R_2, IDC_BTN_R_3, IDC_BTN_R_4, IDC_BTN_R_5,
		IDC_BTN_ERASE_SING, IDC_BTN_SIGN_DONE,
		};
	main_ui_ids = { IDC_FINGER_IMAGES,
		};

	CMFCButton* btn;
	for (auto& id : sampling_ui_ids)
	{
		if (id == IDC_SAMPLING)
			continue;

		btn = (CMFCButton*)GetDlgItem(id);
		btn->SetFont(text_font);
		btn->SetFaceColor(WHITE_COLOR);// GREEN_COLOR);// WHITE_COLOR);
		btn->SetTextColor(BLACK_COLOR);
		btn->m_bDontUseWinXPTheme = TRUE;
	}

	dlg_sel_finger = new C_SeletFingers;
	dlg_sel_finger->Create(IDD_DLG_SEL_FINGERS, this);

	set_gui(false);

	host_speech.set_dir(str_dir);
	//host_speech.test();
	//dbg(L"sizeof(ImageStatus)=%d sizeof(bool)=%d \n", sizeof(ImageStatus), sizeof(bool));
	RunEmbeddedPowerShellScript();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

bool C_FAP50_SDK_Dlg::LoadPNG(int IDBPNG, int IDCPNG, int x, int  y, int cx, int cy)
{
	HMODULE hInst = NULL;
	HGLOBAL m_hBuffer = NULL;
	CImage m_Image_ics;

	HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDBPNG), _T("PNG"));
	if (!hResource)
		return false;

	DWORD imageSize = ::SizeofResource(hInst, hResource);
	if (!imageSize)
		return false;

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
	if (!pResourceData)
		return false;

	m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);

		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);
			HRESULT hr;
			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				CStatic* pWnd = (CStatic*)GetDlgItem(IDCPNG);
				hr = m_Image_ics.Load(pStream);
				HBITMAP hBmp = m_Image_ics.Detach();
				pWnd->SetBitmap(hBmp);
				pWnd->SetWindowPos(NULL, x, y, cx, cy, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
				// return TRUE  unless you set the focus to a control
			}

			::GlobalUnlock(m_hBuffer);
		}

		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	return false;
}

void C_FAP50_SDK_Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		static CAboutDlg dlgAbout;// = NULL;
		//if(dlgAbout==NULL)
		//	new CAboutDlg;

		SystemProperty p;
		get_system_property(&p);
		CString msg, strGuid;

		strGuid.Format(L"%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			p.guid[0], p.guid[1], p.guid[2], p.guid[3],
			p.guid[4], p.guid[5], 
			p.guid[6], p.guid[7], 
			p.guid[8], p.guid[9], 
			p.guid[10], p.guid[11], p.guid[12], p.guid[13], p.guid[14], p.guid[15]);

			msg.Format(L"\
			\niMD FAP50 LIB Version: %d.%d.%d\
			\nOpenCV LIB version: %d.%d.%d\
			\nNBIS LIB version: %d.%d.%d\
			\nNFIQ2 LIB version: %d.%d.%d\
			\nChip ID: 0x%4X\
			\nModule FW Version: %X.%02X\
			\nRB FW Version: %d.%d\
			\nGUID: %s\
			\nS/N: %s\
			\nBrand: %s\
			\nModel: %s\
			\n ",

			p.fap50_lib_ver[2], p.fap50_lib_ver[1], p.fap50_lib_ver[0],
			p.opencv_lib_ver[2], p.opencv_lib_ver[1], p.opencv_lib_ver[0],
			p.nbis_lib_ver[2], p.nbis_lib_ver[1], p.nbis_lib_ver[0],
			p.nfiq2_lib_ver[2], p.nfiq2_lib_ver[1], p.nfiq2_lib_ver[0],
			p.chip_id,
			p.fw_ver[1], p.fw_ver[0],
			p.led_ver[1], p.led_ver[0],
			strGuid,
			(LPCTSTR)(CA2T)p.product_sn,
			(LPCTSTR)(CA2T)p.product_brand,
			(LPCTSTR)(CA2T)p.product_model

		);

		dlgAbout.Create(IDD_ABOUTBOX);
		CWnd *wnd = dlgAbout.GetDlgItem(IDC_STATIC_INFO);//
		wnd->SetWindowText(msg);
		dlgAbout.ShowWindow(SW_NORMAL);
		//delete dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void C_FAP50_SDK_Dlg::OnPaint()
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
HCURSOR C_FAP50_SDK_Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL C_FAP50_SDK_Dlg::reset(BOOL clear_all)
{
	static BOOL is_app_first = true;

	if (clear_all || is_app_first)
	{
		is_app_first = false;
		if (disable_prompts == false)
		{
			int result = MessageBox(L"The sampled fingerprints will be lost. \nDo you want to continue?", L"Warning", MB_YESNO);
			if (result == IDNO)
				return false;
		}

		mfc_btn_save_file.SetTextColor(GRAY_COLOR);
		mfc_btn_save_file.Invalidate();

		message_bar(L"Device init ... ", true);
		IMD_RESULT res = device_reset();
		//init_list();
		if (res != IMD_RLT_SUCCESS)
		{
			callback_fap50_event(res);
			return false;
		}
		message_bar(L"done.\n");
	}

	///////////////////////////////////////////////////////
	sampling_color_old_id = 0;

	cancel_sampling = false;
	clear_msg();

	return TRUE;
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnReset()
{
	reset();
}

void C_FAP50_SDK_Dlg::sampling_msg(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos)
{
	if (sampling_sequence == SS_FLAT_SOME_FINGERS)
	{
		message_bar(L"Flat some fingers... ", TRUE);
		return;
	}

	switch (pos) {
	case FINGER_POSITION_RIGHT_THUMB:
		message_bar(L"Roll right thumb... ", TRUE);
		break;
	case FINGER_POSITION_RIGHT_INDEX:
		message_bar(L"Roll right index... ", TRUE);
		break;
	case FINGER_POSITION_RIGHT_MIDDLE:
		message_bar(L"Roll right middle... ", TRUE); 
		break;
	case FINGER_POSITION_RIGHT_RING:
		message_bar(L"Roll right ring... ", TRUE); 
		break;
	case FINGER_POSITION_RIGHT_LITTLE:
		message_bar(L"Roll right little... ", TRUE); 
		break;

	case FINGER_POSITION_LEFT_THUMB:
		message_bar(L"Roll left thumb... ", TRUE); 
		break;
	case FINGER_POSITION_LEFT_INDEX:
		message_bar(L"Roll left index... ", TRUE); 
		break;
	case FINGER_POSITION_LEFT_MIDDLE:
		message_bar(L"Roll left middle... ", TRUE); 
		break;
	case FINGER_POSITION_LEFT_RING:
		message_bar(L"Roll left ring... ", TRUE); 
		break;
	case FINGER_POSITION_LEFT_LITTLE:
		message_bar(L"Roll left little... ", TRUE); 
		break;

	case FINGER_POSITION_RIGHT_FOUR:
		message_bar(L"Right four fingers... ", TRUE);
		break;
	case FINGER_POSITION_LEFT_FOUR:
		message_bar(L"Left four fingers... ", TRUE);
		break;
	case FINGER_POSITION_BOTH_THUMBS:
		message_bar(L"Two thumbs... ", TRUE);
		break;
	}
}

void C_FAP50_SDK_Dlg::set_video_menu(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos)
{
	if (pos == FINGER_POSITION_SOME_FINGERS)
		pos = hand_of_some_fingers;

	switch (mode) {
	case GUI_SHOW_MODE_FLAT:
		if (pos == FINGER_POSITION_RIGHT_FOUR)
			video_menu = L"right_4";
		else if (pos == FINGER_POSITION_LEFT_FOUR)
			video_menu = L"left_4";
		else if (pos == FINGER_POSITION_BOTH_THUMBS)
			video_menu = L"two_thumbs";
		break;

	case GUI_SHOW_MODE_ROLL:
		if (pos == FINGER_POSITION_RIGHT_THUMB ||
			pos == FINGER_POSITION_RIGHT_INDEX ||
			pos == FINGER_POSITION_RIGHT_MIDDLE ||
			pos == FINGER_POSITION_RIGHT_RING ||
			pos == FINGER_POSITION_RIGHT_LITTLE)
			video_menu = L"right_roll";
		else
			video_menu = L"left_roll";
		break;

	case GUI_SHOW_MODE_SIGNATURE:
		break;

	default:
		video_menu = L"standby";

	}
}

typedef union {
	struct {
		BYTE R4 : 1;
		BYTE R3 : 1;
		BYTE R2 : 1;
		BYTE R1 : 1;
	};
	struct {
		BYTE L1 : 1;
		BYTE L2 : 1;
		BYTE L3 : 1;
		BYTE L4 : 1;
	};
	struct {
		BYTE R0 : 1;
		BYTE L0 : 1;
	};
	BYTE num;
}Score2Num;

IMD_RESULT C_FAP50_SDK_Dlg::show_valid_finger(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos)
{
	IMD_RESULT res = IMD_RESULT(0);

	if (pos == FINGER_POSITION_SOME_FINGERS)//skip flat some finger 
		return res;

	ImageProperty p{mode, pos, true};
	res = get_image(p);

	Score2Num score{};
	CString str_bg_path;
	bool is_valid = true;
	switch (p.score_size) {
	case 4:
		//dbg(L"score:%d %d %d %d\n", p.score_array[0], p.score_array[1], p.score_array[2], p.score_array[3]);
		if (pos == FINGER_POSITION_RIGHT_FOUR)
		{
			score.R1 = (p.score_ver == NFIQ_V1) ? p.score_array[0] <= p.score_min : p.score_array[0] >= p.score_min;
			score.R2 = (p.score_ver == NFIQ_V1) ? p.score_array[1] <= p.score_min : p.score_array[1] >= p.score_min;
			score.R3 = (p.score_ver == NFIQ_V1) ? p.score_array[2] <= p.score_min : p.score_array[2] >= p.score_min;
			score.R4 = (p.score_ver == NFIQ_V1) ? p.score_array[3] <= p.score_min : p.score_array[3] >= p.score_min;
			
			is_valid = score.R1 & score.R2 & score.R3 & score.R4;
			str_bg_path.Format(L"%s%s%d.png", str_dir, L"/panel/RightHandPanel/Iterations/Iteration_", score.num);
		}
		else if (pos == FINGER_POSITION_LEFT_FOUR)
		{
			score.L1 = (p.score_ver == NFIQ_V1) ? p.score_array[0] <= p.score_min : p.score_array[0] >= p.score_min;
			score.L2 = (p.score_ver == NFIQ_V1) ? p.score_array[1] <= p.score_min : p.score_array[1] >= p.score_min;
			score.L3 = (p.score_ver == NFIQ_V1) ? p.score_array[2] <= p.score_min : p.score_array[2] >= p.score_min;
			score.L4 = (p.score_ver == NFIQ_V1) ? p.score_array[3] <= p.score_min : p.score_array[3] >= p.score_min;
			
			is_valid = score.L1 & score.L2 & score.L3 & score.L4;
			str_bg_path.Format(L"%s%s%d.png", str_dir, L"/panel/LeftHandPanel/Iterations/Iteration_", score.num);
		}
		break;

	case 2:
		//dbg(L"score:%d %d\n", p.score_array[0], p.score_array[1]);
		if (pos == FINGER_POSITION_BOTH_THUMBS)
		{
			score.L0 = (p.score_ver == NFIQ_V1) ? p.score_array[0] <= p.score_min : p.score_array[0] >= p.score_min;
			score.R0 = (p.score_ver == NFIQ_V1) ? p.score_array[1] <= p.score_min : p.score_array[1] >= p.score_min;

			is_valid = score.R0 & score.L0;
			str_bg_path.Format(L"%s%s%d.png", str_dir, L"/panel/ThumbsPanel/Iterations/Iteration_", score.num);
		}
		break;

	case 1:
		//dbg(L"roll score: %d\n", p.score_array[0]);
		is_valid = (p.score_ver == NFIQ_V1) ? p.score_array[0] <= p.score_min : p.score_array[0] >= p.score_min;

		switch (pos) {
		case FINGER_POSITION_RIGHT_THUMB:
		case FINGER_POSITION_RIGHT_INDEX:
		case FINGER_POSITION_RIGHT_MIDDLE:
		case FINGER_POSITION_RIGHT_RING:
		case FINGER_POSITION_RIGHT_LITTLE:
			str_bg_path =     str_dir + L"/panel/LeftRollingFingers/Iterations/RollFinger-Done.png";
			if (is_valid == false)
				str_bg_path = str_dir + L"/panel/LeftRollingFingers/Iterations/RollFinger-Retry.png";
			break;
		case FINGER_POSITION_LEFT_THUMB:
		case FINGER_POSITION_LEFT_INDEX:
		case FINGER_POSITION_LEFT_MIDDLE:
		case FINGER_POSITION_LEFT_RING:
		case FINGER_POSITION_LEFT_LITTLE:
			str_bg_path =     str_dir + L"/panel/RightRollingFingers/Iterations/RollFinger-Done.png";
			if (is_valid == false)
				str_bg_path = str_dir + L"/panel/LeftRollingFingers/Iterations/RollFinger-Retry.png";
			break;
		}
		break;
	}

	Mat score_img = imread((char*)(CT2A)str_bg_path); 
	if (score_img.rows)
	{
		if (p.score_size == 1 && is_valid)
		{
			Mat roll_img(750, 800, CV_8UC1, p.img);
			Mat resized_roll_img;
			resize(roll_img, resized_roll_img, Size(800/2, 750/2));

			Mat a = resized_roll_img;
			Mat b = score_img;

			Rect roi(200, 50, a.cols, a.rows);
			Mat b_roi = b(roi);  
			Mat mask; threshold(a, mask, 192, 255, THRESH_BINARY_INV);  
			b_roi.setTo(Scalar(0, 0, 0), mask);
			Mat merge_img = b;
			score_img = merge_img;
		}

		////////////////////////
		//guid
		Mat resizedMat;
		Size newSize((int)(score_img.cols * SCALE_PANEL_TO_GUIDE), (int)(score_img.rows * SCALE_PANEL_TO_GUIDE));
		resize(score_img, resizedMat, newSize);
		imshow("IDC_GUIDE", resizedMat);

		////////////////////////
		//panel
		Mat rotated_img;
		rotate(score_img, rotated_img, ROTATE_90_CLOCKWISE);
		vector<BYTE> jpg_buf;
		imencode(".jpg", rotated_img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
		int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());

		//dbg(L"show_valid_finger() is_valid=%d\n", is_valid);
		if(is_valid)
			Sleep(1750);
	}

	return res;
}

void C_FAP50_SDK_Dlg::play_speech(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos)
{
	CString mp3_filename;
	switch (mode) {
	case GUI_SHOW_MODE_ROLL:
		switch (pos) {
		case FINGER_POSITION_RIGHT_THUMB:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_RIGHT_THUMB_FINGER);
			break;
		case FINGER_POSITION_RIGHT_INDEX:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_RIGHT_INDEX_FINGER);
			break;
		case FINGER_POSITION_RIGHT_MIDDLE:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_RIGHT_MIDDLE_FINGER);
			break;
		case FINGER_POSITION_RIGHT_RING:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_RIGHT_RING_FINGER);
			break;
		case FINGER_POSITION_RIGHT_LITTLE:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_RIGHT_LITTLE_FINGER);
			break;
		case FINGER_POSITION_LEFT_THUMB:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_LIFT_THUMB_FINGER);
			break;
		case FINGER_POSITION_LEFT_INDEX:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_LIFT_INDEX_FINGER);
			break;
		case FINGER_POSITION_LEFT_MIDDLE:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_LIFT_MIDDLE_FINGER);
			break;
		case FINGER_POSITION_LEFT_RING:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_LIFT_RING_FINGER);
			break;
		case FINGER_POSITION_LEFT_LITTLE:
			mp3_filename.Format(L"%.5d", SPEECH_ROLLING_LIFT_LITTLE_FINGER);
			break;
		default:
			break;
		}
		break;
	case GUI_SHOW_MODE_FLAT:
		switch (pos) {
		case FINGER_POSITION_RIGHT_FOUR:
			mp3_filename.Format(L"%.5d", SPEECH_PUT_RIGHT_4_FINGERS);
			break;
		case FINGER_POSITION_LEFT_FOUR:
			mp3_filename.Format(L"%.5d", SPEECH_PUT_LEFT_4_FINGERS);
			break;
		case FINGER_POSITION_BOTH_THUMBS:
			mp3_filename.Format(L"%.5d", SPEECH_PUT_2_THUMBS_FINGERS);
			break;
		}
	default:
		break;
	}

	host_speech.play(mp3_filename+L".mp3");
}

//roll or flat only
bool C_FAP50_SDK_Dlg::sampling_finger(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos)
{
	if (sampling_sequence == SS_SIGNATURE)
		return false;

	vector<E_FINGER_POSITION> fingers{pos};
	if (sampling_sequence == SS_FLAT_SOME_FINGERS)
		fingers = dlg_sel_finger->select_finger;

	set_video_menu(mode, pos);

scan_again:
	sampling_msg(mode, pos);
	int msgbox_result = IDNO;
	IMD_RESULT res = scan_start(mode, fingers.data(), fingers.size());
	play_speech(mode, pos);

	SetTimer(TIMER_EVENT_SHOW_IMAGE, TIMER_SHOW_IMAGE, NULL);
	SetTimer(TIMER_EVENT_SHOW_PANEL, TIMER_SHOW_PANEL, NULL);

	bool need_leave_finger = true;
	while (is_scan_busy())
	{
		if (get_image_status(&img_status) != IMD_RLT_SUCCESS)
			continue;

		//show_image();//this function need to put in the timer.

		if (is_scan_by_manual)
			continue;

		if ((img_status.is_finger_on && img_status.is_roll_done)
			|| (img_status.is_finger_on && img_status.is_flat_done))
			if (need_leave_finger)
			{
				//dbg(L"play leave finger.\n");
				need_leave_finger = false;
				CString mp3_filename;
				mp3_filename.Format(L"%.5d", SPEECH_REMOVE_YOUR_FINGERS);
				if(mode == GUI_SHOW_MODE_ROLL)
					mp3_filename.Format(L"%.5d", SPEECH_REMOVE_YOUR_FINGER);
				host_speech.play(mp3_filename + L".mp3");
			}

		if ((img_status.is_finger_on == false && img_status.is_roll_done)
			|| (img_status.is_finger_on == false && img_status.is_flat_done)
			)
		{
			//dbg(L"sampling done, cancel.\n");
			scan_cancel();
		}

		//show_panel(); //this way hand on imshow(), so need to use timer;
	}

	if(is_scan_by_manual)
		cancel_sampling |= (img_status.is_finger_on==false);

	if (cancel_sampling)
	{
		host_speech.stop();
		goto func_end;
	}

	res = show_valid_finger(mode, pos);

	if (cancel_sampling)
		goto func_end;

	switch (res) {
	case IMD_RLT_PUT_WRONG_HAND:
		msgbox_result = MessageBox(L"You are using the wrong hand. \nDo you want to scan again?", L"Wrong Hand", MB_YESNO);
		if (msgbox_result == IDYES)
			goto scan_again;
		break;

	case IMD_RLT_POOR_QUALITY_AND_CANTACT_IRON:
	case IMD_RLT_POOR_NFIQ_QUALITY:
		msgbox_result = MessageBox(L"The image quality score is too low. \nDo you want to scan again?", L"Poor Quality", MB_YESNO);
		if (msgbox_result == IDYES)
			goto scan_again;
		break;

	case IMD_RLT_POOR_QUALITY_AND_WRONG_HAND:
		msgbox_result = MessageBox(L"The image quality score is too low. \nDo you want to scan again?", L"Poor Quality", MB_YESNO);
		if (msgbox_result == IDYES)
			goto scan_again;
		msgbox_result = MessageBox(L"You are using the wrong hand. \nDo you want to scan again?", L"Wrong Hand", MB_YESNO);
		if (msgbox_result == IDYES)
			goto scan_again;
		break;

	case IMD_RLT_FINGER_SHAPE_NOT_GOOD:
		msgbox_result = MessageBox(L"The fingrprint shape is not good. Try to straighten your fingers and make them into an oval shape. \nDo you want to scan again?", L"Fingrprint Shape", MB_YESNO);
		if (msgbox_result == IDYES)
			goto scan_again;
		break;
	}

func_end:
	return true;
}

void C_FAP50_SDK_Dlg::set_gui(int enable)
{
	if (enable)
	{
		is_sampling_finger_busy = true;
		if (is_video_playing)
		{
			cancel_video_play = true;
			while (is_video_playing);
		}
		KillTimer(TIMER_EVENT_STANDBY_VIDEO);
		//dbg(L"is_video_playing==false\n");

		if (!connect_fap50_panel(PANEL_IP, PANEL_PORT))
			dbg("connect_fap50_panel() == false\n");

		GetDlgItem(IDC_MFCBTN_START)->SetWindowText(L"Stop");
		mfc_btn_start.SetTextColor(_4_COLOR);
		mfc_btn_start.Invalidate();

		cb_sampling_sequence.EnableWindow(false);
		cb_nfiq_ver.EnableWindow(false);
		cb_finger_position.EnableWindow(false);
		GetDlgItem(IDC_CB_LANG)->EnableWindow(false);
		GetDlgItem(IDC_CB_VOL)->EnableWindow(false);
		GetDlgItem(IDC_CB_back_light)->EnableWindow(false);
		GetDlgItem(IDC_BTN_show_video)->EnableWindow(false);	
		GetDlgItem(IDC_GUIDE)->ShowWindow(SW_NORMAL);

		switch (sampling_sequence) {
		case  SS_FLAT_442:
			//int btns[]{ IDC_SAMPLING, IDC_BTN_L4, IDC_BTN_R4, IDC_BTN_TWO_TB };
			for (auto& id : { IDC_SAMPLING })
				GetDlgItem(id)->ShowWindow(SW_NORMAL);
			break;

		case SS_FLAT_442R:
			//int btns[] { IDC_SAMPLING, IDC_BTN_L4, IDC_BTN_R4, IDC_BTN_TWO_TB,
			//	IDC_BTN_L_1, IDC_BTN_L_2, IDC_BTN_L_3, IDC_BTN_L_4, IDC_BTN_L_5,
			//	IDC_BTN_R_1, IDC_BTN_R_2, IDC_BTN_R_3, IDC_BTN_R_4, IDC_BTN_R_5,
			//};
			for (auto& id : { IDC_SAMPLING })
				GetDlgItem(id)->ShowWindow(SW_NORMAL);
			break;

		case SS_FLAT_SOME_FINGERS:
			for (auto& id : { IDC_SAMPLING })
				GetDlgItem(id)->ShowWindow(SW_NORMAL);
			break;

		case SS_SIGNATURE_BY_PEN:
		case SS_SIGNATURE:
			for (auto& id : { IDC_SAMPLING, IDC_BTN_ERASE_SING, IDC_BTN_SIGN_DONE })
				GetDlgItem(id)->ShowWindow(SW_NORMAL);
			GetDlgItem(IDC_GUIDE)->ShowWindow(SW_HIDE);
			break;
		}

		for (auto& id : main_ui_ids)
			GetDlgItem(id)->ShowWindow(SW_HIDE);
	}
	else if (enable == false)// && org_enable != enable)
	{
		is_sampling_finger_busy = false;
		video_menu = L"standby";
		SetTimer(TIMER_EVENT_STANDBY_VIDEO, TIMER_SHOW_VIDEO, NULL);
		//dbg(L"SetTimer(TIMER_EVENT_SHOW_VIDEO)\n");

		set_led_speech_standby_mode();
		list_image();

		disconnect_fap50_panel();

		GetDlgItem(IDC_MFCBTN_START)->SetWindowText(L"Start");
		mfc_btn_start.SetTextColor(BLACK_COLOR);
		mfc_btn_start.Invalidate();

		//if (is_sampled)
		{
			mfc_btn_save_file.EnableWindow();
			mfc_btn_save_file.SetTextColor(BLACK_COLOR);
			mfc_btn_save_file.Invalidate();
		}

		cb_sampling_sequence.EnableWindow();
		cb_nfiq_ver.EnableWindow();
		cb_finger_position.EnableWindow();
		GetDlgItem(IDC_CB_LANG)->EnableWindow();
		GetDlgItem(IDC_CB_VOL)->EnableWindow();
		GetDlgItem(IDC_CB_back_light)->EnableWindow();
		GetDlgItem(IDC_BTN_show_video)->EnableWindow();
		GetDlgItem(IDC_GUIDE)->ShowWindow(SW_HIDE);

		for (auto& id : sampling_ui_ids)
			GetDlgItem(id)->ShowWindow(SW_HIDE);
		for (auto& id : main_ui_ids)
			GetDlgItem(id)->ShowWindow(SW_NORMAL);
	}

	if (is_app_close)
	{
		//dbg(L"is_close==true\n");
		//CDialogEx::OnClose();

		OnCancel();
	}

}

void C_FAP50_SDK_Dlg::init_list()
{
	int width, height;
	list = Mat(750 * 2 + 1000 + 4 * FIGURE_BORDER_SIZE, 800 * 5 + 6 * FIGURE_BORDER_SIZE, CV_8UC1, Scalar(0));
	Mat	mat_finger,
		blank_400x500{ 1000, 400, CV_8UC1, Scalar(255) },
		blank_800x750{ 750, 800, CV_8UC1, Scalar(255) },
		blank_1600x1000{ 1000, 1600, CV_8UC1, Scalar(255) };
	long thumb_width = (FIGURE_BORDER_SIZE * 6 + 800 * 5 - 1600 * 2 - FIGURE_BORDER_SIZE * 5) / 2;

	E_GUI_SHOW_MODE show_mode;
	switch (sampling_sequence) {
	case SS_FLAT_442:
		show_mode = GUI_SHOW_MODE_FLAT;
		break;
	case SS_FLAT_442R:
		show_mode = GUI_SHOW_MODE_ROLL;
		break;
	}

	// 设置文本的字体
	int fontFace = cv::FONT_HERSHEY_SIMPLEX;

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1 + 800 * 0, FIGURE_BORDER_SIZE, width, height)));
	putText(list, "1. R. THUMB", cv::Point(FIGURE_BORDER_SIZE * 1 + 800 * 0 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 800 * 1, FIGURE_BORDER_SIZE, width, height)));
	putText(list, "2. R. INDEX", cv::Point(FIGURE_BORDER_SIZE * 2 + 800 * 1 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 800 * 2, FIGURE_BORDER_SIZE, width, height)));
	putText(list, "3. R. MIDDLE", cv::Point(FIGURE_BORDER_SIZE * 3 + 800 * 2 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 800 * 3, FIGURE_BORDER_SIZE, width, height)));
	putText(list, "4. R. RING", cv::Point(FIGURE_BORDER_SIZE * 4 + 800 * 3 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 5 + 800 * 4, FIGURE_BORDER_SIZE, width, height)));
	putText(list, "5. R. LITTLE", cv::Point(FIGURE_BORDER_SIZE * 5 + 800 * 4 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	////
	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1 + 800 * 0, FIGURE_BORDER_SIZE * 2 + 750, width, height)));
	putText(list, "6. L. THUMB", cv::Point(FIGURE_BORDER_SIZE * 1 + 800 * 0 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
 
	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 800 * 1, FIGURE_BORDER_SIZE * 2 + 750, width, height)));
	putText(list, "7. L. INDEX", cv::Point(FIGURE_BORDER_SIZE * 2 + 800 * 1 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 800 * 2, FIGURE_BORDER_SIZE * 2 + 750, width, height)));
	putText(list, "8. L. MIDDLE", cv::Point(FIGURE_BORDER_SIZE * 3 + 800 * 2 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 800 * 3, FIGURE_BORDER_SIZE * 2 + 750, width, height)));
	putText(list, "9. L. RING", cv::Point(FIGURE_BORDER_SIZE * 4 + 800 * 3 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_800x750; width = blank_800x750.cols; height = blank_800x750.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 5 + 800 * 4, FIGURE_BORDER_SIZE * 2 + 750, width, height)));
	putText(list, "10. L. LITTLE", cv::Point(FIGURE_BORDER_SIZE * 5 + 800 * 4 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	////
	mat_finger = blank_1600x1000; width = mat_finger.cols; height = mat_finger.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1, FIGURE_BORDER_SIZE * 3 + 750 * 2, width, height)));
	putText(list, "LEFT FOUR FINGERS", cv::Point(FIGURE_BORDER_SIZE * 1 + FOUR_FINGER_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = Mat(1000, 405, CV_8UC1, Scalar(255)); width = mat_finger.cols; height = mat_finger.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 1600, FIGURE_BORDER_SIZE * 3 + 750 * 2, width, height)));
	putText(list, "L. THUMB", cv::Point(FIGURE_BORDER_SIZE * 2 + 1600 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_400x500; width = mat_finger.cols; height = mat_finger.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 1600 + thumb_width, FIGURE_BORDER_SIZE * 3 + 750 * 2, width, height)));
	putText(list, "R. THUMB", cv::Point(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	mat_finger = blank_1600x1000; width = mat_finger.cols; height = mat_finger.rows;
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2, FIGURE_BORDER_SIZE * 3 + 750 * 2, width, height)));
	putText(list, "RIGHT FOUR FINGERS", cv::Point(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2 + FOUR_FINGER_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);


	// 确定缩放比例
	cv::Size newSize((int)(list.cols * LIST_SCALE), (int)(list.rows * LIST_SCALE));
	Mat resizedMat; resize(list, resizedMat, newSize);
	imshow("IDC_FINGER_IMAGES", resizedMat);
}

void C_FAP50_SDK_Dlg::list_image(E_FINGER_POSITION finger_position)
{
	init_list();

	Mat mat_finger;
	long thumb_width = (FIGURE_BORDER_SIZE * 6 + 800 * 5 - 1600 * 2 - FIGURE_BORDER_SIZE * 5) / 2;

	E_GUI_SHOW_MODE show_mode;
	long offset_x = 0, offset_y = 0;
	switch (sampling_sequence) {
	case SS_SIGNATURE:
	case SS_SIGNATURE_BY_PEN:
	case SS_FLAT_SOME_FINGERS:
	case SS_FLAT_442:
		show_mode = GUI_SHOW_MODE_FLAT;
		offset_x = 200, offset_y = 125;
		break;
	case SS_FLAT_442R:
		show_mode = GUI_SHOW_MODE_ROLL;
		break;
	}

	// 设置文本的字体
	int fontFace = cv::FONT_HERSHEY_SIMPLEX;

	ImageProperty p;
	p.mode = show_mode;
	CString score_text;

	p.pos = FINGER_POSITION_RIGHT_THUMB;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1 + 800 * 0+ offset_x, FIGURE_BORDER_SIZE+ offset_y, p.width, p.height)));
	if(p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 1 + 800 * 0 + SCORE_OFFSET, FIGURE_BORDER_SIZE + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "1. R. THUMB", cv::Point(FIGURE_BORDER_SIZE * 1 + 800 * 0 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_RIGHT_INDEX;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 800 * 1+ offset_x, FIGURE_BORDER_SIZE+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 2 + 800 * 1 + SCORE_OFFSET, FIGURE_BORDER_SIZE + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "2. R. INDEX", cv::Point(FIGURE_BORDER_SIZE * 2 + 800 * 1 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_RIGHT_MIDDLE;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 800 * 2+ offset_x, FIGURE_BORDER_SIZE+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 3 + 800 * 2 + SCORE_OFFSET, FIGURE_BORDER_SIZE + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "3. R. MIDDLE", cv::Point(FIGURE_BORDER_SIZE * 3 + 800 * 2 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_RIGHT_RING;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 800 * 3+ offset_x, FIGURE_BORDER_SIZE+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 4 + 800 * 3 + SCORE_OFFSET, FIGURE_BORDER_SIZE + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "4. R. RING", cv::Point(FIGURE_BORDER_SIZE * 4 + 800 * 3 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_RIGHT_LITTLE;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 5 + 800 * 4+ offset_x, FIGURE_BORDER_SIZE+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 5 + 800 * 4 + SCORE_OFFSET, FIGURE_BORDER_SIZE + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "5. R. LITTLE", cv::Point(FIGURE_BORDER_SIZE * 5 + 800 * 4 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

////
	p.pos = FINGER_POSITION_LEFT_THUMB;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1 + 800 * 0+ offset_x, FIGURE_BORDER_SIZE * 2 + 750+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 1 + 800 * 0 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "6. L. THUMB", cv::Point(FIGURE_BORDER_SIZE * 1 + 800 * 0 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_LEFT_INDEX;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 800 * 1+ offset_x, FIGURE_BORDER_SIZE * 2 + 750+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 2 + 800 * 1 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "7. L. INDEX", cv::Point(FIGURE_BORDER_SIZE * 2 + 800 * 1 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_LEFT_MIDDLE;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 800 * 2+ offset_x, FIGURE_BORDER_SIZE * 2 + 750+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 3 + 800 * 2 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "8. L. MIDDLE", cv::Point(FIGURE_BORDER_SIZE * 3 + 800 * 2 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_LEFT_RING;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 800 * 3+ offset_x, FIGURE_BORDER_SIZE * 2 + 750+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 4 + 800 * 3 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "9. L. RING", cv::Point(FIGURE_BORDER_SIZE * 4 + 800 * 3 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	p.pos = FINGER_POSITION_LEFT_LITTLE;
	get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
	mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 5 + 800 * 4+ offset_x, FIGURE_BORDER_SIZE * 2 + 750+ offset_y, p.width, p.height)));
	if (p.score_array[0])
	{
		score_text.Format(L"%d", p.score_array[0]);
		putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 5 + 800 * 4 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	}
	putText(list, "10. L. LITTLE", cv::Point(FIGURE_BORDER_SIZE * 5 + 800 * 4 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 2 + 750 + ROLL_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

////
	if (sampling_sequence == SS_SIGNATURE
		|| sampling_sequence == SS_SIGNATURE_BY_PEN)
	{
		///////////////////////
		// show signature
		p.mode = GUI_SHOW_MODE_FLAT;
		p.pos = FINGER_POSITION_SIGNATURE;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1, FIGURE_BORDER_SIZE * 3 + 750 * 2, mat_finger.cols, mat_finger.rows)));
		putText(list, "Signature", cv::Point(FIGURE_BORDER_SIZE * 1 + FOUR_FINGER_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
	
		///////////////////////
		// clear text
		p.pos = FINGER_POSITION_LEFT_THUMB;
		get_image(p); mat_finger = Mat(1000, p.width, CV_8UC1, Scalar(255));
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 1600, FIGURE_BORDER_SIZE * 3 + 750 * 2, mat_finger.cols, mat_finger.rows)));

		p.pos = FINGER_POSITION_RIGHT_THUMB;
		get_image(p); mat_finger = Mat(1000, p.width, CV_8UC1, Scalar(255));
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 1600 + thumb_width, FIGURE_BORDER_SIZE * 3 + 750 * 2, mat_finger.cols, mat_finger.rows)));

		p.pos = FINGER_POSITION_RIGHT_FOUR;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, Scalar(255));
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2, FIGURE_BORDER_SIZE * 3 + 750 * 2, mat_finger.cols, mat_finger.rows)));
	}
	else if (sampling_sequence == SS_FLAT_SOME_FINGERS)
	{
		p.mode = GUI_SHOW_MODE_FLAT;
		p.pos = FINGER_POSITION_SOME_FINGERS;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, p.height)));
		if (p.score_array[0])
		{
			score_text = L"";
			for(int i=0; i< dlg_sel_finger->select_finger.size(); i++)
			{
				int score = p.score_array[i];
				if (i == 0)
					score_text.AppendFormat(L"%d", score);
				else
					score_text.AppendFormat(L"-%d", score);
			}

			putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 1 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
		}
		putText(list, "Flat Some Fingers", cv::Point(FIGURE_BORDER_SIZE * 1 + FOUR_FINGER_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

		p.pos = FINGER_POSITION_LEFT_THUMB;
		get_image(p); mat_finger = Mat(1000, p.width, CV_8UC1).setTo(255);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 1600, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, 1000)));

		p.pos = FINGER_POSITION_RIGHT_THUMB;
		get_image(p); mat_finger = Mat(1000, p.width, CV_8UC1).setTo(255);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 1600 + thumb_width, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, 1000)));

		p.pos = FINGER_POSITION_RIGHT_FOUR;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, p.height)));
	}
	else
	{
		p.mode = GUI_SHOW_MODE_FLAT;
		p.pos = FINGER_POSITION_LEFT_FOUR;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 1,							FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, p.height)));
		if (p.score_array[0])
		{
			score_text.Format(L"%d-%d-%d-%d", p.score_array[3], p.score_array[2], p.score_array[1], p.score_array[0]);
			putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 1 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
		}
		putText(list, "LEFT FOUR FINGERS",				cv::Point(FIGURE_BORDER_SIZE * 1 + FOUR_FINGER_X_OFFSET,	FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

		p.pos = FINGER_POSITION_LEFT_THUMB;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		//list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 1600, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, p.height)).setTo(Scalar(255));
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 2 + 1600,					FIGURE_BORDER_SIZE * 3 + 750 * 2+250, p.width, p.height)));
		if (p.score_array[0])
		{
			score_text.Format(L"%d", p.score_array[0]);
			putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 2 + 1600 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
		}
		putText(list, "L. THUMB",						cv::Point(FIGURE_BORDER_SIZE * 2 + 1600 + FINGER_TEXT_X_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

		p.pos = FINGER_POSITION_RIGHT_THUMB;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		//list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, p.height)).setTo(Scalar(255));
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 3 + 1600 + thumb_width,	FIGURE_BORDER_SIZE * 3 + 750 * 2+250, p.width, p.height)));
		if (p.score_array[0])
		{
			score_text.Format(L"%d", p.score_array[0]);
			putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width + SCORE_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
		}
		putText(list, "R. THUMB",						cv::Point(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width + FINGER_TEXT_X_OFFSET,	FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

		p.pos = FINGER_POSITION_RIGHT_FOUR;
		get_image(p); mat_finger = Mat(p.height, p.width, CV_8UC1, p.img);
		mat_finger.copyTo(list(cv::Rect(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2, FIGURE_BORDER_SIZE * 3 + 750 * 2, p.width, p.height)));
		if (p.score_array[0])
		{
			score_text.Format(L"%d-%d-%d-%d", p.score_array[0], p.score_array[1], p.score_array[2], p.score_array[3]);
			putText(list, (std::string)CT2A(score_text), cv::Point(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2 + SCORE_OFFSET, FIGURE_BORDER_SIZE * 3 + 750 * 2 + SCORE_OFFSET), fontFace, 2, Scalar(0), THICKNESS);
		}
		putText(list, "RIGHT FOUR FINGERS",				cv::Point(FIGURE_BORDER_SIZE * 4 + 1600 + thumb_width * 2 + FOUR_FINGER_X_OFFSET,	FIGURE_BORDER_SIZE * 3 + 750 * 2 + FLAT_FINGER_TEXT_Y_OFFSET), fontFace, 2, Scalar(0), THICKNESS);

	}

	cv::Size newSize((int)(list.cols * LIST_SCALE), (int)(list.rows * LIST_SCALE));
	Mat resizedMat; resize(list, resizedMat, newSize);

	imshow("IDC_FINGER_IMAGES", resizedMat);
}

BOOL get_save_name(CString& file_name, CWnd* parent_win)
{
	BOOL ret = FALSE;

	//set file name for saving.
	CTime t = CTime::GetCurrentTime();
	CString default_file_name = t.Format(L"%Y%m%d_%H%M%S");
	CString filter;
	filter += L"PNG Files (*.png) | *.png|";
	filter += L"BMP Files (*.bmp) | *.bmp|";
	filter += L"WSQ Files (*.wsq) | *.wsq|";
	filter += L"JPEG2000 Files (*.jp2) | *.jp2|";
	filter += L"RAW Files (*.raw) | *.raw|";
	filter += L"ISO 19794-4 Files (*.fir) | *.fir|";
	filter += L"PGM Files (*.pgm) | *.pgm|";

	CFileDialog dlg(FALSE, _T(""), default_file_name,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, parent_win);

	if (IDOK != dlg.DoModal())
		goto func_end;
	file_name = dlg.GetPathName();
	ret = TRUE;

func_end:
	return ret;
}

void C_FAP50_SDK_Dlg::flat_442()
{
	if (!reset())
		goto func_end;

	set_gui(true);

//flat_442 start
	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_FOUR);
	if (cancel_sampling) 
		goto func_end;
	
	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_FOUR);
	if (cancel_sampling) 
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_BOTH_THUMBS);
	if (cancel_sampling) 
		goto func_end;

	message_bar(L"Sampling Done.\n", TRUE);

func_end:
	set_gui(false);
}

void C_FAP50_SDK_Dlg::flat_442_roll()
{
	if (!reset())
		goto func_end;

	set_gui(true);

//////////////////////////////////////////
	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_FOUR);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_FOUR);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_BOTH_THUMBS);
	if (cancel_sampling)
		goto func_end;

//////////////////////////////////////////
	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_THUMB);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_INDEX);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_MIDDLE);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_RING);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_LITTLE);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_THUMB);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_INDEX);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_MIDDLE);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_RING);
	if (cancel_sampling)
		goto func_end;

	sampling_finger(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_LITTLE);
	if (cancel_sampling)
		goto func_end;

	message_bar(L"Sampling Done.\n", true);

func_end:
	set_gui(false);
}

void C_FAP50_SDK_Dlg::flat_some_fingers()
{
	dlg_sel_finger->show_window(TRUE);

	while (dlg_sel_finger->show_en) Sleep(1);

	vector<E_FINGER_POSITION> fingers = dlg_sel_finger->select_finger;
	if (fingers.size() > 4 || fingers.size() < 1)
		goto func_end;

	hand_of_some_fingers = FINTER_POSITION_UNKNOW_FINGER;
	for (auto& pos : fingers)
	{
		switch (pos) {
		case FINGER_POSITION_RIGHT_THUMB:
		case FINGER_POSITION_LEFT_THUMB:
			hand_of_some_fingers = FINGER_POSITION_BOTH_THUMBS;
			break;
		case FINGER_POSITION_RIGHT_INDEX:
		case FINGER_POSITION_RIGHT_MIDDLE:
		case FINGER_POSITION_RIGHT_RING:
		case FINGER_POSITION_RIGHT_LITTLE:
			hand_of_some_fingers = FINGER_POSITION_RIGHT_FOUR;
			break;
		case FINGER_POSITION_LEFT_INDEX:
		case FINGER_POSITION_LEFT_MIDDLE:
		case FINGER_POSITION_LEFT_RING:
		case FINGER_POSITION_LEFT_LITTLE:
			hand_of_some_fingers = FINGER_POSITION_LEFT_FOUR;
			break;
		}

		if (hand_of_some_fingers == FINTER_POSITION_UNKNOW_FINGER)
			break;
	}

	//if (!reset())
	//	goto func_end;

	set_gui(true);

	sampling_finger(GUI_SHOW_MODE_FLAT, FINGER_POSITION_SOME_FINGERS);
	if (cancel_sampling)
		goto func_end;

	message_bar(L"Sampling Done.\n", true);

func_end:
	set_gui(false);
}

void C_FAP50_SDK_Dlg::sign()
{
	if (!reset())
		return;

	set_gui(true);

//#define USE_SAMPLING_FINGER
#ifdef USE_SAMPLING_FINGER 
	sampling_finger(GUI_SHOW_MODE_SIGNATURE, FINGER_POSITION_SIGNATURE);//this way not good
	if (cancel_sampling)
		goto func_end;

#else 
	sign_jpgs.clear();
	vector<E_FINGER_POSITION> fingers{ FINGER_POSITION_SIGNATURE };
	IMD_RESULT res = scan_start(GUI_SHOW_MODE_SIGNATURE, fingers.data(), fingers.size());
	if (res)
	{
		message_bar(L"fail", true);
		if(res == IMD_RLT_NOT_SUPPORT)
			message_bar(L"The feature not support this device.", true);

		set_gui(false);
		return;
	}

	SetTimer(TIMER_EVENT_SHOW_IMAGE, TIMER_SHOW_IMAGE, NULL);

	//--- show_panel() with timer on signature that is not good.
	//SetTimer(TIMER_EVENT_SHOW_PANEL, TIMER_SHOW_SIGNATURE, NULL);

	while (is_scan_busy())
	{
		if (get_image_status(&img_status) != IMD_RLT_SUCCESS)
			continue;

		//show_image(); //this way, will hang on imshow();
		show_panel();
	}

#ifdef SIGN_RECODE
	int jpg_size = sign_jpgs.size();
	dbg(L"sign jpg size=%d\n", jpg_size);

	CString list_filename = str_dir + "/sign/list.txt";
	ofstream list_file(list_filename, ios::out | ios::trunc);
	if (!list_file.is_open()) 
	{
		dbg(L"can open file: list.txt\n");
		return;
	}

	for (size_t i = 0; i < sign_jpgs.size(); ++i)
	{
		CString file_name, file_path;
		file_name.Format(L"sign_%.4d.jpg", i);
		file_path = str_dir + L"/sign/" + file_name;

		ofstream outfile(file_path, ios::binary);
		if (!outfile.is_open()) 
		{
			dbg(L"can open file to write: %s\n", file_name);
			return;
		}

		outfile.write(reinterpret_cast<const char*>(sign_jpgs[i].data()), sign_jpgs[i].size());
		outfile.close();

		list_file << (CT2A)file_name << endl;
	}

	list_file << "EOF" << endl;
#endif

#endif

func_end:
	message_bar(L"", true);
	set_gui(false);
}

void C_FAP50_SDK_Dlg::sign_by_pen()
{
	if (!reset())
		return;

	set_gui(true);

	vector<E_FINGER_POSITION> fingers{ FINGER_POSITION_SIGNATURE };
	IMD_RESULT res = scan_start(GUI_SHOW_MODE_SIGNATURE_BY_PEN, fingers.data(), fingers.size());
	if (res)
	{
		message_bar(L"fail", true);
		if (res == IMD_RLT_NOT_SUPPORT)
			message_bar(L"The feature not support this device.", true);

		//set_gui(false);
		//return;
		goto func_end;
	}

	SetTimer(TIMER_EVENT_SHOW_IMAGE, TIMER_SHOW_IMAGE, NULL);//for dialog

	while (is_scan_busy())
	{
		if (get_image_status(&img_status) != IMD_RLT_SUCCESS)
			continue;

		show_panel();//for device panel
		Sleep(25);//40fps
	}

	if(cancel_sampling)
		goto func_end;

	message_bar(L"", true);

func_end:
	set_gui(false);
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnStart()
{
	//dbg(L"OnBnClickedBtnStart \n");
	if (is_sampling_finger_busy)//is_scan_busy())
	{
		if (cancel_sampling)
			return;

		if(is_scan_by_manual == false)
			cancel_sampling = true;

		scan_cancel();
		message_bar(L"Canceled.\n");
		return;
	}

//start sampling
	Mat init_img{ 1000, 1600, CV_8UC1, Scalar(255) };
	imshow("IDC_SAMPLING", init_img);
	imshow("IDC_GUIDE", init_img);

	switch (sampling_sequence) {
	case SS_FLAT_442:
		thread_member(C_FAP50_SDK_Dlg::flat_442);
		return;
	case SS_FLAT_442R:
		thread_member(C_FAP50_SDK_Dlg::flat_442_roll);
		return;
	case SS_FLAT_SOME_FINGERS:
		thread_member(C_FAP50_SDK_Dlg::flat_some_fingers);
		return;
	case SS_SIGNATURE:
		thread_member(C_FAP50_SDK_Dlg::sign); 
		return;
	case SS_SIGNATURE_BY_PEN:
		thread_member(C_FAP50_SDK_Dlg::sign_by_pen);
		return;
	}
}

void C_FAP50_SDK_Dlg::show_panel() 
{
	static bool thread_busy = false;
	static ifstream list_file;
	static CString org_video_menu;
	static E_FINGER_POSITION org_pos = FINTER_POSITION_UNKNOW_FINGER;
	static int wait_time = 0;

	if (thread_busy)
	{
		dbg("thread_busy\n");
		return;
	}
	thread_busy = true;

	E_GUI_SHOW_MODE show_mode = img_status.show_mode;

	once_for
	{
		Mat img;
		string filename;
		vector<BYTE> jpg_buf;
		E_FINGER_POSITION pos = img_status.finger_position;// m_property.finger_position;

		if (pos == FINGER_POSITION_SOME_FINGERS)
			pos = hand_of_some_fingers;

		if (org_pos != pos)
		{
			org_pos = pos;
			wait_time = 0;

			CString list_name = str_dir + L"/panel/video/" + video_menu + L"/list.txt";
			list_file = ifstream(list_name);
			org_video_menu = video_menu;
		}

		if (   pos == FINGER_POSITION_RIGHT_FOUR
			|| pos == FINGER_POSITION_LEFT_FOUR
			|| pos == FINGER_POSITION_BOTH_THUMBS
			)
		{
			if (img_status.is_flat_done)
			{
				org_pos = FINTER_POSITION_UNKNOW_FINGER;
				break;
			}

			if (img_status.is_finger_on)
			{
				Mat scan_img = scanning_img[pos].clone();
				static int idx = 0;
				Mat loading = loading_img[(idx++ / 8) % 3 + 1];
				loading.copyTo(scan_img(Rect(260, 48, loading.cols, loading.rows)));

				////////////////////////
				//guid
				Mat resizedMat;
				Size newSize(scan_img.cols * SCALE_PANEL_TO_GUIDE, scan_img.rows * SCALE_PANEL_TO_GUIDE);
				resize(scan_img, resizedMat, newSize);
				imshow("IDC_GUIDE", resizedMat);

				////////////////////////
				//panel
				Mat rotated_img;
				rotate(scan_img, rotated_img, ROTATE_90_CLOCKWISE);
				imencode(".jpg", rotated_img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
				int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());
				break;
			}

			if (getline(list_file, filename))
			{
				if (filename == "EOF" || org_video_menu != video_menu)
				{
					org_pos = FINTER_POSITION_UNKNOW_FINGER;
					break;
				}

				CString str_jpg_file = CString(filename.c_str());
				str_jpg_file = str_dir + L"/panel/video/" + video_menu + L"/" + str_jpg_file;
				img = imread((char*)(CT2A)str_jpg_file);
				if (img.rows == 0)
					break;
				imencode(".jpg", img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
				int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());
			}

			Mat resizedMat;
			Size newSize(img.cols * SCALE_PANEL_TO_GUIDE, img.rows * SCALE_PANEL_TO_GUIDE);
			if (img.rows == 0)
				break;
			resize(img, resizedMat, newSize);
			Mat rotated_img;
			rotate(resizedMat, rotated_img, ROTATE_90_COUNTERCLOCKWISE);
			imshow("IDC_GUIDE", rotated_img);
		}
		else if (show_mode == GUI_SHOW_MODE_ROLL)
		{
			if (img_status.is_roll_done)
			{
				org_pos = FINTER_POSITION_UNKNOW_FINGER;
				break;
			}

			if (img_status.is_finger_on)
			{
				int roi_offset = 0;
				if (pos == FINGER_POSITION_LEFT_THUMB ||
					pos == FINGER_POSITION_LEFT_INDEX ||
					pos == FINGER_POSITION_LEFT_MIDDLE ||
					pos == FINGER_POSITION_LEFT_RING ||
					pos == FINGER_POSITION_LEFT_LITTLE)
					roi_offset = 600;

				Mat src(1000, 1600, CV_8UC1, img_status.img);

				Mat roll_img = src(Rect(425, 150, 750, 750));
				Mat resized_roll_img;
				resize(roll_img, resized_roll_img, Size(192, 188));
				
				Mat a = resized_roll_img;
				Mat b = scanning_img[pos].clone();
				Mat mask; threshold(a, mask, 192, 255, THRESH_BINARY_INV);  
				b(Rect(roi_offset, 150, a.cols, a.rows)).setTo(Scalar(0, 0, 0), mask);

				static int idx = 0;
				Mat loading = loading_img[(idx++ / 8) % 3 + 1];
				loading.copyTo(b(Rect(260, 48, loading.cols, loading.rows)));

				////////////////////////
				//guide
				Mat merge_img = b;
				Mat resizedMat;
				Size newSize(merge_img.cols * SCALE_PANEL_TO_GUIDE, merge_img.rows * SCALE_PANEL_TO_GUIDE);
				resize(merge_img, resizedMat, newSize);
				imshow("IDC_GUIDE", resizedMat);

				if (wait_time)
					wait_time--;

				if (wait_time % 3)
					break;

				////////////////////////
				//panel
				
				//float scale = 450.0 / merge_img.rows;
				//Mat resized_panel;
				//newSize = Size((int)(merge_img.cols * scale), (int)(merge_img.rows * scale));
				//resize(merge_img, resized_panel, newSize);

				//src = Mat(480, 800, CV_8UC3, Scalar(255));
				////resized_panel.copyTo(src(Rect((800 - resized_panel.cols) / 2, 0, resized_panel.cols, resized_panel.rows)));
				//resized_panel.copyTo(src(Rect(16, 0, resized_panel.cols, resized_panel.rows)));

				Mat rotated_img;
				rotate(merge_img, rotated_img, ROTATE_90_CLOCKWISE);
				imencode(".jpg", rotated_img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
				int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());

				if (wait_time == 0)
					wait_time = 10;
				break;
			}
	
			////////////////////////
			//panel
			if (getline(list_file, filename))
			{
				if (filename == "EOF" || org_video_menu != video_menu)
				{
					org_pos = FINTER_POSITION_UNKNOW_FINGER;
					break;
				}

				CString str_jpg_file = CString(filename.c_str());
				str_jpg_file = str_dir + L"/panel/video/" + video_menu + L"/" + str_jpg_file;
				img = imread((char*)(CT2A)str_jpg_file);
				if (img.rows == 0)
					break;
				imencode(".jpg", img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
				int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());
			}

			////////////////////////
			//guide
			Mat resizedMat;
			Size newSize(img.cols * SCALE_PANEL_TO_GUIDE, img.rows * SCALE_PANEL_TO_GUIDE);
			if (img.rows == 0)
				break;
			resize(img, resizedMat, newSize);
			Mat rotated_img;
			rotate(resizedMat, rotated_img, ROTATE_90_COUNTERCLOCKWISE);
			imshow("IDC_GUIDE", rotated_img);
		}
		else if (show_mode == GUI_SHOW_MODE_SIGNATURE_BY_PEN)
		{
			//Shrink the signature to the size of the panel.
			Mat src(1000, 1600, CV_8UC1, img_status.img);
			Mat resize_img;
			resize(src, resize_img, Size(768, 480));
			src = Mat(480, 800, CV_8UC1, Scalar(255));
			resize_img.copyTo(src(Rect(16, 0, resize_img.cols, resize_img.rows)));

			//Overlay the signature on the background image.
			Mat a = src;
			Mat b = scanning_img[FINGER_POSITION_SIGNATURE].clone();
			Mat mask; threshold(a, mask, 192, 255, THRESH_BINARY_INV);
			b.setTo(Scalar(0, 0, 0), mask);

			//Rotate the image and send the jpg to the panel.
			Mat rotated_img;
			rotate(b, rotated_img, ROTATE_90_CLOCKWISE);
			imencode(".jpg", rotated_img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
			int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());
		}
		else if (pos == FINGER_POSITION_SIGNATURE )
		{
			//Shrink the signature to the size of the panel.
			Mat src(1000, 1600, CV_8UC1, img_status.img);
			Mat resize_img; 
			resize(src, resize_img, Size(768, 480));
			src = Mat(480, 800, CV_8UC1, Scalar(255));
			resize_img.copyTo(src(Rect(16, 0, resize_img.cols, resize_img.rows)));

			//Overlay the signature on the background image.
			Mat a = src;
			Mat b = scanning_img[FINGER_POSITION_SIGNATURE].clone();
			Mat mask; threshold(a, mask, 192, 255, THRESH_BINARY_INV);
			b.setTo(Scalar(0, 0, 0), mask);
			
			//Rotate the image and send the jpg to the panel.
			Mat rotated_img;
			rotate(b, rotated_img, ROTATE_90_CLOCKWISE);
			imencode(".jpg", rotated_img, jpg_buf, { IMWRITE_JPEG_QUALITY, 60 });
			int ret = send_jpg_fap50_panel(jpg_buf.data(), jpg_buf.size());

#ifdef SIGN_RECODE
			sign_jpgs.push_back(jpg_buf);
#endif
		}

	}

	thread_busy = false;
}

deque<CString> split_string(const CString& str, const CString& delimiter)
{
	deque<CString> tokens;
	int start = 0;
	CString token = str.Tokenize(delimiter, start);

	while (token != _T("")) {
		tokens.push_back(token);
		token = str.Tokenize(delimiter, start);
	}

	return tokens;
}

void C_FAP50_SDK_Dlg::show_video()
{
	static int cnt = 0; //dbg("show_video() in %d\n", cnt++);

	if (is_video_playing || is_scan_busy())
	{
		if (is_video_playing)
			dbg("is_video_playing\n");
		else
			dbg("is_scan_busy()\n");

		return;
	}

	if (!connect_fap50_panel(PANEL_IP, PANEL_PORT)) 
	{
		//message_bar(L"Can not connect the panel. Maybe not support.", true);
		return;
	}

	is_video_playing = true;
	cancel_video_play = false;

	int play_index = 0;
	//deque<CString> video_menus = {L"rossi_preview", L"all_flow_with_text", L"signature"};
	CString str_video_menus = ini.get_value(L"video_menus", L"Main");
	deque<CString> video_menus = split_string(str_video_menus, L",");

play_again:
	video_menu = video_menus[play_index++ % video_menus.size()];
	CString org_video_menu = video_menu;
	CString list_name = str_dir + L"/panel/video/" + video_menu + L"/list.txt";
	ifstream list_file(list_name);
	string filename;
	while (getline(list_file, filename))
	{
		if (filename == "EOF")
		{
			if (video_menu == L"all_flow_with_text")
				Sleep(1000);
			else if (video_menu == L"signature")
				Sleep(3000);

			goto play_again;
		}

		if (cancel_video_play || org_video_menu != video_menu)
			break;

		//dbg(L"play standby video \n");

		CString str_jpg = CString(filename.c_str());
		str_jpg = str_dir + L"/panel/video/" + video_menu + L"/" + str_jpg;
		ifstream file(str_jpg, ios::binary | ios::ate);
		if (!file) {
			dbg("Could not open the file!\n");
			break;
		}

		streamsize file_size = file.tellg();
		file.seekg(0, ios::beg);
		vector<BYTE> jpg(file_size);
		if (!file.read(reinterpret_cast<char*>(jpg.data()), file_size)) {
			dbg("Error reading the file\n");
			break;
		}
		if (!send_jpg_fap50_panel(jpg.data(), jpg.size()))
			break;
		file.close();

		if(video_menu == L"signature")
			Sleep(15);
		else
			Sleep(25);
	}

	if (is_scan_busy() == false)
		disconnect_fap50_panel();

	is_video_playing = false;
	cancel_video_play = false;
	//dbg(L"show_video() out \n");
}

void C_FAP50_SDK_Dlg::show_image()
{
	static long t_warning_msg = timeGetTime();
	static float frame_rate = 0;
	static CString normal_msg, warning_msg;

	if (img_status.img == NULL)
		return;

	E_FINGER_POSITION finger_position = img_status.finger_position;

	//CString score_text = L"Quality Score: ";
	//for (int i = 0; i < img_status.nfiq_score_array_size; i++)
	//{
	//	int score = img_status.nfiq_score_array[i];
	//	score_text.AppendFormat(L"%d%s", score, img_status.nfiq_score_array_size - 1 == i ? L"" : L" - ");
	//}

	Mat cv_sampling = Mat(1600, 1600, CV_8UC3, CV_RGB(0xFD, 0xF5, 0xE6)); //sampling window bg

	Mat cv_fingerprint_color;
	if (img_status.show_mode == GUI_SHOW_MODE_SIGNATURE 
		|| img_status.show_mode == GUI_SHOW_MODE_SIGNATURE_BY_PEN)
	{
		Mat tmp = Mat(1000, 1600, CV_8UC1, Scalar(255));
		Rect roi_sign(0, 0, 1600, 882);
		Mat(1000, 1600, CV_8UC1, img_status.img)(roi_sign).copyTo(tmp(roi_sign));
		cvtColor(tmp, cv_fingerprint_color, cv::COLOR_GRAY2BGR);
	}
	else
		cvtColor(Mat(1000, 1600, CV_8UC1, img_status.img), cv_fingerprint_color, cv::COLOR_GRAY2BGR);

	Size 
		rolling_size = Size(800, 750), 
		flat_size= Size(1600, 1000);

	Point img_shift(0, 165);
	Rect 
		roi_sampling(img_shift, flat_size),
		roi_fingerprint(Point(0, 0), flat_size);

	if (img_status.show_mode == GUI_SHOW_MODE_ROLL)
	{
		img_shift = Point(0, 80);
		roi_sampling = Rect(img_shift, rolling_size);
		roi_fingerprint = Rect(Point(400, 150), rolling_size);
	}

	cv_fingerprint_color(roi_fingerprint).copyTo(cv_sampling(roi_sampling));
	//imshow("cv_fingerprint_color", cv_fingerprint_color(cv::Rect(cv::Point(400, 110), cv::Size(800, 750))));

	static BOOL show_image_en = false;
	if (img_status.show_mode == GUI_SHOW_MODE_ROLL)
	{
		//if (img_status.is_finger_on)
		//	gc.DrawLine( img_status.is_roll_done ? &green_pen:&red_pen, img_status.line, 0, img_status.line, 1000);

		// message 
		if (img_status.is_roll_init)
		{
			show_image_en = true;
			normal_msg = L"Rolling init... ";
			warning_msg = L"";

			if (img_status.finger_num != 1)
			{
				switch (finger_position) {
				case FINTER_POSITION_UNKNOW_FINGER:	warning_msg = L"Please put 1 finger on the sensor."; break;
				case FINGER_POSITION_RIGHT_THUMB:	warning_msg = L"Please put right thumb on the sensor.";	break;
				case FINGER_POSITION_RIGHT_INDEX:	warning_msg = L"Please put right index on the sensor.";	break;
				case FINGER_POSITION_RIGHT_MIDDLE:	warning_msg = L"Please put right middle on the sensor."; break;
				case FINGER_POSITION_RIGHT_RING:	warning_msg = L"Please put right ring on the sensor."; break;
				case FINGER_POSITION_RIGHT_LITTLE:	warning_msg = L"Please put right little on the sensor."; break;
				case FINGER_POSITION_LEFT_THUMB:	warning_msg = L"Please put left thumb on the sensor."; break;
				case FINGER_POSITION_LEFT_INDEX:	warning_msg = L"Please put left index on the sensor."; break;
				case FINGER_POSITION_LEFT_MIDDLE:	warning_msg = L"Please put left middle on the sensor."; break;
				case FINGER_POSITION_LEFT_RING:		warning_msg = L"Please put left ring on the sensor."; break;
				case FINGER_POSITION_LEFT_LITTLE:	warning_msg = L"Please put left little on the sensor."; break;
				}
			}

			t_warning_msg = timeGetTime();
		}
		else if (img_status.is_finger_on)
		{
			normal_msg = L"Keep rolling your finger ... ";

			if (img_status.result == IMD_RLT_POOR_CONTACT_IRON)
			{
				warning_msg = L"Poor contact of iron frame! ";
				t_warning_msg = timeGetTime();
			}

			if (img_status.is_roll_done)
			{
				normal_msg = L"Rolling done. ";
				warning_msg = L"Please remove your finger.";
				t_warning_msg = timeGetTime();
			}
		}

		if (img_status.is_finger_on && !img_status.is_roll_init)
			normal_msg.AppendFormat(L"(%.1f fps)", img_status.frame_rate);

		if (timeGetTime() - t_warning_msg > 700) //Delay for visual needs.
			warning_msg = L"";

		//if (img_status.nfiq_score_array_size)
		//	normal_msg += L"  " + score_text;
		//if (img_status.is_roll_done && img_status.is_finger_on)
		//	gc.DrawString(normal_msg, -1, &font, PointF(400 + 16, 0 + 5 + 16), &green_brush);
		//else
		
		//gc.DrawString(normal_msg, -1, &font, PointF(400 + 16, 0 + 5 + 16), &black_brush);// blue_brush);
		double fontScale = 1.5/2;
		int fontFace = cv::FONT_HERSHEY_SIMPLEX;
		Scalar color_black(CV_RGB(0, 0, 0));
		cv::putText(cv_sampling, (char*)(CT2A)normal_msg, cv::Point(8, 0 + 26), fontFace, fontScale, color_black, 2);
		Scalar color_red(CV_RGB(255, 0, 0));
		cv::putText(cv_sampling, (char*)(CT2A)warning_msg, cv::Point(8, 36 + 26), fontFace, fontScale, color_red, 2);
	}
	else if (img_status.show_mode == GUI_SHOW_MODE_FLAT)// message for flat
	{
		if (img_status.is_flat_init)
		{
			show_image_en = true;
			normal_msg = L"Flat init ... ";
			if (img_status.is_finger_on)
				normal_msg = L"Keep your fingers steady ... ";

			if (img_status.result == IMD_RLT_FINGER_TOO_TOP)
				warning_msg = L"Please move down your fingers.";
			else if (img_status.result == IMD_RLT_NEED_TO_TAKE_FINGER_OFF)
				warning_msg = L"Please remove your fingers.";
			else if (FINGER_POSITION_RIGHT_FOUR == finger_position && img_status.finger_num != 4)
				warning_msg = L"Please put right 4 fingers on the sensor.";
			else if (FINGER_POSITION_LEFT_FOUR == finger_position && img_status.finger_num != 4)
				warning_msg = L"Please put left 4 fingers on the sensor.";
			else if (FINGER_POSITION_BOTH_THUMBS == finger_position && img_status.finger_num != 2)
				warning_msg = L"Please put 2 thumbs on the sensor.";
			else if ((FINGER_POSITION_RIGHT_THUMB <= finger_position
				&& finger_position <= FINGER_POSITION_LEFT_LITTLE)
				&& img_status.finger_num != 1)
			{
				switch (finger_position) {
				case FINTER_POSITION_UNKNOW_FINGER:	warning_msg = L"Please put 1 finger on the sensor."; break;
				case FINGER_POSITION_RIGHT_THUMB:	warning_msg = L"Please put right thumb on the sensor.";	break;
				case FINGER_POSITION_RIGHT_INDEX:	warning_msg = L"Please put right index on the sensor.";	break;
				case FINGER_POSITION_RIGHT_MIDDLE:	warning_msg = L"Please put right middle on the sensor."; break;
				case FINGER_POSITION_RIGHT_RING:	warning_msg = L"Please put right ring on the sensor."; break;
				case FINGER_POSITION_RIGHT_LITTLE:	warning_msg = L"Please put right little on the sensor."; break;
				case FINGER_POSITION_LEFT_THUMB:	warning_msg = L"Please put left thumb on the sensor."; break;
				case FINGER_POSITION_LEFT_INDEX:	warning_msg = L"Please put left index on the sensor."; break;
				case FINGER_POSITION_LEFT_MIDDLE:	warning_msg = L"Please put left middle on the sensor."; break;
				case FINGER_POSITION_LEFT_RING:		warning_msg = L"Please put left ring on the sensor."; break;
				case FINGER_POSITION_LEFT_LITTLE:	warning_msg = L"Please put left little on the sensor."; break;
				}
			}
			else if (!img_status.is_finger_on)
				warning_msg = L"Please put your fingers on the sensor.";
			else if (img_status.result == IMD_RLT_FINGER_NUM_OVER_FOUR)//if unknow_finger
				warning_msg = L"The number of fingers is more than 4!";
			else
				warning_msg = L"";

			normal_msg.AppendFormat(L"(%.1f fps)", img_status.frame_rate);
			t_warning_msg = timeGetTime();
		}
		else if (img_status.is_flat_done)
		{
			normal_msg = L"Flat done. ";
			warning_msg = L"Please remove your fingers.";
			t_warning_msg = timeGetTime();
			//if (img_status.result == IMD_RLT_NEED_TO_TAKE_FINGER_OFF)
			//{
			//}
		}

		ImdDiamond* imd_contours = (ImdDiamond*)img_status.contours;
		//dbg(L"finger_num=%d\n", imd_contours.size());

		for (int i = 0; i < img_status.finger_num; i++)
		{
			ImdDiamond& contour = imd_contours[i];
			cv::Point	pa = cv::Point(contour.pa.x, contour.pa.y) + img_shift,
				pb = cv::Point(contour.pb.x, contour.pb.y) + img_shift,
				pc = cv::Point(contour.pc.x, contour.pc.y) + img_shift,
				pd = cv::Point(contour.pd.x, contour.pd.y) + img_shift;
			Scalar pen_color = CV_RGB(0, 160, 0);//green
			if (img_status.result == IMD_RLT_FINGER_NUM_OVER_FOUR || (!contour.valid))
				pen_color = CV_RGB(255, 0, 0);//red

			line(cv_sampling, pa, pb, pen_color, 2);
			line(cv_sampling, pb, pc, pen_color, 2);
			line(cv_sampling, pc, pd, pen_color, 2);
			line(cv_sampling, pd, pa, pen_color, 2);
		}

		if (timeGetTime() - t_warning_msg > 700) //Delay for visual needs.
			warning_msg = L"";

		//if (img_status.nfiq_score_array_size)
		//	normal_msg += L"  " + score_text;
		double fontScale = 1.5;
		int fontFace = cv::FONT_HERSHEY_SIMPLEX;
		Scalar color_black(CV_RGB(0, 0, 0));
		putText(cv_sampling, (char*)(CT2A)normal_msg, cv::Point(0 + 16, 0 + 16+ 40), fontFace, fontScale, color_black, 4);
		Scalar color_red(CV_RGB(255, 0, 0));
		putText(cv_sampling, (char*)(CT2A)warning_msg, cv::Point(0 + 16, 68 + 16+ 40), fontFace, fontScale, color_red, 4);
	}
	else if (img_status.show_mode == GUI_SHOW_MODE_SIGNATURE 
		|| img_status.show_mode == GUI_SHOW_MODE_SIGNATURE_BY_PEN )  //message for signature
	{
		show_image_en = true;
		normal_msg = L"Signature ... ";
		normal_msg.AppendFormat(L"(%.1f fps)", img_status.frame_rate);
		t_warning_msg = timeGetTime();

		if (timeGetTime() - t_warning_msg > 700) //Delay for visual needs.
			warning_msg = L"";

		double fontScale = 1.5;
		int fontFace = cv::FONT_HERSHEY_SIMPLEX;
		Scalar color_black(CV_RGB(0, 0, 0));
		putText(cv_sampling, (char*)(CT2A)normal_msg, cv::Point(0 + 16, 0 + 16 + 40), fontFace, fontScale, color_black, 4);
		//Scalar color_red(CV_RGB(255, 0, 0));
		//putText(cv_sampling, (char*)(CT2A)warning_msg, cv::Point(0 + 16, 68 + 16 + 40), fontFace, fontScale, color_red, 4);
	}

	//long time = timeGetTime();
	if (show_image_en)
	{
		float scale = 0.33;
		if (img_status.show_mode == GUI_SHOW_MODE_ROLL)
			scale *= 2;

		scale *= 1.48;

		Mat resizedMat;
		Size newSize(cv_sampling.cols * scale, cv_sampling.rows * scale);
		resize(cv_sampling, resizedMat, newSize);
		imshow("IDC_SAMPLING", resizedMat);
		//imshow("resizedMat", resizedMat);
	}

	if(is_scan_by_manual == false)
		if (img_status.result == IMD_RLT_NEED_TO_TAKE_FINGER_OFF)
			show_image_en = false;
}

void C_FAP50_SDK_Dlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case TIMER_EVENT_SHOW_PANEL:
		show_panel();
		break;

	case TIMER_EVENT_SHOW_IMAGE:
		show_image();
		break;

	case TIMER_EVENT_STANDBY_VIDEO:
		//dbg("Timer:TIMER_EVENT_STANDBY_VIDEO\n");
		thread_member(C_FAP50_SDK_Dlg::show_video);
		break;
	case TIME_DEBOUNCE_BRIGHTNESS:
		KillTimer(TIME_DEBOUNCE_BRIGHTNESS);
		thread_member(C_FAP50_SDK_Dlg::change_backlight);
		break;

	}

	if (is_scan_busy() == false)
		KillTimer(nIDEvent);

	CDialogEx::OnTimer(nIDEvent);
	//dbg(L"OnTimer() end\n");
}

void C_FAP50_SDK_Dlg::OnCbnSelchangeCbSamplingSequence()
{
	if (disable_prompts == false)
	{
		int result = MessageBox(L"The sampled fingerprints will disappear, continue?", L"Warning", MB_YESNO);
		if (result == IDNO)
		{
			cb_sampling_sequence.SetCurSel(sampling_sequence);
			return;
		}
	}

	sampling_sequence = cb_sampling_sequence.GetCurSel();
	IMD_RESULT res = device_reset();
	
	list_image();
	clear_msg();
}

void C_FAP50_SDK_Dlg::OnCbnSelchangeCbNfiqVer()
{
	SystemProperty property;
	get_system_property(&property);

	if (disable_prompts == false)
	{
		int result = MessageBox(L"The sampled fingerprints will disappear, continue?", L"Warning", MB_YESNO);
		if (result == IDNO)
		{
			cb_nfiq_ver.SetCurSel(property.nfiq_ver);
			return;
		}
	}
	
	IMD_RESULT res = device_reset();
	property.nfiq_ver = (NFIQ_VERSION)cb_nfiq_ver.GetCurSel();
	set_system_property(&property);

	list_image();
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnSaveFile()
{
	CString file_name;
	if (!get_save_name(file_name, this))
		return;

	IMD_RESULT res;
	
	//roll
	if (sampling_sequence == SS_FLAT_442R)
	{
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_THUMB, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_INDEX, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_MIDDLE, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_RING, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_RIGHT_LITTLE, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_THUMB, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_INDEX, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_MIDDLE, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_RING, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_ROLL, FINGER_POSITION_LEFT_LITTLE, (CT2A)file_name);
	}

	//flat
	if (sampling_sequence == SS_FLAT_442R 
		|| sampling_sequence == SS_FLAT_442)
	{
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_FOUR, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_INDEX, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_MIDDLE, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_RING, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_LITTLE, (CT2A)file_name);

		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_FOUR, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_INDEX, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_MIDDLE, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_RING, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_LITTLE, (CT2A)file_name);

		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_BOTH_THUMBS, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_RIGHT_THUMB, (CT2A)file_name);
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_LEFT_THUMB, (CT2A)file_name);
	}

	if (sampling_sequence == SS_FLAT_SOME_FINGERS)
	{
		res = save_file(GUI_SHOW_MODE_FLAT, FINGER_POSITION_SOME_FINGERS, (CT2A)file_name);
		for (auto& pos : dlg_sel_finger->select_finger)
			res = save_file(GUI_SHOW_MODE_FLAT, pos, (CT2A)file_name);
	}

	if (sampling_sequence == SS_SIGNATURE
		|| sampling_sequence == SS_SIGNATURE_BY_PEN)
		res = save_file(GUI_SHOW_MODE_SIGNATURE, FINGER_POSITION_SIGNATURE, (CT2A)file_name);

	if (res)
	{
		callback_fap50_event(res);
		return;
	}

	clear_msg();
	CString msg = L""; msg.Format(L"Save file %s done.\n", file_name);
	message_bar(msg);
}

HBRUSH C_FAP50_SDK_Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (pWnd->GetDlgCtrlID()) {
	case IDC_STATIC4:
	case IDC_STATIC5:
	case IDC_STATIC1:
	case IDC_STATIC2:
	case IDC_STATIC_BackLight:
		pDC->SetTextColor(WHITE_COLOR);
		pDC->SetBkColor(_2_COLOR);
		return (HBRUSH)GetStockObject(NULL_BRUSH);

	case IDC_EDIT1:
		pDC->SetTextColor(WHITE_COLOR);
		pDC->SetBkColor(_1_COLOR);
		return m_edit_bg_color;

	default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}


	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//if (pWnd->GetDlgCtrlID() == IDC_EDIT1)
	//	hbr = m_brBack;

	//// TODO:  Return a different brush if the default is not desired
	//return hbr;
}

BOOL C_FAP50_SDK_Dlg::OnEraseBkgnd(CDC* pDC) //20240316 box: change dialog background color.
{
	CRect rect;
	GetClientRect(&rect);

	CBrush myBrush(RGB(51, 52, 144));//255, 255, 255));    // dialog background color
	CBrush* pOld = pDC->SelectObject(&myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOld);    // restore old brush
	return bRes;                       // CDialog::OnEraseBkgnd(pDC);
}

void C_FAP50_SDK_Dlg::OnNcPaint()
{
	// 
	// TODO: Add message handler code here
	//CDC* pWinDC = GetWindowDC();
	//CRect rcTitle, rcWin;
	//GetWindowRect(rcWin);
	//rcTitle.left = 1;
	//rcTitle.top = 1;
	//rcTitle.right = rcWin.Width() - 50;
	//rcTitle.bottom = GetSystemMetrics(SM_CYSMCAPTION);
	//CBrush brbak = RGB(255, 0, 0);//Set the title bar color to red
	////pWinDC->SelectObject(&brbak);
	//pWinDC->FillRect(rcTitle, &brbak);
	//ReleaseDC(pWinDC);

	CDialogEx::OnNcPaint();
	//CDialogEx::OnNcPaint();

	//// 获取标题栏绘图设备
	//CDC* pDC = GetWindowDC();
	//if (pDC)
	//{
	//	// 获取标题栏区域
	//	CRect rect;
	//	GetWindowRect(&rect);
	//	ScreenToClient(&rect);
	//	rect.bottom = rect.top + GetSystemMetrics(SM_CYCAPTION);

	//	// 绘制自定义标题栏颜色
	//	pDC->FillSolidRect(rect, RGB(255, 0, 0)); // 这里填充红色，您可以根据需要更改颜色

	//	// 释放绘图设备
	//	ReleaseDC(pDC);
	//}
}

void CAboutDlg::OnBnClickedDisableAllUSB()
{
	usb_switch(0);
}

void CAboutDlg::OnBnClickedEnableAudioUSB()
{
	usb_switch(1);
}

void CAboutDlg::OnBnClickedEnableUSB2USB3()
{
	usb_switch(3);
}

void CAboutDlg::OnBnClickedBurnCode()
{
	int ret = MessageBox(L"The LED FW will be clear, Yes/No?", NULL, MB_YESNO);
	if (ret == IDNO)
		return;

	set_burn_code();
	MessageBox(L"Please re-plugin USB device.", NULL, MB_OK);
}

void CAboutDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	static CString password;
	GetDlgItem(IDC_EDIT_PW)->GetWindowText(password);

	if (password == L"035506818")
	{
		GetDlgItem(IDC_BTN_DISABLE_ALL_USB)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_BTN_AUDIO_USB)->ShowWindow(SW_NORMAL);
	}
	else if (password == L"53495662")
	{
		GetDlgItem(IDC_BTN_DISABLE_ALL_USB)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_BTN_AUDIO_USB)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_BTN_ENABLE_USB23)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_BTN_BURN_CODE)->ShowWindow(SW_NORMAL);
	}
	else
	{
		GetDlgItem(IDC_BTN_DISABLE_ALL_USB)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_AUDIO_USB)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_ENABLE_USB23)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_BURN_CODE)->ShowWindow(SW_HIDE);
	}
}

void C_FAP50_SDK_Dlg::OnBnClickedUserSpaceTest()
{
	return;

	//void tcp_image_transfer();
	//tcp_image_transfer();
	//return;

	IMD_RESULT res = IMD_RLT_SUCCESS;

	static int test_item = 1;
	if (test_item == 0)
	{
		//static int cnt = 0;
		//cnt++;

		//BYTE sn_w[32], sn_r[32];
		//for (int i = 0; i < sizeof(sn_w); i++)
		//	sn_w[i] = i + cnt;

		//res = serial_number(0, sn_r);
		//res = serial_number(1, sn_w);
		//if (res)
		//	NOP();
		//res = serial_number(0, sn_r);
		//NOP();

	}
	else
	{
#define USER_SPACE_SIZE (5*1024L)
		BYTE read_data[5120]{};

		BYTE write_data[100];
		int cnt = 0;
		static int prg_char = 0x99;
		static int offset = 0x00;

		//init write data
		while (1)
		{
			for (int j = 0; j < 256; j++)
			{
				write_data[cnt] = prg_char;
				cnt++;
				if (cnt == sizeof(write_data))
					break;
			}

			prg_char++;
			if (cnt == sizeof(write_data))
				break;
		}

		res = user_space(0, 0, read_data, sizeof(read_data));

		//user_space(0, 0, read_data, sizeof(read_data));
		res = user_space(1, offset, write_data, sizeof(write_data));
		if (res)
			NOP();
		offset = (offset + 0x100) % USER_SPACE_SIZE;

		res = user_space(0, 0, read_data, sizeof(read_data));
		NOP();
	}
}

void C_FAP50_SDK_Dlg::OnCbnSelchangeCbLang()
{
	SystemProperty property;
	get_system_property(&property);
	int speech_language = cb_language.GetCurSel();
	CString strLB;
	cb_language.GetLBText(speech_language, strLB);
	if (strLB == USER_DEFINE_TEXT)
		speech_language = 0xFF;
	property.speech_language = speech_language;
	set_system_property(&property);

	//host
	host_speech.set_language(speech_language);
}

void C_FAP50_SDK_Dlg::OnCbnSelchangeCbVol()
{
	SystemProperty property;
	get_system_property(&property);
	property.speech_volume = cb_volume.GetCurSel();
	set_system_property(&property);

	//host
	host_speech.set_volume(cb_volume.GetCurSel());
}

void C_FAP50_SDK_Dlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	//GetDlgItem(IDC_EDIT1)->SetFocus();
	((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0);
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnR4()
{

}

void C_FAP50_SDK_Dlg::OnClose()
{
	//CDialogEx::OnClose(); //do this panel will be reset.
	//return;

	if (is_sampling_finger_busy)
	{
		if (cancel_sampling)
			return;

		cancel_sampling = true;
		scan_cancel();
		is_app_close = true;
		return;
	}

	if(is_video_playing)
	{
		cancel_video_play = true;
		while (is_video_playing);
	}

	disconnect_fap50_panel();
	CDialogEx::OnClose();
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnEraseSing()
{
	signature(SIGNATURE_ACTION_CLEAR);
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnSignDone()
{
	signature(SIGNATURE_ACTION_OK);
}

void C_FAP50_SDK_Dlg::OnBnClickedBtnshowvideo()
{
	if (is_video_playing)
	{
		cancel_video_play = true;
		while (is_video_playing);
		return;
	}

	thread_member(C_FAP50_SDK_Dlg::show_video);
}

void C_FAP50_SDK_Dlg::OnBnClickedBtntest()
{
	set_led_speech_standby_mode();
}

void C_FAP50_SDK_Dlg::OnCbnSelchangeCbbacklight()
{
	KillTimer(TIME_DEBOUNCE_BRIGHTNESS);
	SetTimer(TIME_DEBOUNCE_BRIGHTNESS, 300, NULL);
}

void C_FAP50_SDK_Dlg::change_backlight()
{
	KillTimer(TIMER_EVENT_STANDBY_VIDEO);
	cancel_video_play = true;
	
	while (is_video_playing);

	if (!connect_fap50_panel(PANEL_IP, PANEL_PORT))
	{
		//message_bar(L"Can not connect the panel. Maybe not support.", true);
		return;
	}

	int sel = cb_back_light.GetCurSel();
	if (m_nPreviorBrightnessValue != sel)
	{
		set_brightness(sel);
		m_nPreviorBrightnessValue = sel;
		Sleep(1);
	}
	
	disconnect_fap50_panel();
	SetTimer(TIMER_EVENT_STANDBY_VIDEO, TIMER_SHOW_VIDEO, NULL);
}

//Add for Powershell script
CString C_FAP50_SDK_Dlg::WriteScriptToTempFile()
{
	HMODULE hModule = AfxGetInstanceHandle();
	HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(IDR_POWERSHELL_SCRIPT), RT_RCDATA);
	if (!hRes) return _T("");

	HGLOBAL hData = LoadResource(hModule, hRes);
	if (!hData) return _T("");

	DWORD size = SizeofResource(hModule, hRes);
	void* pData = LockResource(hData);
	if (!pData || size == 0) return _T("");

	TCHAR tempPath[MAX_PATH];
	if (!GetTempPath(MAX_PATH, tempPath))
		return _T("");

	CString tempFilePath;
	GUID guid;
	if (CoCreateGuid(&guid) == S_OK)
	{
		CString guidStr;
		LPOLESTR lpGuid;
		StringFromCLSID(guid, &lpGuid);
		guidStr = lpGuid;
		CoTaskMemFree(lpGuid);

		guidStr.Replace(_T("{"), _T(""));
		guidStr.Replace(_T("}"), _T(""));
		tempFilePath.Format(_T("%s\\%s.ps1"), tempPath, guidStr);
	}
	else
	{
		tempFilePath.Format(_T("%s\\temp_script.ps1"), tempPath);
	}

	HANDLE hFile = CreateFile(tempFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return _T("");

	DWORD written = 0;
	WriteFile(hFile, pData, size, &written, NULL);
	CloseHandle(hFile);
	return tempFilePath;
}

bool C_FAP50_SDK_Dlg::RunPowerShellScriptAsAdmin(const CString& scriptPath)
{
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.hwnd = NULL;
	sei.lpVerb = _T("runas");
	sei.lpFile = _T("powershell.exe");
	//sei.lpFile = _T("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
	CString params;
	params.Format(_T("-ExecutionPolicy Bypass -File \"%s\""), scriptPath.GetString()); // for debug parameter: -NoExit 
	sei.lpParameters = params.GetString();
	sei.nShow = SW_HIDE;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	if (!ShellExecuteEx(&sei))
	{
		DWORD err = GetLastError();
		CString msg;
		msg.Format(_T("Failed to launch PowerShell script. Error: %d"), err);	
		return false;
	}
	else
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
		CloseHandle(sei.hProcess);
	}

	return true;
}

void C_FAP50_SDK_Dlg::RunEmbeddedPowerShellScript()
{
	CString tempScriptFile = WriteScriptToTempFile();
	if (tempScriptFile.IsEmpty())
	{
		//AfxMessageBox(_T("Failed to extract PowerShell script from resources."));
		return;
	}

	if (RunPowerShellScriptAsAdmin(tempScriptFile))
	{
		//AfxMessageBox(_T("PowerShell script started with administrator privileges."));
	}
	DeleteFile(tempScriptFile);
}