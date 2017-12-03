#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "display/ImageArea.h"
#include "display/KnobMenu.h"
#include "ViewMain.h"
#include "display/gui_global.h"
#include "display/gui_func.h"
#include "display/TopArea.h"
#include "imageControl/Knob2D.h"
#include "comment/NoteArea.h"
#include "probe/ViewProbe.h"
#include "calcPeople/ViewReport.h"
#include "patient/ViewArchive.h"
#include "patient/ViewNewPat.h"
#include "display/ViewIcon.h"
#include "keyboard/KeyDef.h"
#include "keyboard/MultiFuncFactory.h"
#include "keyboard/KeyFunc.h"
#include "imageProc/ImgProc2D.h"
#include "imageControl/KnobPw.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/ScanMode.h"
#include "imageProc/Format2D.h"
#include "imageProc/FormatM.h"
#include "imageProc/FormatPw.h"
#include "imageProc/FormatCfm.h"
#include "Def.h"
#include <pthread.h>
#include "sysMan/SysGeneralSetting.h"
#include "measure/MeasureMan.h"
#include "sysMan/ViewSystem.h"
#include "sysMan/UserDefineKey.h"
#include "display/HintArea.h"
#include "sysMan/ViewSuperuser.h"
#include "imageProc/GlobalClassMan.h"
#include "keyboard/LightDef.h"
#include "imageProc/ScanMode.h"
#include "probe/ProbeMan.h"
#include "sysMan/ViewDicomSupervise.h"
#include <EmpAuthorization.h>
#include "imageProc/ImgProcPw.h"
#include "imageProc/MenuPW.h"
#include "measure/MeasureD.h"
#include "imageProc/Zoom.h"
#include "periDevice/ManRegister.h"
#include "periDevice/MonitorControl.h"
#include "sysMan/UserSelect.h"           //addec by LL

#include "imageControl/FpgaCtrl2D.h"
// test ProjectMode
#include "projectMode/MenuProjectMode.h"
#include "projectMode/KnobProjectMode.h"

#include "imageControl/QuickAdjustmentPw.h"

extern MenuPW g_menuPW;
// test_Artifact
// #include "imageProc/ImgProcCfm.h"

////////////////////////////[static function]/////////////////////////////
int g_tgcSlider[8] = {128, 128, 128, 128, 128, 128, 128, 128}; // global variable of tgc control

bool ViewMain::m_cursorVisible = true;
extern bool g_authorizationOn;

//manual/auto trace in freeze pw
int g_setFunc = 1; //1: no useful, 2: adjust is using, 3: finish to calc auto trace
//current operation (1: start line of cycle, 2: ps positoin, 3: ed position, 4: end line of cycle)
int g_curOper = 1;
/**
 * 1: 【Set】is 确认in pw freeze,
 * 2: 【Set】is 校正
 * 3:  not in manual or trace in freeze pw
 */

// only for test
gboolean test(gpointer data)
{
	if (!ModeStatus::IsFreezeMode())
		FreezeMode::GetInstance()->PressFreeze();

//	ViewReport::GetInstance()->CreateWindow();
	ViewSystem::GetInstance()->CreateWindow();
//	ViewNewPat::GetInstance()->CreateWindow();

	return FALSE;
}

gboolean ClickArchive(gpointer data)
{
	if (ModeStatus::IsUnFreezeMode())
		FreezeMode::GetInstance()->PressFreeze();

    FreezeMode::GetInstance()->ExitAutoReplay();
	ViewArchive::GetInstance()->CreateWindow();

	return false;
}

void InvisibleCursor(bool invisible, bool resetCursor)
{
	if (invisible)
	{
	//	PRINTF("hide system cursor\n");
	}
	else
	{
        if (resetCursor)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        else
            ResetIfOutOfRange();
	}

    if (invisible)
	{
		ChangeSystemCursor(GDK_BLANK_CURSOR);
		ViewMain::GetInstance()->SetCursorVisible(false);
	}
	else
	{
		ChangeSystemCursor(GDK_LEFT_PTR);
		ViewMain::GetInstance()->SetCursorVisible(true);
	}
#if 0
	GtkWidget* widget = ViewMain::GetInstance()->GetMainWindow();

    GdkCursor* cursor;
    GdkDisplay *display = gdk_display_get_default();
    if (invisible)
		cursor = gdk_cursor_new_for_display(display, GDK_BLANK_CURSOR);
		//cursor = gdk_cursor_new_for_display(display, GDK_CLOCK);
    else
		cursor = gdk_cursor_new_for_display(display, GDK_LEFT_PTR);
    gdk_window_set_cursor(widget->window, cursor);
    gdk_cursor_unref(cursor);
#endif
}

void ChangeSystemCursor(GdkCursorType type)
{
	GtkWidget* widget = ViewMain::GetInstance()->GetMainWindow();

    GdkCursor* cursor;
    GdkDisplay *display = gdk_display_get_default();
	cursor = gdk_cursor_new_for_display(display, type);
    gdk_window_set_cursor(widget->window, cursor);
    gdk_cursor_unref(cursor);
}

void TestMode(unsigned char keyValue)
{

	switch(keyValue)
	{

		case 'q':
	//		ViewMain::GetInstance()->t += 2000;
			break;
		case 'a':
	//		ViewMain::GetInstance()->t -= 1000;
			break;
		case 'w':
	//		printf("time is :%d\n",ViewMain::GetInstance()->t);
			break;
	}
}

void ViewMain::KeyEvent(unsigned char keyValue)
{
#ifdef EMP_355
    if(QuickAdjustmentPw::m_onQAPw)
    {
        return;
    }
#endif

	KnobMenu* ptrKnob = KnobMenu::GetInstance();
    AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    char type = Img2D::GetInstance()->ReviseProbeType(para.type);
#ifdef EMP_355
    //////////monitor control///////////////
    MonitorControl monitorCtl;

    switch(keyValue)
    {
        case KEY_BRIGHT_ADD:
            {
                monitorCtl.SetBrightAdd();
            }
            break;

        case KEY_BRIGHT_SUB:
            {
                monitorCtl.SetBrightSub();
            }
            break;

        case KEY_CONTRAST_ADD:
            {
                monitorCtl.SetContrastAdd();
            }
            break;

        case KEY_CONTRAST_SUB:
            {
                monitorCtl.SetContrastSub();
            }
            break;

        case KEY_AUTOADJUST:
            {
                monitorCtl.AutoAdjust();
            }
            break;

        case KEY_AUTOBALANCE:
            {
                monitorCtl.AutoBalance();
            }
            break;
    }
#endif

    if((MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW)&&(ViewSuperuser::GetInstance()->GetDemoStatus()/*menureview super user mode*/))
    {

        switch(keyValue)
        {
			case KEY_SET:
                {
                    ptrMultiFunc->Do();
                }
                break;

            case KEY_CURSOR: // change cursor focus between image and menu
                {
                    KeyCursor kc;
                    kc.Execute();
                }
                break;
#ifndef EMP_460
#ifndef EMP_355
            case KEY_F1:
            case KEY_F2:
    		case KEY_F3:
    		case KEY_F4:
    		//case KEY_F5: //disable save video while demo mode
    			KnobKeyEvent(keyValue);
    			break;
#endif
#endif
            default:
                {
#if (defined(EMP_460) || defined(EMP_355))
            if (keyValue == 'q')
            {
				if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();
                ViewSuperuser::GetInstance()->ExitDemo();
                FreezeMode::GetInstance()->PressUnFreeze();
            }
#else
            if (keyValue == 'q' && m_keyValue == KEY_CURSOR)
                ViewSuperuser::GetInstance()->ExitDemo();
#endif
                }
                break;
        }
		m_keyValue = keyValue;
		return;
    }
    printf("keyvalue = %x\n",keyValue);
    switch(keyValue)
    {
        case KEY_POWER:
            {
                if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();
				else if (ModeStatus::IsAutoReplayMode())
					FreezeMode::GetInstance()->ChangeAutoReplay();

				if(g_authorizationOn)
					CEmpAuthorization::Destroy();

				KeyPowerOff kpower;
				kpower.Execute();
			}
			break;

		case KEY_FREEZE:
			{
				bool isShowMenuCalc = false;
				bool isShowMenuBiopsy=false;//2016.09.09
				if (MenuArea::GetInstance()->GetMenuType() == MenuArea::CALC)
					isShowMenuCalc = true;
				//------2016.09.11------------------------------------------------------//
				if ((MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSY)||(MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYBRACKET)|| (MenuArea::GetInstance()->GetMenuType() == MenuArea::BIOPSYVERIFY))
					isShowMenuBiopsy = true;
				//----------------------------------------------------------------//
				if (ModeStatus::IsUnFreezeMode())
                {
#ifdef EMP_355
                    QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
#endif
                    FreezeMode::GetInstance()->PressFreeze();
                }
				else
				{
					if(ModeStatus::IsPwMode())
					{
						if(g_menuPW.GetAutoTraceStatus())
							ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
                        MeasureMan::GetInstance()->ClearPwTraceUnFreeze();
					}
					FreezeMode::GetInstance()->PressUnFreeze();
				}

				if (isShowMenuCalc)
				{
					AbsUpdateMix* ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
					ptrUpdate->EnterCalc();
					MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
#if defined (EMP_322)
					g_keyInterface.CtrlLight(FALSE,LIGHT_MEASURE);
					g_keyInterface.CtrlLight(TRUE,LIGHT_CALC);
#elif defined(EMP_313)
					g_keyInterface.CtrlLight(TRUE,LIGHT_CALC);
#endif
				}
				//--------2016.09.09-----------------------------------------------//
				if(isShowMenuBiopsy)
				{
					MenuShowUndo();
					MultiFuncUndo();//
				}
				//--------------------------------------------------------------------//
			}
			break;

#ifdef EMP_322
        case KEY_4D:
            {
#ifdef EMP3D
				if ( para.type == 'v' || para.type == 'V' ||
					 para.type == 'c' || para.type == 'C' ||
					 para.type == 'l' || para.type == 'L')
					ScanMode::GetInstance()->Enter4D();
				else
					HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
#else
                HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
#endif
            }
            break;
#endif

		case KEY_2D:
			{
#ifdef EMP_355
                QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
#endif

                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode::GetInstance()->ExitSpecialMeasure();
#if (defined (EMP_322) || defined (EMP_313))
				ScanMode::GetInstance()->DarkAllModeLight();
				g_keyInterface.CtrlLight(TRUE,LIGHT_D2);
#endif
				ScanMode::GetInstance()->Enter2D();
			}
			break;

		case KEY_PAGEUP:
#ifdef EMP_PROJECT
                        if(ViewSuperuser::GetInstance()->GetProjectModeStatus() && m_ptrKnobProjectMode->GetKnobStatus())
                            m_ptrKnobProjectMode->PageUp();
                        else
                            ptrKnob->PageUp();
#else
			ptrKnob->PageUp();
#endif
 break;

		case KEY_PAGEDOWM:
#ifdef EMP_PROJECT
                        if(ViewSuperuser::GetInstance()->GetProjectModeStatus() && m_ptrKnobProjectMode->GetKnobStatus())
                            m_ptrKnobProjectMode->PageDown();
                        else
                            ptrKnob->PageDown();
#else
			ptrKnob->PageDown();
#endif
                        break;
#ifndef EMP_460
#ifndef EMP_355
		case KEY_B:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
			{
				// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();

				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
					Format2D::GetInstance()->ChangeFormat(Format2D::B);
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B);
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B);
				}
			}
			break;
#endif
#endif

#if defined(K24C) //340,360,161
		case KEY_DUAL:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
			{
				// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();

				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
				{
					Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
					if (format != Format2D::BB)
						Format2D::GetInstance()->ChangeFormat(Format2D::BB);
					else
						Format2D::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::BB)
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
					else
						FormatCfm::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
				}
			}
			break;

		case KEY_QUAD:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
			{
				// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();
				`
				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
				{
					Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
					if (format != Format2D::B4)
						Format2D::GetInstance()->ChangeFormat(Format2D::B4);
					else
						Format2D::GetInstance()->SwitchB4();
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::B4)
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
					else
						FormatCfm::GetInstance()->SwitchB4();
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
				}
			}
			break;

		case KEY_DEPTH:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
					{
						KeyDepth kd;
						kd.Execute();
					}
				}

			}
			break;

		case KEY_FOCUS:
			{
				KeyFocus kf;
				kf.Execute();
			}
			break;

		case KEY_UPDATE:
			{
				ptrMultiFunc->Update();
			}
			break;

		case KEY_CW:
            {

#ifdef EMP_3410
                if (type == 'P' && CManRegister::GetInstance()->IsAuthorize("CW"))
//#elif EMP_355
			//		if (1)        //G30暂时屏蔽CW功能
#else
					if (type == 'P')
#endif
                {
                    ScanMode* ptrS = ScanMode::GetInstance();

                    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                    {
                        KeyLocalZoom zoom;
                        zoom.ExitLocalZoom();
                    }
                    // before enter cw
                    ptrS->PrepareEnterCw();
                    // enter cw
                    ModeStatus s;
                    Format2D::EFormat2D format2D = s.GetFormat2D();
                    int mode = s.GetScanMode();
#ifdef EMP_355
				  if(mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT)
                        {
                            ptrMultiFunc->Update();
                        }
                        else
                        {
                            Format2D::EFormat2D format2D = s.GetFormat2D();
                            if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                            {
                                Img2D::GetInstance()->ExitTpView();
                            Img2D::GetInstance()->ExitEFVI();
                        }

                        if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                            ptrS->EnterPwCfmFromCfm();
                        else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                            ptrS->EnterPwPdiFromPdi();
                        else if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT))
                            ptrS->EnterCfm();
                        else if ((mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
                            ptrS->EnterPdi();
                        else
                            ptrS->EnterPw();
#else
                    if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                        ptrS->EnterCwCfmFromCfm();
                    else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                        ptrS->EnterCwPdiFromPdi();
                    else if (mode == ScanMode::CWCFM)
                        ptrS->EnterCfm();
                    else if (mode == ScanMode::CWPDI)
                        ptrS->EnterPdi();
                    else
                        ptrS->EnterCw();
#endif
                }
                else
                {
#ifdef EMP_3D

					if ( para.type == 'v' || para.type == 'V' ||
						 para.type == 'c' || para.type == 'C' ||
						 para.type == 'l' || para.type == 'L')
					{
						///> control light
						ScanMode::GetInstance()->DarkAllModeLight();
                        g_keyInterface.CtrlLight(TRUE, LIGHT_CW);

                        if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                        {
                            KeyLocalZoom zoom;
                            zoom.ExitLocalZoom();
                        }
                        ScanMode::GetInstance()->Enter4D();
                    }
                    else
                        HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
#else
                    HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
#endif
                }
            }
 			break;

		case KEY_PDI:
            {
                ScanMode::GetInstance()->ExitSpecialMeasure();

                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

                if (mode == ScanMode::PWPDI)
                {
                    ptrS->UpdatePw();
                    g_keyInterface.CtrlLight(FALSE, LIGHT_PDI);
                }
                else
                    ptrS->EnterPdi();
            }
            break;

		case KEY_PW:
			{
              printf("pw650!!!!!!!!\n");
                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus()) {
                    {
                        KeyLocalZoom zoom;
                        zoom.ExitLocalZoom();
                    }
                    ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();
#ifdef EMP_355
				ImgPw::GetInstance()->ChangeHPRF(FALSE);
#endif

                if(mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT)
                {
                    ptrMultiFunc->Update();
                }
                else
                {
                    Format2D::EFormat2D format2D = s.GetFormat2D();

                    if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                    {
                        Img2D::GetInstance()->ExitTpView();
                        Img2D::GetInstance()->ExitEFVI();
                    }

                    if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                        ptrS->EnterPwCfmFromCfm();
                    else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                        ptrS->EnterPwPdiFromPdi();
                    else if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT))
                        ptrS->EnterCfm();
                    else if ((mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
                        ptrS->EnterPdi();
                    else
                        ptrS->EnterPw();
                }
            }
            break;

        case KEY_CFM:
            {
                ScanMode::GetInstance()->ExitSpecialMeasure();
                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

                if (mode == ScanMode::PWCFM)
                    ptrS->UpdatePw();
                else
                    ptrS->EnterCfm();
            }
            break;

        case KEY_M:
            {
                if(mode == ScanMode::M_INIT)
                {
                    ptrMultiFunc->Update();
                }
                else
                {
                    if (mode != ScanMode::M)
                        g_keyInterface.CtrlLight(TRUE,LIGHT_M);
                    if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                    {
                        Img2D::GetInstance()->ExitTpView();
                        Img2D::GetInstance()->ExitEFVI();
                    }
                    ScanMode::GetInstance()->EnterM();
                }
            }
            break;

        case KEY_AUTO:
			if (ModeStatus::IsEFOVMode()) {
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
            }
            if (ModeStatus::IsUnFreezeMode())
            {
                ModeStatus s;
                int mode = s.GetScanMode();
  if (!((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM)|| (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI)|| (mode == ScanMode::PWPDI_INIT)
                            || (mode == ScanMode::CWCFM) ||(mode == ScanMode::CWCFM_INIT)|| (mode == ScanMode::CWPDI) ||(mode == ScanMode::CWPDI_INIT)|| (mode == ScanMode::PW_SIMULT)
                            || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)))
                {
                    KeyAutoOptimize key;
                    key.Execute();
                }
            }
			break;
#elif (defined(EMP_460) || defined(EMP_355))
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
			CKnobEvent::FEvent(keyValue);
			break;

		case KEY_DUAL:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
			{
				// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();

				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
				{
					Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
					if (format != Format2D::BB)
                    {
                        Format2D::GetInstance()->ChangeFormat(Format2D::BB);
                        KeyClearScreen kcs;
                        kcs.Execute();
                    }
                    else
						Format2D::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::BB)
                    {
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
                        KeyClearScreen kcs;
                        kcs.Execute();
                    }
                     else
						FormatCfm::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
                    ScanMode::GetInstance()->EnterCfm();
                    FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
                    KeyClearScreen kcs;
                    kcs.Execute();
				}
			}
			break;

		case KEY_QUAD:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
			{
				// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();
				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
				{
					Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
					if (format != Format2D::B4)
                    {
						Format2D::GetInstance()->ChangeFormat(Format2D::B4);
                        KeyClearScreen kcs;
                        kcs.Execute();
                    }
					else
						Format2D::GetInstance()->SwitchB4();
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::B4)
                    {
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
                        KeyClearScreen kcs;
                        kcs.Execute();
                    }
					else
						FormatCfm::GetInstance()->SwitchB4();
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::B4);
                    KeyClearScreen kcs;
                    kcs.Execute();
                }
			}
			break;

		case KEY_P3:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				UserDefineKey ud;
				ud.PressKeyP3();
			}
			break;

        case KEY_FOCUS:
			{
                ModeStatus ms;
                int mode;
                mode = ms.GetScanMode();
                mode == ScanMode::D2;
                if ((ModeStatus::IsUnFreezeMode())&&(mode== ScanMode::D2))
                {
                    if ((ModeStatus::IsMImgMode() && (ms.GetFormatM() == FormatM::M_TOTAL))|| (ModeStatus::IsPWImgMode() && (ms.GetFormatPw() == FormatPw::P_TOTAL)))
                    {
                        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                        break;
                    }
                    else
                    {
                        Img2D* ptrImg = Img2D::GetInstance();
                        if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus() || Zoom::GetInstance()->GetLocalZoomStatus())
                        {
                            HintArea::GetInstance()->UpdateHint(_("When zoom, compound imaging, Tp-View or EFVI is on, it only has one focus."), 4);
                            break;
                        }

                        Img2D::GetInstance()->ChangeFocSum(ROTATE);

                    }
                }
            }
            break;

	case KEY_UPDATE:
			{
				ptrMultiFunc->Update();
			}
			break;

		case KEY_CW:
            {
#ifdef EMP_355
                QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
 				//if(1)
                if (type == 'P')
#else
                if (type == 'P')
#endif
                {
                    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                    {
                        KeyLocalZoom zoom;
                        zoom.ExitLocalZoom();
                    }
                    ScanMode* ptrS = ScanMode::GetInstance();
                    ModeStatus s;
				    int mode = s.GetScanMode();
                    Format2D::EFormat2D format2D = s.GetFormat2D();
#ifdef EMP_355
                    bool mode_bak = m_modeIsFlag;
                    m_modeIsFlag = true;
                    // before enter cw
                    ptrS->PrepareEnterCw();
                    // enter cw
                    if((mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT)&&(mode_bak == m_modeIsFlag))
                    {
                        ptrMultiFunc->Update();
                    }
                    else
                    {
                        Format2D::EFormat2D format2D = s.GetFormat2D();
                        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                        {
                            Img2D::GetInstance()->ExitTpView();
                            Img2D::GetInstance()->ExitEFVI();
                        }

                        if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                            ptrS->EnterPwCfmFromCfm();
                        else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                            ptrS->EnterPwPdiFromPdi();
                        else if (mode_bak && ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT)))
                            ptrS->EnterCfm();
                        else if (mode_bak && ((mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT)))
                            ptrS->EnterPdi();
                        else
                            ptrS->EnterPw();
                    }
#else
                    // before enter cw
                    ptrS->PrepareEnterCw();

                    // enter cw
                    if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                        ptrS->EnterCwCfmFromCfm();
                    else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                        ptrS->EnterCwPdiFromPdi();
                    else if (mode == ScanMode::CWCFM)
                        ptrS->EnterCfm();
                    else if (mode == ScanMode::CWPDI)
                        ptrS->EnterPdi();
                    else
                        ptrS->EnterCw();
#endif
                }
                else
                {

                    HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
                }
            }
            break;

		case KEY_PDI:
			{
#ifdef EMP_355
                QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
#endif
                ScanMode::GetInstance()->ExitSpecialMeasure();

                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();

                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

                if (mode == ScanMode::PWPDI)
                {
                    ptrS->UpdatePw();
                    g_keyInterface.CtrlLight(FALSE, LIGHT_PDI);
                }
                else
                    ptrS->EnterPdi();
            }
            break;

		case KEY_PW:
            {
#ifdef EMP_355
                bool mode_bak = m_modeIsFlag;
                m_modeIsFlag = false;
                QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
                ImgPw::GetInstance()->ChangeHPRF(FALSE);
#endif
                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();
#ifdef EMP_355
                if((mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT)&&(m_modeIsFlag == mode_bak))
#else
                    if(mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT)
#endif
                    {
                        ptrMultiFunc->Update();
                    }
                    else
                    {
                        Format2D::EFormat2D format2D = s.GetFormat2D();
                        if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                        {
                            Img2D::GetInstance()->ExitTpView();
                            Img2D::GetInstance()->ExitEFVI();
                        }

                    if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                        ptrS->EnterPwCfmFromCfm();
                    else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                        ptrS->EnterPwPdiFromPdi();
#ifdef EMP_355
                    else if ((!mode_bak) && ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT)))
#else
                    else if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT))
#endif
                        ptrS->EnterCfm();
#ifdef EMP_355
                    else if ((!mode_bak) && ((mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT)))
#else
                    else if ((mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
#endif
                        ptrS->EnterPdi();
                    else
                        ptrS->EnterPw();
                }
                HintArea::GetInstance()->UpdateHint(_("[Angle]: <AUTO> to adjust size."), 2);
			}
			break;

        case KEY_CFM:
            {
#ifdef EMP_355
                QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
#endif
                ScanMode::GetInstance()->ExitSpecialMeasure();

                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();

                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

                if (mode == ScanMode::PWCFM)
                {
                    ptrS->UpdatePw();
                    g_keyInterface.CtrlLight(FALSE, LIGHT_CFM);
                }
                else
                    ptrS->EnterCfm();
            }
            break;

		case KEY_M:
            {
#ifdef EMP_355
                QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOff();
#endif
                ModeStatus s;
                int mode = s.GetScanMode();
                if(mode == ScanMode::M_INIT)
                {
                    ptrMultiFunc->Update();
                }
                else
                {
                    if (mode != ScanMode::M)
                        g_keyInterface.CtrlLight(TRUE,LIGHT_M);
                    if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                    {
                        Img2D::GetInstance()->ExitTpView();
                        Img2D::GetInstance()->ExitEFVI();
                    }
                    ScanMode::GetInstance()->EnterM();
                }
            }
            break;

		case KEY_DEPTHADD:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
                    {
                        KeyDepth kd;
                        kd.ExecuteAdd();
					}
				}
			}
			break;

		case KEY_DEPTHSUB:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
					{
						KeyDepth kd;
						kd.ExecuteSub();
					}
				}
            }
            break;

        case KEY_AUTO:
            if (ModeStatus::IsEFOVMode()) {
                HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                break;
            }
            if (ModeStatus::IsFreezeMode() && ModeStatus::IsPwMode())
            {
                if(g_setFunc ==  1 || g_setFunc == 0)
                    break;
                static int curOper = 1;
                if(g_setFunc == 3)
                {
                    g_curOper = 1;
                    curOper = 1;
                    HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Auto> press to adjust."));
                    MeasureMan::GetInstance()->ReDrawManualAutoStartLine(true);
                    g_setFunc = 2;
                }
                else if(g_setFunc == 2)
                {
                    curOper += 1;
                    MeasureMan::GetInstance()->ManualAutoAdjust(curOper);
                    if(curOper >= 4)
                        curOper = 0;

                    if(curOper == 0)
                        g_curOper = 4;
                    else
                        g_curOper = curOper;
                }
            }
            if (ModeStatus::IsUnFreezeMode())
            {
                ModeStatus s;
                int mode = s.GetScanMode();
                if((mode ==ScanMode::D2)||(mode == ScanMode::CFM)||(mode == ScanMode::PDI))
                {
                    KeyAutoOptimize key;
                    key.Execute();
                }
                else if((mode == ScanMode::PW) || (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI))
                {
                    if(!GetModeIsFlag())
                    {
                        HintArea::GetInstance()->UpdateHint(_("Any operation is invalid when eTouch is on."));
                        QuickAdjustmentPw::GetInstance()->QuickAdjustmentPwOn();
                    }
                    else
                        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                }
            }
            break;

#elif defined(EMP_322)
        case KEY_F6:
            {
                KnobKeyEvent(keyValue);
            }
            break;

        case KEY_FREQ:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					KeyFreq kf;
					kf.Execute();
				}
			}
			break;

		case KEY_CHROMA:
			{
                if (ModeStatus::IsEFOVMode()) {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }

                HintArea::GetInstance()->ClearHint();
                //int retTemp = D2PressChroma();
				KeyChroma kc;
				kc.Execute();
			}
			break;

		case KEY_FOCUS:
			{
                if (ModeStatus::IsEFOVMode()) {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }

                KeyFocus kfc;
                kfc.Execute();
            }
            break;

		case KEY_TSI:
			{
                if (ModeStatus::IsEFOVMode()) {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }

                KeyTSI ktsi;
				ktsi.Execute();
			}
			break;

		case KEY_BB:
            if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
            {
                	// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();
                ScanMode::GetInstance()->DarkAllModeLight();
                g_keyInterface.CtrlLight(TRUE,LIGHT_BB);
                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }
				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
				{
					Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
					if (format != Format2D::BB)
						Format2D::GetInstance()->ChangeFormat(Format2D::BB);
					else
						Format2D::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::BB)
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
					else
						FormatCfm::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
				}
				else
				{
					ScanMode::GetInstance()->Enter2D();
					Format2D::GetInstance()->ChangeFormat(Format2D::BB);

				}
			}
			break;

        case KEY_M:
            {
                // exit special measure
                ScanMode::GetInstance()->ExitSpecialMeasure();
                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }
                ScanMode::GetInstance()->DarkAllModeLight();
                g_keyInterface.CtrlLight(TRUE,LIGHT_M);
                ScanMode::GetInstance()->EnterM();
                ScanMode::GetInstance()->UpdateM();
            }
            break;

		case KEY_BM:
			{
				// exit special measure
				ScanMode::GetInstance()->ExitSpecialMeasure();
				ScanMode::GetInstance()->DarkAllModeLight();
				g_keyInterface.CtrlLight(TRUE,LIGHT_BM);
				ScanMode::GetInstance()->EnterM();
				//ScanMode::GetInstance()->Enter2D();
				ScanMode::GetInstance()->UpdateBM();
			}
			break;

		case KEY_SPACE:
			ViewSuperuser::GetInstance()->Authenticate();
			break;

		case KEY_DEPTHADD:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
					{
						KeyDepth kd;
						kd.ExecuteAdd();
					}
				}
			}
			break;

		case KEY_DEPTHSUB:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
					{
						KeyDepth kd;
						kd.ExecuteSub();
					}
				}
			}
			break;

		case KEY_AUTO:
			if (ModeStatus::IsEFOVMode()) {
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
            }
            if (ModeStatus::IsUnFreezeMode())
            {
                ModeStatus s;
                int mode = s.GetScanMode();

               if (!((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM)|| (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI)|| (mode == ScanMode::PWPDI_INIT)
                            || (mode == ScanMode::CWCFM) ||(mode == ScanMode::CWCFM_INIT)|| (mode == ScanMode::CWPDI) ||(mode == ScanMode::CWPDI_INIT)|| (mode == ScanMode::PW_SIMULT)
                            || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)))
                {
                  KeyAutoOptimize key;
                  key.Execute();
              }
            }
			break;

#elif defined(EMP_313)
		case KEY_BB:
            if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
            {
                // exit special measure
                ScanMode::GetInstance()->ExitSpecialMeasure();
                ScanMode::GetInstance()->DarkAllModeLight();
                g_keyInterface.CtrlLight(TRUE,LIGHT_BB);

                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

				int current;
				ModeStatus s;
				int mode = s.GetScanMode();

				if (mode == ScanMode::D2)
				{
					Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
					if (format != Format2D::BB)
						Format2D::GetInstance()->ChangeFormat(Format2D::BB);
					else
						Format2D::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::BB)
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
					else
						FormatCfm::GetInstance()->SwitchBB();
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
				}
				else
				{
					ScanMode::GetInstance()->Enter2D();
					Format2D::GetInstance()->ChangeFormat(Format2D::BB);

				}
			}
			break;

        case KEY_M:
			ScanMode::GetInstance()->DarkAllModeLight();
			g_keyInterface.CtrlLight(TRUE,LIGHT_M);

            if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }
            ScanMode::GetInstance()->EnterM();
            ScanMode::GetInstance()->UpdateM();
            break;

        case KEY_BM:
            ScanMode::GetInstance()->DarkAllModeLight();
            g_keyInterface.CtrlLight(TRUE,LIGHT_BM);
            if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
            {
                Img2D::GetInstance()->ExitTpView();
                Img2D::GetInstance()->ExitEFVI();
            }
            ScanMode::GetInstance()->EnterM();
            ScanMode::GetInstance()->UpdateBM();
            break;

        case KEY_DEPTH:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
					{
						KeyDepth kd;
						kd.Execute();
					}
				}
			}
			break;

		case KEY_F6:
			{
				KnobKeyEvent(keyValue);
			}
			break;

		case KEY_FREQ:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					KeyFreq kf;
					kf.Execute();
				}
			}
			break;

		case KEY_FOCUS:
			{
                if (ModeStatus::IsEFOVMode()) {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }

                KeyFocus kfc;
				kfc.Execute();
			}
			break;

		case KEY_SPACE:
			ViewSuperuser::GetInstance()->Authenticate();
			break;
#else //K24D(340,360,161)
		case KEY_BBL:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
            {
                //exit special measure
                ScanMode::GetInstance()->ExitSpecialMeasure();
                int current(0);
                ModeStatus s;
                int mode = s.GetScanMode();

                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }
                if (mode == ScanMode::D2)
                {
                    Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
                    if (format != Format2D::BB)
                    {
                        Format2D::GetInstance()->ChangeFormat(Format2D::BB);
                        MySleep(60000);
                        Format2D::GetInstance()->SwitchBB(FALSE, current);
                        MySleep(80000);
                        Format2D::GetInstance()->SwitchBB(TRUE, current);

                    }
                    else
                    {
                        Format2D::GetInstance()->SwitchBB(TRUE, current);
                    }
                }
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::BB)
					{
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
                        FormatCfm::GetInstance()->SwitchBB(FALSE, current);
                        usleep(450000);
                        FormatCfm::GetInstance()->SwitchBB(TRUE, current);
                        usleep(300000);
                    }
					else
					{
						FormatCfm::GetInstance()->SwitchBB(TRUE, current);
					}
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
					usleep(250000);
					FormatCfm::GetInstance()->SwitchBB(FALSE, current);
					usleep(250000);
					FormatCfm::GetInstance()->SwitchBB(TRUE, current);
				}
			}
			break;

		case KEY_BBR:
			if (MultiFuncFactory::GetInstance()->GetMultiFuncType() != MultiFuncFactory::PIP_ZOOM)
			{
                // exit special measure
                ScanMode::GetInstance()->ExitSpecialMeasure();
                int current(0);
                ModeStatus s;
                int mode = s.GetScanMode();
                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }
                if (mode == ScanMode::D2)
                {
                    Format2D::EFormat2D format = Format2D::GetInstance()->GetFormat();
                    if (format != Format2D::BB)
                    {
                        Format2D::GetInstance()->ChangeFormat(Format2D::BB);
                        MySleep(80000);
                        Format2D::GetInstance()->SwitchBB(FALSE, current);
                    }
				else
					{
						Format2D::GetInstance()->SwitchBB(FALSE, current);
					}
				}
				else if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI))
				{
					FormatCfm::EFormatCfm format = FormatCfm::GetInstance()->GetFormat();
					if (format != FormatCfm::BB)
					{
						FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
						usleep(300000);
                        FormatCfm::GetInstance()->SwitchBB(FALSE, current);
                        usleep(300000);
					}
					else
					{
						FormatCfm::GetInstance()->SwitchBB(FALSE, current);
					}
				}
				else if ((mode == ScanMode::CFM_VS_2D) || (mode == ScanMode::PDI_VS_2D))
				{
					ScanMode::GetInstance()->EnterCfm();
					FormatCfm::GetInstance()->ChangeFormat(FormatCfm::BB);
					usleep(250000);
					FormatCfm::GetInstance()->SwitchBB(FALSE, current);
				}
			}
			break;

		case KEY_MENU: // hide or display menu
			{
				KeyMenu km;
				km.Execute();
			}
			break;

		case KEY_UPDATE:
			{
				ptrMultiFunc->Update();
			}
			break;

		case KEY_CW:
            {

#ifdef EMP_3410
                if (type == 'P' && CManRegister::GetInstance()->IsAuthorize("CW"))
#else
                if (type == 'P')
#endif
                {
                    ScanMode* ptrS = ScanMode::GetInstance();

                    if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                    {
                        KeyLocalZoom zoom;
                        zoom.ExitLocalZoom();
                    }
                    // before enter cw
                    ptrS->PrepareEnterCw();

                    // enter cw
                    ModeStatus s;
                    Format2D::EFormat2D format2D = s.GetFormat2D();

                    int mode = s.GetScanMode();
                    if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                        ptrS->EnterCwCfmFromCfm();
                    else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                        ptrS->EnterCwPdiFromPdi();
                    else if (mode == ScanMode::CWCFM)
                        ptrS->EnterCfm();
                    else if (mode == ScanMode::CWPDI)
                        ptrS->EnterPdi();
                    else
                        ptrS->EnterCw();
                }
                else
                {
#if defined(EMP3D)
					if ( para.type == 'v' || para.type == 'V' ||
						 para.type == 'c' || para.type == 'C' ||
						 para.type == 'l' || para.type == 'L')
                    {
                        ///> control light
                        ScanMode::GetInstance()->DarkAllModeLight();
                        g_keyInterface.CtrlLight(TRUE, LIGHT_CW);

                        if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                        {
                            KeyLocalZoom zoom;
                            zoom.ExitLocalZoom();
                        }
                        ScanMode::GetInstance()->Enter4D();
                    }
                    else
                        HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
#else
                    HintArea::GetInstance()->UpdateHint(_("Not realize now!"), 1);
#endif
                }
            }
            break;

		case KEY_PDI:
			{

                ScanMode::GetInstance()->ExitSpecialMeasure();

                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();

                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

                if (mode == ScanMode::PWPDI)
                {
                    ptrS->UpdatePw();
                    g_keyInterface.CtrlLight(FALSE, LIGHT_PDI);
                }
                else
                    ptrS->EnterPdi();
            }
            break;

		case KEY_PW:
            {
                printf("pw1743!!!!!!!!!!!!\n");
                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();
#ifdef EMP_355
			    ImgPw::GetInstance()->ChangeHPRF(FALSE);
#endif
                if(mode == ScanMode::PW_INIT || mode == ScanMode::PWCFM_INIT || mode == ScanMode::PWPDI_INIT)
                {
                    ptrMultiFunc->Update();
                }
                else
                {
                    Format2D::EFormat2D format2D = s.GetFormat2D();
                    if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                    {
                        Img2D::GetInstance()->ExitTpView();
                        Img2D::GetInstance()->ExitEFVI();
                    }

                    if ((mode == ScanMode::CFM) && (format2D == Format2D::B))
                        ptrS->EnterPwCfmFromCfm();
                    else if ((mode == ScanMode::PDI) && (format2D == Format2D::B))
                        ptrS->EnterPwPdiFromPdi();
                    else if ((mode == ScanMode::PWCFM) || (mode == ScanMode::PWCFM_SIMULT))
                        ptrS->EnterCfm();
                    else if ((mode == ScanMode::PWPDI) || (mode == ScanMode::PWPDI_SIMULT))
                        ptrS->EnterPdi();
                    else
                        ptrS->EnterPw();
                }
			}
			break;

        case KEY_CFM:
            {

				ScanMode::GetInstance()->ExitSpecialMeasure();

                if (MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM && !Zoom::GetInstance()->GetLocalZoomStatus())
                {
                    KeyLocalZoom zoom;
                    zoom.ExitLocalZoom();
                }
                ScanMode* ptrS = ScanMode::GetInstance();
                ModeStatus s;
                int mode = s.GetScanMode();

                if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                {
                    Img2D::GetInstance()->ExitTpView();
                    Img2D::GetInstance()->ExitEFVI();
                }

                if (mode == ScanMode::PWCFM)
                {
                    ptrS->UpdatePw();
                    g_keyInterface.CtrlLight(FALSE, LIGHT_CFM);
                }
                else
                    ptrS->EnterCfm();
            }
            break;

		case KEY_M:
            {
                ModeStatus s;
                int mode = s.GetScanMode();
                if(mode == ScanMode::M_INIT)
                {
                    ptrMultiFunc->Update();
                }
                else
                {
                    if (mode != ScanMode::M)
                        g_keyInterface.CtrlLight(TRUE,LIGHT_M);
                    if (Img2D::GetInstance()->GetTpViewStatus() || Img2D::GetInstance()->GetEFVIStatus())
                    {
                        Img2D::GetInstance()->ExitTpView();
                        Img2D::GetInstance()->ExitEFVI();
                    }
                    ScanMode::GetInstance()->EnterM();
                }
            }
            break;

		case KEY_DEPTHADD:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
                    {
                        KeyDepth kd;
                        kd.ExecuteAdd();
					}
				}
			}
			break;

		case KEY_DEPTHSUB:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				if (ModeStatus::IsUnFreezeMode())
				{
					ScanMode::EScanMode mode = ScanMode::GetInstance()->GetFpgaScanMode();
					if ((mode == ScanMode::PW) || (mode == ScanMode::CW))
					{
						HintArea::GetInstance()->UpdateHint(_("[Depth]: Not valid in current mode."), 1);
					}
					else
					{
						KeyDepth kd;
						kd.ExecuteSub();
					}
				}
			}
			break;

        case KEY_AUTO:
            if (ModeStatus::IsEFOVMode()) {
                HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                break;
            }
            if (ModeStatus::IsFreezeMode() && ModeStatus::IsPwMode())
            {
                if(g_setFunc ==  1 || g_setFunc == 0)
                    break;
               static int curOper = 1;
                if(g_setFunc == 3)
                {
                    g_curOper = 1;
                    curOper = 1;
					HintArea::GetInstance()->UpdateHint(_("[AutoTrack]: <Update> to switch to SemiAuto. <Value> press to adjust."));
					MeasureMan::GetInstance()->ReDrawManualAutoStartLine(true);
                    g_setFunc = 2;
                }
                else if(g_setFunc == 2)
                {
                    curOper += 1;
                    MeasureMan::GetInstance()->ManualAutoAdjust(curOper);
                    if(curOper >= 4)
                        curOper = 0;

                    if(curOper == 0)
                        g_curOper = 4;
                    else
                        g_curOper = curOper;
                }

            }
            if (ModeStatus::IsUnFreezeMode())
            {
                ModeStatus s;
                int mode = s.GetScanMode();

                if (!((mode == ScanMode::PW) || (mode == ScanMode::CW) || (mode == ScanMode::PWCFM)|| (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI)|| (mode == ScanMode::PWPDI_INIT)
                            || (mode == ScanMode::CWCFM) ||(mode == ScanMode::CWCFM_INIT)|| (mode == ScanMode::CWPDI) ||(mode == ScanMode::CWPDI_INIT)|| (mode == ScanMode::PW_SIMULT)
                            || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT)))
                {
                    KeyAutoOptimize key;
                    key.Execute();
                }
            }
            break;

#endif

		case KEY_ARCHIVE: // enter archive management dialog
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();
				else if (ModeStatus::IsAutoReplayMode())
					FreezeMode::GetInstance()->ChangeAutoReplay();

                ViewArchive::GetInstance()->CreateWindow();
			}
			break;

		case KEY_LOCALZOOM:
			{
                KeyLocalZoom klz;
                klz.Execute();
            }
			break;

        case KEY_ENDEXAM: // end exam, add patient data to archive and then clear it
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				FreezeMode::GetInstance()->ExitAutoReplay();
				KeyEndExam kee;
				kee.Execute();
			}
			break;
#if (defined(EMP_460) || defined(EMP_355))
        case KEY_MENU:
#else
	    case KEY_REVIEW:		// change between menu and read image
#endif
			MenuReviewCallBack();
            //CManRegister::GetInstance()->GenerateRegisterKey(1);
            break;

        case KEY_F1:
            printf("hehevalue = %x\n",keyValue);
        case KEY_F2:
		case KEY_F3:
		case KEY_F4:
		case KEY_F5:
			KnobKeyEvent(keyValue);
			break;

		case KEY_ESC:
			{
				ptrMultiFunc->Undo();
			}
			break;

		case KEY_SET:
			{
				ptrMultiFunc->Do();
			}
			break;

		case KEY_LEFTRIGHT:
			{
				if (ModeStatus::IsEFOVMode() || !(ModeStatus::IsUnFreezeMode()))
				{
					//if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				ImgProc2D::GetInstance()->ChangeLR();
			}
			break;

		case KEY_UPDOWN:
			{
				if (ModeStatus::IsEFOVMode() || !(ModeStatus::IsUnFreezeMode()))
				{
					//if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				ImgProc2D::GetInstance()->ChangeUD();
			}
			break;

		case KEY_INVERT:
			{
				if (ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())
					ImgPw::GetInstance()->ChangeInvert();
			}
			break;

		case KEY_MEASURE:
			{
				if (ModeStatus::IsEFOVMode())
				{
					ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();
					if (status == ScanMode::PREPARE || status == ScanMode::CAPTURE)
					{
						HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
						break;
					}
				}
				FreezeMode::GetInstance()->ExitAutoReplay();
				KeyMeasure km;
				km.Execute();
			}
                        break;

                case KEY_BODYMARK:
                        {
                            // enter bodymark interface(menu and knob)
                            FreezeMode::GetInstance()->ExitAutoReplay();
                            KeyBodyMark kbm;
                            kbm.Execute();
                        }
                        break;

                case KEY_UP:
#ifdef EMP_PROJECT
                        if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
                        {
                            if(!m_ptrKnobProjectMode->GetKnobStatus())
                            {
                                m_ptrKnobProjectMode->EnableKnob(true);
                            }
                        }
                        else
                        {
                            ptrMultiFunc->KeyUp();
                        }
#else
                            ptrMultiFunc->KeyUp();
#endif
                        break;

                case KEY_DOWN:
#ifdef EMP_PROJECT
                        if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
                        {
                            if(m_ptrKnobProjectMode->GetKnobStatus())
                            {
                                m_ptrKnobProjectMode->EnableKnob(false);
                            }
                        }
                        else
                        {
                            ptrMultiFunc->KeyDown();
                        }
#else
   			 ptrMultiFunc->KeyDown();
#endif
                        break;

                case KEY_LEFT:
			{
				ptrMultiFunc->KeyLeft();
			}
			break;

		case KEY_RIGHT:
			{
				ptrMultiFunc->KeyRight();
			}
			break;

		case KEY_REPORT:
			{
				if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();
				else if (ModeStatus::IsAutoReplayMode())
					FreezeMode::GetInstance()->ChangeAutoReplay();

				ViewReport::GetInstance()->CreateWindow();

			}
			break;

		case KEY_CALC:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				FreezeMode::GetInstance()->ExitAutoReplay();
				KeyCalc kc;
				kc.Execute();
			}
			// enter calc menu
			break;

		case KEY_SYSTEM:
			{

			/*if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				if (ModeStatus::IsAutoReplayMode())
					FreezeMode::GetInstance()->ChangeAutoReplay();

				KeySystem ks;
				ks.Execute();*/
				//-------2016.07.22--hy-----------------------------------//
				if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				 if (ModeStatus::IsAutoReplayMode())
					 FreezeMode::GetInstance()->ChangeAutoReplay();
				 else if (ModeStatus::IsUnFreezeMode())
					 FreezeMode::GetInstance()->PressFreeze();

				 ViewSystem::GetInstance()->CreateWindow();
				 ViewSystem::GetInstance()->ChangeNoteBookPage(0);

				//-----------------------------------------------//
			}
			break;

		case KEY_TEXT:
			{
				if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus())
				{
					ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();
					if (status == ScanMode::PREPARE || status == ScanMode::CAPTURE) {
						HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
						break;
					} else if (status == ScanMode::REVIEW && Replay::GetInstance()->IsEFOVAutoReplay()) {
						Replay::GetInstance()->EFOVAutoReviewEnd();
					}
				}
				if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();
				FreezeMode::GetInstance()->ExitAutoReplay();

				KeyText kt;
				kt.Execute();
			}
			break;

		case KEY_ARROW:
			{
				if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();
				FreezeMode::GetInstance()->ExitAutoReplay();
				KeyArrow ka;
				ka.Execute();
			}
			break;

		case KEY_CLEAR:
			{
				KeyClearScreen kcs;
				kcs.Execute();
			}
			// clear screen
			break;

		case KEY_SAVE:
			// save picture temply
			{
				if (ModeStatus::IsEFOVMode()) {
					ScanMode::EFOVStatus status = ScanMode::GetInstance()->GetEFOVStatus();
					if (status == ScanMode::PREPARE || status == ScanMode::CAPTURE) {
						HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
						break;
					}
				}
				KeySaveSnap ks;
				ks.Execute();
			}
			break;

		case KEY_P1:
            {
                if (ModeStatus::IsEFOVMode())
                {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }
                UserDefineKey ud;
                ud.PressKeyP1();
            }
            break;

		case KEY_P2:
			{
				if (ModeStatus::IsEFOVMode()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}
				UserDefineKey ud;
				ud.PressKeyP2();
			}
			break;

		case KEY_PROBE: // enter probe dialog
			{
				if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
                }
#if 0
#ifdef EMP_430
                if(ModeStatus::IsAllSpectrumImgMode())
                {
                    ImgPw::GetInstance()->ChangeDopplerSoundStatus(false);
                }
#endif
#endif
				// freeze
                if (ModeStatus::IsUnFreezeMode())
                    FreezeMode::GetInstance()->PressFreeze();
#if  0
#ifdef EMP_430
                //close hv
                ProbeMan::GetInstance()->ActiveHV(FALSE);
                usleep(50000);
#endif
#endif
				FreezeMode::GetInstance()->ExitAutoReplay();
				ViewProbe::GetInstance()->Create();
			}
			break;

		case KEY_PATIENT: // enter patient dialog
			{
				if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus()) {
					HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
					break;
				}

				if (ModeStatus::IsUnFreezeMode())
					FreezeMode::GetInstance()->PressFreeze();

                FreezeMode::GetInstance()->ExitAutoReplay();
                ViewNewPat::GetInstance()->CreateWindow();

                if(ModeStatus::IsPwMode())
                {
                    if(g_menuPW.GetAutoTraceStatus())
                        ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
                    MeasureMan::GetInstance()->ClearPwTraceUnFreeze();
                }

                FreezeMode::GetInstance()->PressUnFreeze();
#if 0
                printf("patient\n");
                char path[256];
                sprintf(path, "%s%s", CFG_RES_PATH, PROJECT_DEBUG_FILE);
                IniFile ini(path);

                ExamItem em;
                ExamItem::ProjectDefaultParaItem proParaItem;
                ExamItem::ProjectDebugParaItem debugParaItem;
                proParaItem.pro2d.freq = 100;
                proParaItem.mode = ScanMode::D2;
                debugParaItem.depth = 40;
                debugParaItem.mode = proParaItem.mode;
                debugParaItem.freq = proParaItem.mode;
                static double test = 2.4;
                double bandpassw1[5];
                for (int i = 0; i < 5; i++)
                    bandpassw1[i] = 1.0;
                proParaItem.pro2d.FilterSection = test;
                double temp_t;
                for(int i = 0; i < 5; i++)
                {
                    temp_t = proParaItem.pro2d.BandPassW1[i] = (double)bandpassw1[i] + (double)test;
                    proParaItem.pro2d.BandPassW2[i] = bandpassw1[i] + test;
                    proParaItem.pro2d.LowPassW[i] = bandpassw1[i] + test;
                    printf("temp_t:%.1f\n", temp_t);
                }
                proParaItem.pro2d.BandPassWindowFunc = test;
                proParaItem.pro2d.LowPassWindowFunc = test;

                //cfm
                proParaItem.procolor.WallFilterSection = test;
                proParaItem.procolor.dopfreq = 118;
                for(int i = 0; i < 3; i++)
                    proParaItem.procolor.WallFilterW[i] = bandpassw1[i] + test + 1.5;

                memcpy(&debugParaItem.defaultPara, &proParaItem, sizeof(ExamItem::ProjectDefaultParaItem));
                //em.WriteProjectPara(&proParaItem, "75L40K", "Default", &ini);

                test += 0.6;
                //em.RemoveUserFromFile("75L40K", "Test", &ini);
                em.WriteProDebugParaItem(&debugParaItem, "75L40K", "user2", &ini);
#endif
            }
			break;

		case KEY_CURSOR: // change cursor focus between image and menu
			{
				KeyCursor kc;
				kc.Execute();
			}
			break;

		case KEY_VOLUMEADD:
			if (ModeStatus::IsEFOVMode()) {
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
			}
			ImgPw::GetInstance()->ChangeSoundVolume(ADD);
			break;

		case KEY_VOLUMESUB:
			if (ModeStatus::IsEFOVMode()) {
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
			}

			ImgPw::GetInstance()->ChangeSoundVolume(SUB);
			break;

		case KEY_PWRADD:
			if (ModeStatus::IsEFOVMode()) {
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
			}

			if (ModeStatus::IsUnFreezeMode())
			{
				Img2D::GetInstance()->ChangeSoundPower(ADD);
				ImgPw::GetInstance()->ChangeSoundPower(ADD);

				// change tis
				ChangeTis();
			}
			break;

		case KEY_PWRSUB:
			if (ModeStatus::IsEFOVMode()) {
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
			}

			// test_Artifact
#if 0
			{
				DSCCONTROLATTRIBUTES *ptrDscPara = DscMan::GetInstance()->GetDscPara();
				ptrDscPara->dcaCFMIPAttrs.ipaArtifact = 0;
				break;
			}
#endif
			if (ModeStatus::IsUnFreezeMode())
			{
				Img2D::GetInstance()->ChangeSoundPower(SUB);
				ImgPw::GetInstance()->ChangeSoundPower(SUB);

				// change tis
				ChangeTis();
			}
			break;

		case KEY_MBP:
			if (ModeStatus::IsEFOVMode())
			{
				HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
				break;
			}

			if (ModeStatus::IsUnFreezeMode())
			{
				ModeStatus s;
				int mode = s.GetScanMode();

				if ((mode == ScanMode::CFM) || (mode == ScanMode::PDI) || (mode == ScanMode::PWCFM_INIT) || (mode == ScanMode::PWPDI_INIT)
						|| (mode == ScanMode::PWCFM) || (mode == ScanMode::PWPDI) || (mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
				{
					ImgCfm::GetInstance()->ChangeMBP(TRUE);
				}
				else
				{
					ImgCfm::GetInstance()->ChangeMBP(FALSE);
				}
			}
			break;

            case KEY_S:
            {
                m_vecSuperAuthenInfo.clear();
                if (m_super_timer > 0)
                {
                    g_source_remove(m_super_timer);
                    m_super_timer = 0;
                }
                m_statusSuperAuthen = TRUE;
                m_super_timer = g_timeout_add(5000, HandleSuperAuthen, NULL);
            }
            // ViewSuperuser::GetInstance()->Authenticate();
			break;
          //  case KEY_SUPER_E:
          //  case KEY_SUPER_M:
            case KEY_SUPER_P:
          //case KEY_SUPER_E:
            case KEY_SUPER_R:
           // case KEY_SUPER_O:
          //case KEY_SUPER_R:
            case KEY_SUPER_1:
            case KEY_SUPER_9:
          //case KEY_SUPER_9:
            case KEY_SUPER_7:
            {
                if(m_statusSuperAuthen)
                    m_vecSuperAuthenInfo.push_back(keyValue);
            }
            break;
#ifndef EMP_3410
            case KEY_D:
            {
                m_vecAuthenInfo.clear();
                if (m_timer > 0)
                {
                    g_source_remove(m_timer);
                    m_timer = 0;
                }
                m_vecAuthenInfo.push_back(keyValue);
                m_statusAuthen = TRUE;
                m_timer = g_timeout_add(5000, HandleAuthen, NULL);
            }
            // ViewDicomSupervise::GetInstance()->Authenticate();
            break;

            case KEY_I:
            case KEY_C:
            {
                if(m_statusAuthen)
                    m_vecAuthenInfo.push_back(keyValue);
            }
            break;
            case KEY_DICOM_M:
            case KEY_O:
            case KEY_E:
            {
                if(m_statusAuthen)
                    m_vecAuthenInfo.push_back(keyValue);
                if(m_statusSuperAuthen)
                    m_vecSuperAuthenInfo.push_back(keyValue);
            }
            break;
#endif
/*
            case '1':
            {
               Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 1);
            }
            break;

        case 'q':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 1);
            }
            break;

        case '2':
            {
               Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 2);
                //ScanMode::GetInstance()->ChangePulseNum(ADD);
            }
            break;

        case 'w':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 2);
                //ScanMode::GetInstance()->ChangePulseNum(SUB);
            }
            break;

        case '3':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 3);
                //ImgCfm::GetInstance()->ChangeLowFilterFc(ADD);
            }
            break;

        case KEY_E:
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 3);
                //ImgCfm::GetInstance()->ChangeLowFilterFc(SUB);
            }
            break;

        case '4':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 4);
            }
            break;

        case 'r':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 4);
            }
            break;

        case '5':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 5);
            }
            break;

        case 't':
            {
                Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 5);
            }
            break;

        case 'a':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(ADD, 1);
            }
            break;

        case 'z':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(SUB, 1);
            }
            break;

        case 's':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(ADD, 2);
            }
            break;

        case 'x':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(SUB, 2);
            }
            break;

        case 'd':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(ADD, 3);
            }
            break;

        case KEY_C:
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(SUB, 3);
            }
            break;

        case 'f':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(ADD, 4);
            }
            break;

        case 'v':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(SUB, 4);
            }
            break;

        case 'g':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(ADD, 5);
            }
            break;

        case 'b':
            {
                Img2D::GetInstance()->ChangeFreqDynamicFilter_test(SUB, 5);
            }
            break;

        case '6':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(ADD, 1);
            }
            break;

        case 'y':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(SUB, 1);
            }
            break;

        case '7':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(ADD, 2);
            }
            break;

        case 'u':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(SUB, 2);
            }
            break;

        case '8':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(ADD, 3);
            }
            break;

        case KEY_I:
        //case 'i':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(SUB, 3);
            }
            break;

        case '9':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(ADD, 4);
            }
            break;

        //case 'o':
        case KEY_O:
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(SUB, 4);
            }
            break;

        case '0':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(ADD, 5);
            }
            break;

        case 'p':
            {
                Img2D::GetInstance()->ChangeDynamicDemodFd(SUB, 5);
            }
            break;

        case 'h':
            {
                EKnobOper oper;
                oper = ADD;
                Img2D::GetInstance()->ChangeBPFilter(oper);
            }
            break;

        case 'n':
            {
                EKnobOper oper;
                oper = SUB;
                Img2D::GetInstance()->ChangeBPFilter(oper);
            }
            break;

        case 'j':
            {
                EKnobOper oper;
                oper = ADD;
                Img2D::GetInstance()->ChangeHarmonicFreq(oper);
            }
            break;

       // case 'm':
        case KEY_DICOM_M:
            {
                EKnobOper oper;
                oper = SUB;
                Img2D::GetInstance()->ChangeHarmonicFreq(oper);
            }
            break;
        case 'k':
            {
                // Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 1);
                ImgPw::GetInstance()->ChangeBandPassFilter(ADD);
            }
            break;

        case KEY_TEST_1:
            {
                //Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 1);
                ImgPw::GetInstance()->ChangeBandPassFilter(SUB);
            }
            break;

        case 'l':
            {
               // Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 2);
                ScanMode::GetInstance()->ChangePulseNum(ADD);
            }
            break;

        case KEY_TEST_2:
            {
                //Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 2);
                ScanMode::GetInstance()->ChangePulseNum(SUB);
            }
            break;

        case KEY_TEST_3:
            {
                //Img2D::GetInstance()->ChangeFreqBPFilter_test(ADD, 3);
                ImgCfm::GetInstance()->ChangeLowFilterFc(ADD);
            }
            break;

        case KEY_UP:
            {
                //Img2D::GetInstance()->ChangeFreqBPFilter_test(SUB, 3);
                ImgCfm::GetInstance()->ChangeLowFilterFc(SUB);
            }
*/

        default:
			break;
	}
}

/* add demo
 * hj
 */
void ViewMain::MenuReviewCallBack(void)
{
	if (ModeStatus::IsEFOVMode() && !Replay::GetInstance()->GetReadSnapStatus())
	{
		if (KeyReview::m_menuRead == TRUE)
		{
			KeyReview km;
			km.ExitMenuReivew();
		}
		else
		{
			HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
		}
	}
    else
    {
    	if (ModeStatus::IsAutoReplayMode())
    		FreezeMode::GetInstance()->ChangeAutoReplay();
    	FreezeMode::GetInstance()->ExitAutoReplay();

    	KeyReview km;
        if(ViewSuperuser::GetInstance()->GetDemoStatus()) //demo
    	    km.SetDemoIDAndPath();
        else
    	    km.SetDefaultIDAndPath();

    	km.Execute();
    }
}
gboolean ViewMain::IsAuthenValid(void)
{
    m_statusAuthen = false;
    if (m_timer > 0)
    {
        g_source_remove(m_timer);
        m_timer = 0;
    }
    //if (m_vecAuthenInfo.empty() || ((!m_vecAuthenInfo.empty()) && m_vecAuthenInfo[0] != 'd'))
   // {
     //   printf("not authen\n");
   // }
   // else
   if(!m_vecAuthenInfo.empty())
    {
        //printf("authen\n");
        unsigned char info[AUTHEN_NUM+1];
        int size = m_vecAuthenInfo.size();
        int i;

        if(size <= AUTHEN_NUM)
        {
            info[size] = '\0';
            for (i = 0; i < size; i ++)
            {
                info[i] = m_vecAuthenInfo[i];
            }

            // clear
            m_vecAuthenInfo.clear();

            // compare
            if (strcmp((const char*)info, "dicome") == 0)
                ViewDicomSupervise::GetInstance()->CreateWindow();
        }
        else
            m_vecAuthenInfo.clear();
    }
    return FALSE;
}

gboolean ViewMain::IsSuperAuthenValid(void)
{
    m_statusSuperAuthen = false;
    if (m_super_timer > 0)
    {
        g_source_remove(m_super_timer);
        m_super_timer = 0;
    }
   if(!m_vecSuperAuthenInfo.empty())
    {
        unsigned char info[SUPER_AUTHEN_NUM+1];
        int size = m_vecSuperAuthenInfo.size();
        int i;

        if(size <= SUPER_AUTHEN_NUM)
        {
            info[size] = '\0';
            for (i = 0; i < size; i ++)
            {
                info[i] = m_vecSuperAuthenInfo[i];
            }

            // clear
            m_vecSuperAuthenInfo.clear();
            // compare
            if (strcmp((const char*)info, "emperor1997") == 0)
                ViewSuperuser::GetInstance()->CreateWindow();

            // compare
            if (strcmp((const char*)info, "e1997") == 0)
                ViewSuperuser::GetInstance()->CreateDemoWindow();
        }
        else
            m_vecSuperAuthenInfo.clear();
    }

    return FALSE;
}

void ViewMain::KnobEvent(unsigned char keyValue, unsigned char offset)
{
	AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();
	ModeStatus s;
	ScanMode::EScanMode mode = s.GetScanMode();

	if ((offset != 1) && (offset != 0))
		offset = 1;

	EKnobOper knob;
	if (offset == 0)
		knob = SUB;
	if (offset == 1)
		knob = ADD;

   //close knob event in demo. lhm
   if ((MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW)&&(ViewSuperuser::GetInstance()->GetDemoStatus()))
      {
        return;
      }

	switch (keyValue)
	{
#if (defined(EMP_460) || defined(EMP_355))
        case KNOB_VALUE:
#endif
			{
                ptrMultiFunc->Value(knob);
			}
			break;

#ifndef EMP_460
#ifndef EMP_355
       	case KNOB_2D: // 2D
          if (ModeStatus::IsEFOVMode()) {
                        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                        break;
                    }
			if (ModeStatus::IsUnFreezeMode())
			{
				Img2D::GetInstance()->ChangeGain2D(knob);
			}
			break;

		case KNOB_M: // M
			if (ModeStatus::IsUnFreezeMode())
			{
                if (mode == ScanMode::M)
                    Img2D::GetInstance()->ChangeGainM(knob);
            }
            break;

        case KNOB_CW: // CW
         if (ModeStatus::IsUnFreezeMode())
            {

                if( ModeStatus::IsCWImgMode())
                {
                 ImgPw::GetInstance()->ChangeGainCw(knob);
                }
            }
            break;
        case KNOB_PW: // PW
            if (ModeStatus::IsUnFreezeMode())
            {
                if (ModeStatus::IsPWImgMode())
                {
                    ImgPw::GetInstance()->ChangeGainPw(knob);
                }
            }
            break;

		case KNOB_PDI: // PDI
			if (ModeStatus::IsUnFreezeMode())
			{
                if (ModeStatus::IsPDIMode() || ModeStatus::IsPDICompareMode())
					ImgCfm::GetInstance()->ChangeGainCfm(knob);
			}
			break;

		case KNOB_CFM: // CFM
			if (ModeStatus::IsUnFreezeMode())
			{
                if (ModeStatus::IsCFMMode() || ModeStatus::IsCFMCompareMode())
					ImgCfm::GetInstance()->ChangeGainCfm(knob);
			}
			break;
#endif
#endif

#if defined(EMP_322)
        case KNOB_GAIN:
            {
                ModeStatus MStatus;
                int mode;
                mode = MStatus.GetScanMode();
                if (ModeStatus::IsEFOVMode()) {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }
                if (ModeStatus::IsUnFreezeMode())
                {
                    if(mode == ScanMode::D2)
                    {
                        Img2D::GetInstance()->ChangeGain2D(knob);
                    }
                    else if((mode == ScanMode::M) || (mode == ScanMode::M_INIT))
                    {
                        Img2D::GetInstance()->ChangeGainM(knob);
                    }
                }
            }
            break;

        case KNOB_VALUE: // value
            {
                if (g_ptrKeyFunc == NULL)
                {
                    ptrMultiFunc->Value(knob);
                }
                else
                {
                    g_ptrKeyFunc(knob);
                }
            }
            break;
#elif (defined(EMP_460) || defined(EMP_355))
     case KNOB_GAIN:
			{
			    ModeStatus MStatus;
                int mode;
                mode = MStatus.GetScanMode();
                if (ModeStatus::IsEFOVMode()) {
                    HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
                    break;
                }
                if (ModeStatus::IsUnFreezeMode())
                {
                    if(mode == ScanMode::D2)
                    {
                        Img2D::GetInstance()->ChangeGain2D(knob);
                    }
                     if(mode == ScanMode::M)
                    {
                        Img2D::GetInstance()->ChangeGainM(knob);
                    }
                     if (ModeStatus::IsColorImgMode())
                     {
                         ImgCfm::GetInstance()->ChangeGainCfm(knob);
                     }
                     if (ModeStatus::IsPWImgMode())
                     {
                         ImgPw::GetInstance()->ChangeGainPw(knob);
                     }
                     if( ModeStatus::IsCWImgMode())
                     {
                         ImgPw::GetInstance()->ChangeGainCw(knob);
                     }
                }
			}
          break;

#elif defined(EMP_313)
      case KNOB_VALUE: // value
            {
                if (g_ptrKeyFunc == NULL)
                {
                    ptrMultiFunc->Value(knob);
                }
                else
                {
                    g_ptrKeyFunc(knob);
                }
            }
            break;
#else
        case KNOB_VALUE: // value
            {
                ptrMultiFunc->Value(knob);
            }
            break;
#endif
		default:
			CKnobEvent::KnobEvent(keyValue, offset);
			break;
	}
}

int g_tgcTimer = 0;
gboolean TgcCallBack(gpointer data)
{
    ModeStatus s;
    int mode = s.GetScanMode();

    Img2D::GetInstance()->ChangeTgc2D(g_tgcSlider);
 //   if (mode == ScanMode::CFM)
  //  {
   //     ImgCfm::GetInstance()->ChangeTgcCfm(g_tgcSlider);
 //   }
    if ((mode == ScanMode::M) || (mode == ScanMode::M_INIT))
        Img2D::GetInstance()->ChangeTgcM(g_tgcSlider);

    g_tgcTimer = 0;
	return false;
}
void ViewMain::SliderEvent(unsigned char keyValue, unsigned char offset)
{
   //close knob event in demo. lhm
   if ((MenuArea::GetInstance()->GetMenuType() == MenuArea::REVIEW)&&(ViewSuperuser::GetInstance()->GetDemoStatus()))
      {
        return;
      }

    if (ModeStatus::IsEFOVMode()) {
        HintArea::GetInstance()->UpdateHint(_("Invalid in current mode."), 1);
        if ((offset >= SLIDER_OFF) && (offset <= (SLIDER_OFF + 8)))
            g_tgcSlider[offset - SLIDER_OFF] = keyValue;
        return ;
    }

    if ((offset >= SLIDER_OFF) && (offset <= (SLIDER_OFF + 7)))
    {
        g_tgcSlider[offset - SLIDER_OFF] = keyValue;

        // remove timer if is start-up
        if (g_tgcTimer > 0)
        {
            if (!g_source_remove(g_tgcTimer))
            {
                PRINTF("remove tgc timer error[%d]\n", offset-SLIDER_OFF);
                return;
            }
            g_tgcTimer = 0;
        }

        // start-up timer
        int interval = 10;
        g_tgcTimer = g_timeout_add(interval, TgcCallBack, NULL);
    }
#if defined (EMP_313)
    else if (offset == (SLIDER_OFF + 8))
    {
        g_tgcSlider[offset - SLIDER_OFF] = keyValue;
        ModeStatus ms;
        ScanMode::EScanMode mode = ms.GetScanMode();

        if (ModeStatus::IsUnFreezeMode())
        {
            if(mode == ScanMode::D2)
            {
                Img2D::GetInstance()->ChangeGain2D(keyValue);
            }
            else if (mode == ScanMode::M)
            {
                Img2D::GetInstance()->ChangeGainM(keyValue);
            }
        }
    }
#endif
}

void ViewMain::MouseEvent(char offsetX, char offsetY)
{
#ifdef EMP_355
    if(QuickAdjustmentPw::m_onQAPw)
    {
        return;
    }
#endif

	AbsMultiFunc* ptrMulti = MultiFuncFactory::GetInstance()->GetObject();
	ptrMulti->Mouse(offsetX, offsetY);
}

void ViewMain::KnobKeyEvent(unsigned char keyValue)
{
#ifdef EMP_355
    if(QuickAdjustmentPw::m_onQAPw)
    {
        return;
    }
#endif

    KnobMenu* ptrKnob = KnobMenu::GetInstance();

    switch(keyValue)
    {
        case KEY_F1:
#ifdef EMP_PROJECT
            if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&m_ptrKnobProjectMode->GetKnobStatus())
                m_ptrKnobProjectMode->Knob1_Press();
            else
                ptrKnob->Knob1_Press();
#else
            ptrKnob->Knob1_Press();
#endif
            break;

        case KEY_F2:
#ifdef EMP_PROJECT
            if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&m_ptrKnobProjectMode->GetKnobStatus())
                m_ptrKnobProjectMode->Knob2_Press();
            else
                ptrKnob->Knob2_Press();
#else
     ptrKnob->Knob2_Press();
#endif
            break;

        case KEY_F3:
#ifdef EMP_PROJECT
            if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&m_ptrKnobProjectMode->GetKnobStatus())
                m_ptrKnobProjectMode->Knob3_Press();
            else
                ptrKnob->Knob3_Press();
#else
   ptrKnob->Knob3_Press();
#endif
            break;

        case KEY_F4:
#ifdef EMP_PROJECT
            if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&m_ptrKnobProjectMode->GetKnobStatus())
                m_ptrKnobProjectMode->Knob4_Press();
            else
                ptrKnob->Knob4_Press();
#else
        ptrKnob->Knob4_Press();
#endif
            break;

        case KEY_F5:
#ifdef EMP_PROJECT
            if(ViewSuperuser::GetInstance()->GetProjectModeStatus()&&m_ptrKnobProjectMode->GetKnobStatus())
                m_ptrKnobProjectMode->Knob5_Press();
            else
                ptrKnob->Knob5_Press();
#else
 ptrKnob->Knob5_Press();
#endif
            break;
#if (defined (EMP_322) || defined(EMP_313))
        case KEY_F6:
            ptrKnob->Knob6_Press();
            break;
#endif
        default:
            break;
    }

}

gboolean UpdateTopArea(gpointer data)
{
    TopArea *tmp;
    tmp = (TopArea *)data;
    tmp->DrawDateTime();
    return TRUE;
}

////////////////////////////[ViewMain]/////////////////////////////
ViewMain* ViewMain::m_ptrInstance = NULL;

ViewMain::ViewMain()
{
    m_mainWindow = 0;
    countN = 0;
    m_ptrKnob = KnobMenu::GetInstance();
    m_ptrImgArea = ImageArea::GetInstance();
    m_ptrTopArea = TopArea::GetInstance();
    m_ptrHintArea = HintArea::GetInstance();
    m_ptrMenuArea = MenuArea::GetInstance();
    m_ptrNoteArea = NoteArea::GetInstance();

#ifdef EMP_PROJECT
    m_ptrKnobProjectMode = KnobProjectMode::GetInstance();
#endif

    m_modeIsFlag = false;
    keyTSIN = 0;
    m_super_timer = 0;
}

ViewMain::~ViewMain()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewMain* ViewMain::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewMain;

    return m_ptrInstance;
}

GtkWidget* ViewMain::GetMainWindow(void)
{
    return m_mainWindow;
}

void ViewMain::Show(void)
{
    gtk_widget_show_all(m_mainWindow);
}

void ViewMain::Hide(void)
{
    gtk_widget_hide_all(m_mainWindow);
}

void ViewMain::ShowMenu(void)
{
	gtk_widget_hide(m_daMenu);
}

void ViewMain::HideMenu(void)
{
	gtk_widget_show(m_daMenu);
}

void ViewMain::Create(void)
{
    UserSelect::GetInstance()->create_userconfig_dir();//addec by LL
     UserSelect::GetInstance()->create_userdefined_dir();
     UserSelect::GetInstance()->create_commentdefined_dir();
    UserSelect::GetInstance()->create_usercalcdefined_dir();
    UserSelect::GetInstance()->creat_username_db(USERNAME_DB);
    UserSelect::GetInstance()->save_active_user_id(0);

    m_mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(m_mainWindow), FALSE);
    gtk_window_set_position(GTK_WINDOW(m_mainWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(m_mainWindow), FALSE);
    gtk_widget_set_size_request(m_mainWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_container_set_border_width(GTK_CONTAINER(m_mainWindow), 0);
    gtk_widget_modify_bg(m_mainWindow, GTK_STATE_NORMAL, g_deep);

//	g_signal_connect(m_mainWindow,"key-press-event",G_CALLBACK(HandleKeyPressEvent),this);

    m_fixedWindow = gtk_fixed_new();
    gtk_widget_set_usize(m_fixedWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_widget_set_uposition(m_fixedWindow, 0, 0);
    gtk_container_add(GTK_CONTAINER(m_mainWindow), m_fixedWindow);

    // Knob Area
    GtkWidget *tableKnob;
    tableKnob = m_ptrKnob->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), tableKnob, KNOB_X, KNOB_Y);
#if 0
    char tmp[20];
    int detal_x = WIDTH_KNOB_MENU / 20;
    int detal_y = HEIGHT_KNOB_MENU / 3;
    GtkWidget* label_tmp;
    for(int i = 0; i < 5; i++)
    {
        sprintf(tmp, "F%d", i+1);
        label_tmp = gtk_label_new_with_mnemonic(tmp);
        gtk_fixed_put(GTK_FIXED(m_fixedWindow), label_tmp, KNOB_X + detal_x + i * detal_x * 18 / 5, KNOB_Y + detal_y);
    }
#endif
    // Menu Area
    GtkWidget *tableMenu;
    tableMenu = m_ptrMenuArea->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), tableMenu, 0, TOP_AREA_H);

    m_daMenu = gtk_drawing_area_new();
	gtk_widget_modify_bg(m_daMenu, GTK_STATE_NORMAL, g_black);
    gtk_drawing_area_size(GTK_DRAWING_AREA(m_daMenu), MENU_AREA_W, IMG_AREA_H+HINT_AREA_H);
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), m_daMenu, 0, TOP_AREA_H+2);

    // 2D knob menu
    KnobD2Create();

    // image area
    GtkWidget *da_image;
    da_image = m_ptrImgArea->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), da_image, IMG_AREA_X, IMG_AREA_Y);
    m_ptrImgArea->AddTimeOutFps();

    // note area
    GtkWidget *canvas_note;
    canvas_note = m_ptrNoteArea->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), canvas_note, IMAGE_X+IMG_AREA_X, IMAGE_Y+IMG_AREA_Y);

    // Top area
    GtkWidget *da_topArea;
    da_topArea = m_ptrTopArea->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), da_topArea, TOP_AREA_X, TOP_AREA_Y);
    m_ptrTopArea->AddTimeOut();

    // status area
    // GtkWidget *status_area = gtk_drawing_area_new();
    // gtk_drawing_area_size(GTK_DRAWING_AREA(status_area), STATUS_WIDTH, STATUS_HEIGHT);
    // gtk_fixed_put(GTK_FIXED(m_fixedWindow),status_area, STATUS_X, STATUS_Y);
    // gtk_widget_modify_bg(status_area, GTK_STATE_NORMAL, g_black);

    GtkWidget *da_hintArea;
    da_hintArea = m_ptrHintArea->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), da_hintArea, HINT_X, HINT_Y);

#ifdef EMP_PROJECT
    GtkWidget *knobProjectMode;
    knobProjectMode = m_ptrKnobProjectMode->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), knobProjectMode, HINT_X, HINT_Y);
  #endif

    // icon view
    ViewIcon::GetInstance()->Create();
    gtk_fixed_put(GTK_FIXED(m_fixedWindow), ViewIcon::GetInstance()->GetIconArea(), 5, 742); // 740

    g_keyInterface.Push(this);

    Show();
    gtk_widget_hide(m_daMenu);

#ifdef EMP_PROJECT
    if(!ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        m_ptrMenuArea->HideProjectModeMenu();
        m_ptrKnobProjectMode->HideKnob();
    }
    else
    {
        m_ptrMenuArea->ShowProjectModeMenu();
        m_ptrKnobProjectMode->ShowKnob();
    }

    m_ptrHintArea->UpdateHintArea();
#endif

    m_ptrNoteArea->Hide();

    // update top area
    SysGeneralSetting *sysGeneralSetting = new SysGeneralSetting;
    string hospital_name;
    sysGeneralSetting->GetHospital(hospital_name);
    delete sysGeneralSetting;

    m_ptrTopArea->UpdateHospitalName(hospital_name.c_str());

    // m_ptrTopArea->UpdateProbeType("4C-RS");
    // m_ptrTopArea->UpdateCheckPart("ABD");
    // m_ptrTopArea->UpdateDepth(167);
    // m_ptrTopArea->UpdateMI(0.9);
    // m_ptrTopArea->UpdateTIS(0.4);

    // patient info
    g_patientInfo.UpdateTopArea();

    // ViewSystem::GetInstance()->CreateWindow();
    // ViewNewPat::GetInstance()->CreateWindow();
    // for test, enter archive dialog
    //ClickArchive(NULL);
    //test(NULL);

#ifdef EMP_3410
	if(g_authorizationOn)
		CEmpAuthorization::Create(&g_keyInterface, REGISTER_FILE_PATH, 0);
#else
	if(g_authorizationOn)
		CEmpAuthorization::Create(&g_keyInterface, REGISTER_FILE_PATH, 1);
#endif
}

void ViewMain::MySleep(int msecond)
{
    int sum = Img2D::GetInstance()->GetFocSum();
    usleep(msecond * sum);
}

/*gboolean ViewMain::KeyFilter(GtkWidget* entry,GdkEventKey* event)
{
	switch(event->keyval)
	{
		case GDK_F1:
		MenuArea::GetInstance()->ShowBiopsyMenu();

			break;
		case GDK_F2:
			MenuArea::GetInstance()->ShowBioBracketMenu();
			break;
		case GDK_F3:
			MenuArea::GetInstance()->ShowBioVerifyMenu();
			break;
		default:
			break;
	}
	return TRUE;
}*/