#include "MeasureFactory.h"
#include "Measure2D.h"
#include "MeasureM.h"
#include "MeasureD.h"
#include "DrawHistogram.h"
#include "MeaCalcFun.h"
//#include "ModeStatus.h"
#include "SysMeasurementSetting.h"
#include "ImgProcPw.h"
#include "../display/HintArea.h"
#include "../keyboard/KeyFunc.h"
#include "../imageProc/ImgProc2D.h"

AbsMeasure* g_ptrAbsMeasure = NULL;

MeasureFactory* MeasureFactory::m_ptrInstance = NULL;

MeasureFactory::MeasureFactory()
{
	m_type = 0;
}
MeasureFactory::~MeasureFactory()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}
MeasureFactory* MeasureFactory::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new MeasureFactory;

	return m_ptrInstance;
}

int MeasureFactory::GetType()
{
	return m_type;
}

void MeasureFactory::UndoLastMeasure()
{
	if(MeasureMan::GetInstance()->GetDequeSize() > 0)
	{
		MeasureMan::GetInstance()->ClearLast();
		UpdateMeasure update;
		update.ClearLast();
	}
}

MeasureFactory::EMeasureMode MeasureFactory::JudgeMode(void)
{
	ModeStatus getMode;
	ScanMode::EScanMode currMode;

	currMode = getMode.GetScanMode();
	switch (currMode)
	{
		case ScanMode::D2:
		case ScanMode::M_INIT:
		case ScanMode::PW_INIT:
		case ScanMode::CW_INIT:
		case ScanMode::CFM:
		case ScanMode::PDI:
		case ScanMode::PWCFM_INIT:
		case ScanMode::PWPDI_INIT:
		case ScanMode::CFM_VS_2D:
		case ScanMode::PDI_VS_2D:
			return MEA_2D;
			break;

		case ScanMode::M:
		case ScanMode::ANATOMIC_M:
			{
				FormatM::EFormatM currMFormat;
				currMFormat = getMode.GetFormatM();
				if (currMFormat == FormatM::M_TOTAL)
					return MEA_M;
				else
					return MEA_2D_M;
			}
			break;

		case ScanMode::PW:
		case ScanMode::CW:
		case ScanMode::PWCFM:
		case ScanMode::PWPDI:
        case ScanMode::CWCFM:
        case ScanMode::CWPDI:
            {
				FormatPw::EFormatPw currPwFormat;
				currPwFormat = getMode.GetFormatPw();
				if (currPwFormat == FormatPw::P_TOTAL)
					return MEA_PW;
				else
					return MEA_2D_PW;
			}
			break;
		
		case ScanMode::PW_SIMULT:
		case ScanMode::PWPDI_SIMULT:
		case ScanMode::PWCFM_SIMULT:
			return MEA_2D_PW;
			break;

		default:
			return MEA_2D;
			break;
	}
	return MEA_2D;
}

FreezeMode::EFreezeMode MeasureFactory::JudgeFreeze(void)
{
	ModeStatus getStatus;

	return(getStatus.GetFreezeMode());
}


AbsMeasure* MeasureFactory::Create(EMeasureFactory type)
{
	unsigned int parError = 0;

	if(g_ptrAbsMeasure != NULL) 
	{
		delete g_ptrAbsMeasure;
		g_ptrAbsMeasure = NULL;
	}
	//自动计算打开时，禁止测量
	bool autoCalcStatus = ImgProcPw::GetInstance()->GetAutoCalc();
    if (autoCalcStatus)
	{
		HintArea::GetInstance()->UpdateHint(_("Please turn off auto calc!"), 1);
		return g_ptrAbsMeasure;
	}

	//检查所选择测量的模式和当前扫描模式是否相符	
	EMeasureMode currMeaMode;
	currMeaMode = JudgeMode();
    ModeStatus ms;
    int mode =  ms.GetScanMode();
    int rotate = 0;
    rotate = ImgProc2D::GetInstance()->GetRotate();

	m_type = type;
	switch(type)
	{
		case D2_DIST_DOT:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureDistDot(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_DIST_LINE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureDistLine(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_LEN_TRACK: 
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureLengthTrack(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_LEN_DOT:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureLengthDot(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_AREA_TRACK:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaTrack(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_AREA_DOT:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaDot(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_AREA_ELLIPSE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolEllipse1(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_AREA_RECTANGLE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAreaRec(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_VOLUME_3DIST:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
			//g_ptrAbsMeasure = new D2MeasureVol3Axis(NULL, D2_VOLUME_3DIST);
            g_ptrAbsMeasure = new D2MeasureVol3Axis(NULL, &BasicInfo[type - BASIC_MEA_START]);

			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_VOLUME_ELLIPSE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolEllipse1(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_VOLUME_ELLIPSE_1DIST:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolEllipse2(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_VOLUME_1DIST:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureVolSpheroid(&BasicMultiInfo[D2_VOLUME_1DIST_MULTI-BASIC_MULTI_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_ANGLE_DOT:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAngle(0,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_ANGLE_LINE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureAngle(1,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_DEPTH:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureDistDot(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

	//	case D2_SIMPSONS:
	//		g_ptrAbsMeasure = new D2MeasureSimpsons(&BasicInfo[type - BASIC_MEA_START]);	

			//========================特殊测量
		case D2_ANGUSTY_DIST:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioDistDot(0,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_RATIO_DIST:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioDistDot(1,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_RATIO_D_P://长度周长比
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioDistPeri(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_ANGUSTY_AREA:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioArea(0,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_RATIO_AREA:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioArea(1,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_RATIO_VOL:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioArea(2,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_RATIO_ANGLE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureRatioAngle(&BasicMultiInfo[D2_RATIO_ANGLE_MULTI-BASIC_MULTI_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_EF:
			if ((currMeaMode == MEA_M) || (currMeaMode == MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureEFTei(&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_PROFILE:
			if ((currMeaMode == MEA_2D) || (currMeaMode == MEA_2D_M) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new D2MeasureProfile();
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D2_HISTOGRAM:
			if (currMeaMode == MEA_2D)
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

			//hlx
        case D2_IMT_ABOVE:
            if (mode == ScanMode::D2)
            {
                if (rotate == 0)
                {
                    if (currMeaMode == MEA_2D)  
                    {
                        g_ptrAbsMeasure = new D2MeasureIMT(false);
                    }
                    else
                        g_ptrAbsMeasure = NULL;  
                }
                else
                { 
                    HintArea::GetInstance()->UpdateHint(_("Invalid when image is rotated!"), 1);
                    parError = 1;
                    break;
                }
            }
            else
            {
                g_ptrAbsMeasure = NULL; 
            }
            break;

        case D2_IMT_BELOW:
            if (mode == ScanMode::D2)
            {
                if (rotate == 0)
                {
                    if (currMeaMode == MEA_2D)  
                    {
                        g_ptrAbsMeasure = new D2MeasureIMT(true);
                    }
                    else
                        g_ptrAbsMeasure = NULL;  
                }
                else
                { 
                    HintArea::GetInstance()->UpdateHint(_("Invalid when image is rotated!"), 1);
                    parError = 1;
                        break;
                }
            }
            else
            {
                g_ptrAbsMeasure = NULL; 
            }
            break;

            //========================M模式测量
        case M_TIME:
            if ((currMeaMode == MEA_M) || (currMeaMode == MEA_2D_M))
                g_ptrAbsMeasure = new MMeasureTime(&BasicInfo[type - BASIC_MEA_START]);	
            else
                g_ptrAbsMeasure = NULL;
            break;

		case M_VEL:
			if ((currMeaMode == MEA_M) || (currMeaMode == MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureVel(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case M_DEPTH:
			if ((currMeaMode == MEA_M) || (currMeaMode == MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureDepth(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case M_HR:
			if ((currMeaMode == MEA_M) || (currMeaMode == MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureTime(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case M_SLOPE:
			if ((currMeaMode == MEA_M) || (currMeaMode == MEA_2D_M))
				g_ptrAbsMeasure = new MMeasureVel(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

			//=========================D模式测量			
		case D_VEL:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureVel(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_RI:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureRI(&BasicMultiInfo[D_RI_MULTI-BASIC_MULTI_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_SD:
            if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
                g_ptrAbsMeasure = new DMeasureRI(&BasicMultiInfo[D_SD_MULTI-BASIC_MULTI_START]);
            else
                g_ptrAbsMeasure = NULL;
            break;

		case D_TIME:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureTime(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_ACCEL:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureAccel(&BasicInfo[type - BASIC_MEA_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_PI:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasurePI(0,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_HR:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasureHR(&BasicMultiInfo[D_HR_MULTI-BASIC_MULTI_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_PGMAX:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasurePGmax(&BasicMultiInfo[D_PGMAX_MULTI-BASIC_MULTI_START]);	
			else
				g_ptrAbsMeasure = NULL;
			break;

		case D_PGMEAN:
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
				g_ptrAbsMeasure = new DMeasurePI(1,&BasicInfo[type - BASIC_MEA_START]);
			else
				g_ptrAbsMeasure = NULL;
			break;

        case D_AUTO_TRACK:
            {
                if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
                {
                    if (JudgeFreeze() == FreezeMode::FREEZE)
                        g_ptrAbsMeasure = new DMeasureManualAuto();
                    else
                    {
                        parError = 2;
                        g_ptrAbsMeasure = NULL;
                    }
                }
                else
                    g_ptrAbsMeasure = NULL;
            }
            break;

		case D_MANUAL_TRACK:
		{
			SysMeasurementSetting sys;
			int manualMethod;
			
			if ((currMeaMode == MEA_PW) || (currMeaMode == MEA_2D_PW))
			{
				manualMethod = sys.GetTraceMethod();
				if (manualMethod == 1)
				{
                    if (JudgeFreeze() == FreezeMode::FREEZE || JudgeFreeze() == FreezeMode::REPLAY)
                    {
                        g_ptrAbsMeasure = new DMeasureManualTrack();
                    }
                    else
                    {
                        parError = 2;
                        g_ptrAbsMeasure = NULL;
                    }
				}
				else if (manualMethod == 0)
					g_ptrAbsMeasure = new DMeasureManualDot();
				else
				{
					if (JudgeFreeze() == FreezeMode::FREEZE)
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

		default:
			g_ptrAbsMeasure = NULL;
			parError = 1;
			break;
	}

	if (g_ptrAbsMeasure)
		MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::MEASURE);
	else
	{
		if (parError == 0)
			HintArea::GetInstance()->UpdateHint(_("[Measure]: Measurement selected is invalid in current mode."), 2);
		else if(parError == 2)
			HintArea::GetInstance()->UpdateHint(_("[Measure]: Measurement selected is only valid in freezing mode."), 2);
	}

	return g_ptrAbsMeasure;
}


