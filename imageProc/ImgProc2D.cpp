#include <math.h>
#include "ImgProc2D.h"
#include "ImgProcCfm.h"
#include "Def.h"
#include "DscMan.h"
#include "GlobalClassMan.h"
#include "ChromaTable.h"
#include "KeyFunc.h"
#include "KnobMenu.h"
#include "KnobEFOV.h"
#include "LightDef.h"
#define eps 0.000000001
ImgProc2D* ImgProc2D::m_ptrInstance = NULL;

const string ImgProc2D::CHROMA_TYPE[MAX_CHROMA] = 
{
	//_("Off"), _("Purple"), _("RedYellow"), _("Cyan"), _("Khaki"), _("Marron"), _("Yellow"), _("Golden")
	N_("OFF"), ("1"), ("2"), ("3"), ("4"), ("5"), ("6"), ("7")
};

const char * EFOVROISizeName[3] =
{
    _("Small"),
    _("Mid"),
    _("Big"),
};

const int ImgProc2D::COLOR_MAP_CHROM[MAX_CHROMA] = 
{
	GRAYMAP, SIEMENS_PURPLE, SIEMENS_HYACINTH, SIEMENS_CYANINE, SIEMENS_KHAKI, ROTBRAUM, HYACINTH, BLOND//, SIXCOLOR
};
const int ImgProc2D::Rotate[MAX_ROTATE] = {0, 90, 180, 270};
const int ImgProc2D::FrameAver[MAX_FRAME] = {64, 56, 48, 40, 32, 28, 24, 16};
const int ImgProc2D::LineAver[MAX_LINE] = {64, 56, 48, 44, 40, 32, 24, 16};
const double ImgProc2D::Gamma[MAX_GAMMA] = {0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3};
const float ImgProc2D::m_zoomEFOVScale[MAX_EFOV_ZOOM_INDEX] = {0.6, 0.8, 1.0, 1.2, 1.5, 2.0, 4.0};
const double ImgProc2D::TRANS_CURVE_RATIO[MAX_TRANS_CURVE] = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0};// 4.32, 4.42, 4.52, 4.72, 4.92, 4.12, 4.32, 4.46};
///> /////////////////////////////////[public func]////////////////////////////

ImgProc2D* ImgProc2D::GetInstance()
{
	if(m_ptrInstance == NULL)
	{
		m_ptrInstance = new ImgProc2D;
	}

	return m_ptrInstance;
}

///> construct
ImgProc2D::ImgProc2D()
{
	m_LR = m_UD = m_Polarity = false;
	m_FrameAver = 4;
	m_FrameAverBak = m_FrameAver;
	m_LineAver = 1;
	m_LineAverBak = m_LineAver;
	m_Smooth = 0;
	m_NoiseReject = false;
	m_Gamma = 4;
	m_Chroma = 0;
    m_ChromaBak = m_Chroma;
	m_rotateEFOV = 0;
	m_zoomEFOVBox.left = m_zoomEFOVBox.top = m_zoomEFOVBox.right = m_zoomEFOVBox.bottom = 0;
	m_zoomEFOVIndex = 2;
	m_zoomEFOVPos = 0;
    m_CurCurveIndex = 1;

	m_RejectPoint = 0;
	m_biopsyEnhance = false;
	InitColorTable();

	m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
	m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();
        m_roiEFOVSize = MID;
}

ImgProc2D::~ImgProc2D()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

/*
 * @brief set the status for LR overturn
 *
 * @param status true or false
 */
void ImgProc2D::ChangeLR()
{
	PRINTF("lr turn \n");
	if (m_LR)
		m_LR = FALSE;
	else
		m_LR = TRUE;
#if (defined (EMP_322) || defined(EMP_313))
    g_keyInterface.CtrlLight(m_LR,LIGHT_LEFTRIGHT);
#endif
	//set to DSC
	SetLR(m_LR);
}

/*
 * @brief set the status for LR overturn
 *
 * @param status true or false
 */
void ImgProc2D::ChangeUD()
{
	PRINTF("ud turn \n");
	if (m_UD)
		m_UD = FALSE;
	else
		m_UD = TRUE;

    //set to DSC
#if (defined (EMP_322) || defined(EMP_313))
    g_keyInterface.CtrlLight(m_UD,LIGHT_UPDOWN);
#endif
    SetUD(m_UD);
}

/*
 * @brief set the status for LR overturn
 *
 * @param status true or false
 */
void ImgProc2D::ChangePolarity(bool status)
{
	m_Polarity = status;

	//set to DSC
	SetPolarity(m_Polarity);
}

/*
 * @brief change noise reject state
 */
void ImgProc2D::ChangeNoiseReject(bool status)
{
	m_NoiseReject = status;
	m_ptrUpdate->NoiseReject(m_NoiseReject);

	SetNoiseReject(m_NoiseReject);
}
/*
 * @brief change angle of image rotation 
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeRotate(enum EKnobOper oper)
{
	if(oper == ADD && m_Rotate < MAX_ROTATE)
		m_Rotate ++;
	else if(oper == SUB && m_Rotate > 0 )
		m_Rotate --;
	else
		return ERROR;

	EKnobReturn ret;
	if(m_Rotate == (MAX_ROTATE-1))
	{
		ret = MAX;
	}
	else if(m_Rotate == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//set FrameAver to DSC
	SetRotate(m_Rotate, ret);
	return ret;
}

/*
 * @brief change frame correlation level
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeFrameAver(enum EKnobOper oper)
{
	if(oper == ADD && m_FrameAver < MAX_FRAME)
		m_FrameAver ++;
	else if(oper == SUB && m_FrameAver > 0 )
		m_FrameAver --;
	else
		return ERROR;
	
	EKnobReturn ret;
	if(m_FrameAver == (MAX_FRAME-1))
	{
		 ret = MAX;
	}
	else if(m_FrameAver == 0)
	{
		 ret = MIN;
	}
	else
	{
		 ret = OK;
	}

	//set FrameAver to DSC
	SetFrameAver(m_FrameAver, ret);
	return ret;
}

/*
 * @brief change line correlation level
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeLineAver(enum EKnobOper oper)
{
	if(oper == ADD && m_LineAver < MAX_LINE - 1)
		m_LineAver ++;
	else if(oper == SUB && m_LineAver > 0 )
		m_LineAver --;
	else
		return ERROR;
	
	EKnobReturn ret;
	if(m_LineAver == (MAX_LINE-1))
	{
		 ret = MAX;
	}
	else if(m_LineAver == 0)
	{
		 ret = MIN;
	}
	else
	{
		 ret = OK;
	}

	//set LineAver to DSC
	SetLineAver(m_LineAver, ret);
	return ret;
}

/*
 * @brief change smooth
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeSmooth(enum EKnobOper oper)
{
	if(oper == ADD && m_Smooth < MAX_SMOOTH)
		m_Smooth++;
	else if(oper == SUB && m_Smooth > 0 )
		m_Smooth--;
	else
		return ERROR;
	
	EKnobReturn ret;
	if(m_Smooth== (MAX_SMOOTH-1))
	{
		 ret = MAX;
	}
	else if(m_Smooth== 0)
	{
		 ret = MIN;
	}
	else
	{
		 ret = OK;
	}
	
	//set Smooth to DSC
	SetSmooth(m_Smooth, ret);
	return ret;
}


/*
 * @brief change gray transform
 *
 * @param table: the colormap pointer
 * @param point: the element to the CurveBasePoint, range(0~8)
 * @param oper SUB or ADD 
 
 * @retval EKnobReturn (Attention: the return value is only meaningful to the current point)
 */
bool ImgProc2D::ChangeGrayTransPointX(enum EKnobOper oper)
{
	if(oper == ADD && m_CurrentPoint < (MAX_TRANS_POINT-1))
		m_CurrentPoint ++;
	else if(oper == SUB && m_CurrentPoint > 0)
		m_CurrentPoint --;
	else
		return FALSE;
	
	m_ptrUpdate->GrayTrans(m_ColorTransTable, sizeof(m_ColorTransTable), m_CurrentPoint, m_CurveBasePoint, MAX_TRANS_POINT);

	return TRUE;
}

bool ImgProc2D::ChangeGrayTransPointY(enum EKnobOper oper)
{
	if(oper == ADD && m_CurveBasePoint[m_CurrentPoint] < 255)
		m_CurveBasePoint[m_CurrentPoint] ++;
	else if(oper == SUB && m_CurveBasePoint[m_CurrentPoint] > 0)
		m_CurveBasePoint[m_CurrentPoint] --;
	else
		return FALSE;

	CustomGammaChange(m_CurveBasePoint, m_ColorTransTable);	
	m_ptrUpdate->GrayTrans(m_ColorTransTable, sizeof(m_ColorTransTable), m_CurrentPoint, m_CurveBasePoint, MAX_TRANS_POINT);
	
	ChangeColormap();

	return TRUE;
}

bool ImgProc2D::ResetGrayTrans()
{
    m_CurCurveIndex++;
    // change to next gray trans curve
    if (m_CurCurveIndex >= MAX_TRANS_CURVE)
        m_CurCurveIndex = 0;

    InitColorTable();
    CustomGammaChange(m_CurveBasePoint, m_ColorTransTable);	
    ChangeColormap();
    m_ptrUpdate->GrayTrans(m_ColorTransTable, sizeof(m_ColorTransTable), m_CurrentPoint, m_CurveBasePoint, MAX_TRANS_POINT);
    
    return TRUE;
}

bool ImgProc2D::ChangeGrayReject(enum EKnobOper oper)
{
	if(oper == ADD && m_RejectPoint < 255)
		m_RejectPoint ++;
	else if(oper == SUB && m_RejectPoint > 0)
		m_RejectPoint --;
	else 
		return FALSE;
	
	m_ptrUpdate->GrayReject(m_RejectPoint);

	//set gamma to DSC
	ChangeColormap();

	return TRUE;
}

void ImgProc2D::DisplayGrayTrans(bool on)
{
	if (on)
		m_ptrUpdate->GrayTrans(m_ColorTransTable, sizeof(m_ColorTransTable), m_CurrentPoint, m_CurveBasePoint, MAX_TRANS_POINT);
	else
		m_ptrUpdate->ClearGrayCurve();
}

void ImgProc2D::DisplayGrayReject(bool on)
{
	if (on)
		m_ptrUpdate->GrayReject(m_RejectPoint);
	else
		m_ptrUpdate->ClearGrayCurve();
}
/*
 * @brief change gamma 
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeGamma(enum EKnobOper oper)
{
	if(oper == ADD && m_Gamma < MAX_GAMMA)
		m_Gamma ++;
	else if(oper == SUB && m_Gamma > 0)
		m_Gamma --;
	else 
		return ERROR;

	//set gamma to DSC
	ChangeColormap();

    // update dsc
    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)
    {
        ptrDscMan->GetWriteLock();
        UpdateDsc();
        ptrDscMan->ReadWriteUnlock();
    }
	
    // update view
    EKnobReturn ret;
	if(m_Gamma == MAX_GAMMA-1)
	{
        ret = MAX;
	}
	else if(m_Gamma == 0)
	{
        ret = MIN;
	}
	else
	{
        ret = OK;
    }
    m_ptrUpdate->Gamma(m_Gamma, ret);

    return ret;
}

/*
 * @brief change chroma
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeChroma(enum EKnobOper oper)
{
#ifdef EMP_355
    int index = m_Chroma;
    if(oper == ADD)
    {
        if(index < (MAX_CHROMA -1))
            index ++;
        else
            index = 0;
    }
    else if(oper == SUB)
    {
        if(index > 0)
            index--;
        else
            index = MAX_CHROMA - 1;
    }
    else
        return ERROR;
    m_Chroma = index;
    m_ChromaBak = m_Chroma;
#else
    if(oper == ADD && m_Chroma < (MAX_CHROMA-1))
    {
        m_Chroma ++;
        m_ChromaBak = m_Chroma;
    }
    else if(oper == SUB && m_Chroma > 0)
    {
        m_Chroma --;
        m_ChromaBak = m_Chroma;
    }
	else 
		return ERROR;
#endif
	if (m_Chroma != 0) // chroma is on
		ImgProcCfm::GetInstance()->SetThresholdOff();
	
	EKnobReturn ret;
	if(m_Chroma == MAX_CHROMA-1)
	{
		ret = MAX;
	}
	else if(m_Chroma == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//set Chroma to DSC
	m_ptrUpdate->Chroma(CHROMA_TYPE[m_Chroma].c_str(), ret);
	ChangeColormap();

    // update dsc
    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)
    {
        ptrDscMan->GetWriteLock();
        UpdateDsc();
        ptrDscMan->ReadWriteUnlock();
    }
    return ret;
}

enum EKnobReturn ImgProc2D::PressChroma(void)
{
    if (m_Chroma != 0)
        m_Chroma = 0;
    else
        m_Chroma = m_ChromaBak;

    if (m_Chroma != 0) // chroma is on
        ImgProcCfm::GetInstance()->SetThresholdOff();

	//set Chroma to DSC
	EKnobReturn ret;
	if(m_Chroma == MAX_CHROMA-1)
	{
		ret = MAX;
	}
	else if(m_Chroma == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	m_ptrUpdate->Chroma(CHROMA_TYPE[m_Chroma].c_str(), ret);
	ChangeColormap();

    // update dsc
    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)
    {
        ptrDscMan->GetWriteLock();
        UpdateDsc();
        ptrDscMan->ReadWriteUnlock();
    }

    return PRESS;
}

enum EKnobReturn ImgProc2D::ChangeImgEhn(enum EKnobOper oper)
{
	if(oper == ADD && m_ImgEhn < MAX_IMG_EHN)
		m_ImgEhn ++;
	else if(oper == SUB && m_ImgEhn > 0 )
		m_ImgEhn --;
	else
		return ERROR;
	
	EKnobReturn ret;
	if(m_ImgEhn == (MAX_IMG_EHN-1))
	{
		 ret = MAX;
	}
	else if(m_ImgEhn == 0)
	{
		 ret = MIN;
	}
	else
	{
		 ret = OK;
	}

	//set image ehn to DSC
	SetImgEhn(m_ImgEhn, ret);
	return ret;

}

/*
 * @brief change a series of paras when init probe, change exam item , and auto optimize
 */
void ImgProc2D::InitOptimize(ExamItem::Para2D* d2)
{
    ASSERT(d2 != NULL);
	EKnobReturn ret;

	// LR
	m_LR = d2->leftRight;
	SetLR(m_LR);

	// UD
	m_UD = d2->upDown;
	SetUD(m_UD);
	
	// Polarity
	m_Polarity = d2->polarity;
	SetPolarity(m_Polarity);
	
	// noiseReject
	m_NoiseReject = d2->noiseReject;
	SetNoiseReject(m_NoiseReject);

	// Rotate
	m_Rotate = d2->rotate;
	if(m_Rotate == MAX_ROTATE-1)
		ret = MAX;
	else if(m_Rotate == 0)
		ret = MIN;
	else
		ret = OK;
	SetRotate(m_Rotate, ret);
	
	// frameAver
	m_FrameAver = d2->frameAver;
	if(m_FrameAver == MAX_FRAME-1)
		ret = MAX;
	else if(m_FrameAver == 0)
		ret = MIN;
	else
		ret = OK;
	SetFrameAver(m_FrameAver, ret);

	// lineAver
	m_LineAver = d2->lineAver;
	if(m_LineAver == MAX_LINE-1)
		ret = MAX;
	else if(m_LineAver == 0)
		ret = MIN;
	else
		ret = OK;
	SetLineAver(m_LineAver, ret);

	// smooth
	m_Smooth = d2->smooth;
	if(m_Smooth == MAX_SMOOTH-1)
		ret = MAX;
	else if(m_Smooth == 0)
		ret = MIN;
	else
		ret = OK;
	SetSmooth(m_Smooth, ret);

	// chroma
	m_Chroma = d2->chroma;
    m_ChromaBak = m_Chroma;
	if(m_Chroma == MAX_CHROMA-1)
		ret = MAX;
	else if(m_Chroma == 0)
		ret = MIN;
	else
		ret = OK;
	m_ptrUpdate->Chroma(CHROMA_TYPE[m_Chroma].c_str(), ret);
	
	// other
	m_Gamma = d2->gamma; //4;
	if(m_Gamma == (MAX_GAMMA - 1))
		ret = MAX;
	else if(m_Gamma == 0)
		ret = MIN;
	else
		ret = OK;
	m_ptrUpdate->Gamma(m_Gamma, ret);

	ChangeColormap();
    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)// update for gamma and chroma
    {
        ptrDscMan->GetWriteLock();
        UpdateDsc(); 
        ptrDscMan->ReadWriteUnlock();
    }

	//image ehn
	m_ImgEhn = d2->imgEhn;
	if(m_ImgEhn == MAX_IMG_EHN-1)
		ret = MAX;
	else if(m_ImgEhn == 0)
		ret = MIN;
	else
		ret = OK;
	SetImgEhn(m_ImgEhn, ret);

    //gray transform
    m_CurCurveIndex = d2->grayTransIndex;

	InitColorTable();
	CustomGammaChange(m_CurveBasePoint, m_ColorTransTable);	
	ChangeColormap();
}

void ImgProc2D::Init(ExamItem::Para2D* d2)
{
    InitOptimize(d2);

    m_RejectPoint = 0;
    //m_CurCurveIndex = 1;
    //InitColorTable();
    //CustomGammaChange(m_CurveBasePoint, m_ColorTransTable);	
	//ChangeColormap();
}

void ImgProc2D::GetCurPara(ExamItem::ParaItem* ptrParaItem)
{
	ptrParaItem->d2.chroma = m_Chroma;
	ptrParaItem->d2.leftRight = m_LR;
	ptrParaItem->d2.upDown = m_UD;
	ptrParaItem->d2.polarity = m_Polarity;
	ptrParaItem->d2.rotate = m_Rotate;
	ptrParaItem->d2.frameAver = m_FrameAver;
	ptrParaItem->d2.lineAver = m_LineAver;
	ptrParaItem->d2.smooth = m_Smooth;
	ptrParaItem->d2.gamma = m_Gamma;
	ptrParaItem->d2.noiseReject = m_NoiseReject;
	ptrParaItem->d2.imgEhn = m_ImgEhn;
    ptrParaItem->d2.grayTransIndex = m_CurCurveIndex;
}

bool ImgProc2D::GetChromaStatus()
{
	if (m_Chroma == 0)
		return FALSE;
	else
		return TRUE;
}
bool ImgProc2D::GetLRStatus()
{
	return m_LR;
}

bool ImgProc2D::GetUDStatus()
{
	return m_UD;
}

int ImgProc2D::GetImgEhnLevel()
{
	return m_ImgEhn;
}

///> /////////////////////////////////[private func]////////////////////////////

/*
 * @brief set LR Overturn status to DSC
 */
void ImgProc2D::SetLR(bool on)
{
    DscMan* ptrDscMan = DscMan::GetInstance();
    ptrDscMan->GetWriteLock();
	m_ptrDscPara->dca2DIPAttrs.ipaLROverturn = on;

    UpdateDsc();
    ptrDscMan->ReadWriteUnlock();

	m_ptrUpdate->LeftRight(on);
}

/*
 * @brief set UD Overturn status to DSC
 */
void ImgProc2D::SetUD(bool on)
{
	PRINTF("UD = %d\n", on);
    DscMan* ptrDscMan = DscMan::GetInstance();
    ptrDscMan->GetWriteLock();
	m_ptrDscPara->dca2DIPAttrs.ipaUDOverturn = on;
    UpdateDsc();
    ptrDscMan->ReadWriteUnlock();

	m_ptrUpdate->UpDown(on);
}

/*
 * @brief set Po Overturn status to DSC
 */
void ImgProc2D::SetPolarity(bool on)
{
    DscMan* ptrDscMan = DscMan::GetInstance();
    ptrDscMan->GetWriteLock();
	m_ptrDscPara->dca2DIPAttrs.ipaPolarityOverturn = on;

    UpdateDsc();
    ptrDscMan->ReadWriteUnlock();

	m_ptrUpdate->Polarity(on, OK);
}

/*
 * @brief set noise reject to DSC
 */
void ImgProc2D::SetNoiseReject(bool on)
{
    DscMan* ptrDscMan = DscMan::GetInstance();
	m_ptrDscPara->dca2DIPAttrs.ipaReject = false;
	if (on)
		m_ptrDscPara->dca2DIPAttrs.ipaMeanCompareFilterThreshold = 25;
		//m_ptrDscPara->dca2DIPAttrs.ipaMeanCompareFilterThreshold = 35;
	else
		m_ptrDscPara->dca2DIPAttrs.ipaMeanCompareFilterThreshold = 0;
	m_ptrUpdate->NoiseReject(on);
}
/*
 * @brief set rotate angle to dsc 
 */
void ImgProc2D::SetRotate(int data, EKnobReturn ret)
{
    m_ptrDscPara->dca2DIPAttrs.ipaRotate = data;
    m_ptrUpdate->Rotate(Rotate[data], ret);
    m_Rotate = data;

    Replay::GetInstance()->ClearCurReplayData(); 
}

//void ImgProc2D::SetFrameAverLevel(int level)
void ImgProc2D::SetFrameAverLevel(int level, bool update)
{
	m_FrameAverBak = m_FrameAver;
	m_FrameAver = level;
	EKnobReturn ret;
	if(m_FrameAver == MAX_FRAME-1)
		ret = MAX;
	else if(m_FrameAver == 0)
		ret = MIN;
	else
		ret = OK;
	SetFrameAver(m_FrameAver, ret, update);
}

void ImgProc2D::RecoverFrameAverLevel()
{
	m_FrameAver = m_FrameAverBak;
	EKnobReturn ret;
	if(m_FrameAver == MAX_FRAME-1)
		ret = MAX;
	else if(m_FrameAver == 0)
		ret = MIN;
	else
		ret = OK;
	SetFrameAver(m_FrameAver, ret, false);
}

/*
 * @brief set frame correlation level to DSC
 */
//void ImgProc2D::SetFrameAver(int data, EKnobReturn ret)
void ImgProc2D::SetFrameAver(int data, EKnobReturn ret, bool update)
{
	m_ptrDscPara->dca2DIPAttrs.ipaFrameAver = FrameAver[data];
	if (update)
	m_ptrUpdate->FrameAver(data, ret);
}

//void ImgProc2D::SetLineAverLevel(int level)
void ImgProc2D::SetLineAverLevel(int level, bool update)
{
	m_LineAverBak = m_LineAver;
	m_LineAver = level;
	EKnobReturn ret;
	if(m_LineAver == MAX_LINE-1)
		ret = MAX;
	else if(m_LineAver == 0)
		ret = MIN;
	else
		ret = OK;

	SetLineAver(m_LineAver, ret, update);
}

void ImgProc2D::RecoverLineAverLevel()
{
	m_LineAver = m_LineAverBak;

	EKnobReturn ret;
	if(m_LineAver == MAX_LINE-1)
		ret = MAX;
	else if(m_LineAver == 0)
		ret = MIN;
	else
		ret = OK;
	SetLineAver(m_LineAver, ret, false);
}
/*
 * @brief set line correlation level to DSC
 */
//void ImgProc2D::SetLineAver(int data, EKnobReturn ret)
void ImgProc2D::SetLineAver(int data, EKnobReturn ret, bool update)
{
	m_ptrDscPara->dca2DIPAttrs.ipaLineAver = LineAver[data];
	if (update)
	m_ptrUpdate->LineAver(data, ret);
}

/*
 * @brief set smooth to DSC
 */
void ImgProc2D::SetSmooth(int data, EKnobReturn ret)
{
	//优先级3
	m_ptrDscPara->dca2DIPAttrs.ipaSmooth = data;
	m_ptrUpdate->Smooth(data, ret);
}


/*
 * @brief calculate the curve value
 * @para BaseValue[in], select point's value 
 * @para CurveValue[ou], result curve
 */
void ImgProc2D::CustomGammaChange (unsigned char* BaseValue, unsigned char* CurveValue)
{
	int i, k;
	float u, v;
	float t1, t2;
	float m[12];
	float p0, p1, p2, p3;
	float x[9], y[9];
	int X, Y;

	x[0] = 0;
	y[0] = BaseValue[0];
	for (i=1; i<9; i++)
	{
		x[i] = 32*i - 1;
		y[i] = BaseValue[i];
	}

	for (i=0; i<8; i++)
	{
		m[i+2] = (y[i+1]-y[i]) / (x[i+1]-x[i]);
	}
	m[1] = 2 * m[2] - m[3];
	m[0] = 2 * m[1] - m[2];
	m[10]= 2 * m[9] - m[8];
	m[11]= 2 * m[10]- m[9];

	if (fabs(m[2]-m[3]) < eps)
	{
		t1 = m[2];
	}
	else
	{
		u = fabs(m[3] - m[2]);
		v = fabs(m[1] - m[0]);
		t1 = (u*m[1] + v*m[2]) / (u + v);
	}

	for (k=0; k<8; k++)
	{
		u = fabs(m[k+4] - m[k+3]);
		v = fabs(m[k+2] - m[k+1]);

		if ((u<eps) && (v<eps))
		{
			t2 = (m[k+2] + m[k+3]) / 2;
		}
		else
		{
			t2 = (u*m[k+2] + v*m[k+3]) / (u + v);
		}

		p0 = y[k];
		p1 = t1;
		p2 = (3*m[k+2] - 2*t1 - t2) / (x[k+1] - x[k]);
		p3 = (t2 + t1 - 2*m[k+2]) / pow((x[k+1] - x[k]), 2);

		for (X=x[k]; X<=x[k+1]; X++)
		{
			Y = p0 + p1*(X-x[k]) + p2*pow((X-x[k]), 2) + p3*pow((X-x[k]), 3);
			if (Y > 255) Y = 255;
			if (Y < 0) Y = 0;

			CurveValue[X] = Y;
		}

		t1 = t2;
	}

#if 0
	PRINTF("---------------------------------color trans table\n");
	for(i = 0; i < MAX_GRAY; i ++)
	{
		//if (i != CurveValue[i])
			PRINTF("[%d] = %d\n", i, CurveValue[i]);
	}
#endif

}

/*
 * @brief change the colormap when Gamma/GrayCurve/GrayReject changed
 */
void ImgProc2D::ChangeColormap()
{
    int i;
    int data = m_Chroma;
    float k;

    //穿刺增强打开时，Gamma不同
    if(m_biopsyEnhance)
        k = (float)(255/pow(255,2.0));
    else
        k = (float)(255/pow(255,Gamma[m_Gamma]));

    for(i=0; i<MAX_GRAY; i++)  
    { 	
        if(i==0)
        {
            m_ColorTable[i] = 0;
        }
        else
        {
            if (m_biopsyEnhance)   //穿刺增强打开时，Gamma不同
                m_ColorTable[i] = (i<=m_RejectPoint? 0:1) * (unsigned char)(k * pow(m_ColorTransTable[i],2.0)); 
            else
                m_ColorTable[i] = (i<=m_RejectPoint? 0:1) * (unsigned char)(k * pow(m_ColorTransTable[i], Gamma[m_Gamma])); 
        }
    }
#if 0
	PRINTF("trans color\n");
	for (i = 0; i < MAX_GRAY; i ++)
	{
		//PRINTF("[%d] = %d\n", i, m_ColorTransTable[i]);
		PRINTF("[%d] = %d\n", i, m_ColorTable[i]);
        //if (m_ColorTransTable[i] != i)
          //  PRINTF("[%d] = %d\n", i, m_ColorTransTable[i]);
	}
#endif

	// send table to dsc
	RGBTRIPLE table[MAX_GRAY];
	int chromaTable[COLOR_NUM][COLOR_GRADE];
	MakePseudoColormap(chromaTable, COLOR_MAP_CHROM[data]);
	for(i=0; i<MAX_GRAY; i++)  
	{
		table[i].rgbtRed = chromaTable[0][m_ColorTable[i]];
		table[i].rgbtGreen = chromaTable[1][m_ColorTable[i]]; 
		table[i].rgbtBlue = chromaTable[2][m_ColorTable[i]];
    //    PRINTF("[%d] = %d\n", i, table[i].rgbtRed);
	}

	SetColorTable(table, MAX_GRAY);
}

/*
 * @brief set color table to DSC
 */
void ImgProc2D::SetColorTable(RGBTRIPLE table[], int size)
{
	int i;
	for (i = 0; i < size; i ++)
    {
		m_ptrDscPara->dcaPalette[i] = table[i];
        //PRINTF("[%d] = %d\n", i, table[i].rgbtBlue);
    }
    //UpdateDsc();

    //view
	m_ptrUpdate->BWColorTable(table, size);
}


void ImgProc2D::InitColorTable()
{
    int i;
    m_CurrentPoint = 0;
    if (m_CurCurveIndex == 0)
    {
        for(i=0; i<MAX_GRAY; i++)
        {
            m_ColorTransTable[i] = i;
        }
    }
    else
    {
        float gray_x1, min;
        float ratio = TRANS_CURVE_RATIO[m_CurCurveIndex - 1];
        min = (-127.5/128.0) * ratio;

        for(i=0; i<MAX_GRAY; i++)
        {
            //m_ColorTransTable[i] = i;
            gray_x1 = ((i - 127.5)/128.0) * ratio;
            m_ColorTransTable[i] = (int) (128 * (((1 - exp(-gray_x1))/(1 + exp(-gray_x1)) ) * ((1 + exp(min))/(1 - exp(min))) + 1)); 
             if (m_ColorTransTable[i] > 255)
                m_ColorTransTable[i] = 255;
        }
    }
	
	m_CurveBasePoint[0] = m_ColorTransTable[0];
	//for(i=1; i<9; i++)
    for(i=1; i< 8; i++)
		m_CurveBasePoint[i] = m_ColorTransTable[i*32-1];
    m_CurveBasePoint[8] = 255;

    if (m_CurCurveIndex == 0)
    {
        CustomGammaChange(m_CurveBasePoint, m_ColorTransTable);	
    }
}

//void ImgProc2D::SetImgEhnLevel(int level)
void ImgProc2D::SetImgEhnLevel(int level, bool update)
{
	m_ImgEhnBak= m_ImgEhn;
	m_ImgEhn = level;
	EKnobReturn ret;
	if(m_ImgEhn == MAX_IMG_EHN-1)
		ret = MAX;
	else if(m_ImgEhn == 0)
		ret = MIN;
	else
		ret = OK;
	SetImgEhn(m_ImgEhn, ret, update);
}

void ImgProc2D::RecoverImgEhnLevel()
{
	m_ImgEhn= m_ImgEhnBak;
	EKnobReturn ret;
	if(m_ImgEhn == MAX_IMG_EHN-1)
		ret = MAX;
	else if(m_ImgEhn == 0)
		ret = MIN;
	else
		ret = OK;
	SetImgEhn(m_ImgEhn, ret, false);
}

//void ImgProc2D::SetImgEhn(int data, EKnobReturn ret)
void ImgProc2D::SetImgEhn(int data, EKnobReturn ret, bool update)
{
	m_ptrDscPara->dca2DIPAttrs.ipaImgEhn = data;
	if (update)
		m_ptrUpdate->ImgEhn(data, ret);
}

int ImgProc2D::GetRotate(void)
{
	if ((m_Rotate < 0) || (m_Rotate > (MAX_ROTATE - 1)))
			m_Rotate = 0;
    return Rotate[m_Rotate];
}

void ImgProc2D::UpdateDsc(void)
{
	CDSC* ptrDsc = DscMan::GetInstance()->GetDsc();
	if (ptrDsc != NULL)
    {
		ptrDsc->Update();
        Update2D::CopyImgDataToReplay();
    }
}

/*
 * @brief change angle of image rotation in mode EFOV 
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
EKnobReturn ImgProc2D::GetKnobEFOVROISize(ImgProc2D::SIZE size)
{
    EKnobReturn ret = ERROR;

    if(size == SMALL)
        ret = MIN;
    else if(size == BIG)
        ret= MAX;
    else
        ret = OK;

    return ret;
}

enum EKnobReturn ImgProc2D::ChangeEFOVROISize(enum EKnobOper opr)
{
    EKnobReturn ret = ERROR;
    int temp = m_roiEFOVSize;
    switch(opr) {
        case ADD:
            if(m_roiEFOVSize != BIG) {
                ++temp;
                m_roiEFOVSize = (SIZE)temp;
            }
            break;

        case SUB:
            if(m_roiEFOVSize != SMALL) {
                --temp;
                m_roiEFOVSize = (SIZE)temp;
            }
            break;
        case ROTATE:
            if (m_roiEFOVSize !=BIG)
            {
                temp ++;
                m_roiEFOVSize = (SIZE)temp;
            }
            else
            {
                temp = 0;
                m_roiEFOVSize = (SIZE)temp;
            }
            break;

        default:
            return ERROR;
            break;
    }
    ret = GetKnobEFOVROISize(m_roiEFOVSize);
    KnobMenu::GetInstance()->SetValue(EFOV_ROI, (char *)(EFOVROISizeName[temp]), ret);
    return ret;
}

enum EKnobReturn ImgProc2D::ChangeEFOVRotate(enum EKnobOper oper)
{
    if(oper == ADD && m_rotateEFOV < MAX_EFOV_ROTATE)
        m_rotateEFOV += 30;
    else if(oper == SUB && m_rotateEFOV > -MAX_EFOV_ROTATE )
        m_rotateEFOV -= 30;
    else if (oper == ROTATE)
    {
        if (m_rotateEFOV < MAX_EFOV_ROTATE)
        {
            m_rotateEFOV += 30;
        }
        else
        {
            m_rotateEFOV = 0;
        }
    }
    else
        return ERROR;

    EKnobReturn ret, retT;

	//renew the efov zoom
	if (m_zoomEFOVIndex != 2)
	{
		m_zoomEFOVIndex = 2;
		m_zoomEFOVPos = (m_zoomEFOVBox.right + m_zoomEFOVBox.left) / 2;
		if(m_zoomEFOVIndex == MAX_EFOV_ZOOM_INDEX - 1)
		{
			retT = MAX;
		}
		else if(m_zoomEFOVIndex == 0)
		{
			retT = MIN;
		}
		else
		{
			retT = OK;
		}

		//set zoom to DSC for mode EFOV
		SetEFOVZoom(m_zoomEFOVIndex, m_zoomEFOVPos, retT);
	}

	if(m_rotateEFOV == MAX_EFOV_ROTATE)
	{
		ret = MAX;
	}
	else if(m_rotateEFOV == -MAX_EFOV_ROTATE)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//set rotate to DSC for mode EFOV
	SetEFOVRotate(m_rotateEFOV, ret);

	return ret;
}

/*
 * @brief set rotate angle to dsc for mode EFOV 
 */
void ImgProc2D::SetEFOVRotate(int data, EKnobReturn ret)
{
	if (DscMan::GetInstance()->GetDsc() != NULL)
	{
		DscMan::GetInstance()->GetDsc()->RotateEFOVImage(data);
	}

	//update view
    m_ptrUpdate->EFOVRotate(data, ret);

    // clear screen
    KeyClearScreen kcs;
    kcs.Execute();
}

/*
 * @brief change scale of image zoom in mode EFOV 
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProc2D::ChangeEFOVZoomScale(enum EKnobOper oper)
{
    if(oper == ADD && m_zoomEFOVIndex < MAX_EFOV_ZOOM_INDEX - 1)
        m_zoomEFOVIndex++;
    else if(oper == SUB && m_zoomEFOVIndex > 0)
        m_zoomEFOVIndex --;
    else if (oper == ROTATE)
    {
        if (m_zoomEFOVIndex < (MAX_EFOV_ZOOM_INDEX - 1))
        {
            m_zoomEFOVIndex ++;
        }
        else
        {
            m_zoomEFOVIndex = 0;
        }
    }
    else
        return ERROR;

	EKnobReturn ret, retT;

	//renew the efov rotate
	if (m_rotateEFOV != 0)
	{
		m_rotateEFOV = 0;
		if(m_rotateEFOV == MAX_EFOV_ROTATE)
		{
			retT = MAX;
		}
		else if(m_rotateEFOV == -MAX_EFOV_ROTATE)
		{
			retT = MIN;
		}
		else
		{
			retT = OK;
		}

		//set rotate to DSC for mode EFOV
		SetEFOVRotate(m_rotateEFOV, retT);
	}

	if(m_zoomEFOVIndex == MAX_EFOV_ZOOM_INDEX - 1)
	{
		ret = MAX;
	}
	else if(m_zoomEFOVIndex == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//set zoom to DSC for mode EFOV
	SetEFOVZoom(m_zoomEFOVIndex, m_zoomEFOVPos, ret);
	return ret;
}

/*
 * @brief set zoom to dsc for mode EFOV 
 */
void ImgProc2D::SetEFOVZoom(int scaleIndex, int pos, EKnobReturn ret)
{
	RECT rect = {0,0,0,0};

	if(DscMan::GetInstance()->GetDsc() != NULL)
	{
		rect = DscMan::GetInstance()->GetDsc()->SetEFOVZoomState(pos, m_zoomEFOVScale[scaleIndex]);
	}
        // update scale
        m_ptrUpdate->UpdateEFOVZoomScale();

	//update knob zoom
	m_ptrUpdate->EFOVZoom(m_zoomEFOVScale[scaleIndex], ret);
	
	//draw box
	//printf("left=%d top=%d right=%d botom=%d\n",rect.left, rect.top, rect.right, rect.bottom );
	m_ptrUpdate->EFOVZoomBox(rect.left, rect.top, rect.right, rect.bottom);

	//draw miniature box
	if(scaleIndex > 2)
	{
		rect.top = m_zoomEFOVBox.top;
		rect.left = m_zoomEFOVBox.left;
		rect.right = m_zoomEFOVBox.right;
		rect.bottom = m_zoomEFOVBox.bottom;
	}
	else
		rect.top = rect.left = rect.right = rect.bottom = 0;

        m_ptrUpdate->EFOVZoomMiniatureBox(rect.left, rect.top, rect.right, rect.bottom);

        // clear screen
        KeyClearScreen kcs;
        kcs.Execute();
}

bool ImgProc2D::ChangeEFOVZoomPos(enum EKnobOper oper)
{
	if (m_zoomEFOVIndex < 3)
		return FALSE;

	if (oper == ADD && m_zoomEFOVPos < m_zoomEFOVBox.right)
		m_zoomEFOVPos ++;
	else if (oper == SUB && m_zoomEFOVPos > m_zoomEFOVBox.left)
		m_zoomEFOVPos --;
	else 
		return FALSE;
	
	EKnobReturn ret;
	if(m_zoomEFOVIndex == MAX_EFOV_ZOOM_INDEX - 1)
	{
		ret = MAX;
	}
	else if(m_zoomEFOVIndex == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//set to DSC
	SetEFOVZoom(m_zoomEFOVIndex, m_zoomEFOVPos, ret);

	return TRUE;
}

void ImgProc2D::GetEFOVSpeed(int &speed_flag, int &speed_maxflag, float &speed)
{
    if (DscMan::GetInstance()->GetDsc() != NULL)
        DscMan::GetInstance()->GetDsc()->GetEFOVSpeed(speed_flag, speed_maxflag, speed);
}

EFOVRECT<int> ImgProc2D::GetEFOVBoxRect(void)
{
	EFOVRECT<int> rect;
	rect.leftup.x = rect.leftdown.x = rect.rightup.x = rect.rightdown.x = 0;
	rect.leftup.y = rect.leftdown.y = rect.rightup.y = rect.rightdown.y = 0;

	if (DscMan::GetInstance()->GetDsc() != NULL)
		rect = DscMan::GetInstance()->GetDsc()->GetEFOVBoxRect();

	return rect;
}

void ImgProc2D::EnterEFOVView()
{
	//Init EFOV zoom and rotate knob
	EKnobReturn ret;

	m_zoomEFOVIndex = 2;
	if(DscMan::GetInstance()->GetDsc() != NULL)
		m_zoomEFOVBox = DscMan::GetInstance()->GetDsc()->GetEFOVZoomBoxRect();
	m_zoomEFOVPos = (m_zoomEFOVBox.right + m_zoomEFOVBox.left) / 2;
	if(m_zoomEFOVIndex == MAX_EFOV_ZOOM_INDEX - 1)
	{
		ret = MAX;
	}
	else if(m_zoomEFOVIndex == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//update knob zoom
	m_ptrUpdate->EFOVZoom(m_zoomEFOVScale[m_zoomEFOVIndex], ret);

	m_rotateEFOV = 0;
	if(m_rotateEFOV == MAX_EFOV_ROTATE)
	{
		ret = MAX;
	}
	else if(m_rotateEFOV == -MAX_EFOV_ROTATE)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	//update knob rotate
	m_ptrUpdate->EFOVRotate(m_rotateEFOV, ret);
}

void ImgProc2D::GetEFOVSpeedColormap(RGBTRIPLE table[MAX_EFOV_COLOR])
{
	int efovColorTable[COLOR_NUM][COLOR_GRADE];
	MakePseudoColormap(efovColorTable, EFOVCOLOR);
	for (int i = 0; i < MAX_EFOV_COLOR; i++)  
	{
		table[i].rgbtRed = efovColorTable[2][i];
		table[i].rgbtGreen = efovColorTable[1][i]; 
		table[i].rgbtBlue = efovColorTable[0][i];
    //    PRINTF("[%d] = %d\n", i, table[i].rgbtRed);
	}
}

void ImgProc2D::EnhanceBiopsy(bool on)
{
    if(on == m_biopsyEnhance)
        return;

    m_biopsyEnhance = on;
    ChangeColormap();

    DscMan* ptrDscMan = DscMan::GetInstance();
    if(ptrDscMan != NULL)
    {
        ptrDscMan->GetWriteLock();
        UpdateDsc();
        ptrDscMan->ReadWriteUnlock();
    }
}

