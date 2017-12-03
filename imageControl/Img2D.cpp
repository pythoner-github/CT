/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: Img2D.cpp
 * @brief: control the imaging parameter of 2D and M scan mdoe.
 *
 * version: V1.0
 * date: 2009-5-9
 * @author: zhanglei
 */
#include "SysGeneralSetting.h"
#include "Img2D.h"
#include "ImgPw.h"
#include "Def.h"
#include "ModeStatus.h"
#include "GlobalClassMan.h"
#include "BDSC.h"
#include "HintArea.h"
#include "../imageProc/Zoom.h"
#include "../imageProc/DscMan.h"
#include "unistd.h"
#include "ViewMain.h"
#include "IoCtrl.h"
#include <stdio.h>
#include "ViewSuperuser.h"
///> /////////////////////////////////[static const]////////////////////////////

///> focus
const int Img2D::FOC_DEPTH = 220; ///< value to calc focus position percent.
const int Img2D::FOC_POS_INDEX_MAX[MAX_FOCUS] = {18, 11, 8, 6};

const unsigned char Img2D::ONE_FOC_POS[18][1] =
{	
	{5},{10},{20},{30},{40},{50},{60},{70},{80},
	{90},{100},{110},{120},{130},{140},{150},{160},{170}
//    {90},{100},{110},{120},{130},{145},{160},{180},{200}
};
const unsigned char Img2D::TWO_FOC_POS[11][2] = 
{
	{5,20},{10,20},{20,30},{30,40},{40,60},{50,70},
	{60,90},{70,100},{80,120},{90,140},{100,170}
};
const unsigned char Img2D::THREE_FOC_POS[8][3] =
{
	{5,20,30},{10,20,30},{20,30,40},{30,40,60},
	{40,60,90},{50,70,100},{60,90,140},{70,100,170}
};
const unsigned char Img2D::FOUR_FOC_POS[6][4] =
{
	{5,20,30,50},{10,20,30,50},	{20,30,40,60},
	{30,40,60,90},{40,60,90,140},{50,70,100,170}
};

///> image scale of depth 
const int Img2D::IMG_SCALE[MAX_SCALE_INDEX] = {10, 11, 12, 14, 15, 17, 18, 20, 22, 23, 25, 27, 29, 30, 32, 33, 35, 37, 39, 40, 42, 45, 47, 50};
//const int Img2D::IMG_SCALE[MAX_SCALE_INDEX] = {10, 12, 15, 18, 20, 23, 26, 30, 33, 36, 40, 8};
const int Img2D::DYNAMIC_DATA_D[Calc2D::MAX_DYNAMIC_INDEX] = {30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160};
const int Img2D::POWER_DATA[MAX_POWER_INDEX] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
const double Img2D::TSI_SPEED[MAX_TSI] = {1.54, 1.58, 1.46, 1.50};//km/s
const int Img2D::STEER_ANGLE[MAX_STEER] = {-15, -10, -5,  0, 5, 10, 15};
const int Img2D::MBP[MAX_MBP_INDEX] = {1, 2, 4};
const int Img2D::FPGAMBP2D[MAX_LINE_DENSITY][MAX_MBP_INDEX] = {{2, 2, 4}, {1, 2, 2}};
const int Img2D::FPGAMBPPw[MAX_LINE_DENSITY][MAX_MBP_INDEX] = {{1, 1, 1}, {1, 1, 1}};
const int Img2D::FPGAMBPColor[MAX_LINE_DENSITY][MAX_MBP_INDEX] = {{4, 6, 6}, {2, 4, 6}};
const string Img2D::TSI_DISPLAY[MAX_TSI] = {N_("Routine"), N_("Muscle"), N_("Liquid"), N_("Fat")}; 
const string Img2D::LINE_DENSITY_DISPLAY[MAX_LINE_DENSITY] = {N_("Low"), N_("High")};
//const int Img2D::SPACE_COMPOUND_ANGLE[MAX_SPACE_COMPOUND] = {0, 4, 7, 10}; //degree //解决深度调节到最大时第3级出现黑色三角形的问题
const int Img2D::SPACE_COMPOUND_ANGLE[MAX_SPACE_COMPOUND] = {0, 3, 5, 8}; //degree

const int Img2D::SCAN_LINES_PHASE_PROBE[MAX_SCAN_LINES] = {64, 128, 192, 256};

int Img2D::m_data = 40;
float Img2D::m_fc01 = 1.0;
float Img2D::m_fc02 = 1.0;
float Img2D::m_fc03 = 1.0;
float Img2D::m_fc04 = 1.0;
float Img2D::m_fc05 = 1.0;
float Img2D::m_fc1 = 1.0;
float Img2D::m_fc2 = 1.0;
float Img2D::m_fc3 = 1.0;
float Img2D::m_fc4 = 1.0;
float Img2D::m_fc5 = 1.0;
int Img2D::m_freqEmit = 100;

///> /////////////////////////////////[private variable]////////////////////////////
Img2D* Img2D::m_ptrInstance = NULL;

///> /////////////////////////////////[public func]////////////////////////////

Img2D* Img2D::GetInstance()
{
	if(m_ptrInstance == NULL)
	{
		m_ptrInstance = new Img2D;
	}

	return m_ptrInstance;
}

///> construct
Img2D::Img2D()
{
	GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
	m_ptrUpdate = ptrGcm->GetUpdate2D();

	DscMan* ptrDscMan = DscMan::GetInstance();
	m_ptrDscPara = ptrDscMan->GetDscPara();	

	//m_ptrImgDraw = ImageAreaDraw::GetInstance();

	m_ptrDsc = NULL;
	m_ptrCalc = NULL;
    m_type = 'c';
	m_ctGain2D.Begin();	
	m_ctGainM.Begin();	
	m_tGain2D = m_tGainM = 0;

	m_ctDepth.Begin();
	m_tDepth = 0;
	m_stepDepth = 0;

	///> 2D mode 
	int i;
	for (i = 0; i < 8; i ++) 
		m_tgc[i] = 128;
	m_gain2D = 98;
	m_gainM = 100;
	m_lines = 255;
	m_freqRange[0] = 0;
	m_freqRange[1] = 255; 
	m_vecFreqRange.clear();
	m_freqIndex = 0;
	m_freq.emit = 70; 
	m_freq.receive = 70; 
	m_depthMax = 250; 
	m_depth = 250; 
	m_imgScaleIndex = 0; 
	m_focSum = 1; 
	m_focPosIndex = 0; 
	for (i = 0; i < MAX_FOCUS; i ++)
	{
		m_focPos[i] = 0;
	}
	m_scanAngleIndex = 0; 
	m_dynamicRangeIndex = 7; 
	m_lineDensityIndex = 0; 
	m_lineDensityIndexBak = 0; 
	m_edgeEnhanceIndex = 0;
	m_soundPowerIndex = 8;
	m_harmonic = FALSE; 
	m_tsiIndex = 0; ///< 常规
	m_mbpIndex = 0;
	m_mbpIndexBak = 0;
	m_agcIndex = 0;
	m_steerIndex = 3; // 0度
    m_steerIndexbak = 3;

	///> M mode
	m_mLine = 0; 
	m_mSpeedIndex = 0; 

	for (i = 0; i < 4; i ++)
	{
		m_scale2D[i] = 1.0;
		m_scale2D_bak[i] = 1.0;
                 m_scale2DZoom[i] = 1.0;    
                 m_scale2DZoom_bak[i] = 1.0;    
	}

	//efov
	m_frameAverEfovBak = 0;
	m_lineAverEfovBak = 0;
	m_mouseSpeedEfovBak = 0;

    // special measure
    for (int i = 0; i < 4; i ++)
    {
        m_scale2DAllSnap[i] = 0.0;
    }
    m_scale2DSnap = 0.0;
    m_scaleMDepthSnap = 0.0;
    m_scaleMTimeSnap = 0.0;

    m_zoomMeasureCtrl = FALSE;
    m_zoomScale = 1.0;
    m_focIndexRange[0] = 0;
    m_focIndexRange[1] = FOC_POS_INDEX_MAX[m_focSum-1] - 1;

    m_spaceCompoundIndex = 2;
    m_spaceCompoundIndexBak = m_spaceCompoundIndex;
    m_spaceCompoundIndexBakBak = m_spaceCompoundIndex;
    m_freqCompoundCtrl = TRUE;
	m_freqCompoundCtrlBak = m_freqCompoundCtrl;
	m_freqCompoundCtrlBakBak = m_freqCompoundCtrl;

    //status of thi
    m_statusTHI = FALSE;

    m_numSumIndex = 0;
    m_curProbeIndex = 0;
    m_curHarmonicFreqIndex = 0;
    m_indexScanLines = MAX_SCAN_LINES - 1;
    m_harmonicFreqIndex = 0;
    m_polarity = FALSE;
}

Img2D::~Img2D()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void Img2D::SetCalc2D(Calc2D* calc)
{
	m_ptrCalc = calc; 

	m_calcPara.depthDots = IMG_H * Calc2D::INIT_SCALE;
	m_calcPara.imgScaleIndexMax = MAX_SCALE_INDEX; 
	m_calcPara.angleIndexMax = MAX_ANGLE;
	m_calcPara.soundSpeed = SOUND_SPEED;
	m_calcPara.soundSpeedTsi = SOUND_SPEED;
	m_calcPara.compoundSpaceCtrl = TRUE;
	m_calcPara.compoundAngle = 6;
	m_calcPara.compoundFreqCtrl= TRUE;
    m_calcPara.tpViewCtrl = FALSE;
    m_calcPara.efviCtrl = FALSE;
    m_calcPara.tpSteer = 0.0;
    m_calcPara.focSum = 1;
 
    PRINTF("setcalc2D width = %d\n", m_calcPara.probeWidth);
	m_calcPara.modeCw = FALSE;

  	m_ptrCalc->SetCalcPara(&m_calcPara);
}

/*
 * @brief call when probe or exam item is changed. Read probe para from class ProbeMan and 
 *  read imging para from class ExamItem.
 *
 * @para ptrMan pointer pointed to class ProbeMan object
 * @para ptrParaItem pointer pointed to class ExamItem object
 */
void Img2D::InitProbe2DOptimizePara(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
	EKnobReturn ret;
	int i;

	///> set item para-common

	// freq
	m_freqIndex = ptrParaItem->d2.freqIndex;
    if(m_freqIndex > (int)(m_vecFreqRange.size() - 1))
        m_freqIndex = 0;

	m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
	m_freq.receive = m_vecFreqRange[m_freqIndex].receive;

	m_calcPara.freq.emit = m_freq.emit;
	m_calcPara.freq.receive = m_freq.receive;
	if (m_freqIndex == 0)
		ret = MIN;
	else if (m_freqIndex == (int)(m_vecFreqRange.size()-1))
#if (defined(EMP_322) || defined(EMP_355))
		ret = OK;
#else
		ret = MAX;
#endif
	else
		ret = OK;
	m_ptrUpdate->Freq(m_freq.receive, ret);
    
    //sound power
    m_soundPowerIndex = ptrParaItem->common.powerIndex;	
#ifdef EMP_340
    if(!strcmp(ptrPara->model, "10L25J"))
    {
        if(m_freq.receive == 180) //9MHz
        {
            m_soundPowerIndex = 9;
        }
        else if(m_freq.receive == 200) //10MHz
        {
            m_soundPowerIndex = 7;
        }
        else if(m_freq.receive == 240) //12MHz
        {
            m_soundPowerIndex = 4;
        }
    }
#endif
    m_calcPara.power = POWER_DATA[m_soundPowerIndex];
    if (m_soundPowerIndex == 0)
		ret = MIN;
	else if (m_soundPowerIndex == (MAX_POWER_INDEX - 1))
		ret = MAX;
	else
		ret = OK;
	m_ptrUpdate->SoundPower(POWER_DATA[m_soundPowerIndex], ret);

	// tgc
	m_gain2D = ptrParaItem->d2.gain2D;
	m_ptrUpdate->Gain2D(m_gain2D);
	
	// focus info
	m_focSum = ptrParaItem->d2.focSum;	
	m_calcPara.focSum = m_focSum;
	m_focPosIndex = ptrParaItem->d2.focPosIndex;	
	GetFocPos();
	for (i = 0; i < MAX_FOCUS; i ++)
	{
		m_calcPara.focPos[i] = m_focPos[i];
	}
	m_ptrUpdate->FocInfo(m_focSum, m_focPos);

    // scan angle,角度实际值需计算后才知道，因此在完成计算后显示。 
    m_scanAngleIndex = ptrParaItem->d2.scanAngle;	
    if(m_scanAngleIndex <= 0)
        m_scanAngleIndex = 0;
    else if(m_scanAngleIndex >= (MAX_ANGLE -1))
        m_scanAngleIndex = MAX_ANGLE - 1;
    SetScanAngle(m_scanAngleIndex);
    m_calcPara.angleIndex = m_scanAngleIndex;

	// dynamicRange
	m_dynamicRangeIndex = ptrParaItem->d2.dynamicRange;	
	if (m_dynamicRangeIndex == 0)
		ret = MIN;
	else if (m_dynamicRangeIndex == (Calc2D::MAX_DYNAMIC_INDEX - 1))
		ret = MAX;
	else
		ret = OK;
	m_ptrUpdate->DynamicRange(DYNAMIC_DATA_D[m_dynamicRangeIndex], ret);

	// Line density
	m_lineDensityIndex = ptrParaItem->d2.lineDensity;	
	m_calcPara.lineDensity = m_lineDensityIndex;
	if (m_lineDensityIndex == 0)
		ret = MIN;
	else if (m_lineDensityIndex == (MAX_LINE_DENSITY - 1))
		ret = MAX;
	else
		ret = OK;
	m_ptrUpdate->LineDensity(LINE_DENSITY_DISPLAY[m_lineDensityIndex].c_str(), ret);

    ///MBP
    m_mbpIndex = ptrParaItem->common.MBP;	
    if (m_mbpIndex == 0)
        ret = MIN;
    else if (m_mbpIndex == (MAX_MBP_INDEX - 1))
        ret = MAX;
    else
        ret = OK;
    m_calcPara.mbp = MBP[m_mbpIndex];
    m_ptrUpdate->MBP(MBP[m_mbpIndex], ret);

	// edge
	m_edgeEnhanceIndex = ptrParaItem->d2.edgeEnhance;
	m_calcPara.edgeEnhance = m_edgeEnhanceIndex;
	m_ptrDscPara->dca2DIPAttrs.ipaEdgeEhn = m_edgeEnhanceIndex;
	if (m_edgeEnhanceIndex == 0)
		ret = MIN;
	else if (m_edgeEnhanceIndex == (MAX_EDGE - 1))
		ret = MAX;
	else 
		ret = OK;
	m_ptrUpdate->EdgeEnhance(m_edgeEnhanceIndex, ret);

	// agc?
	m_agcIndex = ptrParaItem->d2.AGC;
	if (m_agcIndex == 0)
		ret = MIN;
	else if (m_agcIndex == (MAX_AGC - 1))
		ret = MAX;
	else
		ret = OK;
	m_ptrUpdate->Agc(m_agcIndex, ret);

#if 0
	// harmonic
    if (m_freq.emit > MAX_HARMONIC_FREQ)
        m_harmonic = FALSE;
    else
#endif
    m_harmonic = ptrParaItem->d2.harmonic;
    m_calcPara.harmonic = m_harmonic;
    m_statusTHI = m_harmonic;
    //status 
    m_ptrUpdate->HarmonicStatus(m_harmonic, ret);

    if (!m_harmonic)
        ret = MIN;
    else if (m_harmonic)
        ret = MAX;
    else
        ret = OK;
    //m_ptrUpdate->Harmonic(m_harmonic, ret);

    // harmonic freq.
    m_harmonicFreqIndex = ptrParaItem->d2.thiFreqIndex;
    if(m_harmonicFreqIndex <= 0)
    {
        m_harmonicFreqIndex = 0;
#if (defined(EMP_355) || defined(EMP_322))
		ret = OK;
#else
        ret = MIN;
#endif
    }
    else if (m_harmonicFreqIndex >= m_thiFreqSum - 1) //ProbeSocket::MAX_HARMONIC_FREQ - 1) //只显示3个
    {
        m_harmonicFreqIndex = m_thiFreqSum - 1;
        ret = MAX;
    }
    else
        ret = OK;
    
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    for (i = 0; i < NUM_PROBE; i++) 
    { 
        if(strcmp(para.model, PROBE_LIST[i].c_str()) == 0)
            m_curProbeIndex = i;
    }

    if(m_harmonic)
    {
        m_freq.emit = m_thiFreq[m_harmonicFreqIndex] ;//m_vecFreqRange[m_freqIndex].emit;
        m_freq.receive = m_thiFreq[m_harmonicFreqIndex];//m_vecFreqRange[m_freqIndex].receive;

        m_calcPara.freq.emit = m_freq.emit;
        m_calcPara.freq.receive = m_freq.receive;
        
        //value
        m_ptrUpdate->HarmonicFreq(m_thiFreq[m_harmonicFreqIndex], ret);
    }
    else
    {
        m_freq.emit = m_thiFreq[m_harmonicFreqIndex] ;//m_vecFreqRange[m_freqIndex].emit;
        m_freq.receive = m_thiFreq[m_harmonicFreqIndex];//m_vecFreqRange[m_freqIndex].receive;
        m_calcPara.freq.emit = m_freq.emit;
        m_calcPara.freq.receive = m_freq.receive;

        DefaultDynamicDemodFd(m_curProbeIndex, m_harmonicFreqIndex);
        m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
        m_freq.receive = m_vecFreqRange[m_freqIndex].receive;
        m_calcPara.freq.emit = m_freq.emit;
        m_calcPara.freq.receive = m_freq.receive;
    }
#if (defined(EMP_355) || defined(EMP_322))
    m_fc01 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][0] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][0]) / 20.0);
    m_fc02 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][1] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][1]) / 20.0);
    m_fc03 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][2] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][2]) / 20.0);
    m_fc04 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][3] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][3]) / 20.0);
    m_fc05 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][4] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][4]) / 20.0);
#else 
    m_fc01 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][0] / 10.0);
    m_fc02 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][1] / 10.0);
    m_fc03 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][2] / 10.0);
    m_fc04 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][3] / 10.0);
    m_fc05 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][4] / 10.0);
#endif
    m_fc1 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][0] / 10.0);
    m_fc2 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][1] / 10.0);
    m_fc3 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][2] / 10.0);
    m_fc4 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][3] / 10.0);
    m_fc5 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][4] / 10.0);

    DefaultFreqBandPassFilter(m_curProbeIndex, m_harmonicFreqIndex);
    DefaultFreqDynamicFilter(m_curProbeIndex, m_harmonicFreqIndex);
    if(m_harmonic)
        DefaultIndexBandPassFilter(m_curProbeIndex, m_harmonicFreqIndex);
    else
        DefaultIndexBandPassFilterBaseFreq(m_curProbeIndex, m_freqIndex);


    // tsi
    m_tsiIndex = ptrParaItem->d2.TSI;
    m_calcPara.soundSpeedTsi = TSI_SPEED[m_tsiIndex];
    if (m_tsiIndex == 0)
		ret = MIN;
	else if (m_tsiIndex == (MAX_TSI - 1))
		ret = MAX;
	else
		ret = OK;
	m_ptrUpdate->Tsi(TSI_DISPLAY[m_tsiIndex].c_str(), ret);


    if(ModeStatus::IsD2Mode())
    {
        // space compound
        if ((ptrPara->type == 'P') || (ptrPara->type == 'p'))
            m_spaceCompoundIndex = 0;
        else
            m_spaceCompoundIndex = ptrParaItem->d2.spaceCompoundIndex;
        //m_spaceCompoundIndexBak = m_spaceCompoundIndex;
        if (m_spaceCompoundIndex == MAX_SPACE_COMPOUND-1)
            ret = MAX;
        else if (m_spaceCompoundIndex == 0)
            ret = MIN;
        else
            ret = OK;
        m_ptrUpdate->CompoundSpace(SPACE_COMPOUND_ANGLE[m_spaceCompoundIndex], ret);
        SetCompoundSpace(m_spaceCompoundIndex);
    }

	// freq compound
    m_freqCompoundCtrl = FALSE;// ptrParaItem->d2.freqCompoundIndex;//TRUE; //FALSE
	m_freqCompoundCtrlBak = m_freqCompoundCtrl;
    if (m_freqCompoundCtrl)
        ret = MAX;
    else
        ret = MIN;
    m_ptrUpdate->CompoundFreq(m_freqCompoundCtrl, ret);
    SetCompoundFreq(m_freqCompoundCtrl);

    if(m_freqCompoundCtrl)
        m_freqCompoundIndex=1; 
    else
        m_freqCompoundIndex=0;

	// steer
	m_steerIndex = ptrParaItem->d2.steerIndex; // index = 3, angle = 0;
#ifdef EMP_430
    m_steerIndex = 3;
#endif
	m_steerIndexbak = ptrParaItem->d2.steerIndex; // index = 3, angle = 0;

    // if (m_steerIndex != 3)
    if(m_spaceCompoundIndex != 0) 
        m_steerIndex = 3; //开启了空间复合不允许偏转
    m_calcPara.steerAngle = STEER_ANGLE[m_steerIndex];

    if (m_steerIndex == 0)
		ret = MIN;
	else if (m_steerIndex == (MAX_STEER - 1))
		ret = MAX;
	else
		ret = OK;
	m_ptrUpdate->Steer(STEER_ANGLE[m_steerIndex], ret);


    // Tp-view
    m_tpViewIndex = 0;
    TpViewPara(m_tpViewIndex);

    // EFVI
    m_efviCtrl = FALSE;
    EFVIViewPara(m_efviCtrl);

    //test
    m_data = m_thiFreq[m_harmonicFreqIndex];

    m_polarity = ptrParaItem->d2.polarity;
    if (!m_polarity)
        ret = MIN;
    else
        ret = MAX;
    SetPolarity(m_polarity, ret);
}

void Img2D::InitProbe2DOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    InitProbe2DOptimizePara(ptrPara, ptrParaItem);
	///> init class member and calc parameter
	InitProbeCalc2D();
}
void Img2D::InitProbe2D(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    ASSERT((ptrPara!=NULL) && (ptrParaItem != NULL));

	PRINTF("begin init 2D\n");
	int dots = IMG_H * Calc2D::INIT_SCALE;
	m_calcPara.depthDots = dots;	
	m_ptrDscPara->dcaSamplePoints = dots;
	
	///> set dsc para
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        m_ptrDscPara->dcaProbeR =(float)(m_projectCalcPara.probeR / 100.0);
        m_ptrDscPara->dcaProbeWidth = m_projectCalcPara.probeAngle;
    }
    else
    {
        m_ptrDscPara->dcaProbeWidth = ptrPara->width;
        m_ptrDscPara->dcaProbeR =(float)(ptrPara->r / 100.0);
    }

#else
        m_ptrDscPara->dcaProbeWidth = ptrPara->width;
        m_ptrDscPara->dcaProbeR =(float)(ptrPara->r / 100.0);
#endif  
  PRINTF("(((((((((((((((((((((((((((((((((((((((((((99init probe 2d: r = %d\n", ptrPara->r);

    m_type = ptrPara->type;
	m_ptrDscPara->dcaProbeType = ReviseProbeType(ptrPara->type);
    if ((ptrPara->type == 'P') || (ptrPara->type == 'p'))
        m_ptrCalc->CalcPhaseCtrl(TRUE);
    else
        m_ptrCalc->CalcPhaseCtrl(FALSE);

    if (m_ptrDsc != NULL)
    {
        m_ptrDsc->UpdateProbeWidth();
        m_ptrDsc->UpdateTpViewStatus();
    }

	///> set update para
	//m_ptrUpdate->ProbePara(ptrPara->type, ptrPara->lines, ptrPara->width, ptrPara->r);

	///> set class member参数初始化一部分来自读取的探头参数，一部分来自于用户选择的检查项目
	m_freqRange[0] = ptrPara->freqRange[0]; 	
	m_freqRange[1] = ptrPara->freqRange[1];
	m_vecFreqRange = ptrPara->vecFreqRange;

    //harmonic freq
    m_thiFreqSum = 0;
    for (int i = 0; i < ProbeSocket::MAX_HARMONIC_FREQ; i ++)
    {
        if (ptrPara->thiFreq[i] != 0)
        {
            m_thiFreq[i] = ptrPara->thiFreq[i];
			m_thiFreqSum ++;
		}
	}

	m_depthMax = ptrPara->depth;
	m_lines = ptrPara->lines;

    if ((ptrPara->type == 'P') || (ptrPara->type == 'p'))
    {
        m_indexScanLines = ptrParaItem->d2.scanline;

        if(SCAN_LINES_PHASE_PROBE[m_indexScanLines] > m_lines)
            m_indexScanLines = MAX_SCALE_INDEX - 1;

        SetScanLines(m_indexScanLines);
    }
    else
    {
        m_indexScanLines = 3;
        m_calcPara.probeLines = ptrPara->lines;
    }
        EKnobReturn ret = OK;
        if (m_indexScanLines == 0) 
            ret = MIN;
        else if (m_indexScanLines == (MAX_SCAN_LINES - 1))
            ret = MAX;
        else 
            ret = OK;
	///> draw
	m_ptrUpdate->ScanLines(m_indexScanLines, ret);
    ImgCfm::GetInstance()->ScanLinesSetting(m_lines);

	///> set calc parameter
	m_calcPara.probeArray = ptrPara->array;

#ifdef EMP_PROJECT
 if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        ProbeSocket::ProbePara para;
        ProbeMan::GetInstance()->GetCurProbe(para);

        if((strcmp(para.model, "30P16N") ==0)||(strcmp(para.model, "50P10N")==0))
        {

            m_calcPara.probeWidth = ptrPara->width;
            m_calcPara.probeWidthPhase = m_projectCalcPara.probeAngle;
        }
        else
        {
            m_calcPara.probeWidthPhase = ptrPara->widthPhase;
            m_calcPara.probeWidth = m_projectCalcPara.probeAngle;
        }
        m_calcPara.probeR = m_projectCalcPara.probeR;

    }	
    else
    {
        m_calcPara.probeWidthPhase = ptrPara->widthPhase;
        m_calcPara.probeWidth = ptrPara->width;
        m_calcPara.probeR = ptrPara->r;
    }
#else
	m_calcPara.probeWidth = ptrPara->width;
	m_calcPara.probeR = ptrPara->r;
	m_calcPara.probeWidthPhase = ptrPara->widthPhase;
#endif
	m_calcPara.depthMax = ptrPara->depth;
    //set extendedAngle
#ifdef EMP_355
    m_calcPara.extendedAngle = ptrPara->width + 1800;//1000;//8800  为了解决在腔体探头下打开EFVI功能时角度变成和凸阵探头的角度一样的问题。改成了在原来的物理宽度上加上18；不超过180即可。
#else
    m_calcPara.extendedAngle = ptrPara->width + 1000;//8800  为了解决在腔体探头下打开EFVI功能时角度变成和凸阵探头的角度一样的问题。改成了在原来的物理宽度上加上10；不超过180即可。
#endif

	///> read from config file
	// depth
	m_imgScaleIndex = ptrParaItem->d2.imgScale;
	m_depth =(int)(m_depthMax * 10.0 /(float)IMG_SCALE[m_imgScaleIndex] + 0.5);;
	m_calcPara.imgScaleIndex = m_imgScaleIndex;
	m_calcPara.imgScale = IMG_SCALE[m_imgScaleIndex];
	m_calcPara.depth = m_depth;
	m_ptrUpdate->Depth(m_depth);

    InitProbe2DOptimizePara(ptrPara, ptrParaItem);
    if(ModeStatus::IsPureColorMode())
    {
        if ((ptrPara->type == 'P') || (ptrPara->type == 'p'))
            m_spaceCompoundIndexBak = 0;
        else
            m_spaceCompoundIndexBak = ptrParaItem->d2.spaceCompoundIndex;
    }
    else
        m_spaceCompoundIndexBak = ptrParaItem->d2.spaceCompoundIndex; 

 	///> init class member and calc parameter
	PrintImgPara();
	PrintCalcPara();
	InitProbeCalc2D();
}

void Img2D::InitProbeMOptimizePara(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    ASSERT((ptrPara!=NULL) && (ptrParaItem != NULL));

	m_gainM = ptrParaItem->d2.gainM;
	m_ptrUpdate->GainM(m_gainM);
}
void Img2D::InitProbeMOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    ASSERT((ptrPara!=NULL) && (ptrParaItem != NULL));
    InitProbeMOptimizePara(ptrPara, ptrParaItem);

	InitProbeCalcM();
}
void Img2D::InitProbeM(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
	// M mode
	m_mLine = 0;
	m_calcPara.mLine = m_mLine;

	m_mSpeedIndex = 0;
	m_calcPara.mSpeedIndex = m_mSpeedIndex;
	m_ptrCalc->CalcMSpeed(); // used for update MSpeed
	m_ptrUpdate->MSpeed(m_mSpeedIndex, MIN);
	
    InitProbeMOptimizePara(ptrPara, ptrParaItem);	
	InitProbeCalcM();
}

void Img2D::GetCurPara(ExamItem::ParaItem* ptrParaItem)
{
	ptrParaItem->common.MBP = m_mbpIndex;	
	ptrParaItem->common.powerIndex = m_soundPowerIndex;	
	ptrParaItem->d2.freqIndex = m_freqIndex;
	ptrParaItem->d2.imgScale = m_imgScaleIndex;	
	ptrParaItem->d2.gain2D = m_gain2D;	
	ptrParaItem->d2.focSum = m_focSum;	
	ptrParaItem->d2.focPosIndex = m_focPosIndex;	
	ptrParaItem->d2.scanAngle = m_scanAngleIndex;	
	ptrParaItem->d2.dynamicRange = m_dynamicRangeIndex;	
	ptrParaItem->d2.lineDensity = m_lineDensityIndex;	
	ptrParaItem->d2.steerIndex = m_steerIndex;	
	ptrParaItem->d2.AGC = m_agcIndex;	
	ptrParaItem->d2.edgeEnhance = m_edgeEnhanceIndex;	
	ptrParaItem->d2.harmonic = m_harmonic;	
    ptrParaItem->d2.TSI = m_tsiIndex;	
    if(m_gainM > 100)
        m_gainM = 100;
    ptrParaItem->d2.gainM = m_gainM;	
    ptrParaItem->d2.spaceCompoundIndex=m_spaceCompoundIndexBak;
    ptrParaItem->d2.freqCompoundIndex=m_freqCompoundIndex;
    ptrParaItem->d2.thiFreqIndex = m_harmonicFreqIndex;
    ptrParaItem->d2.scanline = m_indexScanLines;
	ptrParaItem->d2.polarity = m_polarity;
    PRINTF("m_freqCompoundindex=%d,m_space=%d\n",m_spaceCompoundIndexBak, m_freqCompoundIndex);
    PrintCalcPara();
}

/*
 * @brief enable probe emit, only call once after unfreeze when power off
 */
void Img2D::EnableEmit()
{
	m_ptrCalc->CalcEnableEmit();
}
/*
 * @brief adjust value of freq, acording to fixed step.
 *
 * @para oper operation to ChangeFreq, noly two choice, ADD or SUB
 * @retval FALSE reach the max or min value,do nothing.
 * @retval TRUE operation oper is done.
 *
 */
EKnobReturn Img2D::ChangeFreq(EKnobOper oper)
{
	int step = 1;
	int size = m_vecFreqRange.size();
	int index = m_freqIndex;
    ModeStatus s;
    int mode = s.GetScanMode();

    ///> change freq
    if (oper == ADD)
    {
        if ((index + step) <= (size - 1))
        {
            index += step;
        }
        else
        {
#if (defined(EMP_460) || defined(EMP_355) || defined(EMP_322))
            IoCtrl io;
            io.Freeze();
            usleep(50000);
            ChangeHarmonicStatus(ROTATE); 
            usleep(44000);
            io.Unfreeze();
            return (OK);
#else
            PRINTF("Img2D: Freq reach max value!\n");
            return MAX;
#endif
        }
    }
    else if (oper == SUB)
    {
        if ((index - step) >= 0)
        {
            index -= step;
        }
        else
        {
            PRINTF("Img2D: Freq reach min value!\n");
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

	m_freqIndex = index;
	m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
	m_freq.receive = m_vecFreqRange[m_freqIndex].receive;
	PRINTF("+++++++++++++++++++++++total size = %d, freq index = %d, emit = %d, receive = %d\n", size, m_freqIndex, m_freq.emit, m_freq.receive);

	EKnobReturn ret = OK;
	if (m_freqIndex == 0) 
		ret = MIN;
	else if (m_freqIndex == (size-1))
	{
#if (defined(EMP_460) || defined(EMP_355) || defined(EMP_322))
		ret = OK;
#else
		ret = MAX;
#endif
	}
	else 
		ret = OK;

#if (defined(EMP_340) || defined(EMP_355))
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if((strcmp(para.model, "10L25J")) == 0)
    {
        if(m_freq.receive == 180) //9MHz
        {
            m_soundPowerIndex = 9;
        }
        else if(m_freq.receive == 200) //10MHz
        {
            m_soundPowerIndex = 7;
        }
        else if(m_freq.receive == 240) //12MHz
        {
            m_soundPowerIndex = 4;
        }
        int soundPower = POWER_DATA[m_soundPowerIndex];
        m_calcPara.power = soundPower;

        ///> calc focus pulse width
        CalcFocPulse();			

        ///> draw
        ret = OK;
        m_ptrUpdate->SoundPower(soundPower, ret);
    }
#endif

    //	    FreezeMode::GetInstance()->PressFreeze();
    IoCtrl io;
    io.Freeze();
    usleep(50000);
    // }

    DefaultIndexBandPassFilterBaseFreq(m_curProbeIndex, m_freqIndex);

    Freq(m_freq, ret);
    //	if (ModeStatus::IsFreezeMode()) //add 9.2
    //	{
    usleep(44000);
    io.Unfreeze();
    //FreezeMode::GetInstance()->PressUnFreeze();

    m_ptrCalc->CalcFilter();
#if 0
    // off harmonic when freq larger than 2.5MHz
    if ((m_freq.emit > MAX_HARMONIC_FREQ) && (m_harmonic))
    {
        m_harmonic = FALSE; 
        // send harmonic status to imging system
        m_calcPara.harmonic = m_harmonic;
        CalcFilter();

        ///> draw
        m_ptrUpdate->Harmonic(m_harmonic, MIN);
    }
#endif

	return (ret);
}

/*
 * @brief change depth, depth scale is from 0.8-4.0
 */
EKnobReturn Img2D::ChangeDepth(EKnobOper oper)
{
	///> change image scale index
	int step = 1;
	int index = m_imgScaleIndex;
#if 0
	int interval = m_ctDepth.End();
	m_ctDepth.Begin();
	step += m_stepDepth;

	if (interval < 300000)
	{
		m_stepDepth ++;
		return ERROR;
	}
	else
	{
		m_stepDepth = 0;
	}
#endif

	if (oper == ADD)
	{
		if (index == (MAX_SCALE_INDEX-1))
		{
			PRINTF("Img2D: depth reach min value!\n");
			return (MAX);
		}
		else
		{
			index += step;
			if (index > (MAX_SCALE_INDEX - 1))
				index = MAX_SCALE_INDEX - 1;
		}
	}
	else if (oper == SUB)
	{
		if (index == 0)
		{
			PRINTF("Img2D: depth reach max value!\n");
			return (MIN);
		}
		else
		{
			index -= step;
			if (index < 0)
				index = 0;
		}
	}
    else
    {
        return ERROR;
    }

	m_imgScaleIndex = index;
	
	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_SCALE_INDEX - 1))
		ret = MAX;
    else 
        ret = OK;
#if (defined(EMP_440)||defined(EMP_161)||defined(EMP_360))
    IoCtrl io;
    io.Freeze();
    usleep(100000);
#endif
    Depth(m_imgScaleIndex);
#if (defined(EMP_440)||defined(EMP_161)||defined(EMP_360))
    usleep(84000);
    io.Unfreeze();
#endif
    return (ret);	
}

/*
 * @brief change tgc curve
 *
 * @para tgcY[] 0-7 is section tgc value, 8 is the total gain value
 */
void Img2D::ChangeTgc2D(int tgcY[])
{
	memcpy(m_tgc, tgcY, 8*sizeof(int));

	///> calc tgc
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);
}

#ifdef TRANSDUCER
void Img2D::ChangeTransducer(int transducer)
{
    SysGeneralSetting *sysGel = new SysGeneralSetting;
    int press_adjust = sysGel->GetPressAjust();
    int Correctionvalue;
    Correctionvalue = 100 + press_adjust; 

    PRINTF("-----------correctionvalue=%d",Correctionvalue);

    int TransducerGain = 0;
    if(transducer <= 26)
        TransducerGain = 0;
    else
    {
        TransducerGain=(transducer-26)*0.4507+Correctionvalue-100;
    }
  if (TransducerGain < 0)
            TransducerGain = 0;

    m_transducer=TransducerGain;

    m_ptrUpdate->Transducer(m_transducer);
}
#endif

void Img2D::ChangeGain2D(int gain)
{
	//m_gain2D = gain; //2012.08.21为了解决调节GAIN的时候界面上显示范围为0-255的问题
	 m_gain2D = gain/255.0*100;
	m_ptrUpdate->Gain2D(m_gain2D);

	///> calc tgc
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);
}

EKnobReturn Img2D::ChangeGain2D(EKnobOper oper)
{
	int gain = m_gain2D;
    int step = 1;
	float tu = m_ctGain2D.End();
	m_ctGain2D.Begin();

	if(tu < INTERVAL)
	{
		if(m_tGain2D >= 3)
			step = 5;
		else
			m_tGain2D ++;
	}
	else
	{
		m_tGain2D = 0;
	}

	if (oper == ADD)
	{
		if(gain == 100)
		{
			PRINTF("Img2D: 2D gian max value!\n");
			return (MAX);
		}
		else
		{
			gain += step;
			if (gain > 100)
			{
				gain = 100;
			}
		}
	}
	else if (oper == SUB)
	{
		if(gain < 0)
		{
			PRINTF("Img2D: 2D gian reach min value!\n");
			return (MIN);
		}
		else
		{
			gain -= step;
			if (gain < 0)
			{
				gain = 0;
			}
		}
	}
    else
    {
        return ERROR;
    }

	m_gain2D = gain;
	m_ptrUpdate->Gain2D(m_gain2D);

	///> calc tgc
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);

	EKnobReturn ret = OK;
	if (gain == 0) 
		ret = MIN;
	else if (gain == MAX_GAIN_2D)
		ret = MAX;
	else 
		ret = OK;

	return (ret);	
}

EKnobReturn Img2D::ChangeFocSum(EKnobOper oper)
{
	int focSum = m_focSum;

	///> change focus sum
	if (oper == ADD)
	{
		if (focSum < MAX_FOCUS)
		{
			focSum += 1;
		}
		else
		{
			PRINTF("Img2D: focus sum reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (focSum > 1)
		{
			focSum -= 1;
		}
		else
		{
			PRINTF("Img2D: focus sum reach min value!\n");
			return (MIN);
		}
	}
    else if (oper == ROTATE)
    {
        if(focSum < MAX_FOCUS)
            focSum += 1;
        else 
            focSum = 1;
    }
    else
    {
        return ERROR;
    }

	m_focSum = focSum;
	m_calcPara.focSum = focSum;
	m_ptrCalc->CalcFocSum();

	///> change focus position
	ResetFocPos(m_focSum, m_focPosIndex);
	GetFocPos();
	m_ptrCalc->CalcFocPos();

	///> draw
	m_ptrUpdate->FocInfo(m_focSum, m_focPos);

	EKnobReturn ret = OK;
	if (focSum == 1) 
		ret = MIN;
	else if (focSum == MAX_FOCUS)
		ret = MAX;
	else 
		ret = OK;
		
	return (ret);	
}

bool Img2D::SetFocSum(int sum)
{
    if (sum > MAX_FOCUS)
    {
        return FALSE;
    }

	m_focSum = sum;
	m_calcPara.focSum = m_focSum;
	m_ptrCalc->CalcFocSum();

	///> change focus position
	ResetFocPos(m_focSum, m_focPosIndex);
	GetFocPos();
	m_ptrCalc->CalcFocPos();

	///> draw
	m_ptrUpdate->FocInfo(m_focSum, m_focPos);

    return TRUE;
}


EKnobReturn Img2D::ChangeFocPos(EKnobOper oper)
{
    int indexB, indexE;
    GetFocPosIndexRange(indexB, indexE);

	///> change focus positin index
	int index = m_focPosIndex;

	if (oper == ADD)
	{
		if (index < indexE)
		{
			index += 1;
		}
		else
		{
			PRINTF("Img2D: foc pos reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > indexB)
		{
			index -= 1;
		}
		else
		{
			PRINTF("Img2D: foc pos reach min value!\n");
			return (MIN);
		}
	}
    else
    {
        return ERROR;
    }

	m_focPosIndex = index;

	///> get focus position
	GetFocPos();
	m_ptrCalc->CalcFocPos();

	///> draw
    PRINTF("change focPos: range(%d, %d), current = %d, focPos = %dmm\n", indexB, indexE, m_focPosIndex, m_focPos[0]);
	m_ptrUpdate->FocInfo(m_focSum, m_focPos);
	
	EKnobReturn ret = OK;
	if (index == indexB) 
		ret = MIN;
	else if (index == indexE)
		ret = MAX;
	else 
		ret = OK;
		
	return (ret);	
}

bool Img2D::SetFocPos(int posIndex)
{
	m_focPosIndex = posIndex;

	///> get focus position
	GetFocPos();
	m_ptrCalc->CalcFocPos();

	///> draw
	m_ptrUpdate->FocInfo(m_focSum, m_focPos);
		
	return (TRUE);	
}

EKnobReturn Img2D::SetScanAngle(int index)
{
	///> calc para
    m_calcPara.angleIndex = index;
    PRINTF("angleindex = %d %d\n", index, m_calcPara.angleIndex);
    m_ptrCalc->CalcScanRange();

    ///> send to dsc 
    SetDscScanLine();

	///> draw 
    return DisplayScanAngle();
}

/*
 * @brief change scan angle, total 8 levels, level range(1-8)
 */
EKnobReturn Img2D::ChangeScanAngle(EKnobOper oper)
{
	///> change angle
    int index = m_scanAngleIndex;
    if (oper == ROTATE)
    {
        if(index < (MAX_ANGLE - 1))
        {
            index += 1;
        }
        else 
        {
            index = 0;
        }
    }
    else if (oper == SUB)
    {
        if (index < (MAX_ANGLE - 1))
        {
            index += 1;
        }
        else
        {
            PRINTF("Img2D: scan angle reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == ADD)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            PRINTF("Img2D: scan angle reach min value!\n");
            return (MIN);
        }
    }		
    else
    {
        return ERROR;
    }

    m_scanAngleIndex = index;

	return (SetScanAngle(m_scanAngleIndex));
}

/*
 * @brief change phase probe scan lines, total 4 levels, 0:64, 1:128; 2:192, 3:256
 */
EKnobReturn Img2D::ChangeScanLines(EKnobOper oper)
{
    int index = m_indexScanLines;
    ImgCfm::GetInstance()->ScanLinesSetting(m_lines);

    if (oper == ROTATE)
    {
        if(index < (MAX_SCAN_LINES - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }

    }
       else if (oper == ADD)
    {
        if (index < (MAX_SCAN_LINES - 1))
        {
            index += 1;
        }
        else
        {
            PRINTF("Img2D: scan lines reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            PRINTF("Img2D: scan lines reach min value!\n");
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    if(ModeStatus::IsColorMode())
    {
        ImgCfm::GetInstance()->ClearBox();
    }
    // clear M image
    if(ModeStatus::IsMMode()) 
    {
        ClearMLine();
    }
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
        Zoom::GetInstance()->ClearLocalZoom();

    m_indexScanLines = index;
    SetScanLines(m_indexScanLines);

    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
    {
        Zoom::GetInstance()->LocalZoomInit();
        Zoom::GetInstance()->RedrawLocalZoom();
    }

    //ImgPw::GetInstance()->ResetSv();
    if(ModeStatus::IsColorMode())
    {
        //show 2D menu 
        int menuIndex;
        MenuArea::GetInstance()->GetMenuIndex(menuIndex);
        if(menuIndex == 0) 
            ImgCfm::GetInstance()->SetScanLines();
    }
    
    //zoom
    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
    {
        Zoom::GetInstance()->LocalZoomInit();
        Zoom::GetInstance()->RedrawLocalZoom();
    }
    //redraw m line
    if(ModeStatus::IsMMode()) 
    {
        ResetMLine();
    }

    EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_SCAN_LINES - 1))
		ret = MAX;
	else 
		ret = OK;
	
	///> draw
	m_ptrUpdate->ScanLines(m_indexScanLines, ret);
    
    return ret;
}

EKnobReturn Img2D::ChangeDynamicRange(EKnobOper oper)
{
	///> change dynamic range index
    int index = m_dynamicRangeIndex;

    if (oper == ROTATE)
    {
        if(index < (Calc2D::MAX_DYNAMIC_INDEX - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }

    }
       else if (oper == ADD)
    {
        if (index < (Calc2D::MAX_DYNAMIC_INDEX - 1))
        {
            index += 1;
        }
        else
        {
            PRINTF("Img2D: scan angle reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            PRINTF("Img2D: scan angle reach min value!\n");
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

	m_dynamicRangeIndex = index;

	///> clac dynamic range
	m_ptrCalc->CalcDynamicRange(index);	

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (Calc2D::MAX_DYNAMIC_INDEX - 1))
		ret = MAX;
	else 
		ret = OK;
	
	///> draw
	m_ptrUpdate->DynamicRange(DYNAMIC_DATA_D[index], ret);

	return ret;
}

/*
 * @brief only mbp = 1(single), line density can change to low density
 */
EKnobReturn Img2D::ChangeLineDensity(EKnobOper oper)
{
	///> change density
    int index = m_lineDensityIndex;
    if (oper == ROTATE)
    {
        if(index < (MAX_LINE_DENSITY - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }
    }
      else if (oper == ADD)
    {
        if (index < (MAX_LINE_DENSITY - 1))
        {
            index += 1;
        }
        else
        {
            PRINTF("Img2D: line density reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            PRINTF("Img2D: line density reach min value!\n");
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_lineDensityIndex = index;
    EKnobReturn ret = OK;
    if (index == 0) 
        ret = MIN;
    else if (index == (MAX_LINE_DENSITY - 1))
        ret = MAX;
    else 
        ret = OK;

    LineDensity(m_lineDensityIndex, ret);

    return (ret);				
}

//void Img2D::SetLineDensity(int index)
void Img2D::SetLineDensity(int index, bool update)
{
    ASSERT((index == 0) || (index == 1));

	m_lineDensityIndexBak = m_lineDensityIndex;
	m_lineDensityIndex = index;
	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_LINE_DENSITY - 1))
		ret = MAX;
	else 
		ret = OK;

	LineDensity(m_lineDensityIndex, ret, update);
}

void Img2D::RecoverLineDensity()
{
    //ASSERT((index == 0) || (index == 1));

	m_lineDensityIndex = m_lineDensityIndexBak;
	EKnobReturn ret = OK;
	if (m_lineDensityIndex == 0) 
		ret = MIN;
	else if (m_lineDensityIndex == (MAX_LINE_DENSITY - 1))
		ret = MAX;
	else 
		ret = OK;

	LineDensity(m_lineDensityIndex, ret, false);
}


EKnobReturn Img2D::ChangeEdgeEnhance(EKnobOper oper)
{
	///> change edge index
	int index = m_edgeEnhanceIndex;
   if (oper == ROTATE)
    {
        if(index <(MAX_EDGE - 1))
        {
            index += 1;
        }
        else 
        {
            index = 0;
        }
    }

   else if (oper == ADD)
	{
		if (index < (MAX_EDGE-1))
		{
			index += 1;
		}
		else
		{
			PRINTF("Img2D: edge enhancement reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > 0)
		{
			index -= 1;
		}
		else
		{
			PRINTF("Img2D: edge enhancement reach min value!\n");
			return (MIN);
		}
	}
    else
    {
        return ERROR;
    }

	m_edgeEnhanceIndex = index;

	///> calc edge enhance 
	m_calcPara.edgeEnhance = m_edgeEnhanceIndex;
	m_ptrDscPara->dca2DIPAttrs.ipaEdgeEhn = m_edgeEnhanceIndex;

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_EDGE - 1))
		ret = MAX;
	else 
		ret = OK;

	///> draw
	m_ptrUpdate->EdgeEnhance(index, ret);
		
	return (ret);			
}

EKnobReturn Img2D::ChangeSoundPower(EKnobOper oper)
{
	///> change sound power
	int index = m_soundPowerIndex;
    if (oper == ROTATE)
    {
        if (index < (MAX_POWER_INDEX -1))
        {
            index ++;
        }
        else 
        {
            index = 0;
        }
    }
    else if (oper == ADD)
	{
		if (index < (MAX_POWER_INDEX - 1))
		{
			index += 1;
		}
		else
		{
			PRINTF("Img2D: edge enhancement reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > 0)
		{
			index -= 1;
		}
		else
		{
			PRINTF("Img2D: edge enhancement reach min value!\n");
			return (MIN);
		}
	}	
    else
    {
        return ERROR;
    }
	
	m_soundPowerIndex = index;

	int soundPower = POWER_DATA[index];
	m_calcPara.power = soundPower;

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_POWER_INDEX - 1))
		ret = MAX;
	else 
		ret = OK;

	bool isChange = FALSE;
	///> calc focus pulse width
	isChange = CalcFocPulse();
	
	///> draw
	m_ptrUpdate->SoundPower(soundPower, ret);
	
	if (!isChange)
		ChangeSoundPower(oper);

	return (ret);	
}

EKnobReturn Img2D::ChangeHarmonic(EKnobOper oper)
{
	bool on = m_harmonic;

	///> change harmonic
	if (oper == ADD)
	{
		if (on == FALSE)
		{
            on = TRUE;
#if 0
            if(m_freq.emit > MAX_HARMONIC_FREQ)
            {
                HintArea::GetInstance()->UpdateHint(_("[Harmonic]: Invalid when frequency larger than 2.5MHz."), 1);
                return MIN;
            }
            else
            {
                on = TRUE;
            }
#endif
		}
		else
		{
			PRINTF("Img2D: harmonic reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (on == TRUE)
		{
			on = FALSE;
		}
		else
		{
			PRINTF("Img2D: harmonic reach min value!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (on)
        {
			on = FALSE;
        }
		else 
        {
            on = TRUE;
#if 0
            if(m_freq.emit > MAX_HARMONIC_FREQ)
            {
                HintArea::GetInstance()->UpdateHint(_("[Harmonic]: Invalid when frequency larger than 2.5MHz."), 1);
                return MIN;
            }
            else
            {
                on = TRUE;
            }
#endif
        }
	}
    else
    {
        return ERROR;
    }

	m_harmonic = on;

    // change freq when current freq is not suit to harmonic
	EKnobReturn ret = OK;
    if (m_harmonic && (m_freq.emit > MAX_HARMONIC_FREQ))
    {
        m_freqIndex = 0;
        m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
        m_freq.receive = m_vecFreqRange[m_freqIndex].receive;
        int size = m_vecFreqRange.size();

        if (m_freqIndex == 0) 
            ret = MIN;
        else if (m_freqIndex == (size-1))
            ret = MAX;
        else 
            ret = OK;

        Freq(m_freq, ret);
    }

    // send harmonic status to imging system
    m_calcPara.harmonic = on;
    CalcFilter();
	if (!on) 
		ret = MIN;
	else if (on)
		ret = MAX;
	else 
		ret = OK;

	///> draw
	//m_ptrUpdate->Harmonic(m_harmonic, ret);

	return ret;
}

EKnobReturn Img2D::ChangeHarmonicStatus(EKnobOper oper)
{
	bool on = m_harmonic;
    m_statusTHI = m_harmonic;
    int size;

	///> change harmonic
    if (oper == ADD)
    {
        if (on == FALSE)
        {
            on = TRUE;
        }
        else
        {
            PRINTF("Img2D: Harmonic is on!\n");
            return (MAX);
        }
    }
	else if (oper == SUB)
	{
		if (on == TRUE)
		{
			on = FALSE;
		}
		else
		{
			PRINTF("Img2D: Harmonic is off!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (on)
        {
			on = FALSE;
        }
		else 
        {
            on = TRUE;
        }
    }
    else
    {
        return ERROR;
    }

	m_harmonic = on;
    m_statusTHI = m_harmonic;
    m_calcPara.harmonic = on;

	EKnobReturn ret = OK;

#if (defined(EMP_460) || defined(EMP_355) || defined(EMP_322))
    if(!m_harmonic)
    {
        size = m_vecFreqRange.size();
		m_freqIndex = size-1;
        m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
        m_freq.receive = m_vecFreqRange[m_freqIndex].receive;
        ret = OK;

        m_calcPara.freq.emit = m_freq.emit;
        m_calcPara.freq.receive = m_freq.receive;

       // Freq(m_freq.receive, ret);
        //draw
        m_ptrUpdate->Freq(m_freq.receive, ret);

        ///> calc focus pulse
        m_ptrCalc->CalcFocPulse();

        DefaultIndexBandPassFilterBaseFreq(m_curProbeIndex, m_freqIndex);

        ///> calc filter data(high filter, dynamic filter, demod)
        m_ptrCalc->CalcFilter();

        ///> when freq compound is on
        if (m_freqCompoundCtrl)
        {
            CompoundFreqCtrl(TRUE);
        }
       PRINTF("this is freq: freq= %d", m_freq.receive);
    }
    else
    {
        size = m_thiFreqSum;
		 m_harmonicFreqIndex = 0;
        m_freq.emit = m_thiFreq[m_harmonicFreqIndex];
        m_freq.receive = m_thiFreq[m_harmonicFreqIndex];
        ret = OK;
        
        m_freqEmit = m_freq.emit;
        HarmonicFreq(m_freq, ret);

        DefaultIndexBandPassFilter(m_curProbeIndex, m_harmonicFreqIndex);
        //m_ptrCalc->CalcFilterChangeDepth();
		m_ptrCalc->CalcFilter();
      
        // compound
        if (m_freqCompoundCtrl)
        {
            int index = m_freqIndex;

            m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexPre].emit;
            m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexPre].receive;
            m_ptrCalc->CalcFilterFreqCompound(1);

            m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexNext].emit;
            m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexNext].receive;
            m_ptrCalc->CalcFilterFreqCompound(2);

            m_calcPara.freq.emit = m_vecFreqRange[index].emit;
            m_calcPara.freq.receive = m_vecFreqRange[index].receive;		
        }
       // HarmonicFreq(m_thiFreq, ret);
        m_ptrUpdate->HarmonicFreq(m_thiFreq[m_harmonicFreqIndex], ret);
       PRINTF("this is harfreq: thi= %d, ", m_thiFreq[m_harmonicFreqIndex]);
    }
#else
    if(!m_harmonic)
    {
        size = m_vecFreqRange.size();
        m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
        m_freq.receive = m_vecFreqRange[m_freqIndex].receive;
        if(m_freqIndex == 0)
            ret = MIN;
        else if (m_freqIndex == (size - 1))
            ret = MAX;
        else 
            ret = OK;

        m_calcPara.freq.emit = m_freq.emit;
        m_calcPara.freq.receive = m_freq.receive;

        //draw
        m_ptrUpdate->Freq(m_freq.receive, ret);
#if (defined(EMP_360) || defined(EMP_161) || defined(EMP_440))
        IoCtrl io;
        io.Freeze();
        usleep(150000);
#endif
        ///> calc focus pulse
        m_ptrCalc->CalcFocPulse();

        DefaultIndexBandPassFilterBaseFreq(m_curProbeIndex, m_freqIndex);

        ///> calc filter data(high filter, dynamic filter, demod)
        m_ptrCalc->CalcFilter();

        ///> when freq compound is on
        if (m_freqCompoundCtrl)
        {
            CompoundFreqCtrl(TRUE);
        }

#if (defined(EMP_360) || defined(EMP_161) || defined(EMP_440))
        usleep(150000);
        io.Unfreeze();
#endif
    }
    else
    {
        size = m_thiFreqSum;
        m_freq.emit = m_thiFreq[m_harmonicFreqIndex];
        m_freq.receive = m_thiFreq[m_harmonicFreqIndex];

        if(m_harmonicFreqIndex == 0)
            ret = MIN;
        else if(m_harmonicFreqIndex == m_thiFreqSum - 1)
            ret = MAX;
        else
            ret = OK;

        m_freqEmit = m_freq.emit;


        IoCtrl io;
        io.Freeze();
#ifdef EMP_340
        usleep(90000);
#else
        usleep(200000);
#endif

        HarmonicFreq(m_freq, ret);

        DefaultIndexBandPassFilter(m_curProbeIndex, m_harmonicFreqIndex);
        //m_ptrCalc->CalcFilterChangeDepth();

        m_ptrCalc->CalcFilter();


        // compound
        if (m_freqCompoundCtrl)
        {
            PRINTF("index = %d, pre = %d, next = %d\n", m_freqIndex, m_freqIndexPre, m_freqIndex);
            int index = m_freqIndex;


            m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexPre].emit;
            m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexPre].receive;
            m_ptrCalc->CalcFilterFreqCompound(1);

            m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexNext].emit;
            m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexNext].receive;
            m_ptrCalc->CalcFilterFreqCompound(2);

            m_calcPara.freq.emit = m_vecFreqRange[index].emit;
            m_calcPara.freq.receive = m_vecFreqRange[index].receive;		

        }
#if (defined(EMP_360) || defined(EMP_161) || defined(EMP_440))
        usleep(100000);
#endif
        io.Unfreeze();

        m_ptrUpdate->HarmonicFreq(m_thiFreq[m_harmonicFreqIndex], ret);

    }

    if (!on) 
        ret = MIN;
	else if (on)
        ret = MAX;
    else 
        ret = OK;
#endif
	///> draw
	m_ptrUpdate->HarmonicStatus(m_harmonic, ret);

	return ret;
}

EKnobReturn Img2D::ChangeD2HarmonicFreq(EKnobOper oper)
{
    int step = 1;
    int size = m_thiFreqSum; 
    int index = m_harmonicFreqIndex;
    
    if (oper == ADD)
    {
        if ((index + step) <= (size - 1))
        {
            index += step;
        }
        else
        {
            PRINTF("Img2D: Harmonic Freq reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if ((index - step) >= 0)
        {
            index -= step;
        }
        else
        {
#if (defined(EMP_460) || defined(EMP_355) || defined(EMP_322))
        IoCtrl io;
        io.Freeze();
		usleep(50000);
		ChangeHarmonicStatus(ROTATE);
		usleep(44000);
        io.Unfreeze();
			return (OK);
#else
            PRINTF("Img2D: Harmonic Freq reach min value!\n");
            return (MIN);
#endif
        }
    }
    else
    {
        return ERROR;
    }

    m_harmonicFreqIndex = index;

    EKnobReturn ret = OK;
    if (m_harmonicFreqIndex == 0)
	{
#if (defined(EMP_460) || defined(EMP_355) || defined(EMP_322))
        ret = OK;
#else
	    ret = MIN;
#endif
	}
    else if (m_harmonicFreqIndex == (size-1))
        ret = MAX;
    else 
        ret = OK;


    m_freq.emit = m_thiFreq[index];//m_data; 
    m_freq.receive = m_thiFreq[index];//m_data; 

    m_freqEmit = m_freq.emit;
    
    
    IoCtrl io;
    io.Freeze();
    usleep(70000);
    HarmonicFreq(m_freq, ret);
    // send harmonic status to imging system
    m_calcPara.harmonic = m_harmonic;
#if (defined(EMP_322) || defined(EMP_355))
	    m_fc01 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][0] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][0]) / 20.0);
		m_fc02 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][1] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][1]) / 20.0);
		m_fc03 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][2] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][2]) / 20.0);
		m_fc04 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][3] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][3]) / 20.0);
		m_fc05 = float((ProbeSocket::BAND_PASS_FILTER_FC1[m_curProbeIndex][m_harmonicFreqIndex][4] + ProbeSocket::BAND_PASS_FILTER_FC2[m_curProbeIndex][m_harmonicFreqIndex][4]) / 20.0);
#else 
		m_fc01 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][0] / 10.0);
		m_fc02 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][1] / 10.0);
		m_fc03 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][2] / 10.0);
		m_fc04 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][3] / 10.0);
		m_fc05 = float(ProbeSocket::BAND_PASS_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][4] / 10.0);
#endif

    m_fc1 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][0] / 10.0);
    m_fc2 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][1] / 10.0);
    m_fc3 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][2] / 10.0);
    m_fc4 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][3] / 10.0);
    m_fc5 = float(ProbeSocket::DYNAMIC_FILTER_FC[m_curProbeIndex][m_harmonicFreqIndex][4] / 10.0);
 

    DefaultFreqBandPassFilter(m_curProbeIndex, m_harmonicFreqIndex);
    DefaultFreqDynamicFilter(m_curProbeIndex, m_harmonicFreqIndex);
    DefaultIndexBandPassFilter(m_curProbeIndex, m_harmonicFreqIndex);

    CalcFilter();
    ///> draw
    usleep(74000);
    io.Unfreeze();

    m_ptrUpdate->HarmonicFreq(m_thiFreq[index], ret);

    return ret;
}

EKnobReturn Img2D::ChangeMBP(EKnobOper oper)
{
	//PRINTF("begin change MBP 2D\n":);
    int index = m_mbpIndex;
    if (oper == ADD)
    {
        if (index < (MAX_MBP_INDEX - 1))
        {
            index ++;
        }
        else
        {
            PRINTF("Img2D: MBP reach max value!\n");
            index = 0;	
        }
    }
    else if (oper == SUB)
    {
		if (index > 0)
		{
			index --;
		}
		else
        {
			PRINTF("Img2D: MBP reach min value!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (index < (MAX_MBP_INDEX - 1))
			index ++;
		else
        {
            index = 0;
        }
	}
    else
    {
        return ERROR;
    }

    m_mbpIndex = index;
	m_calcPara.mbp = MBP[m_mbpIndex];
	m_ptrCalc->CalcMBP(MBP[m_mbpIndex]);

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_MBP_INDEX - 1))
		ret = MAX;
	else 
		ret = OK;

	//draw
	m_ptrUpdate->MBP(MBP[m_mbpIndex], ret);
	return (ret);
}

//void Img2D::SetMBP(int mbpIndex)
void Img2D::SetMBP(int mbpIndex, bool update)
{
	m_mbpIndexBak = m_mbpIndex;
	m_mbpIndex = mbpIndex;

	m_calcPara.mbp = MBP[m_mbpIndex];
	m_ptrCalc->CalcMBP(MBP[m_mbpIndex]);
    m_mbpIndex = mbpIndex;

	//drap
	if (update)
	m_ptrUpdate->MBP(MBP[m_mbpIndex], OK);
}


void Img2D::RecoverMBP()
{
	m_mbpIndex = m_mbpIndexBak;
	
	m_calcPara.mbp = MBP[m_mbpIndex];
	m_ptrCalc->CalcMBP(MBP[m_mbpIndex]);

	//drap
	//m_ptrUpdate->MBP(MBP[m_mbpIndex], OK);
}


/*
 * @brief if mbp is multi, set line density to high
 */
int Img2D::ChangeMBP()
{
	PRINTF("begin change MBP 2D\n");
	if (m_mbpIndex < (MAX_MBP_INDEX - 1))
		m_mbpIndex ++;
	else
		m_mbpIndex = 0;

	m_calcPara.mbp = MBP[m_mbpIndex];
	m_ptrCalc->CalcMBP(MBP[m_mbpIndex]);

	//draw
	m_ptrUpdate->MBP(MBP[m_mbpIndex], OK);
	return MBP[m_mbpIndex];
}

EKnobReturn Img2D::ChangeTSI(EKnobOper oper)
{
	int index = m_tsiIndex;

	///> change speed of sound 
	if (oper == ADD)
	{
		if (index < (MAX_TSI - 1))
		{
			index ++;
		}
		else
		{
			PRINTF("Img2D: TSI reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > 0)
		{
			index --;
		}
		else
		{
			PRINTF("Img2D: TSI reach min value!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (index < (MAX_TSI - 1))
			index ++;
		else
			index = 0;
	}
    else
    {
        return ERROR;
    }

	m_tsiIndex = index;

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_TSI - 1))
		ret = MAX;
	else 
		ret = OK;

	Tsi(m_tsiIndex, ret);

	return ret;
}


EKnobReturn Img2D::ChangeAGC(EKnobOper oper)
{
	int index = m_agcIndex;

	///> change speed of sound 
	if (oper == ADD)
	{
		if (index < (MAX_AGC - 1))
		{
			index ++;
		}
		else
		{
			PRINTF("Img2D: AGC reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > 0)
		{
			index --;
		}
		else
		{
			PRINTF("Img2D: AGC reach min value!\n");
			return (MIN);
		}
	}	
    else
    {
        return ERROR;
    }

	m_agcIndex = index;

	// deal with agc change

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_AGC - 1))
		ret = MAX;
	else 
		ret = OK;

	//draw 
	m_ptrUpdate->Agc(index, ret);
		
	return (ret);		
}

EKnobReturn Img2D::ChangeSteer(EKnobOper oper)
{
    if (IsCompoundSpaceOn())
        SetCompoundSpace(0);


        int index = m_steerIndex;

	///> change speed of sound 
	if (oper == ADD)
	{
		if (index < (MAX_STEER - 1))
		{
			index ++;
		}
		else
		{
			PRINTF("Img2D: steer reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > 0)
		{
			index --;
		}
		else
		{
			PRINTF("Img2D: steer reach min value!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (index < (MAX_STEER - 1))
			index ++;
		else
			index = 0;
	}
    else
    {
        return ERROR;
    }

	m_steerIndex = index;
    m_steerIndexbak = m_steerIndex;

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_STEER - 1))
		ret = MAX;
	else 
		ret = OK;

	Steer(STEER_ANGLE[m_steerIndex],  ret);
		
    if (!(IsCompoundSpaceOn()) && (GetSteer() == 0))
    {
        SetCompoundSpace(m_spaceCompoundIndexBak);
    }
	return (ret);		
}

int Img2D::GetSteer()
{
    return STEER_ANGLE[m_steerIndex];
}

void Img2D::SetSteer(int& index)
{
    m_steerIndex = index;
  
    EKnobReturn ret = OK;
    if (index == 0) 
        ret = MIN;
    else if (index == (MAX_STEER - 1))
        ret = MAX;
    else 
        ret = OK;

    Steer(STEER_ANGLE[m_steerIndex], ret);
}

EKnobReturn Img2D::ChangeTpView(EKnobOper oper)
{
	int index = m_tpViewIndex;

	///> change speed of sound 
	if (oper == ADD)
	{
		if (index < (MAX_TP_VIEW - 1))
		{
			index ++;
		}
		else
		{
			PRINTF("Img2D: tp view reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index > 0)
		{
			index --;
		}
		else
		{
			PRINTF("Img2D: tp view reach min value!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (index < (MAX_TP_VIEW - 1))
			index ++;
		else
			index = 0;
	}
    else
    {
        return ERROR;
    }
    m_tpViewIndex = index;

    return TpView(m_tpViewIndex);
}

EKnobReturn Img2D::ChangeEFVI(EKnobOper oper)
{
	bool index = m_efviCtrl;

	///> change speed of sound 
	if (oper == ADD)
	{
		if (!index)
		{
			index = TRUE; 
		}
		else
		{
			PRINTF("Img2D: efvi reach max value!\n");
			return (MAX);
		}
	}
	else if (oper == SUB)
	{
		if (index)
		{
			index = FALSE;
		}
		else
		{
			PRINTF("Img2D: efvi reach min value!\n");
			return (MIN);
		}
	}	
	else if (oper == ROTATE)
	{
		if (index)
			index = FALSE; 
		else
			index = TRUE;
	}
    else
    {
        return ERROR;
    }

    m_efviCtrl = index;

    return EFVIView(m_efviCtrl);
}

///> m func
/*
 * @brief set m line to the center of scan range.
 */
void Img2D::ResetMLine()
{
	int scanRange[2];
    GetBMLineRange(scanRange);

	m_mLine = (scanRange[0] + scanRange[1])/ 2;
	
	MLine(m_mLine);
}

void Img2D::ClearMLine()
{
	m_ptrUpdate->ClearMLine();
}
void Img2D::ReDrawMLine()
{
	m_ptrUpdate->ReDrawMLine();
}

void Img2D::ChangeTgcM(int tgcY[])
{
	memcpy(m_tgc, tgcY, 8*sizeof(int));

	///> calc tgc
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);

    //2D and M have the same tgc digital in fpga 2014.01.05
    //m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
    // calc tgcM
}

void Img2D::ChangeGainM(int gain)
{
	//m_gainM = gain; //同2D模式 2012.08.21 lihuamei
	 m_gainM = gain/255.0 * 100;
	m_ptrUpdate->GainM(m_gainM);

	///> calc tgc
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
    
#ifdef EMP_430
    //2D and M have the same tgc digital in fpga 2014.04.05
	//m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 0, MAX_GAIN_M);
#else
	m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 0, MAX_GAIN_M);
#endif
}

EKnobReturn Img2D::ChangeGainM(EKnobOper oper)
{
	int gain = m_gainM;
    int step = 1;
	float tu = m_ctGainM.End();
	m_ctGainM.Begin();

	if(tu < INTERVAL)
	{
		if(m_tGainM >= 3)
			step = 5;
		else
			m_tGainM ++;
	}
	else
	{
		m_tGainM = 0;
	}

	if (oper == ADD)
	{
		if(gain == 100)
		{
			PRINTF("Img2D: 2D gian max value!\n");
			return (MAX);
		}
		else
		{
			gain += step;
			if (gain > 100)
			{
				gain = 100;
			}
		}
	}
	else if (oper == SUB)
	{
		if(gain < 0)
		{
			PRINTF("Img2D: 2D gian reach min value!\n");
			return (MIN);
		}
		else
		{
			gain -= step;
			if (gain < 0)
			{
				gain = 0;
			}
		}
	}
    else
    {
        return ERROR;
    }

	m_gainM = gain;
	m_ptrUpdate->GainM(m_gainM);

	///> calc tgc
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
#ifdef EMP_430
	//m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
#else
	m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
#endif

	EKnobReturn ret = OK;
	if (gain == 0) 
		ret = MIN;
	else if (gain == 100)
		ret = MAX;
	else 
		ret = OK;


	return (ret);	
}

void Img2D::ReSendGainM(void)
{
	///> calc tgc
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
	m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
}


void Img2D::ReSendGain2D(void)
{
    ///> calc tgc
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);
}


/*
 * @brief change M line acording to oper
 * 
 * @oper operation, ADD or SUB
 *
 * @retval if M line is at the edge of range
 *  TRUE: M line change as you want
 *  FALSE: reach edge of range
 */

bool Img2D::ChangeMLine(int offsetX, int offsetY)
{
	int line = m_mLine;
	const int interval = 5;
	int scanRange[2];

    GetBMLineRange(scanRange);
	line += offsetX;
	if (line > (scanRange[1] - interval))
		line = scanRange[1] - interval;
	if (line < (scanRange[0] + interval))
		line = scanRange[0] + interval;

    //printf("m_mline:%d\n", m_mLine);
	m_mLine = line;
	MLine(line);

	return (TRUE);		
}
/*
 * @brief change M speed acording to oper
 * 
 * @oper operation, ADD or SUB
 *
 * @retval if M speed is at the edge of range
 *  OK: M speed change as you want
 *  MIN: reach edge of min range
 *  MAX: reach edge of max range
 */

EKnobReturn Img2D::ChangeMSpeed(EKnobOper oper)
{
	int index = m_mSpeedIndex;
    if (oper == ROTATE)
    {
        if(index < (MAX_M_SPEED - 1))
        {
            index += 1;
        }
        else
        {
            index = 0;
        }
    }
    else if (oper == ADD)
    {
        if (index < (MAX_M_SPEED - 1))
        {
            index += 1;
        }
        else
        {
            PRINTF("Img2D: M speed reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= 1;
        }
        else
        {
            PRINTF("Img2D:  M speed reach min value!\n");
            return (MIN);
        }
    }	
    else
    {
        return ERROR;
    }

	m_mSpeedIndex = index;

	///> calc
	m_calcPara.mSpeedIndex = index;
	m_ptrCalc->CalcMSpeed();

	EKnobReturn ret = OK;
	if (index == 0) 
		ret = MIN;
	else if (index == (MAX_M_SPEED - 1))
		ret = MAX;
	else 
		ret = OK;

	//draw
	m_ptrUpdate->MSpeed(index, ret);
		
	return (ret);		
}
    
EKnobReturn Img2D::ChangeCompoundSpace(void)
{
   int index = m_spaceCompoundIndex;
   int steerIndex = 3; //0度
    
    if (index < (MAX_SPACE_COMPOUND -1))
        index ++;
    else
        index = 0;

    if ((index != 0) && (GetSteer() != 0))
    {
        SetSteer(steerIndex);
    }

    if ((index == 0) && (m_steerIndexbak != 3))
    {
        SetSteer(m_steerIndexbak);
    }

	m_spaceCompoundIndex = index;
    m_spaceCompoundIndexBak = m_spaceCompoundIndex;
   
    return SetCompoundSpace(index);
}

EKnobReturn Img2D::ChangeCompoundFreq(void)
{
	bool on = m_freqCompoundCtrl;
    if (on)
        on = FALSE;
    else
        on = TRUE;

    m_freqCompoundCtrl = on;
	m_freqCompoundCtrlBak = m_freqCompoundCtrl;

    return SetCompoundFreq(m_freqCompoundCtrl);
} 

EKnobReturn Img2D::ChangeCompoundSpace(EKnobOper oper)
{
    int index = m_spaceCompoundIndex;
    int steerIndex = 3;

    if (oper == ADD)
    {
        if (index < (MAX_SPACE_COMPOUND -1))
            index ++;
        else
            return MAX;
    }
    else if (oper == SUB)
    {
        if (index > 0)
            index --;
        else
            return MIN;
    }
    else
    {
        return ERROR;
    }

    if ((index != 0) && (GetSteer() != 0))
    {
        SetSteer(steerIndex);
    }
    
    // send steer again when close spacecompound 
    if ((index == 0) && (m_steerIndexbak != 3))
    {
        SetSteer(m_steerIndexbak);
    }
    m_spaceCompoundIndexBak = index;

    return SetCompoundSpace(index);
}
EKnobReturn Img2D::ChangeCompoundFreq(EKnobOper oper)
{
	bool on = m_freqCompoundCtrl;
    if (oper == ADD)
    {
        if (!on)
            on = TRUE;
        else
            return MAX;
    }
    else if (oper == SUB)
    {
        if (on)
            on = FALSE;
        else
            return MIN;
    }
    else if (oper == ROTATE)
    {
        if (on)
            on = FALSE;
        else
            on = TRUE;
    }
    else
    {
        return ERROR;
    }

    m_freqCompoundCtrl = on;
    m_freqCompoundCtrlBak = m_freqCompoundCtrl;
 
    if(m_freqCompoundCtrl)
        m_freqCompoundIndex=1; 
    else
        m_freqCompoundIndex=0;
    
    return SetCompoundFreq(m_freqCompoundCtrl);
}

//EKnobReturn Img2D::SetCompoundSpace(int index)
EKnobReturn Img2D::SetCompoundSpace(int index, bool draw)
{   
	m_spaceCompoundIndexBakBak = m_spaceCompoundIndex;
    m_spaceCompoundIndex = index;

    m_calcPara.compoundAngle = SPACE_COMPOUND_ANGLE[index];
    if (m_calcPara.compoundAngle == 0)
        m_calcPara.compoundSpaceCtrl = FALSE;
    else
        m_calcPara.compoundSpaceCtrl = TRUE;

    EKnobReturn ret = OK;
    if (index == (MAX_SPACE_COMPOUND-1))
        ret = MAX;
    else if (index == 0)
        ret = MIN;
    else
        ret = OK;

	if (draw)
	{
		m_ptrUpdate->CompoundSpace(index, ret);
	}
	
	CompoundSpaceCtrl(m_calcPara.compoundSpaceCtrl);

    return ret;
}


EKnobReturn Img2D::RecoverCompoundSpace()
{   
	m_spaceCompoundIndex = m_spaceCompoundIndexBakBak;

    m_calcPara.compoundAngle = SPACE_COMPOUND_ANGLE[m_spaceCompoundIndex];
    if (m_calcPara.compoundAngle == 0)
        m_calcPara.compoundSpaceCtrl = FALSE;
    else
        m_calcPara.compoundSpaceCtrl = TRUE;

    EKnobReturn ret = OK;
    if (m_spaceCompoundIndex == (MAX_SPACE_COMPOUND-1))
        ret = MAX;
    else if (m_spaceCompoundIndex == 0)
        ret = MIN;
    else
        ret = OK;

   // m_ptrUpdate->CompoundSpace(m_spaceCompoundIndex, ret);
    CompoundSpaceCtrl(m_calcPara.compoundSpaceCtrl);

    return ret;
}

//EKnobReturn Img2D::SetCompoundFreq(bool on)
EKnobReturn Img2D::SetCompoundFreq(bool on, bool draw)
{
	m_freqCompoundCtrlBakBak = m_freqCompoundCtrl;
    m_freqCompoundCtrl = on;
    m_calcPara.compoundFreqCtrl = on;	

    EKnobReturn ret;
    if (m_freqCompoundCtrl)
        ret = MAX;
    else
        ret = MIN;

	if (draw)
	{
        m_ptrUpdate->CompoundFreq(m_freqCompoundCtrl, ret);
    }

    CompoundFreqCtrl(m_freqCompoundCtrl);

    return ret;
}


EKnobReturn Img2D::RecoverCompoundFreq()
{
#if 1
    m_freqCompoundCtrl = m_freqCompoundCtrlBakBak;

    m_calcPara.compoundFreqCtrl = m_freqCompoundCtrl;	

    EKnobReturn ret;
    if (m_freqCompoundCtrl)
        ret = MAX;
    else
        ret = MIN;

   // m_ptrUpdate->CompoundFreq(m_freqCompoundCtrl, ret);
    CompoundFreqCtrl(m_freqCompoundCtrl);

    return ret;
#endif
}
bool Img2D::IsCompoundSpaceOn(void)
{
    return m_calcPara.compoundSpaceCtrl;
}
bool Img2D::IsCompoundFreqOn(void)
{
    return m_calcPara.compoundFreqCtrl;
}
EKnobReturn Img2D::ChangePolarity(EKnobOper oper)
{
	bool on = m_polarity;
    if (oper == ADD)
    {
        if (!on)
            on = TRUE;
        else
            return MAX;
    }
    else if (oper == SUB)
    {
        if (on)
            on = FALSE;
        else
            return MIN;
    }
    else if (oper == ROTATE)
    {
        if (on)
            on = FALSE;
        else
            on = TRUE;
    }
    else
    {
        return ERROR;
    }

    m_polarity = on;
    EKnobReturn ret; 
    if (m_polarity)
        ret = MAX;
    else
        ret = MIN;
    SetPolarity(m_polarity, ret);
    
    return ret;
}

void Img2D::Change2DTis()
{
	// Tis
    Set2DTis(m_scanAngleIndex, m_focPos[m_focSum-1], m_freq.receive, POWER_DATA[m_soundPowerIndex], m_imgScaleIndex);
}

void Img2D::SetCfmTis(int dopFreq, int prfIndex, int boxLineBegin, int boxLineEnd)
{
	float ti = m_ptrCalc->GetCfmTis(dopFreq, m_focPos[m_focSum-1], prfIndex, POWER_DATA[m_soundPowerIndex], boxLineBegin, boxLineEnd);

	m_ptrUpdate->TIS(ti);
}

void Img2D::SetPwTis(int dopFreq, int prfIndex)
{
	float ti = m_ptrCalc->GetPwTis(dopFreq, prfIndex);

	m_ptrUpdate->TIS(ti);
}

void Img2D::SetCwLine(int line)
{
    m_calcPara.cwLine = line;
    m_ptrCalc->CalcCwLinesDelay();
}
void Img2D::GetScanRange(int scanRange[2])
{
	m_ptrCalc->GetScanRange(scanRange);
}
void Img2D::GetDisplayScanRange(int scanRange[2])
{
	ModeStatus s;
	FormatM::EFormatM formatM = s.GetFormatM();
	FormatPw::EFormatPw formatPw = s.GetFormatPw();
    int tempRange[2];
	
	if (((ModeStatus::IsMImgMode()) && (formatM == FormatM::BM11_LR))  
			 || ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) && (formatPw == FormatPw::BP11_LR)))
		m_ptrCalc->CalcDisplayScanRange(tempRange, IMG_W / 2);
	else 
		m_ptrCalc->CalcDisplayScanRange(tempRange, IMG_W);

    int mbp = MBP[m_mbpIndex];
    scanRange[0] = tempRange[0] / mbp * mbp;
    scanRange[1] = tempRange[1] / mbp * mbp;
    if ((tempRange[0] % mbp) != 0)
        scanRange[0] = (tempRange[0] / mbp + 1) * mbp;
}


/*
 * @brief get current scale of 2D image
 * @note: image size control by DSC, not by fpga, so scale to fpga should be correct acording to current mode and display format
 *
 * @para scale save scale value retured in it
 */
double Img2D::GetScale2D()
{
    // Special Measure
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
    {
        return m_scale2DSnap;
    }
    // Normal mode
	ModeStatus s;
	ScanMode::EScanMode mode = s.GetScanMode();
    double scale;
    if (m_zoomMeasureCtrl)
    {
        scale = m_scale2DZoom[s.Get2DCurrentB()];
    }
    else
    {
        scale = m_scale2D[s.Get2DCurrentB()];
    }

	FormatM::EFormatM formatM = s.GetFormatM();
	FormatPw::EFormatPw formatPw = s.GetFormatPw();
	
	switch(mode)
	{
		case ScanMode::D2:
		case ScanMode::PW_INIT:
		case ScanMode::CW_INIT:
			if (s.GetFormat2D() == Format2D::B4)
			{
				scale = scale * 2;
			}
			break;

		case ScanMode::M:
		case ScanMode::ANATOMIC_M:
			if (formatM == FormatM::BM11_UD) 
				scale = scale * 2;
			else if(formatM == FormatM::BM21_UD)
				scale = scale * 3 / 2;
			else if(formatM == FormatM::BM12_UD)
				scale = scale * 3;
			break;

		case ScanMode::PW:
		case ScanMode::PW_SIMULT:
		case ScanMode::PWCFM:
		case ScanMode::PWPDI:
		case ScanMode::PWCFM_SIMULT:
		case ScanMode::PWPDI_SIMULT:
		case ScanMode::CW:
		case ScanMode::CWCFM:
		case ScanMode::CWPDI:
			if (formatPw == FormatPw::BP11_UD) 
				scale = scale * 2;
			else if(formatPw == FormatPw::BP21_UD)
				scale = scale * 3 / 2;
			else if(formatPw == FormatPw::BP12_UD)
				scale = scale * 3;
			break;

		case ScanMode::CFM:
		case ScanMode::PDI:
		case ScanMode::PWCFM_INIT:
		case ScanMode::PWPDI_INIT:
        case ScanMode::CFM_VS_2D:
        case ScanMode::PDI_VS_2D:
		case ScanMode::CWCFM_INIT:
		case ScanMode::CWPDI_INIT:
			if (s.GetFormatCfm() == FormatCfm::B4)
			{
				scale = scale * 2;
			}
			break;
		case ScanMode::EFOV:
			{
				ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();
				if (status == ScanMode::VIEW || status == ScanMode::CAPTURE) 
					scale = scale * m_ptrDsc->GetEFOVScale();
			}

		default:
			break;
	}

	return scale;
}

/*
 * @brief 未局部放大且按最大采样点数的原始图像倍率。
 */
double Img2D::GetScale2DInImgHDot()
{
	ModeStatus s;
	double scale = m_scale2D[s.Get2DCurrentB()];
	return scale;
}

/*
 *  @brief get all B scale 
 *
 *  @para scale[4][out] scale[0]~scale[3] is 2D scale of each B, scale[1] is valid only in BB mode, scale[2], scale[3] is valid only in 4B mode
 */
//针对冻结、解冻、调节深度、切换图像区域情况对scale做不同处理zjx
void Img2D::GetAllScale2D(double scale[4])
{
    // special measure mode
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
    {
        for (int j = 0; j < 4; j ++)
        {
            scale[j] = m_scale2DAllSnap[j];
        }
        return;
    }
    
    // normal mode
	int i;
	ModeStatus s;
	ScanMode::EScanMode mode = s.GetScanMode();
	Format2D::EFormat2D format2D = s.GetFormat2D();
	FormatCfm::EFormatCfm formatCfm = s.GetFormatCfm();

	int correct = 1;
		
	if ((mode == ScanMode::D2) && (format2D  == Format2D::B4))
		correct = 2;
	else if (((mode == ScanMode::CFM) || (mode == ScanMode::PDI)) && (formatCfm == FormatCfm::B4))
        correct = 2;
        else 
        correct = 1;
    
    for (i = 0; i < 4; i ++)
    {
        if (m_zoomMeasureCtrl)
        {
               if (ModeStatus::IsUnFreezeMode())
               {
                   scale[i] = m_scale2DZoom[i] * correct;
               }
               else
               {
                  if(Format2D::GetInstance()->GetCurrentB() == Format2D::GetInstance()->GetLastRealB())
                      scale[i] = m_scale2DZoom[i] * correct;
                  else
                  {
                      Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
                      if(format == Format2D::BB)
                      {
                          if(i ==Format2D::GetInstance()->GetCurrentB())
                              scale[i] = m_scale2DZoom[i] * correct;
                          else
                              scale[i] = m_scale2DZoom_bak[i] * correct;
                      }

                      else if(format == Format2D::B4)
                      {
                          int cur = Format2D::GetInstance()->GetCurrentB();
                          int real = Format2D::GetInstance()->GetLastRealB(); 
                          if(((cur -1)== real) &&(real != 3))
                          {
                              if(i == cur)
                                  scale[i] = m_scale2DZoom[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                          }
                          else if(((real -1) == cur) &&(real !=0))
                          {
                              if(i == real)
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom[i] * correct;
                          }
                          else if((real == 3)&&(cur ==0))
                          {
                              if(i == cur)
                                  scale[i] = m_scale2DZoom[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom_bak[i] * correct;

                          }
                          else if((real == 0) &&(cur ==3))
                          {
                              if(i == real)
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom[i] * correct;
                          }

                          else if((real ==0)&&(cur ==2))
                          {
                              if((i == 0)||(i ==3))
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom[i] * correct;
                          }
                          else if((real ==1)&&(cur == 3))
                          {
                              if((i == 0)||(i ==1))
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom[i] * correct;
                          }
                          else if((real ==2)&&(cur == 0))
                          {
                              if((i == 2)||(i ==1))
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom[i] * correct;
                          }
                          else if((real ==3)&&(cur == 1))
                          {
                              if((i == 2)||(i ==3))
                                  scale[i] = m_scale2DZoom_bak[i] * correct;
                              else
                                  scale[i] = m_scale2DZoom[i] * correct;
                          }
                      }
                      else
                      {
                          scale[i] = m_scale2DZoom[i] * correct;
                      }
                  }
               }
        }
        else
        {
            if (ModeStatus::IsUnFreezeMode())
            {
                    scale[i] = m_scale2D[i] * correct;
            }
            else
            {
                if(Format2D::GetInstance()->GetCurrentB() == Format2D::GetInstance()->GetLastRealB())
                    scale[i] = m_scale2D[i] * correct;
                else
                {
                    Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
                    if(format == Format2D::BB)
                    {
                        if(i ==Format2D::GetInstance()->GetCurrentB())
                            scale[i] = m_scale2D[i] * correct;
                        else
                            scale[i] = m_scale2D_bak[i] * correct;
                    }
                    else if(format == Format2D::B4)
                    {
                        int cur = Format2D::GetInstance()->GetCurrentB();
                        int real = Format2D::GetInstance()->GetLastRealB(); 
                        if(((cur -1)== real) &&(real != 3))
                        {
                            if(i == cur)
                                scale[i] = m_scale2D[i] * correct;
                            else
                                scale[i] = m_scale2D_bak[i] * correct;
                        }
                        else if(((real -1) == cur) &&(real !=0))
                        {
                            if(i == real)
                                scale[i] = m_scale2D_bak[i] * correct;
                            else
                                scale[i] = m_scale2D[i] * correct;
                        }
                        else if((real == 3)&&(cur ==0))
                        {
                            if(i == cur)
                                scale[i] = m_scale2D[i] * correct;
                            else
                                scale[i] = m_scale2D_bak[i] * correct;
                        }
                        else if((real == 0) &&(cur ==3))
                        {
                            if(i == real)
                                scale[i] = m_scale2D_bak[i] * correct;
                            else
                                scale[i] = m_scale2D[i] * correct;
                        }
                        else if((real ==0)&&(cur ==2))
                        {
                            if((i == 0)||(i ==3))
                                scale[i] = m_scale2D_bak[i] * correct;
                            else
                                scale[i] = m_scale2D[i] * correct;
                        }
                        else if((real ==1)&&(cur == 3))
                        {
                            if((i == 0)||(i ==1))
                                scale[i] = m_scale2D_bak[i] * correct;
                            else
                                scale[i] = m_scale2D[i] * correct;
                        }
                        else if((real ==2)&&(cur == 0))
                        {
                            if((i == 2)||(i ==1))
                                scale[i] = m_scale2D_bak[i] * correct;
                            else
                                scale[i] = m_scale2D[i] * correct;
                        }
                        else if((real ==3)&&(cur == 1))
                        {
                            if((i == 2)||(i ==3))
                                scale[i] = m_scale2D_bak[i] * correct;
                            else
                                scale[i] = m_scale2D[i] * correct;
                        }
                    }
                    else
                    {
                        scale[i] = m_scale2D[i] * correct;
                    }
                }
            }
        }
    }
}

/*
 * @brief get scale in depth of M image
 *
 * @para scaleDepth scale in depth direction, return value saved in it
 */
double Img2D::GetScaleMDepth()
{
    // Special measure mode
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
    {
        return m_scaleMDepthSnap;
    }

    // normal mode
	ModeStatus s;
	double scale;
    if (m_zoomMeasureCtrl)
    {
        scale = m_scale2DZoom[s.Get2DCurrentB()];
    }
    else
    {
        scale = m_scale2D[s.Get2DCurrentB()];
    }
	FormatM::EFormatM formatM = s.GetFormatM();

	if (formatM == FormatM::BM11_UD) 
		scale = scale * 2;
	else if(formatM == FormatM::BM21_UD)
		scale = scale * 3;
	else if(formatM == FormatM::BM12_UD)
		scale = scale * 3 / 2;

	return scale;
}

/*
 * @brief get scale in time of M image
 *
 * @para scaleTime scale in time direction, return value saved in it.time of each M line scan(mCycle).
 */
double Img2D::GetScaleMTime()
{
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus()) // Special Measure mode
    {
        return m_scaleMTimeSnap;
    }
    else // normal mode
    {
        return (m_ptrCalc->CalcScaleM()); //second
    }
}

double Img2D::GetScaleAnatomicMTime() //s
{
    int range[2];
    m_ptrCalc->GetScanRange(range);

    int maxperiod = m_ptrCalc->CalcMaxPeriod(); //us
    double mCycle = (range[1] - range[0]) * (maxperiod + 20)/ (float)1000000;
    PRINTF("------------get scale anatomic m cycle = %f, m cycle = %f\n", mCycle, GetScaleMTime());
    return mCycle;
}

void Img2D::CalcEmitDelayPw(float focPos)
{
	m_ptrCalc->CalcEmitDelayPw(focPos);
}
void Img2D::CalcEmitDelayCfm(float focPos)
{
	m_ptrCalc->CalcEmitDelayCfm(focPos);
}

void Img2D::CalcReceiveDelayColor(void)
{
    m_ptrCalc->CalcReceiveDelayColor();
}

void Img2D::SetSteerColor(int angle)
{
    m_calcPara.steerAngleColor = angle;
}
/*
 * @brief change tgc curve
 *
 * @para gain[in] total gain
 * @para tgcY[8] tgc value
 * @para setion[in] 0-2D tgc, 1-pw/m tgc, 2-cfm tgc
 */
void Img2D::CalcTgc(int gain, int tgcY[8], int section)
{
	m_ptrCalc->CalcTgc(gain, m_tgc, m_ptrUpdate, section);
}

void Img2D::CalcTgcDigital(int gain, int section, int maxValue)
{
	m_ptrCalc->CalcTgcDigital(gain, section, maxValue);
}

void Img2D::ExitSimult()
{
    m_ptrCalc->CalcMaxPeriod();
}

#if 0
/*
 * @brief change dots in depth direction
 * note: not used now, in 4B mode, dots = IMG_H, but DSC select one data between two or three to restruct image
 *
 * @para dots dots of depth
 */
void Img2D::SetDepthDots(int dots)
{
	m_calcPara.depthDots = dots;	
	m_ptrDscPara->dcaSamplePoints = dots;

	if (m_ptrDsc != NULL)
		m_ptrDsc->SamplePoints();

	///> sample
	m_ptrCalc->CalcSample();

	///> focus pos
	m_ptrCalc->CalcFocPos();

	///> scale
	SetScale2D(m_ptrCalc->CalcScale2D());
}
#endif
/*
 * @brief change scale of different B image when real-time, set srcIndex's scale to destIndex's scale 
 *
 * @para destIndex scale index to be changed, range(0-3)
 * @para srcIndex scale index to be assigned to destIndex's scale, range(0-3)
 */
void Img2D::ChangeSeperateScale(int destIndex, int srcIndex)
{
    m_scale2D_bak[destIndex] = m_scale2D[destIndex];	
    m_scale2D[destIndex] = m_scale2D[srcIndex];	

    if (m_zoomMeasureCtrl)
    {
        m_scale2DZoom_bak[destIndex] = m_scale2DZoom[destIndex];	
        m_scale2DZoom[destIndex] = m_scale2DZoom[srcIndex];	
    }
}

void Img2D::InitSeperateScale(int srcIndex)
{
    PRINTF("srcIndex = %d\n", srcIndex);
    for (int i = 0; i < 4; ++i) 
    {
        m_scale2D[i] = m_scale2D[srcIndex];
        PRINTF("scale2D = %f\n", m_scale2D[i]);

        if (m_zoomMeasureCtrl)
        {
            m_scale2DZoom[i] = m_scale2DZoom[srcIndex];
            PRINTF("scale2DZoom = %f\n", m_scale2DZoom[i]);
        }
    }
}

void Img2D::UpdateSeperateScaleWhenUnfreeze(void)
{
    int curB = Format2D::GetInstance()->GetCurrentB();
    int lastRealB = Format2D::GetInstance()->GetLastRealB();
    ASSERT(curB <= 4);
    ASSERT(lastRealB <= 4);
    if (curB != lastRealB)
    {
       //解决解冻后刻度尺或box有错误的问题 zjx
        ModeStatus s;
        int mode = s.GetScanMode();
        if (mode == ScanMode::D2)
        {
            Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
            if(format == Format2D::BB)
                Format2D::GetInstance()->SwitchBBForFreeze(curB);	
            if(format == Format2D::B4)
                Format2D::GetInstance()->SwitchB4ForFreeze(curB);
        }
        if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
        {
            FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
            if (format == FormatCfm::BB)
                FormatCfm::GetInstance()->SwitchBBForFreeze(curB);	
            if(format == FormatCfm::B4)
                FormatCfm::GetInstance()->SwitchB4ForFreeze(curB, lastRealB);	
        }
    }
}

///> /////////////////////////////////[private func]////////////////////////////
/*
 * @brief calc probe parameter and send to imging system when probe and exam item if changed.
 */
void Img2D::InitProbeCalc2D()
{
	PRINTF("======================PRINTF IMG2D send para to fpga======================\n");
	///> calc tgc
    m_ptrCalc->CalcTgcMaxperiod();
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);

	///> start clock where 2D image is valid, needed to be send only once when power on
	m_ptrCalc->Valid2DImage();

    ///> compound ctrl
    PRINTF("-----------compound: freq = %d, space = %d\n", m_calcPara.compoundFreqCtrl, m_calcPara.compoundSpaceCtrl);
    m_ptrCalc->CalcSpaceCompoundCtrl(m_calcPara.compoundSpaceCtrl);
    m_ptrCalc->CalcFreqCompoundCtrl(m_calcPara.compoundFreqCtrl);

	///> calc sample
	m_ptrCalc->CalcSample();	
	
	///> calc focus pulse width
	CalcFocPulse();
	
	///> calc max_period(PRF)
	m_ptrCalc->CalcMaxPeriod();

	///> calc focus sum and position
	m_ptrCalc->CalcFocSum();
	m_ptrCalc->CalcFocPos();

    ///> calc for extened imaging
    if (m_calcPara.efviCtrl)
    {
        m_ptrCalc->CalcExtendedImaging(FALSE);
    }

    ///> calc emit delay, has been call in 	m_ptrCalc->CalcFocPos()
    //m_ptrCalc->CalcEmitDelay();

	///> calc receive delay
	m_ptrCalc->CalcReceiveDelay();
	
	///> calc scan angle
	m_ptrCalc->CalcScanRange();
	SetDscScanLine();

	///> calc filter 
	CalcFilter();	

	///> calc edge enhancement
	//m_ptrCalc->CalcEdgeEnhance();

	///> calc dynamic range
	m_ptrCalc->CalcDynamicRange(m_dynamicRangeIndex);
#if 0
    ///>calc voltage reference
    int voltage;
    if(m_harmonic)
        voltage = 125;
    else
        voltage = 197;
    m_ptrCalc->CalcVoltageRefer(voltage);
#endif
	///> calc line density
	m_ptrCalc->CalcLineDensity();

	///> calc MBP
	m_ptrCalc->CalcMBP(MBP[m_mbpIndex]);

	///> calc Tsi
	///> calc steer
	///> calc agc
	
	// init DSC
	SetScale2D(m_ptrCalc->CalcScale2D());
}

void Img2D::InitProbeCalcM()
{
	///> calc M speed
	m_ptrCalc->CalcMLine();

	m_ptrCalc->CalcMSpeed();
	
	///> calc tgc
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
	m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
}
/*
 * @brief do somthing when freq is changed, these things is pulse wide, filter file, demod file

 * @para freq pari of real freq*20  (emit-receive)
 */
void Img2D::Freq(ProbeSocket::FreqPair freq, EKnobReturn ret)
{		
	m_calcPara.freq.emit = freq.emit;
	m_calcPara.freq.receive = freq.receive;
	PRINTF("(((((((((((((((((((((Freq: emit = %d, receive = %d\n", m_calcPara.freq.emit, m_calcPara.freq.receive);

	//draw
	m_ptrUpdate->Freq(freq.receive, ret);

	///> calc focus pulse
	m_ptrCalc->CalcFocPulse();

	///> calc filter data(high filter, dynamic filter, demod)
	m_ptrCalc->CalcFilter();

	///> when freq compound is on
	if (m_freqCompoundCtrl)
	{
		CompoundFreqCtrl(TRUE);
	}
}

#ifdef EMP_PROJECT
void Img2D::ChangeFreqForProject(int freq_index)
{
     if(m_harmonic)
    {
        bool on = false;
        m_harmonic = on;
        m_statusTHI = m_harmonic;
        m_calcPara.harmonic = on;
    }
     int size = m_vecFreqRange.size();

     m_freqIndex = freq_index;
	m_freq.emit = m_vecFreqRange[m_freqIndex].emit;
	m_freq.receive = m_vecFreqRange[m_freqIndex].receive;

	EKnobReturn ret = OK;
	if (m_freqIndex == 0) 
		ret = MIN;
	else if (m_freqIndex == (size-1))
	{
		ret = MAX;
	}
	else 
		ret = OK;

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if((strcmp(para.model, "10L25J")) == 0)
    {
        if(m_freq.receive == 180) //9MHz
        {
            m_soundPowerIndex = 9;
        }
        else if(m_freq.receive == 200) //10MHz
        {
            m_soundPowerIndex = 7;
        }
        else if(m_freq.receive == 240) //12MHz
        {
            m_soundPowerIndex = 4;
        }
        int soundPower = POWER_DATA[m_soundPowerIndex];
        m_calcPara.power = soundPower;

        ///> calc focus pulse width
        CalcFocPulse();			

        ///> draw
        ret = OK;
        m_ptrUpdate->SoundPower(soundPower, ret);
    }
    IoCtrl io;
    io.Freeze();
    usleep(50000);
    Freq(m_freq, ret);
    usleep(44000);
    io.Unfreeze();
    m_ptrUpdate->HarmonicStatus(m_harmonic, ret);
}

void Img2D::ChangeHTIForProject(int harmonic_index)
{
    if(!m_harmonic)
    {
        bool on = true;
        m_harmonic = on;
        m_statusTHI = m_harmonic;
        m_calcPara.harmonic = on;
    }

    int size = m_thiFreqSum; 
    EKnobReturn ret = OK;
    m_harmonicFreqIndex = harmonic_index;

    if (m_harmonicFreqIndex == 0)
    {
        ret = MIN;
	}
    else if (m_harmonicFreqIndex == (size-1))
        ret = MAX;
    else 
        ret = OK;


    m_freq.emit = m_thiFreq[harmonic_index];//m_data; 
    m_freq.receive = m_thiFreq[harmonic_index];//m_data; 
    m_freqEmit = m_freq.emit;
    IoCtrl io;
    io.Freeze();
    usleep(70000);
    HarmonicFreq(m_freq, ret);
    m_ptrUpdate->HarmonicFreq(m_thiFreq[harmonic_index], ret);
    usleep(74000);
    io.Unfreeze();
    m_ptrUpdate->HarmonicStatus(m_harmonic, ret);
    printf("harmonic_index = %d, m_thifreq = %d, m_harmonic = %d" , harmonic_index, m_thiFreq[harmonic_index], m_harmonic);
}
#endif

void Img2D::HarmonicFreq(ProbeSocket::FreqPair freq, EKnobReturn ret)
{
	m_calcPara.freq.emit = freq.emit;
	m_calcPara.freq.receive = freq.receive;
	PRINTF("(((((((((((((((((((((Freq: emit = %d, receive = %d\n", m_calcPara.freq.emit, m_calcPara.freq.receive);

	///> calc focus pulse
	m_ptrCalc->CalcFocPulse();
}

void Img2D::Depth(int scaleIndex)
{
	m_depth = (int)(m_depthMax * 10.0 /  (float)IMG_SCALE[scaleIndex] + 0.5);

	m_calcPara.imgScaleIndex = scaleIndex;
	m_calcPara.imgScale = IMG_SCALE[scaleIndex];
	m_calcPara.depth = m_depth;
	
	///> scale
	SetScale2D(m_ptrCalc->CalcScale2D());

	///> calc focus position
	GetFocPos();
	m_ptrCalc->CalcFocPos();
	m_ptrUpdate->FocInfo(m_focSum, m_focPos);

	// calc tgc tgcX redevide by imageScale
	// 一定要先绘制焦点再发送增益，并且增益要尽早发，否则M图像起始几条线异常。
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);

    // m gain
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
    //fpga中处理M和2D 的数字增益用的是一组数据
    if(ModeStatus::IsMImgMode())
        m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);

	m_ptrCalc->Valid2DImage();

	///> calc max period
	m_ptrCalc->CalcMaxPeriod();

	///> calc sample
	m_ptrCalc->CalcSample();

	///> calc receive delay
	m_ptrCalc->CalcReceiveAperture();

	// clear M image
    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)
    {
        ptrDscMan->GetWriteLock();
		m_ptrDsc->ClearMLine();
        ptrDscMan->ReadWriteUnlock();
    }
    m_ptrCalc->CalcExtendedImagingSample();
    
    ///> filter
#if (defined(EMP_360) || defined(EMP_161) || defined(EMP_440))
    CalcFilter();
#else
    //float zero = 0.0;
    // m_ptrCalc->SetZeroToFdDynamicDemod_test(zero);

    m_ptrCalc->CalcFilterChangeDepth();
    // compound
    if (m_freqCompoundCtrl)
    {
        PRINTF("index = %d, pre = %d, next = %d\n", m_freqIndex, m_freqIndexPre, m_freqIndex);
        int index = m_freqIndex;

        m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexPre].emit;
        m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexPre].receive;
        m_ptrCalc->CalcFilterFreqCompound(1);

        m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexNext].emit;
        m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexNext].receive;
        m_ptrCalc->CalcFilterFreqCompound(2);

        m_calcPara.freq.emit = m_vecFreqRange[index].emit;
        m_calcPara.freq.receive = m_vecFreqRange[index].receive;		
    }
#endif

    // clear replay data
    Update2D::SetCineRemoveImg(3);
    Replay::GetInstance()->ClearCurReplayData(); 

	//draw
	m_ptrUpdate->Depth(m_depth);
}

void Img2D::Steer(int angle, EKnobReturn ret)
{
/**
 * description: close change 2D steer
 * lihuamei 2012.07.04
 */
    //set para 
    m_calcPara.tpViewCtrl = FALSE;
    m_calcPara.tpSteer = angle;  //steer for tp
    m_calcPara.steerAngle = angle; //steer for 2D
    m_ptrDscPara->dcaTpView = 0; 
    m_ptrDscPara->dcaSteer = m_calcPara.tpSteer;
    
    //update dsc tpView status
    if (m_ptrDsc != NULL)
        m_ptrDsc->UpdateTpViewStatus();
    
    //calc 2D
    m_ptrCalc->CalcExtendedImaging(m_calcPara.tpViewCtrl);
    m_ptrCalc->CalcEmitDelay();
    m_ptrCalc->CalcReceiveDelay();//21ms

   // m_ptrImgDraw->Set2DSteerAngle(angle);

	// draw
	m_ptrUpdate->Steer(angle, ret);
}

/*
 * @brief reset focus position, due to different focus sum, different count of focus pos.
 */
void Img2D::ResetFocPos(int focSum, int &focPosIndex)
{
    if (m_zoomMeasureCtrl)
    {
        int indexB, indexE;
        GetFocPosIndexRange(indexB, indexE);
        focPosIndex = (indexB + indexE) / 2;
    }
    else
    {
        focPosIndex = FOC_POS_INDEX_MAX[focSum - 1] / 2;
    }
}
/*
 * @brief get real focus position, relative to focus sum, focus pos index, depth
 */
void Img2D::GetFocPos()
{
	int index = m_focPosIndex;
	int focSum = m_focSum;
	int depth = m_depth;
	int focMax = MAX_FOCUS;

	int i;
	switch(focSum)
	{
		case 1: //one focus
			m_focPos[0] = depth * ONE_FOC_POS[index][0] / (float)FOC_DEPTH + 0.5;
			for (i = 1; i < focMax; i ++) ///< set to 0 
			{
				m_focPos[i] = 0;
			}
			break;

		case 2: //two
			for (i = 0; i < 2; i ++)
			{
				m_focPos[i] = depth * TWO_FOC_POS[index][i] / (float)FOC_DEPTH + 0.5;
			}
			for (i = 2; i < focMax; i ++) //set to 0
			{
				m_focPos[i] = 0;
			}
			break;

		case 3: //three
			for (i = 0; i < 3; i ++)
			{
				m_focPos[i] = depth * THREE_FOC_POS[index][i] / (float)FOC_DEPTH + 0.5;
			}
			for (i = 3; i < focMax; i ++) //set to 0
			{
				m_focPos[i] = 0;
			}
			break;

		case 4: //four
			for (i = 0; i < 4; i ++)
			{
				m_focPos[i] = depth * FOUR_FOC_POS[index][i] / (float)FOC_DEPTH + 0.5;
			}
			for (i = 4; i < focMax; i ++) //set to 0
			{
				m_focPos[i] = 0;
			}
			break;

		default:
			break;
	}
	
    // if out of range
    if (m_focPos[0] < 1)
        m_focPos[0] = 1;
	for (i = 0; i < focMax; i ++) 
	{
		m_calcPara.focPos[i] = m_focPos[i];
	}
}

void Img2D::MLine(int line)
{
	///> send m line
	m_calcPara.mLine = line;
	m_ptrCalc->CalcMLine();

	///> draw m line
	m_ptrUpdate->MLine(line);
}

/*
 * @brief set changed scale to current image 
 */
void Img2D::SetScale2D(double scale)
{
	// send to dsc 
	m_ptrDscPara->dcaScale = (float)scale;

	if (m_ptrDsc != NULL)
	{
		PRINTF("ptrDsc is not NULL\n");
		m_ptrDsc->UpdateScale();
	}


    /**解决切换探头刻度尺没有切换的问题**/
     int RealcurB = Format2D::GetInstance()->GetLastRealB();
    m_scale2D[RealcurB] = scale;
    if (m_zoomMeasureCtrl)
    {
        m_scale2DZoom[RealcurB] = m_scale2D[RealcurB] / m_zoomScale;
    }
#if 0
	Format2D::EFormat2D format = s.GetFormat2D();
	if (format == Format2D::B)
	{
		m_scale2D[0] = scale;
	}
	else if (format == Format2D::BB)
	{
		m_scale2D[curB] = scale;
	}
	else if (format == Format2D::B4)
	{
		m_scale2D[curB] = scale;
	}
#endif

    PRINTF("depth --------m_scale2D = %f, scale2DZoom = %f\n", scale, scale/m_zoomScale);
}

/*
 * @brief set speed of sound when TSI type is changed
 *
 * @para index, current TSI type index
 */
void Img2D::Tsi(int index, EKnobReturn ret)
{
//	ASSERT(index < 4);
	if(index >= 4)
		return;

	m_calcPara.soundSpeedTsi = TSI_SPEED[index];

	///> calc delay
	m_ptrCalc->CalcEmitDelay();
	m_ptrCalc->CalcReceiveDelay();

	///> calc max period
	m_ptrCalc->CalcMaxPeriod();

	///> calc sample
	m_ptrCalc->CalcSample();

	//draw
	m_ptrUpdate->Tsi(TSI_DISPLAY[index].c_str(), ret);
}

//void Img2D::LineDensity(int index, EKnobReturn ret)
void Img2D::LineDensity(int index, EKnobReturn ret, bool update)
{
	///> calc line density
	m_calcPara.lineDensity = index;

    ///> send to fpga
	m_ptrCalc->CalcLineDensity();

	///> draw
	if (update)
	m_ptrUpdate->LineDensity(LINE_DENSITY_DISPLAY[index].c_str(), ret);

//    ///> update MBP
	m_ptrUpdate->MBP(MBP[m_mbpIndex], OK);
}

///> only for test
void Img2D::PrintImgPara()
{
	int i;
		PRINTF("======================PRINTF VARIABLE IMG2D======================\n");

	for (i = 0; i < 8; i ++)
	{
		PRINTF("tgc[%d] = %d\n", i, m_tgc[i]);
	}
	PRINTF("m_gain2D = %d\n", m_gain2D);
	PRINTF("m_gainM = %d\n", m_gainM);
	PRINTF("m_lines = %d\n", m_lines);
	PRINTF("m_freqRange[0] = %d\n", m_freqRange[0]);
	PRINTF("m_freqRange[1] = %d\n", m_freqRange[1]);
	for(i = 0; i < (int)m_vecFreqRange.size(); i ++)
	{
		PRINTF("freq pair[%d]: emit = %d, receive = %d\n", i, m_vecFreqRange[i].emit, m_vecFreqRange[i].receive);
	}
	PRINTF("m_depthMax = %d\n", m_depthMax);
	PRINTF("m_freq emit = %d, receive = %d\n", m_freq.emit, m_freq.receive);
	PRINTF("m_depth = %d\n", m_depth);
	PRINTF("m_imgScaleIndex = %d\n", m_imgScaleIndex);
	PRINTF("m_focSum = %d\n", m_focSum);
	PRINTF("m_focPosIndex = %d\n", m_focPosIndex);
	for (i = 0; i < MAX_FOCUS; i ++)
	{
		PRINTF("focPos[%d] = %d\n", i, m_focPos[i]);
	}

	PRINTF("m_angleIndex = %d\n", m_scanAngleIndex);
	PRINTF("m_dynamicIndex = %d\n", m_dynamicRangeIndex);
	PRINTF("m_lineDensityIndex = %d\n", m_lineDensityIndex);
	PRINTF("m_edgeEnhanceIndex = %d\n", m_edgeEnhanceIndex);
	PRINTF("m_powerIndex = %d\n", m_soundPowerIndex);
	PRINTF("m_harmonic = %d\n", m_harmonic);
	PRINTF("m_tsiIndex = %d\n", m_tsiIndex);
	PRINTF("m_mbpIndex= %d\n", m_mbpIndex);
	PRINTF("m_mLine = %d\n", m_mLine);
	PRINTF("m_mSpeed = %d\n", m_mSpeedIndex);
	for (i = 0; i < 4; i ++)
	{
		PRINTF("scale2D[%d] = %f\n", i, m_scale2D[i]);
		PRINTF("scale2DZoom[%d] = %f\n", i, m_scale2DZoom[i]);
	}
}

void Img2D::PrintCalcPara()
{
	int i;
	
	PRINTF("======================PRINTF VARIABLE calc2D======================\n");
 
    PRINTF("array = %d\n", m_calcPara.probeArray);
    PRINTF("lines = %d\n", m_calcPara.probeLines);
    PRINTF("width = %d\n", m_calcPara.probeWidth);
	PRINTF("probeR = %d\n", m_calcPara.probeR);
	for (i = 0; i < MAX_FOCUS; i ++)
	{
			PRINTF("focPos[%d] = %d\n", i, m_calcPara.focPos[i]);
	}

    PRINTF("soundSpeed = %f\n", m_calcPara.soundSpeed);
    PRINTF("soundSpeedTsi = %f\n", m_calcPara.soundSpeedTsi);
    PRINTF("depth = %d\n", m_calcPara.depth);
    PRINTF("depthMax = %d\n", m_calcPara.depthMax);
    PRINTF("depthDots = %d\n", m_calcPara.depthDots);
    PRINTF("imgScaleIndex = %d\n", m_calcPara.imgScaleIndex);
    PRINTF("imgScaleIndexMax = %d\n", m_calcPara.imgScaleIndexMax);
    PRINTF("imgScale = %d\n", m_calcPara.imgScale);
    PRINTF("angleIndex = %d\n", m_calcPara.angleIndex);
    PRINTF("angleIndexMax = %d\n", m_calcPara.angleIndexMax);
    PRINTF("freq emit = %d, receive = %d\n", m_calcPara.freq.emit, m_calcPara.freq.receive);
    PRINTF("power = %d\n", m_calcPara.power);
    PRINTF("lineDensityIndex = %d\n", m_calcPara.lineDensity);
    PRINTF("edgeEnhanceIndex = %d\n", m_calcPara.edgeEnhance);
    PRINTF("harmonic = %d\n", m_calcPara.harmonic);
    PRINTF("mLine = %d\n", m_calcPara.mLine);
    PRINTF("mSpeed = %d\n", m_calcPara.mSpeedIndex);
    
}

void Img2D::SetDscScanLine()
{
	int scanRange[2];
	m_ptrCalc->GetScanRange(scanRange);
	
	///> send to dsc 
	m_ptrDscPara->dcaCurMaxScanLines = m_lines;
	m_ptrDscPara->dcaCurScanStart = scanRange[0];
	m_ptrDscPara->dcaCurScanEnd = scanRange[1];
	PRINTF("MaxScanLine = %d\n", m_ptrDscPara->dcaCurMaxScanLines);
	PRINTF("scanStart = %d\n", m_ptrDscPara->dcaCurScanStart);
	PRINTF("scanEnd = %d\n", m_ptrDscPara->dcaCurScanEnd);

	if (m_ptrDsc != NULL)
		m_ptrDsc->UpdateCurMaxScanLines();
}

void Img2D::SetScanLines(int index)
{
    m_lines = SCAN_LINES_PHASE_PROBE[m_indexScanLines];
    m_calcPara.probeLines = m_lines; 
    
    m_ptrUpdate->ProbePara(m_type, m_calcPara.probeLines, m_calcPara.extendedAngle, m_calcPara.probeR);

    ///> calc emit delay  
    m_ptrCalc->CalcEmitDelay();

    ///> calc receive delay
    m_ptrCalc->CalcReceiveDelay();

    ///> calc scan angle
    m_ptrCalc->CalcScanRange();
    SetDscScanLine();
}

void Img2D::GetScanAngle(int& ScanAngle)
{
    ScanAngle = m_ptrCalc->CalcRealScanAngle();
}

EKnobReturn Img2D::DisplayScanAngle()
{
	EKnobReturn ret = OK;
    if (m_scanAngleIndex == (MAX_ANGLE - 1)) 
        ret = MIN;
    else if (m_scanAngleIndex == 0)
        ret = MAX;
    else 
        ret = OK;

	int angle = m_ptrCalc->CalcRealScanAngle();
	
	///> draw 
	m_ptrUpdate->ScanAngle(angle, ret);
	return ret;
}

void Img2D::GetDisplayScanAngle(float& scanangle)
{
    scanangle = m_ptrCalc->CalcRealScanAngle();
}

void Img2D::Set2DTis(int scanAngleIndex, float focPos, int freq, int power, int depthIndex)
{
	float ti = m_ptrCalc->Get2DTis(scanAngleIndex, focPos, freq, power, depthIndex);
 
    m_ptrUpdate->TIS(ti);
	
}

int Img2D::GetOutputPower()
{
	return POWER_DATA[m_soundPowerIndex];
}

int Img2D::GetLDensity()
{
	return m_lineDensityIndex;
}

int Img2D::GetMBP()
{
	return MBP[m_mbpIndex];
}

int Img2D::GetMBPIndex()
{
	return m_mbpIndex;
}

int Img2D::GetMBPFpga2D(void)
{
	return FPGAMBP2D[m_lineDensityIndex][m_mbpIndex]; 
}

int Img2D::GetMBPFpgaColor(void)
{
	return FPGAMBPColor[m_lineDensityIndex][m_mbpIndex]; 
}
double Img2D::GetSoundSpeed()
{
	return SOUND_SPEED;
}

int Img2D::GetImgStartDots()
{
	return (m_ptrCalc->CalcImgStartDepth() / GetScale2D() + 0.5);
}

int Img2D::GetScanLines()
{
	return m_lines;
}

/*
 * @brief get width between each lines, only valid for linear probe
 *
 * @retval width unit: mm
 */
double Img2D::GetWidthBetweenLinesL()
{
	return m_ptrCalc->CalcWidthBetweenLines();
}

int Img2D::GetDepth()
{
	return m_depth;
}

int Img2D::GetDepthIndex()
{
	return m_imgScaleIndex;
}

int Img2D::GetFocSum(void)
{
    return m_focSum;
}

bool Img2D::GetHarmonicStatus(void)
{
    return m_harmonic;
}

bool Img2D::GetEFVIStatus(void)
{
    return m_efviCtrl;
}

bool Img2D::GetTpViewStatus(void)
{
    if (m_tpViewIndex == 0)
        return FALSE;
    else
        return TRUE;

    return TRUE;
}

void Img2D::Enter2D()
{
	///> calc tgc
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);
	///> calc focus pulse width
	CalcFocPulse();
   
    // re calc maxperiod, simult on to off
    m_ptrCalc->CalcMaxPeriod();
    if ((m_type != 'P') && (m_type != 'p')) 
    {
        if (!IsCompoundSpaceOn())
        {
            if (((m_type == 'l') || (m_type == 'L')) && m_spaceCompoundIndexBak == 0)
                SetSteer(m_steerIndexbak);
            else
            {
#ifndef EMP_355	
				IoCtrl io;
                io.Freeze();
                usleep(55000);
#endif
                SetCompoundSpace(m_spaceCompoundIndexBak);
#ifndef EMP_355
                usleep(50000);//solve the problem of showing white splots from cfm mode to switch to 2D mode.
                io.Unfreeze();
#endif
            }
        }
    }

    if (!IsCompoundFreqOn())
    {
        SetCompoundFreq(m_freqCompoundCtrlBak);
    }
}

void Img2D::EnterM()
{
	///> calc tgc
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
	m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
}

void Img2D::CalcSampleAgain(void)
{
	m_ptrCalc->CalcSample();
}

/*
 * @brief revise probe type for dsc, because only "C", "L" type can be identification
 * 
 * @para type[in] real type of probe
 *
 * @retval revised probe type, valided to DSC 
 */
char Img2D::ReviseProbeType(char type)
{
	char probeType = 'C';

	switch(type)
	{
		case 'v':
		case 'V':
		case 'c':
		case 'C':
			probeType = 'C';
			break;
		
		case 'n':
		case 'N':
			probeType = 'C';
			break;
		
		case 'l':
		case 'L':
			probeType = 'L';
			break;

		case 't':
		case 'T':
			probeType = 'T';
			break;

		case 'p':
		case 'P':
			probeType = 'P';
			break;

		default:
			probeType = type;
			break;
	}

	return probeType;
}

void Img2D::EnterEFOV()
{
	m_mouseSpeedEfovBak = g_keyInterface.GetMouseSpeed();
	g_keyInterface.SetMouseSpeed(2);
	Depth(13);
#if 0
        m_frameAverEfovBak = m_ptrDscPara->dca2DIPAttrs.ipaFrameAver;
	m_lineAverEfovBak = m_ptrDscPara->dca2DIPAttrs.ipaLineAver;
	m_ptrDscPara->dca2DIPAttrs.ipaFrameAver = 64;
	m_ptrDscPara->dca2DIPAttrs.ipaLineAver = 64;
#endif
}

void Img2D::ExitEFOV()
{
	g_keyInterface.SetMouseSpeed(m_mouseSpeedEfovBak);
	Depth(m_imgScaleIndex);
#if 0
	ExamItem::ParaItem paraItem;
	ImgProc2D::GetInstance()->GetCurPara(&paraItem);
	m_ptrDscPara->dca2DIPAttrs.ipaFrameAver = m_frameAverEfovBak;
	m_ptrDscPara->dca2DIPAttrs.ipaLineAver = m_lineAverEfovBak;
#endif
}

double Img2D::GetScaleEFOV()
{
	double scale = GetScale2D();
	return scale;
}

int Img2D::CalcFPS(void)
{
    return m_ptrCalc->CalcFPS();
}

void Img2D::SetScaleForSpecialMeasure(double scale2DAll[4], double scale2D, double scaleMDepth, double scaleMTime)
{
    for (int i = 0; i < 4; i ++)
    {
        m_scale2DAllSnap[i] = scale2DAll[i];
    }

    m_scale2DSnap = scale2D;
    m_scaleMDepthSnap = scaleMDepth;
    m_scaleMTimeSnap = scaleMTime;
}
void Img2D::SetScale2DZoomMeasure(double scale)
{
    m_zoomMeasureCtrl = TRUE;
    m_zoomScale = scale;

    if (m_zoomScale != ZERO)
    {
        for (int i = 0; i < 4; i ++)
        {
            m_scale2DZoom[i] = m_scale2D[i] / m_zoomScale;
        }
        PRINTF("set scale 2d zoom, scale = %f\n", m_scale2DZoom[0]);
    }
}
void Img2D::RestoreScale2DZoomMasure(void)
{
    m_zoomMeasureCtrl = FALSE;
}

void Img2D::ResetFocPosWhenZoom(void)
{
    // reset foc pos
    int indexB, indexE;
    int index = m_focPosIndex;
    GetFocPosIndexRange(indexB, indexE);
    if ((index < indexB) || (index > indexE))
    {
        index = (indexB + indexE) / 2;
        SetFocPos(index);
    }
}

void Img2D::ModeCwCtrl(bool on)
{
    m_calcPara.modeCw = on;
    m_ptrCalc->CalcEmitDelay();
}
void Img2D::UpdateAutoOptimize(bool on)
{
    m_ptrUpdate->AutoOptimize(on);
}

void Img2D::EnterSingleAperture(bool on)
{
    m_ptrCalc->EnterSingleAperture(on);
}

char Img2D::GetReviseProbeType(void)
{
	return m_ptrDscPara->dcaProbeType;
}

void Img2D::CompoundSpaceCtrl(bool on)
{
    m_ptrCalc->CalcSpaceCompoundCtrl(on);

    if (on)
    {
		m_ptrCalc->CalcEmitDelay();
		m_ptrCalc->CalcReceiveDelay();
		m_ptrCalc->CalcReceiveDelayColor();
    }
}
void Img2D::CompoundFreqCtrl(bool on)
{
    m_ptrCalc->CalcFreqCompoundCtrl(on);

	// change freq compound ctrl
	int size = m_vecFreqRange.size();
	if (on)
	{
		// select freq
		int indexPre;
		int indexNext;
		int index = m_freqIndex;
        if (size == 1)
        {
            indexPre = index;
            indexNext = index;
        }
        else if (size == 2)
        {
            indexPre = index;
            indexNext = index + 1;
        }
        else if (size > 2)
        {
            if (index == 0)//first
            {
                indexPre = index;
                indexNext = index + 1;
            }
            else if (index == (size - 1))// last
            {
                indexPre = index - 1;
                indexNext = index;	
            }
            else
            {
                indexPre = index - 1;
                indexNext = index + 1;			
            }
        }
        m_freqIndexPre = indexPre;
        m_freqIndexNext = indexNext;
		
		// freq1 calc
		m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexPre].emit;
		m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexPre].receive;
		m_ptrCalc->CalcFocPulseFreqCompound(1);
		m_ptrCalc->CalcFilterFreqCompound(1);

		// freq2 calc
		m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexNext].emit;
		m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexNext].receive;
		m_ptrCalc->CalcFocPulseFreqCompound(2);
		m_ptrCalc->CalcFilterFreqCompound(2);	

		// freq compound coef
		m_ptrCalc->CalcFreqCompoundCoef();

		// emit and receive calc
		m_ptrCalc->CalcEmitDelay();
		m_ptrCalc->CalcReceiveDelay();
		m_ptrCalc->CalcReceiveDelayColor();	
		
		// restore freq
		m_calcPara.freq.emit = m_vecFreqRange[index].emit;
		m_calcPara.freq.receive = m_vecFreqRange[index].receive;		
	}
}

void Img2D::CalcFilter(void)
{
    //float zero = 0.0;
    //m_ptrCalc->SetZeroToFdDynamicDemod_test(zero);
    // normal
    m_ptrCalc->CalcFilter();

    // compound
    if (m_freqCompoundCtrl)
    {
        PRINTF("index = %d, pre = %d, next = %d\n", m_freqIndex, m_freqIndexPre, m_freqIndex);
        int index = m_freqIndex;

		m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexPre].emit;
		m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexPre].receive;
		m_ptrCalc->CalcFilterFreqCompound(1);

		m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexNext].emit;
		m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexNext].receive;
		m_ptrCalc->CalcFilterFreqCompound(2);

		m_calcPara.freq.emit = m_vecFreqRange[index].emit;
		m_calcPara.freq.receive = m_vecFreqRange[index].receive;		
    }
}

bool Img2D::CalcFocPulse(void)
{
    bool isChange = FALSE;
    //normal
	isChange = m_ptrCalc->CalcFocPulse();

    // compound
	if (m_freqCompoundCtrl)
    {
        int index = m_freqIndex;

		m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexPre].emit;
		m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexPre].receive;
		m_ptrCalc->CalcFocPulseFreqCompound(1);

		m_calcPara.freq.emit = m_vecFreqRange[m_freqIndexNext].emit;
		m_calcPara.freq.receive = m_vecFreqRange[m_freqIndexNext].receive;
		m_ptrCalc->CalcFocPulseFreqCompound(2);

		m_calcPara.freq.emit = m_vecFreqRange[index].emit;
		m_calcPara.freq.receive = m_vecFreqRange[index].receive;		
    }

    return isChange;
}

void Img2D::GetBMLineRange(int scanRange[2])
{
	int interval = 0;

    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
        Zoom::GetInstance()->GetScanLineRange(scanRange);
    }
    else
    {
        Img2D::GetInstance()->GetDisplayScanRange(scanRange);
    }

	scanRange[0] += interval;
	scanRange[1] -= interval;
}

void Img2D::GetFocPosIndexRange(int &indexB, int &indexE)
{
    if (m_zoomMeasureCtrl)
    {
        int lzDotRange[2];
        Zoom::GetInstance()->GetScanDotRange(lzDotRange);
        PRINTF("lzDotRange(%d, %d)\n", lzDotRange[0], lzDotRange[1]);
        lzDotRange[1] -= 5;//适当校正，避免焦点选在贴近局部放大框底部的位置，导致屏幕上绘制不出来（凸阵）。

        int i;
        double scale = GetScale2DInImgHDot();
        PRINTF("lzDepthRange(%f, %f)\n", lzDotRange[0]*scale, lzDotRange[1]*scale);
        int focSum = m_focSum;
        int sumIndex = FOC_POS_INDEX_MAX[focSum-1];
        switch(focSum)
        {
            case 1:
                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * ONE_FOC_POS[i][0] / FOC_DEPTH / scale) >= lzDotRange[0])
                    {
                        PRINTF("b: [%d] = %d\n, depth = %dmm", i, (int)(m_depth * ONE_FOC_POS[i][0] / FOC_DEPTH / scale), ONE_FOC_POS[i][0]);
                        indexB = i;
                        break;
                    }
                }

                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * ONE_FOC_POS[sumIndex - 1 - i][0] / FOC_DEPTH / scale) <= lzDotRange[1])
                    {
                        PRINTF("e: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * ONE_FOC_POS[sumIndex-1 - i][0] / FOC_DEPTH / scale), ONE_FOC_POS[sumIndex-1-i][0]);
                        indexE = sumIndex-1 - i;
                        break;
                    }
                }
                break;

            case 2:
                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * TWO_FOC_POS[i][0] / FOC_DEPTH / scale) >= lzDotRange[0])
                    {
                        PRINTF("b: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * TWO_FOC_POS[i][0] / FOC_DEPTH / scale), TWO_FOC_POS[i][0]);
                        indexB = i;
                        break;
                    }
                }

                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * TWO_FOC_POS[sumIndex-1 - i][1] / FOC_DEPTH / scale) <= lzDotRange[1])
                    {
                        PRINTF("e: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * TWO_FOC_POS[sumIndex-1-i][1] / FOC_DEPTH / scale), TWO_FOC_POS[sumIndex-1-i][1]);
                        indexE = sumIndex-1 - i;
                        break;
                    }
                }               
                break;

            case 3:
                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * THREE_FOC_POS[i][0] / FOC_DEPTH / scale) >= lzDotRange[0])
                    {
                        PRINTF("b: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * THREE_FOC_POS[i][0] / FOC_DEPTH / scale), THREE_FOC_POS[i][0]);
                        indexB = i;
                        break;
                    }
                }

                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * THREE_FOC_POS[sumIndex-1 - i][2] / FOC_DEPTH / scale) <= lzDotRange[1])
                    {
                        PRINTF("e: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * THREE_FOC_POS[sumIndex-1-i][2] / FOC_DEPTH / scale), THREE_FOC_POS[sumIndex-1-i][2]);
                        indexE = sumIndex-1 - i;
                        break;
                    }
                }                 
                break;

            case 4:
                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * FOUR_FOC_POS[i][0] / FOC_DEPTH / scale) >= lzDotRange[0])
                    {
                        PRINTF("b: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * FOUR_FOC_POS[i][0] / FOC_DEPTH / scale), FOUR_FOC_POS[i][0]);
                        indexB = i;
                        break;
                    }
                }

                for (i = 0; i < sumIndex; i ++)
                {
                    if ((m_depth * FOUR_FOC_POS[sumIndex-1 - i][3] / FOC_DEPTH / scale) <= lzDotRange[1])
                    {
                        PRINTF("e: [%d] = %d, depth = %dmm\n", i, (int)(m_depth * FOUR_FOC_POS[sumIndex-1-i][3] / FOC_DEPTH / scale), FOUR_FOC_POS[sumIndex-1-i][3]);
                        indexE = sumIndex-1 - i;
                        break;
                    }
                }                 

                break;

            default:
                break;
        }
    }
    else
    {
        indexB = 0;
        indexE = FOC_POS_INDEX_MAX[m_focSum-1] - 1;
    }

    if (indexB > indexE)
    {
        indexB = indexE;
    }
    PRINTF("foc pos index range(%d, %d)\n", indexB, indexE);
}

EKnobReturn Img2D::TpViewPara(int index)
{
    switch(index)
    {
        case 0: //normal
            m_calcPara.tpViewCtrl = FALSE;
            m_calcPara.tpSteer = STEER_ANGLE[m_steerIndex];
            m_calcPara.steerAngle = STEER_ANGLE[m_steerIndex];
           // m_calcPara.tpSteer = 0.0;
           // m_calcPara.steerAngle = 0.0;
            m_ptrDscPara->dcaTpView = 0;
            m_ptrDscPara->dcaSteer = m_calcPara.tpSteer;
            break;
#if 0

        case 1: //steer left
            m_calcPara.tpViewCtrl = FALSE;
            m_calcPara.tpSteer = -10.0;
            m_calcPara.steerAngle = -10.0;
            m_ptrDscPara->dcaTpView = 0;
            m_ptrDscPara->dcaSteer = m_calcPara.tpSteer;
            break;

        case 2: //steer right
            m_calcPara.tpViewCtrl = FALSE;
            m_calcPara.tpSteer = 10.0;
            m_calcPara.steerAngle = 10.0;
            m_ptrDscPara->dcaTpView = 0;
            m_ptrDscPara->dcaSteer = m_calcPara.tpSteer;
            break;
#endif
        case 1: //tp View
            m_calcPara.tpViewCtrl = TRUE;
            if (STEER_ANGLE[m_steerIndex] == 0)
             m_calcPara.tpSteer = 10.0;
            else
            m_calcPara.tpSteer = STEER_ANGLE[m_steerIndex];
            m_calcPara.steerAngle = 0.0;
            m_ptrDscPara->dcaTpView = 1;
            m_ptrDscPara->dcaSteer = m_calcPara.tpSteer;
            break;

        default: //normal
            m_calcPara.tpViewCtrl = FALSE;
            m_calcPara.tpSteer = 0.0;
            m_calcPara.steerAngle = 0.0;
            m_ptrDscPara->dcaTpView = 0;
            m_ptrDscPara->dcaSteer = m_calcPara.tpSteer;
            break;
    }

    EKnobReturn ret;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_TP_VIEW - 1))
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->TpView(index, ret);

    return ret;
}

EKnobReturn Img2D::TpView(int index)
{
    EKnobReturn ret;
    ret = TpViewPara(index);
 
    if (m_ptrDsc != NULL)
        m_ptrDsc->UpdateTpViewStatus();

    m_ptrCalc->CalcExtendedImaging(m_calcPara.tpViewCtrl);
    m_ptrCalc->CalcEmitDelay();
    m_ptrCalc->CalcReceiveDelay();

      return ret;
}

void Img2D::ExitTpView(void)
{
    m_tpViewIndex = 0;
    TpView(0);
}

EKnobReturn Img2D::EFVIViewPara(bool on)
{
    if (on)
    {
        m_ptrDscPara->dcaProbeWidth = m_calcPara.extendedAngle;
        m_ptrUpdate->ProbePara(m_type, m_calcPara.probeLines, m_calcPara.extendedAngle, m_calcPara.probeR);
    }
    else
    {
        m_ptrDscPara->dcaProbeWidth = m_calcPara.probeWidth;
        m_ptrUpdate->ProbePara(m_type, m_calcPara.probeLines, m_calcPara.probeWidth, m_calcPara.probeR);

    }

    if (m_ptrDsc != NULL)
        m_ptrDsc->UpdateProbeWidth();

    EKnobReturn ret;
    if (on)
        ret = MAX;
    else 
        ret = MIN;
    m_ptrUpdate->EFVI(on, ret);
	DisplayScanAngle();

    return ret;
}

EKnobReturn Img2D::EFVIView(bool on)
{
    EKnobReturn ret;

    m_calcPara.efviCtrl = on;
    ret = EFVIViewPara(on);

    // realize
 //   m_calcPara.efviCtrl = on;
    m_ptrCalc->CalcExtendedImaging(on);
    m_ptrCalc->CalcEmitDelay();
    m_ptrCalc->CalcReceiveDelay();

    return ret;
}

void Img2D::ExitEFVI(void)
{
    m_efviCtrl = FALSE;
    EFVIView(m_efviCtrl);
}
/*
 * @brief set Po Overturn status to DSC
 */
void Img2D::SetPolarity(bool on, EKnobReturn status)
{
    DscMan* ptrDscMan = DscMan::GetInstance();
    ptrDscMan->GetWriteLock();
	m_ptrDscPara->dca2DIPAttrs.ipaPolarityOverturn = on;

    UpdateDsc();
    ptrDscMan->ReadWriteUnlock();

	m_ptrUpdate->Polarity(on, status);
}
void Img2D::UpdateDsc(void)
{
	CDSC* ptrDsc = DscMan::GetInstance()->GetDsc();
	if (ptrDsc != NULL)
    {
		ptrDsc->Update();
        Update2D::CopyImgDataToReplay();
    }
}


//test for changing channel num cw
void Img2D::SendChannelCW(int num)
{
    m_ptrCalc->ChangeChanelNum(num);
}

//test for changing dynamicfilter current frequence
void Img2D::ChangeFreqDynamicFilter_test(EKnobOper oper, int segment)
{
//    if(!m_harmonic)
 //       return;
    float fc = 0;
    if(oper == ADD)
    {
        switch (segment)
        {
            case 1:
                {
                    m_fc1 += 0.1;
                    fc = m_fc1;
                }
                break;

            case 2:
                {
                    m_fc2 += 0.1;
                    fc = m_fc2;
                }
                break;

            case 3:
                {
                    m_fc3 += 0.1;
                    fc = m_fc3;
                }
                break;

            case 4:
                {
                    m_fc4 += 0.1;
                    fc = m_fc4;
                }
                break;

            case 5:
                {
                    m_fc5 += 0.1;
                    fc = m_fc5;
                }
                break;
        }
    }
    else if(oper == SUB)
    {
        switch(segment)
        {
            case 1:
                {
                    m_fc1 -=0.1;
                    fc = m_fc1;
                }
                break;
            case 2:
                {
                    m_fc2 -=0.1;
                    fc = m_fc2;
                }
                break;
            case 3:
                {
                    m_fc3 -=0.1;
                    fc = m_fc3;
                }
                break;
            case 4:
                {
                    m_fc4 -=0.1;
                    fc = m_fc4;
                }
                break;
            case 5:
                {
                    m_fc5 -=0.1;
                    fc = m_fc5;
                }
                break;
        }
    }
    m_ptrCalc->ChangeFreqDynamicFilter(fc, segment);
}

//改变带通滤波器的fc，杂每段期间成线性变化
void Img2D::ChangeFreqBPFilter_test(EKnobOper oper, int segment)
{
  //  if(!m_harmonic)
   //     return;
    float fc = 0.0;
    if(oper == ADD)
    {
        switch (segment)
        {
            case 1:
                {
                    m_fc01 += 0.1;
                    fc = m_fc01;
                }
                break;

            case 2:
                {
                    m_fc02 += 0.1;
                    fc = m_fc02;
                }
                break;

            case 3:
                {
                    m_fc03 += 0.1;
                    fc = m_fc03;
                }
                break;

            case 4:
                {
                    m_fc04 += 0.1;
                    fc = m_fc04;
                }
                break;

            case 5:
                {
                    m_fc05 += 0.1;
                    fc = m_fc05;
                }
                break;
        }
    }
    else if(oper == SUB)
    {
        switch(segment)
        {
            case 1:
                {
                    m_fc01 -=0.1;
                    fc = m_fc01;
                }
                break;
            case 2:
                {
                    m_fc02 -=0.1;
                    fc = m_fc02;
                }
                break;
            case 3:
                {
                    m_fc03 -=0.1;
                    fc = m_fc03;
                }
                break;
            case 4:
                {
                    m_fc04 -=0.1;
                    fc = m_fc04;
                }
                break;
            case 5:
                {
                    m_fc05 -=0.1;
                    fc = m_fc05;
                }
                break;
        }
    }


     switch(segment)
    {
        case 1:
            m_ptrCalc->ChangeFreqBandPassFilter(fc, 1);
            break;

        case 2:
            m_ptrCalc->ChangeFreqBandPassFilter(fc, 2);
            break;

        case 3:
            m_ptrCalc->ChangeFreqBandPassFilter(fc, 3);
            break;

        case 4:
            m_ptrCalc->ChangeFreqBandPassFilter(fc, 4);
            break;

        case 5:
            m_ptrCalc->ChangeFreqBandPassFilter(fc, 5);
            break;
    }
}

void Img2D::ChangeBPFilter(EKnobOper oper)
{
   // if(!m_harmonic)
    //    return;
    static int bandPassIndex = 1;
    int data = bandPassIndex;
    if(oper == ADD)
    {
        if(data < 7)
            bandPassIndex +=1;
        else
            bandPassIndex = 1;
    }
    else if(oper == SUB)
    {
        if(data > 1)
            bandPassIndex -=1;
        else
            bandPassIndex = 7;
    }
    m_ptrCalc->ChangeBPFilter(bandPassIndex);
}

void Img2D::ChangeDynamicDemodFd(EKnobOper oper, int segment)
{
//    if(!m_harmonic)
 //       return;

    float fd;
    if(oper == ADD)
    {
        fd = 0.1;
        m_ptrCalc->ChangeFdDynamicDemod_test(fd, segment);
    }
    else if (oper == SUB)
    {
        fd = -0.1;
        m_ptrCalc->ChangeFdDynamicDemod_test(fd, segment);
    }
}

void Img2D::ChangeHarmonicFreq(EKnobOper oper)
{
    if(!m_harmonic)
        return;

    int freqEmit;
    if(oper == ADD)
        m_freqEmit += 5;
    else if(oper == SUB)
        m_freqEmit -= 5;

    freqEmit = m_freqEmit;

    m_ptrCalc->CalcPulse(freqEmit);
    usleep(44000);
	IoCtrl io;
    io.Freeze();
    m_ptrUpdate->HarmonicFreq(freqEmit, OK);
}

void Img2D::DefaultFreqBandPassFilter(int probeindex, int thi)
{
    ASSERT(probeindex < NUM_PROBE);
    ASSERT(thi < ProbeSocket::MAX_HARMONIC_FREQ);
#if ((defined EMP_430) || (defined EMP_340) || (defined(EMP_360)) ||(defined(EMP_161)) || defined(EMP_355) || defined(EMP_322) ||(defined (EMP_440)))
	m_ptrCalc->DefaultFreqBPFilter(ProbeSocket::BAND_PASS_FILTER_FC1[probeindex][thi], ProbeSocket::BAND_PASS_FILTER_FC2[probeindex][thi]);
#else
    m_ptrCalc->DefaultFreqBPFilter(ProbeSocket::BAND_PASS_FILTER_FC[probeindex][thi]);
#endif
}

void Img2D::DefaultFreqDynamicFilter(int probeindex, int thi)
{
    ASSERT(probeindex < NUM_PROBE);
    ASSERT(thi < ProbeSocket::MAX_HARMONIC_FREQ);
    m_ptrCalc->DefaultFreqDynamicFilter(ProbeSocket::DYNAMIC_FILTER_FC[probeindex][thi]);
}

void Img2D::DefaultDynamicDemodFd(int probeindex, int thi)
{
    ASSERT(probeindex < NUM_PROBE);
    ASSERT(thi < ProbeSocket::MAX_HARMONIC_FREQ);
    m_ptrCalc->CalcDefaultDemodFd();
}

void Img2D::DefaultIndexBandPassFilter(int probeindex, int thi)
{
//    if(!m_harmonic)
 //       return;

    int num = 0;
#if defined(EMP_430)
    if(probeindex == 0)
    {
        switch(thi)
        {
            case 0:
            case 1:
            case 2:
                num = 4;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else if(probeindex == 1)
    {//65L40E
        switch(thi)
        {
            case 0:
            case 1:
            case 2:
                num = 3;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else if(probeindex == 2)
    {//65C10E
        switch(thi)
        {
            case 0:
                num = 3;
                m_ptrCalc->ChangeBPFilter(num);
                break;

            case 1:
            case 2:
                num = 4;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else if(probeindex == 3)
    {
        switch(thi)
        {
            case 0:
                num = 3;
                m_ptrCalc->ChangeBPFilter(num);
                break;

            case 1:
            case 2:
                num = 2;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else
    {
        m_ptrCalc->ChangeBPFilter(1);
    }
#elif EMP_161
    if(probeindex == 0)
    {//3.5CV
        num = 5;
    }
    else if(probeindex == 1)
    {//7.5LVS
        switch(thi)
        {
            case 1:
                num = 5;
                break;
            default:
                num = 6;
                break;
        }
    }
    else if(probeindex == 2)
    {//6.5VMC
        switch(thi)
        {
            case 0:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 3)
    {//3.5MC
        switch(thi)
        {
            case 0:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else
    {
        num = 1;
    }
    m_ptrCalc->ChangeBPFilter(num);
#elif EMP_440
 if(probeindex == 0)
    {//3.5CV
        num = 5;
    }
    else if(probeindex == 1)
    {//7.5LVS
        switch(thi)
        {
            case 1:
                num = 5;
                break;
            default:
                num = 6;
                break;
        }
    }
    else if(probeindex == 2)
    {//6.5VMC
                num = 4;
    }
    else if(probeindex == 3)
    {
                num = 2;
    }
    else
    {
        num = 1;
    }
    m_ptrCalc->ChangeBPFilter(num);


#elif EMP_360
    if(probeindex == 0)
    {//3.5CV
        num = 5;
    }
    else if(probeindex == 1)
    {//7.5LVS
        num = 6;
    }
    else if(probeindex == 2)
    {//6.5VMC
        switch(thi)
        {
            case 0:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 3)
    {//3.5MC
        switch(thi)
        {
            case 0:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 4)
    {//6.5MC
        num = 2;
    }
    else if(probeindex == 5)
    {//30P16A
        switch(thi)
        {
            case 0:
                num = 6;
                break;
            case 1:
            case 2:
                num = 7;
                break;
            default:
                num = 1;
                break;
        }
    }
    else
    {
        num = 1;
    }
    m_ptrCalc->ChangeBPFilter(num);
#else
    //35C50K
    if(probeindex == 0)
    {
        switch(thi)
        {
            case 0:
                {
                    num = 4;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;

            case 1:
                {
                    num = 5;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;

            case 2:
                {
                    num = 6;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;

            default:
                {
                    num = 1;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
        }
    }
    //75L40K
    else if (probeindex == 1)
    {
        num = 6;
        m_ptrCalc->ChangeBPFilter(num);
    }
    //65C10K
    else if(probeindex == 2)
    {
        switch(thi)
        {
            case 0:
                {
                    num = 5;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 1:
                {
                    num = 4;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 2:
                {
                    num = 2;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            default:
                break;
        }
    }
    //65C15D 
    else if(probeindex == 4)
    {
        switch(thi)
        {
            case 0:
            case 1:
            case 2:
                {
                    num = 5;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            default:
                break;
        }
    }
    //30P16A
    else if (probeindex == 5)
    {
        switch(thi)
        {
            case 0:
                {
                    num = 6;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 1:
            case 2:
                {
                    num = 7;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            default:
                break;
        }
    }
    //35D40J
    else if(probeindex == 8)
    {
        switch(thi)
        {
            case 0:
                {
                    num = 3;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;

            case 1:
                {
                    num = 5;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;

            case 2:
                {
                    num = 4;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;

            default:
                {
                    num = 1;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
        }

    }
    else
    {
        num = 1;
        m_ptrCalc->ChangeBPFilter(num);
    }
#endif
}

void Img2D::DefaultIndexBandPassFilterBaseFreq(int probeindex, int freqindex)
{
    int num = 0;
#if (defined(EMP_430))
    if(probeindex == 0)
    {//35C60E
        switch(freqindex)
        {
            case 0:
                num = 6;
                m_ptrCalc->ChangeBPFilter(num);
                break;
            case 1:
                num = 5;
                m_ptrCalc->ChangeBPFilter(num);
                break;
            case 2:
                num = 4;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else if(probeindex == 1)
    {//65L40E
        switch(freqindex)
        {
            case 0:
            case 2:
                num = 3;
                m_ptrCalc->ChangeBPFilter(num);
                break;
            case 1:
                num = 4;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else if(probeindex == 2)
    {//65C10E
        switch(freqindex)
        {
            case 0:
            case 1:
            case 2:
                num = 1;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else if(probeindex == 3)
    {
        switch(freqindex)
        {
            case 0:
            case 1:
                num = 4;
                m_ptrCalc->ChangeBPFilter(num);
                break;
            case 2:
                num = 3;
                m_ptrCalc->ChangeBPFilter(num);
                break;
        }
    }
    else
    {
        m_ptrCalc->ChangeBPFilter(1);
    }
#elif EMP_161
    if(probeindex == 0)
    {//3.5CV
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            case 4:
                num = 4;
                break;
            default:
                num = 5;
                break;
        }
    }
    else if(probeindex == 1)
    {//7.5LVS
        switch(freqindex)
        {
            case 0:
            case 1:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 2)
    {//6.5VMC
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            case 3:
                num = 5;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 3)
    {//3.5MC
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            case 3:
                num = 5;
                break;
            default:
                num = 4;
                break;
        }
    }
    else
    {
        num = 1;
    }
    m_ptrCalc->ChangeBPFilter(num);

#elif EMP_440
    if(probeindex == 0)
    {//3.5CV
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            default:
                num = 5;
                break;
        }
    }
    else if(probeindex == 1)
    {//7.5LVS
        switch(freqindex)
        {
            case 0:
            case 1:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 2)
    {//6.5VMC
        switch(freqindex)
        {
            case 3:
                num = 5;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 3)
    {//
        num = 4;
    }
    else
    {
        num = 1;
    }
    m_ptrCalc->ChangeBPFilter(num);

#elif EMP_360
    if(probeindex == 0)
    {//3.5CV
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            case 4:
                num = 4;
                break;
            default:
                num = 5;
                break;
        }
    }
    else if(probeindex == 1)
    {//7.5LVS
        switch(freqindex)
        {
            case 0:
            case 1:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 2)
    {//6.5VMC
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            case 3:
                num = 5;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 3)
    {//3.5MC
        switch(freqindex)
        {
            case 0:
                num = 6;
                break;
            case 3:
                num = 5;
                break;
            default:
                num = 4;
                break;
        }
    }
    else if(probeindex == 4)
    {//6.5MC
        num = 4;
    }
    else if(probeindex == 5)
    {//30P16A
        switch(freqindex)
        {
            case 0:
            case 3:
            case 4:
                    num = 6;
                break;
            case 1:
            case 2:
                    num = 7;
                break;
            case 5:
                    num = 5;
                break;
            default:
                num = 1;
                break;
        }
    }
    else
    {
        num = 1;
    }
    m_ptrCalc->ChangeBPFilter(num);
#else
    //35C50K
    if(probeindex == 0)
    {
        num = 3;
        m_ptrCalc->ChangeBPFilter(num);
    }
    //75L40K
    else if(probeindex == 1)
    {
        switch(freqindex)
        {
            case 0:
                {
                    num = 6;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 1:
                {
                    num = 3;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 2:

                {
                    num = 4;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 3:
                {
                    num = 5;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            default:
                break;
        }
    }
    //65C10K
    else if(probeindex == 2)
    {
        switch(freqindex)
        {
            case 1:
            case 2:
                num = 3;
                break;
            default:
                num = 4;
                break;
        }
        m_ptrCalc->ChangeBPFilter(num);
    }
    //65C15D
    else if(probeindex == 4)
    {
        num = 5;
        m_ptrCalc->ChangeBPFilter(num);
    }
    //30P16A
    else if(probeindex == 5)
    {
        switch(freqindex)
        {
            case 0:
            case 3:
            case 4:
                {
                    num = 6;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 1:
            case 2:
                {
                    num = 7;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            case 5:
                {
                    num = 5;
                    m_ptrCalc->ChangeBPFilter(num);
                }
                break;
            default:
                break;
        }
    }
    else if(probeindex == 8)
    {//35D40J
        switch(freqindex)
        {
            case 0:
            case 1:
            case 2:
                num = 4;
                break;
            case 3:
                num = 5;
                break;
            case 4:
                num = 6;
                break;
            default:
                num = 4;
                break;
        }
        m_ptrCalc->ChangeBPFilter(num);
    }
    else
    {
        num = 1;
        m_ptrCalc->ChangeBPFilter(num);
    }
#endif
}

// for 4d tgc
int Img2D::GetImgScaleFor4D()
{
	return m_calcPara.imgScale;
}

int Img2D::GetDepthMaxFor4D()
{
	return m_calcPara.depthMax;
}

int Img2D::GetGainFor4D()
{
    return m_gain2D;
}

int* Img2D::GetTGCFor4D()
{
    return m_tgc;
}

// get gain from 4d by zjj 2015-1-26
void Img2D::SetGainFrom4D(int gain)
{  
    m_gain2D = gain;
    m_ptrUpdate->Gain2D(m_gain2D);
}

void Img2D::SetTGCFrom4D(int *tgc)
{    
    ChangeTgc2D(tgc); // if not enter2D
    //memcpy(m_tgc, tgc, 8*sizeof(int)); 
}

// set 4d depth by zjj 2015-1-26 
void Img2D::Set4DDepth(int depth_scale)
{
    Depth(depth_scale);
    m_imgScaleIndex = depth_scale;
}

#ifdef EMP_PROJECT
int Img2D::GetFreq()
{
    return (m_vecFreqRange[m_freqIndex].emit);
}

int Img2D::GetHarmonicFreq()
{
    return (m_thiFreq[m_harmonicFreqIndex]);
}
#endif

void Img2D::UpdateGain()       //用来解决调节M增益后，进入CFM时，增益错误的问题
{	
	m_ptrCalc->CalcTgc(m_gain2D*MAX_GAIN_2D/100, m_tgc, m_ptrUpdate, 0);
	m_ptrCalc->CalcTgcDigital(m_gain2D*MAX_GAIN_2D/100, 0, MAX_GAIN_2D);

    // m gain
	m_ptrCalc->CalcTgc(m_gainM*MAX_GAIN_M/100, m_tgc, m_ptrUpdate, 1);
    //fpga中处理M和2D 的数字增益用的是一组数据
    if(ModeStatus::IsMImgMode())
        m_ptrCalc->CalcTgcDigital(m_gainM*MAX_GAIN_M/100, 1, MAX_GAIN_M);
}

void Img2D::EnhanceBiopsy(bool on)
{
    if(on)
    {
        m_edgeEnhanceIndexBak = m_edgeEnhanceIndex;
        m_edgeEnhanceIndex = MAX_EDGE - 1;
    }
    else
    {
    
        m_edgeEnhanceIndex = m_edgeEnhanceIndexBak;
    }
    m_calcPara.edgeEnhance = m_edgeEnhanceIndex;
    m_ptrDscPara->dca2DIPAttrs.ipaEdgeEhn = m_edgeEnhanceIndex;
}
