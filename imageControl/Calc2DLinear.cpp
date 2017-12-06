#include "imageControl/Calc2DLinear.h"
#include "Def.h"
#include "probe/ProbeSocket.h"
#include "probe/ProbeMan.h"
#include "imageControl/Img2D.h"
#include "display/TopArea.h"

///> static varible init

int Calc2DLinear::EMIT_CH_NUM_L[20] =            {8, 10, 16, 24, 27, 30, 34,38, 42,  48,  64, 64, 64,64, 64,64, 64, 64, 64, 64};
const int Calc2DLinear::EMIT_CH_NUM_55L60G[20] = {4,  5,  8, 12, 14, 15, 17, 19, 21, 24, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};        //55L60
const int Calc2DLinear::EMIT_CH_NUM_10L25K[20] = {12,18, 20, 24, 32, 48, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};        //10L25

const int Calc2DLinear::EMIT_CH_NUM_70L40J[20] =            {8, 10, 16, 24, 27, 30, 34,38, 42,  48,  64, 64, 64,64, 64,64, 64, 64, 64, 64};        //70L40J

 int Calc2DLinear::EMIT_CH_NUM_L_COLOR[20] =    {14,18, 24, 30, 36, 42, 48, 56, 60, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};
const int Calc2DLinear::EMIT_CH_NUM_L_COLOR_70L40J[20] =    {14,18, 24, 30, 36, 42, 48, 56, 60, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};
const int Calc2DLinear::EMIT_CH_NUM_L_COLOR_55L60G[20] =    {7,9, 12, 15, 18, 21, 24,   28, 30, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
const int Calc2DLinear::TGC_CTL[4] = {90, 140, 9, 60};

float Calc2DLinear::TIS_CFM_BASIC[2][12][11]=
{
    {//5.9Mhz
        //
        {0.0124, 0.0286, 0.0476, 0.0762, 0.1048, 0.1333, 0.1619, 0.1810, 0.2543, 0.3400, 0.4162},
        {0.0334, 0.0771, 0.1286, 0.2057, 0.2829, 0.3600, 0.4371, 0.4886, 0.6866, 0.9180, 1.1237},
        {0.0495, 0.1143, 0.1905, 0.3048, 0.4190, 0.5333, 0.6476, 0.7238, 1.1071, 1.3600, 1.6648},
        {0.0532, 0.1229, 0.2048, 0.3276, 0.4505, 0.5733, 0.6962, 0.7781, 1.0934, 1.4620, 1.7896},
        {0.0619, 0.1429, 0.2381, 0.3810, 0.5238, 0.6667, 0.8095, 0.9048, 1.2714, 1.7000, 2.0810},
        {0.0619, 0.1429, 0.2381, 0.3810, 0.5238, 0.6667, 0.8095, 0.9048, 1.2714, 1.7000, 2.0810},
        {0.0656, 0.1514, 0.2524, 0.4038, 0.5552, 0.7067, 0.8581, 0.9590, 1.3477, 1.8020, 2.2058},
        {0.0656, 0.1514, 0.2524, 0.4038, 0.5552, 0.7067, 0.8581, 0.9590, 1.3477, 1.8020, 2.2058},
        {0.0656, 0.1514, 0.2524, 0.4038, 0.5552, 0.7067, 0.8581, 0.9590, 1.3477, 1.8020, 2.2058},
        {0.0706, 0.1629, 0.2714, 0.4343, 0.5971, 0.7600, 0.9229, 1.0314, 1.4494, 1.9380, 2.3723},
        {0.0706, 0.1629, 0.2714, 0.4343, 0.5971, 0.7600, 0.9229, 1.0314, 1.4494, 1.9380, 2.3723},
        {0.0743, 0.1714, 0.2857, 0.4571, 0.6286, 0.8000, 0.9714, 1.0857, 1.5257, 2.0400, 2.4971},
    },
    {//7.2Mhz
        {0.0194, 0.0342, 0.0685, 0.1107, 0.1483, 0.1826, 0.2202, 0.2590, 0.3537, 0.4712, 0.5705},
        {0.0524, 0.0924, 0.1848, 0.2988, 0.4005, 0.4929, 0.5946, 0.6993, 0.9550, 1.2723, 1.5403},
        {0.0776, 0.1369, 0.2738, 0.4427, 0.5933, 0.7302, 0.8808, 1.0360, 1.4148, 1.8849, 2.2819},
        {0.0834, 0.1472, 0.2944, 0.4759, 0.6378, 0.7850, 0.9469, 1.1137, 1.5209, 2.0262, 2.4530},
        {0.0970, 0.1711, 0.3423, 0.5534, 0.7416, 0.9128, 1.1010, 1.2950, 1.7685, 2.3561, 2.8524},
        {0.0970, 0.1711, 0.3423, 0.5534, 0.7416, 0.9128, 1.1010, 1.2950, 1.7685, 2.3561, 2.8524},
        {0.1028, 0.1814, 0.3628, 0.5866, 0.7861, 0.9675, 1.1671, 1.3727, 1.8746, 2.4974, 3.0235},
        {0.1028, 0.1814, 0.3628, 0.5866, 0.7861, 0.9675, 1.1671, 1.3727, 1.8746, 2.4974, 3.0235},
        {0.1028, 0.1814, 0.3628, 0.5866, 0.7861, 0.9675, 1.1671, 1.3727, 1.8746, 2.4974, 3.0235},
        {0.1106, 0.1951, 0.3902, 0.6308, 0.8454, 1.0405, 1.2552, 1.4763, 2.0161, 2.6859, 3.2517},
        {0.1106, 0.1951, 0.3902, 0.6308, 0.8454, 1.0405, 1.2552, 1.4763, 2.0161, 2.6859, 3.2517},
        {0.1106, 0.1951, 0.3902, 0.6308, 0.8454, 1.0405, 1.2552, 1.4763, 2.0161, 2.6859, 3.2517},
    },
};
float Calc2DLinear::TIS_CFM_POWER[10] =
{
    0.0811, 0.0811, 0.2703, 0.2703, 0.4595, 0.6216, 0.6216, 1.0811, 1.0811, 1.0000,
};
float Calc2DLinear::TIS_CFM_BOX_LINES[8] =
{
    0.850, 1.000, 0.956, 0.837, 0.780, 0.692, 0.573, 0.5286,
};
float Calc2DLinear::TIS_2D_BASIC[13] = // foc pos
{
    0.0489, 0.1319, 0.1954, 0.2101, 0.2443, 0.2443, 0.2589, 0.2589, 0.2589, 0.2785, 0.2785, 0.2931, 0.3078,
};
float Calc2DLinear::TIS_2D_FREQ[5] = //2.0 2.5 ... 6.0
{
    0.5651, 0.9501, 1.0000, 1.0000, 1.2919,
};
float Calc2DLinear::TIS_2D_POWER[10] =
{
    0.0811, 0.0811, 0.2703, 0.2703, 0.4595, 0.6216, 0.6216, 1.0811, 1.0811, 1.0000,
};
float Calc2DLinear::TIS_2D_DEPTH[20] =
{
    2.4953, 2.5607, 2.4299, 2.1215, 2.2430, 2.0561, 1.9907, 1.9907, 1.9346, 1.7477, 1.6262, 1.4953, 1.4953, 1.3738, 1.2430, 1.1869,
    1.1869, 1.0000, 1.0000, 1.0000,
};
float Calc2DLinear::TIS_PW_BASIC[2][15] = //freq-prf
{
    {0.0740, 0.1159, 0.1973, 0.2393, 0.3034, 0.3626, 0.4193, 0.4613, 0.5180, 0.6093, 0.6660, 0.7079, 0.7893, 0.8387, 0.8880},
    {0.0977, 0.1865, 0.2664, 0.3256, 0.4055, 0.5032, 0.5713, 0.6423, 0.7489, 0.8081, 0.8265, 0.9679, 1.0597, 1.1248, 1.2047},
};

///>
Calc2DLinear::Calc2DLinear()
{
#if defined(EMP_355)
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if (strcmp("55L60G", para.model)==0)
    {
        printf("------------55L60G\n");
        for (int i=0;i<20;i++)
        {
            EMIT_CH_NUM_L[i] = EMIT_CH_NUM_55L60G[i];
            EMIT_CH_NUM_L_COLOR[i] = EMIT_CH_NUM_L_COLOR_55L60G[i];
        }
    }
    else if ((strcmp("10L25K",para.model)==0))
    {
        for (int i=0;i<20;i++)
        {
            EMIT_CH_NUM_L[i] = EMIT_CH_NUM_10L25K[i];
            EMIT_CH_NUM_L_COLOR[i] = EMIT_CH_NUM_L_COLOR_70L40J[i];

        }
    }
    else
    {
        for (int i=0;i<20;i++)
        {
            EMIT_CH_NUM_L[i] = EMIT_CH_NUM_70L40J[i];
            EMIT_CH_NUM_L_COLOR[i] = EMIT_CH_NUM_L_COLOR_70L40J[i];
        }

    }
#else
    for (int i=0;i<20;i++)
        {
            EMIT_CH_NUM_L[i] = EMIT_CH_NUM_70L40J[i];
            EMIT_CH_NUM_L_COLOR[i] = EMIT_CH_NUM_L_COLOR_70L40J[i];
        }
#endif
}

Calc2DLinear::~Calc2DLinear()
{

}

void Calc2DLinear::CalcEmitDelay()
{
    int focSum = m_calcPara->focSum;
    int size = sizeof(EMIT_CH_NUM_L) / sizeof(int);
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;

    for (int i = 0; i < focSum; i ++)
    {
        LEmitDelay(i, EMIT_CH_NUM_L, size, compound);
    }
}
void Calc2DLinear::CalcEmitDelayPw(float focPos)
{
    int size = sizeof(EMIT_CH_NUM_L) / sizeof(int);
    LEmitDelayPw(focPos, EMIT_CH_NUM_L, size);
}
void Calc2DLinear::CalcEmitDelayCfm(float focPos)
{
    int size = sizeof(EMIT_CH_NUM_L) / sizeof(int);
    LEmitDelayCfm(focPos, EMIT_CH_NUM_L_COLOR, size);
}
void Calc2DLinear::CalcReceiveDelay()
{
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;
    LReceiveDelay(compound);
}
void Calc2DLinear::CalcReceiveDelayColor()
{
    //LReceiveDelayColor(); //lihuamei 2012.08.06
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;
    LReceiveDelayColor(compound);

}

void Calc2DLinear::CalcReceiveAperture()
{
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;
    LReceiveAperture(compound);
}

/*
 * @brief calc tgc curve of convex
 */
void Calc2DLinear::CalcTgc(int gain, int tgcY[], AbsUpdate2D* ptrUpdate, int section)
{
    ///> calc
    if ((section == 0) || (section == 1))
        Tgc(TGC_X, gain, tgcY, TGC_CTL, ptrUpdate, section);
    else
        TgcColor(TGC_X, gain, tgcY, TGC_CTL, ptrUpdate, section);
}

void Calc2DLinear::CalcFocPos()
{
    int start = 4;

    FocChange(start);

    ///> 2D pulse number according to last focus pos
    //m_fpga.Send2DPulseNum(3);
    m_fpga.Send2DPulseNum(1);
   // m_fpga.Send2DPulseNum(2);

    CalcEmitDelay();
}

bool Calc2DLinear::CalcFocPulse()
{
    int freq = m_calcPara->freq.emit;
    int power = m_calcPara->power;

    return FocPulse(power, freq);
}

void Calc2DLinear::CalcFocPulseFreqCompound(int order)
{
    int freq = m_calcPara->freq.emit;
    int power = m_calcPara->power;

    FocPulseFreqCompound(power, freq, order);
}

/*
 * @brief calc log curve of convex probe
 *
 * @para freq frequency*20 of probe
 */
void Calc2DLinear::CalcLog()
{
    float value = LOG_VALUE_DEFAULT;

    Log(value);
}

void Calc2DLinear::CalcFilter()
{
    CalcDynamicDemod(0);
    CalcFilterBandPass(0);
    CalcDynamicFilter(0);
}

void Calc2DLinear::CalcDefaultDemodFd()
{
    int count = 0;
    if(!m_calcPara->harmonic)
    {
        count = 1;
        m_calcPara->harmonic = TRUE;
    }
    CalcDynamicDemod(0);
    if(count == 1)
        m_calcPara->harmonic = FALSE;
}

void Calc2DLinear::CalcFilterFreqCompound(int order)
{
    CalcDynamicDemod(order);
    CalcFilterBandPass(order);
    CalcDynamicFilter(order);
}
float Calc2DLinear::CalcImgStartDepth()
{
    return 0.0;
}
void Calc2DLinear::CalcDisplayScanRange(int range[2], int widthInDots)
{
    float lineTotal = (float)m_calcPara->probeLines;
    int lineHalf = lineTotal / 2;
    double scale = m_calcPara->depth / (double)m_calcPara->depthDots;
    int width = (m_calcPara->probeWidth / 100) / scale; //dots

    if (width > widthInDots)
    {
        double dline = width / lineTotal; //dots/line

        range[0] = lineHalf - widthInDots /  dline / 2;
        range[1] = lineHalf + widthInDots / dline / 2;
    }
    else
    {
        GetScanRange(range);
    }
}

void Calc2DLinear::CalcWeightingEmit(int pulseWidth)
{
    unsigned char weightingValue[APERTURE_HALF*2];
    int value = 400;//200;

    WeightingEmit(pulseWidth, value, weightingValue);
    m_fpga.SendPulseWidth(weightingValue, APERTURE_HALF*2);
}
void Calc2DLinear::CalcWeightingEmitColor(int pulseWidth)
{
    unsigned char weightingValue[APERTURE_HALF*2];
    int value = 1400;

    WeightingEmit(pulseWidth, value, weightingValue);
    m_fpga.SendColorPulseWidth(weightingValue, APERTURE_HALF*2);
}
void Calc2DLinear::CalcExtendedImagingSample(void)
{
    ExtendedImagingSampleL();
}

float Calc2DLinear::Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex)
{
    depthIndex = Get2DDepthIndex(depthIndex);
    int powerIndex = GetPowerIndex(power);
    int focIndex = GetFocIndex(focPos);
    int freqIndex = Get2DFreqReceiveIndex(freqReceive);

  //PRINTF("enter the Calc2DLine ,m_sacnAngleIndex = %d ,m_focPos = %.1f,m_freq.revice = %d ,powerdata = %d,depthInde = %d\n"
            //,scanAngleIndex, focPos ,freqReceive, power, depthIndex);

    float ti = TIS_2D_BASIC[focIndex] * TIS_2D_POWER[powerIndex] * TIS_2D_FREQ[freqIndex] * TIS_2D_DEPTH[depthIndex];
    //PRINTF("enter the calc2Dline ,2D TIS = %f\n", ti);

    return ti;
}
float Calc2DLinear::GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd)
{
    prfIndex = GetCfmPrfIndex(prfIndex);
    int freqIndex = GetColorFreqIndex(dopFreq);
    int focPosIndex = GetFocIndex(focPos);
    int boxLineIndex = GetCfmLinesIndex(boxLineBegin, boxLineEnd);
    int powerIndex = GetPowerIndex(power);
    if (focPosIndex > 11)
        focPosIndex = 11;

    float ti = TIS_CFM_BASIC[freqIndex][focPosIndex][prfIndex] * TIS_CFM_POWER[powerIndex] * TIS_CFM_BOX_LINES[boxLineIndex];
    PRINTF("CFM TI = %f\n", ti);

    return ti;
}
float Calc2DLinear::GetPwTis(int dopFreq, int prfIndex)
{
    prfIndex = GetPwPrfIndex(prfIndex);
    int freqIndex = GetColorFreqIndex(dopFreq);

    float ti = TIS_PW_BASIC[freqIndex][prfIndex];
    PRINTF("PW TI = %f\n", ti);

    return ti;
}

///> private
void Calc2DLinear::CalcFilterBandPass(int order)
{
    int depth[5];
    int probeIndex= 0;
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    string probe_type = TopArea::GetInstance()->GetProbeType();
    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

    depth[0] = 20;//40;
    depth[1] = 50;//60;
    depth[2] = 80;
    depth[3] = 100;
    depth[4] = 120;

#if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) || (defined(EMP_161)) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
    float fc_bandpass_filter1[5], fc_bandpass_filter2[5];

        if(m_calcPara->harmonic)
        {
            for(int i = 0; i < 5; i++)
            {
                fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] / 10.0);
                fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i] / 10.0);
                PRINTF("-Linear--%d---THI---w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
            }
        }
        else
        {
            for(int i = 0; i < 5; i++)
            {
                fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                PRINTF("-Linear--%d----w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
            }
        }

    BandPassFilterSelect_test(fc_bandpass_filter1, fc_bandpass_filter2, depth, order);

#else
   float fc_bandpass_filter[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
            fc_bandpass_filter[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
    }
    else
    {
        for(int i = 0; i < 5; i++)
            fc_bandpass_filter[i] = float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
    }
    BandPassFilterSelect_test(fc_bandpass_filter, depth, order);

    //BandPassFilterSelect(order);
#endif

}

void Calc2DLinear::CalcDynamicFilter(int order)
{
    int depth[5];
    int probeIndex = 0;
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    string probe_type = TopArea::GetInstance()->GetProbeType();
    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

    depth[0] = 20;//40;
    depth[1] = 50;//60;
    depth[2] = 80;
    depth[3] = 100;
    depth[4] = 120;

    float fc_dynamic_filter[5];

        if(m_calcPara->harmonic)
        {
            for(int i = 0; i < 5; i++)
            {
                fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
                PRINTF("-Linear--%d--THI---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
            }
        }
        else
        {
            for(int i = 0; i < 5; i++)
            {
                fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                PRINTF("-Linear--%d---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
            }
        }
  //  }
    DynamicFilter_test(fc_dynamic_filter, depth, order);

}

void Calc2DLinear::CalcDynamicDemod(int order)
{
    int freq = m_calcPara->freq.receive;
    int depth[5];
    float fd[5];
    bool harmonic = m_calcPara->harmonic;
    float fd_harmonic[5];

    depth[0] = 20;//40;
    depth[1] = 50;//60;
    depth[2] = 80;
    depth[3] = 100;
    depth[4] = 120;

    if(freq < 100)
    {
#ifdef EMP_430
        fd[0] = 7.5;
        fd[1] = 6.5;
        fd[2] = 5.0;
        fd[3] = 3.2;//5.2;
        fd[4] = 3.0;//5.0;
        fd_harmonic[0] = 8.5;
        fd_harmonic[1] = 8.0;
        fd_harmonic[2] = 7.5;
        fd_harmonic[3] = 7.0;
        fd_harmonic[4] = 6.5;
#elif (EMP_161 || EMP_360)
        fd[0] = 7.0;
        fd[1] = 6.5;
        fd[2] = 6.2;
        fd[3] = 5.2;//5.2;
        fd[4] = 4.0;//5.0;
        fd_harmonic[0] = 9.2;
        fd_harmonic[1] = 8.8;
        fd_harmonic[2] = 7.5;
        fd_harmonic[3] = 6.5;
        fd_harmonic[4] = 6.0;
#else
        fd[0] = 7.0;
        fd[1] = 6.6;
        fd[2] = 5.8;
        fd[3] = 3.2;//5.2;
        fd[4] = 2.2;//5.0;
        fd_harmonic[0] = 8.5;
        fd_harmonic[1] = 8.5;
        fd_harmonic[2] = 8.0;
        fd_harmonic[3] = 7.7;
        fd_harmonic[4] = 7.5;
#endif
    }
    else if(freq >= 100 && freq < 115)
    {
#ifdef EMP_430
        fd[0] = 7.5;
        fd[1] = 6.5;
        fd[2] = 5.0;
        fd[3] = 3.2;//5.2;
        fd[4] = 3.0;//5.0;
        fd_harmonic[0] = 9.3;
        fd_harmonic[1] = 8.5;
        fd_harmonic[2] = 8.0;
        fd_harmonic[3] = 7.5;
        fd_harmonic[4] = 7.5;
#elif (EMP_161 || EMP_360)
        fd[0] = 7.0;
        fd[1] = 6.5;
        fd[2] = 5.5;//6.2;
        fd[3] = 4.0;//4.5;//5.2;//5.2;
        fd[4] = 4.0;//5.0;
        fd_harmonic[0] = 10.0;
        fd_harmonic[1] = 9.0;
        fd_harmonic[2] = 8.5;
        fd_harmonic[3] = 7.5;
        fd_harmonic[4] = 6.5;
#else
        fd[0] = 7.0;
        fd[1] = 6.6;
        fd[2] = 5.8;
        fd[3] = 3.2;//5.2;
        fd[4] = 2.2;//5.0;
        fd_harmonic[0] = 9.2;
        fd_harmonic[1] = 8.7;
        fd_harmonic[2] = 8.1;
        fd_harmonic[3] = 8.1;
        fd_harmonic[4] = 7.9;
#endif

    }
    else if (freq>=115 && freq<130)
    {
#ifdef EMP_430
        fd[0] = 7.5;
        fd[1] = 6.5;
        fd[2] = 5.0;
        fd[3] = 3.2;//5.2;
        fd[4] = 3.0;//5.0;
        fd_harmonic[0] = 11.5;
        fd_harmonic[1] = 11.0;
        fd_harmonic[2] = 10.9;
        fd_harmonic[3] = 10.9;
        fd_harmonic[4] = 11.5;
#elif (EMP_161 || EMP_360)
        fd[0] = 7.5;
        fd[1] = 7.0;
        fd[2] = 6.5;
        fd[3] = 6.0;//5.2;
        fd[4] = 5.0;//5.0;
        fd_harmonic[0] = 11.5;
        fd_harmonic[1] = 11.0;
        fd_harmonic[2] = 10.5;
        fd_harmonic[3] = 10.0;
        fd_harmonic[4] = 9.0;
#else
    fd[0] = 8.0;//9.0;
    fd[1] = 8.0;
    fd[2] = 5.0;//7.0;
    fd[3] = 3.0;//5.6;
    fd[4] = 2.6;//5.2;
        fd_harmonic[0] = 10.7;
        fd_harmonic[1] = 10.3;
        fd_harmonic[2] = 10.9;
        fd_harmonic[3] = 11.0;
        fd_harmonic[4] = 10.8;
#endif
    }
    else if (freq>=130 && freq<150)
    {
#ifdef EMP_430
        fd[0] = 8.0;//7.5;//9.0;//10.2;// 6.2;
        fd[1] = 6.8;//7.0;//8.0;//8.6;// 5.5;
        fd[2] = 3.8;//6.5;//5.0;//7.2;// 5.3;
        fd[3] = 3.2;//6.0;//2.9;//5.6;// 5.1;
        fd[4] = 3.0;//5.0;//2.4;//5.2;// 3.6;
#elif (EMP_161 || EMP_360)
        fd[0] = 7.5;
        fd[1] = 7.0;
        fd[2] = 6.5;
        fd[3] = 6.0;//5.2;
        fd[4] = 5.0;//5.0;
#else
        fd[0] = 9.0;//10.2;// 6.2;
    fd[1] = 8.0;//8.6;// 5.5;
    fd[2] = 5.0;//7.2;// 5.3;
    fd[3] = 2.9;//5.6;// 5.1;
    fd[4] = 2.4;//5.2;// 3.6;
#endif
        fd_harmonic[0] = 10.7;
        fd_harmonic[1] = 10.3;
        fd_harmonic[2] = 10.9;
        fd_harmonic[3] = 11.0;
        fd_harmonic[4] = 10.8;
    }
    else if (freq>=150 && freq<180)
    {
#ifdef EMP_430
        fd[0] = 8.8;
        fd[1] = 7.2;
        fd[2] = 4.4;
        fd[3] = 3.4;
        fd[4] = 3.2;//3.2;//5.5;
#elif (EMP_161 || EMP_360)
        fd[0] = 9.0;
        fd[1] = 8.5;
        fd[2] = 7.5;
        fd[3] = 6.5;
        fd[4] = 5.0;
#else
        fd[0] = 11.0;
        fd[1] = 9.0;
        fd[2] = 7.2;
        fd[3] = 5.6;
        fd[4] = 5.2;
#endif
        fd_harmonic[0] = 10.7;
        fd_harmonic[1] = 10.3;
        fd_harmonic[2] = 10.9;
        fd_harmonic[3] = 11.0;
        fd_harmonic[4] = 10.8;
    }
    else if (freq >= 180 && freq < 240)
    {
        fd[0] = 9.5;//11.6;
        fd[1] = 9.0;//9.2;
        fd[2] = 8.0;//7.2;
        fd[3] = 7.0;
        fd[4] = 5.5;
        fd_harmonic[0] = 10.7;
        fd_harmonic[1] = 10.3;
        fd_harmonic[2] = 10.9;
        fd_harmonic[3] = 11.0;
        fd_harmonic[4] = 10.8;
    }
    else
    {
        fd[0] = 11.5;//11.6;
        fd[1] = 11.0;//9.2;
        fd[2] = 9.5;//7.2;
        fd[3] = 7.5;
        fd[4] = 5.0;
        fd_harmonic[0] = 10.7;
        fd_harmonic[1] = 10.3;
        fd_harmonic[2] = 10.9;
        fd_harmonic[3] = 11.0;
        fd_harmonic[4] = 10.8;
    }

        int freqIndex = Img2D::GetInstance()->GetFreqIndex();
        int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
        int probeIndex = 0;
        int i;
        string probe_type = TopArea::GetInstance()->GetProbeType();
        for (i = 0; i < NUM_PROBE; ++i)
        {
            if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
                probeIndex = i;
        }
#ifdef EMP_PROJECT
        if (m_projectCalcPara->filterSecIndex)
        {
            for(i = 0; i < 5; i++)
            {
                  fd[i] =  float(m_projectCalcPara->bandPassW[i] / 10.0);
                  fd_harmonic[i] =  float(m_projectCalcPara->bandPassW[i] / 10.0);
                    PRINTF("----- project test --Linear--%d----demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
            }
        }
        else
        {
            for (i = 0; i < 5; i++)
            {
                // fd[i] =  float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                // fd_harmonic[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
                PRINTF("-Linear--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
            }
        }
#else
    for (i = 0; i < 5; i++)
    {
        fd[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
        fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
        PRINTF("-Linear--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
    }
#endif

    if(harmonic)
    {
        if (order == 0)
            DynamicDemod(fd_harmonic, depth, TRUE);
        else
            DynamicDemodFreqCompound(fd_harmonic, depth, TRUE, order);

    }
    else
    {
        if (order == 0)
            DynamicDemod(fd, depth, TRUE);
        else
            DynamicDemodFreqCompound(fd, depth, TRUE, order);
    }
}

int Calc2DLinear::GetFocIndex(float focPos)
{
    int focIndex = 0;

    if (focPos <= 0.5)
        focIndex = 0;
    else if ((focPos > 0.5) && (focPos <= 1.0))
        focIndex = 1;
    else if ((focPos > 1.0) && (focPos <= 1.5))
        focIndex = 2;
    else if ((focPos > 1.5) && (focPos <= 2.0))
        focIndex = 3;
    else if ((focPos > 2.0) && (focPos <= 2.5))
        focIndex = 4;
    else if ((focPos > 2.5) && (focPos <= 3.0))
        focIndex = 5;
    else if ((focPos > 3.0) && (focPos <= 3.5))
        focIndex = 6;
    else if ((focPos > 3.5) && (focPos <= 4.0))
        focIndex = 7;
    else if ((focPos > 4.0) && (focPos <= 4.5))
        focIndex = 8;
    else if ((focPos > 4.5) && (focPos <= 5.0))
        focIndex = 9;
    else if ((focPos > 5.0) && (focPos <= 5.5))
        focIndex = 10;
    else if ((focPos > 5.5) && (focPos <= 6.0))
        focIndex = 11;
    else if ((focPos > 6.0) && (focPos <= 6.5))
        focIndex = 12;
    else if (focPos > 6.5)
        focIndex = 12;

    return focIndex;
}

int Calc2DLinear::Get2DFreqReceiveIndex(int freq)
{
    int freqIndex = 0;

    if (freq <= 100)
        freqIndex = 0;
    else if ((freq > 100) && (freq <= 110))
        freqIndex = 1;
    else if ((freq > 110) && (freq <= 120))
        freqIndex = 2;
    else if ((freq > 120) && (freq <= 130))
        freqIndex = 3;
    else if ((freq > 130) && (freq <= 140))
        freqIndex = 4;
    else if (freq > 140)
        freqIndex = 4;

    return freqIndex;
}

int Calc2DLinear::GetColorFreqIndex(int freqColor)
{
    int freqIndex = 0;

    if (freqColor <= 118)
        freqIndex = 0;
    else if ((freqColor > 118) && (freqColor <= 144))
        freqIndex = 1;
    else if (freqColor > 144)
        freqIndex = 1;

    return freqIndex;
}