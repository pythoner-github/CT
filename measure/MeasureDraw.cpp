/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: MeasureDraw.cpp
 * @brief: Draw graphic used when measuring 
 * 
 * version: V1.0
 * date: 2009-7-9
 * @author: zhanglei
 */
#include <stdlib.h>
#include "MeasureDraw.h"
#include "ViewMain.h"
#include "../sysMan/SysMeasurementSetting.h"
#include <stdlib.h>
#include <math.h>
#include "../display/ImageAreaDraw.h"
#include "SysGeneralSetting.h"

POINT MeasureDraw::m_cursor;
int MeasureDraw::m_cursorType = 0;
int MeasureDraw::m_orderNumber = 1;

MeasureDraw::MeasureDraw()
{
    m_ptrImgArea = ImageArea::GetInstance();
    // m_cursor.x = IMAGE_W/2;
    // m_cursor.y = IMAGE_Y + IMAGE_H/2;

    m_imgAttr.area = ImageArea::PIXMAP;
    m_imgAttr.mode.gdkMode = GDK_XOR;

    SysMeasurementSetting sysMeasure;
    m_cursorSize = sysMeasure.GetMeasureCursorSize();
    m_colorCurIndex = sysMeasure.GetMeasureColorCur();
    m_colorConfirmIndex = sysMeasure.GetMeasureColorConfirm();
}

MeasureDraw::~MeasureDraw()
{
}

void MeasureDraw::DrawCursor(POINT p, bool isCur, EDrawMode mode, bool restrict)
{
//	printf("%s: x=%d, y=%d, isCur=%d, Mode=%d, restrict=%d\n", __FUNCTION__, p.x, p.y, isCur, mode, restrict);
    if (restrict) {
        int boundUp, boundDown, boundLeft, boundRight;
        ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);
    
        if (p.x < boundLeft) p.x = boundLeft;
        if (p.x > boundRight) p.x = boundRight;
        if (p.y < boundUp) p.y = boundUp;
        if (p.y > boundDown) p.y = boundDown;
    }

    GdkColor *color;

	if (isCur) {
		color = MeasureColorConvert(m_colorCurIndex);
		if (mode == XOR)
			m_ptrImgArea->DrawMeaCursor(GDK_XOR, m_cursorType, m_cursorSize, color, p.x, p.y);
		else if (mode == COPY)
			m_ptrImgArea->DrawMeaCursor(GDK_COPY, m_cursorType, m_cursorSize, color, p.x, p.y);
	}
	else {
		color = MeasureColorConvert(m_colorConfirmIndex);
		if (mode == XOR)
			m_ptrImgArea->DrawMeaCursor(GDK_XOR, m_cursorType, m_cursorSize, color, p.x, p.y);
		else if (mode == COPY)
			m_ptrImgArea->DrawMeaCursor(GDK_COPY, m_cursorType, m_cursorSize, color, p.x, p.y);
	}
	POINT tmp;
	tmp.x = p.x;
	tmp.y = p.y;
	SetCursor(tmp);
}


void MeasureDraw::ChangeCursorType(void)
{
    //if (m_cursorType == 3 || m_cursorType > 3)
	m_cursorType = 0;
    //else
	//++m_cursorType;
}

POINT MeasureDraw::DrawInitialCursor(void)
{
	CalcMeasureCenter();

	// draw cursor at point m_cursor 
	DrawCursor(m_cursor, true, XOR);

	// hide system cursor
	InvisibleCursor(TRUE); //lhm 2012.07.04

	return m_cursor;
}
/*
 *Function: 测量过程中为下一个光标预定一个位置
 *method = 0:普通测量定下一个光标
 *method = 1:深度方向距离测量定下一个光标
 *method = 2:M或PW模式用竖直线的测量定下一个光标
*/
POINT MeasureDraw::CalcNextP(POINT p1, int method)
{
    	POINT p2;
	
	if (method == 0)
	{
		p2.x = p1.x;
		p2.y = p1.y;
	}
	else if (method == 1)
	{
		p2.x = p1.x;
		p2.y = p1.y + 30;
	}
	else if (method == 2)
	{
		p2.x = p1.x + 50; //50
		p2.y = p1.y + 20;
	}
	else
	{
	    	p2 = p1;
	}

	return p2;
}

void MeasureDraw::DrawPoint(POINT p, bool isCur, EDrawMode mode)
{
//	ConvertPoint(p);
    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawPoint(m_imgAttr, color, p.x, p.y);
	} else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawPoint(m_imgAttr, color, p.x, p.y);
	}
    } else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawPoint(m_imgAttr, color, p.x, p.y);
	} else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawPoint(m_imgAttr, color, p.x, p.y);
	}
    }
}

/*
 * @brief draw cursor in point p
 *
 * @para isCur 
 * TRUE: draw current cursor
 * FALSE: draw not current cursor
 */

void MeasureDraw::DrawBaseLine(POINT p1, POINT p2, bool isCur, EDrawMode mode)
{
    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y);
	} else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y);
	}
    } else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y);
	} else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y);
	}
    }
}

void MeasureDraw::DrawDotLine(POINT p1, POINT p2, bool isCur, EDrawMode mode)
{
	SysMeasurementSetting sysMeasure;
    if(sysMeasure.GetMeasureLineDisplay())
    	return;

	GdkColor *color;
	if (isCur) {
		color = MeasureColorConvert(m_colorCurIndex);
		if (mode == XOR) {
			m_imgAttr.mode.gdkMode = GDK_XOR;
			m_ptrImgArea->DrawDashLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y, true, 10);
		} else if (mode == COPY) {
			m_imgAttr.mode.gdkMode = GDK_COPY;
			m_ptrImgArea->DrawDashLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y, true, 10);
		}
	} else {
		color = MeasureColorConvert(m_colorConfirmIndex);
		if (mode == XOR) {
			m_imgAttr.mode.gdkMode = GDK_XOR;
			m_ptrImgArea->DrawDashLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y, true, 10);
		} else if (mode == COPY) {
			m_imgAttr.mode.gdkMode = GDK_COPY;
			m_ptrImgArea->DrawDashLine(m_imgAttr, color, p1.x, p1.y, p2.x, p2.y, true, 10);
		}
	}
}

void MeasureDraw::DrawTraceLine(POINT p1, POINT p2, bool isCur, EDrawMode mode)
{
    DrawBaseLine(p1, p2, isCur, mode);
}
void MeasureDraw::DrawINTEGRALTraceLine(POINT p1, POINT p2, bool isCur, EDrawMode mode)
{

 int boundUp, boundDown, boundLeft, boundRight;
    ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);

    if (p1.y < boundUp) p1.y = (boundUp);
    if (p1.y > boundDown) p1.y = (boundDown-2);
    if (p2.y < boundUp) p2.y = (boundUp);
    if (p2.y > boundDown) p2.y = (boundDown-2);


    DrawBaseLine(p1, p2, isCur, mode);
}
void MeasureDraw::DrawLine(POINT p1, POINT p2, bool isCur, EDrawMode mode)
{
    SysMeasurementSetting sysMeasure;
    if(sysMeasure.GetMeasureLineDisplay())
    	return;

    DrawBaseLine(p1, p2, isCur, mode);
}

//用一个点和角度的tan确定的直线
void MeasureDraw::DrawTanBeeline(POINT point1, double tan, bool isCur, EDrawMode mode)
{
	POINT endp1, endp2;
	double temp;
	const int bou1_x = 0, bou1_y = 0, bou2_x = IMAGE_W, bou2_y = IMAGE_H;

	//	ConvertPoint(point1);

	if(tan == 0)//水平
	{
		endp1.x = bou1_x;
		endp2.x = bou2_x;
		endp1.y = point1.y;
		endp2.y = point1.y;
	}
	else if((tan > 999)||(tan < -999))//竖直
	{
		endp1.y = bou1_y;
		endp2.y = bou2_y;
		endp1.x = point1.x;
		endp2.x = point1.x;
	}
	else//斜线
	{
		temp = (double)point1.y + ((double)(bou2_x-point1.x)) * tan;
		if ((temp >= (double)bou1_y) && (temp<=(double)bou2_y))
		{
			endp1.x = bou2_x;
			endp1.y = (int)temp;
		}
		else
		{
			if(temp < (double)bou1_y)
			{
				endp1.x = point1.x + (int)((double)(bou1_y-point1.y) / tan);
				endp1.y = bou1_y;
			}
			else	
			{  
				endp1.x = point1.x + (int)((double)(bou2_y-point1.y) / tan);
				endp1.y = bou2_y;
			}
		}

		temp = (double)point1.y - ((double)(point1.x - bou1_x)) * tan;
		if ((temp >= (double)bou1_y) && (temp <= (double)bou2_y))
		{
			endp2.x = bou1_x;
			endp2.y = (int)temp;
		}
		else
		{
			if(temp < (double)bou1_y)
			{
				endp2.x = point1.x - (int)((double)(point1.y-bou1_y) / tan);
				endp2.y = bou1_y;
			}
			else
			{
				endp2.x = point1.x - (int)((double)(point1.y-bou2_y) / tan);
				endp2.y = bou2_y;
			}
		}
	}

	GdkColor *color;
	if (isCur)
	{
		color = MeasureColorConvert(m_colorCurIndex);
		if (mode == XOR) {
			m_imgAttr.mode.gdkMode = GDK_XOR;
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		} else if (mode == COPY) {
			m_imgAttr.mode.gdkMode = GDK_COPY;	
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		}
	}
	else
	{
		color = MeasureColorConvert(m_colorConfirmIndex);
		if (mode == XOR) {
			m_imgAttr.mode.gdkMode = GDK_XOR;
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		} else if (mode == COPY) {
			m_imgAttr.mode.gdkMode = GDK_COPY;
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		}
	}
}

//用一个点和角度的确定的线段
vector<POINT> MeasureDraw::DrawTanLineSegment(POINT point1, double theta, int length, bool isCur, EDrawMode mode)
{
	vector<POINT> vec;
	POINT endp1, endp2;
	double tanTheta = tan(theta);
	const int bou1_x = 0, bou1_y = 0, bou2_x = IMAGE_W, bou2_y = IMAGE_H-1;
	const int half_len = length/2;

//	printf("%s-%s: cursor (%d, %d)\n", __FILE__, __FUNCTION__, point1.x, point1.y);
	vec.clear();
	if(tanTheta == 0)//水平
	{
//		printf("%s-%s: H\n", __FILE__, __FUNCTION__);
		endp1.x = ((point1.x - half_len) > bou1_x) ? (point1.x - half_len) : bou1_x;
		endp2.x = ((point1.x + half_len) < bou2_x) ? (point1.x + half_len) : bou2_x;
		endp1.y = point1.y;
		endp2.y = point1.y;
//		printf("p1 (%d, %d), p2 (%d, %d)\n", endp1.x, endp1.y, endp2.x, endp2.y);
	}
	else if(tanTheta > 999 || tanTheta < -999)//竖直
	{
//		printf("%s-%s: V\n", __FILE__, __FUNCTION__);
		endp1.y = ((point1.y - half_len) > bou1_y) ? (point1.y - half_len) : bou1_y;
		endp2.y = ((point1.y + half_len) < bou2_y) ? (point1.y + half_len) : bou2_y;
		endp1.x = point1.x;
		endp2.x = point1.x;
//		printf("p1 (%d, %d), p2 (%d, %d)\n", endp1.x, endp1.y, endp2.x, endp2.y);
	}
	else//斜线
	{
//		printf("%s-%s: O\n", __FILE__, __FUNCTION__);
		double sinTheta = sin(theta);
		double cosTheta = cos(theta);
		endp1.x = point1.x + cosTheta * half_len;
		endp2.x = point1.x - cosTheta * half_len;
		endp1.y = point1.y - sinTheta * half_len;
		endp2.y = point1.y + sinTheta * half_len;
	}
	vec.push_back(endp1);
	vec.push_back(endp2);

	GdkColor *color;
	if (isCur)
	{
		color = MeasureColorConvert(m_colorCurIndex);
		if (mode == XOR) {
			m_imgAttr.mode.gdkMode = GDK_XOR;
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		} else if (mode == COPY) {
			m_imgAttr.mode.gdkMode = GDK_COPY;	
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		}
	}
	else
	{
		color = MeasureColorConvert(m_colorConfirmIndex);
		if (mode == XOR) {
			m_imgAttr.mode.gdkMode = GDK_XOR;
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		} else if (mode == COPY) {
			m_imgAttr.mode.gdkMode = GDK_COPY;
			m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
		}
	}

	return vec;
}

//用两个点确定的直线
void MeasureDraw::DrawDotBeeline(POINT point1, POINT point2, bool isCur, EDrawMode mode)
{
	POINT endp1, endp2;
	double tan;
	double temp;
	const int bou1_x = 0, bou1_y = 0, bou2_x = IMAGE_W, bou2_y = IMAGE_H;
	
	if(point1.y == point2.y)//水平
	{
        endp1.x = bou1_x;
        endp2.x = bou2_x;
        endp1.y = point1.y;
        endp2.y = point1.y;
	}
	else if(point1.x == point2.x)//竖直
	{
        endp1.y = bou1_y;
        endp2.y = bou2_y;
        endp1.x = point1.x;
        endp2.x = point1.x;
	}
	else//斜线
	{
        tan = ((double)(point2.y - point1.y)) / ((double)(point2.x - point1.x));
        temp = (double)point1.y + ((double)(bou2_x-point1.x)) * tan;
        if ((temp >= (double)bou1_y) && (temp<=(double)bou2_y))
        {
            endp1.x = bou2_x;
            endp1.y = (int)temp;
        }
        else
        {
            if(temp < (double)bou1_y)
            {
                endp1.x = point1.x + (int)((double)(bou1_y-point1.y) / tan);
                endp1.y = bou1_y;
            }
            else	
            {  
                endp1.x = point1.x + (int)((double)(bou2_y-point1.y) / tan);
                endp1.y = bou2_y;
            }
        }

        temp = (double)point1.y - ((double)(point1.x - bou1_x)) * tan;
        if ((temp >= (double)bou1_y) && (temp <= (double)bou2_y))
        {
            endp2.x = bou1_x;
            endp2.y = (int)temp;
        }
        else
        {
            if(temp < (double)bou1_y)
            {
                endp2.x = point1.x - (int)((double)(point1.y-bou1_y) / tan);
                endp2.y = bou1_y;
            }
            else
            {
                endp2.x = point1.x - (int)((double)(point1.y-bou2_y) / tan);
                endp2.y = bou2_y;
            }
        }
	}
    //printf("*******endp1 endp2 %d %d %d %d\n",endp1.x, endp1.y, endp2.x, endp2.y);

	GdkColor *color;
	if (isCur)
	{
	    color = MeasureColorConvert(m_colorCurIndex);
	    if (mode == XOR) {
		m_imgAttr.mode.gdkMode = GDK_XOR;
		m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
	    } else if (mode == COPY) {
		m_imgAttr.mode.gdkMode = GDK_COPY;
		m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
	    }
	}
	else
	{
	    color = MeasureColorConvert(m_colorConfirmIndex);
	    if (mode == XOR) {
		m_imgAttr.mode.gdkMode = GDK_XOR;
		m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
	    } else if (mode == COPY) {
		m_imgAttr.mode.gdkMode = GDK_COPY;
		m_ptrImgArea->DrawLine(m_imgAttr, color, endp1.x, endp1.y, endp2.x, endp2.y);
	    }
	}
}


/*
 * @brief draw horizontal line 
 */
void MeasureDraw::DrawHLine(POINT p, bool isCur, EDrawMode mode)
{
//    ConvertPoint(p);
    int boundUp, boundDown, boundLeft, boundRight;
    ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);

    if (p.y < boundUp) p.y = boundUp;
    if (p.y > boundDown) p.y = boundDown;

    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
    }
    else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
    }
}

/*
 * @brief draw vetical line 
 */
void MeasureDraw::DrawVLine(POINT p, bool isCur, EDrawMode mode)
{
//    ConvertPoint(p);

    int boundUp, boundDown, boundLeft, boundRight;
    ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);

    if (p.x < boundLeft) p.x = boundLeft;
    if (p.x > boundRight) p.x = boundRight;

    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
    }
    else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
    }
    POINT tmp;
    tmp.x = p.x;
    tmp.y = p.y;
    SetCursor(tmp);
}
void MeasureDraw::DrawCrossLine(POINT p, bool isCur, EDrawMode mode)
{
//    ConvertPoint(p);
    
    int boundUp, boundDown, boundLeft, boundRight;
    ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);
    
    if (p.x < boundLeft) p.x = boundLeft;
    if (p.x > boundRight) p.x = boundRight;
    if (p.y < boundUp) p.y = boundUp;
    if (p.y > boundDown) p.y = boundDown;

    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
    }
    else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	    m_ptrImgArea->DrawLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
    }

    POINT tmp;
    tmp.x = p.x;
    tmp.y = p.y;
    SetCursor(tmp);
}

void MeasureDraw::DrawHDotLine(POINT p, bool isCur, EDrawMode mode)
{
//    ConvertPoint(p);
    int boundUp, boundDown, boundLeft, boundRight;
    ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);

    if (p.y < boundUp) p.y = (boundUp);
    if (p.y > boundDown) p.y = (boundDown-2);

    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
    }
    else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, boundLeft, p.y, boundRight, p.y);
	}
    }
    POINT tmp;
    tmp.x = p.x;
    tmp.y = p.y;
    SetCursor(tmp);
}

void MeasureDraw::DrawVDotLine(POINT p, bool isCur, EDrawMode mode)
{
//    ConvertPoint(p);
    int boundUp, boundDown, boundLeft, boundRight;
    ImageAreaDraw::GetInstance()->GetBoundary(boundUp, boundDown, boundLeft, boundRight);

    GdkColor *color;
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
    }
    else {
	color = MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR) {
	    m_imgAttr.mode.gdkMode = GDK_XOR;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
	else if (mode == COPY) {
	    m_imgAttr.mode.gdkMode = GDK_COPY;
	    m_ptrImgArea->DrawDashLine(m_imgAttr, color, p.x, boundUp, p.x, boundDown);
	}
    }

    POINT tmp;
    tmp.x = p.x;
    tmp.y = p.y;
    SetCursor(tmp);
}
/*
 * @brief draw initial horizontal line and initial cursor
 */
POINT MeasureDraw::DrawInitialHLine(bool isCur)
{
	CalcMeasureCenter();

	// draw cursor at point m_cursor 
	DrawHDotLine(m_cursor, true, XOR);

	// hide system cursor
	//InvisibleCursor(TRUE);

	return m_cursor;
}

/*
 * @brief draw initial vertical line and initial cursor
 */
POINT MeasureDraw::DrawInitialVLine(bool isCur)
{
	CalcMeasureCenter();

	// draw cursor at point m_cursor 
	DrawVLine(m_cursor, true, XOR);

	// hide system cursor
	//InvisibleCursor(TRUE);

	return m_cursor;
}


POINT MeasureDraw::DrawInitialCrossLine(bool isCur)
{
	CalcMeasureCenter();

	// draw cursor at point m_cursor 
	DrawCrossLine(m_cursor, true, XOR);

	// hide system cursor
	InvisibleCursor(TRUE);

	return m_cursor;	
}

void MeasureDraw::DrawEllipse(POINT p1, POINT p2, POINT p3, bool isCur)
{
    int x1 = p1.x, x2 = p2.x, x3 = p3.x, y1 = p1.y, y2 = p2.y, y3 = p3.y;
    float r,r1,r2,r12,r22,i,j,k,l,length1;
    int x0, y0, x, y, xmax, t;
    float xcos, xsin, ycos, ysin, temp1, temp2, temp3, temp4;
    float tn;

    x0 = (x1+x2)/2; //椭圆圆心(x0, y0)
    y0 = (y1+y2)/2;

    i = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2); //计算长轴半径 r1
    length1= sqrt(i);
    r1 = length1/2;

    j = (x3-x0)*(x3-x0)+(y3-y0)*(y3-y0); //计算短轴半径 r2
    r2 = sqrt(j);

    x = 0; y = r2;
    r12 = r1*r1;
    r22 = r2*r2;
    k = r12+r22;
    xmax = r12/sqrt(k);
    tn = r12-2*r2*r12;

    temp1 = (x1-x2)/length1;
    temp2 = (y1-y2)/length1;
    temp3 = (x1-x2)/length1;
    temp4 = (y1-y2)/length1;

    GdkColor *color;
    if (isCur)
	color = MeasureColorConvert(m_colorCurIndex);
    else
	color = MeasureColorConvert(m_colorConfirmIndex);

    GdkGC *gc = m_ptrImgArea->NewMeasureGC(color, GDK_XOR);

    while (x <= xmax)
    {
	if ( tn<0||y==0 )
	    tn += (4*x+2)*r22;
	else
	{
	    tn += (4*x+2)*r22+(1-y)*4*r12;
	    y--;
	}

	t=x%6;
	if(t==1)t=1;
	else t = 0;

	xcos = x*temp1;
	xsin = x*temp2;
	ycos = y*temp3;
	ysin = y*temp4;

	if (((x0+xcos-ysin)>(0))&&((x0+xcos-ysin)<(IMAGE_W))&&((y0+xsin+ycos)>(0))&&((y0+xsin+ycos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0+xcos-ysin, y0+xsin+ycos);
	if (((x0-xcos-ysin)>(0))&&((x0-xcos-ysin)<(IMAGE_W))&&((y0-xsin+ycos)>(0))&&((y0-xsin+ycos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0-xcos-ysin, y0-xsin+ycos);
	if (((x0+xcos+ysin)>(0))&&((x0+xcos+ysin)<(IMAGE_W))&&((y0+xsin-ycos)>(0))&&((y0+xsin-ycos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0+xcos+ysin, y0+xsin-ycos);
	if (((x0-xcos+ysin)>(0))&&((x0-xcos+ysin)<(IMAGE_W))&&((y0-xsin-ycos)>(0))&&((y0-xsin-ycos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0-xcos+ysin, y0-xsin-ycos);
	x++;
    }

    t=x%6;
    if(t==1)t=1;
    else t = 0;
    
    xcos = x*temp1;
    xsin = x*temp2;
    ycos = y*temp3;
    ysin = y*temp4;

    if (((x0+xcos-ysin)>(0))&&((x0+xcos-ysin)<(IMAGE_W))&&((y0+xsin+ycos)>(0))&&((y0+xsin+ycos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0+xcos-ysin, y0+xsin+ycos);
    if (((x0-xcos-ysin)>(0))&&((x0-xcos-ysin)<(IMAGE_W))&&((y0-xsin+ycos)>(0))&&((y0-xsin+ycos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0-xcos-ysin, y0-xsin+ycos);
    if (((x0+xcos+ysin)>(0))&&((x0+xcos+ysin)<(IMAGE_W))&&((y0+xsin-ycos)>(0))&&((y0+xsin-ycos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0+xcos+ysin, y0+xsin-ycos);
    if (((x0-xcos+ysin)>(0))&&((x0-xcos+ysin)<(IMAGE_W))&&((y0-xsin-ycos)>(0))&&((y0-xsin-ycos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0-xcos+ysin, y0-xsin-ycos);

    r = r1; r1 = r2;
    r2 = (int)r;
    x = 0; y = r2;
    r12 = r1*r1; r22 = r2*r2;
    l = r12 + r22;

    xmax = r12/sqrt(l);
    tn = r12-2*r2*r12;
    while ( x <= xmax )
    {
	if ( tn<0||y==0 )
	    tn += (4*x+2)*r22;
	else
	{
	    tn += (4*x+2)*r22+(1-y)*4*r12;
	    y--;
	}

	t=x%6;
	if(t==1)t=1;
	else t = 0;

	xcos = x*temp1;
	xsin = x*temp2;
	ycos = y*temp3;
	ysin = y*temp4;

	if (((x0+ycos-xsin)>(0))&&((x0+ycos-xsin)<(IMAGE_W))&&((y0+ysin+xcos)>(0))&&((y0+ysin+xcos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0+ycos-xsin, y0+ysin+xcos);
	if (((x0-ycos-xsin)>(0))&&((x0-ycos-xsin)<(IMAGE_W))&&((y0-ysin+xcos)>(0))&&((y0-ysin+xcos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0-ycos-xsin, y0-ysin+xcos);
	if (((x0+ycos+xsin)>(0))&&((x0+ycos+xsin)<(IMAGE_W))&&((y0+ysin-xcos)>(0))&&((y0+ysin-xcos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0+ycos+xsin, y0+ysin-xcos);
	if (((x0-ycos+xsin)>(0))&&((x0-ycos+xsin)<(IMAGE_W))&&((y0-ysin-xcos)>(0))&&((y0-ysin-xcos)<(IMAGE_H)))
	    if(t)
		m_ptrImgArea->DrawPixmapPt(gc, x0-ycos+xsin, y0-ysin-xcos);
	x++;
    }

    t=x%6;
    if(t==1)t=1;
    else t = 0;
    
    xcos = x*temp1;
    xsin = x*temp2;
    ycos = y*temp3;
    ysin = y*temp4;

    if (((x0+ycos-xsin)>(0))&&((x0+ycos-xsin)<(IMAGE_W))&&((y0+ysin+xcos)>(0))&&((y0+ysin+xcos)<(0+IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0+ycos-xsin, y0+ysin+xcos);
    if (((x0-ycos-xsin)>(0))&&((x0-ycos-xsin)<(IMAGE_W))&&((y0-ysin+xcos)>(0))&&((y0-ysin+xcos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0-ycos-xsin, y0-ysin+xcos);
    if (((x0+ycos+xsin)>(0))&&((x0+ycos+xsin)<(IMAGE_W))&&((y0+ysin-xcos)>(0))&&((y0+ysin-xcos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0+ycos+xsin, y0+ysin-xcos);
    if (((x0-ycos+xsin)>(0))&&((x0-ycos+xsin)<(IMAGE_W))&&((y0-ysin-xcos)>(0))&&((y0-ysin-xcos)<(IMAGE_H)))
	if(t)
	    m_ptrImgArea->DrawPixmapPt(gc, x0-ycos+xsin, y0-ysin-xcos);

    g_object_unref(gc);
    m_ptrImgArea->UpdateImgArea();
}

///> private
void MeasureDraw::ConvertPoint(POINT &p)
{
	// p.x = p.x + IMAGE_X;
	// p.y = p.y + IMAGE_Y;
}

void MeasureDraw::CalcMeasureCenter()
{
	m_cursor.x = IMAGE_W/2;
	m_cursor.y = IMAGE_Y + IMAGE_H*2/3;
	//m_cursor.y = IMAGE_Y + IMAGE_H/2;
}

void MeasureDraw::ClearPwCycle(int begin, int end)
{
//    if(begin == 0 && end == 0)
    if(begin == end)
        return;

    int up, down, left, right;
    ImageAreaDraw::GetInstance()->GetBoundary(up, down, left, right);
    m_ptrImgArea->DrawTrace(GDK_COPY, g_black, begin, up, begin, down);
    m_ptrImgArea->DrawTrace(GDK_COPY, g_black, end, up, end, down);
}

void MeasureDraw::PwCycle(int begin, int end)
{
    //    if(begin == 0 && end == 0)
    if(begin == end)
        return;

    int up, down, left, right;
    ImageAreaDraw::GetInstance()->GetBoundary(up, down, left, right);
    m_ptrImgArea->DrawTrace(GDK_COPY, g_yellow, begin, up, begin, down);
    m_ptrImgArea->DrawTrace(GDK_COPY, g_yellow, end, up, end, down);
}

void MeasureDraw::PwTrace(const vector<POINT>& trace, EDrawMode mode)
{
    if (trace.empty())
        return ;

    GdkFunction gdk_mode;
    if (mode == XOR)
	    gdk_mode = GDK_XOR;
    else 
	    gdk_mode = GDK_COPY;

    vector<POINT>::const_iterator iter;
    // draw trace
    if (trace.begin() != trace.end()) {
        for (iter = trace.begin()+1; iter != trace.end(); iter++) {
            m_ptrImgArea->DrawTrace(gdk_mode, g_yellow, (iter-1)->x, (iter-1)->y, iter->x, iter->y);
            m_ptrImgArea->DrawTracePt(gdk_mode, g_yellow, iter->x, iter->y);
        }
    }
}

void MeasureDraw::DrawTraceTag(POINT p, bool isCur, EDrawMode mode, bool update)
{
    if(p.x == 0)
        return;
    GdkColor *color;
    
    if (isCur) {
	color = MeasureColorConvert(m_colorCurIndex);
	if (mode == XOR)
	    m_ptrImgArea->DrawTraceTag(GDK_XOR, color, p.x, p.y, update);
	else if (mode == COPY)
	    m_ptrImgArea->DrawTraceTag(GDK_COPY, color, p.x, p.y, update);
    }
    else {
	color = g_red; //MeasureColorConvert(m_colorConfirmIndex);
	if (mode == XOR)
	    m_ptrImgArea->DrawTraceTag(GDK_XOR, color, p.x, p.y, update);
	else if (mode == COPY)
	    m_ptrImgArea->DrawTraceTag(GDK_COPY, color, p.x, p.y, update);
    }
}

//hlx
void MeasureDraw::DrawIMTBox(POINT p1, POINT p2, bool draw)
{
    m_imgAttr.area = ImageArea::IMT;
    m_imgAttr.mode.cvDraw = draw;
    int w = p2.x - p1.x;
    int h = p2.y - p1.y;
    GdkColor* color = MeasureColorConvert(m_colorCurIndex);
    m_ptrImgArea->DrawRectangle(m_imgAttr, color, p1.x, p1.y, w, h, false);
}

void MeasureDraw::DrawIMTIntima(POINT start, unsigned int *pIntima, int count, bool draw)
{
    m_imgAttr.area = ImageArea::IMT;
    m_imgAttr.mode.cvDraw = draw;
    int x1, y1;
#if 0
    GdkColor* color;
    bool updown = ImgProc2D::GetInstance()->GetUpDownStatusForIMT(); 
    if (updown)
        color = g_yellow;
    else
        color = g_red;
#endif
    for(int i=0;i<count;i++)
    {
        if (pIntima[i] > 0)
        {
            x1 = i+start.x;
            y1 = pIntima[i]+start.y;
            if (y1 <= start.y)
                break;
                m_ptrImgArea->DrawLine(m_imgAttr, g_red, x1, y1, x1, y1);//lihauemi 2012.07.11
                //m_ptrImgArea->DrawLine(m_imgAttr, color, x1, y1, x1, y1);
        }
    }
}

void MeasureDraw::DrawIMTAdvent(POINT start, unsigned int *pAdventitia, int count, bool draw)
{
    m_imgAttr.area = ImageArea::IMT;
    m_imgAttr.mode.cvDraw = draw;
      int x1, y1;
#if 0
      GdkColor* color;
      bool updown = ImgProc2D::GetInstance()->GetUpDownStatusForIMT(); 
      if (updown)
          color = g_red;
      else
          color = g_yellow;
#endif
      for(int i=0;i<count;i++)
    {
        if (pAdventitia[i] > 0)
        {
            x1 = i+start.x;
            y1 = pAdventitia[i]+start.y;
            if (y1 <= start.y)
                break;
                m_ptrImgArea->DrawLine(m_imgAttr, g_yellow, x1, y1, x1, y1);
                //m_ptrImgArea->DrawLine(m_imgAttr, color, x1, y1, x1, y1);
        }
    }
}

void MeasureDraw::DrawAngleArc(POINT center, POINT left, POINT right, bool isCur, EDrawMode mode)
{
	float angle[2] = {0.0f, 0.0f};
	POINT P[2] = {left, right};
	float alpha = 0.0f;
	float beta = 0.0f;
	float value = 0.0f;

	for (int i = 0; i < 2; i++)
	{
		alpha = asin((center.y - P[i].y) / sqrt((float)(P[i].x - center.x) * (P[i].x - center.x) + 
					(center.y - P[i].y) * (center.y - P[i].y)));
		beta = acos((P[i].x - center.x) / sqrt((float)(P[i].x - center.x) * (P[i].x - center.x) + 
					(center.y - P[i].y) * (center.y - P[i].y)));
		if ((alpha >= (float)0) && (beta < (float)PI / 2))
			angle[i] = alpha * 180 / (float)PI;
		else if ((alpha >= (float)0) && (beta >= (float)PI / 2))
			angle[i] = beta * 180 / (float)PI;
		else if ((alpha < (float)0) && (beta < (float)PI / 2))
			angle[i] = alpha * 180 / (float)PI;
		else if ((alpha < (float)0) && (beta >= (float)PI / 2))
		{
			angle[i] = (PI - alpha) * 180 / (float)PI;
		}

		angle[i] -= value;
		value = angle[i];
	}

	if (fabs(angle[1]) > 180)
		angle[1] = -(360 - fabs(angle[1])) * (angle[1] / fabs(angle[1]));

	GdkColor *color;
	if (isCur)
	{
	    color = MeasureColorConvert(m_colorCurIndex);
	    if (mode == XOR) {
		m_imgAttr.mode.gdkMode = GDK_XOR;
		m_ptrImgArea->DrawArc(m_imgAttr, color, center.x - 20, center.y - 20, 40, 40, angle[0], angle[1]);
	    } else if (mode == COPY) {
		m_imgAttr.mode.gdkMode = GDK_COPY;
		m_ptrImgArea->DrawArc(m_imgAttr, color, center.x - 20, center.y - 20, 40, 40, angle[0], angle[1]);
	    }
	}
	else
	{
	    color = MeasureColorConvert(m_colorConfirmIndex);
	    if (mode == XOR) {
		m_imgAttr.mode.gdkMode = GDK_XOR;
		m_ptrImgArea->DrawArc(m_imgAttr, color, center.x - 20, center.y - 20, 40, 40, angle[0], angle[1]);
	    } else if (mode == COPY) {
		m_imgAttr.mode.gdkMode = GDK_COPY;
		m_ptrImgArea->DrawArc(m_imgAttr, color, center.x - 20, center.y - 20, 40, 40, angle[0], angle[1]);
	    }
	}
}

void MeasureDraw::DrawOrderNumber(POINT p, int orderNumber, EDrawMode mode)
{
    
	p.x = p.x + 10;
	if (p.x < 0) p.x = 0;
	if (p.x > IMAGE_W - 20) p.x = IMAGE_W - 20;
	if (p.y < 0) p.y = 0;
	if (p.y > IMAGE_H - 20) p.y = IMAGE_H - 20;
    SysMeasurementSetting sysMeasure;
    m_colorConfirmIndex = sysMeasure.GetMeasureColorConfirm();
	GdkColor *color = MeasureColorConvert(m_colorConfirmIndex);
	char str[3] = {0};
	snprintf(str, 3, "%d", orderNumber);

	if (mode == XOR)
		m_ptrImgArea->DrawString(str, p.x , p.y, color, true);
	else if (mode == COPY)
		m_ptrImgArea->DrawString(str, p.x, p.y, color, false);
}

void MeasureDraw::DrawOrderNumberForErase(POINT p, int orderNumber, int confirmColor, EDrawMode mode)
{

	p.x = p.x + 10;
	if (p.x < 0) p.x = 0;
	if (p.x > IMAGE_W - 20) p.x = IMAGE_W - 20;
	if (p.y < 0) p.y = 0;
	if (p.y > IMAGE_H - 20) p.y = IMAGE_H - 20;
	GdkColor *color = MeasureColorConvert(confirmColor);
	char str[3] = {0};
	snprintf(str, 3, "%d", orderNumber);

	if (mode == XOR)
		m_ptrImgArea->DrawString(str, p.x , p.y, color, true);
	else if (mode == COPY)
		m_ptrImgArea->DrawString(str, p.x, p.y, color, false);
}


void MeasureDraw::ChangeOrderNumber()
{
#ifdef TRANSDUCER
    SysMeasurementSetting sysMeasure;
    int m_meaResultFontSize = sysMeasure.GetMeasureResult();
    int measure_line_max = 14;
    if(m_meaResultFontSize == 1)
    {
        measure_line_max = 10;
    }
	m_orderNumber++;
    printf("m_order=%d\n", m_orderNumber);
    if (m_orderNumber > measure_line_max)
		m_orderNumber = 1;
#else
	m_orderNumber++;
    if (m_orderNumber > MEASURE_RES_LINES_MAX)
		m_orderNumber = 1;
#endif
}

void MeasureDraw::DrawFanshaped(POINT center, POINT left, POINT right, double angle)
{

#if 0
    int radius = 20;
    int alpha = 0;
    int detal = 5;
    int x1, y1;//first point
    int x2, y2;//second point
    //m_ptrImgArea->DrawArc(m_imgAttr, g_green, center.x, center.y, 50, 100, 180 - angle, angle, false);
    while((alpha+detal) < angle)
    {
        alpha += detal;
        x1 = center.x + radius * cos(alpha);
        y1 = center.y + radius * sin(alpha);
        x2 = center.x + radius * cos(alpha + detal);
        y2 = center.y + radius * sin(alpha + detal);
        printf("p1(%d, %d), p2(%d, %d)\n", x1, y1, x2, y2);

        m_ptrImgArea->DrawLine(m_imgAttr, g_yellow, x1, y1, x2, y2);
    }
#endif
#if 0
    int theta;
    double tan_theta;
    if(left.x != center.x)
        tan_theta = (double)(left.y - center.y)/ (double) (left.x - center.x);
    if(tan_theta < 0)
        theta = arctan(abs(tan_theta)) + 90;
    else
        theta = arctan(abs(tan_theta));
    //    if(left.x == center.x)
#endif 
    DrawBaseLine(left, right, true, XOR);
    //m_ptrImgArea->DrawLine(m_imgAttr, g_yellow, left.x, left.y, right.x, right.y);
    //m_ptrImgArea->DrawString(NULL, center.x, center.y, "angle");
}
