/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: MeasureD.cpp
 * @brief: measure in D(PW/CW) scan mode
 *
 * version: V1.0
 * date: 2009-7-15
 * @author: zhanglei
 */

#include "MeasureD.h"
#include <vector>
#include <stdio.h>
#include <math.h>
#include "../display/HintArea.h"
#include "ImageAreaDraw.h"

using std::vector;
extern bool g_calcPwStatus;

DMeasureManualTrack::DMeasureManualTrack():MOUSE_INTERVAL(2)
{
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureManualTrack::~DMeasureManualTrack()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void DMeasureManualTrack::Init()
{
    HintArea::GetInstance()->ClearHint();
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_mouse_count = 0;
	m_track.clear();
    m_trackTemp.clear();
    HintArea::GetInstance()->UpdateHint(_("[ManualTrack]: <Update> to switch to AutoTrack."));
}

void DMeasureManualTrack::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0: // first
#ifdef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[ManualTrack]: <Auto> to cancel or redraw. <Update> to AutoTrack."));
#else
            HintArea::GetInstance()->UpdateHint(_("[ManualTrack]: <Value> to cancel or redraw. <Update> to AutoTrack."));
#endif
			if (m_isDraw)
			{
				m_draw.DrawCursor(m_tempP);
			}
			m_p1 = m_tempP;
			m_line_tempP = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawCursor(m_tempP);

			m_step = 1;
			m_isDraw = TRUE;

			m_track.clear();
			m_track.push_back(m_p1);
            m_trackTemp.clear();

			break;

		case 1:
			{
				char buf[350]= {'\0'};

				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
					for(int i = (m_track.size() - 1); i != 0; i--)
						m_draw.DrawTraceLine(m_track[i-1], m_track[i], TRUE);
						//m_draw.DrawTraceLine(m_track[i], m_track[i-1], TRUE);
				}

				if ((m_tempP.x != m_line_tempP.x) && (m_tempP.y != m_line_tempP.y))
				{
					m_track.push_back(m_tempP);
				}
				m_draw.DrawCursor(m_track[m_track.size() - 1], FALSE);

				for(int i = (m_track.size() - 1); i != 0; i--)
					m_draw.DrawTraceLine(m_track[i-1], m_track[i], FALSE);
					//m_draw.DrawTraceLine(m_track[i], m_track[i-1], FALSE);

				if (m_track.size() > 1)
				{
					m_calc.DCalcTrace(buf, m_track, m_psP, m_edP, 1);
                    if(m_psP.x == m_edP.x)
                    {
                        m_draw.DrawCursor(m_track[0], FALSE);
                        for(int i = (m_track.size() - 1); i != 0; i--)
                            m_draw.DrawTraceLine(m_track[i-1], m_track[i], FALSE);
                            //m_draw.DrawTraceLine(m_track[i], m_track[i-1], FALSE);
                        m_draw.DrawCursor(m_track[m_track.size() - 1], FALSE);
                        Init();

                        // change pointer
                        ChangePointerWhenMeasure();
                        break;
                    }

					m_track.push_back(m_psP);
					m_track.push_back(m_edP);
				}

                ImageAreaDraw::GetInstance()->SetPsEdValue(m_psP, m_edP, 1);
                 m_update.DTrace(buf, attr, true);//为了使结果显示的数目和测量线的数目一致，在此用true的方式多显示了一次
				// save measure result
				m_ptrMan->AddNew(MEASURE_TRACK, m_draw.GetOrderNumber(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				//m_ptrMan->AddNew(MEASURE_TRACK, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				if(m_update.DTrace(buf, attr, false))
					m_draw.ChangeCursorType(); // 更改鼠标类型
				Init();

				// change pointer
                //ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void DMeasureManualTrack::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();

            m_psP.x = m_edP.x = 0;
            ImageAreaDraw::GetInstance()->SetPsEdValue(m_psP, m_edP, 1);
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
				m_mouse_count = 0;
				//	m_update.ClearLast();
				//清除正在测量的结果
			//	m_update.ClearMeasure();
			}
			break;

		default:
			break;
	}

    g_calcPwStatus = false;
}

void DMeasureManualTrack::MouseMove(POINT p)
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
				m_line_tempP = p;
				m_track.push_back(m_line_tempP);
				m_mouse_count = 0;
			}
			m_tempP = p;
            m_trackTemp.clear();

			m_draw.DrawCursor(p);
			m_isDraw = TRUE;

			break;
		default:
			break;
	}
}

void DMeasureManualTrack::Value(EKnobOper opr)
{
    UpdateMeasure::ResultAttr attr;
    attr.cursorType = m_draw.GetCursorType();
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

extern AbsMeasure* g_ptrAbsMeasure;

void DMeasureManualTrack::Change()
{
    if(g_ptrAbsMeasure != NULL)
    {
        delete g_ptrAbsMeasure;
        g_ptrAbsMeasure = NULL;
    }
    g_ptrAbsMeasure = new DMeasureManualAuto();
	return;
}

void DMeasureManualTrack::Esc()
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
	//	m_update.ClearMeasure();
	}
	m_track.clear();
    m_trackTemp.clear();
}

///描点法手动描迹
DMeasureManualDot::DMeasureManualDot()
{
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureManualDot::~DMeasureManualDot()
{
	Esc();
}

void DMeasureManualDot::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialCursor();
	m_dot.clear();
}

void DMeasureManualDot::PressLeft(POINT p)
{
	unsigned int vec_size = 0;
	unsigned int i;

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
			m_p1 = m_tempP;
			m_draw.DrawCursor(m_tempP, FALSE);

			m_tempP = m_draw.CalcNextP(m_tempP, 0);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawCursor(m_tempP);

			m_step = 1;
			m_isDraw = TRUE;

			m_dot.clear();
			m_dot.push_back(m_p1);
			HintArea::GetInstance()->UpdateHint(_("[Trace]: <Update> to set shape point."));
			break;

		case 1:
			{
				char buf[350]= {'\0'};

				HintArea::GetInstance()->ClearHint();
				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP);
				}
				m_draw.DrawCursor(m_tempP, FALSE);

				m_dot.push_back(m_tempP);
				vec_size = m_dot.size();
				for(i=0; i!=(vec_size-1); i++)
				{
					m_draw.DrawLine(m_dot[i], m_dot[i+1]);
				}

				//	m_psTmp = m_dot[0];
				//清除掉中间多余的光标
				for(i=1; i!=vec_size-1; i++)
				{
					m_draw.DrawCursor(m_dot[i], FALSE);
				}

				if (vec_size > 1)
				{
					m_calc.DCalcTrace(buf, m_dot, m_psP, m_edP, 1);
                    m_draw.DrawTraceTag(m_psP, FALSE);
                    m_draw.DrawTraceTag(m_edP, FALSE);
					m_dot.push_back(m_psP);
					m_dot.push_back(m_edP);
				}

				m_update.DTrace(buf, attr, true);//为了使结果显示的数目和测量线的数目一致，在此用true的方式多显示了一次
				// save measure result
				m_ptrMan->AddNew(MEASURE_TRACK, m_draw.GetCursorType(), m_dot, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				if(m_update.DTrace(buf, attr, false))
					m_draw.ChangeCursorType(); // 更改鼠标类型
				Init();

				// change pointer
               // ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void DMeasureManualDot::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:// clear last measure result and measure line
			m_ptrMan->ClearLast();
			m_update.ClearLast();
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
			HintArea::GetInstance()->ClearHint();
			break;

		default:
			break;
	}
}

void DMeasureManualDot::MouseMove(POINT p)
{
	if (m_isDraw)
	{
		m_draw.DrawCursor(m_tempP);
	}

	m_tempP = p;
	m_draw.DrawCursor(m_tempP);
	m_isDraw = TRUE;
}

void DMeasureManualDot::Esc()
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
		HintArea::GetInstance()->ClearHint();
	}
}

void DMeasureManualDot::Change()
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

//============================trace测量使用自动的方式===========================//
extern int g_setFunc;
extern int g_curOper;
DMeasureManualAuto::DMeasureManualAuto()
{
    m_ptrMan = MeasureMan::GetInstance();
    //Init();
    g_curOper = 1;
    m_step = 0;
    m_isDraw = TRUE;
    m_cursorTypeChg = false;
    m_psP.x = m_edP.x = 0;
    AutoTraceInit();
}

DMeasureManualAuto::~DMeasureManualAuto()
{
    HintArea::GetInstance()->ClearHint();
    Esc();
}

void DMeasureManualAuto::Init()
{
	m_step = 0;
	//m_startP = m_endP = m_draw.DrawInitialVLine();
    m_isDraw = TRUE;
    g_curOper = 1;
    g_setFunc = 3;
    m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
#ifdef EMP_355
    HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack. <Auto> press to adjust."));
#else
    HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack. <Value> press to adjust."));
#endif
}

void DMeasureManualAuto::SemiAutoCalc()
{
    m_endP = m_startP = m_draw.DrawInitialVLine();
    //m_draw.DrawVLine(m_startP, true);
    g_setFunc = 1;//未找到合适周期
    HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack."));
    m_cursorTypeChg = false;
    m_auto = false;
}

void DMeasureManualAuto::AutoCalc()
{
    m_auto = true;
    if(m_startP.x != m_endP.x)
    {
        g_setFunc = 3; //找到合适周期
        m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
#if 0
        vector<POINT> vec;
        m_draw.ClearPwCycle(m_startP.x, m_endP.x);
        m_draw.DrawVLine(m_startP, false, COPY);
        m_draw.DrawVLine(m_endP, false, COPY);
        m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
        vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);
        if(!m_track.empty())
            m_track.clear();
        m_track.push_back(m_startP);
        if (!vec.empty())
        {
            for (int i=0; i<vec.size(); i++)
            {
                m_track.push_back(vec[i]);
            }
        }
        m_track.push_back(m_endP);
        m_track.push_back(m_psP);
        m_track.push_back(m_edP);
        UpdateMeasure::ResultAttr attr;
        attr.cursorType = m_draw.GetOrderNumber();
        attr.curColor = m_draw.GetCurColor();
        attr.confirmColor = m_draw.GetConfirmColor();

        static int orderNum = 1;
        int order = m_draw.GetOrderNumber();
        if((orderNum != order) || order == 1)
        {
            if(ImageAreaDraw::GetInstance()->GetCalcAutoPara() != NULL)
            {
                m_update.DTrace(ImageAreaDraw::GetInstance()->GetCalcAutoPara(), attr, true);
                printf("====autoPara is not null: %s\n", ImageAreaDraw::GetInstance()->GetCalcAutoPara());
            }
            else
                printf("====autoPara is null\n");
            m_ptrMan->AddNew(MEASURE_TRACK_AUTO, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
            if(ImageAreaDraw::GetInstance()->GetCalcAutoPara() != NULL)
                m_update.DTrace(ImageAreaDraw::GetInstance()->GetCalcAutoPara(), attr, false);
        }
        orderNum = m_draw.GetOrderNumber();
#endif
#ifdef EMP_355
        HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Auto> press to adjust."));
#else
        HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Value> press to adjust."));
#endif
		m_cursorTypeChg = true;
    }
    else
    {
        g_setFunc = 0; //全自动下没有找到合适周期
        HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto."));
    }
}

void DMeasureManualAuto::AutoTraceInit()
{
    if (ModeStatus::IsUnFreezeMode() && (!ModeStatus::IsPwMode()))
        return;

    POINT cycleStart, cycleEnd;
    POINT ps, ed;

    const Image::AutoTracePara *para = Replay::GetInstance()->GetCurrentTraceData();
    ImageAreaDraw::GetInstance()->DrawTraceTag(para, cycleStart, cycleEnd, ps, ed);
    m_startP = cycleStart;
    m_endP = cycleEnd;
    m_psP = ps;
    m_edP = ed;
    AutoCalc();
}

void DMeasureManualAuto::PressLeft(POINT p)
{
    if(g_setFunc == 3 || g_setFunc == 0)
        return;
	vector<POINT> vec;
	unsigned int i;
    char buf[350] = {'\0'};
    POINT tmpP;
    tmpP.x = 0;
    tmpP.y = 0;
    POINT endP;

    UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();
    int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
    //printf("m_step:%d m_start(%d, %d) end(%d, %d)\n", m_step, m_startP.x, m_startP.y, m_endP.x, m_endP.y);
    switch(m_step)
	{
		case 0: //start point
            // set
            if(m_auto)
#ifdef EMP_355
                HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Auto> press to adjust."));
#else
                HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Value> press to adjust."));
#endif
			else
                HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack."));
            switch(g_curOper)
            {
                case 1:
                    {
                        if(!vec.empty())
                            vec.clear();
                        if (m_isDraw)
                        {
                            if(m_startP.x == m_endP.x)
                            {
                                m_draw.DrawVLine(m_startP, true);
                                m_draw.DrawVLine(m_startP, false);
                                endP.x = m_startP.x + 2;
                                endP.y = m_startP.y;
                                m_endP = endP;
                                m_draw.DrawVLine(endP, true);
                                m_step = 1;
                                break;
                            }
                            else
                            {
                                ImageArea::GetInstance()->ClearTrace();
                                m_draw.DrawVLine(m_startP, true);
                                m_draw.DrawVLine(m_endP, false);
                            }
                        }
                        m_draw.DrawVLine(m_startP, false);
                        m_draw.DrawVLine(m_endP, false);

                        if(m_startP.x != m_endP.x)
                        {
                            vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);

                            m_startP.y = baseLine;
                            m_endP.y = baseLine;
                            m_psP = m_startP;
                            m_edP = m_endP;
                            m_track.clear();
                            m_track.push_back(m_startP);
                            if (!vec.empty())
                            {
                                for (i=0; i<vec.size(); i++)
                                {
                                    m_track.push_back(vec[i]);
                                }
                            }
                            m_track.push_back(m_endP);

                            //m_draw.PwTrace(vec, COPY);
                            m_draw.PwTrace(vec, XOR);
                            if(m_auto)
                                m_calc.DCalcTraceAuto(buf, vec, m_psP, m_edP, 0);
                            else
                                m_calc.DCalcTrace(buf, vec, m_psP, m_edP, 0);
                            m_track.push_back(m_psP);
                            m_track.push_back(m_edP);

                            //m_update.ClearLast();
                            //m_update.DTrace(buf, attr, false);
                            m_update.DTraceAdjust(buf, attr, false);
                            m_cursorTypeChg = true;
                            //m_cursorTypeChg = false;
                            // add to measure man
                            m_ptrMan->AddNewAdjust(MEASURE_TRACK_AUTO, m_draw.GetOrderNumber(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
                        }
                    }
                    //m_isDraw = FALSE;
                    break;
                case 2:
                    {
                        if(m_isDraw)
                        {
                            m_draw.DrawTraceTag(m_psP, true);
                            ImageArea::GetInstance()->ClearTrace();
                        }
                        m_draw.DrawTraceTag(m_edP, false);

                        if(!vec.empty())
                            vec.clear();
                        vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);
                        m_startP.y = baseLine;
                        m_endP.y = baseLine;
                        m_track.clear();
                        m_track.push_back(m_startP);
                        if (!vec.empty())
                        {
                            for (i=0; i<vec.size(); i++)
                            {
                                m_track.push_back(vec[i]);
                            }
                        }
                        m_track.push_back(m_endP);

                        m_draw.PwTrace(vec, XOR);
                        if(m_auto)
                            m_calc.DCalcTraceAutoOther(buf, vec, m_psP, tmpP);
                        else
                            m_calc.DCalcTraceOther(buf, vec, m_psP, tmpP);
                        m_track.push_back(m_psP);
                        m_track.push_back(m_edP);

                        m_update.DTraceAdjust(buf, attr, false);
                        m_cursorTypeChg = true;
                        // add to measure man
                        m_ptrMan->AddNewAdjust(MEASURE_TRACK_AUTO, m_draw.GetOrderNumber(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
                    }
                    break;

                case 3:
                    {
                        if(m_isDraw)
                        {
                            m_draw.DrawTraceTag(m_edP, true);
                            ImageArea::GetInstance()->ClearTrace();
                        }
                        m_draw.DrawTraceTag(m_psP, false);

                        if(!vec.empty())
                            vec.clear();
                        vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);
                        m_startP.y = baseLine;
                        m_endP.y = baseLine;
                        m_track.clear();
                        m_track.push_back(m_startP);
                        if (!vec.empty())
                        {
                            for (i=0; i<vec.size(); i++)
                            {
                                m_track.push_back(vec[i]);
                            }
                        }
                        m_track.push_back(m_endP);

                        m_draw.PwTrace(vec, XOR);
                        if(m_auto)
                            m_calc.DCalcTraceAutoOther(buf, vec, tmpP, m_edP);
                        else
                            m_calc.DCalcTraceOther(buf, vec, tmpP, m_edP);
                        m_track.push_back(m_psP);
                        m_track.push_back(m_edP);

                        m_update.DTraceAdjust(buf, attr, false);
                        m_cursorTypeChg = true;
                        // add to measure man
                        m_ptrMan->AddNewAdjust(MEASURE_TRACK_AUTO, m_draw.GetOrderNumber(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
                    }
                    break;

                case 4:
                    {
                        {
                            if(!vec.empty())
                                vec.clear();
                            if(m_isDraw)
                            {
                                ImageArea::GetInstance()->ClearTrace();
                                m_draw.DrawVLine(m_endP, true);
                            }
                            m_draw.DrawVLine(m_endP, false);

                            vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);
                            m_startP.y = baseLine;
                            m_endP.y = baseLine;
                            m_psP = m_startP;
                            m_edP = m_endP;
                            m_track.clear();
                            m_track.push_back(m_startP);
                            if (!vec.empty())
                            {
                                for (i=0; i<vec.size(); i++)
                                {
                                    m_track.push_back(vec[i]);
                                }
                            }
                            m_track.push_back(m_endP);

                            m_draw.PwTrace(vec, XOR);
                            if(m_auto)
                                m_calc.DCalcTraceAuto(buf, vec, m_psP, m_edP, 0);
                            else
                                m_calc.DCalcTrace(buf, vec, m_psP, m_edP, 0);
                            m_track.push_back(m_psP);
                            m_track.push_back(m_edP);

                            m_update.DTraceAdjust(buf, attr, false);
                            m_cursorTypeChg = true;
                            // add to measure man
                            m_ptrMan->AddNewAdjust(MEASURE_TRACK_AUTO, m_draw.GetOrderNumber(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
                            //m_update.DTrace(buf, attr, true);
                        }
                    }
                    break;
                default:
                    break;
            }

            g_curOper = 1;
            //m_isDraw = FALSE;
            if(m_startP.x == m_endP.x)
                m_step = 1;
            if(g_setFunc == 2)
            {
                g_setFunc = 3;
                m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
                if(m_auto)
#ifdef EMP_355
                    HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Auto> press to adjust."));
#else
                    HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Value> press to adjust."));
#endif
				else
                    HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack."));
            }
            break;

		case 1: //end point
            {
#ifdef EMP_355
                HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack. <Auto> press to adjust."));
#else
                HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack. <Value> press to adjust."));
#endif
                char buf[350] = {'\0'};
				// draw measure line
				if (m_isDraw)
				{
					m_draw.DrawVLine(m_endP, true);
				}

				m_draw.DrawVLine(m_endP, false);
                //printf("start.x=%d end.x=%d\n", m_startP.x, m_endP.x);
				vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);

				m_startP.y = baseLine;
				m_endP.y = baseLine;
				m_psP = m_startP;
				m_edP = m_endP;
				m_track.clear();
				m_track.push_back(m_startP);
				if (!vec.empty())
				{
					for (i=0; i<vec.size(); i++)
					{
						m_track.push_back(vec[i]);
					}
				}
				m_track.push_back(m_endP);

				//m_draw.PwTrace(vec, COPY);
				m_draw.PwTrace(vec, XOR);
                //printf("vec size:%d\n", vec.size());
				m_calc.DCalcTrace(buf, vec, m_psP, m_edP, 0);
			    //printf("b:ps= %d, %d, ed= %d, %d\n", m_psP.x, m_psP.y, m_edP.x, m_edP.y);
				m_track.push_back(m_psP);
				m_track.push_back(m_edP);
				m_update.DTrace(buf, attr, true);//为了使结果显示的数目和测量线的数目一致，在此用true的方式多显示了一次
                // add to measure man
				m_ptrMan->AddNew(MEASURE_TRACK_AUTO, m_draw.GetOrderNumber(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				if(m_update.DTrace(buf, attr, false))
                {
                    m_cursorTypeChg = true;
					m_draw.ChangeCursorType();
                }
                else
                    m_cursorTypeChg = false;

				m_step = 0;
				m_isDraw = TRUE;
				Init();

				// change pointer
				//ChangePointerWhenMeasure();
                if(m_psP.x == m_edP.x)
                {
                    PressRight(m_psP);
                    m_cursorTypeChg = true;
                }
			}
			break;

		default:
			break;
	}
    //printf("left (%d, %d)\n", m_startP.x, m_endP.x);
}

void DMeasureManualAuto::PressRight(POINT p)
{
    if((g_setFunc == 0 || g_setFunc == 3) && (m_auto))
        return;

    //printf("---cursorType:%d setFunc:%d  curOper:%d\n", m_cursorTypeChg, g_setFunc, g_curOper);
    switch(m_step)
    {
        case 0:
            {
                if(g_setFunc == 2)
                {
                    ImageArea::GetInstance()->ClearTrace();
                    switch(g_curOper)
                    {

                        case 1:
                            if(m_startP.x == m_endP.x)
                                m_draw.DrawVLine(m_startP, true);
                            else
                            {
                                m_draw.DrawVLine(m_startP, true);
                                m_draw.DrawVLine(m_endP, false);
                            }
                            break;

                        case 2:
                        case 3:
                            {
                                m_draw.DrawVLine(m_startP, false);
                                m_draw.DrawVLine(m_endP, false);
                            }
                            break;

                        case 4:
                            {
                                m_draw.DrawVLine(m_startP, false);
                                m_draw.DrawVLine(m_endP, true);
                            }
                            break;
                        default:
                            break;
                    }

                    m_update.ClearLast();
                }
                else if(g_setFunc == 1)
                {
                    m_ptrMan->ClearLast();
                    if(m_cursorTypeChg)
                        m_update.ClearLast();
                    m_draw.DrawVLine(m_startP, true);
                }
                else if(g_setFunc == 3)
                {
                    m_ptrMan->ClearLast();
                    if(m_cursorTypeChg)
                        m_update.ClearLast();
                }
                g_setFunc = 1;
                g_curOper = 1;
                m_startP = m_endP = m_draw.DrawInitialVLine();
                m_edP.x = m_psP.x = 0;
            }
            break;

        case 1:
            if (m_isDraw)
            {
                m_draw.DrawVLine(m_endP, true);
            }
            //	m_endP = m_draw.DrawInitialVLine();
            m_draw.DrawVLine(m_startP, false);
            m_startP = m_endP = m_draw.DrawInitialVLine();
            m_psP.x = m_edP.x = 0;
            m_draw.SetCursor(m_startP);
            m_step = 0;
            m_isDraw = TRUE;
            g_setFunc = 1;
            g_curOper = 1;
            break;

        default:
            break;
    }
    g_calcPwStatus = false;
    HintArea::GetInstance()->UpdateHint(_("[SemiAuto]: <Update> to switch to ManualTrack."));
    m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
}

void DMeasureManualAuto::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

    if(g_setFunc == 3 || g_setFunc == 0)
        return;

    PRINTF("==curOpt:%d step:%d m_isDraw:%d setFunc:%d\n", g_curOper, m_step, m_isDraw, g_setFunc);
	switch(m_step)
	{
		case 0: //start point
            if(m_startP.x == m_endP.x)
            {
                if(m_isDraw)
                    m_draw.DrawVLine(m_startP, true);

                m_startP = m_endP = p;
                m_draw.DrawVLine(m_startP, true);
                m_cursor = p;
            }
            else
            {
                switch(g_curOper)
                {
                    case 1:
                        if(p.x > m_psP.x)
                            break;
                        if (m_isDraw)
                        {
                            m_draw.DrawVLine(m_startP, true);
                        }
                        m_startP = p;
                        m_cursor = p;
                        m_draw.DrawVLine(m_startP, true);
                        break;

                    case 2:
                        if((p.x >= m_edP.x - 5) || (p.x <= m_startP.x))
                            break;
                        if(m_isDraw)
                        {
                            m_draw.DrawTraceTag(m_psP, true, XOR, true);
                        }
                        m_psP = p;
                        m_cursor = p;
                        m_draw.DrawTraceTag(m_psP, true, XOR, true);
                        break;

                    case 3:
                        if((p.x > m_endP.x) || (p.x <= m_psP.x+5))
                            break;
                        if(m_isDraw)
                        {
                            m_draw.DrawTraceTag(m_edP, true, XOR, true);
                        }

                        m_edP = p;
                        m_cursor = p;
                        m_draw.DrawTraceTag(m_edP, true, XOR, true);
                        break;

                    case 4:
                        if(p.x < m_edP.x)
                            break;
                        if(m_isDraw)
                        {
                            m_draw.DrawVLine(m_endP, true);
                        }

                        m_endP = p;
                        m_cursor = p;
                        m_draw.DrawVLine(m_endP, true);
                        break;
                    default:
                        break;
                }
            }
			m_isDraw = TRUE;
			break;

		case 1: //end point
            if((p.x - 2) < m_startP.x)
            {
                break;
            }
            if (m_isDraw)
            {
                if(m_endP.x != m_startP.x)
                {
                    m_draw.DrawVLine(m_endP, true);
                }
            }
            m_endP = p;
            m_cursor = p;
            m_draw.DrawVLine(m_endP, true);
            m_isDraw = TRUE;
            break;

		default:
			break;
	}

    if((m_psP.x != 0) && (m_edP.x != 0))
        m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP, g_curOper);
    m_draw.SetCursor(m_cursor);
}

void DMeasureManualAuto::Value(EKnobOper opr)
{
#if 0
    //printf("ps(%d, %d) ed(%d, %d)\n", m_psP.x, m_psP.y, m_edP.x, m_edP.y);
    if((m_startP.x == m_endP.x) || (g_setFunc == 1) || (g_setFunc == 3))
        return;

    if(opr == ADD)
    {
        if(g_curOper < MAX_OPT)
            g_curOper += 1;
        else
            g_curOper = 1;

        switch(g_curOper)
        {
            case 1:
                //start cycle line
                if(m_startP.x != m_endP.x)
                {
                    m_draw.DrawVLine(m_endP, true);
                    m_draw.DrawVLine(m_endP, false);

                    m_draw.DrawVLine(m_startP, false);
                    m_draw.DrawVLine(m_startP, true);
                    m_draw.SetCursor(m_startP);
                }
                break;
            case 2:
                //ps position
                if(m_psP.x != m_edP.x)
                {
                    if(m_isDraw)
                    {
                        m_draw.DrawVLine(m_startP, true);
                        m_draw.DrawVLine(m_startP, false);
                    }

                    m_draw.DrawTraceTag(m_psP, false);
                    m_draw.DrawTraceTag(m_psP, true);
                    m_isDraw = TRUE;
                    m_draw.SetCursor(m_psP);
                }
                else
                {
                    m_draw.DrawVLine(m_startP, true);
                    m_draw.DrawVLine(m_startP, false);
                }
                break;
            case 3:
                //ed position
                if(m_isDraw)
                {
                    if(m_psP.x != m_edP.x)
                    {
                        m_draw.DrawTraceTag(m_psP, true);
                        m_draw.DrawTraceTag(m_psP, false);

                        m_draw.DrawTraceTag(m_edP, false);
                        m_draw.DrawTraceTag(m_edP, true);
                        m_draw.SetCursor(m_edP);
                    }
                }
                break;
            case 4:
                //end cycle line
                if(m_isDraw)
                {
                    if(m_startP.x != m_endP.x)
                    {
                        if(m_psP.x != m_edP.x)
                        {
                            m_draw.DrawTraceTag(m_edP, true);
                            m_draw.DrawTraceTag(m_edP, false);
                        }
                        m_draw.DrawVLine(m_endP, false);
                        m_draw.DrawVLine(m_endP, true);
                        m_draw.SetCursor(m_endP);
                    }
                }
                break;
            default:
                break;
        }

    }
    else if(opr == SUB)
    {
        if(g_curOper > 1)
            g_curOper -= 1;
        else
            g_curOper = MAX_OPT;

        switch(g_curOper)
        {
            case 1:
                //start cycle line
                if(m_startP.x != m_endP.x)
                {
                    if(m_psP.x != m_edP.x)
                    {
                        m_draw.DrawTraceTag(m_psP, true);
                        m_draw.DrawTraceTag(m_psP, false);
                    }

                    m_draw.DrawVLine(m_startP, false);
                    m_draw.DrawVLine(m_startP, true);
                    m_draw.SetCursor(m_startP);
                }
                break;
            case 2:
                //ps position
                if(m_isDraw)
                {
                    if(m_psP.x != m_edP.x)
                    {
                        m_draw.DrawTraceTag(m_edP, true);
                        m_draw.DrawTraceTag(m_edP, false);

                        m_draw.DrawTraceTag(m_psP, false);
                        m_draw.DrawTraceTag(m_psP, true);
                        m_draw.SetCursor(m_psP);
                    }
                }
                break;
            case 3:
                //ed position
                if(m_isDraw)
                {
                    if(m_psP.x != m_edP.x)
                    {
                        m_draw.DrawVLine(m_endP, true);
                        m_draw.DrawVLine(m_endP, false);

                        m_draw.DrawTraceTag(m_edP, false);
                        m_draw.DrawTraceTag(m_edP, true);
                        m_draw.SetCursor(m_edP);
                    }
                    else
                    {
                        m_draw.DrawVLine(m_endP, true);
                        m_draw.DrawVLine(m_endP, false);
                    }
                }
                break;
            case 4:
                //end cycle line
                if(m_isDraw)
                {
                    if(m_startP.x != m_endP.x)
                    {
                        m_draw.DrawVLine(m_startP, true);
                        m_draw.DrawVLine(m_startP, false);

                        m_draw.DrawVLine(m_endP, false);
                        m_draw.DrawVLine(m_endP, true);
                        m_draw.SetCursor(m_endP);
                    }
                }
                break;
            default:
                break;
        }
    }
    m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
#endif
}

void DMeasureManualAuto::Change()
{
    if((g_setFunc == 3 || g_setFunc == 0) && m_auto) //全自动切换到半自动
    {
        SemiAutoCalc();
        return;
    }
    else if (((g_setFunc == 3) && (!m_auto)) || g_setFunc == 1) //半自动切换到手动
    {
        if(g_ptrAbsMeasure != NULL)
        {
            delete g_ptrAbsMeasure;
            g_ptrAbsMeasure = NULL;
        }
        g_ptrAbsMeasure = new DMeasureManualTrack();
        return;
    }
}

void DMeasureManualAuto::Esc()
{
    PRINTF("==curOpt:%d step:%d m_isDraw:%d setFunc:%d\n", g_curOper, m_step, m_isDraw, g_setFunc);
    if(m_step == 0)
    {
        if(g_setFunc == 2)
        {
            ImageArea::GetInstance()->ClearTrace();
            switch(g_curOper)
            {
                case 1:
                if(m_startP.x == m_endP.x)
                    m_draw.DrawVLine(m_startP, true);
                else
                {
                    m_draw.DrawVLine(m_startP, true);
                    m_draw.DrawVLine(m_endP, false);
                }
                break;

                case 2:
                case 3:
                {
                    m_draw.DrawVLine(m_startP, false);
                    m_draw.DrawVLine(m_endP, false);
                }
                break;

                case 4:
                {
                    m_draw.DrawVLine(m_startP, false);
                    m_draw.DrawVLine(m_endP, true);
                }
                break;
                default:
                break;
            }
        }
        else if(g_setFunc == 1)
        {
            m_draw.DrawVLine(m_startP, true);
        }
        else if(g_setFunc == 3)
        {
          //  if(m_cursorTypeChg)
          //      m_ptrMan->ClearFirst();
                //m_ptrMan->ClearLast();
#if 0
            if(m_cursorTypeChg)
                m_update.ClearLast();
#endif
        }
    }
    else
    {
        if (m_isDraw)
        {
            if(m_startP.x != m_endP.x)
                m_draw.DrawVLine(m_startP, false);
            m_draw.DrawVLine(m_endP, true);
        }
    }

    //m_psP.x = m_edP.x = 0;
    if(ModeStatus::IsFreezeMode())
    {
        if((m_psP.x != 0) && (m_edP.x != 0))
        {
            m_ptrMan->SetManualAutoTrace(m_startP, m_endP, m_psP, m_edP);
        }
    }
    g_setFunc = 1;
    g_curOper = 1;
    m_update.ClearMeasure();
}



///integral calacute
DMeasureIntegralTrack::DMeasureIntegralTrack(const SingleItemInfo *ptrSingleItemInfo):MOUSE_INTERVAL(2)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
DMeasureIntegralTrack::~DMeasureIntegralTrack()
{
	Esc();
}
void DMeasureIntegralTrack::Init()
{
	m_step = 0;
	m_isDraw = TRUE;
	m_tempP = m_draw.DrawInitialVLine();
    m_tempP = m_draw.DrawInitialHLine();
	m_mouse_count = 0;
	m_track.clear();
}
void DMeasureIntegralTrack::PressLeft(POINT p)
{
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
                m_draw.DrawVLine(m_tempP,true);
                m_draw.DrawHDotLine(m_tempP,true);
            }
            m_p1 = m_tempP;
            m_line_tempP = m_tempP;

            m_draw.DrawVLine(m_tempP,false);
            m_draw.DrawHDotLine(m_tempP,false);
            m_draw.DrawVLine(m_tempP,true);
            m_draw.DrawHDotLine(m_tempP,true);

            m_perimeter = (double)0;
            m_integral_area = (double)0;

            m_step = 1;
            m_isDraw = TRUE;

            m_track.clear();
            m_track.push_back(m_p1);

            break;

        case 1:
            {
                if (m_isDraw)
                {
                    m_draw.DrawVLine(m_tempP,true);
                    m_draw.DrawHDotLine(m_tempP,true);
                    m_draw.DrawVLine(m_tempP, false);
                    m_draw.DrawHDotLine(m_tempP, false);
                    for(unsigned int i = (m_track.size() - 1); i>0; i--)
                    {
                        if((m_track[i-1].x==m_track[i].x)||(m_track[i-1].y==m_track[i].y)&&(m_track.size()>3)&&(i>1))
                        {
                            m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i-2], TRUE);

                        }
                        else
                            m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i-1], TRUE);
                    }
                }
                for(unsigned int i = (m_track.size() - 1); i>0; i--)
                {
                    if((m_track[i-1].x==m_track[i].x)||(m_track[i-1].y==m_track[i].y)&&(m_track.size()>3)&&(i>1))
                    {
                        m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i-2], FALSE);

                    }
                    else
                        m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i-1], FALSE);
                }

                double m_integral_area0=0.0;
                m_integral_area0= m_calc.DCalcINTEGRALTrack(m_track);

                m_ptrMan->AddNew(INTEGRAL_TRACK, m_draw.GetCursorType(), m_track, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
                double resultTmp;
                resultTmp = m_integral_area0;
                m_ptrMan->SingleMeaDataMan(resultTmp, m_itemInfo, allData, SAVE);

                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

                //begin new track length measure
                m_draw.ChangeCursorType(); // 更改鼠标类型
                Init();

                // change pointer
                ChangePointerWhenMeasure();
                break;
            }
        default:
            break;
    }
}



void DMeasureIntegralTrack::PressRight(POINT p)
{
    switch(m_step)
    {
        case 0:// clear last measure result and measure line
            m_ptrMan->ClearLast();
            m_update.ClearLast();
            break;

        case 1:
            if (m_isDraw)
            {
                m_draw.DrawVLine(m_tempP,true);
                m_draw.DrawHDotLine(m_tempP,true);

            }
            m_tempP = m_p1;
            m_draw.DrawVLine(m_tempP, false);
            m_draw.DrawHDotLine(m_tempP, false);

            m_draw.DrawVLine(m_tempP,true);
            m_draw.DrawHDotLine(m_tempP,true);
            m_draw.SetCursor(m_tempP);

            unsigned int i;
            size_t size;
            size = m_track.size();
            for(i=(size-1); i>0; i--)
            {
                if((m_track[i-1].x==m_track[i].x)||(m_track[i-1].y==m_track[i].y)&&(size>3)&&(i>1))
                {
                    m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i-2], TRUE);
                }
                else
                    m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i-1], TRUE);
            }
            m_isDraw = TRUE;
            m_step = 0;
            m_perimeter = INVALID_VAL;
            m_integral_area = INVALID_VAL;
            m_mouse_count = 0;

            //清除正在测量的结果
            m_update.ClearMeasure();
            break;

        default:
            break;
    }
}
void DMeasureIntegralTrack::MouseMove(POINT p)
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
                m_draw.DrawVLine(m_tempP,true);
                m_draw.DrawHDotLine(m_tempP,true);

            }

            m_tempP = p;
            m_draw.DrawHDotLine(m_tempP,true);
            m_draw.DrawVLine(m_tempP,true);

            m_isDraw = TRUE;
            break;

        case 1:
            if (m_isDraw)
            {

                m_draw.DrawVLine(m_tempP,true);
                m_draw.DrawHDotLine(m_tempP,true);

            }
            unsigned int i;
            size_t size;
            size = m_track.size();
            if ((p.x<m_track[size-1].x)&&(size>=2))
            {  if (size>2)
                {
                    p=m_track[size-2];
                    m_tempP = p;
                    m_draw.DrawVLine(p,true);
                    m_draw.DrawHDotLine(p,true);
                    if(((m_track[size-2].x==m_track[size-1].x)||(m_track[size-2].y==m_track[size-1].y))&&(i>3))
                    {
                        m_draw.DrawINTEGRALTraceLine(m_track[size-1], m_track[size-3], TRUE);

                    }
                    else
                        m_draw.DrawINTEGRALTraceLine(m_track[size-1], m_track[size-2], TRUE);
                    m_track.pop_back();
                    double m_integral_area1=0.0;
                    m_integral_area1= m_calc.DCalcINTEGRALTrack(m_track);

                    m_update.DIntegralArea(m_integral_area1,attr, true);
                    m_line_tempP = m_tempP;
                    m_mouse_count = 0;

                    m_isDraw = TRUE;

                }
                if(size==2){
                    p=m_track[0];
                    m_tempP = p;
                    m_draw.DrawINTEGRALTraceLine(m_track[size-1], m_track[size-2], TRUE);

                    m_track.pop_back();

                    double m_integral_area11=0.0;
                    m_update.DIntegralArea(m_integral_area11,attr, true);
                    m_line_tempP = m_tempP;

                    m_isDraw = FALSE;


                }
            }
            else
            {

                if(p.x>m_track[0].x)
                {
                    m_tempP = p;
                    m_draw.DrawVLine(p,true);
                    m_draw.DrawHDotLine(p,true);
                    if(((p.x==m_line_tempP.x)||(p.y==m_line_tempP.y))&&(size>1))
                        m_draw.DrawINTEGRALTraceLine(p, m_track[size-2], TRUE);
                    else
                        m_draw.DrawINTEGRALTraceLine(p, m_line_tempP, TRUE);
                    m_line_tempP = m_tempP;
                    m_track.push_back(m_line_tempP);
                    double m_integral_area2=0.0;
                    m_integral_area2= m_calc.DCalcINTEGRALTrack(m_track);

                    m_update.DIntegralArea(m_integral_area2,attr, true);
                  // m_line_tempP = m_tempP;
                   // m_track.push_back(m_line_tempP);
                    m_isDraw = TRUE;
                }
             /*   if(p.x==m_track[0].x)
                {
                    p.y = m_track[0].y;
                    m_tempP=p;
                    m_line_tempP = m_tempP;
                    m_isDraw = FALSE;
                    }*/
                if(p.x<=m_track[0].x)
                {
                    p = m_track[0];
                    m_tempP=p;
                    m_line_tempP = m_tempP;
                    m_isDraw = FALSE;
                }

            }
            break;

        default:
            break;
    }
}
void DMeasureIntegralTrack::Change()
{
    return;
}

void DMeasureIntegralTrack::Esc()
{
    if (m_isDraw)
    {
        m_draw.DrawHDotLine(m_tempP, true);
    }

    if (m_step == 0)
    {
        if (m_isDraw)
        {
            m_draw.DrawVLine(m_tempP,true);
        }

    }
    else
    {
        int i;
        int vec_size;

        if (m_isDraw)
        {
            m_draw.DrawVLine(m_tempP,true);
            m_draw.DrawVLine(m_p1,false);
            vec_size = m_track.size() - 1;
            for(i=0; i<vec_size; i++)
                m_draw.DrawINTEGRALTraceLine(m_track[i], m_track[i+1], TRUE);
        }
    }
    m_track.clear();
}




//========================================= [Vel] ====================================//
DMeasureVel::DMeasureVel(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureVel::~DMeasureVel()
{
	Esc();
}

void DMeasureVel::Init()
{
    m_tempP = m_draw.DrawInitialHLine();
    m_draw.DrawCursor(m_tempP);
    m_isDraw = TRUE;
}

void DMeasureVel::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	int i;
	double allData[SINGLE_MAX];

	for (i=0; i<SINGLE_MAX; i++) allData[i] = INVALID_VAL;

	// draw measure line
	if (m_isDraw)
	{
	    m_draw.DrawHDotLine(m_tempP, true);
	    m_draw.DrawCursor(m_tempP, true, XOR, true);
	}

	m_draw.DrawCursor(m_tempP, false, XOR, true);
	m_draw.DrawHDotLine(m_tempP, false);

	// add to measure man
	vector<POINT> vec;
	vec.clear();
	vec.push_back(m_tempP);
	m_ptrMan->AddNew(VEL_D, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

	m_vel = m_calc.DCalcVel(m_tempP);
	m_ptrMan->SingleMeaDataMan(m_vel, m_itemInfo, allData, SAVE);
	m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

	// begin new vel measure
	m_draw.ChangeCursorType();
	Init();

    // change pointer
    ChangePointerWhenMeasure();
}

void DMeasureVel::PressRight(POINT p)
{
#if 0
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}
			m_draw.DrawCursor(m_p1, false, XOR, true);
			m_draw.DrawHDotLine(m_p1, false);
			m_tempP = m_p1;
			m_draw.DrawCursor(m_p1, true, XOR, true);
			m_draw.DrawHDotLine(m_p1, true);
			m_draw.SetCursor(m_p1);
			m_ed = INVALID_VAL;
			m_sd = INVALID_VAL;
			m_step = 0;
			m_isDraw = TRUE;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
#endif
#if 1
	m_ptrMan->ClearLast();
	m_update.ClearLast();
#endif
}

void DMeasureVel::MouseMove(POINT p)
{
	double allData[SINGLE_MAX];
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	if (m_isDraw)
	{
	    m_draw.DrawCursor(m_tempP, true, XOR, true);
	    m_draw.DrawHDotLine(m_tempP, true);
	}

	m_tempP = p;
	m_draw.DrawCursor(p, true, XOR, true);
	m_draw.DrawHDotLine(p, true);
	m_isDraw = TRUE;

	m_vel = m_calc.DCalcVel(m_tempP);
	m_ptrMan->SingleMeaDataMan(m_vel, m_itemInfo, allData, NOT_SAVE);
	m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
}

void DMeasureVel::Esc()
{
	if (m_isDraw)
    {
        m_draw.DrawCursor(m_tempP, true, XOR, true);
        m_draw.DrawHDotLine(m_tempP, true);
        //清除正在测量的结果
        m_update.ClearMeasure();
    }
}

//========================================= PG max ====================================//
DMeasurePGmax::DMeasurePGmax(const MultiItemInfo *ptrMultiItemInfo)
{
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasurePGmax::~DMeasurePGmax()
{
	Esc();
}

void DMeasurePGmax::Init()
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	//	m_ptrMan->ClearFirst();
	m_tempP = m_draw.DrawInitialHLine();
	m_draw.DrawCursor(m_tempP);
	m_isDraw = TRUE;
}

void DMeasurePGmax::PressLeft(POINT p)
{
	double data[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	// draw measure line
	if (m_isDraw)
	{
	    m_draw.DrawCursor(m_tempP, true, XOR, true);
	    m_draw.DrawHDotLine(m_tempP, true);
	}

	m_draw.DrawCursor(m_tempP, false, XOR, true);
	m_draw.DrawHDotLine(m_tempP, false);

	// add to measure man
	vector<POINT> vec;
	vec.clear();
	vec.push_back(m_tempP);
	m_ptrMan->AddNew(VEL_D, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

	m_vel = m_calc.DCalcVel(m_tempP);
	m_PGmax = m_calc.DCalcPGmax(m_vel);
	data[0] = m_vel;
	data[1] = m_PGmax;
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	for (int i=2; i<MEA_MULTI; i++) {data[i] = INVALID_VAL; unit_coeffi[i] = 0;}
	m_ptrMan->MultiMeaDataMan(data, m_itemInfo, allData, SAVE);
	m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

	// begin new vel measure
	m_draw.ChangeCursorType();
	Init();

    // change pointer
    ChangePointerWhenMeasure();
}

void DMeasurePGmax::PressRight(POINT p)
{
	m_ptrMan->ClearLast();
	m_update.ClearLast();
}

void DMeasurePGmax::MouseMove(POINT p)
{
	double data[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	if (m_isDraw)
	{
	    m_draw.DrawCursor(m_tempP, true, XOR, true);
	    m_draw.DrawHDotLine(m_tempP, true);
	}

	m_tempP = p;
	m_draw.DrawCursor(p, true, XOR, true);
	m_draw.DrawHDotLine(p, true);
	m_isDraw = TRUE;

	m_vel = m_calc.DCalcVel(m_tempP);
	m_PGmax = m_calc.DCalcPGmax(m_vel);

	data[0] = m_vel;
	data[1] = m_PGmax;
	unit_coeffi[0] = 1;
	unit_coeffi[1] = 1;
	for (int i=2; i<MEA_MULTI; i++) {data[i] = INVALID_VAL; unit_coeffi[i] = 0;}
	m_ptrMan->MultiMeaDataMan(data, m_itemInfo, allData, NOT_SAVE);
	m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
}

void DMeasurePGmax::Esc()
{
    if (m_isDraw) {
	m_draw.DrawCursor(m_tempP, true, XOR, true);
	m_draw.DrawHDotLine(m_tempP, true);
	//清除正在测量的结果
	m_update.ClearMeasure();
    }
}

//========================================= [RI] ====================================//
//name: 软件包测量时传入该项的名称，在D模式的基本测量中传入NULL
DMeasureRI::DMeasureRI(const MultiItemInfo *ptrMultiItemInfo)
{
	m_itemInfo = ptrMultiItemInfo;
	m_item = ptrMultiItemInfo->multiItem;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureRI::~DMeasureRI()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void DMeasureRI::Init()
{
	m_step = 0;
	m_ps = INVALID_VAL;
	m_ed = INVALID_VAL;
	m_ri = INVALID_VAL;
	m_sd = INVALID_VAL;
	//	m_ptrMan->ClearFirst();
	m_tempP = m_draw.DrawInitialHLine();
	m_draw.DrawCursor(m_tempP);
	m_isDraw = TRUE;
}

void DMeasureRI::PressLeft(POINT p)
{
	vector<POINT> vec;
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
	unit_coeffi[2] = 1;
	unit_coeffi[3] = 1;
	for(i=4; i<MEA_MULTI; i++) {unit_coeffi[i] = 1;}

	switch(m_step)
	{
		case 0: //ps
			// set
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_p1 = m_tempP;
			m_draw.DrawCursor(m_tempP, false, XOR, true);
			m_draw.DrawHDotLine(m_tempP, false);

			dataMea[0] = m_ps = m_calc.DCalcVel(m_tempP);//计算ps速度
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);

			// begin new measure
			m_step = 1;
			m_tempP = m_draw.DrawInitialHLine();
			m_draw.DrawCursor(m_tempP);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			break;

		case 1: //ed
			// draw measure line
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_draw.DrawCursor(m_tempP, false, XOR, true);
			m_draw.DrawHDotLine(m_tempP, false);

			// add to measure man
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			if (m_itemInfo->meaType == PSED)
			    m_ptrMan->AddNew(PSED, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			else if (m_itemInfo->meaType == RI)
			    m_ptrMan->AddNew(RI, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			else
			    m_ptrMan->AddNew(SD, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			if(m_itemInfo->meaType == PSED)
			{
				m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
				dataMea[0] = m_ps;
				dataMea[1] = m_ed;
			}
			else if(m_itemInfo->meaType == RI)
			{
				m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
				m_ri = m_calc.DCalcRI(m_ps, m_ed);
				dataMea[0] = m_ps;
				dataMea[1] = m_ed;
				dataMea[2] = m_ri;
			}
			else
			{
				m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
				m_ri = m_calc.DCalcRI(m_ps, m_ed);
				m_sd = m_calc.DCalcSD(m_ps, m_ed);
				dataMea[0] = m_ps;
				dataMea[1] = m_ed;
				dataMea[2] = m_ri;
				dataMea[3] = m_sd;
			}
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

			m_step = 0;
			m_isDraw = TRUE;

			// begin new vel measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void DMeasureRI::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}
			m_draw.DrawCursor(m_p1, false, XOR, true);
			m_draw.DrawHDotLine(m_p1, false);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
			m_tempP = m_p1;
			m_draw.DrawCursor(m_p1, true, XOR, true);
			m_draw.DrawHDotLine(m_p1, true);
			m_draw.SetCursor(m_p1);
			m_ed = INVALID_VAL;
			m_ri = INVALID_VAL;
			m_sd = INVALID_VAL;
			m_step = 0;
			m_isDraw = TRUE;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void DMeasureRI::MouseMove(POINT p)
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
		case 0: //ps
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(p, true, XOR, true);
			m_draw.DrawHDotLine(p, true);
			m_isDraw = TRUE;

			dataMea[0] = m_ps = m_calc.DCalcVel(m_tempP);
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		case 1: //ed
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(p, true, XOR, true);
			m_draw.DrawHDotLine(p, true);
			m_isDraw = TRUE;

			//calc
			if(m_itemInfo->meaType == PSED)
			{
				m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
				dataMea[0] = m_ps;
				dataMea[1] = m_ed;
			}
			else if(m_itemInfo->meaType == RI)
			{
				m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
				m_ri = m_calc.DCalcRI(m_ps, m_ed);
				dataMea[0] = m_ps;
				dataMea[1] = m_ed;
				dataMea[2] = m_ri;
			}
			else
			{
				m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
				m_ri = m_calc.DCalcRI(m_ps, m_ed);
				m_sd = m_calc.DCalcSD(m_ps, m_ed);
				dataMea[0] = m_ps;
				dataMea[1] = m_ed;
				dataMea[2] = m_ri;
				dataMea[3] = m_sd;
			}
			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		default:
			break;
	}
}

void DMeasureRI::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
		    m_draw.DrawCursor(m_tempP, true, XOR, true);
		    m_draw.DrawHDotLine(m_tempP, true);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_p1, false, XOR, true);
			m_draw.DrawHDotLine(m_p1, false);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_draw.DrawHDotLine(m_tempP, true);
		}
	}
    m_update.ClearMeasure();
}

//========================================= [SD] ====================================//

void DMeasureSD::Init()
{
	m_step = 0;
	m_ps = INVALID_VAL;
	m_ed = INVALID_VAL;
	m_sd = INVALID_VAL;
	m_tempP = m_draw.DrawInitialHLine();
	m_draw.DrawCursor(m_tempP);
	m_isDraw = TRUE;
}

DMeasureSD::DMeasureSD()
{
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}
DMeasureSD::~DMeasureSD()
{
	Esc();
}

void DMeasureSD::PressLeft(POINT p)
{
	vector<POINT> vec;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0: //ps
			// set
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_p1 = m_tempP;
			m_draw.DrawCursor(m_tempP, false, XOR, true);
			m_draw.DrawHDotLine(m_tempP, false);

			m_ps = m_calc.DCalcVel(m_tempP);//计算ps速度
			m_update.DSD(m_ps, m_ed, m_sd, attr);

			// begin new measure
			m_step = 1;
			m_tempP = m_draw.DrawInitialHLine();
			m_draw.DrawCursor(m_tempP, true);
			m_isDraw = TRUE;

			break;

		case 1: //ed
			// draw measure line
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_draw.DrawCursor(m_tempP, false, XOR, true);
			m_draw.DrawHDotLine(m_tempP, false);

			// add to measure man
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			m_ptrMan->AddNew(SD, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			m_ed = m_calc.DCalcVel(m_tempP);//计算ed速度
			m_sd = m_calc.DCalcSD(m_ps, m_ed);
			m_update.DSD(m_ps, m_ed, m_sd, attr, false);
			m_step = 0;
			m_isDraw = TRUE;

			// begin new vel measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}

void DMeasureSD::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}
			m_draw.DrawCursor(m_p1, false, XOR, true);
			m_draw.DrawHDotLine(m_p1, false);
			m_tempP = m_p1;
			m_draw.DrawCursor(m_p1, true, XOR, true);
			m_draw.DrawHDotLine(m_p1, true);
			m_draw.SetCursor(m_p1);
			m_ed = INVALID_VAL;
			m_sd = INVALID_VAL;
			m_step = 0;
			m_isDraw = TRUE;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void DMeasureSD::MouseMove(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0: //ps
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(p, true, XOR, true);
			m_draw.DrawHDotLine(p, true);
			m_isDraw = TRUE;

			m_ps = m_calc.DCalcVel(m_tempP);
			m_update.DSD(m_ps, m_ed, m_sd, attr);
			break;

		case 1: //ed
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			    m_draw.DrawHDotLine(m_tempP, true);
			}

			m_tempP = p;
			m_draw.DrawCursor(p, true, XOR, true);
			m_draw.DrawHDotLine(p, true);
			m_isDraw = TRUE;

			//calc
			m_ed = m_calc.DCalcVel(m_tempP);
			m_sd = m_calc.DCalcSD(m_ps, m_ed);
			m_update.DSD(m_ps, m_ed, m_sd, attr);
			break;

		default:
			break;
	}

}

void DMeasureSD::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
		    m_draw.DrawCursor(m_tempP, true, XOR, true);
		    m_draw.DrawHDotLine(m_tempP, true);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawCursor(m_p1, false, XOR, true);
			m_draw.DrawHDotLine(m_p1, false);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_draw.DrawHDotLine(m_tempP, true);
		}
	}

	//清除正在测量的结果
    m_update.ClearMeasure();
}

//===============method = 0:Time
DMeasureTime::DMeasureTime(const SingleItemInfo *ptrSingleItemInfo)
{
	m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureTime::~DMeasureTime()
{
	Esc();
}

void DMeasureTime::Init()
{
	m_step = 0;
	m_tempP = m_draw.DrawInitialVLine();
	m_isDraw = TRUE;
	m_time = INVALID_VAL;
}

void DMeasureTime::PressLeft(POINT p)
{
	double allData[SINGLE_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0:
			//set
			if (m_isDraw)
			{
			    m_draw.DrawVLine(m_tempP, true);
			}
			m_p1 = m_tempP;
			m_draw.DrawVLine(m_tempP, false);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());

			// next step
			m_step = 1;
			m_draw.DrawVLine(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			{
				// draw measure line
				if (m_isDraw)
				{
					m_draw.DrawVLine(m_tempP, true);
				}
				m_draw.DrawVLine(m_tempP, false);

				// add to measure man
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_p1);
				vec.push_back(m_tempP);
				m_ptrMan->AddNew(TIME_D, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				m_time = m_calc.DCalcTime(m_p1, m_tempP);

				m_ptrMan->SingleMeaDataMan(m_time, m_itemInfo, allData, SAVE);
				m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

				// begin new time measure
				// m_draw.ChangeCursorType();
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}

void DMeasureTime::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawVLine(m_tempP, true);
			}
			m_tempP = m_p1;
			m_draw.DrawVLine(m_tempP, false);
			m_draw.DrawVLine(m_tempP, true);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
			m_isDraw = TRUE;
			m_step = 0;
			m_time = INVALID_VAL;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void DMeasureTime::MouseMove(POINT p)
{
	double allData[SINGLE_MAX];

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				m_draw.DrawVLine(m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawVLine(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				m_draw.DrawVLine(m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawVLine(m_tempP, true);
			m_isDraw = TRUE;

			// calc
			m_time = m_calc.DCalcTime(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_time, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			break;

		default:
			break;
	}
}

void DMeasureTime::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
			m_draw.DrawVLine(m_tempP, true);
			m_draw.DrawVLine(m_tempP, false);
		}
		m_draw.DrawVLine(m_p1, false);
		m_draw.DrawVLine(m_p1, true);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.SetCursor(m_tempP);
	}
}

void DMeasureTime::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
		    m_draw.DrawVLine(m_tempP, true);
		}
	}
	else
	{
		if (m_isDraw)
		{
			m_draw.DrawVLine(m_tempP, true);
			m_draw.DrawVLine(m_p1, false);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
		}
	}
	//清除正在测量的结果
    m_update.ClearMeasure();
}

//========================================= HR ====================================//
DMeasureHR::DMeasureHR(const MultiItemInfo *ptrMultiItemInfo)
{
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureHR::~DMeasureHR()
{
	Esc();
}

void DMeasureHR::Init()
{
	m_step = 0;
	m_tempP = m_draw.DrawInitialVLine();
	m_isDraw = TRUE;
	m_time = INVALID_VAL;
	m_hr = INVALID_VAL;
}

void DMeasureHR::PressLeft(POINT p)
{
	int i;
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
		case 0:
			//set
			if (m_isDraw)
			{
			    m_draw.DrawVLine(m_tempP, true);
			}
			m_p1 = m_tempP;
			m_draw.DrawVLine(m_tempP, false);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());

			// next step
			m_step = 1;
			m_draw.DrawVLine(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			{
				// draw measure line
				if (m_isDraw)
				{
				    m_draw.DrawVLine(m_tempP, true);
				}
				m_draw.DrawVLine(m_tempP, false);

				// add to measure man
				//	if ((m_p1.x != m_tempP.x) && (m_p1.y != m_tempP.y))
				//	{
				vector<POINT> vec;
				vec.clear();
				vec.push_back(m_p1);
				vec.push_back(m_tempP);

				m_ptrMan->AddNew(HR_D, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
				m_time = m_calc.DCalcTime(m_p1, m_tempP);
				m_hr = fabs(m_time) > ZERO ? m_calc.DCalcHR(m_time) : (double)INVALID_VAL;
				dataMea[0] = m_time;
				dataMea[1] = m_hr;

				m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, SAVE);
				m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);

				// begin new time measure
				Init();

                // change pointer
                ChangePointerWhenMeasure();
			}
			break;

		default:
			break;
	}
}
void DMeasureHR::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawVLine(m_tempP, true);
			}
			m_tempP = m_p1;
			m_draw.DrawVLine(m_tempP, false);
			m_draw.DrawVLine(m_tempP, true);
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

void DMeasureHR::MouseMove(POINT p)
{
	int i;
	double allData[MULTI_MAX];
	double dataMea[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];

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
		case 0:
			if (m_isDraw)
			{
			    m_draw.DrawVLine(m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawVLine(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
			    m_draw.DrawVLine(m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawVLine(m_tempP, true);
			m_isDraw = TRUE;

			// calc
			m_time = m_calc.DCalcTime(m_p1, m_tempP);
			m_hr = fabs(m_time) > ZERO ? m_calc.DCalcHR(m_time) : (double)INVALID_VAL;
	//		m_hr = m_calc.DCalcHR(m_time);
			dataMea[0] = m_time;
			dataMea[1] = m_hr;

			m_ptrMan->MultiMeaDataMan(dataMea, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
			break;

		default:
			break;
	}
}

void DMeasureHR::Change()
{
	if (m_step == 1)
	{
		if (m_isDraw)
		{
		    m_draw.DrawVLine(m_tempP, true);
		    m_draw.DrawVLine(m_tempP, false);
		}
		m_draw.DrawVLine(m_p1, false);
		m_draw.DrawVLine(m_p1);

		POINT p1;
		p1 = m_p1;
		m_p1 = m_tempP;
		m_tempP = p1;
		m_draw.SetCursor(m_tempP);
	}
}

void DMeasureHR::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
		    m_draw.DrawVLine(m_tempP, true);
		}
	}
	else
	{
		if (m_isDraw)
		{
		    m_draw.DrawVLine(m_tempP, true);
		    m_draw.DrawVLine(m_p1, false);
			m_draw.DrawOrderNumber(m_p1, m_draw.GetOrderNumber());
		}
	}
	//清除正在测量的结果
    m_update.ClearMeasure();
}


//========================================= [Accel] ====================================//
DMeasureAccel::DMeasureAccel(const SingleItemInfo *ptrSingleItemInfo)
{
	m_itemInfo = ptrSingleItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureAccel::~DMeasureAccel()
{
	Esc();
}

void DMeasureAccel::Init()
{
	m_step = 0;
	m_tempP = m_draw.DrawInitialCursor();
	m_draw.DrawVDotLine(m_tempP, true);
	m_isDraw = TRUE;
	m_result = INVALID_VAL;
}

void DMeasureAccel::PressLeft(POINT p)
{
	double allData[SINGLE_MAX];
	vector<POINT> vec;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0:
			//set
			if (m_isDraw)
			{
			    m_draw.DrawVDotLine(m_tempP, true);
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_p1 = m_tempP;
			m_draw.DrawVDotLine(m_tempP, false);
			m_draw.DrawCursor(m_tempP, false, XOR, true);

			// next step
			m_step = 1;
			m_tempP = m_draw.CalcNextP(m_tempP, 2);
			m_draw.SetCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_isDraw = TRUE;
			break;

		case 1:
			// draw measure line
			if (m_isDraw)
			{
			    m_draw.DrawLine(m_p1, m_tempP, true);
			    m_draw.DrawVDotLine(m_tempP, true);
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_draw.DrawLine(m_p1, m_tempP, false);
			m_draw.DrawVDotLine(m_tempP, false);
			m_draw.DrawCursor(m_tempP, false, XOR, true);

			// add to measure man
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			if (m_itemInfo->meaType == ACCEL)
			    m_ptrMan->AddNew(ACCEL, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			else
			    m_ptrMan->AddNew(SLOPE_D, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			m_result = m_calc.DCalcAccel(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_result, m_itemInfo, allData, SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}
void DMeasureAccel::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			//erase
			if (m_isDraw)
			{
				//		m_draw.DrawCursor(m_tempP);
			    m_draw.DrawLine(m_p1, m_tempP, true);
			    m_draw.DrawVDotLine(m_tempP, true);
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			}

			// undo
			m_tempP = m_p1;
			m_draw.DrawVDotLine(m_tempP, false);
			m_draw.DrawCursor(m_tempP, false, XOR, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			//	m_draw.DrawCursor(m_p1, FALSE);
			//	m_draw.DrawCursor(m_p1);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_result = INVALID_VAL;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void DMeasureAccel::MouseMove(POINT p)
{
	double allData[SINGLE_MAX];
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();

	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
				//		m_draw.DrawCursor(m_tempP);
				m_draw.DrawVDotLine(m_tempP, true);
				m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_tempP = p;
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				//		m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP, true);
				m_draw.DrawVDotLine(m_tempP, true);
				m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_tempP = p;
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_isDraw = TRUE;

			// calc
			m_result = m_calc.DCalcAccel(m_p1, m_tempP);
			m_ptrMan->SingleMeaDataMan(m_result, m_itemInfo, allData, NOT_SAVE);
			m_update.GenDisplaySingle(m_itemInfo, allData, attr, true, 1);
			break;

		default:
			break;
	}
}

void DMeasureAccel::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
		}
	}
	else
	{
		if (m_isDraw)
		{
			//	m_draw.DrawCursor(m_p1, FALSE);
			m_draw.DrawVDotLine(m_p1, false);
			m_draw.DrawCursor(m_p1, false, XOR, true);
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
		}
	}
	//清除正在测量的结果
    m_update.ClearMeasure();
}

//========================================= [P1/2t] ====================================//
DMeasureP12t::DMeasureP12t(const MultiItemInfo *ptrMultiItemInfo)
{
	m_itemInfo = ptrMultiItemInfo;

	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

DMeasureP12t::~DMeasureP12t()
{
	Esc();
}

void DMeasureP12t::Init()
{
	m_step = 0;
	m_tempP = m_draw.DrawInitialCursor();
//	m_draw.DrawCursor(m_tempP);
	m_draw.DrawVDotLine(m_tempP);
	m_isDraw = TRUE;
	m_Vmax = INVALID_VAL;
	m_slope = INVALID_VAL;
	m_p12t = INVALID_VAL;
}

void DMeasureP12t::PressLeft(POINT p)
{
	vector<POINT> vec;

	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();


	switch(m_step)
	{
		case 0:
			//set
			if (m_isDraw)
			{
				//	m_draw.DrawCursor(m_tempP);
				m_draw.DrawVDotLine(m_tempP, true);
				m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_p1 = m_tempP;
			//	m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawVDotLine(m_tempP, false);
			m_draw.DrawCursor(m_tempP, false, XOR, true);

			// next step
			m_step = 1;
			m_tempP = m_draw.CalcNextP(m_tempP, 2);
			m_draw.SetCursor(m_tempP);
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_isDraw = TRUE;
			break;

		case 1:
			// draw measure line
			if (m_isDraw)
			{
				//		m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP, true);
				m_draw.DrawVDotLine(m_tempP, true);
				m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			//	m_draw.DrawCursor(m_tempP, FALSE);
			m_draw.DrawLine(m_p1, m_tempP, false);
			m_draw.DrawVDotLine(m_tempP, false);
			m_draw.DrawCursor(m_tempP, false, XOR, true);

			// add to measure man
			vec.clear();
			vec.push_back(m_p1);
			vec.push_back(m_tempP);
			m_ptrMan->AddNew(P12T, m_draw.GetCursorType(), vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());

			Result(SAVE, attr);

			// begin new HR measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}
void DMeasureP12t::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			//erase
			if (m_isDraw)
			{
				//		m_draw.DrawCursor(m_tempP);
			    m_draw.DrawLine(m_p1, m_tempP, true);
			    m_draw.DrawVDotLine(m_tempP, true);
			    m_draw.DrawCursor(m_tempP, true, XOR, true);
			}

			// undo
			m_tempP = m_p1;
			m_draw.DrawVDotLine(m_tempP, false);
			m_draw.DrawCursor(m_tempP, false, XOR, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			//	m_draw.DrawCursor(m_p1, FALSE);
			//	m_draw.DrawCursor(m_p1);
			m_draw.SetCursor(m_p1);
			m_isDraw = TRUE;
			m_step = 0;
			m_Vmax = INVALID_VAL;
			m_slope = INVALID_VAL;
			m_p12t = INVALID_VAL;
			m_update.ClearMeasure();
			break;

		default:
			break;
	}
}

void DMeasureP12t::MouseMove(POINT p)
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
				//		m_draw.DrawCursor(m_tempP);
				m_draw.DrawVDotLine(m_tempP, true);
				m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_tempP = p;
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_isDraw = TRUE;
			break;

		case 1:
			if (m_isDraw)
			{
				//		m_draw.DrawCursor(m_tempP);
				m_draw.DrawLine(m_p1, m_tempP, true);
				m_draw.DrawVDotLine(m_tempP, true);
				m_draw.DrawCursor(m_tempP, true, XOR, true);
			}
			m_tempP = p;
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
			m_isDraw = TRUE;

			// calc
			Result(NOT_SAVE, attr);
			break;

		default:
			break;
	}
}

void DMeasureP12t::Esc()
{
	if (m_step == 0)
	{
		if (m_isDraw)
		{
			//	m_draw.DrawCursor(m_tempP);
		    m_draw.DrawVDotLine(m_tempP, true);
		    m_draw.DrawCursor(m_tempP, true, XOR, true);
		}
	}
	else
	{
		if (m_isDraw)
		{
			//	m_draw.DrawCursor(m_p1, FALSE);
			m_draw.DrawVDotLine(m_p1, false);
			m_draw.DrawCursor(m_p1, false, XOR, true);
			//	m_draw.DrawCursor(m_tempP);
			m_draw.DrawLine(m_p1, m_tempP, true);
			m_draw.DrawVDotLine(m_tempP, true);
			m_draw.DrawCursor(m_tempP, true, XOR, true);
		}
	}
	//清除正在测量的结果
    m_update.ClearMeasure();
}

void DMeasureP12t::Result(int saveR, UpdateMeasure::ResultAttr& attr)
{
	double data[MEA_MULTI];
	int unit_coeffi[MEA_MULTI];
	double allData[MULTI_MAX];
	double velTmp1, velTmp2;

	velTmp1 = m_calc.DCalcVel(m_p1);
	velTmp2 = m_calc.DCalcVel(m_tempP);
	if (velTmp1 < (double)0) velTmp1 = (double)0 - velTmp1;
	if (velTmp2 < (double)0) velTmp2 = (double)0 - velTmp2;
	m_Vmax = (velTmp1 > velTmp2) ? velTmp1 : velTmp2;
	m_p12t = m_calc.DCalcTime(m_p1, m_tempP);
	m_slope = m_calc.DCalcAccel(m_p1, m_tempP);
	data[0] = m_Vmax; unit_coeffi[0] = 1;
	data[1] = m_p12t; unit_coeffi[1] = 1;
	data[2] = m_slope; unit_coeffi[2] = 1;
	for (int i=3; i<MEA_MULTI; i++) {data[i] = INVALID_VAL; unit_coeffi[i] = 0;}
	m_ptrMan->MultiMeaDataMan(data, m_itemInfo, allData, saveR);
	if (saveR == SAVE)
	    m_update.GenDisplayMulti(m_itemInfo, allData, attr, false, unit_coeffi);
	else
	    m_update.GenDisplayMulti(m_itemInfo, allData, attr, true, unit_coeffi);
	return;
}

//========================================= [PI] ====================================//
/*
 *>method = 0: PI测量
 *>method = 1: PG mean测量
 * */
DMeasurePI::DMeasurePI(int method,const SingleItemInfo *ptrSingleItemInfo)
{
    m_item = ptrSingleItemInfo->item;
	m_itemInfo = ptrSingleItemInfo;

	m_method = method;
	m_ptrMan = MeasureMan::GetInstance();
	Init();
}

void DMeasurePI::Init()
{
	m_step = 0;
	m_tempP = m_draw.DrawInitialCursor();
	m_isDraw = TRUE;
	m_vec.clear();
    m_trackTemp.clear();
	m_pi = INVALID_VAL;
	m_pgmean = INVALID_VAL;
    HintArea::GetInstance()->ClearHint();
}

DMeasurePI::~DMeasurePI()
{
    HintArea::GetInstance()->ClearHint();
	Esc();
}

void DMeasurePI::PressLeft(POINT p)
{
	UpdateMeasure::ResultAttr attr;
	attr.cursorType = m_draw.GetOrderNumber();
	attr.curColor = m_draw.GetCurColor();
	attr.confirmColor = m_draw.GetConfirmColor();
    double allData[SINGLE_MAX];

	switch(m_step)
	{
		case 0:
            if(m_method == 0)
#ifdef EMP_355
                HintArea::GetInstance()->UpdateHint(_("[PI]: <Auto> to cancel or redraw."));
#else
                HintArea::GetInstance()->UpdateHint(_("[PI]: <Value> to cancel or redraw."));
#endif
			else
#ifdef EMP_355
                HintArea::GetInstance()->UpdateHint(_("[PG Mean]: <Auto> to cancel or redraw."));
#else
                HintArea::GetInstance()->UpdateHint(_("[PG Mean]: <Value> to cancel or redraw."));
#endif
				//set
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_vec.clear();
			m_vec.push_back(m_tempP);
			m_draw.DrawCursor(m_tempP, false);
            m_trackTemp.clear();

			// next step
			m_step = 1;
			m_draw.DrawCursor(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			// draw measure line
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			    for (int i = (m_vec.size()-1); i != 0; i--)
				m_draw.DrawTraceLine(m_vec[i], m_vec[i-1], true);
			}

			m_draw.DrawCursor(m_tempP, false);
			for (int i = (m_vec.size()-1); i != 0; i--)
			    m_draw.DrawTraceLine(m_vec[i], m_vec[i-1], false);

			//calc
			if (m_method == 0)
			{
				m_calc.DCalcPI(m_vec, m_psP, m_edP, m_ps, m_ed, m_tamax, m_pi);
                if(m_pi != INVALID_VAL)
                {
                    m_draw.DrawTraceTag(m_psP, FALSE);
                    m_draw.DrawTraceTag(m_edP, FALSE);
                    m_vec.push_back(m_psP);
                    m_vec.push_back(m_edP);
                    m_update.DPI(m_ps, m_ed, m_tamax, m_pi, attr, true);
                    m_ptrMan->AddNew(PI_D, m_draw.GetCursorType(), m_vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
                    //	m_update.DPI(m_ps, m_ed, m_tamax, m_pi, attr, false);
                    m_ptrMan->SingleMeaDataMan(m_pi, m_itemInfo, allData, SAVE);
                    m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);
                }
                else
                {
                    m_draw.DrawCursor(m_vec[0], false);
                    m_draw.DrawCursor(m_tempP, false);
                    for (int i=0; i<(m_vec.size()-1); i++)
                        m_draw.DrawTraceLine(m_vec[i], m_vec[i+1], false);
                }
            }
			else
			{
				m_pgmean = m_calc.DCalcPGmean(m_vec);
				m_update.DPGmean(m_pgmean, attr, true);
				m_ptrMan->AddNew(PGMEAN, m_draw.GetCursorType(), m_vec, m_draw.GetCursorSize(), m_draw.GetConfirmColor());
			//	m_update.DPGmean(m_pgmean, attr, false);
                m_ptrMan->SingleMeaDataMan(m_pgmean, m_itemInfo, allData, SAVE);
                m_update.GenDisplaySingle(m_itemInfo, allData, attr, false, 1);

			}

			// begin new HR measure
			m_draw.ChangeCursorType();
			Init();

            // change pointer
            ChangePointerWhenMeasure();
			break;

		default:
			break;
	}
}
void DMeasurePI::PressRight(POINT p)
{
	switch(m_step)
	{
		case 0:
			m_ptrMan->ClearLast();
			m_update.ClearLast();
			break;

		case 1:
			{
				int vec_size, i;
				vec_size = m_vec.size();
				if (vec_size > 1)
				{
					m_draw.DrawCursor(m_vec[0], false);
					m_draw.DrawCursor(m_tempP, true);
					for (i=0; i<(vec_size-1); i++)
					{
					    m_draw.DrawTraceLine(m_vec[i], m_vec[i+1], true);
					}
					m_tempP = m_vec[0];
					m_draw.SetCursor(m_tempP);
					m_draw.DrawCursor(m_tempP, true);
				}
				else
				{
					m_tempP = m_vec[0];
					m_draw.DrawCursor(m_vec[0], false);
					m_draw.DrawCursor(m_vec[0], true);
				}
				m_vec.clear();
                m_trackTemp.clear();
				m_step = 0;
				m_pi = INVALID_VAL;
				m_pgmean = INVALID_VAL;
				m_isDraw = TRUE;
			}
			break;

		default:
			break;
	}
}
void DMeasurePI::MouseMove(POINT p)
{
	switch(m_step)
	{
		case 0:
			if (m_isDraw)
			{
			    m_draw.DrawCursor(m_tempP, true);
			}
			m_tempP = p;
			m_draw.DrawCursor(m_tempP, true);
			m_isDraw = TRUE;
			break;

		case 1:
			{
				if (abs(p.y - m_tempP.y) + abs(p.x - m_tempP.x) < 3)
				{
					break;
				}

				if (m_isDraw)
				{
					m_draw.DrawCursor(m_tempP, true);
				}
				m_draw.DrawTraceLine(m_tempP, p, true);
				m_tempP = p;
				m_vec.push_back(m_tempP);
				m_draw.DrawCursor(m_tempP, true);

                m_trackTemp.clear();
                m_isDraw = TRUE;
			}
			break;

		default:
			break;
	}

}
void DMeasurePI::Value(EKnobOper opr)
{
    UpdateMeasure::ResultAttr attr;
    //attr.cursorType = m_draw.GetCursorType();
	attr.cursorType = m_draw.GetOrderNumber();
    attr.curColor = m_draw.GetCurColor();
    attr.confirmColor = m_draw.GetConfirmColor();
    int size_track = m_vec.size();
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
            m_draw.DrawTraceLine(m_vec[size_track -1], m_tempP, TRUE);
            m_vec.push_back(m_tempP);
            m_trackTemp.pop_back();

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
            m_tempP = m_vec[size_track - 1];
            m_trackTemp.push_back(m_tempP);
            m_vec.pop_back();
            size_track = m_vec.size();
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

        m_draw.DrawTraceLine(m_vec[size_track - 1], m_tempP, TRUE);
        m_tempP = m_vec[size_track - 1];
        m_draw.DrawCursor(m_tempP);
    }
    else
    {
        return;
    }

    m_isDraw = TRUE;
}


void DMeasurePI::Esc()
{
    if (m_step==0) {
        if (m_isDraw)
            m_draw.DrawCursor(m_tempP, true);
    } else {
        if (m_isDraw) {
            m_draw.DrawCursor(m_tempP, true);
            m_draw.DrawCursor(m_vec[0], false);
            for (int i = (m_vec.size()-1); i != 0; i--)
                m_draw.DrawTraceLine(m_vec[i], m_vec[i-1], true);

        }
    }
	//清除正在测量的结果
	m_update.ClearMeasure();
    m_vec.clear();
    m_trackTemp.clear();
}


//#include <iostream>
//using namespace std;
//========================================= [PGmean] ====================================//
// void DMeasurePGmean::Init()
// {
//     m_vec.clear();
//     m_step = 0;
//     m_tempP = m_draw.DrawInitialCursor();
//     m_isDraw = TRUE;
//     m_pg = INVALID_VAL;
// }
// DMeasurePGmean::DMeasurePGmean()
// {
// 	m_ptrMan = MeasureMan::GetInstance();
// 	Init();
// }
// DMeasurePGmean::~DMeasurePGmean()
// {
// 	Esc();
// }

// void DMeasurePGmean::PressLeft(POINT p)
// {
// 	UpdateMeasure::ResultAttr attr;
// 	attr.cursorType = m_draw.GetOrderNumber();
// 	attr.curColor = m_draw.GetCurColor();
// 	attr.confirmColor = m_draw.GetConfirmColor();

// 	switch(m_step)
// 	{
// 		case 0:
// 			//set
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 			}
// 			m_vec.push_back(m_tempP);
// 			m_draw.DrawCursor(m_tempP, FALSE);

// 			// next step
// 			m_step = 1;
// 			m_draw.DrawCursor(m_tempP);
// 			m_isDraw = TRUE;
// 			break;

// 		case 1:
// 			// draw measure line
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 			}

// 			m_draw.DrawCursor(m_tempP, FALSE);

// 			//calc
// 			m_pg = m_calc.DCalcPGmean(m_vec);
// 			m_update.DPGmean(m_pg, attr, false);
// 			// add to measure man
// 			m_ptrMan->AddNew(PGMEAN, m_draw.GetCursorType(), m_vec);

// 			// begin new HR measure
// 			Init();
// 			break;

// 		default:
// 			break;
// 	}

// }
// void DMeasurePGmean::PressRight(POINT p)
// {
// 	switch(m_step)
// 	{
// 		case 0:
// 			m_ptrMan->ClearLast();
// 			m_update.ClearLast();
// 			break;

// 		case 1:
// 			{
// 				int vec_size, i;
// 				vec_size = m_vec.size();
// 				if (vec_size > 1)
// 				{
// 					m_draw.DrawCursor(m_vec[0], false);
// 					m_draw.DrawCursor(m_tempP);
// 					for (i=0; i<(vec_size-1); i++)
// 					{
// 						m_draw.DrawLine(m_vec[i], m_vec[i+1]);
// 					}
// 					m_tempP = m_vec[0];
// 					m_draw.SetCursor(m_tempP);
// 					m_draw.DrawCursor(m_tempP);
// 				}
// 				else
// 				{
// 					m_draw.DrawCursor(m_vec[0], false);
// 					m_draw.DrawCursor(m_vec[0]);
// 				}
// 				m_vec.clear();
// 				m_step = 0;
// 				m_pg = INVALID_VAL;
// 				m_isDraw = TRUE;
// 			}
// 			break;

// 		default:
// 			break;
// 	}

// }
// void DMeasurePGmean::MouseMove(POINT p)
// {
// 	switch(m_step)
// 	{
// 		case 0:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 			}
// 			m_tempP = p;
// 			m_draw.DrawCursor(m_tempP);
// 			m_isDraw = TRUE;
// 			break;

// 		case 1:
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 			}
// 			m_draw.DrawCursor(p);
// 			m_draw.DrawLine(m_tempP, p);
// 			m_vec.push_back(m_tempP);
// 			m_tempP = p;
// 			m_isDraw = TRUE;
// 			break;

// 		default:
// 			break;
// 	}

// }

// void DMeasurePGmean::Esc()
// {
// 	POINT p;

// 	if (m_step==0)
// 	{
// 		m_draw.DrawCursor(m_tempP);
// 	}
// 	else
// 	{
// 		if (m_vec.size() > 1)
// 		{
// 			//erase
// 			m_draw.DrawCursor(m_tempP);
// 			m_draw.DrawLine(m_vec[m_vec.size() - 1], m_tempP);
// 			p = m_vec[m_vec.size() - 1];
// 			m_draw.DrawCursor(p, FALSE);
// 			m_draw.DrawCursor(p);
// 		}
// 		else
// 		{
// 			if (m_isDraw)
// 			{
// 				m_draw.DrawCursor(m_tempP);
// 				m_draw.DrawLine(m_vec[0], m_tempP);
// 			}

// 			p = m_vec[0];
// 			m_draw.DrawCursor(p, FALSE);
// 			m_draw.DrawCursor(p);
// 		}
// 	}
// }