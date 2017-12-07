#include "display/KnobMenu.h"
#include "display/KnobNone.h"
#include "imageProc/KnobReplay.h"
#include "patient/KnobLoadSnap.h"
#include "imageControl/Knob2D.h"
#include "imageControl/KnobM.h"
#include "imageControl/KnobPw.h"
#include "imageControl/KnobCfm.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/ScanMode.h"
#include "imageControl/KnobEFOV.h"

#ifdef CT_322
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