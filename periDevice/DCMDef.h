/**
 * @brief DCMDef.h
 * @author hehao
 * @version 1.0
 * @date 2009-10-8
 *
 */

#ifndef DCMDEF_H
#define DCMDEF_H

#include <string>
#include <vector>
using namespace std;

namespace DCM
{
	typedef struct tagRECT
	{
		unsigned int left;				//!< Specifies the x-coordinate of the upper-left corner of the rectangle.
		unsigned int top;					//!< Specifies the y-coordinate of the upper-left corner of the rectangle.
		unsigned int right;				//!< Specifies the x-coordinate of the lower-right corner of the rectangle.
		unsigned int bottom;				//!< Specifies the y-coordinate of the lower-right corner of the rectangle.
	} RECT, *PRECT;


	enum EDCMPrintColor { GRAYSCALE, COLOR };
	enum EDCMPriorities { HIGH, MIDDLE, LOW };
	enum EDCMPrintFormat { STD1X1, STD1X2, STD2X2, STD2X3, STD3X3, STD3X4, STD3X5, STD4X4, STD4X5, STD4X6 };
	enum EDCMOrientation { PORTRAIT, LANDSCAPE };
	enum EDCMMagnification { REPLICATE, BILINEAR, CUBIC, MAGNIFICATION_NONE };
	enum EDCMMediumType { PAPER, CLEARFILM, BLUEFILM };
	enum EDCMFilmSize { SIZE8INX10IN, SIZE10INX12IN, SIZE10INX14IN, SIZE11INX14IN, SIZE14INX14IN, SIZE14INX17IN, SIZE24CMX24CM, SIZE24CMX30CM, SIZEA4 };
	enum EDCMDestination { MAGAZINE, PROCESSOR };
	enum EDCMPolarity { NORMAL, REVERSE, POLARITY_UNKNOWN };
	enum EDCMDensityType { BLACK, WHITE, NUMBER, DENSITY_UNKNOWN };
	enum EDCMRequestedResolutionID { RRID_STANDARD, RRID_HIGH, RRID_NONE };
}

typedef struct tagDCMStudyElement	
{
	string stSpecificCharacterSet;	///< from Worklist, given empty string if no.
	string stPatientName;
	string stPatientID;			///< patient id, each patient have unique value, not empty string.
	string stPatientBirthDate;	///< (YMD: 20070818)
	string stPatientSex;		///< F:Female; M:Male; O:Other
	string stPatientAge;		///< (nnnY/M/D: 018Y, 18 years old; 100Y, 100 years old; M is month, D is day)
	string stPatientSize;		///< Patient's height or length in meters.
	string stPatientWeight;		///< Weight of the patient in kilograms.
	string stStudyInstanceUID;	///< from Worklist, given empty string if no.
	string stStudyDescription;	///< study description, may be from Worklist, or user specify, given empty string if no.
	string stAccessionNumber;	///< from Worklist, given empty string if no.
	string stStudyDate;			///< (YMD: 20070818)
	string stStudyTime;			///< (HMS: 093654)
	string stStudyDoctor;		///< study doctor
	string stFactoryName;		///< factory name
	string stHospital;			///< hospital name
	string stMechineType;		///< as EMP-2800
	string stMechineSN;			///< mechine serial number
	unsigned int stStudyNo;		///< study number, generate by mechine, each study have unique value.
} DCMSTUDYELEMENT, *PDCMSTUDYELEMENT;

typedef struct tagDCMImageElement
{
	unsigned int imgHeight;			///< height of image
	unsigned int imgWidth;			///< width of image
	unsigned int imgBitCount;		///< bit count of image, as 8 or 24
	unsigned int imgFrameNumber;	///< frame number of image, single frame=1, multiplay frame>1
	string imgFilename;				///< filename of image saved, include extension, but exclude path, <256char
	DCM::RECT imgCalibrationRegion;	///< the rectange coordinate of measureable image region, relative to entire image.
	double imgScale;				///< the scale of image, depth/pixels, unit:cm/pixel.
	string imgStudyPart;			///< SKULL, CSPINE, TSPINE, LSPINE, SSPINE, COCCYX, CHEST, CLAVICLE, BREAST, ABDOMEN, PELVIS, HIP, SHOULDER, ELBOW, KNEE, ANKLE, HAND, FOOT, HEAD, HEART, NECK, LEG, ARM, JAW
} DCMIMAGEELEMENT, *PDCMIMAGEELEMENT;

typedef struct tagDCMSRElement
{
	string srIndication;
	string srComments;
	vector<string> srReferenceImageFilename;
} DCMSRELEMENT, *PDCMSRELEMENT;

typedef struct tagDCMWorklistElement
{
	string wlSpecificCharacterSet;
	string wlPatientName;
	string wlPatientID;
	string wlPatientBirthDate;	///< (format:YYYYMMDD)
	string wlPatientSex;			///< F:Female; M:Male; O:Other
	string wlPatientAge;			///< (format:nnnD/nnnW/nnnM/nnnY)
	string wlPatientSize;			///< Patient's height or length in meters.
	string wlPatientWeight;		///< Weight of the patient in kilograms.
	string wlPatientAddress;
	string wlRequestedProcedureID;
	string wlStudyInstanceUID;
	string wlAccessionNumber;
	string wlStudyDescription;	///< the description of study part
	string wlScheduleDate;		///< (format:YYYYMMDD)
	string wlStudyDoctor;
	string wlRequestedProcedureDescription;	///< for MPPS
	string wlScheduledProcedureStepID;		///< for MPPS
} DCMWORKLISTELEMENT, *PDCMWORKLISTELEMENT;

typedef struct tagDCMMPPSElement
{
	string msSpecificCharacterSet;	///< from Worklist, given empty string if no.
	string msPatientName;
	string msPatientID;
	string msPatientBirthDate;	///< (format:YYYYMMDD)
	string msPatientSex;			///< F:Female; M:Male; O:Other
	string msRequestedProcedureID;	///< from Worklist, given empty string if no.
	string msStudyInstanceUID;	///< from Worklist, given empty string if no.
	string msAccessionNumber;	///< from Worklist, given empty string if no.
	string msStudyDescription;	///< from Worklist, given empty string if no.
	string msRequestedProcedureDescription;	///< from Worklist, given empty string if no.
	string msScheduledProcedureStepID;		///< from Worklist, given empty string if no.
	string msStudyStartDate;					///< (format:YYYYMMDD)
	string msStudyStartTime;					///< (format:HHMMSS)
} DCMMPPSELEMENT, *PDCMMPPSELEMENT;

typedef struct tagDCMQRQueryRes
{
	string qrPatientName;
	string qrPatientID;			///< patient id, each patient have unique value, not empty string.
	string qrPatientBirthDate;	///< (YMD: 20070818)
	string qrPatientSex;		///< F:Female; M:Male; O:Other
	string qrPatientAge;		///< (nnnY/M/D: 018Y, 18 years old; 100Y, 100 years old; M is month, D is day)
	string qrPatientSize;		///< Patient's height or length in meters.
	string qrPatientWeight;		///< Weight of the patient in kilograms.
	string qrAccessionNumber;	///< from Worklist, given empty string if no.
	string qrStudyDate;			///< (YMD: 20070818)
	string qrStudyTime;			///< (HMS: 093654)
	string qrStudyDoctor;		///< study doctor
	string qrStudyDescription;
	unsigned int qrItemIndex;
} DCMQRQUERYRES, *PDCMQRQUERYRES;

typedef struct tagDCMDICOMDIRFILELEVEL
{
	string levelName;
	int index;
	string filePath;
} DCMDICOMDIRFILELEVEL, *PDCMDICOMDIRFILELEVEL;

typedef struct tagDCMDICOMDIRSERIESLEVEL
{
	string levelName;
	int index;
	string type;
	vector<DCMDICOMDIRFILELEVEL> vecFileLevel;
} DCMDICOMDIRSERIESLEVEL, *PDCMDICOMDIRSERIESLEVEL;

typedef struct tagDCMDICOMDIRSTUDYLEVEL
{
	string levelName;
	int index;
	string patientName;
	string patientID;
	string studyDate;		///< (YMD: 20070818)
	string studyTime;		///< (HMS: 093654)
	string accessionNumber;
	string studyDescription;
	string studyInstanceUID;
	vector<DCMDICOMDIRSERIESLEVEL> vecSeriesLevel;
} DCMDICOMDIRSTUDYLEVEL, *PDCMDICOMDIRSTUDYLEVEL;

typedef struct tagDCMPrintConfigElement
{
	DCM::EDCMPrintColor pcColor;
	DCM::EDCMPrintFormat pcFormat;
	DCM::EDCMOrientation pcOrientation;
	DCM::EDCMMagnification pcMagnification;
	char pcMinDensity[10];
	char pcMaxDensity[10];
	DCM::EDCMMediumType pcMediumType;
	DCM::EDCMFilmSize pcFilmSize;
	DCM::EDCMDestination pcDestination;
	DCM::EDCMPriorities pcPriorities;
	char pcCopies;
	char pcConfigurationInformation[101];
} DCMPRINTCONFIGELEMENT, *PDCMPRINTCONFIGELEMENT;

typedef struct tagDCMPrintImageInfo
{
	unsigned int index;					///< base on 0;
	string imageFilePath;		///< absolute path.
} DCMPRINTIMAGEINFO, *PDCMPRINTIMAGEINFO;

enum EDCMReturnStatus {DCMSUCCESS, DCMNODEFALUTSERVER, DCMCONNECTIONFAILURE, DCMNONSUPPORTSTUDYROOT, DCMINVALIDVALUE, DCMINVALIDQRDESTSERVER, DCMRETRIEVEFAILURE, DCMSTUDYEXISTED, DCMINVALIDSTORAGEDEVICE, DCMEXPORTFAILURE, DCMNOENOUGHSPACE, DCMNONEXISTDICOMDIR, DCMNONENTIREDICOMDIR, DCMIMPORTFAILURE, DCMNOPRINTIMAGE, DCMPRINTFAILURE};

typedef void (*GetSingleServerAttribute)(string device, string ip, void *pData);
typedef void (*GetSingleServiceAttribute)(string device, string serviceName, string aeTitle, int port, bool isDefault, void *pData);

/**
 * @param nPos current position of progress, the total value is 100.
 */
typedef void (*PROGRESSSTATUS)(int nPos);	

/**
 *@return return studyNo.
 */
typedef unsigned int (*GetDCMStudyElement)(DCMSTUDYELEMENT element);

/**
 *@param progressPos the progress pos of retrieve current image or/and sr.
 *@return return image filename.
 */
typedef string (*GetDCMImageElement)(DCMIMAGEELEMENT element, unsigned char *pImageBuf, unsigned long bufLen);

/**
 *@param progressPos the progress pos of retrieve current image or/and sr.
 */
typedef void (*GetDCMSRElement)(DCMSRELEMENT element);
	
#endif //DCMDEF_H

