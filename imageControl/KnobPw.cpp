#include "imageControl/KnobPw.h"
#include "imageControl/ImgPw.h"
#include "imageControl/Img2D.h"
#include "display/KnobMenu.h"
#include "display/gui_global.h"
#include "imageControl/Knob2D.h"
#include "display/HintArea.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "imageProc/ScanMode.h"
#include "ViewMain.h"
#include "imageProc/GlobalClassMan.h"

#if (defined(CT_460) || defined(CT_355))
KnobMenu::KnobItem KnobPwMenu[15] = {
    {_("SV Length"), "", MIN, PwChgSVLength, NULL},
    {_("Fast Angle"), "", MIN, PwChgAngleFast, PwPressAngleFast},
    {_("Scale"), "", MIN, PwChgScale, NULL},
    {_("Baseline"), "", MIN, PwChgBaseline, NULL},
    {_("Steer"), "", MIN, PwChgSteer, NULL},

    {_("Doppler Freq."), "", MIN, PwChgDopFreq, NULL},
    {_("Wall Filter"), "", MIN, PwChgWallFilter, NULL},
    {_("Spectrum Speed"), "", MIN, PwChgSpeed, NULL},
    {_("Dynamic Range"), "", MIN, PwChgDynamicRange, NULL},
    {_("Noise Threshold"), "", MIN, PwChgNoise, NULL},

    {_("Simult"), "", MIN, PwChgSimult, NULL},
    {_("HPRF"), "", MIN, PwChgHPRF, NULL},
    {_("Sound"), "Off", MIN, PwChgSoundStatus, NULL},
    //{_("PW Map"), "", MIN, PwChgMap, NULL},
    //{_("Log"), "", MIN, PwChgLog, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
};
#else
KnobMenu::KnobItem KnobPwMenu[15] = {
    {_("SV Length"), "", MIN, PwChgSVLength, NULL},
    {_("Angle"), "", MIN, PwChgAngle, PwPressAngleFast},
    {_("Scale"), "", MIN, PwChgScale, NULL},
    {_("Baseline"), "", MIN, PwChgBaseline, NULL},
    {_("Steer"), "", MIN, PwChgSteer, NULL},

    {_("Doppler Freq."), "", MIN, PwChgDopFreq, NULL},
    {_("Wall Filter"), "", MIN, PwChgWallFilter, NULL},
    {_("Spectrum Speed"), "", MIN, PwChgSpeed, NULL},
    {_("Dynamic Range"), "", MIN, PwChgDynamicRange, NULL},
    {_("Noise Threshold"), "", MIN, PwChgNoise, NULL},

    {_("Simult"), "", MIN, PwChgSimult, NULL},
    {_("HPRF"), "", MIN, PwChgHPRF, NULL},
    {_("Sound"), "Off", MIN, PwChgSoundStatus, NULL},
    //{_("PW Map"), "", MIN, PwChgMap, NULL},
    //{_("Log"), "", MIN, PwChgLog, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
};

#endif

void KnobPwCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobPwMenu, sizeof(KnobPwMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::PW);
}

//sync
void SyncKnobPw(EKnobPw type, const char* s, EKnobReturn status, bool draw)
{
    sprintf(KnobPwMenu[type].value, "%s", s);
    KnobPwMenu[type].status = status;
    if (draw)
        KnobMenu::GetInstance()->Update();
}

// callback
bool PwOperValid()
{
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
    if ((ModeStatus::IsPWImgMode()) || (ModeStatus::IsColorPDIMode()))
        return TRUE;
    else
        return FALSE;

#if 0
    if ((mode == ScanMode::PW_INIT) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI_INIT))
        return FALSE;
        // return TRUE;
    else
        return TRUE;
    // return FALSE;
#endif
}

bool CwOperValid()
{
    if (ModeStatus::IsCWImgMode() || ModeStatus::IsCWColorImgMode())
        return TRUE;
    else
        return FALSE;
}

EKnobReturn PwChgSVLength(EKnobOper oper)
{
    return ImgPw::GetInstance()->ChangeSVLength(oper);
}

EKnobReturn PwChgAngle(EKnobOper oper)
{
    return ImgPw::GetInstance()->ChangeCorrectAngle(oper);
}

EKnobReturn PwChgAngleFast(EKnobOper oper)
{
    return ImgPw::GetInstance()->ChangeCorrectAngleFast(oper);
}

EKnobReturn PwPressAngleFast(void)
{
    return ImgPw::GetInstance()->ChangeCorrectAngleFast(ROTATE);
}

EKnobReturn PwChgScale(EKnobOper oper)
{
    EKnobReturn ret;
    if (PwOperValid() || CwOperValid())
    {
        ModeStatus s;
        ScanMode::EScanMode mode = s.GetScanMode();

        if (mode == ScanMode::PW_SIMULT)
            ret = ImgPw::GetInstance()->ChangeScaleSimult2(oper);
        else if ((mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
            ret = ImgPw::GetInstance()->ChangeScaleSimult3(oper);
        else
            ret = ImgPw::GetInstance()->ChangeScale(oper);
#ifdef CT_355
    if(!ViewMain::GetInstance()->GetModeIsFlag())
        {
            if (ret != ERROR)
                ChangeTis();
            else
                HintArea::GetInstance()->UpdateHint(_("Invalid when HPRF is on."), 1);
        }
        else
        {
            ChangeTis();
        }
#else
        if (ret != ERROR)
            ChangeTis();
        else
            HintArea::GetInstance()->UpdateHint(_("Invalid when HPRF is on."), 1);
#endif
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        ret = ERROR;
    }

    return ret;
}

EKnobReturn PwChgBaseline(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeBaseline(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}

EKnobReturn PwChgSteer(EKnobOper oper)
{
    EKnobReturn ret  = ERROR;
    char probeType = GlobalClassMan::GetInstance()->GetProbeType();

    if ((probeType == 'L') || (probeType == 'l'))
    {
        ModeStatus s;
        int flag = 0;

        if (s.IsColorMode())
            flag = 1;
        else if (s.IsSpectrumMode())
            flag = 2;
        else if (s.IsSpectrumColorMode())
            flag = 3;

        if ((flag == 1) || (flag == 3))
            ImgCfm::GetInstance()->ClearBox();
        if ((flag == 2) || (flag == 3))
            ImgPw::GetInstance()->ClearSv();

        ret = ImgPw::GetInstance()->ChangeSteer(oper);
        ImgCfm::GetInstance()->ChangeSteer();

        if ((flag == 1) || (flag == 3))
            ImgCfm::GetInstance()->ReDrawBox();
        if ((flag == 2) || (flag == 3))
            ImgPw::GetInstance()->ReDrawSv();
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Steer]: Only valid for linear probe."), 1);
    }
    return ret;
}

EKnobReturn PwChgWallFilter(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeWallFilter(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}

EKnobReturn PwChgDopFreq(EKnobOper oper)
{
    if (PwOperValid())
    {
        EKnobReturn ret;
        ret = ImgPw::GetInstance()->ChangeDopplerFreq(oper);

        if (ModeStatus::IsSpectrumImgMode())
            ImgPw::GetInstance()->UpdateDopplerFreq();
        else if (ModeStatus::IsColorImgMode())
            ImgCfm::GetInstance()->ChangeDopplerFreq();
        else if (ModeStatus::IsSpectrumColorImgMode())
        {
            ImgPw::GetInstance()->UpdateDopplerFreq();
            ImgCfm::GetInstance()->ChangeDopplerFreq();
        }

        // change tis
        ChangeTis();

        return ret;
    }
    else if (ModeStatus::IsCWImgMode())
    {

         HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;

    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}

EKnobReturn PwChgSpeed(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeSpeed(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}

EKnobReturn PwChgNoise(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeNoiseThreshold(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}
EKnobReturn PwChgSimult(EKnobOper oper)
{
#ifdef CT_355
  int mode = ViewMain::GetInstance()->GetModeIsFlag();
    if (PwOperValid()&&(!mode))
#else
    if (PwOperValid())
#endif
    {
        ModeStatus ms;
        ScanMode::EScanMode mode = ms.GetScanMode();

        if (mode == ScanMode::PW_SIMULT)
        {
            if (oper == SUB)
                return ScanMode::GetInstance()->EnterPwSimult(FALSE);
        }
        else if (mode == ScanMode::PWCFM_SIMULT)
        {
            if (oper == SUB)
                return ScanMode::GetInstance()->EnterPwCfmSimult(FALSE);
        }
        else if (mode == ScanMode::PWPDI_SIMULT)
        {
            if (oper == SUB)
                return ScanMode::GetInstance()->EnterPwPdiSimult(FALSE);
        }
        else if (mode == ScanMode::PW)
        {
            if (ImgPw::GetInstance()->GetHPRFStatus())
            {
                HintArea::GetInstance()->UpdateHint(_("[Simult]: Can not be on when HPRF is on."), 2);
                return ERROR;
            }
            else
            {
                if (oper == ADD)
                    return ScanMode::GetInstance()->EnterPwSimult(TRUE);
            }
        }
        else if (mode == ScanMode::PWCFM)
        {
            if (ImgPw::GetInstance()->GetHPRFStatus())
            {
                HintArea::GetInstance()->UpdateHint(_("[Simult]: Can not be on when HPRF is on."), 2);
                return ERROR;
            }
            else
            {
                if (oper == ADD)
                    return ScanMode::GetInstance()->EnterPwCfmSimult(TRUE);
            }
        }
        else if (mode == ScanMode::PWPDI)
        {
            if (ImgPw::GetInstance()->GetHPRFStatus())
            {
                HintArea::GetInstance()->UpdateHint(_("[Simult]: Can not be on when HPRF is on."), 2);
                return ERROR;
            }
            else
            {
                if (oper == ADD)
                    return ScanMode::GetInstance()->EnterPwPdiSimult(TRUE);
            }
        }
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Simult]: Operation is invalid in current mode."), 1);
            return ERROR;
        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
    return OK;
}

EKnobReturn PwChgDynamicRange(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeDynamicRange(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[DynamicRange]: Operation is invalid."), 1);
        return ERROR;
    }
}

EKnobReturn PwChgMap(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeGrayMap(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}

EKnobReturn PwChgHPRF(EKnobOper oper)
{
#ifdef CT_355
   bool mode = ViewMain::GetInstance()->GetModeIsFlag();
    if ((PwOperValid() || CwOperValid())&&(!mode))
#else
    if (PwOperValid() || CwOperValid())
#endif
    {
        if (oper == ADD)
        {
            if (!ImgPw::GetInstance()->GetHPRFStatus())
                return ImgPw::GetInstance()->ChangeHPRF(TRUE);
        }
        else if (oper == SUB)
        {
            if (ImgPw::GetInstance()->GetHPRFStatus())
            return ImgPw::GetInstance()->ChangeHPRF(FALSE);
        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }

    return OK;
}

EKnobReturn PwChgSoundStatus(EKnobOper oper)
{
    return ImgPw::GetInstance()->ChangeSoundStatus(oper);
}

EKnobReturn PwChgLog(EKnobOper oper)
{
    return ImgPw::GetInstance()->ChangeLog(oper);
}

// for test
EKnobReturn PwChgSoundFilter(EKnobOper oper)
{
    if (PwOperValid() || CwOperValid())
    {
        return ImgPw::GetInstance()->ChangeSoundFilter(oper);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
}