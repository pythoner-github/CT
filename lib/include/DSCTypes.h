/**
* @file DSCTypes.h
* @brief define types for the DSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef DSCTYPES_H
#define DSCTYPES_H

#include "Def.h"

/** @brief A struct type.
* This structure defines the coordinates of the upper-left and lower-right corners of a rectangle.
*/
typedef struct tagRECT 
{ 
  unsigned int left;				//!< Specifies the x-coordinate of the upper-left corner of the rectangle.
  unsigned int top;					//!< Specifies the y-coordinate of the upper-left corner of the rectangle.
  unsigned int right;				//!< Specifies the x-coordinate of the lower-right corner of the rectangle.
  unsigned int bottom;				//!< Specifies the y-coordinate of the lower-right corner of the rectangle.
} RECT, *PRECT; 

/** @brief A struct type. 
* The TRACE structure defines the attributes of trace.
*/
typedef struct tagTRACE
{
	bool status;					//!< Specifies the status of trace, true:turn on trace, false:turn off trace.
	unsigned char type;				//!< Specifies the type of trace, 0:Max, 1:Mean, 2:Scatter.
	unsigned char direction;		//!< Specifies the direction of trace, 0:above, 1:below, 2:both.
	unsigned char threshold;		//!< Specifies the threshold of trace, value:0~4.
	unsigned char smooth;			//!< Specifies the smooth of trace, value:0~2.
	char baseline;					//!< Specifies the position of baseline, >=-128 && <=127.
	char fs;						//!< Specifies the MIT cut off frequency.
	unsigned char noiseThreshold;	//!< Specifies the noise threshold.
	unsigned char gain;				//!< Specifies the pw gain.
	unsigned char dynRange;			//!< Specifies the pw dynamic range.
	unsigned char reserve1;			//!< not define 
	unsigned char reserve2;			//!< not define 
	float interval;					//!< Specifies the interval between two adjacent pw lines, in second.
} TRACE, *PTRACE;

#if 0
/** @brief A struct type. 
* The POINT structure defines the x- and y- coordinates of a point.
*/
typedef struct tagPOINT
{
	unsigned int x;					//!< Specifies the x-coordinate of the point.
	unsigned int y;					//!< Specifies the y-coordinate of the point.
} POINT, *PPOINT;

#endif
#if 0
/** @brief A struct type.
* The RGBTRIPLE structure describes a color consisting of relative intensities of red, green, and blue.
*/
typedef struct tagRGBTRIPLE 
{ 
  unsigned char rgbtRed;			//!< Specifies the intensity of red in the color.
  unsigned char rgbtGreen;			//!< Specifies the intensity of green in the color.
  unsigned char rgbtBlue;			//!< Specifies the intensity of blue in the color.
} RGBTRIPLE;
#endif

/** @brief A struct type.
* This structure defines the attributes of image process in 2D.*/
typedef struct tagIP2DAttributes
{
	bool ipaPolarityOverturn;		//!< false:no overturn, true:overturn
	bool ipaUDOverturn;				//!< ibid
	bool ipaLROverturn;				//!< ibid
	unsigned char ipaRotate;		//!< 0:0, 1:90, 2:180, 3:270; it only use for mode B.
	unsigned char ipaFrameAver;		//!< value: 64,56,48,40,32,28,24,16
	unsigned char ipaLineAver;		//!< value: 64,56,48,44,40,32,24,16
	bool ipaPIPZoom;				//!< false:OFF, true:ON; it only use for mode B.
	bool ipaGlobalZoom;				//!< ibid
	bool ipaLocalZoom;				//!< ibid
	unsigned char ipaEdgeEhn;		//!< edge enhance, value:0~7
	unsigned char ipaSmooth;		//!< Smooth, value:0~7
	unsigned char ipaReject;		//!< noise reject, 0:off, 1:on.
	unsigned char ipaImgEhn;		//!< level of image enhance, value:0~3, 0:off.
	unsigned char ipaMeanCompareFilterThreshold;		//!< threshold, default value:35, 0:off.
	unsigned char ipaReserved1;		//!< not define
	unsigned char ipaReserved2;		//!< not define
} IP2DATTRIBUTES, *PIP2DATTRIBUTES;

/** @brief A struct type.
* This structure defines the attributes of image process in CFM.*/
typedef struct tagIPCFMAttributes
{
	unsigned char ipaThreshold;		//!< 0~255, it must be 255 or 0 when chroma is enabled. 
	unsigned char ipaFrameAver;		//!< value: 64,56,48,40,32,28,24,16
	bool ipaHideBackground;			//!< false:OFF, true:ON.
	unsigned char ipaSmooth;		//!< Smooth, value:0~7
	unsigned char ipaArtifact;		//!< artifact, 0~6, 0:OFF.
	unsigned char ipaReserved1;		//!< not define
	unsigned char ipaReserved2;		//!< not define
	unsigned char ipaReserved3;		//!< not define
} IPCFMATTRIBUTES, *PIPCFMATTRIBUTES;

/** @brief A struct type,
* This structure defines the attributes of image process in PW.*/
typedef struct tagIPPWAttributes
{
	TRACE ipaTrace;					//!< attributes of trace
	bool ipaUDOverturn;				//!< false:no overturn, true:overturn
	unsigned char ipaReserved1;		//!< not define
	unsigned char ipaReserved2;		//!< not define
	unsigned char ipaReserved3;		//!< not define
} IPPWATTRIBUTES, *PIPPWTTRIBUTES;

typedef struct tagIPMAttributes
{
	unsigned char ipaSmooth;			//!< Smooth, value:0~7
	unsigned char ipaEnhance;			//!< M enhance, value:0~3
	unsigned char ipaReserved1;			//!< not define
	unsigned char ipaReserved2;			//!< not define
} IPMATTRIBUTES, *PIPMTTRIBUTES;

/** @brief A struct type.
* This structure defines all attributes of DSC.*/
typedef struct tagDSCControlAttributes
{
	unsigned int dcaProbeWidth;			//!< probe width
	unsigned int dcaCurMaxScanLines;	//!< 256/128/192/96, relate to probe arraynum.
	unsigned int dcaCurScanStart;		//!< 0~255	
	unsigned int dcaCurScanEnd;			//!< 0~255 End>Start
	unsigned int dcaSamplePoints;		//!< 1~480
	float dcaScale;						//!< display depth/samppoints
	float dcaSteer;						//!< steer angle, +/-x:x degree. and angle in tp-view , only used in linear probe
	int dcaTpView;						//!< if or not in tp view mode. only used in linear probe 1-tpView, 0-normal
	float dcaProbeR;			//!< probe radius
	unsigned char dcaProbeType;			//!< now:C,L,T,P
	unsigned char dcaB4Flag;			//!< 0,1,2,3
	bool dcaBBFlag;						//!< false:left dsc;  true:right dsc;
	unsigned char dcaReserved6;			//!< not define
	RGBTRIPLE dcaPalette[256];			//!< the palette of grayscale bitmap, it lies on gamma, Gray Transform, 
										//!< Gray Rejection and chroma, dcaPalette[0] must be (0,0,0).
	IP2DATTRIBUTES dca2DIPAttrs;		//!< 2D image process attributs

	unsigned int dcaCFMScanLStart;		//!< 0~255, it must be divided by 2 when two beam, by 4 when four beam. 	
	unsigned int dcaCFMScanLEnd;		//!< 0~255, End>Start, it must be divided by 2 when two beam, by 4 when four beam.
	unsigned int dcaCFMSampPtStart;		//!< 0~479
	unsigned int dcaCFMSampPtEnd;		//!< 0~479 End>Start
	unsigned int dcaCFMLDensity;		//!< 1:single beam, 2:two beam, 4:four beam
	int dcaCFMSteer;					//!< steer angle, +/-x:x degree.
	int dcaCFMBaseline;					//!< baseline of CFM, 0~255, current==128
	int dcaCFMIsDirection;				//!< If the color flow have direction, 0:no, 1:yes, used for PDI and CFM
	RGBTRIPLE dcaCFMPalette[256];		//!< the palette of CFM.
	IPCFMATTRIBUTES dcaCFMIPAttrs;		//!< CFM image process attributs

	unsigned int dcaPWSamplePoints;		//!< <=460, current value=256
	unsigned int dcaPWStartPosAxisY;	//!< start pos of Spectrum in y-axis, in UD format, range:0~(IHEIGHT/2).
	unsigned int dcaPWSpeed;			//!< the number of lines each refresh, <=IWIDTH/2 and >0
	unsigned char dcaPWFlag;			//!< real-time state, 0:B and PW, 1:only B, 2:only PW
	unsigned char dcaPWDisplayFormat;	//!< 0:full PW, 1:UBDPW1/1, 2:UBDPW1/2, 3:UBDPW2/1, 4:LBRPW1/1.	
	unsigned char dcaReserved1;			//!< not define
	unsigned char dcaReserved2;			//!< not define
	IPPWATTRIBUTES dcaPWIPAttrs;		//!< PW image process attributs

	unsigned int dcaMSpeed;				//!< the number of lines each refresh, <=IWIDTH/2 and >0
	unsigned char dcaMDisplayFormat;	//!< 0:M, 1:UBDM1/1, 2:UBDM1/2, 3:UBDM2/1, 4:LBRM1/1.
	unsigned char dcaMSampleLine;			//!< the position of m sample line.
	unsigned char dcaInsertMLines;			//!< the M line count inserted between consecutive M lines. range:0~32
	unsigned char dcaReserved5;			//!< not define
	IPMATTRIBUTES dcaMIPAttrs;			//!< M image Process attributs
} DSCCONTROLATTRIBUTES, *PDSCCOTROLATTRIBUTES;
 

/** @brief A function pointer type.
* It update the image area of window, it define in window class, and transfer in DSC class.
*
* @param pBits datas of image, order:RGB, from left to right, from up to down
* @param nWidth the width of image.
* @param nHeight the Height of image.
*/
typedef void (*UPDATEFRAME) (void *pBits, int nWidth, int nHeight); 

const int IWIDTH = IMG_W; //628;				//!< constant, the width of image.
const int IHEIGHT = IMG_H; //440;			//!< constant, the height of image.
//const float PI = 3.1415926f;		//!< constant, PI.

/** @brief A enum type.
*/
enum ETraceDirection{ UP, DOWN };

/** @brief A template type
 */
template<typename T>
struct EFOVPOINT
{
    T x;
    T y;
};

/** @brief A template type
 */
template<typename T>
struct EFOVRECT
{
    EFOVPOINT <T>  leftup;
    EFOVPOINT <T>  leftdown;
    EFOVPOINT <T>  rightup;
    EFOVPOINT <T>  rightdown;
};

#endif	//DSCTYPES_H

