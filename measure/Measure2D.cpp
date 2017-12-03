/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: Measure2D.cpp
 * @brief: measure in 2D scan mode
 *
 * version: V1.0
 * date: 2009-7-9
 * @author: zhanglei
 */

#include <vector>
#include <stdlib.h>
#include <math.h>
#include <libavutil/time.h>
#include "Def.h"
#include "measure/Measure2D.h"
#include "measure/MeasureMan.h"
#include "display/ImageArea.h"
#include "measure/MeasureFactory.h"
#include "measure/DrawHistogram.h"
#include "calcPeople/MeaFactoryMainSingle.h"
#include "display/HintArea.h"
#include <stdio.h>
#include <errno.h>
#include "display/gui_global.h"
#include <iostream>
//#include "../IMT/IMTDef.h"
#include <IMT.h>
#include "measure/CDrawIMT.h"
#include "measure/MenuMeasure.h"
using namespace std;
using std::vector;

///>两点法测距离, 深度方向上测距离
D2MeasureDistDot::D2MeasureDistDot(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureDistDot::~D2MeasureDistDot()
{
	Esc();
}

void D2MeasureDistDot::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dist = INVALID_VAL;
}

void D2MeasureDistDot::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			{
				ModeStatus modeRotate;
				m_2DRotate = modeRotate.GetRotate();

				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}
				m_draw.DrawCursor(m_tempP, FALSE);
				m_p1 = m_tempP;
				if (m_itemInfo->meaType == DIST_DOT)//两点法测距离
				{
					m_tempP = m_draw.CalcNextP(m_tempP, 0);
					m_draw.SetCursor(m_tempP);
				}
			//	else//深度方向测距离
			//		m_tempP = m_draw.CalcNextP(m_tempP, 1);

				m_draw.DrawCursor(m_tempP);//第一次按左键就画好完整的测量线
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE); // modify: 标记为正在绘制的线
				m_isDraw = TRUE;
				m_step = 1;
			}
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawDotLine(m_p1, m_tempP, TRUE); // modify:擦出正在绘制的线
				}
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawDotLine(m_p1, m_tempP, FALSE); // modify:修改当前线颜色为确认的线

				// save measure result
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_p1);
				vec.push_back(m_tempP);
				if (m_itemInfo->meaType == DIST_DOT)
				    m_ptrMan->AddNew(DIST_DOT, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor()); // modify: 添加光标类型，大小，确认颜色
				else
				    m_ptrMan->AddNew(DEPTH_DIST, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

				// 确定测量结果，第二个参数为false时表示测量结束
				m_dist = m_calc.D2CalcDist(m_p1, m_tempP);

				m_ptrMan->SingleMeaDataMan(m_dist, m_itemInfo, allData, SAVE);
				m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

				//begin new dist measure
				m_draw.ChangeCursorType(); // modify:更改鼠标类型
				Init();

				// change pointer
				ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureDistDot::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			//清除已经测量的结果
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE); // modify：擦出当前线
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_dist = INVALID_VAL;
			//清除正在测量的结果
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureDistDot::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();
	double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE); // modify:擦除当前线
			}
			if (m_itemInfo->meaType==DIST_DOT)
				m_tempP = p;
			else
			{
				if ((m_2DRotate == 90) || (m_2DRotate == 270))
					m_tempP.x = p.x;
				else
					m_tempP.y = p.y;
			}
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE); // modify:绘制当前线
			m_isDraw = TRUE;

			m_dist = m_calc.D2CalcDist(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_dist, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureDistDot::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
		}
		m_draw.DrawCursor(m_p1, FALSE);
		m_draw.DrawCursor(m_p1);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
		m_draw.SetCursor(m_tempP);
	}
}

void D2MeasureDistDot::Esc()
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
}

///> Dist-两线法测距离
D2MeasureDistLine::D2MeasureDistLine(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureDistLine::~D2MeasureDistLine()
{
	Esc();
}

void D2MeasureDistLine::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dist = INVALID_VAL;
}

void D2MeasureDistLine::PressLeft(POINT p)
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

			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, TRUE);
			m_tan = (m_tempP.y==m_p1.y) ? 1000.0 : ((double)(m_p1.x - m_tempP.x) / (double)(m_tempP.y - m_p1.y));
			m_draw.DrawTanBeeline(m_p1, m_tan, TRUE);
			m_draw.DrawTanBeeline(m_tempP, m_tan, TRUE);
			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawLine(m_p1, m_tempP, TRUE);
					m_draw.DrawTanBeeline(m_p1, m_tan, TRUE);
					m_draw.DrawTanBeeline(m_tempP, m_tan, TRUE);
				}
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawLine(m_p1, m_tempP, FALSE);
				m_tan = (m_tempP.y==m_p1.y) ? 1000.0 : ((double)(m_p1.x - m_tempP.x) / (double)(m_tempP.y - m_p1.y));
				m_draw.DrawTanBeeline(m_p1, m_tan, FALSE);
				m_draw.DrawTanBeeline(m_tempP, m_tan, FALSE);
				// 确定测量结果，第二个参数为false时表示测量结束
				m_dist = m_calc.D2CalcDist(m_p1, m_tempP);

				// save measure result
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_p1);
				vec.push_back(m_tempP);
				m_ptrMan->AddNew(DIST_LINE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

                m_ptrMan->SingleMeaDataMan(m_dist, m_itemInfo, allData, SAVE);
				m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//	m_update.D2Dist(m_dist, attr, false);
				//begin new dist measure
				m_draw.ChangeCursorType();
				Init();

				// change pointer
				ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureDistLine::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP, TRUE);
				m_draw.DrawTanBeeline(m_p1, m_tan, TRUE);
				m_draw.DrawTanBeeline(m_tempP, m_tan, TRUE);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_dist = INVALID_VAL;

			//清除正在测量的结果
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureDistLine::MouseMove(POINT p)
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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP, TRUE);
				m_draw.DrawTanBeeline(m_p1, m_tan, TRUE);
				m_draw.DrawTanBeeline(m_tempP, m_tan, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, TRUE);
			m_tan = (m_tempP.y==m_p1.y) ? 1000.0 : ((double)(m_p1.x - m_tempP.x) / (double)(m_tempP.y - m_p1.y));
			m_draw.DrawTanBeeline(m_p1, m_tan, TRUE);
			m_draw.DrawTanBeeline(m_tempP, m_tan, TRUE);
			m_isDraw = TRUE;

			m_dist = m_calc.D2CalcDist(m_p1, m_tempP);
			m_update.D2Dist(m_dist, attr, true);
			break;

		default:
			break;
	}
}

void D2MeasureDistLine::Change()
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

void D2MeasureDistLine::Esc()
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
			m_draw.DrawLine(m_p1, m_tempP, TRUE);
			m_draw.DrawTanBeeline(m_p1, m_tan, TRUE);
			m_draw.DrawTanBeeline(m_tempP, m_tan, TRUE);
		}
		m_update.ClearMeasure();
	}
}

///>轨迹法测长度
D2MeasureLengthTrack::D2MeasureLengthTrack(const SingleItemInfo *ptrSingleItemInfo):MOUSE_INTERVAL(2)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();

    Init();

}
D2MeasureLengthTrack::~D2MeasureLengthTrack()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void D2MeasureLengthTrack::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_length = INVALID_VAL;
	m_mouse_count = 0;
	m_track.clear();
	m_trackTemp.clear();
}

void D2MeasureLengthTrack::PressLeft(POINT p)
{
#ifdef EMP_355
    HintArea::GetInstance()->UpdateHint(_("[Length]: <Auto> to cancel or redraw."));
#else
    HintArea::GetInstance()->UpdateHint(_("[Length]: <Value> to cancel or redraw."));
#endif
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();
	double allData[MULTI_MAX];

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_p1 = m_tempP;
			m_line_tempP = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_length = (double)0;

			m_step = 1;
			m_isDraw = TRUE;

			m_track.clear();
            m_trackTemp.clear();
			m_track.push_back(m_p1);

			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				for(int i = (m_track.size() - 1); i != 0; i--)
				    m_draw.DrawTraceLine(m_track[i-1], m_track[i], TRUE);
				    //m_draw.DrawTraceLine(m_track[i], m_track[i-1], TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawTraceLine(m_tempP, m_line_tempP, FALSE);
			for(int i = (m_track.size() - 1); i != 0; i--)
			    m_draw.DrawTraceLine(m_track[i-1], m_track[i], FALSE);
			    //m_draw.DrawTraceLine(m_track[i], m_track[i-1], FALSE);

			m_length += m_calc.D2CalcLenTrack(m_line_tempP, m_tempP);

			// save measure result
			m_track.push_back(m_tempP);
			m_ptrMan->AddNew(LENGTH_TRACK, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

            m_ptrMan->SingleMeaDataMan((double)m_length, m_itemInfo, allData, SAVE);
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//m_update.D2LenTrack(m_length, attr, false);
			//begin new track length measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();

            HintArea::GetInstance()->ClearHint();
            break;

		default:
			break;
	}
}
void D2MeasureLengthTrack::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}

				m_tempP = m_p1;
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);

				unsigned int i;
				size_t size;
				size = m_track.size();
				for(i=(size-1); i!=0; i--)
				{
				    m_draw.DrawTraceLine(m_track[i-1], m_track[i], TRUE);
				    //m_draw.DrawTraceLine(m_track[i], m_track[i-1], TRUE);
					//	m_track.pop_back();
				}
				m_track.clear();//清除vector中的轨迹记录
                m_trackTemp.clear();

				m_isDraw = TRUE;
				m_step = 0;
				m_length = INVALID_VAL;
				m_mouse_count = 0;
				//	m_update.ClearLast();
				//清除正在测量的结果
				m_update.ClearMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureLengthTrack::MouseMove(POINT p)
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
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_mouse_count++;
			if (m_mouse_count >= MOUSE_INTERVAL)
			{
				m_draw.DrawTraceLine(m_line_tempP, p, TRUE);
				//m_draw.DrawTraceLine(p, m_line_tempP, TRUE);
				m_length += m_calc.D2CalcLenTrack(m_line_tempP, p);
				m_update.D2LenTrack(m_length, attr);
				m_line_tempP = p;
				m_track.push_back(m_line_tempP);
				m_mouse_count = 0;
			}
			m_tempP = p;
			m_draw.DrawCursor(p);
            m_isDraw = TRUE;
            m_trackTemp.clear();
			break;
		default:
			break;
	}
}

void D2MeasureLengthTrack::Value(EKnobOper opr)
{
    UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();
    int size_track = m_track.size();
    int size_track_temp = m_trackTemp.size();

    if(opr == ADD)
    {//redraw
        if((size_track > 0) && (size_track_temp > 0))
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }

            m_tempP = m_trackTemp[size_track_temp - 1];
            m_draw.DrawTraceLine(m_track[size_track -1], m_tempP, TRUE);
            m_length += m_calc.D2CalcLenTrack(m_track[size_track - 1], m_tempP);
            m_update.D2LenTrack(m_length, attr);
            m_track.push_back(m_tempP);
            m_trackTemp.pop_back();
            m_line_tempP = m_tempP;

            if(m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        else
            return;
    }
    else if(opr == SUB)
    {//clear
        if(size_track > 1)
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        if(size_track > 0)
        {
            m_tempP = m_track[size_track - 1];
            m_trackTemp.push_back(m_tempP);
            m_track.pop_back();
            size_track = m_track.size();
        }
        else
            return;

        if(size_track == 0)
        {
            HintArea::GetInstance()->ClearHint();
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP, FALSE);
            }
            m_step = 0;
            return;
        }

        m_draw.DrawTraceLine(m_track[size_track - 1], m_tempP, TRUE);
        if(size_track >= 1)
        {
            m_length -= m_calc.D2CalcLenTrack(m_track[size_track - 1], m_tempP);
            m_update.D2LenTrack(abs(m_length), attr);
        }

        m_tempP = m_track[size_track - 1];
        m_draw.DrawCursor(m_tempP);
        m_line_tempP = m_tempP;
    }
    else
    {
        return;
    }

    m_isDraw = TRUE;
}

void D2MeasureLengthTrack::Change()
{
	return;
}

void D2MeasureLengthTrack::Esc()
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
		int i;
		int vec_size;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			vec_size = m_track.size() - 1;
			for(i=0; i<vec_size; i++)
				m_draw.DrawTraceLine(m_track[i], m_track[i+1], TRUE);
		}
		m_update.ClearMeasure();
	}
	m_track.clear();
    m_trackTemp.clear();
}

///>描点法测长度
D2MeasureLengthDot::D2MeasureLengthDot(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureLengthDot::~D2MeasureLengthDot()
{
	Esc();
}

void D2MeasureLengthDot::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_length = INVALID_VAL;
	m_dot.clear();

}

void D2MeasureLengthDot::PressLeft(POINT p)
{
	unsigned int vec_size = 0;
	unsigned int i;

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
			m_p1 = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);

			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);

			m_step = 1;
			m_isDraw = TRUE;

			m_dot.clear();
			m_dot.push_back(m_p1);
			HintArea::GetInstance()->UpdateHint(_("[Length-Point]: <Update> to set shape point."));
			break;

		case 1:
			HintArea::GetInstance()->ClearHint();

			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);

			m_length = (double)0;
			m_dot.push_back(m_tempP);
			vec_size = m_dot.size();
			for(i=0; i!=(vec_size-1); i++)
			{
				m_draw.DrawLine(m_dot[i], m_dot[i+1]);
				m_length += m_calc.D2CalcDist(m_dot[i], m_dot[i+1]);
			}

			//清除掉中间多余的光标
			for(i=1; i!=vec_size-1; i++)
			{
				m_draw.DrawCursor(m_dot[i], FALSE);
			}
            //>为了使结果显示的数目和测量线的数目一致，在此用true的方式多显示了一次
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
            // save measure result
			m_ptrMan->AddNew(LENGTH_DOT, m_draw.GetCursorType(), m_dot, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				m_ptrMan->SingleMeaDataMan(m_length, m_itemInfo, allData, SAVE);
				m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

                //m_update.D2LenTrack(m_length, attr, false);
			//begin new track length measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureLengthDot::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			HintArea::GetInstance()->ClearHint();
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_p1;

			unsigned int i, vec_size;
			vec_size = m_dot.size();
			for(i=0; i!=vec_size; i++)
			{
				m_draw.DrawCursor(m_dot[i], FALSE);
			}
			m_dot.clear();

			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;
			m_length = INVALID_VAL;

			m_update.ClearMeasure();
			HintArea::GetInstance()->ClearHint();
			break;

		default:
			break;
	}
}
void D2MeasureLengthDot::MouseMove(POINT p)
{
	if (m_isDraw)
	{
		m_draw.DrawCursor(m_tempP);
	}

	m_tempP = p;
	m_draw.DrawCursor(m_tempP);
    m_isDraw = TRUE;
}

void D2MeasureLengthDot::Esc()
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
		int i;
		int vec_size;

		if (m_isDraw)
		{
			vec_size = m_dot.size();
			m_draw.DrawCursor(m_tempP);
			for(i=0; i<vec_size; i++)
				m_draw.DrawCursor(m_dot[i], FALSE);
		}
		m_update.ClearMeasure();
		HintArea::GetInstance()->ClearHint();
	}
}

void D2MeasureLengthDot::Change()
{
	switch(m_step)
	{
		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_dot.push_back(m_tempP);

			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;//TRUE;=======be careful
			break;
		default:
break;
	}
}

///>轨迹法测面积
D2MeasureAreaTrack::D2MeasureAreaTrack(const SingleItemInfo *ptrSingleItemInfo):MOUSE_INTERVAL(2)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureAreaTrack::~D2MeasureAreaTrack()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void D2MeasureAreaTrack::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_perimeter = INVALID_VAL;
	m_area = INVALID_VAL;
	m_mouse_count = 0;
	m_track.clear();
    m_trackTemp.clear();
}

void D2MeasureAreaTrack::PressLeft(POINT p)
{
#ifdef EMP_355
    HintArea::GetInstance()->UpdateHint(_("[AreaTrack]: <Auto> to cancel or redraw."));
#else
    HintArea::GetInstance()->UpdateHint(_("[AreaTrack]: <Value> to cancel or redraw."));
#endif
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	double allData[SINGLE_MAX + 1];

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_p1 = m_tempP;
			m_line_tempP = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);

			m_perimeter = (double)0;
			m_area = (double)0;

			m_step = 1;
			m_isDraw = TRUE;

			m_track.clear();
			m_track.push_back(m_p1);
            m_trackTemp.clear();

			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				for(unsigned int i = (m_track.size() - 1); i!=0; i--)
				    m_draw.DrawTraceLine(m_track[i-1], m_track[i], TRUE);
				    //m_draw.DrawTraceLine(m_track[i], m_track[i-1], TRUE);
			}
			for(unsigned int i = (m_track.size() - 1); i!=0; i--)
			    m_draw.DrawTraceLine(m_track[i-1], m_track[i], FALSE);
			    //m_draw.DrawTraceLine(m_track[i], m_track[i-1], FALSE);

			m_draw.DrawTraceLine(m_tempP, m_line_tempP);
			m_perimeter += m_calc.D2CalcLenTrack(m_line_tempP, m_tempP);
			m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_tempP, m_p1);

			m_line_tempP = m_tempP;
			m_draw.DrawTraceLine(m_p1, m_line_tempP);
			//m_draw.DrawTraceLine(m_line_tempP, m_p1);
			m_perimeter += m_calc.D2CalcLenTrack(m_line_tempP, m_p1);
			m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_p1, m_p1);
			if (m_area < 0)
				m_area = 0 - m_area;

			// save measure result
			m_track.push_back(m_tempP);

            //fixed deque_result add by hzq
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);

			m_ptrMan->AddNew(AREA_TRACK, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			double resultTmp;
			resultTmp = (m_itemInfo->meaType == PERI_TRACK) ? m_perimeter : m_area;
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, SAVE);

            m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//begin new track length measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

			// change pointer
			ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureAreaTrack::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}

				m_tempP = m_p1;
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);

				unsigned int i;
				size_t size;
				size = m_track.size();
				for(i=(size-1); i!=0; i--)
				{
				    m_draw.DrawTraceLine(m_track[i-1], m_track[i], TRUE);
				    //m_draw.DrawTraceLine(m_track[i], m_track[i-1], TRUE);
				}
				m_track.clear();
                m_trackTemp.clear();

				m_isDraw = TRUE;
				m_step = 0;
				m_perimeter = INVALID_VAL;
				m_area = INVALID_VAL;
				m_mouse_count = 0;

				//清除正在测量的结果
				m_update.ClearMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureAreaTrack::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	double allData[SINGLE_MAX + 1];

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = p;
			m_draw.DrawCursor(p);

			m_mouse_count++;
			if (m_mouse_count >= MOUSE_INTERVAL)
			{
			    m_draw.DrawTraceLine(m_line_tempP, p, TRUE);
			    //m_draw.DrawTraceLine(p, m_line_tempP, TRUE);
				m_perimeter += m_calc.D2CalcLenTrack(m_line_tempP, p);
				m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_tempP, m_p1);

				double resultTmp;
				resultTmp = (m_itemInfo->meaType == PERI_TRACK) ? m_perimeter : (double)INVALID_VAL;
				m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);

				if (m_itemInfo->meaType == PERI_TRACK)
					m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
				else
					m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);

				m_line_tempP = m_tempP;
				m_track.push_back(m_line_tempP);
				m_mouse_count = 0;
			}
            m_isDraw = TRUE;
            m_trackTemp.clear();

			break;
		default:
			break;
	}
}

void D2MeasureAreaTrack::Value(EKnobOper opr)
{
    UpdateMeasure::ResultAttr attr;
    //attr.cursorType = m_draw.GetCursorType();
	attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();
    int size_track = m_track.size();
    int size_track_temp = m_trackTemp.size();
	double allData[SINGLE_MAX + 1];

    if(opr == ADD)
    {//redraw
        if((size_track > 0) && (size_track_temp > 0))
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }

            m_tempP = m_trackTemp[size_track_temp - 1];
            m_draw.DrawTraceLine(m_track[size_track - 1], m_tempP, TRUE);
            m_perimeter += m_calc.D2CalcLenTrack(m_track[size_track - 1], m_tempP);
            m_area += m_calc.D2CalcAreaTrack(m_track[size_track - 1], m_tempP, m_p1);
            double resultTmp;
            resultTmp = (m_itemInfo->meaType == PERI_TRACK) ? m_perimeter : (double)INVALID_VAL;
            m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);

            if (m_itemInfo->meaType == PERI_TRACK)
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
            else
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);

            m_track.push_back(m_tempP);
            m_trackTemp.pop_back();
            m_line_tempP = m_tempP;

            if(m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        else
            return;
    }
    else if(opr == SUB)
    {//clear
        if(size_track > 1)
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        if(size_track > 0)
        {
            m_tempP = m_track[size_track - 1];
            m_trackTemp.push_back(m_tempP);
            m_track.pop_back();
            size_track = m_track.size();
        }
        else
            return;

        if(size_track == 0)
        {
            HintArea::GetInstance()->ClearHint();
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP, FALSE);
            }
            m_step = 0;
            return;
        }

        m_draw.DrawTraceLine(m_track[size_track - 1], m_tempP, TRUE);
        if(size_track >= 1)
        {
            m_perimeter -= m_calc.D2CalcLenTrack(m_track[size_track - 1], m_tempP);
            m_area -= m_calc.D2CalcAreaTrack(m_track[size_track - 1], m_tempP, m_p1);
            double resultTmp;
            resultTmp = (m_itemInfo->meaType == PERI_TRACK) ? m_perimeter : (double)INVALID_VAL;
            m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);

            if (m_itemInfo->meaType == PERI_TRACK)
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
            else
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
        }

        m_tempP = m_track[size_track - 1];
        m_draw.DrawCursor(m_tempP);
        m_line_tempP = m_tempP;
    }
    else
    {
        return;
    }

    m_isDraw = TRUE;
}

void D2MeasureAreaTrack::Change()
{
	return;
}

void D2MeasureAreaTrack::Esc()
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
		int i;
		int vec_size;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			vec_size = m_track.size() - 1;
			for(i=0; i<vec_size; i++)
				m_draw.DrawTraceLine(m_track[i], m_track[i+1], TRUE);
		}
	}
	m_track.clear();
    m_trackTemp.clear();
}

///>Simpson's测EDV和ESV
D2MeasureSimpson::D2MeasureSimpson(const MultiItemInfo *ptrMultiItemInfo):MOUSE_INTERVAL(2)
{
	m_item = ptrMultiItemInfo->item;
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureSimpson::~D2MeasureSimpson()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void D2MeasureSimpson::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_area = INVALID_VAL;
	m_length = INVALID_VAL;
	m_vol = INVALID_VAL;
	m_mouse_count = 0;
	m_directX = m_directY = m_direct = 0;
	m_track.clear();
    HintArea::GetInstance()->ClearHint();
}

void D2MeasureSimpson::PressLeft(POINT p)
{
    unsigned int i;
	int diamSquSum;
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
        case 0: // first
#ifdef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[Simpsom]: <Auto> to cancle or redraw."));
#else
            HintArea::GetInstance()->UpdateHint(_("[Simpsom]: <Value> to cancle or redraw."));
#endif
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_p1 = m_tempP;
			m_line_tempP = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);

			m_length = (double)0;
			m_area = (double)0;

			m_step = 1;
			m_isDraw = TRUE;

			m_track.clear();
			m_track.push_back(m_p1);
            m_trackTemp.clear();
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				for(i = (m_track.size() - 1); i!=0; i--)
				    //m_draw.DrawLine(m_track[i], m_track[i-1], TRUE);
				    m_draw.DrawLine(m_track[i-1], m_track[i], TRUE);
			}
			for(i = (m_track.size() - 1); i!=0; i--)
			    //m_draw.DrawLine(m_track[i], m_track[i-1], FALSE);
			    m_draw.DrawLine(m_track[i-1], m_track[i], FALSE);

			//m_draw.DrawLine(m_tempP, m_line_tempP);
			m_draw.DrawLine(m_line_tempP, m_tempP);
			m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_tempP, m_p1);

			m_line_tempP = m_tempP;
			//m_draw.DrawLine(m_line_tempP, m_p1);
			m_draw.DrawLine(m_p1, m_line_tempP);
			m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_p1, m_p1);
			if (m_area < 0)
				m_area = 0 - m_area;
			m_track.push_back(m_line_tempP);

			m_lenStart.x = (m_p1.x + m_line_tempP.x) / 2;
			m_lenStart.y = (m_p1.y + m_line_tempP.y) / 2;

			m_lenPosi = m_track.size() / 2;
			m_tempP = m_track[m_lenPosi];
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
			diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, TRUE);

			m_length = m_calc.D2CalcDist(m_lenStart, m_tempP);
			m_vol = m_calc.D2CalcSimpson(m_length, diamSquSum);
			dataMea[0] = m_area;
			dataMea[1] = m_length;
			dataMea[2] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			m_step = 2;

			m_direct = (abs(m_directX) < abs(m_directY)) ? 1 : 0;
            m_trackTemp.clear();
            HintArea::GetInstance()->ClearHint();

			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    	m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
				diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_lenStart, m_tempP, FALSE);
			diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, FALSE);
			m_track.push_back(m_track[m_lenPosi]);

			dataMea[0] = m_area;
			dataMea[1] = m_length;
			dataMea[2] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);

			m_ptrMan->AddNew(SIMPSONS, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

			//begin new track length measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureSimpson::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}

				m_tempP = m_p1;
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);

				unsigned int i;
				size_t size;
				size = m_track.size();
				for(i=(size-1); i!=0; i--)
				{
				    //m_draw.DrawLine(m_track[i], m_track[i-1], TRUE);
				    m_draw.DrawLine(m_track[i-1], m_track[i], TRUE);
				}
				m_track.clear();
                m_trackTemp.clear();

				m_isDraw = TRUE;
				m_step = 0;
				m_area = INVALID_VAL;
				m_length = INVALID_VAL;
				m_vol = INVALID_VAL;
				m_mouse_count = 0;
				m_directX = m_directY = m_direct = 0;

				//清除正在测量的结果
				m_update.ClearMeasure();
			}
			break;

		case 2:
			{
				int diamSquSum;

				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
					diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, TRUE);
				}

				m_tempP = m_p1;
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);

				unsigned int i;
				size_t size;
				size = m_track.size();
				for(i=(size-1); i!=0; i--)
				{
					//m_draw.DrawLine(m_track[i], m_track[i-1], FALSE);
					m_draw.DrawLine(m_track[i-1], m_track[i], FALSE);
				}
				m_draw.DrawLine(m_track[0], m_track[size - 1], FALSE);
		//		m_draw.DrawLine(m_line_tempP, m_p1);
				m_track.clear();
                m_trackTemp.clear();

				m_isDraw = TRUE;
				m_step = 0;
				m_area = INVALID_VAL;
				m_length = INVALID_VAL;
				m_vol = INVALID_VAL;
				m_mouse_count = 0;
				m_directX = m_directY = m_direct = 0;

				//清除正在测量的结果
				m_update.ClearMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureSimpson::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	int diamSquSum;

	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 1:
			if (((p.x-m_tempP.x)*(p.x-m_tempP.x) + (p.y-m_tempP.y)*(p.y-m_tempP.y)) < 4)
				break;
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			if (p.x > m_tempP.x)
				m_directX++;
			else if (p.x < m_tempP.x)
				m_directX--;

			if (p.y > m_tempP.y)
				m_directY++;
			else if (p.y < m_tempP.y)
				m_directY--;

			m_tempP = p;
			m_draw.DrawCursor(p);

			m_mouse_count++;
			if (m_mouse_count >= MOUSE_INTERVAL)
			{
				//m_draw.DrawLine(p, m_line_tempP, TRUE);
				m_draw.DrawLine(m_line_tempP, p, TRUE);
                m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_tempP, m_p1);

				m_line_tempP = m_tempP;
				m_track.push_back(m_line_tempP);
				m_mouse_count = 0;
			}
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			m_isDraw = TRUE;
            m_trackTemp.clear();
			break;

		case 2:
			m_slowMouse++;
			if (m_slowMouse > 3)
			{
				m_slowMouse = 0;
			}
			else
				break;
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    	m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
				diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, TRUE);
			}

			if (m_direct == 0)
			{
			//	m_lenPosi = ((m_directX*(p.x - m_tempP.x))<0) ? (m_lenPosi-1) : (m_lenPosi+1);
				if ((m_directX*(p.x - m_tempP.x) >= 0)&&(m_lenPosi < m_track.size() - 2))
					m_lenPosi++;
				else if((m_directX*(p.x - m_tempP.x) < 0) && (m_lenPosi > 1))
					m_lenPosi--;
				//((m_directX*(p.x - m_tempP.x))<0) ? m_lenPosi-- : m_lenPosi++;
			}
			else
			{
				if ((m_directY*(p.y - m_tempP.y) >= 0)&&(m_lenPosi < m_track.size() - 2))
					m_lenPosi++;
				else if((m_directY*(p.y - m_tempP.y) < 0) && (m_lenPosi > 1))
					m_lenPosi--;
			//	m_lenPosi = ((m_directY*(p.y - m_tempP.y))<0) ? (m_lenPosi-1) : (m_lenPosi+1);
			//	((m_directY*(p.y - m_tempP.y))<0) ? m_lenPosi-- : m_lenPosi++;
			}
		//	printf("m_direct= %d, m_directX= %d, m_directY= %d, m_lenPosi= %d\n", m_direct, m_directX, m_directY, m_lenPosi);

		/*
			direcSign = LenEnd(p, m_lenPosi);
			if ((direcSign == 1)&&(m_lenPosi < m_track.size() - 2))
				m_lenPosi++;
			else if((direcSign == -1)&&(m_lenPosi > 1))
				m_lenPosi--;
		*/
			m_tempP = m_track[m_lenPosi];
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lenStart, m_tempP, TRUE);

			diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, TRUE);
			m_length = m_calc.D2CalcDist(m_lenStart, m_tempP);
			m_vol = m_calc.D2CalcSimpson(m_length, diamSquSum);
			dataMea[0] = m_area;
			dataMea[1] = m_length;
			dataMea[2] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);

			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		default:
			break;
	}
}

void D2MeasureSimpson::Value(EKnobOper opr)
{
    UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
    //attr.cursorType = m_draw.GetCursorType();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();
    int size_track = m_track.size();
    int size_track_temp = m_trackTemp.size();
    //printf("size_track=%d  size_track_temp=%d\n", size_track, size_track_temp);

    int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	for(i=0; i<MULTI_MAX; i++)
	{
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

    if(opr == ADD)
    {//redraw
        if((size_track > 0) && (size_track_temp > 0))
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }

            m_tempP = m_trackTemp[size_track_temp - 1];
            if (m_tempP.x > m_track[size_track - 1].x)
                m_directX++;
            else if (m_tempP.x < m_track[size_track - 1].x)
                m_directX--;

            if (m_tempP.y > m_track[size_track - 1].y)
                m_directY++;
            else if (m_tempP.y < m_track[size_track - 1].y)
                m_directY--;

            m_draw.DrawLine(m_track[size_track - 1], m_tempP, TRUE);
            m_area += m_calc.D2CalcAreaTrack(m_track[size_track - 1], m_tempP, m_p1);

            m_track.push_back(m_tempP);
            m_trackTemp.pop_back();
            m_line_tempP = m_tempP;

            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
            if(m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        else
            return;
    }
    else if(opr == SUB)
    {//clear
        if(size_track > 1 && (m_step == 0 || m_step == 1))
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        else
            return;

        if(size_track > 0)
        {
            m_tempP = m_track[size_track - 1];
            m_trackTemp.push_back(m_tempP);
            m_track.pop_back();
            size_track = m_track.size();

            if (m_tempP.x > m_track[size_track - 1].x)
                m_directX++;
            else if (m_tempP.x < m_track[size_track - 1].x)
                m_directX--;

            if (m_tempP.y > m_track[size_track - 1].y)
                m_directY++;
            else if (m_tempP.y < m_track[size_track - 1].y)
                m_directY--;

        }
        else
            return;

        if(size_track == 0)
        {
            HintArea::GetInstance()->ClearHint();
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP, FALSE);
            }
            m_step = 0;
            return;
        }

        m_draw.DrawLine(m_track[size_track - 1], m_tempP, TRUE);

        if(size_track >= 1)
        {
            m_area -= m_calc.D2CalcAreaTrack(m_track[size_track - 1], m_tempP, m_p1);
        }
        m_tempP = m_track[size_track - 1];
        m_draw.DrawCursor(m_tempP);
        m_line_tempP = m_tempP;

        m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
        m_isDraw = TRUE;
    }
    else
    {
        return;
    }
}

int D2MeasureSimpson::DiamLine(POINT lenStart, POINT lenEnd, int lenEndPosi, bool isCur)
{
	int i;
	const int diamNum = 20;
	double diamCenterX, diamCenterY;
	double diamSlope;//直径的斜率
	double lenLength;
	int vecSize;
	int tempPosi1, tempPosi2;
	int diamSquSum = 0;

	vecSize = m_track.size();

	tempPosi1 = lenEndPosi;
	tempPosi2 = lenEndPosi;

	diamSlope = (double)(lenStart.x - lenEnd.x) / (double)(lenEnd.y - lenStart.y);
	lenLength = sqrt((double)((lenEnd.y - lenStart.y) * (lenEnd.y - lenStart.y) + (lenEnd.x - lenStart.x) * (lenEnd.x - lenStart.x)));

	double tempX1, tempY1, tempX2, tempY2;
//	double tempX21, tempY21, tempX22, tempY22;
	POINT tmpFront, tmpBeh;
	POINT diamEP1, diamEP2, diamCen;
	double temp;
	double x0, y0, x1, y1, x2, y2;
	for (i=0; i<diamNum; i++)
	{
		diamCenterX = lenEnd.x + (lenStart.x - lenEnd.x) * (double)(i+1) / (double)(diamNum + 1);
		diamCenterY = lenEnd.y + (lenStart.y - lenEnd.y) * (double)(i+1) / (double)(diamNum + 1);
		diamCen.x = (int)diamCenterX;
		diamCen.y = (int)diamCenterY;

		//第1个端点
		tmpFront = m_track[tempPosi1];
		tempX1 = (double)tmpFront.x;
		tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
		tmpBeh = (tempPosi1 == 0) ? lenStart : m_track[tempPosi1 - 1];
		tempX2 = (double)tmpBeh.x;
		tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);
		while((tempY1 - tmpFront.y) * (tempY2 - tmpBeh.y) > 0)
		{
			if (tempPosi1 > 0) tempPosi1--;
			tmpFront = m_track[tempPosi1];
			tempX1 = (double)tmpFront.x;
			tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
			tmpBeh = (tempPosi1 == 0) ? lenStart : m_track[tempPosi1 - 1];
			tempX2 = (double)tmpBeh.x;
			tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);

			if(tempPosi1 == 0)
				break;
		}
		x0 = diamCenterX;
		y0 = diamCenterY;
		x1 = (double)tmpFront.x;
		y1 = (double)tmpFront.y;
		x2 = (double)tmpBeh.x;
		y2 = (double)tmpBeh.y;

		temp = ((y0-y2)*x1 - (y0-y1)*x2 + diamSlope*x0*(x2-x1)) / (diamSlope*(x2-x1)+y1-y2);
		diamEP1.x = (int)temp;
		diamEP1.y = (int)(diamSlope * (temp - x0) + y0);

		//第2个端点
		tmpFront = m_track[tempPosi2];
		tempX1 = (double)tmpFront.x;
		tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
		tmpBeh = (tempPosi2 == vecSize - 1) ? lenStart : m_track[tempPosi2 + 1];
		tempX2 = (double)tmpBeh.x;
		tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);
		while((tempY1 - tmpFront.y) * (tempY2 - tmpBeh.y) > 0)
		{
			if (tempPosi1 < vecSize - 1) tempPosi2++;
			tmpFront = m_track[tempPosi2];
			tempX1 = (double)tmpFront.x;
			tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
			tmpBeh = (tempPosi2 == vecSize - 1) ? lenStart : m_track[tempPosi2 + 1];
			tempX2 = (double)tmpBeh.x;
			tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);

			if(tempPosi2 == vecSize - 1)
				break;
		}
		x0 = diamCenterX;
		y0 = diamCenterY;
		x1 = (double)tmpFront.x;
		y1 = (double)tmpFront.y;
		x2 = (double)tmpBeh.x;
		y2 = (double)tmpBeh.y;

		temp = ((y0-y2)*x1 - (y0-y1)*x2 + diamSlope*x0*(x2-x1)) / (diamSlope*(x2-x1)+y1-y2);
		diamEP2.x = (int)temp;
		diamEP2.y = (int)(diamSlope * (temp - x0) + y0);

		m_draw.DrawLine(diamEP1, diamEP2, isCur);
		diamSquSum += m_calc.D2CalcSquDist(diamEP1, diamEP2);

		if (isCur == FALSE)
		{
			m_track.push_back(diamEP1);
			m_track.push_back(diamEP2);
		}
	}

	return diamSquSum;
}

int D2MeasureSimpson::LenEnd(POINT mousePoint, int vecPosi)
{
	int vecSize;
	int xySign = 0;

	vecSize = m_track.size();

	if (vecPosi < vecSize - 1)
	{
		if (abs(m_track[vecPosi + 1].x - m_tempP.x) > abs(m_track[vecPosi + 1].y - m_tempP.y))
			xySign = 1;
		else
			xySign = 0;
	}
	else
	{
		if (abs(m_track[vecPosi - 1].x - m_tempP.x) > abs(m_track[vecPosi - 1].y - m_tempP.y))
			xySign = 1;
		else
			xySign = 0;
	}

	xySign = 1;
	if (xySign == 1)
	{
		if (mousePoint.x - m_tempP.x > 0)
		{
			if ((m_track[vecPosi + 1].x - m_tempP.x >= 0) && (vecPosi < vecSize - 1))
				return 1;
			else if ((m_track[vecPosi + 1].x - m_tempP.x < 0) && (vecPosi > 0))
				return -1;
			else
				return 0;
		}
		else if (mousePoint.x - m_tempP.x < 0)
		{
			if ((m_track[vecPosi + 1].x - m_tempP.x <= 0) && (vecPosi < vecSize - 1))
				return 1;
			else if ((m_track[vecPosi + 1].x - m_tempP.x > 0) && (vecPosi > 0))
				return -1;
			else
				return 0;
		}
		else
			return 0;
	}
	else
	{
		if (mousePoint.y - m_tempP.y > 0)
		{
			if ((m_track[vecPosi + 1].y - m_tempP.y >= 0) && (vecPosi < vecSize - 1))
				return 1;
			else if ((m_track[vecPosi + 1].y - m_tempP.y < 0) && (vecPosi > 0))
				return -1;
			else
				return 0;
		}
		else if (mousePoint.y - m_tempP.y < 0)
		{
			if ((m_track[vecPosi + 1].y - m_tempP.y <= 0) && (vecPosi < vecSize - 1))
				return 1;
			else if ((m_track[vecPosi + 1].y - m_tempP.y > 0) && (vecPosi > 0))
				return -1;
			else
				return 0;
		}
		else
			return 0;
	}

	return 0;
}

void D2MeasureSimpson::Change()
{
	return;
}

void D2MeasureSimpson::Esc()
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
		int i;
		int vec_size;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			vec_size = m_track.size() - 1;
			for(i=0; i<vec_size; i++)
				m_draw.DrawLine(m_track[i], m_track[i+1], TRUE);
		}
	}
	else
	{
		int i;
		int vec_size;
		int diamSquSum;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
			diamSquSum = DiamLine(m_lenStart, m_tempP, m_lenPosi, TRUE);
		}

		m_draw.DrawCursor(m_p1, FALSE);
		vec_size = m_track.size() - 1;
		for(i=0; i<vec_size; i++)
			m_draw.DrawLine(m_track[i], m_track[i+1], FALSE);

		m_draw.DrawLine(m_track[0], m_track[vec_size - 1]);
	}
	m_track.clear();
    m_trackTemp.clear();
}

///>A/L法测EDV和ESV
D2MeasureAL::D2MeasureAL(const MultiItemInfo *ptrMultiItemInfo):MOUSE_INTERVAL(2)
{
	m_item = ptrMultiItemInfo->item;
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureAL::~D2MeasureAL()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void D2MeasureAL::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_area = INVALID_VAL;
	m_length = INVALID_VAL;
	m_vol = INVALID_VAL;
	m_mouse_count = 0;
	m_track.clear();
    m_trackTemp.clear();
    HintArea::GetInstance()->ClearHint();
}

void D2MeasureAL::PressLeft(POINT p)
{
	unsigned int i;
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0: // first
#ifdef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[A/L]: <Auto> to cancle or redraw."));
#else
            HintArea::GetInstance()->UpdateHint(_("[A/L]: <Value> to cancle or redraw."));
#endif
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_p1 = m_tempP;
			m_line_tempP = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);

			m_length = (double)0;
			m_area = (double)0;

			m_step = 1;
			m_isDraw = TRUE;

			m_track.clear();
			m_track.push_back(m_p1);
            m_trackTemp.clear();
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				for(i = (m_track.size() - 1); i!=0; i--)
				    //m_draw.DrawLine(m_track[i], m_track[i-1], TRUE);
				    m_draw.DrawLine(m_track[i-1], m_track[i], TRUE);
			}
			for(i = (m_track.size() - 1); i!=0; i--)
			    //m_draw.DrawLine(m_track[i], m_track[i-1], FALSE);
				    m_draw.DrawLine(m_track[i-1], m_track[i], FALSE);

			//m_draw.DrawLine(m_tempP, m_line_tempP);
			m_draw.DrawLine(m_line_tempP, m_tempP);
			m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_tempP, m_p1);

			m_line_tempP = m_tempP;
			//m_draw.DrawLine(m_line_tempP, m_p1);
			m_draw.DrawLine(m_p1, m_line_tempP);
			m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_p1, m_p1);
			if (m_area < 0)
				m_area = 0 - m_area;
			m_track.push_back(m_line_tempP);

			m_lenStart.x = (m_p1.x + m_line_tempP.x) / 2;
			m_lenStart.y = (m_p1.y + m_line_tempP.y) / 2;

			m_tempP = m_lenStart;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
			m_draw.SetCursor(m_tempP);

			m_length = m_calc.D2CalcDist(m_lenStart, m_tempP);
			m_vol = m_calc.D2CalcAL(m_length, m_area);
			dataMea[0] = m_area;
			dataMea[1] = m_length;
			dataMea[2] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			m_step = 2;
            m_trackTemp.clear();
            HintArea::GetInstance()->ClearHint();

			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    	m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_lenStart, m_tempP, FALSE);
			m_track.push_back(m_tempP);

			dataMea[0] = m_area;
			dataMea[1] = m_length;
			dataMea[2] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);

			m_ptrMan->AddNew(AL, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

			//begin new track length measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureAL::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}

				m_tempP = m_p1;
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);

				unsigned int i;
				size_t size;
				size = m_track.size();
				for(i=(size-1); i!=0; i--)
				{
				    //m_draw.DrawLine(m_track[i], m_track[i-1], TRUE);
				    m_draw.DrawLine(m_track[i-1], m_track[i], TRUE);
				}
				m_track.clear();
                m_trackTemp.clear();

				m_isDraw = TRUE;
				m_step = 0;
				m_area = INVALID_VAL;
				m_length = INVALID_VAL;
				m_vol = INVALID_VAL;
				m_mouse_count = 0;

				//清除正在测量的结果
				m_update.ClearMeasure();
			}
			break;

		case 2:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
				}

				m_tempP = m_p1;
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);

				unsigned int i;
				size_t size;
				size = m_track.size();
				for(i=(size-1); i!=0; i--)
				{
					//m_draw.DrawLine(m_track[i], m_track[i-1], FALSE);
					m_draw.DrawLine(m_track[i-1], m_track[i], FALSE);
				}
				m_draw.DrawLine(m_track[0], m_track[size - 1], FALSE);
		//		m_draw.DrawLine(m_line_tempP, m_p1);
				m_track.clear();
                m_trackTemp.clear();

				m_isDraw = TRUE;
				m_step = 0;
				m_area = INVALID_VAL;
				m_length = INVALID_VAL;
				m_vol = INVALID_VAL;
				m_mouse_count = 0;

				//清除正在测量的结果
				m_update.ClearMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureAL::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = p;
			m_draw.DrawCursor(p);

			m_mouse_count++;
			if (m_mouse_count >= MOUSE_INTERVAL)
			{
				//m_draw.DrawLine(p, m_line_tempP, TRUE);
				m_draw.DrawLine(m_line_tempP, p, TRUE);
				m_area += m_calc.D2CalcAreaTrack(m_line_tempP, m_tempP, m_p1);

				m_line_tempP = m_tempP;
				m_track.push_back(m_line_tempP);
				m_mouse_count = 0;
			}
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			m_isDraw = TRUE;
            m_trackTemp.clear();
			break;

		case 2:
			if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
                m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
            }

			m_tempP = p;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lenStart, m_tempP, TRUE);

			m_length = m_calc.D2CalcDist(m_lenStart, m_tempP);
			m_vol = m_calc.D2CalcAL(m_length, m_area);
			dataMea[0] = m_area;
			dataMea[1] = m_length;
			dataMea[2] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);

			m_isDraw = TRUE;
			break;

		default:
			break;
	}
}

void D2MeasureAL::Value(EKnobOper opr)
{
    UpdateMeasure::ResultAttr attr;
    //attr.cursorType = m_draw.GetCursorType();
	attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();
    int size_track = m_track.size();
    int size_track_temp = m_trackTemp.size();
    PRINTF("size_track=%d  size_track_temp=%d\n", size_track, size_track_temp);

    int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	for(i=0; i<MULTI_MAX; i++)
	{
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

    if(opr == ADD)
    {//redraw
        if((size_track > 0) && (size_track_temp > 0))
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }

            m_tempP = m_trackTemp[size_track_temp - 1];
            m_draw.DrawLine(m_track[size_track - 1], m_tempP, TRUE);
            m_area += m_calc.D2CalcAreaTrack(m_track[size_track - 1], m_tempP, m_p1);

            m_track.push_back(m_tempP);
            m_trackTemp.pop_back();
            m_line_tempP = m_tempP;

            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
            if(m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }

            m_isDraw = TRUE;
        }
        else
            return;
    }
    else if(opr == SUB)
    {//clear
        if(size_track > 1 && (m_step == 0 || m_step == 1))
        {
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
        }
        else
            return;

        if(size_track > 0)
        {
            m_tempP = m_track[size_track - 1];
            m_trackTemp.push_back(m_tempP);
            m_track.pop_back();
            size_track = m_track.size();
        }
        else
            return;

        if(size_track == 0)
        {
            HintArea::GetInstance()->ClearHint();
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP, FALSE);
            }
            m_step = 0;
            return;
        }

        m_draw.DrawLine(m_track[size_track - 1], m_tempP, TRUE);

        if(size_track >= 1)
        {
            m_area -= m_calc.D2CalcAreaTrack(m_track[size_track - 1], m_tempP, m_p1);
        }
        m_tempP = m_track[size_track - 1];
        m_draw.DrawCursor(m_tempP);
        m_line_tempP = m_tempP;

        m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
        m_isDraw = TRUE;
    }
    else
    {
        return;
    }
}

void D2MeasureAL::Change()
{
	return;
}

void D2MeasureAL::Esc()
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
		int i;
		int vec_size;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);
			vec_size = m_track.size() - 1;
			for(i=0; i<vec_size; i++)
                m_draw.DrawLine(m_track[i], m_track[i+1], TRUE);
        }
	}
	else
	{
		int i;
		int vec_size;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_lenStart, m_tempP, TRUE);
		}

		m_draw.DrawCursor(m_p1, FALSE);
		vec_size = m_track.size() - 1;
		for(i=0; i<vec_size; i++)
			m_draw.DrawLine(m_track[i], m_track[i+1], FALSE);

		m_draw.DrawLine(m_track[0], m_track[vec_size - 1]);
	}
	m_track.clear();
	m_trackTemp.clear();
}

///>描点法测面积
D2MeasureAreaDot::D2MeasureAreaDot(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureAreaDot::~D2MeasureAreaDot()
{
	Esc();
}

void D2MeasureAreaDot::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dot_tempP = m_tempP;
	m_area = INVALID_VAL;
	m_dot.clear();
}

void D2MeasureAreaDot::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	unsigned int vec_size = 0;
	unsigned int i;

    double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_p1 = m_tempP;
			m_dot_tempP = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);

			m_step = 1;
			m_isDraw = TRUE;
			m_area = 0;

			m_dot.clear();
			m_dot.push_back(m_p1);
			HintArea::GetInstance()->UpdateHint(_("[Area-Point]: <Update> to set shape point."));
			break;

		case 1:
			HintArea::GetInstance()->ClearHint();

			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);

			m_draw.DrawLine(m_p1, m_tempP);
			m_area += m_calc.D2CalcAreaDot(m_dot_tempP, m_tempP, m_p1);
			m_area += m_calc.D2CalcAreaDot(m_tempP, m_p1, m_p1);
			if (m_area < 0)
				m_area = 0 - m_area;
			m_dot_tempP = m_tempP;
			m_dot.push_back(m_dot_tempP);

			vec_size = m_dot.size();
			for(i=0; i!=vec_size-1; i++)
			{
				m_draw.DrawLine(m_dot[i], m_dot[i+1]);
				m_draw.DrawCursor(m_dot[i+1], FALSE);
			}

            //>为了使结果显示的数目和测量线的数目一致，在此用true的方式多显示了一次
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			// save measure result
		 	m_ptrMan->AddNew(AREA_DOT, m_draw.GetCursorType(), m_dot, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
            m_ptrMan->SingleMeaDataMan(m_area, m_itemInfo, allData, SAVE);
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

		//	m_update.D2AreaDot(m_area, attr, false);
			//begin new track length measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;
	}
}
void D2MeasureAreaDot::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			HintArea::GetInstance()->ClearHint();
			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}

				m_tempP = m_p1;

				unsigned int i, vec_size;
				vec_size = m_dot.size();
				for(i=0; i!=vec_size; i++)
				{
					m_draw.DrawCursor(m_dot[i], FALSE);
				}
				m_dot.clear();

				m_draw.DrawCursor(m_tempP);
				m_draw.SetCursor(m_tempP);
				m_isDraw = TRUE;
				m_step = 0;
				m_area = INVALID_VAL;
			}
			m_update.ClearMeasure();
			HintArea::GetInstance()->ClearHint();
			break;

		default:
			break;
	}
}

void D2MeasureAreaDot::MouseMove(POINT p)
{
	if (m_isDraw)
	{
		m_draw.DrawCursor(m_tempP);
	}

	m_tempP = p;
	m_draw.DrawCursor(p);
	m_isDraw = TRUE;
}

void D2MeasureAreaDot::Esc()
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
		int i;
		int vec_size;

		if (m_isDraw)
		{
			vec_size = m_dot.size();
			m_draw.DrawCursor(m_tempP);
			for(i=0; i<vec_size; i++)
				m_draw.DrawCursor(m_dot[i], FALSE);
		}
		m_update.ClearMeasure();
		HintArea::GetInstance()->ClearHint();
	}
}

void D2MeasureAreaDot::Change()
{
	switch(m_step)
	{
		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_dot.push_back(m_tempP);

			m_draw.DrawCursor(m_tempP, FALSE);
			m_dot_tempP = m_tempP;
			m_isDraw = FALSE;//TRUE;=======be careful
			break;

		default:
			break;
	}
}

///> Area-矩阵法
D2MeasureAreaRec::D2MeasureAreaRec(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureAreaRec::~D2MeasureAreaRec()
{
	Esc();
}

void D2MeasureAreaRec::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_area = INVALID_VAL;
}
void D2MeasureAreaRec::PressLeft(POINT p)
{
	vector<POINT> vec;
	POINT tempP;

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
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			tempP.x = m_p1.x;
			tempP.y = m_tempP.y;
			m_draw.DrawLine(m_p1, tempP, TRUE);
			m_draw.DrawLine(tempP, m_tempP, TRUE);
			tempP.x = m_tempP.x;
			tempP.y = m_p1.y;
			m_draw.DrawLine(m_p1, tempP, TRUE);
			m_draw.DrawLine(tempP, m_tempP, TRUE);

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				tempP.x = m_p1.x;
				tempP.y = m_tempP.y;
				m_draw.DrawLine(m_p1, tempP, TRUE);
				m_draw.DrawLine(tempP, m_tempP, TRUE);
				tempP.x = m_tempP.x;
				tempP.y = m_p1.y;
				m_draw.DrawLine(m_p1, tempP, TRUE);
				m_draw.DrawLine(tempP, m_tempP, TRUE);

			}
			m_draw.DrawCursor(m_tempP, FALSE);
			tempP.x = m_p1.x;
			tempP.y = m_tempP.y;
			m_draw.DrawLine(m_p1, tempP, FALSE);
			m_draw.DrawLine(tempP, m_tempP, FALSE);
			tempP.x = m_tempP.x;
			tempP.y = m_p1.y;
			m_draw.DrawLine(m_p1, tempP, FALSE);
			m_draw.DrawLine(tempP, m_tempP, FALSE);

			// save measure result
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			m_ptrMan->AddNew(AREA_REC, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			m_area = m_calc.D2CalcAreaRec(m_p1, m_tempP);

			// 确定测量结果，第二个参数为false时表示测量结束
            m_ptrMan->SingleMeaDataMan(m_area, m_itemInfo, allData, SAVE);
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

            //m_update.D2AreaRec(m_area, attr, false);

			//begin new dist measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureAreaRec::PressRight(POINT p)
{
	POINT tempP;

	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			//清除已经测量的结果
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				tempP.x = m_p1.x;
				tempP.y = m_tempP.y;
				m_draw.DrawLine(m_p1, tempP, TRUE);
				m_draw.DrawLine(tempP, m_tempP, TRUE);
				tempP.x = m_tempP.x;
				tempP.y = m_p1.y;
				m_draw.DrawLine(m_p1, tempP, TRUE);
				m_draw.DrawLine(tempP, m_tempP, TRUE);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_area = INVALID_VAL;
			//清除正在测量的结果
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureAreaRec::MouseMove(POINT p)
{
	POINT tempP;
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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				tempP.x = m_p1.x;
				tempP.y = m_tempP.y;
				m_draw.DrawLine(m_p1, tempP, TRUE);
				m_draw.DrawLine(tempP, m_tempP, TRUE);
				tempP.x = m_tempP.x;
				tempP.y = m_p1.y;
				m_draw.DrawLine(m_p1, tempP, TRUE);
				m_draw.DrawLine(tempP, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);

			tempP.x = m_p1.x;
			tempP.y = p.y;
			m_draw.DrawLine(m_p1, tempP, TRUE);
			m_draw.DrawLine(tempP, p, TRUE);
			tempP.x = p.x;
			tempP.y = m_p1.y;
			m_draw.DrawLine(m_p1, tempP, TRUE);
			m_draw.DrawLine(tempP, p, TRUE);

			m_isDraw = TRUE;

			m_area = m_calc.D2CalcAreaRec(m_p1, p);
			m_update.D2AreaRec(m_area, attr);
			break;
		default:
			break;
	}
}

void D2MeasureAreaRec::Change()
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

void D2MeasureAreaRec::Esc()
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
		POINT p12, p21;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);

			p12.x = m_p1.x;
			p12.y = m_tempP.y;
			p21.x = m_tempP.x;
			p21.y = m_p1.y;
			m_draw.DrawLine(m_p1, p12, TRUE);
			m_draw.DrawLine(m_p1, p21, TRUE);
			m_draw.DrawLine(p12, m_tempP, TRUE);
			m_draw.DrawLine(p21, m_tempP, TRUE);
		}
		m_update.ClearMeasure();
	}
}

///> Vol-三轴法
//D2MeasureVol3Axis::D2MeasureVol3Axis(const char *name, int item, const SingleItemInfo *ptrSingleItemInfo)
D2MeasureVol3Axis::D2MeasureVol3Axis( const char *name,const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

   // m_item = item;

    if (name != NULL)
		strcpy(m_name, name);
	else
		m_name[0] = '\0';

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureVol3Axis::~D2MeasureVol3Axis()
{
	Esc();
}

void D2MeasureVol3Axis::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();

	m_x_len = (double)0;
	m_y_len = (double)0;
	m_z_len = (double)0;
	m_vol = (double)0;
}

void D2MeasureVol3Axis::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

    double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			m_ptrMan->ClearFirst();

			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_x_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_x_end = m_tempP;
			m_step = 1;
			m_isDraw = TRUE;

			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_x_end = m_tempP;
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			//	m_draw.DrawInitialCursor();

			m_step = 2;

			m_x_len = m_calc.D2CalcDist(m_x_start, m_x_end);
			m_update.D2Vol3Axis(m_name, m_x_len, m_y_len, m_z_len, m_vol, attr);

			break;

		case 2: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_y_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_y_end = m_tempP;

			m_step = 3;
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_y_end = m_tempP;
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			//	m_draw.DrawInitialCursor();

			m_step = 4;

			m_y_len = m_calc.D2CalcDist(m_y_start, m_y_end);
			m_update.D2Vol3Axis(m_name, m_x_len, m_y_len, m_z_len, m_vol, attr);

			break;

		case 4: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_z_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_z_end = m_tempP;

			m_step = 5;
			m_isDraw = TRUE;
			break;

		case 5:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
				}
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
				m_z_end = m_tempP;

				m_z_len = m_calc.D2CalcDist(m_z_start, m_z_end);
				m_vol = PI * m_x_len * m_y_len * m_z_len / 6.0;
			//	m_ptrMan->SetMeasureResult(m_vol, (double)0, m_item);//科别测量保存测量结果

				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_x_start);
				vec.push_back(m_x_end);
				vec.push_back(m_y_start);
				vec.push_back(m_y_end);
				vec.push_back(m_z_start);
				vec.push_back(m_z_end);
				m_ptrMan->AddNew(VOL_3AXIS, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

                m_ptrMan->SingleMeaDataMan(m_vol, m_itemInfo, allData, SAVE);
                //m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

                m_update.D2Vol3Axis(m_name, m_x_len, m_y_len, m_z_len, m_vol, attr, false);
				//begin new vol 3 axis measure
				m_draw.ChangeCursorType(); // 更改鼠标类型
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureVol3Axis::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_p1 = m_x_start;
			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_x_len = (double)0;

			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_x_end;
			m_p1 = m_x_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			m_step = 1;
			//	m_ptrMan->ModifyCurrDel(2, m_step);

			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_p1 = m_y_start;
			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 2;
			m_y_len = (double)0;

			m_update.ClearMeasure();
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_y_end;
			m_p1 = m_y_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			m_step = 3;
			//		m_ptrMan->ModifyCurrDel(2, m_step);

			m_update.ClearMeasure();
			break;

		case 5:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_p1 = m_z_start;
			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 4;
			m_z_len = (double)0;
			m_vol = (double)0;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureVol3Axis::MouseMove(POINT p)
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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, TRUE);
			m_isDraw = TRUE;

			m_x_len = m_calc.D2CalcDist(m_p1, p);
			m_update.D2Vol3Axis(m_name, m_x_len, m_y_len, m_z_len, m_vol, attr);
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, TRUE);
			m_isDraw = TRUE;

			m_y_len = m_calc.D2CalcDist(m_p1, p);
			m_update.D2Vol3Axis(m_name, m_x_len, m_y_len, m_z_len, m_vol, attr);
			break;
		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 5:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, TRUE);
			m_isDraw = TRUE;

			m_z_len = m_calc.D2CalcDist(m_p1, p);
			m_vol = PI * m_x_len * m_y_len * m_z_len / 6.0;
			m_update.D2Vol3Axis(m_name, m_x_len, m_y_len, m_z_len, m_vol, attr);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureVol3Axis::Change()
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
		m_x_start = m_p1;
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
		m_y_start = m_p1;
		m_draw.SetCursor(m_tempP);
	}
	if (m_step == 5)
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
		m_z_start = m_p1;
		m_draw.SetCursor(m_tempP);
	}
}

void D2MeasureVol3Axis::Esc()
{
	//	m_draw.DrawCursor(m_tempP);
	//	m_ptrMan->ClearLast();
	if (m_step == 0 )
	{
		m_draw.DrawCursor(m_tempP);
	}
	else if (m_step == 1)
	{
		m_draw.DrawDotLine(m_x_start, m_tempP, TRUE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 2)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 3)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_y_start, m_tempP, TRUE);
		m_draw.DrawCursor(m_y_start, FALSE);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 4)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_y_start, m_y_end, FALSE);
		m_draw.DrawCursor(m_y_start, FALSE);
		m_draw.DrawCursor(m_y_end, FALSE);
		m_draw.DrawOrderNumber(m_y_end, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 5)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_y_start, m_y_end, FALSE);
		m_draw.DrawCursor(m_y_start, FALSE);
		m_draw.DrawCursor(m_y_end, FALSE);
		m_draw.DrawOrderNumber(m_y_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_z_start, m_tempP, TRUE);
		m_draw.DrawCursor(m_z_start, FALSE);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
}

///> Sac-三轴法
D2MeasureSac3Axis::D2MeasureSac3Axis(const MultiItemInfo *ptrMultiItemInfo)
{
    m_item = ptrMultiItemInfo->item;
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureSac3Axis::~D2MeasureSac3Axis()
{
	Esc();
}

void D2MeasureSac3Axis::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();

	m_x_len = (double)0;
	m_y_len = (double)0;
	m_z_len = (double)0;
}

void D2MeasureSac3Axis::PressLeft(POINT p)
{
	vector<POINT> vec;
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();//m_draw.GetCursorType();
	attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();
    int i;
    for(i=0; i<MULTI_MAX; i++)
	{
		 if(i < MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}

    for(i=0; i<MEA_MULTI; i++)
    {
        unit_coeffi[i] = 1;
    }

	switch(m_step)
	{
		case 0: // sac diam1
			m_ptrMan->ClearFirst();

			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_x_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_x_end = m_tempP;
			m_step = 1;
			m_isDraw = TRUE;

			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_x_end = m_tempP;
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
            m_step = 2;
            dataMea[0] = m_x_len = m_calc.D2CalcDist(m_x_start, m_x_end);
            m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		case 2: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_y_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_y_end = m_tempP;

			m_step = 3;
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_y_end = m_tempP;
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
            dataMea[0] = m_x_len;
            dataMea[1] = m_y_len = m_calc.D2CalcDist(m_y_start, m_y_end);
            m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);

            m_step = 4;
            m_isDraw = TRUE;
            break;

		case 4: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_z_start = m_p1;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_z_end = m_tempP;

			m_step = 5;
			m_isDraw = TRUE;
			break;

		case 5:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
				}
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
				m_z_end = m_tempP;
                dataMea[0] = m_x_len;
                dataMea[1] = m_y_len;
                dataMea[2] = m_z_len = m_calc.D2CalcDist(m_z_start, m_z_end);
                if((dataMea[0] != INVALID_VAL) && (dataMea[1] != INVALID_VAL) && (dataMea[2] != INVALID_VAL))
                    dataMea[3] = (dataMea[0] + dataMea[1] + dataMea[2]) / 3.0;
                vec.clear();
                vec.push_back(m_x_start);
                vec.push_back(m_x_end);
                vec.push_back(m_y_start);
                vec.push_back(m_y_end);
                vec.push_back(m_z_start);
                vec.push_back(m_z_end);

                m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);
                m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

                m_ptrMan->AddNew(VOL_3AXIS, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

				//begin new vol 3 axis measure
				m_draw.ChangeCursorType(); // 更改鼠标类型
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureSac3Axis::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}
			m_p1 = m_x_start;
			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_x_len = (double)0;

			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_x_end;
			m_p1 = m_x_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_isDraw = TRUE;
			m_step = 1;
			//	m_ptrMan->ModifyCurrDel(2, m_step);

			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_p1 = m_y_start;
			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 2;
			m_y_len = (double)0;

			m_update.ClearMeasure();
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_y_end;
			m_p1 = m_y_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_p1, m_tempP, FALSE);
			m_draw.DrawOrderNumber(m_tempP, m_draw.GetOrderNumber());
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			m_isDraw = TRUE;
			m_step = 3;
			//		m_ptrMan->ModifyCurrDel(2, m_step);

			m_update.ClearMeasure();
			break;

		case 5:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_p1 = m_z_start;
			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 4;
			m_z_len = (double)0;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureSac3Axis::MouseMove(POINT p)
{
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

    UpdateMeasure::ResultAttr attr;
    attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	int i;
	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, TRUE);
			m_isDraw = TRUE;

			dataMea[0] = m_x_len = m_calc.D2CalcDist(m_p1, p);
            m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, TRUE);
			m_isDraw = TRUE;

			dataMea[0] = m_x_len;
			dataMea[1] = m_y_len = m_calc.D2CalcDist(m_p1, p);
            m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;
		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 5:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, TRUE);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, TRUE);
			m_isDraw = TRUE;

            dataMea[0] = m_x_len;
			dataMea[1] = m_y_len;
			dataMea[2] = m_z_len = m_calc.D2CalcDist(m_p1, p);
            m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
            m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureSac3Axis::Change()
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
		m_x_start = m_p1;
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
		m_y_start = m_p1;
		m_draw.SetCursor(m_tempP);
	}
	if (m_step == 5)
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
		m_z_start = m_p1;
		m_draw.SetCursor(m_tempP);
	}
}

void D2MeasureSac3Axis::Esc()
{
	if (m_step == 0 )
	{
		m_draw.DrawCursor(m_tempP);
	}
	else if (m_step == 1)
	{
		m_draw.DrawDotLine(m_x_start, m_tempP, TRUE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 2)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 3)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_y_start, m_tempP, TRUE);
		m_draw.DrawCursor(m_y_start, FALSE);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 4)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_y_start, m_y_end, FALSE);
		m_draw.DrawCursor(m_y_start, FALSE);
		m_draw.DrawCursor(m_y_end, FALSE);
		m_draw.DrawOrderNumber(m_y_end, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
	else if (m_step == 5)
	{
		m_draw.DrawDotLine(m_x_start, m_x_end, FALSE);
		m_draw.DrawCursor(m_x_start, FALSE);
		m_draw.DrawCursor(m_x_end, FALSE);
		m_draw.DrawOrderNumber(m_x_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_y_start, m_y_end, FALSE);
		m_draw.DrawCursor(m_y_start, FALSE);
		m_draw.DrawCursor(m_y_end, FALSE);
		m_draw.DrawOrderNumber(m_y_end, m_draw.GetOrderNumber());

		m_draw.DrawDotLine(m_z_start, m_tempP, TRUE);
		m_draw.DrawCursor(m_z_start, FALSE);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
}

/**************这是基本测量中正式用的，为软件包测量的修改而保留一份****************/
///> method = 0:椭圆法测面积
///> method = 1:椭圆法1测体积
// D2MeasureVolEllipse1::D2MeasureVolEllipse1(int method)
// {
// 	m_method = method;
// 	m_ptrMan = MeasureMan::GetInstance();
// 	Init();
// }

// D2MeasureVolEllipse1::~D2MeasureVolEllipse1()
// {
// 	Esc();
// }

// void D2MeasureVolEllipse1::Init()
// {
// 	//	vector<POINT> vec;

// 	m_step = 0;
// 	m_isDraw = TRUE;
// 	m_tempP = m_draw.DrawInitialCursor();
// 	m_vol = 0;
// }

// void D2MeasureVolEllipse1::PressLeft(POINT p)
// {
// 	vector<POINT> vec;

// 	switch(m_step)
// 	{
// 		case 0: // first
// 			//	m_ptrMan->ClearFirst();

// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 			}
// 			m_draw.DrawCursor(m_tempP, FALSE);
// 			m_s_start = m_l_end = m_l_start = m_tempP;
// 			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			m_step = 1;
// 			m_isDraw = TRUE;

// 			break;

// 		case 1:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			}
// 			m_l_end = m_tempP;
// 			m_s_start = CycleVertex(m_l_start, m_l_end);
// 			m_tempP = m_s_start;
// 			m_draw.SetCursor(m_s_start);
// 			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			CalcResult();
// 			if(m_method==0)
// 			{
// 				m_update.D2AreaEllipse(m_perimeter, m_area);
// 			}
// 			else
// 			{
// 				m_update.D2VolEllipse1(m_vol);
// 			}
// 			m_step = 2;
// 			break;

// 		case 2:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			}
// 			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, FALSE);
// 			CalcResult();
// 			if(m_method==0)
// 			{
// 				m_update.D2AreaEllipse(m_perimeter, m_area, false);
// 			}
// 			else
// 			{
// 				m_update.D2VolEllipse1(m_vol, false);
// 			}

// 			// save measure result
// 			vec.clear();
// 			vec.push_back(m_l_start);
// 			vec.push_back(m_l_end);
// 			vec.push_back(m_s_start);
// 			m_ptrMan->AddNew(VOL_ELLIPSE1, m_draw.GetCursorType(), vec);

// 			//begin new vol ellipse1 measure
// 			Init();
// 			break;

// 			//default:
// 			//	break;
// 	}
// }

// void D2MeasureVolEllipse1::PressRight(POINT p)
// {
// 	switch(m_step)
// 	{
// 		case 0:// clear last measure result and measure line
// 			m_ptrMan->ClearLast();
// 			m_update.ClearLast();
// 			break;

// 		case 1:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			}

// 			m_tempP = m_l_start;
// 			m_draw.DrawCursor(m_tempP, FALSE);
// 			m_draw.DrawCursor(m_tempP);
// 			m_isDraw = TRUE;
// 			m_draw.SetCursor(m_l_start);
// 			m_step = 0;
// 			m_perimeter = 0;
// 			m_area = 0;
// 			m_vol = 0;
// 			m_update.ClearMeasure();
// 			break;

// 		case 2:
// 			if (m_isDraw)
// 			{
// 			}

// 			m_tempP = m_l_end;
// 			m_draw.SetCursor(m_tempP);
// 			m_isDraw = TRUE;
// 			m_step = 1;
// 			m_update.ClearMeasure();
// 			break;

// 		default:
// 			break;
// 	}
// }

// void D2MeasureVolEllipse1::MouseMove(POINT p)
// {
// 	switch(m_step)
// 	{
// 		case 0:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 			}

// 			m_tempP = p;
// 			m_draw.DrawCursor(p);
// 			m_isDraw = TRUE;
// 			break;

// 		case 1:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			}
// 			m_tempP = p;
// 			m_l_end = p;
// 			m_s_start = CycleVertex(m_l_start, m_l_end);
// 			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			m_isDraw = TRUE;

// 			CalcResult();
// 			if(m_method==0)
// 			{
// 				m_update.D2AreaEllipse(m_perimeter, m_area);
// 			}
// 			else
// 			{
// 				m_update.D2VolEllipse1(m_vol);
// 			}
// 			break;

// 		case 2:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			}

// 			if (abs(p.x-m_tempP.x) >= abs(p.y-m_tempP.y))
// 			{
// 				m_s_start.x += (p.x - m_tempP.x);
// 				if ((m_l_start.y - m_l_end.y) != 0)
// 					m_s_start.y = ((double)m_l_start.y+(double)m_l_end.y) / 2.0 +
// 						((double)m_s_start.x - ((double)m_l_start.x+(double)m_l_end.x) / 2.0) *
// 						(((double)m_l_end.x-(double)m_l_start.x) /
// 						 ((double)m_l_start.y-(double)m_l_end.y));
// 			}
// 			else
// 			{
// 				m_s_start.y += (p.y - m_tempP.y);
// 				if ((m_l_start.x - m_l_end.x) != 0)
// 					m_s_start.x = ((double)m_l_start.x+(double)m_l_end.x) / 2.0 +
// 						((double)m_s_start.y - ((double)m_l_start.y+(double)m_l_end.y) / 2.0) *
// 						(((double)m_l_end.y - (double)m_l_start.y) /
// 						 ((double)m_l_start.x -(double)m_l_end.x));
// 			}

// 			m_tempP = m_s_start;
// 			m_draw.SetCursor(m_tempP);
// 			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 			m_isDraw = TRUE;

// 			CalcResult();
// 			if(m_method==0)
// 			{
// 				m_update.D2AreaEllipse(m_perimeter, m_area);
// 			}
// 			else
// 			{
// 				m_update.D2VolEllipse1(m_vol);
// 			}
// 			break;
// 		default:
// 			break;
// 	}
// }

// /*
//  * @brief Change current moveable point between p1 and p2
//  */
// void D2MeasureVolEllipse1::Change()
// {
// 	if (m_step == 1)
// 	{
// 		if (m_isDraw)
// 		{
// 			m_draw.DrawCursor(m_tempP, FALSE);
// 		}
// 		m_draw.DrawCursor(m_l_start, FALSE);

// 		POINT p1;
// 		p1 = m_l_start;
// 		m_l_start = m_l_end;
// 		m_l_end = p1;
// 		m_tempP = m_l_end;
// 		m_draw.SetCursor(m_tempP);
// 	}
// }

// void D2MeasureVolEllipse1::Esc()
// {
// 	if (m_step==0)
// 		m_draw.DrawCursor(m_tempP);
// 	else
// 	{
// 		m_draw.DrawCursor(m_l_start, FALSE);
// 		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
// 		m_update.ClearMeasure();
// 	}

// }

// void D2MeasureVolEllipse1::CalcResult(void)
// {
// 	double long_axis, short_axis;
// 	double center_x, center_y;
// 	double scale = Img2D::GetInstance()->GetScale2D();

// 	long_axis = scale * sqrt((double)((m_l_end.x-m_l_start.x) * (m_l_end.x-m_l_start.x)) +
// 			(double)((m_l_end.y-m_l_start.y)*(m_l_end.y-m_l_start.y)));

// 	center_x = (double)(m_l_start.x + m_l_end.x) / 2.0;
// 	center_y = (double)(m_l_start.y + m_l_end.y) / 2.0;

// 	short_axis = scale * sqrt((double)((m_s_start.x-center_x) * (m_s_start.x-center_x)) +
// 			(double)((m_s_start.y-center_y)*(m_s_start.y-center_y))) * 2.0;

// 	m_vol = PI * long_axis * short_axis * short_axis / 6.0;
// 	m_area = PI * long_axis * short_axis  / 4.0;
// 	m_perimeter = PI * (1.5 * (long_axis + short_axis) - sqrt(long_axis * short_axis));

// 	return;
// }

///> method = 0:基本测量椭圆法测面积
///> method = 1:基本测量椭圆法1测体积
///> method = 2:基本测量椭圆法1测周长
///> method = 3:软件包测量AC、HC等估算胎龄
///>re_display:本测量项的属性，包括计算项的计算公式
///>item: 软件包测量项的标号，传给UpdateMeasure中把科别测量结果保存起来
D2MeasureVolEllipse1::D2MeasureVolEllipse1(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureVolEllipse1::~D2MeasureVolEllipse1()
{
	Esc();
}

void D2MeasureVolEllipse1::Init()
{
    m_step = 0;
    m_isDraw = TRUE;
    m_tempP = m_draw.DrawInitialCursor();
    m_perimeter = INVALID_VAL;
    m_area = INVALID_VAL;
    m_vol = INVALID_VAL;
}

void D2MeasureVolEllipse1::PressLeft(POINT p)
{
	vector<POINT> vec;
	double allData[SINGLE_MAX];
	int unit_coeffi;
	double resultTmp;
	int x0, y0;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_s_start = m_l_end = m_l_start = m_tempP;
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_step = 1;
			m_isDraw = FALSE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawCursor(m_tempP, FALSE);
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}
			m_l_end = m_tempP;
			m_s_start = CycleVertex(m_l_start, m_l_end);
			m_tempP = m_s_start;
			x0 = (m_l_start.x + m_l_end.x) / 2;
			y0 = (m_l_start.y + m_l_end.y) / 2;
			m_SA_P = 2 * sqrt( (m_tempP.x - x0)*(m_tempP.x - x0) + (m_tempP.y - y0)*(m_tempP.y - y0) );
			m_draw.SetCursor(m_s_start);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			//	CalcResult();
			if (m_itemInfo->meaType == AREA_ELLIPSE)//面积
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
			}
			else if (m_itemInfo->meaType == VOL_ELLIPSE1)//体积
			{
				m_vol = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 3);
				resultTmp = m_vol;
				unit_coeffi = 1;
			}
			else if (m_itemInfo->meaType == PERI_ELLIPSE)//周长
			{
				m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);
				resultTmp = m_perimeter;
				unit_coeffi = 1;
			}
			else
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
			}
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, unit_coeffi);

			m_step = 2;
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, false);
			// save measure result
			vec.clear();
			vec.push_back(m_l_start);
			vec.push_back(m_l_end);
			vec.push_back(m_s_start);

			if (m_itemInfo->meaType == AREA_ELLIPSE)//面积
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
				m_ptrMan->AddNew(AREA_ELLIPSE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			}
			else if (m_itemInfo->meaType == VOL_ELLIPSE1)//体积
			{
				m_vol = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 3);
				//printf("%s-%s: l start=(%d,%d), l end=(%d,%d), s start=(%d,%d)\n", __FILE__, __FUNCTION__, m_l_start.x, m_l_start.y, m_l_end.x, m_l_end.y, m_s_start.x, m_s_start.y);
				resultTmp = m_vol;
				//printf("%s-%s: m_vol = %3.2f\n", __FILE__, __FUNCTION__, m_vol);
				unit_coeffi = 1;
				m_ptrMan->AddNew(VOL_ELLIPSE1, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			}
			else if (m_itemInfo->meaType == PERI_ELLIPSE)//周长
			{
				m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);
				resultTmp = m_perimeter;
				unit_coeffi = 1;
				m_ptrMan->AddNew(PERI_ELLIPSE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			}
			else
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
				m_ptrMan->AddNew(AREA_ELLIPSE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			}
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, unit_coeffi);

			//begin new vol ellipse1 measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;
	}
}

void D2MeasureVolEllipse1::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			m_tempP = m_l_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 0;
			m_perimeter = INVALID_VAL;
			m_area = INVALID_VAL;
			m_vol = INVALID_VAL;
			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
			}

			m_tempP = m_l_end;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 1;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureVolEllipse1::MouseMove(POINT p)
{
	double allData[SINGLE_MAX];
	int unit_coeffi;
	double resultTmp;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	int m = 2;
	int offsetX, offsetY, x0, y0;
	double k, temp;

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}
			m_tempP = p;
			m_l_end = p;
			m_s_start = CycleVertex(m_l_start, m_l_end);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_isDraw = TRUE;

			if (m_itemInfo->meaType == AREA_ELLIPSE)//面积
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
			}
			else if (m_itemInfo->meaType == VOL_ELLIPSE1)//体积
			{
				m_vol = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 3);
				resultTmp = m_vol;
				unit_coeffi = 1;
			}
			else if (m_itemInfo->meaType == PERI_ELLIPSE)//周长
			{
				m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);
				resultTmp = m_perimeter;
				unit_coeffi = 1;
			}
			else
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
			}
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			offsetX = p.x - m_tempP.x;
			offsetY = p.y - m_tempP.y;
			x0 = (m_l_start.x + m_l_end.x) / 2;
			y0 = (m_l_start.y + m_l_end.y) / 2;

			if (abs(offsetX) > abs(offsetY) && offsetX < 0)
				m_SA_P += m;
			else if ((abs(offsetX) > abs(offsetY)) && (offsetX > 0))
				m_SA_P -= m;
			else if ((abs(offsetX) <= abs(offsetY)) && (offsetY > 0))
				m_SA_P += m;
			else if ((abs(offsetX) <= abs(offsetY)) && (offsetY < 0))
				m_SA_P -= m;

			if(m_l_start.x == m_l_end.x)
			{
				m_s_start.x = x0 + m_SA_P / 2;
				m_s_start.y = y0;
			}
			else
			{
				k = (m_l_end.y - m_l_start.y) / (double)(m_l_end.x - m_l_start.x);
				temp = 2 * sqrt(k * k + 1);

				m_s_start.x = x0 + k * m_SA_P / temp;
				m_s_start.y = y0 - m_SA_P / temp;
			}

			m_tempP = m_s_start;
			m_draw.SetCursor(m_tempP);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_isDraw = TRUE;

			if (m_itemInfo->meaType == AREA_ELLIPSE)//面积
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
			}
			else if (m_itemInfo->meaType == VOL_ELLIPSE1)//体积
			{
				//printf("%s-%s: l start=(%d,%d), l end=(%d,%d), s start=(%d,%d)\n", __FILE__, __FUNCTION__, m_l_start.x, m_l_start.y, m_l_end.x, m_l_end.y, m_s_start.x, m_s_start.y);
				m_vol = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 3);
				//printf("%s-%s: m_vol = %3.2f\n", __FILE__, __FUNCTION__, m_vol);
				resultTmp = m_vol;
				unit_coeffi = 1;
			}
			else if (m_itemInfo->meaType == PERI_ELLIPSE)//周长
			{
				m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);
				resultTmp = m_perimeter;
				unit_coeffi = 1;
			}
			else
			{
				m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);
				resultTmp = m_area;
				unit_coeffi = 1;
			}
			m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, unit_coeffi);

			break;
		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureVolEllipse1::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_l_start, FALSE);
		}

		POINT p1;
		p1 = m_l_start;
		m_l_start = m_l_end;
		m_l_end = p1;
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end);
		m_tempP = m_l_end;
		m_draw.SetCursor(m_tempP);
	}
	else if(m_step == 2)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_l_start, FALSE);
			m_draw.DrawCursor(m_l_end, FALSE);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
		}

		POINT p1, p2, p3, p4;
		p1 = m_l_start;
		p2 = m_l_end;
		p3 = m_s_start;
		p4.x = m_l_start.x + m_l_end.x - m_s_start.x;
		p4.y = m_l_start.y + m_l_end.y - m_s_start.y;
		m_l_start = p3;
		m_l_end = p4;
		m_s_start = p1;
		m_tempP = m_s_start;

		int x0 = (m_l_start.x + m_l_end.x) / 2;
		int y0 = (m_l_start.y + m_l_end.y) / 2;
		m_SA_P = 2 * sqrt( (m_tempP.x - x0)*(m_tempP.x - x0) + (m_tempP.y - y0)*(m_tempP.y - y0) );
		//printf("%s-%s: l start=(%d,%d), l end=(%d,%d), s start=(%d,%d)\n", __FILE__, __FUNCTION__, m_l_start.x, m_l_start.y, m_l_end.x, m_l_end.y, m_s_start.x, m_s_start.y);

		m_draw.SetCursor(m_tempP);
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end, FALSE);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
	}
}

void D2MeasureVolEllipse1::Esc()
{
	if (m_step==0)
	{
		m_draw.DrawCursor(m_tempP);
	}
	else if(m_step==1)
	{
		m_draw.DrawCursor(m_tempP);
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
		m_update.ClearMeasure();
	}
	else if(m_step==2)
	{
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end, FALSE);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
		m_update.ClearMeasure();
	}
}

/*
   void D2MeasureVolEllipse1::CalcResult(void)
   {
   double long_axis, short_axis;
   double center_x, center_y;
   double scale = Img2D::GetInstance()->GetScale2D();

   long_axis = scale * sqrt((double)((m_l_end.x-m_l_start.x) * (m_l_end.x-m_l_start.x)) +
   (double)((m_l_end.y-m_l_start.y)*(m_l_end.y-m_l_start.y)));

   center_x = (double)(m_l_start.x + m_l_end.x) / 2.0;
   center_y = (double)(m_l_start.y + m_l_end.y) / 2.0;

   short_axis = scale * sqrt((double)((m_s_start.x-center_x) * (m_s_start.x-center_x)) +
   (double)((m_s_start.y-center_y)*(m_s_start.y-center_y))) * 2.0;

   m_vol = PI * long_axis * short_axis * short_axis / 6.0;
   m_area = PI * long_axis * short_axis  / 4.0;
   m_perimeter = PI * (1.5 * (long_axis + short_axis) - sqrt(long_axis * short_axis));

   return;
   }
 */

///> Vol-椭圆法2
D2MeasureVolEllipse2::D2MeasureVolEllipse2(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureVolEllipse2::~D2MeasureVolEllipse2()
{
	Esc();
}

void D2MeasureVolEllipse2::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_vol = INVALID_VAL;
}

void D2MeasureVolEllipse2::PressLeft(POINT p)
{
	vector<POINT> vec;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();
	int x0, y0;

    double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			//	m_ptrMan->ClearFirst();

			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_s_start = m_l_end = m_l_start = m_tempP;
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_step = 1;
			m_isDraw = FALSE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}
			m_l_end = m_tempP;
			m_s_start = CycleVertex(m_l_start, m_l_end);
			m_tempP = m_s_start;
			x0 = (m_l_start.x + m_l_end.x) / 2;
			y0 = (m_l_start.y + m_l_end.y) / 2;
			m_SA_P = 2 * sqrt( (m_tempP.x - x0)*(m_tempP.x - x0) + (m_tempP.y - y0)*(m_tempP.y - y0) );
			m_draw.SetCursor(m_s_start);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_draw.DrawCursor(m_l_end, FALSE);
			m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);	//计算面积
			m_update.D2VolEllipse2(m_area, 0, 0, attr);

			m_step = 2;
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, false);
			m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);	//计算面积
			m_update.D2VolEllipse2(m_area, 0, 0, attr);
			m_draw.DrawOrderNumber(m_s_start, m_draw.GetOrderNumber());
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 3;
			break;

		case 3: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_p1, m_tempP, true);

			m_step = 4;
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
            m_draw.DrawDotLine(m_p1, m_tempP, false);

            m_dist = m_calc.D2CalcDist(m_p1, m_tempP);
            m_vol = m_calc.CalcVol(m_l_start, m_l_end, m_s_start, m_dist);

            // save measure result
			vec.clear();
			vec.push_back(m_l_start);
			vec.push_back(m_l_end);
			vec.push_back(m_s_start);
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			m_ptrMan->AddNew(VOL_ELLIPSE2, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
		//	m_update.D2VolEllipse2(m_area, m_dist, m_vol, attr, false);

            m_ptrMan->SingleMeaDataMan(m_vol, m_itemInfo, allData, SAVE);
            m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			//begin new dist measure
			m_draw.ChangeCursorType(); // 更改鼠标类型
			Init();

            // change pointer
            ChangePointerWhenMeasure();

			break;
		default:
			break;
	}
}

void D2MeasureVolEllipse2::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			m_tempP = m_l_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 0;
			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
			}
			m_tempP = m_l_end;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 1;
			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, false);
			    m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_draw.DrawOrderNumber(m_s_start, m_draw.GetOrderNumber());
			m_tempP = m_s_start;
			m_draw.SetCursor(m_tempP);
			//	m_draw.DrawCursor(m_tempP, FALSE);
			//	m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_vol = INVALID_VAL;
			m_step = 2;
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 3;
			break;

		default:
			break;
	}
}

void D2MeasureVolEllipse2::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	int m = 2;
	int offsetX, offsetY, x0, y0;
	double k, temp;

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			m_tempP = p;
			m_l_end = p;
			m_s_start = CycleVertex(m_l_start, m_l_end);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_isDraw = TRUE;

			m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);	//计算面积
			m_update.D2VolEllipse2(m_area, 0, 0, attr);
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			offsetX = p.x - m_tempP.x;
			offsetY = p.y - m_tempP.y;
			x0 = (m_l_start.x + m_l_end.x) / 2;
			y0 = (m_l_start.y + m_l_end.y) / 2;

			if (abs(offsetX) > abs(offsetY) && offsetX < 0)
				m_SA_P += m;
			else if ((abs(offsetX) > abs(offsetY)) && (offsetX > 0))
				m_SA_P -= m;
			else if ((abs(offsetX) <= abs(offsetY)) && (offsetY > 0))
				m_SA_P += m;
			else if ((abs(offsetX) <= abs(offsetY)) && (offsetY < 0))
				m_SA_P -= m;

			if(m_l_start.x == m_l_end.x)
			{
				m_s_start.x = x0 + m_SA_P / 2;
				m_s_start.y = y0;
			}
			else
			{
				k = (m_l_end.y - m_l_start.y) / (double)(m_l_end.x - m_l_start.x);
				temp = 2 * sqrt(k * k + 1);

				m_s_start.x = x0 + k * m_SA_P / temp;
				m_s_start.y = y0 - m_SA_P / temp;
			}

			m_tempP = m_s_start;
			m_draw.SetCursor(m_tempP);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_isDraw = TRUE;

			m_area = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 1);	//计算面积
			m_update.D2VolEllipse2(m_area, 0, 0, attr);
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_p1 = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, true);
            m_isDraw = TRUE;

            m_dist = m_calc.D2CalcDist(m_p1, m_tempP);
            m_vol = m_calc.CalcVol(m_l_start, m_l_end, m_s_start, m_dist);
            m_update.D2VolEllipse2(m_area, m_dist, m_vol, attr);

        default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureVolEllipse2::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_l_start, FALSE);
		}

		POINT p1;
		p1 = m_l_start;
		m_l_start = m_l_end;
		m_l_end = p1;
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end);
		m_tempP = m_l_end;
		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 4)
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

void D2MeasureVolEllipse2::Esc()
{
	if (m_step==0)
		m_draw.DrawCursor(m_tempP);
	else if (m_step == 1)
	{
		m_draw.DrawCursor(m_l_start, FALSE);
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_l_end);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
		}
	}
	else if(m_step == 2)
	{
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end, FALSE);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
		m_update.ClearMeasure();
	}
	else if(m_step == 3)
	{
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end, FALSE);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
		m_draw.DrawOrderNumber(m_s_start, m_draw.GetOrderNumber());
		m_draw.DrawCursor(m_tempP, true);
		m_update.ClearMeasure();
	}
	else if(m_step == 4)
	{
		m_draw.DrawCursor(m_l_start, FALSE);
		m_draw.DrawCursor(m_l_end, FALSE);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
		m_draw.DrawOrderNumber(m_s_start, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_p1, FALSE);
		m_draw.DrawDotLine(m_p1, m_tempP, true);
		m_draw.DrawCursor(m_tempP);
		m_update.ClearMeasure();
	}
}

///> 体积测量-圆球法
D2MeasureVolSpheroid::D2MeasureVolSpheroid(const MultiItemInfo *ptrMultiItemInfo)
{
//	m_item = ptrMultiItemInfo->item;
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureVolSpheroid::~D2MeasureVolSpheroid()
{
	Esc();
}

void D2MeasureVolSpheroid::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_diam = INVALID_VAL;
	m_vol = INVALID_VAL;
}

void D2MeasureVolSpheroid::PressLeft(POINT p)
{
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	for(i=2; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);
			//m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawDotLine(m_p1, m_tempP, true);
			m_diam = INVALID_VAL;
			m_vol = INVALID_VAL;
			m_step = 1;
			m_isDraw = TRUE;

			break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawDotLine(m_p1, m_tempP, true);
				}
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawDotLine(m_p1, m_tempP, false);

				m_diam = m_calc.D2CalcDist(m_p1, m_tempP);
				m_vol = PI * m_diam * m_diam * m_diam / 6.0;
				dataMea[0] = m_diam;
				dataMea[1] = m_vol;
				m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);

				// save measure result
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_p1);
				vec.push_back(m_tempP);
				m_ptrMan->AddNew(VOL_SPHEROID, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				m_step = 0;
				m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

				//begin new dist measure
				m_draw.ChangeCursorType();
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureVolSpheroid::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_diam = 0;
			m_vol = 0;
			m_step = 0;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureVolSpheroid::MouseMove(POINT p)
{
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	for(i=0; i<MULTI_MAX; i++)
	{
		if (i<MEA_MULTI) dataMea[i] = INVALID_VAL;
		allData[i] = INVALID_VAL;
	}
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	for(i=2; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_p1, m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawDotLine(m_p1, p, true);
			m_isDraw = TRUE;

			m_diam = m_calc.D2CalcDist(m_p1, m_tempP);
			m_vol = PI * m_diam * m_diam * m_diam / 6.0;
			dataMea[0] = m_diam;
			dataMea[1] = m_vol;
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureVolSpheroid::Change()
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

void D2MeasureVolSpheroid::Esc()
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
}

///> Angle-三点法，用于科别测量中
D2MeasureAngleSingle::D2MeasureAngleSingle(const SingleItemInfo *ptrSingleItemInfo)
{
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureAngleSingle::~D2MeasureAngleSingle()
{
	Esc();
}

void D2MeasureAngleSingle::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();

	m_angle = INVALID_VAL;
}

void D2MeasureAngleSingle::PressLeft(POINT p)
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
				m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_vertex = m_tempP;
			m_left = m_right = m_vertex;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex, m_tempP, true);

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, true);
			}

			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex, m_tempP, false);

			m_left = m_tempP;
			m_tempP = m_vertex;
			m_step = 2;
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex, m_tempP, true);

			m_isDraw = TRUE;
			break;

		case 2:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawLine(m_vertex, m_tempP, true);
				}

				m_draw.DrawCursor(m_tempP, false);
				m_draw.DrawLine(m_vertex, m_tempP, false);

				m_right = m_tempP;

				m_angle = m_calc.D2CalcAngle(m_vertex, m_left, m_right);
				m_ptrMan->SingleMeaDataMan(m_angle, m_itemInfo, allData, SAVE);

				// save measure result
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_left);
				vec.push_back(m_vertex);
				vec.push_back(m_right);
				m_ptrMan->AddNew(ANGLE_3DOT, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
#if 0
                m_angle = m_calc.D2CalcAngle(m_vertex, m_left, p);
                m_ptrMan->SingleMeaDataMan(m_angle, m_itemInfo, allData, NOT_SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
#endif

				//begin new dist measure
				m_draw.ChangeCursorType();
				Init();

				// change pointer
				ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureAngleSingle::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, true);
			}

			m_tempP = m_vertex;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;

			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, true);

			}

			m_tempP = m_left;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex, m_tempP, false);
			m_draw.DrawLine(m_vertex, m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 1;
			m_angle = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureAngleSingle::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();
	double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, true);
			}

			m_tempP = p;
			m_left = p;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex, m_tempP, true);

			m_isDraw = TRUE;
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, true);
			}

			m_tempP = p;
			m_right = p;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex, m_tempP, true);
			m_isDraw = TRUE;

			m_angle = m_calc.D2CalcAngle(m_vertex, m_left, p);
			m_ptrMan->SingleMeaDataMan(m_angle, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureAngleSingle::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_vertex, FALSE);
		m_draw.DrawCursor(m_vertex);

		POINT p1;
		p1 = m_vertex;
		m_vertex = m_tempP;
		m_tempP = p1;
		m_left = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 2)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_vertex, m_tempP, true);
			m_draw.DrawLine(m_vertex, m_tempP, false);
		}

		POINT p1;
		p1 = m_left;
		m_left = m_tempP;
		m_tempP = p1;
		m_right = m_tempP;
		m_draw.DrawCursor(m_right, FALSE);
		m_draw.DrawCursor(m_right);
		m_draw.DrawLine(m_vertex, m_right, false);
		m_draw.DrawLine(m_vertex, m_right, true);

		m_draw.SetCursor(m_right);
	}
}

void D2MeasureAngleSingle::Esc()
{
	if(m_step == 0)
		m_draw.DrawCursor(m_tempP);
	else if(m_step == 1 )
	{
		m_draw.DrawCursor(m_vertex, false);
		m_draw.DrawCursor(m_tempP);
		m_draw.DrawLine(m_vertex, m_tempP, true);
	}
	else if(m_step == 2 )
	{
		m_draw.DrawCursor(m_vertex, false);
		m_draw.DrawCursor(m_left, false);
		m_draw.DrawLine(m_vertex, m_left, false);

		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex, m_tempP, true);
	}
	m_update.ClearMeasure();
}

///> Angle-两线法
D2MeasureAngle::D2MeasureAngle(int method,const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_method = method;
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureAngle::~D2MeasureAngle()
{
	Esc();
}

void D2MeasureAngle::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();

	m_dist_l = INVALID_VAL;
	m_dist_r = INVALID_VAL;
	m_angle = INVALID_VAL;
}

void D2MeasureAngle::PressLeft(POINT p)
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
			    m_draw.DrawCursor(m_tempP, true);
			}
			if (m_method==0)
			    m_draw.DrawCursor(m_tempP, false);
			else
			    m_draw.DrawDotBeeline(m_tempP, m_tempP, true);
			m_vertex = m_tempP;
			m_left = m_right = m_vertex;
			m_left.x = m_vertex.x + 1;
			m_tempP = m_left;
			m_dist_l = m_dist_r = 0;
			if (m_method==0)
			{
			    m_draw.DrawCursor(m_tempP);
			    m_draw.DrawLine(m_vertex, m_tempP, true);
			}

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    if (m_method==0) {
				m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, true);
			    }
			    else
                {
                    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
                }
			}

			if (m_method==0) {
			    m_draw.DrawCursor(m_tempP, false);
			    m_draw.DrawLine(m_vertex, m_tempP, false);
			}
			else {
			    m_draw.DrawDotBeeline(m_vertex, m_tempP, false);
			}

			m_left = m_tempP;
			m_tempP = m_vertex;
            m_right = m_vertex;
			m_right.x = m_vertex.x + 1;
			m_step = 2;
			m_draw.SetCursor(m_tempP);
			if (m_method==0)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex, m_tempP, true);
			}
			else
			{
			    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
				m_draw.DrawAngleArc(m_vertex, m_left, m_right, true);
			}

			m_isDraw = TRUE;

			m_dist_l = m_calc.D2CalcDist(m_vertex, m_left);
			if (m_method==0)
			    m_update.D2Angle3Dot(m_dist_l, 0, 0, attr);
			break;

		case 2:
			{
				if (m_isDraw)
				{
				    if (m_method==0) {
					m_draw.DrawCursor(m_tempP);
					m_draw.DrawLine(m_vertex, m_tempP, true);
				    }
				    else {
					m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
					m_draw.DrawAngleArc(m_vertex, m_left, m_right, true);
				    }
				}

				if (m_method==0) {
				    m_draw.DrawCursor(m_tempP, false);
				    m_draw.DrawLine(m_vertex, m_tempP, false);
				}
				else {
				    m_draw.DrawDotBeeline(m_vertex, m_tempP, false);
					m_draw.DrawAngleArc(m_vertex, m_left, m_right, false);
				}

				m_right = m_tempP;

				m_dist_r = m_calc.D2CalcDist(m_vertex, m_right);
				m_angle = m_calc.D2CalcAngle(m_vertex, m_left, m_right);

				// save measure result
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_left);
				vec.push_back(m_vertex);
				vec.push_back(m_right);
				if (m_method==0)
				    m_ptrMan->AddNew(ANGLE_3DOT, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				else
				    m_ptrMan->AddNew(ANGLE_2LINE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

				if (m_method==0)
                {
                    //  m_update.D2Angle3Dot(m_dist_l, m_dist_r, m_angle, attr, false);
                    m_ptrMan->SingleMeaDataMan(m_angle, m_itemInfo, allData, SAVE);
                    //m_ptrMan->SingleMeaDataMan(m_dist_l, m_itemInfo, allData, SAVE);
                    m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
                }
				else
                {
                    // m_update.D2Angle2Line(m_angle, attr, false);
                    m_ptrMan->SingleMeaDataMan(m_angle, m_itemInfo, allData, SAVE);
                    m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
                }

				//begin new dist measure
				m_draw.ChangeCursorType();
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureAngle::PressRight(POINT p)
{
    switch(m_step)
    {
        case 0:// clear last measure result and measure line
            m_ptrMan->ClearLast();
            m_update.ClearLast();
            m_draw.SetCursor(m_tempP);
            break;

        case 1:
            if (m_isDraw)
            {
                if (m_method==0)
                {
                    m_draw.DrawCursor(m_tempP, true);
                    m_draw.DrawLine(m_vertex, m_tempP, true);
                }
                else
                {
                    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
                }
            }

            m_tempP = m_vertex;
            if (m_method == 0) {
                m_draw.DrawCursor(m_tempP, FALSE);
            }
            m_draw.DrawCursor(m_tempP);
            m_draw.SetCursor(m_tempP);
            m_isDraw = TRUE;
            m_step = 0;
            m_dist_l = INVALID_VAL;

            m_update.ClearMeasure();
            break;

        case 2:
            if (m_isDraw)
            {
                if (m_method==0)
                {
                    m_draw.DrawCursor(m_tempP, true);
                    m_draw.DrawLine(m_vertex, m_tempP, true);
                }
                else
                {
                    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
					m_draw.DrawAngleArc(m_vertex, m_left, m_right, true);
                }
            }

            m_tempP = m_left;
            if (m_method==0)
            {
                m_draw.DrawCursor(m_tempP, FALSE);
                m_draw.DrawCursor(m_tempP);
                m_draw.DrawLine(m_vertex, m_tempP, false);
                m_draw.DrawLine(m_vertex, m_tempP, true);
                m_draw.SetCursor(m_tempP);
            }
            else
            {
                m_draw.DrawDotBeeline(m_vertex, m_tempP, false);
                m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
            }
            m_isDraw = TRUE;
            m_step = 1;
            m_dist_r = INVALID_VAL;
            m_angle = INVALID_VAL;

            m_update.ClearMeasure();
            break;

        default:
            break;
    }
}

void D2MeasureAngle::MouseMove(POINT p)
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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				if (m_method==0)
				{
				    m_draw.DrawCursor(m_tempP, true);
				    m_draw.DrawLine(m_vertex, m_tempP, true);
				}
				else
				    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
			}

			m_tempP = p;
			m_left = p;
			if (m_method==0)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex, m_tempP, true);
			}
			else
			    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);

			m_isDraw = TRUE;

			m_dist_l = m_calc.D2CalcDist(m_vertex, p);
			if (m_method==0)
			    m_update.D2Angle3Dot(m_dist_l, 0, 0, attr);
			break;

		case 2:
			if (m_isDraw)
			{
            if (m_method==0)
				{
				    m_draw.DrawCursor(m_tempP, true);
				    m_draw.DrawLine(m_vertex, m_tempP, true);
				}
				else
                {
                    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
					m_draw.DrawAngleArc(m_vertex, m_left, m_right, true);
                }
			}

			m_tempP = p;
			m_right = p;
			if (m_method==0)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex, m_tempP, true);
			}
			else
            {
			    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
				m_draw.DrawAngleArc(m_vertex, m_left, m_right, true);
            }
			m_isDraw = TRUE;
			m_dist_r = m_calc.D2CalcDist(m_vertex, p);
			m_angle = m_calc.D2CalcAngle(m_vertex, m_left, p);
			if (m_method==0)
			    m_update.D2Angle3Dot(m_dist_l, m_dist_r, m_angle, attr);
			else
			    m_update.D2Angle2Line(m_angle, attr);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureAngle::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			if (m_method==0)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawCursor(m_tempP, FALSE);
			}
		}
		if (m_method==0)
		{
			m_draw.DrawCursor(m_vertex, FALSE);
			m_draw.DrawCursor(m_vertex);

			POINT p1;
			p1 = m_vertex;
			m_vertex = m_tempP;
			m_tempP = p1;
			m_left = m_tempP;

			m_draw.SetCursor(m_tempP);
		}
	}
	else if (m_step == 2)
	{
		if (m_isDraw)
		{
			if (m_method==0)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawCursor(m_tempP, FALSE);
				m_draw.DrawLine(m_vertex, m_tempP, true);
				m_draw.DrawLine(m_vertex, m_tempP, false);
			} else {
			    m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
			    m_draw.DrawDotBeeline(m_vertex, m_tempP, false);
			}
		}

		POINT p1;
		p1 = m_left;
		m_left = m_tempP;
		m_tempP = p1;
		m_right = m_tempP;
		if (m_method==0)
		{
			m_draw.DrawCursor(m_right, FALSE);
			m_draw.DrawCursor(m_right);
			m_draw.DrawLine(m_vertex, m_right, false);
			m_draw.DrawLine(m_vertex, m_right, true);
		} else {
		    m_draw.DrawDotBeeline(m_vertex, m_right, false);
		    m_draw.DrawDotBeeline(m_vertex, m_right, true);
		}

		m_draw.SetCursor(m_right);
	}
}

void D2MeasureAngle::Esc()
{
	if(m_step == 0)
		m_draw.DrawCursor(m_tempP);
	else if(m_step == 1 )
	{
		if (m_method == 0)
		{
		    m_draw.DrawCursor(m_vertex, false);
		    m_draw.DrawCursor(m_tempP);
		    m_draw.DrawLine(m_vertex, m_tempP, true);
		}
		else
		{
            m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
		}
    }
    else if(m_step == 2 )
    {
        if (m_method == 0)
        {
            m_draw.DrawCursor(m_vertex, false);
            m_draw.DrawCursor(m_left, false);
            m_draw.DrawLine(m_vertex, m_left, false);

            m_draw.DrawCursor(m_tempP, true);
            m_draw.DrawLine(m_vertex, m_tempP, true);
        }
        else
        {
            m_draw.DrawDotBeeline(m_vertex, m_left, false);
            m_draw.DrawDotBeeline(m_vertex, m_tempP, true);
			m_draw.DrawAngleArc(m_vertex, m_left, m_right, true);
        }
    }
    m_update.ClearMeasure();
}

#if 1
//************髋关节测量*************
D2MeasureHip::D2MeasureHip(const MultiItemInfo *ptrMultiItemInfo)
{
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureHip::~D2MeasureHip()
{
	Esc();
}

void D2MeasureHip::Init()
{
	//m_tempP = m_draw.DrawInitialCursor();
	m_tempP.x = IMAGE_W/2;
	m_tempP.y = IMAGE_Y + IMAGE_H/2;
	//m_draw.DrawCursor(m_tempP, true, XOR);
	m_draw.SetCursor(m_tempP);

	m_step = 0;
	m_len = 200;
	m_alpha = INVALID_VAL;
	m_beta = INVALID_VAL;
	m_blRecord.angle = 0.0;
	m_rlRecord.angle = 55.0;
	m_ilRecord.angle = 120.0;
	m_theta = 0.0;

	m_isDraw = TRUE;
	m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
}

void D2MeasureHip::PressLeft(POINT p)
{
	double dataMea[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	vector<POINT> vec;
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	for (i=0; i<MEA_MULTI; i++)
		dataMea[i] = INVALID_VAL;

	if (m_isDraw)
		m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
	vec = m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, false);
	//printf("%s-%s: Confirm line (%d,%d) to (%d,%d)\n", __FILE__, __FUNCTION__, vec[0].x, vec[0].y, vec[1].x, vec[1].y);

	switch(m_step)
	{
		case 0:
			m_bl_p1 = vec[0];
			m_bl_p2 = vec[1];
			m_blRecord.point = m_tempP;

			m_theta = m_rlRecord.angle * PI / 180;
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 1;

			m_alpha = m_calc.D2CalcHipAlpha(m_blRecord.angle, m_rlRecord.angle);
			m_update.D2Hip(m_alpha, m_beta, attr);
			m_draw.DrawOrderNumber(m_bl_p2, m_draw.GetOrderNumber());
			break;

		case 1:
			m_rl_p1 = vec[0];
			m_rl_p2 = vec[1];
			m_rlRecord.point = m_tempP;

			m_theta = m_ilRecord.angle * PI / 180;
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 2;

			m_beta = m_calc.D2CalcHipBeta(m_blRecord.angle, m_ilRecord.angle);
			m_update.D2Hip(m_alpha, m_beta, attr);
			m_draw.DrawOrderNumber(m_rl_p2, m_draw.GetOrderNumber());
			break;

		case 2:
			{
				m_il_p1 = vec[0];
				m_il_p2 = vec[1];
				m_ilRecord.point = m_tempP;
				m_update.D2Hip(m_alpha, m_beta, attr, false);

				// save measure result
				dataMea[0] = m_alpha;
				dataMea[1] = m_beta;
				m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);

				vec.push_back(m_bl_p1);
				vec.push_back(m_bl_p2);
				vec.push_back(m_rl_p1);
				vec.push_back(m_rl_p2);
				vec.push_back(m_il_p1);
				vec.push_back(m_il_p2);
				m_ptrMan->AddNew(HIP, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

                //begin new dist measure
				m_draw.ChangeCursorType();
                Init();

                // change pointer
                ChangePointerWhenMeasure();

                break;
			}

		default:
			break;
	}
}

void D2MeasureHip::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw)
				m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);

			m_theta = m_blRecord.angle * PI / 180;
			m_tempP = m_blRecord.point;
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, false);
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
			m_draw.SetCursor(m_tempP);
			m_step = 0;
			m_update.ClearMeasure();
			m_draw.DrawOrderNumber(m_bl_p2, m_draw.GetOrderNumber());
			break;

		case 2:
			if (m_isDraw)
				m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);

			m_beta = INVALID_VAL;
			m_theta = m_rlRecord.angle * PI / 180;
			m_tempP = m_rlRecord.point;
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, false);
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
			m_draw.SetCursor(m_tempP);
			m_step = 1;
			m_update.ClearMeasure();
			m_draw.DrawOrderNumber(m_rl_p2, m_draw.GetOrderNumber());
			break;

		default:
			break;
	}
}

void D2MeasureHip::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	//printf("%s-%s: (%d,%d)\n", __FILE__, __FUNCTION__, p.x, p.y);
	if(p.x > IMAGE_W-1)
		p.x = IMAGE_W-1;
	if(p.y > IMAGE_H-1)
		p.y = IMAGE_H-1;

	if (m_isDraw)
		m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);

	m_tempP = p;
	m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
	m_isDraw = TRUE;

	if(m_step == 1)
	{
		while(m_rlRecord.angle >= 360)
			m_rlRecord.angle -= 360;
		while(m_rlRecord.angle < 0)
			m_rlRecord.angle += 360;
		m_theta = m_rlRecord.angle * PI / 180.0;

		m_alpha = m_calc.D2CalcHipAlpha(m_blRecord.angle, m_rlRecord.angle);
		m_update.D2Hip(m_alpha, m_beta, attr);
	}
	else if(m_step == 2)
	{
		while(m_ilRecord.angle >= 360)
			m_ilRecord.angle -= 360;
		while(m_ilRecord.angle < 0)
			m_ilRecord.angle += 360;
		m_theta = m_ilRecord.angle * PI / 180.0;

		m_beta = m_calc.D2CalcHipBeta(m_blRecord.angle, m_ilRecord.angle);
		m_update.D2Hip(m_alpha, m_beta, attr);
	}
}

void D2MeasureHip::Value(EKnobOper opr)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	int dir;
	if(opr==ADD)
		dir = -1;
	else if(opr==SUB)
		dir = 1;

	if (m_isDraw)
	{
		m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
	}
	if (m_step == 0)
	{
		m_blRecord.angle += dir;
		while(m_blRecord.angle >= 360)
			m_blRecord.angle -= 360;
		while(m_blRecord.angle < 0)
			m_blRecord.angle += 360;
		m_theta = m_blRecord.angle * PI / 180.0;
		//printf("%s-%s: angle=%.2f, theta=%.2f\n", __FILE__, __FUNCTION__, m_blRecord.angle, m_theta);
	}
	else if(m_step == 1)
	{
		m_rlRecord.angle += dir;
		while(m_rlRecord.angle >= 360)
			m_rlRecord.angle -= 360;
		while(m_rlRecord.angle < 0)
			m_rlRecord.angle += 360;
		m_theta = m_rlRecord.angle * PI / 180.0;
		//printf("%s-%s: angle=%.2f, theta=%.2f\n", __FILE__, __FUNCTION__, m_rlRecord.angle, m_theta);

		m_alpha = m_calc.D2CalcHipAlpha(m_blRecord.angle, m_rlRecord.angle);
		m_update.D2Hip(m_alpha, m_beta, attr);
	}
	else
	{
		m_ilRecord.angle += dir;
		while(m_ilRecord.angle >= 360)
			m_ilRecord.angle -= 360;
		while(m_ilRecord.angle < 0)
			m_ilRecord.angle += 360;
		m_theta = m_ilRecord.angle * PI / 180.0;
		//printf("%s-%s: angle=%.2f, theta=%.2f\n", __FILE__, __FUNCTION__, m_ilRecord.angle, m_theta);

		m_beta = m_calc.D2CalcHipBeta(m_blRecord.angle, m_ilRecord.angle);
		m_update.D2Hip(m_alpha, m_beta, attr);
	}

	m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
	m_isDraw = TRUE;
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureHip::Change()
{

}

void D2MeasureHip::Esc()
{
	if(m_step == 0)
	{
		if(m_isDraw)
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);
	}
	else if(m_step == 1)
	{
		if(m_isDraw)
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);

		m_theta = m_blRecord.angle * PI / 180;
		m_draw.DrawTanLineSegment(m_blRecord.point, m_theta, m_len, false);
		m_draw.DrawOrderNumber(m_bl_p2, m_draw.GetOrderNumber());
	}
	else if(m_step == 2)
	{
		if(m_isDraw)
			m_draw.DrawTanLineSegment(m_tempP, m_theta, m_len, true);

		m_theta = m_blRecord.angle * PI / 180;
		m_draw.DrawTanLineSegment(m_blRecord.point, m_theta, m_len, false);

		m_theta = m_rlRecord.angle * PI / 180;
		m_draw.DrawTanLineSegment(m_rlRecord.point, m_theta, m_len, false);
		m_draw.DrawOrderNumber(m_bl_p2, m_draw.GetOrderNumber());
		m_draw.DrawOrderNumber(m_rl_p2, m_draw.GetOrderNumber());
	}
	m_update.ClearMeasure();
}
#endif

///>距离比例测量－两点法
D2MeasureRatioDistDot::D2MeasureRatioDistDot(int method,const SingleItemInfo *ptrSingleItemInfo)
{	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;
	m_method = method;
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureRatioDistDot::~D2MeasureRatioDistDot()
{
	Esc();
}

void D2MeasureRatioDistDot::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dist1 = 0.0;
	m_dist2 = 0.0;
	m_ratio = INVALID_VAL;
	m_angusty = INVALID_VAL;
}

void D2MeasureRatioDistDot::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	vector<POINT> vec;

    double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_dist1_start = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawDotLine(m_dist1_start, m_tempP, true);

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawDotLine(m_dist1_start, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawDotLine(m_dist1_start, m_tempP, false);
			m_dist1_end = m_tempP;
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.DrawOrderNumber(m_dist1_end, m_draw.GetOrderNumber());

			m_step = 2;
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_dist2_start = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawDotLine(m_dist2_start, m_tempP, true);

			m_step = 3;
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_dist2_start, m_tempP, true);
			}
			m_dist2_end = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawDotLine(m_dist2_start, m_tempP, false);

			// save measure result
			vec.clear();
			vec.push_back(m_dist1_start);
			vec.push_back(m_dist1_end);
			vec.push_back(m_dist2_start);
			vec.push_back(m_dist2_end);
			m_ptrMan->AddNew(RATIO_DIST_DOT, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			m_dist1 = m_calc.D2CalcDist(m_dist1_start, m_dist1_end);
			m_dist2 = m_calc.D2CalcDist(m_dist2_start, m_dist2_end);
			m_ratio = m_calc.CalcRatio(m_dist1, m_dist2);
			m_angusty = m_calc.D2CalcAngusty(m_dist1, m_dist2);
			// 确定测量结果，第二个参数为false时表示测量结束
			if(m_method==0)//距离狭窄比
            {
               // m_update.D2AngustyDist(m_dist1, m_dist2, m_angusty, attr, false);
                m_ptrMan->SingleMeaDataMan((double)m_angusty, m_itemInfo, allData, SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
            }
			else
            {
               // m_update.D2RatioDist(m_dist1, m_dist2, m_ratio, attr, false);
                m_ptrMan->SingleMeaDataMan((double)m_ratio, m_itemInfo, allData, SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
            }

			//begin new dist measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

	}
}

void D2MeasureRatioDistDot::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawDotLine(m_dist1_start, m_tempP, true);
			}
			m_tempP = m_dist1_start;
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawCursor(m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;
			m_dist1 = INVALID_VAL;
			m_dist2 = INVALID_VAL;
			m_ratio = INVALID_VAL;
			m_angusty = INVALID_VAL;
			//清除正在测量的结果
			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = m_dist1_end;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_dist1_start, m_tempP, false);
			m_draw.DrawDotLine(m_dist1_start, m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawOrderNumber(m_dist1_end, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			m_step = 1;
			m_dist2 = INVALID_VAL;
			m_ratio = INVALID_VAL;
			m_angusty = INVALID_VAL;

			//清除正在测量的结果
			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
				m_draw.DrawDotLine(m_dist2_start, m_tempP, true);
			}
			m_tempP = m_dist2_start;
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 2;
			m_dist2 = INVALID_VAL;
			m_ratio = INVALID_VAL;
			m_angusty = INVALID_VAL;

			//清除正在测量的结果
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureRatioDistDot::MouseMove(POINT p)
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
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawDotLine(m_dist1_start, m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawDotLine(m_dist1_start, m_tempP, true);
			m_isDraw = TRUE;

			m_dist1 = m_calc.D2CalcDist(m_dist1_start, m_tempP);
			if (m_method==0)//距离狭窄比
			    m_update.D2AngustyDist(m_dist1, (double)0, (double)0, attr);
			else
			    m_update.D2RatioDist(m_dist1, (double)0, (double)0, attr);

			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawDotLine(m_dist2_start, m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawDotLine(m_dist2_start, m_tempP, true);
			m_isDraw = TRUE;

			m_dist2 = m_calc.D2CalcDist(m_dist2_start, m_tempP);
			m_ratio = m_calc.CalcRatio(m_dist1, m_dist2);
			m_angusty = m_calc.D2CalcAngusty(m_dist1, m_dist2);
			if (m_method==0)//距离狭窄比
			    m_update.D2AngustyDist(m_dist1, m_dist2, m_angusty, attr);
			else
			    m_update.D2RatioDist(m_dist1, m_dist2, m_ratio, attr);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureRatioDistDot::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_dist1_start, FALSE);
		m_draw.DrawCursor(m_dist1_start);

		POINT p1;
		p1 = m_dist1_start;
		m_dist1_start = m_tempP;
		m_tempP = p1;
		m_draw.SetCursor(m_tempP);
	}
	else if(m_step == 3)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_dist2_start, FALSE);
		m_draw.DrawCursor(m_dist2_start);

		POINT p1;
		p1 = m_dist2_start;
		m_dist2_start = m_tempP;
		m_tempP = p1;
		m_draw.SetCursor(m_tempP);
	}
}

void D2MeasureRatioDistDot::Esc()
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
			m_draw.DrawCursor(m_dist1_start, FALSE);
			m_draw.DrawDotLine(m_dist1_start, m_tempP, true);
		}
		m_update.ClearMeasure();
	}
	else if (m_step == 2)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_dist1_start, FALSE);
			m_draw.DrawCursor(m_dist1_end, FALSE);
			m_draw.DrawDotLine(m_dist1_start, m_dist1_end);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawOrderNumber(m_dist1_end, m_draw.GetOrderNumber());
		}
		m_update.ClearMeasure();
	}
	else if (m_step == 3)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_dist1_start, FALSE);
			m_draw.DrawCursor(m_dist1_end, FALSE);
			m_draw.DrawDotLine(m_dist1_start, m_dist1_end);
			m_draw.DrawOrderNumber(m_dist1_end, m_draw.GetOrderNumber());

			m_draw.DrawCursor(m_dist2_start, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawDotLine(m_dist2_start, m_tempP, true);
		}
		m_update.ClearMeasure();
	}
}

///>距离周长比例测量
D2MeasureRatioDistPeri::D2MeasureRatioDistPeri(const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureRatioDistPeri::~D2MeasureRatioDistPeri()
{
	Esc();
}

void D2MeasureRatioDistPeri::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dist = 0;
	m_perimeter = INVALID_VAL;
	m_ratio = INVALID_VAL;
}

void D2MeasureRatioDistPeri::PressLeft(POINT p)
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
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_dist_start = m_tempP;
			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawDotLine(m_dist_start, m_tempP, true);
			m_dist_end = m_tempP;

			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawDotLine(m_dist_start, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawDotLine(m_dist_start, m_tempP, false);
			m_dist_end = m_tempP;
			m_tempP = m_draw.DrawInitialCursor();
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.DrawOrderNumber(m_dist_end, m_draw.GetOrderNumber());

			m_step = 2;
			break;

		case 2: // first
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_s_start = m_l_end = m_l_start = m_tempP;
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_step = 3;
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}
			m_l_end = m_tempP;
			m_s_start = CycleVertex(m_l_start, m_l_end);
			m_tempP = m_s_start;
			m_draw.SetCursor(m_s_start);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_step = 4;
			m_isDraw = TRUE;
			break;

		case 4:
			{
				if (m_isDraw)
				{
				    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
				}
				m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, false);

				// save measure result
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_dist_start);
				vec.push_back(m_dist_end);
				vec.push_back(m_l_start);
				vec.push_back(m_l_end);
				vec.push_back(m_s_start);
				m_ptrMan->AddNew(RATIO_D_P, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

				m_dist = m_calc.D2CalcDist(m_dist_start, m_dist_end);
				m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);//计算面积
				if(m_perimeter>ZERO)
                    m_ratio = m_dist / m_perimeter;
			//	m_update.D2RatioDistPeri(m_dist, m_perimeter, m_ratio, attr, false);//要显示比例

                m_ptrMan->SingleMeaDataMan((double)m_ratio, m_itemInfo, allData, SAVE);
				m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

				//begin new dist measure
				m_draw.ChangeCursorType(); // 更改鼠标类型
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void D2MeasureRatioDistPeri::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_dist_end, true);
			    m_draw.DrawDotLine(m_dist_start, m_dist_end, true);
			}
			m_draw.DrawCursor(m_dist_start, false);
			m_draw.DrawCursor(m_dist_start, true);
			m_draw.SetCursor(m_dist_start);
			m_tempP = m_dist_start;
			m_isDraw = TRUE;
			m_step = 0;
			m_dist = INVALID_VAL;
			m_perimeter = INVALID_VAL;
			m_ratio = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		case 2:// clear last measure result and measure line
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_l_start);
				m_draw.DrawDotLine(m_dist_start, m_dist_end, false);
			}
			m_draw.DrawCursor(m_dist_end, false);
			m_draw.DrawCursor(m_dist_end);
			m_draw.DrawDotLine(m_dist_start, m_dist_end, true);
			m_draw.SetCursor(m_dist_end);
			m_draw.DrawOrderNumber(m_dist_end, m_draw.GetOrderNumber());
			m_tempP = m_dist_end;
			m_isDraw = TRUE;
			m_step = 1;
			m_perimeter = INVALID_VAL;
			m_ratio = INVALID_VAL;

			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			m_tempP = m_l_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 2;
			break;

		case 4:
			//	if (m_isDraw)
			//	{
			//		m_draw.DrawCursor(m_tempP);
			//	}

			m_tempP = m_l_end;
			m_draw.SetCursor(m_tempP);
			//	m_draw.DrawCursor(m_tempP, FALSE);
			//	m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 3;
			break;

		default:
			break;
	}
}

void D2MeasureRatioDistPeri::MouseMove(POINT p)
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
			    m_draw.DrawCursor(m_tempP, true);
			}

			m_tempP = p;
			m_dist_start = m_tempP;
			m_draw.DrawCursor(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawDotLine(m_dist_start, m_tempP, true);
			}

			m_tempP = p;
			m_dist_end = m_tempP;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawDotLine(m_dist_start, m_tempP, true);
			m_isDraw = TRUE;

			m_dist = m_calc.D2CalcDist(m_dist_start, m_dist_end);
			//		m_update.D2RatioDist(dist, (double)0, (double)0);
			m_update.D2RatioDistPeri(m_dist, (double)0, (double)0, attr);//要显示比例
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}

			m_tempP = p;
			m_l_start = m_tempP;
			m_draw.DrawCursor(p, true);
			m_isDraw = TRUE;
			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			m_tempP = p;
			m_l_end = p;
			m_s_start = CycleVertex(m_l_start, m_l_end);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_isDraw = TRUE;

			m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);//计算面积
			m_ratio = m_dist / m_perimeter;
			m_update.D2RatioDistPeri(m_dist, m_perimeter, m_ratio, attr);//要显示比例
			break;

		case 4:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			}

			//	double theta, pox, poy,
			if (abs(p.x-m_tempP.x) >= abs(p.y-m_tempP.y))
			{
				m_s_start.x += (p.x - m_tempP.x);
				if ((m_l_start.y - m_l_end.y) != 0)
					m_s_start.y = ((double)m_l_start.y+(double)m_l_end.y) / 2.0 +
						((double)m_s_start.x - ((double)m_l_start.x+(double)m_l_end.x) / 2.0) *
						(((double)m_l_end.x-(double)m_l_start.x) /
						 ((double)m_l_start.y-(double)m_l_end.y));
			}
			else
			{
				m_s_start.y += (p.y - m_tempP.y);
				if ((m_l_start.x - m_l_end.x) != 0)
					m_s_start.x = ((double)m_l_start.x+(double)m_l_end.x) / 2.0 +
						((double)m_s_start.y - ((double)m_l_start.y+(double)m_l_end.y) / 2.0) *
						(((double)m_l_end.y - (double)m_l_start.y) /
						 ((double)m_l_start.x -(double)m_l_end.x));
			}

			m_tempP = m_s_start;
			m_draw.SetCursor(m_tempP);
			m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
			m_isDraw = TRUE;

			m_perimeter = m_calc.D2CalcEllipse(m_l_start, m_l_end, m_s_start, 2);//计算面积
			if(m_perimeter>ZERO)
               m_ratio = m_dist / m_perimeter;
			m_update.D2RatioDistPeri(m_dist, m_perimeter, m_ratio, attr);//要显示比例
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureRatioDistPeri::Change()
{
    if (m_step == 1) {
	if (m_isDraw) {
	    m_draw.DrawCursor(m_tempP);
	    m_draw.DrawCursor(m_tempP, FALSE);
	}
	m_draw.DrawCursor(m_dist_start, FALSE);
	m_draw.DrawCursor(m_dist_start);

	POINT p1;
	p1 = m_dist_start;
	m_dist_start = m_tempP;
	m_tempP = p1;
	m_draw.SetCursor(m_tempP);

    } else if (m_step == 3) {
	if (m_isDraw) {
	    m_draw.DrawCursor(m_tempP, FALSE);
	}
	m_draw.DrawCursor(m_l_start, FALSE);

	POINT p1;
	p1 = m_l_start;
	m_l_start = m_l_end;
	m_l_end = p1;
	m_tempP = m_l_end;
	m_draw.SetCursor(m_tempP);
    }
}

void D2MeasureRatioDistPeri::Esc()
{
	if (m_step==0)
		m_draw.DrawCursor(m_tempP);
	else if (m_step == 1)
	{
		m_draw.DrawCursor(m_dist_start, FALSE);
		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawDotLine(m_dist_start, m_dist_end, true);
		m_update.ClearMeasure();
	}
	else if (m_step == 2)
	{
		m_draw.DrawCursor(m_dist_start, FALSE);
		m_draw.DrawCursor(m_dist_end, FALSE);
		m_draw.DrawDotLine(m_dist_start, m_dist_end, false);
		m_draw.DrawOrderNumber(m_dist_end, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_tempP, true);
		m_update.ClearMeasure();
	}
	else
	{
		m_draw.DrawCursor(m_dist_start, false);
		m_draw.DrawCursor(m_dist_end, false);
		m_draw.DrawDotLine(m_dist_start, m_dist_end, false);
		m_draw.DrawOrderNumber(m_dist_end, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_l_start, false);
		m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start, true);
		m_update.ClearMeasure();
	}
}

///> 面积比例-椭圆法
D2MeasureRatioArea::D2MeasureRatioArea(int method,const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_method = method;
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureRatioArea::~D2MeasureRatioArea()
{
	Esc();
}

void D2MeasureRatioArea::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_area1 = INVALID_VAL;
	m_area2 = INVALID_VAL;
	m_ratio = INVALID_VAL;
	m_angusty = INVALID_VAL;
}

void D2MeasureRatioArea::PressLeft(POINT p)
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
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_s1_start = m_l1_end = m_l1_start = m_tempP;
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			m_step = 1;
			m_isDraw = TRUE;

			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			}
			m_l1_end = m_tempP;
			m_s1_start = CycleVertex(m_l1_start, m_l1_end);
			m_tempP = m_s1_start;
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			m_draw.SetCursor(m_s1_start);
			m_step = 2;
			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 3);
				m_update.D2RatioVol(m_vol1, (double)0, (double)0, attr);
			}
			else
			{
				m_area1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 1);
				if (m_method==0)
				    m_update.D2AngustyArea(m_area1, (double)0, (double)0, attr);
				else
				    m_update.D2RatioArea(m_area1, (double)0, (double)0, attr);
			}
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			}
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, false);
			m_isDraw = TRUE;
			m_step = 3;
			m_draw.DrawOrderNumber(m_s1_start, m_draw.GetOrderNumber());

			if (m_method==2)
			{
				m_vol1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 3);
				m_update.D2RatioVol(m_vol1, (double)0, (double)0, attr);
			}
			else
			{
				m_area1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 1);
				if (m_method==0)
				    m_update.D2AngustyArea(m_area1, (double)0, (double)0, attr);
				else
				    m_update.D2RatioArea(m_area1, (double)0, (double)0, attr);
			}

			//begin new dist measure
			m_tempP = m_draw.DrawInitialCursor();
			break;

		case 3: // first
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_s2_start = m_l2_end = m_l2_start = m_tempP;
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			m_step = 4;
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			}
			m_l2_end = m_tempP;
			m_s2_start = CycleVertex(m_l2_start, m_l2_end);
			m_tempP = m_s2_start;
			m_draw.SetCursor(m_s2_start);
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			m_step = 5;
			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 3);
				if (m_vol2 > ZERO)
				{
					m_ratio = m_vol1 / m_vol2;
				}
			}
			else
			{
				m_area2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 1);
				if (m_area2 > ZERO)
				{
					m_angusty = fabs(m_area1 - m_area2) / m_area2;
					m_ratio = m_area1 / m_area2;
				}
			}

			if (m_method == 0)
			    m_update.D2AngustyArea(m_area1, m_area2, m_angusty, attr);
			else if (m_method == 1)
			    m_update.D2RatioArea(m_area1, m_area2, m_ratio, attr);
			else if (m_method == 2)
			    m_update.D2RatioVol(m_vol1, m_vol2, m_ratio, attr);

			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			}
			m_s2_start = m_tempP;
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, false);

			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 3);
				if (m_vol2 > ZERO)
				{
					m_ratio = m_vol1 / m_vol2;
				}
			}
			else
			{
				m_area2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 1);
				if (m_area2 > ZERO)
				{
					m_angusty = fabs(m_area1 - m_area2) / m_area2;
					m_ratio = m_area1 / m_area2;
				}

			}

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_l1_start);
			vec.push_back(m_l1_end);
			vec.push_back(m_s1_start);
			vec.push_back(m_l2_start);
			vec.push_back(m_l2_end);
			vec.push_back(m_s2_start);
			m_ptrMan->AddNew(RATIO_AREA, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			if (m_method == 0)
            {
               // m_update.D2AngustyArea(m_area1, m_area2, m_angusty, attr, false);
                m_ptrMan->SingleMeaDataMan(m_angusty, m_itemInfo, allData, SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
            }
			else if (m_method == 1)
            {
                //m_update.D2RatioArea(m_area1, m_area2, m_ratio, attr, false);
                m_ptrMan->SingleMeaDataMan(m_ratio, m_itemInfo, allData, SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
            }
			else if (m_method == 2)
            {
               // m_update.D2RatioVol(m_vol1, m_vol2, m_ratio, attr, false);
                m_ptrMan->SingleMeaDataMan(m_ratio, m_itemInfo, allData, SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
            }

			m_area1 = INVALID_VAL;
			m_area2 = INVALID_VAL;
			m_angusty = INVALID_VAL;
			m_ratio = INVALID_VAL;

			//begin new dist measure
			m_step = 0;
			m_tempP = m_draw.DrawInitialCursor();
			m_l1_start = m_tempP;
			m_l1_end = m_tempP;
			m_s1_start = m_tempP;
			m_l2_start = m_tempP;
			m_l2_end = m_tempP;
			m_s2_start = m_tempP;

            // change pointer
            ChangePointerWhenMeasure();
			break;
			//default:
			//	break;
	}
}

void D2MeasureRatioArea::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			}

			m_tempP = m_l1_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 0;

			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			}

			m_tempP = m_l1_end;
			m_s1_start = CycleVertex(m_l1_start, m_l1_end);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 1;
			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, false);
			}

			m_tempP = m_l1_end;
			m_s1_start = CycleVertex(m_l1_start, m_l1_end);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			m_draw.DrawOrderNumber(m_s1_start, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 2;
			m_update.ClearMeasure();
			break;

		case 4:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			}

			m_tempP = m_l2_start;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 3;

			m_update.ClearMeasure();
			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			}

			m_tempP = m_l2_end;
			m_s2_start = CycleVertex(m_l2_start, m_l2_end);
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			m_isDraw = TRUE;
			m_draw.SetCursor(m_tempP);
			m_step = 4;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureRatioArea::MouseMove(POINT p)
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
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			}

			m_tempP = p;
			m_l1_end = p;
			m_s1_start = CycleVertex(m_l1_start, m_l1_end);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 3);
				m_update.D2RatioVol(m_vol1, (double)0, (double)0, attr);
			}
			else
			{
				m_area1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 1);
				if (m_method==0)
				    m_update.D2AngustyArea(m_area1, (double)0, (double)0, attr);
				else
				    m_update.D2RatioArea(m_area1, (double)0, (double)0, attr);
			}

			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			}
			//	double theta, pox, poy,
			if (abs(p.x-m_tempP.x) >= abs(p.y-m_tempP.y))
			{
				m_s1_start.x += (p.x - m_tempP.x);
				if ((m_l1_start.y - m_l1_end.y) != 0)
					m_s1_start.y = ((double)m_l1_start.y+(double)m_l1_end.y) / 2.0 +
						((double)m_s1_start.x - ((double)m_l1_start.x+(double)m_l1_end.x) / 2.0) *
						(((double)m_l1_end.x-(double)m_l1_start.x) /
						 ((double)m_l1_start.y-(double)m_l1_end.y));
			}
			else
			{
				m_s1_start.y += (p.y - m_tempP.y);
				if ((m_l1_start.x - m_l1_end.x) != 0)
					m_s1_start.x = ((double)m_l1_start.x+(double)m_l1_end.x) / 2.0 +
						((double)m_s1_start.y - ((double)m_l1_start.y+(double)m_l1_end.y) / 2.0) *
						(((double)m_l1_end.y - (double)m_l1_start.y) /
						 ((double)m_l1_start.x -(double)m_l1_end.x));
			}

			//	m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
			m_tempP = m_s1_start;
			m_draw.SetCursor(m_tempP);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
			//	m_draw.DrawCursor(p);
			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 3);
				m_update.D2RatioVol(m_vol1, (double)0, (double)0, attr);
			}
			else
			{
				m_area1 = m_calc.D2CalcEllipse(m_l1_start, m_l1_end, m_s1_start, 1);
				if (m_method==0)
				    m_update.D2AngustyArea(m_area1, (double)0, (double)0, attr);
				else
				    m_update.D2RatioArea(m_area1, (double)0, (double)0, attr);
			}

			break;

		case 3:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			}

			m_tempP = p;
			m_l2_end = p;
			m_s2_start = CycleVertex(m_l2_start, m_l2_end);
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 3);
				if (m_vol2 > ZERO)
				{
					m_ratio = m_vol1 / m_vol2;
				}
			}
			else
			{
				m_area2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 1);
				if (m_area2 > ZERO)
				{
					m_angusty = fabs(m_area1 - m_area2) / m_area2;
					m_ratio = m_area1 / m_area2;
				}
			}

			if (m_method == 0)
			    m_update.D2AngustyArea(m_area1, m_area2, m_angusty, attr);
			else if (m_method == 1)
			    m_update.D2RatioArea(m_area1, m_area2, m_ratio, attr);
			else if (m_method == 2)
			    m_update.D2RatioVol(m_vol1, m_vol2, m_ratio, attr);
			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			}

			//	double theta, pox, poy,
			if (abs(p.x-m_tempP.x) >= abs(p.y-m_tempP.y))
			{
				m_s2_start.x += (p.x - m_tempP.x);
				if ((m_l2_start.y - m_l2_end.y) != 0)
					m_s2_start.y = ((double)m_l2_start.y+(double)m_l2_end.y) / 2.0 +
						((double)m_s2_start.x - ((double)m_l2_start.x+(double)m_l2_end.x) / 2.0) *
						(((double)m_l2_end.x-(double)m_l2_start.x) /
						 ((double)m_l2_start.y-(double)m_l2_end.y));
			}
			else
			{
				m_s2_start.y += (p.y - m_tempP.y);
				if ((m_l2_start.x - m_l2_end.x) != 0)
					m_s2_start.x = ((double)m_l2_start.x+(double)m_l2_end.x) / 2.0 +
						((double)m_s2_start.y - ((double)m_l2_start.y+(double)m_l2_end.y) / 2.0) *
						(((double)m_l2_end.y - (double)m_l2_start.y) /
						 ((double)m_l2_start.x -(double)m_l2_end.x));
			}

			//	m_draw.DrawEllipse(m_l_start, m_l_end, m_s_start);
			m_tempP = m_s2_start;
			m_draw.SetCursor(m_tempP);
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
			m_isDraw = TRUE;

			if (m_method==2)
			{
				m_vol2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 3);
				if (m_vol2 > ZERO)
				{
					m_ratio = m_vol1 / m_vol2;
				}
			}
			else
			{
				m_area2 = m_calc.D2CalcEllipse(m_l2_start, m_l2_end, m_s2_start, 1);
				if (m_area2 > ZERO)
				{
					m_angusty = fabs(m_area1 - m_area2) / m_area2;
					m_ratio = m_area1 / m_area2;
				}
			}

			if (m_method == 0)
			    m_update.D2AngustyArea(m_area1, m_area2, m_angusty, attr);
			else if (m_method == 1)
			    m_update.D2RatioArea(m_area1, m_area2, m_ratio, attr);
			else if (m_method == 2)
			    m_update.D2RatioVol(m_vol1, m_vol2, m_ratio, attr);

			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureRatioArea::Change()
{
	POINT p1;

	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_l1_start, FALSE);

		p1 = m_l1_start;
		m_l1_start = m_l1_end;
		m_l1_end = p1;
		m_tempP = m_l1_end;
		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 4)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_l2_start, FALSE);

		p1 = m_l2_start;
		m_l2_start = m_l2_end;
		m_l2_end = p1;
		m_tempP = m_l2_end;
		m_draw.SetCursor(m_tempP);
	}
}

void D2MeasureRatioArea::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
		}
	}
	else if (m_step == 3)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_l1_start, FALSE);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, false);
			m_draw.DrawOrderNumber(m_s1_start, m_draw.GetOrderNumber());
			m_draw.DrawCursor(m_tempP);
		}
	}
	else if((m_step == 1) || (m_step == 2))
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_l1_start, FALSE);
			//	m_draw.DrawCursor(m_l1_end);
			//	m_draw.DrawCursor(m_s1_start);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, true);
		}
	}
	else if((m_step == 4) || (m_step == 5))
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_l1_start, FALSE);
			m_draw.DrawEllipse(m_l1_start, m_l1_end, m_s1_start, false);
			m_draw.DrawOrderNumber(m_s1_start, m_draw.GetOrderNumber());
			m_draw.DrawCursor(m_l2_start, FALSE);
			//	m_draw.DrawCursor(m_l2_end);
			//	m_draw.DrawCursor(m_s2_start);
			m_draw.DrawEllipse(m_l2_start, m_l2_end, m_s2_start, true);
		}
	}
	m_update.ClearMeasure();
}

//角度比例、髋关节
D2MeasureRatioAngle::D2MeasureRatioAngle(const MultiItemInfo *ptrMultiItemInfo)
{
//	m_item = ptrMultiItemInfo->item;
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureRatioAngle::~D2MeasureRatioAngle()
{
	Esc();
}

void D2MeasureRatioAngle::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_angle1 = INVALID_VAL;
	m_angle2 = INVALID_VAL;
	m_ratio = INVALID_VAL;
}

void D2MeasureRatioAngle::PressLeft(POINT p)
{
	double dataMea[MEA_MULTI];
	double allData[MULTI_MAX];
	int i;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	for (i=0; i<MEA_MULTI; i++)
		dataMea[i] = INVALID_VAL;

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_vertex1 = m_tempP;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			    m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_left1 = m_tempP;
			m_tempP = m_vertex1;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_step = 2;
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex1, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_right1 = m_tempP;
			m_angle1 = m_calc.D2CalcAngle(m_vertex1, m_left1, m_right1);
			dataMea[0] = m_angle1;
			if (m_itemInfo->meaType == RATIO_ANGLE)
				m_update.D2RatioAngle(m_angle1, 0, 0, attr);
			else
				m_update.D2Hip(m_angle1, 0, attr);
			m_draw.DrawOrderNumber(m_right1, m_draw.GetOrderNumber());

			m_step = 3;
			m_draw.DrawCursor(m_tempP);//第二个较的顶点
			m_isDraw = TRUE;
			break;

		case 3: // second
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_vertex2 = m_tempP;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_step = 4;
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex2, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex2, m_tempP, false);
			m_left2 = m_tempP;
			m_tempP = m_vertex2;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_step = 5;
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 5:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex2, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex2, m_tempP, false);

			m_right2 = m_tempP;
			m_angle2 = m_calc.D2CalcAngle(m_vertex2, m_left2, m_right2);
			dataMea[1] = m_angle2;
			if (m_itemInfo->meaType == RATIO_ANGLE)
			{
				if (m_angle2 >ZERO)
					m_ratio = m_angle1 / m_angle2;
			}
			else
			{
				dataMea[0] = m_angle1;
				dataMea[1] = m_angle2;
				m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);
			}

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_left1);
			vec.push_back(m_vertex1);
			vec.push_back(m_right1);
			vec.push_back(m_left2);
			vec.push_back(m_vertex2);
			vec.push_back(m_right2);
			if (m_itemInfo->meaType == RATIO_ANGLE)
				m_ptrMan->AddNew(RATIO_ANGLE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			else
				m_ptrMan->AddNew(HIP, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			if (m_itemInfo->meaType == RATIO_ANGLE)
			{
				m_update.D2RatioAngle(m_angle1, m_angle2, m_ratio, attr, false);
			}
			else
			{
				m_update.D2Hip(m_angle1, m_angle2, attr, false);
			}

			//begin new dist measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

			//default:
			//	break;
	}
}
void D2MeasureRatioAngle::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = m_vertex1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;

			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = m_left1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_draw.SetCursor(m_tempP);

			m_angle1 = INVALID_VAL;
			m_isDraw = TRUE;
			m_step = 1;

			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_right1;
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_draw.DrawOrderNumber(m_right1, m_draw.GetOrderNumber());
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 2;

			m_update.ClearMeasure();
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = m_vertex2;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 3;

			m_update.ClearMeasure();
			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = m_left2;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex2, m_tempP, false);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_angle2 = INVALID_VAL;
			m_ratio = INVALID_VAL;
			m_isDraw = TRUE;
			m_step = 4;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureRatioAngle::MouseMove(POINT p)
{
	double dataMea[MEA_MULTI];
	double allData[MULTI_MAX];

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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = p;
			m_left1 = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_vertex1, p, true);
			m_isDraw = TRUE;
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = p;
			m_right1 = p;
			m_draw.DrawCursor(p, true);
			m_draw.DrawLine(m_vertex1, p, true);
			m_isDraw = TRUE;

			m_angle1 = m_calc.D2CalcAngle(m_vertex1, m_left1, p);
			dataMea[0] = m_angle1;
			if (m_itemInfo->meaType == RATIO_ANGLE)
				m_update.D2RatioAngle(m_angle1, 0, 0, attr);
			else
				m_update.D2Hip(m_angle1, 0, attr);
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = p;
			m_left2 = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_vertex2, p, true);
			m_isDraw = TRUE;
			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = p;
			m_right2 = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_vertex2, p, true);
			m_isDraw = TRUE;

			m_angle2 = m_calc.D2CalcAngle(m_vertex2, m_left2, p);
			if (m_itemInfo->meaType == RATIO_ANGLE)
			{
				if (m_angle2 >ZERO)
					m_ratio = m_angle1 / m_angle2;
				m_update.D2RatioAngle(m_angle1, m_angle2, m_ratio, attr);
			}
			else
			{
				m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
				m_update.D2Hip(m_angle1, m_angle2, attr);
			}
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureRatioAngle::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_vertex1, FALSE);
		m_draw.DrawCursor(m_vertex1);

		POINT p1;
		p1 = m_vertex1;
		m_vertex1 = m_tempP;
		m_tempP = p1;
		m_left1 = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 2)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
		}

		POINT p1;
		p1 = m_left1;
		m_left1 = m_tempP;
		m_tempP = p1;
		m_right1 = m_tempP;
		m_draw.DrawCursor(m_right1, FALSE);
		m_draw.DrawCursor(m_right1);
		m_draw.DrawLine(m_vertex1, m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, true);
		m_draw.SetCursor(m_right1);
	}
	if (m_step == 4)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_vertex2, FALSE);
		m_draw.DrawCursor(m_vertex2);

		POINT p1;
		p1 = m_vertex2;
		m_vertex2 = m_tempP;
		m_tempP = p1;
		m_left2 = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 5)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_draw.DrawLine(m_vertex2, m_tempP, false);
		}

		POINT p1;
		p1 = m_left2;
		m_left2 = m_tempP;
		m_tempP = p1;
		m_right2 = m_tempP;
		m_draw.DrawCursor(m_right2, FALSE);
		m_draw.DrawCursor(m_right2);
		m_draw.DrawLine(m_vertex2, m_right2, false);
		m_draw.DrawLine(m_vertex2, m_right2, true);

		m_draw.SetCursor(m_right2);
	}
}

void D2MeasureRatioAngle::Esc()
{
	if(m_step == 0)
		m_draw.DrawCursor(m_tempP);
	else if(m_step == 1 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_tempP);
		m_draw.DrawLine(m_vertex1, m_tempP, true);
	}
	else if(m_step == 2 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex1, m_tempP, true);
	}
	else if(m_step == 3 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, false);
		m_draw.DrawOrderNumber(m_right1, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_tempP, true);
	}
	else if(m_step == 4 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, false);
		m_draw.DrawOrderNumber(m_right1, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_vertex2, false);
		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex2, m_tempP, true);
	}
	else if(m_step == 5 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, false);
		m_draw.DrawOrderNumber(m_right1, m_draw.GetOrderNumber());

		m_draw.DrawCursor(m_vertex2, false);
		m_draw.DrawCursor(m_left2, false);
		m_draw.DrawLine(m_vertex2, m_left2, false);

		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex2, m_tempP, true);
	}
	m_update.ClearMeasure();
}

#if 0
///> 角度比例测量-三点法
D2MeasureRatioAngle::D2MeasureRatioAngle()
{
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureRatioAngle::~D2MeasureRatioAngle()
{
	Esc();
}

void D2MeasureRatioAngle::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_angle1 = INVALID_VAL;
	m_angle2 = INVALID_VAL;
	m_ratio = INVALID_VAL;
}

void D2MeasureRatioAngle::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_vertex1 = m_tempP;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_step = 1;
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			    m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_left1 = m_tempP;
			m_tempP = m_vertex1;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_step = 2;
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 2:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex1, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_right1 = m_tempP;
			m_angle1 = m_calc.D2CalcAngle(m_vertex1, m_left1, m_right1);
			m_update.D2RatioAngle(m_angle1, 0, 0, attr);
			m_step = 3;
			m_draw.DrawCursor(m_tempP);//第二个较的顶点
			m_isDraw = TRUE;
			break;

		case 3: // second
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_vertex2 = m_tempP;
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_step = 4;
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex2, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex2, m_tempP, false);
			m_left2 = m_tempP;
			m_tempP = m_vertex2;
			m_draw.DrawCursor(m_tempP, true);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_step = 5;
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			break;

		case 5:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex2, m_tempP, true);
			}
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawLine(m_vertex2, m_tempP, false);

			m_right2 = m_tempP;
			m_angle2 = m_calc.D2CalcAngle(m_vertex2, m_left2, m_right2);
			if (m_angle2 > ZERO)
				m_ratio = m_angle1 / m_angle2;

			// save measure result
			vector<POINT> vec;
			vec.clear();
			vec.push_back(m_left1);
			vec.push_back(m_vertex1);
			vec.push_back(m_right1);
			vec.push_back(m_left2);
			vec.push_back(m_vertex2);
			vec.push_back(m_right2);
			m_ptrMan->AddNew(RATIO_ANGLE, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			m_update.D2RatioAngle(m_angle1, m_angle2, m_ratio, attr, false);
			//begin new dist measure
			m_draw.ChangeCursorType();
			Init();
			break;

			//default:
			//	break;
	}
}
void D2MeasureRatioAngle::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = m_vertex1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;

			m_update.ClearMeasure();
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = m_left1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_draw.SetCursor(m_tempP);

			m_angle1 = INVALID_VAL;
			m_isDraw = TRUE;
			m_step = 1;

			m_update.ClearMeasure();
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = m_right1;
			m_draw.DrawCursor(m_tempP, false);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 2;

			m_update.ClearMeasure();
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = m_vertex2;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 3;

			m_update.ClearMeasure();
			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = m_left2;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_vertex2, m_tempP, false);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_angle2 = INVALID_VAL;
			m_ratio = INVALID_VAL;
			m_isDraw = TRUE;
			m_step = 4;

			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void D2MeasureRatioAngle::MouseMove(POINT p)
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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = p;
			m_left1 = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_vertex1, p, true);
			m_isDraw = TRUE;
			break;

		case 2:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex1, m_tempP, true);
			}

			m_tempP = p;
			m_right1 = p;
			m_draw.DrawCursor(p, true);
			m_draw.DrawLine(m_vertex1, p, true);
			m_isDraw = TRUE;

			m_angle1 = m_calc.D2CalcAngle(m_vertex1, m_left1, p);
			m_update.D2RatioAngle(m_angle1, 0, 0, attr);
			break;

		case 3:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 4:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = p;
			m_left2 = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_vertex2, p, true);
			m_isDraw = TRUE;
			break;

		case 5:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    m_draw.DrawLine(m_vertex2, m_tempP, true);
			}

			m_tempP = p;
			m_right2 = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_vertex2, p, true);
			m_isDraw = TRUE;

			m_angle2 = m_calc.D2CalcAngle(m_vertex2, m_left2, p);
			if(m_angle2 >ZERO)
				m_ratio = m_angle1 / m_angle2;
			m_update.D2RatioAngle(m_angle1, m_angle2, m_ratio, attr);
			break;

		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureRatioAngle::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_vertex1, FALSE);
		m_draw.DrawCursor(m_vertex1);

		POINT p1;
		p1 = m_vertex1;
		m_vertex1 = m_tempP;
		m_tempP = p1;
		m_left1 = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 2)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_vertex1, m_tempP, true);
			m_draw.DrawLine(m_vertex1, m_tempP, false);
		}

		POINT p1;
		p1 = m_left1;
		m_left1 = m_tempP;
		m_tempP = p1;
		m_right1 = m_tempP;
		m_draw.DrawCursor(m_right1, FALSE);
		m_draw.DrawCursor(m_right1);
		m_draw.DrawLine(m_vertex1, m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, true);
		m_draw.SetCursor(m_right1);
	}
	if (m_step == 4)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
		}
		m_draw.DrawCursor(m_vertex2, FALSE);
		m_draw.DrawCursor(m_vertex2);

		POINT p1;
		p1 = m_vertex2;
		m_vertex2 = m_tempP;
		m_tempP = p1;
		m_left2 = m_tempP;

		m_draw.SetCursor(m_tempP);
	}
	else if (m_step == 5)
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_vertex2, m_tempP, true);
			m_draw.DrawLine(m_vertex2, m_tempP, false);
		}

		POINT p1;
		p1 = m_left2;
		m_left2 = m_tempP;
		m_tempP = p1;
		m_right2 = m_tempP;
		m_draw.DrawCursor(m_right2, FALSE);
		m_draw.DrawCursor(m_right2);
		m_draw.DrawLine(m_vertex2, m_right2, false);
		m_draw.DrawLine(m_vertex2, m_right2, true);

		m_draw.SetCursor(m_right2);
	}
}

void D2MeasureRatioAngle::Esc()
{
	if(m_step == 0)
		m_draw.DrawCursor(m_tempP);
	else if(m_step == 1 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_tempP);
		m_draw.DrawLine(m_vertex1, m_tempP, true);
	}
	else if(m_step == 2 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex1, m_tempP, true);
	}
	else if(m_step == 3 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, false);

		m_draw.DrawCursor(m_tempP, true);
	}
	else if(m_step == 4 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, false);

		m_draw.DrawCursor(m_vertex2, false);
		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex2, m_tempP, true);
	}
	else if(m_step == 5 )
	{
		m_draw.DrawCursor(m_vertex1, false);
		m_draw.DrawCursor(m_left1, false);
		m_draw.DrawLine(m_vertex1, m_left1, false);

		m_draw.DrawCursor(m_right1, false);
		m_draw.DrawLine(m_vertex1, m_right1, false);

		m_draw.DrawCursor(m_vertex2, false);
		m_draw.DrawCursor(m_left2, false);
		m_draw.DrawLine(m_vertex2, m_left2, false);

		m_draw.DrawCursor(m_tempP, true);
		m_draw.DrawLine(m_vertex2, m_tempP, true);
	}
	m_update.ClearMeasure();
}
#endif

///> 直方图－包括灰度直方图和彩色直方图
D2MeasureHistogram::D2MeasureHistogram()
{
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
D2MeasureHistogram::~D2MeasureHistogram()
{
	Esc();
}

void D2MeasureHistogram::Init()
{
	int i;

	m_step = 0;
	m_isDraw = TRUE;
	m_ptrMan->ClearFirst();
	m_tempP = m_draw.DrawInitialCursor();
	for (i=0; i<256; i++)
		m_sum[i]=0;
}
void D2MeasureHistogram::PressLeft(POINT p)
{
	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
		    m_p1 = m_tempP;
			m_area_start = m_p1;

			m_step = 1;
			m_isDraw = FALSE;
            break;

		case 1:
			{
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}
				m_area_end = m_tempP;

				POINT tempP;
				m_draw.DrawCursor(m_p1, FALSE);
				//	m_draw.DrawCursor(m_tempP);
				tempP.x = m_p1.x;
				tempP.y = m_tempP.y;
				m_draw.DrawLine(m_p1, tempP);
				m_draw.DrawLine(tempP, m_tempP);
				tempP.x = m_tempP.x;
				tempP.y = m_p1.y;
				m_draw.DrawLine(m_p1, tempP);
				m_draw.DrawLine(tempP, m_tempP);

				m_step = 2;
				DrawHistogram::GetInstance()->SetArea(m_area_start, m_area_end);
				DrawHistogram::GetInstance()->SetOnOff(1);
				m_isDraw = FALSE;

                // change pointer
                //ChangePointerWhenMeasure();
				MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
				break;
			}

		default:
			break;
	}
}
void D2MeasureHistogram::PressRight(POINT p)
{
	POINT tempP;

	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				tempP.x = m_p1.x;
				tempP.y = m_tempP.y;
				m_draw.DrawLine(m_p1, tempP);
				m_draw.DrawLine(tempP, m_tempP);
				tempP.x = m_tempP.x;
				tempP.y = m_p1.y;
				m_draw.DrawLine(m_p1, tempP);
				m_draw.DrawLine(tempP, m_tempP);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_isDraw = TRUE;
			m_step = 0;

			m_update.ClearLast();
			break;

		default:
			break;
	}
}

void D2MeasureHistogram::MouseMove(POINT p)
{
	POINT tempP;
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
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:

            if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				tempP.x = m_p1.x;
				tempP.y = m_tempP.y;
				m_draw.DrawLine(m_p1, tempP);
				m_draw.DrawLine(tempP, m_tempP);
				tempP.x = m_tempP.x;
				tempP.y = m_p1.y;
				m_draw.DrawLine(m_p1, tempP);
				m_draw.DrawLine(tempP, m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);

			tempP.x = m_p1.x;
			tempP.y = p.y;
			m_draw.DrawLine(m_p1, tempP);
			m_draw.DrawLine(tempP, p);
			tempP.x = p.x;
			tempP.y = m_p1.y;
			m_draw.DrawLine(m_p1, tempP);
			m_draw.DrawLine(tempP, p);
			m_isDraw = TRUE;

		//	double area;
		//	area = m_calc.D2CalcAreaRec(m_p1, p);
		//	m_update.D2AreaRec(area, attr);
			break;
		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureHistogram::Change()
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
	}
}

void D2MeasureHistogram::Esc()
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
		POINT p12, p21;

		if (m_isDraw)
		{
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawCursor(m_p1, FALSE);

			p12.x = m_p1.x;
			p12.y = m_tempP.y;
			p21.x = m_tempP.x;
			p21.y = m_p1.y;
			m_draw.DrawLine(m_p1, p12);
			m_draw.DrawLine(m_p1, p21);
			m_draw.DrawLine(p12, m_tempP);
			m_draw.DrawLine(p21, m_tempP);
		}
	}
}

//断面图
D2MeasureProfile::D2MeasureProfile()
{
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

D2MeasureProfile::~D2MeasureProfile()
{
	Esc();
}

void D2MeasureProfile::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_ptrMan->ClearFirst();
	m_tempP = m_draw.DrawInitialCursor();
}

void D2MeasureProfile::PressLeft(POINT p)
{
	switch(m_step)
	{
		case 0: // first
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_p1 = m_tempP;;
			m_start = m_p1;

			m_step = 1;
			m_isDraw = FALSE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_draw.DrawCursor(m_tempP, FALSE);
			m_end = m_tempP;

			unsigned char *image;
			ImageArea::GetInstance()->GetImgPtr(&image);
			DrawProfile(m_start, m_end, image);

			//begin new dist measure
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

			//default:
			//	break;
	}
}

void D2MeasureProfile::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_draw.SetCursor(m_tempP);
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP);
			}

			m_tempP = m_p1;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;

			m_update.ClearLast();
			break;

		default:
			break;
	}
}

void D2MeasureProfile::MouseMove(POINT p)
{
	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP);
			}

			m_tempP = p;
			m_draw.DrawCursor(p);
			m_draw.DrawLine(m_p1, p);
			m_isDraw = TRUE;

			double dist;
			dist = m_calc.D2CalcDist(m_p1, p);
			break;
		default:
			break;
	}
}

/*
 * @brief Change current moveable point between p1 and p2
 */
void D2MeasureProfile::Change()
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
		m_start = m_p1;
		m_end = m_tempP;
		m_draw.SetCursor(m_tempP);
	}
}

void D2MeasureProfile::DrawProfile(POINT start, POINT end, unsigned char *image_p)
{
	int i;
	//	unsigned char *dot_p;
	POINT coor_origin;
	POINT tmp;
	unsigned int max_x, min_x, max_y, min_y;

	max_x = (start.x > end.x) ? start.x : end.x;
	min_x = (start.x > end.x) ? end.x : start.x;
	max_y = (start.y > end.y) ? start.y : end.y;
	min_y = (start.y > end.y) ? end.y : start.y;

	if (abs(end.x - start.x)>=abs(end.y - start.y))
	{
		if (min_y > 150)
			coor_origin.y = min_y - 5;
		else if (max_y < IMAGE_H - 150)
			coor_origin.y = max_y + 150;
		else if ((min_y <= 150)&&(max_y >= IMAGE_H - 150))
			coor_origin.y = max_y;

		coor_origin.x = min_x;

		tmp.x = max_x;
		tmp.y = coor_origin.y;
		m_draw.DrawLine(coor_origin, tmp, FALSE, COPY);
		tmp.x = coor_origin.x;
		tmp.y = coor_origin.y - 128;
		m_draw.DrawLine(coor_origin, tmp, FALSE, COPY);

		double slope;
		int dir_sign;
		if (end.x >= start.x)
			dir_sign = 1;
		else
			dir_sign = -1;
		slope = (double)(end.y - start.y) / (double)(end.x - start.x);
		for (i=0; i<abs(end.x-start.x); i++)
		{
			//好像不会出现水平或竖直的问题
			tmp.x = start.x + dir_sign * i;
			tmp.y = start.y + (int)((double)(i*dir_sign) * slope);//像素点的坐标
			//	m_draw.DrawPoint(tmp, FALSE);
			tmp.y = coor_origin.y - (*(image_p + BYTES_DOT * (IMAGE_W * tmp.y + tmp.x))) / 2;//断面图点的坐标
			m_draw.DrawPoint(tmp, FALSE, COPY);
		}
	}
	else
	{
		if (min_x > 150)
			coor_origin.x = min_x - 5;
		else if (max_x < (IMG_W-150))
			coor_origin.x = max_x + 150;
		else if ((min_x <= 150)&&(max_x >= (IMG_W-150)))
			coor_origin.x = min_x;

		coor_origin.y = max_y;

		tmp.y = min_y;
		tmp.x = coor_origin.x;
		m_draw.DrawLine(coor_origin, tmp, FALSE, COPY);
		tmp.y = coor_origin.y;
		tmp.x = coor_origin.x - 128;
		m_draw.DrawLine(coor_origin, tmp, FALSE, COPY);

		double slope;
		int dir_sign;
		if (start.y >= end.y)
			dir_sign = 1;
		else
			dir_sign = -1;
		slope = (double)(end.x - start.x) / (double)(end.y - start.y);
		for (i=0; i<abs(end.y-start.y); i++)
		{
			tmp.y = start.y - dir_sign * i;
			tmp.x = start.x - (int)((double)(i*dir_sign) * slope);
			tmp.x = coor_origin.x - (*(image_p + BYTES_DOT * (IMAGE_W * tmp.y + tmp.x))) / 2;//断面图点的坐标
			m_draw.DrawPoint(tmp, FALSE, COPY);
		}
	}

}

void D2MeasureProfile::Esc()
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
			m_draw.DrawLine(m_p1, m_tempP);
		}
		m_update.ClearMeasure();
	}
}

POINT CycleVertex( POINT p1, POINT p2 )
{
	POINT p3;
	double pox, poy, p1x, p1y, p2x, p2y;

	p1x = (double)p1.x;
	p1y = (double)p1.y;
	p2x = (double)p2.x;
	p2y = (double)p2.y;

	pox = (p1x + p2x) / 2.0;
	poy = (p1y + p2y) / 2.0;

	p3.x = (p1y - p2y) / 2.0 + pox;
	p3.y = (p2x - p1x) / 2.0 + poy;
	return p3;
}

//IMT
POINT D2MeasureIMT::m_tempP_old;
POINT D2MeasureIMT::m_p1_old;
unsigned int *D2MeasureIMT::pIntima = NULL;
unsigned int *D2MeasureIMT::pAdventitia = NULL;
int D2MeasureIMT::roi_width=0;
int D2MeasureIMT::roi_height=0;
D2MeasureIMT::D2MeasureIMT(bool dir)
{
    m_ptrMan = MeasureMan::GetInstance();
    m_dir = dir;
    m_ImtStatus = TRUE;
    Init();
    m_RealIMTStatus = FALSE;
    CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
    //CDrawIMT::GetInstance()->GetIMTStatus(m_ImtStatus);

}

D2MeasureIMT::~D2MeasureIMT()
{
#if 0
    m_RealIMTStatus = FALSE;
    CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
    CDrawIMT::GetInstance()->ClearInAdventIMT();
#endif

    HintArea::GetInstance()->ClearHint();
    Esc();
}

void D2MeasureIMT::Init()
{
    m_step = 0;
    m_isDraw = TRUE;
    m_tempP = m_draw.DrawInitialCursor();
    //	m_dist = INVALID_VAL;
}

void D2MeasureIMT::PressLeft(POINT p)
{
    UpdateMeasure::ResultAttr attr;
    attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();

    switch(m_step)
    {
        case 0: // first
            m_draw.DrawIMTBox(m_p1_old, m_tempP_old, FALSE);

            CDrawIMT::GetInstance()->ClearInAdventIMT();

            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
            }
            m_draw.DrawCursor(m_tempP, FALSE);
            m_p1 = m_tempP;

            m_tempP = m_draw.CalcNextP(m_tempP, 0);
            m_draw.SetCursor(m_tempP);
            m_draw.DrawCursor(m_tempP);
            m_draw.DrawIMTBox(m_p1, m_tempP, TRUE);
            m_step = 1;
            m_isDraw = TRUE;
            m_RealIMTStatus = FALSE;
            CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
            break;

        case 1:
            {
                if (m_isDraw)
                {
                    m_draw.DrawCursor(m_tempP);
                    m_draw.DrawIMTBox(m_p1, m_tempP, TRUE);
                }
                m_draw.DrawCursor(m_p1, FALSE);
                m_draw.DrawIMTBox(m_p1, m_tempP, TRUE);

                if (m_p1.x < m_tempP.x)
                {
                    start.x = m_p1.x;
                    end.x = m_tempP.x;
                }
                else
                {
                    start.x = m_tempP.x;
                    end.x = m_p1.x;
                }
                if (m_p1.y < m_tempP.y)
                {
                    end.y = m_tempP.y;
                    start.y = m_p1.y;
                }
                else
                {
                    end.y = m_p1.y;
                    start.y = m_tempP.y;
                }
                m_p1_old = start;
                m_tempP_old = end;

                int w = m_p1.x-m_tempP.x;
                w = w>0?w:-w;
                int h = m_p1.y-m_tempP.y;
                h = h>0?h:-h;
                roi_width = w;
                roi_height = h;

                PRINTF("*******w h start.x start.y end.x end.y********%d %d %d %d %d %d\n",w,h,start.x,start.y,end.x,end.y);

                CDrawIMT::GetInstance()->GetWH(w, h);
                CDrawIMT::GetInstance()->GetArea(start, end);
                CDrawIMT::GetInstance()->GetResultAttr(attr);
                CDrawIMT::GetInstance()->GetDirIMT(m_dir);
                if (ModeStatus::IsFreezeMode())
                {
                    if (Replay::GetInstance()->GetSelectImgForIMT() == NULL)
                    {
                        HintArea::GetInstance()->UpdateHint(_("please selected frame numbers are more than one."),1);
                        m_draw.DrawIMTBox(m_p1, m_tempP, FALSE);
                        Init();
                        break;
                    }
                    else
                    {
                        CDrawIMT::GetInstance()->UpdateIMT();
                    }
                }
                else
                {
                    m_RealIMTStatus = TRUE;
                    CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
                }
                Init();
            }
            break;

        default:
            break;
    }
}

void D2MeasureIMT::PressRight(POINT p) //press undo
{
/**
 * liahuamei
 * 2012.07.10
 */
#if 1
    switch(m_step)
    {
        case 0:// clear last measure result and measure line
            m_ptrMan->ClearLast();
            m_update.ClearLast();
            m_draw.DrawIMTBox(m_p1_old, m_tempP_old, FALSE);

            CDrawIMT::GetInstance()->ClearInAdventIMT();
            m_RealIMTStatus = FALSE;

            CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
            break;

        case 1:
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
                m_draw.DrawIMTBox(m_p1, m_tempP, FALSE);
            }

            m_tempP = m_p1;
            m_draw.DrawCursor(m_tempP, FALSE);
            m_draw.DrawCursor(m_tempP);
            m_draw.SetCursor(m_p1);
            m_isDraw = TRUE;
            m_step = 0;

            m_draw.DrawIMTBox(m_p1_old, m_tempP_old, FALSE);
             m_RealIMTStatus = FALSE;
             CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);

            //清除正在测量的结果
            m_update.ClearMeasure();
            break;

        default:
            break;
    }
#endif
#if 0
  switch(m_step)
    {
        case 0:// clear last measure result and measure line
            m_ptrMan->ClearLast();
            m_update.ClearLast();
            m_draw.DrawIMTBox(m_p1_old, m_tempP_old, FALSE);

            //remove timer
            if (ModeStatus::IsUnFreezeMode())
                CDrawIMT::GetInstance()->RemoveTimer();
            else
                CDrawIMT::GetInstance()->ClearInAdventIMT();
            break;

        case 1:
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
                m_draw.DrawIMTBox(m_p1, m_tempP, FALSE);
            }

            m_tempP = m_p1;
            m_draw.DrawCursor(m_tempP, FALSE);
            m_draw.DrawCursor(m_tempP);
            m_draw.SetCursor(m_p1);
            m_isDraw = TRUE;
            m_step = 0;

            m_draw.DrawIMTBox(m_p1_old, m_tempP_old, FALSE);

            //remove timer
            if (ModeStatus::IsUnFreezeMode())
                CDrawIMT::GetInstance()->RemoveTimer();
            else
                CDrawIMT::GetInstance()->ClearInAdventIMT();

            //清除正在测量的结果
            m_update.ClearMeasure();
            break;

        default:
            break;
    }
#endif

}

void D2MeasureIMT::MouseMove(POINT p)
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
            m_draw.DrawCursor(p);
            m_isDraw = TRUE;
            break;

        case 1:
            if (m_isDraw)
            {
                m_draw.DrawCursor(m_tempP);
                m_draw.DrawIMTBox(m_p1, m_tempP, FALSE);
            }

            m_tempP = p;
            m_draw.DrawCursor(m_tempP);
            m_draw.DrawIMTBox(m_p1, m_tempP, TRUE);
            m_isDraw = TRUE;
            m_RealIMTStatus = FALSE;
            CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
            break;

        default:
            break;
    }
}

void D2MeasureIMT::Change()
{
    if (m_step == 1)
    {
        if (m_isDraw)
        {
            m_draw.DrawCursor(m_tempP);
            m_draw.DrawCursor(m_tempP, FALSE);
        }
        m_draw.DrawIMTBox(m_p1, m_tempP, FALSE);
        m_draw.DrawCursor(m_p1, FALSE);
        m_draw.DrawCursor(m_p1);

        POINT p1;
        p1 = m_p1;
        m_p1 = m_tempP;
        m_tempP = p1;
        m_draw.SetCursor(m_tempP);
    }
    m_RealIMTStatus = FALSE;
    CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);

    CDrawIMT::GetInstance()->ClearInAdventIMT();
}

void D2MeasureIMT::Esc() //press point and other keyboard
{

 if (m_step == 0)
    {
        if (m_isDraw)
        {
            m_draw.DrawCursor(m_tempP);
        }
        m_draw.DrawIMTBox(m_p1_old, m_tempP_old, FALSE);

        m_RealIMTStatus = FALSE;
        CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
        CDrawIMT::GetInstance()->ClearInAdventIMT();
        //m_ImtStatus = FALSE;
        //CDrawIMT::GetInstance()->GetIMTStatus(m_ImtStatus);

        //clear measure result
        m_update.ClearMeasure(); //擦除冻结时最后一帧的结果

    }
    else
    {
        if (m_isDraw)
        {
            m_draw.DrawCursor(m_tempP);
            m_draw.DrawCursor(m_p1, FALSE);
        }
        CDrawIMT::GetInstance()->ClearInAdventIMT();

        //clear roi recentage
        m_draw.DrawIMTBox(m_p1, m_tempP, FALSE);

        m_RealIMTStatus = FALSE;
        CDrawIMT::GetInstance()->GetRealIMTStatus(m_RealIMTStatus);
        //m_ImtStatus = FALSE;
        //CDrawIMT::GetInstance()->GetIMTStatus(m_ImtStatus);

        //clear mesure result
        m_update.ClearMeasure();
    }
}