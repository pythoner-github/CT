#include "imageControl/UpdateCfm.h"
#include "imageControl/KnobCfm.h"
#include "imageProc/MenuCFM.h"
#include "keyboard/KeyFunc.h"
#include "display/ImageAreaPara.h"
#include "display/ImageAreaDraw.h"
#include "display/KnobMenu.h"
#include "probe/MenuBiopsy.h"
#include "ViewMain.h"

const char * Toggle_Cfm[2] =
{
    N_("OFF"),
    N_("ON")
};

UpdateCfm::UpdateCfm()
{
    m_ptrImgPara = ImageAreaPara::GetInstance();
    m_ptrImgDraw = ImageAreaDraw::GetInstance();
    m_ptrMenu = MenuArea::GetInstance();
}

void UpdateCfm::SetBoxStatus(bool changeSize)
{
    m_ptrImgDraw->ClearCfmBox();
    m_ptrImgDraw->SetCfmBoxDashStatus(changeSize);
    m_ptrImgDraw->ReDrawCfmBoxForFreeze();
}

void UpdateCfm::Box(int lineBegin, int lineEnd, int dotBegin, int dotEnd)
{
    PRINTF("draw: line(%d, %d), dot(%d, %d)\n", lineBegin, lineEnd, dotBegin, dotEnd);
    m_ptrImgDraw->Update();
    m_ptrImgDraw->DrawCfmBox(lineBegin, lineEnd, dotBegin, dotEnd);
    if (ModeStatus::IsSpectrumColorMode()) {
#ifdef EMP_355
 int mode = ViewMain::GetInstance()->GetModeIsFlag();
        if(mode)
        {
            m_ptrImgDraw->ClearSVCW();
            m_ptrImgDraw->ReDrawSVCW();
        }
        else{
            m_ptrImgDraw->ClearSV();
            m_ptrImgDraw->ReDrawSV();
        }
#else
    m_ptrImgDraw->ClearSV();
    m_ptrImgDraw->ReDrawSV();
#endif
    }
    else if (ModeStatus::IsCWColorMode()) {
        m_ptrImgDraw->ClearSVCW();
        m_ptrImgDraw->ReDrawSVCW();
    }
}
void UpdateCfm::BoxClear()
{
    m_ptrImgDraw->Update();
    m_ptrImgDraw->ClearCfmBox();
}
void UpdateCfm::BoxReDraw()
{
    m_ptrImgDraw->Update();
    m_ptrImgDraw->ReDrawCfmBox();
}
void UpdateCfm::GainCfm(int gain)
{
    m_ptrImgPara->UpdateCfmGain(gain);
}
void UpdateCfm::PRF(int data, EKnobReturn status)
{
    bool draw;

    sprintf(m_str, "%.2fKHz", (float)data/1000);
    SyncKnobCfm(CFM_SCALE, m_str, status);
    if (ModeStatus::IsColorImgMode() || ModeStatus::IsSpectrumColorMode())
        draw = TRUE;
    else
        draw = FALSE;
    m_ptrImgPara->UpdateCfmPRF(data, draw);
}
void UpdateCfm::PRFRange(int data, EKnobReturn status)
{
    sprintf(m_str, "%.2fKHz", (float)data/1000);
    SyncKnobCfm(CFM_SCALE, m_str, status);
}
void UpdateCfm::Baseline(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_BASELINE, m_str, status);
}
void UpdateCfm::WallFilter(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_WALL_FILTER, m_str, status);
    m_ptrImgPara->UpdateCfmWF(index);
    ImageArea::GetInstance()->UpdateImgArea();
}
#if 0
char * LineDensityLevel[] = {
    _("LOW"),
    _("HIGH")
};
#endif
void UpdateCfm::LineDensity(int index, EKnobReturn status)
{

    const char * Level[2] = {
    N_("LOW"),
    N_("HIGH")
    };

    SyncKnobCfm(CFM_LINE_DENSITY,(char*)Level[index], status);
    //m_ptrImgPara->Update2DLineDensity(index);
}
void UpdateCfm::PacketSize(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_SENSITIVE, m_str, status);
    m_ptrImgPara->UpdateCfmSensitive(index);
}
void UpdateCfm::ColorMap(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_MAP, m_str, status);
}
void UpdateCfm::Turbo(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_TURBO, m_str, status);
}

void UpdateCfm::ScanLines()
{
    int index = Img2D::GetInstance()->GetScanLineIndex();
    EKnobReturn ret = OK;
    if(index == 3)
        ret = MAX;
    else if (index == 0)
        ret = MIN;
    else
        ret = OK;
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_SCAN_LINES, m_str, ret);
}

void UpdateCfm::SetVelRange(double max, double min, double maxKHz, double minKHz)
{
    m_ptrImgDraw->SetColorScaleRange(max, min, maxKHz, minKHz);
}

void UpdateCfm::ColorInvert(bool data, EKnobReturn status)
{
    SyncKnobCfm(CFM_INVERT, (char*)Toggle_Cfm[data], status);
}
void UpdateCfm::FlowOpt(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    SyncKnobCfm(CFM_FLOW_OPT, m_str, status);
}

void UpdateCfm::FocPosRatio(int index, EKnobReturn status)
{
    if (index == 0)
        sprintf(m_str, "1/4");
    else if (index == 1)
        sprintf(m_str, "1/2");
    else
        sprintf(m_str, "3/4");
    SyncKnobCfm(CFM_FOC_POS, m_str, status);
}

void UpdateCfm::EnterCfm()
{
    EnterCfmMenu();
    EnterCfmKnob();
    EnterCfmImg();
}

void UpdateCfm::EnterCfmMenu(void)
{
    // change to cfm menu
    m_ptrMenu->ShowCFMMenu();
}

void UpdateCfm::EnterCfmKnob(void)
{
    // change to cfm knob
    KnobCfmCreate();
}

// update参数为true时，使用在自动优化时的清屏，解决进入模式时某些非实时情况下的标志绘制
// 默认参数为false, 切换模式时使用
void UpdateCfm::EnterCfmImg(bool update)
{
    // change to cfm image area
    m_ptrImgDraw->ClearTextArea(update);

    // draw centerline
    KeyCenterLine key;
    key.Execute();

    m_ptrImgDraw->Update();
    m_ptrImgDraw->ReDrawCfmBox();
    if (update) {
        m_ptrImgDraw->SetSDFreeze();
        m_ptrImgDraw->SetScaleYFreeze();
    }
    m_ptrImgDraw->DrawScanDirection();
    m_ptrImgDraw->DrawScaleY();
    m_ptrImgDraw->DrawGrayScale();
    m_ptrImgDraw->DrawColorScale();
    m_ptrImgPara->DrawGeneralPara();
    m_ptrImgPara->DrawCfmPara();
    m_ptrImgDraw->ReDrawFocus();
#ifdef TRANSDUCER
 //redraw transducer
    m_ptrImgPara->ReDrawTransducer();
#endif

     g_menuBiopsy.UpdateBiopsyLine();//2016.09.18--hy

}

void UpdateCfm::EnterPdi()
{
    EnterCfm();
}

void UpdateCfm::EnterCfmVs2D(void)
{
    EnterCfm();
}

void UpdateCfm::ChangeFormatCfm(int format)
{
    m_ptrImgDraw->ClearTextArea();
    m_ptrImgDraw->Update();
    m_ptrImgDraw->DrawScanDirection();
    m_ptrImgDraw->DrawScaleY();
    m_ptrImgDraw->DrawGrayScale();
    m_ptrImgDraw->DrawColorScale();
    m_ptrImgPara->DrawGeneralPara();
    m_ptrImgPara->DrawCfmPara();
    m_ptrImgDraw->ReDrawCfmBox();
    m_ptrImgDraw->ReDrawFocus();

    ImageArea::GetInstance()->RedrawBodyMarkWhenModeChanged();
#ifdef TRANSDUCER
    m_ptrImgPara->ReDrawTransducer();
#endif
}

void UpdateCfm::ChangeCurrentImgBB(int current)
{
    m_ptrImgDraw->SetCfmBoxFreeze();
    m_ptrImgDraw->Update();
    m_ptrImgDraw->UnSetCfmBoxFreeze();
    m_ptrImgDraw->ReDrawCfmBox();

//  ImageArea::GetInstance()->ChangeCurrentBDMK(current);
}

//解决解冻后box有错误的问题 zjx
void UpdateCfm::ChangeCurrentImg4BForFreeze(int current)
{
    m_ptrImgDraw->SetCfmBoxFreeze(current);
    m_ptrImgDraw->Update();
    m_ptrImgDraw->UnSetCfmBoxFreeze();
    m_ptrImgDraw->ReDrawCfmBox();

    //  ImageArea::GetInstance()->ChangeCurrentBDMK(current);
}

void UpdateCfm::ChangeCurrentImg4B(int current)
{
    if (ModeStatus::IsUnFreezeMode())
    {
        m_ptrImgDraw->SetCfmBoxFreeze();
    }
    else
    {
        m_ptrImgDraw->SetCfmBoxFreeze(current);
    }
    m_ptrImgDraw->Update();
    m_ptrImgDraw->UnSetCfmBoxFreeze();
    m_ptrImgDraw->ReDrawCfmBox();

//  ImageArea::GetInstance()->ChangeCurrentBDMK(current);
}

void UpdateCfm::ColorTable(unsigned char table[][4], int len, int width, int bitDepth)
{
    m_ptrImgDraw->SetColorTable(table, len, width, bitDepth);
}

// post process
void UpdateCfm::BackgroundHide(bool on)
{
    g_menuCFM.UpdateBackgroundHide(on);
}
void UpdateCfm::BCWidth(bool on)
{
    g_menuCFM.UpdateBCWidth(on);
}
void UpdateCfm::CfmCompare(bool on)
{
    g_menuCFM.UpdateCfmCompare(on);
}
void UpdateCfm::PdiDirStatus(bool on)
{
    g_menuCFM.UpdatePdiDir(on);
}
void UpdateCfm::Threshold(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    g_menuCFM.UpdateThreshold(m_str, status);
}
void UpdateCfm::Persist(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    g_menuCFM.UpdatePersist(m_str, status);
    m_ptrImgPara->UpdateCfmPersist(index);
}

void UpdateCfm::Reject(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    g_menuCFM.UpdateReject(m_str, status);
}

void UpdateCfm::Smooth(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    g_menuCFM.UpdateSmooth(m_str, status);
}

void UpdateCfm::Artifact(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
    g_menuCFM.UpdateArtifact(m_str, status);
}

///////////////////////////////  test  //////////////////////////////////////
void UpdateCfm::TissueGate(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
//  SyncKnobCfm(CFM_TISSUE, m_str, status);
}

void UpdateCfm::NoiseGate(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
//  SyncKnobCfm(CFM_NOISE, m_str, status);
}

void UpdateCfm::SpeedGate(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
//  SyncKnobCfm(CFM_RATE, m_str, status);
}

void UpdateCfm::VarianceGate(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
//  SyncKnobCfm(CFM_VARIANCE, m_str, status);
}

void UpdateCfm::GainGate(int index, EKnobReturn status)
{
    sprintf(m_str, "%d", index);
//  SyncKnobCfm(CFM_GAIN_GATE, m_str, status);
}