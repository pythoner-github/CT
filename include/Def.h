#ifndef __DEF_H__
#define __DEF_H__

//#include "./lib/opencv/include/cv.h"
#include <opencv/cv.h>
#include <stdio.h>
#include <cassert>

/*
 * machine selection, one of three must be defined, 
 * otherwise machine type is set to EMP-340
 * you can defined it in Makefile
 */
//#define EMP_340//G70
//#define EMP_360//G60
//#define EMP_161//EMP-3000

///> general define
#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif

#ifndef NULL
	#define NULL 0
#endif

#define PI 3.1415926
#define SOUND_SPEED 1.54 //km/s
#define ZERO 0.00000001 

///> output debug info
#ifdef DEBUG
	#define PRINTF(fmt, args...) printf(fmt, ##args);
	#define ASSERT assert
#else
	#define PRINTF(fmt, args...)
	#define ASSERT
#endif

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

///> ultrasound define
#define IMG_W 628
#define IMG_H 440
#define IMG_BPP 3 //bytes per pixels

///> bodymark size
#define BDMK_W 60
#define BDMK_H 75
#define BDMK_PROBE_LEN 24
#define BDMK_MAX_SCALE 1.5
#define BDMK_PROBE_MAX_SCALE 1.2

#define BYTES_DOT 3	//每个像素点用3个字节表示
#define USERCONFIG_LEN 50  
#define FONT_STRING "WenQuanYi Zen Hei, medium"

#define DCMDB_DIR "./res"
#define DCMRES_DIR "./res"
#define OPTIONAL_DIR  "/mnt/disk3/optional"
#define CFG_RES_PATH "./"//"/mnt/harddisk"
#define APP_NAME "09001Color"
#define RC_PATH "res/gtkrc"
#define REGISTER_FILE_PATH "/mnt/disk3"

#define STORE_PATH "/mnt/harddisk/store"
#define SLIDE_PATH "/mnt/harddisk/slide"
//#define UDISK_PATH "/media/SONY"
#define UDISK_PATH "/mnt/udisk"
#define UDISK_DATA_PATH "/mnt/udisk/emp-data"
//#define UDISK_DATA_PATH "/media/SONY/emp-dicom"
#define UPGRADE_DEST_PATH "/mnt/harddisk/emp"
#define UPGRADE_INI_PATH "res/config/Upgrade.ini"
#define UPGRADE_LIST "list.txt"
#define UPGRADE_SRC_PATH "/mnt/udisk/upgrade"
#define OB_CUSTOM_PATH "res/config/CustomOB.txt"
#define CALC_TMP_DATA_PATH "./res/config/tmp_data/"
//#define USERCONFIG_PARENT_PATH  "res/config/"  
//#define USERCONFIG_PATH      "./res/config/userconfig/"  
//
//read select item after open machine before selected item

//#define COMMENT_FILE "res/config/CommentSetting.ini"
///> system config file
#if defined(EMP_360)
#define SYS_SETTING_FILE "res/config-360/SysSetting.ini"
#define USERCONFIG_PARENT_PATH  "res/config-360/"  
#define USERCONFIG_PATH      "./res/config-360/userconfig/"  
#define USER_PATH  "./res/config-360/username"
#define KB_CAL_SETTING_FILE "res/config-360/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config-360/SelectProbeItem.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-360/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config-360/CommentSetting.ini"
#define COMMENT_PATH "res/config-360/commentsetting/"
#define COMMENT_COPY "res/config-360/DefaultComment.ini"
#define CALC_FILE "res/config-360/calcsetting/DSCalcSetting.ini"
#define CALC_PATH "res/config-360/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-360/DefaultCalcSetting.ini"
#define DEFAULT_MEASURE_FILE "res/config-360/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config-360/measuresetting/"
#define MEASURE_FILE "res/config-360/measuresetting/DSMeasureSetting.ini"
#define BIOPSY_SETTING_FILE "res/config-360/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-360/DefaultBiopsySetting.ini"
#elif defined(EMP_161)
#define SYS_SETTING_FILE "res/config-161/SysSetting.ini"
#define USERCONFIG_PARENT_PATH  "res/config-161/"  
#define USERCONFIG_PATH      "./res/config-161/userconfig/"  
#define USER_PATH  "./res/config-161/username"
#define KB_CAL_SETTING_FILE "res/config-161/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config-161/SelectProbeItem.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-161/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config-161/CommentSetting.ini"
#define COMMENT_PATH "res/config-161/commentsetting/"
#define COMMENT_COPY "res/config-161/DefaultComment.ini"
#define CALC_FILE "res/config-161/calcsetting/DSCalcSetting.ini"
#define CALC_PATH "res/config-161/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-161/DefaultCalcSetting.ini"
#define DEFAULT_MEASURE_FILE "res/config-161/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config-161/measuresetting/"
#define MEASURE_FILE "res/config-161/measuresetting/DSMeasureSetting.ini"
#define BIOPSY_SETTING_FILE "res/config-161/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-161/DefaultBiopsySetting.ini"
#elif defined(EMP_440)
#define SYS_SETTING_FILE "res/config-440/SysSetting.ini"
#define USERCONFIG_PARENT_PATH  "res/config-440/"  
#define USERCONFIG_PATH      "./res/config-440/userconfig/"  
#define USER_PATH  "./res/config-440/username"
#define KB_CAL_SETTING_FILE "res/config-440/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config-440/SelectProbeItem.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-440/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config-440/CommentSetting.ini"
#define COMMENT_PATH "res/config-440/commentsetting/"
#define COMMENT_COPY "res/config-440/DefaultComment.ini"
#define CALC_FILE "res/config-440/calcsetting/DSCalcSetting.ini"
#define CALC_PATH "res/config-440/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-440/DefaultCalcSetting.ini"
#define DEFAULT_MEASURE_FILE "res/config-440/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config-440/measuresetting/"
#define MEASURE_FILE "res/config-440/measuresetting/DSMeasureSetting.ini"
#define BIOPSY_SETTING_FILE "res/config-440/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-440/DefaultBiopsySetting.ini"
#elif defined(EMP_322) 
#define SYS_SETTING_FILE "res/config-322/SysSetting.ini"
#define USERCONFIG_PARENT_PATH  "res/config-322/"  
#define USERCONFIG_PATH      "./res/config-322/userconfig/"  
#define USER_PATH  "./res/config-322/username"
#define KB_CAL_SETTING_FILE "res/config-322/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config-322/SelectProbeItem.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-322/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config-322/CommentSetting.ini"
#define COMMENT_PATH "res/config-322/commentsetting/"
#define COMMENT_COPY "res/config-322/DefaultComment.ini"
#define CALC_PATH "res/config-322/calcsetting/"
#define CALC_FILE "res/config-322/calcsetting/DSCalcSetting.ini"
#define DEFAULT_CALC_FILE "res/config-322/DefaultCalcSetting.ini"
#define DEFAULT_MEASURE_FILE "res/config-322/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config-322/measuresetting/"
#define MEASURE_FILE "res/config-322/measuresetting/DSMeasureSetting.ini"
#define BIOPSY_SETTING_FILE "res/config-322/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-322/DefaultBiopsySetting.ini"
#elif defined(EMP_313)
#define SYS_SETTING_FILE "res/config-313/SysSetting.ini"
#define USERCONFIG_PARENT_PATH  "res/config-313/"   
#define USERCONFIG_PATH      "./res/config-313/userconfig/"  
#define USER_PATH  "./res/config-313/username"
#define KB_CAL_SETTING_FILE "res/config-313/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config-313/SelectProbeItem.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-313/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config-313/CommentSetting.ini"
#define COMMENT_PATH "res/config-313/commentsetting/"
#define COMMENT_COPY "res/config-313/DefaultComment.ini"
#define CALC_FILE "res/config-313/calcsetting/DSCalcSetting.ini"
#define CALC_PATH "res/config-313/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-313/DefaultCalcSetting.ini"
#define BIOPSY_SETTING_FILE "res/config-313/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-313/DefaultBiopsySetting.ini"

#elif defined(EMP_430)
#define SYS_SETTING_FILE "res/config-430/SysSetting.ini"
#define USERCONFIG_PARENT_PATH  "res/config-430/"   
#define USERCONFIG_PATH      "./res/config-430/userconfig/"  
#define USER_PATH  "./res/config-430/username"
#define KB_CAL_SETTING_FILE "res/config-430/KbCalcSetting.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-430/DefaultProbeItem.ini"
#define STORE_ITEM_PATH "res/config-430/SelectProbeItem.ini"
#define COMMENT_FILE "res/config-430/CommentSetting.ini"
#define COMMENT_PATH "res/config-430/commentsetting/"
#define COMMENT_COPY "res/config-430/DefaultComment.ini"
#define CALC_FILE "res/config-430/calcsetting/DSCalcSetting.ini"
#define CALC_PATH "res/config-430/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-430/DefaultCalcSetting.ini"
#define DEFAULT_MEASURE_FILE "res/config-430/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config-430/measuresetting/"
#define MEASURE_FILE "res/config-430/measuresetting/DSMeasureSetting.ini"
#define BIOPSY_SETTING_FILE "res/config-430/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-430/DefaultBiopsySetting.ini"

#elif defined(EMP_355)
#define SYS_SETTING_FILE "res/config-355/SysSetting.ini"
#define USER_PATH  "./res/config-355/username"
#define KB_CAL_SETTING_FILE "res/config-355/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config-355/SelectProbeItem.ini"
#define BIOPSY_SETTING_FILE "res/config-355/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config-355/DefaultBiopsySetting.ini"
#ifdef VET
#define COMMENT_FILE "res/config-355/vet/VetCommentSetting.ini"
#define CALC_FILE "res/config-355/vet/calcsetting/VetDSCalcSetting.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config-355/vet/VetDefaultProbeItem.ini"
#define USERCONFIG_PARENT_PATH  "res/config-355/vet/"   
#define COMMENT_PATH "res/config-355/vet/commentsetting/"
#define CALC_PATH "res/config-355/vet/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-355/vet/VetDefaultCalcSetting.ini"
#define COMMENT_COPY "res/config-355/vet/VetDefaultComment.ini"
#define USERCONFIG_PATH      "./res/config-355/vet/userconfig/"  
#define MEASURE_PATH "res/config-355/vet/measuresetting/"
#define DEFAULT_MEASURE_FILE "res/config-355/vet/VetDefaultMeasureSetting.ini"
#define MEASURE_FILE "res/config-355/vet/measuresetting/VetDSMeasureSetting.ini"
#else
#define USERCONFIG_PARENT_PATH  "res/config-355/"  
#define STORE_DEFAULT_ITEM_PATH "res/config-355/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config-355/CommentSetting.ini"
#define CALC_FILE "res/config-355/calcsetting/DSCalcSetting.ini"
#define COMMENT_PATH "res/config-355/commentsetting/"
#define CALC_PATH "res/config-355/calcsetting/"
#define DEFAULT_CALC_FILE "res/config-355/DefaultCalcSetting.ini"
#define COMMENT_COPY "res/config-355/DefaultComment.ini"
#define USERCONFIG_PATH      "./res/config-355/userconfig/"  
#define DEFAULT_MEASURE_FILE "res/config-355/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config-355/measuresetting/"
#define MEASURE_FILE "res/config-355/measuresetting/DSMeasureSetting.ini"
#endif
#else
#define SYS_SETTING_FILE "res/config/SysSetting.ini"
#define USER_PATH  "./res/config/username"
#define KB_CAL_SETTING_FILE "res/config/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config/SelectProbeItem.ini"
#define BIOPSY_SETTING_FILE "res/config/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config/DefaultBiopsySetting.ini"
#ifdef VET
#define COMMENT_FILE "res/config/vet/VetCommentSetting.ini"
#define CALC_FILE "res/config/vet/calcsetting/VetDSCalcSetting.ini"
#define STORE_DEFAULT_ITEM_PATH "res/config/vet/VetDefaultProbeItem.ini"
#define USERCONFIG_PARENT_PATH  "res/config/vet/"   
#define COMMENT_PATH "res/config/vet/commentsetting/"
#define CALC_PATH "res/config/vet/calcsetting/"
#define DEFAULT_CALC_FILE "res/config/vet/VetDefaultCalcSetting.ini"
#define MEASURE_PATH "res/config/vet/measuresetting/"
#define DEFAULT_MEASURE_FILE "res/config/vet/VetDefaultMeasureSetting.ini"
#define MEASURE_FILE "res/config/vet/measuresetting/VetDSMeasureSetting.ini"
#define COMMENT_COPY "res/config/vet/VetDefaultComment.ini"
#define USERCONFIG_PATH      "./res/config/vet/userconfig/"  
#else
#define USERCONFIG_PARENT_PATH  "res/config/"  
#define STORE_DEFAULT_ITEM_PATH "res/config/DefaultProbeItem.ini"
#define COMMENT_FILE "res/config/CommentSetting.ini"
#define CALC_FILE "res/config/calcsetting/DSCalcSetting.ini"
#define COMMENT_PATH "res/config/commentsetting/"
#define CALC_PATH "res/config/calcsetting/"
#define DEFAULT_CALC_FILE "res/config/DefaultCalcSetting.ini"
#define COMMENT_COPY "res/config/DefaultComment.ini"
#define USERCONFIG_PATH      "./res/config/userconfig/"  
#define DEFAULT_MEASURE_FILE "res/config/DefaultMeasureSetting.ini"
#define MEASURE_PATH "res/config/measuresetting/"
#define MEASURE_FILE "res/config/measuresetting/DSMeasureSetting.ini"
#endif
#endif


///> define new type
enum EKnobOper{SUB, ADD, ROTATE};

enum EKnobReturn{ERROR, OK, MIN, MAX, PRESS};

enum EDrawMode{COPY, XOR};


#ifdef VET
enum EStudy{ANIMAL, PART};
#else

#if (defined(EMP_322) || defined(EMP_313))
enum EStudy{ABD, OB, GYN, UR, CAR, SP, ORT, EXTRA};
#else
enum EStudy{ABD, OB, GYN, UR, CAR, SP, VES, ORT, EXTRA};
#endif
//enum EStudy{ABD, OB, GYN, UR, CAR, SP, VES, ORT, EXTRA};
#endif
typedef struct tagPOINT 
{
	int x;
	int y;
} POINT, *PPOINT;

/* The RGBTRIPLE structure describes a color consisting of relative intensities of red, green, and blue.
*/
typedef struct tagRGBTRIPLE 
{ 
  unsigned char rgbtRed;			//!< Specifies the intensity of red in the color.
  unsigned char rgbtGreen;			//!< Specifies the intensity of green in the color.
  unsigned char rgbtBlue;			//!< Specifies the intensity of blue in the color.
} RGBTRIPLE;
typedef EKnobReturn (*pKeyFunc)(EKnobOper);

//#define FEATURE 9101
#define FEATURE 13004

extern char user_configure[USERCONFIG_LEN]; 

#endif
