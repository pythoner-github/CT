/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: GlobalClassMan.cpp
 * @brief: manage global class object 
 *
 * version: V1.0
 * date: 2009-7-31
 * @author: zhanglei
 */

#include "GlobalClassMan.h"
#include "Calc2DConvex.h"
#include "Calc2DLinear.h"
#include "Calc2DTV.h"
#include "Calc2DTinyConvex.h"
#include "Calc2DTinyConvexHigh.h"
#include "Calc2DPhase.h"

GlobalClassMan* GlobalClassMan::m_ptrInstance = NULL;

pKeyFunc g_ptrKeyFunc = NULL;

GlobalClassMan::GlobalClassMan()
{
	m_ptrCalc2D = NULL;
	m_probeType = 'C';
}

GlobalClassMan::~GlobalClassMan()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
    g_ptrKeyFunc = NULL;

}

GlobalClassMan* GlobalClassMan::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new GlobalClassMan;

	return m_ptrInstance;	
}

Calc2D* GlobalClassMan::GetCalc2D(string model)
{
	if (m_ptrCalc2D != NULL)
		delete m_ptrCalc2D;

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
	m_probeType = para.type;

	const char* type = model.c_str();

	if (strcmp(type, "35C50K") == 0)
	{
		m_ptrCalc2D = new Calc2DConvex;
	}
	else if (strcmp(type, "75L40K") == 0)
	{
		m_ptrCalc2D = new Calc2DLinear;
	}
	else if (strcmp(type, "65C10K") == 0) //腔体
	{
		m_ptrCalc2D = new Calc2DTV;
	}
	else if (strcmp(type, "35C20H") == 0) //低频微凸
	{
		m_ptrCalc2D = new Calc2DTinyConvex;
	}
	else if (strcmp(type, "65C15D") == 0) //高频微凸
	{
		m_ptrCalc2D = new Calc2DTinyConvexHigh;
	}
	else if (strcmp(type, "30P16A") == 0) //phase
	{
		m_ptrCalc2D = new Calc2DPhase;
	}
	else
	{
		switch(m_probeType)
		{
			case 'C':
			case 'T':
				m_ptrCalc2D = new Calc2DConvex;
				break;

			case 'N':
				m_ptrCalc2D = new Calc2DTinyConvex;
				break;
			
			case 'n':
			case 'c':
				m_ptrCalc2D = new Calc2DTinyConvexHigh;
				break;

			case 'l':
			case 'L':
				m_ptrCalc2D = new Calc2DLinear;
				break;

			case 't':
				m_ptrCalc2D = new Calc2DTV;
				break;

            case 'P':
                m_ptrCalc2D = new Calc2DPhase;
                break;

			default:
				m_ptrCalc2D = new Calc2DConvex;
				break;			
		}
	}

	return m_ptrCalc2D;
}


