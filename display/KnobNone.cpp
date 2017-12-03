/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: KnobNone.cpp
 * @brief: knob in no knob func 
 *
 * version: V1.0
 * date: 2009-11-30
 * @author: zhanglei
 */

#include "KnobMenu.h"
#include "KnobNone.h"
#include "KnobReplay.h"
#include "KnobLoadSnap.h"
#include "Knob2D.h"
#include "KnobM.h"
#include "KnobPw.h"
#include "KnobCfm.h"
#include "ModeStatus.h"
#include "ScanMode.h"
#include "../imageControl/KnobEFOV.h"
#ifdef EMP_322
KnobMenu::KnobItem KnobNoneMenu[6] = {
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
};	
#else
KnobMenu::KnobItem KnobNoneMenu[5] = {
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
};	
#endif
void KnobNoneCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobNoneMenu, sizeof(KnobNoneMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::NONE);   
}


void KnobUndo()
{
	ModeStatus ms;
	ScanMode::EScanMode mode = ms.GetScanMode();

    if ((mode == ScanMode::EFOV) && !(Replay::GetInstance()->GetReadSnapStatus())) {
        ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();
        switch (status) {
            case ScanMode::PREPARE:
                KnobEFOVPrepareCreate();
                break;
            case ScanMode::CAPTURE:
                KnobEFOVCaptureCreate();
                break;
            case ScanMode::VIEW:
                KnobEFOVViewCreate();
                break;
            case ScanMode::REVIEW:
                KnobEFOVReviewCreate();
                break;
        }
        return ;
    }

    if (ms.GetFreezeMode() == FreezeMode::UNFREEZE) {
        switch(mode) {
        case ScanMode::D2:
        case ScanMode::M_INIT:
            KnobD2Create();
            break;

        case ScanMode::M:
            KnobMCreate();
            break;

        case ScanMode::PW_INIT:
        case ScanMode::PW:
        case ScanMode::PW_SIMULT:
        case ScanMode::CW_INIT:
        case ScanMode::CW:
        case ScanMode::PWCFM_INIT:
        case ScanMode::PWCFM:
        case ScanMode::PWCFM_SIMULT:
        case ScanMode::PWPDI_INIT:
        case ScanMode::PWPDI:
        case ScanMode::PWPDI_SIMULT:
            KnobPwCreate();
            break;

        case ScanMode::CFM:
        case ScanMode::CFM_VS_2D:
        case ScanMode::PDI:
        case ScanMode::PDI_VS_2D:
            KnobCfmCreate();
            break;

        case ScanMode::ANATOMIC_M:
            KnobAnatomicMCreate();
            break;

        default:
            KnobD2Create();
            break;
        }
    } else {
		if(Replay::GetInstance()->GetReadSnapStatus())
			KnobLoadSnapCreate();
		else
			KnobReplayCreate();
    }
}


