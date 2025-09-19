
// fap50_sdk_appDlg.h : header file
//

#pragma once

#include "InterfaceImdFap50Method.h"
#include "C_SeletFingers.h"
#include "INI.h"
#include "HostSpeech.h"


enum SAMPLING_SEQUENCE {
	SS_FLAT_442,
	SS_FLAT_442R,
	SS_FLAT_SOME_FINGERS,
	SS_SIGNATURE,
	SS_SIGNATURE_BY_PEN,
};

enum E_FAP_TYPE {
	F_FAP50,
	E_FAP50PLUS,
	E_NONE
};


// Cfap50sdkappDlg dialog
class C_FAP50_SDK_Dlg : public CDialogEx
{
	//DECLARE_DYNAMIC(C_FAP50_SDK_Dlg)

	bool cancel_sampling = false,
		is_app_close = false,
		is_sampling_finger_busy = false,
		disable_prompts = false,
		is_scan_by_manual = false;

	E_FAP_TYPE fap_type = E_FAP50PLUS;
	Mat list, flat_down_img;
	int sampling_sequence;
	CBrush m_edit_bg_color;// .CreateSolidBrush(RGB(0, 255, 0));
	vector<int> sampling_ui_ids, main_ui_ids;
	CIniReader ini;
	CString str_dir, str_speech_language;
	deque <vector<BYTE>> sign_jpgs;
	map<E_FINGER_POSITION, Mat> scanning_img;
	map<int, Mat> loading_img;
	HostSpeech host_speech;
	E_FINGER_POSITION hand_of_some_fingers = FINTER_POSITION_UNKNOW_FINGER;

	bool LoadPNG(int IDBPNG, int IDCPNG, int x, int  y, int cx, int cy);
	int sampling_color_old_id;

public:
	ImageStatus img_status;
	E_GUI_SHOW_MODE resample_mode;// = GUI_SHOW_MODE_ROLL;
	E_FINGER_POSITION resample_pos;// = FINTER_POSITION_UNKNOW_FINGER;
	C_SeletFingers* dlg_sel_finger;

	C_FAP50_SDK_Dlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~C_FAP50_SDK_Dlg();

	void show_panel();
	void my_imshow(string id, Mat img);

	void callback_fap50_event(IMD_RESULT e);
	void message_bar(CString msg, BOOL clear_msg_en);
	void clear_msg();
	void show_image();

	void flat_442();
	void flat_442_roll();
	void flat_some_fingers();
	void sign();
	void sign_by_pen();
	BOOL get_sampling_mode(int x, int y);
	void resample();

	IMD_RESULT show_valid_finger(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos);
	bool sampling_finger(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos);
	void play_speech(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos);
	void set_video_menu(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos);
	void sampling_msg(E_GUI_SHOW_MODE mode, E_FINGER_POSITION pos);
	void set_gui(int enable);
	void init_list();
	void list_image(
		E_FINGER_POSITION finger_position= FINGER_POSITION_SIZE);
	BOOL reset(BOOL clear_all=true);
	//Add for powershell script
	void RunEmbeddedPowerShellScript();
	bool RunPowerShellScriptAsAdmin(const CString& scriptPath);
	CString WriteScriptToTempFile();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FAP50_SDK_APP_DIALOG };
#endif

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
    afx_msg void OnBnClickedBtnReset();
    CString msg_window;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    CComboBox cb_finger_position;
   // afx_msg void OnCbnSelchangeFingerPos();
	//afx_msg void OnMove(int x, int y);
	//afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
    CComboBox cb_nfiq_ver;
 	afx_msg void OnCbnSelchangeCbNfiqVer();
	CComboBox cb_sampling_sequence;
    afx_msg void OnCbnSelchangeCbSamplingSequence();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnSaveFile();
	CStatic finger_images;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CMFCButton mfc_btn_start;
	CMFCButton mfc_btn_save_file;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnBnClickedUserSpaceTest();
    CComboBox cb_language;
    CComboBox cb_volume;
    afx_msg void OnCbnSelchangeCbLang();
    afx_msg void OnCbnSelchangeCbVol();
	afx_msg void OnEnChangeEdit1();
    afx_msg void OnBnClickedBtnR4();
    afx_msg void OnClose();
    afx_msg void OnBnClickedBtnEraseSing();
    afx_msg void OnBnClickedBtnSignDone();

	bool is_video_playing = false,
		cancel_video_play = false;
	bool is_video_menu_change = false;
	CString video_menu;
	void show_video();
	afx_msg void OnBnClickedBtnshowvideo();
    afx_msg void OnBnClickedBtntest();
	CComboBox cb_back_light;
	afx_msg void OnCbnSelchangeCbbacklight();
	void change_backlight();
	int m_nPreviorBrightnessValue;
};
