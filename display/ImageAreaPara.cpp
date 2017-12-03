/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: ImageAreaPara.cpp
 * @brief: display image parameter on image area
 *
 * version: V1.0
 * date: 2009-7-23
 * @author: yangneng
 */
#include "display/ImageArea.h"
#include "ImageAreaPara.h"
#include "CalcTime.h"
#include "imageProc/ModeStatus.h"
#include <stdlib.h>
#include "keyboard/KeyDef.h"
#include "ViewMain.h"
ImageAreaPara* ImageAreaPara::m_ptrInstance = NULL;

#ifdef TRANSDUCER
extern volatile int Transducer;
#endif

char * ImageAreaPara::Level[2] = {
    _("Low"),
    //_("MID"),
    _("High")
};	

#ifdef EMP_355
extern bool g_printProcessFps;
#endif

ImageAreaPara::ImageAreaPara()
{
    m_ptrImg = ImageArea::GetInstance();	
    memset(m_bufText, 0, sizeof (m_bufText));
    memset(m_freq, 0, sizeof (m_freq));
    memset(m_freqDoppler, 0, sizeof (m_freqDoppler));

    m_pwr = 100;
    m_zoomScale = 1.0;
    m_fps = 20;
    m_fpsForReplay = m_fps;
    m_fpsBak = m_fps;

    m_gain2D = 90;
    m_dynamicRange = 1;
    m_mbp = 1;
    m_thi = FALSE;
    m_tsi = "";
    m_gainM = 90;

    m_gainCw = 50;
    m_gainPw = 70;
    m_wfPw = 19;
    m_svPw = 2.0;
    m_PRFPw = 4.5;
    m_anglePw = 14;
    m_volumePw = 40;
    m_svPosPw = 10;
    m_HPRFStatus = FALSE;
#ifdef TRANSDUCER
 	m_transducer=0;
    m_transducerBak=0;
#endif
    m_wfCw = 19;
    m_PRFCw = 400;
    m_angleCw = 29;

    m_gainCfm = 80;
    m_wfCfm = 25;
    m_prfCfm = 350;
    m_persistCfm = 1;
    m_sensitiveCfm = 2;

    m_gainPdi = 80;
    m_optimize = FALSE;

    m_attr.area = ImageArea::PARA;
    m_attr.mode.cvDraw = true;
}

ImageAreaPara::~ImageAreaPara()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}
ImageAreaPara* ImageAreaPara::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ImageAreaPara;

    return m_ptrInstance;
}

int ImageAreaPara::GetGenFpsReplay()
{
    return m_fpsForReplay;
}

int ImageAreaPara::GetGenFps()
{
    return m_fps;
}
void ImageAreaPara::UpdateGenPwr(int data, bool draw)
{
    m_pwr = data;
    if (draw)
        DrawGeneralPara();
}

void ImageAreaPara::UpdateGenZoomScale(double data, bool draw)
{
    m_zoomScale = data;
    if (draw)
        DrawGeneralPara();
}

#ifdef EMP_355
void ImageAreaPara::UpdateGenFps(int receiveFps, int processFps, bool draw)
{
    m_fps = receiveFps;
    m_fpsForReplay = receiveFps;

    ModeStatus s;
    if (ModeStatus::IsUnFreezeMode()) 
    {

	//去掉1帧的波动。    
	if ((m_fps < m_fpsBak -2) || (m_fps > m_fpsBak +2))
	{       
	    m_fpsBak = m_fps;
	    if (draw)
		DrawGeneralPara();
	}
    }
    else
    { 
        m_fps = 0;
        //draw when freeze mode
        sprintf(m_bufText, " %3dfps", m_fps);
        m_ptrImg->DrawStringFps(m_bufText, 0, 0);
    }

    if (g_printProcessFps)
    { 
        int fps = processFps;
        FormatM::EFormatM formatM = s.GetFormatM();
        FormatPw::EFormatPw formatPw = s.GetFormatPw();
        int curPwImg = s.GetPwCurImg();
        if (ModeStatus::IsSpectrumSimult() || ModeStatus::IsSpectrumColorSimult())
        {
            float speed = (float)ImgPw::GetInstance()->GetRealSpectrumSpeed();
            int prf = ImgPw::GetInstance()->GetRealPRF();
            float n = (float)(DscMan::GetInstance()->GetDscPara()->dcaPWSpeed);
            int fpsPw = prf / speed / n;
            fps = fps - fpsPw;
        }
        else if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) && ((formatPw == FormatPw::P_TOTAL) || (curPwImg == 2)))
        {
            fps = 0;
        }
        else if (ModeStatus::IsMImgMode())
        {
            float updateSpeed = (float)(DscMan::GetInstance()->GetDscPara()->dcaMSpeed);
            int fpsM = 1 / Img2D::GetInstance()->GetScaleMTime() / updateSpeed; //fps of M image
            fps = fps - fpsM;

            if (formatM == FormatM::M_TOTAL)
            {
                fps = 0;
            }
        }
        PRINTF("Receive fps is %d, Process fps is %d\n", m_fps, fps);
    }
}
#else
void ImageAreaPara::UpdateGenFps(int data, bool draw)
{
    m_fpsForReplay = data;
    m_fps = data;

    ModeStatus s;
    FormatM::EFormatM formatM = s.GetFormatM();
    FormatPw::EFormatPw formatPw = s.GetFormatPw();
    int curPwImg = s.GetPwCurImg();

    if (ModeStatus::IsUnFreezeMode()) 
    {
        if (ModeStatus::IsSpectrumSimult() || ModeStatus::IsSpectrumColorSimult())
        {
            m_fpsForReplay = m_fps;

            float speed = (float)ImgPw::GetInstance()->GetRealSpectrumSpeed();
            int prf = ImgPw::GetInstance()->GetRealPRF();
            float n = (float)(DscMan::GetInstance()->GetDscPara()->dcaPWSpeed);
            int fpsPw = prf / speed / n;
            m_fps = m_fps - fpsPw;
        }
        else if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) && ((formatPw == FormatPw::P_TOTAL) || (curPwImg == 2)))
        {
                m_fps = 0;
        }
        else if (ModeStatus::IsMImgMode())
        {
            float updateSpeed = (float)(DscMan::GetInstance()->GetDscPara()->dcaMSpeed);
            int fpsM = 1 / Img2D::GetInstance()->GetScaleMTime() / updateSpeed; //fps of M image
            m_fps = m_fps - fpsM;
            m_fpsForReplay = m_fps;

            if (formatM == FormatM::M_TOTAL)
            {
                m_fps = 0;
            }
        }
        
        //去掉1帧的波动。
        if ((m_fps >= m_fpsBak -2) && (m_fps <= m_fpsBak +2))
            return;
        
        if ((m_fps > 70) || (m_fps < 0))
            m_fps = 70;
        m_fpsBak = m_fps;

        if (draw)
            DrawGeneralPara();
    } else {
        m_fps = 0;
        m_fpsBak = m_fps;
        //draw when freeze mode
        sprintf(m_bufText, " %3dfps", m_fps);
        m_ptrImg->DrawStringFps(m_bufText, 0, 0);
    }

}
#endif

#ifdef TRANSDUCER
void ImageAreaPara::UpdateTransducer(int data,bool draw)
{
    m_transducerBak = m_transducer;
    m_transducer = data;

    //    if (draw)
    //     ReDrawTransducer();

    DrawTransducer();
}
#endif

void ImageAreaPara::Update2DGain(int data, bool draw)
{
    m_gain2D = data;
    if (draw)
        Draw2DPara();
}
void ImageAreaPara::Update2DDynamicRange(int data, bool draw)
{
    m_dynamicRange = data;
    if (draw)
        Draw2DPara();
}
void ImageAreaPara::Update2DLineDensity(const char* data, bool draw)
{
    m_lineDensity = data;
    if (draw)
        Draw2DPara();
}

void ImageAreaPara::Update2DMBP(int data, bool draw)
{
    m_mbp = data;
    if (draw)
        Draw2DPara();
}

void ImageAreaPara::Update2DTHI(bool data, bool draw)
{
    m_thi = data;
    if (draw)
        Draw2DPara();
}
void ImageAreaPara::Update2DTSI(const char* data, bool draw)
{
    m_tsi = data;
    if (draw)
        Draw2DPara();
}
void ImageAreaPara::UpdateMGain(int data, bool draw)
{
    m_gainM = data;
    if (draw)
        DrawMPara();
}
void ImageAreaPara::UpdateMDynamicRange(int data, bool draw)
{
    m_dynamicRange = data;
    if (draw)
        DrawMPara();
}

void ImageAreaPara::UpdatePwGain(int data, bool draw)
{
    m_gainPw = data;
    if (draw)
        DrawPwGenPara();
}

void ImageAreaPara::UpdatePwWF(int data, bool draw)
{
    m_wfPw = data;
    if (draw)
        DrawPwGenPara();
}

void ImageAreaPara::UpdatePwSV(double data, bool draw)
{
    m_svPw = data;
    if (draw)
        DrawPwGen2Para();
}

void ImageAreaPara::UpdatePwPRF(double data, bool draw)
{
    m_PRFPw = data;
    if (draw)
        DrawPwGenPara();
}

void ImageAreaPara::UpdatePwAngle(int data, bool draw)
{
    m_anglePw = data;
    if (draw)
        DrawPwGen2Para();
}
void ImageAreaPara::UpdatePwSVPos(double data, bool draw)
{
    m_svPosPw = data;
    if (draw)
        DrawPwGenPara();
}

void ImageAreaPara::UpdatePwHPRFStatus(bool on, bool draw)
{
    m_HPRFStatus = on;
    if (draw) 
        DrawPwGen2Para();
}

void ImageAreaPara::UpdateVolume(int data, bool draw)
{
    m_volumePw = data;
    if (draw)
        DrawGeneralPara();
}

void ImageAreaPara::UpdateCwGain(int data, bool draw)
{
    m_gainCw = data;
    if (draw)
        DrawPwGenPara();
}
void ImageAreaPara::UpdateCwWF(int data, bool draw)
{
    m_wfCw = data;
    if (draw)
        DrawCwPara();
}
void ImageAreaPara::UpdateCwPRF(double data, bool draw)
{
    m_PRFCw = data;
    if (draw)
        DrawCwPara();
}
void ImageAreaPara::UpdateCwAngle(int data, bool draw)
{
    m_angleCw = data;
    if (draw)
        DrawCwPara();
}

void ImageAreaPara::UpdateCwSVPos(double data, bool draw)
{
    m_svPosCw = data;
    if (draw)
        DrawCwPara();
}
void ImageAreaPara::UpdateCfmGain(int data, bool draw)
{
    m_gainCfm = data;
    if (draw)
        DrawCfmPara();
}
void ImageAreaPara::UpdateCfmWF(int data, bool draw)
{
    m_wfCfm = data;
    if (draw)
        DrawCfmPara();
}
void ImageAreaPara::UpdateCfmPRF(int data, bool draw)
{
    m_prfCfm = data;
    if (draw)
        DrawCfmPara();
}
void ImageAreaPara::UpdateCfmPersist(int data, bool draw)
{
    m_persistCfm = data;
    if (draw)
        DrawCfmPara();
}
void ImageAreaPara::UpdateCfmSensitive(int data, bool draw)
{
    m_sensitiveCfm = data;
    if (draw)
        DrawCfmPara();
}

void ImageAreaPara::UpdatePdiGain(int data, bool draw)
{
    m_gainPdi = data;
    if (draw)
        DrawPdiPara();
}
void ImageAreaPara::UpdatePdiWF(int data, bool draw)
{
    m_wfCw = data;
    if (draw)
        DrawPdiPara();
}
void ImageAreaPara::UpdatePdiPersist(int data, bool draw)
{
    m_persistCfm = data;
    if (draw)
        DrawPdiPara();
}
void ImageAreaPara::UpdatePdiSensitive(int data, bool draw)
{
    m_sensitiveCfm = data;
    if (draw)
        DrawPdiPara();
}
void ImageAreaPara::UpdateAutoOptimize(bool data, bool draw)
{
    m_optimize = data;
    if (draw)
        DrawGeneralPara();
}
void ImageAreaPara::UpdateFreq(const char* freq, bool draw)
{
    sprintf(m_freq, "%s", freq);
    if (draw)
        Draw2DPara();
}
void ImageAreaPara::UpdateCfmDopplerFreq(const char* freq, bool draw)
{
    sprintf(m_freqDoppler, "%s", freq);
    if (draw)
        DrawCfmPara();
}
void ImageAreaPara::UpdatePdiDopplerFreq(const char* freq, bool draw)
{
    sprintf(m_freqDoppler, "%s", freq);
    if (draw)
        DrawPdiPara();
}
void ImageAreaPara::UpdateDepth(int depth, bool draw)
{
    m_depth = depth;
    if (draw)
        Draw2DPara();
}

///>  private
void ImageAreaPara::DrawGeneralPara()
{
    int x = 5;
    int y = 5;

    FillRectangle(x, y, m_eareWl, m_eareH);
#if (defined(EMP_322) || defined(EMP_313))
    sprintf(m_bufText, "%3dP %3.1fX %3dfps", m_pwr, m_zoomScale, m_fps);
    if (m_optimize)
        sprintf(m_bufText, "%3dP %3.1fX %3dfps eSpeed", m_pwr, m_zoomScale, m_fps);
    else
        sprintf(m_bufText, "%3dP %3.1fX %3dfps", m_pwr, m_zoomScale, m_fps);
#else
    sprintf(m_bufText, "%3dP %3.1fX %2dV %3dfps", m_pwr, m_zoomScale, m_volumePw, m_fps);

    if (m_optimize)
        sprintf(m_bufText, "%3dP %3.1fX %2dV %3dfps eSpeed", m_pwr, m_zoomScale, m_volumePw, m_fps);
    else
    {
        sprintf(m_bufText, "%3dP %3.1fX %2dV %3dfps", m_pwr, m_zoomScale, m_volumePw, m_fps);
    }
#endif
    m_ptrImg->DrawString(m_bufText, x, y);
}

void ImageAreaPara::Draw2DPara()
{
    int x = 5;
    int y = 30;

    FillRectangle(x, y, m_eareWs, m_eareH);
    m_ptrImg->DrawString("<b><span underline=\"low\" >2D</span></b>", x, y);
	
    FillRectangle(x, y+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "%dG %ddB %dMBP %s", m_gain2D, m_dynamicRange, m_mbp, m_tsi.c_str());
    m_ptrImg->DrawString(m_bufText, x, y+20);

    FillRectangle(x, y+40, m_eareWl, m_eareH);
    sprintf(m_bufText, "%s %dmm", m_freq, m_depth);
    m_ptrImg->DrawString(m_bufText, x, y+40);
}

void ImageAreaPara::DrawMPara()
{
    Draw2DPara();

    int x = 5;
    int y = 75 + 20;

    FillRectangle(x, y, m_eareWs, m_eareH);
    m_ptrImg->DrawString("<b><span underline=\"low\" >M</span></b>", x, y);

    FillRectangle(x, y+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "%3dG", m_gainM);
    m_ptrImg->DrawString(m_bufText, x, y+20);
}

void ImageAreaPara::DrawPwPara()
{
    Draw2DPara();

    int x = 5;
    int y = 120+20+20;

    FillRectangle(x, y, m_eareWs, m_eareH);
    m_ptrImg->DrawString("<b><span underline=\"low\" >Spectrum</span></b>", x, y);

    DrawPwGenPara();
    DrawPwGen2Para();
}

void ImageAreaPara::DrawPwGenPara(void)
{
    int x = 5;
    int y = 140+20+20;
    FillRectangle(x, y, m_eareWl, m_eareH);
    ModeStatus s1;
     ScanMode::EScanMode mode = s1.GetScanMode();
#ifdef EMP_355
   if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
    {
       sprintf(m_bufText, "%dG %.2fKHz WF%d %.1fcm", m_gainPw, m_PRFPw, m_wfPw, m_svPosPw/10);
    }
#else
     if (ModeStatus::IsCWMode() || ModeStatus::IsCWColorMode())
     {
         sprintf(m_bufText, "%dG %.2fKHz WF%d %.1fcm", m_gainCw, m_PRFPw, m_wfPw, m_svPosPw/10);
     }
     else if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
     {
         sprintf(m_bufText, "%dG %.2fKHz WF%d %.1fcm", m_gainPw, m_PRFPw, m_wfPw, m_svPosPw/10);
    }
#endif
    m_ptrImg->DrawString(m_bufText, x, y);
}

void ImageAreaPara::DrawPwGen2Para(void)
{
    int x = 5;
    int y = 160+20+20;
    FillRectangle(x, y, m_eareWl, m_eareH);
    ModeStatus s1;
    ScanMode::EScanMode mode = s1.GetScanMode();
    if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode())
    {
#ifdef EMP_355
		if(!ViewMain::GetInstance()->GetModeIsFlag())
		{
     	 	sprintf(m_bufText, "%.1fmm %dH", m_svPw, (int)m_HPRFStatus);
      	 	m_ptrImg->DrawString(m_bufText, x, y);
       	 	x = 85;
		}
#else
        sprintf(m_bufText, "%.1fmm %dH", m_svPw, (int)m_HPRFStatus);
        m_ptrImg->DrawString(m_bufText, x, y);
        x = 85;
#endif

    }
    else if (ModeStatus::IsCWMode() || ModeStatus::IsCWColorMode())
    {
        sprintf(m_bufText, "%dH", (int)m_HPRFStatus);
        m_ptrImg->DrawString(m_bufText, x, y);
        x = 35;
    }

    if (abs(m_anglePw) > 60) {
        sprintf(m_bufText, "%d°", m_anglePw);
    	m_ptrImg->DrawString(m_bufText, x, y, g_red);
    } else {
    	sprintf(m_bufText, "%d°", m_anglePw);
    	m_ptrImg->DrawString(m_bufText, x, y, g_white);
    }
}

void ImageAreaPara::DrawCwPara()
{
    Draw2DPara();

    int x = 5;
    int y = 120+20+20;

    FillRectangle(x, y, m_eareWs, m_eareH);
    m_ptrImg->DrawString("<b><span underline=\"low\" >CW</span></b>", x, y);
	
    FillRectangle(x, y+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "GC%d %.1fcm %dKHz %dHz %d°", m_gainCw, m_svPosCw/10, (int)m_PRFCw, m_wfCw, m_angleCw);
    m_ptrImg->DrawString(m_bufText, x, y+20);
}

void ImageAreaPara::DrawCfmPara()
{
    Draw2DPara();

    int x = 5;
    int y = 75+20;

    FillRectangle(x, y, m_eareWs, m_eareH);
    m_ptrImg->DrawString("<b><span underline=\"low\" >Color</span></b>", x, y);

    FillRectangle(x, y+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "%dG %.2fKHz WF%d %dP %dS", m_gainCfm, m_prfCfm/(float)1000, m_wfCfm, m_persistCfm, m_sensitiveCfm);
    m_ptrImg->DrawString(m_bufText, x, y+20);
    
    FillRectangle(x, y+20+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "%s",m_freqDoppler);
    m_ptrImg->DrawString(m_bufText, x, y+40);
}

void ImageAreaPara::DrawPdiPara()
{
    int x = 5;
    int y = 75+20;

    FillRectangle(x, y, m_eareWs, m_eareH);
    m_ptrImg->DrawString("<b><span underline=\"low\" >PDI</span></b>", x, y);

    FillRectangle(x, y+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "%dG %.2fKHz WF%d %dP %dS", m_gainPdi, m_prfCfm/(float)1000, m_wfCfm, m_persistCfm, m_sensitiveCfm);
    m_ptrImg->DrawString(m_bufText, x, y+20);

    FillRectangle(x, y+20+20, m_eareWl, m_eareH);
    sprintf(m_bufText, "%s",m_freqDoppler);
    m_ptrImg->DrawString(m_bufText, x, y+40);
}

#ifdef TRANSDUCER
void ImageAreaPara::DrawTransducer(void)
{
    char strbuffer[350];
    int width = 26, height = 95;
    int x = 20, y = 220+5;

     sprintf( strbuffer, "-0.1");
     m_ptrImg->DrawString(strbuffer, x + width + 2+3, y-9+2, g_white);
     sprintf(strbuffer, "-0.0%d%dMPa", (int)m_transducer/10, (int)m_transducer%10);
     m_ptrImg->DrawString(strbuffer, x, y + height + 2, g_white);	

     m_ptrImg->DrawTransducerResult(x, y, width, height, m_transducer);
}

void ImageAreaPara::ReDrawTransducer(void)
{
    
    DrawTransducer();
#if 0
    int width = 26, height = 100;
    //int x = 5, y = 40 + 280;
    int x = 20, y = 220;

    m_ptrImg->DrawTransducerResult(x, y, width, height, m_transducerBak);
#endif
}
#endif

void ImageAreaPara::FillRectangle(int x, int y, int w, int h)
{
    m_ptrImg->DrawRectangle(m_attr, g_black, x, y, w, h, true, false);
}


