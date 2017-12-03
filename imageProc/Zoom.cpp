#include  "Zoom.h"
#include  "GlobalClassMan.h"
#include  "HintArea.h"
#include "ImgProc2D.h"

Zoom* Zoom::m_ptrInstance = NULL;
const double Zoom::GLOBAL_SCALE[MAX_GLOBAL_SCALE] = {1.0, 1.2, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0};
const double Zoom::PIP_SCALE[MAX_PIP_SCALE] = {1.0, 1.2, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0};

Zoom::Zoom()
{
	m_ptrDscPara = DscMan::GetInstance()->GetDscPara();
	m_ptrDscMan = DscMan::GetInstance();
	m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();

	m_globalCtrl = FALSE;
	m_globalScale = 0;
	m_globalPos.x = IMG_W/2;
	m_globalPos.y = IMG_H/2;
	m_globalOffsetX = m_globalPos.x;
	m_globalOffsetY = m_globalPos.y;

	m_PIPScale = 0;
	m_PIPPos.x = IMG_W/2;
	m_PIPPos.y = IMG_H/2;

    m_localCtrl = FALSE;
	m_localLineBegin = 60;
	m_localLineEnd = 140;
	m_localDotBegin = IMG_H/2 - 120;
	m_localDotEnd = IMG_H/2 + 120;
    m_localScale = 1.0;    
    m_timeout = 0;
}
Zoom::~Zoom()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}
Zoom* Zoom::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new Zoom;

	return m_ptrInstance;
}

// global zoom
void Zoom::GlobalZoomCtrl(bool on)
{
	if (on)
	{
		m_globalCtrl = TRUE;

		m_globalScale = 0;
		m_globalPos.x = IMG_W/2;
		m_globalPos.y = IMG_H/2;
                m_globalOffsetX = IMG_W / GLOBAL_SCALE[m_globalScale] / 2;
		m_globalOffsetY = IMG_H / GLOBAL_SCALE[m_globalScale] / 2;
		SetGlobalInfo(m_globalPos, m_globalScale);
	}
    else
    {
        m_globalCtrl = FALSE;
        m_globalScale = 0;
        m_globalPos.x = IMG_W/2;
        m_globalPos.y = IMG_H/2;
        m_globalOffsetX = m_globalPos.x;
        m_globalOffsetY = m_globalPos.y;

        m_ptrUpdate->ZoomScale(GLOBAL_SCALE[m_globalScale]);
        m_ptrUpdate->GlobalZoomScaleY();

        m_ptrDscMan->GetWriteLock();
        SetGlobalInfo(m_globalPos, m_globalScale);
        if (m_ptrDsc != NULL)
        {
            m_ptrDsc->Update();
        }
        m_ptrDscMan->ReadWriteUnlock();

        ImageArea::CopyImgDataToReplay();
    }
	m_ptrDscPara->dca2DIPAttrs.ipaGlobalZoom = on;
}
bool Zoom::GlobalZoomScale(EKnobOper oper)
{
	if ((oper == ADD) && (m_globalScale < (MAX_GLOBAL_SCALE-1)))
	{
		m_globalScale ++;
	}
	else if ((oper == SUB) && (m_globalScale > 0))
	{
		m_globalScale --;
	}
	else
	{
		return FALSE;
	}

    m_ptrDscMan->GetWriteLock();
    m_globalOffsetX = IMG_W / GLOBAL_SCALE[m_globalScale] / 2;
    m_globalOffsetY = IMG_H / GLOBAL_SCALE[m_globalScale] / 2;

        if (m_globalPos.y < m_globalOffsetY)
            m_globalPos.y = m_globalOffsetY;
        else if (m_globalPos.y > IMG_H - m_globalOffsetY)
            m_globalPos.y = IMG_H - m_globalOffsetY;

        if (m_globalPos.x < m_globalOffsetX)
            m_globalPos.x = m_globalOffsetX;
        else if (m_globalPos.x > IMG_W - m_globalOffsetX)
            m_globalPos.x = IMG_W - m_globalOffsetX;

	SetGlobalInfo(m_globalPos, m_globalScale);
	m_ptrDscMan->ReadWriteUnlock();
	return TRUE;
}
bool Zoom::GlobalZoomScroll(int offsetX, int offsetY)
{
	int stepx = offsetX;
	int stepy = -offsetY;

	if (!m_globalCtrl)
	{
		HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: Please zoom image first."));
		return FALSE;
	}

	// x
	m_globalPos.x += stepx;
	if (m_globalPos.x > (IMG_W - m_globalOffsetX))
		m_globalPos.x = IMG_W - m_globalOffsetX;
	else if (m_globalPos.x < m_globalOffsetX)
		m_globalPos.x = m_globalOffsetX;
	
	// y
	m_globalPos.y += stepy;
	if (m_globalPos.y > (IMG_H - m_globalOffsetY))
		m_globalPos.y = IMG_H - m_globalOffsetY;
	else if (m_globalPos.y < m_globalOffsetY)
		m_globalPos.y = m_globalOffsetY;

	m_ptrDscMan->GetWriteLock();
	SetGlobalPos(m_globalPos);
	m_ptrDscMan->ReadWriteUnlock();
	return TRUE;	
}

void Zoom::GetGlobalZoomInfo(double &scale, int &offsetInOriginal)
{
    bool UDStatus = ImgProc2D::GetInstance()->GetUDStatus();

    if (UDStatus)
        offsetInOriginal = IMG_H - m_globalOffsetY - m_globalPos.y;
    else
        offsetInOriginal = m_globalPos.y - m_globalOffsetY;

    scale = GLOBAL_SCALE[m_globalScale];
}

// PIP
void Zoom::PIPZoomCtrl(bool on)
{
	if (on)
	{
#ifdef EMP_355
	    ImageAreaDraw::GetInstance()->SetClearStatus(false);
#endif
		m_PIPScale = 0;
		m_PIPPos.x = IMG_W/2;
		m_PIPPos.y = IMG_H/2;
		SetPIPInfo(m_PIPPos, m_PIPScale);

        m_ptrDscMan->GetWriteLock();
        m_ptrDscPara->dca2DIPAttrs.ipaPIPZoom = on;
        m_ptrDscMan->ReadWriteUnlock();
	}
	else
	{
#ifdef EMP_355
        if(m_timeout > 0)
        {
            g_source_remove(m_timeout);
            m_timeout = 0;
        }
#endif
        m_PIPScale = 0;
        m_ptrUpdate->ZoomScale(PIP_SCALE[m_PIPScale]);
		m_ptrUpdate->ClearPIPMagnifier();
		m_ptrUpdate->ClearPIPBox();

        m_ptrDscMan->GetWriteLock();
        m_ptrDscPara->dca2DIPAttrs.ipaPIPZoom = on;
        if (m_ptrDsc != NULL)
        {
            m_ptrDsc->Update();
        }
        m_ptrDscMan->ReadWriteUnlock();

        ImageArea::CopyImgDataToReplay();
	}

}
bool Zoom::PIPZoomScale(EKnobOper oper)
{
	if ((oper == ADD) && (m_PIPScale < (MAX_PIP_SCALE-1)))
	{
		m_PIPScale ++;
	}
	else if ((oper == SUB) && (m_PIPScale > 0))
	{
		m_PIPScale --;
	}
	else
	{
		return FALSE;
	}

	m_ptrDscMan->GetWriteLock();
	SetPIPInfo(m_PIPPos, m_PIPScale);
	m_ptrDscMan->ReadWriteUnlock();
	return TRUE;	
}

gboolean Zoom::HandleSetPos(gpointer data)
{
    Zoom *pClass = (Zoom *)data;
    POINT *p = &pClass->m_PIPPos;
    pClass->m_ptrDscMan->GetWriteLock(); 
    pClass->SetPIPPos(*p); 
	pClass->m_ptrDscMan->ReadWriteUnlock();
    pClass->m_timeout = 0;
    return FALSE;
}
/*
 * @brief move magnifier's pos x
 */

bool Zoom::PIPZoomPos(int offsetX, int offsetY)
{
	int stepx = offsetX;
	int stepy = -offsetY;

	// x
	m_PIPPos.x += stepx;
	if (m_PIPPos.x > IMG_W)
		m_PIPPos.x = IMG_W;
	else if (m_PIPPos.x < 0)
		m_PIPPos.x = 0;

	// y
	m_PIPPos.y += stepy;
	if (m_PIPPos.y > IMG_H)
		m_PIPPos.y = IMG_H;
	else if (m_PIPPos.y < 0)
		m_PIPPos.y = 0;

    m_ptrUpdate->PIPMagnifier(m_PIPPos);
    
#ifdef EMP_355
    if (m_timeout == 0)
        m_timeout = g_timeout_add(200, HandleSetPos, this);
#else
    HandleSetPos((gpointer)this);
#endif
    return TRUE;		
}

// local zoom
void Zoom::LocalZoomInit()
{
    int scanRange[2];
    Img2D::GetInstance()->GetDisplayScanRange(scanRange);

    // init line
    int lineQuarter;
    if (((scanRange[1] - scanRange[0]) / 2) < INTERLINE)
    {
        lineQuarter = INTERLINE / 2;
    }
    else
    {
        lineQuarter = (scanRange[1] - scanRange[0]) / 4;
    }

	int lineCenter = Img2D::GetInstance()->GetScanLines() / 2;
    //printf("*******lineCenter scanlines scanRange[0] scanRange[1] lineQuarter %d %d %d %d %d\n",lineCenter, Img2D::GetInstance()->GetScanLines(), scanRange[0], scanRange[1], lineQuarter);
	m_localCtrl = FALSE;
	m_localLineBegin = lineCenter - lineQuarter;
	m_localLineEnd = lineCenter + lineQuarter;

    // init dot
    int dotCenter = IMG_H / 2;
    int dotWidthHalf = IMG_H * (m_localLineEnd - m_localLineBegin + 1) / (scanRange[1] - scanRange[0] + 1) / 2;
	m_localDotBegin = abs(dotCenter - dotWidthHalf);
	m_localDotEnd = dotCenter + dotWidthHalf;
    if(m_localDotEnd > IMG_H)
        m_localDotEnd = IMG_H;
    //printf("dotCenter:%d dotWidthHalf:%d\n", dotCenter, dotWidthHalf);

    //printf("localLineBegin: %d End:%d, dotBegin:%d dotEnd:%d\n", m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
    m_ptrUpdate->LocalZoomBox(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);

    //在图像区显示局部放大的倍数zjx
    double scale;
    scale =(double)IMG_H /(double)(m_localDotEnd - m_localDotBegin);
    if(scale > 10)
        scale = scale *1.3;
    UpdateLocalZoom(scale);
}
void Zoom::LocalZoomCtrl(bool on)
{
	if (on)
	{
		if (m_localCtrl) //avoid repeat set true
			return;

		m_ptrDscMan->GetWriteLock();
		SetLocalInfo(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
		m_ptrDscMan->ReadWriteUnlock();

        //printf("localLineBegin: %d End:%d, dotBegin:%d dotEnd:%d\n", m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
		// enter in local zoom measure status
		Img2D::GetInstance()->SetScale2DZoomMeasure(m_localScale); 
		m_localCtrl = TRUE;

		//clear zoom box  and redraw scale, focus
		m_ptrUpdate->ClearLocalZoomBox();
		m_ptrUpdate->EnterLocalZoom();
		Img2D::GetInstance()->ResetFocPosWhenZoom();

		// zoom image
		m_ptrDscMan->GetWriteLock();
		m_ptrDscPara->dca2DIPAttrs.ipaLocalZoom = TRUE;

		if (m_ptrDsc != NULL)
		{
			m_ptrDsc->Update();
		}
		m_ptrDscMan->ReadWriteUnlock();
		MultiFuncUndo();
	}
	else
	{
		// zoom image
		m_ptrDscMan->GetWriteLock();
		m_ptrDscPara->dca2DIPAttrs.ipaLocalZoom = FALSE;

		if (m_ptrDsc != NULL)
		{
			m_ptrDsc->UpdateScale();// need for exit local zoo 偏转的情况下不能进入localzoomm
		}
		if (m_ptrDsc != NULL)
		{
			m_ptrDsc->Update();
		}
		m_ptrDscMan->ReadWriteUnlock();

		// exit local zoom measure status
		Img2D::GetInstance()->RestoreScale2DZoomMasure(); 

		if (!m_localCtrl)	
			m_ptrUpdate->ClearLocalZoomBox();
		m_localCtrl = FALSE;

		//redraw scale, focus
		m_ptrUpdate->ExitLocalZoom();
        //局放关闭，更新图像显示倍数zjx
        UpdateLocalZoom(1.0);
	}

	// 拷贝到回放区，用于显示，否则冻结情况下的图像无法更新上去。
	ImageArea::CopyImgDataToReplay();
}

void Zoom::UpdateLocalZoom(double scale)
{
    m_ptrUpdate->ZoomScale(scale);
}

bool Zoom::LocalZoomBoxPos(int offsetX, int offsetY)
{
    int stepLine = offsetX;
	int stepDot = -offsetY;

	if (!m_localCtrl)
	{
		int scanLine[2];
		Img2D::GetInstance()->GetDisplayScanRange(scanLine);

		if (((m_localLineBegin + stepLine) >= scanLine[0]) && ((m_localLineEnd + stepLine) <= scanLine[1]))
		{
			m_localLineBegin += stepLine;
			m_localLineEnd += stepLine;
		}

		if (((m_localDotBegin + stepDot) >= 0) && ((m_localDotEnd + stepDot) <= (IMG_H-1)))
		{
			m_localDotBegin += stepDot;
			m_localDotEnd += stepDot;
		}

		m_ptrUpdate->LocalZoomBox(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
	}

	return TRUE;	
}
#if 0
bool Zoom::LocalZoomBoxSize(int offsetX, int offsetY, bool lrOverturn, bool udOverturn)
{
	int interLine = 50;
	int stepLine = offsetX;
	int interDot = 100;
	//int stepDot = -offsetY;
    int stepDot = IMG_H * stepLine / (scanLine[1] - scanLine[0] + 1);

	if (!m_localCtrl)
	{
		// line
		int scanLine[2];
		Img2D::GetInstance()->GetDisplayScanRange(scanLine);

		if (lrOverturn)
		{
			m_localLineBegin += stepLine;
			if (m_localLineBegin < scanLine[0])
				m_localLineBegin = scanLine[0];
			if (m_localLineBegin > (m_localLineEnd - interLine))
				m_localLineBegin = m_localLineEnd - interLine;
		}
		else
		{
			m_localLineEnd += stepLine;
			if (m_localLineEnd > scanLine[1])
				m_localLineEnd = scanLine[1];
			if ((m_localLineEnd - m_localLineBegin) < interLine)
				m_localLineEnd = m_localLineBegin + interLine;
		}

		// dot
		if (udOverturn)
		{
			m_localDotBegin += stepDot;
			if (m_localDotBegin < 0)
				m_localDotBegin = 0;
			if (m_localDotBegin > (m_localDotEnd - interDot))
				m_localDotBegin = m_localDotEnd - interDot;
		}
		else
		{
			m_localDotEnd += stepDot;
			if (m_localDotEnd > (IMG_H-1))
				m_localDotEnd = IMG_H - 1;
			if ((m_localDotEnd - m_localDotBegin) < interDot)
				m_localDotEnd = m_localDotBegin + interDot;
		}

		m_ptrUpdate->LocalZoomBox(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
	}
	return TRUE;		
}
#endif
bool Zoom::LocalZoomBoxSize(int offsetX, int offsetY, bool lrOverturn, bool udOverturn)
{
	int interLine = INTERLINE;
    int scanLine[2];
    Img2D::GetInstance()->GetDisplayScanRange(scanLine);
    PRINTF("zoom display range(%d, %d)\n", scanLine[0], scanLine[1]);
	int stepLine = - offsetY;

	offsetY = -offsetY;
	if (udOverturn)
		offsetY = -offsetY;
	stepLine = offsetY;

	if (!m_localCtrl)
	{
		if (offsetY > 0)
		{
			if (m_localLineBegin - stepLine >= scanLine[0])
				m_localLineBegin -= stepLine;
			if (m_localLineEnd + stepLine <= scanLine[1])
				m_localLineEnd += stepLine;
			int offsetDot = IMG_H * (m_localLineEnd - m_localLineBegin + 1) / (scanLine[1] - scanLine[0] + 1);
			if ((m_localDotBegin + m_localDotEnd) / 2 - offsetDot / 2 >= 0 && (m_localDotBegin + m_localDotEnd) / 2 + offsetDot / 2 < IMG_H)
			{
				m_localDotBegin = (m_localDotEnd + m_localDotBegin) / 2 - offsetDot / 2;
				m_localDotEnd = m_localDotBegin + offsetDot;
			}
			else if ((m_localDotBegin + m_localDotEnd) / 2 - offsetDot / 2 < 0)
			{
				m_localDotBegin = 0;
				m_localDotEnd = m_localDotBegin + offsetDot - 1;
			}
			else if ((m_localDotBegin + m_localDotEnd) / 2 + offsetDot / 2 >= IMG_H)
			{
				m_localDotEnd = IMG_H - 1;
				m_localDotBegin = m_localDotEnd - offsetDot + 1;
			}
		}
		else if (offsetY < 0)
		{
			if (m_localLineBegin - stepLine < m_localLineEnd + stepLine - interLine)
			{
				int offsetDot = IMG_H * (m_localLineEnd - m_localLineBegin + 2 * stepLine) / (scanLine[1] - scanLine[0] + 1);
				m_localLineBegin -= stepLine;
				m_localLineEnd += stepLine;
				m_localDotBegin = (m_localDotEnd + m_localDotBegin) / 2 - offsetDot / 2;
				m_localDotEnd = m_localDotBegin + offsetDot;
			}
		}

        m_ptrUpdate->LocalZoomBox(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
       
        //在图像区显示局部放大的倍数zjx
        double scale;
        scale =(double)IMG_H /(double)(m_localDotEnd - m_localDotBegin);
        if(scale > 10)
            scale = scale *1.3;
        UpdateLocalZoom(scale);
    }

	return TRUE;		
}

bool Zoom::ReSendLocalZoomInfo(void)
{
    if (m_localCtrl)
    {
        SetLocalInfo(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
        return TRUE;
    }
    return FALSE;
}

bool Zoom::GetLocalZoomStatus(void)
{
    return m_localCtrl;
}
void Zoom::GetScanLineRange(int lineRange[2])
{
    lineRange[0] = m_localLineBegin;
    lineRange[1] = m_localLineEnd;
}
void Zoom::GetScanDotRange(int dotRange[2])
{
    dotRange[0] = m_localDotBegin;
    dotRange[1] = m_localDotEnd;
}

int Zoom::GetScanDotBeginDepth(void)
{
    double scale = Img2D::GetInstance()->GetScale2DInImgHDot();
    return (int)(m_localDotBegin * scale);
}

void Zoom::GetLocalZoomScale(double &scale)
{
    scale = m_localScale;
}

void Zoom::ClearLocalZoom()
{
    //clear zoom box 
    m_ptrUpdate->ClearLocalZoomBox();
}

void Zoom::RedrawLocalZoom()
{
    //draw zoom box 
	m_ptrUpdate->LocalZoomBox(m_localLineBegin, m_localLineEnd, m_localDotBegin, m_localDotEnd);
}

void Zoom::SetLocalZoomBoxStatus(bool changeSize)
{
	m_ptrUpdate->SetLocalZoomBoxStatus(changeSize);
}

///>private
void Zoom::SetGlobalInfo(POINT p, int scaleIndex)
{
	// dsc
    if (m_ptrDsc != NULL)
    {
        m_ptrDsc->SetGlobalZoomState(p, GLOBAL_SCALE[scaleIndex]);
    }

	// draw
	m_ptrUpdate->ZoomScale(GLOBAL_SCALE[scaleIndex]);
    m_ptrUpdate->GlobalZoomScaleY();
}

void Zoom::SetGlobalPos(POINT p)
{
	// dsc
    if (m_ptrDsc != NULL)
    {
        m_ptrDsc->SetGlobalZoomState(p, GLOBAL_SCALE[m_globalScale]);
    }
    m_ptrUpdate->GlobalZoomScaleY();
}

void Zoom::SetPIPInfo(POINT p, int scaleIndex)
{
	RECT r;
	
	// dsc 
    if (m_ptrDsc != NULL)
    {
        r = m_ptrDsc->SetPIPZoomState(p, PIP_SCALE[scaleIndex]);
    }

	// draw
	POINT p1, p2;
	p1.x = r.left;
	p1.y = r.top;
	p2.x = r.right;
	p2.y = r.bottom;
	m_ptrUpdate->PIPBox(p1, p2);
	m_ptrUpdate->PIPMagnifier(p);
	m_ptrUpdate->ZoomScale(PIP_SCALE[scaleIndex]);
}

void Zoom::SetPIPPos(POINT p)
{
	RECT r;
printf("P1.X = %d, p1.y = %d\n", p.x, p.y);	
	// dsc 
    if (m_ptrDsc != NULL)
    {
        r = m_ptrDsc->SetPIPZoomState(p, PIP_SCALE[m_PIPScale]);
    }
	// draw
	POINT p1, p2;
	p1.x = r.left;
	p1.y = r.top;
	p2.x = r.right;
	p2.y = r.bottom;
	m_ptrUpdate->PIPBox(p1, p2);
}
void Zoom::SetLocalInfo(int lineBegin, int lineEnd, int dotBegin, int dotEnd)
{
	int dDot = dotEnd - dotBegin;
	int dLine = lineEnd - lineBegin;

	int scanRange[2];
	Img2D::GetInstance()->GetScanRange(scanRange);
	float scaleLine = (scanRange[1] - scanRange[0] + 1) / (float)dLine;
	float scaleDot =  IMG_H / (float)dDot; 

    m_localScale = (scaleLine < scaleDot)? scaleLine:scaleDot;

    if (m_ptrDsc != NULL)
    {
        PRINTF("lineB = %d, lineE = %d, dotB = %d, dotE = %d\n", lineBegin, lineEnd, dotBegin, dotEnd);
        m_ptrDsc->SetLocalZoomState(lineBegin, lineEnd, dotBegin, dotEnd, m_localScale);
    }

	// draw
	m_ptrUpdate->LocalZoomBox(lineBegin, lineEnd, dotBegin, dotEnd);
}

