/*
 * 2010, 深圳恩普电子技术有限公司
 *
 * @file: MultiFuncMode.cpp
 * @brief: keyboard multifunction for scan mode
 *
 * version: V1.0
 * date: 2010-9-27
 * @author: zhanglei
 */

#include "MultiFuncMode.h"
#include "ScanMode.h"
#include "ImgPw.h"
#include "ImgCfm.h"
#include "MultiFuncFactory.h"
#include "gui_func.h"
#include "FreezeMode.h"
#include "ModeStatus.h"
#include "KeyFunc.h"
#include "ImgProcM.h"
#include "ImgProc2D.h"
#include "MeasureFactory.h"
#include "ViewSuperuser.h"
#include "MeasureD.h"

bool updateoffsound=false;
extern int soundstatus;
int tempoffsound=0;
///>global 
void OverturnMouseOpr(int &offsetX, int &offsetY)
{
	bool lrStatus = ImgProc2D::GetInstance()->GetLRStatus();
	bool udStatus = ImgProc2D::GetInstance()->GetUDStatus();
	if (lrStatus)
		offsetX = -offsetX;
	if (udStatus)
		offsetY = -offsetY;
}

///> none
void MultiFuncNone::Do()
{
	doBtnEvent(1, 1);
	doBtnEvent(1, 0);
}

void MultiFuncNone::Mouse(int offsetX, int offsetY)
{
	fakeXMotionEvent(offsetX, offsetY);
}
void MultiFuncNone::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncNone::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncNone::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncNone::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}

void MultiFuncNone::Update()
{
    MultiFuncUndo();    
    MultiFuncFactory::EMultiFunc type = MultiFuncFactory::GetInstance()->GetMultiFuncType();
    if (type != MultiFuncFactory::NONE)
    {
        AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();
        ptrMultiFunc->Update();
    }
}

void MultiFuncNone::Value(EKnobOper opr)
{
    MultiFuncUndo();    
    MultiFuncFactory::EMultiFunc type = MultiFuncFactory::GetInstance()->GetMultiFuncType();
    if (type != MultiFuncFactory::NONE)
    {
        AbsMultiFunc* ptrMultiFunc = MultiFuncFactory::GetInstance()->GetObject();
        ptrMultiFunc->Value(opr);
    }
}

void MultiFuncNone::Undo()
{
    //if (MenuArea::GetInstance()->GetMenuType() == MenuArea::MEASURE)
        MeasureFactory::GetInstance()->UndoLastMeasure();
}
///> Replay 
extern AbsMeasure* g_ptrAbsMeasure;

void MultiFuncReplay::Update()
{
    ModeStatus Modestatus;
    FreezeMode::EFreezeMode mode = Modestatus.GetFreezeMode();
    ScanMode::EScanMode scanMode = ScanMode::GetInstance()->GetScanMode(); 
    if((mode == FreezeMode::FREEZE || mode == FreezeMode::REPLAY) && (scanMode == ScanMode::PW || scanMode == ScanMode::PWCFM || scanMode == ScanMode::PWPDI))
    {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::MEASURE);
        if(g_menuPW.GetAutoTraceStatus())
        {
            ImgProcPw::GetInstance()->SetAutoCalc(FALSE);
        }

        if(g_ptrAbsMeasure != NULL)
        {
            delete g_ptrAbsMeasure;
            g_ptrAbsMeasure = NULL;
        }
        g_ptrAbsMeasure = new DMeasureManualAuto();
    }
#if 0
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode(); 
    if (mode == ScanMode::PW)
    {
        MultiFuncPw multi;
        multi.Update();
    }
    else if (mode == ScanMode::PWCFM)
    {
        MultiFuncPwCfm multi;
        multi.Update();
    }
    else if (mode == ScanMode::PWPDI)
    {
        MultiFuncPwPdi multi;
        multi.Update();
    }
#endif
}

void MultiFuncReplay::Mouse(int offsetX, int offsetY)
{
    if(ViewSuperuser::GetInstance()->GetDemoStatus())return;//demo mode do nothing and exit!!
    
	if (m_count < 4) // to control switch beteen auto-replay and manual-replayy
	{
		m_count ++;
	}
	else
	{
		m_count = 0;

		if (offsetX > 0)
		{
          // delete auto-replay 

			//if (abs(offsetX) > 2)
		//	{
		//		FreezeMode::GetInstance()->EnterAutoReplayView();
		//	}
		//	else
		//	{
				FreezeMode::GetInstance()->ExitAutoReplay();
				Replay::GetInstance()->ViewOneImg();
		//	}
		}
		else if (offsetX < 0)
		{
		//	if (abs(offsetX) > 2)
		//	{
		//		FreezeMode::GetInstance()->EnterAutoReplayReview();
		//	}
		//	else
		//	{
				FreezeMode::GetInstance()->ExitAutoReplay();
				Replay::GetInstance()->ReviewOneImg();
		//	}
		}
	}
}
void MultiFuncReplay::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncReplay::Do()
{
}
void MultiFuncReplay::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
///> M init
void MultiFuncMInit::Update()
{
	ScanMode::GetInstance()->UpdateM();
}
void MultiFuncMInit::Mouse(int offsetX, int offsetY)
{
	ModeStatus ms;
	if (ModeStatus::IsMImgMode() && (ms.GetFormatM() == FormatM::M_TOTAL))
		return;

	OverturnMouseOpr(offsetX, offsetY);
	Img2D::GetInstance()->ChangeMLine(offsetX, offsetY);
}
void MultiFuncMInit::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;
	
	Mouse(offsetX, offsetY);
}
void MultiFuncMInit::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;
	
	Mouse(offsetX, offsetY);
}
///> M
void MultiFuncM::Mouse(int offsetX, int offsetY)
{
	ModeStatus s;
	ScanMode::EScanMode mode = s.GetScanMode();
	FormatM::EFormatM format = s.GetFormatM();

	if ((mode == ScanMode::M) && (format == FormatM::M_TOTAL))
		return;
	
	OverturnMouseOpr(offsetX, offsetY);
	Img2D::GetInstance()->ChangeMLine(offsetX, offsetY);
}
void MultiFuncM::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncM::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
////> pw init
void MultiFuncPwInit::Update()
{
    ScanMode::GetInstance()->UpdatePw();
}

void MultiFuncPwInit::Value(EKnobOper oper)
{
#if (defined(EMP_460) || defined(EMP_355))
     if (ModeStatus::IsUnFreezeMode())
	 {
         ImgPw::GetInstance()->ChangeCorrectAngle(oper);
	 }
#else
	 return;
#endif
}
void MultiFuncPwInit::Mouse(int offsetX, int offsetY)
{
	OverturnMouseOpr(offsetX, offsetY);

    //redraw center line
    KeyCenterLine key;
    key.Execute();

	ImgPw::GetInstance()->ChangePwSV(offsetX, offsetY);
}
void MultiFuncPwInit::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncPwInit::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncPwInit::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncPwInit::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}
// pw
void MultiFuncPw::Update()
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsNormalSpectrumImgMode() && (format == FormatPw::P_TOTAL))
		return;

    if (ModeStatus::IsSimultMode())
        return;

	ScanMode::GetInstance()->SwitchPw();
}

void MultiFuncPw::Value(EKnobOper oper)
{
#if (defined(EMP_460) || defined(EMP_355))
     if (ModeStatus::IsUnFreezeMode())
	 {
         ImgPw::GetInstance()->ChangeCorrectAngle(oper);
	 }
#else
	 return;
#endif
}

void MultiFuncPw::Mouse(int offsetX, int offsetY)
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsSpectrumImgMode() && (format == FormatPw::P_TOTAL))
		return;
    
  	OverturnMouseOpr(offsetX, offsetY);
	ImgPw::GetInstance()->ChangePwSV(offsetX, offsetY);
}
void MultiFuncPw::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncPw::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncPw::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncPw::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}
///> cfm 
void MultiFuncCfm::Do()
{
	bool changeSize = ImgCfm::GetInstance()->GetBoxStatus();
	ImgCfm::GetInstance()->SetBoxStatus(!changeSize);
}
void MultiFuncCfm::Mouse(int offsetX, int offsetY)
{
	OverturnMouseOpr(offsetX, offsetY);
    
    //redraw center line
    KeyCenterLine key;
    key.Execute();

	bool changeSize = ImgCfm::GetInstance()->GetBoxStatus();
    if (changeSize)
	{
		bool lrStatus = ImgProc2D::GetInstance()->GetLRStatus();
		bool udStatus = ImgProc2D::GetInstance()->GetUDStatus();
		ImgCfm::GetInstance()->ChangeBoxSize(offsetX, offsetY, lrStatus, udStatus);

		// Change Tis
		ChangeTis();
	}
	else
	{
		ImgCfm::GetInstance()->ChangeBoxPos(offsetX, offsetY);
	}

}
void MultiFuncCfm::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCfm::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCfm::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncCfm::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}

/// pwcfm init
void MultiFuncPwCfmInit::Do()
{
	if (m_change < 2)
		m_change ++;
	else
		m_change = 0;
	
	if ((m_change == 1) || (m_change == 2))
		ImgCfm::GetInstance()->SetBoxStatus(m_change-1);
	else
		ImgCfm::GetInstance()->SetBoxStatus(false);
}
void MultiFuncPwCfmInit::Update()
{
    ScanMode::GetInstance()->UpdatePwCfmFromInit();
}

void MultiFuncPwCfmInit::Value(EKnobOper oper)
{
#if (defined(EMP_460) || defined(EMP_355))
     if (ModeStatus::IsUnFreezeMode())
	 {
         ImgPw::GetInstance()->ChangeCorrectAngle(oper);
	 }
#else
	 return;
#endif
}

void MultiFuncPwCfmInit::Mouse(int offsetX, int offsetY)
{
	ModeStatus ms;
	FormatPw::EFormatPw formatPw = ms.GetFormatPw();

	if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) &&
			formatPw == FormatPw::P_TOTAL)
		return;
  
    OverturnMouseOpr(offsetX, offsetY);

    //redraw center line
        KeyCenterLine key;
        key.Execute();

    if (m_change == 0) //sv
    {
        ImgPw::GetInstance()->ChangePwSV(offsetX, offsetY);
    }
    else if (m_change == 1) //box pos
    {
        ImgCfm::GetInstance()->ChangeBoxPos(offsetX, offsetY);
    }
    else // box size
    {
        bool lrStatus = ImgProc2D::GetInstance()->GetLRStatus();
        bool udStatus = ImgProc2D::GetInstance()->GetUDStatus();

        ImgCfm::GetInstance()->ChangeBoxSize(offsetX, offsetY, lrStatus, udStatus);

        // Change Tis
        ChangeTis();
    }
}

void MultiFuncPwCfmInit::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncPwCfmInit::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncPwCfmInit::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncPwCfmInit::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}

///> pw+cfm
void MultiFuncPwCfm::Update()
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsNormalSpectrumColorImgMode() && (format == FormatPw::P_TOTAL))
		return;

    if (ModeStatus::IsSimultMode())
        return;

	ScanMode::GetInstance()->SwitchPwCfm();
}

///> pw+pdi init
void MultiFuncPwPdiInit::Update()  
{
	ScanMode::GetInstance()->UpdatePwPdiFromInit();
}

///> pw+pdi
void MultiFuncPwPdi::Update()
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsNormalSpectrumColorImgMode() && (format == FormatPw::P_TOTAL))
		return;

    if (ModeStatus::IsSimultMode())
        return;

	ScanMode::GetInstance()->SwitchPwPdi();
}

///> anotomic M mode
void MultiFuncAnatomicM::Mouse(int offsetX, int offsetY)
{
	//OverturnMouseOpr(offsetX, offsetY);

	if (m_moveStatus) // move anatomic line
	{
		ImgProcM::GetInstance()->MoveAnatomicLine(offsetX, offsetY);
	}
	else // change anatomic line
	{
		ImgProcM::GetInstance()->ChangeAnatomicLine(offsetX, offsetY);
	}
}
void MultiFuncAnatomicM::Do()
{
	if (m_moveStatus)
		m_moveStatus = FALSE;
	else
		m_moveStatus = TRUE;
}
void MultiFuncAnatomicM::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncAnatomicM::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}
void MultiFuncAnatomicM::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);	
}
void MultiFuncAnatomicM::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);	
}

void MultiFuncEFOV::Update()
{
	if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::PREPARE)
	{
		ScanMode::GetInstance()->EnterEFOVCapture();
	}
        else if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::CAPTURE)
	{
		ScanMode::GetInstance()->EnterEFOVPrepare();
	}
}

void MultiFuncEFOV::Mouse(int offsetX, int offsetY)
{
	if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::VIEW)
	{
		if (offsetX >= 0)
			ImgProc2D::GetInstance()->ChangeEFOVZoomPos(ADD);
		else
			ImgProc2D::GetInstance()->ChangeEFOVZoomPos(SUB);
	}
	else if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::REVIEW)
	{
		if (offsetX >= 0)
			Replay::GetInstance()->ChangeEFOVReviewDir(ADD);
		else
			Replay::GetInstance()->ChangeEFOVReviewDir(SUB);
	}
}

void MultiFuncEFOV::Do()
{
	if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::PREPARE)
	{
		ScanMode::GetInstance()->EnterEFOVCapture();
	}
        else if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::CAPTURE)
	{
		ScanMode::GetInstance()->EnterEFOVPrepare();
	}
}

void MultiFuncEFOV::Undo()
{
	if (ScanMode::GetInstance()->GetEFOVStatus() == ScanMode::REVIEW)
	{
		Replay::GetInstance()->ExitEFOVReview();
	}
}

void MultiFuncEFOV::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);	
}

void MultiFuncEFOV::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);	
}

////> cw init
void MultiFuncCwInit::Update()
{/*
   if(soundstatus)
    {
        m_ptrCalc->CalcSoundStatus(0);
    }
    */
    ScanMode::GetInstance()->UpdateCw();
}
void MultiFuncCwInit::Mouse(int offsetX, int offsetY)
{
	OverturnMouseOpr(offsetX, offsetY);
	ImgPw::GetInstance()->ChangePwSV(offsetX, offsetY);
}
void MultiFuncCwInit::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCwInit::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCwInit::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncCwInit::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}
// cw
void MultiFuncCw::Update()
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsNormalSpectrumImgMode() && (format == FormatPw::P_TOTAL))
		return;

    if (ModeStatus::IsSimultMode())
        return;

    ScanMode::GetInstance()->SwitchCw();
}
//extern int soundstatus;
void MultiFuncCw::Mouse(int offsetX, int offsetY)
{
    ModeStatus s;
    FormatPw::EFormatPw format = s.GetFormatPw();

    //	if (((ModeStatus::IsSpectrumImgMode()) && (format == FormatPw::P_TOTAL)) || (ScanMode::GetInstance()->GetFpgaScanMode() == ScanMode::CW))
    if ((ModeStatus::IsSpectrumImgMode()) && (format == FormatPw::P_TOTAL))
        return;

    if(ModeStatus::IsCWImgMode())
    {
        if(offsetX||offsetY)
        {
            if(soundstatus)
            {
                m_ptrCalc->CalcSoundStatus(0);
                tempoffsound=1;
            }
        }
    }
    OverturnMouseOpr(offsetX, offsetY);
    ImgPw::GetInstance()->ChangePwSV(offsetX, offsetY);
}
void MultiFuncCw::KeyLeft()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncCw::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCw::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncCw::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}

/// cwcfm init
void MultiFuncCwCfmInit::Do()
{
	if (m_change < 2)
		m_change ++;
	else
		m_change = 0;
	
	if ((m_change == 1) || (m_change == 2))
		ImgCfm::GetInstance()->SetBoxStatus(m_change-1);
	else
		ImgCfm::GetInstance()->SetBoxStatus(false);
}
void MultiFuncCwCfmInit::Update()
{
    ScanMode::GetInstance()->UpdateCwCfmFromInit();
}
void MultiFuncCwCfmInit::Mouse(int offsetX, int offsetY)
{
    ModeStatus ms;
    FormatPw::EFormatPw formatPw = ms.GetFormatPw();

    //if (((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) && formatPw == FormatPw::P_TOTAL) || (ScanMode::GetInstance()->GetFpgaScanMode() == ScanMode::CW))
    if (((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) && formatPw == FormatPw::P_TOTAL))
        return;

    if(ModeStatus::IsCWImgMode())
    {
        if(offsetX||offsetY)
        {
            if(soundstatus)
            {
                m_ptrCalc->CalcSoundStatus(0);
                tempoffsound = 1;
            }
        }
    }

    OverturnMouseOpr(offsetX, offsetY);
    if (m_change == 0) //sv
    {
        ImgPw::GetInstance()->ChangePwSV(offsetX, offsetY);
    }
    else if (m_change == 1) //box pos
    {
        ImgCfm::GetInstance()->ChangeBoxPos(offsetX, offsetY);
    }
    else // box size
	{
		bool lrStatus = ImgProc2D::GetInstance()->GetLRStatus();
		bool udStatus = ImgProc2D::GetInstance()->GetUDStatus();

		ImgCfm::GetInstance()->ChangeBoxSize(offsetX, offsetY, lrStatus, udStatus);
		
		// Change Tis
		ChangeTis();
	}
}

void MultiFuncCwCfmInit::KeyLeft()
{
	int offsetX = -1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCwCfmInit::KeyRight()
{
	int offsetX = 1;
	int offsetY = 0;

	Mouse(offsetX, offsetY);
}
void MultiFuncCwCfmInit::KeyUp()
{
	int offsetX = 0;
	int offsetY = 1;

	Mouse(offsetX, offsetY);
}
void MultiFuncCwCfmInit::KeyDown()
{
	int offsetX = 0;
	int offsetY = -1;

	Mouse(offsetX, offsetY);
}

///> cw+cfm
void MultiFuncCwCfm::Update()
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsNormalSpectrumColorImgMode() && (format == FormatPw::P_TOTAL))
		return;

    if (ModeStatus::IsSimultMode())
        return;

	ScanMode::GetInstance()->SwitchCwCfm();
}

///> cw+pdi init
void MultiFuncCwPdiInit::Update()  
{
    /*
     if(soundstatus)
    {
        m_ptrCalc->CalcSoundStatus(0);
        updateoffsound=true;
    }*/

	ScanMode::GetInstance()->UpdateCwPdiFromInit();
}

///> cw+pdi
void MultiFuncCwPdi::Update()
{
	ModeStatus s;
	FormatPw::EFormatPw format = s.GetFormatPw();

	if (ModeStatus::IsNormalSpectrumColorImgMode() && (format == FormatPw::P_TOTAL))
		return;

    if (ModeStatus::IsSimultMode())
        return;

	ScanMode::GetInstance()->SwitchCwPdi();
}
