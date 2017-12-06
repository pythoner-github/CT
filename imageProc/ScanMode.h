#ifndef SCAN_MODE_H
#define SCAN_MODE_H

#include "imageControl/FpgaCtrl2D.h"
#include "imageControl/Img2D.h"
#include "imageControl/ImgPw.h"
#include "imageControl/ImgCfm.h"
#include "imageProc/Format2D.h"
#include "imageProc/FormatM.h"
#include "imageProc/FormatPw.h"
#include "imageProc/FormatCfm.h"
#include "AbsUpdate2D.h"
#include "AbsUpdatePw.h"
#include "AbsUpdateCfm.h"
#include "keyboard/MultiFuncFactory.h"
#include "imageProc/Replay.h"
#include <UIS4DReceiveInterface.h>

class ScanMode
{
public:

    ///> new type
    enum EScanMode{D2, M_INIT, M, PW_INIT, PW, PW_SIMULT, CW_INIT, CW, CFM, PDI, PWCFM_INIT, PWCFM, PWCFM_SIMULT, PWPDI_INIT, PWPDI, PWPDI_SIMULT, CWCFM_INIT, CWCFM, CWPDI_INIT, CWPDI, CFM_VS_2D, PDI_VS_2D, ANATOMIC_M, EFOV}; ///< scan mode, "PWCFM"=PW+CFM, "CWPDI"=CW+PDI,
    enum EFOVStatus{PREPARE, CAPTURE, VIEW, REVIEW};

    struct SpecialMeasurePara
    {
        EScanMode mode;
        Format2D::EFormat2D format2D;
        FormatCfm::EFormatCfm formatCfm;
        FormatPw::EFormatPw formatPw;
        FormatM::EFormatM formatM;
        double scale2DAll[4];
        double scale2D;
        double scaleMDepth;
        double scaleMTime;
        double scalePwVel;
        double scalePwTime;
        int baselineCalc;
    };

    ~ScanMode();
    static ScanMode* GetInstance();

    void Enter2D();

    void EnterM(); ///< enter initial M mode, 2D + MCursor
    void UpdateBM(); ///< enter real M mode from initial M mode
    void UpdateM(); ///< enter real M mode from initial M mode

    void EnterPw();
    void UpdatePw();
    int SwitchPw();

    void EnterCfm();
    void EnterPdi();

    void Enter4D();

    void EnterPwCfmFromCfm();
    void UpdatePwCfmFromInit();
    int SwitchPwCfm();

    void EnterPwPdiFromPdi();
    void UpdatePwPdiFromInit();
    int SwitchPwPdi();

    EKnobReturn EnterPwSimult(bool on);
    EKnobReturn EnterPwCfmSimult(bool on);
    EKnobReturn EnterPwPdiSimult(bool on);

    void PrepareEnterCw(void);
    void EnterCw();
    void UpdateCw();
    int SwitchCw();

    void EnterCwCfmFromCfm();
    void UpdateCwCfmFromInit();
    int SwitchCwCfm();

    void EnterCwPdiFromPdi();
    void UpdateCwPdiFromInit();
    int SwitchCwPdi();

    void EnterHPRF(bool on);
    void EnterCfmVs2D();
    void EnterPdiVs2D();

    void EnterAnatomicM();

    ScanMode::EScanMode GetScanMode()
    {
        if (m_isSpecialMeasure)
            return m_scanModeMeasure;
        else
            return m_scanMode;
    }
    ScanMode::EScanMode GetPreScanMode()
    {
        if (m_isSpecialMeasure)
            return m_scanModeMeasure;
        else
            return m_preScanMode;
    }

    ScanMode::EScanMode GetFpgaScanMode() { return m_fpgaScanMode; }
    int GetPwCurImg() { return m_pwCurImg; } //1-2d, 2-pw, 3-both 2d and pw

    void EnterEFOV(void);
    void EnterEFOVPrepare(void);
    void EnterEFOVCapture(void);
    void EnterEFOVView(void);
    void EnterEFOVReview(void);
    EFOVStatus GetEFOVStatus();

    ///> spetial measure(example: measure on snap image)
    void GetSpecialMeasurePara(SpecialMeasurePara *para);
    void EnterSpecialMeasure(SpecialMeasurePara para);
    void ExitSpecialMeasure(void);
    bool IsSpecialMeasureStatus(void);
    void SetPWPulseNum(void);

private:
    ScanMode();

    static ScanMode* m_ptrInstance;
            AbsUpdatePw* m_ptrUpdate;
    // extern
    FpgaCtrl2D m_fpgaCtrl2D;

    AbsUpdate2D* m_ptrUpdate2D;
    Img2D* m_ptrImg2D;
    Format2D* m_ptrFormat2D;
    FormatM* m_ptrFormatM;

    AbsUpdatePw* m_ptrUpdatePw;
    ImgPw* m_ptrImgPw;
    FormatPw* m_ptrFormatPw;

    AbsUpdateCfm* m_ptrUpdateCfm;
    ImgCfm* m_ptrImgCfm;
    FormatCfm* m_ptrFormatCfm;
    MultiFuncFactory* m_ptrMultiFuncFactory;

    DSCCONTROLATTRIBUTES* m_ptrDscPara;
    Replay* m_ptrReplay;

    // internal
    EScanMode m_scanMode;
    EScanMode m_preScanMode;
    EScanMode m_fpgaScanMode;

    int m_workMode;
    int m_packetSize;
    int m_pulseNum;

    ///>test
    int m_pulseNumTmp;

        ///> 0-pw and 2D both real
    ///> 1-2D real
    ///> 2-pw real
    int m_pwCurImg;

    ///> special measure
    bool m_isSpecialMeasure;
    EScanMode m_scanModeMeasure;
public:
    void SetScanMode(EScanMode mode);
    void DarkAllModeLight();

    ///> test
    void SetMScanMode();

    ///>test
    ///>func
    void ChangePulseNum(EKnobOper oper);
    int GetCFMPulseNum() { return m_pulseNumTmp; }
};
#endif