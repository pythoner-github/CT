
#include "UpdatePw.h"
#include "KnobPw.h"
#include "Knob2D.h"
#include "KnobCfm.h"
#include "MenuPW.h"
#include "ViewIcon.h"
#include "ViewMain.h"
#include "../imageProc/ModeStatus.h"

const char * SpeedLevel[3] = 
{
    N_("Low"),
    N_("Mid"),
    N_("High")
};

const char * Toggle_Pw[2] = 
{
    N_("OFF"),
    N_("ON")
};

UpdatePw::UpdatePw()
{
	m_ptrImgPara = ImageAreaPara::GetInstance();
	m_ptrImgDraw = ImageAreaDraw::GetInstance();
	m_ptrMenu = MenuArea::GetInstance();
	m_timeMark = false;
}

void UpdatePw::SampleVolume(int line, int dotBegin, int dotEnd, vector<int> HPRFEmitPos)
{
	//if (ModeStatus::IsSpectrumMode() || ModeStatus::IsSpectrumColorMode())
   if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
    {
#ifdef EMP_355
       if(!ViewMain::GetInstance()->GetModeIsFlag())
        {
            m_ptrImgDraw->Update();
            m_ptrImgDraw->DrawSV(line, dotBegin, dotEnd, HPRFEmitPos, FALSE);
        }
#else
        m_ptrImgDraw->Update();
        m_ptrImgDraw->DrawSV(line, dotBegin, dotEnd, HPRFEmitPos, FALSE);
#endif
    }
    //vector<int>::iterator iter;
    //for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++)
    //	printf("**** HPRF pos = %d\n", *iter);

    if (ModeStatus::IsSpectrumColorMode()) {
	    m_ptrImgDraw->ClearCfmBox();
	    m_ptrImgDraw->ReDrawCfmBox();
	}
}

void UpdatePw::SampleVolumeClear()
{
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ClearSV();
}
void UpdatePw::SampleVolumeCW(int line, int dotPos)
{
#ifdef EMP_355
  if(ViewMain::GetInstance()->GetModeIsFlag())
#else
	if (ModeStatus::IsCWMode() || ModeStatus::IsCWColorMode())
#endif
    {
        m_ptrImgDraw->Update();
        m_ptrImgDraw->DrawSVCW(line, dotPos, FALSE);
    }
    //vector<int>::iterator iter;
    //for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++)
    //	printf("**** HPRF pos = %d\n", *iter);

    if (ModeStatus::IsSpectrumColorMode()) {
	    m_ptrImgDraw->ClearCfmBox();
	    m_ptrImgDraw->ReDrawCfmBox();
	}
}

void UpdatePw::SampleVolumeCWClear()
{
	m_ptrImgDraw->Update();
#ifdef EMP_355
    if(ViewMain::GetInstance()->GetModeIsFlag())
#endif
	m_ptrImgDraw->ClearSVCW();
}

void UpdatePw::SpectrumInvert(bool on)
{

}

void UpdatePw::GainPw(int gain)
{
	m_ptrImgPara->UpdatePwGain(gain);
}

void UpdatePw::GainCw(int gain)
{
    m_ptrImgPara->UpdateCwGain(gain);
}

void UpdatePw::SVLength(int len, EKnobReturn status)
{
	float length = (float)len/10;
	sprintf(m_str, "%.1fmm", length);
	SyncKnobPw(PW_SV_LEN, m_str, status);
	m_ptrImgPara->UpdatePwSV(length);
}

void  UpdatePw::SVPos(double depth)
{
	bool draw = TRUE;

	if (ModeStatus::IsSpectrumMode() || ModeStatus::IsSpectrumColorMode())
		draw = TRUE;
	else
        draw = FALSE;
	
	m_ptrImgPara->UpdatePwSVPos(depth, draw);
}

void UpdatePw::SetValueCorrectAngle(int angle)
{
	m_ptrImgDraw->SetAngleValue(angle);
}
void UpdatePw::CorrectAngle(int angle, EKnobReturn status)
{
	bool draw = TRUE;
    bool isCw = TRUE;

	// para
	if (ModeStatus::IsSpectrumMode() || ModeStatus::IsSpectrumColorMode())
		draw = TRUE;
	else
		draw = FALSE;

    if (ModeStatus::IsCWMode() || ModeStatus::IsCWColorMode())
        isCw = TRUE;
    else
        isCw = FALSE;

	m_ptrImgPara->UpdatePwAngle(angle, draw);
	m_ptrImgDraw->UpdateAngle(angle, isCw, draw);

	// knob
	sprintf(m_str, "%d°", angle);
	SyncKnobPw(PW_ANGLE, m_str, status);

	// image
	if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())
	{
		m_ptrImgDraw->Update();
		m_ptrImgDraw->DrawScaleYPw();
		if (m_timeMark)
		    m_ptrImgDraw->DrawGridPw();
	}
	if (ModeStatus::IsSpectrumColorMode()) {
	    m_ptrImgDraw->ClearCfmBox();
	    m_ptrImgDraw->ReDrawCfmBox();
	}
}

void UpdatePw::PRF(int data, EKnobReturn status)
{
	bool draw = TRUE;
	double prf = (float)data / 1000;

	// para
	if (ModeStatus::IsSpectrumMode() || ModeStatus::IsSpectrumColorMode())
		draw = TRUE;
	else
		draw = FALSE;
	m_ptrImgPara->UpdatePwPRF(prf, draw);

	// knob
	sprintf(m_str, "%.2fKHz", prf);
	SyncKnobPw(PW_SCALE, m_str, status);

	// image
	if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())
	{
		m_ptrImgDraw->Update();
		m_ptrImgDraw->DrawScaleXPw();
		m_ptrImgDraw->DrawScaleYPw();
		if (m_timeMark)
			m_ptrImgDraw->DrawGridPw();
	}
}
void UpdatePw::PRFMax(int data, EKnobReturn status)
{
	double prf = (float)data / 1000;
	// knob
	sprintf(m_str, "%.2fKHz", prf);
	SyncKnobPw(PW_SCALE, m_str, status);
}


void UpdatePw::PRFRange(int data, EKnobReturn status)
{
	// knob
	double prf = (float)data / 1000;
	sprintf(m_str, "%.2fKHz", prf);
	SyncKnobPw(PW_SCALE, m_str, status);
}

void UpdatePw::Baseline(int index, int maxIndex, EKnobReturn status)
{
    bool draw;

	sprintf(m_str, "%d", index);
	SyncKnobPw(PW_BASELINE, m_str, status);
	if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())
        draw = TRUE;
    else
        draw = FALSE;
    m_ptrImgDraw->SetPwBaseLine(index, maxIndex, draw);

	if (m_timeMark)
	    m_ptrImgDraw->DrawGridPw();

}

void UpdatePw::Steer(int angle, EKnobReturn status)
{
	sprintf(m_str, "%d°", angle);
    SyncKnobPw(PW_STEER, m_str, status);	
	SyncKnobCfm(CFM_STEER, m_str, status);	
	m_ptrImgDraw->SetSteerAngle(angle);
}
void UpdatePw::WallFilter(int data, EKnobReturn status)
{
	bool draw = TRUE;

	// para
	if (ModeStatus::IsSpectrumMode() || ModeStatus::IsSpectrumColorMode())
		draw = TRUE;
	else
		draw = FALSE;

	m_ptrImgPara->UpdatePwWF(data, draw);

	// knob
	sprintf(m_str, "%d", data);
	SyncKnobPw(PW_WALL_FILTER, m_str, status);
	ImageArea::GetInstance()->UpdateImgArea();
}

void UpdatePw::DopFreq(int freq, EKnobReturn status)
{
#ifdef EMP_430
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    const char* probeModel = para.model;
#if 0
    if (strcmp(probeModel, "35C60E") == 0)
    {
        if (freq == 48)
            freq = 54; //A60 convex probe doppler freq is 52
        else 
            freq = 66;
    }
#endif
    if (strcmp(probeModel, "65L40E") == 0)
    {
        if (freq == 100) //just for A60
            freq = 118;
        else
            freq = 144;
    }
#endif
	sprintf(m_str, "%.1fMHz", (float)freq/20);
    SyncKnobPw(PW_DOP_FREQ, m_str, status);	
	SyncKnobCfm(CFM_DOP_FREQ, m_str, status);	

	if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())
	{
		m_ptrImgDraw->Update();
		m_ptrImgDraw->DrawScaleYPw();
		if (m_timeMark)
			m_ptrImgDraw->DrawGridPw();
    }
    if(ModeStatus::IsCFMImgMode())
        m_ptrImgPara->UpdateCfmDopplerFreq(m_str);
    else if(ModeStatus::IsPDIImgMode())
        m_ptrImgPara->UpdatePdiDopplerFreq(m_str);
    else
        m_ptrImgPara->UpdateCfmDopplerFreq(m_str);
}

void UpdatePw::Speed(int index, EKnobReturn status)
{
	// knob
	sprintf(m_str, "%d", index); 
	SyncKnobPw(PW_SPEED, m_str, status);

	// image
    if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) {
        if (m_timeMark)
            m_ptrImgDraw->ClearGridPw();
        m_ptrImgDraw->Update();
        m_ptrImgDraw->DrawScaleXPw();
        if (m_timeMark)
            m_ptrImgDraw->DrawGridPw();
    }
}

void UpdatePw::SoundVolume(int index, EKnobReturn status)
{
	m_ptrImgPara->UpdateVolume(index);
}


void UpdatePw::Simult(bool on, EKnobReturn status)
{
    const char * Toggle[2] = 
	{
		N_("OFF"),
		N_("ON")
	};

	SyncKnobPw(PW_SIMULT, (char*)Toggle[on], status);	
	SyncKnobCfm(CFM_SIMULT, (char*)Toggle[on], status);	
}
void UpdatePw::DynamicRange(int index, EKnobReturn status)
{
	sprintf(m_str, "%d", index);
	SyncKnobPw(PW_DYN, m_str, status);
}
void UpdatePw::HPRF(bool on, EKnobReturn status)
{
	SyncKnobPw(PW_HPRF, (char*)Toggle_Pw[on], status);	
	m_ptrImgPara->UpdatePwHPRFStatus(on);
}

void UpdatePw::GrayMap(int index, EKnobReturn status)
{
	sprintf(m_str, "%d", index);
	SyncKnobPw(PW_MAP, m_str, status);
}

void UpdatePw::SoundStatus(int index, EKnobReturn status)
{
	SyncKnobPw(PW_SOUNDSTATUS, Toggle_Pw[index], status);	
	ViewIcon::GetInstance()->Sound((bool)index);
}

void UpdatePw::Noise(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	SyncKnobPw(PW_NOISE, m_str, status);
}

void UpdatePw::Log(int value, EKnobReturn status)
{
	PRINTF("update pw log = %d\n", value);
	sprintf(m_str, "%d", value);
	SyncKnobPw(PW_LOG, m_str, status);
}

void UpdatePw::AutoCalc(bool on)
{
    g_menuPW.UpdateAutoCalc(on);
}

void UpdatePw::AutoStatus(bool on)
{
    g_menuPW.UpdateAutoTrace(on);
    g_menuCW.UpdateAutoTrace(on);
}

void UpdatePw::EnterPwMode()
{
    // change to pw knob
    KnobPwCreate();

    EnterPwImg();
}

void UpdatePw::EnterPwImg()
{
    // draw sv
    if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
        m_ptrImgDraw->ReDrawSV();
    else
        m_ptrImgDraw->ReDrawSVCW();
    
    // draw para
    m_ptrImgPara->DrawPwPara();

#ifdef TRANSDUCER
    //redraw transducer
    m_ptrImgPara->ReDrawTransducer();
#endif
}

void UpdatePw::UpdatePwMode(void)
{
    UpdatePwMenu();
    UpdatePwImg();
}

void UpdatePw::UpdateCwMode(void)
{
    UpdateCwMenu();
    UpdatePwImg();
}

void UpdatePw::UpdateCwMenu(void)
{
	// change to pw menu
    m_ptrMenu->ShowCWMenu();
}

void UpdatePw::UpdatePwMenu(void)
{
	// change to pw menu
    m_ptrMenu->ShowPWMenu();
}

void UpdatePw::UpdatePwImg()
{
    // change to Pw image area
    m_ptrImgDraw->ClearTextArea();
    m_ptrImgDraw->Update();
    m_ptrImgDraw->DrawScanDirection();
    m_ptrImgDraw->DrawScaleY();
    m_ptrImgDraw->DrawGrayScale();
    if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
        m_ptrImgDraw->ReDrawSV();
    else
        m_ptrImgDraw->ReDrawSVCW();
    m_ptrImgDraw->DrawScaleYPw();
    m_ptrImgDraw->DrawScaleXPw();
    m_ptrImgPara->DrawGeneralPara();
    m_ptrImgPara->DrawPwPara();
    m_ptrImgDraw->ReDrawFocus();
    if (m_timeMark)
	m_ptrImgDraw->DrawGridPw();
#ifdef TRANSDUCER
    m_ptrImgPara->ReDrawTransducer();
#endif
}

void UpdatePw::EnterPwCfmFromPw()
{
    m_ptrMenu->ShowPWCFMMenu(true);
}

void UpdatePw::EnterCwCfmFromCfm()
{
    m_ptrMenu->ShowCWCFMMenu(false);
    m_ptrImgPara->DrawPwPara();
    
    // change to pw knob
    KnobPwCreate();
}
void UpdatePw::EnterPwCfmFromCfm()
{
    m_ptrMenu->ShowPWCFMMenu(false);
    m_ptrImgPara->DrawPwPara();
    
    // change to pw knob
    KnobPwCreate();
}

void UpdatePw::UpdateCwCfmMode(void)
{
    UpdateCwCfmMenu();
    UpdatePwCfmImg();
}

void UpdatePw::UpdateCwCfmMenu(void)
{
    m_ptrMenu->ShowCWCFMMenu(true);
}

void UpdatePw::UpdatePwCfmMode(void)
{
    UpdatePwCfmMenu();
    UpdatePwCfmImg();
}

void UpdatePw::UpdatePwCfmMenu(void)
{
    m_ptrMenu->ShowPWCFMMenu(true);
}

void UpdatePw::UpdatePwCfmImg(void)
{
    m_ptrImgDraw->ClearTextArea();
    m_ptrImgDraw->Update();

    m_ptrImgDraw->DrawScanDirection();
    m_ptrImgDraw->DrawScaleY();
    if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
        m_ptrImgDraw->ReDrawSV();
    else
        m_ptrImgDraw->ReDrawSVCW();
    m_ptrImgDraw->DrawGrayScale();
    m_ptrImgDraw->DrawScaleYPw();
    m_ptrImgDraw->DrawScaleXPw();
    m_ptrImgPara->DrawGeneralPara();
    m_ptrImgPara->DrawPwPara();
    m_ptrImgPara->DrawCfmPara();
    m_ptrImgDraw->DrawColorScale();    
    m_ptrImgDraw->ReDrawCfmBox();
    m_ptrImgDraw->ReDrawFocus();
    if (m_timeMark)
	m_ptrImgDraw->DrawGridPw();
#ifdef TRANSDUCER
    m_ptrImgPara->ReDrawTransducer();
#endif
}

void UpdatePw::EnterPwPdiFromPw()
{
}

void UpdatePw::EnterCwPdiFromPdi()
{
    m_ptrMenu->ShowCWCFMMenu(false);
    m_ptrImgPara->DrawPwPara();
    
    // change to pw knob
    KnobPwCreate();	
}
void UpdatePw::UpdateCwPdiMode()
{
    UpdateCwCfmMenu();
    UpdatePwCfmImg();
}
void UpdatePw::EnterPwPdiFromPdi()
{
    m_ptrMenu->ShowPWCFMMenu(false);
    m_ptrImgPara->DrawPwPara();
    
    // change to pw knob
    KnobPwCreate();	
}
void UpdatePw::UpdatePwPdiMode()
{
    UpdatePwCfmMenu();
    UpdatePwCfmImg();
}

void UpdatePw::ChangeFormatPw(int format)
{
    m_ptrImgDraw->ClearTextArea();
    m_ptrImgDraw->Update();
    m_ptrImgDraw->DrawGrayScale();
    m_ptrImgDraw->DrawScanDirection();
    m_ptrImgDraw->DrawScaleY();
    m_ptrImgDraw->DrawScaleYPw();
    m_ptrImgDraw->DrawScaleXPw();
    m_ptrImgPara->DrawGeneralPara();
    m_ptrImgPara->DrawPwPara();
    m_ptrImgDraw->ReDrawSV();
    m_ptrImgDraw->ReDrawFocus();
    if (m_timeMark)
	m_ptrImgDraw->DrawGridPw();
#ifdef TRANSDUCER    
    m_ptrImgPara->ReDrawTransducer();
#endif
}

// pw post process
void UpdatePw::TimeRes(int index, EKnobReturn status)
{
	sprintf(m_str, "%d", index);
	g_menuPW.UpdateTimeRes(m_str, status);
}

void UpdatePw::TimeMarkPw(bool on)
{
    m_timeMark = on;
    g_menuPW.UpdateTimeMark(on);
    if (on)
	m_ptrImgDraw->DrawGridPw();
    else
	m_ptrImgDraw->ClearGridPw();
}

// for test
void UpdatePw::SoundFilter(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	//SyncKnobPw(PW_SOUNDFILTER, m_str, status);
}
