
#include "UpdateMix.h"
#include "MenuArea.h"
#include "ImageAreaDraw.h"
#include "ModeStatus.h"
#include "KnobReplay.h"
#include "KnobNone.h"
#include "KeyValueOpr.h"
#include "LightDef.h"

const char * Toggle_Mix[2] = 
{
#if (defined(EMP_460) || defined(EMP_355))
    N_("Speed"),
    N_("Speed")
#else
    "OFF",
    "ON"
#endif
};


UpdateMix::UpdateMix()
{
	m_ptrMenu = MenuArea::GetInstance();
}

void UpdateMix::Freeze()
{
	KnobReplayCreate();

#if 0
	// Cleer LocalZoom Box
	ImageAreaDraw* ptrImgDraw = ImageAreaDraw::GetInstance();
	if(MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)
		ptrImgDraw->ClearLocalZoom();
#endif
}

void UpdateMix::UnFreeze()
{
	KnobUndo();
	MenuShowUndo();

	// update image
	ImageAreaDraw* ptrImgDraw = ImageAreaDraw::GetInstance();
	if(ModeStatus::IsColorImgMode())
		ptrImgDraw->ClearCfmBox();
	ptrImgDraw->ClearFocus();
    ptrImgDraw->Update();
    ptrImgDraw->DrawScaleY();
    ptrImgDraw->ReDrawFocus();

	if(ModeStatus::IsColorImgMode())
		ptrImgDraw->ReDrawCfmBox();
}

void UpdateMix::EnterMeasure()
{
	m_ptrMenu->ShowMeasureMenu();
#if defined(EMP_322)
    g_keyInterface.CtrlLight(TRUE,LIGHT_MEASURE);
#endif
}

void UpdateMix::ExitMeasure()
{
	MenuShowUndo();
#if defined(EMP_322)
    g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
#endif

}

void UpdateMix::EnterCalc()
{
#if defined(EMP_322)
    g_keyInterface.CtrlLight(TRUE,LIGHT_CALC);
#endif
	m_ptrMenu->ShowCalcMenu();
}

void UpdateMix::ExitCalc()
{
#if defined(EMP_322)
    g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
#endif
	MenuShowUndo();
}

void UpdateMix::EnterSystem(void)
{
	m_ptrMenu->ShowSystemMenu();
}

void UpdateMix::ExitSystem(void)
{
	MenuShowUndo();
}

void UpdateMix::ReplayBar(int cur, int total, int trimLeft, int trimRight)
{
    //printf("updateMix repayBar total = %d trimleft = %d trimright = %d\n",total,trimLeft,trimRight);
	ImageAreaDraw::GetInstance()->DrawReplayBar(cur, total, trimLeft, trimRight);
}

void UpdateMix::ClearReplayBar()
{
	ImageAreaDraw::GetInstance()->ClearReplayBar();
}

void UpdateMix::ReplayCtrl(bool on)
{
   // printf("ReplayCtrl value = %d\n",on);
    sprintf(m_str, "%s", Toggle_Mix[on]);
	SyncKnobReplay(REPLAY_CTRL, m_str, PRESS);	
}
#if (defined (EMP_322) || defined(EMP_313))
void UpdateMix::SpeedCtrl(int indexSpeed)
{
    sprintf(m_str, "%d", indexSpeed);
    SyncKnobReplay(REPLAY_SPEED, m_str, PRESS);	
}
#endif
void UpdateMix::ReplayCtrl(bool on, int indexSpeed, EKnobReturn status)
{
    sprintf(m_str, "%s: %d", Toggle_Mix[on], indexSpeed);
	SyncKnobReplay(REPLAY_CTRL, m_str, status);	
}

void UpdateMix::ReplayTrimLeft(int data)
{
	sprintf(m_str, "%d", data);
	SyncKnobReplay(REPLAY_TRIM_LEFT, m_str, PRESS);	
}
void UpdateMix::ReplayTrimRight(int data)
{
	sprintf(m_str, "%d", data);
	SyncKnobReplay(REPLAY_TRIM_RIGHT, m_str, PRESS);	
}

