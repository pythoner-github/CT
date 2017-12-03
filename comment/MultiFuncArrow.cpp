/**
 * @brief MultiFuncArrow.cpp: implementation for the MultiFuncArrow class.
 *
 * MultiFuncArrow handle all events of arrow operation, include
 * delete cancel move and so on. MultiFuncArrow derive from
 * class CAbsUpdateArrow KnobEvent and AbsMultiFunc.
 *
 * emperor
 * @version: 1.0
 * @author: hehao
 * @date: 2009-9-6
 */

#include "keyboard/MultiFuncValue.h"
#include "keyboard/KeyValueOpr.h"
#include "comment/MultiFuncArrow.h"
#include "ViewMain.h"
#include "display/ImageArea.h"
#include "keyboard/KeyDef.h"
#include "comment/MultiFuncArrow.h"
#include "display/KnobNone.h"
#include "comment/Arrow.h"

MultiFuncArrow * MultiFuncArrow::m_pThis = NULL;

enum EKnobArrow {ARROW_SHAPE, ARROW_SIZE, ARROW_COLOR};
#if (defined (EMP_322) || defined(EMP_313))
KnobMenu::KnobItem KnobArrowMenu[6] =
{
    {_("Shape"), "", MIN, MultiFuncArrow::SetShape, MultiFuncArrow::PressSetShape},
	{_("Size"), "", MIN, MultiFuncArrow::SetSize, MultiFuncArrow::PressSetSize},
	{_("Color"), "", MIN, MultiFuncArrow::SetColor, MultiFuncArrow::PressSetColor},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
};
#else
KnobMenu::KnobItem KnobArrowMenu[5] =
{
	{_("Shape"), "", MIN, MultiFuncArrow::SetShape, NULL},
	{_("Size"), "", MIN, MultiFuncArrow::SetSize, NULL},
	{_("Color"), "", MIN, MultiFuncArrow::SetColor, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
};
#endif

const char * arrowShapeName[2] =
{
	_("Simple Arrow"),
	_("Hollow Arrow")
};
const char * arrowSizeName[3] =
{
	_("Small"),
	_("Mid"),
	_("Big"),
};
const char * arrowColorName[6] =
{
	_("White"),
	_("Gray"),
	_("Red"),
	_("Green"),
	_("Yellow"),
	_("Blue"),
};

MultiFuncArrow::MultiFuncArrow()
:m_arrow(NULL), m_pressMode(CLICK), m_arrowOpr(ADD), m_knobItemBku(NULL)
{
	//init arrow pos
	g_keyInterface.Push(this);

	m_pos.x = IMAGE_W/2;
	m_pos.y = IMAGE_H/2;
	m_arrow = new CArrow(*this, m_pos);
	PRINTF("new arrow\n");

	m_timer = 0;
	m_pThis = this;
	KnobArrowCreate();
}

MultiFuncArrow::~MultiFuncArrow()
{
	g_keyInterface.Pop();

	if(m_arrow != NULL)
		delete m_arrow;
	m_arrow = NULL;
	m_pThis = NULL;
	//KnobMenu::GetInstance()->SetItem(m_knobItemBku, sizeof(m_knobItemBku)/sizeof(KnobMenu::KnobItem), m_knobTypeBku);
	KnobUndo();
}

void MultiFuncArrow::Do()
{
//	ASSERT(m_arrow != NULL);
	if(m_arrow == NULL)
		return;

	switch(m_pressMode)
	{
		case CLICK:
			{
				switch(m_arrowOpr)
				{
					case ADD:
						m_arrow->Add();
						break;

					case MODIFY:
						m_arrow->MoveEnd();
						m_arrowOpr = ADD;
						//hide hand cursor, add...
						InvisibleCursor(TRUE);
						break;

					default:
						break;
				}
			}
			break;

		case DBLCLK:
			if(m_arrow->MoveBegin())
			{
				//enter moving mode
				m_arrowOpr = MODIFY;

				// set system cursor to current pos
				m_pos.x = m_arrow->GetArrowPos().x;
				m_pos.y = m_arrow->GetArrowPos().y;
				//show hand cursor, add...
				InvisibleCursor(FALSE);
				SetSystemCursor(IMG_AREA_X + IMAGE_X + m_pos.x, IMG_AREA_Y + IMAGE_Y + m_pos.y);
				ChangeSystemCursor(GDK_HAND1);
			}
			break;

		default:
			break;
	}
}

/*
 *@brief delete arrow one by one from last
 *@retval if or not arrow list is empty
 */
void MultiFuncArrow::Undo()
{
	if(m_arrow == NULL)
		return;

	switch(m_arrowOpr)
	{
		case ADD:
			m_arrow->Delete();
			break;

		case MODIFY:
			m_arrow->UndoMove();
			m_arrowOpr = ADD;
			//hide hand cursor, add...
			InvisibleCursor(TRUE);
			break;

		default:
			break;
	}
}

void MultiFuncArrow::Value(EKnobOper opr)
{
	if(m_arrow == NULL)
		return;

	switch(opr)
	{
		case ::ADD:
			{
				CArrow::DIRECTION dir = m_arrow->GetArrowDir();
				if(dir == CArrow::DI23)
					m_arrow->SetArrowDir(CArrow::DI0);
				else
					m_arrow->SetArrowDir(CArrow::DIRECTION(dir+1));
			}
			break;

		case ::SUB:
			{
				CArrow::DIRECTION dir = m_arrow->GetArrowDir();
				if(dir == CArrow::DI0)
					m_arrow->SetArrowDir(CArrow::DI23);
				else
					m_arrow->SetArrowDir(CArrow::DIRECTION(dir-1));
			}
			break;

		default:
			break;
	}
}

void MultiFuncArrow::Mouse(int offsetX, int offsetY)
{
	if(m_arrow == NULL)
        return;

	m_pos = GetPoint(offsetX, offsetY);
	m_arrow->SetArrowPos(m_pos);
	if (m_arrowOpr == MODIFY)
		SetSystemCursor(IMG_AREA_X + IMAGE_X + m_pos.x, IMG_AREA_Y + IMAGE_Y + m_pos.y);
}

void MultiFuncArrow::Esc()
{
	if(m_arrowOpr == MODIFY) // clear arrow modified now
		Undo();

	MultiFuncUndo();

	if(m_pThis != NULL)
	{
		m_pThis = NULL;
	}
}

void MultiFuncArrow::DrawArrow(POINT pos, unsigned int direction, double scale,
		unsigned int color, unsigned int shape)
{
	if (shape == CArrow::SIMPLEARROW)
		ImageArea::GetInstance()->DrawArrowSimple(pos.x, pos.y, direction, CArrow::DIRECTION_MAX, scale, color);
	else
		ImageArea::GetInstance()->DrawArrowHollow(pos.x, pos.y, direction, CArrow::DIRECTION_MAX, scale, color);
}

gboolean MultiFuncArrow::Clicked()
{
	m_pressMode = CLICK;
	Do();
	m_timer = 0;
	return FALSE;
}

void MultiFuncArrow::KeyEvent(unsigned char keyValue)
{
	switch(keyValue)
	{
		case KEY_SET:
			if(m_timer > 0)
			{
				g_source_remove(m_timer);
				m_timer = 0;
				m_pressMode = DBLCLK;
				Do();
			}
			else
			{
				m_timer = g_timeout_add(250, HandleClicked, NULL);
			}
			break;

		case KEY_ESC:
            if (m_arrow->ArrowListEmpty())
                Esc();
            else
                Undo();
			break;

		case KEY_ARROW:
			Esc();
			break;

        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
#if (defined (EMP_322) || defined(EMP_313))
        case KEY_F6:
            ViewMain::GetInstance()->KnobKeyEvent(keyValue);
#else
            ViewMain::GetInstance()->KnobKeyEvent(keyValue);
#endif
            break;

#if (defined(EMP_460) || defined(EMP_355))
        case KEY_F1ADD:
		case KEY_F1SUB:
		case KEY_F2ADD:
		case KEY_F2SUB:
		case KEY_F3ADD:
		case KEY_F3SUB:
		case KEY_F4ADD:
		case KEY_F4SUB:
		case KEY_F5ADD:
		case KEY_F5SUB:
            {
                CKnobEvent::FEvent(keyValue);
            }
            break;
#endif

		default:
			switch(keyValue)
			{
#if not defined(EMP_322)
#if not defined(EMP_313)
				case KEY_PAGEUP:
				case KEY_PAGEDOWM:
#endif
#endif
				case KEY_UP:
				case KEY_DOWN:
				case KEY_LEFT:
				case KEY_RIGHT:
				case 0x0D:
					return;

				default:
					if(keyValue >= 0x20 && keyValue <= 0x7F)
						return;
					break;
			}
			Esc();
			ViewMain::GetInstance()->KeyEvent(keyValue);
			break;
	}
}

void MultiFuncArrow::KnobEvent(unsigned char keyValue, unsigned char offset)
{
	switch(keyValue)
	{
		case KNOB_VALUE:
			Value((EKnobOper)offset);
			break;

		default:
			CKnobEvent::KnobEvent(keyValue, offset);
			break;
	}
}

void MultiFuncArrow::MouseEvent(char offsetX, char offsetY)
{
	Mouse(offsetX, offsetY);
}

POINT MultiFuncArrow::GetPoint(int offsetX, int offsetY)
{
	if (((m_pos.x + offsetX) >= 0) &&((m_pos.x + offsetX) <= IMG_W))
		m_pos.x += offsetX;

	offsetY = -offsetY;
	if (((m_pos.y + offsetY) >= 0) &&((m_pos.y + offsetY) <= IMG_H))
		m_pos.y += offsetY;

	return m_pos;
}

void MultiFuncArrow::KnobArrowCreate()
{
	if(m_arrow == NULL)
		return;

	KnobMenu::GetInstance()->SetItem(KnobArrowMenu, sizeof(KnobArrowMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::ARROW);

	//shape
	CArrow::SHAPE shape = m_arrow->GetArrowShape();
	KnobMenu::GetInstance()->SetValue(ARROW_SHAPE, (char *)(arrowShapeName[shape]), GetKnobRetShape(shape));

	//size
	CArrow::SIZE size = m_arrow->GetArrowSize();
	KnobMenu::GetInstance()->SetValue(ARROW_SIZE, (char *)(arrowSizeName[size]), GetKnobRetSize(size));

	//color
	CArrow::COLOR color = m_arrow->GetArrowColor();
	KnobMenu::GetInstance()->SetValue(ARROW_COLOR, (char *)(arrowColorName[color]), GetKnobRetColor(color));
}

//emp5800 按键切换，改变箭头的形状
EKnobReturn MultiFuncArrow::PressSetShape(void)
{
    EKnobReturn ret = ERROR;
    if (m_pThis != NULL)
    {
        if (m_pThis->m_arrowOpr == MODIFY)
            return ERROR;

        CArrow::SHAPE shape = m_pThis->m_arrow->GetArrowShape();
        if (shape == CArrow::HOLLOWARROW)
        {
            shape = CArrow::SIMPLEARROW;
            m_pThis->m_arrow->SetArrowShape(shape);
        }
        else if(shape == CArrow::SIMPLEARROW)
            {
                shape = CArrow::HOLLOWARROW;
                m_pThis->m_arrow->SetArrowShape(shape);
            }
        ret = GetKnobRetShape(shape);
    KnobMenu::GetInstance()->SetValue(ARROW_SHAPE,(char *)(arrowShapeName[shape]),ret);
}
  return ret;
}
EKnobReturn MultiFuncArrow::SetShape(EKnobOper opr)
{
	EKnobReturn ret = ERROR;

	int temp = 0;

   	if(m_pThis != NULL)
	{
        if (m_pThis->m_arrowOpr == MODIFY)
            return ERROR;

		CArrow::SHAPE shape = m_pThis->m_arrow->GetArrowShape();
		temp = shape;

		switch(opr)
		{
            case ::ADD:
				if(shape != CArrow::HOLLOWARROW)
				{
					temp++;
					shape = (CArrow::SHAPE)temp;
					m_pThis->m_arrow->SetArrowShape(shape);
				}
				break;

            case ::SUB:
				if(shape != CArrow::SIMPLEARROW)
				{
					temp--;
					shape = (CArrow::SHAPE)temp;
					m_pThis->m_arrow->SetArrowShape(shape);
				}
				break;

			default:
                return ERROR;
				break;
		}

		ret = GetKnobRetShape(shape);

		if (shape < (int)sizeof(arrowShapeName))
			KnobMenu::GetInstance()->SetValue(ARROW_SHAPE, (char *)(arrowShapeName[shape]), ret);
	}

	return ret;
}

//按键切换，实现改变箭头的大小（当temp = 3,切换到小箭头
EKnobReturn MultiFuncArrow::PressSetSize(void)
{
   EKnobReturn ret = ERROR;
   int temp = 0;
   if (m_pThis != NULL)
   {
       if (m_pThis->m_arrowOpr == MODIFY)
           return ERROR;

       CArrow::SIZE size = m_pThis->m_arrow->GetArrowSize();
       temp = size;
       if (temp <= CArrow::BIG)
       {
           temp ++;
           size = (CArrow::SIZE)temp;
           m_pThis->m_arrow->SetArrowSize(size);
       }

       if(temp == 3)
       {
           size = CArrow::SMALL;
           m_pThis->m_arrow->SetArrowSize(size);
       }
	    ret = GetKnobRetSize(size);
	    KnobMenu::GetInstance()->SetValue(ARROW_SIZE,(char *)(arrowSizeName[size]),ret);
   }
   return ret;
 }

EKnobReturn MultiFuncArrow::SetSize(EKnobOper opr)
{
	EKnobReturn ret = ERROR;
	int temp = 0;

	if(m_pThis != NULL)
	{
        if (m_pThis->m_arrowOpr == MODIFY)
            return ERROR;

        CArrow::SIZE size = m_pThis->m_arrow->GetArrowSize();
		temp = size;

		switch(opr)
		{
			case ::ADD:
				if(size != CArrow::BIG)
				{
					temp++;
					size = (CArrow::SIZE)temp;
					m_pThis->m_arrow->SetArrowSize(size);
				}
				break;

			case ::SUB:
				if(size != CArrow::SMALL)
				{
					temp--;
					size = (CArrow::SIZE)temp;
					m_pThis->m_arrow->SetArrowSize(size);
				}
				break;

			default:
                return ERROR;
				break;
		}

		ret = GetKnobRetSize(size);

		if (size < (int)sizeof(arrowSizeName))
			KnobMenu::GetInstance()->SetValue(ARROW_SIZE, (char *)(arrowSizeName[size]), ret);
	}

	return ret;
}

//按键切换，实现箭头颜色改变。蓝/黄/绿/红/灰/白
EKnobReturn MultiFuncArrow::PressSetColor(void)
{
	EKnobReturn ret = ERROR;
   int temp = 0;
   if (m_pThis != NULL)
   {
        if (m_pThis->m_arrowOpr == MODIFY)
            return ERROR;

       CArrow::COLOR color = m_pThis->m_arrow->GetArrowColor();
       temp = color;
       if (temp <= CArrow::BLUE)
       {
           temp ++;
           color = (CArrow::COLOR)temp;
           m_pThis->m_arrow->SetArrowColor(color);
       }

       if(temp == 6)
       {
           color = CArrow::WHITE;
           m_pThis->m_arrow->SetArrowColor(color);
       }
	    ret = GetKnobRetColor(color);
	    KnobMenu::GetInstance()->SetValue(ARROW_COLOR,(char *)(arrowColorName[color]),ret);
   }
   return ret;

}
EKnobReturn MultiFuncArrow::SetColor(EKnobOper opr)
{
	EKnobReturn ret = ERROR;
	int temp = 0;

	if(m_pThis != NULL)
	{
        if (m_pThis->m_arrowOpr == MODIFY)
            return ERROR;

        CArrow::COLOR color = m_pThis->m_arrow->GetArrowColor();
        temp = color;

		switch(opr)
		{
			case ::ADD:
				if(color != CArrow::BLUE)
				{
					temp++;
					color = (CArrow::COLOR)temp;
					m_pThis->m_arrow->SetArrowColor(color);
				}
				break;

			case ::SUB:
				if(color != CArrow::WHITE)
				{
					temp--;
					color = (CArrow::COLOR)temp;
					m_pThis->m_arrow->SetArrowColor(color);
				}
				break;

			default:
                return ERROR;
				break;
		}

		ret = GetKnobRetColor(color);

		if (color < (int)sizeof(arrowColorName))
			KnobMenu::GetInstance()->SetValue(ARROW_COLOR, (char *)(arrowColorName[color]), ret);
	}

	return ret;
}

EKnobReturn MultiFuncArrow::GetKnobRetShape(CArrow::SHAPE shape)
{
	EKnobReturn ret = ERROR;

	if(shape == CArrow::SIMPLEARROW)
		ret = MIN;
	else if(shape == CArrow::HOLLOWARROW)
		ret = MAX;
	else
		ret = OK;

	return ret;
}
EKnobReturn MultiFuncArrow::GetKnobRetSize(CArrow::SIZE size)
{
	EKnobReturn ret = ERROR;

	if(size == CArrow::SMALL)
		ret = MIN;
	else if(size == CArrow::BIG)
		ret= MAX;
	else
		ret = OK;

	return ret;
}
EKnobReturn MultiFuncArrow::GetKnobRetColor(CArrow::COLOR color)
{
	EKnobReturn ret = ERROR;

	if(color == CArrow::WHITE)
		ret = MIN;
	else if(color == CArrow::BLUE)
		ret = MAX;
	else
		ret = OK;

	return ret;
}

gboolean MultiFuncArrow::HandleClicked(gpointer data)
{
    if(m_pThis == NULL)
    {
        return FALSE;
    }

    return m_pThis->Clicked();
}