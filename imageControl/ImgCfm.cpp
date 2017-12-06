#include "imageControl/ImgCfm.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/GlobalClassMan.h"
#include "display/HintArea.h"
#include "keyboard/MultiFuncMode.h"
#include "imageProc/Zoom.h"

ImgCfm* ImgCfm::m_ptrInstance = NULL;
FpgaCfm ImgCfm::m_fpga;
const int ImgCfm::CFM_PRF[MAX_PRF_INDEX] = {350, 500, 750, 1000, 1500, 2000, 2500, 3000, 3500, 5000, 6000, 7000, 9000};
const short ImgCfm::WALL_FILTER[WALL_FILTER_SIZE] =
{
#include "res/filter/Cfm/MITCoef.h"
};

const int ImgCfm::WALL_FILTER_SELECT_IDNEX[MAX_PKTSIZE_INDEX][8] =
{
    {0, 3, 6, 8, 10, 11, 12, 13},
    {0, 2, 5, 7, 12, 13, 14, 15},
    {0, 3, 6, 9, 12, 13, 14, 15},
    {0, 2, 5, 8, 11, 12, 13, 14},
    {0, 3, 7, 9, 11, 12, 13, 14},
    {0, 3, 7, 9, 11, 12, 13, 14},
    {0, 4, 6, 8, 12, 13, 14, 15},
    {1, 5, 8, 11, 13, 14, 15, 4},
    {1, 4, 7, 9, 12, 13, 14, 15},
    {2, 5, 8, 12, 13, 14, 15, 4},
};
const float ImgCfm::WALLFILTER_DISPLAY[MAX_PKTSIZE_INDEX][MAX_WALLFILTER_INDEX_ALL] =
{
    // pkt=8 index=0
    {0.0178, 0.0334, 0.0623, 0.0750, 0.0834, 0.1000, 0.1331, 0.1486, 0.1649, 0.1734, 0.1923, 0.2000, 0.2190, 0.2210, 0.2289, 0.2350},
    // pkt=10 index=1
    {0.0175, 0.0359, 0.0534, 0.0700, 0.0752, 0.0799, 0.0850, 0.0932, 0.0100, 0.1500, 0.1532, 0.1568, 0.1600, 0.1935, 0.2000, 0.2050},
    // pkt=12 index=2
    {0.0150, 0.0368, 0.0529, 0.0689, 0.0911, 0.1071, 0.1197, 0.1291, 0.1433, 0.1567, 0.1644, 0.1793, 0.1831, 0.1913, 0.2012, 0.2076},
    // pkt=14 index=3
    {0.0156, 0.0350, 0.0627, 0.0700, 0.0814, 0.0956, 0.1001, 0.1235, 0.1396, 0.1562, 0.1600, 0.1641, 0.1784, 0.1822, 0.1856, 0.1968},
    // pkt=16 index=4
    {0.0150, 0.0283, 0.0543, 0.0603, 0.0687, 0.0785, 0.0832, 0.1065, 0.1123, 0.1302, 0.1459, 0.1637, 0.1984, 0.2387, 0.2597, 0.2752},
    // pkt=18 index=5
    {0.0172, 0.0237, 0.0603, 0.0687, 0.0785, 0.0832, 0.0973, 0.1065, 0.1302, 0.1468, 0.1637, 0.1846, 0.2012, 0.2160, 0.2297, 0.2552},
    // pkt=20 index=6
    {0.0173, 0.0250, 0.0543, 0.0603, 0.0687, 0.0785, 0.0832, 0.1048, 0.1165, 0.1246, 0.1302, 0.1478, 0.1637, 0.1763, 0.1912, 0.2104},
    // pkt=22 index=7
    {0.0194, 0.0276, 0.0543, 0.0603, 0.0687, 0.0785, 0.0832, 0.1048, 0.1165, 0.1246, 0.1302, 0.1478, 0.1637, 0.1763, 0.1854, 0.2138},
    // pkt=24 index=8
    {0.0173, 0.0264, 0.0543, 0.0603, 0.0785, 0.0911, 0.1048, 0.1134, 0.1246, 0.1359, 0.1494, 0.1562, 0.1637, 0.1763, 0.1883, 0.2024},
    // pkt=26 index=9
    {0.0173, 0.0264, 0.0689, 0.0785, 0.0881, 0.0912, 0.0962, 0.1048, 0.1088, 0.1123, 0.1163, 0.1291, 0.1457, 0.1683, 0.1793, 0.2024}
};

const int ImgCfm::PACKSIZE[MAX_PKTSIZE_INDEX] = {8, 10, 12, 14, 16, 18, 20, 22, 24, 26};
const string ImgCfm::LINE_DENSITY_DISPLAY[MAX_LINE_DENSITY] =
{
    N_("Low"), N_("High")
};

const int ImgCfm::FLOW_OPT[MAX_FLOW_OPT][5] =
{
    // tissue noise rate variance gain
    {500,  0, 0, 16320+15, 0},
    {1000, 0, 0, 16326+15, 0},
    {1500, 0, 0, 16332+15, 0},
    {3000, 0, 0, 16336+15, 0},
    {4000, 0, 0, 16342+15, 0},
    {5000, 0, 0, 16350+15, 0},
    {6000, 0, 0, 16355+15, 0},
    {8000, 0, 0, 16357+15, 0}
};

const int ImgCfm::FLOW_OPT_PDI[MAX_FLOW_OPT][5] =
{
    // tissue noise rate variance gain
    {1000, 0, 0, 16326+15, 0},
    {1500, 0, 0, 16332+15, 0},
    {3000, 0, 0, 16336+15, 0},
    {4000, 0, 0, 16342+15, 0},
    {5000, 0, 0, 16350+15, 0},
    {6000, 0, 0, 16355+15, 0},
    {8000, 0, 0, 16357+15, 0},
    {8000, 0, 0, 16357+15, 0}
};

const float ImgCfm::CFM_FOC_RATIO[MAX_CFM_FOC_RATIO] = {0.25, 0.5, 0.75};

const int ImgCfm::COLOR_MAP_CFM[MAX_COLOR_MAP_CFM] =
{
    COLORMAP_CFM_ASIA0, COLORMAP_CFM_ASIA1, COLORMAP_CFM_ASIA2, COLORMAP_CFM_ASIA3, COLORMAP_CFM_ASIA4, COLORMAP_CFM_ASIA5
};

const int ImgCfm::COLOR_MAP_PDI[MAX_COLOR_MAP_PDI] =
{
    COLORMAP_PDI_ASIA0, COLORMAP_PDI_ASIA1, COLORMAP_PDI_ASIA2, COLORMAP_PDI_ASIA3, COLORMAP_PDI_ASIA4, COLORMAP_PDI_ASIA5
};

const int ImgCfm::COLOR_MAP_PDIDIR[MAX_COLOR_MAP_PDIDIR] =
{
    COLORMAP_CFM_ASIA0, COLORMAP_CFM_ASIA1, COLORMAP_CFM_ASIA2, COLORMAP_CFM_ASIA3, COLORMAP_CFM_ASIA4, COLORMAP_CFM_ASIA5
};
const float ImgCfm::CFM_FOCUS[3] = {20.0, 80.0, 150.0}; //mm

ImgCfm* ImgCfm::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ImgCfm;

    return m_ptrInstance;
}

ImgCfm::ImgCfm()
{
    GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
    m_ptrUpdate = ptrGcm->GetUpdateCfm();
    m_ptrUpdatePw = ptrGcm->GetUpdatePw();
    m_ptrUpdate2d = ptrGcm->GetUpdate2D();

    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)
        m_ptrDscPara = ptrDscMan->GetDscPara();

    m_ptrDsc = NULL;
    m_ptrImg2D = Img2D::GetInstance();
    m_ptrImgPw = ImgPw::GetInstance();
    m_ptrCalc = NULL;

    m_ptrCalc2d = NULL;

    m_ctGainCfm.Begin();
    m_tGainCfm = 0;
    m_scale2DBefore = 0.0;

    m_gainCfm = 100;
    m_PRFIndex = 3;
    m_baselineIndex = 0;
    m_wallFilterIndex = 2;
    m_lineDensity = 1;
    m_pktsizeIndex = 4;
    m_turbIndex = 0;
    m_colorMapCfm = 0;
    m_colorMapPdi = 0;
    m_colorMapPdiDir = 0;
    m_tissueGate = 500;
    m_noiseGate = 0;
    m_speedGate = 0;
    m_varianceGate = 128;
    m_gainGate = 8;
    m_colorMode = COLOR_CFM;
    m_invert = FALSE;

    m_boxLine[0] = 100;
    m_boxLine[1] = 149;
    m_boxDot[0] = 50;
    m_boxDot[1] = 200;
    m_boxDotBefore[0] = m_boxDot[0];
    m_boxDotBefore[1] = m_boxDot[1];
    m_boxStatus = FALSE;
    m_flowOptIndex = 0;
    m_clusterSize = 1;
    m_maxPRFIndex = MAX_PRF_INDEX - 1;
    m_balance = FALSE;
    m_BCWidth = FALSE;
    m_boxLineBak[0] = m_boxLine[0];
    m_boxLineBak[1] = m_boxLine[1];
    m_boxDotBak[0] = m_boxDotBefore[0];
    m_boxDotBak[1] = m_boxDotBefore[1];
    m_focIndex = 0;
    m_frameRemove = 0;

    int i;
    for (i = 0; i < 8; i ++)
        m_tgc[i] = 128;

    m_cfmFocusIndex = -1;
    m_fcLowFilter = 1.0;

    m_scanLineBak = 1;
    m_timerOpenColorFlow = 0;
    m_colorPersistBak = 0;
    m_colorThresholdBak = 0;
}
ImgCfm::~ImgCfm()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

void ImgCfm::SetCalcCfm(CalcCfm* calc)
{
    m_ptrCalc = calc;

    ///> init m_calcPara, then
    m_ptrCalc->SetCalcPara(&m_calcPara);
}

void ImgCfm::InitProbeOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    PRINTF("======================optimize PRINTF IMGCFM send para to fpga======================\n");
    EKnobReturn ret;

    ASSERT((ptrPara!=NULL) && (ptrParaItem!=NULL));

    ///> read para from exam item
    ResetBox(TRUE);
    LDensity(FALSE); // deal with mbp

    // gain
    m_gainCfm = ptrParaItem->color.gain;
    m_ptrUpdate->GainCfm(m_gainCfm);

    // flow sensitive, sometimes determined by wall filter
    m_pktsizeIndex = ptrParaItem->color.sensitive;
    m_ptrUpdate->PacketSize(m_pktsizeIndex, OK);

    // PRF
    //m_PRFIndex = 4;
    m_PRFIndex = ptrParaItem->color.PRFIndex;
    if (m_PRFIndex == 0)
        ret = MIN;
    else if (m_PRFIndex == (MAX_PRF_INDEX - 1))
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(CFM_PRF[m_PRFIndex], ret);

    // wall filter, sometime, determined by PRF
    m_wallFilterIndex = ptrParaItem->color.wallFilter;
    m_ptrUpdate->WallFilter(m_wallFilterIndex, OK);

    // flow opt
#ifdef EMP_430
    m_flowOptIndex = 6; //4
#else
    m_flowOptIndex = ptrParaItem->color.flowOpt;
#endif
    FlowOpt(m_flowOptIndex);

    // color line density
    m_lineDensity = ptrParaItem->color.lineDensity;
    if (m_lineDensity == 0)
        ret = MIN;
    else if (m_lineDensity == (MAX_LINE_DENSITY - 1))
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->LineDensity(m_lineDensity, ret);

    // turbo
    m_turbIndex = 0;
    //m_turbIndex = ptrParaItem->color.turb;
    if (m_turbIndex == 0)
        ret = MIN;
    else if (m_turbIndex == (MAX_TURB))
        ret = MAX;
    else
        ret = OK;
    Turbo(m_turbIndex, ret);

    // invert
    m_invert = ptrParaItem->color.invert;
    if (m_invert)
        ret = MAX;
    else
        ret = MIN;
    m_ptrUpdate->ColorInvert(m_invert, ret);

    if(ptrPara->type == 'p' || ptrPara->type == 'P')
    {
        SetScanLines();
    }
    m_ptrUpdate->ScanLines();

#if 0
    // for test
    m_tissueGate = 500;
    m_ptrUpdate->TissueGate(m_tissueGate, OK);
    m_noiseGate = 0;
    m_ptrUpdate->NoiseGate(m_noiseGate, OK);
    m_speedGate = 0;
    m_ptrUpdate->SpeedGate(m_speedGate, OK);
    m_varianceGate = 128;
    m_ptrUpdate->VarianceGate(m_varianceGate, OK);
    m_gainGate = 4;
    m_ptrUpdate->GainGate(m_gainGate, OK);
#endif

    // baseline
    //m_baselineIndex = 0;
    m_baselineIndex = ptrParaItem->color.baseline;
    if (m_baselineIndex == 0)
        ret = MIN;
    else if (m_baselineIndex == (MAX_BASELINE_INDEX - 1))
        ret = MAX;
    else
        ret = OK;
    //calc parament in InitProbeCalc function
    m_ptrUpdate->Baseline(m_baselineIndex, ret);

    // calc
    InitProbeCalc();
}
void ImgCfm::InitProbe(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    EKnobReturn ret;

    PRINTF("======================PRINTF IMGCFM send para to fpga======================\n");
    InitProbeOptimize(ptrPara, ptrParaItem);

    ///> read para from exam item
    m_boxStatus = FALSE;
    ResetBox(TRUE);
    LDensity(FALSE); // deal with mbp

    // color map
    m_colorMode = COLOR_CFM;
    m_colorMapCfm = 0;
    m_colorMapPdi = 0;
    m_colorMapPdiDir = 0;
    m_ptrUpdate->ColorMap(m_colorMapCfm, MIN);

    // cfm foc ratio
    m_focIndex = 1;
    if (m_focIndex == MAX_CFM_FOC_RATIO - 1)
        ret = MIN;
    else if (m_focIndex == 0)
        ret = MAX;
    else
        ret = OK;

    m_ptrUpdate->FocPosRatio(m_focIndex, ret);
#if 0
    // for test
    m_tissueGate = 500;
    m_ptrUpdate->TissueGate(m_tissueGate, OK);
    m_noiseGate = 0;
    m_ptrUpdate->NoiseGate(m_noiseGate, OK);
    m_speedGate = 0;
    m_ptrUpdate->SpeedGate(m_speedGate, OK);
    m_varianceGate = 128;
    m_ptrUpdate->VarianceGate(m_varianceGate, OK);
    m_gainGate = 4;
    m_ptrUpdate->GainGate(m_gainGate, OK);
#endif
    // calc
    InitProbeCalc();

}

void ImgCfm::GetCurPara(ExamItem::ParaItem* ptrParaItem)
{
    ptrParaItem->color.gain = m_gainCfm;
    ptrParaItem->color.PRFIndex = m_PRFIndex;
    ptrParaItem->color.wallFilter = m_wallFilterIndex;
    ptrParaItem->color.lineDensity = m_lineDensity;
    ptrParaItem->color.sensitive = m_pktsizeIndex;
    ptrParaItem->color.turb = m_turbIndex;
    ptrParaItem->color.invert = m_invert;
    ptrParaItem->color.flowOpt = m_flowOptIndex;
    ptrParaItem->color.baseline = m_baselineIndex;
}

void ImgCfm::ChangeDopplerFreq()
{
    // vel range
    VelRange(m_PRFIndex, m_baselineIndex);
}
void ImgCfm::BCWidth()
{
    if (m_BCWidth)
    {
        m_BCWidth = FALSE;
        ExitMaxBox();
    }
    else
    {
        m_BCWidth = TRUE;
        MaxBox();
    }
}

void ImgCfm::ResetBox(bool draw)
{
    int boxLineRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);
    int lines = boxLineRange[1] - boxLineRange[0] + 1;

    m_boxLine[0] = lines * 3 / 8 + boxLineRange[0];
    m_boxLine[1] = lines * 5 / 8 + boxLineRange[0];
    //m_boxLine[1] = lines * 1 / 4 + m_boxLine[0];
    PRINTF(" rangeline[0] = %d, rangeline[1] = %d, lineB = %d, lineE = %d\n", boxLineRange[0], boxLineRange[1], m_boxLine[0], m_boxLine[1]);
    m_boxDotBefore[0] = 50;
    m_boxDotBefore[1] = 200;
    BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);

    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], draw);
    m_BCWidth = FALSE;
}

void ImgCfm::ReSendBox(void)
{
    LDensity(TRUE);
}

void ImgCfm::SetBoxStatus(bool changeSize)
{
    m_boxStatus = changeSize;
    m_ptrUpdate->SetBoxStatus(changeSize);
}

bool ImgCfm::GetBoxStatus()
{
    return m_boxStatus;
}

bool ImgCfm::ChangeBoxSize(int offsetX, int offsetY, bool lrOverturn, bool udOverturn)
{
    StopColorFlow();

    if (offsetX > 0)
        offsetX = 1;
    else if (offsetX < 0)
        offsetX = -1;

    int ySign = 1;
    if (offsetY < 0)
        ySign = -1;
    offsetY = (offsetY + ySign) / 2;

    if (lrOverturn)
        offsetX = -offsetX;
    offsetY = -offsetY;
    if (udOverturn)
        offsetY = -offsetY;

    int mbp = m_ptrImg2D->GetMBP();
    int stepLine = offsetX*mbp;
    int stepDot = offsetY;
    int interLine = 20;
    int interDot = 20;

    // line
    int boxLineRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);

    if (offsetX > 0)
    {
        if (m_boxLine[1] + stepLine <= boxLineRange[1])
            m_boxLine[1] += stepLine;
        if (m_boxLine[0] - stepLine >= boxLineRange[0])
            m_boxLine[0] -= stepLine;
    }
    else if (offsetX < 0)
    {
        if (m_boxLine[0] - stepLine < m_boxLine[1] + stepLine -interLine)
        {
            m_boxLine[0] -= stepLine;
            m_boxLine[1] += stepLine;
        }
    }

    // dot
    int boxDotRange[2];
    GetBoxDotRange(boxDotRange[0], boxDotRange[1]);

    if (offsetY > 0)
    {
        if (m_boxDotBefore[0] - stepDot > boxDotRange[0])
            m_boxDotBefore[0] -= stepDot;
        if (m_boxDotBefore[1] + stepDot < boxDotRange[1])
            m_boxDotBefore[1] += stepDot;
    }
    else if (offsetY < 0)
    {
        if (m_boxDotBefore[0] - stepDot < m_boxDotBefore[1] + stepDot - interDot)
        {
            m_boxDotBefore[0] -= stepDot;
            m_boxDotBefore[1] += stepDot;
        }
    }

    BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);

    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);
    m_BCWidth = FALSE;

    return (TRUE);
}

bool ImgCfm::ChangeBoxPos(int offsetX, int offsetY)
{
    StopColorFlow();

    if (offsetX > 0)
        offsetX = 1;
    else if (offsetX < 0)
        offsetX = -1;

    int ySign = 1;
    if (offsetY < 0)
        ySign = -1;
    offsetY = (offsetY + ySign) / 2;

    int mbp = m_ptrImg2D->GetMBP();
    int stepLine = offsetX*mbp;
    int stepDot = -offsetY;
    m_frameRemove = 2;

    int boxLineRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);
    if (((m_boxLine[1] + stepLine) <= boxLineRange[1]) && ((m_boxLine[0] + stepLine) >= boxLineRange[0]))
    {
        m_boxLine[1] += stepLine;
        m_boxLine[0] += stepLine;
    }

    int boxDotRange[2];
    GetBoxDotRange(boxDotRange[0], boxDotRange[1]);
    if (((m_boxDotBefore[1] + stepDot) <= boxDotRange[1]) && ((m_boxDotBefore[0] + stepDot) >= boxDotRange[0]))
    {
        m_boxDotBefore[1] += stepDot;
        m_boxDotBefore[0] += stepDot;
    }
    BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);

    PRINTF("changBoxPos: lineB = %d, lineE = %d, dotB = %d, dotE = %d\n", m_boxLine[0], m_boxLine[1], m_boxDotBefore[0], m_boxDotBefore[1]);
    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);

    return (TRUE);
}

void ImgCfm::GetBoxInfo(int &lineBegin, int &lineEnd, int &dotBegin, int &dotEnd)
{
    lineBegin = m_boxLine[0];
    lineEnd = m_boxLine[1];
    dotBegin = m_boxDotBefore[0];
    dotEnd = m_boxDotBefore[1];
}
void ImgCfm::SetBoxInfo(int lineBegin, int lineEnd, int dotBegin, int dotEnd)
{
    StopColorFlow();
    m_boxLine[0] = lineBegin;
    m_boxLine[1] = lineEnd;
    m_boxDotBefore[0] = dotBegin;
    m_boxDotBefore[1] = dotEnd;
    BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);
    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);
}

void ImgCfm::ChangeTgcCfm(int tgcY[8])
{
    memcpy(m_tgc, tgcY, 8*sizeof(int));

#ifdef EMP_430
    m_ptrImg2D->ChangeTgc2D(tgcY);
#endif

    m_ptrImg2D->CalcTgc(m_gainCfm*MAX_GAIN_CFM/100, m_tgc, 3);
    m_ptrImg2D->CalcTgcDigital(m_gainCfm*MAX_GAIN_CFM/100, 3, MAX_GAIN_CFM);
}

void ImgCfm::ChangeMBP(bool drawBox)
{
    m_ptrImgPw->ChangeMBP();
    LDensity(drawBox);
}

/*
 * @brief change color Cfm gain
 *
 * @para gain value of color gain
 */
EKnobReturn ImgCfm::ChangeGainCfm(EKnobOper oper)
{
    int gain= m_gainCfm;
    int step = 1;

    float tu = m_ctGainCfm.End();
    m_ctGainCfm.Begin();

    if(tu < INTERVAL)
    {
        if(m_tGainCfm >= 3)
            step = 5;
        else
            m_tGainCfm ++;
    }
    else
    {
        m_tGainCfm = 0;
    }

    if (oper == ADD)
    {
        if(gain == 100)
        {
            PRINTF("Img2D: 2D gian max value!\n");
            return (MAX);
        }
        else
        {
            gain += step;
            if (gain > 100)
            {
                gain = 100;
            }
        }
    }
    else if (oper == SUB)
    {
        if(gain < 0)
        {
            PRINTF("Img2D: 2D gian reach min value!\n");
            return (MIN);
        }
        else
        {
            gain -= step;
            if (gain < 0)
            {
                gain = 0;
            }
        }
    }
    else
    {
        return ERROR;
    }

    m_gainCfm = gain;

    EKnobReturn ret = OK;
    if (gain == 0)
        ret = MIN;
    else if (gain == 100)
        ret = MAX;
    else
        ret = OK;

    GainCfm(m_gainCfm);
    return (ret);
}

EKnobReturn ImgCfm::ChangeScale(EKnobOper oper)
{
    int data = m_PRFIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < m_maxPRFIndex)
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_PRFIndex = data;

    EKnobReturn ret;

    ret = PRF(m_PRFIndex);

    return (ret);
}

EKnobReturn ImgCfm::ChangeBaseline(EKnobOper oper)
{
    if (m_colorMode == COLOR_PDI)
        return ERROR;

    int data = m_baselineIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < MAX_BASELINE_INDEX)
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > (- MAX_BASELINE_INDEX))
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_baselineIndex = data;

    EKnobReturn ret = OK;
    if (data == (-MAX_BASELINE_INDEX))
        ret = MIN;
    else if (data == MAX_BASELINE_INDEX)
        ret = MAX;
    else
        ret = OK;

    Baseline(m_baselineIndex, ret);
    return (ret);
}
EKnobReturn ImgCfm::ChangeWallFilter(EKnobOper oper)
{
    int data = m_wallFilterIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < MAX_WALLFILTER_INDEX - 1)
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_wallFilterIndex = data;

    //EKnobReturn ret = WallFilter(WALL_FILTER_SELECT_IDNEX[m_pktsizeIndex][m_wallFilterIndex], m_pktsizeIndex);
    EKnobReturn ret = WallFilter(m_wallFilterIndex, m_pktsizeIndex);

    return (ret);
}

EKnobReturn ImgCfm::ChangeLineDensity(EKnobOper oper)
{
    int data = m_lineDensity;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_LINE_DENSITY-1))
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_lineDensity = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_LINE_DENSITY - 1))
        ret = MAX;
    else
        ret = OK;
    LineDensity(m_lineDensity, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeSensitive(EKnobOper oper)
{
    int data = m_pktsizeIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < MAX_PKTSIZE_INDEX - 1)
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_pktsizeIndex = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == MAX_PKTSIZE_INDEX - 1)
        ret = MAX;
    else
        ret = OK;

    PktSize(m_pktsizeIndex, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeTurbo(EKnobOper oper)
{
    int data = m_turbIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_TURB-1))
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_turbIndex = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_TURB-1))
        ret = MAX;
    else
        ret = OK;

    Turbo(m_turbIndex, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeColorMap(EKnobOper oper)
{
    EKnobReturn ret = OK;

    if (m_colorMode == COLOR_CFM)
        ret = ChangeColorMapCfm(oper);
    else if (m_colorMode == COLOR_PDI)
        ret = ChangeColorMapPdi(oper);
    else if (m_colorMode == COLOR_PDI_DIR)
        ret = ChangeColorMapPdiDir(oper);

    return ret;
}
EKnobReturn ImgCfm::ChangeColorMapCfm(EKnobOper oper)
{
    int data = m_colorMapCfm;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_COLOR_MAP_CFM-1))
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_colorMapCfm = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_COLOR_MAP_CFM-1))
        ret = MAX;
    else
        ret = OK;

    ColorMapCfm(m_colorMapCfm, m_baselineIndex, m_turbIndex, m_invert, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeColorMapPdi(EKnobOper oper)
{
    int data = m_colorMapPdi;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_COLOR_MAP_PDI-1))
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_colorMapPdi = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_COLOR_MAP_PDI-1))
        ret = MAX;
    else
        ret = OK;

    ColorMapPdi(m_colorMapPdi, m_baselineIndex, m_turbIndex, m_invert, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeColorMapPdiDir(EKnobOper oper)
{
    int data = m_colorMapPdiDir;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_COLOR_MAP_PDIDIR-1))
        {
            data += step;
        }
        else
        {
            data = 0;
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_colorMapPdiDir = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_COLOR_MAP_PDIDIR-1))
        ret = MAX;
    else
        ret = OK;

    ColorMapPdiDir(m_colorMapPdiDir, m_baselineIndex, m_turbIndex, m_invert, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeColorFocus(EKnobOper oper)
{
    int data = m_focIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_CFM_FOC_RATIO - 1))
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    m_focIndex = data;

    EKnobReturn ret = OK;
    if (m_focIndex == MAX_CFM_FOC_RATIO - 1)
        ret = MAX;
    else if (m_focIndex == 0)
        ret = MIN;
    else
        ret = OK;

    FocPos(m_focIndex);
    m_ptrUpdate->FocPosRatio(m_focIndex, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeFlowOpt(EKnobOper oper)
{
    int data = m_flowOptIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_FLOW_OPT - 1))
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_flowOptIndex = data;
    return FlowOpt(m_flowOptIndex);
}

EKnobReturn ImgCfm::ChangeInvert(EKnobOper oper)
{
    bool data = m_invert;

    if ((m_turbIndex != 0) || (m_colorMode == COLOR_PDI))
    {
        HintArea::GetInstance()->UpdateHint(_("[Color Invert]: Invalid in turbo and PDI mode."), 1);
        return ERROR;
    }

    if (oper == ADD)
    {
        if (!data)
            data = TRUE;
        else
            return MAX;
    }
    else if (oper == SUB)
    {
        if (data)
            data = FALSE;
        else
            return MIN;
    }
    else
    {
        return ERROR;
    }

    m_invert = data;

    EKnobReturn ret = OK;
    if (data)
        ret = MAX;
    else
        ret = MIN;
    m_ptrUpdate->ColorInvert(m_invert, ret);

    // when invert is changed
    ColorMap();

    return ret;
}

void ImgCfm::ScanLinesSetting(int scanline)
{
    m_scanLineBak = scanline;
}

/*
 * @brief change phase probe scan line
 */
EKnobReturn ImgCfm::ChangeScanLines(EKnobOper oper)
{
    EKnobReturn ret = OK;
    ret = Img2D::GetInstance()->ChangeScanLines(oper);

    SetScanLines();

    return ret;
}

void ImgCfm::SetScanLines()
{
    int scanLine = Img2D::GetInstance()->GetScanLines();

    // recalc emit delay
    FocPos(m_focIndex);
    m_ptrImg2D->CalcReceiveDelayColor();

    ///> if box is out of range
    int boxLineRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);
    if (m_BCWidth)
    {
        // max
        m_ptrImg2D->GetDisplayScanRange(boxLineRange);
        m_boxLine[0] = boxLineRange[0];
        m_boxLine[1] = boxLineRange[1];
#if defined(EMP_430)
        BoxDotCorrect(0, (IMG_H-1));
#elif defined(EMP_355)
        int dots = IMG_H * Calc2D::INIT_SCALE - 1;
        m_boxDotBak[0] = 0;
        m_boxDotBak[1] = dots;
        BoxDotCorrect(m_boxDotBak[0], m_boxDotBak[1]);
#else
        m_boxDotBak[0] = 0;
        m_boxDotBak[1] = IMG_H-1;
        BoxDotCorrect(m_boxDotBak[0], m_boxDotBak[1]);
#endif
        Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);
        m_ptrUpdate->ScanLines();
        return;
    }
    else
    {
        m_boxLine[0] = m_boxLine[0] * scanLine / m_scanLineBak;
        m_boxLine[1] = m_boxLine[1] * scanLine / m_scanLineBak;
        m_scanLineBak = scanLine;
    }

    PRINTF("ldendit: lineb = %d, linee = %d, range[0] = %d, range[1] = %d\n", m_boxLine[0], m_boxLine[1], boxLineRange[0], boxLineRange[1]);

    // correct box
    if ((m_boxLine[0] < boxLineRange[0]) || (m_boxLine[1] > boxLineRange[1]))
    {
        ResetBox(TRUE);
    }
    else
    {
        BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);
        Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);
    }

    m_ptrUpdate->ScanLines();
}

/*
 * @brief deal some thing before depth is changed
 */
void ImgCfm::ChangeDepthBefore()
{
    m_scale2DBefore = m_ptrImg2D->GetScale2DInImgHDot();
}

/*
 * @brief change depth, must call after 2D's ChangDepth
 */
void ImgCfm::ChangeDepth()
{
    // change boxdot's pos
    double scale2D = m_ptrImg2D->GetScale2DInImgHDot();
    int depth = m_ptrImg2D->GetDepth();
    int depthBefore = m_boxDotBefore[1] * m_scale2DBefore + 0.5;
    int dots = IMG_H * Calc2D::INIT_SCALE;
    if (depthBefore < depth)
    {
        m_BCWidth = FALSE;
        m_boxDotBefore[0] = m_boxDotBefore[0] * m_scale2DBefore / scale2D;
        m_boxDotBefore[1] = m_boxDotBefore[1] * m_scale2DBefore / scale2D;

        if ((m_boxDotBefore[1] - m_boxDotBefore[0]) < 20)
        {
            m_boxDotBefore[1] = m_boxDotBefore[0] + 20;
#ifdef EMP_355
            if (m_boxDotBefore[1] > dots)
            {
                m_boxDotBefore[1] = dots;
                m_boxDotBefore[0] = m_boxDotBefore[1] - 20;
            }
#else
            if (m_boxDotBefore[1] > IMG_H)
            {
                m_boxDotBefore[1] = IMG_H;
                m_boxDotBefore[0] = m_boxDotBefore[1] - 20;
            }
#endif
        }
        BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);
        Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], FALSE);
    }
    else
    {
        FocPos(m_focIndex);
        ClusterSize(CFM_PRF[m_PRFIndex]);
    }
}

/*
 * @brief change steer angle, must call after Pw's ChangeSteer
 */
void ImgCfm::ChangeSteer()
{
    Steer();
}

void ImgCfm::ChangeTSI(void)
{
    FocPos(m_focIndex);
}

/*
 *@brief display TIS in cfm mode, factors that can impact upon Tis's value are doppler frequency, focus position, index of sound power,
 *       index of PRF, range of box line.
 */
void ImgCfm::ChangeCfmTis(void)
{
    // Tis
    SetTis(m_PRFIndex, m_boxLine[0], m_boxLine[1]);
}

void ImgCfm::ChangeSoundSpeed()
{
}
void ImgCfm::ClearBox()
{
    m_ptrUpdate->BoxClear();
}

void ImgCfm::ReDrawBox()
{
    PRINTF("redraw box: lineB = %d, lineE = %d, dotB = %d, dotE = %d\n", m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1]);
    ///>为了解决调节mbp和steer时彩色血流和取样框没对准的问题
    int mbp = m_ptrImg2D->GetMBP();
    int lineDensity = m_ptrImg2D->GetLDensity();
    int lineBegin = m_boxLine[0];
    int lineEnd = m_boxLine[1];
    int boxLineTemp[2];
    if((mbp > 2 && lineDensity > 0) || (mbp > 1 && lineDensity == 0))
    {
        boxLineTemp[0] = lineBegin/12*12*4/6+3;
        boxLineTemp[1] = (lineBegin/12*12 + (lineEnd-lineBegin)/3*3)*4/6 - 2;
    }
    else if((mbp == 2 && lineDensity > 0) || (mbp == 1 && lineDensity == 0))
    {
        boxLineTemp[0] = lineBegin/4*4+2;
        boxLineTemp[1] = lineBegin/4*4 + (lineEnd-lineBegin)/4*4;
    }
    else
    {
        boxLineTemp[0] = lineBegin/2*2+1;
        boxLineTemp[1] = lineBegin/2*2 + (lineEnd-lineBegin)/2*2;
    }

    if((mbp > 2 && lineDensity > 0) || (mbp > 1 && lineDensity == 0))
        m_ptrUpdate->Box(boxLineTemp[0]*6/4+1, boxLineTemp[1]*6/4+3, m_boxDot[0], m_boxDot[1]);
    else
        m_ptrUpdate->Box(boxLineTemp[0], boxLineTemp[1], m_boxDot[0], m_boxDot[1]);

    //m_ptrUpdate->Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1]);
}
void ImgCfm::SetColorMode(EColorMode mode)
{
    m_colorMode = mode;

    bool on;
    int baseline = BASELINE_VALUE;
    if (m_colorMode == COLOR_CFM)
    {
        on = FALSE;
        m_ptrDscPara->dcaCFMIsDirection = 1;
        m_ptrDscPara->dcaCFMBaseline = baseline;

    }
    else if (m_colorMode == COLOR_PDI)
    {
        on = FALSE;
        m_ptrDscPara->dcaCFMIsDirection = 0;
        m_ptrDscPara->dcaCFMBaseline = 0;
        m_ptrCalc->CalcSwitchPdi(FALSE);
    }
    else
    {
        on = TRUE;
        m_ptrDscPara->dcaCFMIsDirection = 1;
        m_ptrDscPara->dcaCFMBaseline = baseline;
        m_ptrCalc->CalcSwitchPdi(TRUE);
    }
    if(m_ptrDsc != NULL)
        m_ptrDsc->UpdateCFMPDIDirection();
    m_ptrUpdate->PdiDirStatus(on);

    ColorMap();
}

//private
void ImgCfm::InitProbeCalc()
{
    // emit delay
    FocPos(m_focIndex);

    GainCfm(m_gainCfm);

    ClusterSize(CFM_PRF[m_PRFIndex]);

    //send colormap
    ColorMap();
    SetColorMode(m_colorMode);

    m_ptrCalc->CalcPacketSize(PACKSIZE[m_pktsizeIndex]);

    // send wall filter
    //WallFilter(WALL_FILTER_SELECT_IDNEX[m_pktsizeIndex][m_wallFilterIndex], m_pktsizeIndex);
    WallFilter(m_wallFilterIndex, m_pktsizeIndex);

    m_ptrCalc->CalcLowFilter(m_fcLowFilter);

#if 0
    m_fpga.SendTissueGate((INT16U)m_tissueGate);
    m_fpga.SendNoiseGate((INT16U)m_noiseGate);
    m_fpga.SendSpeedGate((INT16U)m_speedGate);
    m_fpga.SendVarianceGate((INT16U)m_varianceGate);

    m_ptrCalc->CalcGainGate(m_gainGate);
    m_ptrUpdate->GainGate(m_gainGate, OK);
#endif
    // vel range
    VelRange(m_PRFIndex, m_baselineIndex);
}

void ImgCfm::Box(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool draw)
{
    PRINTF("-----cfm box: lineb = %d, linee = %d, dotb = %d, dote = %d\n", lineBegin, lineEnd, dotBegin, dotEnd);
    PRINTF("-----cfm box: width= %d, height= %d\n", lineEnd - lineBegin, dotEnd - dotBegin);
    PRINTF("--------------2D dsc begin:lineb = %d, linee = %d\n", m_ptrDscPara->dcaCurScanStart,  m_ptrDscPara->dcaCurScanEnd);
    // box
    ReviseBoxLine(lineBegin, lineEnd);
    m_boxLine[0] = lineBegin;
    m_boxLine[1] = lineEnd;
    PRINTF("-----cfm box: lineb = %d, linee = %d, dotb = %d, dote = %d\n", lineBegin, lineEnd, dotBegin, dotEnd);
    ASSERT((lineBegin >= (int)m_ptrDscPara->dcaCurScanStart) && (lineEnd <= (int)m_ptrDscPara->dcaCurScanEnd));

    int boxLineTemp[2];
    int mbp = m_ptrImg2D->GetMBP();
    int lineDensity = m_ptrImg2D->GetLDensity();
    if((mbp > 2 && lineDensity > 0) || (mbp > 1 && lineDensity == 0) )
    {
        boxLineTemp[0] = lineBegin/12*12+9;
        boxLineTemp[1] = lineBegin/12*12 + (lineEnd-lineBegin)/3*3;
    }
    else if((mbp == 2 && lineDensity > 0) || (mbp == 1 && lineDensity == 0) )               //(mbp > 1)
    {
        boxLineTemp[0] = lineBegin/4*4+2;
        boxLineTemp[1] = lineBegin/4*4 + (lineEnd-lineBegin)/4*4;
    }
    else
    {
        boxLineTemp[0] = lineBegin/2*2+1;
        boxLineTemp[1] = lineBegin/2*2 + (lineEnd-lineBegin)/2*2;
    }

    if(boxLineTemp[1] <= boxLineTemp[0])
        boxLineTemp[1] = boxLineTemp[0] + 1;

    // send to fapa
#ifdef EMP_355
    int offset = 4; //0;     //-6;//0-2;
#else
    int offset = 0;     //-6;//0-2;
#endif
    int boxLineFpga[2];
    int boxDotFpga[2];
    boxDotFpga[0] = dotBegin+offset;
    boxDotFpga[1] = dotEnd+offset;
    if((mbp > 2 && lineDensity > 0) || (mbp > 1 && lineDensity == 0) )          //(mbp>2)
    {
        boxLineFpga[0] = boxLineTemp[0]-9;
        boxLineFpga[1] = boxLineTemp[1];
    }
    else
    {
        boxLineFpga[0] = boxLineTemp[0];
        boxLineFpga[1] = boxLineTemp[1];
    }

    // correct 解决选框右侧边缘在某些固定位置会有多余伪像的问题。
    int lines = m_ptrImg2D->GetScanLines();
    if ((boxLineFpga[1]+4) < lines)
    {
        boxLineFpga[1] += 4;
    }
    m_ptrCalc->CalcBox(boxLineFpga[0], boxLineFpga[1], boxDotFpga[0], boxDotFpga[1]);   //6
    PRINTF("--------------CFM BOX fpga:lineb = %d, linee = %d, dotb = %d, dote = %d\n", boxLineFpga[0], boxLineFpga[1], boxDotFpga[0], boxDotFpga[1]);

    // send to DSC
    double scale = 1.0;
    if((mbp > 2 && lineDensity > 0) || (mbp > 1 && lineDensity == 0))           //(mbp > 2)
    {
        boxLineTemp[0] = lineBegin/12*12*4/6+3;
        boxLineTemp[1] = (lineBegin/12*12 + (lineEnd-lineBegin)/3*3)*4/6 - 2;
        scale = 1.5;
    }

    if(boxLineTemp[1] <= boxLineTemp[0])
        boxLineTemp[1] = boxLineTemp[0] + 1;

    int boxLineDsc[2];
    boxLineDsc[0] = boxLineTemp[0];
    boxLineDsc[1] = boxLineTemp[1];
    if ((boxLineDsc[0] * scale) < (int)m_ptrDscPara->dcaCurScanStart)
    {
        boxLineDsc[0] = m_ptrDscPara->dcaCurScanStart / scale + 0.5;
        if (boxLineDsc[1] <= boxLineDsc[0])
            boxLineDsc[1] = boxLineDsc[0] + 1;
    }
    if ((boxLineDsc[1] * scale) > (int)m_ptrDscPara->dcaCurScanEnd)
    {
        boxLineDsc[1] = m_ptrDscPara->dcaCurScanEnd / scale; // need not add +0.5
        if (boxLineDsc[1] <= boxLineDsc[0])
            boxLineDsc[0] = boxLineDsc[1] - 1;
    }

    if (dotBegin == 0)
    {
        dotBegin = 1;
    }

    if ((dotEnd - dotBegin) < 1)
    {
        dotEnd += 1;
#ifdef EMP_355
        int dots = IMG_H * Calc2D::INIT_SCALE;
        if (dotEnd > dots)
        {
            dotEnd = dots;
        }
#else
        if (dotEnd > IMG_H)
        {
            dotEnd = IMG_H;
        }
#endif
    }

    PRINTF("--------------CFM BOX dsc :lineb = %d, linee = %d, dotb = %d, dote = %d\n", m_ptrDscPara->dcaCFMScanLStart,  m_ptrDscPara->dcaCFMScanLEnd, m_ptrDscPara->dcaCFMSampPtStart, m_ptrDscPara->dcaCFMSampPtEnd);
    m_ptrDscPara->dcaCFMScanLStart = boxLineDsc[0];
#ifdef EMP_355
    if(mbp == 4)
        m_ptrDscPara->dcaCFMScanLEnd = boxLineDsc[1];
    else
      m_ptrDscPara->dcaCFMScanLEnd = boxLineDsc[1]-offset;
#else
    m_ptrDscPara->dcaCFMScanLEnd = boxLineDsc[1];
#endif
    m_ptrDscPara->dcaCFMSampPtStart = dotBegin;
    m_ptrDscPara->dcaCFMSampPtEnd = dotEnd;
    PRINTF("--------------2D dsc end :lineb = %d, linee = %d\n", m_ptrDscPara->dcaCurScanStart,  m_ptrDscPara->dcaCurScanEnd);
    PRINTF("--------------CFM BOX dsc :lineb = %d, linee = %d, dotb = %d, dote = %d\n", m_ptrDscPara->dcaCFMScanLStart,  m_ptrDscPara->dcaCFMScanLEnd, m_ptrDscPara->dcaCFMSampPtStart, m_ptrDscPara->dcaCFMSampPtEnd);
    // change clusterSize and maxPRF
    ClusterSize(CFM_PRF[m_PRFIndex]);

    PRINTF("----------box: dotBegin = %d, dotEnd = %d\n", dotBegin, dotEnd);
    // update view
    if (draw)
    {
        if((mbp > 2 && lineDensity > 0) || (mbp > 1 && lineDensity == 0) )              //(mbp > 2)
            m_ptrUpdate->Box(boxLineTemp[0]*6/4+1, boxLineTemp[1]*6/4+3, dotBegin, dotEnd);
        else
            m_ptrUpdate->Box(boxLineTemp[0], boxLineTemp[1], dotBegin, dotEnd);
    }

    // recalc emit delay
    FocPos(m_focIndex);

    // Tis
    SetTis(m_PRFIndex, m_boxLine[0], m_boxLine[1]);

    // cfm+pw mode, balance sv and box
    if (m_balance)
        BalancePwInfo();
}

void ImgCfm::MaxBox()
{
    // bak
    m_boxLineBak[0] = m_boxLine[0];
    m_boxLineBak[1] = m_boxLine[1];
    m_boxDotBak[0] = m_boxDotBak[0];
    m_boxDotBak[1] = m_boxDotBak[1];
    // max
    int scanRange[2];
    GetBoxLineRange(scanRange[0], scanRange[1]);

    //m_ptrImg2D->GetDisplayScanRange(scanRange);
    m_boxLine[0] = scanRange[0];
    m_boxLine[1] = scanRange[1];

#if defined(EMP_430)
    //exit BCWidth the height is not (IMG_W-1)
    BoxDotCorrect(0, (IMG_H-1));
#elif defined(EMP_355)
    int dots = IMG_H * Calc2D::INIT_SCALE - 1;
     m_boxDotBak[0] = 0;
     m_boxDotBak[1] = dots;
    BoxDotCorrect(m_boxDotBak[0], m_boxDotBak[1]);
#else
     m_boxDotBak[0] = 0;
     m_boxDotBak[1] = IMG_H-1;
    BoxDotCorrect(m_boxDotBak[0], m_boxDotBak[1]);
#endif
     //2016.7.20
    m_boxDotBefore[0] = m_boxDot[0];
    m_boxDotBefore[1] = m_boxDot[1];

    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);
}

void ImgCfm::ExitMaxBox()
{
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    if((type == 'p') || (type == 'P'))
    {
        int scanRange[2];
        GetBoxLineRange(scanRange[0], scanRange[1]);
        m_boxLine[0] = scanRange[0] + (scanRange[1] - scanRange[0]) / 4;
        m_boxLine[1] = scanRange[0] + (scanRange[1] - scanRange[0]) / 4 * 3;
    }
    else
    {
        m_boxLine[1] = m_boxLineBak[0];
        m_boxLine[1] = m_boxLineBak[1];
    }
#ifdef EMP_430
    m_boxLineBak[0] = 0;
    m_boxLineBak[1] = 150;
#endif
    m_boxDotBefore[0] = m_boxDotBak[0];
    m_boxDotBefore[1] = m_boxDotBak[1];
    BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);
    PRINTF("-----exit max box max box: lineb = %d, linee = %d, dotb = %d, dote = %d\n", m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1]);
    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], TRUE);
}

void ImgCfm::GainCfm(int data)
{
    // udpate view
    m_ptrUpdate->GainCfm(data);

#ifdef EMP_430
    data = data/2 + 50;
#endif
    // calc tgc
    m_ptrImg2D->CalcTgc(data*MAX_GAIN_CFM/100, m_tgc, 3);
    m_ptrImg2D->CalcTgcDigital(data*MAX_GAIN_CFM/100, 3, MAX_GAIN_CFM);

    // recalc vairance
    SetVariance(data, m_flowOptIndex);
}

void ImgCfm::EnterCfm()
{
    // must send focus delay
    m_cfmFocusIndex = -1;

    // init
    LDensity();
    m_ptrCalc->CalcPacketSize(PACKSIZE[m_pktsizeIndex]);

    // enter cfm mode
    SetColorMode(COLOR_CFM);

    // resend flow para
    FlowOpt(m_flowOptIndex);

    SetBoxStatus(false);

    char type = GlobalClassMan::GetInstance()->GetProbeType();
    if(type == 'p' || type == 'P')
    {
        SetScanLines();
    }
}

void ImgCfm::EnterPdi(void)
{
    // must send focus delay
    m_cfmFocusIndex = -1;

    LDensity();
    m_ptrCalc->CalcPacketSize(PACKSIZE[m_pktsizeIndex]);

    // enter cfm mode
    SetColorMode(COLOR_PDI);

    // resend flow para
    FlowOpt(m_flowOptIndex);

    SetBoxStatus(false);
}

/*
 * @brief witch pdi and dir pdi, only in pdi mode
 *
 * @para enterPdi[in] if or not enter dir pdi
 */
void ImgCfm::SwitchPdiAndDir(bool enterPdiDir)
{
    // change color map
    if (enterPdiDir)
    {
        SetColorMode(COLOR_PDI_DIR);
    }
    else
    {
        SetColorMode(COLOR_PDI);
    }
}

///////////////////////////////////////   test   ///////////////////////////////////
EKnobReturn ImgCfm::ChangeTissue(EKnobOper oper)
{
    int data = m_tissueGate;
    int step =100; // 5;

    if (oper == ADD)
    {
        if (data < 20000)   //1500
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data >= 10)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_tissueGate = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == 20000) //1500
        ret = MAX;
    else
        ret = OK;

    m_fpga.SendTissueGate((INT16U)m_tissueGate);
    m_ptrUpdate->TissueGate(m_tissueGate, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeNoise(EKnobOper oper)
{
    int data = m_noiseGate;
    int step = 10;

    if (oper == ADD)
    {
        if (data < 260)
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_noiseGate = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == 260)
        ret = MAX;
    else
        ret = OK;

    m_fpga.SendNoiseGate((INT16U)m_noiseGate);
    m_ptrUpdate->NoiseGate(m_noiseGate, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeSpeed(EKnobOper oper)
{
    int data = m_speedGate;
    int step = 5;

    if (oper == ADD)
    {
        if (data < 1019)
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 4)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_speedGate = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == 1020)
        ret = MAX;
    else
        ret = OK;

    m_fpga.SendSpeedGate((INT16U)m_speedGate);
    m_ptrUpdate->SpeedGate(m_speedGate, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeVariance(EKnobOper oper)
{
    int data = m_varianceGate;
    int step = 1;

    if (oper == ADD)
    {
        if (data < 16384)   //1028
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 20)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_varianceGate = data;

    EKnobReturn ret = OK;
    if (data == 8)
        ret = MIN;
    else if (data == 16383) //1028
        ret = MAX;
    else
        ret = OK;

    m_fpga.SendVarianceGate((INT16U)m_varianceGate);
    m_ptrUpdate->VarianceGate(m_varianceGate, ret);

    return (ret);
}

EKnobReturn ImgCfm::ChangeGainGate(EKnobOper oper)
{
    int data = m_gainGate;
    int step = 1;

    if (oper == ADD)
    {
        if (data < 100)
        {
            data += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > 0)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
    m_gainGate = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == 100)
        ret = MAX;
    else
        ret = OK;

    m_ptrCalc->CalcGainGate(m_gainGate);

    m_ptrUpdate->GainGate(m_gainGate, ret);

    return (ret);
}

int ImgCfm::GetBoxRange(unsigned char range[2])
{
    range[0] = m_boxLine[0];
    range[1] = m_boxLine[1];

    return 1;
}

int ImgCfm::GetFrameRemove(void)
{
    return m_frameRemove;
}
void ImgCfm::SubFrameRemove(void)
{
    if (m_frameRemove > 0)
        m_frameRemove --;
}

bool ImgCfm::WhenBoxIsOutRange(bool draw)
{
    int boxLineRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);

    if ((m_boxLine[0] < boxLineRange[0]) || (m_boxLine[1] > boxLineRange[1]))
        ResetBox(draw);
    return TRUE;
}

int ImgCfm::GetPktSize(void)
{
    return PACKSIZE[m_pktsizeIndex];
}
/////////////////////////////////////////////////////////////////////////////////////////
EKnobReturn ImgCfm::PRF(int index)
{
    //cluster size
    float scale = m_ptrImg2D->GetScale2DInImgHDot();
    int depth = scale * m_boxDot[1];
    bool retVal = ClusterSizeNormal(CFM_PRF[index], depth);

    // update view
    EKnobReturn ret = OK;
    if (m_PRFIndex == 0) // must use m_PRFIndex, PRF mybe call recursive in ClusterSizeNormal functon
        ret = MIN;
    else if (m_PRFIndex == m_maxPRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(CFM_PRF[m_PRFIndex], ret);

    // prf
    if (retVal)
        WhenPRFChanged(index);

    return ret;
}

void ImgCfm::WhenPRFChanged(int index)
{
    EKnobReturn ret;
    // wall filter
    if (m_wallFilterIndex == 0)
        ret = MIN;
    else if (m_wallFilterIndex == (MAX_WALLFILTER_INDEX - 1))
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->WallFilter(m_wallFilterIndex, ret);

    // vel range
    VelRange(index, m_baselineIndex);
}

void ImgCfm::Baseline(int index, EKnobReturn ret)
{
    //send colormap, 0 is temporary
    ColorMap();

    m_ptrUpdate->Baseline(m_baselineIndex, ret);

    // vel range
    VelRange(m_PRFIndex, m_baselineIndex);
}
void ImgCfm::VelRange(int prfIndex, int baselineIndex)
{
    double max, min;
    double maxKHz, minKHz;
    int colorFreq = ImgPw::GetInstance()->GetColorFreq();

    double speed = SOUND_SPEED * 1000;
    double maxVel =  m_ptrCalc->CalcMaxVel(CFM_PRF[prfIndex], colorFreq, speed);

    max = maxVel * (MAX_BASELINE_INDEX - baselineIndex) / MAX_BASELINE_INDEX;
    min = maxVel * (-MAX_BASELINE_INDEX - baselineIndex) / MAX_BASELINE_INDEX;

    maxKHz = CFM_PRF[prfIndex] * (MAX_BASELINE_INDEX - baselineIndex) / MAX_BASELINE_INDEX / (float)1000;
    minKHz = CFM_PRF[prfIndex] * (-MAX_BASELINE_INDEX - baselineIndex) / MAX_BASELINE_INDEX / (float)1000;

    PRINTF("-----------------------max = %f, min = %f\n", max, min);
    m_ptrUpdate->SetVelRange(max, min, maxKHz, minKHz);
}
void ImgCfm::ReviseBoxLine(int &lineBegin, int &lineEnd)
{
    int mbp = m_ptrImg2D->GetMBP();
    if(mbp > 1) //multiply mbp
    {
        lineBegin = lineBegin / mbp * mbp; // avoid out of range
        lineEnd = lineEnd / mbp * mbp;
    }
}

void ImgCfm::LDensity(bool draw)
{
    ///> if box is out of range
    int boxLineRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);

    // lock
    DscMan* ptrDscMan = DscMan::GetInstance();
    if (ptrDscMan != NULL)
        ptrDscMan->GetWriteLock();

    PRINTF("ldendit: lineb = %d, linee = %d, range[0] = %d, range[1] = %d\n", m_boxLine[0], m_boxLine[1], boxLineRange[0], boxLineRange[1]);
    // correct box
    if ((m_boxLine[0] < boxLineRange[0]) || (m_boxLine[1] > boxLineRange[1]))
    {
        ResetBox(draw);
    }
    else
    {
        BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);
        Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], draw);
    }

    //color line density
    int mbp = m_ptrImg2D->GetMBP();
    int lineDensity = m_ptrImg2D->GetLDensity();
    if(lineDensity == 0) //low
    {
        if(mbp > 1)
            m_ptrDscPara->dcaCFMLDensity = 4;
        else
            m_ptrDscPara->dcaCFMLDensity = 1;
    }
    else //high
    {
        if(mbp > 2)
            m_ptrDscPara->dcaCFMLDensity = 4;
        else
            m_ptrDscPara->dcaCFMLDensity = 1;
    }
    if (ptrDscMan != NULL)
        ptrDscMan->ReadWriteUnlock();
    // lock end

    if (m_ptrDsc != NULL)
        m_ptrDsc->UpdateCFMLDensity();
}

void ImgCfm::PktSize(int index, EKnobReturn ret)
{
    //WallFilter(WALL_FILTER_SELECT_IDNEX[m_pktsizeIndex][m_wallFilterIndex], index);
    WallFilter(m_wallFilterIndex, index);
    m_ptrCalc->CalcPacketSize(PACKSIZE[index]);

    m_ptrUpdate->PacketSize(index, ret);
}

/*
 * @brief change color map of CFM
 *
 * @para index[in] index of color map
 * @para baselineIndex[in] index of baseline of CFM
 * @para turb[in] grade of turbo, range(0-MAX_TURBO-1)
 * @para invert[in] if or not color is invert
 * @para ret[in] status of knobe "Color Map"
 */
void ImgCfm::ColorMapCfm(int index, int baselineIndex, int turb, bool invert, EKnobReturn ret)
{
    unsigned char colorMap[CalcCfm::COLOR_LENGTH * CalcCfm::COLOR_WIDTH][CalcCfm::COLOR_DEPTH];
    int baseline = GetRealBaseline(baselineIndex);
    if (turb == 0)
        m_ptrCalc->CalcColorGrayCfm(COLOR_MAP_CFM[index], baseline, turb, invert, colorMap);
    else
        m_ptrCalc->CalcColorGrayCfmVar(COLORMAP_CFMVAR_ASIA0, baseline, turb, invert, colorMap);

    m_ptrUpdate->ColorMap(index, ret);
    m_ptrUpdate->ColorTable(colorMap, CalcCfm::COLOR_LENGTH, CalcCfm::COLOR_WIDTH, CalcCfm::COLOR_DEPTH);

    PRINTF("--------colorMap[baseline].rgbred=%d\n",colorMap[baseline * CalcCfm::COLOR_WIDTH][0]);
    PRINTF("--------colorMap[baseline].rgbgreen=%d\n",colorMap[baseline * CalcCfm::COLOR_WIDTH][1]);
    PRINTF("--------colorMap[baseline].rgbbule=%d\n",colorMap[baseline * CalcCfm::COLOR_WIDTH][2]);
    // send color map to dsc
    int i;
    unsigned char dValue = baseline - BASELINE_VALUE;
    int j;

    for (i = 0; i < CalcCfm::COLOR_LENGTH; i ++)
    {
        j = (i + dValue) % 256;
        m_ptrDscPara->dcaCFMPalette[i].rgbtRed = colorMap[j * CalcCfm::COLOR_WIDTH][0];
        m_ptrDscPara->dcaCFMPalette[i].rgbtGreen = colorMap[j * CalcCfm::COLOR_WIDTH][1];
        m_ptrDscPara->dcaCFMPalette[i].rgbtBlue = colorMap[j * CalcCfm::COLOR_WIDTH][2];
    }
}

void ImgCfm::ColorMapPdi(int index, int baselineIndex, int turb, bool invert, EKnobReturn ret)
{
    unsigned char colorMap[CalcCfm::COLOR_LENGTH * CalcCfm::COLOR_WIDTH][CalcCfm::COLOR_DEPTH];
    int baseline = GetRealBaseline(baselineIndex);

    m_ptrCalc->CalcColorGrayPdi(COLOR_MAP_PDI[index], baseline, turb, invert, colorMap);

    m_ptrUpdate->ColorMap(index, ret);
    m_ptrUpdate->ColorTable(colorMap, CalcCfm::COLOR_LENGTH, CalcCfm::COLOR_WIDTH, CalcCfm::COLOR_DEPTH);

    // send color map to dsc
    int i;
    int j;
    int len = CalcCfm::COLOR_LENGTH;
   unsigned char dValue = baseline - BASELINE_VALUE;
    for (i = 0; i < len; i ++)
    {
        j = (i + dValue) % 256;
        m_ptrDscPara->dcaCFMPalette[i].rgbtRed = colorMap[(len - 1 - j) * CalcCfm::COLOR_WIDTH][0];
        m_ptrDscPara->dcaCFMPalette[i].rgbtGreen = colorMap[(len - 1 - j) * CalcCfm::COLOR_WIDTH][1];
        m_ptrDscPara->dcaCFMPalette[i].rgbtBlue = colorMap[(len - 1 - j) * CalcCfm::COLOR_WIDTH][2];
    }
}
void ImgCfm::ColorMapPdiDir(int index, int baselineIndex, int turb, bool invert, EKnobReturn ret)
{
    unsigned char colorMap[CalcCfm::COLOR_LENGTH * CalcCfm::COLOR_WIDTH][CalcCfm::COLOR_DEPTH];
    int baseline = GetRealBaseline(baselineIndex);

    m_ptrCalc->CalcColorGrayCfm(COLOR_MAP_PDIDIR[index], baseline, turb, invert, colorMap);

    m_ptrUpdate->ColorMap(index, ret);
    m_ptrUpdate->ColorTable(colorMap, CalcCfm::COLOR_LENGTH, CalcCfm::COLOR_WIDTH, CalcCfm::COLOR_DEPTH);

    // send color map to dsc
    int i;
    int j;
    unsigned char dValue = baseline - BASELINE_VALUE;
    for (i = 0; i < CalcCfm::COLOR_LENGTH; i ++)
    {
        j = (i + dValue) % 256;
        m_ptrDscPara->dcaCFMPalette[i].rgbtRed = colorMap[j * CalcCfm::COLOR_WIDTH][0];
        m_ptrDscPara->dcaCFMPalette[i].rgbtGreen = colorMap[j * CalcCfm::COLOR_WIDTH][1];
        m_ptrDscPara->dcaCFMPalette[i].rgbtBlue = colorMap[j * CalcCfm::COLOR_WIDTH][2];
    }
}
void ImgCfm::ColorMap()
{
    EKnobReturn ret;

    if (m_colorMode == COLOR_CFM)
    {
        if (m_colorMapCfm == 0)
            ret = MIN;
        else if (m_colorMapCfm == (MAX_COLOR_MAP_CFM-1))
            ret = MAX;
        else
            ret = OK;
        ColorMapCfm(m_colorMapCfm, m_baselineIndex, m_turbIndex, m_invert, ret);
    }
    else if (m_colorMode == COLOR_PDI)
    {
        if (m_colorMapPdi == 0)
            ret = MIN;
        else if (m_colorMapPdi == (MAX_COLOR_MAP_PDI-1))
            ret = MAX;
        else
            ret = OK;

        ColorMapPdi(m_colorMapPdi, m_baselineIndex, m_turbIndex, m_invert, ret);
    }
    else //dir PDI
    {
        if (m_colorMapPdiDir == 0)
            ret = MIN;
        else if (m_colorMapPdiDir == (MAX_COLOR_MAP_PDIDIR-1))
            ret = MAX;
        else
            ret = OK;

        ColorMapPdiDir(m_colorMapPdiDir, m_baselineIndex, m_turbIndex, m_invert, ret);
    }

}
void ImgCfm::Turbo(int index, EKnobReturn ret)
{
    //ColorMap();
    m_ptrUpdate->Turbo(index, ret);
}
void ImgCfm::LineDensity(int index, EKnobReturn ret)
{
    m_ptrUpdate->LineDensity(index, ret);
    // waiting to be finished
}

void ImgCfm::FocPos(int focIndex)
{
    ASSERT(focIndex <= (MAX_CFM_FOC_RATIO - 1));

    float scale = m_ptrImg2D->GetScale2DInImgHDot();
    float ratio = CFM_FOC_RATIO[focIndex];
    int dot = m_boxDot[0] * ratio + m_boxDot[1] * (1 - ratio);
    float focPos = dot * scale;
    PRINTF("focPos = %f\n", focPos);
    int index = GetCfmFocus(focPos);

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    char type = m_ptrImg2D->ReviseProbeType(para.type);
      if (type == 'P'){
        if (m_cfmFocusIndex != index)
        {
            PRINTF("%s: calcEmit, index = %d\n", __FUNCTION__, index);
            m_ptrImg2D->CalcEmitDelayCfm(CFM_FOCUS[index]);
            m_cfmFocusIndex = index;
        }
        else {}
    }
    else
        m_ptrImg2D->CalcEmitDelayCfm(focPos);
}

void ImgCfm::SetTis(int prfIndex, int boxLineBegin, int boxLineEnd)
{
    int dopFreq = m_ptrImgPw->GetNameColorFreq();
    m_ptrImg2D->SetCfmTis(dopFreq, prfIndex, boxLineBegin, boxLineEnd);
}
EKnobReturn ImgCfm::FlowOpt(int index)
{
    m_tissueGate = GetFlowOptValue(index, 0);
    m_fpga.SendTissueGate((INT16U)m_tissueGate);

    m_noiseGate = GetFlowOptValue(index, 1);
    m_fpga.SendNoiseGate((INT16U)m_noiseGate);

    //m_speedGate = GetFlowOptValue(index, 2);
    char type = GlobalClassMan::GetInstance()->GetProbeType();
    if((type == 'p') || (type == 'P'))
        m_speedGate = 30;
    else
        m_speedGate = 10;
    m_fpga.SendSpeedGate((INT16U)m_speedGate);

    //m_varianceGate = GetFlowOptValue(index, 3);
    //m_fpga.SendVarianceGate((INT16U)m_varianceGate);
    SetVariance(m_gainCfm, index);

    m_gainGate = GetFlowOptValue(index, 4);
    m_ptrCalc->CalcGainGate(m_gainGate);

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_FLOW_OPT - 1))
        ret = MAX;
    else
        ret = OK;

    m_ptrUpdate->FlowOpt(index, ret);

    // only for test
#if 0
    m_ptrUpdate->TissueGate(m_tissueGate, OK);
    m_ptrUpdate->NoiseGate(m_noiseGate, OK);
    m_ptrUpdate->SpeedGate(m_speedGate, OK);
    //m_ptrUpdate->VarianceGate(m_varianceGate, OK);
    m_ptrUpdate->GainGate(m_gainGate, OK);
#endif

    return ret;
}

/*
 * @brief change cluster size, when one of depth, soundspeed, index of prf is changed
 * @para prf cfm[in]  prf
 * @para detph[in] depth of end of sample dot(box)
 */
bool ImgCfm::ClusterSizeNormal(int prf, int depthEndDot)
{
    double speed = m_ptrImg2D->GetSoundSpeed() * 1000; //m/s
    int depth = depthEndDot;
    int tDepth;

    int mbp = 1;
    if (m_ptrImg2D != NULL)
        mbp = m_ptrImg2D->GetMBPFpgaColor();
    m_clusterSize = m_ptrCalc->CalcClusterSize(prf, depth, speed, mbp, tDepth);

    // change Maxperiod
    m_ptrCalc->CalcMaxPeriod(prf, m_clusterSize);

    // change prf
    int prfMax = 1000000 / (float)tDepth;
    m_maxPRFIndex = GetPRFIndex(prfMax);
    PRINTF("tdepth = %d, prfmax = %d, m_maxPRFIndex  = %d\n", tDepth, prfMax, m_maxPRFIndex);
    if (m_PRFIndex > m_maxPRFIndex)
    {
        m_PRFIndex = m_maxPRFIndex;
        PRF(m_PRFIndex);
        return FALSE;
    }

    EKnobReturn ret = OK;
    if (m_PRFIndex == 0)
        ret = MIN;
    else if (m_PRFIndex == m_maxPRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRFRange(CFM_PRF[m_PRFIndex], ret);
    return TRUE;
}

void ImgCfm::ClusterSize(int prf)
{
    bool simult3 = m_ptrImgPw->GetSimut3Status();
    bool simult2 = m_ptrImgPw->GetSimut2Status();
    if ((!simult3) && (!simult2))
    {
        float scale = m_ptrImg2D->GetScale2DInImgHDot();
        int depth = scale * m_boxDot[1];
        ClusterSizeNormal(prf, depth);
    }
}

int ImgCfm::GetRealBaseline(int baselineIndex)
{
    double colorLen = COLOR_MAX - COLOR_MIN;
    int baseline = colorLen / 2 - colorLen / 2 * baselineIndex / MAX_BASELINE_INDEX;
    PRINTF("---------------baseline index = %d\n, baseline = %d\n", baselineIndex, baseline);

    if (baseline> COLOR_MAX)
        baseline = COLOR_MAX;
    else if (baseline < COLOR_MIN)
        baseline = COLOR_MIN;

    return baseline;
}
void ImgCfm::GetBoxLineRange(int &begin, int &end)
{
    int interval = 0;//mbp;
    int scanRange[2];

    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
        Zoom::GetInstance()->GetScanLineRange(scanRange);
    }
    else
    {
        m_ptrImg2D->GetDisplayScanRange(scanRange);
    }

    begin = scanRange[0] + interval;
    end = scanRange[1] - interval;
}
void ImgCfm::GetBoxDotRange(int &begin, int &end)
{
    int interval = 1;

    begin = 0 + interval;
#ifdef EMP_355
    end = IMG_H*Calc2D::INIT_SCALE - interval;
#else
    end = IMG_H - interval;
#endif
}

/*
 * @brief change box range when steer, make center of box is not changed
 */
void ImgCfm::Steer(void)
{
    //StopColorFlow();
    int centerDots = (m_boxDotBefore[1] + m_boxDotBefore[0]) / 2;
    int centerLines = (m_boxLine[1] + m_boxLine[0]) / 2;
    int dDots = (m_boxDotBefore[1] - m_boxDotBefore[0] + 1) / 2;
    int dLines = (m_boxLine[1] - m_boxLine[0] + 1) / 2;
    PRINTF("CENTER DOTS = %d, cener line = %d, dDots = %d, dLine = %d\n", centerDots, centerLines, dDots, dLines);

    int current, before;
    double angleCurrent, angleBefore, dAngle;
    before = m_ptrImgPw->GetSteerAngle(current);
    angleCurrent = current * PI / 180;
    angleBefore = before * PI / 180;
    dAngle = angleCurrent - angleBefore;

    float wLines = m_ptrImg2D->GetWidthBetweenLinesL();
    double scale = m_ptrImg2D->GetScale2DInImgHDot();
    double dPos = centerDots * scale;
    int nLines = dPos * sin(dAngle) / wLines;
    int nDots = centerDots * fabs(cos(angleBefore)) / cos(angleCurrent) - centerDots;
    centerDots = centerDots + nDots;
    centerLines = centerLines - nLines;

    int boxLineRange[2], boxDotRange[2];
    GetBoxLineRange(boxLineRange[0], boxLineRange[1]);
    GetBoxDotRange(boxDotRange[0], boxDotRange[1]);
    int mbp = m_ptrImg2D->GetMBP();

    PRINTF("m_boxLine(%d, %d), m_boxDot(%d, %d)\n", m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1]);
    PRINTF("cener line = %d, dLine = %d, boxLineRange(%d, %d)\n", centerLines, dLines, boxLineRange[0], boxLineRange[1]);
    if (((centerLines + dLines) <= boxLineRange[1]) && ((centerLines - dLines) >= boxLineRange[0]))
    {
        m_boxLine[1] = centerLines + dLines;
        m_boxLine[0] = centerLines - dLines;

        m_boxLine[0] = m_boxLine[0] / mbp * mbp;
        m_boxLine[1] = m_boxLine[1] / mbp * mbp;
    }

    if (((centerDots + dDots) <= boxDotRange[1]) && ((centerDots - dDots) >= boxDotRange[0]))
    {
        m_boxDotBefore[1] = centerDots + dDots;
        m_boxDotBefore[0] = centerDots - dDots;
        BoxDotCorrect(m_boxDotBefore[0], m_boxDotBefore[1]);
    }

    Box(m_boxLine[0], m_boxLine[1], m_boxDot[0], m_boxDot[1], FALSE);
}

EKnobReturn ImgCfm::WallFilter(int indexWallFilter, int indexPktsize)
{
    /*unsigned short* filter;
    const int SIZE = 26*32;
    PRINTF("indexwallfilter=%d  , indexPktsize= %d \n",indexWallFilter, indexPktsize);
    filter = (unsigned short*)WALL_FILTER + (indexPktsize * MAX_WALLFILTER_INDEX_ALL + indexWallFilter) * SIZE;

    m_ptrCalc->CalcWallFilter(filter, SIZE);*/
    m_ptrCalc->CalcWallFilter(indexWallFilter, PACKSIZE[indexPktsize]);

    EKnobReturn ret;
    if (m_wallFilterIndex == 0)
        ret = MIN;
    else if (m_wallFilterIndex == (MAX_WALLFILTER_INDEX - 1))
        ret = MAX;
    else
        ret = OK;

    m_ptrUpdate->WallFilter(m_wallFilterIndex, ret);

    return ret;
}

/*
 * @brief get end of wall filter freq (hz)
 */
int ImgCfm::GetWFDisplay(int prfIndex, int pktIndex, int wfIndex)
{
    int wf = 0;

    if (prfIndex >= MAX_PRF_INDEX)
        return 0;

    wf = (float)CFM_PRF[prfIndex] / 2 * WALLFILTER_DISPLAY[pktIndex][wfIndex];
    PRINTF("prfIndex = %d, pktIndex = %d, wfIndex = %d\n", prfIndex, pktIndex, wfIndex);

    return wf;
}

/*
 * @brief get PRF index, which PRF is similar to prf
 *
 * @para prf reference of PRF, unit: hz
 *
 * @retval index of PRF
 */
int ImgCfm::GetPRFIndex(int prf) //unit:hz
{
    int i;
    for (i = 0; i < MAX_PRF_INDEX; i ++)
    {
        if (CFM_PRF[i] == prf)
        {
            break;
        }
        else if (CFM_PRF[i] > prf)
        {
            i--;
            break;
        }
    }

    if (i > MAX_PRF_INDEX-1)
        i = MAX_PRF_INDEX-1;
    else if (i < 0)
        i = 0;

    PRINTF("get index of prf, prf = %d\n", CFM_PRF[i]);

    return i;
}

EKnobReturn ImgCfm::UpdatePRFRangeStatus(int index, int maxIndex)
{
    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == maxIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(CFM_PRF[index], ret);

    return ret;
}

void ImgCfm::PRFSimult3(int prf, EKnobReturn ret)
{
    m_PRFIndex = GetPRFIndex(prf);
    WhenPRFChanged(m_PRFIndex);

    // update view
    m_ptrUpdate->PRF(CFM_PRF[m_PRFIndex], ret);
}

void ImgCfm::ExitSimult3()
{
   PRF(m_PRFIndex);
}
void ImgCfm::BalancePwInfo(void)
{
    int pwLine, dotBegin, dotEnd, dotSvLen;
    m_ptrImgPw->GetPwInfo(pwLine, dotBegin, dotEnd, dotSvLen);

    PRINTF("balance pw before: cfm lineb = %d, linee = %d, pw line = %d\n", m_boxLine[0], m_boxLine[1], pwLine);
    bool outRange = FALSE;
    if (pwLine < m_boxLine[0])
    {
        pwLine = m_boxLine[0];
        outRange = TRUE;
    }
    else if (pwLine > m_boxLine[1])
    {
        pwLine = m_boxLine[1];
        outRange = TRUE;
    }

    if (dotBegin < m_boxDotBefore[0])
    {
        dotBegin = m_boxDotBefore[0];
        dotEnd = dotBegin + dotSvLen;
        outRange = TRUE;
    }
    if (dotEnd > m_boxDotBefore[1])
    {
        dotEnd = m_boxDotBefore[1];
        dotBegin = dotEnd - dotSvLen;
        outRange = TRUE;
    }

    PRINTF("balance pw after: cfm lineb = %d, linee = %d, pw line = %d\n", m_boxLine[0], m_boxLine[1], pwLine);
    if (outRange)
        m_ptrImgPw->SetPwInfo(pwLine, dotBegin, dotEnd);
}

void ImgCfm::SetVariance(int cfmGain, int flowOptIndex)
{
    const int MAXVAR = 16380;
    const int THRESHOLDGAIN = 70;//80;

    int variance = GetFlowOptValue(flowOptIndex, 3);
    if (cfmGain >= THRESHOLDGAIN)
    {
       variance += (MAXVAR - variance) * (cfmGain - THRESHOLDGAIN) / (100 - THRESHOLDGAIN);
    }

    m_varianceGate = variance;
    m_fpga.SendVarianceGate((INT16U)m_varianceGate);

    m_ptrUpdate->VarianceGate(m_varianceGate, OK);
}

void ImgCfm::BoxDotCorrect(int dotBegin, int dotEnd)
{
    int dotB = dotBegin;
    int dotE = dotEnd;

    Zoom* ptrZoom = Zoom::GetInstance();
   if (ptrZoom->GetLocalZoomStatus())
   {
       int dotRange[2];
       double scale;
       ptrZoom->GetScanDotRange(dotRange);
       ptrZoom->GetLocalZoomScale(scale);

       dotB = (float)dotB / scale + dotRange[0];
       dotE = (float)dotE / scale + dotRange[0];
       PRINTF("dot correct: scale = %f, dotBegin = %d, dotEnd = %d, dotRange[0] = %d, dotCorrectB = %d, dotCorrectE = %d\n", scale, dotBegin, dotEnd, dotRange[0], dotB, dotE);
   }

    m_boxDot[0] = dotB;
    m_boxDot[1] = dotE;
}

int ImgCfm::GetFlowOptValue(int flowIndex, int itemIndex)
{
    ASSERT(flowIndex < MAX_FLOW_OPT);
    ASSERT(itemIndex < 5);

    int value;
    if (m_colorMode == COLOR_CFM)
    {
        value = FLOW_OPT[flowIndex][itemIndex];
    }
    else
    {
        value = FLOW_OPT_PDI[flowIndex][itemIndex];
    }

    PRINTF("flowIndex = %d, itemIndex = %d, value = %d\n", flowIndex, itemIndex, value);

    return value;
}

/*
 * divde cfm focus three section
 * @para focPos[in] current pw focus pos, unit:mm
 */
int ImgCfm::GetCfmFocus(float focusPos)
{
    int index;

    if (focusPos < 40.0)
    {
        index = 0;
    }
    else if ((focusPos >=40.0) && (focusPos <120.0))
    {
        index = 1;
    }
    else
    {
        index = 2;
    }

    return index;
}

///>test
void ImgCfm::ChangeLowFilterFc(EKnobOper oper)
{
    float step = 0.2;
    if(oper == ADD)
    {
        if(m_fcLowFilter + step < 12.0)
            m_fcLowFilter += step;
    }
    else if(oper == SUB)
    {
        if(m_fcLowFilter - step > 0.1)
            m_fcLowFilter -= step;
    }

    m_ptrCalc->CalcLowFilter(m_fcLowFilter);

    UpdateHint();
}

void ImgCfm::UpdateHint()
{
    int pulsenum = ScanMode::GetInstance()->GetCFMPulseNum();
    int bplist = ImgPw::GetInstance()->GetBandPassFilterList();
    char str_bplist[12]={'\0'};
    switch(bplist)
    {
        case 1:
            strcpy(str_bplist, "04");
            break;
        case 2:
            strcpy(str_bplist, "05");
            break;
        case 3:
            strcpy(str_bplist, "06");
            break;
        case 4:
            strcpy(str_bplist, "07");
            break;
        case 5:
            strcpy(str_bplist, "08");
            break;
        case 6:
            strcpy(str_bplist, "04bp");
            break;
        case 7:
            strcpy(str_bplist, "05bp");
            break;
        case 8:
            strcpy(str_bplist, "08bp");
            break;
    }

    char str_filter[256];
    HintArea::GetInstance()->ClearHint();
    sprintf(str_filter, "  ===bplist: %s,  ===pulseNum: %d,  ===fcLowFilter: %.1f", str_bplist, pulsenum, m_fcLowFilter);
    HintArea::GetInstance()->UpdateHint(str_filter);
}

void ImgCfm::StopColorFlow()
{
    if (m_timerOpenColorFlow == 0)
    {
        m_colorThresholdBak = m_ptrDscPara->dcaCFMIPAttrs.ipaThreshold;
        m_ptrDscPara->dcaCFMIPAttrs.ipaThreshold = 0;
        m_colorPersistBak = m_ptrDscPara->dcaCFMIPAttrs.ipaFrameAver;
        m_ptrDscPara->dcaCFMIPAttrs.ipaFrameAver = 64;
    }

    if (m_timerOpenColorFlow > 0)
    {
        g_source_remove(m_timerOpenColorFlow);
        m_timerOpenColorFlow = 0;
    }

    m_timerOpenColorFlow = g_timeout_add(250, TimeOutOpenColorFlow, NULL);
}

void ImgCfm::OpenColorFlow()
{
    m_ptrDscPara->dcaCFMIPAttrs.ipaThreshold = m_colorThresholdBak;
    m_ptrDscPara->dcaCFMIPAttrs.ipaFrameAver = m_colorPersistBak;
    m_timerOpenColorFlow = 0;
}