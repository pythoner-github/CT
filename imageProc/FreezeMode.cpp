/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: FreezeMode.cpp
 * @brief: control freeze mode
 *
 * version: V1.0
 * date: 2009-5-29
 * @author: zhanglei
 */

#include "Def.h"
#include "LightDef.h"
#include "FreezeMode.h"
#include "IoCtrl.h"
#include "GlobalClassMan.h"
#include "KeyFunc.h"
#include "ViewIcon.h"
#include "Img2D.h"
#include "../display/MenuArea.h"
#include "Menu2D.h"
#include "MenuM.h"
#include "MenuCFM.h"
#include "MenuPW.h"
#include "../sysMan/SysGeneralSetting.h"
#include "../display/TopArea.h"
#include "../probe/ProbeSelect.h"
#include "ViewMain.h"
#include "../patient/PatientInfo.h"
#include "ViewSystem.h"

FreezeMode* FreezeMode::m_ptrInstance = NULL;

FreezeMode* FreezeMode::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new FreezeMode;

	return m_ptrInstance;
}

FreezeMode::FreezeMode()
{
	m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
	m_ptrReplay = Replay::GetInstance();
	m_freezeMode = UNFREEZE;
	m_unFreezeForbidStatus = FALSE;
}
FreezeMode::~FreezeMode()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

/*
 * @brief enter image freeze status
 */
void FreezeMode::PressFreeze()
{
	if (ModeStatus::IsEFOVMode())
	{
		FreezeEFOV();
	}
	else
	{
		// freeze
		Freeze();

		g_menu2D.Sensitive(false);
		g_menuM.Sensitive(false);
		g_menuCFM.Sensitive(false);
		g_menuPW.Sensitive(false);
		g_menuCW.Sensitive(false);
		// draw
		m_ptrUpdate->Freeze(); // draw, also use for delay

		// enter replay mode
		EnterReplayMode();
	}
}

/*
 * @brief enter image unfreeze status
 */
void FreezeMode::PressUnFreeze()
{
	if (m_unFreezeForbidStatus)
		return;
    // exit read mode
    Replay::GetInstance()->ExitReadSnap();
    Replay::GetInstance()->ExitReadCine();

	if (ModeStatus::IsEFOVMode())
	{
		UnFreezeEFOV();
	}
	else
	{
		// exit auto replay
		if (m_freezeMode == AUTOREPLAY)
		{
			m_ptrReplay->AutoReplayEnd();
			ExitAutoReplay();
		}

		// unfreeze
		UnFreeze();

		g_menu2D.Sensitive(true);
		g_menuM.Sensitive(true);
		g_menuCFM.Sensitive(true);
        g_menuPW.Sensitive(true);
        g_menuCW.Sensitive(true);
        // exit replay mode
		ExitReplayMode();

		// draw
		m_ptrUpdate->UnFreeze();

		//clear
		KeyClearScreen kcs;
	//	kcs.Execute();
		kcs.UnFreezeExecute();
	}
}
/*
 * @brief change image replay status
 */
EKnobReturn FreezeMode::ChangeAutoReplay()
{
	if ((m_freezeMode == FREEZE) || (m_freezeMode == REPLAY))
	{
		EnterAutoReplayView();
	}
	else if (m_freezeMode == AUTOREPLAY)
	{
		m_freezeMode = REPLAY;
		m_ptrReplay->UpdateReplayCtrl(FALSE, PRESS);
		m_ptrReplay->AutoReplayEnd();
	}

	return (PRESS);
}

void FreezeMode::EnterAutoReplayView()
{
	bool ret;

	if ((m_freezeMode == FREEZE) || (m_freezeMode == REPLAY))
	{
		ret = m_ptrReplay->AutoReplayStartView();
		if (ret)
		{
			m_freezeMode = AUTOREPLAY;
            m_ptrReplay->UpdateReplayCtrl(TRUE, PRESS);
		}
	}

}

void FreezeMode::EnterAutoReplayReview()
{
	bool ret;

	if ((m_freezeMode == FREEZE) || (m_freezeMode == REPLAY))
	{
		ret = m_ptrReplay->AutoReplayStartReview();
		if (ret)
		{
			m_freezeMode = AUTOREPLAY;
            m_ptrReplay->UpdateReplayCtrl(TRUE, PRESS);
		}
	}
}

void FreezeMode::EnterReplayMode()
{
	///> prepare for replay
	MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::REPLAY);
	m_ptrReplay->PrepareForReplay();
}

void FreezeMode::ExitReplayMode()
{
	// clear current image's replay images
	m_ptrReplay->PrepareForEndReplay();
	MultiFuncUndo();
}
void FreezeMode::ExitAutoReplay()
{
 //   FreezeMode::EFreezeMode freezemode = GetFreezeMode();
 //   printf("freezeMode = %d\n",freezemode);
	if (m_freezeMode == AUTOREPLAY)
	{
		m_freezeMode = REPLAY;
        m_ptrReplay->UpdateReplayCtrl(FALSE, PRESS);
		m_ptrReplay->AutoReplayEnd();
	}
}

void FreezeMode::SetReplayMode()
{
	m_freezeMode = REPLAY;
}
void FreezeMode::Freeze()
{
	// set fps
	Replay::GetInstance()->SetReplayFps(ImageAreaPara::GetInstance()->GetGenFpsReplay());
	///> lighten freeze lamp
	m_freezeMode = FREEZE;
	g_keyInterface.CtrlLight(TRUE, LIGHT_FREEZE);

	///> send to imaging system
	IoCtrl io;
	io.Freeze();
    ImgPw::GetInstance()->OnCwImgCtrl(FALSE); //控制CW图像的实时与冻结。

	// update icon
	ViewIcon::GetInstance()->Replay(TRUE);
    Update2D::SetCineRemoveImg(3);
}
extern bool system_save;
extern bool review_pic;
void FreezeMode::UnFreeze()
{
	///> darken freeze lamp
	m_freezeMode = UNFREEZE;
#if  defined (EMP_322)
    if( (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC) || (MenuArea::GetInstance()->GetMenuType() == MenuArea::MEASURE))
        g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
        g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
#elif defined (EMP_313)
        if (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC)
            g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
#endif
        g_keyInterface.CtrlLight(FALSE, LIGHT_FREEZE);

        ///> send to imaging system
        IoCtrl io;
        io.Unfreeze();

    if(review_pic)
        {
            if(system_save)
            {
                string m_hospitalname;
                SysGeneralSetting sys;
                int date_format = sys.GetDateFormat();
                sys.GetHospital(m_hospitalname);
                UpdateHospitalandpart(date_format, m_hospitalname.c_str());
                system_save = false;
            }
            review_pic = false;
        }
        ImgPw::GetInstance()->OnCwImgCtrl(TRUE); //控制CW图像的实时与冻结。

        // update
        ViewIcon::GetInstance()->Replay(FALSE);

    //change scale:
      Img2D::GetInstance()->UpdateSeperateScaleWhenUnfreeze();
}
void FreezeMode::UpdateHospitalandpart(int date_format, const char *hospital_name)
{

		TopArea::GetInstance()->SetDateFormat(date_format);

    g_patientInfo.UpdateTopArea();

	TopArea::GetInstance()->UpdateHospitalName(hospital_name);

	ExamItem exam;
    char str_path[256];
    sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(str_path);
    bool userFlag = exam.ReadUserItemFlag(&ini);
    string userItemName = exam.ReadDefaultProbeDefaultItemName(&ini);
    if(userFlag)
    {
        TopArea::GetInstance()->UpdateCheckPart(_(userItemName.c_str()));
    }
    else
    {
#ifdef VET
        int probeIndex;
        ExamItem exam;
        string probemodel=exam.ReadDefaultProbe(&ini);
        for (int j = 0; j < NUM_PROBE; j++) {
            if(strcmp(probemodel.c_str(), PROBE_LIST[j].c_str())==0)
            {
                probeIndex=j;
                break;
            }
        }
        ViewSystem::GetInstance()->UpdateExamName(probeIndex);
#else
        ExamItem::EItem index = ProbeSelect::GetItemIndex();
        TopArea::GetInstance()->UpdateCheckPart(_(examType[index]));
#endif
    }
}


void FreezeMode::FreezeEFOV()
{
	if (ScanMode::GetInstance()->GetEFOVStatus() != ScanMode::CAPTURE)
		return;

	///> lighten freeze lamp
	m_freezeMode = FREEZE;
	g_keyInterface.CtrlLight(TRUE, LIGHT_FREEZE);

	///> send to imaging system
	IoCtrl io;
	io.Freeze();

	ScanMode::GetInstance()->EnterEFOVView();
}

void FreezeMode::UnFreezeEFOV()
{
	///> darken freeze lamp
	m_freezeMode = UNFREEZE;
	g_keyInterface.CtrlLight(FALSE, LIGHT_FREEZE);

	///> send to imaging system
	IoCtrl io;
	io.Unfreeze();

	Replay::GetInstance()->EFOVAutoReviewEnd();
	ScanMode::GetInstance()->EnterEFOVPrepare();
}

void FreezeMode::Freeze4D()
{
    ///> lighten freeze lamp
    m_freezeMode = FREEZE;
    g_keyInterface.CtrlLight(TRUE, LIGHT_FREEZE);

    ///> send to imaging system
    IoCtrl io;
    io.Freeze();
}

void FreezeMode::UnFreeze4D()
{
    ///> send to imaging system
    IoCtrl io;
    io.Unfreeze();

    usleep(10000);

    ///> darken freeze lamp
    m_freezeMode = UNFREEZE;
    g_keyInterface.CtrlLight(FALSE, LIGHT_FREEZE);
}

void FreezeMode::SetUnFreezeForbid(bool on)
{
	m_unFreezeForbidStatus = on;
}