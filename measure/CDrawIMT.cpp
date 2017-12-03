/**
 * Copyritht @ 2012 Shenzhen Emperor Electronic Co. ,Ltd.
 *
 * @file: CDrawIMT.cpp
 * @brief: Draw IMT measure in the roi area
 *
 * version: V1.0
 * @author: lihuamei
 * date: 2012-7-10
 */


#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "CDrawIMT.h"
#include "lib/include/IMT.h"
#include "display/HintArea.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/FreezeMode.h"

 using namespace std;


CIMT *m_ptrImt = CIMT::GetInstance();

CDrawIMT* CDrawIMT::m_ptrInstance = NULL;
gboolean IMTCallBack(gpointer data)
{
    CDrawIMT::GetInstance()->ClearInAdventIMT();
    CDrawIMT::GetInstance()->UpdateIMT();
    return TRUE;
}

CDrawIMT* CDrawIMT::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new CDrawIMT;

	return m_ptrInstance;
}

CDrawIMT::CDrawIMT()
{
    m_on_off = 0;
    g_imtTimer = 0;
    pIntima = NULL;
    pAdventitia = NULL;
    pRoiPre = NULL;
    pRoi = NULL;
    m_RealIMTStatus = FALSE;
    m_IMTStatus = FALSE;
}

CDrawIMT::~CDrawIMT()
{
    if (m_ptrInstance != NULL)
		delete m_ptrInstance;
    if (pIntima)
    {
        delete [] pIntima;
        pIntima = NULL;
    }
    if (pAdventitia)
    {
        delete [] pAdventitia;
        pAdventitia = NULL;
    }
    if (pRoiPre)
    {
        delete [] pRoiPre;
        pRoiPre = NULL;
    }
    if (pRoi)
    {
        delete [] pRoi;
        pRoi = NULL;
    }
   m_RealIMTStatus = FALSE;

}
/**
 * function description: Get the witdth and height of the roi rectangle.
 * param w :the width of the roi rectangle.
 * param h: the height of the roi rectangle.
 * return : null
 */
void CDrawIMT::GetWH(int& w, int& h)
{
    roi_w = w;
    roi_h = h;
    SetRoiMem();
    SetInAdventIMTMem();
}

/**
 * function description: Get the start and end point of the roi rectangle.
 * param area_start :the start point of the roi rectangle.
 * param end_point: the end point of the roi rectangle.
 * return : null
 */
void CDrawIMT::GetArea(POINT& area_start, POINT& area_end)
{
    m_area_start = area_start;
    m_area_end = area_end;
}

/**
 * function description: get selected IMT measure mode (false: above ;true: below)
 * param dir : false: above ,true: below.
 * return : null
 */
void CDrawIMT::GetDirIMT(bool& dir)
{
    m_dir = dir;
}

void CDrawIMT::GetRealIMTStatus(bool& imtstatus)
{
    m_RealIMTStatus = imtstatus;
}

void CDrawIMT::GetIMTStatus(bool& imtstatus)
{
    m_IMTStatus = imtstatus;
}
/**
 * function description: start timer to begin to calculate intima and adventitia.
 * param  :null
 * return : null
 */
void CDrawIMT::StartTimer(void)
{
    if (g_imtTimer > 0)
    {
        if (!g_source_remove(g_imtTimer))
        {
            return;
        }
        g_imtTimer = 0;
    }
   g_imtTimer = g_timeout_add(200, IMTCallBack, NULL);//900ms
}

/**
 * function description: timer closing
 * param  :null
 * return : null
 */
void CDrawIMT::RemoveTimer(void)
{
    ClearInAdventIMT();
    if (g_imtTimer > 0)
    {
        if (!g_source_remove(g_imtTimer))
        {
            return;
        }
        g_imtTimer = 0;
    }
    if (pRoiPre)
    {
        delete [] pRoiPre;
        pRoiPre = NULL;
    }
    if (pRoi)
    {
        delete [] pRoi;
        pRoi = NULL;
    }
}

void CDrawIMT::GetResultAttr(UpdateMeasure::ResultAttr& attr)
{
    m_attr = attr;
}

/**
 * function description: update imt measure .get replay date to imt measure.
 * param  :null
 * return : null
 */
void CDrawIMT::UpdateIMT(void)
{
    ModeStatus ms;
    if ((Replay::GetInstance()->GetNextImgForIMT()) == NULL)
    {
        HintArea::GetInstance()->UpdateHint(_("There are not enough images for IMT measure."), 1);
        return;
    }
    if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
    {
        for (unsigned int i = 0; i < roi_w; i++)
        {
            for (unsigned int j = 0; j < roi_h; j++)
            {
                pRoi[i*roi_h+j] = *(ImageArea::m_bitsImg + (m_area_start.y+j)*IMAGE_W*3 + (m_area_start.x+i)*3 + 0);
                pRoiPre[i*roi_h+j] = *(ImageArea::m_TpbitsIMT + (m_area_start.y+j)*IMAGE_W*3 + (m_area_start.x+i)*3 + 0);
                // pRoi[i*roi_h+j] =(unsigned char )*(Replay::GetInstance()->GetLastImgForIMT()+ (m_area_start.y+j+IMAGE_Y)*IMG_AREA_W*3/4 +
                //                (m_area_start.x+i+IMAGE_X)*3/4 + 0);
                //pRoiPre[i*roi_h+j] =(unsigned char )*(Replay::GetInstance()->GetNextLastImgForIMT()+ (m_area_start.y+j+IMAGE_Y)*IMG_AREA_W*3/4 +
                //                (m_area_start.x+i+IMAGE_X)*3/4 + 0);
            }
        }
    }
    else if ((ms.GetFreezeMode() == FreezeMode::FREEZE) || (ms.GetFreezeMode() == FreezeMode::REPLAY))
    {
        for (unsigned int i = 0; i < roi_w; i++)
        {
            for (unsigned int j = 0; j < roi_h; j++)
            {
                pRoi[i*roi_h+j] =(unsigned char )*(Replay::GetInstance()->GetSelectImgForIMT()+ (m_area_start.y+j+IMAGE_Y)*IMG_AREA_W*3/4 +
                        (m_area_start.x+i+IMAGE_X)*3/4 + 0);
                pRoiPre[i*roi_h+j] =(unsigned char )*(Replay::GetInstance()->GetSelectNextImgForIMT()+ (m_area_start.y+j+IMAGE_Y)*IMG_AREA_W*3/4 +
                        (m_area_start.x+i+IMAGE_X)*3/4 + 0);
            }
        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Measurement is invalid in current mode."), 1);
        return;

    }

   // CIMT::GetInstance()->CalcTrack(pIntima, pAdventitia, pRoi, roi_h, roi_w, m_dir, m_UpDownStatus);
    CIMT::GetInstance()->CalcTrack(pIntima, pAdventitia, pRoi, roi_h, roi_w, m_dir);

    m_draw.DrawIMTBox(m_area_start, m_area_end, TRUE);
    if (m_dir)
    {
        m_draw.DrawIMTIntima(m_area_start, pIntima, roi_w*roi_h, TRUE);
        m_draw.DrawIMTAdvent(m_area_start, pAdventitia, roi_w*roi_h, TRUE);
    }
    else
    {
        m_draw.DrawIMTIntima(m_area_start, pAdventitia, roi_w*roi_h, TRUE);
        m_draw.DrawIMTAdvent(m_area_start,pIntima, roi_w*roi_h, TRUE);
    }

    SIMTPARA imt_para;
    imt_para.imt_max = 0.0;	             // IMT MAX
    imt_para.imt_min = 0.0;		        // IMT MIN
    imt_para.imt_std = 0.0;	           // IMT 标准差
    imt_para.imt_mean = 0.0;		  // IMT 平均值
    imt_para.imt_length = 0.0;	     // IMT 测量长度
    imt_para.imt_quality = 0.0;	 	// IMT 品质因子

     CIMT::GetInstance()->AnalyseData(imt_para, pRoi, pRoiPre, pIntima, pAdventitia, roi_w);

    //显示结果
    m_update.IMTResult(imt_para, m_attr, true);

}
/**
 * function description: clear imtbox , intima  and adventitia line
 * param attr: get the resultattr
 * return : null
 */
void CDrawIMT::ClearInAdventIMT(void)
{
    ModeStatus ms;
    if (ms.GetFreezeMode() != FreezeMode::UNFREEZE)
        m_draw.DrawIMTBox(m_area_start, m_area_end, FALSE);
    if (pIntima)
    {
        m_draw.DrawIMTIntima(m_area_start, pIntima, roi_w*roi_h, FALSE);
    }
    if (pAdventitia)
    {
        m_draw.DrawIMTAdvent(m_area_start, pAdventitia, roi_w*roi_h, FALSE);
    }
}


/**
 * function description: get up and down status
 * param  : null
 * return : null
 */
void CDrawIMT::GetUpDownStatus(void)
{
    m_UpDownStatus = ImgProc2D::GetInstance()->GetUpDownStatusForIMT();
}

void CDrawIMT::SetRoiMem(void)
{
    if (pRoiPre)
    {
        delete [] pRoiPre;
        pRoiPre = NULL;
    }
    pRoiPre = new unsigned char[roi_w*roi_h];
    memset(pRoiPre, 0, roi_h*roi_w);
    if (pRoi)
    {
        delete [] pRoi;
        pRoi = NULL;
    }
    pRoi = new unsigned char[roi_w*roi_h];
    memset(pRoi, 0, roi_w*roi_h);
}

void CDrawIMT::SetInAdventIMTMem(void)
{
    //调用计算内膜外膜的函数，并返回
    if (pIntima)
    {
        delete [] pIntima;
        pIntima = NULL;
    }
    pIntima = new unsigned int [roi_w*roi_h];
    memset(pIntima, 0, roi_w*roi_h*sizeof(unsigned int));
    if (pAdventitia)
    {
        delete [] pAdventitia;
        pAdventitia = NULL;
    }
    pAdventitia = new unsigned int [roi_w*roi_h];
    memset(pAdventitia, 0, roi_w*roi_h*sizeof(unsigned int));
}

