/**
* @brief Arrow.cpp: implementation for the CArrow class.
* CArrow is a base class.
*
* @version: 1.0
* @author: hehao
* @date: 2009-8-25
*/

#include <math.h>
#include "Arrow.h"

CArrow::SHAPE CArrow::m_curShape = CArrow::HOLLOWARROW;
CArrow::SIZE CArrow::m_curSize = CArrow::MID;
CArrow::COLOR CArrow::m_curColor = CArrow::RED;
deque<CArrow::ArrowElem> CArrow::m_arrowList(0);
const double CArrow::ARROW_SCALE[3] = {1.0, 1.5, 2.0};

CArrow::CArrow(CAbsUpdateArrow& updateArrow, POINT initPos)
:m_curPos(initPos), m_curDir(DI0), m_updateArrow(updateArrow), 
//:m_curShape(HOLLOWARROW), m_curPos(initPos), m_curDir(DI0), m_updateArrow(updateArrow), 
m_bDrawed(false), m_bMoving(false), m_initPos(initPos)
{
	m_updateArrow =  updateArrow;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = true;
}

CArrow::~CArrow()
{
	if(m_bDrawed)
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = false;
	m_bMoving = false;
}

void CArrow::SetArrowPos(POINT pos)
{
	if(m_bDrawed)
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	
	m_curPos = pos;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = true;
}

void CArrow::SetArrowDir(DIRECTION direction)
{
	if(m_bDrawed)
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);

	m_curDir = direction;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = true;
}

void CArrow::SetArrowSize(SIZE size)
{
	if(m_bDrawed)
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);

	m_curSize = size;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = true;
}

void CArrow::SetArrowColor(COLOR color)
{
	if(m_bDrawed)
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);

	m_curColor = color;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = true;
}

bool CArrow::ArrowListEmpty(void)
{
    return m_arrowList.empty();
}
void CArrow::SetArrowShape(SHAPE shape)
{
	if(m_bMoving)
		return;

	if(m_bDrawed)
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);

	m_curShape = shape;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bDrawed = true;
}

POINT CArrow::GetArrowPos()
{
	return m_curPos;
}

CArrow::DIRECTION CArrow::GetArrowDir()
{
	return m_curDir;
}

CArrow::SIZE CArrow::GetArrowSize()
{
	return m_curSize;
}

CArrow::COLOR CArrow::GetArrowColor()
{
	return m_curColor;
}

CArrow::SHAPE CArrow::GetArrowShape()
{
	return m_curShape;
}

bool CArrow::Add()
{
	if(m_bMoving)
		return false;

	ArrowElem arrowElem;
	arrowElem.pos = m_curPos;
	arrowElem.direction = m_curDir;
	arrowElem.size = m_curSize;
	arrowElem.color = m_curColor;
	arrowElem.shape = m_curShape;
	m_arrowList.push_back(arrowElem);

	// for test
	ArrowElem arrowElem1 = m_arrowList.back();

	m_bDrawed = false;
	return true;
}

bool CArrow::Delete()
{
	if(m_bMoving)
		return false;

	if(m_arrowList.empty())
		return false; 

	//ArrowElem arrowElem = m_arrowList.front();
	ArrowElem arrowElem = m_arrowList.back();
	m_updateArrow.DrawArrow(arrowElem.pos, arrowElem.direction, ARROW_SCALE[arrowElem.size], 
		arrowElem.color, arrowElem.shape);
	m_arrowList.pop_back();
	//m_arrowList.pop_front();

	if(!m_bDrawed)
	{
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
		m_bDrawed = true;
	}
	return true;
}

bool CArrow::DeleteAll()
{
	if(m_bMoving)
		return false;

	if(m_arrowList.empty())
		return false;

	ArrowElem arrowElem;
	while(m_arrowList.size())
	{
		arrowElem = m_arrowList.back();
		m_updateArrow.DrawArrow(arrowElem.pos, arrowElem.direction, arrowElem.size, 
			arrowElem.color, arrowElem.shape);
		m_arrowList.pop_back();
	}

	if(!m_bDrawed)
	{
		m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
		m_bDrawed = true;
	}
	return true;
}

void CArrow::DeleteAllForFreeze()
{
#if 0
	while(m_arrowList.size())
	{
		m_arrowList.pop_back();
	}
#endif
	m_arrowList.clear();
}

bool CArrow::MoveBegin()
{
	if(m_bMoving)
		return false;

	for(m_pFind = m_arrowList.end()-1; m_pFind >= m_arrowList.begin(); m_pFind--)
	{
		if(GetDistance(m_curPos, m_pFind->pos) <= MAXDIS)
		{
			if(m_bDrawed)
				m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);

			m_bDrawed = true;
			m_bMoving = true;
			m_sizeT = m_curSize;
			m_colorT = m_curColor;
			m_shapeT = m_curShape;

			m_curPos = m_pFind->pos;
			m_curDir = m_pFind->direction;
			m_curSize = m_pFind->size;
			m_curColor = m_pFind->color;
			m_curShape = m_pFind->shape;
			return true;
		}
	}
	return false;
}

bool CArrow::MoveEnd()
{
	if(!m_bMoving)
		return false;

	m_arrowList.erase(m_pFind);
	m_bMoving = false;
	Add();

	m_curSize = m_sizeT;
	m_curColor = m_colorT;
	m_curShape = m_shapeT;
	return true;
}

bool CArrow::UndoMove()
{
	if(!m_bMoving)
		return false;

	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_updateArrow.DrawArrow(m_pFind->pos, m_pFind->direction, ARROW_SCALE[m_pFind->size], m_pFind->color, m_pFind->shape);
	m_curSize = m_sizeT;
	m_curColor = m_colorT;
	m_curShape = m_shapeT;
	m_updateArrow.DrawArrow(m_curPos, m_curDir, ARROW_SCALE[m_curSize], m_curColor, m_curShape);
	m_bMoving = false;
	return true;
}

unsigned int CArrow::GetDistance(POINT pos1, POINT pos2)
{
	int x = pos1.x - pos2.x;
	int y = pos1.y - pos2.y;
	return (sqrt(x*x + y*y));
}

