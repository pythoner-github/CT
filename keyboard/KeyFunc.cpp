#include <cups/cups.h>
#include <iostream>
#include <stdlib.h>
#include "keyboard/KeyFunc.h"
#include "imageControl/Img2D.h"
#include "imageControl/ImgPw.h"
#include "imageControl/ImgCfm.h"
#include "imageProc/GlobalClassMan.h"
#include "measure/MeasureFactory.h"
#include "display/gui_func.h"
#include "ViewMain.h"
#include "patient/PatientInfo.h"
#include "patient/ImgMan.h"
#include "keyboard/LightDef.h"
#include "display/ImageArea.h"
#include "display/ViewDialog.h"
#include "comment/Arrow.h"
#include "comment/NoteArea.h"
#include "measure/MeasureMan.h"
#include "imageControl/Knob2D.h"
#include "imageControl/KnobM.h"
#include "imageControl/KnobPw.h"
#include "imageControl/KnobCfm.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/ImgProcCfm.h"
#include "display/KnobNone.h"
#include "sysMan/SysOptions.h"
#include "imageProc/ScanMode.h"
#include "periDevice/PeripheralMan.h"
#include "display/HintArea.h"
#include "measure/DrawHistogram.h"
#include "imageProc/MenuReview.h"
#include "probe/MenuBiopsy.h"
#include "calcPeople/MenuCalcNew.h"
#include "patient/ViewNewPat.h"
#include "imageProc/Zoom.h"
#include "calcPeople/ViewReport.h"
#include "sysMan/CalcSetting.h"
#include "periDevice/DCMMan.h"
#include "sysMan/SysDicomSetting.h"
#include "patient/ViewWorkList.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/ViewSystem.h"
#include "measure/MenuMeasure.h"

int g_count11 = 0;
extern bool g_calcPwStatus;

////////////////////////////////////////// [Switch IM] //////////////////////////////////////
bool KeySwitchIM::m_imOn = FALSE;
bool KeySwitchIM::Execute()
{
    PRINTF("Switch Input Method\n");
    doKeyEvent(37, 1);
    doKeyEvent(65, 1);
    doKeyEvent(65, 0);
    doKeyEvent(37, 0);
    if(m_imOn)
        m_imOn = FALSE;
    else
        m_imOn = TRUE;
    return TRUE;
}

bool KeySwitchIM::ExcuteChange(bool issitch)
{
    if (issitch)
    {
        doKeyEvent(37, 1);
        doKeyEvent(65, 1);
        doKeyEvent(65, 0);
        doKeyEvent(37, 0);
        if(m_imOn)
            m_imOn = FALSE;
        else
            m_imOn = TRUE;
    }
    return TRUE;
}

////////////////////////////////////////// [Power OFF] //////////////////////////////////////
int PowerOff(gpointer data)
{
    execlp("poweroff", "poweroff", NULL);   //system() is deprecated; when memory is lack, it will be not run!
    //  int ret = execlp("init", "init", "0", NULL);    //system() is deprecated; when memory is lack, it will be not run!
    perror("PowerOff Failed");

    return TRUE;
}

bool KeyPowerOff::Execute()
{
#ifdef CT_355
    FpgaCtrl2D ctrl2D;
    ctrl2D.SendPowerOff();
#endif

    //  const char* info = N_("Power OFF?");
    //const char* info = N_("Are you sure you want to close the program and shut down the computer?");

    //ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()), ViewDialog::QUESTION, _(info), PowerOff);
    PowerOff(NULL);
    return TRUE;
}

////////////////////////////////////////// [change tsi] //////////////////////////////////////
/*
 * @brief change tsi
 */
int KeyTSI::keyNum = 0;

bool KeyTSI::Execute()
{
    Img2D::GetInstance()->ChangeTSI(EKnobOper(ROTATE));
    return TRUE;
}
bool KeyTSI::Do()
{
    Img2D::GetInstance()->ChangeTSI(EKnobOper(ROTATE));
    return TRUE;
}

bool KeyTSI::Undo()
{

    MultiFuncUndo();
    return TRUE;
}

////////////////////////////////////////// [auto optimize] //////////////////////////////////////
/*
 * @brief auto optimize image
 */
bool KeyAutoOptimize::m_autoOn = FALSE;
ExamItem::ParaItem KeyAutoOptimize::m_itemPara;

bool KeyAutoOptimize::Execute()
{
    if(m_autoOn == FALSE)
    {
        m_autoOn = TRUE;
#ifndef CT_355
        g_keyInterface.CtrlLight(m_autoOn, LIGHT_AUTO);
#endif
        Do();
    }
    else
    {
        m_autoOn = FALSE;
#ifndef CT_355
        g_keyInterface.CtrlLight(m_autoOn, LIGHT_AUTO);
#endif
        Undo();
    }

    Img2D::GetInstance()->UpdateAutoOptimize(m_autoOn);

    return TRUE;
}

void KeyAutoOptimize::AutoOff(void)
{
    if (m_autoOn)
    {
        m_autoOn = FALSE;
        Img2D::GetInstance()->UpdateAutoOptimize(m_autoOn);
    }
}

bool KeyAutoOptimize::Do()
{
    // bakup current parameter
    BackupPara();

    // apply auto optimize
    ProbeMan* ptrMan = ProbeMan::GetInstance();
    ProbeSocket::ProbePara pPara;
    ptrMan->GetCurProbe(pPara);

    ExamItem e;
    ExamItem::ParaItem ePara;
    e.GetImgOptimize(pPara.model, ePara);
#ifndef CT_340
    if(!ModeStatus::IsD2Mode())
        ePara.d2.spaceCompoundIndex = 0;
#endif
    ImgOptimize(pPara, ePara);

    return TRUE;
}

bool KeyAutoOptimize::Undo()
{
    // back to bakup parameter
    ProbeMan* ptrMan = ProbeMan::GetInstance();
    ProbeSocket::ProbePara pPara;
    ptrMan->GetCurProbe(pPara);

    ImgOptimize(pPara, m_itemPara);

    return TRUE;
}

void KeyAutoOptimize::ImgOptimize(ProbeSocket::ProbePara p, ExamItem::ParaItem i)
{
    ModeStatus ms;
    ScanMode::EScanMode mode = ms.GetScanMode();

    //exit simult
    if (ImgPw::GetInstance()->GetSimut2Status())
    {
        ScanMode::GetInstance()->EnterPwSimult(FALSE);
    }
    if (ImgPw::GetInstance()->GetSimut3Status())
    {
        if (mode == ScanMode::PWPDI_SIMULT)
            ScanMode::GetInstance()->EnterPwPdiSimult(FALSE);
        else
            ScanMode::GetInstance()->EnterPwCfmSimult(FALSE);
    }
#if (defined(CT_440)||defined(CT_161)||defined(CT_360))
    IoCtrl io;
    io.Freeze();
    usleep(700000);
#else
    usleep(500000);
#endif

    // init 2D
    Img2D* ptrImg2D = Img2D::GetInstance();
    ptrImg2D->InitProbe2DOptimize(&p, &i);
    ImgProc2D::GetInstance()->InitOptimize(&(i.d2));

    // init M
    ptrImg2D->InitProbeMOptimize(&p, &i);
    ImgProcM::GetInstance()->InitOptimize(&(i.d2));
#ifndef CT_322
    // init pw
    ImgPw* ptrImgPw = ImgPw::GetInstance();
    ptrImgPw->InitProbeOptimize(&p, &i);
    ImgProcPw::GetInstance()->InitOptimize(&(i.spectrum));

    // init cfm
    ImgCfm* ptrImgCfm = ImgCfm::GetInstance();
    ptrImgCfm->InitProbeOptimize(&p, &i);
    ImgProcCfm::GetInstance()->InitOptimize(&(i.color));
#endif
#if (defined(CT_440)||defined(CT_161)||defined(CT_360))
    usleep(200000);
    io.Unfreeze();
#endif
    // 如果是M模式，则需要重新计算M增益，因为在PW InitProbeOptimize中PW增益覆盖了M增益
    if (ModeStatus::IsMImgMode())
    {
        ptrImg2D->ReSendGainM();
    }
    if (ModeStatus::IsD2ImgMode())
    {
        ptrImg2D->ReSendGain2D();
    }

    //clear screen
    KeyClearScreen kcs;
    kcs.Execute();
    kcs.ModeExecute(true);

    // update img when need
    int curPw = ms.GetPwCurImg();

    if ((mode == ScanMode::PW) && (curPw == 2))
    {
        ScanMode::GetInstance()->SwitchPw();
        usleep(500000);
        ScanMode::GetInstance()->SwitchPw();
    }
    else if ((mode == ScanMode::PWCFM) && (curPw == 2))
    {
        ScanMode::GetInstance()->SwitchPwCfm();
        usleep(500000);
        ScanMode::GetInstance()->SwitchPwCfm();
    }
    else if ((mode == ScanMode::PWPDI) && (curPw == 2))
    {
        ScanMode::GetInstance()->SwitchPwPdi();
        usleep(500000);
        ScanMode::GetInstance()->SwitchPwPdi();
    }

    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
        ImageAreaDraw::GetInstance()->ReDrawLocalZoom();
    }
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::PIP_ZOOM) {
        ImageAreaDraw::GetInstance()->ReDrawMagnifier();
        ImageAreaDraw::GetInstance()->ReDrawPIPBox();
    }
}

void KeyAutoOptimize::BackupPara()
{
    Img2D::GetInstance()->GetCurPara(&m_itemPara);
    ImgProc2D::GetInstance()->GetCurPara(&m_itemPara);
#ifndef CT_340
    if(!ModeStatus::IsD2Mode())
    {
        m_itemPara.d2.spaceCompoundIndex = 0;
        Img2D::GetInstance()->SetCompoundSpace(0);
    }
#endif
#ifndef CT_322
    ImgPw::GetInstance()->GetCurPara(&m_itemPara);
    ImgCfm::GetInstance()->GetCurPara(&m_itemPara);

    ImgProcPw::GetInstance()->GetCurPara(&m_itemPara);
    ImgProcCfm::GetInstance()->GetCurPara(&m_itemPara);
#endif
}

////////////////////////////////////////// [measure] //////////////////////////////////////
bool KeyMeasure::Execute()
{
    AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();

    // printf("in the Measure which type is :%s\n",MenuArea::GetInstance()->GetMenuType());

    if (MenuArea::GetInstance()->GetMenuType() == MenuArea::MEASURE)
    {
        if(g_ptrAbsMeasure != NULL)
        {
            delete g_ptrAbsMeasure;
            g_ptrAbsMeasure = NULL;
        }
        m_ptrUpdate->ExitMeasure();
        MultiFuncUndo();
#if defined (CT_322)
        g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
#endif
    }
    else
    {
        if (g_menuReview.GetLimit() == 2) //note: pic read from external media can not measure
        {
            HintArea::GetInstance()->UpdateHint(_("[Measure]: Measure is disable in current image."), 2);
#if defined (CT_322)
            g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
#endif
        }
        else
        {
            PRINTF("Enter measure\n");
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
            if(g_ptrAbsMeasure != NULL)
            {
                delete g_ptrAbsMeasure;
                g_ptrAbsMeasure = NULL;
            }
            //SetChangePointerMeasure();
            m_ptrUpdate->EnterMeasure();
            //DarkFucusLight();
#if defined (CT_322)
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
            g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
            g_keyInterface.CtrlLight(TRUE,LIGHT_MEASURE);
#endif
        }
    }

    return TRUE;
}

////////////////////////////////////////// [calc] //////////////////////////////////////
bool KeyCalc::Execute()
{
    AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
    if (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC)
    {
        if(g_ptrAbsMeasure != NULL)
        {
            delete g_ptrAbsMeasure;
            g_ptrAbsMeasure = NULL;
        }
        m_ptrUpdate->ExitCalc();
        MultiFuncUndo();
#if (defined (CT_322) || defined(CT_313))
        g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
#endif
    }
    else
    {
        if (g_menuReview.GetLimit() != 0) //note: pic read from archived and usb media can not calc
        {
            HintArea::GetInstance()->UpdateHint(_("[Calc]: Calc is disable in current image."), 2);
        }
        else
        {
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

            if(g_ptrAbsMeasure !=NULL)
            {
                delete g_ptrAbsMeasure;
                g_ptrAbsMeasure = NULL;
            }
        //    SetChangePointerCalc();
            m_ptrUpdate->EnterCalc();
#if defined (CT_322)
            g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
            g_keyInterface.CtrlLight(TRUE,LIGHT_CALC);
#elif defined(CT_313)
            g_keyInterface.CtrlLight(TRUE,LIGHT_CALC);
#endif
        }
    }

    return TRUE;
}

////////////////////////////////////////// [System] //////////////////////////////////////
bool KeySystem::Execute(void)
{
#if 0
    int i;
                for (i = 0; i < 2000; i++)
                {
                    printf("-----------------i = %d\n", i);
                    int num_dests = 0;          /* Number of user destinations */
                    cups_dest_t *dests = NULL;  /* User destinations */

                    num_dests = cupsGetDests(&dests);
                    printf("-------------------------dests :%x\n", dests);
                    //cupsFreeDests(num_dests, dests);

                };
#endif

    AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
    MultiFuncFactory* ptr = MultiFuncFactory::GetInstance();

    if (MenuArea::GetInstance()->GetMenuType() == MenuArea::SYSTEM)
    {
        delete g_ptrAbsMeasure;
        g_ptrAbsMeasure = NULL;
        m_ptrUpdate->ExitSystem();
        MultiFuncUndo();
    }
    else
    {
        delete g_ptrAbsMeasure;
        g_ptrAbsMeasure = NULL;
        m_ptrUpdate->EnterSystem();
        ptr->Create(MultiFuncFactory::NONE);
    }

    return TRUE;
}

////////////////////////////////////////// [cursor] //////////////////////////////////////
bool KeyCursor::Execute()
{
    MultiFuncFactory* ptr = MultiFuncFactory::GetInstance();
    MultiFuncFactory::EMultiFunc type = ptr->GetMultiFuncType();

    if ((type != MultiFuncFactory::EFOV) && (type != MultiFuncFactory::NONE))
    {
       // ptr->Create(MultiFuncFactory::NONE);
       //---------2016.08.26--huyang---------------//
        if(type== MultiFuncFactory::BIOPSY_VERIFY)
       {
           ptr->Create(MultiFuncFactory::BIOPSY);
           SetSystemCursor(90,130);//   SetSystemCursor(90,100);
           doBtnEvent(1, 1);//fake btn press
           doBtnEvent(1, 0);//fake btn unpress
          // SetMenuBiopsyCursorYRange(128,188);
           SetMenuBiopsyCursorYRange(128,250);
       }
       else
       {
            if(type== MultiFuncFactory::BIOPSY)
           {
               //---------------------------------//
               if(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYVERIFY)
               {
                  SetSystemCursor(90,155);//SetSystemCursor(90,130);//
                  doBtnEvent(1, 1);//fake btn press
                  doBtnEvent(1, 0);
               }
               //---------------------------------//
                MultiFuncUndo();
           }
           else
           {
                 ptr->Create(MultiFuncFactory::NONE);
           }
       }
       //--------------------------------------------------//
    }
    else
    {
        MultiFuncUndo();
    }

    return TRUE;
}
////////////////////////////////////////// [text] //////////////////////////////////////
bool KeyText::Execute()
{
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::TEXT);
    return TRUE;
}

////////////////////////////////////////// [arrow] //////////////////////////////////////
bool KeyArrow::Execute()
{
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::ARROW)
        MultiFuncUndo();
    else
    {
#ifdef CT_355
        HintArea::GetInstance()->UpdateHint(_("[Arrow]: <Auto> to change arrow direction."), 2);
#else
        HintArea::GetInstance()->UpdateHint(_("[Arrow]: <Value> to change arrow direction."), 2);
#endif
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::ARROW);
    }
    return TRUE;
}
////////////////////////////// [biopsy] /////////////////////////////////////////////////
bool KeyBiopsy::Execute()
{

    if ((MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSY)||(MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYBRACKET)||(MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYVERIFY))
    {
        MenuShowUndo();
        MultiFuncUndo();
    }
    else
    {
        ModeStatus ms;
        /*if (((ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B))||(ms.IsCFMMode() &&(ms.GetFormatCfm()==FormatCfm::B))||(ms.IsPDIMode() &&(ms.GetFormatCfm()==FormatCfm::B)))&& ms.IsUnFreezeMode())
        {
            MenuArea::GetInstance()->ShowBiopsyMenu();
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY);
        }
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Biopsy]: Only valid in B mode,CFM,PDI and UnFreeze status."), 1);
        }*/
    //-------------------------------------------------------------------//
        if ((((ms.GetScanMode()==ScanMode::D2) && (ms.GetFormat2D() == Format2D::B))||((ms.GetScanMode()==ScanMode::CFM) &&(ms.GetFormatCfm()==FormatCfm::B))||((ms.GetScanMode()==ScanMode::PDI) &&(ms.GetFormatCfm()==FormatCfm::B)))&& ms.IsUnFreezeMode())
        {
            MenuArea::GetInstance()->ShowBiopsyMenu();
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY);
        }
        else
        {
#ifdef CT_322
            HintArea::GetInstance()->UpdateHint(_("[Biopsy]: Only valid in B mode and UnFreeze status."), 1);
#else
            HintArea::GetInstance()->UpdateHint(_("[Biopsy]: Only valid in B mode,CFM,PDI and UnFreeze status."), 1);
#endif
        }

    //---------------------------------------------------------------------//

    }

    return TRUE;

}
////////////////////////////////////////// [BodyMark] //////////////////////////////////////
bool KeyBodyMark::Execute()
{
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BODYMARK);
    return TRUE;
}

////////////////////////////////////////// [save snap] //////////////////////////////////////
static ImgMan::ImgItem item;
static gboolean DelayToSaveSnap(gpointer data)
{
    //  PRINTF("*******************************************START SAVE\n");
    ScanMode::SpecialMeasurePara para;
    ScanMode::GetInstance()->GetSpecialMeasurePara(&para);
    item.para = para;
    ImageArea::GetInstance()->GetSnapData(&item.data, &item.width, &item.height);
    ImgMan::ImgItem item_frm;
    ImageArea::GetInstance()->GetFrmData(&item_frm.data, &item_frm.width, &item_frm.height);

    int format_bak = ImgMan::GetInstance()->GetImgFormat();

    // save to image manage
    if(ImgMan::GetInstance()->GetImgStorage() == ImgMan::UDISK)
    {
        PeripheralMan* ptrPeripheralMan = PeripheralMan::GetInstance();

        if(!ptrPeripheralMan->CheckUsbStorageState())
        {
            HintArea::GetInstance()->UpdateHint(_("Error: No USB device Plug-in!"), 2);
            free(data);
            free(item.data);
            item.data = NULL;
            return ERROR;
        }
        if(!ptrPeripheralMan->MountUsbStorage())
        {
            HintArea::GetInstance()->UpdateHint(_("Error: Failed to mount USB device!"), 2);
            free(data);
            free(item.data);
            item.data = NULL;
            return ERROR;
        }

        if(ImgMan::GetInstance()->SaveSnap(0, (char*)data, UDISK_PATH, &item) == 0)
        {
            ImgMan::GetInstance()->SetImgFormat(ImgMan::FRM);
            ImgMan::GetInstance()->SaveSnap(0, (char*)data, UDISK_PATH, &item_frm);
            ImgMan::GetInstance()->SetImgFormat(format_bak);
        }

        ptrPeripheralMan->UmountUsbStorage();
    }
    else // save to harddisk
    {
        if(ImgMan::GetInstance()->SaveSnap(0, (char*)data, STORE_PATH, &item) == 0)
        {
            ImgMan::GetInstance()->SetImgFormat(ImgMan::FRM);
            ImgMan::GetInstance()->SaveSnap(0, (char*)data, STORE_PATH, &item_frm);
            ImgMan::GetInstance()->SetImgFormat(format_bak);
        }
    }

    //  PRITNF("item.data = 0x%x\n", item.data);
    free(item.data);
    item.data = NULL;
    free(item_frm.data);
    free(data);
    //  PRINTF("*******************************************END SAVE\n");

    if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW)
    {
        g_menuReview.UpdatePreviewList();
        g_menuReview.UpdateLabel();
    }

#if 0
    // update review menu
    MenuArea* ptrMenu = MenuArea::GetInstance();
    if (ptrMenu->GetMenuType() == MenuArea::REVIEW)
        ptrMenu->ShowReviewMenu();
#endif

    return FALSE;
}

static int SaveSnap(gpointer data)
{
    //  PRINTF("SaveSnap file name: %s\n", (char*)data);

    HintArea::GetInstance()->UpdateHint(_("Saving..."), 0);

    if (MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW)
    {
        g_menuReview.HideTooltips();
    }

    g_timeout_add(100, DelayToSaveSnap, data);

    return OK;
}

bool KeySaveSnap::Execute()
{
    // get current image para
    //  ImgMan::ImgItem item;
    if(item.data)
        return  TRUE;
    //  ImageArea::GetInstance()->GetSnapData(&item.data, &item.width, &item.height);
    //  PRINTF("data=0x%x\n", item.data);

    SysOptions so;
    if(!so.GetImageAutoName())
    {
        if(!ModeStatus::IsFreezeMode())
            FreezeMode::GetInstance()->PressFreeze();
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::FILENAME,
                _("Please input the file name(without suffix)"),
                &SaveSnap);
    }
    else
    {
        // generate file name automaticly
        time_t at;
        at = time(&at);

        struct tm* ct;
        ct = localtime(&at);

        char *fileName = (char*)malloc(256);
        memset(fileName, 0, 15);
        sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10);

        SaveSnap(fileName);
    }

    return TRUE;
}

////////////////////////////////////////// [Menu] //////////////////////////////////////
bool KeyReview::m_menuRead = FALSE;
bool KeyReview::Execute()
{
    MenuArea* ptrMenu = MenuArea::GetInstance();
    MultiFuncFactory* ptrFunc = MultiFuncFactory::GetInstance();

    if (m_menuRead)
    {
        //FreezeMode::GetInstance()->PressUnFreeze();
        MenuShowUndo();
        m_menuRead = FALSE;
#if (defined (CT_322) || defined(CT_313))
        g_keyInterface.CtrlLight(FALSE,LIGHT_READ);
#endif
    }
    else
    {
        // freeze
        if (!ModeStatus::IsFreezeMode())
            FreezeMode::GetInstance()->PressFreeze();
        ptrMenu->ShowReviewMenu();
        ptrFunc->Create(MultiFuncFactory::NONE);
        m_menuRead = TRUE; // must after ShowReviewMenu(HideAllMenu), be care of "HideMenuReview"
#if (defined (CT_322) || defined(CT_313))
        g_keyInterface.CtrlLight(TRUE,LIGHT_READ);
#endif
    }

    return TRUE;
}

void KeyReview::HideMenuReview()
{
    if (m_menuRead)
        m_menuRead = FALSE;
}

void KeyReview::ExitMenuReivew()
{
    if (m_menuRead)
    {
        MenuShowUndo();
        m_menuRead = FALSE;

        FreezeMode::GetInstance()->PressUnFreeze();
#if (defined (CT_322) || defined(CT_313))
        g_keyInterface.CtrlLight(m_menuRead,LIGHT_READ);
#endif

    }
}

void KeyReview::SetDefaultIDAndPath()
{
    // set menureview's file path and folder name
    if (!m_menuRead)
    {
        PatientInfo::Info info;
        g_patientInfo.GetInfo(info);
        g_menuReview.SetLimit(0);
        g_menuReview.SetPatientID(0);
        g_menuReview.SetImgPath((const char*) STORE_PATH);
    }
}

#define DEMO_PATH "/mnt/harddisk/demo"
void KeyReview::SetDemoIDAndPath()
{
    // set menureview's file path and folder name
    if (!m_menuRead)
    {
        PatientInfo::Info info;
        g_patientInfo.GetInfo(info);
        g_menuReview.SetLimit(0);
        g_menuReview.SetPatientID(0);
        g_menuReview.SetImgPath((char*) DEMO_PATH);
    }
}
////////////////////////////////////////// [PIP] //////////////////////////////////////
bool KeyPIP::Execute()
{
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::PIP_ZOOM)
    {
        MultiFuncUndo();
    }
    else
    {
        ModeStatus ms;
        if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode())
        {
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::PIP_ZOOM);
        }
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[PIP]: Only valid in B mode and UnFreeze status."), 1);
        }
    }

    return TRUE;
}

////////////////////////////////////////// [LocalZoom] //////////////////////////////////////
bool KeyLocalZoom::Execute()
{
    if (ExitLocalZoom())
    {
        return TRUE;

    }

    if(MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
    {
        MultiFuncUndo();
        Zoom::GetInstance()->LocalZoomCtrl(FALSE);
#if (defined (CT_322) || defined(CT_313))
        g_keyInterface.CtrlLight(FALSE,LIGHT_ZOOM);
#endif
    }
    else
    {
        ModeStatus ms;
        SysOptions sysOption;

        bool inSnap = Replay::GetInstance()->GetReadSnapStatus();
        if (inSnap)
        {
            HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when in status read picture."), 1);
            return TRUE;
        }

        if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && (ms.IsUnFreezeMode() || ms.IsPureFreezeMode()))
            //if ((ms.IsD2Mode()||ms.IsPureColorMode()) && (ms.GetFormat2D() == Format2D::B) && (ms.IsUnFreezeMode() || ms.IsPureFreezeMode()))
        {
            if ((Img2D::GetInstance()->GetTpViewStatus()) || (g_menuBiopsy.GetDrawStatus()) || (!sysOption.GetCenterLine()))
            {
                HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when Tp-view, biopsy line or centerline is on."), 1);
            }
            else
            {
                int steer = Img2D::GetInstance()->GetSteer();
                if (steer == 0)
                {
                    int rotate = ImgProc2D::GetInstance()->GetRotate();
                    if (rotate == 0)
                    {
                        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::LOCAL_ZOOM);
#if (defined (CT_322) || defined(CT_313))
                        g_keyInterface.CtrlLight(TRUE,LIGHT_ZOOM);
#endif
                    }
                    else
                        HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when image is rotated."), 1);
                }
                else
                {
                    HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Invalid when image is steered."), 1);
                }
            }
        }
        else
        {
#if (defined(CT_322) || defined(CT_313))
            HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Only valid in B mode and not replay status."), 1);
#else
            HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Only valid in 2D B mode and not replay status."), 1);
#endif
        }
    }
    return TRUE;
}

bool KeyLocalZoom::ExitLocalZoom()
{
    ModeStatus s;
    int flag = 0;

    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
#if (defined (CT_322) || defined (CT_313))
        g_keyInterface.CtrlLight(FALSE,LIGHT_ZOOM);
#endif
        if(MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
        {
            MultiFuncUndo();
        }

        // judge mode
        if (s.IsColorMode())
            flag = 1;
        else if (s.IsSpectrumMode())
            flag = 2;
        else if (s.IsSpectrumColorMode())
            flag = 3;
        else if (s.IsMMode())
            flag = 4;

        // clear line according to mode
        if ((flag == 1) || (flag == 3))
            ImgCfm::GetInstance()->ClearBox();
        if ((flag == 2) || (flag == 3))
            ImgPw::GetInstance()->ClearSv();
        else if (flag == 4)
            Img2D::GetInstance()->ClearMLine();

        // clear biopsy line
        g_menuBiopsy.ClearBiopsyLine();

        Zoom::GetInstance()->LocalZoomCtrl(FALSE);

        //redraw biopsyline
        g_menuBiopsy.UpdateBiopsyLine();

        // redraw line according to mode
        if ((flag == 1) || (flag == 3))
            ImgCfm::GetInstance()->ReDrawBox();
        if ((flag == 2) || (flag == 3))
            ImgPw::GetInstance()->ReDrawSv();
        else if (flag == 4)
            Img2D::GetInstance()->ReDrawMLine();
        HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Exit."), 1);
        return TRUE;
    }
    else
    {
        Zoom::GetInstance()->UpdateLocalZoom(1.0);
    }

    return FALSE;
}
////////////////////////////////////////// [focus] //////////////////////////////////////
bool KeyFocus::Execute()
{
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::FOCUS)
    {
        MultiFuncUndo();
#if (defined (CT_322) || defined (CT_313))
        g_keyInterface.CtrlLight(FALSE,LIGHT_FOCUS);
#endif
    }
    else
    {
        ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();
        if ((mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT) || (mode == ScanMode::EFOV))
        {
            HintArea::GetInstance()->UpdateHint(_("[Focus]: Invalid when simult is on."), 1);
        }
        else
        {
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::FOCUS);
            // DarkFucusLight();
#if (defined (CT_322) || defined (CT_313))
            g_keyInterface.CtrlLight(TRUE,LIGHT_FOCUS);
#endif
        }
    }

    return TRUE;
}
////////////////////////////////////////// [depth] //////////////////////////////////////
bool KeyDepth::ExecuteAdd()
{
    DepthExe(ADD);
    return TRUE;
}

bool KeyDepth::ExecuteSub()
{
    DepthExe(SUB);
    return TRUE;
}

void KeyDepth::DepthExe(EKnobOper oper)
{
    ModeStatus s;
    int flag = 0;
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();

    if (s.IsColorMode())
        flag = 1;
    else if (s.IsSpectrumMode())
        flag = 2;
    else if (s.IsSpectrumColorMode())
        flag = 3;
    else if (s.IsMMode())
        flag = 4;

    if ((flag == 1) || (flag == 3))
        ImgCfm::GetInstance()->ClearBox();
    if ((flag == 2) || (flag == 3))
        ImgPw::GetInstance()->ClearSv();
    else if (flag == 4)
        Img2D::GetInstance()->ClearMLine();

    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
        Zoom::GetInstance()->ClearLocalZoom();

    // clear biopsy line
    g_menuBiopsy.ClearBiopsyLine();
    //prepare
    ImgCfm::GetInstance()->ChangeDepthBefore();

    //apply
    if (Img2D::GetInstance()->ChangeDepth(oper) != ERROR)
    {
        ImgCfm::GetInstance()->ChangeDepth();
        ImgPw::GetInstance()->ChangeDepth();
    }
    if ((flag == 1) || (flag == 3))
        ImgCfm::GetInstance()->ReDrawBox();
    if ((flag == 2) || (flag == 3))
        ImgPw::GetInstance()->ReDrawSv();
    else if (flag == 4)
        Img2D::GetInstance()->ReDrawMLine();

    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
        Zoom::GetInstance()->RedrawLocalZoom();

    // redraw biopsy line
    g_menuBiopsy.UpdateBiopsyLine();

    // change Tis
    ChangeTis();
}

bool KeyDepth::Execute()
{

    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::DEPTH)
    {
        MultiFuncUndo();
#if defined (CT_313)
        g_keyInterface.CtrlLight(FALSE,LIGHT_DEPTH);
#endif
    }
    else
    {
#if defined (CT_313)
        g_keyInterface.CtrlLight(TRUE,LIGHT_DEPTH);
#endif
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::DEPTH);
    }

    return TRUE;
}

////////////////////////////////////////// [freq.] //////////////////////////////////////
bool KeyFreq::Execute()
{
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::FREQ)
    {
        MultiFuncUndo();
#if (defined (CT_322) || defined (CT_313))
        g_keyInterface.CtrlLight(FALSE,LIGHT_FREQ);
#endif
    }
    else
    {
        ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();
        if ((mode == ScanMode::PW_SIMULT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
        {
            HintArea::GetInstance()->UpdateHint(_("[Freq]: Invalid when simult is on."), 1);
        }
        else
        {
#if (defined (CT_322) || defined (CT_313))
            // DarkFucusLight();
            g_keyInterface.CtrlLight(TRUE,LIGHT_FREQ);
#endif
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::FREQ);
        }
    }

    return TRUE;
}
////////////////////////////////////////// [Chroma] //////////////////////////////////////
bool KeyChroma::Execute()
{
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::CHROMA)
    {
        MultiFuncUndo();
#if defined (CT_322)
        g_keyInterface.CtrlLight(FALSE,LIGHT_CHROMA);
#endif
    }
    else
    {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::CHROMA);
#if defined (CT_322)
        // DarkFucusLight();
        g_keyInterface.CtrlLight(TRUE,LIGHT_CHROMA);
#endif
    }

    return TRUE;
}

////////////////////////////////////////// [clear screen] //////////////////////////////////////
bool KeyClearScreen::Execute()
{
    DeleteDataForClearScreen();
    // draw centerline
    KeyCenterLine key;
    //key.Clear();

    DrawHistogram::GetInstance()->SetOnOff(0);
    if ((MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSY)||(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYBRACKET)||(MenuArea::GetInstance()->GetMenuType()==MenuArea::BIOPSYVERIFY))
    {
        MenuShowUndo();//2016.11.07--solve biopsy menu don't exit after biopsy line erased or biopsy line appeared when biopsy menu exited
        MultiFuncUndo();//2016.11.07--solve problem that biopsy verify don't exit.
    }

    ImageArea::GetInstance()->ClearScreen();

    if (Replay::GetInstance()->GetReadSnapStatus()) {
        ImageArea::GetInstance()->ResetReadImg();
        return true;
    }

    key.Execute();

    // Biopsy line
    g_menuBiopsy.UpdateBiopsyLine();

    // if or not freeze
    if (!ModeStatus::IsUnFreezeMode())
    {
        Replay::GetInstance()->DisplayReplayBar();
    }

    return TRUE;
}

bool KeyClearScreen::UnFreezeExecute(bool update)
{
    // draw centerline
    KeyCenterLine key;
    //key.Clear();
    DrawHistogram::GetInstance()->SetOnOff(0);
    ImageArea::GetInstance()->ClearScreenUnFreeze();

    if (Replay::GetInstance()->GetReadSnapStatus()) {
        //ImageArea::GetInstance()->ResetReadImg();
        return true;
    }

    key.Execute();

    // Biopsy line
    g_menuBiopsy.UpdateBiopsyLine();

    // if or not freeze
    if (!ModeStatus::IsUnFreezeMode())
    {
        Replay::GetInstance()->DisplayReplayBar();
    }

    return TRUE;
}

bool KeyClearScreen::ModeExecute(bool update)
{
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();

    //ImageArea::GetInstance()->ClearArea();
    DeleteDataForClearScreen();

    AbsUpdate2D* ptrUpdate2D = GlobalClassMan::GetInstance()->GetUpdate2D();
    AbsUpdatePw* ptrUpdatePw = GlobalClassMan::GetInstance()->GetUpdatePw();
    AbsUpdateCfm* ptrUpdateCfm = GlobalClassMan::GetInstance()->GetUpdateCfm();

    switch(mode) {
        case ScanMode::D2:
            ptrUpdate2D->Enter2DImg(update);
            break;
        case ScanMode::M_INIT:
            {
                ptrUpdate2D->Enter2DImg();
                ptrUpdate2D->EnterMImg();
            }
            break;
        case ScanMode::M:
            ptrUpdate2D->UpdateMImg();
            break;
        case ScanMode::CW_INIT:
        case ScanMode::PW_INIT:
            {
                ptrUpdate2D->Enter2DImg();
                ptrUpdatePw->EnterPwImg();
            }
            break;
        case ScanMode::PW:
        case ScanMode::PW_SIMULT:
        case ScanMode::CW:
            ptrUpdatePw->UpdatePwImg();
            break;
        case ScanMode::PWCFM_INIT:
       case ScanMode::CWCFM_INIT:
            {
                ptrUpdateCfm->EnterCfmImg();
                ptrUpdatePw->EnterPwImg();
            }
            break;
        case ScanMode::PWPDI_INIT:
       case ScanMode::CWPDI_INIT:
            {
                ptrUpdateCfm->EnterCfmImg();
                ptrUpdatePw->EnterPwImg();
            }
            break;
        case ScanMode::PWCFM:
        case ScanMode::PWCFM_SIMULT:
        case ScanMode::PWPDI:
        case ScanMode::PWPDI_SIMULT:
       case ScanMode::CWCFM:
       case ScanMode::CWPDI:
            ptrUpdatePw->UpdatePwCfmImg();
            break;
        case ScanMode::CFM:
        case ScanMode::PDI:
            ptrUpdateCfm->EnterCfmImg(update);
            break;

        case ScanMode::CFM_VS_2D:
        case ScanMode::PDI_VS_2D:
            ptrUpdateCfm->EnterCfmVs2D();
            break;

        case ScanMode::ANATOMIC_M:
            ptrUpdate2D->EnterAnatomicM();

        case ScanMode::EFOV:
            break;
    }
    return true;
}

void KeyClearScreen::DeleteDataForClearScreen()
{
    // clear arrow data
    CArrow::DeleteAllForFreeze();

    // clear text data
    NoteArea::GetInstance()->Hide();
    ImageAreaDraw::GetInstance()->ClearMagnifier();

    // clear measure
    MeasureMan::GetInstance()->DeleteAllForClearScreen();

    if (g_ptrAbsMeasure != NULL) {
        delete g_ptrAbsMeasure;
        g_ptrAbsMeasure = NULL;
        //MultiFuncUndo();
        ModeStatus ms;
        ScanMode::EScanMode mode = ms.GetScanMode();
        if(mode != ScanMode::EFOV)
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
        else
            MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::EFOV);
    }

}

////////////////////////////////////////// [End Exam] //////////////////////////////////////
int OpenNewPatWindow(gpointer data)
{
    ViewNewPat::GetInstance()->CreateWindow();
    return 0;
}

int EndExam(gpointer data)
{
    PRINTF("begin Exam\n");
    string errmsg;
    // archive patient info
    if (g_patientInfo.ArchivePatientInfo(errmsg)) {
#ifndef VET
        ViewNewPat::GetInstance()->UpdateStudyInfo();
#endif
        g_menuCalc.ClearAllData();
        g_menuMeasure.ClearAllData();
        // archive and clear image and viedo
        //g_patientInfo.ArchiveImg();
        // archive report
        ViewReport::GetInstance()->LoadMeasureDataForArchive();
        g_patientInfo.ArchiveReport();
        // archive and clear image and viedo
        g_patientInfo.ArchiveImg();
        // clear patient info
        g_patientInfo.ClearAll();
        // clear desctiption and comment
        ViewReport::GetInstance()->ClearIndicationandComments();
        //clear screen
        KeyClearScreen kcs;
        kcs.Execute();

        // update review menu
        MenuArea* ptrMenu = MenuArea::GetInstance();
        if (ptrMenu->GetMenuType() == MenuArea::REVIEW)
            ptrMenu->ShowReviewMenu();

        if (GTK_IS_WIDGET(ViewNewPat::GetInstance()->GetWindow())) {
            if (ViewNewPat::GetInstance()->GetClearStatus())
                ViewNewPat::GetInstance()->ClearData();
            else
                ViewNewPat::GetInstance()->ClearExamData();
        }
#ifndef VET
         setlocale(LC_NUMERIC, "en_US.UTF-8");
        if(CDCMMan::GetMe()->EndStudy())
        {
            SysDicomSetting sysDicomSetting;
            if(sysDicomSetting.GetMPPS())
            {
                if(ViewNewPat::GetInstance()->GetMPPSFlag())
                {
                    int Year, Month, Day, Hour, Minute, Second;
                    GetCurrentDateTime(Year, Month, Day, Hour, Minute, Second);

                    string year, mon, day, studyEndDate;
                    ChangeDateFormatToString(Year, Month, Day, year, mon, day);
                    studyEndDate = year+mon+day;

                    string hour, min, sec, studyEndTime;
                    ChangeTimeFormatToString(Hour, Minute, Second, hour, min, sec);
                    studyEndTime = hour + min + sec;

                    CDCMMan::GetMe()->EndMPPS(studyEndDate,studyEndTime);
                    ViewNewPat::GetInstance()->SetMPPSFlag(false);
                }
            }

            SysGeneralSetting sysGS;
            int language= sysGS.GetLanguage();
            if (ZH == language)
            {
                setlocale(LC_NUMERIC, "zh_CN.UTF-8");
            }
            else if (RU == language)
            {
                setlocale(LC_NUMERIC, "ru_RU.UTF-8");
            }
            else if (PL == language)
            {
                setlocale(LC_NUMERIC, "pl_PL.UTF-8");
            }
            else if (FR == language)
            {
                setlocale(LC_NUMERIC, "fr_FR.UTF-8");
            }
            else if (DE == language)
            {
                setlocale(LC_NUMERIC, "de_DE.UTF-8");
            }
            else if(ES == language)
            {
                setlocale(LC_NUMERIC, "es_ES.UTF-8");
            }
            else
            {
                setlocale(LC_NUMERIC, "en_US.UTF-8");
            }

            ViewWorkList::GetInstance()->ClearQueryRes();
            ViewNewPat::GetInstance()->ClearStudyInfo();
            PRINTF("End Exam\n");
        }
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[End Exam]: All details of patient has been archived."), 1);
            PRINTF("End Exam\n");
        }
#else
        HintArea::GetInstance()->UpdateHint(_("[End Exam]: All details of animal has been archived."), 1);
        PRINTF("End Exam\n");
#endif
    }
    else {

        if (GTK_IS_WIDGET(ViewNewPat::GetInstance()->GetWindow())) {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()), ViewDialog::ERROR, _(errmsg.c_str()), NULL);
        }
        else {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()), ViewDialog::ERROR, _(errmsg.c_str()), OpenNewPatWindow, OpenNewPatWindow);
        }
    }
    return 0;
}

bool KeyEndExam::Execute()
{
    const char* info = N_("End Exam?");

    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()), ViewDialog::QUESTION, _(info), EndExam);
    PRINTF("end end exam execute\n");

    return TRUE;
}

bool KeyCenterLine::Execute()
{
    ModeStatus ms;
    if (((ms.GetScanMode() == ScanMode::D2) && (ms.GetFormat2D() == Format2D::B))
            || (((ms.GetScanMode() == ScanMode::CFM) || (ms.GetScanMode() == ScanMode::PDI)) && (ms.GetFormatCfm() == FormatCfm::B))
            || (ms.GetScanMode() == ScanMode::PWCFM_INIT) //solve that the center line is cleaned int pwcmf_init mode. lhm
            || (ms.GetScanMode() == ScanMode::PW_INIT))//solve that the center line is cleaned int pw_init mode. lhm
    {
        SysOptions sys;
        ImageAreaDraw::GetInstance()->ClearCenterLine();
        if (sys.GetCenterLine() == 0)
        {
            ImageAreaDraw::GetInstance()->DrawCenterLine();
        }
        else
        {
            ImageAreaDraw::GetInstance()->ClearCenterLine();
        }
    }

    return TRUE;
}

void KeyCenterLine::Clear()
{
    ModeStatus ms;
    SysOptions sys;

    if (((ms.GetScanMode() == ScanMode::D2) && (ms.GetFormat2D() == Format2D::B))
            || (((ms.GetScanMode() == ScanMode::CFM) || (ms.GetScanMode() == ScanMode::PDI)) && (ms.GetFormatCfm() == FormatCfm::B))) {
        //if (sys.GetCenterLine() == 0)
        //  ImageAreaDraw::GetInstance()->ClearCenterLine();
    }
    return ;
}

bool KeyMenu::Execute()
{
    if (MenuArea::GetInstance()->GetMenuVisible())
        MenuArea::GetInstance()->HideMenu();
    else
        MenuArea::GetInstance()->ShowMenu();

    return TRUE;
}

////////////////////////////////////////// [Tis] //////////////////////////////////////
void ChangeTis()
{
    ModeStatus s;
    int curImg = s.GetPwCurImg();

    if (ModeStatus::IsSpectrumImgMode())
    {
        if (curImg == 2)
            ImgPw::GetInstance()->ChangePwTis();
        else if (curImg == 1)
            Img2D::GetInstance()->Change2DTis();
    }
    else if (ModeStatus::IsColorImgMode())
    {
        ImgCfm::GetInstance()->ChangeCfmTis();
    }
    else if (ModeStatus::IsSpectrumColorImgMode())
    {
        if (curImg == 2) //PW
            ImgPw::GetInstance()->ChangePwTis();
        else if (curImg == 1)
            ImgCfm::GetInstance()->ChangeCfmTis();
    }
    else
    {
        Img2D::GetInstance()->Change2DTis();
    }
}
void DarkFucusLight()
{
#if defined (CT_322)
    g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
    g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
    g_keyInterface.CtrlLight(FALSE,LIGHT_SYSTEM);
    g_keyInterface.CtrlLight(FALSE,LIGHT_CURSOR);
    g_keyInterface.CtrlLight(FALSE,LIGHT_BODYMARK);
    g_keyInterface.CtrlLight(FALSE,LIGHT_READ);
    g_keyInterface.CtrlLight(FALSE,LIGHT_SAVE);
    g_keyInterface.CtrlLight(FALSE,LIGHT_TSI);
#elif defined (CT_313)
    g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
    g_keyInterface.CtrlLight(FALSE,LIGHT_CURSOR);
    g_keyInterface.CtrlLight(FALSE,LIGHT_READ);
#endif
}