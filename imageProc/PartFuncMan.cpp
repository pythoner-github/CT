#include "PartFuncMan.h"
#include "Img2D.h"
#include "gui_global.h"
#include "GlobalClassMan.h"
#include "HintArea.h"
#include "KeyFunc.h"
#include "ImgProc2D.h"
#include "ImgProcM.h"
#include "imageProc/Zoom.h"
/*
 * @管理一部分要用到value键来进行图像处理的功能函数
 */

EKnobReturn GlobalZoom(EKnobOper oper)
{
    Zoom::GetInstance()->GlobalZoomScale(oper);
    return OK;
}

EKnobReturn PIPZoom(EKnobOper oper)
{
	Zoom::GetInstance()->PIPZoomScale(oper);
    return OK;
}

EKnobReturn GrayTransport(EKnobOper oper)
{   
    ImgProc2D::GetInstance()->ChangeGrayTransPointX(oper);
    return OK;
}

