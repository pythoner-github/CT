/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: Knob2D.cpp
 * @brief: knob in 2D mode
 *
 * version: V1.0
 * date: 2009-7-22
 * @author: zhanglei
 */

#include "Knob2D.h"
#include "Img2D.h"
#include "KnobMenu.h"
#include "gui_global.h"
#include "GlobalClassMan.h"
#include "HintArea.h"
#include "KeyFunc.h"
#include "../imageProc/Zoom.h"
#include "MenuBiopsy.h"

#if defined(K24C)
KnobMenu::KnobItem Knob2DMenu[15] = {
    {N_("Freq."), "", MIN, D2ChgFreq, NULL},
    {N_("Scan Angle"), "", MIN, D2ChgScanAngle, NULL},
    {N_("Space Compound"), "", MIN, D2ChgSpaceCompound, NULL},
    {N_("Freq. Compound"), "", MIN, D2ChgFreqCompound, NULL},
    {N_("【Chroma】"), "", MIN, D2ChgChroma, D2PressChroma},
    {N_("Dynamic Range"), "", MIN, D2ChgDynamicRange, NULL},
    {N_("THI"), "", MIN, D2ChgTHI, NULL},
    {N_("Steer"), "", MIN, D2ChgSteer, NULL},
    {N_("TP-View"), "", MIN, NULL, D2PressChgTpView},
    {N_("EFVI"), "", MIN, NULL, D2PressChgEFVI},
	{N_("TSI"), "", MIN, D2ChgTSI, NULL},
    {N_("Line Density"), "", MIN, D2ChgLineDensity, NULL},
    {N_("Edge"), "", MIN, D2ChgEdge, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
};
#elif defined(EMP_322) || defined(EMP_313)
KnobMenu::KnobItem Knob2DMenu[12] = {
   // {N_("Freq."), "", MIN, D2ChgFreq, NULL},
    {N_("Scan Angle"), "", MIN, D2ChgScanAngle, D2PressChgScanAngle},
    {N_("Space Compound"), "", MIN, D2ChgSpaceCompound, D2PressChgSpaceCompound},
    {N_("Freq. Compound"), "", MIN, D2ChgFreqCompound, D2PressChgFreqCompound},
    {N_("Dynamic Range"), "", MIN, D2ChgDynamicRange, D2PressChgDynamicRange},
    //{N_("THI"), "", MIN, D2ChgTHI, D2PressChgTHI},
    {N_("Line Density"), "", MIN, D2ChgLineDensity, D2PressChgLineDensity},
    {N_("Steer"), "", MIN, NULL, D2PressChgSteer},
    {N_("TP-View"), "", MIN, NULL, D2PressChgTpView},
    {N_("EFVI"), "", MIN, NULL, D2PressChgEFVI},
    {N_("Edge"), "", MIN, D2ChgEdge, D2PressChgEdge},
    {N_("MBP"),"", MIN, NULL, D2PressChgMBP},
    {N_("Sound Power"),"", MIN, NULL, D2PressChgSoundPower},
    {N_("Scan Line"), "", MIN, NULL, D2PressChgScanLine},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
   // {"", "", ERROR, NULL, NULL},
};
#elif (defined(EMP_460) || defined(EMP_355))
KnobMenu::KnobItem Knob2DMenu[20] = {
    {N_("Freq."), "", MIN, D2ChgFreq, D2PresssChgStatusTHI},
    {N_("Dynamic Range"), "", MIN, D2ChgDynamicRange, NULL},
    {N_("Focus"), "", MIN, D2ChgFocPos, D2PressFocus},
    {N_("Space Compound"), "", MIN, D2ChgSpaceCompound, NULL},
    {N_("【Chroma】"), "", MIN, D2ChgChroma, D2PressChroma},
    {N_("Freq. Compound"), "", MIN, D2ChgFreqCompound, NULL},
    {N_("Scan Angle"), "", MIN, D2ChgScanAngle, NULL},
    {N_("Steer"), "", MIN, D2ChgSteer, NULL},
    {N_("TP-View"), "", MIN, D2ChgTpView, NULL},
    {N_("TSI"), "", MIN, D2ChgTSI, NULL},
    {N_("Line Density"), "", MIN, D2ChgLineDensity, NULL},
    {N_("Edge"), "", MIN, D2ChgEdge, NULL},
    {N_("EFVI"), "", MIN, D2ChgEFVI, NULL},
    {N_("Scan Line"), "", MIN, D2ChgScanLine, NULL},
    {N_("Polarity"), "", MIN, D2ChgPolarity, NULL},
    {N_("Pwr"), "", MIN, D2ChgPwr, NULL},//增加pwr+/-
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
};
#elif defined(EMP_3410)
KnobMenu::KnobItem Knob2DMenu[15] = {
    {N_("Freq."), "", MIN, D2ChgFreq, D2PresssChgStatusTHI},
    {N_("Dynamic Range"), "", MIN, D2ChgDynamicRange, NULL},
    {N_("Focus"), "", MIN, D2ChgFocPos, D2PressFocus},
    {N_("Space Compound"), "", MIN, D2ChgSpaceCompound, NULL},
    {N_("【Chroma】"), "", MIN, D2ChgChroma, D2PressChroma},
    {N_("Scan Angle"), "", MIN, D2ChgScanAngle, NULL},
    {N_("Steer"), "", MIN, D2ChgSteer, NULL},
    {N_("TP-View"), "", MIN, D2ChgTpView, NULL},
    {N_("TSI"), "", MIN, D2ChgTSI, NULL},
    {N_("Line Density"), "", MIN, D2ChgLineDensity, NULL},
    {N_("Edge"), "", MIN, D2ChgEdge, NULL},
    {N_("EFVI"), "", MIN, D2ChgEFVI, NULL},
    {N_("Scan Line"), "", MIN, D2ChgScanLine, NULL},
    {N_("Polarity"), "", MIN, D2ChgPolarity, NULL},
    {N_("Freq. Compound"), "", MIN, D2ChgFreqCompound, NULL},
};
#else
KnobMenu::KnobItem Knob2DMenu[15] = {
    {N_("Freq."), "", MIN, D2ChgFreq, D2PresssChgStatusTHI},
    {N_("Dynamic Range"), "", MIN, D2ChgDynamicRange, NULL},
    {N_("Focus"), "", MIN, D2ChgFocPos, D2PressFocus},
    {N_("Space Compound"), "", MIN, D2ChgSpaceCompound, NULL},
    {N_("【Chroma】"), "", MIN, D2ChgChroma, D2PressChroma},
    {N_("Freq. Compound"), "", MIN, D2ChgFreqCompound, NULL},
    {N_("Scan Angle"), "", MIN, D2ChgScanAngle, NULL},
    {N_("Steer"), "", MIN, D2ChgSteer, NULL},
    {N_("TP-View"), "", MIN, D2ChgTpView, NULL},
    {N_("TSI"), "", MIN, D2ChgTSI, NULL},
    {N_("Line Density"), "", MIN, D2ChgLineDensity, NULL},
    {N_("Edge"), "", MIN, D2ChgEdge, NULL},
    {N_("EFVI"), "", MIN, D2ChgEFVI, NULL},
    {N_("Scan Line"), "", MIN, D2ChgScanLine, NULL},
    {N_("Polarity"), "", MIN, D2ChgPolarity, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
};	
#endif

void KnobD2Create()
{
    KnobMenu::GetInstance()->SetItem(Knob2DMenu, sizeof(Knob2DMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::D2);
}

///> knob menu need to be sync
void SyncKnobD2(EKnobD2 type, const char* s, EKnobReturn status, bool draw)
{
    sprintf(Knob2DMenu[type].value, "%s", s);
	Knob2DMenu[type].status = status;
    if (draw)
        KnobMenu::GetInstance()->Update();
}

void SyncKnobD2StatusTHI(EKnobD2 type, const char* s, bool draw)
{
    Knob2DMenu[type].name = (char *)s;
    if (draw)
        KnobMenu::GetInstance()->Update();
}

EKnobReturn D2ChgPwr(EKnobOper oper)
{
	EKnobReturn ret;
	Img2D::GetInstance()->ChangeSoundPower(oper);
	ChangeTis();
    //SyncKnobD2(D2_SOUND_POWER, m_str, status);	
	return ret;
}
///> callback function
EKnobReturn D2ChgFreq(EKnobOper oper)
{
	EKnobReturn ret;
    bool status = Img2D::GetInstance()->GetStatusTHI();
    //printf("status:%d\n", status);

    if(status)
        ret = Img2D::GetInstance()->ChangeD2HarmonicFreq(oper);
    else
        ret = Img2D::GetInstance()->ChangeFreq(oper);

    // change Tis
	ChangeTis();

	return ret;
}

EKnobReturn D2PresssChgStatusTHI(void)
{
    EKnobReturn ret;
    ret = ERROR;
    Img2D::GetInstance()->ChangeHarmonicStatus(ROTATE);

    return ret;
}

void ResetSampleLine(void)
{
    if (ModeStatus::IsMMode())
    {
        Img2D::GetInstance()->ResetMLine();
    }
    else if (ModeStatus::IsSpectrumMode())
    {
        ImgPw::GetInstance()->ResetSv();
    }
    else if (ModeStatus::IsColorMode())
    {
        ImgCfm::GetInstance()->ResetBox(TRUE);
    }
    else if (ModeStatus::IsSpectrumColorMode())
    {
        ImgPw::GetInstance()->ResetSv();
        ImgCfm::GetInstance()->ResetBox(TRUE);
    }
}
//ENP5800 按键切换 ，改变扫描角度（24 - 72）
EKnobReturn D2PressChgScanAngle(void)
{
    EKnobReturn ret = ERROR;
    EKnobOper oper = ROTATE;

	char type = GlobalClassMan::GetInstance()->GetProbeType();

	if ((type == 'L') || (type == 'l'))
	{
		ret = ERROR;
		HintArea::GetInstance()->UpdateHint(_("[Scan Angle]: Invalid for linear probe."), 1);
	}
	else
	{
     if (!Zoom::GetInstance()->GetLocalZoomStatus() && 
			MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::LOCAL_ZOOM)
        {
        ResetSampleLine();
		ret = Img2D::GetInstance()->ChangeScanAngle(oper);

		// change Tis
		ChangeTis();
        }
      else
        {
            HintArea::GetInstance()->UpdateHint(_("[Scan Angle]: Invalid when Zoom is on."), 1);
        }

	}

	return ret;
}
EKnobReturn D2ChgScanAngle(EKnobOper oper)
{
	EKnobReturn ret = ERROR;
	char type = GlobalClassMan::GetInstance()->GetProbeType();

	if ((type == 'L') || (type == 'l'))
	{
		ret = ERROR;
		HintArea::GetInstance()->UpdateHint(_("[Scan Angle]: Invalid for linear probe."), 1);
	}
	else
	{
        if (!Zoom::GetInstance()->GetLocalZoomStatus() && 
			MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::LOCAL_ZOOM)
        {
            ResetSampleLine();
            ret = Img2D::GetInstance()->ChangeScanAngle(oper);

            // change Tis
            ChangeTis();
        }
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Scan Angle]: Invalid when Zoom is on."), 1);
        }
    }

    return ret;
}

//EMP5800 按键相应切换，改变动态范围(0 -- 7)
EKnobReturn D2PressChgDynamicRange(void)
{

    EKnobReturn ret = ERROR;
    EKnobOper oper = ROTATE;
    ret = Img2D::GetInstance()->ChangeDynamicRange(oper);
    return ret;
}
EKnobReturn D2ChgDynamicRange(EKnobOper oper)
{
    return Img2D::GetInstance()->ChangeDynamicRange(oper);
}

//em5800 按键切换，改变线密度等级（高/低）
EKnobReturn D2PressChgLineDensity(void)
{
    EKnobReturn ret = ERROR;
    EKnobOper oper = ROTATE;
  //change line density after open compound image.
    Img2D *ptrImg = Img2D::GetInstance();
    if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn()) 
    {
        HintArea::GetInstance()->UpdateHint(_("Invalid when compound imaging is on."), 2);
        return ERROR;
    }
    // change line density
    ret = ImgPw::GetInstance()->ChangeLineDensity(oper);

    if (ModeStatus::IsColorMode() || ModeStatus::IsSpectrumColorMode())
        ImgCfm::GetInstance()->ReSendBox();
    
    return ret;

}
EKnobReturn D2ChgLineDensity(EKnobOper oper)
{
	//change line density afert open compound image .
    Img2D *ptrImg = Img2D::GetInstance();
    if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn()) 
    {
        HintArea::GetInstance()->UpdateHint(_("Invalid when compound imaging is on."), 2);
        return ERROR;
    }
    // change line density
    EKnobReturn ret;
    ret = ImgPw::GetInstance()->ChangeLineDensity(oper);

    if (ModeStatus::IsColorMode() || ModeStatus::IsSpectrumColorMode())
        ImgCfm::GetInstance()->ReSendBox();
    
    return ret;
}

//emp5800 按键切换，实现THI开/关
EKnobReturn D2PressChgTHI(void)
{
    EKnobReturn ret = ERROR;
    EKnobOper oper = ROTATE;
    Img2D *ptrImg = Img2D::GetInstance();
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    if ((type == 'C') || (type == 'N'))
    {
        if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn()) 
        {
            HintArea::GetInstance()->UpdateHint(_("Invalid when compound imaging is on."), 2);
            return ERROR;
        }
        else
        {
            ret = Img2D::GetInstance()->ChangeHarmonic(oper);
            return ret;
        }
    }
    else
    {
		HintArea::GetInstance()->UpdateHint(_("[Harmonic]: Only valid for low frequency convex probe."), 2);
        return ERROR;
    }

}
EKnobReturn D2ChgTHI(EKnobOper oper)
{
    Img2D *ptrImg = Img2D::GetInstance();
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    if ((type == 'C') || (type == 'N'))
    {
        if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn()) 
        {
            HintArea::GetInstance()->UpdateHint(_("Invalid when compound imaging is on."), 2);
            return ERROR;
        }
        else
        {
            return Img2D::GetInstance()->ChangeHarmonic(oper);
        }
    }
    else
    {
		HintArea::GetInstance()->UpdateHint(_("[Harmonic]: Only valid for low frequency convex probe."), 2);
        return ERROR;
    }
}
EKnobReturn D2ChgChroma(EKnobOper oper)
{
	return ImgProc2D::GetInstance()->ChangeChroma(oper);
}

EKnobReturn D2PressChroma(void)
{
	return ImgProc2D::GetInstance()->PressChroma();
}

EKnobReturn D2ChgTSI(EKnobOper oper)
{
    EKnobReturn ret = Img2D::GetInstance()->ChangeTSI(oper);
    ImgPw::GetInstance()->ChangeTSI();
    ImgCfm::GetInstance()->ChangeTSI();

    return ret;
}

EKnobReturn D2ChgAGC(EKnobOper oper)
{
    return Img2D::GetInstance()->ChangeAGC(oper);
}

EKnobReturn D2ChgMBP(EKnobOper oper)
{
    return Img2D::GetInstance()->ChangeMBP(oper);
}

//emp5800 按键切换功能，改变边缘增强值的大小（0 ---7）
EKnobReturn D2PressChgEdge(void)
{
    EKnobOper oper = ROTATE;
    EKnobReturn ret = ERROR;
    ret = Img2D::GetInstance()->ChangeEdgeEnhance(oper);
    return ret;
}

EKnobReturn D2ChgEdge(EKnobOper oper)
{
    return Img2D::GetInstance()->ChangeEdgeEnhance(oper);
}

EKnobReturn D2ChgSteer(EKnobOper oper)
{
    EKnobReturn ret  = ERROR;
#ifdef EMP_430
    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
    return ret;
#endif

    char probeType = GlobalClassMan::GetInstance()->GetProbeType();
    if (Img2D::GetInstance()->GetTpViewStatus())
        Img2D::GetInstance()->ExitTpView();
#if 0
    if (Img2D::GetInstance()->IsCompoundSpaceOn()) 
    {
        HintArea::GetInstance()->UpdateHint(_("Invalid when compound imaging is on."), 2);
        return ERROR;
    }
#endif
    if ((probeType == 'L') || (probeType == 'l'))
    {
        if (!Zoom::GetInstance()->GetLocalZoomStatus())
            ret = Img2D::GetInstance()->ChangeSteer(oper);
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Steer]: Invalid when localzoom is open!."), 1);

        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Steer]: Only valid for linear probe."), 1);
    }
    return ret;
}

/**
 * description: adjust steer to change 2D steerangle
 * para: NULL, 
 * return: ret
 */
EKnobReturn D2PressChgSteer(void)
{
    EKnobReturn ret  = ERROR;
    char probeType = GlobalClassMan::GetInstance()->GetProbeType();
    if (Img2D::GetInstance()->GetTpViewStatus())
        Img2D::GetInstance()->ExitTpView();

    if (Img2D::GetInstance()->IsCompoundSpaceOn()) 
    {
        HintArea::GetInstance()->UpdateHint(_("Invalid when compound imaging is on."), 2);
        return ERROR;
    }

    if ((probeType == 'L') || (probeType == 'l'))
    { 
        if (!Zoom::GetInstance()->GetLocalZoomStatus())
            ret = Img2D::GetInstance()->ChangeSteer(ROTATE);
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Steer]: Invalid when localzoom is open!."), 1);

        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Steer]: Only valid for linear probe."), 1);
    }
    return ret;
}

EKnobReturn D2PressFocus(void)
{
#if 0
    if (ModeStatus::IsUnFreezeMode())
    {
        ModeStatus ms;
        if ((ModeStatus::IsMImgMode() && (ms.GetFormatM() == FormatM::M_TOTAL))
                || (ModeStatus::IsPWImgMode() && (ms.GetFormatPw() == FormatPw::P_TOTAL)))
        {
            HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        }
        else
        {
            KeyFocus kf;
            kf.Execute();
        }
    }
    return OK;
#endif
#if 1
    EKnobReturn ret = ERROR;
    if (ModeStatus::IsUnFreezeMode())
    {
        ModeStatus ms;
        if ((ModeStatus::IsMImgMode() && (ms.GetFormatM() == FormatM::M_TOTAL))
                || (ModeStatus::IsPWImgMode() && (ms.GetFormatPw() == FormatPw::P_TOTAL)))
        {
            HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
            return ret;
        }
        else
        {
            Img2D* ptrImg = Img2D::GetInstance();
            if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus() || Zoom::GetInstance()->GetLocalZoomStatus())
            { 
                HintArea::GetInstance()->UpdateHint(_("[Focus]: Knob to move focus. When zoom, compound imaging, Tp-View or EFVI is on, it only has one focus."), 4);
                return ret;
            }
            else
                HintArea::GetInstance()->UpdateHint(_("[Focus]: Knob to move focus, Press to change focus sum."), 4);
            
            ret = Img2D::GetInstance()->ChangeFocSum(ROTATE);

            // change tis
            ChangeTis();
            return ret;
        }
    }
    return ret;
#endif

}

EKnobReturn ChgFocusNum(EKnobOper  oper)
{
    Img2D* ptrImg = Img2D::GetInstance();
    if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus() || Zoom::GetInstance()->GetLocalZoomStatus())
        return ERROR;

    EKnobReturn ret = Img2D::GetInstance()->ChangeFocSum(oper);

    // change tis
    ChangeTis();

    return ret;
}

EKnobReturn D2ChgFocPos(EKnobOper  oper)
{
#ifdef EMP_355
    if (SUB==oper)
		oper=ADD;
	else if (ADD==oper)
		oper=SUB;	
#endif
    return Img2D::GetInstance()->ChangeFocPos(oper);
}

void ParepareForCompound(void)
{
    // when low line density and MBP = 4, change line density to high
    Img2D* ptrImg = Img2D::GetInstance();
    if ( (ptrImg->GetMBP() >= 4) && (ptrImg->GetLDensity() == 0))
    {
        ptrImg->SetLineDensity(1);
    }

    // when focus is larger than 1, set focus to 1
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::FOCUS)
        MultiFuncUndo();
    if (ptrImg->GetFocSum() > 1)
    {
        ptrImg->SetFocSum(1);
    }
}

//EMP5800 直接按按键来切换
EKnobReturn D2PressChgSpaceCompound(void)
{
    EKnobReturn ret = ERROR;	
    //int index = 3;
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    Img2D* ptrImg2D = Img2D::GetInstance();
    bool harmonicOff = !ptrImg2D->GetHarmonicStatus();
    //bool valid = (ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode()) && ((type == 'L') || (type == 'l')) && (harmonicOff);
    bool valid = (ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode()) && (harmonicOff) && (!(type == 'P') || (type == 'p'));
    if (!valid)
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Only valid in 2D and M mode, no phase probe, harmonic off."), 2);
        return ERROR;
    }

    if (ptrImg2D->GetTpViewStatus() || ptrImg2D->GetEFVIStatus())
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Invalid when Tp-view or EFVI is open."), 1);
        return ERROR;
    }
 
    ParepareForCompound();
    ret = ptrImg2D->ChangeCompoundSpace();
    return ret;

}
EKnobReturn D2ChgSpaceCompound(EKnobOper oper)
{
    // valid mode
	char type = GlobalClassMan::GetInstance()->GetProbeType();
    //int index = 3;
    bool harmonicOff = !Img2D::GetInstance()->GetHarmonicStatus();
    Img2D* ptrImg = Img2D::GetInstance();
    bool valid = (ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode()) && (((type != 'P') && (type != 'p'))) ;
    //bool valid = (ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode()) && (harmonicOff) && (((type != 'P') && (type != 'p'))) ;
    if (!valid)
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Only valid in 2D and M mode, no phase probe, harmonic off."), 2);
        return ERROR;
    }

    if (ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus())
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Invalid when Tp-view or EFVI is open."), 1);
        return ERROR;
    }

    ParepareForCompound();
    return ptrImg->ChangeCompoundSpace(oper);
}

//emp5800 按键切换空间频率复合
EKnobReturn D2PressChgFreqCompound(void)
{
	char type = GlobalClassMan::GetInstance()->GetProbeType();
    EKnobReturn ret = ERROR;
    EKnobOper oper = ROTATE;
	Img2D* ptrImg2D = Img2D::GetInstance();
    bool harmonicOff = !ptrImg2D->GetHarmonicStatus();
    bool valid = (ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode()) && (harmonicOff);
    if (!valid)
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Only valid in 2D and M mode, harmonic off."), 2);
        return ERROR;
    }
   if (ptrImg2D->GetTpViewStatus() || ptrImg2D->GetEFVIStatus())
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Invalid when Tp-view or EFVI is open."), 1);
        return ERROR;
    }

    ParepareForCompound();
    ret = ptrImg2D->ChangeCompoundFreq(oper);
    return ret;
}
EKnobReturn D2ChgFreqCompound(EKnobOper oper)
{
#ifdef EMP_3410 
    if(!CManRegister::GetInstance()->IsAuthorize("eFci"))
        return ERROR;
#endif

    // valid mode
    bool harmonicOff = !Img2D::GetInstance()->GetHarmonicStatus();
    bool valid = (ModeStatus::IsD2ImgMode() || ModeStatus::IsMImgMode()) && (harmonicOff);
	Img2D* ptrImg2D = Img2D::GetInstance();

    if (!valid)
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Only valid in 2D and M mode, harmonic off."), 2);
        return ERROR;
    }
    if (ptrImg2D->GetTpViewStatus() || ptrImg2D->GetEFVIStatus())
    {
        HintArea::GetInstance()->UpdateHint(_("[Compound]: Invalid when Tp-view or EFVI is open."), 2);
        return ERROR;
    }


    ParepareForCompound();
    return Img2D::GetInstance()->ChangeCompoundFreq(oper);
}

// change mbp
EKnobReturn D2PressChgMBP(void)
{
    if (ModeStatus::IsEFOVMode() || (Img2D::GetInstance()->GetEFVIStatus()) || (Img2D::GetInstance()->GetTpViewStatus()) || (!ModeStatus::IsUnFreezeMode()))
   {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        return ERROR;
    }

    return Img2D::GetInstance()->ChangeMBP(ROTATE);
 }

//change sound power
EKnobReturn D2PressChgSoundPower(void)
{
    EKnobOper oper = ROTATE;
    EKnobReturn ret = ERROR;
    ret = Img2D::GetInstance()->ChangeSoundPower(oper);
    return ret;
}

//按键切换，改变扫描深度
EKnobReturn D2PressDepth(EKnobOper oper)
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

    // clear biopsy line
    g_menuBiopsy.ClearBiopsyLine();

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


#if 0
  if (mode == ScanMode::D2 )
    {
        Img2D::GetInstance()->ChangeDepth(oper); 
    }
    else
    {
        //prepare
       ImgCfm::GetInstance()->ChangeDepthBefore();

        //apply
        if (Img2D::GetInstance()->ChangeDepth(oper) != ERROR)
        {
            ImgCfm::GetInstance()->ChangeDepth();
            ImgPw::GetInstance()->ChangeDepth();
        }
    }
#endif
#if 1
    //prepare
    ImgCfm::GetInstance()->ChangeDepthBefore();

    //apply

	if (Img2D::GetInstance()->ChangeDepth(oper) != ERROR)
	{
	    ImgCfm::GetInstance()->ChangeDepth();
	    ImgPw::GetInstance()->ChangeDepth();
	}
#endif

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

    return OK;
}

void ParepareForTpView(void)
{
    // when low line density and MBP = 4, change line density to high
    Img2D* ptrImg = Img2D::GetInstance();
    if (ptrImg->GetLDensity() == 0)
    {
        ptrImg->SetLineDensity(1);
    }

    if ( (ptrImg->GetMBPIndex() != 1))
    {
        ptrImg->SetMBP(1);
    }

    // when focus is larger than 1, set focus to 1
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::FOCUS)
        MultiFuncUndo();
    if (ptrImg->GetFocSum() > 1)
    {
        ptrImg->SetFocSum(1);
    }    
}

EKnobReturn D2ChgTpView(EKnobOper oper)
{
	ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
	Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
	ProbeSocket::ProbePara para;
	ProbeMan::GetInstance()->GetCurProbe(para);
    char type = Img2D::GetInstance()->ReviseProbeType(para.type);
	if(type =='L' && ModeStatus::IsUnFreezeMode() && mode==ScanMode::D2 && format==Format2D::B)
    {
        Img2D* ptrImg2D = Img2D::GetInstance();
        
        if (ptrImg2D->GetEFVIStatus() || ptrImg2D->IsCompoundFreqOn() || ptrImg2D->IsCompoundSpaceOn() || Zoom::GetInstance()->GetLocalZoomStatus())
        {
            HintArea::GetInstance()->UpdateHint(_("[TP-View]: Invalid when EFVI , compound or localzoom is open."), 2);
            return ERROR;
        }
    

        ParepareForTpView();
        return ptrImg2D->ChangeTpView(oper);
    }
	else
	{
		HintArea::GetInstance()->UpdateHint(_("[TP-View]: Only valid when probe type is linear and unfreeze status in B mode."), 2);
		return ERROR;
	}
}

EKnobReturn D2PressChgTpView()
{
	ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
	Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
	ProbeSocket::ProbePara para;
	ProbeMan::GetInstance()->GetCurProbe(para);
    char type = Img2D::GetInstance()->ReviseProbeType(para.type);
	if(type == 'L' && ModeStatus::IsUnFreezeMode() && mode==ScanMode::D2 && format==Format2D::B)
    {
        Img2D* ptrImg2D = Img2D::GetInstance();
        
        if (ptrImg2D->GetEFVIStatus() || ptrImg2D->IsCompoundFreqOn() || ptrImg2D->IsCompoundSpaceOn() || Zoom::GetInstance()->GetLocalZoomStatus())
        {
            HintArea::GetInstance()->UpdateHint(_("[TP-View]: Invalid when EFVI , compound or localzoom is open."), 2);
            return ERROR;
        }
    

        ParepareForTpView();
        return ptrImg2D->ChangeTpView(ROTATE);
    }
	else
	{
		HintArea::GetInstance()->UpdateHint(_("[TP-View]: Only valid when probe type is linear and unfreeze status in B mode."), 2);
		return ERROR;
	}
}

EKnobReturn D2ChgEFVI(EKnobOper oper)
{
	ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
	Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
	ProbeSocket::ProbePara para;
	ProbeMan::GetInstance()->GetCurProbe(para);
    char type = Img2D::GetInstance()->ReviseProbeType(para.type);

	if((type != 'L') && ModeStatus::IsUnFreezeMode() && mode==ScanMode::D2 && format==Format2D::B)
    {
        Img2D* ptrImg2D = Img2D::GetInstance();
        if (ptrImg2D->GetTpViewStatus() || ptrImg2D->IsCompoundFreqOn() || ptrImg2D->IsCompoundSpaceOn())
        {
            HintArea::GetInstance()->UpdateHint(_("[EFVI]: Invalid when Tp-view or compound is open."), 2);
            return ERROR;
        }

        ParepareForTpView();
		return ptrImg2D->ChangeEFVI(oper);
    }
	else
	{
		HintArea::GetInstance()->UpdateHint(_("[EFVI]: Only valid when probe type is convex and unfreeze status in B mode."), 1);
		return ERROR;
	}
}


EKnobReturn D2PressChgEFVI()
{
	ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
	Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
	ProbeSocket::ProbePara para;
	ProbeMan::GetInstance()->GetCurProbe(para);
    char type = Img2D::GetInstance()->ReviseProbeType(para.type);

	if((type!='L') && ModeStatus::IsUnFreezeMode() && mode==ScanMode::D2 && format==Format2D::B)
    {
        Img2D* ptrImg2D = Img2D::GetInstance();
        if (ptrImg2D->GetTpViewStatus() || ptrImg2D->IsCompoundFreqOn() || ptrImg2D->IsCompoundSpaceOn() || g_menuBiopsy.GetDrawStatus())
        {
            HintArea::GetInstance()->UpdateHint(_("[EFVI]: Invalid when Tp-view , compound or biopsy line is open."), 2);
            return ERROR;
        }

        ParepareForTpView();
		return ptrImg2D->ChangeEFVI(ROTATE);
    }
	else
	{
		HintArea::GetInstance()->UpdateHint(_("[EFVI]: Only valid when probe type is convex and unfreeze status in B mode."), 1);
		return ERROR;
	}
}

EKnobReturn D2PressChgScanLine(void)
{
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    FormatCfm::EFormatCfm formatCfm = FormatCfm::GetInstance()->GetFormat();
    Format2D::EFormat2D format2D = Format2D::GetInstance()->GetFormat();
    if(type == 'p' || type == 'P')
    {
        if (!Zoom::GetInstance()->GetLocalZoomStatus()) 
            return Img2D::GetInstance()->ChangeScanLines(ROTATE);
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Scan Line]: Invalid when localzoom is open!."), 1);
            return ERROR;
        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Scan Line]: Only valid when probe type is phase."), 1);
        return ERROR;
    }
}

EKnobReturn D2ChgScanLine(EKnobOper oper)
{
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    FormatCfm::EFormatCfm formatCfm = FormatCfm::GetInstance()->GetFormat();
    Format2D::EFormat2D format2D = Format2D::GetInstance()->GetFormat();
    //if((type == 'p' || type == 'P') && (format2D == Format2D::B || formatCfm == FormatCfm::B))
    if(type == 'p' || type == 'P')
    {
        if (!Zoom::GetInstance()->GetLocalZoomStatus()) 
            return Img2D::GetInstance()->ChangeScanLines(oper);
        else
        {
            HintArea::GetInstance()->UpdateHint(_("[Scan Line]: Invalid when localzoom is open!."), 1);
            return ERROR;
        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Scan Line]: Only valid when probe type is phase."), 1);
        return ERROR;
    }
}

EKnobReturn D2ChgPolarity(EKnobOper oper)
{
    Img2D::GetInstance()->ChangePolarity(oper);
}
