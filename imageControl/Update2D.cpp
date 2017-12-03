#include <stdlib.h>
#include "Update2D.h"
#include "gui_global.h"
#include "Knob2D.h"
#include "KnobM.h"
#include "KnobPw.h"
#include "KnobCfm.h"
#include "Menu2D.h"
#include "MenuM.h"
#include "MenuBiopsy.h"
#include "CusSpin.h"
#include "TopArea.h"
#include "GlobalClassMan.h"
#include "KeyFunc.h"
#include "Zoom.h"
#include "../patient/PatientInfo.h"
#include "../display/ImageArea.h"
#include "KnobEFOV.h"
#include "ViewMain.h"
const char * Toggle[2] = 
{
	N_("OFF"),
	N_("ON")
};

const char * Toggle_THI[2] = 
{
#if(defined(EMP_460) || defined(EMP_355))
	N_("Freq"),
	N_("Freq")
#else
	N_("THI OFF"),
	N_("THI ON")
#endif
};

const char * Toggle_EFOVReplay[2] = 
{
	N_("OFF"),
	N_("ON")
};

const char * Level_TSI[4] = {
	N_("Routine"),
	N_("Muscle"),
	N_("Liquid"),
	N_("Fat"),
};

const char * Level_LD[2] = {
	N_("LOW"),
	N_("HIGH")
};

const char * TpName[4] = 
{
    N_("Normal"),
    N_("Steer Left"),
    N_("Steer Right"),
    N_("Tp-View"),
};

Update2D::Update2D()
{
	m_ptrImgPara = ImageAreaPara::GetInstance();
	m_ptrImgDraw = ImageAreaDraw::GetInstance();
	m_ptrMenu = MenuArea::GetInstance();
	m_timeMark = false;
	m_tgcTimeout = 0;
}

void Update2D::ProbeType(const char* type)
{
	TopArea::GetInstance()->UpdateProbeType(type);
}

#ifdef VET
void Update2D::ExamItemByProbe(const char *type)
{
	TopArea::GetInstance()->UpdateCheckPart(type);
	const string examtype =type;
	g_patientInfo.SetExamType(examtype);
}
#endif


//update user defined item 
void Update2D::UserDefineExamItem(const char *useritem)
{
	TopArea::GetInstance()->UpdateCheckPart(useritem);
	g_patientInfo.SetExamType(useritem);
}

void Update2D::ExamItem(int index)
{
	TopArea::GetInstance()->UpdateCheckPart(_(examType[index]));
	const string examtype = _(examType[index]);
	g_patientInfo.SetExamType(examtype);
}

//> knob
void Update2D::Freq(int freq, EKnobReturn status)
{
#if(defined(EMP_460) || defined(EMP_355))
	sprintf(m_str, "%.1fMHz", (float)freq/20);
#else
   sprintf(m_str, "F:%.1fMHz", (float)freq/20);
#endif
   //sprintf(m_str, "%.1fMHz", (float)freq/20);

#if (defined (EMP_322) || defined(EMP_313))
    TopArea::GetInstance()->UpdateFreq(m_str);
#else
    SyncKnobD2(D2_FREQ, m_str, status);	
    SyncKnobM(M_FREQ, m_str, status);	
#endif
    m_ptrImgPara->UpdateFreq(m_str);
}

void Update2D::ScanAngle(int angleIndex, EKnobReturn status)
{
	sprintf(m_str, "%d°", angleIndex);
	SyncKnobD2(D2_ANGLE, m_str, status);
}

void Update2D::ScanLines(int scanLinesIndex, EKnobReturn status)
{
	sprintf(m_str, "%d", scanLinesIndex);
	SyncKnobD2(D2_SCAN_LINES, m_str, status);
	SyncKnobCfm(CFM_SCAN_LINES, m_str, status);
}

void Update2D::DynamicRange(int range, EKnobReturn status)
{
	sprintf(m_str, "%ddB", range);
	SyncKnobD2(D2_DYN, m_str, status);
	m_ptrImgPara->Update2DDynamicRange(range);
}

void Update2D::LineDensity(const char* density, EKnobReturn status)
{
	SyncKnobD2(D2_LINE_DENSITY, density, status);
	m_ptrImgPara->Update2DLineDensity(density);
}

void  Update2D::Harmonic(bool on, EKnobReturn status) 
{
	SyncKnobD2(D2_THI, Toggle[on], status);	
	m_ptrImgPara->Update2DTHI(on);
}

void  Update2D::HarmonicStatus(bool on, EKnobReturn status) 
{
#ifndef EMP_322
#ifndef EMP_313
    SyncKnobD2StatusTHI(D2_FREQ, Toggle_THI[on]);
#endif
#endif
}

void  Update2D::HarmonicFreq(int harFreq, EKnobReturn status) 
{
#if(defined(EMP_460))
    sprintf(m_str, "H:%.1fMHz", (float)harFreq/20.0*2);
#elif (defined(EMP_355)||defined(EMP_340))
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
   int freq_tmp;
    if((strcmp(para.model, "70L40J") == 0) || ((strcmp(para.model, "75L40K") == 0)) || ((strcmp(para.model, "75L40J") ==0)))
    {
        if(harFreq == 160)
        {
            freq_tmp = 90;
        }
        else if(harFreq == 200)
        {
            freq_tmp  = 100;
        }
        else if(harFreq == 240)
        {
            freq_tmp = 120;
        }
        else 
            freq_tmp =harFreq;
    }
    else if((strcmp(para.model, "65C10L") == 0)||(strcmp(para.model, "65C10K") == 0) ||(strcmp(para.model, "65C10H") == 0))
    {
        if(harFreq == 120)
        {
            freq_tmp = 80;
        }
        else if(harFreq == 160)
        {
            freq_tmp  = 90;
        }
        else if(harFreq == 200)
        {
            freq_tmp = 100;
        }
        else 
            freq_tmp =harFreq;

    }
    else if((strcmp(para.model, "10L25K")) == 0)
    {
        if(harFreq == 180)
        {
            freq_tmp = 100;
        }
        else if(harFreq == 200)
        {
            freq_tmp  = 110;
        }
        else if(harFreq == 240)
        {
            freq_tmp = 120;
        }
        else 
            freq_tmp =harFreq;
    }
    else
        freq_tmp =harFreq;
    sprintf(m_str, "H:%.1fMHz", (float)freq_tmp/20.0*2);
#else
    sprintf(m_str, "FH:%.1fMHz", (float)harFreq/20.0*2);
#endif
#ifndef EMP_322
#ifndef EMP_313
    SyncKnobD2(D2_FREQ, m_str, status);	
    SyncKnobM(M_FREQ, m_str, status);	
#endif
#endif
    m_ptrImgPara->UpdateFreq(m_str);
}

void Update2D::Tsi(const char* data, EKnobReturn status) 
{
#if (defined (EMP_322) || defined(EMP_313))
	m_ptrImgPara->Update2DTSI(data);
#else
	SyncKnobD2(D2_TSI, data, status);	
	m_ptrImgPara->Update2DTSI(data);
#endif
}

void Update2D::Agc(int index, EKnobReturn status) 
{
	sprintf(m_str, "%d", index);
	//SyncKnobD2(D2_AGC, m_str, status);
}

void Update2D::MBP(int mbp, EKnobReturn status) 
{
    if(Img2D::GetInstance()->GetLDensity() == 0)
        mbp *= 2;
#if (defined (EMP_322) ||defined(EMP_313))
    sprintf(m_str, "%d", mbp);
    SyncKnobD2(D2_MBP,m_str,status);
    SyncKnobM(M_MBP, m_str, status);
#endif
    m_ptrImgPara->Update2DMBP(mbp);
}

void  Update2D::EdgeEnhance(int edgeIndex, EKnobReturn status) 
{
	sprintf(m_str, "%d", edgeIndex);
	SyncKnobD2(D2_EDGE, m_str, status);
}

void Update2D::Steer(int angle, EKnobReturn status) 
{
	sprintf(m_str, "%d°", angle);
	SyncKnobD2(D2_STEER, m_str, status);	
    //m_ptrImgDraw->Set2DSteerAngle(angle); 偏转的情况下不能进入localzoom
}

void Update2D::Depth(int depth)
{
	//m_ptrImgPara->DrawGeneralPara();
	//update system head
    //TopArea::GetInstance()->UpdateDepth(depth);
    
    m_ptrImgPara->UpdateDepth(depth);
    m_ptrImgDraw->Update();
    m_ptrImgDraw->DrawScaleY();
    m_ptrImgDraw->DrawScaleYM();
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();
    if (mode == ScanMode::ANATOMIC_M)
        m_ptrImgDraw->DrawScaleXM();
    if (m_timeMark)
        m_ptrImgDraw->DrawGridM();
	m_ptrImgDraw->DrawScanDirection();
}

void  Update2D::FocInfo(int focSum, int focPos[])
{
#ifndef EMP_322
#ifndef EMP_313
	EKnobReturn status = OK;
	if (focSum == 1) 
		status = MIN;
	else if (focSum == Img2D::MAX_FOCUS)
		status = MAX;
	else 
		status = OK;

	sprintf(m_str, "%d", focSum);
	SyncKnobD2(D2_FOCUS, m_str, status);	
#endif
#endif
	m_ptrImgDraw->ClearFocus();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawFocus(focSum, focPos);
}

static gboolean ClearTgcCurve(gpointer data)
{
	ImageAreaDraw *tmp;
	tmp = (ImageAreaDraw*)data;
	tmp->ClearTGCLine();
	return FALSE;
}

void  Update2D::TgcCurve(unsigned char* tgc, int tgcSize)
{
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawTGCLine(tgc, tgcSize);
	if (m_tgcTimeout)
		gtk_timeout_remove(m_tgcTimeout);
	m_tgcTimeout = g_timeout_add(1000, ClearTgcCurve, m_ptrImgDraw);
}

#ifdef TRANSDUCER
void Update2D::Transducer(int transducer)
{
	m_ptrImgPara->UpdateTransducer(transducer); 
}
#endif

void  Update2D::Gain2D(int total) 
{
	m_ptrImgPara->Update2DGain(total); 
}

void  Update2D::GainM(int total) 
{
	m_ptrImgPara->UpdateMGain(total); 
}

void Update2D::SoundPower(int power, EKnobReturn status)
{
	PRINTF("power = %d\n", power);
#if (defined (EMP_322) || defined(EMP_313))
    sprintf(m_str, "%d", power);
    SyncKnobD2(D2_SOUND_POWER,m_str,status);
   	SyncKnobM(M_SOUND_POWER, m_str, status);
	PRINTF("power = %d\n", power);
    m_ptrImgPara->UpdateGenPwr(power);	
#elif (defined(EMP_460) || defined(EMP_355))
    sprintf(m_str, "%d", power);
    SyncKnobD2(D2_SOUND_POWER,m_str,status);
   //	SyncKnobM(M_SOUND_POWER, m_str, status);
	PRINTF("power = %d\n", power);
    m_ptrImgPara->UpdateGenPwr(power);
#else
    m_ptrImgPara->UpdateGenPwr(power);
#endif
}

void  Update2D::MLine(int line) 
{
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawBMLine(line);
}

void  Update2D::ClearMLine() 
{
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ClearBMLine();
}

void  Update2D::ReDrawMLine() 
{
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawBMLine();
}

void  Update2D::MSpeed(int speedIndex, EKnobReturn status) 
{
	// knob
	sprintf(m_str, "%d", speedIndex);
	SyncKnobM(M_SPEED, m_str, status);

	// scale 
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawScaleXM();
	if (m_timeMark)
		m_ptrImgDraw->DrawGridM();
}

void  Update2D::ProbePara(int type, int lines, int width, int r) 
{
	m_ptrImgDraw->GetProbePara(type, lines, width, r);
}

void  Update2D::Enter2DMode(void) 
{
	Enter2DMenu();
	Enter2DKnob();
	Enter2DImg();
} 

void Update2D::Enter2DMenu(void)
{
	// change to 2D menu
	m_ptrMenu->Show2DMenu();
}

void Update2D::Enter2DKnob(void)
{
	// change to 2D knob
	KnobD2Create();
}

// update参数为true时，使用在自动优化时的清屏，解决进入模式时某些非实时情况下的标志绘制
// 默认参数为false, 切换模式时使用
void Update2D::Enter2DImg(bool update)
{
	// change to 2D image area
	m_ptrImgDraw->ClearTextArea(update);
	m_ptrImgDraw->Update();
	if (update) {
		m_ptrImgDraw->SetSDFreeze();
		m_ptrImgDraw->SetScaleYFreeze();
	}
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawScanDirection();
	m_ptrImgDraw->DrawGrayScale();
	m_ptrImgDraw->ReDrawFocus();

	// draw centerline
	KeyCenterLine key;
	key.Execute();

	// change to 2D image area para
	m_ptrImgPara->DrawGeneralPara();
	m_ptrImgPara->Draw2DPara();

#ifdef TRANSDUCER
    // display transducer
    m_ptrImgPara->DrawTransducer();	

#endif

    g_menuBiopsy.UpdateBiopsyLine();

}

void Update2D::UpdateEFOVZoomScale(void)
{
	m_ptrImgDraw->DrawEFOVScaleX();
	m_ptrImgDraw->DrawEFOVScaleY();
}

void Update2D::EnterEFOVPrepare(void)
{
	// Menu
	m_ptrMenu->ShowEFOVPrepare();
	// Knob
	KnobEFOVPrepareCreate();
	// ImageArea
	m_ptrImgDraw->ClearEFOVZoomBox();
	m_ptrImgDraw->ClearEFOVViewBox();
	m_ptrImgDraw->ClearTextArea(true);
	m_ptrImgDraw->DrawEFOV2DScaleY(true);
	m_ptrImgPara->DrawGeneralPara();
#ifdef TRANSDUCER
    //para area
    //m_ptrImgPara->ReDrawTransducer();
#endif
}

void Update2D::EnterEFOVCapture(void)
{
	// Menu
	m_ptrMenu->ShowEFOVCapture();
	// Knob
	KnobEFOVCaptureCreate();
	// ImageArea
	m_ptrImgDraw->ClearEFOVZoomBox();
	m_ptrImgDraw->ClearEFOVViewBox();
	m_ptrImgDraw->ClearTextArea(true);
	m_ptrImgPara->DrawGeneralPara();
#ifdef TRANSDUCER 
    //para area
    //m_ptrImgPara->ReDrawTransducer();
#endif
}

void Update2D::EnterEFOVView(void)
{
	// Menu
	m_ptrMenu->ShowEFOVView();
	// Knob
	KnobEFOVViewCreate();
	// ImageArea
	m_ptrImgDraw->ClearEFOVZoomMiniatureBox();
	m_ptrImgDraw->ClearEFOVZoomBox();
	m_ptrImgDraw->ClearEFOVViewBox();
	m_ptrImgDraw->ClearTextArea(true);
	m_ptrImgDraw->DrawEFOVScaleY();
	m_ptrImgDraw->DrawEFOVScaleX();
#ifdef TRANSDUCER   
    //para area
    //m_ptrImgPara->ReDrawTransducer();
#endif
}

void Update2D::EnterEFOVReview(void)
{
	// Menu
	m_ptrMenu->ShowEFOVReview();
	// Knob
	KnobEFOVReviewCreate();
	// ImageArea
	m_ptrImgDraw->ClearEFOVZoomMiniatureBox();
	m_ptrImgDraw->ClearEFOVZoomBox();
	m_ptrImgDraw->ClearEFOVViewBox();
	m_ptrImgDraw->ClearTextArea(true);
	m_ptrImgDraw->DrawEFOV2DScaleY(false);
}

void Update2D::EFOVZoom(float scale, EKnobReturn status) 
{
	sprintf(m_str, "%.1f", scale);
	SyncKnobEFOV(EFOV_ZOOM, m_str, status);	
}

void Update2D::EFOVRotate(int angle, EKnobReturn status) 
{
	sprintf(m_str, "%d°", angle);
	SyncKnobEFOV(EFOV_ROTATE, m_str, status);	
}

void Update2D::EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y)
{
	m_ptrImgDraw->DrawEFOVZoomMiniatureBox(start_x, start_y, end_x, end_y);
}

void Update2D::ClearEFOVZoomMiniatureBox(void)
{
	m_ptrImgDraw->ClearEFOVZoomMiniatureBox();
}


void Update2D::EFOVZoomBox(int start_x, int start_y, int end_x, int end_y)
{
	m_ptrImgDraw->DrawEFOVZoomBox(start_x, start_y, end_x, end_y);
}

void Update2D::ClearEFOVZoomBox(void)
{
	m_ptrImgDraw->ClearEFOVZoomBox();
}

void Update2D::EFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right)
{
	m_ptrImgDraw->DrawEFOVViewBox(up_left, down_left, up_right, down_right);
}

void Update2D::ClearEFOVViewBox(void)
{
	m_ptrImgDraw->ClearEFOVViewBox();
}

void Update2D::EFOVReplay(bool on, EKnobReturn status) 
{
	SyncKnobEFOVReview(EFOV_REPLAY, Toggle_EFOVReplay[on], status);	
}

void Update2D::ClearEFOVImg()
{
	ImageArea::GetInstance()->ClearEFOVImg();
}

void  Update2D::EnterMMode() 
{
	EnterMImg();
}

void Update2D::EnterMImg()
{
	m_ptrImgDraw->ReDrawBMLine();
}
void  Update2D::UpdateM() 
{
	UpdateMKnob();
	UpdateMMenu();
	UpdateMImg();
}

void Update2D::UpdateMMenu(void)
{
	// change to M menu
	m_ptrMenu->ShowMMenu();
}
void Update2D::UpdateMKnob(void)
{
	// change to M knob
	KnobMCreate();
}
void Update2D::UpdateMImg(void)
{
	// change to M image area
	m_ptrImgDraw->ClearTextArea();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawBMLine();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawScaleYM();
	m_ptrImgDraw->DrawScaleXM();
	m_ptrImgDraw->DrawGrayScale();
	m_ptrImgPara->DrawGeneralPara();
	m_ptrImgPara->DrawMPara();
	m_ptrImgDraw->ReDrawFocus();
	if (m_timeMark)
		m_ptrImgDraw->DrawGridM();
	m_ptrImgDraw->DrawScanDirection();

#ifdef TRANSDUCER  
     //para area
    m_ptrImgPara->ReDrawTransducer();
#endif


}

void Update2D::EnterAnatomicM()
{
	//    m_ptrImgDraw->ClearBMLine();
	m_ptrImgDraw->ClearTextArea();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawAnatomicalM();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawScaleYM();
	m_ptrImgDraw->DrawScaleXM();
	m_ptrImgDraw->DrawGrayScale();
	m_ptrImgPara->DrawGeneralPara();
	m_ptrImgPara->DrawMPara();
	m_ptrImgDraw->ReDrawFocus();
	if (m_timeMark)
		m_ptrImgDraw->DrawGridM();
	m_ptrImgDraw->DrawScanDirection();
}

void Update2D::ChangeFormat2D(int format) 
{
	m_ptrImgDraw->ClearTextArea();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawGrayScale();
	m_ptrImgDraw->ReDrawFocus();
	m_ptrImgDraw->DrawScanDirection();

	ImageArea::GetInstance()->RedrawBodyMarkWhenModeChanged();

	// change to 2D image area para
	m_ptrImgPara->DrawGeneralPara();
	m_ptrImgPara->Draw2DPara();
} 

void Update2D::ChangeCurrentImgBB(int current) 
{
	m_ptrImgDraw->ClearFocus();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawFocus();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->SetSDFreeze();
	m_ptrImgDraw->DrawScanDirection();

	ImageArea::GetInstance()->ChangeCurrentBDMK(current);
} 

void  Update2D::ChangeCurrentImg4B(int current) 
{
	m_ptrImgDraw->ClearFocus();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawFocus();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->SetSDFreeze();
	m_ptrImgDraw->DrawScanDirection();

	ImageArea::GetInstance()->ChangeCurrentBDMK(current);
} 

void  Update2D::ChangeFormatM(int format) 
{
	m_ptrImgDraw->ClearTextArea();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawBMLine();
	m_ptrImgDraw->ReDrawFocus();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawScanDirection();
	m_ptrImgDraw->DrawScaleYM();
	m_ptrImgDraw->DrawScaleXM();
	m_ptrImgDraw->DrawGrayScale();
	m_ptrImgPara->DrawGeneralPara();
	m_ptrImgPara->DrawMPara();
	m_ptrImgDraw->ReDrawFocus();
	if (m_timeMark)
		m_ptrImgDraw->DrawGridM();
#ifdef TRANSDUCER  
 //para area
    m_ptrImgPara->ReDrawTransducer();
#endif
} 

void Update2D::LeftRight(bool on)
{
	ModeStatus s;
	ScanMode::EScanMode mode = s.GetScanMode();
#ifdef EMP_355
    bool mode_bak = ViewMain::GetInstance()->GetModeIsFlag();
#endif
	if (mode == ScanMode::D2) {
		if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
			m_ptrImgDraw->ClearLocalZoom();
		}
        m_ptrImgDraw->ClearFocus();
        m_ptrImgDraw->Update();
        m_ptrImgDraw->ReDrawFocus();
        m_ptrImgDraw->DrawScaleY();
        m_ptrImgDraw->DrawScanDirection();
        g_menuBiopsy.UpdateBiopsyLine();

        if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
            m_ptrImgDraw->ReDrawLocalZoom();
        }
    }
    else if ((mode == ScanMode::M) || (mode == ScanMode::M_INIT) || (mode == ScanMode::ANATOMIC_M)) {
		if (mode != ScanMode::ANATOMIC_M)
			m_ptrImgDraw->ClearBMLine();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScanDirection();
		if (mode != ScanMode::ANATOMIC_M)
			m_ptrImgDraw->ReDrawBMLine();
	}
	else if ((mode == ScanMode::PW) || (mode == ScanMode::PW_INIT) || (mode == ScanMode::PW_SIMULT)) {
#ifdef EMP_355
 if(mode_bak)
            m_ptrImgDraw->ClearSVCW();
        else
            m_ptrImgDraw->ClearSV();
#else
		m_ptrImgDraw->ClearSV();
#endif
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
#ifdef EMP_355
  if(mode_bak)
            m_ptrImgDraw->ReDrawSVCW();
        else
            m_ptrImgDraw->ReDrawSV();
#else
		m_ptrImgDraw->ReDrawSV();
#endif
		m_ptrImgDraw->DrawScanDirection();
	}	
	else if ((mode == ScanMode::CW_INIT) || (mode == ScanMode::CW)){
		m_ptrImgDraw->ClearSVCW();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->ReDrawSVCW();
		m_ptrImgDraw->DrawScanDirection();
	}
	else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI) || (mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D)){	
		m_ptrImgDraw->ClearCfmBox();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->ReDrawCfmBox();
		m_ptrImgDraw->DrawScanDirection();
        g_menuBiopsy.UpdateBiopsyLine();//2016.11.05--hy
	}
	else if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_INIT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)) {
		m_ptrImgDraw->ClearCfmBox();
#ifdef EMP_355
    if(mode_bak)
            m_ptrImgDraw->ClearSVCW();
        else
            m_ptrImgDraw->ClearSV();
#else
		m_ptrImgDraw->ClearSV();
#endif
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->ReDrawCfmBox();
#ifdef EMP_355
   if(mode_bak)
            m_ptrImgDraw->ReDrawSVCW();
        else
            m_ptrImgDraw->ReDrawSV();
#else
		m_ptrImgDraw->ReDrawSV();
#endif
		m_ptrImgDraw->DrawScanDirection();
	}
    else if(ModeStatus::IsCWColorMode()) {
		m_ptrImgDraw->ClearCfmBox();
		m_ptrImgDraw->ClearSVCW();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScanDirection();
		m_ptrImgDraw->ReDrawCfmBox();
		m_ptrImgDraw->ReDrawSVCW();
	}
}

void Update2D::UpDown(bool on)
{
	//	AbsUpdateCfm* ptrUpdateCfm = GlobalClassMan::GetInstance()->GetUpdateCfm();
	ModeStatus s;
	ScanMode::EScanMode mode = s.GetScanMode();
#ifdef EMP_355
    bool mode_bak = ViewMain::GetInstance()->GetModeIsFlag();
#endif
	if (mode == ScanMode::D2) {
		if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
			m_ptrImgDraw->ClearLocalZoom();
		}
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScanDirection();
        g_menuBiopsy.UpdateBiopsyLine();

        if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
			m_ptrImgDraw->ReDrawLocalZoom();
		}
	}
	else if ((mode == ScanMode::M) || (mode == ScanMode::M_INIT) || (mode == ScanMode::ANATOMIC_M)) {
		if (mode != ScanMode::ANATOMIC_M)
			m_ptrImgDraw->ClearBMLine();
		m_ptrImgDraw->ClearFocus();
		if (m_timeMark)
			m_ptrImgDraw->ClearGridM();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScaleYM();
		if (m_timeMark)
			m_ptrImgDraw->DrawGridM();
		if (mode != ScanMode::ANATOMIC_M)
			m_ptrImgDraw->ReDrawBMLine();
		m_ptrImgDraw->DrawScanDirection();
	}
	else if ((mode == ScanMode::PW) || (mode == ScanMode::PW_INIT) || (mode == ScanMode::PW_SIMULT)) {
#ifdef EMP_355
       if(mode_bak)
            m_ptrImgDraw->ClearSVCW();
        else
            m_ptrImgDraw->ClearSV();
#else
		m_ptrImgDraw->ClearSV();
#endif
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScanDirection();
#ifdef EMP_355
        if(mode_bak)
            m_ptrImgDraw->ReDrawSVCW();
        else
            m_ptrImgDraw->ReDrawSV();
#else
		m_ptrImgDraw->ReDrawSV();
#endif
	}
   else if ((mode == ScanMode::CW_INIT) || (mode == ScanMode::CW)){
      m_ptrImgDraw->ClearSVCW();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScanDirection();
		m_ptrImgDraw->ReDrawSVCW();
     }
	else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI) || (mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D)) {
		//	    ptrUpdateCfm->EnterCfm();
		m_ptrImgDraw->ClearCfmBox();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->DrawScanDirection();
		m_ptrImgDraw->ReDrawCfmBox();
        g_menuBiopsy.UpdateBiopsyLine();//2016.11.05---hy
	}
	else if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_INIT) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)) {
		m_ptrImgDraw->ClearCfmBox();
#ifdef EMP_355
        if(mode_bak)
            m_ptrImgDraw->ClearSVCW();
        else
            m_ptrImgDraw->ClearSV();
#else
		m_ptrImgDraw->ClearSV();
#endif
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->ReDrawCfmBox();
#ifdef EMP_355
        if(mode_bak)
            m_ptrImgDraw->ReDrawSVCW();
        else
            m_ptrImgDraw->ReDrawSV();
#else
		m_ptrImgDraw->ReDrawSV();
#endif
		m_ptrImgDraw->DrawScanDirection();
	}	
	else if ((mode == ScanMode::CWCFM_INIT) || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI_INIT) || (mode == ScanMode::CWPDI)) {
		m_ptrImgDraw->ClearCfmBox();
		m_ptrImgDraw->ClearSVCW();
		m_ptrImgDraw->ClearFocus();
		m_ptrImgDraw->Update();
		m_ptrImgDraw->ReDrawFocus();
		m_ptrImgDraw->DrawScaleY();
		m_ptrImgDraw->ReDrawCfmBox();
		m_ptrImgDraw->ReDrawSVCW();
		m_ptrImgDraw->DrawScanDirection();
	}

}
void Update2D::Polarity(bool on, EKnobReturn status)
{
//	g_menu2D.UpdatePolarity(on);
//	g_menuM.UpdatePolarity(on);
	SyncKnobD2(D2_POLARITY, Toggle[on], status);	
}

void Update2D::NoiseReject(bool on)
{
	g_menu2D.UpdateNoiseReject(on);
}

void Update2D::Rotate(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menu2D.UpdateRotate(m_str, status);

	m_ptrImgDraw->ClearFocus();
  //  m_ptrImgDraw->ClearCenterLine();
    m_ptrImgDraw->Update();
    m_ptrImgDraw->DrawScaleY();
    m_ptrImgDraw->ReDrawFocus();
    m_ptrImgDraw->DrawScanDirection();
    g_menuBiopsy.UpdateBiopsyLine();
	KeyCenterLine key;
	key.Execute();
}

void Update2D::FrameAver(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menu2D.UpdateFrame(m_str, status);
}
void Update2D::LineAver(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menu2D.UpdateLine(m_str, status);
	g_menuM.UpdateLine(m_str, status);
}

void Update2D::Smooth(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menu2D.UpdateSmooth(m_str, status);
}

void Update2D::Gamma(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menu2D.UpdateGamma(m_str, status);
}

void Update2D::Chroma(const char* data, EKnobReturn status)
{
#if not defined(EMP_322) 
#if not defined(EMP_313)
	SyncKnobD2(D2_CHROMA, data, status, TRUE);
	SyncKnobM(M_CHROMA, data, status, TRUE);
#endif
#endif
}

void Update2D::ImgEhn(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menu2D.UpdateImgEhn(m_str, status);
}

void Update2D::CompoundSpace(int angle, EKnobReturn status)
{
    if (angle == 0)
        sprintf(m_str, "%s", Toggle[0]);
    else
        //sprintf(m_str, "%d°", angle);
        sprintf(m_str, "%d", angle);

	SyncKnobD2(D2_SPACE_COMPOUND, m_str, status);	
}
void Update2D::CompoundFreq(bool on, EKnobReturn status)
{
	SyncKnobD2(D2_FREQ_COMPOUND, Toggle[on], status);	
}

void Update2D::TpView(int index, EKnobReturn status)
{
	//SyncKnobD2(D2_TP_VIEW, TpName[index], status);	
	 SyncKnobD2(D2_TP_VIEW, Toggle[index], status);	
}

void Update2D::EFVI(bool on, EKnobReturn status)
{
	SyncKnobD2(D2_EFVI, Toggle[on], status);	
}

// M
void Update2D::TimeMarkM(bool on)
{
	m_timeMark = on;
	g_menuM.UpdateTimeMark(on);
	if (on)
		m_ptrImgDraw->DrawGridM();
	else
		m_ptrImgDraw->ClearGridM();
}

void Update2D::EnhanceM(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menuM.UpdateEnhanceM(m_str, status);
}
void Update2D::SmoothM(int data, EKnobReturn status)
{
	sprintf(m_str, "%d", data);
	g_menuM.UpdateSmooth(m_str, status);
}
void Update2D::TIS(float data)
{
	TopArea::GetInstance()->UpdateTIS(data);
}

// zoom
void Update2D::ZoomScale(double scale)
{
	m_ptrImgPara->UpdateGenZoomScale(scale);
	// m_ptrImgDraw->Update();
	// m_ptrImgDraw->DrawScaleY();
}

void Update2D::GlobalZoomScaleY(void)
{
	m_ptrImgDraw->ClearFocus();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->ReDrawFocus();
}

void Update2D::MoveFlag(POINT p)
{
	m_ptrImgDraw->DrawMoveFlag(p.x, p.y);
}
void Update2D::ClearMoveFlag()
{
	m_ptrImgDraw->ClearMoveFlag();
}

void Update2D::PIPMagnifier(POINT p)
{
	m_ptrImgDraw->DrawMagnifier(p.x, p.y);
}

void Update2D::ClearPIPMagnifier()
{
	m_ptrImgDraw->ClearMagnifier();
}
void Update2D::PIPBox(POINT p1, POINT p2)
{
	m_ptrImgDraw->DrawPIPBox(p1.x, p1.y, (p2.x-p1.x), (p2.y-p1.y));
}

void Update2D::ClearPIPBox()
{
	m_ptrImgDraw->ClearPIPBox();
}

void Update2D::EnterLocalZoom(void)
{
	m_ptrImgDraw->ClearFocus();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawFocus();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawScanDirection();
}

void Update2D::ExitLocalZoom(void)
{
	m_ptrImgDraw->ClearFocus();
	m_ptrImgDraw->Update();
	m_ptrImgDraw->ReDrawFocus();
	m_ptrImgDraw->DrawScaleY();
	m_ptrImgDraw->DrawScanDirection();
}
void Update2D::SetLocalZoomBoxStatus(bool changeSize)
{
	m_ptrImgDraw->ClearLocalZoom();
	m_ptrImgDraw->SetLocalZoomBoxDashStatus(changeSize);
	m_ptrImgDraw->ReDrawLocalZoom();
}

void Update2D::LocalZoomBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd)
{
	m_ptrImgDraw->Update();
	m_ptrImgDraw->DrawLocalZoom(lineBegin, lineEnd, dotBegin, dotEnd);
//	m_ptrImgDraw->DrawScaleY();
//	m_ptrImgDraw->ClearFocus();
//	m_ptrImgDraw->ReDrawFocus();
}

void Update2D::ClearLocalZoomBox()
{
	m_ptrImgDraw->ClearLocalZoom();
}

void Update2D::GrayReject(int rejecPoint)
{
	m_ptrImgDraw->DrawGrayRejectCurve(rejecPoint);
}

void Update2D::GrayTrans(unsigned char* colorTable, int size, int curPoint, unsigned char* basePoint, int maxPoint)
{
	m_ptrImgDraw->DrawGrayTransCurve(colorTable, basePoint, (unsigned char)curPoint);
}

void Update2D::ClearGrayCurve()
{
	m_ptrImgDraw->ClearGrayCurve();
}

void Update2D::BWColorTable(RGBTRIPLE *table, int size)
{
	m_ptrImgDraw->SetGrayTable(table, size);
}

void Update2D::SetCineRemoveImg(int count)
{
	ImageArea::SetCineRemoveImg(count);
}
void Update2D::CopyImgDataToReplay(void)
{
    ImageArea::CopyImgDataToReplay();
}

void Update2D::AutoOptimize(bool on)
{
    m_ptrImgPara->UpdateAutoOptimize(on);
}
