#include "keyboard/MultiFuncValue.h"
#include "keyboard/MultiFuncMode.h"
#include "imageProc/ModeStatus.h"
#include "imageControl/Img2D.h"
#include "comment/MultiFuncArrow.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyFunc.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/Zoom.h"
#include "probe/MenuBiopsy.h"
#include "ViewMain.h"

///> private
///> focus
void MultiFuncFocus::Value(EKnobOper opr)
{
    Img2D* ptrImg = Img2D::GetInstance();
    if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || Zoom::GetInstance()->GetLocalZoomStatus() || ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus())
        return;

    Img2D::GetInstance()->ChangeFocSum(opr);

    // change tis
    ChangeTis();
}
void MultiFuncFocus::Mouse(int offsetX, int offsetY)
{
    OverturnMouseOpr(offsetX, offsetY);

    if (offsetY > 0)
        Img2D::GetInstance()->ChangeFocPos(SUB);
    else if (offsetY < 0)
        Img2D::GetInstance()->ChangeFocPos(ADD);

    // change tis
    ChangeTis();
}
void MultiFuncFocus::Undo()
{
    MultiFuncUndo();
}
void MultiFuncFocus::KeyUp()
{
    int offsetX = 0;
    int offsetY = 1;

    Mouse(offsetX, offsetY);
}
void MultiFuncFocus::KeyDown()
{
    int offsetX = 0;
    int offsetY = -1;

    Mouse(offsetX, offsetY);
}

//Depth
void MultiFuncDepth::Value(EKnobOper opr)
{
    ModeStatus s;
    int flag = 0;
    ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();

    if (s.IsColorMode())
        flag = 1;
    else if (s.IsSpectrumMode())
        flag = 2;
    else if (s.IsSpectrumColorMode())
        flag = 3;
    else if (s.IsMMode())
        flag = 4;

    // clear biopsy line
    g_menuBiopsy.ClearBiopsyLine();

    if ((flag == 1) || (flag == 3))
        ImgCfm::GetInstance()->ClearBox();
    if ((flag == 2) || (flag == 3))
        ImgPw::GetInstance()->ClearSv();
    else if (flag == 4)
        Img2D::GetInstance()->ClearMLine();
#if 0
  if (mode == ScanMode::D2 )
    {
        Img2D::GetInstance()->ChangeDepth(opr);
    }
    else
    {
        //prepare

        ImgCfm::GetInstance()->ChangeDepthBefore();

        //apply
        if (Img2D::GetInstance()->ChangeDepth(opr) != ERROR)
        {
            ImgCfm::GetInstance()->ChangeDepth();
            ImgPw::GetInstance()->ChangeDepth();
        }
    }
#endif

    //prepare
    ImgCfm::GetInstance()->ChangeDepthBefore();

    //apply
    if (Img2D::GetInstance()->ChangeDepth(opr) != ERROR)
    {
        ImgCfm::GetInstance()->ChangeDepth();
        ImgPw::GetInstance()->ChangeDepth();
    }

    if ((flag == 1) || (flag == 3))
        ImgCfm::GetInstance()->ReDrawBox();
    if ((flag == 2) || (flag == 3))
        ImgPw::GetInstance()->ReDrawSv();
    else if (flag == 4)
        Img2D::GetInstance()->ReDrawMLine();

    // redraw biopsy line
    g_menuBiopsy.UpdateBiopsyLine();

    // change Tis
    ChangeTis();

}
void MultiFuncDepth::Undo()
{
    MultiFuncUndo();
}

///> local zoom
#if (defined (CT_322) || defined(CT_313))
void MultiFuncLocalZoom::Do()
{
    n1++;
    if(n1 == 3)
    {
        n1 = 1;
        HintArea::GetInstance()->ClearHint();
        Zoom::GetInstance()->LocalZoomCtrl(TRUE);
    }

    m_changeSize = !m_changeSize;
}
#else
void MultiFuncLocalZoom::Do()
{
    m_changeSize = !m_changeSize;
    Zoom::GetInstance()->SetLocalZoomBoxStatus(m_changeSize);
}
#endif
void MultiFuncLocalZoom::Undo()
{
    MultiFuncUndo();
}
void MultiFuncLocalZoom::Update()
{
    HintArea::GetInstance()->ClearHint();
    Zoom::GetInstance()->LocalZoomCtrl(TRUE);
}
void MultiFuncLocalZoom::Mouse(int offsetX, int offsetY)
{
    OverturnMouseOpr(offsetX, offsetY);

    if (m_changeSize) // change size
    {
        bool lrStatus = ImgProc2D::GetInstance()->GetLRStatus();
        bool udStatus = ImgProc2D::GetInstance()->GetUDStatus();
        Zoom::GetInstance()->LocalZoomBoxSize(offsetX, offsetY, lrStatus, udStatus);
    }
    else // move pos
    {
        Zoom::GetInstance()->LocalZoomBoxPos(offsetX, offsetY);
    }
}

void MultiFuncLocalZoom::KeyLeft()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncLocalZoom::KeyRight()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncLocalZoom::KeyUp()
{
    int offsetX = 0;
    int offsetY = 1;

    Mouse(offsetX, offsetY);
}
void MultiFuncLocalZoom::KeyDown()
{
    int offsetX = 0;
    int offsetY = -1;

    Mouse(offsetX, offsetY);
}
///> global zoom
void MultiFuncGlobalZoom::Value(EKnobOper opr)
{
    Zoom::GetInstance()->GlobalZoomScale(opr);
}
void MultiFuncGlobalZoom::Mouse(int offsetX, int offsetY)
{
    OverturnMouseOpr(offsetX, offsetY);
    Zoom::GetInstance()->GlobalZoomScroll(offsetX, offsetY);
}
void MultiFuncGlobalZoom::Undo()
{
    MultiFuncUndo();
}
void MultiFuncGlobalZoom::KeyLeft()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncGlobalZoom::KeyRight()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncGlobalZoom::KeyUp()
{
    int offsetX = 0;
    int offsetY = 1;

    Mouse(offsetX, offsetY);
}
void MultiFuncGlobalZoom::KeyDown()
{
    int offsetX = 0;
    int offsetY = -1;

    Mouse(offsetX, offsetY);
}
///> PIP zoom
void MultiFuncPIPZoom::Value(EKnobOper opr)
{
    Zoom::GetInstance()->PIPZoomScale(opr);
}
void MultiFuncPIPZoom::Mouse(int offsetX, int offsetY)
{
    OverturnMouseOpr(offsetX, offsetY);
    Zoom::GetInstance()->PIPZoomPos(offsetX, offsetY);
}
void MultiFuncPIPZoom::Undo()
{
    MultiFuncUndo();
}
void MultiFuncPIPZoom::KeyLeft()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncPIPZoom::KeyRight()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncPIPZoom::KeyUp()
{
    int offsetX = 0;
    int offsetY = 1;

    Mouse(offsetX, offsetY);
}
void MultiFuncPIPZoom::KeyDown()
{
    int offsetX = 0;
    int offsetY = -1;

    Mouse(offsetX, offsetY);
}

///> measure
#if 1
void MultiFuncMeasure::Do()
{
    if (g_ptrAbsMeasure != NULL)
        g_ptrAbsMeasure->PressLeft(m_p);
}
void MultiFuncMeasure::Undo()
{
    if (g_ptrAbsMeasure != NULL)
        g_ptrAbsMeasure->PressRight(m_p);
}
void MultiFuncMeasure::Update()
{
    if (g_ptrAbsMeasure != NULL)
        g_ptrAbsMeasure->Change();
}
void MultiFuncMeasure::Value(EKnobOper opr)
{
    if (g_ptrAbsMeasure != NULL)
        g_ptrAbsMeasure->Value(opr);
}
void MultiFuncMeasure::Mouse(int offsetX, int offsetY)
{
    if (g_ptrAbsMeasure != NULL)
    {
        GetPoint(offsetX, offsetY);
        g_ptrAbsMeasure->MouseMove(m_p);
    }
}
void MultiFuncMeasure::KeyLeft()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncMeasure::KeyRight()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncMeasure::KeyUp()
{
    int offsetX = 0;
    int offsetY = 1;

    Mouse(offsetX, offsetY);
}
void MultiFuncMeasure::KeyDown()
{
    int offsetX = 0;
    int offsetY = -1;

    Mouse(offsetX, offsetY);
}
POINT MultiFuncMeasure::GetPoint(int offsetX, int offsetY)
{
    m_p = m_draw.GetCursor();

    if (((m_p.x + offsetX) >= 0) &&((m_p.x + offsetX) <= IMAGE_W))
        m_p.x += offsetX;

    offsetY = -offsetY;
    if (((m_p.y + offsetY) >= 0) &&((m_p.y + offsetY) <= IMAGE_H))
        m_p.y += offsetY;

    m_draw.SetCursor(m_p);

    return m_p;
}
#endif
#if 0
///Calc
void MultiFuncCalc::Undo()
{
    MultiFuncUndo();
}
#endif
///>Text
void MultiFuncText::Undo()
{
    NoteArea::GetInstance()->Undo();
    MultiFuncUndo();
}
///> gray trans
void MultiFuncGrayTrans::Mouse(int offsetX, int offsetY)
{
    if (offsetY > 0)
        ImgProc2D::GetInstance()->ChangeGrayTransPointY(ADD);
    else if (offsetY < 0)
        ImgProc2D::GetInstance()->ChangeGrayTransPointY(SUB);
}
void MultiFuncGrayTrans::Value(EKnobOper opr)
{
    ImgProc2D::GetInstance()->ChangeGrayTransPointX(opr);
}

void MultiFuncGrayTrans::Do()
{
#if (defined(CT_322) || defined(CT_313))
    ImgProc2D::GetInstance()->ResetGrayTrans();
#endif

}

void MultiFuncGrayTrans::Undo()
{
    MultiFuncUndo();
}
void MultiFuncGrayTrans::Update()
{
    ImgProc2D::GetInstance()->ResetGrayTrans();
}
void MultiFuncGrayTrans::KeyUp()
{
    int offsetX = 0;
    int offsetY = 1;

    Mouse(offsetX, offsetY);
}
void MultiFuncGrayTrans::KeyDown()
{
    int offsetX = 0;
    int offsetY = -1;

    Mouse(offsetX, offsetY);
}
///> gray reject
void MultiFuncGrayReject::Mouse(int offsetX, int offsetY)
{
    if (offsetX > 0)
        ImgProc2D::GetInstance()->ChangeGrayReject(ADD);
    else if (offsetX < 0)
        ImgProc2D::GetInstance()->ChangeGrayReject(SUB);
}
void MultiFuncGrayReject::Undo()
{
    MultiFuncUndo();
}
void MultiFuncGrayReject::KeyLeft()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncGrayReject::KeyRight()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}

void MultiFuncFreq::Undo()
{
    MultiFuncUndo();
}

void MultiFuncChroma::Undo()
{
    MultiFuncUndo();
}
void MultiFuncBiopsyVerify::Value(EKnobOper opr) ///< verify angle
{
    BiopsyLine::GetInstance()->AngleChange(opr);
}
void MultiFuncBiopsyVerify::Mouse(int offsetX, int offsetY) ///< verify position
{
    OverturnMouseOpr(offsetX, offsetY);
    BiopsyLine::GetInstance()->PositionChange(offsetX);
}
void MultiFuncBiopsyVerify::MultiFuncBiopsyVerify::Undo()
{
    //  MultiFuncUndo();
    //--------------2016.09.08-----------------------------//
    BiopsyLine::GetInstance()->Clear();
    BiopsyLine::GetInstance()->InitPara();
    BiopsyLine::GetInstance()->Draw();
    //----------------------------------------------------------//

}
void MultiFuncBiopsyVerify::Do()
{
      MultiFuncFactory* ptr = MultiFuncFactory::GetInstance();

    ptr->Create(MultiFuncFactory::BIOPSY);

    SetSystemCursor(90,130);//  SetSystemCursor(90,100);
    doBtnEvent(1, 1);//fake btn press
    doBtnEvent(1, 0);//fake btn unpress
  //SetMenuBiopsyCursorYRange(128,188);
    //SetMenuBiopsyCursorYRange(113,188);
    SetMenuBiopsyCursorYRange(113,250);
   //--------------------------------------------------//
}

void MultiFuncBiopsyVerify::KeyLeft()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncBiopsyVerify::KeyRight()
{
    int offsetX =-1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);
}
void MultiFuncBiopsyVerify::KeyUp()
{
    int offsetX = -1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);

}
void MultiFuncBiopsyVerify::KeyDown()
{
    int offsetX = 1;
    int offsetY = 0;

    Mouse(offsetX, offsetY);

}
/*void MultiFuncBiopsyVerify::KeyEvent(unsigned char keyValue)
{
    switch(keyValue)
    {
        case KEY_FREEZE:
            MenuShowUndo();
            ViewMain::GetInstance()->KeyEvent(keyValue);
            break;
        default:
            break;
    }
}*/

void MultiFuncBiopsy::Mouse(int offsetX, int offsetY)
{
    fakeXMotionEventMenuBiopsy(offsetX,offsetY);
}
void MultiFuncBiopsy::Undo()
{
    MultiFuncUndo();
}
void MultiFuncBiopsy::Do()
{
    doBtnEvent(1, 1);
    doBtnEvent(1, 0);
}
/*void MultiFuncBiopsy::KeyEvent(unsigned char keyValue)
{
    switch(keyValue)
    {
        case KEY_FREEZE:
        //  MenuShowUndo();
            ViewMain::GetInstance()->KeyEvent(keyValue);
            MenuShowUndo();
            break;
        default:
            break;
    }

}*/