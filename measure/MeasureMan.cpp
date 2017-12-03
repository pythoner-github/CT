#include <iostream>
#include "measure/MeasureMan.h"
#include "calcPeople/MeaCalcFun.h"
#include "measure/MeasureFactory.h"
#include "sysMan/SysMeasurementSetting.h"
#include "display/MenuArea.h"
#include "calcPeople/MenuCalcNew.h"
#include "display/ImageAreaDraw.h"
#include "measure/MenuMeasure.h"

using namespace std;

bool g_changePointerAfterMeasure = FALSE;
void SetChangePointerMeasure(void)
{
	SysMeasurementSetting sysm;
	int order = sysm.GetMeasureSequence(); //0-default, 1-repeat, 2-none

	if (order == 0)
	{
		g_changePointerAfterMeasure = FALSE;
	}
	else if (order == 1)
	{
		g_changePointerAfterMeasure = FALSE;
	}
	else
	{
		g_changePointerAfterMeasure = TRUE;
	}
}

void SetChangePointerCalc(void)
{
	SysMeasurementSetting sysm;
	int order = sysm.GetMeasureSequence(); //0-default, 1-repeat, 2-none

	if (order == 0)
	{
		g_changePointerAfterMeasure = TRUE;
	}
	else if (order == 1)
	{
		g_changePointerAfterMeasure = FALSE;
	}
	else
	{
		g_changePointerAfterMeasure = TRUE;
	}
}

void ChangePointerWhenMeasure(void)
{
	if (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC)
	{
        g_menuCalc.CompleteCurrentMeasure();
        g_menuCalc.SelectNext();
    }
    else
    {
        g_menuMeasure.CompleteCurrentMeasure();
        g_menuMeasure.SelectNext();
    }
}

MeasureMan* MeasureMan::s_ptrInstance = NULL;
MeasureMan* MeasureMan::GetInstance()
{
	if (s_ptrInstance == NULL)
		s_ptrInstance = new MeasureMan;

	return s_ptrInstance;
}

MeasureMan::~MeasureMan()
{
    if (s_ptrInstance != NULL)
		delete s_ptrInstance;
}

MeasureMan::MeasureMan()
{
	m_deq.clear();
	m_ptrMeaResult = MeaResult::GetInstance();
    m_pwAdjust = false;
    m_orderNumBak = 1;
}
void MeasureMan::AddNewAdjust(EMeaType measureType, int cursorType ,vector<POINT> vec, int cursorSize, int confirmColor)
{
	// add info
	MeasureInfo info;
	info.measureType = measureType;
	info.cursorType = cursorType;
	info.cursorSize = cursorSize;
	info.confirmColor = confirmColor;
	info.vecPoint = vec;
	//	info.step = 0;
	info.orderNumber = m_draw.GetOrderNumber();

	m_deq.push_back(info);

	m_draw.DrawOrderNumber(vec[vec.size()-1], info.orderNumber);
	m_draw.ChangeOrderNumber();
}

void MeasureMan::AddNew(EMeaType measureType, int cursorType ,vector<POINT> vec, int cursorSize, int confirmColor)
{
    ClearFirst();

	// add info
	MeasureInfo info;
	info.measureType = measureType;
	info.cursorType = cursorType;
	info.cursorSize = cursorSize;
	info.confirmColor = confirmColor;
	info.vecPoint = vec;
	//	info.step = 0;
	info.orderNumber = m_draw.GetOrderNumber();

	m_deq.push_back(info);

	m_draw.DrawOrderNumber(vec[vec.size()-1], info.orderNumber);
	m_draw.ChangeOrderNumber();
}

using namespace std;

void MeasureMan::ClearPwTraceUnFreeze()
{
    int size = m_deq.size();
    vector<MeasureInfo> vecInfo;
    int k;
    bool flag = false;
    //clear trace
    if(size > 0)
    {
        for(int i = 0; i <= size - 1; i++)
        {
            if(m_deq[i].measureType == MEASURE_TRACK_AUTO || m_deq[i].measureType == MEASURE_TRACK)
            {
                EraseMeasureLine(m_deq[i]);
                //printf("curType:%d\n", m_deq[i].cursorType);
                ImageArea::GetInstance()->ClearTrace();
                k = i;
                flag = true;
                break;
            }
        }
        if(flag)
        {
            //clear trace result
            m_update.ClearPwResultUnFreeze(m_deq[k].cursorType);

            for(int i = size - 1; i > k; i--)
            {
                vecInfo.push_back(m_deq[i]);
                m_deq.pop_back();
            }

            if(m_deq.size() > 0)
            {
                m_deq.pop_back();
            }

            for(int i = vecInfo.size() - 1; i >= 0; i--)
            {
                m_deq.push_back(vecInfo[i]);
            }

            m_psP.x = m_edP.x = 0;
            ImageAreaDraw::GetInstance()->SetPsEdValue(m_psP, m_edP, 1);
        }
        else
        {
            if(m_pwAdjust)
            {
                m_update.ClearPwResultUnFreeze(m_orderNumBak);
                m_pwAdjust = false;
            }
        }
    }
    else if(size == 0)
    {
        if(m_pwAdjust)
            m_update.ClearPwResultUnFreeze(m_orderNumBak);
        m_pwAdjust = false;
    }
}

void MeasureMan::ClearLast()
{
	int size = m_deq.size();
    //printf("last size:%d  %d\n", size, m_deq[size - 1].orderNumber);

	if (size > 0)
	{
		// erase measure line
		EraseMeasureLine(m_deq[size - 1]);
		m_draw.SetOrderNumber(m_deq[size - 1].orderNumber);

        m_orderNumBak = m_deq[size - 1].orderNumber;
        // erase data
		m_deq.pop_back();
		if (m_deq.size() == 0)
        {
			m_draw.SetOrderNumber(1);
        }
	}
}

void MeasureMan::ClearFirst()
{
	int size = m_deq.size();
	int lineNum;

	lineNum = m_update.GetResultCount();
    //printf("size:%d  lineNum:%d\n", size, lineNum);

	while (size-- > lineNum)
	{
		// erase measure line
		EraseMeasureLine(m_deq[0]);

		// erase data
		m_deq.pop_front();
	}
}

void MeasureMan::ClearAll()
{
	int size = m_deq.size();

	if (size > 0)
	{
		// clear all measure info
		deque<MeasureInfo>::iterator iter;
		for (iter = m_deq.begin(); iter < m_deq.end(); iter ++)
		{
			EraseMeasureLine(*iter);
		}
		//m_update.ClearAll();

        // erase data
        m_deq.clear();
        m_draw.SetOrderNumber(1);
	}
}

int MeasureMan::GetDequeSize()
{
	return m_deq.size();
}

void MeasureMan::DeleteAllForClearScreen(bool update)
{
    m_psP.x = m_edP.x = 0;
	ClearAllMeasureOrderNumber();
	m_deq.clear();
    m_update.ClearAll(update);
    m_draw.SetOrderNumber(1);
}

void MeasureMan::SetManualAutoTrace(POINT start, POINT end, POINT ps, POINT ed, int curOpt)
{
    m_startP = start;
    m_endP = end;
    m_psP = ps;
    m_edP = ed;
    ImageAreaDraw::GetInstance()->SetPsEdValue(m_psP, m_edP, curOpt);
}

void MeasureMan::ReDrawManualAutoStartLine(bool draw)
{
    vector<POINT> vec;
    ClearLast();
    m_draw.DrawVLine(m_startP, draw);
    m_draw.DrawVLine(m_endP, false);
    if(!vec.empty())
        vec.clear();
    vec = Replay::GetInstance()->GetTraceData(m_startP.x, m_endP.x);
    m_draw.PwTrace(vec, COPY);
    m_draw.DrawTraceTag(m_psP, false, XOR, true);
    m_draw.DrawTraceTag(m_edP, false, XOR, true);

    m_pwAdjust = true;
    m_draw.SetCursor(m_startP);
}

void MeasureMan::ManualAutoAdjust(int current, bool draw)
{
    PRINTF("current:%d\n", current);
    switch(current)
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
                m_draw.DrawVLine(m_startP, true);
                m_draw.DrawVLine(m_startP, false);

                m_draw.DrawTraceTag(m_psP, false, XOR, true);
                m_draw.DrawTraceTag(m_psP, true, XOR, true);
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
            if(m_psP.x != m_edP.x)
            {
                m_draw.DrawTraceTag(m_psP, true, XOR, true);
                m_draw.DrawTraceTag(m_psP, false, XOR, true);

                m_draw.DrawTraceTag(m_edP, false, XOR, true);
                m_draw.DrawTraceTag(m_edP, true, XOR, true);
                m_draw.SetCursor(m_edP);
            }
            break;
        case 4:
            //end cycle line
            if(m_startP.x != m_endP.x)
            {
                if(m_psP.x != m_edP.x)
                {
                    m_draw.DrawTraceTag(m_edP, true, XOR, true);
                    m_draw.DrawTraceTag(m_edP, false, XOR, true);
                }
                m_draw.DrawVLine(m_endP, false);
                m_draw.DrawVLine(m_endP, true);
                m_draw.SetCursor(m_endP);
            }
            break;
        default:
            break;
    }
}

///> private
void MeasureMan::EraseMeasureLine(MeasureInfo info)
{
	int cursorTypeBak = m_draw.GetCursorType();
	int cursorSizeBak = m_draw.GetCursorSize();
	int confirmColorBak = m_draw.GetConfirmColor();

	switch(info.measureType)
	{
		case DIST_DOT:
		case DEPTH_DIST:
		case DEPTH_DIST_M:
			info.meaType = DIST_DOT;
			EraseDist(info);
			break;

		case DIST_LINE:
			info.meaType = DIST_LINE;
			EraseDist2Line(info);
			break;

		case LENGTH_TRACK:
			info.meaType = LENGTH_TRACK;
			EraseTrack(info);
			break;

		case LENGTH_DOT:
			info.meaType = LENGTH_DOT;
			EraseDot(info);
			break;

        case AREA_TRACK:
            info.meaType = AREA_TRACK;
            EraseTrack(info);
            break;

        case INTEGRAL_TRACK:
            info.meaType = INTEGRAL_TRACK;
            EraseTime(info);
            EraseVelD(info);
            EraseINTEGRALTrack(info);
            break;

        case AREA_DOT:
            info.meaType = AREA_DOT;
			EraseDot(info);
			break;

		case AREA_REC:
			info.meaType = AREA_REC;
			EraseRectangle(info);
			break;

        case VOL_3AXIS:
        case SAC_3AXIS:
			info.meaType = VOL_3AXIS;
			EraseVol3Axis(info);
			break;

		case SIMPSONS:
			info.meaType = SIMPSONS;
			EraseSimpson(info);
			break;

		case AL:
			info.meaType = AL;
			EraseAL(info);
			break;

		case AREA_ELLIPSE:
		case VOL_ELLIPSE1:
		case PERI_ELLIPSE:
			info.meaType = VOL_ELLIPSE1;
			EraseEllipse(info);
			break;

		case VOL_ELLIPSE2:
			info.meaType = VOL_ELLIPSE2;
			EraseVolEllipse2(info);
			break;

		case VOL_SPHEROID:
			info.meaType = VOL_SPHEROID;
			EraseDist(info);
			break;

		case ANGLE_3DOT:
			info.meaType = ANGLE_3DOT;
			EraseAngleDot(info);
			break;

		case ANGLE_2LINE:
			info.meaType = ANGLE_2LINE;
			EraseAngleLine(info);
			break;

		case RATIO_DIST_DOT:
			info.meaType = RATIO_DIST_DOT;
			EraseRatioDistDot(info);
			break;

		case RATIO_D_P://距离周长比
			info.meaType = RATIO_D_P;
			EraseRatioDistPeri(info);
			break;

		case RATIO_AREA:
			info.meaType = RATIO_AREA;
			EraseRatioArea(info);
			break;

		case RATIO_VOL:
			info.meaType = RATIO_VOL;
			EraseRatioVol(info);
			break;

		case RATIO_ANGLE:
			info.meaType = RATIO_ANGLE;
			EraseRatioAngle(info);
			break;

		case HIP:
			info.meaType = HIP;
			EraseHip(info);
			break;

		case EF:
			info.meaType = EF;
			EraseEF(info);
			break;

		case SLOPE:
			info.meaType = SLOPE;
			EraseDist(info);
			break;

		case DIST_SLOPE_M:
			info.meaType = DIST_SLOPE_M;
			EraseDist(info);
			break;

		case VEL_M://M模式
			info.meaType = VEL_M;
			EraseDist(info);
			break;

		case HR_M:
		case TIME_M:
			info.meaType = HR_M;
			EraseTime(info);//心率测量和时间测量形式一样
			break;

		case VEL_D:
			info.meaType = VEL_D;
			EraseVelD(info);
			break;

		case TIME_D:
			info.meaType = TIME_D;
			EraseTime(info);
			break;

		case RI:
			info.meaType = RI;
			EraseRI(info);
			break;

		case SD:
			info.meaType = SD;
			EraseRI(info);
			break;

		case PSED:
			info.meaType = SD;
			EraseRI(info);
			break;

		case PI_D:
			info.meaType = PI_D;
			EraseTrace(info);
			break;

		case PGMEAN:
			info.meaType = PI_D;
			//	EraseTrace(info);
			ErasePGmean(info);
			break;

		case MEASURE_TRACK:
			info.meaType = MEASURE_TRACK;
			EraseTrace(info);
			//	EraseTrack(info);
			break;

		case MEASURE_TRACK_AUTO:
			info.meaType = MEASURE_TRACK_AUTO;
			EraseTrace(info);
            break;

		case ACCEL:
			info.meaType = ACCEL;
			EraseAccel(info);
            break;

		case SLOPE_D:
			info.meaType = SLOPE_D;
			EraseAccel(info);
			break;

		case P12T:
			info.meaType = P12T;
			EraseAccel(info);
			break;

		case HR_D:
			info.meaType = HR_D;
			EraseTime(info);
			break;

		default:
			break;
	}
	EraseMeasureOrderNumber(info);

	m_draw.SetCursorType(cursorTypeBak);
	m_draw.SetCursorSize(cursorSizeBak);
	m_draw.SetConfirmColor(confirmColorBak);
}

void MeasureMan::EraseDist(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 2) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);
}

void MeasureMan::EraseDist2Line(MeasureInfo info)//两线法测距离
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 2) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawLine(p1, p2, FALSE);

	double tan;
	tan = (p1.y == p2.y) ? 1000.0 : ((double)(p1.x - p2.x) / (double)(p2.y - p1.y));

	m_draw.DrawTanBeeline(p1, tan);
	m_draw.DrawTanBeeline(p2, tan);
}

void MeasureMan::EraseINTEGRALTrack(MeasureInfo info)
{
	unsigned int vec_size, i;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	POINT p1, p2;
	vec_size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (vec_size < 2) )
		return;

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[vec_size - 1];

	// clear measure line
	//m_draw.DrawCursor(p1, FALSE);
	//	m_draw.DrawCursor(p2, FALSE);

	for(i=0; i!=vec_size-1; i++)
	{
if((info.vecPoint[i].x==info.vecPoint[i+1].x)||(info.vecPoint[i].y==info.vecPoint[i+1].y)&&(i>0))
                {
                    m_draw.DrawINTEGRALTraceLine(info.vecPoint[i-1], info.vecPoint[i+1], FALSE);

                }
                else
                    m_draw.DrawINTEGRALTraceLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);

	//	m_draw.DrawTraceLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);
	}
}

void MeasureMan::EraseTrack(MeasureInfo info)
{
	unsigned int vec_size, i;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	POINT p1, p2;
	vec_size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (vec_size < 2) )
		return;

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[vec_size - 1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	if(info.meaType == LENGTH_TRACK)
		m_draw.DrawCursor(p2, FALSE);

	for(i=0; i!=vec_size-1; i++)
	{
		m_draw.DrawTraceLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);
	}

	if(info.meaType == AREA_TRACK)
		m_draw.DrawTraceLine(info.vecPoint[0], info.vecPoint[vec_size - 1], FALSE);
}

void MeasureMan::EraseSimpson(MeasureInfo info)
{
	unsigned int track_size, i;
	const unsigned int diamNum = 20;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	POINT p1, p2;
	track_size = info.vecPoint.size() - diamNum * 2 -1;
	if ( (info.vecPoint.empty()) || (track_size < 2) )
		return;

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[track_size - 1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);

	for(i=0; i!=track_size-1; i++)
	{
		m_draw.DrawLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);
	}

	m_draw.DrawLine(info.vecPoint[0], info.vecPoint[track_size - 1], FALSE);

	p1.x = (info.vecPoint[0].x + info.vecPoint[track_size - 1].x) / 2;
	p1.y = (info.vecPoint[0].y + info.vecPoint[track_size - 1].y) / 2;
	m_draw.DrawLine(p1, info.vecPoint[info.vecPoint.size() - 1], FALSE);
	m_draw.DrawCursor(info.vecPoint[info.vecPoint.size() - 1], FALSE);

	for (i=0; i<diamNum; i++)
	{
		m_draw.DrawLine(info.vecPoint[track_size + i*2], info.vecPoint[track_size + i*2 + 1], FALSE);
	}

	return;
}

void MeasureMan::EraseAL(MeasureInfo info)
{
	unsigned int track_size, i;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	POINT p1, p2;
	track_size = info.vecPoint.size() - 1;
	if ( (info.vecPoint.empty()) || (track_size < 2) )
		return;

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[track_size - 1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);

	for(i=0; i!=track_size-1; i++)
	{
		m_draw.DrawLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);
	}

	m_draw.DrawLine(info.vecPoint[0], info.vecPoint[track_size - 1], FALSE);

	p1.x = (info.vecPoint[0].x + info.vecPoint[track_size - 1].x) / 2;
	p1.y = (info.vecPoint[0].y + info.vecPoint[track_size - 1].y) / 2;
	m_draw.DrawLine(p1, info.vecPoint[info.vecPoint.size() - 1], FALSE);
	m_draw.DrawCursor(info.vecPoint[info.vecPoint.size() - 1], FALSE);

	return;
}

/*
   void MeasureMan::EraseSimpsonDiam(vector<POINT> track)
   {
   int i;
   const int diamNum = 20;
   double diamCenterX, diamCenterY;
   double lenSlope;//LV Length线的斜率
   double diamSlope;//直径的斜率
   int trackSize;
   int tempPosi1, tempPosi2;
   int diamSquSum = 0;
   POINT lenStart, lenEnd;

   trackSize = track.size() - 1;

   tempPosi1 = track[trackSize].x;
   tempPosi2 = track[trackSize].x;

   lenStart.x = (track[0].x + track[trackSize - 1].x) / 2;
   lenStart.y = (track[0].y + track[trackSize - 1].y) / 2;
   lenEnd = track[track[trackSize].x];

   diamSlope = (double)(lenStart.x - lenEnd.x) / (double)(lenEnd.y - lenStart.y);

   double tempX1, tempY1, tempX2, tempY2;
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
tmpFront = track[tempPosi1];
tempX1 = (double)tmpFront.x;
tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
tmpBeh = (tempPosi1 == 0) ? lenStart : track[tempPosi1 - 1];
tempX2 = (double)tmpBeh.x;
tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);
while((tempY1 - tmpFront.y) * (tempY2 - tmpBeh.y) > 0)
{
if (tempPosi1 > 0) tempPosi1--;
tmpFront = track[tempPosi1];
tempX1 = (double)tmpFront.x;
tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
tmpBeh = (tempPosi1 == 0) ? lenStart : track[tempPosi1 - 1];
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
tmpFront = track[tempPosi2];
tempX1 = (double)tmpFront.x;
tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
tmpBeh = (tempPosi2 == trackSize - 1) ? lenStart : track[tempPosi2 + 1];
tempX2 = (double)tmpBeh.x;
tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);
while((tempY1 - tmpFront.y) * (tempY2 - tmpBeh.y) > 0)
{
	if (tempPosi1 < trackSize - 1) tempPosi2++;
	tmpFront = track[tempPosi2];
	tempX1 = (double)tmpFront.x;
	tempY1 = diamCenterY + diamSlope * (tempX1 - diamCenterX);
	tmpBeh = (tempPosi2 == trackSize - 1) ? lenStart : track[tempPosi2 + 1];
	tempX2 = (double)tmpBeh.x;
	tempY2 = diamCenterY + diamSlope * (tempX2 - diamCenterX);

	if(tempPosi2 == trackSize - 1)
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

m_draw.DrawLine(diamEP1, diamEP2, FALSE);
}
}
*/

void MeasureMan::EraseDot(MeasureInfo info)
{
	unsigned int vec_size, i;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	vec_size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (vec_size < 2) )
		return;

	for(i=0; i!=vec_size-1; i++)
	{
		m_draw.DrawLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);
	}

	m_draw.DrawCursor(info.vecPoint[0], FALSE);
	if(info.meaType == LENGTH_DOT)
		m_draw.DrawCursor(info.vecPoint[vec_size - 1], FALSE);
	if(info.meaType == AREA_DOT)
	{
		m_draw.DrawLine(info.vecPoint[0], info.vecPoint[vec_size - 1], FALSE);
	}
}

void MeasureMan::EraseRectangle(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 2) )
		return;

	POINT p11, p12, p21, p22;
	p11 = info.vecPoint[0];
	p22 = info.vecPoint[1];
	p12.x = info.vecPoint[0].x;
	p12.y = info.vecPoint[1].y;
	p21.x = info.vecPoint[1].x;
	p21.y = info.vecPoint[0].y;
	// clear measure line
	m_draw.DrawCursor(p11, FALSE);
	m_draw.DrawCursor(p22, FALSE);
	m_draw.DrawLine(p11, p12, FALSE);
	m_draw.DrawLine(p11, p21, FALSE);
	m_draw.DrawLine(p12, p22, FALSE);
	m_draw.DrawLine(p21, p22, FALSE);
}

void MeasureMan::EraseEllipse(MeasureInfo info)
{
	POINT p1, p2, p3;
	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 3) )
		return;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawEllipse(p1, p2, p3, false);
}

void MeasureMan::EraseAngleDot(MeasureInfo info)
{
	POINT p1, p2, p3;

	if (info.vecPoint.size() != 3)
		return;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];

	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawCursor(p3, FALSE);
	m_draw.DrawLine(p1, p2);
	m_draw.DrawLine(p2, p3);
}

void MeasureMan::EraseAngleLine(MeasureInfo info)
{
	POINT p1, p2, p3;

	if (info.vecPoint.size() != 3)
		return;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];

	m_draw.DrawDotBeeline(p1, p2);
	m_draw.DrawDotBeeline(p2, p3);
	m_draw.DrawAngleArc(p2, p1, p3, false);
}

void MeasureMan::EraseVol3Axis(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 6) )
		return;

	POINT p1, p2;

	p1 = info.vecPoint[4];
	p2 = info.vecPoint[5];
	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);

	p1 = info.vecPoint[2];
	p2 = info.vecPoint[3];
	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);

	return;
}

void MeasureMan::EraseVolEllipse2(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 5) )
		return;

	POINT p1, p2, p3;

	p1 = info.vecPoint[3];
	p2 = info.vecPoint[4];
	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawEllipse(p1, p2, p3);
	return;
}

void MeasureMan::EraseRatioDistDot(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 4) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);

	p1 = info.vecPoint[2];
	p2 = info.vecPoint[3];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);
}

void MeasureMan::EraseRatioDistPeri(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 5) )
		return;

	POINT p1, p2, p3;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawDotLine(p1, p2, FALSE);

	p1 = info.vecPoint[2];
	p2 = info.vecPoint[3];
	p3 = info.vecPoint[4];
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawEllipse(p1, p2, p3);
}

void MeasureMan::EraseRatioArea(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 6) )
		return;

	POINT p1, p2, p3;

	p1 = info.vecPoint[3];
	p2 = info.vecPoint[4];
	p3 = info.vecPoint[5];
	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawEllipse(p1, p2, p3);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawEllipse(p1, p2, p3);
	return;
}

void MeasureMan::EraseRatioVol(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 6) )
		return;

	POINT p1, p2, p3;

	p1 = info.vecPoint[3];
	p2 = info.vecPoint[4];
	p3 = info.vecPoint[5];
	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawEllipse(p1, p2, p3);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawEllipse(p1, p2, p3);
	return;
}

void MeasureMan::EraseRatioAngle(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 6) )
		return;

	POINT p1, p2, p3;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	p3 = info.vecPoint[2];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawCursor(p3, FALSE);
	m_draw.DrawLine(p1, p2, FALSE);
	m_draw.DrawLine(p2, p3, FALSE);

	p1 = info.vecPoint[3];
	p2 = info.vecPoint[4];
	p3 = info.vecPoint[5];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawCursor(p3, FALSE);
	m_draw.DrawLine(p1, p2, FALSE);
	m_draw.DrawLine(p2, p3, FALSE);
}

void MeasureMan::EraseHip(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 6) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
	m_draw.DrawLine(p1, p2, FALSE);
	PRINTF("%s-%s: clear line (%d,%d) to (%d,%d)\n", __FILE__, __FUNCTION__, p1.x, p1.y, p2.x, p2.y);
	p1 = info.vecPoint[2];
	p2 = info.vecPoint[3];
	m_draw.DrawLine(p1, p2, FALSE);
	PRINTF("%s-%s: clear line (%d,%d) to (%d,%d)\n", __FILE__, __FUNCTION__, p1.x, p1.y, p2.x, p2.y);
	p1 = info.vecPoint[4];
	p2 = info.vecPoint[5];
	m_draw.DrawLine(p1, p2, FALSE);
	PRINTF("%s-%s: clear line (%d,%d) to (%d,%d)\n", __FILE__, __FUNCTION__, p1.x, p1.y, p2.x, p2.y);
}

void MeasureMan::EraseEF(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 4) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawLine(p1, p2, FALSE);

	p1 = info.vecPoint[2];
	p2 = info.vecPoint[3];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);
	m_draw.DrawLine(p1, p2, FALSE);
}

void MeasureMan::EraseTime(MeasureInfo info)
{
	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 2) )
		return;

	POINT p1, p2;

	if(info.meaType == INTEGRAL_TRACK)
    {
    	p1 = info.vecPoint[size-1];
	p2 = info.vecPoint[0];

    }
    else
    {
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];
    }
	m_draw.DrawVLine(p1, FALSE);
	m_draw.DrawVLine(p2, FALSE);
}

void MeasureMan::EraseVelD(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

    if(info.vecPoint.empty())
        return;

    int size = info.vecPoint.size();
    if((info.meaType == INTEGRAL_TRACK) && (size < 2))
        return;

	POINT p1, p2;

    if(info.meaType == INTEGRAL_TRACK)
    {
        p1 = info.vecPoint[size-1];
        p2 = info.vecPoint[0];
        //	m_draw.DrawCursor(p1, FALSE, XOR, true);
        m_draw.DrawHDotLine(p1, FALSE);
        m_draw.DrawHDotLine(p2, FALSE);

    }
    else
    {
        p1 = info.vecPoint[0];
        m_draw.DrawCursor(p1, FALSE, XOR, true);
        m_draw.DrawHDotLine(p1, FALSE);
    }
}

void MeasureMan::EraseRI(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 2) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	m_draw.DrawCursor(p1, FALSE, XOR, true);
	m_draw.DrawHDotLine(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE, XOR, true);
	m_draw.DrawHDotLine(p2, FALSE);
}

void MeasureMan::EraseAccel(MeasureInfo info)
{
	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	int size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (size < 2) )
		return;

	POINT p1, p2;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[1];

	//	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawLine(p1, p2);
	m_draw.DrawVDotLine(p1, false);
	m_draw.DrawCursor(p1, false, XOR, true);
	m_draw.DrawVDotLine(p2, FALSE);
	m_draw.DrawCursor(p2, false, XOR, true);
}

void MeasureMan::ErasePGmean(MeasureInfo info)
{
	unsigned int vec_size, i;

	m_draw.SetCursorType(info.cursorType);
	m_draw.SetCursorSize(info.cursorSize);
	m_draw.SetConfirmColor(info.confirmColor);

	POINT p1, p2;
	vec_size = info.vecPoint.size();
	if ( (info.vecPoint.empty()) || (vec_size < 2) )
		return;
	p1 = info.vecPoint[0];
	p2 = info.vecPoint[vec_size - 1];

	// clear measure line
	m_draw.DrawCursor(p1, FALSE);
	m_draw.DrawCursor(p2, FALSE);

	for(i=0; i!=vec_size-1; i++)
	{
		m_draw.DrawTraceLine(info.vecPoint[i], info.vecPoint[i+1], FALSE);
	}
	return;
}

void MeasureMan::EraseTrace(MeasureInfo info)
{
	unsigned int vec_size;
	POINT psP, edP;

    //m_draw.SetCursorType(info.cursorType);
    m_draw.SetCursorSize(info.cursorSize);
    m_draw.SetConfirmColor(info.confirmColor);

	if (info.vecPoint.empty())
		return;

	POINT p1, p2;
	vec_size = info.vecPoint.size();

	if (vec_size > 2)
		vec_size = vec_size - 2;//除去ps和ed占的长度
	else
		return;

	if (vec_size < 2)//至少有两个端点
		return;

    PRINTF("vec_size= %d\n", vec_size);

	p1 = info.vecPoint[0];
	p2 = info.vecPoint[vec_size - 1];
	psP = info.vecPoint[vec_size];
	edP = info.vecPoint[vec_size + 1];

	// clear measure line
	if ((info.meaType == MEASURE_TRACK) || (info.meaType == PI_D))//非半自动描迹
	{
        m_draw.SetCursorType(m_draw.GetCursorType());
        m_draw.DrawCursor(p1, FALSE);
		m_draw.DrawCursor(p2, FALSE);
	}
	else//半自动描迹
    {
        m_draw.SetCursorType(info.cursorType);
        m_draw.DrawVLine(p1, FALSE);
        m_draw.DrawVLine(p2, FALSE);
    }

	vector<POINT> vec;
	size_t i;
	if (info.meaType == MEASURE_TRACK_AUTO)
	{
		for (i = 1; i < vec_size-1; ++i) {//提取原始的描迹（把端点分离处理）
			vec.push_back(info.vecPoint[i]);
		}
		m_draw.PwTrace(vec, XOR);

		if (vec.size() < 2)//前面计算时，vec长度不小于2时才会计算和画ps和ed的标志，故此处小于2也不画DrawTraceTag
			return;
	}
	else
	{
		vec.push_back(info.vecPoint[0]);
		for (i = 1; i < vec_size; ++i) {
			vec.push_back(info.vecPoint[i]);
			m_draw.DrawTraceLine(vec[i-1], vec[i], FALSE);
		}
	}

    m_draw.DrawTraceTag(psP, FALSE);
	m_draw.DrawTraceTag(edP, FALSE);

	return;
}

//保存软件包测量结果
//item:本测量项的枚举号
//void MeasureMan::SetMeasureResult(const float data1, const float data2, const float data3, const float data4, const int item, const int valueNums)
void MeasureMan::SetMeasureResult(double value[MEA_MULTI], const int item, const int valueNums)
{
	m_ptrMeaResult->SetLastValue(value, item, valueNums);
}

const char* MeasureMan::GetMeasureTitle(int item)
{
    if((item>=BASIC_MEA_START) &&(item<BASIC_MEA_END))
    {
        return BasicInfo[item-BASIC_MEA_START].title;
    }
    else if ((item>=ABD_MEA_START) && (item<ABD_MEA_END))
	{
		return AbdoInfo[item-ABD_MEA_START].title;
	}
	else if ((item>=UR_MEA_START) && (item<UR_MEA_END))
	{
		return URInfo[item-UR_MEA_START].title;
	}
	else if ((item>=ADULT_MEA_START) && (item<ADULT_MEA_END))
	{
		return AdultInfo[item-ADULT_MEA_START].title;
	}
	else if ((item>=FETAL_MEA_START) && (item<FETAL_MEA_END))
	{
		return FetalInfo[item-FETAL_MEA_START].title;
	}
	else if ((item>=SP_MEA_START) && (item<SP_MEA_END))
	{
		return SPInfo[item-SP_MEA_START].title;
	}
	else if ((item>=VS_MEA_START) && (item<VS_MEA_END))
	{
		return VSInfo[item-VS_MEA_START].title;
	}
	else if ((item>=TCD_MEA_START) && (item<TCD_MEA_END))
	{
		return TCDInfo[item-TCD_MEA_START].title;
	}
	else if ((item>=ORTHO_MEA_START) && (item<ORTHO_MEA_END))
	{
		return OrthoInfo[item-ORTHO_MEA_START].title;
	}
#ifdef VET
//添加 动物超 TD
//hlx 12.20
	else if ((item>=TD_MEA_START) && (item<TD_MEA_END))
	{
		return TDInfo[item-TD_MEA_START].title;
	}
//添加 动物超 产科
//hlx 12.23
	else if ((item>=ANOB_MEA_START) && (item<ANOB_MEA_END))
	{
		return AnOBInfo[item-ANOB_MEA_START].title;
	}
#else
    else if ((item>=OB_MEA_START) && (item<OB_MEA_END))
	{
		return OBInfo[item-OB_MEA_START].title;
	}
    else if ((item>=GYN_MEA_START) && (item<GYN_MEA_END))
	{
		return GYNInfo[item-GYN_MEA_START].title;
	}
    else if((item>=EFW_MEA_START) && (item<EFW_MEA_END))
    {
		return EFWInfo[item-EFW_MEA_START].title;
    }
#endif
    else if (item >=USER_START)
    {
        return userDef_new[item-USER_START].title;
    }
	return NULL;
}

void MeasureMan::GetMeasureString(int item, const SingleItemInfo **ptrSingleItemInfo, const MultiItemInfo **ptrMultiItemInfo)
{
	const SingleItemInfo *ptrSingleItemStart;
	const MultiItemInfo *ptrMultiItemStart;
	int itemStart, itemMultiStart;

	if ((item>=ABD_MEA_START) && (item<ABD_MEA_END))
	{
		itemStart = (int)ABD_MEA_START;
		itemMultiStart = (int)ABD_MULTI_START;
		ptrSingleItemStart = AbdoInfo;
		ptrMultiItemStart = AbdoMultiInfo;
	}
	else if ((item>=UR_MEA_START) && (item<UR_MEA_END))
	{
		itemStart = (int)UR_MEA_START;
		itemMultiStart = (int)UR_MULTI_START;
		ptrSingleItemStart = URInfo;
		ptrMultiItemStart = URMultiInfo;
	}
	else if ((item>=ADULT_MEA_START) && (item<ADULT_MEA_END))
	{
		itemStart = (int)ADULT_MEA_START;
		itemMultiStart = (int)ADULT_MULTI_START;
		ptrSingleItemStart = AdultInfo;
		ptrMultiItemStart = AdultMultiInfo;
	}
	else if ((item>=FETAL_MEA_START) && (item<FETAL_MEA_END))
	{
		itemStart = (int)FETAL_MEA_START;
		itemMultiStart = (int)FETAL_MULTI_START;
		ptrSingleItemStart = FetalInfo;
		ptrMultiItemStart = FetalMultiInfo;
	}
	else if ((item>=GYN_MEA_START) && (item<GYN_MEA_END))
	{
		itemStart = (int)GYN_MEA_START;
		itemMultiStart = (int)GYN_MULTI_START;
		ptrSingleItemStart = GYNInfo;
		ptrMultiItemStart = GYNMultiInfo;
	}
	else if ((item>=SP_MEA_START) && (item<SP_MEA_END))
	{
		itemStart = (int)SP_MEA_START;
		itemMultiStart = (int)SP_MULTI_START;
		ptrSingleItemStart = SPInfo;
		ptrMultiItemStart = SPMultiInfo;
	}
	else if ((item>=VS_MEA_START) && (item<VS_MEA_END))
	{
		itemStart = (int)VS_MEA_START;
		itemMultiStart = (int)VS_MULTI_START;
		ptrSingleItemStart = VSInfo;
		ptrMultiItemStart = VSMultiInfo;
	}
	else if ((item>=TCD_MEA_START) && (item<TCD_MEA_END))
	{
		itemStart = (int)TCD_MEA_START;
		itemMultiStart = (int)TCD_MULTI_START;
		ptrSingleItemStart = TCDInfo;
		ptrMultiItemStart = TCDMultiInfo;
	}
	else if ((item>=ORTHO_MEA_START) && (item<ORTHO_MEA_END))
	{
		itemStart = (int)ORTHO_MEA_START;
		itemMultiStart = (int)ORTHO_MULTI_START;
		ptrSingleItemStart = OrthoInfo;
		ptrMultiItemStart = OrthoMultiInfo;
	}
#ifdef VET
	else if ((item>=TD_MEA_START)&&(item<TD_MEA_END))
	{
		itemStart = (int)TD_MEA_START;
		itemMultiStart = (int)TD_MULTI_START;
		ptrSingleItemStart = TDInfo;
		ptrMultiItemStart = TDMultiInfo;
	}
	else if ((item>=ANOB_MEA_START)&&(item<ANOB_MEA_END))
	{
		itemStart = (int)ANOB_MEA_START;
		itemMultiStart = (int)ANOB_MULTI_START;
		ptrSingleItemStart = AnOBInfo;
        ptrMultiItemStart = AnOBMultiInfo;
    }
#else
    else if ((item>=OB_MEA_START) && (item<OB_MEA_END))
    {
        itemStart = (int)OB_MEA_START;
        itemMultiStart = (int)OB_MULTI_START;
        ptrSingleItemStart = OBInfo;
        ptrMultiItemStart = OBMultiInfo;
    }

#endif
    else if ((item>= USER_START) && (item< USER_END ))
	{
       		itemStart =(int)USER_START;
	//	itemMultiStart = (int)ORTHO_MULTI_START;
		ptrSingleItemStart = userDef_new;
	//	ptrMultiItemStart = OrthoMultiInfo;
	}
	else
	{
		itemStart = 0;
		itemMultiStart = 0;
		*ptrSingleItemInfo = NULL;
		*ptrMultiItemInfo = NULL;
		return;
	}

	*ptrSingleItemInfo = ptrSingleItemStart + (item - itemStart);
	if ((*ptrSingleItemInfo)->unitItem > UNIT_END)
		*ptrMultiItemInfo = ptrMultiItemStart + ((*ptrSingleItemInfo)->unitItem - itemMultiStart);
	else
		*ptrMultiItemInfo = NULL;

    return;
}

void MeasureMan::ClearAllValue(void)
{
    m_ptrMeaResult->ClearAllValue();
}

void MeasureMan::GetMeasureUnit(double& coeffi, string& units, int unit)
{
    SysMeasurementSetting sysMeasure;
    if(unit == CM)
    {
        int unit = sysMeasure.GetUnitDist();
        if(unit)
        {
            units = CustomUnitArray[0].unit2;
            coeffi = 10.0;
        }
        else
        {
            units = CustomUnitArray[0].unit1;
            coeffi = 1.0;
        }
    }
    else if(unit == CM2)
    {
        int unit = sysMeasure.GetUnitArea();
        if(unit)
        {
            units =  CustomUnitArray[3].unit2;
            coeffi = 100.0;
        }
        else
        {
            units = CustomUnitArray[3].unit1;
            coeffi = 1.0;
        }
    }
    else if(unit == CM3)
    {
        int unit = sysMeasure.GetUnitVol();
        if(unit)
        {
            units = CustomUnitArray[4].unit2;
            coeffi = 1000.0;
        }
        else
        {
            units = CustomUnitArray[4].unit1;
            coeffi = 1.0;
        }
    }
    else if(unit ==SEC)
    {
        int unit = sysMeasure.GetUnitTime();
        if(unit)
        {
            units = CustomUnitArray[5].unit2;
            coeffi = 1000.0;
        }
        else
        {
            units = CustomUnitArray[5].unit1;
            coeffi = 1.0;
        }
    }
    else if(unit == CMS)
    {
        int unit = sysMeasure.GetUnitVel();
        if(unit == 0)
        {
            units = CustomUnitArray[1].unit1;
            coeffi = 1.0;
        }
        else if(unit == 1)
        {
            units = CustomUnitArray[1].unit2;
            coeffi = 10.0;
        }
        else
        {
            units = CustomUnitArray[1].unit3;
            coeffi = 0.01;
        }
    }
    else if(unit == CMS2)
    {
        int unit = sysMeasure.GetUnitAccel();
        if(unit == 0)
        {
            units = CustomUnitArray[2].unit1;
            coeffi = 1.0;
        }
        else if(unit == 1)
        {
            units = CustomUnitArray[2].unit2;
            coeffi = 10.0;
        }
        else
        {
            units = CustomUnitArray[2].unit1;
            coeffi = 0.01;
        }
    }
    else if(unit == KG)
    {
        int unit = sysMeasure.GetUnitEfw();
        if(unit)
        {
            units = CustomUnitArray[7].unit2;
            coeffi = 1000.0;
        }
        else
        {
            units = CustomUnitArray[7].unit1;
            coeffi = 1.0;
        }
    }
    else
    {
        units = CustomUnitArray[unit].unit1;
        coeffi = 1.0;
    }
}

#include <stdio.h>

void MeasureMan::SingleMeaDataMan(const double meaData, const SingleItemInfo *itemInfo, double allData[SINGLE_MAX], const int saveR)
{
	double value[MEA_MULTI];
	int i, j;
	CalcInfoTmp calcInfo;
	const CalcInfoArray *multiMethInfo;
	//	const CalcMethodInfo *subMultiMeInfo;
	if (saveR == SAVE)
	{
		value[0] = meaData;
		for (i=1; i<MEA_MULTI; i++) value[i] = (double)INVALID_VAL;
		SetMeasureResult(value, itemInfo->item, MEA_SINGLE);//科别测量保存测量结果
	}

	multiMethInfo = (CalcInfoArray *)(itemInfo->ptrCalcInfo);
	for(i=0; i<SINGLE_MAX; i++) { allData[i] = (double)INVALID_VAL; }
	allData[0] = meaData;

	if (multiMethInfo == NULL)
		return;

    i = 0;
	int calcPosi = MEA_SINGLE;//结果在allData中存放的位置，其值将在calcFun中自动增加
	while ((*multiMethInfo)[i] != NULL)
	{
		if (calcPosi >= SINGLE_MAX )
			break;
		calcInfo.item = (*multiMethInfo)[i]->item;
		for (j=0; j<CALC_PAR_NUMS; j++) calcInfo.pars[j] = (*multiMethInfo)[i]->pars[j];
		calcInfo.ptrHCalcInfo = (*multiMethInfo)[i]->ptrHCalcInfo;
		(*multiMethInfo)[i]->calcFun(&calcInfo, allData, itemInfo->item, saveR, &calcPosi, 0);
		i++;
	}

	return;
}

void MeasureMan::MultiMeaDataMan(double meaData[MEA_MULTI], const MultiItemInfo *itemInfo, double allData[MULTI_MAX], const int saveR)
{
	int i, j;
	CalcInfoTmp calcInfo;
	const CalcInfoArray *multiMethInfo;

	if (saveR == SAVE)
	{
		SetMeasureResult(meaData, itemInfo->multiItem, MEA_MULTI);//科别测量保存测量结果
	}

	multiMethInfo = (CalcInfoArray *)itemInfo->ptrCalcInfo;
	for(i=0; i<MEA_MULTI; i++) { allData[i] = meaData[i]; }
	for(i=MEA_MULTI; i<MULTI_MAX; i++) { allData[i] = INVALID_VAL; }

	if (multiMethInfo == NULL)
		return;

	i = 0;
	int calcPosi = MEA_MULTI;//结果在allData中存放的位置，其值将在calcFun中自动增加
	while ((*multiMethInfo)[i] != NULL)
	{
		if (calcPosi >= MULTI_MAX)
			break;
		calcInfo.item = (*multiMethInfo)[i]->item;
		for (j=0; j<CALC_PAR_NUMS; j++) calcInfo.pars[j] = (*multiMethInfo)[i]->pars[j];
		calcInfo.ptrHCalcInfo = (*multiMethInfo)[i]->ptrHCalcInfo;

		(*multiMethInfo)[i]->calcFun(&calcInfo, allData, itemInfo->item, saveR, &calcPosi, 0);
		i++;
	}
	return;
}

void MeasureMan::EraseMeasureOrderNumber(MeasureInfo info)
{
	int confirmColorBak = m_draw.GetConfirmColor();

	switch(info.measureType)
	{
		case DIST_DOT:
		case DEPTH_DIST:
		case DEPTH_DIST_M:
		case DIST_LINE:
		case LENGTH_TRACK:
		case AREA_TRACK:
		case LENGTH_DOT:
        case AREA_DOT:
        case INTEGRAL_TRACK:
		case AREA_REC:
		case SIMPSONS:
		case AL:
		case AREA_ELLIPSE:
		case VOL_ELLIPSE1:
		case PERI_ELLIPSE:
		case VOL_SPHEROID:
		case ANGLE_3DOT:
		case ANGLE_2LINE:
		case SLOPE:
		case DIST_SLOPE_M:
		case VEL_M://M模式
		case VEL_D:
		case PI_D:
		case MEASURE_TRACK:
		case MEASURE_TRACK_AUTO:
		case PGMEAN:
		case ACCEL:
		case SLOPE_D:
		case P12T:
			m_draw.DrawOrderNumberForErase(info.vecPoint[info.vecPoint.size() - 1], info.orderNumber, info.confirmColor);
			break;

		case VOL_3AXIS:
		case HIP:
			m_draw.DrawOrderNumberForErase(info.vecPoint[1], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[3], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[5], info.orderNumber, info.confirmColor);
			break;

		case VOL_ELLIPSE2:
			m_draw.DrawOrderNumberForErase(info.vecPoint[4], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[2], info.orderNumber, info.confirmColor);
			break;

		case RATIO_DIST_DOT:
		case EF:
			m_draw.DrawOrderNumberForErase(info.vecPoint[1], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[3], info.orderNumber, info.confirmColor);
			break;

		case RATIO_D_P://距离周长比
			m_draw.DrawOrderNumberForErase(info.vecPoint[1], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[4], info.orderNumber, info.confirmColor);
			break;

		case RATIO_AREA:
		case RATIO_VOL:
		case RATIO_ANGLE:
			m_draw.DrawOrderNumberForErase(info.vecPoint[2], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[5], info.orderNumber, info.confirmColor);
			break;

		case HR_M:
		case TIME_M:
		case RI:
		case TIME_D:
		case SD:
		case PSED:
		case HR_D:
			m_draw.DrawOrderNumberForErase(info.vecPoint[0], info.orderNumber, info.confirmColor);
			m_draw.DrawOrderNumberForErase(info.vecPoint[info.vecPoint.size() - 1], info.orderNumber, info.confirmColor);
			break;

		default:
			break;
	}

	m_draw.SetConfirmColor(confirmColorBak);
}

void MeasureMan::ClearAllMeasureOrderNumber()
{
	int size = m_deq.size();

	if (size > 0)
	{
		deque<MeasureInfo>::iterator iter;
		for (iter = m_deq.begin(); iter < m_deq.end(); iter ++)
		{
			EraseMeasureOrderNumber(*iter);
		}
	}
}