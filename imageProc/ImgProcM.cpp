#include "Def.h"
#include "ImgProcM.h"
#include "DscMan.h"
#include "GlobalClassMan.h"
#include "gui_func.h"

ImgProcM* ImgProcM::m_ptrInstance = NULL;

///> /////////////////////////////////[public func]////////////////////////////

ImgProcM* ImgProcM::GetInstance()
{
	if(m_ptrInstance == NULL)
	{
		m_ptrInstance = new ImgProcM;
	}

	return m_ptrInstance;
}

///> construct
ImgProcM::ImgProcM()
{
	m_EnhanceM = 0;
	m_MSmooth = 0;
	m_LineDraw = FALSE;
	m_LineBegin.x = IMAGE_W / 4;
	m_LineBegin.y = IMAGE_H / 4;
	m_LineEnd.x = m_LineBegin.x + 50;
	m_LineEnd.y = m_LineBegin.y + 50;
	m_TimeMark = FALSE;

	m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
	m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();
	m_ptrImgDraw = ImageAreaDraw::GetInstance();
	m_ptrImgPara = ImageAreaPara::GetInstance();
	m_ptrDsc = NULL;
}

ImgProcM::~ImgProcM()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

void ImgProcM::InitOptimize(ExamItem::Para2D* d2)
{
}
void ImgProcM::Init(ExamItem::Para2D* d2)
{
    InitOptimize(d2);

	// EnhanceM
	m_EnhanceM = 0;
	SetEnhanceM(m_EnhanceM, MIN);

	// MSmooth
	m_MSmooth = 0; //lineAver
	SetMSmooth(m_MSmooth, MIN);

	// TimeMark
	m_TimeMark = FALSE;
	SetTimeMark(m_TimeMark);
}
/*
 * @brief change enhance M level
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProcM::ChangeEnhanceM(enum EKnobOper oper)
{
	if(oper == ADD && m_EnhanceM < MAX_ENHANCE_M)
		m_EnhanceM ++;
	else if(oper == SUB && m_EnhanceM > 0)
		m_EnhanceM --;
	else 
		return ERROR;

	EKnobReturn ret;
	if(m_EnhanceM == (MAX_ENHANCE_M-1))
		ret = MAX;
	else if(m_EnhanceM == 0)
		ret = MIN;
	else
		ret = OK;

	//set Enhance M to DSC
	SetEnhanceM(m_EnhanceM, ret);

	return ret;
}

/*
 * @brief change M smooth level
 *
 * @param oper SUB or ADD 
 *
 * @retval EKnobReturn
 */
enum EKnobReturn ImgProcM::ChangeMSmooth(enum EKnobOper oper)
{
	if(oper == ADD && m_MSmooth < MAX_MSMOOTH)
		m_MSmooth ++;
	else if(oper == SUB && m_MSmooth > 0)
		m_MSmooth --;
	else 
		return ERROR;
	
	EKnobReturn ret;
	if(m_MSmooth == (MAX_MSMOOTH-1))
		ret = MAX;
	else if(m_MSmooth == 0)
		ret = MIN;
	else
		ret = OK;

	// set M Smooth ot DSC
	SetMSmooth(m_MSmooth, ret);
	return ret;
}

/*
 * @brief change anatomic M status 
 *
 * @param on[in] TRUE-on, FALSE-off
 */
void ImgProcM::AnatomicMInit(void)
{	
	//create DSC object
	m_ptrDsc = DscMan::GetInstance()->CreateDscObj(DscMan::MANATOMIC);	//create DSC object
	
	// init anatomit line
	GetAnatomicInitLine(m_LineBegin, m_LineEnd);

	// set
	m_LineDraw = FALSE;
	SetAnatomicLine(m_LineBegin, m_LineEnd); //must call after CreateDscObj
}
	
/*
 * @brief change anatomic line's length and angle, according to then change of endpoint
 *
 * @para p[in] endpoint of line changed
 */
void ImgProcM::ChangeAnatomicLine(int offsetX, int offsetY)
{
    int length = FormatM::GetInstance()->GetBImgHeightDot();
    POINT p;
    
    int up, down, left, right;
    m_ptrImgDraw->GetAnatomicBoundary(up, down, left, right);
    ClearAnatomicLine(m_LineBegin, m_LineEnd);

    if (((m_LineEnd.x + offsetX) > left) && ((m_LineEnd.x + offsetX) < right)) {
	p.x = m_LineEnd.x + offsetX;
	p.y = m_LineEnd.y;

	if (CalcLength(m_LineBegin, p) <= length)
	    m_LineEnd.x += offsetX;
    }

    if (((m_LineEnd.y - offsetY) > up) && ((m_LineEnd.y - offsetY) < down)) {
	p.x = m_LineEnd.x;
	p.y = m_LineEnd.y - offsetY;

	if (CalcLength(m_LineBegin, p) <= length)
	    m_LineEnd.y -= offsetY;
    }
    SetAnatomicLine(m_LineBegin, m_LineEnd);
}

/*
 * @brief 
 */
void ImgProcM::MoveAnatomicLine(int offsetX, int offsetY)
{
    ClearAnatomicLine(m_LineBegin, m_LineEnd);

    int up, down, left, right;
    m_ptrImgDraw->GetAnatomicBoundary(up, down, left, right);

    if (((m_LineBegin.x + offsetX) > left) && ((m_LineBegin.x + offsetX) < right) 
	&& ((m_LineEnd.x + offsetX) > left) && ((m_LineEnd.x + offsetX) < right)) {
	m_LineBegin.x += offsetX;
	m_LineEnd.x += offsetX;
    }

    if (((m_LineBegin.y - offsetY) > up) && ((m_LineBegin.y - offsetY) < down) 
	&& ((m_LineEnd.y - offsetY) > up) && ((m_LineEnd.y - offsetY) < down)) {
	m_LineBegin.y -= offsetY;
	m_LineEnd.y -= offsetY;
    }

    SetAnatomicLine(m_LineBegin, m_LineEnd);
}

void ImgProcM::AnatomicLineInit(void)
{
    ClearAnatomicLine(m_LineBegin, m_LineEnd);
    GetAnatomicInitLine(m_LineBegin, m_LineEnd);
    SetAnatomicLine(m_LineBegin, m_LineEnd);
}

void ImgProcM::GetAnatomicInitLine(POINT &begin, POINT &end)
{
    int half = 30;
    POINT p = FormatM::GetInstance()->GetBCenterPoint();

    begin.x = p.x - half;
    begin.y = p.y - 3 * half/2;

    end.x = p.x + half;
    end.y = p.y + 2 * half - begin.y;
}

/*
 * @brief change time mark state, ON->OFF or OFF->ON
 *
 * @retval the final state of time mark
 */
void ImgProcM::ChangeTimeMark(bool on)
{
	m_TimeMark = on;

	// set Time Mark to DSC
	SetTimeMark(m_TimeMark);
}

///> /////////////////////////////////[private func]////////////////////////////
/*
 * @brief set enchance M to DSC
 */
void ImgProcM::SetEnhanceM(int data, EKnobReturn ret)
{
	m_ptrDscPara->dcaMIPAttrs.ipaEnhance = data;
	m_ptrUpdate->EnhanceM(data, ret);
}
/*
 * @brief set M smooth to DSC
 */
void ImgProcM::SetMSmooth(int data, EKnobReturn ret)
{
	m_ptrDscPara->dcaMIPAttrs.ipaSmooth = data;
	m_ptrUpdate->SmoothM(data, ret);
}
/*
 * @brief send anatomic line endpoint to DSC
 */
void ImgProcM::SetAnatomicLine(POINT begin, POINT end)
{
    // send to dsc
    if ((m_ptrDsc != NULL) && (m_ptrImgDraw != NULL)) {
	m_ptrDsc->SetCMMLinePosition(begin, end);
		
	// draw line and point
	if (!m_LineDraw) {
	    m_ptrImgDraw->DrawAnatomicalM(end, begin, true);
	    m_LineDraw = TRUE;
	}
    }
    // redraw
    // m_ptrImgPara->DrawMPara();
}

void ImgProcM::ClearAnatomicLine(POINT begin, POINT end)
{
    if ((m_LineDraw) && (m_ptrImgDraw != NULL)) {
	m_ptrImgDraw->DrawAnatomicalM(end, begin, false);
	m_LineDraw = FALSE;
    }
}

/*
 * @brief set time mark 
 */
void ImgProcM::SetTimeMark(bool on)
{
	m_ptrUpdate->TimeMarkM(on);
}



