// C_SeletFingers.cpp : implementation file
//

#include "pch.h"
#include "afxdialogex.h"
#include "C_SeletFingers.h"


// C_SeletFingers dialog

IMPLEMENT_DYNAMIC(C_SeletFingers, CDialogEx)

C_SeletFingers::C_SeletFingers(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SEL_FINGERS, pParent)
{
	show_en = FALSE;
}

C_SeletFingers::~C_SeletFingers()
{
}

void C_SeletFingers::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(C_SeletFingers, CDialogEx)
	ON_BN_CLICKED(IDOK, &C_SeletFingers::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_L5, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_L4, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_L3, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_L2, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_L1, &C_SeletFingers::OnBnClickedChk)

	ON_BN_CLICKED(IDC_CHK_R5, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_R4, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_R3, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_R2, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_R1, &C_SeletFingers::OnBnClickedChk)
	ON_BN_CLICKED(IDC_CHK_right_full, &C_SeletFingers::OnBnClickedChkrightfull)
	ON_BN_CLICKED(IDC_CHK_left_full, &C_SeletFingers::OnBnClickedChkleftfull)
END_MESSAGE_MAP()


// C_SeletFingers message handlers


void C_SeletFingers::show_window(BOOL enable)
{
	if (enable)
		ShowWindow(SW_NORMAL);
	else
		ShowWindow(SW_HIDE);

	show_en = enable;
}

void C_SeletFingers::OnBnClickedOk()
{
	//CDialogEx::OnOK();
	show_window(FALSE);
}

void C_SeletFingers::OnBnClickedChk()
{
	CButton* button;// = (CButton*)GetDlgItem(IDC_CHECKBOX);
	vector<int> ids{ 
		IDC_CHK_R1, IDC_CHK_R2, IDC_CHK_R3, IDC_CHK_R4, IDC_CHK_R5,
		IDC_CHK_L5, IDC_CHK_L4, IDC_CHK_L3, IDC_CHK_L2, IDC_CHK_L1,
		IDC_CHK_left_full, IDC_CHK_right_full,
	};

	select_finger.clear();

	for (auto& id : ids)
	{
		button = (CButton*)GetDlgItem(id);
		if (button->GetCheck() == FALSE)
			button->EnableWindow();
	}

	enum SAMPLING_TYPE {
		UNKNOWN,
		LEFT,
		RIGHT,
		THUMBS,
		RIGHT_FULL,
		LEFT_FULL,
	};
	SAMPLING_TYPE sampling_type = UNKNOWN;

	for (auto& id : ids)
	{
		button = (CButton*)GetDlgItem(id);
		if (button->GetCheck())
			switch (id) {
			case IDC_CHK_left_full:
				sampling_type = LEFT_FULL;
				select_finger.push_back(FINGER_POSITION_LEFT_FULL);
				break;
			case IDC_CHK_right_full:
				sampling_type = RIGHT_FULL;
				select_finger.push_back(FINGER_POSITION_RIGHT_FULL);
				break;

			case IDC_CHK_L4:
				sampling_type = LEFT;
				select_finger.push_back(FINGER_POSITION_LEFT_RING);
				break;
			case IDC_CHK_L3:
				sampling_type = LEFT;
				select_finger.push_back(FINGER_POSITION_LEFT_MIDDLE);
				break;
			case IDC_CHK_L2:
				sampling_type = LEFT;
				select_finger.push_back(FINGER_POSITION_LEFT_INDEX);
				break;
			case IDC_CHK_L1:
				sampling_type = THUMBS;
				select_finger.push_back(FINGER_POSITION_LEFT_THUMB);
				break;
			case IDC_CHK_R5:
				sampling_type = RIGHT;
				select_finger.push_back(FINGER_POSITION_RIGHT_LITTLE);
				break;
			case IDC_CHK_R4:
				sampling_type = RIGHT;
				select_finger.push_back(FINGER_POSITION_RIGHT_RING);
				break;
			case IDC_CHK_R3:
				sampling_type = RIGHT;
				select_finger.push_back(FINGER_POSITION_RIGHT_MIDDLE);
				break;
			case IDC_CHK_R2:
				sampling_type = RIGHT;
				select_finger.push_back(FINGER_POSITION_RIGHT_INDEX);
				break;
			case IDC_CHK_R1:
				sampling_type = THUMBS;
				select_finger.push_back(FINGER_POSITION_RIGHT_THUMB);
				break;
			}
	}

	if (select_finger.size() > 0)
	{
		vector<int> disable_ids;

		switch (sampling_type) {
		case LEFT:
		{
			vector<int> ids{
				IDC_CHK_R1, IDC_CHK_R2, IDC_CHK_R3, IDC_CHK_R4, IDC_CHK_R5,
				IDC_CHK_L1, IDC_CHK_right_full, IDC_CHK_left_full,
			};
			disable_ids = ids;
			break;
		}
		case RIGHT:
		{
			vector<int> ids{
				IDC_CHK_L5, IDC_CHK_L4, IDC_CHK_L3, IDC_CHK_L2, IDC_CHK_L1,
				IDC_CHK_R1, IDC_CHK_right_full,IDC_CHK_left_full,
			};
			disable_ids = ids;
			break;
		}
		case THUMBS:
		{
			vector<int> ids{
				IDC_CHK_R2, IDC_CHK_R3, IDC_CHK_R4, IDC_CHK_R5,
				IDC_CHK_L5, IDC_CHK_L4, IDC_CHK_L3, IDC_CHK_L2,
				 IDC_CHK_right_full, IDC_CHK_left_full,
			};
			disable_ids = ids;
			break;
		}
		case RIGHT_FULL:
		{
			vector<int> ids{
				IDC_CHK_R1, IDC_CHK_R2, IDC_CHK_R3, IDC_CHK_R4, IDC_CHK_R5,
				IDC_CHK_L5, IDC_CHK_L4, IDC_CHK_L3, IDC_CHK_L2, IDC_CHK_L1,
				IDC_CHK_left_full,
			};
			disable_ids = ids;
			break;
		}
		case LEFT_FULL:
		{
			vector<int> ids{
				IDC_CHK_R1, IDC_CHK_R2, IDC_CHK_R3, IDC_CHK_R4, IDC_CHK_R5,
				IDC_CHK_L5, IDC_CHK_L4, IDC_CHK_L3, IDC_CHK_L2, IDC_CHK_L1,
				IDC_CHK_right_full,
			};
			disable_ids = ids;
			break;
		}
		}

		for (auto& id : disable_ids)
		{
			button = (CButton*)GetDlgItem(id);
			button->EnableWindow(0);
		}
	}

}

void C_SeletFingers::OnBnClickedChkrightfull()
{
	OnBnClickedChk();
}

void C_SeletFingers::OnBnClickedChkleftfull()
{
	OnBnClickedChk();
}
