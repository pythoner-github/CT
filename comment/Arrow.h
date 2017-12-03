/**
* @brief Arrow.h: interface for the CArrow class.
* CArrow is a base class.
*
* @version: 1.0
* @author: hehao
* @date: 2009-8-25
*/
#ifndef ARROW_H
#define ARROW_H

#include <deque>
#include "Def.h"
#include "AbsUpdateArrow.h"
using namespace std;

class CArrow
{
public:
	CArrow(CAbsUpdateArrow& updateArrow, POINT initPos);
	virtual ~CArrow();

	enum COLOR{ WHITE, GRAY, RED, GREEN, YELLOW, BLUE };
	enum SIZE{ SMALL, MID, BIG };
	enum SHAPE{ SIMPLEARROW, HOLLOWARROW };
	enum { DIRECTION_MAX = 24 };
	enum DIRECTION{ DI0, DI1, DI2, DI3, DI4, DI5, DI6, DI7, DI8, DI9, DI10, DI11, DI12, DI13,
	DI14, DI15, DI16, DI17, DI18, DI19, DI20, DI21, DI22, DI23 };

	void SetArrowPos(POINT pos);
	void SetArrowDir(DIRECTION direction);
	void SetArrowShape(SHAPE shape);
	void SetArrowSize(SIZE size);
	void SetArrowColor(COLOR color);
    bool ArrowListEmpty(void);


	POINT GetArrowPos();
	DIRECTION GetArrowDir();
	SHAPE GetArrowShape();
	SIZE GetArrowSize();
	COLOR GetArrowColor();


	virtual bool Add();
	virtual bool Delete();
	virtual bool DeleteAll();
	virtual bool MoveBegin();
	virtual bool MoveEnd();
	virtual bool UndoMove();
	static void DeleteAllForFreeze();

protected:
	unsigned int GetDistance(POINT pos1, POINT pos2);

	static SIZE m_curSize;
	static COLOR m_curColor;
	static SHAPE m_curShape;
	POINT m_curPos;
	DIRECTION m_curDir;

	CAbsUpdateArrow &m_updateArrow;

	typedef struct tagArrowElem
	{
		POINT pos;
		DIRECTION direction;
		SIZE size;
		COLOR color;
		SHAPE shape;
	} ArrowElem, *PArrowElem;

	static deque<ArrowElem> m_arrowList;

private:
	static const double ARROW_SCALE[3];
	bool m_bDrawed;
	bool m_bMoving;
	POINT m_initPos;
	SIZE m_sizeT;
	COLOR m_colorT;
	SHAPE m_shapeT;
	deque<ArrowElem>::iterator m_pFind;
	enum {MAXDIS = 15};
};

#endif //ARROW_H