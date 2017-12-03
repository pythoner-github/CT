/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: UserDefineKey
 * @brief: manage userDefine key, realize key p1 and p2 functin acording to system setting 
 *
 * version: V1.0
 * date: 2010-5-28
 * @author: zhanglei
 */

#include "UserDefineKey.h"
#include "gui_global.h"
#include "Def.h"
#include "Img2D.h"
#include "ImgPw.h"
#include "KnobReplay.h"
#include "Knob2D.h"
#include "ModeStatus.h"
#include "HintArea.h"
#include "Menu2D.h"
#include "SysUserDefinedKey.h"
#include "PrintScreen.h"
#include "../imageControl/Knob2D.h"
#include "../sysMan/ViewSystem.h"
#include "../display/MenuArea.h"
#include "../keyboard/KeyFunc.h" //2016.09.07

#if defined(EMP_322)
const string UserDefineKey::KeyFunctionList[MAX_KEY] = {
	N_("NULL"),
	//N_("THI"),
	N_("TSI"),
	//N_("Save Image"),
	N_("Save Video"),
	//N_("Send DCM Image"),
	N_("Print Screen"),
	//N_("Steer"),
	N_("PIP"),
	N_("Biopsy")
	//N_("Spectrum Sound"),
	//N_("HPRF"),
	//N_("Display Biopsy Line"),
	//N_("Center Line"),
	//N_("Auto Trace"),
	//N_("ECG"),
};
#else
const string UserDefineKey::KeyFunctionList[MAX_KEY] = {
	N_("NULL"),
	//N_("THI"),
	N_("TSI"),
	//N_("Save Image"),
	N_("Save Video"),
	//N_("Send DCM Image"),
	N_("Print Screen"),
	N_("Steer"),
	N_("PIP"),
	N_("Spectrum Sound"),
	N_("HPRF"),
    N_("Image Preset"),
	N_("Biopsy"),
	//N_("Center Line"),
	//N_("Auto Trace"),
	//N_("ECG"),
};
#endif

UserDefineKey::UserDefineKey()
{
}
UserDefineKey::~UserDefineKey()
{
}

void UserDefineKey::PressKeyP1(void)
{
	SysUserDefinedKey ud;
    int func = ud.GetFuncP1();
	if(PRINT == func)
    {
        PrintScreen ps;
        ps.PrintP1();
    }
    else
    {
        FactoryCreate((EKeyFunc)func);
    }
}

void UserDefineKey::PressKeyP2(void)
{
	SysUserDefinedKey ud;
    int func = ud.GetFuncP2();
    if(PRINT == func)
    {
        ModeStatus ms;
        if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
            FreezeMode::GetInstance()->PressFreeze();
        
        PrintScreen ps;  
        ps.PrintP2();
    }
    else
    {
        FactoryCreate((EKeyFunc)func);
    }
}
void UserDefineKey::PressKeyP3(void)
{
	SysUserDefinedKey ud;
    int func = ud.GetFuncP3();
    if(PRINT == func)
    {
        ModeStatus ms;
        if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
            FreezeMode::GetInstance()->PressFreeze();
        
        PrintScreen ps;  
        ps.PrintP3();
    }
    else
    {
        FactoryCreate((EKeyFunc)func);
	}
}

void UserDefineKey::FactoryCreate(EKeyFunc type)
{
	ModeStatus ms;
	PrintScreen ps;
    switch (type)
    {
    	case NONE:
			break;
/*
#if defined(EMP_322)
		case THI:
			if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
			{
                D2ChgTHI(ROTATE);
			}
			break;
#endif
*/
		case TSI:
			if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
			{
				Img2D::GetInstance()->ChangeTSI(ROTATE);
			}
			break;

		case SAVE_CINE:
			if (ms.GetFreezeMode() != FreezeMode::UNFREEZE)
			{
                ReplayChgSaveCine();
			}
			break;

		case PRINT:
            //if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
             //   FreezeMode::GetInstance()->PressFreeze();
           // ps.Print();
           // HintArea::GetInstance()->UpdateHint(_("[PRINT]: Not realize now."), 2);
			break;
        case BIOPSY:
			{
				KeyBiopsy kb;
				kb.Execute();
			}
			break;

#if defined(EMP_322)
		case PIP:
			g_menu2D.BtnPIP(NULL);
			break;
#else

		case STEER:
			if (ms.GetFreezeMode() == FreezeMode::UNFREEZE)
			{
				D2ChgSteer(ROTATE);
			}
			break;

		case PIP:
			g_menu2D.BtnPIP(NULL);
			break;

		case PW_SOUND:
			ImgPw::GetInstance()->ChangeSoundStatus(ROTATE);
			break;

		case HPRF:
			if (ms.IsSpectrumMode() || ms.IsSpectrumColorMode())
			{
                if (ImgPw::GetInstance()->GetHPRFStatus())
                    ImgPw::GetInstance()->ChangeHPRF(FALSE);
                else
                    ImgPw::GetInstance()->ChangeHPRF(TRUE);
                //ImgPw::GetInstance()->ChangeHPRF(ROTATE);
			}
			break;

        case IMAGE_PRESET:
            {
                if (ModeStatus::IsAutoReplayMode())
                    FreezeMode::GetInstance()->ChangeAutoReplay();
                else if (ModeStatus::IsUnFreezeMode())	
                    FreezeMode::GetInstance()->PressFreeze();

                ViewSystem::GetInstance()->CreateWindow();
#ifndef EMP_355        //按自定义键进入systemsetting时，保持冻结
                FreezeMode::GetInstance()->PressUnFreeze();
#endif
            }
            break;
#endif
		default:
			break;
	}
}
