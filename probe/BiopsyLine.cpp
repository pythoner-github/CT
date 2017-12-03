#include "gtk/gtk.h"
#include "gui_func.h"
#include "gui_global.h"
#include "BiopsyLine.h"
#include "MenuBiopsy.h"
#include "ProbeSocket.h"
#include "ProbeMan.h"
#include "../sysMan/SysBiopsySetting.h"
#include "../display/HintArea.h"
#include "../display/gui_global.h"
#include "SysNoteSetting.h"
#include "BiopsyMan.h"

BiopsyLine* BiopsyLine::m_ptrInstance = NULL;

BiopsyLine* BiopsyLine::GetInstance()
{
    if (m_ptrInstance == NULL) {
	m_ptrInstance = new BiopsyLine;
    }
    return m_ptrInstance;
}

BiopsyLine::BiopsyLine(void)
{
    m_biopX = 0;
    m_biopY = 0;
    Angle = 0;
    m_baseAngle = 0;
    m_adjustAngle = 0;

    m_baseVal_x = 0;
    m_baseVal_y = 0;
    m_adjustRange_x = 0;
    m_adjustRange_y = 0;
    m_size = 0;
    m_ptrImgDraw = ImageAreaDraw::GetInstance();
	//------------------------------------------------//
	 m_biopX1=0;
	 m_biopY1=0;
	 m_biopX2=0;
	 m_biopY2=0;
     m_biopAngle1=0;
     m_biopAngle2=0;
	//---------------------------------------------------//
}

BiopsyLine::~BiopsyLine()
{
    if (m_ptrInstance != NULL)
	delete m_ptrInstance;
}

bool BiopsyLine::Create(void)
{
    ProbeSocket::ProbePara curPara;
    ProbeMan::GetInstance()->GetCurProbe(curPara);

    m_probeType = curPara.type;
    m_probeRadius = curPara.r;

    PRINTF("probe type = %c, radius = %d\n", m_probeType, m_probeRadius);

    if (InitPara()) {
       // g_menuBiopsy.SetStatus(true);
    } else {
       // g_menuBiopsy.SetStatus(false);
        return false;
    }

    Clear();
   // InitBiopsy();
    if (g_menuBiopsy.GetDrawStatus())
        Draw();
    
    return true;
}

bool BiopsyLine::InitPara(void)
{
	//---------------------------------------//
	Angle=BiopsyMan::GetInstance()->GetCurBioAngleValue();
	m_biopX=BiopsyMan::GetInstance()->GetBioXOfCurBioBracketAngleType();
	//------------------------------------------//
	/*m_biopX1=m_biopX+1;
	m_biopX2=m_biopX-1;*/
	/*m_biopX1=m_biopX+0.5;
	m_biopX2=m_biopX-0.5;*/
    m_biopX1=m_biopX;
	m_biopX2=m_biopX;
    m_biopAngle1=Angle-5;
    m_biopAngle2=Angle+5;
	//-----------------------------------------//
	float m_angle = Angle * PI /180;
    float m_angle1=m_biopAngle1 * PI /180;
    float m_angle2=m_biopAngle2 * PI /180;
    if (tan(m_angle) < 0.001)
    {
		 m_biopY = 250;//MaxDepth
		 m_biopY1=250;
		 m_biopY2=250;
    }
    else
    {
         m_biopY= m_biopX / tan(m_angle);
		/* m_biopY1= m_biopX1 / tan(m_angle);
		 m_biopY2= m_biopX2 / tan(m_angle);*/
         m_biopY1= m_biopX1 / tan(m_angle1);
		 m_biopY2= m_biopX2 / tan(m_angle2);
    }

      //  HintArea::GetInstance()->UpdateHint(_("No biopsy bracket found!"), 2);
    return true;
}

void BiopsyLine::WritePara(void)
{
   	BiopsyMan::GetInstance()->WriteXAngleValue(m_biopX,Angle);

	HintArea::GetInstance()->UpdateHint(_("saving parameter succeed!"), 1);//2016.09.22
}

void BiopsyLine::Draw(void)
{
	 if (g_menuBiopsy.GetDoubleLineStatus())
	 {
		 m_ptrImgDraw->DrawBiopsyDoubleLine(m_biopX1, m_biopY1, m_biopX2, m_biopY2);
	 }
	 else
	 {
		m_ptrImgDraw->DrawBiopsyLine(m_biopX, m_biopY);
	 }

}

void BiopsyLine::Clear(void)
{
    // m_ptrImgDraw->ClearBiopsyLine();
     if (g_menuBiopsy.GetDoubleLineStatus())
	 {
          m_ptrImgDraw->ClearBiopsyDoubleLine();
	 }
	 else
	 {
        m_ptrImgDraw->ClearBiopsyLine();
	 }
}

void BiopsyLine::Redraw(void)
{
   // m_ptrImgDraw->ReDrawBiopsyLine();
 if (g_menuBiopsy.GetDoubleLineStatus())
	 {
         m_ptrImgDraw->ReDrawBiopsyDoubleLine();
	 }
	 else
	 {
         m_ptrImgDraw->ReDrawBiopsyLine();
	 }

}

void BiopsyLine::SetDotSize(int size)
{
    m_size = size;
}

void BiopsyLine::SetDefault(void)
{
	Angle=BiopsyMan::GetInstance()->GetCurBioAngleValueDefault();
	m_biopX=BiopsyMan::GetInstance()->GetDefaultBioXOfCurBioBracketAngleType();
	/*m_biopX1=m_biopX+1;
	m_biopX2=m_biopX-1;*/
    /*m_biopX1=m_biopX+0.5;
	m_biopX2=m_biopX-0.5;*/
    m_biopX1=m_biopX;
	m_biopX2=m_biopX;
    m_biopAngle1=Angle-5;
    m_biopAngle2=Angle+5;

	float m_angle = Angle * PI /180;
    float m_angle1=m_biopAngle1 *PI /180;
    float m_angle2=m_biopAngle2 *PI /180;
    if (tan(m_angle) < 0.001)
    {
		 m_biopY = 250;//MaxDepth
		 m_biopY1=250;
		 m_biopY2=250;
    }
    else
    {
         m_biopY = m_biopX / tan(m_angle);
		/* m_biopY1 = m_biopX1 / tan(m_angle);
		 m_biopY2 = m_biopX2 / tan(m_angle);*/
         m_biopY1 = m_biopX1 / tan(m_angle1);
		 m_biopY2 = m_biopX2 / tan(m_angle2);

    }
	Draw();
    BiopsyMan::GetInstance()->WriteXAngleValue(m_biopX,Angle);//2016.11.05--save to current configure file
	HintArea::GetInstance()->UpdateHint(_("Restoring default factory succeed!"), 1);

	//------------------------------------------//
}

void BiopsyLine::InitBiopsy(void)
{
    const unsigned int maxAngle = m_baseAngle + m_adjustAngle; 
    unsigned int minAngle = 0;
    if (( m_baseAngle - m_adjustAngle) < 0)
    {
         minAngle = 0;
    }
    else {
          minAngle = m_baseAngle - m_adjustAngle;
    }
    const unsigned int maxPos = m_baseVal_x + m_adjustRange_x;
    const unsigned int minPos = m_baseVal_x - m_adjustRange_x;
    int valueAngle = Angle -m_baseAngle;
    int valuePos = m_biopX - m_baseVal_x;

    EKnobReturn ret;
    char str[5];
    if (m_biopY == maxAngle)
	ret = MAX;
    else if (m_biopY == minAngle)
	ret = MIN;
    else 
	ret = OK;
    sprintf(str, "%d", valueAngle);
   // g_menuBiopsy.UpdateAngle(str, ret);

    if (m_biopX == maxPos)
	ret = MAX;
    else if (m_biopX == minPos)
	ret = MIN;
    else 
	ret = OK;
    sprintf(str, "%d", valuePos);
   // g_menuBiopsy.UpdatePosition(str, ret);
    
}

void BiopsyLine::AngleChange(EKnobOper oper)
{
   /* const unsigned int max = m_baseAngle + m_adjustAngle;
    int min = 0;
    float m_Angle = 0;

    if ( (m_baseAngle - m_adjustAngle) < 0)
    {
		min = 0;
    }
    else 
    {
        min = m_baseAngle - m_adjustAngle;
    }*/

	int max=80;//88//90---------max angle
	int min=1;//0-------------min angle
	float m_Angle=0;
    
    float m_Angle1=0;
    float m_Angle2=0;

    if (oper == ADD && Angle < max)
    {
        ++Angle;
        m_Angle = Angle * PI /180;
        //------------------------------------------//
         m_biopAngle1=Angle-5;
         m_biopAngle2=Angle+5;
         m_Angle1=m_biopAngle1 *PI/180;
         m_Angle2=m_biopAngle2 *PI/180;
        //--------------------------------------------------//

        if ( ABS(tan(m_Angle)) <= 0.001)
        {
            m_biopY = 250;//MaxDepth;
			m_biopY1 =250;
			m_biopY2 =250;
        }
        else
        {
            m_biopY = m_biopX / tan(m_Angle);
			/*m_biopY1 = m_biopX1 / tan(m_Angle);
			m_biopY2 = m_biopX2 / tan(m_Angle);*/
            m_biopY1 = m_biopX1 / tan(m_Angle1);
			m_biopY2 = m_biopX2 / tan(m_Angle2);
        }

    }
    else if (oper == SUB && Angle > min)
    {
        --Angle;
        m_Angle = Angle * PI/180; 
        //------------------------------------------//
         m_biopAngle1=Angle - 5;
         m_biopAngle2=Angle + 5;
         m_Angle1=m_biopAngle1 *PI/180;
         m_Angle2=m_biopAngle2 *PI/180;
        //--------------------------------------------------//

        if ( ABS(tan(m_Angle)) <= 0.001)
        {
            m_biopY = 250;//MaxDepth;
			m_biopY1 =250;
			m_biopY2 =250;

        }
        else
        {
            m_biopY = m_biopX / tan(m_Angle);
            m_biopY1 = m_biopX1 / tan(m_Angle1);
			m_biopY2 = m_biopX2 / tan(m_Angle2);
        }
    }

    Draw();
}

void BiopsyLine::PositionChange(int offsetX)
{
    float angleTmp;
    float angleTmp1;
    float angleTmp2;

    m_baseVal_x=BiopsyMan::GetInstance()->GetDefaultBioXOfCurBioBracketAngleType();//

    int max=m_baseVal_x+25;
    int min=m_baseVal_x-25;

    if(min<0)//防止为m_biopx < 0
    {
        min=0;
    }
    //----------------------------------//
    if((m_biopX+offsetX>min)&&(m_biopX+offsetX<max))
    {
        m_biopX=m_biopX+offsetX;
        m_biopX1=m_biopX;
	    m_biopX2=m_biopX;
	}
	//------------------------------------------------//

    angleTmp = Angle * PI /180;
    angleTmp1= m_biopAngle1 * PI /180;
    angleTmp2= m_biopAngle2 * PI /180;

    if ( ABS(tan(angleTmp)) <= 0.001)
    {
        m_biopY = 250;//MaxDepth;
		m_biopY1 =250;
		m_biopY2 =250;
    }
    else
    {
        if(m_biopX / tan(angleTmp)<2)
            return;

        m_biopY = m_biopX / tan(angleTmp);
        m_biopY1 = m_biopX1 / tan(angleTmp1);
		m_biopY2 = m_biopX2 / tan(angleTmp2);
    }
    
    Draw();
}

void BiopsyLine::GetPara(unsigned int &biop_x, unsigned int &biop_y)
{
    float Temp1 = Angle * PI / 180;
    biop_x = m_biopX;
    if ( ABS(tan(Temp1)) <= 0.001)
    {
        biop_y = 250;//MaxDepth;
    }
    else
    {
        biop_y = m_biopX / tan(Temp1);
    }
}
void BiopsyLine::AngleSwitch(void) //2016.08.08
{
	Angle=BiopsyMan::GetInstance()->GetCurBioAngleValue();
	m_biopX=BiopsyMan::GetInstance()->GetBioXOfCurBioBracketAngleType();
    
    m_biopX1=m_biopX;
	m_biopX2=m_biopX;

    m_biopAngle1=Angle -5;
    m_biopAngle2=Angle +5;

	float m_angle = Angle * PI /180;
    float m_angle1 = m_biopAngle1 * PI /180;
    float m_angle2 = m_biopAngle2 * PI /180;

    if (tan(m_angle) < 0.001)
    {
		 m_biopY = 250;//MaxDepth
		 m_biopY1 =250;
	 	 m_biopY2 =250;
    }
    else
    {
         m_biopY = m_biopX / tan(m_angle);
         m_biopY1 = m_biopX1 / tan(m_angle1);
		 m_biopY2 = m_biopX2 / tan(m_angle2);

    }

	Draw();
}
void BiopsyLine::BiopsyLine::ClearSingleLine(void)
{
    m_ptrImgDraw->ClearBiopsyLine();
}
void BiopsyLine::BiopsyLine::ClearDoubleLine(void)
{
    m_ptrImgDraw->ClearBiopsyDoubleLine();
}

void BiopsyLine::BiopsyEnhance()
{
    bool on = ImgProc2D::GetInstance()->GetEnhanceBiopsy();
    Img2D::GetInstance()->EnhanceBiopsy(!on);
    ImgProc2D::GetInstance()->EnhanceBiopsy(!on);
}
