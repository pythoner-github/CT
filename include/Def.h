#ifndef __DEF_H__
#define __DEF_H__

#include <opencv/cv.h>
#include <stdio.h>
#include <cassert>

//#define CT_340//G70
//#define CT_360//G60
//#define CT_161//CT-3000

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

#define BYTES_DOT 3 //每个像素点用3个字节表示
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
#define UDISK_DATA_PATH "/mnt/udisk/ct-data"
//#define UDISK_DATA_PATH "/media/SONY/ct-dicom"
#define UPGRADE_DEST_PATH "/mnt/harddisk/ct"
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

#define SYS_SETTING_FILE "res/config/SysSetting.ini"
#define USER_PATH  "./res/config/username"
#define KB_CAL_SETTING_FILE "res/config/KbCalcSetting.ini"
#define STORE_ITEM_PATH "res/config/SelectProbeItem.ini"
#define BIOPSY_SETTING_FILE "res/config/BiopsySetting.ini"
#define DEFAULT_SETTING_FILE "res/config/DefaultBiopsySetting.ini"
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

///> define new type
enum EKnobOper{SUB, ADD, ROTATE};

enum EKnobReturn{ERROR, OK, MIN, MAX, PRESS};

enum EDrawMode{COPY, XOR};

#ifdef VET
enum EStudy{ANIMAL, PART};
#else

#if (defined(CT_322) || defined(CT_313))
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
  unsigned char rgbtRed;            //!< Specifies the intensity of red in the color.
  unsigned char rgbtGreen;          //!< Specifies the intensity of green in the color.
  unsigned char rgbtBlue;           //!< Specifies the intensity of blue in the color.
} RGBTRIPLE;
typedef EKnobReturn (*pKeyFunc)(EKnobOper);

//#define FEATURE 9101
#define FEATURE 13004

extern char user_configure[USERCONFIG_LEN];

#endif