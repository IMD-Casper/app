#include "imd_framework.h"
#include "InterfaceImdFap50Method.h"


#define RETURN_FUNC(ret) {return ret; }

int main()
{
	IMD_RESULT res = device_reset();
	dbg("device_reset... ");
	if (res != IMD_RLT_SUCCESS)
	{
		dbg("fail.\n");
		RETURN_FUNC(-1);// return -1;
	}
	else
		dbg("ok.\n");

	SystemProperty p;
	dbg("get_system_property... ");
	get_system_property(&p);
	if (res != IMD_RLT_SUCCESS)
	{
		dbg("fail.\n");
		RETURN_FUNC(-1);// return -1;
	}
	else
	{
		dbg("\n");
		dbg("LIB version v%d.%d.%d\n", p.fap50_lib_ver[2], p.fap50_lib_ver[1], p.fap50_lib_ver[0]);
		dbg("FW version v%X.%.2X\n", p.fw_ver[1], p.fw_ver[0]);
		dbg("Opencv version v%d.%d.%d\n", p.opencv_lib_ver[2], p.opencv_lib_ver[1], p.opencv_lib_ver[0]);
		dbg("NBIS version v%d.%d.%d\n", p.nbis_lib_ver[2], p.nbis_lib_ver[1], p.nbis_lib_ver[0]);
		dbg("NFIQ2 version v%d.%d.%d\n", p.nfiq2_lib_ver[2], p.nfiq2_lib_ver[1], p.nfiq2_lib_ver[0]);
	}

	E_GUI_SHOW_MODE mode = GUI_SHOW_MODE_FLAT;
	E_FINGER_POSITION pos = FINGER_POSITION_RIGHT_FOUR;
	vector<E_FINGER_POSITION> fingers{ pos };

	dbg("scan_start ...\n");
	res = scan_start(mode, fingers.data(), fingers.size());

	ImageStatus img_status;
	while (is_scan_busy())
	{
		if (get_image_status(&img_status) != IMD_RLT_SUCCESS)
			continue;

		if ((img_status.is_finger_on == false && img_status.is_roll_done)
			|| (img_status.is_finger_on == false && img_status.is_flat_done)
			)
		{
			dbg("sampling done, cancel.\n");
			scan_cancel();
		}

		//show_panel(); //this way hand on imshow(), so need to use timer;
	}

	{
		ImageProperty p;
		p.mode = mode;
		p.pos = pos;
		get_image(p);
		//Mat img = Mat(p.height, p.width, CV_8UC1, p.img);
		//imshow("FINGER_POSITION_RIGHT_FOUR", img);

		dbg("Score: %d-%d-%d-%d\n", p.score_array[0], p.score_array[1], p.score_array[2], p.score_array[3]);
	}

func_end:
	
	RETURN_FUNC(0);//return 0;
}