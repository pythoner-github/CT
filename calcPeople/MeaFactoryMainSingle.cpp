#include "MeasureFactory.h"
#include "Measure2D.h"
#include "MeasureM.h"
#include "MeasureD.h"
#include "MeaCalcFun.h"
#include "MeaResult.h"
#include "MeaFactoryMainSingle.h"
#include "ViewCalcDialog.h"
#include "ImgProcPw.h"
#include <stdio.h>
#include "../display/HintArea.h"
#include "SysMeasurementSetting.h"
#include "DrawHistogram.h"
#include "MenuCalcNew.h"

MeaFactoryMainSingle* MeaFactoryMainSingle::m_ptrInstance = NULL;

MeaFactoryMainSingle::MeaFactoryMainSingle()
{
    	m_type = 0;
}

MeaFactoryMainSingle::~MeaFactoryMainSingle()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

MeaFactoryMainSingle* MeaFactoryMainSingle::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new MeaFactoryMainSingle;

	return m_ptrInstance;
}

int MeaFactoryMainSingle::GetType()
{
	return m_type;
}

int MeaFactoryMainSingle::SectionItem(int item, int *itemStart, const SingleItemInfo **ptrSingleItemInfo, const MultiItemInfo **ptrMultiItemInfo)
{
	if ((item >= ABD_MEA_START)&&(item < ABD_MEA_END))
	{
		*ptrSingleItemInfo = AbdoInfo;
		*ptrMultiItemInfo = AbdoMultiInfo;
		*itemStart = ABD_MEA_START;
		return ABDO_M;
	}	
	else if ((item >= ADULT_MEA_START)&&(item < ADULT_MEA_END))
	{
		*ptrSingleItemInfo = AdultInfo;
		*ptrMultiItemInfo = AdultMultiInfo;
		*itemStart = ADULT_MEA_START;
		return ADULT_M;
	}
	else if ((item >= UR_MEA_START)&&(item < UR_MEA_END))
	{
		*ptrSingleItemInfo = URInfo;
		*ptrMultiItemInfo = URMultiInfo;
		*itemStart = UR_MEA_START;
		return UR_M;
	}
	else if ((item >= OB_MEA_START)&&(item < OB_MEA_END))
	{
		*ptrSingleItemInfo = OBInfo;
		*ptrMultiItemInfo = OBMultiInfo;
		*itemStart = OB_MEA_START;
		return OB_M;
	}
#ifndef VET
	else if ((item >= GYN_MEA_START)&&(item < GYN_MEA_END))
	{
		*ptrSingleItemInfo = GYNInfo;
		*ptrMultiItemInfo = GYNMultiInfo;
		*itemStart = GYN_MEA_START;
		return GYN_M;
	}
#endif
	else if ((item >= SP_MEA_START)&&(item < SP_MEA_END))
	{
		*ptrSingleItemInfo = SPInfo;
		*ptrMultiItemInfo = SPMultiInfo;
		*itemStart = SP_MEA_START;
		return SP_M;
	}
	else if ((item >= FETAL_MEA_START)&&(item < FETAL_MEA_END))
	{
		*ptrSingleItemInfo = FetalInfo;
		*ptrMultiItemInfo = FetalMultiInfo;
		*itemStart = FETAL_MEA_START;
		return FETAL_M;
	}
#ifdef EMP_322
#else
	else if ((item >= VS_MEA_START)&&(item < VS_MEA_END))
	{
		*ptrSingleItemInfo = VSInfo;
		*ptrMultiItemInfo = VSMultiInfo;
		*itemStart = VS_MEA_START;
		return VS_M;
	}
	else if ((item >= TCD_MEA_START)&&(item < TCD_MEA_END))
	{
		*ptrSingleItemInfo = TCDInfo;
		*ptrMultiItemInfo = TCDMultiInfo;
		*itemStart = TCD_MEA_START;
		return TCD_M;
	}
#endif
    else if ((item >= ORTHO_MEA_START)&&(item < ORTHO_MEA_END))
    {
        *ptrSingleItemInfo = OrthoInfo;
        *ptrMultiItemInfo = OrthoMultiInfo;
        *itemStart = ORTHO_MEA_START;
        return ORTHO_M;
    }
    else if((item >= USER_START) && (item < USER_END))
    {
        int a(0);
        //*ptrSingleItemInfo =(const SingleItemInfo *)userDef_new;
        *ptrSingleItemInfo = userDef_new;
        *itemStart = USER_START;
        return USER_M;
    }
#ifdef VET
//添加 动物超 TD
	else if ((item >= TD_MEA_START)&&(item < TD_MEA_END))
	{
        *ptrSingleItemInfo = TDInfo;
        *ptrMultiItemInfo = TDMultiInfo;
        *itemStart = TD_MEA_START;
		return TD_M;
	}
//添加 动物超 产科
	else if ((item >= ANOB_MEA_START)&&(item < ANOB_MEA_END))
	{
        *ptrSingleItemInfo = AnOBInfo;
		*ptrMultiItemInfo = AnOBMultiInfo;
		*itemStart = ANOB_MEA_START;
		return ANOB_M;
	}
#else
    else
        return SECTION_END;
#endif
}

#include <stdio.h>
AbsMeasure* MeaFactoryMainSingle::Create(int type)
{
	unsigned int parError = 0;
	MeaResult *ptrMeaResult;
	int offsetS, offsetM;

	if(g_ptrAbsMeasure != NULL) 
	{
		delete g_ptrAbsMeasure;
		g_ptrAbsMeasure = NULL;
	}
	
	//自动计算打开时，禁止测量
	bool autoCalc = ImgProcPw::GetInstance()->GetAutoCalc();
	if  (autoCalc)
	{
		HintArea::GetInstance()->UpdateHint(_("Please turn off auto calc!"), 1);
		return g_ptrAbsMeasure;
	}
	
	MeasureFactory::EMeasureMode currMeaMode;
	currMeaMode = MeasureFactory::GetInstance()->JudgeMode();

	m_type = type;
	ptrMeaResult = MeaResult::GetInstance();
//检查所选择测量的模式和当前扫描模式是否相符	
	ModeStatus s;
//	int mode = s.GetScanMode();

	const SingleItemInfo *ptrSingleItemInfo;
	const MultiItemInfo *ptrMultiItemInfo;
	int itemStart;
	int section;

	section = SectionItem(type, &itemStart, &ptrSingleItemInfo, &ptrMultiItemInfo);
	offsetS = ptrMeaResult -> FindSingleMeaInfo(section, type);
	if (offsetS == -1)
	{
		return g_ptrAbsMeasure;
	}

    if ((ptrSingleItemInfo + offsetS)->unitItem > UNIT_END)//有多项测量的测量项
    {
        offsetM = ptrMeaResult -> FindDoubleMeaInfo(section, (ptrSingleItemInfo + offsetS)->unitItem);
        ptrMultiItemInfo = ptrMultiItemInfo + offsetM;
        parError = NewMultiMeasure(ptrMultiItemInfo, currMeaMode);
    }
	else
	{
		ptrSingleItemInfo = ptrSingleItemInfo + offsetS;
		parError = NewSingleMeasure(ptrSingleItemInfo, currMeaMode);
	}

	if (g_ptrAbsMeasure)
		MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::MEASURE);
	else
	{
		if (parError == 0)
			HintArea::GetInstance()->UpdateHint(_("[Calc]: Calculation selected is invalid in current mode."), 2);
	}

	return g_ptrAbsMeasure;
}

unsigned int MeaFactoryMainSingle::NewSingleMeasure(const SingleItemInfo *ptrSingleItemInfo, MeasureFactory::EMeasureMode currMeaMode)
{	
    unsigned int parError = 0;
	switch (ptrSingleItemInfo->meaType)
	{
        case DIST_LINE:
            if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
                g_ptrAbsMeasure = new D2MeasureDistLine(ptrSingleItemInfo);
            else
                g_ptrAbsMeasure = NULL;
            break;

        case DEPTH_DIST:
        case DIST_DOT:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureDistDot(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
        
        case LENGTH_TRACK:
            if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
                g_ptrAbsMeasure = new D2MeasureLengthTrack(ptrSingleItemInfo);
            else
                g_ptrAbsMeasure = NULL;
            break;

        case LENGTH_DOT:
            if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
                g_ptrAbsMeasure = new D2MeasureLengthDot(ptrSingleItemInfo);
            else
                g_ptrAbsMeasure = NULL;
            break;

        case INTEGRAL_TRACK:
            if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
                g_ptrAbsMeasure = new DMeasureIntegralTrack(ptrSingleItemInfo);
            else
                g_ptrAbsMeasure = NULL;
            break;

		case AREA_TRACK:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaTrack(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case AREA_DOT:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaDot(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case AREA_REC:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaRec(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
#if 0        
        case AREA_ELLIPSE:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolEllipse1(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
#endif
		case PERI_TRACK:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaTrack(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
		
		case PERI_ELLIPSE:
		case AREA_ELLIPSE:
		case VOL_ELLIPSE1:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolEllipse1(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
        case VOL_3AXIS:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
                //  g_ptrAbsMeasure = new D2MeasureVol3Axis(NULL, D2_VOLUME_3DIS,ptrSingleItemInfo);
                g_ptrAbsMeasure = new D2MeasureVol3Axis(NULL,ptrSingleItemInfo);
            else
				g_ptrAbsMeasure = NULL;
			break;

        case VOL_ELLIPSE2:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolEllipse2(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case ANGLE_3DOT:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAngleSingle(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
        case ANGLE_2LINE:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAngle(1,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case ANGUSTY_DIST_DOT:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioDistDot(0,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case RATIO_DIST_DOT:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioDistDot(1,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case ANGUSTY_AREA:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioArea(0,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case RATIO_AREA:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
			 g_ptrAbsMeasure = new D2MeasureRatioArea(1,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case RATIO_D_P:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioDistPeri(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case RATIO_VOL:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioArea(2,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case RATIO_ANGLE:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioAngle(&BasicMultiInfo[D2_RATIO_ANGLE_MULTI-BASIC_MULTI_START]);
            else
				g_ptrAbsMeasure = NULL;
			break;

		case DEPTH_DIST_M:
			if ((currMeaMode == MeasureFactory::MEA_M) || (currMeaMode == MeasureFactory::MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureDepth(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
#if 0      
        case DEPTH_DIST:
			if ((currMeaMode == MeasureFactory::MEA_M) || (currMeaMode == MeasureFactory::MEA_2D_M))
				g_ptrAbsMeasure = new D2MeasureDistDot(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;
#endif
	
        case EF:
			if ((currMeaMode == MeasureFactory::MEA_M) || (currMeaMode == MeasureFactory::MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureEFTei(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case PROFILE:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureProfile();
			else
				g_ptrAbsMeasure = NULL;
			break;

        case HISTOGRAM:
			if (currMeaMode == MeasureFactory::MEA_2D )
            {
                if (DrawHistogram::GetInstance()->GetOnOff())
                {
                    DrawHistogram::GetInstance()->SetOnOff(0);
                    g_ptrAbsMeasure = NULL;
                    parError = 3;
                }
                else
                {
                    g_ptrAbsMeasure = new D2MeasureHistogram();
                }
            }
			else
				g_ptrAbsMeasure = NULL;
			break;

	
		case TIME_M:
		case HR_M:
			if ((currMeaMode == MeasureFactory::MEA_M) || (currMeaMode == MeasureFactory::MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureTime(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case VEL_M:
        case SLOPE:
			if ((currMeaMode == MeasureFactory::MEA_M) || (currMeaMode == MeasureFactory::MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureVel(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case VEL_D:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureVel(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case MEASURE_TRACK:
            {          
                SysMeasurementSetting sys;
                int manualMethod;

                if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
                {
                    manualMethod = sys.GetTraceMethod();
                    if (manualMethod == 1)
                    {
                        g_ptrAbsMeasure = new DMeasureManualTrack();         
                    }
                    else if (manualMethod == 0)
                    {
                        g_ptrAbsMeasure = new DMeasureManualDot();
                    }
                    else
                    {
                        if (MeasureFactory::GetInstance()->JudgeFreeze() == FreezeMode::FREEZE)
                            g_ptrAbsMeasure = new DMeasureManualAuto();
                        else
                        {
                            parError = 2;
                            g_ptrAbsMeasure = NULL;
                        }
                    }
                }
                else 
                    g_ptrAbsMeasure = NULL;
            }
			break;
		case TIME_D:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureTime(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case ACCEL:
        case SLOPE_D:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureAccel(ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case PGMEAN:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasurePI(1,ptrSingleItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

	//	case DIALOG_BOX:
	//		if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
	//			ViewCalcDialog::GetInstance()->Create(ViewCalcDialog::CAV);
	//		break;

		default:
			g_ptrAbsMeasure = NULL;
			parError = 1;
			break;
	}
	return parError;
}

unsigned int MeaFactoryMainSingle::NewMultiMeasure(const MultiItemInfo *ptrMultiItemInfo, MeasureFactory::EMeasureMode currMeaMode)
{
	unsigned int parError = 0;
	switch (ptrMultiItemInfo->meaType)
	{
		case VOL_SPHEROID:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolSpheroid(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case SIMPSONS:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureSimpson(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case AL:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAL(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case PSED:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
			//g_ptrAbsMeasure = new DMeasureRI(meaInfo[0].title, multiItem, 0);
				g_ptrAbsMeasure = new DMeasureRI(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case RI:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureRI(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case SD:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureRI(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case HR_D:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureHR(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case PGMAX:
            if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasurePGmax(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case P12T:
			if ((currMeaMode == MeasureFactory::MEA_PW) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureP12t(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case DIST_SLOPE_M:
			if ((currMeaMode == MeasureFactory::MEA_M) || (currMeaMode == MeasureFactory::MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureDistSlope(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case HIP:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureHip(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case SAC_3AXIS:
			if ((currMeaMode == MeasureFactory::MEA_2D) || (currMeaMode == MeasureFactory::MEA_2D_M) || (currMeaMode == MeasureFactory::MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureSac3Axis(ptrMultiItemInfo);
			else
				g_ptrAbsMeasure = NULL;
			break;

		default:
			g_ptrAbsMeasure = NULL;
			parError = 1;
			break;
	}
	return parError;
}
