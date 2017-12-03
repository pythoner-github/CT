
#include "ImgProcPw.h"
#include "DscMan.h"
#include "GlobalClassMan.h"
#include "MenuPW.h"

extern MenuPW g_menuPW;
ImgProcPw* ImgProcPw::m_ptrInstance = NULL;

ImgProcPw::ImgProcPw()
{
	m_timeSmooth = 0;
	m_TimeMark = false;
    m_autoCalc = false;
    m_drawTraceStatus = false;
	m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
	m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdatePw();
}
ImgProcPw::~ImgProcPw()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

ImgProcPw* ImgProcPw::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new ImgProcPw;

	return m_ptrInstance;
}

EKnobReturn ImgProcPw::ChangeTimeSmooth(EKnobOper oper)
{

	if ((oper == ADD) && (m_timeSmooth < (MAX_TIME_RES-1)))
		m_timeSmooth ++;
	else if ((oper == SUB) && (m_timeSmooth > 0))
		m_timeSmooth --;
	else 
		return ERROR;

	EKnobReturn ret;
	if(m_timeSmooth == (MAX_TIME_RES-1))
	{
		ret = MAX;
	}
	else if(m_timeSmooth == 0)
	{
		ret = MIN;
	}
	else
	{
		ret = OK;
	}

	SetTimeSmooth(m_timeSmooth, ret);
	return ret;
}
void ImgProcPw::ChangeTimeMark(bool on)
{
	m_TimeMark = on;
	SetTimeMark(on);
}

void ImgProcPw::InitOptimize(ExamItem::ParaSpectrum* pw)
{
	EKnobReturn ret;

	// time smooth
	m_timeSmooth = pw->timeSmooth;
	if(m_timeSmooth == (MAX_TIME_RES-1))
		ret = MAX;
	else if(m_timeSmooth == 0)
		ret = MIN;
	else
		ret = OK;
	SetTimeSmooth(m_timeSmooth, ret);
}
void ImgProcPw::Init(ExamItem::ParaSpectrum* pw)
{
    InitOptimize(pw);

	// timeMark
	m_TimeMark = false;
	SetTimeMark(m_TimeMark);
    
    //autotrace
    m_drawTraceStatus = false;//false;
    m_ptrUpdate->AutoStatus(m_drawTraceStatus);

    // autoCalc
    m_autoCalc = false;
    m_ptrUpdate->AutoCalc(m_autoCalc);
        
}
void ImgProcPw::GetCurPara(ExamItem::ParaItem* ptrParaItem)
{
	ptrParaItem->spectrum.timeSmooth = m_timeSmooth;
}

/*
 * @brief get draw trace status
 */
bool ImgProcPw::GetTraceStatus()
{
	return m_drawTraceStatus;
}

/*
 * @brief get trace status of dsc
 */
bool ImgProcPw::GetDscTraceStatus()
{
	return m_ptrDscPara->dcaPWIPAttrs.ipaTrace.status;
}

void ImgProcPw::SetDscTraceStatus(bool status)
{
    DscMan::GetInstance()->GetWriteLock();
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.status = status;
    CDSC* ptrDsc = DscMan::GetInstance()->GetDsc();
    if (ptrDsc != NULL)
        ptrDsc->UpdateTrace();
    DscMan::GetInstance()->ReadWriteUnlock();
}

vector<POINT> ImgProcPw::GetTraceData(void)
{
	return DscMan::GetInstance()->GetDsc()->GetTraceData();
}

vector<POINT> ImgProcPw::GetTraceCalcData(void)
{
	return DscMan::GetInstance()->GetDsc()->GetTraceMaxVelocity();
}

void ImgProcPw::GetTraceCycle(int &begin, int &end)
{
	DscMan::GetInstance()->GetDsc()->GetTraceCycle(begin, end);
}

void ImgProcPw::SetTraceStatus(bool status)
{
    m_drawTraceStatus = status;
}

void ImgProcPw::SetTraceType(unsigned char type)
{
	m_ptrDscPara->dcaPWIPAttrs.ipaTrace.type = type;
}

void ImgProcPw::SetTraceDirection(unsigned char direction)
{
	m_ptrDscPara->dcaPWIPAttrs.ipaTrace.direction = direction;
}

void ImgProcPw::SetTraceThreshold(unsigned char threshold)
{
	m_ptrDscPara->dcaPWIPAttrs.ipaTrace.threshold = threshold;
}

void ImgProcPw::SetTraceSmooth(unsigned char smooth)
{
	m_ptrDscPara->dcaPWIPAttrs.ipaTrace.smooth = smooth;
}

void ImgProcPw::SetTraceBaseline(char baseline)
{
	m_ptrDscPara->dcaPWIPAttrs.ipaTrace.baseline = baseline;
}

void ImgProcPw::SetTraceFs(char fs)
{
	m_ptrDscPara->dcaPWIPAttrs.ipaTrace.fs = fs;
}

///> private
void ImgProcPw::SetTimeSmooth(int data, EKnobReturn ret)
{
	// send to dsc

	// draw
	m_ptrUpdate->TimeRes(data, ret);
}

/*
 * @brief set time mark 
 */
void ImgProcPw::SetTimeMark(bool on)
{
	m_ptrUpdate->TimeMarkPw(on);
}

