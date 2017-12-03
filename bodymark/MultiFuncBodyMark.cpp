
#include  "MultiFuncBodyMark.h"
#include  "KeyValueOpr.h"
#include  "ViewMain.h"
#include  "KeyDef.h"
#include  "KeyFunc.h"
#include  "KnobNone.h"
#include "gui_global.h"
#include "ImageArea.h"
#include "KnobMenu.h"
#include "HintArea.h"
#include "../keyboard/LightDef.h"
#include "SysNoteSetting.h"
#include "ModeStatus.h"

MultiFuncBodyMark* MultiFuncBodyMark::m_ptrThis = NULL;
enum EKnobBodymark {BDMK_BODYSIZE, BDMK_PROBECOLOR};
#if (defined (EMP_322) || defined(EMP_313))
KnobMenu::KnobItem KnobBodymarkMenu[6] = 
{
	{_("BodyMark Size"), "", MIN, MultiFuncBodyMark::SetBodyMarkSize, MultiFuncBodyMark::PressSetBodyMarkSize},
	{_("ProbeMark Color"), "", MIN, MultiFuncBodyMark::SetProbeMarkColor, MultiFuncBodyMark::PressSetProbeMarkColor},
	{_("Hide BodyMark"), "Press", PRESS, NULL,MultiFuncBodyMark::PressSetHideBodyMark},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL}
};
#elif (defined(EMP_460) || defined(EMP_355))
KnobMenu::KnobItem KnobBodymarkMenu[5] = 
{
	{_("BodyMark Size"), "", MIN, MultiFuncBodyMark::SetBodyMarkSize, NULL},
	{_("ProbeMark Color"), "", MIN, MultiFuncBodyMark::SetProbeMarkColor, NULL},
	{_("Hide BodyMark"), "Press", PRESS,MultiFuncBodyMark::PressSetHideBodyMark, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL}
};
#else
KnobMenu::KnobItem KnobBodymarkMenu[5] = 
{
	{_("BodyMark Size"), "", MIN, MultiFuncBodyMark::SetBodyMarkSize, NULL},
	{_("ProbeMark Color"), "", MIN, MultiFuncBodyMark::SetProbeMarkColor, NULL},
	{_("Hide BodyMark"), "Press", PRESS, NULL,MultiFuncBodyMark::PressSetHideBodyMark},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL}
};
#endif

const char * bodyMarkSizeName[3] =
{
	_("Small"),
	_("Mid"),
	_("Big"),
};
const char * bodyMarkColorName[6] =
{
	_("White"),
	_("Gray"),
	_("Red"),
	_("Green"),
	_("Yellow"),
	_("Blue"),
};

MultiFuncBodyMark::MultiFuncBodyMark()
{
	// add message
	g_keyInterface.Push(this);

	// create bodymark
	m_probePosOffset.x = BDMK_W / 2;
	m_probePosOffset.y = BDMK_H / 2;
	m_ptrBodyMark  = NULL;
	//m_ptrBodyMark = new BodyMark(m_bodyPos, m_probePos);	
	for(int i=0; i<4; i++)
	{
		m_bodyPos[i].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
		m_bodyPos[i].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
		m_probePos[i].x = m_probePosOffset.x;
		m_probePos[i].y = m_probePosOffset.y;
		//m_firstDraw[i] = TRUE;
	}
	ModeStatus ms;
	if(ms.IsD2Mode() || ms.IsCFMMode() || ms.IsPDIMode())
		m_active = ms.Get2DCurrentB();
	else
		m_active = 0;

	// update view
	CUpdateBodyMark::EnterBodyMark();
		
    m_bodyOpr = IDEL;
    m_ptrThis = this;
    KnobBodyMarkCreate();
    DarkFucusLight();
#if defined (EMP_322)
    g_keyInterface.CtrlLight(TRUE,LIGHT_BODYMARK);
#endif
}

MultiFuncBodyMark::~MultiFuncBodyMark()
{
	if (m_ptrBodyMark != NULL)
	{
		delete m_ptrBodyMark;
		m_ptrBodyMark = NULL;
	}
    g_keyInterface.Pop();
#if defined (EMP_322)
   g_keyInterface.CtrlLight(FALSE,LIGHT_BODYMARK);
#endif
   KnobUndo();
}

void MultiFuncBodyMark::SetNewBodyMark(GdkPixbuf* pixbuf)
{
	if (m_ptrThis != NULL)
	{
		m_ptrThis->AddNewBodyMark(pixbuf);
		m_ptrThis->BeginMoveProbeMark();
	}
}
// event
void MultiFuncBodyMark::KeyEvent(unsigned char keyValue)
{	
    if (m_ptrBodyMark == NULL)
		m_ptrBodyMark = new BodyMark(m_bodyPos[m_active], m_probePos[m_active]);	
	switch(keyValue)
	{
#if not defined(EMP_322)
#if not defined(EMP_313)
		case KEY_UPDATE: 
			if (m_bodyOpr == MOVE_PROBE)
			{
				EndMoveProbeMark();
			}
			else if(m_bodyOpr == ADD)
			{
				BeginMoveProbeMark();
			}
			break;
#endif
#endif
		case KEY_SET:
			if (m_bodyOpr == MOVE_PROBE)
			{
				//End ProbeMark Move and Enter BodyMark Move
				EndMoveProbeMark();
				DClicked();
#if 0
				if (m_timer > 0) // double clicked
				{
					g_source_remove(m_timer);
					m_timer = 0;
					DClicked();
				}
				else
				{
					m_timer = g_timeout_add(250, HandleClicked, NULL);
				}
#endif
			}
			else if (m_bodyOpr == MOVE_BODY)
			{
				EndMoveBodyMark();
			}
			else if ((m_bodyOpr == IDEL) || (m_bodyOpr == ADD))
			{
				FakeXEvent::KeyEvent(keyValue);
			}
			break;

		case KEY_CLEAR:
			if ((m_bodyOpr == IDEL) || (m_bodyOpr == ADD))
			{
				KeyClearScreen kcs;
				kcs.Execute();
			}
			break;

		case KEY_BODYMARK:
			Esc();
			break;

        case KEY_CURSOR:
            if(m_bodyOpr == MOVE_BODY || m_bodyOpr == MOVE_PROBE)
            {
                EndMoveBodyMark();
            }
            break;

        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
            ViewMain::GetInstance()->KnobKeyEvent(keyValue);
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
            Esc();
            ViewMain::GetInstance()->KeyEvent(keyValue);
            break;

    }
}
void MultiFuncBodyMark::KnobEvent(unsigned char keyValue, unsigned char offset)
{
    if (m_ptrBodyMark == NULL)
		m_ptrBodyMark = new BodyMark(m_bodyPos[m_active], m_probePos[m_active]);	
	
    CKnobEvent ke;
	switch(keyValue)
	{
		case KNOB_VALUE:
			Value((EKnobOper)offset);
			break;

		default:
			ke.KnobEvent(keyValue, offset);
			break;
	}
}
void MultiFuncBodyMark::MouseEvent(char offsetX, char offsetY)
{
	if (m_bodyOpr == MOVE_PROBE)
	{
		if (m_ptrBodyMark == NULL)
			return;

		m_probePos[m_active] = GetPointProbe(offsetX, offsetY);
		m_ptrBodyMark->SetProbeMarkPos(m_probePos[m_active], m_active);
	}
	else if (m_bodyOpr == MOVE_BODY)
	{
		if (m_ptrBodyMark == NULL)
			return;

		m_bodyPos[m_active] = GetPointBody(offsetX, offsetY);
		m_probePos[m_active].x = m_probePosOffset.x;
		m_probePos[m_active].y = m_probePosOffset.y;

		m_ptrBodyMark->MoveBody(m_bodyPos[m_active], m_probePos[m_active], m_active);

		double scale = m_ptrBodyMark->GetBodyMarkScale();
		int x = IMG_AREA_X + IMAGE_X + m_bodyPos[m_active].x + BDMK_W * scale / 2;
		int y = IMG_AREA_Y + IMAGE_Y + m_bodyPos[m_active].y + BDMK_H * scale / 2;
		SetSystemCursor(x, y);
	}
	else if ((m_bodyOpr == IDEL) || (m_bodyOpr == ADD))
	{
		FakeXEvent::MouseEvent(offsetX, offsetY);	
	}
}

void MultiFuncBodyMark::KnobBodyMarkCreate()
{
#if 0
    if(m_ptrBodyMark == NULL)
		return;
#endif
	KnobMenu::GetInstance()->SetItem(KnobBodymarkMenu, sizeof(KnobBodymarkMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::BDMK);

    m_ptrBodyMark->GetBodyMarkKnobValue();
	//size
	BodyMark::SIZE size = m_ptrBodyMark->GetBodyMarkSize();
	KnobMenu::GetInstance()->SetValue(BDMK_BODYSIZE, (char *)(bodyMarkSizeName[size]), GetKnobRetBodyMarkSize(size));

	//color
	BodyMark::COLOR color = m_ptrBodyMark->GetProbeMarkColor();
	KnobMenu::GetInstance()->SetValue(BDMK_PROBECOLOR, (char *)(bodyMarkColorName[color]), GetKnobRetProbeMarkColor(color));
}

///> private

void MultiFuncBodyMark::Value(EKnobOper opr)
{
	if (m_ptrBodyMark != NULL)
		m_ptrBodyMark->ChangeProbeMarkDir(opr, m_active);
}

POINT MultiFuncBodyMark::GetPointProbe(int offsetX, int offsetY)
{
	double scaleProbe = m_ptrBodyMark->GetProbeMarkScale();
	int probeW = BDMK_PROBE_LEN * scaleProbe / 2;
	int w;
	int h;

	GetBodyMarkSize(w, h);
	PRINTF("-------w= %d, h = %d, offsetx = %d, offsety = %d\n", w, h, offsetX, offsetY);

	if (((m_probePosOffset.x + offsetX) >= probeW) &&((m_probePosOffset.x + offsetX) <= (w - probeW)))
		m_probePosOffset.x += offsetX;

	offsetY = - offsetY;
	if (((m_probePosOffset.y + offsetY) >= probeW) &&((m_probePosOffset.y + offsetY) <= (h - probeW)))
		m_probePosOffset.y += offsetY;

	m_probePos[m_active].x = m_probePosOffset.x;
	m_probePos[m_active].y = m_probePosOffset.y;

	return m_probePos[m_active];
}

POINT MultiFuncBodyMark::GetPointBody(int offsetX, int offsetY)
{
#if 0
	double scale = m_ptrBodyMark->GetBodyMarkScale();
	int w = BDMK_W * scale;
	int h = BDMK_H * scale;

	if (((m_bodyPos[m_active].x + offsetX) > 0) &&((m_bodyPos[m_active].x + offsetX) <= IMG_W - w))
		m_bodyPos[m_active].x += offsetX;

	offsetY = -offsetY;
	if (((m_bodyPos[m_active].y + offsetY) > 0) &&((m_bodyPos[m_active].y + offsetY) <= IMG_H - h))
		m_bodyPos[m_active].y += offsetY;
#endif
	int w, h;
	GetBodyMarkSize(w, h);

	if (((m_bodyPos[m_active].x + offsetX) > m_bodyPosRange.x) && 
		((m_bodyPos[m_active].x + offsetX) <= m_bodyPosRange.x + m_bodyPosRange.width - w))
		m_bodyPos[m_active].x += offsetX;

	offsetY = -offsetY;
	if (((m_bodyPos[m_active].y + offsetY) > m_bodyPosRange.y) && 
		((m_bodyPos[m_active].y + offsetY) <= m_bodyPosRange.y + m_bodyPosRange.height - h))
		m_bodyPos[m_active].y += offsetY;

	return m_bodyPos[m_active];
}

void MultiFuncBodyMark::EndMoveProbeMark()
{
	m_ptrThis->m_bodyOpr = ADD;

	// display system cursor 
	InvisibleCursor(FALSE);
}
void MultiFuncBodyMark::EndMoveBodyMark()
{
	m_ptrThis->m_bodyOpr = ADD;

	// display system cursor 
	InvisibleCursor(FALSE);

	if (m_ptrBodyMark != NULL)
		m_ptrBodyMark->MoveBodyEnd(m_active);
}
void MultiFuncBodyMark::UndoMoveBodyMark()
{
	m_ptrThis->m_bodyOpr = ADD;

	// display system cursor 
	InvisibleCursor(FALSE);

	if (m_ptrBodyMark != NULL)
		m_ptrBodyMark->UndoMoveBody(m_active);
}
void MultiFuncBodyMark::Esc()
{
	if (m_bodyOpr == MOVE_BODY)
		EndMoveBodyMark();
		//UndoMoveBodyMark();

	g_keyInterface.Pop();

	MultiFuncUndo();

#if 0
	if (m_ptrBodyMark != NULL)
	{
		PRINTF("(%s:%d): delete m_ptrBodyMark = 0x%x\n", __FILE__, __LINE__, (char*)m_ptrBodyMark);
		delete m_ptrBodyMark;
		m_ptrBodyMark = NULL;
	}
#endif
	
	CUpdateBodyMark::ExitBodyMark();
}

void MultiFuncBodyMark::Hide()
{
	m_bodyOpr = IDEL;

	// display system cursor 
	InvisibleCursor(FALSE);
	
	m_ptrBodyMark->HideBodyMark();
}

bool MultiFuncBodyMark::Clicked()
{
	EndMoveProbeMark();
	m_timer = 0;

	return FALSE;
}

bool MultiFuncBodyMark::DClicked()
{
	if (m_ptrBodyMark != NULL)
	{
		m_bodyOpr = MOVE_BODY;
		m_ptrBodyMark->MoveBodyBegin(m_active);
		InvisibleCursor(FALSE);

		double scale = m_ptrBodyMark->GetBodyMarkScale();
		int x = IMG_AREA_X + IMAGE_X + m_bodyPos[m_active].x + BDMK_W * scale / 2;
		int y = IMG_AREA_Y + IMAGE_Y + m_bodyPos[m_active].y + BDMK_H * scale / 2;
		SetSystemCursor(x, y);
		ChangeSystemCursor(GDK_HAND1);
	}

	return TRUE;
}

//>private
void MultiFuncBodyMark::AddNewBodyMark(GdkPixbuf* pixbuf)
{
	if (m_ptrBodyMark != NULL)
	{
		ModeStatus ms;
		Format2D::EFormat2D format2D = ms.GetFormat2D();
		FormatCfm::EFormatCfm formatCfm = ms.GetFormatCfm();
		// BB Mode
		if( (ms.IsD2Mode() && format2D == Format2D::BB)
				|| ((ms.IsCFMMode() || ms.IsPDIMode()) && formatCfm == FormatCfm::BB) )
		{
			if(m_active == 0)
			{
				m_bodyPos[m_active].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE;
				m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
				m_bodyPosRange.x = 0;
				m_bodyPosRange.y = 0;
			}
			else
			{
				m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
				m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
				m_bodyPosRange.x = IMG_W / 2;
				m_bodyPosRange.y = 0;
			}
			m_bodyPosRange.width = IMG_W / 2;
			m_bodyPosRange.height = IMG_H;
		}
		// 4B Mode
		else if( (ms.IsD2Mode() && format2D == Format2D::B4)
				|| ((ms.IsCFMMode() || ms.IsPDIMode()) && formatCfm == FormatCfm::B4) )
		{
			if(m_active == 0)
			{
				m_bodyPos[m_active].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE;
				m_bodyPos[m_active].y = IMG_H / 2 - BDMK_H * BDMK_MAX_SCALE;
				m_bodyPosRange.x = 0;
				m_bodyPosRange.y = 0;
			}
			else if(m_active == 1)
			{
				m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
				m_bodyPos[m_active].y = IMG_H / 2 - BDMK_H * BDMK_MAX_SCALE;
				m_bodyPosRange.x = IMG_W / 2;
				m_bodyPosRange.y = 0;
			}
			else if(m_active == 2)
			{
				m_bodyPos[m_active].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE;
				m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
				m_bodyPosRange.x = 0;
				m_bodyPosRange.y = IMG_H / 2;
			}
			else
			{
				m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
				m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
				m_bodyPosRange.x = IMG_W / 2;
				m_bodyPosRange.y = IMG_H / 2;
			}
			m_bodyPosRange.width = IMG_W / 2;
			m_bodyPosRange.height = IMG_H / 2;
		}
		//Single B Mode
		else
		{
			m_active = 0; 
			m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
			m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
			m_bodyPosRange.x = 0;
			m_bodyPosRange.y = 0;
			m_bodyPosRange.width = IMG_W;
			m_bodyPosRange.height = IMG_H;
		}

		m_ptrBodyMark->DisplayNewBodyMark(pixbuf, m_bodyPos[m_active], m_probePos[m_active], m_bodyPosRange, m_active, TRUE);//m_firstDraw[m_active]);

#if 0
		if (m_firstDraw[m_active])
			m_firstDraw[m_active] = FALSE;
#endif
	}
}

void MultiFuncBodyMark::BeginMoveProbeMark()
{
//	ASSERT(m_ptrThis != NULL);
	if(m_ptrThis != NULL)
	{
		m_ptrThis->m_bodyOpr = MOVE_PROBE;

		// hide system cursor 
		InvisibleCursor(TRUE);
	}
}

void MultiFuncBodyMark::GetBodyMarkSize(int &w, int &h)
{
	double scale = m_ptrBodyMark->GetBodyMarkScale();
	
	w = BDMK_W * scale;
	h = BDMK_H * scale;
}

void MultiFuncBodyMark::ReviseProbePos(double scale)
{
	int w, h;

	m_probePosOffset.x = m_probePosOffset.x * scale;
	m_probePosOffset.y = m_probePosOffset.y * scale;

	GetBodyMarkSize(w, h);
	if (m_probePosOffset.x > w)
		m_probePosOffset.x = w / 2;

	if (m_probePosOffset.y > h)
		m_probePosOffset.y = h / 2;

	m_probePos[m_active].x = m_probePosOffset.x;
	m_probePos[m_active].y = m_probePosOffset.y;

	if (m_ptrBodyMark != NULL)
		m_ptrBodyMark->SetProbeMarkPos(m_probePos[m_active], m_active);
}

//按键切换，改变体标索引图的大小
EKnobReturn MultiFuncBodyMark::PressSetBodyMarkSize(void)
{
	EKnobReturn ret = ERROR;
	int temp = 0;

	if((m_ptrThis != NULL) && (m_ptrThis->m_ptrBodyMark != NULL))
	{
        if (!m_ptrThis->m_ptrBodyMark->GetBodyMarkStatus(m_ptrThis->m_active))
        {
            HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);
            return ERROR;
        }
        BodyMark::SIZE size = m_ptrThis->m_ptrBodyMark->GetBodyMarkSize();
		double old_scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();
        temp = size;
        if(temp < BodyMark::BIG)
        {
            temp++;
            size = (BodyMark::SIZE)temp;
        }
        else
        {
            size = BodyMark::SMALL;
        }
		m_ptrThis->m_bodyPos[m_ptrThis->m_active] = m_ptrThis->m_ptrBodyMark->SetBodyMarkSize(size, m_ptrThis->m_active);
		double new_scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();
		m_ptrThis->ReviseProbePos(new_scale/old_scale);

        double scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();
        int x = IMG_AREA_X + IMAGE_X + m_ptrThis->m_bodyPos[m_ptrThis->m_active].x + BDMK_W * scale / 2;
        int y = IMG_AREA_Y + IMAGE_Y + m_ptrThis->m_bodyPos[m_ptrThis->m_active].y + BDMK_H * scale / 2;
        SetSystemCursor(x, y);

        SysNoteSetting sys;
        sys.SetBodyMarkSize(size);
        sys.SyncFile();

        ret = GetKnobRetBodyMarkSize(size);
        KnobMenu::GetInstance()->SetValue(BDMK_BODYSIZE, (char *)(bodyMarkSizeName[size]), ret);
    }

	return ret;
}

EKnobReturn MultiFuncBodyMark::SetBodyMarkSize(EKnobOper opr)
{
	EKnobReturn ret = ERROR;
	int temp = 0;

	if((m_ptrThis != NULL) && (m_ptrThis->m_ptrBodyMark != NULL))
	{
		if (!m_ptrThis->m_ptrBodyMark->GetBodyMarkStatus(m_ptrThis->m_active))
		{
			HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);
			return ERROR;
		}
		BodyMark::SIZE size = m_ptrThis->m_ptrBodyMark->GetBodyMarkSize();
		temp = size;
		double old_scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();

		switch(opr)
		{
			case ADD:
				if(size != BodyMark::BIG)
				{
					temp++;
					size = (BodyMark::SIZE)temp;
					m_ptrThis->m_bodyPos[m_ptrThis->m_active] = m_ptrThis->m_ptrBodyMark->SetBodyMarkSize(size, m_ptrThis->m_active);
					double new_scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();
					m_ptrThis->ReviseProbePos(new_scale/old_scale);
				}
				break;

			case SUB:
				if(size != BodyMark::SMALL)
				{
					temp--;
					size = (BodyMark::SIZE)temp;
					m_ptrThis->m_bodyPos[m_ptrThis->m_active] = m_ptrThis->m_ptrBodyMark->SetBodyMarkSize(size, m_ptrThis->m_active);
					double new_scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();
					m_ptrThis->ReviseProbePos(new_scale/old_scale);
				}
				break;

			default:
                return ERROR;
				break;
		}	

		double scale = m_ptrThis->m_ptrBodyMark->GetBodyMarkScale();
		int x = IMG_AREA_X + IMAGE_X + m_ptrThis->m_bodyPos[m_ptrThis->m_active].x + BDMK_W * scale / 2;
		int y = IMG_AREA_Y + IMAGE_Y + m_ptrThis->m_bodyPos[m_ptrThis->m_active].y + BDMK_H * scale / 2;
		SetSystemCursor(x, y);

		SysNoteSetting sys;

		sys.SetBodyMarkSize(size);
		sys.SyncFile();

		ret = GetKnobRetBodyMarkSize(size);

		if (size < (int)sizeof(bodyMarkSizeName)) 
			KnobMenu::GetInstance()->SetValue(BDMK_BODYSIZE, (char *)(bodyMarkSizeName[size]), ret);
	}

	return ret;
}

EKnobReturn MultiFuncBodyMark::PressSetProbeMarkColor(void)
{
    EKnobReturn ret = ERROR;
    int temp = 0;

    if((m_ptrThis != NULL) && (m_ptrThis->m_ptrBodyMark != NULL))
    {
        if (!m_ptrThis->m_ptrBodyMark->GetBodyMarkStatus(m_ptrThis->m_active))
        {
            HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);
            return ERROR;
        }

        BodyMark::COLOR color = m_ptrThis->m_ptrBodyMark->GetProbeMarkColor();	
        temp = color;
        if(temp < BodyMark::BLUE)
        {
            temp++;
            color = (BodyMark::COLOR)temp;
            m_ptrThis->m_ptrBodyMark->SetProbeMarkColor(color, m_ptrThis->m_active);
        }
        else
        {
            color = BodyMark::WHITE;
            m_ptrThis->m_ptrBodyMark->SetProbeMarkColor(color, m_ptrThis->m_active);
        }
        SysNoteSetting sys;
        sys.SetBodyMarkColor(color);
        sys.SyncFile();

        ret = GetKnobRetProbeMarkColor(color);
        KnobMenu::GetInstance()->SetValue(BDMK_PROBECOLOR, (char *)(bodyMarkColorName[color]), ret);
    }

	return ret;

}

EKnobReturn MultiFuncBodyMark::PressSetHideBodyMark(EKnobOper oper)
{
	BodyMark::HideBodyMark();
    return OK;
}

EKnobReturn MultiFuncBodyMark::PressSetHideBodyMark(void)
{
	BodyMark::HideBodyMark();
    return OK;
}

EKnobReturn MultiFuncBodyMark::SetProbeMarkColor(EKnobOper opr)
{
	EKnobReturn ret = ERROR;
	int temp = 0;

	if((m_ptrThis != NULL) && (m_ptrThis->m_ptrBodyMark != NULL))
	{
		if (!m_ptrThis->m_ptrBodyMark->GetBodyMarkStatus(m_ptrThis->m_active))
		{
			HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);
			return ERROR;
		}

		BodyMark::COLOR color = m_ptrThis->m_ptrBodyMark->GetProbeMarkColor();
	
		temp = color;

		switch(opr)
		{
            case ::ADD:
				if(color != BodyMark::BLUE)
				{
					temp++;
					color = (BodyMark::COLOR)temp;
					m_ptrThis->m_ptrBodyMark->SetProbeMarkColor(color, m_ptrThis->m_active);
				}
				break;

            case ::SUB:
				if(color != BodyMark::WHITE)
				{
					temp--;
					color = (BodyMark::COLOR)temp;
					m_ptrThis->m_ptrBodyMark->SetProbeMarkColor(color, m_ptrThis->m_active);
				}
				break;

			default:
                return ERROR;
				break;
		}	

		ret = GetKnobRetProbeMarkColor(color);

		SysNoteSetting sys;
		sys.SetBodyMarkColor(color);
		sys.SyncFile();

		if (color < (int)sizeof(bodyMarkColorName)) 
			KnobMenu::GetInstance()->SetValue(BDMK_PROBECOLOR, (char *)(bodyMarkColorName[color]), ret);
	}

	return ret;

}

EKnobReturn MultiFuncBodyMark::GetKnobRetBodyMarkSize(BodyMark::SIZE size)
{
	EKnobReturn ret = ERROR;
	
	if(size == BodyMark::SMALL)
		ret = MIN;
	else if(size == BodyMark::BIG)
		ret= MAX;
	else
		ret = OK;

	return ret;
}

EKnobReturn MultiFuncBodyMark::GetKnobRetProbeMarkColor(BodyMark::COLOR color)
{
	EKnobReturn ret = ERROR;

	if(color == BodyMark::WHITE)
		ret = MIN;
	else if(color == BodyMark::BLUE)
		ret = MAX;
	else
		ret = OK;

	return ret;
}

#if 0
/*
 * called when mode changed
 */
void MultiFuncBodyMark::RedrawWhenModeChanged(void)
{
	if((m_ptrThis == NULL) || (m_ptrThis->m_ptrBodyMark == NULL))
		return;

	guint active = Format2D::GetInstance()->GetCurrentB();
	ImageArea::GetInstance()->RedrawBodyMarkWhenModeChanged();

#if 0
	POINT bodyPos = m_ptrThis->m_bodyPos[m_active];
	POINT probePos = m_ptrThis->m_probePos[m_active];
	bool firstDraw = m_ptrThis->m_firstDraw[m_active];

	for(int i=0; i<4; i++)
	{
		m_ptrThis->m_bodyPos[i].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
		m_ptrThis->m_bodyPos[i].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
		m_ptrThis->m_probePos[i].x = m_ptrThis->m_probePosOffset.x;
		m_ptrThis->m_probePos[i].y = m_ptrThis->m_probePosOffset.y;
		m_ptrThis->m_firstDraw[i] = TRUE;
	}

	m_active = 0;
	m_bodyPos[m_active] = bodyPos;
	m_probePos[m_active] = probePos;
	m_firstDraw[m_active] = firstDraw;
#endif
}
#endif
