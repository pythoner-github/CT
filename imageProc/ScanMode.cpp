#include "imageProc/ScanMode.h"
#include "imageControl/FpgaCtrl2D.h"
#include "Def.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/DscMan.h"
#include "keyboard/LightDef.h"
#include "keyboard/KeyFunc.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcPw.h"
#include "measure/DrawHistogram.h"
#include "display/ViewIcon.h"
#include "imageProc/Zoom.h"
#include <UIS4DReceiveInterface.h>
#include "imageControl/D4FuncMan.h"
#include "display/ViewDialog.h"
#include "sysMan/ViewSystem.h"
#include "ViewMain.h"
#include "sysMan/SysOptions.h"
#include "keyboard/LightDef.h"
#include "keyboard/KeyValueOpr.h"
#include "imageControl/UsbControl.h"
#include "calcPeople/MenuCalcNew.h"
#include "measure/MenuMeasure.h"
extern bool g_init;
extern int s_fdcom;
ScanMode* ScanMode::m_ptrInstance = NULL;
extern CUIS4DReceiveInterface g_uis4d_ri;
GIOChannel *keyboard_channel = 0;
extern guint input_handle;

ScanMode* ScanMode::GetInstance()
{
    if ( m_ptrInstance == NULL)
        m_ptrInstance = new ScanMode;
    return m_ptrInstance;
}

ScanMode::ScanMode()
{
    GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
    m_ptrUpdate2D = ptrGcm->GetUpdate2D();
    m_ptrUpdatePw = ptrGcm->GetUpdatePw();
    m_ptrUpdateCfm = ptrGcm->GetUpdateCfm();

    m_ptrImg2D = Img2D::GetInstance();
    m_ptrImgPw = ImgPw::GetInstance();
    m_ptrImgCfm = ImgCfm::GetInstance();

    m_ptrFormat2D = Format2D::GetInstance();
    m_ptrFormatM = FormatM::GetInstance();
    m_ptrFormatPw = FormatPw::GetInstance();
    m_ptrFormatCfm = FormatCfm::GetInstance();

    m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
    m_ptrMultiFuncFactory = MultiFuncFactory::GetInstance();
    m_ptrReplay = Replay::GetInstance();

    m_scanMode = D2;
    m_preScanMode = D2;
    m_fpgaScanMode = D2;

    m_workMode = 0;
    m_packetSize = 0;
    m_pulseNum = 0;

    m_pwCurImg = 0;
    m_isSpecialMeasure = FALSE;

    m_pulseNumTmp = 4;
}

ScanMode::~ScanMode()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

/*
 * @brief enter scan mode 2D from other mode
 */
void ScanMode::Enter2D()
{
    ///> exit simult
    if (ImgPw::GetInstance()->GetSimut2Status())
        ImgPw::GetInstance()->ChangeSimult2(FALSE);
    if (ImgPw::GetInstance()->GetSimut3Status())
        ImgPw::GetInstance()->ChangeSimult3(FALSE);
#ifdef CT_355
    m_ptrImgPw->SetSpectrumModeCW(TRUE);
#endif

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::NONE);

    ///> set mode to 2d
    m_scanMode = D2;
    SetScanMode(m_scanMode);

    m_ptrImg2D->Enter2D();
    m_ptrImgPw->SetBalanceStatus(FALSE);
    m_ptrImgCfm->SetBalanceStatus(FALSE);

    ///> set format to B
    m_ptrFormat2D->ChangeFormat(Format2D::B);

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    ///> update 2d image view
    m_ptrUpdate2D->Enter2DMode();

    ///> control light
#if not defined (CT_322)
#if not defined (CT_313)
    DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE, LIGHT_D2);
#endif
#endif
    ///> change tis
    ChangeTis();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();

  }

/*
 * @brief enter scan mode initial M(2D + MLine) from other mode
 */
void ScanMode::EnterM()
{
    Enter2D();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

    // send work mode
    m_scanMode = M_INIT;
    SetScanMode(m_scanMode);

    // set rotate to 0
    ImgProc2D::GetInstance()->SetRotate(0, MIN);

    ///> reset bm line
    m_ptrImg2D->ResetMLine();
    m_ptrUpdate2D->EnterMMode();
#if not defined (CT_322)
#if not defined (CT_313)
    ///> control light
    DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE, LIGHT_M);
#endif
#endif
}

/*
 * brief enter scan mode real M from other mode, B and M always act the same way
*/
void ScanMode::UpdateBM()
{
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

    // set scan mode
    m_scanMode = M;
    SetScanMode(m_scanMode);

    //m_ptrImg2D->Enter2D();
     m_ptrImg2D->EnterM();

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::M);

    // set format to UD11
    //m_ptrFormatM->ChangeFormat(FormatM::BM11_LR); //only for test
    ///> set format to M
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatM();
    FormatM::GetInstance()->ChangeFormat(FormatM::EFormatM(format));

    // update
    m_ptrUpdate2D->UpdateM();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

/*
 * @brief enter scan mode real M from other mode, B and M always act the same way
 */
void ScanMode::UpdateM()
{
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

    // set scan mode
    m_scanMode = M;
    SetScanMode(m_scanMode);

    m_ptrImg2D->EnterM();

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::M);

    ///> set format to M
    //m_ptrFormatM->ChangeFormat(FormatM::M_TOTAL); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatM();
    FormatM::GetInstance()->ChangeFormat(FormatM::EFormatM(format));

    // update
    m_ptrUpdate2D->UpdateM();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

/*
 * @brief enter scan mode initial Pw(2D + PwLine) from other mode
 */
void ScanMode::EnterPw()
{
    Enter2D();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PW_INIT);

    ///> set scan mode
    m_scanMode = PW_INIT;
    SetScanMode(m_scanMode);

    // set rotate to 0
    ImgProc2D::GetInstance()->SetRotate(0, MIN);

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    ///> reset Pw line
    m_ptrImgPw->SetSpectrumModeCW(FALSE);
    m_ptrImgPw->ReSendSv();
    m_ptrUpdatePw->EnterPwMode();

    ///> control light
    DarkAllModeLight();

#if (defined(CT_340) || defined(CT_161) || defined(CT_360) || defined(CT_440) || defined(CT_430))
    g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
#endif

#ifdef CT_355
    int mode = ViewMain::GetInstance()->GetModeIsFlag();
    if(mode)
        g_keyInterface.CtrlLight(TRUE, LIGHT_CW);
    else
        g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
#endif
}

/*
 * @brief enter scan mode real Pw from other mode
 */
void ScanMode::UpdatePw()
{
    // set 2D is real
    m_pwCurImg = 1;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    m_ptrDscPara->dcaCFMIsDirection = 1;
    SetScanMode(D2);
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PW);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::PW);

    usleep(500000);

    // send mode to fpga
    m_scanMode = PW;
    SetScanMode(m_scanMode);
    m_ptrImgPw->SetBalanceStatus(FALSE);
    m_ptrImgCfm->SetBalanceStatus(FALSE);

    // set pw is real
    m_pwCurImg = 2;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;

    m_ptrImgPw->EnterPw();

    // set format to UD11 a
    //m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatPW();
    FormatPw::GetInstance()->ChangeFormat(FormatPw::EFormatPw(format));

    // update
    m_ptrUpdatePw->UpdatePwMode();

    // clear histogram
    if (DrawHistogram::GetInstance()->GetOnOff())
        DrawHistogram::GetInstance()->SetOnOff(0);

    ///> change tis
    ChangeTis();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

int ScanMode::SwitchPw()
{
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

    if (m_pwCurImg == 1) //B to pw
    {
        //enter pw mode
        SetScanMode(PW);

        // send to dsc
        m_pwCurImg = 2;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;

        ImgPw::GetInstance()->OnPwImgCtrl(TRUE);

    }
    else if (m_pwCurImg == 2) //pw to B
    {
        ImgPw::GetInstance()->OnPwImgCtrl(FALSE);
        // enter B mode
        SetScanMode(D2);

        // send to dsc
        m_pwCurImg = 1;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }

    ///> replay region
    if (m_pwCurImg == 1)
        m_ptrReplay->SwitchArea(1, unfreeze);
    else
        m_ptrReplay->SwitchArea(0, unfreeze);
    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> change tis
    ChangeTis();

    return m_pwCurImg;
}

/*
 * @brief enter scan mode cfm from other mode
 */
void ScanMode::EnterCfm()
{
    ///> exit simult
    if (ImgPw::GetInstance()->GetSimut2Status())
        ImgPw::GetInstance()->ChangeSimult2(FALSE);
    if (ImgPw::GetInstance()->GetSimut3Status())
        ImgPw::GetInstance()->ChangeSimult3(FALSE);

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CFM);
#ifdef CT_355
    m_ptrImgPw->SetSpectrumModeCW(TRUE);
#endif

    // set to CFM mode
    m_scanMode = CFM;
    SetScanMode(m_scanMode);

    ///> exit rotate
    ImgProc2D::GetInstance()->SetRotate(0, MIN);
    m_ptrImgPw->SetBalanceStatus(FALSE);
    m_ptrImgCfm->SetBalanceStatus(FALSE);

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    ///> set format to
    m_ptrImgCfm->EnterCfm();
    m_ptrFormatCfm->ChangeFormat(FormatCfm::B);

    ///> update 2d image view
    m_ptrUpdateCfm->EnterCfm();

    ///> control light
    DarkAllModeLight();
#if not defined (CT_322)
#if not defined (CT_313)
    g_keyInterface.CtrlLight(TRUE, LIGHT_CFM);
#endif
#endif
    ///> change tis
    ChangeTis();

#ifdef CT_355
    Img2D::GetInstance()->UpdateGain();  //解决调节M增益后，进入CFM时，增益错误的问题
#endif

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

/*
 * @brief enter scan mode pdi from other mode
 */
void ScanMode::EnterPdi()
{
    ///> exit simult
    if (ImgPw::GetInstance()->GetSimut2Status())
        ImgPw::GetInstance()->ChangeSimult2(FALSE);
    if (ImgPw::GetInstance()->GetSimut3Status())
        ImgPw::GetInstance()->ChangeSimult3(FALSE);

    ///> exit rotate
    ImgProc2D::GetInstance()->SetRotate(0, MIN);

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CFM);

    // enter PDI mode
    m_scanMode = PDI;
    SetScanMode(m_scanMode);
#ifdef CT_355
    m_ptrImgPw->SetSpectrumModeCW(TRUE);
#endif
    m_ptrImgPw->SetBalanceStatus(FALSE);
    m_ptrImgCfm->SetBalanceStatus(FALSE);

    ///> set format to B
    m_ptrImgCfm->EnterPdi();
    m_ptrFormatCfm->ChangeFormat(FormatCfm::B);

    KeyClearScreen kcs;
    kcs.Execute();

    ///> update 2d image view
    m_ptrUpdateCfm->EnterPdi();

    ///> control light
    DarkAllModeLight();
#if not defined (CT_322)
#if not defined (CT_313)
    g_keyInterface.CtrlLight(TRUE, LIGHT_PDI);
#endif
#endif
    ///> change tis
    ChangeTis();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

/*
 * @brief enter scan mode pw+cfm from cfm mode
 */
void ScanMode::EnterPwCfmFromCfm()
{
    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PWCFM_INIT);

    // set work mode
    m_scanMode = PWCFM_INIT;
    SetScanMode(m_scanMode);
    m_ptrImgPw->SetBalanceStatus(TRUE);
    m_ptrImgCfm->SetBalanceStatus(TRUE);

    ///> display sv
    m_ptrImgPw->SetSpectrumModeCW(FALSE);
    m_ptrImgPw->ResetSvAccordingColor();

    // update
    m_ptrUpdatePw->EnterPwCfmFromCfm();

    ///> control light
    DarkAllModeLight();
#if (defined(CT_340) || defined(CT_161) || defined(CT_360) || defined(CT_440)|| defined(CT_430))
    g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
    g_keyInterface.CtrlLight(TRUE, LIGHT_CFM);
#elif (defined(CT_355))
    int mode = ViewMain::GetInstance()->GetModeIsFlag();
    if(mode)
        g_keyInterface.CtrlLight(TRUE, LIGHT_CW);
    else
        g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
    g_keyInterface.CtrlLight(TRUE, LIGHT_CFM);
#endif
}

/*
 *
 */
void ScanMode::UpdatePwCfmFromInit()
{
    // set cfm is real first
    m_pwCurImg = 1;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(CFM); //pwcfm mode and cfm is real
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PWCFM);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::PWCFM);

    // wait data needed by one frame cfm
    usleep(700000);
    ///> send work mode
    m_scanMode = PWCFM;

    // set pw is real
    m_pwCurImg = 2;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(PW); //pwcfm mode and pw is real

    // set format to UD11
    m_ptrImgPw->EnterPw();
//  m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test

    // set format to UD11 a
    //m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatPW();
    FormatPw::GetInstance()->ChangeFormat(FormatPw::EFormatPw(format));
    ///> change tis
    ChangeTis();

    // update
    m_ptrUpdatePw->UpdatePwMode();

    // update
    m_ptrUpdatePw->UpdatePwCfmMode();

    // clear histogram
    if (DrawHistogram::GetInstance()->GetOnOff())
        DrawHistogram::GetInstance()->SetOnOff(0);
}

int ScanMode::SwitchPwCfm()
{
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

    if (m_pwCurImg == 1) //cfm to pw
    {
        //enter pw mode
        SetScanMode(PW);

        // send to dsc
        m_pwCurImg = 2;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }
    else if (m_pwCurImg == 2) //pw to cfm
    {
        // enter CFM mode
        SetScanMode(CFM);

        // send to dsc
        m_pwCurImg = 1;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }

    ///> replay region
    if (m_pwCurImg == 1)
        m_ptrReplay->SwitchArea(1, unfreeze);
    else
        m_ptrReplay->SwitchArea(0, unfreeze);
    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> change tis
    ChangeTis();

    return m_pwCurImg;
}

/*
 * @brief enter scan mode pw+pdi from pdi mode
 */
void ScanMode::EnterPwPdiFromPdi()
{
    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PWPDI_INIT);

    ///> send work mode
    m_scanMode = PWPDI_INIT;
    SetScanMode(m_scanMode);
    m_ptrImgPw->SetBalanceStatus(TRUE);
    m_ptrImgCfm->SetBalanceStatus(TRUE);

    ///> display box
    m_ptrImgPw->SetSpectrumModeCW(FALSE);
    m_ptrImgPw->ResetSvAccordingColor();

    // update
    m_ptrUpdatePw->EnterPwPdiFromPdi();

    ///> control light
    DarkAllModeLight();
#if (defined(CT_340) || defined(CT_161) || defined(CT_360) || defined(CT_440) || defined(CT_430))
    g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
    g_keyInterface.CtrlLight(TRUE, LIGHT_PDI);
#elif (defined(CT_355))
    int mode = ViewMain::GetInstance()->GetModeIsFlag();
    if(mode)
        g_keyInterface.CtrlLight(TRUE, LIGHT_CW);
    else
        g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
    g_keyInterface.CtrlLight(TRUE, LIGHT_PDI);
#endif
}

void ScanMode::UpdatePwPdiFromInit()
{
    // set pdi is real first
    m_pwCurImg = 1;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(PDI); //pwcfm mode and pw is real
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PWPDI);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::PWCFM);

    // wait data needed by one frame pdi
    usleep(700000);
    ///> send work mode
    m_scanMode = PWPDI;

    // set pw is real
    m_pwCurImg = 2;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(PW); // pw+cfm mode and pw is real

    // set format to UD11
    m_ptrImgPw->EnterPw();
//  m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
        // set format to UD11 a
    //m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatPW();
    FormatPw::GetInstance()->ChangeFormat(FormatPw::EFormatPw(format));

    ///> change tis
    ChangeTis();

    // update
    m_ptrUpdatePw->UpdatePwPdiMode();

    // clear histogram
    if (DrawHistogram::GetInstance()->GetOnOff())
        DrawHistogram::GetInstance()->SetOnOff(0);
}

int ScanMode::SwitchPwPdi()
{
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

    if (m_pwCurImg == 1) //PDI to pw
    {
        //enter pw mode
        SetScanMode(PW);

        // send to dsc
        m_pwCurImg = 2;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }
    else if (m_pwCurImg == 2) //pw to PDI
    {
        // enter CFM mode
        SetScanMode(PDI);

        // send to dsc
        m_pwCurImg = 1;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }

    ///> replay region
    if (m_pwCurImg == 1)
        m_ptrReplay->SwitchArea(1, unfreeze);
    else
        m_ptrReplay->SwitchArea(0, unfreeze);

    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> change tis
    ChangeTis();

    return m_pwCurImg;
}

EKnobReturn ScanMode::EnterPwSimult(bool on)
{
    if (on)
    {
        // set foc sum to 1 when foc sum lager than 1
        if (m_ptrImg2D->GetFocSum() > 1)
        {
            m_ptrImg2D->SetFocSum(1);
        }

        m_scanMode = PW_SIMULT;// set mode to pw simult
    }
    else
    {
        m_scanMode = PW;// set mode to pw simult
    }

    SetScanMode(m_scanMode);

    //change simult status
    return m_ptrImgPw->ChangeSimult2(on);
}
EKnobReturn ScanMode::EnterPwCfmSimult(bool on)
{
    if (on)
    {
        // set foc sum to 1 when foc sum lager than 1
        if (m_ptrImg2D->GetFocSum() > 1)
        {
            m_ptrImg2D->SetFocSum(1);
        }

        m_scanMode = PWCFM_SIMULT;// set mode to pwcfm simult
    }
    else
    {
        m_scanMode = PWCFM;// set mode to pwcfm simult
    }

    SetScanMode(m_scanMode);
    //change simult status
    return m_ptrImgPw->ChangeSimult3(on);
}

EKnobReturn ScanMode::EnterPwPdiSimult(bool on)
{
    if (on)
    {
        // set foc sum to 1 when foc sum lager than 1
        if (m_ptrImg2D->GetFocSum() > 1)
        {
            m_ptrImg2D->SetFocSum(1);
        }

        m_scanMode = PWPDI_SIMULT;// set mode to pwpdi simult
    }
    else
    {
        m_scanMode = PWPDI;
    }

    SetScanMode(m_scanMode);
    //change simult status
    return m_ptrImgPw->ChangeSimult3(on);
}

/*
 * @brief do something before enter cw imaging mode
 *  must call before enter cw and cw + color mode
 */
void ScanMode::PrepareEnterCw(void)
{
    if (ImgPw::GetInstance()->GetSimut2Status())
        ImgPw::GetInstance()->ChangeSimult2(FALSE);
    if (ImgPw::GetInstance()->GetSimut3Status())
        ImgPw::GetInstance()->ChangeSimult3(FALSE);
#ifdef CT_355
    if (!(ImgPw::GetInstance()->GetHPRFStatus()))
    {
        ImgPw::GetInstance()->ChangeHPRF(TRUE);
    }
#else
    if (ImgPw::GetInstance()->GetHPRFStatus())
        ImgPw::GetInstance()->ChangeHPRF(FALSE);
#endif
}

/*
 * @brief enter scan mode initial Cw(2D + CwLine) from other mode
 */
void ScanMode::EnterCw()
{
    Enter2D();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CW_INIT);

    ///> set scan mode
    m_scanMode = CW_INIT;
    SetScanMode(m_scanMode);

    // set rotate to 0
    ImgProc2D::GetInstance()->SetRotate(0, MIN);

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    ///> reset cw line
    m_ptrImgPw->SetSpectrumModeCW(TRUE);
    m_ptrImgPw->ReSendSv();
    m_ptrUpdatePw->EnterPwMode();

#ifndef CT_322
#ifndef CT_313
    ///> control light
    DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE, LIGHT_CW);
#endif
#endif
}

/*
 * @brief enter scan mode real Cw from other mode
 */
void ScanMode::UpdateCw()
{
    // set 2D is real
    m_pwCurImg = 1;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(D2);
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CW);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::PW);

    usleep(500000);

    // send mode to fpga
    m_scanMode = CW;
    SetScanMode(m_scanMode);
    m_ptrImgPw->SetBalanceStatus(FALSE);
    m_ptrImgCfm->SetBalanceStatus(FALSE);

    // set Cw is real
    m_pwCurImg = 2;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;

    // set format to UD11
    m_ptrImgPw->EnterCw();
    //m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatPW();
    FormatPw::GetInstance()->ChangeFormat(FormatPw::EFormatPw(format));

    // update
    //m_ptrUpdatePw->UpdateCwMode();
    m_ptrUpdatePw->UpdatePwMode();

    // clear histogram
    if (DrawHistogram::GetInstance()->GetOnOff())
        DrawHistogram::GetInstance()->SetOnOff(0);

    ///> change tis
    ChangeTis();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

int ScanMode::SwitchCw()
{
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

    if (m_pwCurImg == 1) //B to cw
    {
        //enter pw mode
        SetScanMode(CW);

        m_ptrImgPw->EnterCw();

        // send to dsc
        m_pwCurImg = 2;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }
    else if (m_pwCurImg == 2) //cw to B
    {
        //ImgPw::GetInstance()->OnCwImgCtrl(FALSE);

        // enter B mode
        SetScanMode(D2);

        // send to dsc
        m_pwCurImg = 1;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }

    ///> replay region
    if (m_pwCurImg == 1)
        m_ptrReplay->SwitchArea(1, unfreeze);
    else
        m_ptrReplay->SwitchArea(0, unfreeze);

    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }
    ///> change tis
    ChangeTis();

    return m_pwCurImg;
}

/*
 * @brief enter scan mode cw+cfm from cfm mode
 */
void ScanMode::EnterCwCfmFromCfm()
{
    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
#ifdef CT_355
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::PWCFM_INIT);

#else
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CWCFM_INIT);
#endif

    // set work mode
    m_scanMode = CWCFM_INIT;
    SetScanMode(m_scanMode);
    m_ptrImgPw->SetBalanceStatus(TRUE);
    m_ptrImgCfm->SetBalanceStatus(TRUE);

    ///> display sv
    m_ptrImgPw->SetSpectrumModeCW(TRUE);
    m_ptrImgPw->ResetSvAccordingColor();

    // update
//  m_ptrUpdatePw->EnterPwCfmFromCfm();
#ifdef CT_355
    m_ptrUpdatePw->EnterPwCfmFromCfm();
#else
    m_ptrUpdatePw->EnterCwCfmFromCfm();
#endif

    ///> control light
#ifndef CT_322
#ifndef CT_313
    DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
    g_keyInterface.CtrlLight(TRUE, LIGHT_CFM);
#endif
#endif
}

/*
 *
 */
void ScanMode::UpdateCwCfmFromInit()
{
    // set cfm is real first
    m_pwCurImg = 1;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(CFM); //cwcfm mode and cw is real
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CWCFM);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::PWCFM);

    // wait data needed by one frame cfm
    usleep(700000);
    ///> send work mode
    m_scanMode = CWCFM;

    // set pw is real
    m_pwCurImg = 2;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(CW); //pwcfm mode and pw is real

    // set format to UD11
    m_ptrImgPw->EnterCw();
//  m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    // set format to UD11 a
    //m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatPW();
    FormatPw::GetInstance()->ChangeFormat(FormatPw::EFormatPw(format));

    ///> change tis
    ChangeTis();

    // update
    m_ptrUpdatePw->UpdatePwCfmMode();
    //m_ptrUpdatePw->UpdateCwCfmMode();

    // clear histogram
    if (DrawHistogram::GetInstance()->GetOnOff())
        DrawHistogram::GetInstance()->SetOnOff(0);
}

int ScanMode::SwitchCwCfm()
{
    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

    if (m_pwCurImg == 1) //cfm to cw
    {
        //enter cw mode
        SetScanMode(CW);

        m_ptrImgPw->EnterCw();

        //ImgPw::GetInstance()->OnCwImgCtrl(TRUE);

        // send to dsc
        m_pwCurImg = 2;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }
    else if (m_pwCurImg == 2) //cw to cfm
    {
        // enter CFM mode
        //ImgPw::GetInstance()->OnCwImgCtrl(FALSE);

        SetScanMode(CFM);

        // send to dsc
        m_pwCurImg = 1;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }

    ///> replay region
    if (m_pwCurImg == 1)
        m_ptrReplay->SwitchArea(1, unfreeze);
    else
        m_ptrReplay->SwitchArea(0, unfreeze);
    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> change tis
    ChangeTis();

    return m_pwCurImg;
}

/*
 * @brief enter scan mode cw+pdi from pdi mode
 */
void ScanMode::EnterCwPdiFromPdi()
{
    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CWPDI_INIT);

    ///> send work mode
    m_scanMode = CWPDI_INIT;
    SetScanMode(m_scanMode);
    m_ptrImgPw->SetBalanceStatus(TRUE);
    m_ptrImgCfm->SetBalanceStatus(TRUE);

    ///> display box
    m_ptrImgPw->SetSpectrumModeCW(TRUE);
    m_ptrImgPw->ResetSvAccordingColor();

    // update
    m_ptrUpdatePw->EnterPwPdiFromPdi();
    //m_ptrUpdatePw->EnterCwPdiFromPdi();

    ///> control light
#ifndef CT_322
#ifndef CT_313
    DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE, LIGHT_PW);
    g_keyInterface.CtrlLight(TRUE, LIGHT_PDI);
#endif
#endif
}

void ScanMode::UpdateCwPdiFromInit()
{
    // set pdi is real first
    m_pwCurImg = 1;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(PDI); //cwcfm mode and pdi is real
    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CWPDI);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::PWCFM);

    // wait data needed by one frame pdi
    usleep(700000);
    ///> send work mode
    m_scanMode = CWPDI;

    // set cw is real
    m_pwCurImg = 2;
    m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    SetScanMode(CW); // cw+cfm mode and cw is real

    // set format to UD11
    m_ptrImgPw->EnterCw();
//  m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
        // set format to UD11 a
    //m_ptrFormatPw->ChangeFormat(FormatPw::BP11_UD); //only for test
    SysOptions sysoption;
    int format = sysoption.GetDisplayFormatPW();
    FormatPw::GetInstance()->ChangeFormat(FormatPw::EFormatPw(format));

    ///> change tis
    ChangeTis();

    // update
    m_ptrUpdatePw->UpdatePwPdiMode();
    //m_ptrUpdatePw->UpdateCwPdiMode();

    // clear histogram
    if (DrawHistogram::GetInstance()->GetOnOff())
        DrawHistogram::GetInstance()->SetOnOff(0);
}

int ScanMode::SwitchCwPdi()
{

    bool unfreeze = FALSE;
    if (ModeStatus::IsUnFreezeMode())
        unfreeze = TRUE;

    if (m_pwCurImg == 1) //PDI to cw
    {
        //enter cw mode
        SetScanMode(CW);

        m_ptrImgPw->EnterCw();

        //ImgPw::GetInstance()->OnCwImgCtrl(TRUE);

        // send to dsc
        m_pwCurImg = 2;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }
    else if (m_pwCurImg == 2) //cw to PDI
    {
        // enter CFM mode
        //ImgPw::GetInstance()->OnCwImgCtrl(FALSE);
        SetScanMode(PDI);

        // send to dsc
        m_pwCurImg = 1;
        m_ptrDscPara->dcaPWFlag = m_pwCurImg;
    }

    ///> replay region
    if (m_pwCurImg == 1)
        m_ptrReplay->SwitchArea(1, unfreeze);
    else
        m_ptrReplay->SwitchArea(0, unfreeze);

    if (!unfreeze)
    {
        m_ptrReplay->PrepareForReplay();
        m_ptrReplay->ReviewOneImg();
    }

    ///> change tis
    ChangeTis();

    return m_pwCurImg;
}
void ScanMode::EnterCfmVs2D()
{
    PRINTF("------------scan mode enter cfm vs 2D\n");
    // Exit special measure
    ExitSpecialMeasure();

    ///> set format to B
    m_ptrFormatCfm->ChangeFormat(FormatCfm::B);

    ///> set mode to 2d
    m_scanMode = CFM_VS_2D;
    SetScanMode(m_scanMode);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::CFMVS2D);

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CFM);

    ///> update
    m_ptrUpdateCfm->EnterCfmVs2D();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

void ScanMode::EnterPdiVs2D()
{
    PRINTF("------------scan mode enter pdi vs 2D\n");
    // Exit special measure
    ExitSpecialMeasure();

    ///> set format to B
    m_ptrFormatCfm->ChangeFormat(FormatCfm::B);

    ///> set mode to 2d
    m_scanMode = PDI_VS_2D;
    SetScanMode(m_scanMode);

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::CFMVS2D);

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::CFM);

    ///> update
    m_ptrUpdateCfm->EnterCfmVs2D();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

void ScanMode::EnterAnatomicM()
{
    if ((FormatM::GetInstance()->GetFormat() == FormatM::M_TOTAL) || (m_scanMode == ANATOMIC_M))
        return;

    m_scanMode = ANATOMIC_M;
    SetScanMode(m_scanMode);

    // set mode to anatomic
    ImgProcM::GetInstance()->AnatomicMInit(); // something init,

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::ANATOMIC_M);

    // update screen
    m_ptrUpdate2D->EnterAnatomicM();

    // unfreeze
    ModeStatus s;
    FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
    if (freeze != FreezeMode::UNFREEZE)
        FreezeMode::GetInstance()->PressUnFreeze();
}

void ScanMode::EnterHPRF(bool on)
{
    if (on)
    {
        ///> send pulse number
        m_pulseNum =  10;//30;
        m_fpgaCtrl2D.SendPWPulseNum(m_pulseNum);
    }
    else
    {
        ///> send pulse number
        m_pulseNum =  10;//30;
        m_fpgaCtrl2D.SendPWPulseNum(m_pulseNum);
    }
}

void ScanMode::EnterEFOV(void)
{
    // Exit special measure
    ExitSpecialMeasure();

    m_ptrMultiFuncFactory->Create(MultiFuncFactory::EFOV);

    DscMan::GetInstance()->GetWriteLock();
    m_ptrUpdate2D->ClearEFOVImg();
    DscMan::GetInstance()->ReadWriteUnlock();
    m_scanMode = EFOV;
    SetScanMode(m_scanMode);

    m_ptrImg2D->EnterEFOV();

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    // create Dsc Object
    DscMan::GetInstance()->CreateDscObj(DscMan::EFOV);

    // set replay para
    m_ptrReplay->SetAreaNum(1);

    m_ptrUpdate2D->EnterEFOVPrepare();
}

void ScanMode::EnterEFOVPrepare(void)
{
    // Exit special measure
    ExitSpecialMeasure();

    CDSC::EFOVSTATUS status = CDSC::PREPARE;
    if (DscMan::GetInstance()->GetDsc() != NULL)
    {
        DscMan::GetInstance()->GetWriteLock();
        DscMan::GetInstance()->GetDsc()->SetEFOVStatus(status);
        DscMan::GetInstance()->ReadWriteUnlock();
    }

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    m_ptrUpdate2D->EnterEFOVPrepare();

        m_ptrReplay->ClearCurReplayData();
    ViewIcon::GetInstance()->Replay(FALSE);
}

void ScanMode::EnterEFOVCapture(void)
{
    // Exit special measure
    ExitSpecialMeasure();

    CDSC::EFOVSTATUS status = CDSC::CAPTURE;
    if(DscMan::GetInstance()->GetDsc() != NULL)
    {
        DscMan::GetInstance()->GetWriteLock();
        DscMan::GetInstance()->GetDsc()->SetROISize(ImgProc2D::GetInstance()->GetEFOVROISize());
        DscMan::GetInstance()->GetDsc()->SetEFOVStatus(status);
        DscMan::GetInstance()->ReadWriteUnlock();
    }

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    m_ptrUpdate2D->EnterEFOVCapture();
}

void ScanMode::EnterEFOVView(void)
{
    // Exit special measure
    ExitSpecialMeasure();

    CDSC::EFOVSTATUS status = CDSC::VIEW;
    if (DscMan::GetInstance()->GetDsc() != NULL)
    {
        DscMan::GetInstance()->GetWriteLock();
        DscMan::GetInstance()->GetDsc()->SetEFOVStatus(status);
        DscMan::GetInstance()->ReadWriteUnlock();
    }

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

        m_ptrReplay->ClearCurReplayData();

    m_ptrUpdate2D->EnterEFOVView();

    ImgProc2D::GetInstance()->EnterEFOVView();

    ViewIcon::GetInstance()->Replay(TRUE);
}

void ScanMode::EnterEFOVReview(void)
{
    // Exit special measure
    ExitSpecialMeasure();

    CDSC::EFOVSTATUS status = CDSC::REVIEW;
    if (DscMan::GetInstance()->GetDsc() != NULL)
        DscMan::GetInstance()->GetDsc()->SetEFOVStatus(status);

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();

    //update view
    m_ptrUpdate2D->EnterEFOVReview();
}

ScanMode::EFOVStatus ScanMode::GetEFOVStatus()
{
    EFOVStatus status = PREPARE;

    CDSC::EFOVSTATUS statusDsc = CDSC::PREPARE;
    if(DscMan::GetInstance()->GetDsc() != NULL)
    {
        statusDsc = DscMan::GetInstance()->GetDsc()->GetEFOVStatus();
    }

    switch(statusDsc)
    {
    case CDSC::PREPARE:
        status = PREPARE;
        break;
    case CDSC::CAPTURE:
        status = CAPTURE;
        break;
    case CDSC::VIEW:
        status = VIEW;
        break;
    case CDSC::REVIEW:
        status = REVIEW;
        break;
    default:
        status = PREPARE;
        break;
    }

    return status;
}
static void D4QuitCallback(int flag)
{
    D4FuncMan::GetInstance()->Exit4D();
    usleep(1000);// 保证信号忽略是在子进程结束后
    signal(SIGCHLD, SIG_IGN);
}

void ScanMode::Enter4D()
{
    // Exit special measure
    ExitSpecialMeasure();

    //if no probe exist, exit
    if(!ProbeMan::GetInstance()->IsProbeExist())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::ERROR,
                _("Failed to Enter 3D/4D mode!\nNo Probe was found."),
                NULL);
        return;
    }

    Enter2D(); // if Review

    //focus: set foc sum to 1 when foc sum lager than 1
    if (m_ptrImg2D->GetFocSum() > 1)
    {
        m_ptrImg2D->SetFocSum(1);
    }

    //depth
    if(Img2D::GetInstance()->GetDepth() >= 300)
    {
        KeyDepth kd;
        kd.ExecuteAdd();
    }
    // depth restrict by zjj 2015-1-26
    if (Img2D::GetInstance()->GetDepth() <= 93)
    {
        Img2D::GetInstance()->Set4DDepth(14);
    }
    //set scan angle to 61
    Img2D::GetInstance()->SetScanAngle(1); //0表示最大角度，因此这个给1

    // send para prepare
    SUIS4DPARA Para;
    D4FuncMan::GetInstance()->GetPara(Para);
    Para.max_period = GlobalClassMan::GetInstance()->GetCalc2D()->CalcMaxPeriod();
    Para.tx = Calc2D::TX;
    Para.mbp = Img2D::GetInstance()->GetMBP();

    //TGC
    Para.imgScale = Img2D::GetInstance()->GetImgScaleFor4D();
    Para.depthMax = Img2D::GetInstance()->GetDepthMaxFor4D();
    Para.gain = Img2D::GetInstance()->GetGainFor4D();
    int *tgc = Img2D::GetInstance()->GetTGCFor4D();
    Para.tgc_0 = tgc[0];
    Para.tgc_1 = tgc[1];
    Para.tgc_2 = tgc[2];
    Para.tgc_3 = tgc[3];
    Para.tgc_4 = tgc[4];
    Para.tgc_5 = tgc[5];
    Para.tgc_6 = tgc[6];
    Para.tgc_7 = tgc[7];

    // get m_fdEzUsbId
    Para.ezusb_id = EzUsb::GetInstance()->GetCurEzUsbId();
    g_source_remove(input_handle);

    // -----------shut down 09001 keyboard-----------------
    g_io_channel_unref(keyboard_channel);

    if (keyboard_channel) {
        GError *err = 0;
        if (G_IO_STATUS_NORMAL !=
                g_io_channel_shutdown(keyboard_channel, true, &err))
        {
            PRINTF("ERROR: %s, %s\n", __PRETTY_FUNCTION__, err->message);
            return;
        }
        keyboard_channel = 0;
    }
    close(s_fdcom);
    // -----------shut down 09001 keyboard-----------------

    pid_t pid = -1;

    struct sigaction d4_quit;
    d4_quit.sa_handler = D4QuitCallback;
    sigemptyset(&d4_quit.sa_mask);
    d4_quit.sa_flags = 0;
    sigaction(SIGCHLD, &d4_quit, 0);

    if ((pid = vfork()) == 0)//子进程
    {
        FreezeMode::GetInstance()->ExitReplayMode();//Clear 2D Replay memory

        g_uis4d_ri.SaveConfigInfo(Para); // save para list

        const string SECTION = "Upgrade";
        char path[256];
        sprintf(path, "%s", "/mnt/harddisk/emp/11002/11002");
        if (execl(path,"11002",(char *)0) < 0)
            perror("execlp error\n");
    }
    else if (pid == -1)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::ERROR,
                _("Failed to Enter 3D/4D mode!\nThe process cannot be created."),
                NULL);
        return;
    }
    else
    {
        D4FuncMan::GetInstance()->Set4DMode(true);

        int status;
        int pid_4d = wait(&status);
    }
}

void ScanMode::GetSpecialMeasurePara(SpecialMeasurePara *para)
{
    para->mode = GetScanMode();
    para->format2D = m_ptrFormat2D->GetFormat();
    para->formatCfm = m_ptrFormatCfm->GetFormat();
    para->formatPw = m_ptrFormatPw->GetFormat();
    para->formatM = m_ptrFormatM->GetFormat();
    m_ptrImg2D->GetAllScale2D(para->scale2DAll);
    para->scale2D =  m_ptrImg2D->GetScale2D();
    para->scaleMDepth = m_ptrImg2D->GetScaleMDepth();
    para->scaleMTime = m_ptrImg2D->GetScaleMTime();
    para->scalePwVel = m_ptrImgPw->GetScaleVel();
    para->scalePwTime = m_ptrImgPw->GetScaleTime();
    para->baselineCalc = m_ptrImgPw->GetBaseLineForCalc();
}
void ScanMode::EnterSpecialMeasure(SpecialMeasurePara para)
{
    m_isSpecialMeasure = TRUE;

    m_scanModeMeasure = para.mode;
    m_ptrImg2D->SetScaleForSpecialMeasure(para.scale2DAll, para.scale2D, para.scaleMDepth, para.scaleMTime);
    m_ptrImgPw->SetScaleForSpecialMeasure(para.scalePwVel, para.scalePwTime, para.baselineCalc);
    m_ptrFormat2D->SetFormatForSnap(para.format2D);
    m_ptrFormatM->SetFormatForSnap(para.formatM);
    m_ptrFormatPw->SetFormatForSnap(para.formatPw);
    m_ptrFormatCfm->SetFormatForSnap(para.formatCfm);

}
void ScanMode::ExitSpecialMeasure(void)
{
    m_isSpecialMeasure = FALSE;
}

bool ScanMode::IsSpecialMeasureStatus(void)
{
    return m_isSpecialMeasure;
}

void ScanMode::SetPWPulseNum(void)
{
    if (ImgPw::GetInstance()->GetRealPRF() <= 1500)//3000)
    {
#ifdef CT_355
        m_pulseNum = 4;
#else
        m_pulseNum = 8;
#endif
    }
    else
    {
#ifdef CT_355
        m_pulseNum = 3;
#else
        m_pulseNum = 5;
#endif
    }
    m_fpgaCtrl2D.SendPWPulseNum(m_pulseNum);
}

///> private
void ScanMode::SetScanMode(EScanMode mode)
{
    g_menuCalc.ChangeScanMode(m_scanMode);
    g_menuMeasure.ChangeScanMode(m_scanMode);
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);

    const char* type = para.model;

    // exit EFOV mode
    if (m_preScanMode == EFOV && mode != EFOV) {
        m_ptrReplay->EFOVAutoReviewEnd();
        m_ptrImg2D->ExitEFOV();
    }

    //send trigger= false and cwmode =false and calc emit delay to fpga.
  if ((m_preScanMode == CW || m_preScanMode==CWCFM || m_preScanMode == CWPDI) && (mode != CW || mode != CWCFM || mode != CWPDI))
    {
        ImgPw::GetInstance()->OnCwImgCtrl(FALSE);
        m_ptrImg2D->ModeCwCtrl(FALSE);
    }

#if 0
        m_ptrImg2D->ModeCwCtrl(TRUE);
#endif

    int index = 3;

    // exit compound when scan mode is not 2D
    bool valid = ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode();
    if(!valid)
    {
        if (m_ptrImg2D->IsCompoundSpaceOn())
            m_ptrImg2D->SetCompoundSpace(0);
        if (m_ptrImg2D->IsCompoundFreqOn())
            m_ptrImg2D->SetCompoundFreq(FALSE);
        if (m_ptrImg2D->GetSteer()!=0)
            m_ptrImg2D->SetSteer(index);
    }

    // control auto trace and zuto calc
    AbsUpdatePw* ptrUpdate = GlobalClassMan::GetInstance()->GetUpdatePw();
    ImgProcPw *ptrProcPw = ImgProcPw::GetInstance();
    if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())
    {
        ptrProcPw->SetDscTraceStatus(TRUE);
    }
    else
    {
        ptrProcPw->SetDscTraceStatus(FALSE);
        // if auto calc is on and mode is not pw image mode, off the auto calc
        if (ImgProcPw::GetInstance()->GetAutoCalc())
        {
            ptrUpdate->AutoCalc(FALSE);
            ptrUpdate->AutoStatus(FALSE);
            ptrProcPw->SetTraceStatus(FALSE);
        }
    }
    //A60
    //open doppler sound just in pw mode
#ifdef CT_430
    if (((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI)
                || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI) || (mode == ScanMode::PW_SIMULT)
                || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
       )
        ImgPw::GetInstance()->ChangeDopplerSoundStatus(true);
    else
    {
        if(!g_init)
            ImgPw::GetInstance()->ChangeDopplerSoundStatus(false);
    }
    if(mode != m_preScanMode)
    {
        // close high volatage
        ProbeMan::GetInstance()->ActiveHV(FALSE);
        usleep(50000);
    }
#endif

    switch(mode)
    {
        case D2:
            m_fpgaScanMode = D2;
            // work mode
            m_workMode = 0x01;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);
            break;

        case M_INIT:
            m_fpgaScanMode = D2;
            break;

        case M:
            m_fpgaScanMode = M;
            // work mode
            m_workMode = 0x06;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);
            break;

        case PW_INIT:
            m_fpgaScanMode = D2;
            //m_scanMode = PW_INIT;
            break;

        case PW:
            m_fpgaScanMode = PW;
            // work mode
            m_workMode = 0x05;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);

            ///> send pulse number
            SetPWPulseNum();
            break;

        case PW_SIMULT:
            m_fpgaScanMode = PW_SIMULT;
            // work mode
            m_workMode = 0x08;//0x08;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);

            ///> send pulse number
            m_pulseNum = 6;//10;    //20;
            m_fpgaCtrl2D.SendPWPulseNum(m_pulseNum);
            break;

        case CW_INIT:
            m_fpgaScanMode = D2;
            break;

        case CW:

            m_fpgaScanMode = CW;
            // work mode
            m_workMode = 0x09;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);
            break;

        case CFM:
        case PWCFM_INIT: //CFM mode
        case CWCFM_INIT:
            m_fpgaScanMode = CFM;
            // work mode
            m_workMode = 0x02;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);

            if (strcmp(type, "35C60E") == 0)
            {
                m_pulseNum = 4;//3; //linear =  6;//4;  //6//12;//10;
            }
            else if ((strcmp(type, "65L40E") == 0) || (strcmp(type, "65C10E") == 0) || (strcmp(type, "65C15D") == 0))
                m_pulseNum = 4; //6;
            else
                m_pulseNum = 4;

            m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            break;

        case PDI:
        case PWPDI_INIT: //PDI mode
        case CWPDI_INIT:
            m_fpgaScanMode = PDI;
            // work mode
            m_workMode = 0x03;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);
            if (strcmp(type, "35C60E") == 0)
            {
                m_pulseNum = 3; //linear =  6;//4;  //6//12;//10;
            }
            else if ((strcmp(type, "65L40E") == 0) || (strcmp(type, "65C10E") == 0) || (strcmp(type, "65C15D") == 0))
                m_pulseNum = 6;
            else
                m_pulseNum = 4;

            ///> send pulse number
            //m_pulseNum = 6;//4;   //12;
            m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            break;

        case PWCFM:
            if (m_pwCurImg == 1)//CFM
            {
                m_fpgaScanMode = CFM;
                // work mode
                m_workMode = 0x02;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);

                ///> send pulse number
                m_pulseNum = 4; //6;//4;
                m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            }
            else
            {
                m_fpgaScanMode = PW;
                // work mode
                m_workMode = 0x05;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);

                ///> send pulse number
                SetPWPulseNum();
            }
            break;

        case PWPDI:
            if (m_pwCurImg == 1)
            {
                m_fpgaScanMode = PDI;
                // work mode
                m_workMode = 0x03;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);

                ///> send pulse number
                m_pulseNum = 6;//4;     //10;
                m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            }
            else
            {
                m_fpgaScanMode = PW;
                // work mode
                m_workMode = 0x05;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);

                ///> send pulse number
                 SetPWPulseNum();
            }
            // work mode
            break;

        case PWCFM_SIMULT:
            m_fpgaScanMode = PWCFM_SIMULT;
            m_workMode = 0x04;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);

            ///> send pulse number
            m_pulseNum = 6;//10;
            m_fpgaCtrl2D.SendPWPulseNum(m_pulseNum);
            m_pulseNum = 6;//4;
            m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            break;

        case PWPDI_SIMULT:
            m_fpgaScanMode = PWPDI_SIMULT;
            // work mode
            m_workMode = 0x07;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);

            ///> send pulse number
            m_pulseNum = 6;//10;        //10;
            m_fpgaCtrl2D.SendPWPulseNum(m_pulseNum);
            m_pulseNum = 6;//4;//6;     //10;
            m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            break;

        case CWCFM:
            if (m_pwCurImg == 1)//CFM
            {
                m_fpgaScanMode = CFM;
                // work mode
                m_workMode = 0x02;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);

                ///> send pulse number
                m_pulseNum = 6;//4;
                m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            }
            else
            {
                m_fpgaScanMode = CW;
                // work mode
                m_workMode = 0x09;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);
            }
            break;

        case CWPDI:
            if (m_pwCurImg == 1)
            {
                m_fpgaScanMode = PDI;
                // work mode
                m_workMode = 0x03;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);

                ///> send pulse number
                m_pulseNum = 6;//4;     //10;
                m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNum);
            }
            else
            {
                m_fpgaScanMode = CW;
                // work mode
                m_workMode = 0x09;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);
            }
            break;

        case CFM_VS_2D:
            m_fpgaScanMode = CFM;
            break;

        case PDI_VS_2D:
            m_fpgaScanMode = PDI;
            break;

        case ANATOMIC_M:
            m_fpgaScanMode = D2;

            // work mode
            m_workMode = 0x01;
            m_fpgaCtrl2D.SendWorkModel(m_workMode);
            break;
        case EFOV:
            m_fpgaScanMode = D2;
            break;
    }
#ifdef CT_430
    if(mode != m_preScanMode)
    {
        ProbeMan::GetInstance()->ActiveHV(TRUE);
    }
#endif

#if 0
    if ((m_preScanMode == CW || m_preScanMode==CWCFM || m_preScanMode == CWPDI)
            && (mode != CW && mode != CWCFM && mode != CWPDI))
    {
        ImgPw::GetInstance()->OnCwImgCtrl(FALSE);
    }
    else
    {
        ImgPw::GetInstance()->OnCwImgCtrl(TRUE);
    }
#endif

    m_pulseNumTmp = m_pulseNum;
    m_preScanMode = mode;

    PRINTF("::::::::::::::::::::pulse num: m_pulseNum = %d \n",m_pulseNum);
}

void ScanMode::DarkAllModeLight()
{
#if (defined (CT_322) || defined(CT_313))
        g_keyInterface.CtrlLight(FALSE, LIGHT_D2);
        g_keyInterface.CtrlLight(FALSE, LIGHT_M);
        g_keyInterface.CtrlLight(FALSE, LIGHT_BM);
        g_keyInterface.CtrlLight(FALSE, LIGHT_BB);
#else
        if(g_keyInterface.IsLighten(LIGHT_D2))
            g_keyInterface.CtrlLight(FALSE, LIGHT_D2);
        if(g_keyInterface.IsLighten(LIGHT_M))
            g_keyInterface.CtrlLight(FALSE, LIGHT_M);
        if(g_keyInterface.IsLighten(LIGHT_PW))
            g_keyInterface.CtrlLight(FALSE, LIGHT_PW);
        if(g_keyInterface.IsLighten(LIGHT_CW))
            g_keyInterface.CtrlLight(FALSE, LIGHT_CW);
        if(g_keyInterface.IsLighten(LIGHT_CFM))
            g_keyInterface.CtrlLight(FALSE, LIGHT_CFM);
        if(g_keyInterface.IsLighten(LIGHT_PDI))
            g_keyInterface.CtrlLight(FALSE, LIGHT_PDI);
#endif
}

///>test
void ScanMode::ChangePulseNum(EKnobOper oper)
{
    if(m_scanMode != CFM)
        return;

    int step = 1;
    if(oper == ADD)
    {
        m_pulseNumTmp += step;
    }
    else if (oper == SUB)
    {
        if((m_pulseNumTmp - step) >= 1)
            m_pulseNumTmp -= step;
    }

    m_fpgaCtrl2D.SendCFMPulseNum(m_pulseNumTmp);
    //printf("m_pulseNumTmp:%d\n", m_pulseNumTmp);
    ImgCfm::GetInstance()->UpdateHint();
}

void ScanMode::SetMScanMode()
{
    static int count = 0;

    switch(count)
    {
        case 0:
            {
                m_scanMode = M;
                m_fpgaScanMode = M;
                // work mode
                m_workMode = 0x06;
                m_fpgaCtrl2D.SendWorkModel(m_workMode);
                break;
            }

        case 1:
            {
                Enter2D();
                break;
            }
        case 2:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                break;
            }
        case 3:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                // set rotate to 0
                ImgProc2D::GetInstance()->SetRotate(0, MIN);

                ///> reset bm line
                m_ptrImg2D->ResetMLine();
                m_ptrUpdate2D->EnterMMode();

                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

                break;
            }

        case 4:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                // set rotate to 0
                ImgProc2D::GetInstance()->SetRotate(0, MIN);

                ///> reset bm line
                m_ptrImg2D->ResetMLine();
                m_ptrUpdate2D->EnterMMode();

                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

                // set scan mode
                m_scanMode = M;
                SetScanMode(m_scanMode);

                break;
            }
        case 5:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                // set rotate to 0
                ImgProc2D::GetInstance()->SetRotate(0, MIN);

                ///> reset bm line
                m_ptrImg2D->ResetMLine();
                m_ptrUpdate2D->EnterMMode();

                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

                // set scan mode
                m_scanMode = M;
                SetScanMode(m_scanMode);

                m_ptrImg2D->EnterM();

                break;
            }

        case 6:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                // set rotate to 0
                ImgProc2D::GetInstance()->SetRotate(0, MIN);

                ///> reset bm line
                m_ptrImg2D->ResetMLine();
                m_ptrUpdate2D->EnterMMode();

                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

                // set scan mode
                m_scanMode = M;
                SetScanMode(m_scanMode);

                m_ptrImg2D->EnterM();

                // create Dsc Object
                DscMan::GetInstance()->CreateDscObj(DscMan::M);

                break;
            }

        case 7:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                // set rotate to 0
                ImgProc2D::GetInstance()->SetRotate(0, MIN);

                ///> reset bm line
                m_ptrImg2D->ResetMLine();
                m_ptrUpdate2D->EnterMMode();

                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

                // set scan mode
                m_scanMode = M;
                SetScanMode(m_scanMode);

                m_ptrImg2D->EnterM();

                // create Dsc Object
                DscMan::GetInstance()->CreateDscObj(DscMan::M);

                ///> set format to M
                //m_ptrFormatM->ChangeFormat(FormatM::M_TOTAL); //only for test
                SysOptions sysoption;
                int format = sysoption.GetDisplayFormatM();
                FormatM::GetInstance()->ChangeFormat(FormatM::EFormatM(format));

                break;
            }

        case 8:
            {
                // update
                m_ptrUpdate2D->UpdateM();

                break;
            }

        case 9:
            {
                Enter2D();
                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M_INIT);

                // send work mode
                m_scanMode = M_INIT;
                SetScanMode(m_scanMode);

                // set rotate to 0
                ImgProc2D::GetInstance()->SetRotate(0, MIN);

                ///> reset bm line
                m_ptrImg2D->ResetMLine();
                m_ptrUpdate2D->EnterMMode();

                m_ptrMultiFuncFactory->Create(MultiFuncFactory::M);

                // set scan mode
                m_scanMode = M;
                SetScanMode(m_scanMode);

                m_ptrImg2D->EnterM();

                // create Dsc Object
                DscMan::GetInstance()->CreateDscObj(DscMan::M);

                ///> set format to M
                //m_ptrFormatM->ChangeFormat(FormatM::M_TOTAL); //only for test
                SysOptions sysoption;
                int format = sysoption.GetDisplayFormatM();
                FormatM::GetInstance()->ChangeFormat(FormatM::EFormatM(format));

                // update
                m_ptrUpdate2D->UpdateM();

                // unfreeze
                ModeStatus s;
                FreezeMode::EFreezeMode freeze = s.GetFreezeMode();
                if (freeze != FreezeMode::UNFREEZE)
                    FreezeMode::GetInstance()->PressUnFreeze();
                break;
            }
        default:
            {
                m_scanMode = M;
                SetScanMode(m_scanMode);
                //printf("default\n");
            }
    }
    //printf("count:%d\n", count);
    count++;
}