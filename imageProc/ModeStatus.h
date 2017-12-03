#ifndef MODE_STATUS_H
#define MODE_STATUS_H

#include "ScanMode.h"
#include "FreezeMode.h"
#include "Format2D.h"
#include "FormatM.h"
#include "FormatCfm.h"
#include "ImgPw.h"
#include "ImgProc2D.h"

/*
 * @brief manage system mode status
 */
class ModeStatus
{
	public:
		enum EKeyFunP{ON}; // need to be modify
		enum EKnobFunValue{NOFUN, DEPTH, ZOOM, FOCUS};

		ModeStatus(){}
		~ModeStatus(){}
		
		ScanMode::EScanMode GetScanMode();
		FreezeMode::EFreezeMode GetFreezeMode();
		Format2D::EFormat2D GetFormat2D();
		int Get2DCurrentB();
		FormatM::EFormatM GetFormatM();
		FormatPw::EFormatPw GetFormatPw();
		FormatCfm::EFormatCfm GetFormatCfm();
		int GetPwCurImg();
        int GetRotate();

		// judge
		static bool IsD2Mode();
		static bool IsMMode();
		static bool IsSpectrumMode();
		static bool IsColorMode();
		static bool IsPureColorMode();
		static bool IsSpectrumColorMode();
		static bool IsPwMode();
		static bool IsCFMMode();
		static bool IsPDIMode();
        static bool IsSpectrumSimult();
        static bool IsSpectrumColorSimult();
		static bool IsPWMode();
		static bool IsPWColorMode();
		static bool IsCWMode();
		static bool IsCWColorMode();

		static bool IsNormalMode();
		static bool IsAnatomicMode();
		static bool IsCFMCompareMode();
		static bool IsPDICompareMode();
		static bool IsSimultMode();

        static bool IsD2ImgMode();
        static bool IsMImgMode();
        static bool IsSpectrumImgMode();
        static bool IsCwImgMode();
        static bool IsColorImgMode();
        static bool IsSpectrumColorImgMode();
        //   static bool IsPWCFMImgMode();
        //   static bool IsPWPDIImgMode();
        static bool IsPWImgMode();
        static bool IsCWImgMode();
        static bool IsPWColorImgMode();
        static bool IsCWColorImgMode();
        static bool IsPWCFMImgMode();
        static bool IsPWPDIImgMode();
        static bool IsCWCFMImgMode();
        static bool IsCWPDIImgMode();
        static bool IsColorPDIMode();
        static bool IsCFMImgMode();
        static bool IsPDIImgMode();
        static bool IsColorVS2DMode();

		static bool IsFreezeMode();
		static bool IsUnFreezeMode();
		static bool IsPureFreezeMode();
		static bool IsAutoReplayMode();
		static bool IsReplayMode();

        static bool IsNormalSpectrumImgMode();
        static bool IsNormalSpectrumColorImgMode();

		static bool IsEFOVMode();

        // measure
        static bool IsMeasure2DMode(); 
        static bool IsMeasureMMode(); 
        static bool IsMeasureDMode(); 
        
        static bool IsAllSpectrumImgMode();

		// Knob type
#if 0
		bool isKnobD2();
		bool isKnobM();
		bool isKnobPw();
		bool isKnobCw();
		bool isKnobCfm();
		bool isKnobPdi();
#endif
};

///> inline defination
inline ScanMode::EScanMode ModeStatus::GetScanMode()
{
	ScanMode* m_ptrScan = ScanMode::GetInstance();
	return m_ptrScan->GetScanMode();
}

inline FreezeMode::EFreezeMode ModeStatus::GetFreezeMode()
{
	FreezeMode* ptrFreeze = FreezeMode::GetInstance();
	return ptrFreeze->GetFreezeMode();
}

inline Format2D::EFormat2D ModeStatus::GetFormat2D()
{
	Format2D* ptrImg = Format2D::GetInstance();
	return ptrImg->GetFormat();
}

inline int ModeStatus::Get2DCurrentB()
{
	Format2D* ptrImg = Format2D::GetInstance();
	return ptrImg->GetCurrentB();
}
inline FormatM::EFormatM ModeStatus::GetFormatM()
{
	FormatM* ptrImg = FormatM::GetInstance();
	return ptrImg->GetFormat();
}
inline FormatPw::EFormatPw ModeStatus::GetFormatPw()
{
	FormatPw* ptrFormat = FormatPw::GetInstance();
	return ptrFormat->GetFormat();
}

inline FormatCfm::EFormatCfm ModeStatus::GetFormatCfm()
{
	FormatCfm* ptrFormat = FormatCfm::GetInstance();
	return ptrFormat->GetFormat();
}

inline int ModeStatus::GetPwCurImg()
{
	return ScanMode::GetInstance()->GetPwCurImg();
}

inline int ModeStatus::GetRotate()
{
    return ImgProc2D::GetInstance()->GetRotate();
}

#endif
