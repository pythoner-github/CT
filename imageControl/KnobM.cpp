/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: KnobM.cpp
 * @brief: knob in 2D mode
 *
 * version: V1.0
 * date: 2009-8-10
 * @author: zhanglei
 */

#include "KnobM.h"
#include "Img2D.h"
#include "KnobMenu.h"
#include "gui_global.h"
#include "../imageProc/ImgProc2D.h"
#include "../imageProc/ModeStatus.h"
#include "../display/HintArea.h"
#include "KeyFunc.h"

#if defined (EMP_322) || defined(EMP_313)
KnobMenu::KnobItem KnobMMenu[6] = {
//	{_("Freq."), "", MIN, MChgFreq, NULL},
    {_("M Speed"), "", MIN, MChgSpeed, MPressChgSpeed},
 //   {_("【Chroma】"), "", MIN, MChgChroma, MPressChroma},
    {N_("MBP"),"", MIN, NULL, MPressChgMBP},
    {N_("Sound Power"),"", MIN, NULL, MPressChgSoundPower},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},

};
KnobMenu::KnobItem KnobAnatomicMMenu[6] = {
//	{_("Freq."), "", MIN, MChgFreq, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},

};
#else
KnobMenu::KnobItem KnobMMenu[5] = {
	{_("Freq."), "", MIN, MChgFreq, MPressChgStatusTHI},
	{_("M Speed"), "", MIN, MChgSpeed, NULL},
    {_("【Chroma】"), "", MIN, MChgChroma, MPressChroma},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
};	
KnobMenu::KnobItem KnobAnatomicMMenu[5] = {
	{_("Freq."), "", MIN, MChgFreq, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
	{"", "", ERROR, NULL, NULL},
};	
#endif

void KnobMCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobMMenu, sizeof(KnobMMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::M);   
}

void KnobAnatomicMCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobAnatomicMMenu, sizeof(KnobAnatomicMMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::ANATOMIC_M);   
}

///> knob menu need to be sync
void SyncKnobM(EKnobM type, const char* s, EKnobReturn status, bool draw)
{
    sprintf(KnobMMenu[type].value, "%s", s);
	KnobMMenu[type].status = status;
#if not defined(EMP_322)
    if (type == M_FREQ)
    {
        sprintf(KnobAnatomicMMenu[type].value, "%s", s);
        KnobAnatomicMMenu[type].status = status;
    }
#endif
    if (draw)
        KnobMenu::GetInstance()->Update();
}

///> callback function
EKnobReturn MChgFreq(EKnobOper oper)
{	
	EKnobReturn ret;
    bool status = Img2D::GetInstance()->GetStatusTHI();
    //printf("status:%d\n", status);

    if(status)
        ret = Img2D::GetInstance()->ChangeD2HarmonicFreq(oper);
    else
        ret = Img2D::GetInstance()->ChangeFreq(oper);

    // change Tis
	ChangeTis();

	return ret;
    //return Img2D::GetInstance()->ChangeFreq(oper);                //M模式下也应该由谐波
}

EKnobReturn MPressChgStatusTHI(void)
{
    EKnobReturn ret;
    ret = ERROR;
    Img2D::GetInstance()->ChangeHarmonicStatus(ROTATE);

    return ret;
}


//emp5800 按键切换，改变M模式下的速度（0 --- 7）
EKnobReturn MPressChgSpeed(void)
{
    EKnobOper oper;
    oper = ROTATE;
    EKnobReturn ret = ERROR;
    ret = Img2D::GetInstance()->ChangeMSpeed(oper);
    return ret;
}

EKnobReturn MChgSpeed(EKnobOper oper)
{
    return Img2D::GetInstance()->ChangeMSpeed(oper);
}
EKnobReturn MChgChroma(EKnobOper oper)
{
    return ImgProc2D::GetInstance()->ChangeChroma(oper);
}

EKnobReturn MPressChroma(void)
{
	return ImgProc2D::GetInstance()->PressChroma();
}

// change mbp
EKnobReturn MPressChgMBP(void)
{
    if (ModeStatus::IsEFOVMode() || (!ModeStatus::IsUnFreezeMode()))
    {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        return ERROR;
    }

    return Img2D::GetInstance()->ChangeMBP(ROTATE);
 }

//change sound power
EKnobReturn MPressChgSoundPower(void)
{
    EKnobOper oper = ROTATE;
    EKnobReturn ret = ERROR;
    ret = Img2D::GetInstance()->ChangeSoundPower(oper);
    return ret;

}

