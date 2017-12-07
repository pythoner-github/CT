// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |           Symbol            |          Spectra           |            Para            |       PixmapArea        |
// |   size:(IMAGE_W*IMAGE_H)    |   size:(IMAGE_W*IMAGE_H)   |size:(IMG_AREA_W*IMG_AREA_H)| size:(IMAGE_W*IMAGE_H)  |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |BM Line                      |Grid(M and PW)              |Focus                       |GrayReject               |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |CFMBox                       |PW BaseLine                 |Scale X and ScaleY          |GrayTrans                |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |Anatomical M                 |                            |GrayScale                   |CenterLine               |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |PW Sample Volumn             |                            |ColorScale                  |Measure things           |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |Local Zoom                   |                            |TGC                         |                         |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |PIP Box                      |                            |ScanDirection               |                         |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |Magnifier                    |                            |ImageAreaPara               |                         |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |Move Flag                    |                            |                            |                         |
// +-----------------------------+----------------------------+----------------------------+-------------------------+
// |BiopsyLine                   |                            |                            |                         |
// +-----------------------------+----------------------------+----------------------------+-------------------------+

#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>
#include "display/ImageAreaDraw.h"
#include "display/ImageArea.h"
#include "display/gui_func.h"
#include "Def.h"
#include "imageControl/Img2D.h"
#include "imageControl/ImgPw.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcPw.h"
#include "probe/BiopsyLine.h"
#include "measure/MeasureCalc.h"
#include "measure/MeasureDraw.h"
#include "measure/UpdateMeasure.h"
#include "base/CalcTime.h"
#include "imageProc/Zoom.h"
#include "sysMan/SysGeneralSetting.h"
#include "imageProc/MenuPW.h"
#include "display/ImageAreaPara.h"
#include "measure/MeasureMan.h"
#include "sysMan/SysMeasurementSetting.h"

extern MenuPW g_menuPW;
extern int g_setFunc;
extern bool g_calcPwStatus;

    ImageAreaDraw * ImageAreaDraw::m_ptrInstance = NULL;

    ImageAreaDraw * ImageAreaDraw::GetInstance()
    {
        if (m_ptrInstance == NULL)
        m_ptrInstance = new ImageAreaDraw;
        return m_ptrInstance;
    }

    ImageAreaDraw::~ImageAreaDraw()
    {
        if (m_ptrInstance != NULL)
        delete m_ptrInstance;
    }

    ImageAreaDraw::ImageAreaDraw()
    {
        m_ptrImg = ImageArea::GetInstance();
        m_curB = 0;
        m_countSD = 1;
        m_countScaleY = 0;
        m_heightScaleY = 0;

        m_focusDraw = false;

        for (int i = 0; i < 4; i++)
        m_drawSD[i] = false;

        m_drawBiopsy = false;
        m_preBiopsy.LRStatus = false;
        m_preBiopsy.UDStatus = false;
        m_preBiopsy.rotate = 0;
        m_preBiopsy.xDist = 0;
        m_preBiopsy.yDist = 0;
        //----------------------------//
        m_drawBiopsyDouble=false;
        m_preBiopsyDouble0.LRStatus = false;
        m_preBiopsyDouble0.UDStatus = false;
        m_preBiopsyDouble0.rotate = 0;
        m_preBiopsyDouble0.xDist = 0;
        m_preBiopsyDouble0.yDist = 0;

        m_preBiopsyDouble1.LRStatus = false;
        m_preBiopsyDouble1.UDStatus = false;
        m_preBiopsyDouble1.rotate = 0;
        m_preBiopsyDouble1.xDist = 0;
        m_preBiopsyDouble1.yDist = 0;

        //------------------------------//
        m_preRotate = 0;

        m_mLineDraw = false;
        m_drawMline = false;

        m_drawGridM = false;

        m_SVDraw = false;

        for (int i = 0; i < 4; i++)
        m_drawBox[i] = false;

        for (int i = 0; i < 4; i ++)
        m_PIPBox[i] = 0;
        m_PIPBoxDraw = FALSE;

        m_LZDraw = FALSE;

    //    strcpy(m_pwScaleUnit, "cm/s");
        m_pwUnit = CMPS;
        m_cfmUnit = CMPS;

        m_graySize = 0;

        m_magnifier.x = 0;
        m_magnifier.y = 0;
        m_magnifierDraw = FALSE;

        m_moveFlag.x = 0;
        m_moveFlag.y = 0;
        m_moveFlagDraw = FALSE;

        m_LRStatus = false;
        m_UDStatus = false;

        m_colRange[0] = 0;
        m_colRange[1] = 0;
        m_colRange[2] = 0;
        m_colRange[3] = 0;

        m_pwMaxVel = 75;
        m_drawPwBaseline = false;
        m_drawGridPw = false;
        m_pwMaxLevel = 8;
        m_drawSV = false;
        m_formatCfm = FormatCfm::B;

    //  m_pwTrace.clear();

        m_zoomScale = 1.0;
        m_zoomOffset = 0;

        m_drawEFOVZoomMiniature = false;
        m_EFOVZoomMiniatureBox[0].x = 0;
        m_EFOVZoomMiniatureBox[0].y = 0;
        m_EFOVZoomMiniatureBox[1].x = 0;
        m_EFOVZoomMiniatureBox[1].y = 0;

        m_drawEFOVZoom = false;
        m_EFOVZoomBox[0].x = 0;
        m_EFOVZoomBox[0].y = 0;
        m_EFOVZoomBox[1].x = 0;
        m_EFOVZoomBox[1].y = 0;

        m_drawEFOVView = false;
        m_EFOVViewBox[0].x = 0;
        m_EFOVViewBox[0].y = 0;
        m_EFOVViewBox[1].x = 0;
        m_EFOVViewBox[1].y = 0;

        m_ratio2D = (double)1;
        m_dashCfmBox = false;
        m_dashLZBox = false;
        m_format = SINGLEB;

        SysGeneralSetting sysGeneralSetting;
        m_pwUnit = SCALEUNIT(sysGeneralSetting.GetScaleUnit());

        m_autoPara = NULL;

    m_pointScaleYPw.x = IMAGE_X + IMAGE_W/2;
    m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2;
    //add by hzq
    m_widthScaleYPw = IMAGE_W;
}

void ImageAreaDraw::GetProbePara(int type, int lines, int width, int r)
{
    m_probeType = type;
    m_probeLines = lines;
    m_probeWidth =width / 100;
    m_probeR = r / 100;
}

void ImageAreaDraw::ClearTextArea(bool update)
{
    m_ptrImg->ClearArea(update);
    m_ptrImg->ClearTrace();
    m_ptrImg->ClearEFOVSpeedbar();
    m_focusDraw = false;
    m_LZDraw = false;
    m_magnifierDraw = false;
    m_PIPBoxDraw = false;
    for (int i = 0; i < 4; i++)
        m_drawBox[i] = false;
    if (!update) {
        for (int i = 0; i < 4; i++)
            m_drawSD[i] = false;
        m_drawBiopsy = false;
        m_mLineDraw = false;
        m_drawGridM = false;
        m_drawGridPw = false;
        m_drawPwBaseline = false;
        m_SVDraw = false;
    }
    //  m_pwTrace.clear();
}

void ImageAreaDraw::UpdateAngle(int angle, bool isCw, bool draw, bool update)
{
    if (m_SVDraw) {
        if (draw)
            SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_preSV.LRStatus, m_preSV.UDStatus, m_preSV.pointPw, isCw, false);
    }
    m_correctAngle = angle;
    if (draw) {
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_preSV.LRStatus, m_preSV.UDStatus, m_preSV.pointPw, isCw, true);
        m_SVDraw = TRUE;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetAngleValue(int angle)
{
    m_correctAngle = angle;
}

void ImageAreaDraw::Update(void)
{
    ModeStatus modeStatus;

    m_scanMode = modeStatus.GetScanMode();

    m_ratioYM = Img2D::GetInstance()->GetScaleMDepth();
    m_ratioXM = Img2D::GetInstance()->GetScaleMTime();
    m_ratioXPw = ImgPw::GetInstance()->GetScaleTime();
    if (m_pwUnit == KHZ) {
        m_pwMaxVel = ImgPw::GetInstance()->GetMaxVelKHz();
        m_ratioYPw = ImgPw::GetInstance()->GetScaleVelKHz();
    } else {
        m_pwMaxVel = ImgPw::GetInstance()->GetMaxVel();
        m_ratioYPw = ImgPw::GetInstance()->GetScaleVel();
    }

    m_LRStatus = ImgProc2D::GetInstance()->GetLRStatus();
    m_UDStatus = ImgProc2D::GetInstance()->GetUDStatus();
    m_rotate = ImgProc2D::GetInstance()->GetRotate();
    Zoom::GetInstance()->GetGlobalZoomInfo(m_zoomScale, m_zoomOffset);

    m_ratio2D = Img2D::GetInstance()->GetScale2D() / m_zoomScale;
    PRINTF("display update: ratio2D = %f, zoomScale = %f\n", m_ratio2D, m_zoomScale);
    Img2D::GetInstance()->GetAllScale2D(m_ratio2DAll);

    m_inLZMode = Zoom::GetInstance()->GetLocalZoomStatus();
    if (m_inLZMode)
    {
        Zoom::GetInstance()->GetScanLineRange(m_LZLineRange);
        Zoom::GetInstance()->GetScanDotRange(m_LZDotRange);
        Zoom::GetInstance()->GetLocalZoomScale(m_LZScale);
        m_LZDotBeginDepth = Zoom::GetInstance()->GetScanDotBeginDepth();

        int probeR = m_probeR + m_LZDotBeginDepth;
        double scanAngle = m_probeWidth/2 * PI / 180 * (m_LZLineRange[1] - m_LZLineRange[0]) / (m_probeLines - 1);

        //calculate ScaleY Spacing of the top image
        if (m_probeType == 'L' || m_probeType == 'l')
            m_scaleYSpacing = 0;
        else
            m_scaleYSpacing = probeR - cos(scanAngle) * probeR;
        PRINTF("scaleYSpacing = %d, scale = %f\n", m_scaleYSpacing, m_ratio2D);
    }
    else
    {
        m_LZLineRange[0] = 0;
        m_LZLineRange[1] = m_probeLines - 1;
        m_LZDotRange[0] = 0;
        m_LZDotRange[1] = IMAGE_H;
        m_LZScale = 1.0;

        //calculate ScaleY Spacing of the top image
        if (m_probeType == 'L' || m_probeType == 'l')
            m_scaleYSpacing = 0;
        else
            m_scaleYSpacing = m_probeR - (cos(m_probeWidth/2 * PI/180) * m_probeR);
    }

    switch(m_scanMode) {
        case (ScanMode::D2):
            m_format2D = modeStatus.GetFormat2D();
            m_curB = modeStatus.Get2DCurrentB();
            m_ratio2D = m_ratio2DAll[m_curB] / m_zoomScale;
            ChangeFormat2D(m_format2D, m_curB, m_ratio2D, m_LRStatus, m_UDStatus);
            break;
        case (ScanMode::M_INIT):
            ChangeFormatMInit();
            break;
        case (ScanMode::M):
            m_formatM = modeStatus.GetFormatM();
            ChangeFormatM(m_formatM);
            break;
        case (ScanMode::ANATOMIC_M):
            m_formatM = modeStatus.GetFormatM();
            m_ratioXM = Img2D::GetInstance()->GetScaleAnatomicMTime();
            ChangeFormatAnatomicM(m_formatM);
            break;
        case (ScanMode::PW_INIT):
        case (ScanMode::CW_INIT):
            ChangeFormatPwInit();
            break;
        case (ScanMode::PW):
        case (ScanMode::PW_SIMULT):
        case (ScanMode::CW):
            m_formatPw = modeStatus.GetFormatPw();
            ChangeFormatPw(m_formatPw);
          // break;
       // case (ScanMode::CW_INIT):
          //  break;
       // case (ScanMode::CW):
            break;
        case (ScanMode::CFM):
        case (ScanMode::PDI):
            m_formatCfm = modeStatus.GetFormatCfm();
            m_curB = modeStatus.Get2DCurrentB();
            m_ratio2D = m_ratio2DAll[m_curB] / m_zoomScale;
            ChangeFormatCfm(m_formatCfm, m_curB, m_ratio2D, m_LRStatus, m_UDStatus);
            break;
        case (ScanMode::PWCFM_INIT):
        case (ScanMode::PWPDI_INIT):
        case (ScanMode::CWCFM_INIT):
        case (ScanMode::CWPDI_INIT):
            ChangeFormatPwCfmInit();
            break;
        case (ScanMode::PWCFM):
        case (ScanMode::PWPDI):
        case (ScanMode::PWCFM_SIMULT):
        case (ScanMode::PWPDI_SIMULT):
        case (ScanMode::CWCFM):
        case (ScanMode::CWPDI):
            m_formatPw = modeStatus.GetFormatPw();
            ChangeFormatPwCfm(m_formatPw);
            break;
        case (ScanMode::CFM_VS_2D):
        case (ScanMode::PDI_VS_2D):
            ChangeFormatColorVs2D();
            break;
        case (ScanMode::EFOV):
            break;
    }
}

void ImageAreaDraw::ChangeFormat2D(Format2D::EFormat2D format, int curB, double ratio2D, bool LRStatus, bool UDStatus)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    m_ptrImg->UpdateSpectraArea(0, 0, 0, 0);

    switch(format) {
        case (Format2D::B):
            m_format = SINGLEB;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
            // ScanDirection
            m_countSD = 1;
            if (UDStatus && !LRStatus) {
                if (m_rotate == 90) {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + 10;
                } else if (m_rotate == 180) {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + 10;
                } else if (m_rotate == 270) {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                } else {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                }
            } else if (LRStatus && !UDStatus) {
                if (m_rotate == 90) {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                } else if (m_rotate == 180) {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                } else if (m_rotate == 270) {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + 10;
                } else {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + 10;
                }
            } else if (UDStatus && LRStatus) {
                if (m_rotate == 90) {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                } else if (m_rotate == 180) {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + 10;
                } else if (m_rotate == 270) {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + 10;
                } else {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                }
            } else {
                if (m_rotate == 90) {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + 10;
                } else if (m_rotate == 180) {
                    pointSD.x = IMAGE_X + IMAGE_W - 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                } else if (m_rotate == 270) {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + IMAGE_H - 10;
                } else {
                    pointSD.x = IMAGE_X + 30;
                    pointSD.y = IMAGE_Y + 10;
                }
            }
            m_pointSD.clear();
            m_pointSD.push_back(pointSD);

            // Scale Y
            m_countScaleY = 1;
            if (UDStatus) {
                if (m_rotate == 90) {
                    m_widthScaleY = (IMAGE_W - (IMAGE_W - IMAGE_H) - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + (IMAGE_W - IMAGE_H)/2 + m_scaleYSpacing / ratio2D;
                    pointScaleY.y = IMAGE_Y + IMAGE_H;
                } else if (m_rotate == 180) {
                    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + IMAGE_W;
                    pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
                } else if (m_rotate == 270) {
                    m_widthScaleY = (IMAGE_W - (IMAGE_W - IMAGE_H) - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + IMAGE_W - (IMAGE_W - IMAGE_H)/2 - m_scaleYSpacing / ratio2D;
                    pointScaleY.y = IMAGE_Y + IMAGE_H;
                } else {
                    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + IMAGE_W;
                    pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                }
            } else {
                if (m_rotate == 90) {
                    m_widthScaleY = (IMAGE_W - (IMAGE_W - IMAGE_H) - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + IMAGE_W - (IMAGE_W - IMAGE_H)/2 - m_scaleYSpacing / ratio2D;
                    pointScaleY.y = IMAGE_Y + IMAGE_H;
                } else if (m_rotate == 180) {
                    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + IMAGE_W;
                    pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                } else if (m_rotate == 270) {
                    m_widthScaleY = (IMAGE_W - (IMAGE_W - IMAGE_H) - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + (IMAGE_W - IMAGE_H)/2 + m_scaleYSpacing / ratio2D;
                    pointScaleY.y = IMAGE_Y + IMAGE_H;
                } else {
                    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
                    pointScaleY.x = IMAGE_X + IMAGE_W;
                    pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
                }
            }
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            if (m_rotate == 90 || m_rotate == 270) {
                clearScaleY.x = IMAGE_X;
                clearScaleY.y = IMAGE_Y + IMAGE_H;
                m_widthClearScaleY = IMAGE_W;
            } else {
                clearScaleY.x = IMAGE_X + IMAGE_W;
                clearScaleY.y = IMAGE_Y;
                m_heightClearScaleY = IMAGE_H;
            }
            m_clearScaleY.push_back(clearScaleY);

            if (UDStatus) {
                if (m_rotate == 90) {
                    m_pointFocus.x = IMAGE_X + m_scaleYSpacing / ratio2D + (IMAGE_W - IMAGE_H)/2;
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - 5;
                } else if (m_rotate == 180) {
                    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
                } else if (m_rotate == 270) {
                    m_pointFocus.x = IMAGE_X + IMAGE_W - m_scaleYSpacing / ratio2D - (IMAGE_W - IMAGE_H)/2;
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - 5;
                } else {
                    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                }
            } else {
                if (m_rotate == 90) {
                    m_pointFocus.x = IMAGE_X + IMAGE_W - m_scaleYSpacing / ratio2D - (IMAGE_W - IMAGE_H)/2;
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - 5;
                } else if (m_rotate == 180) {
                    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                } else if (m_rotate == 270) {
                    m_pointFocus.x = IMAGE_X + m_scaleYSpacing / ratio2D + (IMAGE_W - IMAGE_H)/2;
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - 5;
                } else {
                    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
                }
            }

            // Local Zoom
            m_pointLZ.x = IMAGE_W / 2;
            if (UDStatus)
                m_pointLZ.y = IMAGE_H;
            else
                m_pointLZ.y = 0;

            break;
        case (Format2D::BB):
            m_format = DUAL;
            if (curB == 0)
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            else if (curB == 1)
                m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);

            //ScanDirection
            m_countSD = 2;
            m_pointSD.clear();

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            if (curB == 1 && UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (curB == 1 && LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (curB == 1 && UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/2 + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale Y
            m_countScaleY = 2;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
            m_pointScaleY.clear();

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            if (curB == 0) {
                m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            } else if (curB == 1) {
                m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            }

            break;
        case (Format2D::B4):
            m_format = QUAD;
            if (curB == 0)
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H/2);
            else if (curB == 1)
                m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H/2);
            else if (curB == 2)
                m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y+IMAGE_H/2, IMAGE_W/2, IMAGE_H/2);
            else if (curB == 3)
                m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y+IMAGE_H/2, IMAGE_W/2, IMAGE_H/2);

            // ScanDirection
            m_countSD = 4;
            m_pointSD.clear();

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            } else if (UDStatus && LRStatus){
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 4;
            m_heightScaleY = (IMAGE_H/2 - m_scaleYSpacing / ratio2D) * ratio2D;
            m_pointScaleY.clear();

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y + IMAGE_H/2;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y + IMAGE_H/2;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/2;

            // focus
            if (curB == 0) {
                m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            } else if (curB == 1) {
                m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            } else if (curB == 2) {
                m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            } else if (curB == 3) {
                m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            }
            break;
    }
}

void ImageAreaDraw::ChangeFormatM(FormatM::EFormatM format)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    switch(format) {
        case (FormatM::M_TOTAL):
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
            m_ptrImg->UpdateSymbolArea(0, 0, 0, 0);

            m_drawMline = false;
            m_countSD = 0;

            // scale Y
            m_countScaleY = 0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            //scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointScaleYM.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_heightScaleYM = IMAGE_H;// - m_scaleYSpacing / m_ratio2D;

            // scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;
            break;
        case (FormatM::BM11_UD):
            m_format = U1D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/2);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/2, IMAGE_W, IMAGE_H/2);
            // scan direction
            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus){
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/2 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/2;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // BM line
            m_drawMline = true;
            m_pointBM.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointBM.y = IMAGE_H/2;
            else
                m_pointBM.y = 0;
            m_widthImgBM = IMAGE_W/2;
            m_heightImgBM = IMAGE_H/2;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H;
            else
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H/2;
            m_heightScaleYM = IMAGE_H/2;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;

            break;
        case (FormatM::BM21_UD):
            m_format = U2D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0*2.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0*2.0, IMAGE_W, IMAGE_H/3.0);
            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;    // --50-- To be confirmed --
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0*2.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50 - 5; // --50-- To be confirmed --
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // BM line
            m_drawMline = true;
            m_pointBM.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointBM.y = IMAGE_H/3.0*2.0;
            else
                m_pointBM.y = 0;
            m_widthImgBM = IMAGE_W/3*2;
            m_heightImgBM = IMAGE_H/3*2;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H;
            else
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H/3*2;
            m_heightScaleYM = IMAGE_H/3.0;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;

            break;
        case (FormatM::BM12_UD):
            m_format = U1D2;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0, IMAGE_W, IMAGE_H/3.0*2.0);
            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // BM line
            m_drawMline = true;
            m_pointBM.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointBM.y = IMAGE_H/3.0;
            else
                m_pointBM.y = 0;
            m_widthImgBM = IMAGE_W/3;
            m_heightImgBM = IMAGE_H/3;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H;
            else
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H/3;
            m_heightScaleYM = IMAGE_H/3.0*2.0;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;

            break;
        case (FormatM::BM11_LR):
            m_format = L1R1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            m_ptrImg->UpdateSpectraArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            m_countSD = 1;
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.clear();
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            m_pointScaleY.clear();

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // BM line
            m_drawMline = true;
            m_pointBM.x = IMAGE_W/4;
            if (m_UDStatus)
                m_pointBM.y = IMAGE_H;
            else
                m_pointBM.y = 0;
            m_widthImgBM = IMAGE_W/2;
            m_heightImgBM = IMAGE_H;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointScaleYM.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_heightScaleYM = IMAGE_H; // - m_scaleYSpacing / m_ratio2D;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X + IMAGE_W/2;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W/2;
            break;
    }
}

void ImageAreaDraw::ChangeFormatAnatomicM(FormatM::EFormatM format)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    switch(format) {
        case (FormatM::M_TOTAL):
            break;
        case (FormatM::BM11_UD):
            m_format = U1D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/2);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/2, IMAGE_W, IMAGE_H/2);
            // scan direction
            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus){
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/2 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/2;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H;
            else
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H/2;
            m_heightScaleYM = IMAGE_H/2;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;

            break;
        case (FormatM::BM21_UD):
            m_format = U2D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0*2.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0*2.0, IMAGE_W, IMAGE_H/3.0);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;    // --50-- To be confirmed --
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0*2.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50 - 5; // --50-- To be confirmed --
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H;
            else
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H/3*2;
            m_heightScaleYM = IMAGE_H/3.0;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;

            break;
        case (FormatM::BM12_UD):
            m_format = U1D2;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0, IMAGE_W, IMAGE_H/3.0*2.0);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H;
            else
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H/3;
            m_heightScaleYM = IMAGE_H/3.0*2.0;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W;

            break;
        case (FormatM::BM11_LR):
            m_format = L1R1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            m_ptrImg->UpdateSpectraArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);

            m_countSD = 1;
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.clear();
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            m_pointScaleY.clear();

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // Scale Y M
            m_pointScaleYM.x = IMAGE_X + IMAGE_W;
            if (m_UDStatus)
                m_pointScaleYM.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointScaleYM.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_heightScaleYM = IMAGE_H; // - m_scaleYSpacing / m_ratio2D;

            // Scale X M
            m_pointScaleXM.x = IMAGE_X + IMAGE_W/2;
            m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
            m_widthScaleXM = IMAGE_W/2;
            break;
    }
}

void ImageAreaDraw::ChangeFormatMInit(void)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    m_format = SINGLEB;
    m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
    m_ptrImg->UpdateSpectraArea(0, 0, 0, 0);

    // scan direction
    m_countSD = 1;
    if (m_UDStatus && !m_LRStatus) {
        pointSD.x = IMAGE_X + 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else if (m_LRStatus && !m_UDStatus) {
        pointSD.x = IMAGE_X + IMAGE_W - 30;
        pointSD.y = IMAGE_Y + 10;
    } else if (m_UDStatus && m_LRStatus) {
        pointSD.x = IMAGE_X + IMAGE_W - 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else {
        pointSD.x = IMAGE_X + 30;
        pointSD.y = IMAGE_Y + 10;
    }
    m_pointSD.clear();
    m_pointSD.push_back(pointSD);

    // scale y
    m_countScaleY = 1;
    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
    pointScaleY.x = IMAGE_X + IMAGE_W;
    if (m_UDStatus)
        pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    else
        pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
    m_pointScaleY.clear();
    m_pointScaleY.push_back(pointScaleY);

    m_clearScaleY.clear();
    clearScaleY.x = IMAGE_X + IMAGE_W;
    clearScaleY.y = IMAGE_Y;
    m_clearScaleY.push_back(clearScaleY);
    m_heightClearScaleY = IMAGE_H;

    // focus
    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
    if (m_UDStatus)
        m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    else
        m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

    // BM line
    m_drawMline = true;
    m_pointBM.x = IMAGE_W/2;
    if (m_UDStatus)
        m_pointBM.y = IMAGE_H;
    else
        m_pointBM.y = 0;
    m_widthImgBM = IMAGE_W;
    m_heightImgBM = IMAGE_H;
}

void ImageAreaDraw::ChangeFormatPw(FormatPw::EFormatPw format)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    switch(format) {
        case (FormatPw::P_TOTAL):
            m_ptrImg->UpdateSymbolArea(0, 0, 0, 0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
            m_countSD = 0;

            // scale y
            m_countScaleY = 0;

            // sample volumn
            m_drawSV = false;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            break;
        case (FormatPw::BP11_UD):
            m_format = U1D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/2);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/2, IMAGE_W, IMAGE_H/2);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus){
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/2 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/2;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H/2;
            else
                m_pointPw.y = 0;
            m_heightImgPw = IMAGE_H/2;
            m_widthImgPw = IMAGE_W/2;
            m_zoomRatioPw = 1.0/2.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2 + IMAGE_H/4;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H/2;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            break;
        case (FormatPw::BP21_UD):
            m_format = U2D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0*2.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0*2.0, IMAGE_W, IMAGE_H/3.0);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0*2.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50- 5; // -- To be confirmed --
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H/3.0*2.0;
            else
                m_pointPw.y = 0;
            m_heightImgPw = IMAGE_H / 3.0 * 2.0;
            m_widthImgPw = IMAGE_W / 3.0 *2.0;
            m_zoomRatioPw = 2.0/3.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H / 6.0 * 5.0;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H/3.0;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            break;
        case (FormatPw::BP12_UD):
            m_format = U1D2;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0, IMAGE_W, IMAGE_H/3.0*2.0);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H/3.0;
            else
                m_pointPw.y = 0;
            m_heightImgPw = IMAGE_H / 3.0;
            m_widthImgPw = IMAGE_W / 3.0;
            m_zoomRatioPw = 1.0 / 3.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H / 3.0 * 2.0;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H / 3.0 * 2.0;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            break;
        case (FormatPw::BP11_LR):
            m_format = L1R1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            m_ptrImg->UpdateSpectraArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);

            m_countSD = 1;
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.clear();
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D)* m_ratio2D;
            m_pointScaleY.clear();
            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/4;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H;
            else
                m_pointPw.y = 0;
            m_heightImgPw = IMAGE_H;
            m_widthImgPw = IMAGE_W / 2;
            m_zoomRatioPw = 1.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X + IMAGE_W/2;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2;
            m_widthScaleYPw = IMAGE_W/2;
            m_heightScaleYPw = IMAGE_H;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X + IMAGE_W/2;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W/2 * m_ratioXPw;

            break;
    }
}

void ImageAreaDraw::ChangeFormatPwInit(void)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    m_format = SINGLEB;
    m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
    m_ptrImg->UpdateSpectraArea(0, 0, 0, 0);
    // ScanDirection
    m_countSD = 1;
    if (m_UDStatus && !m_LRStatus) {
        pointSD.x = IMAGE_X + 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else if (m_LRStatus && !m_UDStatus) {
        pointSD.x = IMAGE_X + IMAGE_W - 30;
        pointSD.y = IMAGE_Y + 10;
    } else if (m_UDStatus && m_LRStatus) {
        pointSD.x = IMAGE_X + IMAGE_W - 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else {
        pointSD.x = IMAGE_X + 30;
        pointSD.y = IMAGE_Y + 10;
    }
    m_pointSD.clear();
    m_pointSD.push_back(pointSD);

    // Scale Y
    m_countScaleY = 1;
    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
    pointScaleY.x = IMAGE_X + IMAGE_W;
    if (m_UDStatus)
        pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    else
        pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
    m_pointScaleY.clear();
    m_pointScaleY.push_back(pointScaleY);

    m_clearScaleY.clear();
    clearScaleY.x = IMAGE_X + IMAGE_W;
    clearScaleY.y = IMAGE_Y;
    m_clearScaleY.push_back(clearScaleY);
    m_heightClearScaleY = IMAGE_H;

    // focus
    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
    if (m_UDStatus)
        m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    else
        m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

    // sample volumn
    m_drawSV = true;
    m_pointPw.x = IMAGE_W/2;
    if (m_UDStatus)
        m_pointPw.y = IMAGE_H;
    else
        m_pointPw.y = 0;
    m_widthImgPw = IMAGE_W;
    m_heightImgPw = IMAGE_H;
    m_zoomRatioPw = 1.0;
}

void ImageAreaDraw::ChangeFormatCfm(FormatCfm::EFormatCfm format, int curB, double ratio2D, bool LRStatus, bool UDStatus)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    m_ptrImg->UpdateSpectraArea(0, 0, 0, 0);

    switch(format) {
        case (FormatCfm::B):
            m_format = SINGLEB;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
            // ScanDirection
            m_countSD = 1;
            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            }
            else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10;
            }
            else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 50;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.clear();
            m_pointSD.push_back(pointSD);

            // Scale Y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
            if (UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;

            // cfm Box
            m_pointCfm.x = IMAGE_W / 2;
            if (UDStatus)
                m_pointCfm.y = IMAGE_H;
            else
                m_pointCfm.y = 0;
            m_zoomRatioCfm = 1.0;
            //  m_widthImgCfm = IMAGE_W;
            //  m_heightImgCfm = IMAGE_H;

            // Local Zoom
            m_pointLZ.x = IMAGE_W / 2;
            if (UDStatus)
                m_pointLZ.y = IMAGE_H;
            else
                m_pointLZ.y = 0;

            break;
        case (FormatCfm::BB):
            m_format = DUAL;
            if (curB == 0)
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            else if (curB == 1)
                m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);

            m_countSD = 2;
            m_pointSD.clear();

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale Y
            m_countScaleY = 2;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / ratio2D) * ratio2D;
            m_pointScaleY.clear();

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            if (curB == 0) {
                m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            } else if (curB == 1) {
                m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            }

            // Cfm Box
            m_zoomRatioCfm = 1.0;
            if (curB == 0) {
                m_pointCfm.x = IMAGE_W/4;
            } else if (curB == 1) {
                m_pointCfm.x = IMAGE_W - IMAGE_W/4;
            }
            if (UDStatus)
                m_pointCfm.y = IMAGE_H;
            else
                m_pointCfm.y = 0;
            //  m_widthImgCfm = IMAGE_W/2;
            //  m_heightImgCfm = IMAGE_H;

            break;
        case (FormatCfm::B4):
            m_format = QUAD;
            if (curB == 0)
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H/2);
            else if (curB == 1)
                m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H/2);
            else if (curB == 2)
                m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y+IMAGE_H/2, IMAGE_W/2, IMAGE_H/2);
            else if (curB == 3)
                m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y+IMAGE_H/2, IMAGE_W/2, IMAGE_H/2);

            // ScanDirection
            m_countSD = 4;
            m_pointSD.clear();

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            } else if (UDStatus && LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            }
            m_pointSD.push_back(pointSD);

            if (UDStatus && !LRStatus) {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (LRStatus && !UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            } else if (UDStatus && LRStatus){
                pointSD.x = IMAGE_X + IMAGE_W - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
                pointSD.y = IMAGE_Y + 10 + IMAGE_H/2;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 4;
            m_heightScaleY = (IMAGE_H/2 - m_scaleYSpacing / ratio2D) * ratio2D;
            m_pointScaleY.clear();

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            pointScaleY.x = IMAGE_X + IMAGE_W;
            if (UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
            else
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y + IMAGE_H/2;
            m_clearScaleY.push_back(clearScaleY);
            clearScaleY.x = IMAGE_X + IMAGE_W;
            clearScaleY.y = IMAGE_Y + IMAGE_H/2;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/2;

            // focus
            if (curB == 0) {
                m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            } else if (curB == 1) {
                m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / ratio2D;
            } else if (curB == 2) {
                m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            } else if (curB == 3) {
                m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
                if (UDStatus)
                    m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / ratio2D;
                else
                    m_pointFocus.y = IMAGE_Y + IMAGE_H/2 + m_scaleYSpacing / ratio2D;
            }

            // Cfm Box
            m_zoomRatioCfm = 1.0 / 2.0;
            if (curB == 0) {
                m_pointCfm.x = IMAGE_W/4;
                if (UDStatus)
                    m_pointCfm.y = IMAGE_H/2;
                else
                    m_pointCfm.y = 0;
            } else if (curB == 1) {
                m_pointCfm.x = IMAGE_W - IMAGE_W/4;
                if (UDStatus)
                    m_pointCfm.y = IMAGE_H/2;
                else
                    m_pointCfm.y = 0;
            } else if (curB == 2) {
                m_pointCfm.x = IMAGE_W/4;
                if (UDStatus)
                    m_pointCfm.y = IMAGE_H;
                else
                    m_pointCfm.y = IMAGE_H/2;
            } else if (curB == 3) {
                m_pointCfm.x = IMAGE_W - IMAGE_W/4;
                if (UDStatus)
                    m_pointCfm.y = IMAGE_H;
                else
                    m_pointCfm.y = IMAGE_H/2;
            }
            //  m_widthImgCfm = IMAGE_W/2;
            //  m_heightImgCfm = IMAGE_H/2;

            break;
    }
}

void ImageAreaDraw::ChangeFormatPwCfm(FormatPw::EFormatPw format)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    GdkPoint clearScaleY;

    switch(format) {
        case (FormatPw::P_TOTAL):
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
            m_ptrImg->UpdateSymbolArea(0, 0, 0, 0);
            m_countSD = 0;

            // scale y
            m_countScaleY = 0;

            // sample volumn
            m_drawSV = false;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            break;
        case (FormatPw::BP11_UD):
            m_format = U1D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/2);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/2, IMAGE_W, IMAGE_H/2);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus){
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H/2 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/2 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/2;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/2 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H/2;
            else
                m_pointPw.y = 0;
            m_widthImgPw = IMAGE_W/2;
            m_heightImgPw = IMAGE_H/2;
            m_zoomRatioPw = 1.0/2.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2 + IMAGE_H/4;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H/2;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            // cfm Box
            m_pointCfm.x = IMAGE_W / 2;
            if (m_UDStatus)
                m_pointCfm.y = IMAGE_H / 2;
            else
                m_pointCfm.y = 0;
            m_zoomRatioCfm = 1.0/2.0;

            break;
        case (FormatPw::BP21_UD):
            m_format = U2D1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0*2.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0*2.0, IMAGE_W, IMAGE_H/3.0);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0*2.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0*2.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4 + 50- 5; // -- To be confirmed --
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0*2.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H/3.0*2.0;
            else
                m_pointPw.y = 0;
            m_widthImgPw = IMAGE_W / 3.0 * 2.0;
            m_heightImgPw = IMAGE_H / 3.0 * 2.0;
            m_zoomRatioPw = 2.0/3.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H / 6.0 * 5.0;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H/3.0;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            // cfm Box
            m_pointCfm.x = IMAGE_W / 2;
            if (m_UDStatus)
                m_pointCfm.y = IMAGE_H / 3.0 * 2.0;
            else
                m_pointCfm.y = 0;
            m_zoomRatioCfm = 2.0/3.0;

            break;
        case (FormatPw::BP12_UD):
            m_format = U1D2;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H/3.0);
            m_ptrImg->UpdateSpectraArea(IMAGE_X, IMAGE_Y+IMAGE_H/3.0, IMAGE_W, IMAGE_H/3.0*2.0);

            m_countSD = 1;
            m_pointSD.clear();
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/4*3 - 60;
                pointSD.y = IMAGE_Y + IMAGE_H/3.0 - 10;
            } else {
                pointSD.x = IMAGE_X + IMAGE_W/4 + 60;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
            pointScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.clear();
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H/3.0;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W - IMAGE_W/4;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H/3.0 - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/2;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H/3.0;
            else
                m_pointPw.y = 0;
            m_heightImgPw = IMAGE_H / 3.0;
            m_widthImgPw = IMAGE_W / 3.0;
            m_zoomRatioPw = 1.0 / 3.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H / 3.0 * 2.0;
            m_widthScaleYPw = IMAGE_W;
            m_heightScaleYPw = IMAGE_H / 3.0 * 2.0;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W * m_ratioXPw;

            // cfm Box
            m_pointCfm.x = IMAGE_W / 2;
            if (m_UDStatus)
                m_pointCfm.y = IMAGE_H / 3.0;
            else
                m_pointCfm.y = 0;
            m_zoomRatioCfm = 1.0/3.0;

            break;
        case (FormatPw::BP11_LR):
            m_format = L1R1;
            m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W/2, IMAGE_H);
            m_ptrImg->UpdateSpectraArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);

            m_countSD = 1;
            if (m_UDStatus && !m_LRStatus) {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else if (m_LRStatus && !m_UDStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + 10;
            } else if (m_UDStatus && m_LRStatus) {
                pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
                pointSD.y = IMAGE_Y + IMAGE_H - 10;
            } else {
                pointSD.x = IMAGE_X + 30;
                pointSD.y = IMAGE_Y + 10;
            }
            m_pointSD.clear();
            m_pointSD.push_back(pointSD);

            // scale y
            m_countScaleY = 1;
            m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D)* m_ratio2D;
            m_pointScaleY.clear();
            pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            if (m_UDStatus)
                pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
            m_pointScaleY.push_back(pointScaleY);

            m_clearScaleY.clear();
            clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
            clearScaleY.y = IMAGE_Y;
            m_clearScaleY.push_back(clearScaleY);
            m_heightClearScaleY = IMAGE_H;

            // focus
            m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
            if (m_UDStatus)
                m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
            else
                m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;

            // sample volumn
            m_drawSV = true;
            m_pointPw.x = IMAGE_W/4;
            if (m_UDStatus)
                m_pointPw.y = IMAGE_H;
            else
                m_pointPw.y = 0;
            m_heightImgPw = IMAGE_H;
            m_widthImgPw = IMAGE_W / 2;
            m_zoomRatioPw = 1.0;

            // scale Y pw
            m_pointScaleYPw.x = IMAGE_X + IMAGE_W/2;
            m_pointScaleYPw.y = IMAGE_Y + IMAGE_H/2;
            m_widthScaleYPw = IMAGE_W/2;
            m_heightScaleYPw = IMAGE_H;

            // scale X pw
            m_pointScaleXPw.x = IMAGE_X + IMAGE_W/2;
            m_pointScaleXPw.y = IMAGE_Y + IMAGE_H + 2;
            m_widthScaleXPw = IMAGE_W/2 * m_ratioXPw;

            // cfm Box
            m_pointCfm.x = IMAGE_W / 4;
            if (m_UDStatus)
                m_pointCfm.y = IMAGE_H;
            else
                m_pointCfm.y = 0;
            m_zoomRatioCfm = 1.0;

            break;
    }
}

void ImageAreaDraw::ChangeFormatPwCfmInit(void)
{
    ChangeFormatPwInit();
    // cfm Box
    m_pointCfm.x = IMAGE_W / 2;
    if (m_UDStatus)
        m_pointCfm.y = IMAGE_H;
    else
        m_pointCfm.y = 0;
    m_zoomRatioCfm = 1.0;
}

void ImageAreaDraw::ChangeFormatColorVs2D(void)
{
    GdkPoint pointSD;
    GdkPoint pointScaleY;
    m_format = DUAL;
    // GdkPoint clearScaleY;
    //  m_ptrImg->UpdateSymbolArea(IMAGE_X, IMAGE_Y, IMAGE_W, IMAGE_H);
    m_ptrImg->UpdateSymbolArea(IMAGE_X+IMAGE_W/2, IMAGE_Y, IMAGE_W/2, IMAGE_H);
    m_ptrImg->UpdateSpectraArea(0, 0, 0, 0);

    m_countSD = 2;
    m_pointSD.clear();

    if (m_UDStatus && !m_LRStatus) {
        pointSD.x = IMAGE_X + 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else if (m_LRStatus && !m_UDStatus) {
        pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
        pointSD.y = IMAGE_Y + 10;
    } else if (m_UDStatus && m_LRStatus) {
        pointSD.x = IMAGE_X + IMAGE_W/2 - 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else {
        pointSD.x = IMAGE_X + 30;
        pointSD.y = IMAGE_Y + 10;
    }
    m_pointSD.push_back(pointSD);

    if (m_UDStatus && !m_LRStatus) {
        pointSD.x = IMAGE_X + IMAGE_W/2 + 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else if (m_LRStatus && !m_UDStatus) {
        pointSD.x = IMAGE_X + IMAGE_W - 30;
        pointSD.y = IMAGE_Y + 10;
    } else if (m_UDStatus && m_LRStatus) {
        pointSD.x = IMAGE_X + IMAGE_W - 30;
        pointSD.y = IMAGE_Y + IMAGE_H - 10;
    } else {
        pointSD.x = IMAGE_X + 30 + IMAGE_W/2;
        pointSD.y = IMAGE_Y + 10;
    }
    m_pointSD.push_back(pointSD);

    // scale Y
    m_countScaleY = 1;
    m_heightScaleY = (IMAGE_H - m_scaleYSpacing / m_ratio2D) * m_ratio2D;
    m_pointScaleY.clear();

    // pointScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
    // if (m_UDStatus)
    //  pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    // else
    //  pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
    // m_pointScaleY.push_back(pointScaleY);

    pointScaleY.x = IMAGE_X + IMAGE_W;
    if (m_UDStatus)
        pointScaleY.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    else
        pointScaleY.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
    m_pointScaleY.push_back(pointScaleY);

    // m_clearScaleY.clear();
    // clearScaleY.x = IMAGE_X + IMAGE_W/2 - SCALE_W;
    // clearScaleY.y = IMAGE_Y;
    // m_clearScaleY.push_back(clearScaleY);
    // clearScaleY.x = IMAGE_X + IMAGE_W;
    // clearScaleY.y = IMAGE_Y;
    // m_clearScaleY.push_back(clearScaleY);
    // m_heightClearScaleY = IMAGE_H;

    // focus
    // if (m_curB == 0) {
    //  m_pointFocus.x = IMAGE_X + IMAGE_W/2 - SCALE_W - 5;
    //  if (m_UDStatus)
    //      m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    //  else
    //      m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
    // } else if (m_curB == 1) {
    m_pointFocus.x = IMAGE_X + IMAGE_W - 5;
    if (m_UDStatus)
        m_pointFocus.y = IMAGE_Y + IMAGE_H - m_scaleYSpacing / m_ratio2D;
    else
        m_pointFocus.y = IMAGE_Y + m_scaleYSpacing / m_ratio2D;
    // }

    // Cfm Box
    m_zoomRatioCfm = 1.0;
    m_pointCfm.x = IMAGE_W - IMAGE_W/4;
    if (m_UDStatus)
        m_pointCfm.y = IMAGE_H;
    else
        m_pointCfm.y = 0;
}

void ImageAreaDraw::GetBoundary(int &up, int &down, int &left, int &right)
{
    ModeStatus modeStatus;
    ScanMode::EScanMode scanMode;
    FormatM::EFormatM formatM = FormatM::BM11_UD;
    FormatPw::EFormatPw formatPw = FormatPw::BP11_UD;

    scanMode = modeStatus.GetScanMode();
    switch (scanMode) {
        case (ScanMode::M):
        case (ScanMode::ANATOMIC_M):
            formatM = modeStatus.GetFormatM();
            break;
        case (ScanMode::PW):
        case (ScanMode::PW_SIMULT):
        case (ScanMode::PWCFM):
        case (ScanMode::PWCFM_SIMULT):
        case (ScanMode::PWPDI):
        case (ScanMode::PWPDI_SIMULT):
        case (ScanMode::CW):
        case (ScanMode::CWCFM):
        case (ScanMode::CWPDI):
            formatPw = modeStatus.GetFormatPw();
            break;
        default:
            break;
    }

    if (((scanMode == ScanMode::M) && (formatM == FormatM::BM11_UD))
            || ((scanMode == ScanMode::ANATOMIC_M) && (formatM == FormatM::BM11_UD))
            || ((scanMode == ScanMode::PW) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::PW_SIMULT) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::PWCFM) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::PWCFM_SIMULT) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::PWPDI) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::PWPDI_SIMULT) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::CW) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::CWCFM) && (formatPw == FormatPw::BP11_UD))
            || ((scanMode == ScanMode::CWPDI) && (formatPw == FormatPw::BP11_UD))){
        up = IMAGE_H/2;
        down = IMAGE_H-1;
        left = 0;
        right = IMAGE_W-1;
    }
    else if (((scanMode == ScanMode::M) && (formatM == FormatM::BM21_UD))
            || ((scanMode == ScanMode::ANATOMIC_M) && (formatM == FormatM::BM21_UD))
            || ((scanMode == ScanMode::PW) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::PW_SIMULT) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::PWCFM_SIMULT) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::PWCFM) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::PWPDI_SIMULT) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::PWPDI) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::CW) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::CWCFM) && (formatPw == FormatPw::BP21_UD))
            || ((scanMode == ScanMode::CWPDI) && (formatPw == FormatPw::BP21_UD)) ) {
        up = (IMAGE_H / 3.0) * 2.0;
        down = IMAGE_H-1;
        left = 0;
        right = IMAGE_W-1;
    }
    else if (((scanMode == ScanMode::M) && (formatM == FormatM::BM12_UD))
            || ((scanMode == ScanMode::ANATOMIC_M) && (formatM == FormatM::BM12_UD))
            || ((scanMode == ScanMode::PW) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::PW_SIMULT) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::PWCFM_SIMULT) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::PWCFM) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::PWPDI_SIMULT) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::PWPDI) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::CW) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::CWCFM) && (formatPw == FormatPw::BP12_UD))
            || ((scanMode == ScanMode::CWPDI) && (formatPw == FormatPw::BP12_UD)) ) {
        up = IMAGE_H/3.0;
        down = IMAGE_H-1;
        left = 0;
        right = IMAGE_W-1;
    }
    else if (((scanMode == ScanMode::M) && (formatM == FormatM::BM11_LR))
            || ((scanMode == ScanMode::ANATOMIC_M) && (formatM == FormatM::BM11_LR))
            || ((scanMode == ScanMode::PW) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::PW_SIMULT) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::PWCFM_SIMULT) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::PWCFM) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::PWPDI_SIMULT) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::PWPDI) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::CW) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::CWCFM) && (formatPw == FormatPw::BP11_LR))
            || ((scanMode == ScanMode::CWPDI) && (formatPw == FormatPw::BP11_LR)) ) {
        up = 0;
        down = IMAGE_H-1;
        left = IMAGE_W/2;
        right = IMAGE_W-1;
    }
    else if (((scanMode == ScanMode::M) && (formatM == FormatM::M_TOTAL))
            || ((scanMode == ScanMode::ANATOMIC_M) && (formatM == FormatM::M_TOTAL))
            || ((scanMode == ScanMode::PW) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::PW_SIMULT) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::PWCFM_SIMULT) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::PWCFM) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::PWPDI_SIMULT) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::PWPDI) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::CW) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::CWCFM) && (formatPw == FormatPw::P_TOTAL))
            || ((scanMode == ScanMode::CWPDI) && (formatPw == FormatPw::P_TOTAL))) {
        up = 0;
        down = IMAGE_H-1;
        left = 0;
        right = IMAGE_W-1;
    }
}

int ImageAreaDraw::GetPointPos(int x, int y)
{
    ModeStatus modeStatus;
    ScanMode::EScanMode scan_mode = modeStatus.GetScanMode();
    Format2D::EFormat2D format_2d = Format2D::B;
    FormatCfm::EFormatCfm format_cfm = FormatCfm::B;

    if (scan_mode == ScanMode::D2)
        format_2d = modeStatus.GetFormat2D();
    else if (scan_mode == ScanMode::CFM || scan_mode == ScanMode::PDI)
        format_cfm = modeStatus.GetFormatCfm();

    if (format_2d == Format2D::BB || format_cfm == FormatCfm::BB) {
        if (x >= 0 && x <= IMAGE_W / 2)
            return 0;
        else if (x > IMAGE_W / 2 && x <= IMAGE_W)
            return 1;
        else
            return -1;
    } else if (format_2d == Format2D::B4 || format_cfm == FormatCfm::B4) {
        if (x >= 0 && x <= IMAGE_W/2 && y >= 0 && y <= IMAGE_H/2)
            return 0;
        else if (x > IMAGE_W/2 && x <= IMAGE_W && y >= 0 && y <= IMAGE_H/2)
            return 1;
        else if (x >= 0 && x <= IMAGE_W/2 && y > IMAGE_H/2 && y <= IMAGE_H)
            return 2;
        else if (x > IMAGE_W/2 && x <= IMAGE_W && y > IMAGE_H/2 && y <= IMAGE_H)
            return 3;
        else
            return -1;
    } else {
        return -1;
    }

    return 0;
}

void ImageAreaDraw::GetAnatomicBoundary(int &up, int &down, int &left, int &right)
{
    ModeStatus modeStatus;
    ScanMode::EScanMode scanMode;
    FormatM::EFormatM formatM;

    scanMode = modeStatus.GetScanMode();
    if (scanMode == ScanMode::ANATOMIC_M) {
        formatM = modeStatus.GetFormatM();
    } else {
        up = 0;
        down = IMAGE_H;
        left = 0;
        right = IMAGE_W;
        return;
    }

    if (formatM == FormatM::BM11_UD) {
        up = 0;
        down = IMAGE_H/2;
        left = IMAGE_W/4;
        right = IMAGE_W/4*3;
    } else if (formatM == FormatM::BM21_UD) {
        up = 0;
        down = (IMAGE_H / 3.0 * 2.0);
        left = IMAGE_W/4;
        right = IMAGE_W/4*3;
    } else if (formatM == FormatM::BM12_UD) {
        up = 0;
        down = (IMAGE_H / 3.0);
        left = IMAGE_W/4;
        right = IMAGE_W/4*3;
    } else if (formatM == FormatM::BM11_LR) {
        up = 0;
        down = IMAGE_H;
        left = 0;
        right = IMAGE_W/2;
    } else if (formatM == FormatM::M_TOTAL) {
        up = 0;
        down = IMAGE_H;
        left = 0;
        right = IMAGE_W;
    }
}

// ========================= [2D] =========================
void ImageAreaDraw::CalcScanDirection(int &xPos, int &yPos)
{
    int width = 0, height = 0;
    switch (m_format)
    {
        case SINGLEB:
            width = IMAGE_W;
            height = IMAGE_H;
            break;
        case DUAL:
        case L1R1:
            width = IMAGE_W/2;
            height = IMAGE_H;
            break;
        case QUAD:
        case U1D1:
            width = IMAGE_W/2;
            height = IMAGE_H/2;
            break;
        case U2D1:
            width = IMAGE_W*2/3;
            height = IMAGE_H*2/3;
            break;
        case U1D2:
            width = IMAGE_W/3;
            height = IMAGE_H/3;
            break;
        default:
            width = IMAGE_W;
            height = IMAGE_H;
            break;
    }

    int x = 0, y = 0;
    int offsetX = 15;

        if(m_ratio2D == 0)
                 m_ratio2D = 1;
    assert(m_ratio2D > (double)0);

    if (m_probeType == 'L' || m_probeType == 'l')
        x = (width - (double)m_probeWidth / m_ratio2D) / 2 - offsetX;
    else
        x = width/2 - sin((double)m_probeWidth/2*PI/180.0f) * (m_probeR / m_ratio2D + m_LZDotRange[0] * m_LZScale) - offsetX;

    if (x < 0) x = 0;
    y = 0;

    if (m_UDStatus)
        y = height - 1 - y;
    if (m_LRStatus)
        x = width - 1 - x;

    int x0 = 0, y0 = 0;
    if (m_rotate == 90)
    {
        if (m_format == DUAL || m_format == L1R1)
            x0 = width - 1 - y;
        else
            x0 = (width + height) / 2 - y - 1;
        y0 = x - (width - height) / 2;
    }
    else if (m_rotate == 180)
    {
        x0 = width - 1 - x;
        y0 = height - 1 - y;
    }
    else if (m_rotate == 270)
    {
        if (m_format == DUAL || m_format == L1R1)
            x0 = y;
        else
            x0 = (width - height) / 2 + y;
        y0 = (width + height) / 2 - x - 1;
    }
    else
    {
        x0 = x;
        y0 = y;
    }

    int symbolWidthHalf = 15;
    int symbolHeightHalf = 10;
    if (x0 < symbolWidthHalf)
        x0 = symbolWidthHalf;
    else if (x0 > width - symbolWidthHalf)
        x0 = width - symbolWidthHalf;
    if (y0 < symbolHeightHalf)
        y0 = symbolHeightHalf;
    else if (y0 > height - symbolHeightHalf)
        y0 = height - symbolHeightHalf;

    int startX = 0, startY = 0;
    switch (m_format)
    {
        case SINGLEB:
        case L1R1:
            startX = 0;
            startY = 0;
            break;
        case U1D2:
            startX = (IMAGE_W- (double)IMAGE_W/3) / 2;
            startY = 0;
            break;
        case U2D1:
            startX = (IMAGE_W- (double)IMAGE_W*2/3) / 2;
            startY = 0;
            break;
        case U1D1:
            startX = (IMAGE_W- (double)IMAGE_W/2) / 2;
            startY = 0;
            break;
        case DUAL:
            {
                if (m_curB == 0)
                {
                    startX = 0;
                    startY = 0;
                }
                else
                {
                    startX = IMAGE_W/2;
                    startY = 0;
                }
            }
            break;
        case QUAD:
            {
                if (m_curB == 0)
                {
                    startX = 0;
                    startY = 0;
                }
                else if (m_curB == 1)
                {
                    startX = IMAGE_W/2;
                    startY = 0;
                }
                else if (m_curB == 2)
                {
                    startX = 0;
                    startY = IMAGE_H/2;
                }
                else if(m_curB == 3)
                {
                    startX = IMAGE_W/2;
                    startY = IMAGE_H/2;
                }
            }
            break;
    }

    xPos = IMAGE_X + x0 + startX;
    yPos = IMAGE_Y + y0 + startY;
}

void ImageAreaDraw::DrawScanDirection(bool update)
{
    if (m_countSD == 0)
        return ;

    //bigE:w=10 h=19;  smallEMP:w=24 h=17
    const int width = 30;
    const int height = 20;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = false;

    if (m_drawSD[m_curB])
        m_ptrImg->DrawRectangle(attr, g_black, m_prePointSD[m_curB].x-width/2, m_prePointSD[m_curB].y-height/2, width, height, true, false);

    int xPos = 0, yPos = 0;
    CalcScanDirection(xPos, yPos);
    ScanDirection(xPos, yPos, true);
    m_prePointSD[m_curB].x = xPos;
    m_prePointSD[m_curB].y = yPos;
    // 用ScanDirection记录翻转，以供SetScaleYFreeze使用
    m_prePointSD[m_curB].LRStatus = m_LRStatus;
    m_prePointSD[m_curB].UDStatus = m_UDStatus;
    m_drawSD[m_curB] = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetSDFreeze(bool update)
{
    // int preB;
    // if (m_formatCfm == FormatCfm::BB || m_format2D == Format2D::BB) {
    //     if (m_curB == 0) preB = 1;
    //     else if (m_curB == 1) preB = 0;
    // } else if (m_formatCfm == FormatCfm::B4 || m_format2D == Format2D::B4) {
    //     if (m_curB == 0) preB = 3;
    //     else if (m_curB == 1) preB = 0;
    //     else if (m_curB == 2) preB = 1;
    //     else if (m_curB == 3) preB = 2;
    // }
    for (int i = 0; i < 4; ++i) {
        if (m_drawSD[i]){
            ScanDirection(m_prePointSD[i].x, m_prePointSD[i].y, false);
        }
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ScanDirection(int x, int y, bool CurB)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;
    GdkColor *color;
    m_ptrImg->DrawRectangle(attr, g_black, x-15, y-10, 30, 20, true, false);

    if (CurB)
        color = g_green;
    else
        color = g_white;

    m_ptrImg->DrawString("<b><big>W</big></b>", x-5, y-9, color);
}

void ImageAreaDraw::DrawHScaleY(bool L2R)
{
    int curB = 0; // must in 2D single B mode

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    double zoom_offset_pixel = m_zoomScale * m_zoomOffset;

    GdkColor *color = g_white;
    int y = m_pointScaleY[curB].y;
    for (int i = 0; i < m_widthScaleY * m_zoomScale; i+=50) {
        int x;
        if (L2R)
            x = m_pointScaleY[curB].x + (int)(i/m_ratio2D) - zoom_offset_pixel;
        else
            x = m_pointScaleY[curB].x - (int)(i/m_ratio2D) + zoom_offset_pixel;
        if (x < IMAGE_X || x > IMAGE_X + IMAGE_W)
            continue;
        m_ptrImg->DrawLine(attr, color, x-1, y, x-1, y+SCALE_W, false);
        m_ptrImg->DrawLine(attr, color, x,   y, x,   y+SCALE_W, false);
        m_ptrImg->DrawLine(attr, color, x+1, y, x+1, y+SCALE_W, false);
    }
    for(int i=0; i<m_widthScaleY * m_zoomScale; i+=10) {
        int x;
        if (L2R)
            x = m_pointScaleY[curB].x + (int)(i/m_ratio2D) - zoom_offset_pixel;
        else
            x = m_pointScaleY[curB].x - (int)(i/m_ratio2D) + zoom_offset_pixel;
        if (x < IMAGE_X || x > IMAGE_X + IMAGE_W)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y, x, y+SCALE_W/2, false);
    }
}

void ImageAreaDraw::DrawVScaleY(bool U2D, int curB)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    double zoom_offset_pixel = m_zoomScale * m_zoomOffset;

    GdkColor *color = g_white;
   int x = m_pointScaleY[curB].x;
   PRINTF("pointScaleY(%d, %d), zoom_scale=%.2f, zoom_offSet=%d, zoom_offset_pixel=%.2f, heightScale=%d\n", m_pointScaleY[curB].x, m_pointScaleY[curB].y, m_zoomScale, m_zoomOffset, zoom_offset_pixel, m_heightScaleY);
    for(int i=0; i<m_heightScaleY*m_zoomScale; i+=50) {
        int y;
        if (U2D) {
            y = (int)(i/m_ratio2D) + m_pointScaleY[curB].y - zoom_offset_pixel;
        } else {
            y = m_pointScaleY[curB].y - (int)(i/m_ratio2D) + zoom_offset_pixel;
        }
        if (y < IMAGE_Y || y > IMAGE_Y + IMAGE_H)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y-1, x+SCALE_W, y-1, false);
        m_ptrImg->DrawLine(attr, color, x, y,   x+SCALE_W, y,   false);
        m_ptrImg->DrawLine(attr, color, x, y+1, x+SCALE_W, y+1, false);
    }
    for(int i=0; i<m_heightScaleY*m_zoomScale; i+=10) {
        int y;
        if (U2D)
            y = (int)(i/m_ratio2D) + m_pointScaleY[curB].y - zoom_offset_pixel;
        else
            y = m_pointScaleY[curB].y - (int)(i/m_ratio2D) + zoom_offset_pixel;
        if (y < IMAGE_Y || y > IMAGE_Y + IMAGE_H)
        {
            continue;
        }
        m_ptrImg->DrawLine(attr, color, x, y, x+SCALE_W/2, y, false);
    }
}

void ImageAreaDraw::DrawScaleY(bool update)
{
    if (m_countScaleY == 0)
        return;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = false;

    if (m_scanMode == ScanMode::D2 && m_format2D == Format2D::B) {  // must in 2D single B mode
        m_ptrImg->DrawRectangle(attr, g_black, IMAGE_X + IMAGE_W, IMAGE_Y-1, SCALE_W+2, IMAGE_H+2, true, false);
        m_ptrImg->DrawRectangle(attr, g_black, IMAGE_X-1, IMAGE_Y+IMAGE_H, IMAGE_W+2, SCALE_W+2, true, false);

        if (m_UDStatus) {
            if (m_rotate == 90)
                DrawHScaleY(true);
            else if (m_rotate == 180)
                DrawVScaleY(true, m_curB);
            else if (m_rotate == 270)
                DrawHScaleY(false);
            else
                DrawVScaleY(false, m_curB);
        } else {
            if (m_rotate == 90)
                DrawHScaleY(false);
            else if (m_rotate == 180)
                DrawVScaleY(false, m_curB);
            else if (m_rotate == 270)
                DrawHScaleY(true);
            else
                DrawVScaleY(true, m_curB);
        }
    } else {
        if ((m_scanMode == ScanMode::M_INIT || m_scanMode == ScanMode::PW_INIT) && (m_rotate != 0 || m_rotate != 180))
            m_ptrImg->DrawRectangle(attr, g_black, IMAGE_X-1, IMAGE_Y+IMAGE_H, IMAGE_W+2, SCALE_W+2, true, false);

        m_ptrImg->DrawRectangle(attr, g_black, m_clearScaleY[m_curB].x, m_clearScaleY[m_curB].y-1, SCALE_W+2, m_heightClearScaleY+2, true, false);
        if (m_UDStatus)
            DrawVScaleY(false, m_curB);
        else
            DrawVScaleY(true, m_curB);
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetScaleYFreeze(void)
{
    for (int i = 0; i < 4; ++i) {
        if (m_drawSD[i]){
            if (m_scanMode == ScanMode::D2) {
                m_ratio2D = m_ratio2DAll[i];
                ChangeFormat2D(m_format2D, i, m_ratio2D, m_prePointSD[i].LRStatus, m_prePointSD[i].UDStatus);
                if (m_prePointSD[i].UDStatus)
                    DrawVScaleY(false, i);
                else
                    DrawVScaleY(true, i);
            } else if (m_scanMode == ScanMode::CFM || m_scanMode == ScanMode::PDI) {
                m_ratio2D = m_ratio2DAll[i];
                ChangeFormatCfm(m_formatCfm, i, m_ratio2D, m_prePointSD[i].LRStatus, m_prePointSD[i].UDStatus);
                if (m_prePointSD[i].UDStatus)
                    DrawVScaleY(false, i);
                else
                    DrawVScaleY(true, i);
            }
        }
    }
    Update();
    m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetGrayTable(RGBTRIPLE *table, int size, bool update)
{
    if (size > 256)
        return;

    m_graySize = size;

    for (int i=0; i<size; i++) {
        m_grayTable[i].rgbtRed = table[i].rgbtRed;
        m_grayTable[i].rgbtGreen = table[i].rgbtGreen;
        m_grayTable[i].rgbtBlue = table[i].rgbtBlue;
    }

    DrawGrayScale(update);
}

void ImageAreaDraw::DrawGrayScale(bool update)
{
    const int x = IMAGE_X + IMAGE_W + SCALE_W + TGC_W + IMAGE_TMP + 1;
    const int y = 20;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, x, y, GRAYSCALE_W, GRAYSCALE_H, true, false);

    GdkColor *color = NULL;

    for(int i=0; i<m_graySize; i++) {
        color = new_color(m_grayTable[i].rgbtRed*0xffff/0xff, m_grayTable[i].rgbtGreen*0xffff/0xff, m_grayTable[i].rgbtBlue*0xffff/0xff);
        m_ptrImg->DrawLine(attr, color, x, y+m_graySize-i, x+GRAYSCALE_W, y+m_graySize-i, false);
        free_color(color);
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawFocus(int focSum, int focPos[], bool update)
{
    if (m_countScaleY == 0)
        return ;

    if (m_focusDraw)
        Focus(m_preFocInfo.focPos, false);

    vector<int> tmpFocPos;
    for (int i = 0; i < focSum; i++) {
        tmpFocPos.push_back(focPos[i]);
    }
    Focus(tmpFocPos, true);

    m_preFocInfo.focPos.clear();
    m_preFocInfo.focSum = focSum;
    for (int i = 0; i < focSum; i++)
    {
        m_preFocInfo.focPos.push_back(focPos[i]);
    }
    m_focusDraw = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawFocus(bool update)
{
    if (m_countScaleY == 0)
        return ;

    if (!m_focusDraw) {
        Focus(m_preFocInfo.focPos, true);
        m_focusDraw = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearFocus(bool update)
{
    if (m_countScaleY == 0)
        return ;

    PRINTF("%s: m_focusDraw = %d, ", __FUNCTION__, m_focusDraw);
    if (m_focusDraw) {
        Focus(m_preFocInfo.focPos, false);
        m_focusDraw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::VerFocus(vector<int> &focPos, bool draw, bool U2D)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = draw;

    double zoom_offset_pixel = m_zoomScale * m_zoomOffset;

    PRINTF("%s: m_ratio2D = %.2f, m_zoomScale = %.2f, m_zoomOffset = %d, m_pointFocus.y = %d\n m_LZDotRange[0] = %d, m_LZScale = %.2f, zoom_offset_pixel = %.2f\n", __FUNCTION__, m_ratio2D, m_zoomScale, m_zoomOffset, m_pointFocus.y, m_LZDotRange[0], m_LZScale, zoom_offset_pixel);
    GdkPoint init_points[3] = {{-6, 0}, {0, 6}, {0, -6}};
    GdkPoint points[3];
    int max_y = 0;
    int min_y = 0;
    for (unsigned int i = 0; i < focPos.size(); i++) {
        for (int j=0; j<3; j++) {
            points[j].x = init_points[j].x + m_pointFocus.x;
            if (U2D)
            {
                if (m_inLZMode)
                    points[j].y = init_points[j].y + (int)(focPos[i]/m_ratio2D - m_LZDotRange[0]* m_LZScale) + m_pointFocus.y - zoom_offset_pixel;
                else
                    points[j].y = init_points[j].y + (int)(focPos[i]/m_ratio2D) + m_pointFocus.y - zoom_offset_pixel;
            }
            else
            {
                if (m_inLZMode)
                    points[j].y = init_points[j].y + m_pointFocus.y - (int)(focPos[i]/m_ratio2D - m_LZDotRange[0]*m_LZScale) + zoom_offset_pixel;
                else
                    points[j].y = init_points[j].y + m_pointFocus.y - (int)(focPos[i]/m_ratio2D) + zoom_offset_pixel;
            }
        }
        max_y = max_3(points[0].y, points[1].y, points[2].y);
        min_y = min_3(points[0].y, points[1].y, points[2].y);
        if (min_y < IMAGE_Y - init_points[1].y || max_y > IMAGE_Y + IMAGE_H + init_points[1].y)
            continue;
        m_ptrImg->DrawPolygon(attr, g_white, points, 3, false);
    }
}

void ImageAreaDraw::HorFocus(vector<int> &focPos, bool draw, bool L2R)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = draw;

    double zoom_offset_pixel = m_zoomScale * m_zoomOffset;

    GdkPoint init_points[3] = {{-6, 0}, {0, -6}, {6, 0}};
    GdkPoint points[3];
    int max_x = 0;
    int min_x = 0;

    for (unsigned int i = 0; i < focPos.size(); i++) {
        for (int j=0; j<3; j++) {
            points[j].y = init_points[j].y + m_pointFocus.y;
            if (L2R)
            {
                if (m_inLZMode)
                    points[j].x = init_points[j].x + (int)(focPos[i]/m_ratio2D - m_LZDotRange[0]*m_LZScale) + m_pointFocus.x - zoom_offset_pixel;
                else
                    points[j].x = init_points[j].x + (int)(focPos[i]/m_ratio2D) + m_pointFocus.x - zoom_offset_pixel;
            }
            else
            {
                if (m_inLZMode)
                    points[j].x = init_points[j].x + m_pointFocus.x - (int)(focPos[i]/m_ratio2D - m_LZDotRange[0]*m_LZScale) + zoom_offset_pixel;
                else
                    points[j].x = init_points[j].x + m_pointFocus.x - (int)(focPos[i]/m_ratio2D) + zoom_offset_pixel;
            }
        }
        max_x = max_3(points[0].x, points[1].x, points[2].x);
        min_x = min_3(points[0].x, points[1].x, points[2].x);
        if (min_x < IMAGE_X - init_points[2].x || max_x > IMAGE_X + IMAGE_W + init_points[2].x)
            continue;
        m_ptrImg->DrawPolygon(attr, g_white, points, 3, false);
    }
}

void ImageAreaDraw::Focus(vector<int> &focPos, bool draw)
{
    if (m_scanMode == ScanMode::D2 && m_format2D == Format2D::B && m_rotate != 0) {
        if (m_UDStatus) {
            if (m_rotate == 90)
                HorFocus(focPos, draw, true);
            else if (m_rotate == 180)
                VerFocus(focPos, draw, true);
            else if (m_rotate == 270)
                HorFocus(focPos, draw, false);
        } else {
            if (m_rotate == 90)
                HorFocus(focPos, draw, false);
            else if (m_rotate == 180)
                VerFocus(focPos, draw, false);
            else if (m_rotate == 270)
                HorFocus(focPos, draw, true);
        }
    } else {
        if (m_UDStatus)
            VerFocus(focPos, draw, false);
        else
            VerFocus(focPos, draw, true);
    }
}

void ImageAreaDraw::DrawTGCLine(unsigned char* tgc, int tgcSize, bool update)
{
    const int x = IMAGE_X + IMAGE_W + SCALE_W + IMAGE_TMP - 5;
    const int y = IMAGE_Y;
    const int tgcWidth = 255;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, x, y, TGC_W, IMAGE_H, true, false);

    double spacing = (double)IMAGE_H / (tgcSize);
    double tgcRatio = (double)TGC_W / tgcWidth;
    for (int i=0; i<tgcSize; i++) {
        m_ptrImg->DrawPoint(attr, g_gray, x+tgc[i]*tgcRatio, y+i*spacing, false);
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearTGCLine(bool update)
{
    const int x = IMAGE_X + IMAGE_W + SCALE_W + IMAGE_TMP - 5;
    const int y = IMAGE_Y;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    m_ptrImg->DrawRectangle(attr, g_black, x, y, TGC_W, IMAGE_H, true, update);
}

// 跟穿刺线的实现方法一致
// lihuamei
void ImageAreaDraw::ClearCenterLine(bool update)
{
    int rotate;
    rotate = m_preRotate;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = FALSE;

    if (rotate == 90 || rotate == 270) {
        m_ptrImg->DrawDashLine(attr, g_red, (IMAGE_W - IMAGE_H)/2, IMAGE_H/2, (IMAGE_W - IMAGE_H)/2 + IMAGE_H, IMAGE_H/2, false);
    } else if (rotate == 0 || rotate == 180) {
        m_ptrImg->DrawDashLine(attr, g_red, IMAGE_W/2, 0, IMAGE_W/2, IMAGE_H, false);
    }

    if (update)
        m_ptrImg->UpdateImgArea();
}

// 跟穿刺线的实现方法一致
// lihuamei
void ImageAreaDraw::DrawCenterLine(bool update)
{
    m_preRotate = m_rotate;
    int rotate;
    rotate = m_rotate;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
   attr.mode.cvDraw = TRUE;

    if (rotate == 90 || rotate == 270) {
        m_ptrImg->DrawDashLine(attr, g_red, (IMAGE_W - IMAGE_H)/2, IMAGE_H/2, (IMAGE_W - IMAGE_H)/2 + IMAGE_H, IMAGE_H/2, TRUE);
    } else if (rotate == 0 || rotate == 180) {
        m_ptrImg->DrawDashLine(attr, g_red, IMAGE_W/2, 0, IMAGE_W/2, IMAGE_H, TRUE);
    }

    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::CenterLine(int rotate)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PIXMAP;
    attr.mode.gdkMode = GDK_XOR;

    if (rotate == 90 || rotate == 270) {
        m_ptrImg->DrawDashLine(attr, g_red, (IMAGE_W - IMAGE_H)/2, IMAGE_H/2, (IMAGE_W - IMAGE_H)/2 + IMAGE_H, IMAGE_H/2, false);
    } else if (rotate == 0 || rotate == 180) {
        m_ptrImg->DrawDashLine(attr, g_red, IMAGE_W/2, 0, IMAGE_W/2, IMAGE_H, false);
    }
}

void ImageAreaDraw::ClearBiopsyLine(bool update)
{
   /* if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;*/
    /*if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI))*/
        if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))

    {
        return;
    }
    if (m_drawBiopsy) {
        BiopsyLine(m_preBiopsy.xDist, m_preBiopsy.yDist, m_preBiopsy.LRStatus, m_preBiopsy.UDStatus, m_preBiopsy.rotate, false);
        m_drawBiopsy = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}
void ImageAreaDraw::ClearBiopsyDoubleLine(bool update)
{
   /* if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;*/
    /*if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI))*/
    if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))
    {
        return;
    }

    if (m_drawBiopsyDouble) {
        BiopsyLine(m_preBiopsyDouble0.xDist, m_preBiopsyDouble0.yDist, m_preBiopsyDouble0.LRStatus, m_preBiopsyDouble0.UDStatus, m_preBiopsyDouble0.rotate, false);
        BiopsyLine(m_preBiopsyDouble1.xDist, m_preBiopsyDouble1.yDist, m_preBiopsyDouble1.LRStatus, m_preBiopsyDouble1.UDStatus, m_preBiopsyDouble1.rotate, false);
        m_drawBiopsyDouble = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();

}

/*void ImageAreaDraw::ReDrawBiopsyLine(bool update)
{
    if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;

    if (!m_drawBiopsy) {
        BiopsyLine(m_preBiopsy.xDist, m_preBiopsy.yDist, m_LRStatus, m_UDStatus, m_rotate, true);
        m_preBiopsy.LRStatus = m_LRStatus;
        m_preBiopsy.UDStatus = m_UDStatus;
        m_preBiopsy.rotate = m_rotate;
        m_drawBiopsy = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}*/

void ImageAreaDraw::ReDrawBiopsyLine(bool update)
{
   /* if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;*/
    //-------2016.09.19--hy-----------------------------------//
    /*if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI))*/
   if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))
    {
        return;
    }
    //-------------------------------------------------------------------------------------------------------//

    if (!m_drawBiopsy) {
        BiopsyLine(m_preBiopsy.xDist, m_preBiopsy.yDist, m_LRStatus, m_UDStatus, m_rotate, true);
        m_preBiopsy.LRStatus = m_LRStatus;
        m_preBiopsy.UDStatus = m_UDStatus;
        m_preBiopsy.rotate = m_rotate;
        m_drawBiopsy = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}
void ImageAreaDraw::ReDrawBiopsyDoubleLine(bool update)
{
    //-------2016.09.19--hy-----------------------------------//
    /*if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI))*/
    if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))
    {
        return;
    }
    //-------------------------------------------------------------------------------------------------------//

    if (!m_drawBiopsyDouble) {
        BiopsyLine(m_preBiopsyDouble0.xDist, m_preBiopsyDouble0.yDist, m_LRStatus, m_UDStatus, m_rotate, true);
         BiopsyLine(m_preBiopsyDouble1.xDist, m_preBiopsyDouble1.yDist, m_LRStatus, m_UDStatus, m_rotate, true);

        m_preBiopsyDouble0.LRStatus = m_LRStatus;
        m_preBiopsyDouble0.UDStatus = m_UDStatus;
        m_preBiopsyDouble0.rotate = m_rotate;

        m_preBiopsyDouble1.LRStatus = m_LRStatus;
        m_preBiopsyDouble1.UDStatus = m_UDStatus;
        m_preBiopsyDouble1.rotate = m_rotate;

        m_drawBiopsyDouble = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();

}
// xDist 为m_pointBiopsy和穿刺线起点的距离
// yDist 为m_pointBiopsy和穿刺线终点的距离
void ImageAreaDraw::DrawBiopsyLine(int xDist, int yDist, int size, bool update)
{
    if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;

    if (m_drawBiopsy)
        BiopsyLine(m_preBiopsy.xDist, m_preBiopsy.yDist, m_preBiopsy.LRStatus, m_preBiopsy.UDStatus, m_preBiopsy.rotate, size, false);
    BiopsyLine(xDist, yDist, m_LRStatus, m_UDStatus, m_rotate, size, true);

    m_preBiopsy.LRStatus = m_LRStatus;
    m_preBiopsy.UDStatus = m_UDStatus;
    m_preBiopsy.rotate = m_rotate;
    m_preBiopsy.xDist = xDist;
    m_preBiopsy.yDist = yDist;
    m_drawBiopsy = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

// xDist 为m_pointBiopsy和穿刺线起点的距离
// yDist 为m_pointBiopsy和穿刺线终点的距离
/*void ImageAreaDraw::DrawBiopsyLine(int xDist, int yDist, bool update)
{
    if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;

    if (m_drawBiopsy)
        BiopsyLine(m_preBiopsy.xDist, m_preBiopsy.yDist, m_preBiopsy.LRStatus, m_preBiopsy.UDStatus, m_preBiopsy.rotate, false);
    BiopsyLine(xDist, yDist, m_LRStatus, m_UDStatus, m_rotate, true);

    m_preBiopsy.LRStatus = m_LRStatus;
    m_preBiopsy.UDStatus = m_UDStatus;
    m_preBiopsy.rotate = m_rotate;
    m_preBiopsy.xDist = xDist;
    m_preBiopsy.yDist = yDist;
    m_drawBiopsy = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}*/

// xDist 为m_pointBiopsy和穿刺线起点的距离
// yDist 为m_pointBiopsy和穿刺线终点的距离
//void ImageAreaDraw::DrawBiopsyLine(int xDist, int yDist, bool update)
void ImageAreaDraw::DrawBiopsyLine(double xDist, double  yDist, bool update)
{
   /* if ((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))
        return;*/
    //-------2016.09.19--hy-----------------------------------//
    /*if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI))*/
    if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))
    {
        return;
    }
    //-------------------------------------------------------------------------------------------------------//

    if (m_drawBiopsy)
        BiopsyLine(m_preBiopsy.xDist, m_preBiopsy.yDist, m_preBiopsy.LRStatus, m_preBiopsy.UDStatus, m_preBiopsy.rotate, false);
    BiopsyLine(xDist, yDist, m_LRStatus, m_UDStatus, m_rotate, true);

    m_preBiopsy.LRStatus = m_LRStatus;
    m_preBiopsy.UDStatus = m_UDStatus;
    m_preBiopsy.rotate = m_rotate;
    m_preBiopsy.xDist = xDist;
    m_preBiopsy.yDist = yDist;
    m_drawBiopsy = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}
/*void ImageAreaDraw::DrawBiopsyDoubleLine(int xDist0, int yDist0, int xDist1, int yDist1, bool update)
{
    if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))
    {
        return;
    }
    if (m_drawBiopsyDouble)
    {
        BiopsyLine(m_preBiopsyDouble0.xDist, m_preBiopsyDouble0.yDist, m_preBiopsyDouble0.LRStatus, m_preBiopsyDouble0.UDStatus, m_preBiopsyDouble0.rotate, false);
        BiopsyLine(m_preBiopsyDouble1.xDist, m_preBiopsyDouble1.yDist, m_preBiopsyDouble1.LRStatus, m_preBiopsyDouble1.UDStatus, m_preBiopsyDouble1.rotate, false);

    }
    BiopsyLine(xDist0, yDist0, m_LRStatus, m_UDStatus, m_rotate, true);
    BiopsyLine(xDist1, yDist1, m_LRStatus, m_UDStatus, m_rotate, true);

    m_preBiopsyDouble0.LRStatus = m_LRStatus;
    m_preBiopsyDouble0.UDStatus = m_UDStatus;
    m_preBiopsyDouble0.rotate = m_rotate;
    m_preBiopsyDouble0.xDist = xDist0;
    m_preBiopsyDouble0.yDist = yDist0;

    m_preBiopsyDouble1.LRStatus = m_LRStatus;
    m_preBiopsyDouble1.UDStatus = m_UDStatus;
    m_preBiopsyDouble1.rotate = m_rotate;
    m_preBiopsyDouble1.xDist = xDist1;
    m_preBiopsyDouble1.yDist = yDist1;

    m_drawBiopsyDouble = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}*/
void ImageAreaDraw::DrawBiopsyDoubleLine(double xDist0, double yDist0, double xDist1, double yDist1, bool update)
{
    /*if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))*/
    if (((m_scanMode != ScanMode::D2) || (m_format2D != Format2D::B))&&((m_scanMode!=ScanMode::CFM)||(m_formatCfm != FormatCfm::B))&&(m_scanMode!=ScanMode::PDI)&&(ModeStatus::IsFreezeMode()))
    {
        return;
    }
    if (m_drawBiopsyDouble)
    {
        BiopsyLine(m_preBiopsyDouble0.xDist, m_preBiopsyDouble0.yDist, m_preBiopsyDouble0.LRStatus, m_preBiopsyDouble0.UDStatus, m_preBiopsyDouble0.rotate, false);
        BiopsyLine(m_preBiopsyDouble1.xDist, m_preBiopsyDouble1.yDist, m_preBiopsyDouble1.LRStatus, m_preBiopsyDouble1.UDStatus, m_preBiopsyDouble1.rotate, false);

    }
    BiopsyLine(xDist0, yDist0, m_LRStatus, m_UDStatus, m_rotate, true);
    BiopsyLine(xDist1, yDist1, m_LRStatus, m_UDStatus, m_rotate, true);
    m_preBiopsyDouble0.LRStatus = m_LRStatus;
    m_preBiopsyDouble0.UDStatus = m_UDStatus;
    m_preBiopsyDouble0.rotate = m_rotate;
    m_preBiopsyDouble0.xDist = xDist0;
    m_preBiopsyDouble0.yDist = yDist0;

    m_preBiopsyDouble1.LRStatus = m_LRStatus;
    m_preBiopsyDouble1.UDStatus = m_UDStatus;
    m_preBiopsyDouble1.rotate = m_rotate;
    m_preBiopsyDouble1.xDist = xDist1;
    m_preBiopsyDouble1.yDist = yDist1;

    m_drawBiopsyDouble = true;
    if (update)
        m_ptrImg->UpdateImgArea();

}

void ImageAreaDraw::BiopsyLine(int xDist, int yDist, bool LRStatus, bool UDStatus, int rotate,int size, bool draw)
{
    int xDistPixel = xDist / m_ratio2D;
    int yDistPixel = yDist / m_ratio2D;

    const int rotateSpace = (IMAGE_W - IMAGE_H)/2;
    GdkPoint start;
    GdkPoint end;
    GdkPoint center;
    if (UDStatus && !LRStatus) {
        if (rotate == 90) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        } else if (rotate == 270) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        }
    } else if (LRStatus && !UDStatus) {
        if (rotate == 90) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        } else if (rotate == 270) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        }
    } else if (UDStatus && LRStatus) {
        if (rotate == 90) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        } else if (rotate == 270) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        }
    } else {
        if (rotate == 90) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        } else if (rotate == 270) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        }
    }

    const int boundLeft = 0, boundUp = 0, boundRight = IMAGE_W, boundDown = IMAGE_H;
    double tanLine = ((double)(end.y - start.y)) / ((double)(end.x - start.x));
    double tempLine = (double)start.y + ((double)(boundRight-start.x)) * tanLine;
    GdkPoint endp1, endp2;
    if ((tempLine >= (double)boundUp) && (tempLine<=(double)boundDown)) {
        endp1.x = boundRight;
        endp1.y = (int)tempLine;
    } else {
        if(tempLine < (double)boundUp) {
            endp1.x = start.x + (int)((double)(boundUp-start.y) / tanLine);
            endp1.y = boundUp;
        } else {
            endp1.x = start.x + (int)((double)(boundDown-start.y) / tanLine);
            endp1.y = boundDown;
        }
    }

    tempLine = (double)start.y - ((double)(start.x - boundLeft)) * tanLine;
    if ((tempLine >= (double)boundUp) && (tempLine <= (double)boundDown)) {
        endp2.x = boundLeft;
        endp2.y = (int)tempLine;
    } else {
        if(tempLine < (double)boundUp) {
            endp2.x = start.x - (int)((double)(start.y-boundUp) / tanLine);
            endp2.y = boundUp;
        } else {
            endp2.x = start.x - (int)((double)(start.y-boundDown) / tanLine);
            endp2.y = boundDown;
        }
    }

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = draw;
    m_ptrImg->DrawDashLine(attr, g_red, endp1.x, endp1.y, endp2.x, endp2.y, size, false);
    //m_ptrImg->DrawDashLine(attr, g_white, endp1.x, endp1.y, endp2.x, endp2.y, false);
}
/*void ImageAreaDraw::BiopsyLine(int xDist, int yDist, bool LRStatus, bool UDStatus, int rotate, bool draw)*/
void ImageAreaDraw::BiopsyLine(double xDist, double yDist, bool LRStatus, bool UDStatus, int rotate, bool draw)
{
    int xDistPixel = xDist / m_ratio2D;
    int yDistPixel = yDist / m_ratio2D;

    const int rotateSpace = (IMAGE_W - IMAGE_H)/2;
    GdkPoint start;
    GdkPoint end;
    GdkPoint center;
    if (UDStatus && !LRStatus) {
        if (rotate == 90) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        } else if (rotate == 270) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        }
    } else if (LRStatus && !UDStatus) {
        if (rotate == 90) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        } else if (rotate == 270) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        }
    } else if (UDStatus && LRStatus) {
        if (rotate == 90) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        } else if (rotate == 270) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        }
    } else {
        if (rotate == 90) {
            center.x = IMAGE_W - rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y - xDistPixel;
            end.x = center.x - yDistPixel;
            end.y = center.y;
        } else if (rotate == 180) {
            center.x = IMAGE_W/2;
            center.y = IMAGE_H;
            start.x = center.x + xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y - yDistPixel;
        } else if (rotate == 270) {
            center.x = rotateSpace;
            center.y = IMAGE_H/2;
            start.x = center.x;
            start.y = center.y + xDistPixel;
            end.x = center.x + yDistPixel;
            end.y = center.y;
        } else {
            center.x = IMAGE_W/2;
            center.y = 0;
            start.x = center.x - xDistPixel;
            start.y = center.y;
            end.x = center.x;
            end.y = center.y + yDistPixel;
        }
    }

    const int boundLeft = 0, boundUp = 0, boundRight = IMAGE_W, boundDown = IMAGE_H;
    double tanLine = ((double)(end.y - start.y)) / ((double)(end.x - start.x));
    double tempLine = (double)start.y + ((double)(boundRight-start.x)) * tanLine;
    GdkPoint endp1, endp2;
    if ((tempLine >= (double)boundUp) && (tempLine<=(double)boundDown)) {
        endp1.x = boundRight;
        endp1.y = (int)tempLine;
    } else {
        if(tempLine < (double)boundUp) {
            endp1.x = start.x + (int)((double)(boundUp-start.y) / tanLine);
            endp1.y = boundUp;
        } else {
            endp1.x = start.x + (int)((double)(boundDown-start.y) / tanLine);
            endp1.y = boundDown;
        }
    }

    tempLine = (double)start.y - ((double)(start.x - boundLeft)) * tanLine;
    if ((tempLine >= (double)boundUp) && (tempLine <= (double)boundDown)) {
        endp2.x = boundLeft;
        endp2.y = (int)tempLine;
    } else {
        if(tempLine < (double)boundUp) {
            endp2.x = start.x - (int)((double)(start.y-boundUp) / tanLine);
            endp2.y = boundUp;
        } else {
            endp2.x = start.x - (int)((double)(start.y-boundDown) / tanLine);
            endp2.y = boundDown;
        }
    }

    m_ptrImg->DrawBiopsyLine(GDK_XOR, g_green, endp1.x, endp1.y, endp2.x, endp2.y, false,20);
}

void ImageAreaDraw::ClearReplayBar(void)
{
    m_ptrImg->ClearReplayBar();
}

void ImageAreaDraw::DrawReplayBar(int cur_img, int total_img, int left, int right)
{
    m_ptrImg->DrawReplayBar(cur_img, total_img, left, right);

    m_ptrImg->UpdateImgArea();
}

// ========================= [M] =========================
void ImageAreaDraw::DrawBMLine(int curBMLine, bool update)
{
    if (!m_drawMline)
        return;

    if (m_mLineDraw)
        BMLine(m_preMLine.BMLine, m_preMLine.LRStatus, m_preMLine.UDStatus, m_preMLine.pointBM, false);
    BMLine(curBMLine, m_LRStatus, m_UDStatus, m_pointBM, true);

    m_preMLine.BMLine = curBMLine;
    m_preMLine.LRStatus = m_LRStatus;
    m_preMLine.UDStatus = m_UDStatus;
    m_preMLine.pointBM.x = m_pointBM.x;
    m_preMLine.pointBM.y = m_pointBM.y;

    m_mLineDraw = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearBMLine(bool update)
{
    if (!m_drawMline)
        return;

    if (m_mLineDraw) {
        BMLine(m_preMLine.BMLine, m_preMLine.LRStatus, m_preMLine.UDStatus, m_preMLine.pointBM, false);
        m_mLineDraw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawBMLine(bool update)
{
    if (!m_drawMline)
        return;

    if (!m_mLineDraw) {
        BMLine(m_preMLine.BMLine, m_LRStatus, m_UDStatus, m_pointBM, true);

        m_preMLine.LRStatus = m_LRStatus;
        m_preMLine.UDStatus = m_UDStatus;
        m_preMLine.pointBM.x = m_pointBM.x;
        m_preMLine.pointBM.y = m_pointBM.y;

        m_mLineDraw = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawScaleYM(bool update)
{
    //  m_pointScaleYM;
    //  m_heightScaleYM;
    //  m_ratioYM;
    if (m_scanMode != ScanMode::M && m_scanMode != ScanMode::ANATOMIC_M)
        return ;

    GdkColor *color = g_white;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, m_pointScaleYM.x, IMAGE_Y+IMAGE_H-m_heightScaleYM-2, SCALE_W+1, m_heightScaleYM+4, true, false);

    int x = m_pointScaleYM.x;
    for (int i=0; i<m_heightScaleYM*m_ratioYM; i+=50) {
        int y;
        if (m_UDStatus)
            y = m_pointScaleYM.y - (int)(i/m_ratioYM);
        else
            y = m_pointScaleYM.y + (int)(i/m_ratioYM);
        if (y < IMAGE_Y || y > IMAGE_Y + IMAGE_H)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y-1, x+SCALE_W, y-1, false);
        m_ptrImg->DrawLine(attr, color, x, y, x+SCALE_W, y, false);
        m_ptrImg->DrawLine(attr, color, x, y+1, x+SCALE_W, y+1, false);
    }
    for(int i=0; i<m_heightScaleYM*m_ratioYM; i+=10) {
        int y;
        if (m_UDStatus)
            y = m_pointScaleYM.y - (int)(i/m_ratioYM);
        else
            y = m_pointScaleYM.y + (int)(i/m_ratioYM);
        if (y < IMAGE_Y || y > IMAGE_Y + IMAGE_H)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y, x+SCALE_W/2, y, false);
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawScaleXM(bool update)
{
    // for test
    // m_pointScaleXM.x = IMAGE_X;
    // m_pointScaleXM.y = IMAGE_Y + IMAGE_H;
    // m_widthScaleXM = IMAGE_W;
    // m_ratioXM;

    GdkColor *color = g_white;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, m_pointScaleXM.x-1, m_pointScaleXM.y, m_widthScaleXM, SCALE_H+2, true, false);

    int y = m_pointScaleXM.y + SCALE_H;
    for (int i=0; i<m_widthScaleXM*m_ratioXM; i+=1) {
        int x = (int)(i/m_ratioXM) + m_pointScaleXM.x;
        m_ptrImg->DrawLine(attr, color, x-1, y-SCALE_H, x-1, y, false);
        m_ptrImg->DrawLine(attr, color, x,   y-SCALE_H, x,   y, false);
        m_ptrImg->DrawLine(attr, color, x+1, y-SCALE_H, x+1, y, false);
    }
    for (float i=0.5; i<m_widthScaleXM*m_ratioXM; i+=1) {
        int x = (int)(i/m_ratioXM) + m_pointScaleXM.x;
        m_ptrImg->DrawLine(attr, color, x, y-SCALE_H/2, x, y, false);
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawGridM(bool update)
{
    if (m_scanMode != ScanMode::M && m_scanMode != ScanMode::ANATOMIC_M)
        return ;

    if (m_drawGridM) {
        GridM(m_ratioXMPre, m_ratioYMPre, m_prePointScaleYM, false);
        //      m_drawGridM = false;
    }
    GridM(m_ratioXM, m_ratioYM, m_pointScaleYM, true);
    m_ratioXMPre = m_ratioXM;
    m_ratioYMPre = m_ratioYM;
    m_prePointScaleYM = m_pointScaleYM;
    m_drawGridM = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearGridM(bool update)
{
    if (m_scanMode != ScanMode::M && m_scanMode != ScanMode::ANATOMIC_M)
        return ;

    if (m_drawGridM) {
        GridM(m_ratioXM, m_ratioYM, m_pointScaleYM, false);
        m_drawGridM = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::GridM(double ratioX, double ratioY, GdkPoint pointScaleYM, bool draw)
{
    // for test
    // m_pointScaleXM.x = IMAGE_X;
    // m_pointScaleYM.y = IMAGE_Y;
    // m_widthScaleXM = IMAGE_W;
    // m_heightScaleYM = IMAGE_H;
    // m_ratioXM = m_ratioX;
    // m_ratioYM = m_ratioY;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SPECTRA;
    attr.mode.cvDraw = draw;

    for(int j=0; j<m_heightScaleYM*ratioY; j+=10) {
        for (int i=0; i<m_widthScaleXM*ratioX; i+=1) {
            int x = m_pointScaleXM.x - IMAGE_X + (int)i/ratioX;
            int y;
            if (m_UDStatus)
                y = pointScaleYM.y - IMAGE_Y - (int)j/ratioY;
            else
                y = pointScaleYM.y - IMAGE_Y + (int)j/ratioY;
            m_ptrImg->DrawPoint(attr, g_green, x, y, false);
        }
    }
}

void ImageAreaDraw::DrawAnatomicalM(POINT head, POINT tail, bool draw, bool update)
{
    m_headMPre.x = head.x;
    m_headMPre.y = head.y;
    m_tailMPre.x = tail.x;
    m_tailMPre.y = tail.y;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = draw;

    m_ptrImg->DrawCircle(attr, g_blue, tail.x, tail.y, 5, true, false);
    m_ptrImg->DrawArrow(head, tail, draw, g_yellow, false);
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawAnatomicalM(bool update)
{
    DrawAnatomicalM(m_headMPre, m_tailMPre, true, update);
}

// ========================= [PW] =========================
void ImageAreaDraw::DrawSV(int curPwLine, int dotBegin, int dotEnd, vector<int> HPRFEmitPos, bool update)
{
#ifdef CT_430
    if (ModeStatus::IsSpectrumColorMode())
    {
        if (curPwLine < m_curBox.lineBegin)
            curPwLine = m_curBox.lineBegin;
        else if (curPwLine > m_curBox.lineEnd)
            curPwLine = m_curBox.lineEnd;

        //set curPwLine to 4 ,if not then pw line is out of cfm box when its value is less than 2.
        if (curPwLine <=2)
            curPwLine = 4;
    }
#endif

    if (!m_drawSV) {
        m_preSV.pwLine = curPwLine;
        m_preSV.dotBegin = dotBegin;
        m_preSV.dotEnd = dotEnd;
        m_preSV.HPRFEmitPos = HPRFEmitPos;
        m_preSV.LRStatus = m_LRStatus;
        m_preSV.UDStatus = m_UDStatus;
        m_preSV.pointPw.x = m_pointPw.x;
        m_preSV.pointPw.y = m_pointPw.y;
        return ;
    }

    // clear
    if (m_SVDraw)
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_preSV.LRStatus, m_preSV.UDStatus, m_preSV.pointPw, false, false);
    // draw
    SampleVolumn(curPwLine, dotBegin, dotEnd, HPRFEmitPos, m_LRStatus, m_UDStatus, m_pointPw, false, true);
    PRINTF("**********dotBegin dotEnd x y %d %d %d %d\n",dotBegin,dotEnd,m_pointPw.x,m_pointPw.y);
    m_preSV.pwLine = curPwLine;
    m_preSV.dotBegin = dotBegin;
    m_preSV.dotEnd = dotEnd;
    m_preSV.HPRFEmitPos = HPRFEmitPos;
    m_preSV.LRStatus = m_LRStatus;
    m_preSV.UDStatus = m_UDStatus;
    m_preSV.pointPw.x = m_pointPw.x;
    m_preSV.pointPw.y = m_pointPw.y;
    m_SVDraw = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearSV(bool update)
{
    if (!m_drawSV)
        return ;

    // clear
    if (m_SVDraw) {
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_preSV.LRStatus, m_preSV.UDStatus, m_preSV.pointPw, false, false);
        m_SVDraw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawSV(bool update)
{
    if (!m_drawSV)
        return;

    if (!m_SVDraw) {
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_LRStatus, m_UDStatus, m_pointPw, false, true);
        m_preSV.LRStatus = m_LRStatus;
        m_preSV.UDStatus = m_UDStatus;
        m_preSV.pointPw.x = m_pointPw.x;
        m_preSV.pointPw.y = m_pointPw.y;
        m_SVDraw = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

// ========================= [CW] =========================
void ImageAreaDraw::DrawSVCW(int curPwLine, int dotPos, bool update)
{
    vector<int> vecTemp;

    if (!m_drawSV) {
        m_preSV.pwLine = curPwLine;
        m_preSV.dotBegin = dotPos;
        m_preSV.dotEnd = dotPos;
        m_preSV.HPRFEmitPos = vecTemp;
        m_preSV.LRStatus = m_LRStatus;
        m_preSV.UDStatus = m_UDStatus;
        m_preSV.pointPw.x = m_pointPw.x;
        m_preSV.pointPw.y = m_pointPw.y;
        return ;
    }

    // clear
    if (m_SVDraw)
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_preSV.LRStatus, m_preSV.UDStatus, m_preSV.pointPw, true, false);
    // draw
    SampleVolumn(curPwLine, dotPos, dotPos, vecTemp, m_LRStatus, m_UDStatus, m_pointPw, true, true);
    m_preSV.pwLine = curPwLine;
    m_preSV.dotBegin = dotPos;
    m_preSV.dotEnd = dotPos;
    m_preSV.HPRFEmitPos = vecTemp;
    m_preSV.LRStatus = m_LRStatus;
    m_preSV.UDStatus = m_UDStatus;
    m_preSV.pointPw.x = m_pointPw.x;
    m_preSV.pointPw.y = m_pointPw.y;
    m_SVDraw = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearSVCW(bool update)
{
    if (!m_drawSV)
        return ;

    // clear
    if (m_SVDraw) {
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_preSV.LRStatus, m_preSV.UDStatus, m_preSV.pointPw, true, false);
        m_SVDraw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawSVCW(bool update)
{
    if (!m_drawSV)
        return;

    if (!m_SVDraw) {
        SampleVolumn(m_preSV.pwLine, m_preSV.dotBegin, m_preSV.dotEnd, m_preSV.HPRFEmitPos, m_LRStatus, m_UDStatus, m_pointPw, false, true);
        m_preSV.LRStatus = m_LRStatus;
        m_preSV.UDStatus = m_UDStatus;
        m_preSV.pointPw.x = m_pointPw.x;
        m_preSV.pointPw.y = m_pointPw.y;
        m_SVDraw = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetPwScaleUnit(SCALEUNIT unit, bool update)
{
    m_pwUnit = unit;

    if (unit == KHZ) {
        m_ratioYPw = ImgPw::GetInstance()->GetScaleVelKHz();
        m_pwMaxVel = ImgPw::GetInstance()->GetMaxVelKHz();
    } else {
        m_ratioYPw = ImgPw::GetInstance()->GetScaleVel();
        m_pwMaxVel = ImgPw::GetInstance()->GetMaxVel();
    }
    DrawScaleYPw(update);
}

void ImageAreaDraw::SetPwBaseLine(int curLevel, int maxLevel, bool update)
{
    m_pwMaxLevel = maxLevel;
    m_pwCurLevel = curLevel;
    DrawScaleYPw(update);
}

// x,y is the coordinate of the base line
void ImageAreaDraw::DrawScaleYPw(bool update)
{
    // m_pwMaxVel = 75;
    // m_ratioYPw = m_ratio2D;

    GdkColor *color = g_white;
    const int hChar = 8;
   int adjustY = 1;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;
    const float kMaxVel = 500.0; // 大于kMaxVel时转换单位

    int BoundUp, BoundDown;
    int pwHeight = (m_pwMaxVel / m_ratioYPw);
   // double pwHeight =(double)m_pwMaxVel / (double)m_ratioYPw;
// double pwHeight =(double)(m_pwMaxVel / m_ratioYPw);
    PRINTF("pwHeight=%f\n", pwHeight);
    BoundUp = m_pointScaleYPw.y - pwHeight;
    BoundDown = m_pointScaleYPw.y + pwHeight;

    // clean
    // 擦除PwScaleY的区域，不包含baseline, 刻度值+刻度线的宽度为50
    m_ptrImg->DrawRectangle(attr, g_black, m_pointScaleYPw.x+m_widthScaleYPw, IMAGE_Y+IMAGE_H-m_heightScaleYPw-7, 50, m_heightScaleYPw+14, true, false);

    // base line
    double pwBaseLine = (double)m_pwCurLevel / m_pwMaxLevel * (double)m_pwMaxVel;
    GdkPoint baseline;
    baseline.x = m_pointScaleYPw.x;
    baseline.y = m_pointScaleYPw.y - (pwBaseLine / m_ratioYPw);

    if (baseline.y <= BoundUp)//IMAGE_Y)
        baseline.y = BoundUp;//IMAGE_Y+adjustY;
    else if (baseline.y >= BoundDown)  // IMAGE_H + IMAGE_Y - adjustY)
        baseline.y = BoundDown-adjustY; //IMAGE_H + IMAGE_Y - adjustY;
    PwBaseline(baseline.x - IMAGE_X, baseline.y - IMAGE_Y, color); // baseline 画在spectra区域上

    PRINTF("Bound(%d, %d) baseLine.y=%d\n", BoundUp, BoundDown, baseline.y);

    if (m_pwCurLevel == 0)
    {
        int detalUpY, detalDownY;
        detalUpY = baseline.y - BoundUp;
        detalDownY = BoundDown - baseline.y;

        if (detalUpY > detalDownY)
            BoundUp = baseline.y - detalDownY;
        else
            BoundDown = detalUpY + baseline.y;
    }

    PRINTF("pw maxvel= %.2f\n", m_pwMaxVel);

    // Unit
    if (m_pwUnit == CMPS) {
        if (m_pwMaxVel > kMaxVel)
            m_ptrImg->DrawString("m/s", m_pointScaleYPw.x + m_widthScaleYPw+SCALE_W/2, baseline.y-hChar);
        else
            m_ptrImg->DrawString("cm/s", m_pointScaleYPw.x + m_widthScaleYPw+SCALE_W/2, baseline.y-hChar);
    }
    else if (m_pwUnit == KHZ) {
        m_ptrImg->DrawString("KHz", m_pointScaleYPw.x + m_widthScaleYPw+SCALE_W/2, baseline.y-hChar);
    }

    int x = m_pointScaleYPw.x;

#if 0
    if(m_preScanModel==CW)
#endif

    char scaleValue[10];
    int ValuePerScale = PwScaleCalc(m_pwMaxVel / m_pwMaxLevel);

    if (ValuePerScale < 0)
        return ;

    for (int i=ValuePerScale*2; i<m_pwMaxVel*2; i+=ValuePerScale*2) {
        int k = (int)(i/m_ratioYPw);
        //up scale
        if (baseline.y - k > BoundUp) {
            m_ptrImg->DrawLine(attr, color, x+m_widthScaleYPw, baseline.y-k, x+m_widthScaleYPw+SCALE_W, baseline.y-k, false);
            if (m_pwMaxVel > kMaxVel) {
                sprintf(scaleValue, "%-3.1f", i/100.0);
            } else {
                sprintf(scaleValue, "%-3d", i);
            }
            m_ptrImg->DrawString(scaleValue, x+m_widthScaleYPw+SCALE_W+3, baseline.y-k-hChar);
        }

        //down scale
        if (baseline.y + k < BoundDown) {
            m_ptrImg->DrawLine(attr, color, x+m_widthScaleYPw, baseline.y+k, x+m_widthScaleYPw+SCALE_W, baseline.y+k, false);
            if (m_pwMaxVel > kMaxVel) {
                sprintf(scaleValue, "-%-3.1f", i/100.0);
            } else {
                sprintf(scaleValue, "-%-3d", i);
            }
            m_ptrImg->DrawString(scaleValue, x+m_widthScaleYPw+SCALE_W+3, baseline.y+k-hChar);

        }
    }
    for (int i=ValuePerScale; i<m_pwMaxVel*2; i+=ValuePerScale*2) {
        int k = (int)(i/m_ratioYPw);
        if (baseline.y - k > BoundUp)
            m_ptrImg->DrawLine(attr, color, x+m_widthScaleYPw, baseline.y-k, x+m_widthScaleYPw+SCALE_W/2, baseline.y-k, false);
        if (baseline.y + k < BoundDown)
            m_ptrImg->DrawLine(attr, color, x+m_widthScaleYPw, k+baseline.y, x+m_widthScaleYPw+SCALE_W/2, k+baseline.y, false);
    }

    if (update)
    {
        m_ptrImg->UpdateImgArea();
    }
}

int ImageAreaDraw::PwScaleCalc(double vel)
{
    int value = (int)vel;
    if (vel > value)
        value += 1;

    int tmp = value % 10;

    if (value < 5)
        return value;

    if (tmp == 0 || tmp == 5)
        return value;
    else if (tmp < 5)
        return value += (5 - tmp);
    else if (tmp > 5)
        return value += (10 - tmp);
    return -1;
}

void ImageAreaDraw::PwBaseline(int x, int y, GdkColor *color)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SPECTRA;
    attr.mode.cvDraw = false;

    if (m_drawPwBaseline) {
        m_ptrImg->DrawLine(attr, color, m_pwBaseline.x, m_pwBaseline.y, m_pwBaseline.x+m_widthScaleYPw, m_pwBaseline.y, false);
    }
    attr.mode.cvDraw = true;
    //m_ptrImg->DrawLine(attr, color, x, y, x+m_widthScaleYPw, y, false);
    m_ptrImg->DrawLine(attr, color, x, y, x+m_widthScaleYPw, y, true);
    m_pwBaseline.x = x;
    m_pwBaseline.y = y;
    m_drawPwBaseline = true;
}

void ImageAreaDraw::DrawScaleXPw(bool update)
{
    // for test
    // m_pointScaleXPw.x = IMAGE_X;
    // m_pointScaleXPw.y = IMAGE_Y + IMAGE_H;
    // m_ratioXPw = m_ratio2D;
    // m_widthScaleXPw = IMAGE_W * m_ratioXPw;

    GdkColor *color = g_white;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    m_ptrImg->DrawRectangle(attr, g_black, m_pointScaleXPw.x, m_pointScaleXPw.y, m_widthScaleXPw/m_ratioXPw, SCALE_H+2, true, false);

    int y = m_pointScaleXPw.y + SCALE_H;
    for (int i=0; i<m_widthScaleXPw; i+=1) {
        int x = (int)(i/m_ratioXPw) + m_pointScaleXPw.x;
        m_ptrImg->DrawLine(attr, color, x-1, y-SCALE_H, x-1, y, false);
        m_ptrImg->DrawLine(attr, color, x,   y-SCALE_H, x,   y, false);
        m_ptrImg->DrawLine(attr, color, x+1, y-SCALE_H, x+1, y, false);
    }
    for (float i=0.5; i<m_widthScaleXPw; i+=1) {
        int x = (int)(i/m_ratioXPw) + m_pointScaleXPw.x;
        m_ptrImg->DrawLine(attr, color, x, y-SCALE_H/2, x, y, false);
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawGridPw(bool update)
{
    if (m_drawGridPw) {
        GridPw(m_pwMaxVelPre, m_pwMaxLevelPre, m_pwCurLevelPre, m_ratioXPwPre, m_ratioYPwPre, false);
    }
    GridPw(m_pwMaxVel, m_pwMaxLevel, m_pwCurLevel, m_ratioXPw, m_ratioYPw, true);
    m_pwMaxVelPre = m_pwMaxVel;
    m_pwMaxLevelPre = m_pwMaxLevel;
    m_pwCurLevelPre = m_pwCurLevel;
    m_ratioXPwPre = m_ratioXPw;
    m_ratioYPwPre = m_ratioYPw;
    m_drawGridPw = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearGridPw(bool update)
{
    if (m_drawGridPw) {
        GridPw(m_pwMaxVel, m_pwMaxLevel, m_pwCurLevel, m_ratioXPw, m_ratioYPw, false);
        m_drawGridPw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::GridPw(float maxVel, int maxLevel, int curLevel, float ratioX, float ratioY, bool draw)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SPECTRA;
    attr.mode.cvDraw = draw;

    int ValuePerScale = PwScaleCalc(maxVel / maxLevel);
    double pwBaseLine = (double)curLevel / maxLevel * (double)maxVel;
    int baseline = m_pointScaleYPw.y - IMAGE_Y - (pwBaseLine / ratioY);
    double pwHeight = maxVel / ratioY;

    for(int j=ValuePerScale*2; j<maxVel*2; j+=ValuePerScale*2) {
        for (int i=0; i<m_widthScaleXPw; i+=1) {
            int x = m_pointScaleXPw.x - IMAGE_X + (int)i/ratioX;
            int y = baseline - (int)j/ratioY;
            if (y > m_pointScaleYPw.y -IMAGE_Y - pwHeight) {
                m_ptrImg->DrawPoint(attr, g_green, x, y, false);
            }

            y = baseline + (int)j/ratioY;
            if (y < m_pointScaleYPw.y -IMAGE_Y + pwHeight) {
                m_ptrImg->DrawPoint(attr, g_green, x, y, false);
            }
        }
    }
}
//draw trace and calculate in auto calc;
void ImageAreaDraw::DrawPwTraceAuto(bool auto_calc, const Image::AutoTracePara* para)
{
    bool traceStatus = ImgProcPw::GetInstance()->GetTraceStatus();
    vector<POINT> validTrace;
    static POINT psP, edP;
    MeasureDraw draw;
    UpdateMeasure update;
    UpdateMeasure::ResultAttr attr;

    if (para == NULL)
    {
        return;
    }
    //attr.cursorType = draw.GetCursorType();
    attr.cursorType = draw.GetOrderNumber();
    attr.curColor = draw.GetCurColor();
    attr.confirmColor = draw.GetConfirmColor();

    if (traceStatus) {
        m_ptrImg->ClearTrace();
        draw.PwTrace(para->vecTrace);
        if (auto_calc) {
            if (para->vecMaxTrace.size() > 1) {
                int cycleBegin, cycleEnd;
                if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())&& (m_formatPw == FormatPw::BP11_LR)) {
                    cycleBegin = para->cycleBegin - IMAGE_W/2;
                    cycleEnd = para->cycleEnd - IMAGE_W/2;
                } else {
                    cycleBegin = para->cycleBegin;
                    cycleEnd = para->cycleEnd;
                }

                draw.PwCycle(para->vecMaxTrace[cycleBegin].x, para->vecMaxTrace[cycleEnd].x);

                MeasureCalc calc;
                for (int i=0; i<=(cycleEnd-cycleBegin); i++) {
                    validTrace.push_back(para->vecMaxTrace[cycleBegin+i]);
                }

                if (validTrace.size() > 1) {
                        if(m_autoPara == NULL)
                    {
                        m_autoPara = (char*) malloc(350);
                        memset(m_autoPara, 0, sizeof(m_autoPara));
                    }
                    char buf[350] = {'\0'};
                    MeasureMan::GetInstance()->ClearFirst();
                    calc.DCalcTraceAuto(buf, validTrace, psP, edP, 0);
                    g_calcPwStatus = false;
                    memcpy(m_autoPara, buf, 350);
                    update.DTrace(buf, attr, true);
                }
                else
                {
                    if(g_calcPwStatus)
                        if((m_psP.x != 0) && (m_edP.x != 0))
                        {
                            draw.DrawTraceTag(m_psP, FALSE);
                            draw.DrawTraceTag(m_edP, FALSE);
                        }
                }
            }
        }
        else
        {
            if(m_psP.x != m_edP.x)
            {
                if(m_curOpt == 2)
                {
                    draw.DrawTraceTag(m_psP, TRUE);
                    draw.DrawTraceTag(m_edP, FALSE);
                }
                else if(m_curOpt == 3)
                {
                    draw.DrawTraceTag(m_psP, FALSE);
                    draw.DrawTraceTag(m_edP, TRUE);
                }
                else
                {
                    draw.DrawTraceTag(m_psP, FALSE);
                    draw.DrawTraceTag(m_edP, FALSE);
                }
            }
        }
    }
    else //draw tag when mode unFreeze
    {
        if(g_calcPwStatus){
            if(m_psP.x != 0 && m_edP.x != 0)
            {
                draw.DrawTraceTag(m_psP, FALSE);
                draw.DrawTraceTag(m_edP, FALSE);
            }}
        else
            m_psP.x = m_edP.x = 0; //draw tag to black
    }
}

void ImageAreaDraw::DrawPwTrace(bool auto_calc, const Image::AutoTracePara* para)
{
    bool traceStatus = ImgProcPw::GetInstance()->GetTraceStatus();
    vector<POINT> validTrace;
    static POINT psP, edP;
    MeasureDraw draw;
    UpdateMeasure update;
    UpdateMeasure::ResultAttr attr;

    if (para == NULL)
    {
        return;
    }
    attr.cursorType = draw.GetCursorType();
    attr.curColor = draw.GetCurColor();
    attr.confirmColor = draw.GetConfirmColor();

    if (traceStatus) {
        m_ptrImg->ClearTrace();
        draw.PwTrace(para->vecTrace);
        if (auto_calc) {
            if (para->vecMaxTrace.size() > 1) {
                int cycleBegin, cycleEnd;
                if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())&& (m_formatPw == FormatPw::BP11_LR)) {
                    cycleBegin = para->cycleBegin - IMAGE_W/2;
                    cycleEnd = para->cycleEnd - IMAGE_W/2;
                } else {
                    cycleBegin = para->cycleBegin;
                    cycleEnd = para->cycleEnd;
                }

                draw.PwCycle(para->vecMaxTrace[cycleBegin].x, para->vecMaxTrace[cycleEnd].x);

                MeasureCalc calc;
                for (int i=0; i<=(cycleEnd-cycleBegin); i++) {
                    validTrace.push_back(para->vecMaxTrace[cycleBegin+i]);
                }

                if (validTrace.size() > 1) {
                    if(m_autoPara == NULL)
                    {
                        m_autoPara = (char*) malloc(350);
                        memset(m_autoPara, 0, sizeof(m_autoPara));
                    }
                    char buf[350] = {'\0'};
                    calc.DCalcTrace(buf, validTrace, psP, edP, 0);
                    memcpy(m_autoPara, buf, 350);
                    update.DTrace(buf, attr, true);
                    // draw.DrawTraceTag(psP, FALSE);
                    // draw.DrawTraceTag(edP, FALSE);
                }
            }
        }
        else
        {
            if(m_psP.x != m_edP.x)
            {
                if(m_curOpt == 2)
                {
                    draw.DrawTraceTag(m_psP, TRUE);
                    draw.DrawTraceTag(m_edP, FALSE);
                }
                else if(m_curOpt == 3)
                {
                    draw.DrawTraceTag(m_psP, FALSE);
                    draw.DrawTraceTag(m_edP, TRUE);
                }
#if 0
                else
                {
                    draw.DrawTraceTag(m_psP, FALSE);
                    draw.DrawTraceTag(m_edP, FALSE);
                }
#endif
            }
        }
    }
}

/**
 * @brief: clear auto calc result when autocalc is off
 */
void ImageAreaDraw::ClearAutoCalcPara(bool auto_calc)
{
    MeasureDraw draw;
    UpdateMeasure update;
    UpdateMeasure::ResultAttr attr;

    attr.cursorType = draw.GetCursorType();
    attr.curColor = 5;
    attr.confirmColor = draw.GetConfirmColor();
    if(m_autoPara != NULL)
    {
        //update.DTrace(m_autoPara, attr, true);
        update.ClearCurColorPwResult();
        free(m_autoPara);
        m_autoPara = NULL;
    }
}

char* ImageAreaDraw::GetCalcAutoPara()
{
    return m_autoPara;
}

void ImageAreaDraw::DrawTraceTag(const Image::AutoTracePara* para, POINT &cycStart, POINT &cycEnd, POINT &ps, POINT &ed)
{
    int up, down, left, right;
    vector<POINT> validTrace;
    static POINT psP, edP;
    MeasureDraw draw;
    UpdateMeasure update;
    UpdateMeasure::ResultAttr attr;

    GetBoundary(up, down, left, right);
    cycStart.x = 0;
    cycStart.y = up;
    cycEnd.x = 0;
    cycEnd.y = down;
    ps.x = 0;
    ps.y = up;
    ed.x = 0;
    ed.y = down;
    if (para == NULL)
    {
        return;
    }
    attr.cursorType = draw.GetOrderNumber();
    attr.curColor = draw.GetCurColor();
    attr.confirmColor = draw.GetConfirmColor();

       if (para->vecMaxTrace.size() > 1) {
        int cycleBegin, cycleEnd;
        if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode())&& (m_formatPw == FormatPw::BP11_LR)) {
            cycleBegin = para->cycleBegin - IMAGE_W/2;
            cycleEnd = para->cycleEnd - IMAGE_W/2;
        } else {
            cycleBegin = para->cycleBegin;
            cycleEnd = para->cycleEnd;
        }

        draw.PwCycle(para->vecMaxTrace[cycleBegin].x, para->vecMaxTrace[cycleEnd].x);
        cycStart.x = para->vecMaxTrace[cycleBegin].x;
        cycStart.y = up;
        cycEnd.x = para->vecMaxTrace[cycleEnd].x;
        cycEnd.y = down;

        MeasureCalc calc;
        for (int i=0; i<=(cycleEnd-cycleBegin); i++) {
            validTrace.push_back(para->vecMaxTrace[cycleBegin+i]);
        }

        if (validTrace.size() > 1) {
            char buf[350] = {'\0'};
            m_ptrImg->ClearTrace();
            //redraw tag
            if(g_calcPwStatus)
            {
                if(m_psP.x != 0 && m_edP.x != 0)
                {
                    draw.DrawTraceTag(m_psP, FALSE);
                    draw.DrawTraceTag(m_edP, FALSE);
                }
            }
            draw.DrawVLine(cycStart, false);
            if(m_autoPara == NULL)
            {
                m_autoPara = (char*) malloc(350);
                memset(m_autoPara, 0, sizeof(m_autoPara));
            }
            calc.DCalcTraceAuto(buf, validTrace, psP, edP, 1);
            validTrace.push_back(psP);
            validTrace.push_back(edP);
            update.DTrace(buf, attr, true);
            memcpy(m_autoPara, buf, 350);
            g_calcPwStatus = true;
            ps = psP;
            ed = edP;
            // save measure result
            MeasureMan::GetInstance()->AddNew(MEASURE_TRACK_AUTO, attr.cursorType, validTrace, draw.GetCursorSize(), attr.confirmColor);
            //pop edP
            validTrace.pop_back();
            //pop psP
            validTrace.pop_back();

            m_ptrImg->ClearTrace();
            if(psP.x != 0 && edP.x != 0)
            {
                draw.DrawTraceTag(psP, FALSE);
                draw.DrawTraceTag(edP, FALSE);
            }
            draw.PwTrace(validTrace);
            draw.DrawVLine(cycEnd, false);
            update.DTrace(buf, attr, false);
        }
    }
}

void ImageAreaDraw::GetPwTracePara(Image::AutoTracePara &para)
{
    para = m_pwTracePara;
}

Image::AutoTracePara* ImageAreaDraw::NewPwTracePara(void)
{
    m_pwTracePara.vecTrace = ImgProcPw::GetInstance()->GetTraceData();
    m_pwTracePara.vecMaxTrace = ImgProcPw::GetInstance()->GetTraceCalcData();
#if 0
    for(int i = 0; i < m_pwTracePara.vecMaxTrace.size(); i++)
        printf("+++x= %d y= %d\n", m_pwTracePara.vecMaxTrace[i].x, m_pwTracePara.vecMaxTrace[i].y);
#endif
    int cycleBegin = 0, cycleEnd = 0;
    ImgProcPw::GetInstance()->GetTraceCycle(cycleBegin, cycleEnd);

    //printf("cycleBegin = %d, cycleEnd = %d\n", cycleBegin, cycleEnd);
    m_pwTracePara.cycleBegin = cycleBegin;
    m_pwTracePara.cycleEnd = cycleEnd;

    return &m_pwTracePara;
}

void ImageAreaDraw::SetPsEdValue(POINT ps, POINT ed, int curOpt)
{
    m_psP = ps;
    m_edP = ed;
    m_curOpt = curOpt;
}

//>=========================[CFM]=====================================
void ImageAreaDraw::SetColorTable(unsigned char table[][4], int len, int width, int bitDepth, bool update)
{
    m_colorTable.clear();
    for (int i = 0; i < len*width; i++) {
        ColorScale tmpColor;
        tmpColor.red = table[i][0];
        tmpColor.green = table[i][1];
        tmpColor.blue = table[i][2];
        tmpColor.alpha = table[i][3];
        m_colorTable.push_back(tmpColor);
    }

    m_colorScaleLen = len;
    m_colorScaleWidth = width;

    DrawColorScale(update);
}

void ImageAreaDraw::DrawVelScaleRange(void)
{
    const int x = IMAGE_X + IMAGE_W + SCALE_W + TGC_W + GRAYSCALE_W + IMAGE_TMP;
    const int y = 20;

    char RangeMax[15];
    char RangeMin[15];

    if (m_cfmUnit == CMPS) {
        if (fabs(m_colRange[0])>100.0 || fabs(m_colRange[1])>100.0) {
            sprintf(RangeMax, "%4.1f", m_colRange[0]/100);
            sprintf(RangeMin, "%5.1f\n  m/s", m_colRange[1]/100);
        } else {
            sprintf(RangeMax, "%4.1f", m_colRange[0]);
            sprintf(RangeMin, "%5.1f\n cm/s", m_colRange[1]);
        }
    } else if (m_cfmUnit == KHZ) {
        sprintf(RangeMax, "%4.1f", m_colRange[2]);
        sprintf(RangeMin, "%5.1f\n KHz", m_colRange[3]);
    }

    ImageAreaPara::GetInstance()->FillRectangle(x-10,0,GRAYSCALE_W + COLORSCALE_W, 20);
    m_ptrImg->DrawString(RangeMax, x-10, 0, g_white);

    ImageAreaPara::GetInstance()->FillRectangle(x-15, y +COLORSCALE_H +5, GRAYSCALE_W+COLORSCALE_W+10, 20+30);
    m_ptrImg->DrawString(RangeMin, x-15, y+COLORSCALE_H+5, g_white);
}

void ImageAreaDraw::DrawColorScale(bool update)
{
    const int x = IMAGE_X + IMAGE_W + SCALE_W + TGC_W + GRAYSCALE_W + 3 + IMAGE_TMP;
    const int y = 20;
    GdkColor *color = NULL;

    int len = m_colorScaleLen;
    int width = m_colorScaleWidth;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < width; j++) {
            color = new_color((m_colorTable[i*width+j].red)*0xffff/0xff, (m_colorTable[i*width+j].green)*0xffff/0xff, (m_colorTable[i*width+j].blue)*0xffff/0xff);
            m_ptrImg->DrawPoint(attr, color, x+j, y+i, false);
            free_color(color);
        }
    }
    if (ModeStatus::IsCFMMode() || ModeStatus::IsCFMCompareMode())
        DrawVelScaleRange();

    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetColorScaleUnit(SCALEUNIT unit)
{
    if (ModeStatus::IsCFMMode() || ModeStatus::IsCFMCompareMode()) {
        m_cfmUnit = unit;
        DrawVelScaleRange();
    }
}

void ImageAreaDraw::SetColorScaleRange(double max, double min, double maxKHz, double minKHz)
{
    m_colRange[0] = max;
    m_colRange[1] = min;
    m_colRange[2] = maxKHz;
    m_colRange[3] = minKHz;

    if (ModeStatus::IsCFMMode() || ModeStatus::IsCFMCompareMode()) {
        DrawVelScaleRange();
    }
}

void ImageAreaDraw::SetCfmBoxDashStatus(bool isDash)
{
    m_dashCfmBox = isDash;
}

void ImageAreaDraw::DrawCfmBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool update)
{
    //printf("drawcfm----------lineEnd = %d, lineBegin = %d, dotBegin = %d, dotEnd = %d\n", lineEnd, lineBegin, dotBegin, dotEnd);
    if (m_drawBox[m_curB])
        CfmBox(m_curBox.lineBegin, m_curBox.lineEnd, m_curBox.dotBegin, m_curBox.dotEnd, m_curBox.LRStatus, m_curBox.UDStatus, m_curBox.point, false, m_dashCfmBox);

    CfmBox(lineBegin, lineEnd, dotBegin, dotEnd, m_LRStatus, m_UDStatus, m_pointCfm, true, m_dashCfmBox);

    m_preBox[m_curB].lineBegin = lineBegin;
    m_preBox[m_curB].lineEnd = lineEnd;
    m_preBox[m_curB].dotBegin = dotBegin;
    m_preBox[m_curB].dotEnd = dotEnd;
    m_preBox[m_curB].LRStatus = m_LRStatus;
    m_preBox[m_curB].UDStatus = m_UDStatus;
    m_preBox[m_curB].point.x = m_pointCfm.x;
    m_preBox[m_curB].point.y = m_pointCfm.y;

    m_curBox.lineBegin = lineBegin;
    m_curBox.lineEnd = lineEnd;
    m_curBox.dotBegin = dotBegin;
    m_curBox.dotEnd = dotEnd;
    m_curBox.LRStatus = m_LRStatus;
    m_curBox.UDStatus = m_UDStatus;
    m_curBox.point.x = m_pointCfm.x;
    m_curBox.point.y = m_pointCfm.y;

    m_drawBox[m_curB] = true;

    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearCfmBox(bool update)
{
    if (m_drawBox[m_curB]){
        CfmBox(m_preBox[m_curB].lineBegin, m_preBox[m_curB].lineEnd, m_preBox[m_curB].dotBegin, m_preBox[m_curB].dotEnd, m_preBox[m_curB].LRStatus, m_preBox[m_curB].UDStatus, m_preBox[m_curB].point, false, m_dashCfmBox);
        m_drawBox[m_curB] = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

//重绘box，冻结时切换不用备份
void ImageAreaDraw::ReDrawCfmBoxForFreeze(bool update)
{
    if (!m_drawBox[m_curB]) {
        m_curBox.LRStatus = m_LRStatus;
        m_curBox.UDStatus = m_UDStatus;
        m_curBox.point.x = m_pointCfm.x;
        m_curBox.point.y = m_pointCfm.y;

        CfmBox(m_curBox.lineBegin, m_curBox.lineEnd, m_curBox.dotBegin, m_curBox.dotEnd, m_curBox.LRStatus, m_curBox.UDStatus, m_curBox.point, true, m_dashCfmBox);

        if (ModeStatus::IsUnFreezeMode())
        {
            m_preBox[m_curB].lineBegin = m_curBox.lineBegin;
            m_preBox[m_curB].lineEnd   = m_curBox.lineEnd;
            m_preBox[m_curB].dotBegin  = m_curBox.dotBegin;
            m_preBox[m_curB].dotEnd    = m_curBox.dotEnd;
            m_preBox[m_curB].LRStatus  = m_curBox.LRStatus;
            m_preBox[m_curB].UDStatus  = m_curBox.UDStatus;
            m_preBox[m_curB].point.x = m_pointCfm.x;
            m_preBox[m_curB].point.y = m_pointCfm.y;
        }
        m_drawBox[m_curB] = true;
    }

    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawCfmBox(bool update)
{
    if (!m_drawBox[m_curB]) {
        m_curBox.LRStatus = m_LRStatus;
        m_curBox.UDStatus = m_UDStatus;
        m_curBox.point.x = m_pointCfm.x;
        m_curBox.point.y = m_pointCfm.y;

        CfmBox(m_curBox.lineBegin, m_curBox.lineEnd, m_curBox.dotBegin, m_curBox.dotEnd, m_curBox.LRStatus, m_curBox.UDStatus, m_curBox.point, true, m_dashCfmBox);
        m_preBox[m_curB].lineBegin = m_curBox.lineBegin;
        m_preBox[m_curB].lineEnd   = m_curBox.lineEnd;
        m_preBox[m_curB].dotBegin  = m_curBox.dotBegin;
        m_preBox[m_curB].dotEnd    = m_curBox.dotEnd;
        m_preBox[m_curB].LRStatus  = m_curBox.LRStatus;
        m_preBox[m_curB].UDStatus  = m_curBox.UDStatus;
        m_preBox[m_curB].point.x = m_pointCfm.x;
        m_preBox[m_curB].point.y = m_pointCfm.y;

        m_drawBox[m_curB] = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

// 当冻结后切换图像然后解冻后box应该用冻结前的最后box  zjx
void ImageAreaDraw::SetCfmBoxFreeze(int preB)
{
    if (m_formatCfm == FormatCfm::B)
        return ;

    ImageArea::RectArea area;
    if (m_formatCfm == FormatCfm::BB) {
            if(preB == 1)
            {
            area.x = IMAGE_W/2;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H;
        } else if (preB == 0) {
            area.x = 0;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H;
        }
    } else if (m_formatCfm == FormatCfm::B4) {
            if(preB == 3){
            area.x = IMAGE_W/2;
            area.y = IMAGE_H/2;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (preB == 0) {
            area.x = 0;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (preB == 1) {
            area.x = IMAGE_W/2;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (preB == 2) {
            area.x = 0;
            area.y = IMAGE_H/2;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        }
    }
    // if (m_drawBox[preB]){
    // draw preCFMBox
    m_ptrImg->DrawCFMPreBox(area);
    // clear SYMBOL surface
    CfmBox(m_preBox[preB].lineBegin, m_preBox[preB].lineEnd, m_preBox[preB].dotBegin, m_preBox[preB].dotEnd, m_preBox[preB].LRStatus, m_preBox[preB].UDStatus, m_preBox[preB].point, false, m_dashCfmBox);
    m_drawBox[preB] = false;
    // }
}

void ImageAreaDraw::SetCfmBoxFreeze(void)
{
    int preB = 0;
    if (m_formatCfm == FormatCfm::B)
        return ;

    ImageArea::RectArea area;
    if (m_formatCfm == FormatCfm::BB) {
        if (m_curB == 0) {
            preB = 1;
            area.x = IMAGE_W/2;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H;
        } else if (m_curB == 1) {
            preB = 0;
            area.x = 0;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H;
        }
    } else if (m_formatCfm == FormatCfm::B4) {
        if (m_curB == 0) {
            preB = 3;
            area.x = IMAGE_W/2;
            area.y = IMAGE_H/2;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (m_curB == 1) {
            preB = 0;
            area.x = 0;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (m_curB == 2) {
            preB = 1;
            area.x = IMAGE_W/2;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (m_curB == 3) {
            preB = 2;
            area.x = 0;
            area.y = IMAGE_H/2;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        }
    }
    // if (m_drawBox[preB]){
    // draw preCFMBox
    m_ptrImg->DrawCFMPreBox(area);
    // clear SYMBOL surface
    CfmBox(m_preBox[preB].lineBegin, m_preBox[preB].lineEnd, m_preBox[preB].dotBegin, m_preBox[preB].dotEnd, m_preBox[preB].LRStatus, m_preBox[preB].UDStatus, m_preBox[preB].point, false, m_dashCfmBox);
    m_drawBox[preB] = false;
    // }
}

void ImageAreaDraw::UnSetCfmBoxFreeze(void)
{
    if (m_formatCfm == FormatCfm::B)
        return;

    ImageArea::RectArea area;
    if (m_formatCfm == FormatCfm::BB) {
        if (m_curB == 0) {
            area.x = 0;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H;
        } else if (m_curB == 1) {
            area.x = IMAGE_W/2;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H;
        }
    } else if (m_formatCfm == FormatCfm::B4) {
        if (m_curB == 0) {
            area.x = 0;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (m_curB == 1) {
            area.x = IMAGE_W/2;
            area.y = 0;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (m_curB == 2) {
            area.x = 0;
            area.y = IMAGE_H/2;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        } else if (m_curB == 3) {
            area.x = IMAGE_W/2;
            area.y = IMAGE_H/2;
            area.w = IMAGE_W/2;
            area.h = IMAGE_H/2;
        }
    }

    // if (m_drawBox[m_curB]){
    // clear preCFMBox
    m_ptrImg->ClearCFMPreBox(area);
    m_drawBox[m_curB] = false;
    // }
}

// other
void ImageAreaDraw::DrawPIPBox(int x, int y, int width, int height, bool update)
{
#ifdef CT_355
    if (m_clear)
        return;
#endif
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = false;

    // clear
    if (m_PIPBoxDraw)
        m_ptrImg->DrawRectangle(attr, g_black, m_PIPBox[0], m_PIPBox[1], m_PIPBox[2], m_PIPBox[3], false, false);
    //draw
    attr.mode.cvDraw = true;
    m_ptrImg->DrawRectangle(attr, g_green, x, y, width, height, false, false);

    m_PIPBoxDraw = TRUE;
    m_PIPBox[0] = x;
    m_PIPBox[1] = y;
    m_PIPBox[2] = width;
    m_PIPBox[3] = height;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearPIPBox(bool update)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = false;

    if (m_PIPBoxDraw) {
        m_ptrImg->DrawRectangle(attr, g_black, m_PIPBox[0], m_PIPBox[1], m_PIPBox[2], m_PIPBox[3], false, false);
        m_PIPBoxDraw = FALSE;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
#ifdef CT_355
    m_clear=true;
#endif
}

void ImageAreaDraw::ReDrawPIPBox(bool update)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = true;

    if (!m_PIPBoxDraw) {
        m_ptrImg->DrawRectangle(attr, g_green, m_PIPBox[0], m_PIPBox[1], m_PIPBox[2], m_PIPBox[3], false, false);
        m_PIPBoxDraw = true;
    }

    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawGrayTransCurve(unsigned char *CurveValue, unsigned char *BasePoint, unsigned char Current, bool update)
{
    int i;
    unsigned char count=0;
    const int maxvalue=128;
    const int dis=16;
    int startX = IMAGE_W - maxvalue - 5;
    int startY = IMAGE_H - 5;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PIXMAP;
    attr.mode.gdkMode = GDK_COPY;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, startX-3, startY-maxvalue-3, maxvalue+10, maxvalue+10, true, false);

    int index(0);
    index = ImgProc2D::GetInstance()->GetGrayTranIndex();
    char buf[16];
    sprintf(buf, "C%d", index);
    int posX = startX -3 + (maxvalue + 10)/2;
    int posY = startY - maxvalue -3;

    GdkGC *gc = m_ptrImg->GetPixmapDC(g_white, GDK_COPY);
    for(i=0; i<maxvalue; i+=((i==0)?(dis*2-1):(dis*2))) {
        m_ptrImg->DrawPixmapLine(gc, startX, startY-i, startX+maxvalue-1, startY-i);
        m_ptrImg->DrawPixmapLine(gc, startX+i, startY, startX+i, startY-maxvalue+1);
    }
    //m_ptrImg->DrawString(gc, posX, posY, "");
    //m_ptrImg->DrawString(gc, posX, posY, buf);

    // Draw Point
    for(i=0; i<=maxvalue/dis; i++) {
        int j = (i==0)?(i*dis):(i*dis-1);
        int m,n;
        if(Current==count) {
            char round[7][7]={{0,0,1,1,1,0,0},
                {0,1,1,1,1,1,0},
                {1,1,1,1,1,1,1},
                {1,1,1,1,1,1,1},
                {1,1,1,1,1,1,1},
                {0,1,1,1,1,1,0},
                {0,0,1,1,1,0,0}};
            for(m=0; m<7; m++)
                for (n=0; n<7; n++) {
                    if(round[m][n]==1)
                        m_ptrImg->DrawPixmapPt(gc, startX+j+m-3, startY-(BasePoint[i]/2)+n-3);
                }
        }
        else {
            char round[5][5]={{0,1,1,1,0},
                {1,1,1,1,1},
                {1,1,1,1,1},
                {1,1,1,1,1},
                {0,1,1,1,0}};
            for(m=0; m<5; m++)
                for (n=0; n<5; n++) {
                    if(round[m][n]==1)
                        m_ptrImg->DrawPixmapPt(gc, startX+j+m-2, startY-(BasePoint[i]/2)+n-2);
                }
        }
        count++;
    }
    for(i=0; i<maxvalue; i++) {
        m_ptrImg->DrawPixmapPt(gc, startX+i, startY-CurveValue[i*2]/2);
        m_ptrImg->DrawPixmapPt(gc, startX+i+1, startY-CurveValue[i*2]/2);
    }
    g_object_unref(gc);
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearGrayCurve(bool update)
{
    const int maxvalue=128;
    int startX=IMAGE_W - maxvalue - 8;
    int startY=IMAGE_H - 8;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PIXMAP;
    attr.mode.gdkMode = GDK_COPY;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, startX-3, startY-maxvalue-3, maxvalue+10, maxvalue+10, true, update);
}

void ImageAreaDraw::DrawGrayRejectCurve(unsigned char Point, bool update)
{
    int i;
    const int maxvalue=128;
    const int dis=32;
    int startX=IMAGE_W - maxvalue - 8;
    int startY=IMAGE_H - 8;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PIXMAP;
    attr.mode.gdkMode = GDK_COPY;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, startX-3, startY-maxvalue-3, maxvalue+6, maxvalue+10, true);

    GdkGC *gc = m_ptrImg->GetPixmapDC(g_white, GDK_COPY);
    for(i=0; i<maxvalue; i+=((i==0)?(dis-1):(dis))) {
        m_ptrImg->DrawPixmapLine(gc, startX, startY-i, startX+maxvalue-1, startY-i);
        m_ptrImg->DrawPixmapLine(gc, startX+i, startY, startX+i, startY-maxvalue+1);
    }

    m_ptrImg->DrawPixmapLine(gc, startX, startY, startX+Point/2, startY);
    m_ptrImg->DrawPixmapLine(gc, startX+Point/2, startY, startX+Point/2, startY-Point/2);
    m_ptrImg->DrawPixmapLine(gc, startX+Point/2, startY-Point/2, startX+127, startY-127);

    m_ptrImg->DrawPixmapLine(gc, startX+Point/2, startY+2, startX+Point/2, startY+6);
    for(i=1; i<4; i++) {
        m_ptrImg->DrawPixmapLine(gc, startX+Point/2+i, startY+2+i, startX+Point/2+i, startY+6);
        m_ptrImg->DrawPixmapLine(gc, startX+Point/2-i, startY+2+i, startX+Point/2-i, startY+6);
    }
    g_object_unref(gc);
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::SetLocalZoomBoxDashStatus(bool isDash)
{
    m_dashLZBox = isDash;
}

void ImageAreaDraw::DrawLocalZoom(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool update)
{
    if (m_LZDraw)
        LocalZoom(m_preLZ.lineBegin, m_preLZ.lineEnd, m_preLZ.dotBegin, m_preLZ.dotEnd, m_preLZ.LRStatus, m_preLZ.UDStatus, m_preLZ.point, false, m_dashLZBox);

    LocalZoom(lineBegin, lineEnd, dotBegin, dotEnd, m_LRStatus, m_UDStatus, m_pointLZ, true, m_dashLZBox);

    m_preLZ.lineBegin = lineBegin;
    m_preLZ.lineEnd = lineEnd;
    m_preLZ.dotBegin = dotBegin;
    m_preLZ.dotEnd = dotEnd;
    m_preLZ.LRStatus = m_LRStatus;
    m_preLZ.UDStatus = m_UDStatus;
    m_preLZ.point.x = m_pointLZ.x;
    m_preLZ.point.y = m_pointLZ.y;

    m_LZDraw = true;

    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearLocalZoom(bool update)
{
    if (m_LZDraw) {
        LocalZoom(m_preLZ.lineBegin, m_preLZ.lineEnd, m_preLZ.dotBegin, m_preLZ.dotEnd, m_preLZ.LRStatus, m_preLZ.UDStatus, m_preLZ.point, false, m_dashLZBox);
        m_LZDraw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawLocalZoom(bool update)
{
    if (!m_LZDraw) {
        m_preLZ.LRStatus = m_LRStatus;
        m_preLZ.UDStatus = m_UDStatus;
        m_preLZ.point.x = m_pointLZ.x;
        m_preLZ.point.y = m_pointLZ.y;
        LocalZoom(m_preLZ.lineBegin, m_preLZ.lineEnd, m_preLZ.dotBegin, m_preLZ.dotEnd, m_preLZ.LRStatus, m_preLZ.UDStatus, m_preLZ.point, true, m_dashLZBox);
        m_LZDraw = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::LocalZoom(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool LRStatus, bool UDStatus, GdkPoint pointLZ, bool draw, bool dash)
{
    // for test
    // m_probeType = 'L';
    // m_pointLZ.x = IMAGE_X + IMAGE_W/2;
    // m_pointLZ.y = IMAGE_Y;
    // m_cfmAngle = -10;

    GdkPoint p1, p2, p3, p4;
    int boxBegin = dotBegin;
    int boxEnd = dotEnd;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = draw;

    if (m_probeType == 'L' || m_probeType == 'l') {
        double delta = (m_probeWidth / (m_probeLines - 1)) / m_ratio2D;
        double xOffsetLineBegin = (lineBegin - m_probeLines/2) * delta;
        double xOffsetLineEnd = (lineEnd - m_probeLines/2) * delta;

        //double angle = (double)m_cfmAngle * PI / 180;
        //double angle = (double)m_steerAngle2D * PI / 180;偏转之后不能进入局部放大
        double angle = 0;
        double xAngleOffset = sin(angle) * (boxEnd - boxBegin);
        double yAngleOffset = cos(angle) * (boxEnd - boxBegin);
        if (UDStatus && !LRStatus) {
            p1.x = pointLZ.x + xOffsetLineBegin;
            p1.y = pointLZ.y - boxBegin;
            p2.x = p1.x + xAngleOffset;
            p2.y = p1.y - yAngleOffset;
            p3.x = pointLZ.x + xOffsetLineEnd;
            p3.y = p1.y;
            p4.x = p3.x + xAngleOffset;
            p4.y = p3.y - yAngleOffset;
        } else if (LRStatus && !UDStatus) {
            p1.x = pointLZ.x - xOffsetLineBegin;
            p1.y = pointLZ.y + boxBegin;
            p2.x = p1.x - xAngleOffset;
            p2.y = p1.y + yAngleOffset;
            p3.x = pointLZ.x - xOffsetLineEnd;
            p3.y = p1.y;
            p4.x = p3.x - xAngleOffset;
            p4.y = p3.y + yAngleOffset;
        } else if (UDStatus && LRStatus) {
            p1.x = pointLZ.x - xOffsetLineBegin;
            p1.y = pointLZ.y - boxBegin;
            p2.x = p1.x - xAngleOffset;
            p2.y = p1.y - yAngleOffset;
            p3.x = pointLZ.x - xOffsetLineEnd;
            p3.y = p1.y;
            p4.x = p3.x - xAngleOffset;
            p4.y = p3.y - yAngleOffset;
        } else {
            p1.x = pointLZ.x + xOffsetLineBegin;
            p1.y = pointLZ.y + boxBegin;
            p2.x = p1.x + xAngleOffset;
            p2.y = p1.y + yAngleOffset;
            p3.x = pointLZ.x + xOffsetLineEnd;
            p3.y = p1.y;
            p4.x = p3.x + xAngleOffset;
            p4.y = p3.y + yAngleOffset;
        }
        if (dash)
        {
            m_ptrImg->DrawDashLine(attr, g_green, p1.x, p1.y, p3.x, p3.y, false, 5);
            m_ptrImg->DrawDashLine(attr, g_green, p2.x, p2.y, p4.x, p4.y, false, 5);
        }
        else
        {
            m_ptrImg->DrawLine(attr, g_green, p1.x, p1.y, p3.x, p3.y, false);
            m_ptrImg->DrawLine(attr, g_green, p2.x, p2.y, p4.x, p4.y, false);
        }
    } else {
        double halfLines = m_probeLines / 2;
        double scanAngle = m_probeWidth * PI / 180;
        double halfAngle = scanAngle / 2;
        double delta = scanAngle / (m_probeLines - 1);

        double theta1 = delta * ((lineBegin < halfLines) ? (halfLines - lineBegin) : (lineBegin - halfLines - 1));
        theta1 = theta1 + delta/2;
        double sineTheta1 = sin(theta1);
        double cosineTheta1 = cos(theta1);

        double theta2 = delta * ((lineEnd < halfLines) ? (halfLines - lineEnd) : (lineEnd - halfLines - 1));
        theta2 = theta2 + delta/2;
        double sineTheta2 = sin(theta2);
        double cosineTheta2 = cos(theta2);

        double probeRadius = m_probeR / m_ratio2D;
        double tmpLen = cos(halfAngle) * probeRadius;

        double xOffsetP1 = sineTheta1 * (probeRadius + boxBegin);
        double yOffsetP1 = cosineTheta1 * (probeRadius + boxBegin) - tmpLen;
        double xOffsetP2 = sineTheta1 * (probeRadius + boxEnd);
        double yOffsetP2 = cosineTheta1 * (probeRadius + boxEnd) - tmpLen;
        double xOffsetP3 = sineTheta2 * (probeRadius + boxBegin);
        double yOffsetP3 = cosineTheta2 * (probeRadius + boxBegin) - tmpLen;
        double xOffsetP4 = sineTheta2 * (probeRadius + boxEnd);
        double yOffsetP4 = cosineTheta2 * (probeRadius + boxEnd) - tmpLen;

        GdkPoint centre;
        GdkPoint arcPoint1, arcPoint2;
        if (UDStatus) {
            centre.x = pointLZ.x;
            centre.y = pointLZ.y + tmpLen;
        } else {
            centre.x = pointLZ.x;
            centre.y = pointLZ.y - tmpLen;
        }
        arcPoint1.x = centre.x - probeRadius - boxBegin;
        arcPoint1.y = centre.y - probeRadius - boxBegin;
        arcPoint2.x = centre.x - probeRadius - boxEnd;
        arcPoint2.y = centre.y - probeRadius - boxEnd;

        int arcWidth1 = 2 * (probeRadius + boxBegin);
        int arcWidth2 = 2 * (probeRadius + boxEnd);
        double angle1;
        double angle2;

        if (UDStatus && !LRStatus) {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 90 + theta2 * 180 / PI;
                angle2 = 90 + theta1 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 90 - theta2 * 180 / PI;
                angle2 = 90 - theta1 * 180 / PI;
            } else {
                angle1 = 90 - theta2 * 180 / PI;
                angle2 = 90 + theta1 * 180 / PI;
            }
        } else if (LRStatus && !UDStatus) {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 270 + theta2 * 180 / PI;
                angle2 = 270 + theta1 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 270 - theta2 * 180 / PI;
                angle2 = 270 - theta1 * 180 / PI;
            } else {
                angle1 = 270 - theta2 * 180 / PI;
                angle2 = 270 + theta1 * 180 / PI;
            }
        } else if (UDStatus && LRStatus) {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 90 - theta1 * 180 / PI;
                angle2 = 90 - theta2 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 90 + theta1 * 180 / PI;
                angle2 = 90 + theta2 * 180 / PI;
            } else {
                angle1 = 90 - theta1 * 180 / PI;
                angle2 = 90 + theta2 * 180 / PI;
            }
        } else {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 270 - theta1 * 180 / PI;
                angle2 = 270 - theta2 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 270 + theta1 * 180 / PI;
                angle2 = 270 + theta2 * 180 / PI;
            } else {
                angle1 = 270 - theta1 * 180 / PI;
                angle2 = 270 + theta2 * 180 / PI;
            }
        }

        if (lineBegin <= halfLines) {
            if (UDStatus && !LRStatus) {
                p1.x = pointLZ.x - xOffsetP1;
                p1.y = pointLZ.y - yOffsetP1;
                p2.x = pointLZ.x - xOffsetP2;
                p2.y = pointLZ.y - yOffsetP2;
            } else if (LRStatus && !UDStatus) {
                p1.x = pointLZ.x + xOffsetP1;
                p1.y = pointLZ.y + yOffsetP1;
                p2.x = pointLZ.x + xOffsetP2;
                p2.y = pointLZ.y + yOffsetP2;
            } else if (UDStatus && LRStatus) {
                p1.x = pointLZ.x + xOffsetP1;
                p1.y = pointLZ.y - yOffsetP1;
                p2.x = pointLZ.x + xOffsetP2;
                p2.y = pointLZ.y - yOffsetP2;
            } else {
                p1.x = pointLZ.x - xOffsetP1;
                p1.y = pointLZ.y + yOffsetP1;
                p2.x = pointLZ.x - xOffsetP2;
                p2.y = pointLZ.y + yOffsetP2;
            }
        } else {
            if (UDStatus && !LRStatus) {
                p1.x = pointLZ.x + xOffsetP1;
                p1.y = pointLZ.y - yOffsetP1;
                p2.x = pointLZ.x + xOffsetP2;
                p2.y = pointLZ.y - yOffsetP2;
            } else if (LRStatus && !UDStatus) {
                p1.x = pointLZ.x - xOffsetP1;
                p1.y = pointLZ.y + yOffsetP1;
                p2.x = pointLZ.x - xOffsetP2;
                p2.y = pointLZ.y + yOffsetP2;
            } else if (UDStatus && LRStatus) {
                p1.x = pointLZ.x - xOffsetP1;
                p1.y = pointLZ.y - yOffsetP1;
                p2.x = pointLZ.x - xOffsetP2;
                p2.y = pointLZ.y - yOffsetP2;
            } else {
                p1.x = pointLZ.x + xOffsetP1;
                p1.y = pointLZ.y + yOffsetP1;
                p2.x = pointLZ.x + xOffsetP2;
                p2.y = pointLZ.y + yOffsetP2;
            }
        }

        if (lineEnd <= halfLines) {
            if (UDStatus && !LRStatus) {
                p3.x = pointLZ.x - xOffsetP3;
                p3.y = pointLZ.y - yOffsetP3;
                p4.x = pointLZ.x - xOffsetP4;
                p4.y = pointLZ.y - yOffsetP4;
            } else if (LRStatus && !UDStatus) {
                p3.x = pointLZ.x + xOffsetP3;
                p3.y = pointLZ.y + yOffsetP3;
                p4.x = pointLZ.x + xOffsetP4;
                p4.y = pointLZ.y + yOffsetP4;
            } else if (UDStatus && LRStatus) {
                p3.x = pointLZ.x + xOffsetP3;
                p3.y = pointLZ.y - yOffsetP3;
                p4.x = pointLZ.x + xOffsetP4;
                p4.y = pointLZ.y - yOffsetP4;
            } else {
                p3.x = pointLZ.x - xOffsetP3;
                p3.y = pointLZ.y + yOffsetP3;
                p4.x = pointLZ.x - xOffsetP4;
                p4.y = pointLZ.y + yOffsetP4;
            }
        } else {
            if (UDStatus && !LRStatus) {
                p3.x = pointLZ.x + xOffsetP3;
                p3.y = pointLZ.y - yOffsetP3;
                p4.x = pointLZ.x + xOffsetP4;
                p4.y = pointLZ.y - yOffsetP4;
            } else if (LRStatus && !UDStatus) {
                p3.x = pointLZ.x - xOffsetP3;
                p3.y = pointLZ.y + yOffsetP3;
                p4.x = pointLZ.x - xOffsetP4;
                p4.y = pointLZ.y + yOffsetP4;
            } else if (UDStatus && LRStatus) {
                p3.x = pointLZ.x - xOffsetP3;
                p3.y = pointLZ.y - yOffsetP3;
                p4.x = pointLZ.x - xOffsetP4;
                p4.y = pointLZ.y - yOffsetP4;
            } else {
                p3.x = pointLZ.x + xOffsetP3;
                p3.y = pointLZ.y + yOffsetP3;
                p4.x = pointLZ.x + xOffsetP4;
                p4.y = pointLZ.y + yOffsetP4;
            }
        }

        if (dash)
        {
            m_ptrImg->DrawDashArc(attr, g_green, arcPoint1.x, arcPoint1.y, arcWidth1, arcWidth1, angle1, angle2, false);
            m_ptrImg->DrawDashArc(attr, g_green, arcPoint2.x, arcPoint2.y, arcWidth2, arcWidth2, angle1, angle2, false);
        }
        else
        {
            m_ptrImg->DrawArc(attr, g_green, arcPoint1.x, arcPoint1.y, arcWidth1, arcWidth1, angle1, angle2, false);
            m_ptrImg->DrawArc(attr, g_green, arcPoint2.x, arcPoint2.y, arcWidth2, arcWidth2, angle1, angle2, false);
        }
    }
    if (dash)
    {
        m_ptrImg->DrawDashLine(attr, g_green, p1.x, p1.y, p2.x, p2.y, false, 5);
        m_ptrImg->DrawDashLine(attr, g_green, p3.x, p3.y, p4.x, p4.y, false, 5);
    }
    else
    {
        m_ptrImg->DrawLine(attr, g_green, p1.x, p1.y, p2.x, p2.y, false);
        m_ptrImg->DrawLine(attr, g_green, p3.x, p3.y, p4.x, p4.y, false);
    }
}

void ImageAreaDraw::DrawMagnifier(int x, int y, bool update)
{
    if (m_magnifierDraw) {
        m_ptrImg->DrawMagnifier(m_magnifier.x, m_magnifier.y);
    }

    m_magnifier.x = x;
    m_magnifier.y = y;
    m_ptrImg->DrawMagnifier(m_magnifier.x, m_magnifier.y);
    m_magnifierDraw = TRUE;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearMagnifier(bool update)
{
    if (m_magnifierDraw) {
        m_ptrImg->DrawMagnifier(m_magnifier.x, m_magnifier.y);
        m_magnifierDraw = FALSE;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ReDrawMagnifier(bool update)
{
    if (!m_magnifierDraw) {
        m_ptrImg->DrawMagnifier(m_magnifier.x, m_magnifier.y);
        m_magnifierDraw = true;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::DrawMoveFlag(int x, int y, bool update)
{
    const int radius = 8;
    GdkColor *color = g_green;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = false;

    if (m_moveFlagDraw) {
        m_ptrImg->DrawCircle(attr, color, m_moveFlag.x+radius, m_moveFlag.y+radius, radius, false, false);
    }

    m_moveFlag.x = x;
    m_moveFlag.y = y;
    attr.mode.cvDraw = true;
    m_ptrImg->DrawCircle(attr, color, x+radius, y+radius, radius, false, false);
    m_moveFlagDraw = true;
    if (update)
        m_ptrImg->UpdateImgArea();
}

void ImageAreaDraw::ClearMoveFlag(bool update)
{
    const int radius = 8;
    GdkColor *color = g_green;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = false;

    if (m_moveFlagDraw) {
        m_ptrImg->DrawCircle(attr, color, m_moveFlag.x+radius, m_moveFlag.y+radius, radius, false, false);
        m_moveFlagDraw = false;
    }
    if (update)
        m_ptrImg->UpdateImgArea();
}

///> private
//M
void ImageAreaDraw::BMLine(int curBMLine, bool LRStatus, bool UDStatus, GdkPoint pointBM, bool draw)
{
    // for test
    // m_pointBM.x = IMAGE_X + IMAGE_W/2;
    // m_pointBM.y = IMAGE_Y;
    // m_widthImgBM = IMAGE_W/2;
    // m_heightImgBM = IMAGE_H/2;
    // m_ratioY *= 2;
    // m_ratioX *= 2;

    int x1, y1, x2, y2;

    if (m_probeType == 'L' || m_probeType == 'l') {
        double delta = (m_probeWidth / (m_probeLines - 1)) / m_ratio2D;
        double centerLine;
        if (m_inLZMode)
            centerLine = (m_LZLineRange[0] + m_LZLineRange[1]) / 2;
        else
            centerLine = m_probeLines/2;
        double xOffset = (curBMLine - centerLine) * delta;
        if (UDStatus && !LRStatus) {
            x1 = pointBM.x + xOffset;
            y1 = pointBM.y;
            x2 = x1;
            y2 = pointBM.y - m_heightImgBM;
        } else if (LRStatus && !UDStatus) {
            x1 = pointBM.x - xOffset;
            y1 = pointBM.y;
            x2 = x1;
            y2 = pointBM.y + m_heightImgBM;
        } else if (UDStatus && LRStatus) {
            x1 = pointBM.x - xOffset;
            y1 = pointBM.y;
            x2 = x1;
            y2 = pointBM.y - m_heightImgBM;
        } else {
            x1 = pointBM.x + xOffset;
            y1 = pointBM.y;
            x2 = x1;
            y2 = pointBM.y + m_heightImgBM;
        }
    } else {
        double scanAngle = m_probeWidth * PI / 180;
        double delta = scanAngle / (m_probeLines - 1);
        double halfLines, halfAngle, theta, probeRadius;
        int totalLines;

        if (m_inLZMode)
        {
            halfLines = (m_LZLineRange[1] + m_LZLineRange[0]) / 2; //一半扫描线的位置。
            totalLines = m_LZLineRange[1] - m_LZLineRange[0] + 1;
            halfAngle = scanAngle / 2 * (totalLines - 1) / (m_probeLines - 1);
            probeRadius =  (m_probeR + m_LZDotBeginDepth) / m_ratio2D;
            PRINTF("lineB = %d, lintE = %d, dotB = %d, dotE = %d, LZ scale = %f\n", m_LZLineRange[0], m_LZLineRange[1], m_LZDotRange[0], m_LZDotRange[1], m_LZScale);
       }
        else
        {
            halfLines = (0 + m_probeLines) / 2 - 1;
            totalLines = m_probeLines;
            halfAngle = scanAngle / 2;
            probeRadius =  m_probeR / m_ratio2D;
        }

        if (totalLines % 2 == 0)
        {
            theta = delta * ((curBMLine < halfLines) ? (halfLines - curBMLine) : (curBMLine - halfLines - 1)); //奇偶问题
            theta = theta + delta/2;
        }
        else
        {
            theta = delta * ((curBMLine < halfLines) ? (halfLines - curBMLine) : (curBMLine - halfLines)); //奇偶问题
        }

        double sineTheta = sin(theta);
        double cosineTheta = cos(theta);
        double tangentTheta = tan(theta);
        double tmpLen = cos(halfAngle) * probeRadius;

        double offset_x1 = sineTheta * probeRadius;
        double offset_y1 = cosineTheta * probeRadius - tmpLen;
        double offset_x2 = sineTheta * (probeRadius + m_heightImgBM);
        double offset_y2 = cosineTheta * (probeRadius + m_heightImgBM) - tmpLen;

        int boundX1, boundX2, boundY, boundDown;
        boundX1 = pointBM.x - m_widthImgBM/2;
        boundX2 = pointBM.x + m_widthImgBM/2;
        boundY = (m_widthImgBM / 2.0) / tangentTheta - tmpLen;

        if (UDStatus)
            boundDown = pointBM.y - m_heightImgBM;
        else
            boundDown = pointBM.y + m_heightImgBM;

        if (curBMLine <=  halfLines) {
            if (UDStatus && !LRStatus) {
                x1 = pointBM.x - offset_x1;
                y1 = pointBM.y - offset_y1;
                x2 = pointBM.x - offset_x2;
                y2 = pointBM.y - offset_y2;
            } else if (LRStatus && !UDStatus) {
                x1 = pointBM.x + offset_x1;
                y1 = pointBM.y + offset_y1;
                x2 = pointBM.x + offset_x2;
                y2 = pointBM.y + offset_y2;
            } else if (UDStatus && LRStatus) {
                x1 = pointBM.x + offset_x1;
                y1 = pointBM.y - offset_y1;
                x2 = pointBM.x + offset_x2;
                y2 = pointBM.y - offset_y2;
            } else {
                x1 = pointBM.x - offset_x1;
                y1 = pointBM.y + offset_y1;
                x2 = pointBM.x - offset_x2;
                y2 = pointBM.y + offset_y2;
            }
        } else {
            if (UDStatus && !LRStatus) {
                x1 = pointBM.x + offset_x1;
                y1 = pointBM.y - offset_y1;
                x2 = pointBM.x + offset_x2;
                y2 = pointBM.y - offset_y2;
            } else if (LRStatus && !UDStatus) {
                x1 = pointBM.x - offset_x1;
                y1 = pointBM.y + offset_y1;
                x2 = pointBM.x - offset_x2;
                y2 = pointBM.y + offset_y2;
            } else if (UDStatus && LRStatus) {
                x1 = pointBM.x - offset_x1;
                y1 = pointBM.y - offset_y1;
                x2 = pointBM.x - offset_x2;
                y2 = pointBM.y - offset_y2;
            } else {
                x1 = pointBM.x + offset_x1;
                y1 = pointBM.y + offset_y1;
                x2 = pointBM.x + offset_x2;
                y2 = pointBM.y + offset_y2;
            }
        }
   PRINTF("**********aaaaaa x1 y1 x2 y2 boundX1 boundX2 UDStatus %d %d %d %d %d %d %d\n",x1,y1,x2,y2,boundX1,boundX2,UDStatus);
/**
 * 为了解决腔体探头下M线在向边界移动的过程中下面一部分没有显示的问题
 * modifier: lihuamei
 * date:2012-09-06
 */
#if 0
        if (x2 < boundX1 && UDStatus) {
            x2 = boundX1;
            y2 = pointBM.y - boundY;
        } else if (x2 < boundX1 && !UDStatus) {
            x2 = boundX1;
            y2 = pointBM.y + boundY;
        }

        if (x2 > boundX2 && UDStatus) {
            x2 = boundX2;
            y2 = pointBM.y - boundY;
        } else if (x2 > boundX2 && !UDStatus) {
            x2 = boundX2;
            y2 = pointBM.y + boundY;
        }

        if (UDStatus && y2 < boundDown)
            y2 = boundDown;

        if (!UDStatus && y2 > boundDown)
            y2 = boundDown;
#endif
   PRINTF("**********bbbbbb x1 y1 x2 y2 boundX1 boundX2 UDStatus %d %d %d %d %d %d %d\n",x1,y1,x2,y2,boundX1,boundX2,UDStatus);

    }

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = draw;

    m_ptrImg->DrawDashLine(attr, g_green, x1, y1, x2, y2, false);
}

void ImageAreaDraw::SampleVolumn(int curPwLine, int dotBegin, int dotEnd, vector<int> HPRFEmitPos, bool LRStatus, bool UDStatus, GdkPoint pointPw, bool isCw, bool draw)
{
    // for test
    // m_probeType = 'L';
    // m_pointPw.x = IMAGE_X + IMAGE_W/2;
    // m_pointPw.y = IMAGE_Y;
    // m_heightImgPw = IMAGE_H;
    // m_widthImgPw = IMAGE_W;
    // m_zoomRatioPw = 1.0;
    // m_correctAngle = 60;

    // 假设扫描线数为偶数
    const int svWidth = 20; // 取样容积线宽度
    const int angleLen = 30;    // 角度线的长度

    GdkPoint point1, point2, point3, point4; // 1为取样容积线起点,2为终点,3为容积起点,4为终点
    GdkSegment beginSeg;
    GdkSegment endSeg;
    GdkSegment lineUpSeg;
    GdkSegment lineDownSeg;

    vector<GdkPoint> HPRFEmitPoint;
    vector<int>::iterator iter;

    PRINTF("%s: curPwLine = %d, dotBegin = %d, dotEnd = %d\n", __FUNCTION__, curPwLine, dotBegin, dotEnd);
    int svBegin;
    int svEnd;
    if (m_inLZMode)
    {
        svBegin = (dotBegin - m_LZDotRange[0]) * m_LZScale * m_zoomRatioPw;
        svEnd = (dotEnd - m_LZDotRange[0]) * m_LZScale * m_zoomRatioPw;
        PRINTF("svBegin = %d, svEnd = %d\n", svBegin, svEnd);
    }
    else
    {
        svBegin = dotBegin * m_zoomRatioPw;
        svEnd = dotEnd * m_zoomRatioPw;
    }
    double angle = (double)m_correctAngle * PI / 180;

    if (m_probeType == 'L' || m_probeType == 'l') {
        double probeWidth = m_probeWidth / m_ratio2D;
        double delta = probeWidth / (m_probeLines - 1);
        double centerLine;
        if (m_inLZMode)
            centerLine = (m_LZLineRange[0] + m_LZLineRange[1]) /2;
        else
            centerLine = m_probeLines/2;
        double xOffset = (curPwLine - centerLine) * delta;

        double svHeight = svEnd - svBegin;
        // double xOffsetLineOn = tan(angle) * (svHeight/2);

        // double xOffsetLineOff = sin(angle) * angleLen/2;
        // double yOffsetLineOff = cos(angle) * angleLen/2;

        double steerAngle = (double)m_steerAngle * PI / 180;
        double sineSteer = sin(steerAngle);
        double cosineSteer = cos(steerAngle);
        double tangentSteer = tan(steerAngle);
        double xSteerOffset[2], ySteerOffset[2];
        xSteerOffset[0] = sineSteer * svBegin;
        ySteerOffset[0] = cosineSteer * svBegin;
        xSteerOffset[1] = sineSteer * svEnd;
        ySteerOffset[1] = cosineSteer * svEnd;
        double p2SteerOffset = tangentSteer * m_heightImgPw;

        double xOffsetSeg = cosineSteer * (svWidth / 2);
        double yOffsetSeg = sineSteer * (svWidth / 2);

        double tempLenth = tan(angle) * (svHeight/2);
        double xOffsetLineOn = cosineSteer * tempLenth;
        double yOffsetLineOn = sineSteer * tempLenth;

        double tmpAngle = PI/2 + steerAngle - angle;
        double xOffsetLineOff = cos(tmpAngle) * (angleLen / 2);
        double yOffsetLineOff = sin(tmpAngle) * (angleLen / 2);

        if (UDStatus && !LRStatus) {
            point1.x = pointPw.x + xOffset;
            point1.y = pointPw.y;
            point2.x = point1.x + p2SteerOffset;
            point2.y = pointPw.y - m_heightImgPw;
            point3.x = point1.x + xSteerOffset[0];
            point3.y = point1.y - ySteerOffset[0];
            point4.x = point1.x + xSteerOffset[1];
            point4.y = point1.y - ySteerOffset[1];

            beginSeg.x1 = point3.x - xOffsetSeg;
            beginSeg.y1 = point3.y - yOffsetSeg;
            beginSeg.x2 = point3.x + xOffsetSeg;
            beginSeg.y2 = point3.y + yOffsetSeg;
            endSeg.x1 = point4.x - xOffsetSeg;
            endSeg.y1 = point4.y - yOffsetSeg;
            endSeg.x2 = point4.x + xOffsetSeg;
            endSeg.y2 = point4.y + yOffsetSeg;

            lineUpSeg.x1 = point3.x + xOffsetLineOn;
            lineUpSeg.y1 = point3.y + yOffsetLineOn;
            lineUpSeg.x2 = lineUpSeg.x1 + xOffsetLineOff;
            lineUpSeg.y2 = lineUpSeg.y1 + yOffsetLineOff;

            lineDownSeg.x1 = point4.x - xOffsetLineOn;
            lineDownSeg.y1 = point4.y - yOffsetLineOn;
            lineDownSeg.x2 = lineDownSeg.x1 - xOffsetLineOff;
            lineDownSeg.y2 = lineDownSeg.y1 - yOffsetLineOff;

            if (!isCw) {
                for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                    GdkPoint point;
                    point.x = point1.x;
                    point.y = point1.y - (*iter) * m_zoomRatioPw;
                    HPRFEmitPoint.push_back(point);
                }
            }
        } else if (LRStatus && !UDStatus) {
            point1.x = pointPw.x - xOffset;
            point1.y = pointPw.y;
            point2.x = point1.x - p2SteerOffset;
            point2.y = pointPw.y + m_heightImgPw;
            point3.x = point1.x - xSteerOffset[0];
            point3.y = point1.y + ySteerOffset[0];
            point4.x = point1.x - xSteerOffset[1];
            point4.y = point1.y + ySteerOffset[1];

            beginSeg.x1 = point3.x + xOffsetSeg;
            beginSeg.y1 = point3.y + yOffsetSeg;
            beginSeg.x2 = point3.x - xOffsetSeg;
            beginSeg.y2 = point3.y - yOffsetSeg;
            endSeg.x1 = point4.x + xOffsetSeg;
            endSeg.y1 = point4.y + yOffsetSeg;
            endSeg.x2 = point4.x - xOffsetSeg;
            endSeg.y2 = point4.y - yOffsetSeg;

            lineUpSeg.x1 = point3.x - xOffsetLineOn;
            lineUpSeg.y1 = point3.y - yOffsetLineOn;
            lineUpSeg.x2 = lineUpSeg.x1 - xOffsetLineOff;
            lineUpSeg.y2 = lineUpSeg.y1 - yOffsetLineOff;

            lineDownSeg.x1 = point4.x + xOffsetLineOn;
            lineDownSeg.y1 = point4.y + yOffsetLineOn;
            lineDownSeg.x2 = lineDownSeg.x1 + xOffsetLineOff;
            lineDownSeg.y2 = lineDownSeg.y1 + yOffsetLineOff;

            if (!isCw) {
                for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                    GdkPoint point;
                    point.x = point1.x;
                    point.y = point1.y + (*iter) * m_zoomRatioPw;
                    HPRFEmitPoint.push_back(point);
                }
            }
        } else if (UDStatus && LRStatus) {
            point1.x = pointPw.x - xOffset;
            point1.y = pointPw.y;
            point2.x = point1.x - p2SteerOffset;
            point2.y = pointPw.y - m_heightImgPw;
            point3.x = point1.x - xSteerOffset[0];
            point3.y = point1.y - ySteerOffset[0];
            point4.x = point1.x - xSteerOffset[1];
            point4.y = point1.y - ySteerOffset[1];

            beginSeg.x1 = point3.x + xOffsetSeg;
            beginSeg.y1 = point3.y - yOffsetSeg;
            beginSeg.x2 = point3.x - xOffsetSeg;
            beginSeg.y2 = point3.y + yOffsetSeg;
            endSeg.x1 = point4.x + xOffsetSeg;
            endSeg.y1 = point4.y - yOffsetSeg;
            endSeg.x2 = point4.x - xOffsetSeg;
            endSeg.y2 = point4.y + yOffsetSeg;

            lineUpSeg.x1 = point3.x - xOffsetLineOn;
            lineUpSeg.y1 = point3.y + yOffsetLineOn;
            lineUpSeg.x2 = lineUpSeg.x1 - xOffsetLineOff;
            lineUpSeg.y2 = lineUpSeg.y1 + yOffsetLineOff;

            lineDownSeg.x1 = point4.x + xOffsetLineOn;
            lineDownSeg.y1 = point4.y - yOffsetLineOn;
            lineDownSeg.x2 = lineDownSeg.x1 + xOffsetLineOff;
            lineDownSeg.y2 = lineDownSeg.y1 - yOffsetLineOff;

            if (!isCw) {
                for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                    GdkPoint point;
                    point.x = point1.x;
                    point.y = point1.y - (*iter) * m_zoomRatioPw;
                    HPRFEmitPoint.push_back(point);
                }
            }

        } else {
            point1.x = pointPw.x + xOffset;
            point1.y = pointPw.y;
            point2.x = point1.x + p2SteerOffset;
            point2.y = pointPw.y + m_heightImgPw;
            point3.x = point1.x + xSteerOffset[0];
            point3.y = point1.y + ySteerOffset[0];
            point4.x = point1.x + xSteerOffset[1];
            point4.y = point1.y + ySteerOffset[1];

            beginSeg.x1 = point3.x - xOffsetSeg;
            beginSeg.y1 = point3.y + yOffsetSeg;
            beginSeg.x2 = point3.x + xOffsetSeg;
            beginSeg.y2 = point3.y - yOffsetSeg;
            endSeg.x1 = point4.x - xOffsetSeg;
            endSeg.y1 = point4.y + yOffsetSeg;
            endSeg.x2 = point4.x + xOffsetSeg;
            endSeg.y2 = point4.y - yOffsetSeg;

            lineUpSeg.x1 = point3.x + xOffsetLineOn;
            lineUpSeg.y1 = point3.y - yOffsetLineOn;
            lineUpSeg.x2 = lineUpSeg.x1 + xOffsetLineOff;
            lineUpSeg.y2 = lineUpSeg.y1 - yOffsetLineOff;
            lineDownSeg.x1 = point4.x - xOffsetLineOn;
            lineDownSeg.y1 = point4.y + yOffsetLineOn;
            lineDownSeg.x2 = lineDownSeg.x1 - xOffsetLineOff;
            lineDownSeg.y2 = lineDownSeg.y1 + yOffsetLineOff;

            if (!isCw) {
                for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                    GdkPoint point;
                    point.x = point1.x;
                    point.y = point1.y + (*iter) * m_zoomRatioPw;
                    HPRFEmitPoint.push_back(point);
                }
            }
        }
    } else {
        double halfLines, halfAngle, theta, probeRadius;
        double scanAngle = m_probeWidth * PI / 180;
        double delta = scanAngle / (m_probeLines - 1);
        int totalLines;

        if (m_inLZMode)
        {
            halfLines = (m_LZLineRange[1] + m_LZLineRange[0]) / 2 - 1;
            totalLines = m_LZLineRange[1] - m_LZLineRange[0] + 1;
            halfAngle = scanAngle / 2 * (totalLines - 1) / (m_probeLines -1);
            probeRadius = (m_probeR + m_LZDotBeginDepth) / m_ratio2D;
        }
        else
        {
            halfLines = m_probeLines / 2;
            totalLines = m_probeLines;
            halfAngle = scanAngle / 2;
            probeRadius =  m_probeR / m_ratio2D;
        }

        if (totalLines % 2 == 0)
        {
            theta = delta * ((curPwLine < halfLines) ? (halfLines - curPwLine) : (curPwLine - halfLines - 1));
            theta = theta + delta/2;
        }
        else
        {
            theta = delta * ((curPwLine < halfLines) ? (halfLines - curPwLine) : (curPwLine - halfLines - 1));
        }

        double sineTheta = sin(theta);
        double cosineTheta = cos(theta);
        double tangentTheta = tan(theta);

        double tmpLen = cos(halfAngle) * probeRadius;

        double offset_x1 = sineTheta * probeRadius;
        double offset_y1 = cosineTheta * probeRadius - tmpLen;
        double offset_x2 = sineTheta * (probeRadius + m_heightImgPw);
        double offset_y2 = cosineTheta * (probeRadius + m_heightImgPw) - tmpLen;

        double offset_x3 = sineTheta * (probeRadius + svBegin);
        double offset_y3 = cosineTheta * (probeRadius + svBegin) - tmpLen;
        double offset_x4 = sineTheta * (probeRadius + svEnd);
        double offset_y4 = cosineTheta * (probeRadius + svEnd) - tmpLen;
        PRINTF("offset x1=%.2f, y1=%.2f, x2=%.2f, y2=%.2f, x3=%.2f, y3=%.2f, x4=%.2f, y4=%.2f\n",
                offset_x1, offset_y1, offset_x2, offset_y2, offset_x3, offset_y3, offset_x4, offset_y4);

        double xOffsetSeg = cosineTheta * (svWidth / 2);
        double yOffsetSeg = sineTheta * (svWidth / 2);

        double svHeight = svEnd - svBegin;
        // double xOffsetCenter = sineTheta * (svHeight/2 + svBegin);
        // double yOffsetCenter = cosineTheta * (svHeight/2 + svBegin);

        double tempLenth = tan(angle) * (svHeight/2);
        double xOffsetLineOn = cosineTheta * tempLenth;
        double yOffsetLineOn = sineTheta * tempLenth;

        double tmpAngle = PI/2 - theta - angle;
        double xOffsetLineOff = cos(tmpAngle) * (angleLen / 2);
        double yOffsetLineOff = sin(tmpAngle) * (angleLen / 2);

        double tmpAngle2 = angle - theta;
        double x_OffsetLineOff = sin(tmpAngle2) * (angleLen / 2);
        double y_OffsetLineOff = cos(tmpAngle2) * (angleLen / 2);

        int boundX1, boundX2, boundY, boundDown;
        boundX1 = pointPw.x - m_widthImgPw/2;
        boundX2 = pointPw.x + m_widthImgPw/2;
        boundY = (m_widthImgPw / 2.0) / tangentTheta - tmpLen;

        if (UDStatus)
            boundDown = pointPw.y - m_heightImgPw;
        else
            boundDown = pointPw.y + m_heightImgPw;

        if (curPwLine <=  halfLines) {
            if (UDStatus && !LRStatus) {
                point1.x = pointPw.x - offset_x1;
                point1.y = pointPw.y - offset_y1;
                point2.x = pointPw.x - offset_x2;
                point2.y = pointPw.y - offset_y2;
                point3.x = pointPw.x - offset_x3;
                point3.y = pointPw.y - offset_y3;
                point4.x = pointPw.x - offset_x4;
                point4.y = pointPw.y - offset_y4;

                beginSeg.x1 = point3.x + xOffsetSeg;
                beginSeg.y1 = point3.y - yOffsetSeg;
                beginSeg.x2 = point3.x - xOffsetSeg;
                beginSeg.y2 = point3.y + yOffsetSeg;
                endSeg.x1 = point4.x + xOffsetSeg;
                endSeg.y1 = point4.y - yOffsetSeg;
                endSeg.x2 = point4.x - xOffsetSeg;
                endSeg.y2 = point4.y + yOffsetSeg;

                // pCenter.x = point1.x - xOffsetCenter;
                // pCenter.y = point1.y - yOffsetCenter;

                lineUpSeg.x1 = point3.x + xOffsetLineOn;
                lineUpSeg.y1 = point3.y - yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 + xOffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 + yOffsetLineOff;

                lineDownSeg.x1 = point4.x - xOffsetLineOn;
                lineDownSeg.y1 = point4.y + yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 - xOffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 - yOffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x - (sineTheta * radius);
                        point.y = pointPw.y - (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }

            } else if (LRStatus && !UDStatus) {
                point1.x = pointPw.x + offset_x1;
                point1.y = pointPw.y + offset_y1;
                point2.x = pointPw.x + offset_x2;
                point2.y = pointPw.y + offset_y2;
                point3.x = pointPw.x + offset_x3;
                point3.y = pointPw.y + offset_y3;
                point4.x = pointPw.x + offset_x4;
                point4.y = pointPw.y + offset_y4;

                beginSeg.x1 = point3.x - xOffsetSeg;
                beginSeg.y1 = point3.y + yOffsetSeg;
                beginSeg.x2 = point3.x + xOffsetSeg;
                beginSeg.y2 = point3.y - yOffsetSeg;
                endSeg.x1 = point4.x - xOffsetSeg;
                endSeg.y1 = point4.y + yOffsetSeg;
                endSeg.x2 = point4.x + xOffsetSeg;
                endSeg.y2 = point4.y - yOffsetSeg;

                // pCenter.x = point1.x + xOffsetCenter;
                // pCenter.y = point1.y + yOffsetCenter;

                lineUpSeg.x1 = point3.x - xOffsetLineOn;
                lineUpSeg.y1 = point3.y + yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 - xOffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 - yOffsetLineOff;

                lineDownSeg.x1 = point4.x + xOffsetLineOn;
                lineDownSeg.y1 = point4.y - yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 + xOffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 + yOffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x + (sineTheta * radius);
                        point.y = pointPw.y + (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }

            } else if (UDStatus && LRStatus) {
                point1.x = pointPw.x + offset_x1;
                point1.y = pointPw.y - offset_y1;
                point2.x = pointPw.x + offset_x2;
                point2.y = pointPw.y - offset_y2;
                point3.x = pointPw.x + offset_x3;
                point3.y = pointPw.y - offset_y3;
                point4.x = pointPw.x + offset_x4;
                point4.y = pointPw.y - offset_y4;

                beginSeg.x1 = point3.x - xOffsetSeg;
                beginSeg.y1 = point3.y - yOffsetSeg;
                beginSeg.x2 = point3.x + xOffsetSeg;
                beginSeg.y2 = point3.y + yOffsetSeg;
                endSeg.x1 = point4.x - xOffsetSeg;
                endSeg.y1 = point4.y - yOffsetSeg;
                endSeg.x2 = point4.x + xOffsetSeg;
                endSeg.y2 = point4.y + yOffsetSeg;

                // pCenter.x = point1.x + xOffsetCenter;
                // pCenter.y = point1.y - yOffsetCenter;

                lineUpSeg.x1 = point3.x - xOffsetLineOn;
                lineUpSeg.y1 = point3.y - yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 - xOffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 + yOffsetLineOff;

                lineDownSeg.x1 = point4.x + xOffsetLineOn;
                lineDownSeg.y1 = point4.y + yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 + xOffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 - yOffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x + (sineTheta * radius);
                        point.y = pointPw.y - (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }

            } else {
                point1.x = pointPw.x - offset_x1;
                point1.y = pointPw.y + offset_y1;
                point2.x = pointPw.x - offset_x2;
                point2.y = pointPw.y + offset_y2;
                point3.x = pointPw.x - offset_x3;
                point3.y = pointPw.y + offset_y3;
                point4.x = pointPw.x - offset_x4;
                point4.y = pointPw.y + offset_y4;

                beginSeg.x1 = point3.x + xOffsetSeg;
                beginSeg.y1 = point3.y + yOffsetSeg;
                beginSeg.x2 = point3.x - xOffsetSeg;
                beginSeg.y2 = point3.y - yOffsetSeg;
                endSeg.x1 = point4.x + xOffsetSeg;
                endSeg.y1 = point4.y + yOffsetSeg;
                endSeg.x2 = point4.x - xOffsetSeg;
                endSeg.y2 = point4.y - yOffsetSeg;

                // pCenter.x = point1.x - xOffsetCenter;
                // pCenter.y = point1.y + yOffsetCenter;

                lineUpSeg.x1 = point3.x + xOffsetLineOn;
                lineUpSeg.y1 = point3.y + yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 + xOffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 - yOffsetLineOff;

                lineDownSeg.x1 = point4.x - xOffsetLineOn;
                lineDownSeg.y1 = point4.y - yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 - xOffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 + yOffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x - (sineTheta * radius);
                        point.y = pointPw.y + (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }
            }
        } else {
            if (UDStatus && !LRStatus) {
                point1.x = pointPw.x + offset_x1;
                point1.y = pointPw.y - offset_y1;
                point2.x = pointPw.x + offset_x2;
                point2.y = pointPw.y - offset_y2;
                point3.x = pointPw.x + offset_x3;
                point3.y = pointPw.y - offset_y3;
                point4.x = pointPw.x + offset_x4;
                point4.y = pointPw.y - offset_y4;

                beginSeg.x1 = point3.x - xOffsetSeg;
                beginSeg.y1 = point3.y - yOffsetSeg;
                beginSeg.x2 = point3.x + xOffsetSeg;
                beginSeg.y2 = point3.y + yOffsetSeg;
                endSeg.x1 = point4.x - xOffsetSeg;
                endSeg.y1 = point4.y - yOffsetSeg;
                endSeg.x2 = point4.x + xOffsetSeg;
                endSeg.y2 = point4.y + yOffsetSeg;

                // pCenter.x = point1.x + xOffsetCenter;
                // pCenter.y = point1.y - yOffsetCenter;

                lineUpSeg.x1 = point3.x + xOffsetLineOn;
                lineUpSeg.y1 = point3.y + yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 + x_OffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 + y_OffsetLineOff;

                lineDownSeg.x1 = point4.x - xOffsetLineOn;
                lineDownSeg.y1 = point4.y - yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 - x_OffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 - y_OffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x + (sineTheta * radius);
                        point.y = pointPw.y - (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }

            } else if (LRStatus && !UDStatus) {
                point1.x = pointPw.x - offset_x1;
                point1.y = pointPw.y + offset_y1;
                point2.x = pointPw.x - offset_x2;
                point2.y = pointPw.y + offset_y2;
                point3.x = pointPw.x - offset_x3;
                point3.y = pointPw.y + offset_y3;
                point4.x = pointPw.x - offset_x4;
                point4.y = pointPw.y + offset_y4;

                beginSeg.x1 = point3.x + xOffsetSeg;
                beginSeg.y1 = point3.y + yOffsetSeg;
                beginSeg.x2 = point3.x - xOffsetSeg;
                beginSeg.y2 = point3.y - yOffsetSeg;
                endSeg.x1 = point4.x + xOffsetSeg;
                endSeg.y1 = point4.y + yOffsetSeg;
                endSeg.x2 = point4.x - xOffsetSeg;
                endSeg.y2 = point4.y - yOffsetSeg;

                // pCenter.x = point1.x - xOffsetCenter;
                // pCenter.y = point1.y + yOffsetCenter;

                lineUpSeg.x1 = point3.x - xOffsetLineOn;
                lineUpSeg.y1 = point3.y - yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 - x_OffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 - y_OffsetLineOff;

                lineDownSeg.x1 = point4.x + xOffsetLineOn;
                lineDownSeg.y1 = point4.y + yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 + x_OffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 + y_OffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x - (sineTheta * radius);
                        point.y = pointPw.y + (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }

            } else if (UDStatus && LRStatus) {
                point1.x = pointPw.x - offset_x1;
                point1.y = pointPw.y - offset_y1;
                point2.x = pointPw.x - offset_x2;
                point2.y = pointPw.y - offset_y2;
                point3.x = pointPw.x - offset_x3;
                point3.y = pointPw.y - offset_y3;
                point4.x = pointPw.x - offset_x4;
                point4.y = pointPw.y - offset_y4;

                beginSeg.x1 = point3.x + xOffsetSeg;
                beginSeg.y1 = point3.y - yOffsetSeg;
                beginSeg.x2 = point3.x - xOffsetSeg;
                beginSeg.y2 = point3.y + yOffsetSeg;
                endSeg.x1 = point4.x + xOffsetSeg;
                endSeg.y1 = point4.y - yOffsetSeg;
                endSeg.x2 = point4.x - xOffsetSeg;
                endSeg.y2 = point4.y + yOffsetSeg;

                // pCenter.x = point1.x - xOffsetCenter;
                // pCenter.y = point1.y - yOffsetCenter;

                lineUpSeg.x1 = point3.x - xOffsetLineOn;
                lineUpSeg.y1 = point3.y + yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 - x_OffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 + y_OffsetLineOff;

                lineDownSeg.x1 = point4.x + xOffsetLineOn;
                lineDownSeg.y1 = point4.y - yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 + x_OffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 - y_OffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x - (sineTheta * radius);
                        point.y = pointPw.y - (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }

            } else {
                point1.x = pointPw.x + offset_x1;
                point1.y = pointPw.y + offset_y1;
                point2.x = pointPw.x + offset_x2;
                point2.y = pointPw.y + offset_y2;
                point3.x = pointPw.x + offset_x3;
                point3.y = pointPw.y + offset_y3;
                point4.x = pointPw.x + offset_x4;
                point4.y = pointPw.y + offset_y4;

                beginSeg.x1 = point3.x - xOffsetSeg;
                beginSeg.y1 = point3.y + yOffsetSeg;
                beginSeg.x2 = point3.x + xOffsetSeg;
                beginSeg.y2 = point3.y - yOffsetSeg;
                endSeg.x1 = point4.x - xOffsetSeg;
                endSeg.y1 = point4.y + yOffsetSeg;
                endSeg.x2 = point4.x + xOffsetSeg;
                endSeg.y2 = point4.y - yOffsetSeg;

                // pCenter.x = point1.x + xOffsetCenter;
                // pCenter.y = point1.y + yOffsetCenter;

                lineUpSeg.x1 = point3.x + xOffsetLineOn;
                lineUpSeg.y1 = point3.y - yOffsetLineOn;
                lineUpSeg.x2 = lineUpSeg.x1 + x_OffsetLineOff;
                lineUpSeg.y2 = lineUpSeg.y1 - y_OffsetLineOff;

                lineDownSeg.x1 = point4.x - xOffsetLineOn;
                lineDownSeg.y1 = point4.y + yOffsetLineOn;
                lineDownSeg.x2 = lineDownSeg.x1 - x_OffsetLineOff;
                lineDownSeg.y2 = lineDownSeg.y1 + y_OffsetLineOff;

                if (!isCw) {
                    for (iter = HPRFEmitPos.begin(); iter != HPRFEmitPos.end(); iter++) {
                        GdkPoint point;
                        double radius = (*iter) * m_zoomRatioPw + probeRadius;
                        point.x = pointPw.x + (sineTheta * radius);
                        point.y = pointPw.y + (cosineTheta * radius - tmpLen);
                        HPRFEmitPoint.push_back(point);
                    }
                }
            }
        }

        if (point2.x < boundX1 && UDStatus) {
            point2.x = boundX1;
            point2.y = pointPw.y - boundY;
        } else if (point2.x < boundX1 && !UDStatus) {
            point2.x = boundX1;
            point2.y = pointPw.y + boundY;
        }

        if (point2.x > boundX2 && UDStatus) {
            point2.x = boundX2;
            point2.y = pointPw.y - boundY;
        } else if (point2.x > boundX2 && !UDStatus) {
            point2.x = boundX2;
            point2.y = pointPw.y + boundY;
        }

        if (UDStatus && point2.y < boundDown)
            point2.y = boundDown;

        if (!UDStatus && point2.y > boundDown)
            point2.y = boundDown;

    }
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = draw;

    m_ptrImg->DrawDashLine(attr, g_green, point1.x, point1.y, point3.x, point3.y, false);
    m_ptrImg->DrawDashLine(attr, g_green, point4.x, point4.y, point2.x, point2.y, false);

    //     m_ptrImg->DrawSegments(GDK_XOR, g_green, &beginSeg, 1);
    // //   if (! ((beginSeg.x1 == endSeg.x1) && (beginSeg.y1 == endSeg.y1)))
    //     m_ptrImg->DrawSegments(GDK_XOR, g_green, &endSeg, 1);

    //     m_ptrImg->DrawSegments(GDK_XOR, g_green, &lineUpSeg, 1);
    //     m_ptrImg->DrawSegments(GDK_XOR, g_green, &lineDownSeg, 1);

    m_ptrImg->DrawLine(attr, g_green, beginSeg.x1, beginSeg.y1, beginSeg.x2, beginSeg.y2, false);
    m_ptrImg->DrawLine(attr, g_green, endSeg.x1, endSeg.y1, endSeg.x2, endSeg.y2, false);
    m_ptrImg->DrawLine(attr, g_green, lineUpSeg.x1, lineUpSeg.y1, lineUpSeg.x2, lineUpSeg.y2, false);
    m_ptrImg->DrawLine(attr, g_green, lineDownSeg.x1, lineDownSeg.y1, lineDownSeg.x2, lineDownSeg.y2, false);

    if (!isCw) {
        vector<GdkPoint>::iterator iter_point;
        for (iter_point = HPRFEmitPoint.begin(); iter_point != HPRFEmitPoint.end(); iter_point++) {
            m_ptrImg->DrawCircle(attr, g_green, (*iter_point).x, (*iter_point).y, 5, true, false);
        }
    }
}

void ImageAreaDraw::SetSteerAngle(int angle)
{
    m_steerAngle = angle;
}

void ImageAreaDraw::Set2DSteerAngle(int angle)
{
    m_steerAngle2D = angle;
}

void ImageAreaDraw::CfmBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool LRStatus, bool UDStatus, GdkPoint pointCfm, bool draw, bool dash)
{
    //printf("lineEnd = %d, lineBegin = %d, dotBegin = %d, dotEnd = %d\n", lineEnd, lineBegin, dotBegin, dotEnd);
    PRINTF("probeLines:%d\n", m_probeLines);
#ifdef CT_430
    if ((lineBegin == 8) && (lineBegin > 0))
        lineBegin--;
#endif
    GdkColor *color;
    GdkPoint p1, p2, p3, p4;
    int boxBegin;
    int boxEnd;

    if(m_inLZMode)
    {
        boxBegin = (dotBegin - m_LZDotRange[0]) * m_LZScale * m_zoomRatioCfm;
        boxEnd = (dotEnd - m_LZDotRange[0]) * m_LZScale * m_zoomRatioCfm;
        PRINTF("boxBegin = %d, boxEnd = %d\n", boxBegin, boxEnd);
    }
    else
    {
        boxBegin = dotBegin * m_zoomRatioCfm;
        boxEnd = dotEnd * m_zoomRatioCfm;
    }

    ImageArea::DrawAttr attr;
    color = g_green;
    attr.area = ImageArea::SYMBOL;
    attr.mode.cvDraw = draw;

    if (m_probeType == 'L' || m_probeType == 'l') {
        double delta = (m_probeWidth / (m_probeLines - 1)) / m_ratio2D;
        double centerLine;
        if (m_inLZMode)
            centerLine = (m_LZLineRange[0] + m_LZLineRange[1]) / 2;
        else
            centerLine = m_probeLines/2;

        double xOffsetLineBegin = (lineBegin - centerLine) * delta;
        double xOffsetLineEnd = (lineEnd - centerLine) * delta;

        double angle = (double)m_steerAngle * PI / 180;
        double sineAngle = sin(angle);
        double cosineAngle = cos(angle);
        double xAngleOffset[2], yAngleOffset[2];
        xAngleOffset[0] = sineAngle * boxBegin;
        yAngleOffset[0] = cosineAngle * boxBegin;
        xAngleOffset[1] = sineAngle * boxEnd;
        yAngleOffset[1] = cosineAngle * boxEnd;

        if (UDStatus && !LRStatus) {
            p1.x = pointCfm.x + xOffsetLineBegin + xAngleOffset[0];
            p1.y = pointCfm.y - yAngleOffset[0];
            p2.x = pointCfm.x + xOffsetLineBegin + xAngleOffset[1];
            p2.y = pointCfm.y - yAngleOffset[1];
            p3.x = pointCfm.x + xOffsetLineEnd + xAngleOffset[0];
            p3.y = p1.y;
            p4.x = pointCfm.x + xOffsetLineEnd + xAngleOffset[1];
            p4.y = p2.y;
        } else if (LRStatus && !UDStatus) {
            p1.x = pointCfm.x - xOffsetLineBegin - xAngleOffset[0];
            p1.y = pointCfm.y + yAngleOffset[0];
            p2.x = pointCfm.x - xOffsetLineBegin - xAngleOffset[1];
            p2.y = pointCfm.y + yAngleOffset[1];
            p3.x = pointCfm.x - xOffsetLineEnd - xAngleOffset[0];
            p3.y = p1.y;
            p4.x = pointCfm.x - xOffsetLineEnd - xAngleOffset[1];
            p4.y = p2.y;
        } else if (UDStatus && LRStatus) {
            p1.x = pointCfm.x - xOffsetLineBegin - xAngleOffset[0];
            p1.y = pointCfm.y - yAngleOffset[0];
            p2.x = pointCfm.x - xOffsetLineBegin - xAngleOffset[1];
            p2.y = pointCfm.y - yAngleOffset[1];
            p3.x = pointCfm.x - xOffsetLineEnd - xAngleOffset[0];
            p3.y = p1.y;
            p4.x = pointCfm.x - xOffsetLineEnd - xAngleOffset[1];
            p4.y = p2.y;
        } else {
            p1.x = pointCfm.x + xOffsetLineBegin + xAngleOffset[0];
            p1.y = pointCfm.y + yAngleOffset[0];
            p2.x = pointCfm.x + xOffsetLineBegin + xAngleOffset[1];
            p2.y = pointCfm.y + yAngleOffset[1];
            p3.x = pointCfm.x + xOffsetLineEnd + xAngleOffset[0];
            p3.y = p1.y;
            p4.x = pointCfm.x + xOffsetLineEnd + xAngleOffset[1];
            p4.y = p2.y;
        }

        if (dash)
        {
            m_ptrImg->DrawDashLine(attr, color, p1.x, p1.y, p3.x, p3.y, false, 5);
            m_ptrImg->DrawDashLine(attr, color, p2.x, p2.y, p4.x, p4.y, false, 5);
        }
        else
        {
            m_ptrImg->DrawLine(attr, color, p1.x, p1.y, p3.x, p3.y, false);
            m_ptrImg->DrawLine(attr, color, p2.x, p2.y, p4.x, p4.y, false);
        }
    } else {
        double halfLines, halfAngle, theta1, theta2, probeRadius;
        double scanAngle = m_probeWidth * PI / 180;
        double delta = scanAngle / (m_probeLines - 1);
        int totalLines;

        if (m_inLZMode)
        {
            halfLines = (m_LZLineRange[1] + m_LZLineRange[0]) / 2 - 1;
            totalLines = m_LZLineRange[1] - m_LZLineRange[0] + 1;
            halfAngle = scanAngle / 2 * (totalLines - 1) / (m_probeLines - 1);
            probeRadius =  (m_probeR + m_LZDotBeginDepth) / m_ratio2D;
        }
        else
        {
            halfLines = m_probeLines / 2 - 1;
            totalLines = m_probeLines;
            halfAngle = scanAngle / 2;
            probeRadius = m_probeR / m_ratio2D;
        }

        if (totalLines % 2 == 0)
        {
            theta1 = delta * ((lineBegin < halfLines) ? (halfLines - lineBegin) : (lineBegin - halfLines - 1));
            theta2 = delta * ((lineEnd < halfLines) ? (halfLines - lineEnd) : (lineEnd - halfLines - 1));
            theta1 = theta1 + delta/2;
            theta2 = theta2 + delta/2;
        }
        else
        {
            theta1 = delta * ((lineBegin < halfLines) ? (halfLines - lineBegin) : (lineBegin - halfLines));
            theta2 = delta * ((lineEnd < halfLines) ? (halfLines - lineEnd) : (lineEnd - halfLines));
        }

        double sineTheta1 = sin(theta1);
        double cosineTheta1 = cos(theta1);
        double sineTheta2 = sin(theta2);
        double cosineTheta2 = cos(theta2);

        double tmpLen = cos(halfAngle) * probeRadius;
        PRINTF("delta=%.2f, theta1=%.2f, sin1=%.2f, cos1=%.2f, halfAngle=%.2f\n", delta, theta1, sineTheta1, cosineTheta1, halfAngle);

        double xOffsetP1 = sineTheta1 * (probeRadius + boxBegin);
        double yOffsetP1 = cosineTheta1 * (probeRadius + boxBegin) - tmpLen;
        double xOffsetP2 = sineTheta1 * (probeRadius + boxEnd);
        double yOffsetP2 = cosineTheta1 * (probeRadius + boxEnd) - tmpLen;
        double xOffsetP3 = sineTheta2 * (probeRadius + boxBegin);
        double yOffsetP3 = cosineTheta2 * (probeRadius + boxBegin) - tmpLen;
        double xOffsetP4 = sineTheta2 * (probeRadius + boxEnd);
        double yOffsetP4 = cosineTheta2 * (probeRadius + boxEnd) - tmpLen;
        PRINTF("yOffsetP1 = %.2f, cosineTheta1 = %.2f, probeRadius = %.2f, tmpLen = %.2f\n", yOffsetP1, cosineTheta1, probeRadius, tmpLen);

        GdkPoint centre;
        GdkPoint arcPoint1, arcPoint2;
        if (UDStatus) {
            centre.x = pointCfm.x;
            centre.y = pointCfm.y + tmpLen;
        } else {
            centre.x = pointCfm.x;
            centre.y = pointCfm.y - tmpLen;
        }
        arcPoint1.x = centre.x - probeRadius - boxBegin;
        arcPoint1.y = centre.y - probeRadius - boxBegin;
        arcPoint2.x = centre.x - probeRadius - boxEnd;
        arcPoint2.y = centre.y - probeRadius - boxEnd;

        int arcWidth1 = 2 * (probeRadius + boxBegin);
        int arcWidth2 = 2 * (probeRadius + boxEnd);
        double angle1;
        double angle2;

        if (UDStatus && !LRStatus) {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 90 + theta2 * 180 / PI;
                angle2 = 90 + theta1 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 90 - theta2 * 180 / PI;
                angle2 = 90 - theta1 * 180 / PI;
            } else {
                angle1 = 90 - theta2 * 180 / PI;
                angle2 = 90 + theta1 * 180 / PI;
            }
        } else if (LRStatus && !UDStatus) {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 270 + theta2 * 180 / PI;
                angle2 = 270 + theta1 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 270 - theta2 * 180 / PI;
                angle2 = 270 - theta1 * 180 / PI;
            } else {
                angle1 = 270 - theta2 * 180 / PI;
                angle2 = 270 + theta1 * 180 / PI;
            }
        } else if (UDStatus && LRStatus) {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 90 - theta1 * 180 / PI;
                angle2 = 90 - theta2 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 90 + theta1 * 180 / PI;
                angle2 = 90 + theta2 * 180 / PI;
            } else {
                angle1 = 90 - theta1 * 180 / PI;
                angle2 = 90 + theta2 * 180 / PI;
            }

        } else {
            if (lineBegin < halfLines && lineEnd < halfLines) {
                angle1 = 270 - theta1 * 180 / PI;
                angle2 = 270 - theta2 * 180 / PI;
            } else if (lineBegin > halfLines && lineEnd > halfLines) {
                angle1 = 270 + theta1 * 180 / PI;
                angle2 = 270 + theta2 * 180 / PI;
            } else {
                angle1 = 270 - theta1 * 180 / PI;
                angle2 = 270 + theta2 * 180 / PI;
            }
        }

        if (lineBegin <= halfLines) {
            if (UDStatus && !LRStatus) {
                p1.x = pointCfm.x - xOffsetP1;
                p1.y = pointCfm.y - yOffsetP1;
                p2.x = pointCfm.x - xOffsetP2;
                p2.y = pointCfm.y - yOffsetP2;
            } else if (LRStatus && !UDStatus) {
                p1.x = pointCfm.x + xOffsetP1;
                p1.y = pointCfm.y + yOffsetP1;
                p2.x = pointCfm.x + xOffsetP2;
                p2.y = pointCfm.y + yOffsetP2;
            } else if (UDStatus && LRStatus) {
                p1.x = pointCfm.x + xOffsetP1;
                p1.y = pointCfm.y - yOffsetP1;
                p2.x = pointCfm.x + xOffsetP2;
                p2.y = pointCfm.y - yOffsetP2;
            } else {
                p1.x = pointCfm.x - xOffsetP1;
                p1.y = pointCfm.y + yOffsetP1;
                p2.x = pointCfm.x - xOffsetP2;
                p2.y = pointCfm.y + yOffsetP2;
            }
        } else {
            if (UDStatus && !LRStatus) {
                p1.x = pointCfm.x + xOffsetP1;
                p1.y = pointCfm.y - yOffsetP1;
                p2.x = pointCfm.x + xOffsetP2;
                p2.y = pointCfm.y - yOffsetP2;
            } else if (LRStatus && !UDStatus) {
                p1.x = pointCfm.x - xOffsetP1;
                p1.y = pointCfm.y + yOffsetP1;
                p2.x = pointCfm.x - xOffsetP2;
                p2.y = pointCfm.y + yOffsetP2;
            } else if (UDStatus && LRStatus) {
                p1.x = pointCfm.x - xOffsetP1;
                p1.y = pointCfm.y - yOffsetP1;
                p2.x = pointCfm.x - xOffsetP2;
                p2.y = pointCfm.y - yOffsetP2;
            } else {
                p1.x = pointCfm.x + xOffsetP1;
                p1.y = pointCfm.y + yOffsetP1;
                p2.x = pointCfm.x + xOffsetP2;
                p2.y = pointCfm.y + yOffsetP2;
            }
        }

        if (lineEnd <= halfLines) {
            if (UDStatus && !LRStatus) {
                p3.x = pointCfm.x - xOffsetP3;
                p3.y = pointCfm.y - yOffsetP3;
                p4.x = pointCfm.x - xOffsetP4;
                p4.y = pointCfm.y - yOffsetP4;
            } else if (LRStatus && !UDStatus) {
                p3.x = pointCfm.x + xOffsetP3;
                p3.y = pointCfm.y + yOffsetP3;
                p4.x = pointCfm.x + xOffsetP4;
                p4.y = pointCfm.y + yOffsetP4;
            } else if (UDStatus && LRStatus) {
                p3.x = pointCfm.x + xOffsetP3;
                p3.y = pointCfm.y - yOffsetP3;
                p4.x = pointCfm.x + xOffsetP4;
                p4.y = pointCfm.y - yOffsetP4;
            } else {
                p3.x = pointCfm.x - xOffsetP3;
                p3.y = pointCfm.y + yOffsetP3;
                p4.x = pointCfm.x - xOffsetP4;
                p4.y = pointCfm.y + yOffsetP4;
            }
        } else {
            if (UDStatus && !LRStatus) {
                p3.x = pointCfm.x + xOffsetP3;
                p3.y = pointCfm.y - yOffsetP3;
                p4.x = pointCfm.x + xOffsetP4;
                p4.y = pointCfm.y - yOffsetP4;
            } else if (LRStatus && !UDStatus) {
                p3.x = pointCfm.x - xOffsetP3;
                p3.y = pointCfm.y + yOffsetP3;
                p4.x = pointCfm.x - xOffsetP4;
                p4.y = pointCfm.y + yOffsetP4;
            } else if (UDStatus && LRStatus) {
                p3.x = pointCfm.x - xOffsetP3;
                p3.y = pointCfm.y - yOffsetP3;
                p4.x = pointCfm.x - xOffsetP4;
                p4.y = pointCfm.y - yOffsetP4;
            } else {
                p3.x = pointCfm.x + xOffsetP3;
                p3.y = pointCfm.y + yOffsetP3;
                p4.x = pointCfm.x + xOffsetP4;
                p4.y = pointCfm.y + yOffsetP4;
            }
        }

        if (dash)
        {
            m_ptrImg->DrawDashArc(attr, color, arcPoint1.x, arcPoint1.y, arcWidth1, arcWidth1, angle1, angle2, false);
            m_ptrImg->DrawDashArc(attr, color, arcPoint2.x, arcPoint2.y, arcWidth2, arcWidth2, angle1, angle2, false);
        }
        else
        {
            //m_ptrImg->DrawArc(attr, color, arcPoint1.x, arcPoint1.y, arcWidth1, arcWidth1, angle1, angle2, false);
            //m_ptrImg->DrawArc(attr, color, arcPoint2.x, arcPoint2.y, arcWidth2, arcWidth2, angle1, angle2, false);

            m_ptrImg->DrawDashArc(attr, color, arcPoint1.x, arcPoint1.y, arcWidth1, arcWidth1, angle1, angle2, false, 1, 0.9);
            m_ptrImg->DrawDashArc(attr, color, arcPoint2.x, arcPoint2.y, arcWidth2, arcWidth2, angle1, angle2, false, 1, 0.9);
        }
    }

    if (dash)
    {
        m_ptrImg->DrawDashLine(attr, color, p1.x, p1.y, p2.x, p2.y, false, 5);
        m_ptrImg->DrawDashLine(attr, color, p3.x, p3.y, p4.x, p4.y, false, 5);
    }
    else
    {
        m_ptrImg->DrawLine(attr, color, p1.x, p1.y, p2.x, p2.y, false);
        m_ptrImg->DrawLine(attr, color, p3.x, p3.y, p4.x, p4.y, false);
    }
}

void ImageAreaDraw::DrawEFOVZoomMiniatureBox(int start_x,  int start_y, int end_x, int end_y)
{
    if (m_drawEFOVZoomMiniature)
        EFOVZoomMiniatureBox(m_EFOVZoomMiniatureBox[0].x, m_EFOVZoomMiniatureBox[0].y,
                m_EFOVZoomMiniatureBox[1].x, m_EFOVZoomMiniatureBox[1].y, false);
    EFOVZoomMiniatureBox(start_x, start_y, end_x, end_y, true);
    m_EFOVZoomMiniatureBox[0].x = start_x;
    m_EFOVZoomMiniatureBox[0].y = start_y;
    m_EFOVZoomMiniatureBox[1].x = end_x;
    m_EFOVZoomMiniatureBox[1].y = end_y;
    m_drawEFOVZoomMiniature = true;
}

void ImageAreaDraw::ReDrawEFOVZoomMiniatureBox(void)
{
    if (!m_drawEFOVZoomMiniature)
        EFOVZoomMiniatureBox(m_EFOVZoomMiniatureBox[0].x, m_EFOVZoomMiniatureBox[0].y,
                m_EFOVZoomMiniatureBox[1].x, m_EFOVZoomMiniatureBox[1].y, true);
    m_drawEFOVZoomMiniature = true;
}

void ImageAreaDraw::ClearEFOVZoomMiniatureBox(void)
{
    if (m_drawEFOVZoomMiniature)
        EFOVZoomMiniatureBox(m_EFOVZoomMiniatureBox[0].x, m_EFOVZoomMiniatureBox[0].y,
                m_EFOVZoomMiniatureBox[1].x, m_EFOVZoomMiniatureBox[1].y, false);
    m_drawEFOVZoomMiniature = false;
}

void ImageAreaDraw::EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y, bool draw)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = draw;

    if (end_x-start_x == 0 && end_y-start_y == 0)
        return;

    m_ptrImg->DrawRectangle(attr, g_yellow, start_x, start_y, end_x-start_x, end_y-start_y, false, false);

    // GdkGC *gc = gdk_gc_new(m_pixmapEFOV);
    // gdk_gc_set_foreground(gc, g_green);
    // gdk_gc_set_function(gc, GDK_XOR);
    // gdk_draw_rectangle(m_pixmapEFOV, gc, false, start_x, start_y, end_x-start_x, end_y-start_y);
    // g_object_unref(gc);
}

void ImageAreaDraw::DrawEFOVZoomBox(int start_x,  int start_y, int end_x, int end_y)
{
    if (m_drawEFOVZoom)
        EFOVZoomBox(m_EFOVZoomBox[0].x, m_EFOVZoomBox[0].y, m_EFOVZoomBox[1].x, m_EFOVZoomBox[1].y, false);
    EFOVZoomBox(start_x, start_y, end_x, end_y, true);
    m_EFOVZoomBox[0].x = start_x;
    m_EFOVZoomBox[0].y = start_y;
    m_EFOVZoomBox[1].x = end_x;
    m_EFOVZoomBox[1].y = end_y;
    m_drawEFOVZoom = true;
}

void ImageAreaDraw::ReDrawEFOVZoomBox(void)
{
    if (!m_drawEFOVZoom)
        EFOVZoomBox(m_EFOVZoomBox[0].x, m_EFOVZoomBox[0].y, m_EFOVZoomBox[1].x, m_EFOVZoomBox[1].y, true);
    m_drawEFOVZoom = true;
}

void ImageAreaDraw::ClearEFOVZoomBox(void)
{
    if (m_drawEFOVZoom)
        EFOVZoomBox(m_EFOVZoomBox[0].x, m_EFOVZoomBox[0].y, m_EFOVZoomBox[1].x, m_EFOVZoomBox[1].y, false);
    m_drawEFOVZoom = false;
}

void ImageAreaDraw::EFOVZoomBox(int start_x,  int start_y, int end_x, int end_y, bool draw)
{
    GdkColor *color = g_green;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = draw;

    int width = end_x - start_x;
    int height = end_y - start_y;
    if (width <= 0 || height <= 0)
        return ;
    const int length = 5;

    // left up corner
    m_ptrImg->DrawLine(attr, color, start_x, start_y, start_x + length, start_y, false);
    m_ptrImg->DrawLine(attr, color, start_x, start_y, start_x, start_y + length, false);

    // left down corner
    m_ptrImg->DrawLine(attr, color, start_x, end_y, start_x + length, end_y, false);
    m_ptrImg->DrawLine(attr, color, start_x, end_y, start_x, end_y - length, false);

    // right up corner
    m_ptrImg->DrawLine(attr, color, end_x, start_y, end_x - length, start_y, false);
    m_ptrImg->DrawLine(attr, color, end_x, start_y, end_x, start_y + length, false);

    // right down corner
    m_ptrImg->DrawLine(attr, color, end_x, end_y, end_x - length, end_y, false);
    m_ptrImg->DrawLine(attr, color, end_x, end_y, end_x, end_y - length, false);
}

void ImageAreaDraw::DrawEFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right)
{
    if (m_drawEFOVView)
        EFOVViewBox(m_EFOVViewBox[0], m_EFOVViewBox[1], m_EFOVViewBox[2], m_EFOVViewBox[3], false);
    EFOVViewBox(up_left, down_left, up_right, down_right, true);
    m_EFOVViewBox[0].x = up_left.x;
    m_EFOVViewBox[0].y = up_left.y;
    m_EFOVViewBox[1].x = down_left.x;
    m_EFOVViewBox[1].y = down_left.y;
    m_EFOVViewBox[2].x = up_right.x;
    m_EFOVViewBox[2].y = up_right.y;
    m_EFOVViewBox[3].x = down_right.x;
    m_EFOVViewBox[3].y = down_right.y;
    m_drawEFOVView = true;
}

void ImageAreaDraw::ReDrawEFOVViewBox(void)
{
    if (!m_drawEFOVView)
        EFOVViewBox(m_EFOVViewBox[0], m_EFOVViewBox[1], m_EFOVViewBox[2], m_EFOVViewBox[3], true);
    m_drawEFOVView = true;
}

void ImageAreaDraw::ClearEFOVViewBox(void)
{
    if (m_drawEFOVView)
        EFOVViewBox(m_EFOVViewBox[0], m_EFOVViewBox[1], m_EFOVViewBox[2], m_EFOVViewBox[3], false);
    m_drawEFOVView = false;
}

void ImageAreaDraw::EFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right, bool draw)
{

    if (up_left.x == 0 && up_left.y == 0 && down_right.x == 0 && down_right.y == 0)
        return ;

    GdkColor *color = g_green;
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = draw;

    m_ptrImg->DrawLine(attr, color, up_left.x, up_left.y, down_left.x, down_left.y, false);
    m_ptrImg->DrawLine(attr, color, up_left.x, up_left.y, up_right.x, up_right.y, false);
    m_ptrImg->DrawLine(attr, color, down_left.x, down_left.y, down_right.x, down_right.y, false);
    m_ptrImg->DrawLine(attr, color, up_right.x, up_right.y, down_right.x, down_right.y, false);
}

void ImageAreaDraw::EFOVScaleY(int scale_x, int scale_y, int height, double ratio)
{
    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    m_ptrImg->DrawRectangle(attr, g_black, scale_x, scale_y-1, SCALE_W+2, height/ratio+2, true, false);

    GdkColor *color = g_white;
    int x = scale_x;
    for(int i=0; i<height; i+=50) {
        int y = scale_y + (int)(i/ratio);
        if (y < 0 || y > IMG_AREA_H)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y-1, x+SCALE_W, y-1, false);
        m_ptrImg->DrawLine(attr, color, x, y,   x+SCALE_W, y,   false);
        m_ptrImg->DrawLine(attr, color, x, y+1, x+SCALE_W, y+1, false);
    }
    for(int i=0; i<height; i+=10) {
        int y = scale_y + (int)(i/ratio);
        if (y < 0 || y > IMG_AREA_H)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y, x+SCALE_W/2, y, false);
    }

    // GdkGC *gc = gdk_gc_new(m_pixmapEFOV);
    // gdk_gc_set_function(gc, GDK_COPY);
    // gdk_gc_set_foreground(gc, g_black);
    // gdk_draw_rectangle(m_pixmapEFOV, gc, true, IMAGE_X + IMAGE_W + 80, IMAGE_Y-1, SCALE_W+2, IMAGE_H+2);
    // gdk_gc_set_foreground(gc, g_white);
    // int x = scale_x;
    // for(int i=0; i<height; i+=50) {
    //     int y = scale_y + (int)(i/ratio);
    //     if (y < 0 || y > IMG_AREA_H)
    //         continue;
    //     gdk_draw_line(m_pixmapEFOV, gc, x, y-1, x+SCALE_W, y-1);
    //     gdk_draw_line(m_pixmapEFOV, gc, x, y,   x+SCALE_W, y);
    //     gdk_draw_line(m_pixmapEFOV, gc, x, y+1, x+SCALE_W, y+1);
    // }
    // for(int i=0; i<height; i+=10) {
    //     int y = scale_y + (int)(i/ratio);
    //     if (y < 0 || y > IMG_AREA_H)
    //         continue;
    //     gdk_draw_line(m_pixmapEFOV, gc, x, y, x+SCALE_W/2, y);
    // }
    // g_object_unref(gc);
}

void ImageAreaDraw::EFOVScaleX(int scale_x, int scale_y, int width, double ratio)
{
    GdkColor *color = g_white;

    ImageArea::DrawAttr attr;
    attr.area = ImageArea::PARA;
    attr.mode.cvDraw = true;

    // clean
    m_ptrImg->DrawRectangle(attr, g_black, scale_x-1, scale_y, width/ratio, SCALE_H+2, true, false);

    int y = scale_y + SCALE_H;
    for (int i=0; i<width; i+=50) {
        int x = (int)(i/ratio) + scale_x;
        if (x < 0 || x > IMG_AREA_W)
            continue;
        m_ptrImg->DrawLine(attr, color, x-1, y-SCALE_H, x-1, y, false);
        m_ptrImg->DrawLine(attr, color, x,   y-SCALE_H, x,   y, false);
        m_ptrImg->DrawLine(attr, color, x+1, y-SCALE_H, x+1, y, false);
    }
    for (float i=0; i<width; i+=10) {
        int x = (int)(i/ratio) + scale_x;
        if (x < 0 || x > IMG_AREA_W)
            continue;
        m_ptrImg->DrawLine(attr, color, x, y-SCALE_H/2, x, y, false);
    }
}

void ImageAreaDraw::DrawEFOV2DScaleY(bool prepare_mode)
{
    int scale_x, scale_y;
    if (prepare_mode) {
        scale_x = 580;
        scale_y = 0;
    } else {
        scale_x = 780;
        scale_y = 60;
    }
    const double ratio = Img2D::GetInstance()->GetScale2D();
    const int height = IMAGE_H * ratio;
    EFOVScaleY(scale_x, scale_y, height, ratio);

    // const int scale_x = IMAGE_X + IMAGE_W + 80;
    // const int scale_y = 0;
    // const int height = IMG_AREA_H;
    // const double ratio = Img2D::GetInstance()->GetScale2D();
    // EFOVScaleY(scale_x, scale_y, height, ratio);
}

void ImageAreaDraw::DrawEFOVScaleY(void)
{
    const int scale_x = IMAGE_X + IMAGE_W + 50;
    const int scale_y = IMAGE_Y;
    const double ratio = Img2D::GetInstance()->GetScaleEFOV();
    const int height = (IMAGE_H + 50)* ratio;
    //printf("scale y ratio = %.2f\n", ratio);
    EFOVScaleY(scale_x, scale_y, height, ratio);

    // const int scale_x = IMAGE_X + IMAGE_W + 80;
    // const int scale_y = 0;
    // const int height = IMG_AREA_H;
    // const double ratio = Img2D::GetInstance()->GetScaleEFOV();

    // EFOVScaleY(scale_x, scale_y, height, ratio);
}

void ImageAreaDraw::DrawEFOVScaleX(void)
{
    const int scale_x = IMAGE_X - 50;
    const int scale_y = IMAGE_Y + IMAGE_H + 50;
    const double ratio = Img2D::GetInstance()->GetScaleEFOV();
    const int width = (IMAGE_W + 100) * ratio;
    //printf("scale x ratio = %.2f\n", ratio);

    EFOVScaleX(scale_x, scale_y, width, ratio);

    // const int scale_x = IMAGE_X + IMAGE_W + 80;
    // const int scale_y = 0;
    // const int width = IMAGE_W;
    // const double ratio = Img2D::GetInstance()->GetScaleEFOV();

    // GdkGC *gc = gdk_gc_new(m_pixmapEFOV);
    // gdk_gc_set_function(gc, GDK_COPY);
    // gdk_gc_set_foreground(gc, g_black);
    // gdk_draw_rectangle(m_pixmapEFOV, gc, true, IMAGE_X-1, IMAGE_Y+IMAGE_H, IMAGE_W+2+80, SCALE_W+2);
    // gdk_gc_set_foreground(gc, g_white);
    // int y = scale_y;
    // for(int i=0; i<width; i+=50) {
    //     int x = scale_x - (int)(i/ratio);
    //     if (x < 0 || x > IMAGE_X + IMAGE_W + 80)
    //         continue;
    //     gdk_draw_line(m_pixmapEFOV, gc, x-1, y, x-1, y+SCALE_W);
    //     gdk_draw_line(m_pixmapEFOV, gc, x,   y, x,   y+SCALE_W);
    //     gdk_draw_line(m_pixmapEFOV, gc, x+1, y, x+1, y+SCALE_W);
    // }
    // for(int i=0; i<width; i+=10) {
    //     int x = scale_x - (int)(i/ratio);
    //     if (x < 0 || x > IMAGE_X + IMAGE_W + 100)
    //         continue;
    //     gdk_draw_line(m_pixmapEFOV, gc, x, y, x, y+SCALE_W/2);
    // }
    // g_object_unref(gc);
}

#ifdef CT_355
void ImageAreaDraw::SetClearStatus(bool on)
{
    m_clear=on;
}
#endif