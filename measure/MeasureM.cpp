#include "measure/MeasureM.h"

#include <iostream>
using namespace std;

///>method: 0－时间，1－心率
///>re_display:本测量项的属性，包括计算项的计算公式
///>item: 软件包测量项的标号，传给UpdateMeasure中把科别测量结果保存起来
MMeasureTime::MMeasureTime(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

MMeasureTime::~MMeasureTime()
{
	Esc();
}

void MMeasureTime::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialVLine();
	m_time = INVALID_VAL;
	m_hr = INVALID_VAL;
}

void MMeasureTime::PressLeft(POINT p)
{
	double allData[SINGLE_MAX];
	float resultTmp;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch (m_step)
	{
		case 0:
			if (m_isDraw) {
				m_draw.DrawVLine(m_tempP);
			}
			m_draw.DrawVLine(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 2);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawVLine(m_tempP);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawVLine(m_tempP);
			}
			m_draw.DrawVLine(m_tempP, FALSE);

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			if (m_itemInfo->meaType == TIME_M)
			    m_ptrMan->AddNew(TIME_M, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			else
			    m_ptrMan->AddNew(HR_M, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			if (m_itemInfo->meaType == TIME_M)
			{
				m_time = m_calc.MCalcTime(m_p1, m_tempP);
				resultTmp = m_time;
			}
			else
			{
				m_time = m_calc.MCalcTime(m_p1, m_tempP);
				m_hr = m_calc.MCalcHR(m_time);
				resultTmp = m_hr;
			}
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//begin new dist measure
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;
	}
}

void MMeasureTime::PressRight(POINT p)
{
	switch (m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawVLine(m_tempP);
			}

			m_tempP = m_p1;
			m_draw.DrawVLine(m_tempP, FALSE);
			m_draw.DrawVLine(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			m_step = 0;
			m_time = INVALID_VAL;
			m_hr = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void MMeasureTime::MouseMove(POINT p)
{
	double allData[SINGLE_MAX];
	float resultTmp;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch (m_step)
	{
		case 0:
			if (m_isDraw) {
				m_draw.DrawVLine(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawVLine(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawVLine(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawVLine(m_tempP);
			m_isDraw = TRUE;

			if (m_itemInfo->meaType == TIME_M)
			{
				m_time = m_calc.MCalcTime(m_p1, m_tempP);
				resultTmp = m_time;
			}
			else
			{
				m_time = m_calc.MCalcTime(m_p1, m_tempP);
				m_hr = m_calc.MCalcHR(m_time);
				resultTmp = m_hr;
			}
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);

			break;

		default:
			break;
	}
}

void MMeasureTime::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawVLine(m_tempP);
			m_draw.DrawVLine(m_tempP, FALSE);
		}
		m_draw.DrawVLine(m_p1, FALSE);
		m_draw.DrawVLine(m_p1);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.SetCursor(m_tempP);
	}
}

void MMeasureTime::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			m_draw.DrawVLine(m_tempP);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawVLine(m_tempP);
			m_draw.DrawVLine(m_p1, FALSE);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
		}
	}
}

//M模式速度测量
MMeasureVel::MMeasureVel(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

MMeasureVel::~MMeasureVel()
{
	Esc();
}

void MMeasureVel::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCrossLine();
	m_result = INVALID_VAL;
}

void MMeasureVel::PressLeft(POINT p)
{
    UpdateMeasure::ResultAttr attr;
    attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();

	double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw) {
				m_draw.DrawCrossLine(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCrossLine(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP);

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:

			if (m_isDraw) {
				m_draw.DrawCrossLine(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			if (m_itemInfo->meaType == VEL_M)
			    m_ptrMan->AddNew(VEL_M, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			else
			    m_ptrMan->AddNew(SLOPE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			m_result = m_calc.MCalcVel(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_result, m_itemInfo, allData, SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//begin new measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

	}
}

void MMeasureVel::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0: // clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawCrossLine(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCrossLine(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;
			m_result = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void MMeasureVel::MouseMove(POINT p)
{
	double allData[SINGLE_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch (m_step)
	{
		case 0:
			if (m_isDraw) {
				m_draw.DrawCrossLine(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCrossLine(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawCrossLine(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCrossLine(p);
			m_draw.DrawDotLine(m_p1, p);
			m_isDraw = TRUE;

			m_result = m_calc.MCalcVel(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_result, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			break;

		default:
			break;
	}
}

void MMeasureVel::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCrossLine(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.DrawCursor(m_tempP, FALSE);
		m_draw.DrawCrossLine(m_tempP);
		m_draw.SetCursor(m_tempP);
	}
}

void MMeasureVel::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			m_draw.DrawCrossLine(m_tempP);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawCrossLine(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP);
		}
	}
}

//M模式带距离的斜率测量
MMeasureDistSlope::MMeasureDistSlope(const MultiItemInfo *ptrMultiItemInfo)
{
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

MMeasureDistSlope::~MMeasureDistSlope()
{
	Esc();
}

void MMeasureDistSlope::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCrossLine();
	m_dist = INVALID_VAL;
	m_slope = INVALID_VAL;
}

void MMeasureDistSlope::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw) {
			    m_draw.DrawCrossLine(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_p1 = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCrossLine(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, true);

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:

			if (m_isDraw) {
			    m_draw.DrawCrossLine(m_tempP);
			    m_draw.DrawDotLine(m_p1, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawDotLine(m_p1, m_tempP, false);

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			m_ptrMan->AddNew(DIST_SLOPE_M, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			Result(SAVE, attr);

			//begin new measure
			m_draw.ChangeCursorType();
			Init();

            		// change pointer
            		ChangePointerWhenMeasure();
			break;

	}
}

void MMeasureDistSlope::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0: // clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawCrossLine(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCrossLine(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;
			m_dist = INVALID_VAL;
			m_slope = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void MMeasureDistSlope::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0:
			if (m_isDraw) {
			    m_draw.DrawCrossLine(m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCrossLine(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw) {
			    m_draw.DrawCrossLine(m_tempP);
			    m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCrossLine(p);
			m_draw.DrawDotLine(m_p1, p, true);
			m_isDraw = TRUE;

			Result(NOT_SAVE, attr);
			break;

		default:
			break;
	}
}

void MMeasureDistSlope::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCrossLine(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.DrawCursor(m_tempP, FALSE);
		m_draw.DrawCrossLine(m_tempP);
		m_draw.SetCursor(m_tempP);
	}
}

void MMeasureDistSlope::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			m_draw.DrawCrossLine(m_tempP);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawCrossLine(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, true);
		}
	}
	m_update.ClearMeasure();
}

void MMeasureDistSlope::Result(int saveR, UpdateMeasure::ResultAttr& attr)
{
	double data[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

	m_dist = m_calc.MCalcDepthDist(m_p1, m_tempP);
	m_slope = m_calc.MCalcVel(m_p1, m_tempP);

	data[0] = m_dist; unit_coeffi[0] = 1;
	data[1] = m_slope; unit_coeffi[1] = 1;
	for (int i=2; i<MEA_MULTI; i++) {data[i] = INVALID_VAL; unit_coeffi[i] = 0;}
	m_ptrMan->MultiMeaDataMan(data, m_itemInfo, allData, saveR);
	if (saveR == SAVE)
	    m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);
	else
	    m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
	return;
}

//深度方向的距离测量
MMeasureDepth::MMeasureDepth(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

MMeasureDepth::~MMeasureDepth(void)
{
	Esc();
}

void MMeasureDepth::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dist = INVALID_VAL;
}

void MMeasureDepth::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	double allData[SINGLE_MAX];

	switch(m_step) {
		case 0: // first
			if (m_isDraw) {
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 1);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);//第一次按左键就画好完整的测量线
			m_draw.DrawDotLine(m_p1, m_tempP, true);

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			m_ptrMan->AddNew(DEPTH_DIST_M, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			m_dist = m_calc.MCalcDepthDist(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_dist, m_itemInfo, allData, SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//begin new dist measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;
	}
}

void MMeasureDepth::PressRight(POINT p)
{
	switch(m_step) {
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;
			m_dist = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void MMeasureDepth::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	double allData[SINGLE_MAX];

	switch(m_step) {
		case 0:
			if (m_isDraw) {
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw) {
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			p.x = m_p1.x;
			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, true);
			m_isDraw = TRUE;

			m_dist = m_calc.MCalcDepthDist(m_p1, m_tempP);

			m_ptrMan->SingleMeaDataMan(m_dist, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			break;

		default:
			break;
	}
}

void MMeasureDepth::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_p1, FALSE);
		m_draw.DrawCursor(m_p1);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.SetCursor(m_tempP);
	}
}

void MMeasureDepth::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, true);
		}
		m_update.ClearMeasure();
	}
	m_update.ClearMeasure();
}

///> 射血分数-M模式的Teicholz方法
MMeasureEFTei::MMeasureEFTei(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

MMeasureEFTei::~MMeasureEFTei()
{
	Esc();
}

void MMeasureEFTei::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_lvdd = INVALID_VAL;
	m_lvds = INVALID_VAL;
	m_lvvd = INVALID_VAL;
	m_lvvs = INVALID_VAL;
	m_ef = INVALID_VAL;
}

void MMeasureEFTei::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

    double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_lvdd_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 1);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lvdd_start, m_tempP);
			m_lvdd_end = m_tempP;

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_lvdd_end = m_tempP;
			m_lvdd = m_calc.MCalcDepth(m_lvdd_start, m_lvdd_end);
			m_lvvd = m_calc.MCalcLvTei(m_lvdd);
	//		m_update.MEF(m_lvvd, 0, 0, attr);
			m_update.MEF(m_lvdd, m_lvds, m_lvvd, m_lvvs, m_ef, attr, true);
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.DrawOrderNumber(m_lvdd_end, m_draw.GetOrderNumber());

			m_step = 2;
			break;

		case 2: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_lvds_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 1);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lvds_start, m_tempP);
			m_lvds_end = m_tempP;

			m_step = 3;
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_lvds_end = m_tempP;

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_lvdd_start);
			vec.push_back(m_lvdd_end);
			vec.push_back(m_lvds_start);
			vec.push_back(m_lvds_end);
			m_ptrMan->AddNew(EF, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			m_lvds = m_calc.MCalcDepth(m_lvds_start, m_lvds_end);
			m_lvvs = m_calc.MCalcLvTei(m_lvds);
			m_ef = m_calc.CalcEf(m_lvvd, m_lvvs);
		//	m_update.MEF(m_lvdd, m_lvds, m_lvvd, m_lvvs, m_ef, attr, false);

            m_ptrMan->SingleMeaDataMan(m_ef, m_itemInfo, allData, SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//begin new dist measure
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

			//default:
			//	break;
	}
}

void MMeasureEFTei::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_lvdd_end);
				m_draw.DrawLine(m_lvdd_start, m_lvdd_end);
			}

			m_draw.DrawCursor(m_lvdd_start, FALSE);
			m_draw.DrawCursor(m_lvdd_start);
			m_draw.SetCursor(m_tempP);
			m_tempP = m_lvdd_start;
			m_isDraw = TRUE;
			m_step = 0;
			m_lvdd = INVALID_VAL;
			m_lvds = INVALID_VAL;
			m_lvvd = INVALID_VAL;
			m_lvvs = INVALID_VAL;
			m_ef = INVALID_VAL;

			m_update.ClearLast();
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_lvds_start);
			}

			m_draw.DrawCursor(m_lvdd_end, FALSE);
			m_draw.DrawCursor(m_lvdd_end);
			m_draw.SetCursor(m_lvdd_end);
			m_draw.DrawOrderNumber(m_lvdd_end, m_draw.GetOrderNumber());
			m_tempP = m_lvdd_end;
			m_isDraw = TRUE;
			m_step = 1;

			m_update.ClearLast();
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_lvds_end);
				m_draw.DrawLine(m_lvds_start, m_lvds_end);
			}

			m_draw.DrawCursor(m_lvds_start, FALSE);
			m_draw.DrawCursor(m_lvds_start);
			m_draw.SetCursor(m_lvds_start);
			m_tempP = m_lvds_start;
			m_isDraw = TRUE;
			m_step = 2;
			m_lvvs = INVALID_VAL;
			m_lvds = INVALID_VAL;
			m_ef = INVALID_VAL;

			m_update.ClearLast();
			break;

		default:
			break;
	}
}

void MMeasureEFTei::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_lvdd_start = m_tempP;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_lvdd_start, m_tempP);
			}

			m_tempP.y = p.y;
			m_lvdd_end.y = m_tempP.y;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lvdd_start, m_tempP);
			//	m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;

			m_lvdd = m_calc.MCalcDepth(m_lvdd_start, m_lvdd_end);
			m_lvvd = m_calc.MCalcLvTei(m_lvdd);
			m_update.MEF(m_lvdd, m_lvds, m_lvvd, m_lvvs, m_ef, attr, true);
	//		m_update.MEF(m_lvvd, 0, 0, attr);
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_lvds_start = m_tempP;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_lvds_start, m_tempP);
			}

			m_tempP.y = p.y;
			m_lvds_end.y = m_tempP.y;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lvds_start, m_tempP);
			//	m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;

			m_lvds = m_calc.MCalcDepth(m_lvds_start, m_lvds_end);
			m_lvvs = m_calc.MCalcLvTei(m_lvds);
			m_ef = m_calc.CalcEf(m_lvvd, m_lvvs);
			m_update.MEF(m_lvdd, m_lvds, m_lvvd, m_lvvs, m_ef, attr, true);
		//	m_update.MEF(m_lvvd, m_lvvs, m_ef, attr);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void MMeasureEFTei::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_p1, FALSE);
		m_draw.DrawCursor(m_p1);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;

		m_lvdd_start = m_p1;
		m_lvdd_end = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 3)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_p1, FALSE);
		m_draw.DrawCursor(m_p1);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;

		m_lvds_start = m_p1;
		m_lvds_end = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
}

void MMeasureEFTei::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
		}
	}
	else if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_lvdd_start, FALSE);
			m_draw.DrawLine(m_lvdd_start, m_tempP);
		}
		m_update.ClearMeasure();
	}
	else if (m_step == 2)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_lvdd_start, FALSE);
			m_draw.DrawCursor(m_lvdd_end, FALSE);
			m_draw.DrawLine(m_lvdd_start, m_lvdd_end);

			m_draw.DrawCursor(m_tempP);
			m_draw.DrawOrderNumber(m_lvdd_end, m_draw.GetOrderNumber());
		}
		m_update.ClearMeasure();
	}
	else if (m_step == 3)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_lvdd_start, FALSE);
			m_draw.DrawCursor(m_lvdd_end, FALSE);
			m_draw.DrawLine(m_lvdd_start, m_lvdd_end);
			m_draw.DrawOrderNumber(m_lvdd_end, m_draw.GetOrderNumber());

			m_draw.DrawCursor(m_lvds_start, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lvds_start, m_tempP);
		}
		m_update.ClearMeasure();
	}
}