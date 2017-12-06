#include "imageControl/Calc2DTV.h"
#include "Def.h"
#include "probe/ProbeSocket.h"
#include "display/TopArea.h"
#include "imageControl/Img2D.h"

///> static varible init
const int Calc2DTV::EMIT_CH_NUM[20] =   {20,24, 28, 36, 46, 56, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};
float Calc2DTV::TIS_CFM_BASIC[2][11][11]=
{
    {//5.9Mhz
        //
        {0.0190, 0.0381, 0.0438, 0.1010, 0.1333, 0.1581, 0.2095, 0.2229, 0.3429, 0.5333, 0.6038},
        {0.0219, 0.0438, 0.0504, 0.1161, 0.1533, 0.1818, 0.2410, 0.2563, 0.3943, 0.6133, 0.6944},
        {0.0314, 0.0629, 0.0723, 0.1666, 0.2200, 0.2609, 0.3457, 0.3677, 0.5657, 0.8800, 0.9963},
        {0.0314, 0.0629, 0.0723, 0.1666, 0.2200, 0.2609, 0.3457, 0.3677, 0.5657, 0.8800, 0.9963},
        {0.0410, 0.0819, 0.0942, 0.2170, 0.2867, 0.3399, 0.4505, 0.4791, 0.7371, 1.1467, 1.2982},
        {0.0448, 0.0895, 0.1030, 0.2372, 0.3133, 0.3715, 0.4924, 0.5237, 0.8057, 1.2533, 1.4190},
        {0.0543, 0.1086, 0.1249, 0.2877, 0.3800, 0.4506, 0.5971, 0.6351, 0.9771, 1.5200, 1.7209},
        {0.0638, 0.1276, 0.1468, 0.3382, 0.4467, 0.5296, 0.7019, 0.7466, 1.1486, 1.7867, 2.0228},
        {0.0733, 0.1467, 0.1687, 0.3887, 0.5133, 0.6087, 0.8067, 0.8580, 1.3200, 2.0533, 2.3247},
        {0.0762, 0.1524, 0.1752, 0.4038, 0.5333, 0.6324, 0.8381, 0.8914, 1.3714, 2.1333, 2.4152},
        {0.0762, 0.1524, 0.1752, 0.4038, 0.5333, 0.6324, 0.8381, 0.8914, 1.3714, 2.1333, 2.4152},
    },
    {//7.2Mhz
        {0.0222, 0.0222, 0.0222, 0.1110, 0.1110, 0.1997, 0.1997, 0.3107, 0.4283, 0.5925, 0.5925},
        {0.0255, 0.0255, 0.0255, 0.1276, 0.1276, 0.2297, 0.2297, 0.3573, 0.4926, 0.6814, 0.6814},
        {0.0366, 0.0366, 0.0366, 0.1276, 0.1276, 0.3296, 0.3296, 0.5127, 0.7067, 0.9777, 0.9777},
        {0.0366, 0.0366, 0.0366, 0.1831, 0.1831, 0.3296, 0.3296, 0.5127, 0.7067, 0.9777, 0.9777},
        {0.0477, 0.0477, 0.0477, 0.2386, 0.2386, 0.4294, 0.4294, 0.6680, 0.9209, 1.2740, 1.2740},
        {0.0522, 0.0522, 0.0522, 0.2608, 0.2608, 0.4694, 0.4694, 0.7301, 1.0065, 1.3925, 1.3925},
        {0.0632, 0.0632, 0.0632, 0.3162, 0.3162, 0.5692, 0.5692, 0.8855, 1.2207, 1.6888, 1.6888},
        {0.0743, 0.0743, 0.0743, 0.3717, 0.3717, 0.6691, 0.6691, 1.0408, 1.4349, 1.9850, 1.9850},
        {0.0854, 0.0854, 0.0854, 0.4272, 0.4272, 0.7690, 0.7690, 1.1962, 1.6490, 2.2813, 2.2813},
        {0.0888, 0.0888, 0.0888, 0.4439, 0.4439, 0.7989, 0.7989, 1.2428, 1.7133, 2.3702, 2.3702},
        {0.0888, 0.0888, 0.0888, 0.4439, 0.4439, 0.7989, 0.7989, 1.2428, 1.7133, 2.3702, 2.3702},
    },
};
float Calc2DTV::TIS_CFM_POWER[10] =
{
    0.078, 0.0780, 0.0780, 0.241, 0.2410, 0.518, 0.5180, 0.759, 0.7590, 1.0000,
};
float Calc2DTV::TIS_CFM_BOX_LINES[8] =
{
    1.1067, 1.000, 0.9249, 0.7668, 0.6324, 0.5296, 0.4980, 0.4190,
};
float Calc2DTV::TIS_2D_BASIC[11][8] = // scan lines--foc pos
{
    {0.2000, 0.1609, 0.1218, 0.1218, 0.1218, 0.0920, 0.0851, 0.0759},
    {0.2300, 0.1851, 0.1401, 0.1401, 0.1401, 0.1058, 0.0978, 0.0872},
    {0.3300, 0.2655, 0.2010, 0.2010, 0.2010, 0.1517, 0.1404, 0.1252},
    {0.3300, 0.2655, 0.2010, 0.2010, 0.2010, 0.1517, 0.1404, 0.1252},
    {0.4300, 0.3460, 0.2620, 0.2620, 0.2620, 0.1977, 0.1829, 0.1631},
    {0.4700, 0.3782, 0.2863, 0.2863, 0.2863, 0.2161, 0.1999, 0.1783},
    {0.5700, 0.4587, 0.3473, 0.3473, 0.3473, 0.2621, 0.2424, 0.2162},
    {0.6700, 0.5391, 0.4082, 0.4082, 0.4082, 0.3081, 0.2850, 0.2542},
    {0.7701, 0.6296, 0.4691, 0.4691, 0.4691, 0.3540, 0.3275, 0.2921},
    {0.8001, 0.6437, 0.7874, 0.4874, 0.4874, 0.3678, 0.3403, 0.3035},
    {0.8001, 0.6437, 0.7874, 0.4874, 0.4874, 0.3678, 0.3403, 0.3035},
};
float Calc2DTV::TIS_2D_FREQ[9] = //2.0 2.5 ... 6.0
{
    0.7792, 1.0000, 0.9666, 1.0000, 1.1598, 1.2436, 1.1985, 1.1985, 1.0736,
};
float Calc2DTV::TIS_2D_POWER[10] =
{
    0.078, 0.0780, 0.0780, 0.241, 0.2410, 0.518, 0.5180, 0.759, 0.7590, 1.0000,
};
float Calc2DTV::TIS_2D_DEPTH[20] =
{
    2.5532, 2.5532, 2.5532, 2.3404, 2.5532, 2.3404, 2.3404, 2.1915, 1.8511, 1.8511, 1.8511, 1.5532, 1.6383, 1.3404, 1.3404,
    1.0638, 1.0638, 1.0000, 1.0000, 1.0000,
};
float Calc2DTV::TIS_PW_BASIC[2][15] = //freq-prf
{
    {0.0636, 0.0997, 0.1336, 0.1846, 0.2270, 0.2758, 0.3182, 0.3606, 0.4094, 0.4455, 0.4879, 0.5515, 0.5728, 0.6428, 0.6852},
    {0.0667, 0.1236, 0.1656, 0.2224, 0.2793, 0.3213, 0.2546, 0.4276, 0.4869, 0.5438, 0.5932, 0.6426, 0.6995, 0.7489, 0.7983},
};

///>
Calc2DTV::Calc2DTV()
{

}

Calc2DTV::~Calc2DTV()
{

}

/*
 * @brief calc tgc curve of convex
 */
void Calc2DTV::CalcTgc(int gain, int tgcY[],AbsUpdate2D* ptrUpdate, int section)
{
    ///> calc
    if ((section == 0) || (section == 1))
        Tgc(TGC_X, gain, tgcY, TGC_CTL_DEFAULT, ptrUpdate, section);
    else
        TgcColor(TGC_X, gain, tgcY, TGC_CTL_DEFAULT, ptrUpdate, section);
}

void Calc2DTV::CalcFocPos()
{
    int start = 4;

    FocChange(start);

    ///> 2D pulse number according to last focus pos
        ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);

#if (defined(EMP_340) || defined(EMP_355) || defined(EMP_322))
    m_fpga.Send2DPulseNum(1);
#elif (defined(EMP_430))
    if((strcmp(para.model, "65C15D")) == 0)
    {
        m_fpga.Send2DPulseNum(2);
    }
    else
    {
        m_fpga.Send2DPulseNum(1);
    }
#else
    m_fpga.Send2DPulseNum(2);
#endif

    // m_fpga.Send2DPulseNum(2);
    //m_fpga.Send2DPulseNum(1);
    //m_fpga.Send2DPulseNum(2);

    CalcEmitDelay();
}

void Calc2DTV::CalcEmitDelay()
{
    int focSum = m_calcPara->focSum;
    int size = sizeof(EMIT_CH_NUM) / sizeof(int);
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;

    int i;
    for (i = 0; i < focSum; i ++)
    {
        CEmitDelay(i, EMIT_CH_NUM, size, compound);
    }
}
void Calc2DTV::CalcFilter()
{
    CalcDynamicDemod(0);
    CalcFilterBandPass(0);
    CalcDynamicFilter(0);
}

void Calc2DTV::CalcDefaultDemodFd()
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

void Calc2DTV::CalcFilterFreqCompound(int order)
{
    CalcDynamicDemod(order);
    CalcFilterBandPass(order);
    CalcDynamicFilter(order);
}
float Calc2DTV::Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex)
{
    scanAngleIndex = Get2DScanAngleIndex(scanAngleIndex);
    depthIndex = Get2DDepthIndex(depthIndex);
    int powerIndex = GetPowerIndex(power);
    int focIndex = GetFocIndex(focPos);
    int freqIndex = Get2DFreqReceiveIndex(freqReceive);

    float ti = TIS_2D_BASIC[focIndex][scanAngleIndex] * TIS_2D_POWER[powerIndex] * TIS_2D_FREQ[freqIndex] * TIS_2D_DEPTH[depthIndex];

    return ti;
}
float Calc2DTV::GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd)
{
    prfIndex = GetCfmPrfIndex(prfIndex);
    int freqIndex = GetColorFreqIndex(dopFreq);
    int focPosIndex = GetFocIndex(focPos);
    int boxLineIndex = GetCfmLinesIndex(boxLineBegin, boxLineEnd);
    int powerIndex = GetPowerIndex(power);

    float ti = TIS_CFM_BASIC[freqIndex][focPosIndex][prfIndex] * TIS_CFM_POWER[powerIndex] * TIS_CFM_BOX_LINES[boxLineIndex];
    PRINTF("CFM TI = %f\n", ti);

    return ti;
}
float Calc2DTV::GetPwTis(int dopFreq, int prfIndex)
{
    prfIndex = GetPwPrfIndex(prfIndex);
    int freqIndex = GetColorFreqIndex(dopFreq);

    float ti = TIS_PW_BASIC[freqIndex][prfIndex];
    PRINTF("PW TI = %f\n", ti);

    return ti;
}
void Calc2DTV::CalcFilterBandPass(int order)
{
    int probeR = m_calcPara->probeR / 100;
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

#if defined(EMP_340)
    depth[0] = 10;
    depth[1] = 25;
    depth[2] = 40;
    depth[3] = 60;
         depth[4] = 80;
#else
        depth[0] = 40;
    depth[1] = 60;
    depth[2] = 80;
    depth[3] = 100;
         depth[4] = 120;
#endif
 #if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) || defined(EMP_161) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
    float fc_bandpass_filter1[5], fc_bandpass_filter2[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i] / 10.0);
            PRINTF("-TV--%d---THI---w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            PRINTF("-TV--%d-----w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
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

void Calc2DTV::CalcDynamicFilter(int order)
{
    int probeIndex = 0;
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    string probe_type = TopArea::GetInstance()->GetProbeType();
    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }
    int probeR = m_calcPara->probeR / 100;

    ////printf("freq:%d\n", freq);
    int depth[5];
    float fd[5];
    float fd_harmonic[5];

    depth[0] = 40;
    depth[1] = 60;
    depth[2] = 80;
    depth[3] = 100;
    depth[4] = 120;

    float fc_dynamic_filter[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
            PRINTF("-TV--%d--THI---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            PRINTF("-TV--%d---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
        }
    }
    DynamicFilter_test(fc_dynamic_filter, depth, order);
}

void Calc2DTV::CalcDynamicDemod(int order)
{
    int i;
    int probeR = m_calcPara->probeR / 100;
    int freq = m_calcPara->freq.receive;
    int depth[5];
    float fd[5];
    float fd_harmonic[5];

    depth[0] = 40;
    depth[1] = 60;
    depth[2] = 80;
    depth[3] = 100;
    depth[4] = 120;

    if (probeR <= 13)
    {
        if (freq < 90)
        {
#ifdef EMP_430
            fd[0] = 6.8;
            fd[1] = 5.5;
            fd[2] = 3.8;
            fd[3] = 3.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 7.5;
            fd_harmonic[1] = 6.2;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 4.0;
            fd_harmonic[4] = 3.0;
#elif (EMP_161 || EMP_360)
            fd[0] = 5.5;
            fd[1] = 5.3;
            fd[2] = 5.0;
            fd[3] = 4.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 7.5;
            fd_harmonic[1] = 7.0;
            fd_harmonic[2] = 5.5;
            fd_harmonic[3] = 4.5;
            fd_harmonic[4] = 4.0;
#else
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.5;
            fd[3] = 5.0;
            fd[4] = 4.5;
            fd_harmonic[0] = 7.3;
            fd_harmonic[1] = 6.2;
            fd_harmonic[2] = 5.1;
            fd_harmonic[3] = 4.6;
            fd_harmonic[4] = 3.8;
#endif

        }
        else if (freq >= 90 && freq < 100)
        {
#ifdef EMP_430
            fd[0] = 6.8;
            fd[1] = 5.5;
            fd[2] = 3.8;
            fd[3] = 3.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 7.7;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 3.6;
            fd_harmonic[4] = 2.8;
#elif (EMP_161 || EMP_360)
            fd[0] = 5.5;
            fd[1] = 5.3;
            fd[2] = 5.0;
            fd[3] = 4.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 8.5;
            fd_harmonic[1] = 7.5;
            fd_harmonic[2] = 6.5;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 4.5;
#else
            fd[0] = 7.0;
            fd[1] = 6.5;
            fd[2] = 5.8;
            fd[3] = 4.9;
            fd[4] = 4.6;

            fd_harmonic[0] = 8.2;
            fd_harmonic[1] = 7.2;
            fd_harmonic[2] = 6.0;
            fd_harmonic[3] = 5.6;
            fd_harmonic[4] = 5.0;
#endif

        }
        else if (freq >= 100 && freq < 110)
        {
#ifdef EMP_430
            fd[0] = 6.8;
            fd[1] = 5.5;
            fd[2] = 3.8;
            fd[3] = 3.5;
            fd[4] = 3.0;

            fd_harmonic[0] = 7.7;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 3.6;
            fd_harmonic[4] = 2.8;
#elif (EMP_161 || EMP_360)
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.5;
            fd[3] = 4.0;//4.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 9.5;
            fd_harmonic[1] = 8.5;
            fd_harmonic[2] = 7.5;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 4.5;
#else
            fd[0] = 7.0;
            fd[1] = 6.5;
            fd[2] = 5.8;
            fd[3] = 4.9;
            fd[4] = 4.6;

            fd_harmonic[0] = 9.5;
            fd_harmonic[1] = 8.6;
            fd_harmonic[2] = 7.5;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 4.5;
#endif
        }
        else if (freq>=110 && freq<135)
        {
#ifdef EMP_430
            fd[0] = 7.0;
            fd[1] = 5.6;
            fd[2] = 3.6;
            fd[3] = 3.3;
            fd[4] = 3.0;
            fd_harmonic[0] = 7.7;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 3.6;
            fd_harmonic[4] = 2.8;
#elif (EMP_161 || EMP_360)
            fd[0] = 7.5;
            fd[1] = 6.8;
            fd[2] = 6.0;
            fd[3] = 4.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#else
            fd[0] = 7.5;
            fd[1] = 7.0;
            fd[2] = 5.0;
            fd[3] = 3.6;
            fd[4] = 2.8;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#endif

        }
        else if (freq>=135 && freq<150)
        {
#ifdef EMP_430
            fd[0] = 7.0;
            fd[1] = 5.6;
            fd[2] = 3.6;
            fd[3] = 3.3;
            fd[4] = 3.0;
            fd_harmonic[0] = 7.7;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 3.6;
            fd_harmonic[4] = 2.8;
#elif (EMP_161 || EMP_360)
            fd[0] = 8.0;
            fd[1] = 7.0;
            fd[2] = 6.5;
            fd[3] = 5.0;
            fd[4] = 4.0;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#else
            fd[0] = 9.0;
            fd[1] = 8.2;
            fd[2] = 7.5;
            fd[3] = 6.7;
            fd[4] = 6.0;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#endif
        }
        else if (freq>=150 && freq<165)
        {
#ifdef EMP_430
            fd[0] = 7.5;
            fd[1] = 6.0;
            fd[2] = 4.0;
            fd[3] = 3.5;
            fd[4] = 3.0;
            fd_harmonic[0] = 7.7;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 3.6;
            fd_harmonic[4] = 2.8;
#elif (EMP_161 || EMP_360)
            fd[0] = 8.0;
            fd[1] = 7.0;
            fd[2] = 6.5;
            fd[3] = 5.0;
            fd[4] = 4.0;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#else
            fd[0] = 7.8;
            fd[1] = 6.0;
            fd[2] = 4.8;
            fd[3] = 3.6;
            fd[4] = 2.6;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#endif
        }
        else
        {
#ifdef EMP_161
            fd[0] = 8.5;
            fd[1] = 8.5;
            fd[2] = 7.5;
            fd[3] = 6.0;
            fd[4] = 4.5;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#elif EMP_360
            fd[0] = 8.3;//8.5;
            fd[1] = 7.0;//8.0;//8.5;
            fd[2] = 6.5;//7.3;//7.5;
            fd[3] = 6.0;
            fd[4] = 4.5;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#else
            fd[0] = 10.0;
            fd[1] = 9.0;
            fd[2] = 7.4;
            fd[3] = 5.4;
            fd[4] = 3.0;
            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
#endif
        }
    }
    else if (probeR>13 && probeR<=15) //probeR = 15
    {
        if (freq < 90)
        {
            fd[0] = 4.8;
            fd[1] = 4.6;
            fd[2] = 4.4;
            fd[3] = 4.2;
            fd[4] = 3.5;

            fd_harmonic[0] = 7.5;
            fd_harmonic[1] = 7.5;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.0;
            fd_harmonic[4] = 7.0;
        }
        else if (freq >= 90 && freq < 100)
        {

            fd[0] = 4.8;
            fd[1] = 4.6;
            fd[2] = 4.4;
            fd[3] = 4.2;
            fd[4] = 3.5;

            fd_harmonic[0] = 8.2;
            fd_harmonic[1] = 7.2;
            fd_harmonic[2] = 6.0;
            fd_harmonic[3] = 5.6;
            fd_harmonic[4] = 5.0;
        }
        else if (freq >= 100 && freq < 110)
        {
            fd[0] = 4.8;
            fd[1] = 4.6;
            fd[2] = 4.4;
            fd[3] = 4.2;
            fd[4] = 3.5;

            fd_harmonic[0] = 9.5;
            fd_harmonic[1] = 8.6;
            fd_harmonic[2] = 7.5;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 4.5;
        }
        else if (freq>=110 && freq<135)
        {
            fd[0] = 4.8;
            fd[1] = 4.6;
            fd[2] = 4.4;
            fd[3] = 4.2;
            fd[4] = 3.5;

            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
        }
        else if (freq>=135 && freq<165)
        {
            fd[0] = 5.0;
            fd[1] = 4.8;
            fd[2] = 4.6;
            fd[3] = 4.4;
            fd[4] = 3.7;

            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
        }
        else if (freq>=165)
        {
            fd[0] = 5.2;
            fd[1] = 5.0;
            fd[2] = 4.8;
            fd[3] = 4.6;
            fd[4] = 3.9;

            fd_harmonic[0] = 11.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.2;
            fd_harmonic[3] = 7.1;
            fd_harmonic[4] = 5.6;
        }
    }
    else
    {
        fd[0] = (float)freq/20.0;
        fd[1] = (float)freq/20.0*9.0/10.0;
        fd[2] = (float)freq/20.0*7.5/10.0;
        fd[3] = (float)freq/20.0*6.5/10.0;
        fd[4] = (float)freq/20.0*5.5/10.0;

        fd_harmonic[0] = (float)freq/20.0;
        fd_harmonic[1] = (float)freq/20.0*9.0/10.0;
        fd_harmonic[2] = (float)freq/20.0*7.5/10.0;
        fd_harmonic[3] = (float)freq/20.0*6.5/10.0;
        fd_harmonic[4] = (float)freq/20.0*5.5/10.0;
    }

 #if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) || defined(EMP_161) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int probeIndex = 0;
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

            PRINTF("----- project test --TV--%d----demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            //fd[i] =  float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
           // fd_harmonic[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);

            //fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
            //fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
            PRINTF("-TV--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
#else

    for (i = 0; i < 5; i++)
    {
        fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
        fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
        PRINTF("-TV--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
    }
#endif
#endif

    bool harmonic = m_calcPara->harmonic;

    if(harmonic)
    {
        if (order == 0)
        {
            DynamicDemod(fd_harmonic, depth, TRUE);
        }
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

#if 0

    if (order == 0)
        DynamicDemod(fd, depth, TRUE);
    //DynamicDemod(fd, depth, FALSE);
    else
        DynamicDemodFreqCompound(fd, depth, TRUE, order);
#endif
    //DynamicDemodFreqCompound(fd, depth, FALSE, order);
}

///> private
int Calc2DTV::Get2DFreqReceiveIndex(int freq)
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
    else if ((freq > 140) && (freq <= 150))
        freqIndex = 5;
    else if ((freq > 150) && (freq <= 160))
        freqIndex = 6;
    else if ((freq > 160) && (freq <= 170))
        freqIndex = 7;
    else if ((freq > 170) && (freq <= 180))
        freqIndex = 8;
    else if (freq > 180)
        freqIndex = 8;

    return freqIndex;
}

int Calc2DTV::GetColorFreqIndex(int freq)
{
    int freqIndex = 0;

    if (freq <= 98)
        freqIndex = 0;
    else if ((freq > 98) && (freq <= 116))
        freqIndex = 1;
    else if (freq > 116)
        freqIndex = 1;

    return freqIndex;
}