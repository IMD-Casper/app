#pragma once

#ifdef __ANDROID__
typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint8_t* PBYTE;
typedef void* HANDLE;

#ifndef OUT
#define OUT
#endif

#ifndef IN
#define IN
#endif
#endif // __ANDROID__


#ifdef BUILD_IMD_DLL
	#ifdef _WIN32
		#define IMD_STD_API  	extern "C" __declspec(dllexport) 
		#define EXPORT_IMD_DLL 	__declspec(dllexport)
	#else
		#define IMD_STD_API  	extern "C" __attribute__((visibility("default")))
		#define EXPORT_IMD_DLL 	__attribute__((visibility("default"))) 
	#endif
#else
	#ifdef _WIN32
		#define IMD_STD_API  	extern "C" __declspec(dllimport)
		#define EXPORT_IMD_DLL 	__declspec(dllimport)
	#else
		#define IMD_STD_API  	 extern "C" __attribute__((visibility("default"))) 
		#define EXPORT_IMD_DLL 	__attribute__((visibility("default"))) 
	#endif
#endif


struct ImdPoint2d {
	double x, y;
};
struct ImdDiamond {
	ImdPoint2d pa, pb, pc, pd, cntr;
	double angle;
	bool valid;
};
enum E_SIGNATURE_ACTION {
	SIGNATURE_ACTION_CLEAR,
	SIGNATURE_ACTION_OK,
};
enum E_GUI_SHOW_MODE {
	GUI_SHOW_MODE_NONE,
	GUI_SHOW_MODE_CAPTURE,	//scan fingerprints immediately
	GUI_SHOW_MODE_ROLL,
	GUI_SHOW_MODE_FLAT,
	GUI_SHOW_MODE_SIGNATURE,
	GUI_SHOW_MODE_SIGNATURE_BY_PEN,
};
enum E_FINGER_POSITION {
	FINTER_POSITION_UNKNOW_FINGER = 0,

	FINGER_POSITION_RIGHT_THUMB,
	FINGER_POSITION_RIGHT_INDEX,
	FINGER_POSITION_RIGHT_MIDDLE,
	FINGER_POSITION_RIGHT_RING,
	FINGER_POSITION_RIGHT_LITTLE,

	FINGER_POSITION_LEFT_THUMB,
	FINGER_POSITION_LEFT_INDEX,
	FINGER_POSITION_LEFT_MIDDLE,
	FINGER_POSITION_LEFT_RING,
	FINGER_POSITION_LEFT_LITTLE,

	FINGER_POSITION_RIGHT_FOUR = 13,
	FINGER_POSITION_LEFT_FOUR,
	FINGER_POSITION_BOTH_THUMBS,
	FINGER_POSITION_SOME_FINGERS,
	FINGER_POSITION_SIGNATURE,
	FINGER_POSITION_RIGHT_FULL,
	FINGER_POSITION_LEFT_FULL,
	FINGER_POSITION_SIZE,
};
enum SPEECH_KIND {
	SPEECH_OFF,

	SPEECH_PUT_RIGHT_4_FINGERS,
	SPEECH_PUT_LEFT_4_FINGERS,
	SPEECH_PUT_2_THUMBS_FINGERS,
	SPEECH_REMOVE_YOUR_FINGERS,

	SPEECH_ROLLING_RIGHT_THUMB_FINGER,
	SPEECH_ROLLING_RIGHT_INDEX_FINGER,
	SPEECH_ROLLING_RIGHT_MIDDLE_FINGER,
	SPEECH_ROLLING_RIGHT_RING_FINGER,
	SPEECH_ROLLING_RIGHT_LITTLE_FINGER,
	SPEECH_ROLLING_LIFT_THUMB_FINGER,
	SPEECH_ROLLING_LIFT_INDEX_FINGER,
	SPEECH_ROLLING_LIFT_MIDDLE_FINGER,
	SPEECH_ROLLING_LIFT_RING_FINGER,
	SPEECH_ROLLING_LIFT_LITTLE_FINGER,
	SPEECH_MOVE_DOWN_YOUR_FINGERS,

	SPEECH_REMOVE_YOUR_FINGER,
};
enum NFIQ_VERSION {
	NFIQ_V1,
	NFIQ_V2,
	NFIQ_VER_SIZE,
};
enum IMD_RESULT {
	/** If IMD_RLT_SUCCESS is returned, it means success.If not, it may be a hint
		or an error.*/
	IMD_RLT_SUCCESS = 0x00,

	IMD_RLT_PROMPT = 0x100,
	/** This prompt is triggered when a scan is started.In GUI_SHOW_MODE_CAPTURE,
		there will be no prompt. */
	IMD_RLT_SCAN_THREAD_START,
	//The device is busy scanning images.
	IMD_RLT_SCAN_THREAD_IS_BUSY,
	/** After starting to scan, if no finger is placed for more than 30 seconds,
		the device will stop scanning.*/
	IMD_RLT_SCAN_THREAD_IDLE_TIMEOUT,
	//The prompt is triggered when the device stops.
	IMD_RLT_SCAN_THREAD_END,

	IMD_RLT_FAIL = 0x200,
	//This API does not have any implementation.
	IMD_RLT_NO_IMPLEMENTATION,
	IMD_RLT_NULL_POINTER,
	IMD_RLT_FLASH_WRITE_FAIL,
	//There is a problem with the input parameters of the API.
	IMD_RLT_PARAM_WRONG,
	//No USB devices were found.
	IMD_RLT_CANT_FIND_ANY_DEVICE,
	//Device reset failed.
	IMD_RLT_RESET_DEVICE_FAIL,
	//The device's CHIP ID is incorrect.
	IMD_RLT_CHIP_ID_FAIL,
	//Failed to adjust image background.
	IMD_RLT_CLAI_TIMEOUT,
	//Exception occurred while reading image from USB.
	IMD_RLT_USB_READ_IMAGE_EXCEPTION,
	IMD_RLT_USB_READ_IMAGE_TIMEOUT,
	//No images available.
	IMD_RLT_NO_AVAILABLE_IMAGE,
	//Hardware's USB is too slow
	IMD_RLT_USB_TOO_SLOW,
	//Hardware's DCMI is stuck.
	IMD_RLT_DCMI_IS_STUCK,
	//Scanned images are not yet complete.
	IMD_RLT_NO_ANY_SCAN_DONE,
	//Scanning images cannot be stopped.
	IMD_RLT_STOP_SCAN_TIMEOUT,
	//No fingers were detected.
	IMD_RLT_NO_ANY_FINGER,

	IMD_RLT_NEED_TO_TAKE_FINGER_OFF,

	IMD_RLT_FINGER_TOO_TOP = 0x300,
	IMD_RLT_FINGER_NUM_OVER_FOUR,
	IMD_RLT_PUT_WRONG_HAND,
	IMD_RLT_POOR_NFIQ_QUALITY,
	IMD_RLT_POOR_QUALITY_AND_WRONG_HAND,
	IMD_RLT_POOR_QUALITY_AND_CANTACT_IRON,
	IMD_RLT_POOR_CONTACT_IRON,

	IMD_RLT_OPEN_FILE_FAIL,
	IMD_RLT_INVALID_BIN_FILE,
	IMD_RLT_UPDATE_FW_FAIL,
	IMD_RLT_NOT_SUPPORT,
	IMD_RLT_FINGER_SHAPE_NOT_GOOD,
};

//#pragma pack(push, 8) 
struct SystemProperty {
	/** (TODO) After reset, these variables will be set to default values. */

	E_FINGER_POSITION finger_position;

	int png_compress_ratio;		/** The PNG compression ratio is 0~9. (default:9) */
	int jp2_quality;			/** The Jpeg2000 quality level is 0~1000. (default:750)
								 Zero is loseless compression. */

	float wsq_bit_rate;			/** The bit rate is set as follows:
								 2.25 yields around 5:1 compression
								 0.75 yields around 15:1 compression. (default) */
	char* wsq_comment_text;		/** The WSQ comment default is NULL. */

	NFIQ_VERSION nfiq_ver;				//default:NFIQ_V1
	int nfiq_score_minimum_acceptable;	//1~5, default:3
	int nfiq2_score_minimum_acceptable;	//0~100, default:35
	bool speech_en;						//default:true
	int speech_language;				//0:EN, 1:CH 2:ESP 255:user's define
	int speech_volume;					//0:small, 1:medium, 2:large 3:mute

	bool life_check_en;					//default:false. If debug the prj, need to set 0.
	DWORD scan_timeout_ms;				//unit:ms default:120*1000ms

	//--- signature buttons			//Signature ROI(X,Y,Width,Hight) = (0, 30, 1600, 876)
	int signature_clear_roi[4],		//clear roi:395,830,324,94 (default)
		signature_confirm_roi[4];	//confirm roi:886,825,324,94 (default)

	OUT int image_width, image_height;
	OUT int image_bit_per_pix, image_pix_per_inch;
	OUT BYTE fap50_lib_ver[3];
	OUT BYTE opencv_lib_ver[3];		//OpenCV LIB version
	OUT BYTE nbis_lib_ver[3];		//NBIS LIB version
	OUT BYTE nfiq2_lib_ver[3];		//NFIQ2 LIB version
	OUT BYTE guid[16];
	OUT WORD chip_id;
	OUT BYTE fw_ver[3];				//[1]Major, [0]Minor, [2]Type
	OUT BYTE led_ver[2];			//[1]Major, [0]Minor
	OUT char* product_sn;			//Product Serial Number
	OUT char* product_brand;		//Product Brand
	OUT char* product_model;		//Product Model

	bool scan_by_manual;			//default is false.
};

#define SCORE_ARRAY_SIZE 4
struct ImageProperty {
	E_GUI_SHOW_MODE mode;
	E_FINGER_POSITION pos;
	bool this_scan;

	OUT PBYTE img;
	OUT int width, height;
	OUT int score_array[SCORE_ARRAY_SIZE];
	OUT int score_size;
	OUT int score_min;
	OUT NFIQ_VERSION score_ver;
};

struct ImageStatus {
	/** It is up to the user to decide the fingerprint reading mode. */
	E_GUI_SHOW_MODE show_mode;
	E_FINGER_POSITION finger_position;

	/** Status of roll : If init, the flag is_roll_init will be TRUE.If finished,
		the flag is_roll_done will be TRUE. The value line is the current guideline.*/
	bool is_roll_init, is_roll_done;

	/** Status of flat : The flag is_roll_init will be TRUE if is in init.
		If finished, the flag is_flat_done will be TRUE. */
	bool is_flat_init, is_flat_done;

	int finger_num;		//The "finger_num" is the number of finger presses.

	bool is_finger_on;	//If any finger is touching, the flag "is_finger_on" is TRUE.

	/** This "contours" is a container for fingerprint outline coordinates, which records the
		coordinates of the rectangle. Use this to extract images for each finger separately.
		(The type void* is ImdDiamond*) */
	void* contours;

	float frame_rate;	//Unit:fps (frame per second). Update every 250ms.
	BYTE* img;			//The FAP50 image buffer. (1600*1000)

	IMD_RESULT result;
};
//#pragma pack(pop) 

/** @brief callback_event: This callback is triggered at each execution stage
when scanning is started.
Ex:
	IMD_RLT_SCAN_THREAD_START,
	IMD_RLT_SCAN_THREAD_IDLE_TIMEOUT,
	IMD_RLT_DCMI_IS_STUCK,
	IMD_RLT_DCMI_TOO_FAST,
	IMD_RLT_USB_READ_IMAGE_EXCEPTION,
	IMD_RLT_SCAN_THREAD_END,
@param prompt: prompt code.
@return void:
*/
using Fap50CallbackEvent = void (*)(IMD_RESULT prompt);

struct InterfaceImdFap50Method {
	Fap50CallbackEvent callback_event = 0;
	HANDLE	device_handle = 0;

	virtual IMD_RESULT device_reset(void) = 0;
	virtual IMD_RESULT get_system_property(SystemProperty* p) = 0;
	virtual IMD_RESULT set_system_property(SystemProperty* p) = 0;
	virtual IMD_RESULT scan_start(E_GUI_SHOW_MODE mode, E_FINGER_POSITION* pos_buf, int num) = 0;
	virtual bool is_scan_busy(void) = 0;
	virtual IMD_RESULT scan_cancel(void) = 0;
	virtual IMD_RESULT get_image_status(ImageStatus* status) = 0;

	virtual IMD_RESULT save_file(E_GUI_SHOW_MODE mode, E_FINGER_POSITION finger_pos, const char* file_path) = 0;
	virtual IMD_RESULT get_image(ImageProperty& img_property) = 0;
	virtual IMD_RESULT usb_switch(BYTE usb_sel) = 0;
	virtual IMD_RESULT set_led_speech_standby_mode() = 0;
	virtual IMD_RESULT set_burn_code() = 0;
	virtual IMD_RESULT user_space(bool write, WORD offset, BYTE* data, int len) = 0; //5KB r/w for user.
	virtual IMD_RESULT signature(E_SIGNATURE_ACTION action) = 0;

	//fw update	
	virtual IMD_RESULT clear_flag() = 0;
	virtual IMD_RESULT update_app(const char* file_name) = 0;
};


IMD_STD_API IMD_RESULT set_event(Fap50CallbackEvent e);

/** @brief device_reset(): A reset is required after each power-on, which will
adjust the background and initialize some set values.
@param void:
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT device_reset();

/** @brief get_system_property():Get system property.
@param property:
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT get_system_property(SystemProperty* p);

/** @brief set_system_property(): Set system property.
@param property: Read-only members in property are not affected.
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT set_system_property(SystemProperty* p);

/** @brief scan_start(): The device performs a scan.
@param mode: According to your needs, enter the parameters of E_GUI_SHOW_MODE.
	GUI_SHOW_MODE_CAPTURE: capture mode
	GUI_SHOW_MODE_ROLL: rolling mode
	GUI_SHOW_MODE_FLAT: flat mode
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT scan_start(E_GUI_SHOW_MODE mode, E_FINGER_POSITION* pos_buf, int num);

/* @brief is_scan_busy(): Confirm the status of the device.
@param void:
@return bool: TRUE: busy FALSE: idle
*/
IMD_STD_API bool is_scan_busy();

/* @brief scan_cancel(): Cancel scan.
@para void:
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT scan_cancel();

/** @brief get_image_status(): Get the current roll, flat or capture image status.
@param img_status: Please refer to the content of ImageStatus, which records
some available parameters.
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT get_image_status(ImageStatus* status);

/** @brief save_file(): Save the current image as an image file.
@param file_path: Full path is required. If you want to save in PNG format,
the file extension needs to be ".png". Ex: "d:\\flat_mode.png".
@return IMD_RESULT:
*/
IMD_STD_API IMD_RESULT save_file(E_GUI_SHOW_MODE mode, E_FINGER_POSITION finger_pos, const char* file_path);
IMD_STD_API IMD_RESULT get_image(ImageProperty& img_property);
IMD_STD_API IMD_RESULT usb_switch(BYTE usb_sel);
IMD_STD_API IMD_RESULT set_led_speech_standby_mode();
IMD_STD_API IMD_RESULT set_burn_code();
IMD_STD_API IMD_RESULT user_space(bool is_write, WORD offset, BYTE* data, int len);
IMD_STD_API IMD_RESULT signature(E_SIGNATURE_ACTION action);
IMD_STD_API IMD_RESULT set_android_info(int fd, unsigned int uiVID, unsigned int uiPID);
IMD_STD_API IMD_RESULT start_polling(int DevType);
IMD_STD_API IMD_RESULT stop_polling(int DevType);

// panel functions
IMD_STD_API bool connect_fap50_panel(const char* host, uint16_t port);
IMD_STD_API bool send_jpg_fap50_panel(const uint8_t* buffer, uint32_t size);
IMD_STD_API void disconnect_fap50_panel();
IMD_STD_API void set_brightness(int i);


