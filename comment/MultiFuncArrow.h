/**
* @brief MultiFuncArrow.h: interface for the MultiFuncArrow class.
*
* MultiFuncArrow handle all events of arrow operation, include 
* add delete cancel move and so on. MultiFuncArrow derive from 
* class CAbsUpdateArrow KnobEvent and AbsMultiFunc.
*
* emperor
* @version: 1.0
* @author: hehao
* @date: 2009-9-6
*/

#ifndef __MULTIFUNCARROW_H__
#define __MULTIFUNCARROW_H__

#include "Def.h"
#include "../include/AbsUpdateArrow.h"
#include "KnobEvent.h"
#include "../include/AbsMultiFunc.h"
#include "Arrow.h"
#include "KnobMenu.h"
#include "CalcTime.h"
 
class MultiFuncArrow: public CAbsUpdateArrow, public CKnobEvent, public AbsMultiFunc
{
public:
	MultiFuncArrow();
	virtual ~MultiFuncArrow();

	virtual void Do();
	virtual void Undo();
	virtual void Value(EKnobOper opr);
	virtual void Mouse(int offsetX, int offsetY);
	virtual void Esc();

    virtual void DrawArrow(POINT pos, unsigned int direction, double scale, 
            unsigned int color, unsigned int shape);

	virtual void KeyEvent(unsigned char keyValue);
	virtual void KnobEvent(unsigned char keyValue, unsigned char offset);
	virtual void MouseEvent(char offsetX, char offsetY);
	virtual void KnobArrowCreate();

    static EKnobReturn PressSetShape(void);
	static EKnobReturn SetShape(EKnobOper opr);
    static EKnobReturn PressSetSize(void);
	static EKnobReturn SetSize(EKnobOper opr);
	static EKnobReturn PressSetColor(void);
	static EKnobReturn SetColor(EKnobOper opr);
	POINT GetPoint(int offsetX, int offsetY);

protected:
	enum PRESSMODE{ CLICK, DBLCLK };
	enum ARROWOPR{ ADD, MODIFY };

private:
	static MultiFuncArrow *m_pThis;
	CArrow *m_arrow;
	POINT m_pos;
	PRESSMODE m_pressMode;
	ARROWOPR m_arrowOpr;
	int m_timer;
	KnobMenu::KnobItem *m_knobItemBku;
    KnobMenu::EKnobType m_knobTypeBku;

	gboolean Clicked();

	static EKnobReturn GetKnobRetShape(CArrow::SHAPE shape);
	static EKnobReturn GetKnobRetSize(CArrow::SIZE size);
	static EKnobReturn GetKnobRetColor(CArrow::COLOR color);

	static gboolean HandleClicked(gpointer data);
};

#endif //MULTIFUNCARROW_H
