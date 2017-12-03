// -*- c++ -*-
#ifndef MEASURE_DRAW_H
#define MEASURE_DRAW_H

#include <gtk/gtk.h>
#include "Def.h"
#include "../display/ImageArea.h"
#include "gui_global.h"
#include "gui_func.h"
#include "ModeStatus.h"
#include "../sysMan/SysMeasurementSetting.h"

class MeasureDraw
{
public:
    MeasureDraw();
    ~MeasureDraw();

    int GetCursorType();
    void SetCursorType(int type);
    void SetCursor(POINT p);
    POINT GetCursor(void);
    void SetCursorSize(int size);
    int GetCursorSize(void);
    void SetConfirmColor(int color);
    int GetConfirmColor(void);
    void SetCurColor(int color);
    int GetCurColor(void);
	int GetOrderNumber(void);
	void SetOrderNumber(int number);

    POINT DrawInitialCursor(void);
    POINT DrawInitialHLine(bool isCur=TRUE);
    POINT DrawInitialVLine(bool isCur=TRUE);
    POINT DrawInitialCrossLine(bool isCur=TRUE);

    void DrawCursor(POINT p, bool isCur=true, EDrawMode mode=XOR, bool restrict=false);
    void DrawTraceTag(POINT p, bool isCur=true, EDrawMode mode=XOR, bool update=false);
    void DrawTraceLine(POINT p1, POINT p2, bool isCur=FALSE, EDrawMode mode=XOR);
    void DrawINTEGRALTraceLine(POINT p1, POINT p2, bool isCur=FALSE, EDrawMode mode=XOR);
    void DrawLine(POINT p1, POINT p2, bool isCur=FALSE, EDrawMode mode=XOR);
	void DrawDotLine(POINT p1, POINT p2, bool isCur=FALSE, EDrawMode mode=XOR);
    void DrawHDotLine(POINT p, bool isCur=TRUE, EDrawMode mode=XOR);
    void DrawVDotLine(POINT p, bool isCur=TRUE, EDrawMode mode=XOR);
    void DrawHLine(POINT p, bool isCur=TRUE, EDrawMode mode=XOR);
    void DrawVLine(POINT p, bool isCur=TRUE, EDrawMode mode=XOR); // no cursor
    void DrawTanBeeline(POINT point, double tan, bool isCur=FALSE, EDrawMode mode=XOR);
	vector<POINT> DrawTanLineSegment(POINT point1, double angle, int length, bool isCur=FALSE, EDrawMode mode=XOR);
    void DrawDotBeeline(POINT point1, POINT point2, bool isCur=FALSE, EDrawMode mode=XOR);
    void DrawCrossLine(POINT p, bool isCur=TRUE, EDrawMode mode=XOR);

    // void DrawDotLine(POINT p1, POINT p2, bool isCur=FALSE, EDrawMode mode=XOR);

    void DrawEllipse(POINT p1, POINT p2, POINT p3, bool isCur=FALSE);
    void DrawPoint(POINT p, bool isCur, EDrawMode mode);

    void ChangeCursorType(void);
    POINT CalcNextP(POINT p1, int method);

    void PwTrace(const vector<POINT>& trace, EDrawMode mode=COPY);
    void ClearPwCycle(int begin, int end);
    void PwCycle(int begin, int end);

    void DrawIMTBox(POINT p1, POINT p2, bool draw);//hlx 
    void DrawIMTIntima(POINT start, unsigned int *pIntima, int count, bool draw);
    void DrawIMTAdvent(POINT start, unsigned int *pAdventitia, int count, bool draw);
	
	void DrawAngleArc(POINT center, POINT left, POINT right, bool isCur=TRUE, EDrawMode mode=XOR);
	void DrawOrderNumber(POINT p, int orderNumber, EDrawMode mode = XOR);
	void ChangeOrderNumber(void);
    void DrawOrderNumberForErase(POINT p, int orderNumber, int confirmColor, EDrawMode mode = XOR);

    void DrawFanshaped(POINT center, POINT left, POINT right, double angle);

private:
    ImageArea *m_ptrImgArea;
    ImageArea::DrawAttr m_imgAttr;

    static int m_cursorType;
    int m_cursorSize;
    static POINT m_cursor; // point of measure, according to image area
	static int m_orderNumber;

    int m_colorCurIndex;
    int m_colorConfirmIndex;
//    int m_lineDisplay;
    int m_lineDensity;

    void ConvertPoint(POINT &p);
    void DrawBaseLine(POINT p1, POINT p2, bool isCur, EDrawMode mode);
    void CalcMeasureCenter();
};

inline int MeasureDraw::GetCursorType()
{
    return m_cursorType;
}

inline void MeasureDraw::SetCursorType(int type)
{
    m_cursorType = type;
}

inline int MeasureDraw::GetCursorSize(void)
{
    return m_cursorSize;
}
inline void MeasureDraw::SetCursorSize(int size)
{
    m_cursorSize = size;
}

inline int MeasureDraw::GetCurColor(void)
{
    return m_colorCurIndex;
}
inline void MeasureDraw::SetCurColor(int color)
{
    m_colorCurIndex = color;
}

inline int MeasureDraw::GetConfirmColor(void)
{
    return m_colorConfirmIndex;
}
inline void MeasureDraw::SetConfirmColor(int color)
{
    m_colorConfirmIndex = color;
}

inline void MeasureDraw::SetCursor( POINT p )
{
	m_cursor = p;
}

inline POINT MeasureDraw::GetCursor(void)
{
	return m_cursor;
}

inline int MeasureDraw::GetOrderNumber(void)
{
#ifdef TRANSDUCER
    SysMeasurementSetting sysMeasure;
    int m_meaResultFontSize = sysMeasure.GetMeasureResult();
    int measure_line_max = 14;
    if(m_meaResultFontSize == 1)
    {
        measure_line_max = 10;
    }
    if (m_orderNumber > measure_line_max)
		m_orderNumber = 1;
#endif

    return m_orderNumber;
}

inline void MeasureDraw::SetOrderNumber(int number)
{
	m_orderNumber = number;
}

#endif
