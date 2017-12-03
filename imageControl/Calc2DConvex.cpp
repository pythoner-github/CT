/*
:A
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: Calc2DConvex.cpp 35C50K
 * @brief: derived from class Calc2D, deal with calculation of convex probe("35C50K").
 *
 * version: V1.0
 * date: 2009-5-12
 * @author: zhanglei
 */

#include "imageControl/Calc2DConvex.h"
#include "Def.h"
#include "probe/ProbeSocket.h"
#include "imageControl/Img2D.h"
#include "display/TopArea.h"

float Calc2DConvex::TIS_CFM_BASIC[2][11][11]=
{
	{//2.7Mhz
		//
		{0.0084, 0.0126, 0.0210, 0.0315, 0.0420, 0.0483, 0.0567, 0.0672, 0.0861, 0.1113, 0.1302}, //0.5
		{0.0172, 0.0258, 0.0430, 0.0645, 0.0860, 0.0989, 0.1161, 0.1376, 0.1763, 0.2279, 0.2666}, //1.2
		{0.0228, 0.0342, 0.0570, 0.0855, 0.1140, 0.1311, 0.1539, 0.1824, 0.2337, 0.3021, 0.3534}, //2.5
		{0.0316, 0.0474, 0.0790, 0.1185, 0.1580, 0.1817, 0.2133, 0.2528, 0.3239, 0.4187, 0.4898}, //3.5
		{0.04, 0.06, 0.10, 0.15, 0.20, 0.23, 0.27, 0.32, 0.41, 0.53, 0.62}, //5.0
		{0.0424, 0.0636, 0.1060, 0.1590, 0.2120, 0.2438, 0.2862, 0.3392, 0.4346, 0.5618, 0.6572}, //6.0
		{0.0512, 0.0768, 0.1280, 0.1920, 0.2560, 0.2944, 0.3456, 0.4096, 0.5248, 0.6784, 0.7936}, //7.2
		{0.0568, 0.0852, 0.1420, 0.2130, 0.2840, 0.3266, 0.3834, 0.4544, 0.5822, 0.7526, 0.8804}, //8.5
		{0.0592, 0.0888, 0.1480, 0.2220, 0.2960, 0.3404, 0.3996, 0.4736, 0.6068, 0.7844, 0.9176}, //9.8
		{0.0620, 0.0930, 0.1550, 0.2325, 0.3100, 0.3565, 0.4185, 0.4960, 0.6355, 0.8215, 0.9610}, //11.0
		{0.0620, 0.0930, 0.1550, 0.2325, 0.3100, 0.3565, 0.4185, 0.4960, 0.6355, 0.8215, 0.9610}, //12.4
	},
	{//3.3Mhz
		{0.008, 0.0147, 0.032, 0.048, 0.063, 0.076, 0.088, 0.097, 0.124, 0.162, 0.195}, //0.5
		{0.0172, 0.0301, 0.0645, 0.0989, 0.1290, 0.1548, 0.1806, 0.1978, 0.2537, 0.3311, 0.3999}, //1.2
		{0.0228, 0.0399, 0.0855, 0.1311, 0.1710, 0.2052, 0.2394, 0.2622, 0.3363 , 0.4389, 0.5301}, //2.5
		{0.0316, 0.0553, 0.1185, 0.1817, 0.2370, 0.2844, 0.3318, 0.3634, 0.4661, 0.6083, 0.7347}, //3.5
		{0.04, 0.07, 0.15, 0.23, 0.30, 0.36, 0.42, 0.46, 0.59, 0.77, 0.93}, //5.0
		{0.0424, 0.0742, 0.1590, 0.2438, 0.3180, 0.3816, 0.4452, 0.4876, 0.6254, 0.8162, 0.9858}, //6.0
		{0.0512, 0.0896, 0.1920, 0.2944, 0.3840, 0.4608, 0.5376, 0.5888, 0.7552, 0.9856, 1.1904}, //7.2
		{0.0568, 0.0994, 0.2130, 0.3266, 0.4260, 0.5112, 0.5964, 0.6532, 0.8378, 1.0934, 1.3206}, //8.5
		{0.0592, 0.1036, 0.2220, 0.3404, 0.4440, 0.5328, 0.6216, 0.6808, 0.8732, 1.1396, 1.3764}, //9.8
		{0.0620, 0.1085, 0.2325, 0.3565, 0.4650, 0.5580, 0.6510, 0.7130, 0.9145, 1.1935, 1.4415}, //11.0
		{0.0620, 0.1085, 0.2325, 0.3565, 0.4650, 0.5580, 0.6510, 0.7130, 0.9145, 1.1935, 1.4415}, //12.4
	},
};
float Calc2DConvex::TIS_CFM_POWER[10] =
{
	0.05, 0.10, 0.30, 0.30, 0.40, 0.60, 0.75, 0.90, 1.0, 1.0
};
float Calc2DConvex::TIS_CFM_BOX_LINES[8] =
{
	1.1, 1.0, 0.74, 0.59, 0.46, 0.44, 0.39, 0.31
};
float Calc2DConvex::TIS_2D_BASIC[11][8] = // scan lines--foc pos
{
	{0.0078, 0.0102, 0.0120, 0.0120 ,0.0120, 0.0161, 0.0239, 0.0257}, //0.5
	{0.0159, 0.0208, 0.0245, 0.0245, 0.0245, 0.0331, 0.0490, 0.0527}, //1.2
	{0.0211, 0.0276, 0.0325, 0.0325, 0.0325, 0.0438, 0.0649, 0.0698}, //2.5
	{0.0292, 0.0382, 0.0450, 0.0450, 0.0450, 0.0607, 0.0900, 0.0967}, //3.5
	{0.0370, 0.0484, 0.0570, 0.0570, 0.0570, 0.0769, 0.1139, 0.1224}, //5.0
	{0.0392, 0.0513, 0.0604, 0.0604, 0.0604, 0.0815, 0.1207, 0.1298}, //6.0
	{0.0474, 0.0620, 0.0729, 0.0729, 0.0729, 0.0984, 0.1458, 0.1567}, //7.2
	{0.0526, 0.0687, 0.0809, 0.0809, 0.0809, 0.1092, 0.1617, 0.1739}, //8.5
	{0.0548, 0.0716, 0.0843, 0.0843, 0.0843, 0.1138, 0.1686, 0.1812}, //9.8
	{0.0574, 0.0750, 0.0883, 0.0883, 0.0883, 0.1192, 0.1766, 0.1898}, //11.0
	{0.0574, 0.0750, 0.0883, 0.0883, 0.0883, 0.1192, 0.1766, 0.1898}, //12.4
};
float Calc2DConvex::TIS_2D_FREQ[9] = //2.0 2.5 ... 6.0
{
	0.3216, 0.4958, 0.7795, 1.0000, 1.0801, 1.1735, 1.0730, 0.9228, 0.8153,
};
float Calc2DConvex::TIS_2D_POWER[10] =
{
	0.05, 0.10, 0.30, 0.30, 0.40, 0.60, 0.75, 0.90, 1.0, 1.0,
};
float Calc2DConvex::TIS_2D_DEPTH[20] =
{
	3.5581, 3.5581, 3.4186, 3.1860, 3.0930, 3.0233, 2.7907, 2.8605, 2.7209, 2.3953, 2.1744, 2.1628, 2.0233, 1.7907, 1.8605, 1.6279, 1.5581, 1.3256, 1.1628, 1.0000,
};
float Calc2DConvex::TIS_PW_BASIC[2][15] = //freq-prf
{
	{0.0763, 0.1308, 0.1668, 0.2213, 0.2726, 0.3129, 0.3631, 0.4176, 0.4688, 0.5201, 0.5669, 0.6215, 0.6836, 0.7381, 0.7774}, // 2.7MHZ
	{0.1793, 0.2689, 0.3518, 0.4482, 0.5759, 0.6879, 0.7910, 0.8963, 1.0151, 1.1204, 1.2548, 1.3512, 1.4632, 1.5618, 1.6739}, //3.3MHZ
};
///>pulic
Calc2DConvex::Calc2DConvex()
{
}
Calc2DConvex::~Calc2DConvex()
{

}

#if 0
void Calc2DConvex::CalcFocPos()
{
    int start = 0;

    FocChange(start);

    ///> 2D pulse number according to last focus pos
    m_fpga.Send2DPulseNum(2);

    CalcEmitDelay();
}
#endif

void Calc2DConvex::CalcFilter()
{
	CalcDynamicDemod(0);
	CalcFilterBandPass(0);
	CalcDynamicFilter(0);
}

void Calc2DConvex::CalcDefaultDemodFd()
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

void Calc2DConvex::CalcFilterFreqCompound(int order)
{
	CalcDynamicDemod(order);
	CalcFilterBandPass(order);
	CalcDynamicFilter(order);
}
float Calc2DConvex::Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex)
{
	scanAngleIndex = Get2DScanAngleIndex(scanAngleIndex);
	depthIndex = Get2DDepthIndex(depthIndex);
	int powerIndex = GetPowerIndex(power);
	int focIndex = GetFocIndex(focPos);
	int freqIndex = Get2DFreqReceiveIndex(freqReceive);

	float ti = TIS_2D_BASIC[focIndex][scanAngleIndex] * TIS_2D_POWER[powerIndex] * TIS_2D_FREQ[freqIndex] * TIS_2D_DEPTH[depthIndex];
	PRINTF("2D TI = %f\n", ti);

	return ti;
}
float Calc2DConvex::GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd)
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
float Calc2DConvex::GetPwTis(int dopFreq, int prfIndex)
{
	prfIndex = GetPwPrfIndex(prfIndex);
	int freqIndex = GetColorFreqIndex(dopFreq);

	float ti = TIS_PW_BASIC[freqIndex][prfIndex];
	PRINTF("PW TI = %f\n", ti);

	return ti;
}

void Calc2DConvex::CalcWeightingEmit(int pulseWidth)
{
	unsigned char weightingValue[APERTURE_HALF*2];
	int value = 600;

	WeightingEmit(pulseWidth, value, weightingValue);
	m_fpga.SendPulseWidth(weightingValue, APERTURE_HALF*2);
}
void Calc2DConvex::CalcWeightingEmitColor(int pulseWidth)
{
	unsigned char weightingValue[APERTURE_HALF*2];
	int value = 600;

	WeightingEmit(pulseWidth, value, weightingValue);
	m_fpga.SendColorPulseWidth(weightingValue, APERTURE_HALF*2);
}
///> private
void Calc2DConvex::CalcFilterBandPass(int order)
{
    BandPassFilterSelect(order);
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

    if (probeR >= 40)
	{
		depth[0] = 50;
		depth[1] = 100;
		depth[2] = 150;
		depth[3] = 200;
		depth[4] = 250;
	}
	else if (probeR >= 20)
	{
		depth[0] = 50;
		depth[1] = 100;
		depth[2] = 150;
		depth[3] = 200;
		depth[4] = 250;
	}
    else if (probeR < 5)
    {
		depth[0] = 100;
		depth[1] = 120;
		depth[2] = 160;
		depth[3] = 200;
		depth[4] = 250;
    }
	else //if (probeR < 20)
	{
		depth[0] = 100;
		depth[1] = 120;
		depth[2] = 160;
		depth[3] = 200;
		depth[4] = 250;
	}

#if (defined(EMP_340) || defined(EMP_430) || defined(EMP_161)||defined(EMP_360) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
    float fc_bandpass_filter1[5], fc_bandpass_filter2[5];
    if(m_calcPara->harmonic)
        {
            for(int i = 0; i < 5; i++)
            {
                fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] / 10.0);
                fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i] / 10.0);
                PRINTF("-convex--%d---THI---w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
            }
        }
        else
        {
            for(int i = 0; i < 5; i++)
            {
                fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                PRINTF("-convex--%d-----w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
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
#endif

}

void Calc2DConvex::CalcDynamicFilter(int order)
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

	int depth[5];
	float fd[5];
    float fd_harmonic[5];

	if (probeR >= 40)
	{
        depth[0] = 50;
        depth[1] = 100;
        depth[2] = 150;
        depth[3] = 200;
        depth[4] = 250;
	}
	else if (probeR >= 20)
	{
        depth[0] = 50;
        depth[1] = 100;
        depth[2] = 150;
        depth[3] = 200;
        depth[4] = 250;
    }
    else if (probeR < 5)
    {
        depth[0] = 100;
        depth[1] = 120;
        depth[2] = 160;
        depth[3] = 200;
        depth[4] = 250;
    }
	else //if (probeR < 20)
	{
		depth[0] = 100;
		depth[1] = 120;
		depth[2] = 160;
		depth[3] = 200;
		depth[4] = 250;
    }

    float fc_dynamic_filter[5];

        if(m_calcPara->harmonic)
        {
            for(int i = 0; i < 5; i++)
            {
                fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
                PRINTF("-Convex--%d--THI---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
            }
        }
        else
        {
            for(int i = 0; i < 5; i++)
            {
                fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                PRINTF("-Convex--%d---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
            }
        }

    DynamicFilter_test(fc_dynamic_filter, depth, order);

}

void Calc2DConvex::CalcDynamicDemod(int order)
{
    int i;
    int freq;
    int probeR = m_calcPara->probeR /100;
    bool harmonic = m_calcPara->harmonic;
    freq = m_calcPara->freq.receive;

	int depth[5];
    float fd[5];
    float fd_harmonic[5];
    float fd_harmonic_35D40J[5];
    float fd_35D40J[5];
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);

    if(harmonic)
    {
        if (probeR >= 40)
        {
            depth[0] = 50;
            depth[1] = 100;
            depth[2] = 150;
            depth[3] = 200;
            depth[4] = 250;
        }
        else if (probeR >= 20)
        {
            depth[0] = 50;
            depth[1] = 100;
            depth[2] = 150;
            depth[3] = 200;
            depth[4] = 250;
        }
        else if (probeR < 5)
        {
            depth[0] = 50;
            depth[1] = 100;
            depth[2] = 150;
            depth[3] = 200;
            depth[4] = 250;
        }
        else //if (probeR < 20)
        {
            depth[0] = 40;
            depth[1] = 60;
            depth[2] = 80;
            depth[3] = 100;
            depth[4] = 120;
        }

    }
    else
    {
        if (probeR >= 40)
        {
            depth[0] = 100;
            depth[1] = 120;
            depth[2] = 160;
            depth[3] = 200;
            depth[4] = 250;
        }
        else if (probeR >= 20)
        {
            depth[0] = 100;
            depth[1] = 120;
            depth[2] = 160;
            depth[3] = 200;
            depth[4] = 250;
        }
        else if (probeR < 5)
        {
            depth[0] = 100;
            depth[1] = 120;
            depth[2] = 160;
            depth[3] = 200;
            depth[4] = 250;
        }
        else //if (probeR < 20)
        {
            depth[0] = 40;
            depth[1] = 60;
            depth[2] = 80;
            depth[3] = 100;
            depth[4] = 120;
        }
    }

	if ((probeR >= 40) || (probeR < 5))		//凸阵R50
	{
        if (freq < 35)
        {
#ifdef EMP_430
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.0;
            fd_harmonic[1] = 3.9;
            fd_harmonic[2] = 3.4;
            fd_harmonic[3] = 2.9;
            fd_harmonic[4] = 2.5;
#elif (EMP_161 || EMP_360)
            fd[0] = 4.0;
            fd[1] = 3.8;
            fd[2] = 3.0;
            fd[3] = 3.0;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.2;
            fd_harmonic[1] = 4.0;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.0;
            fd_harmonic[4] = 2.5;
#else
            fd[0] = 3.8;
            fd[1] = 3.6;
            fd[2] = 3.4;
            fd[3] = 2.8;
            fd[4] = 2.2;
            fd_harmonic[0] = 4.3;
            fd_harmonic[1] = 4.1;
            fd_harmonic[2] = 3.4;
            fd_harmonic[3] = 2.8;
            fd_harmonic[4] = 2.2;
#endif
            fd_35D40J[0] = 4.4;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.6;
            fd_35D40J[4] = 2.3;

            fd_harmonic_35D40J[0] = 4.4;
            fd_harmonic_35D40J[1] = 4.2;
            fd_harmonic_35D40J[2] = 3.6;
            fd_harmonic_35D40J[3] = 3.2;
            fd_harmonic_35D40J[4] = 2.7;
        }
        else if (freq >= 35 && freq < 40)
        {
#ifdef EMP_430
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.0;
            fd_harmonic[1] = 3.9;
            fd_harmonic[2] = 3.4;
            fd_harmonic[3] = 2.9;
            fd_harmonic[4] = 2.5;
#elif (EMP_161 || EMP_360)
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 3.0;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.2;
            fd_harmonic[1] = 4.0;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.0;
            fd_harmonic[4] = 2.5;
#else
            fd[0] = 3.8;
            fd[1] = 3.6;
            fd[2] = 3.4;
            fd[3] = 2.8;
            fd[4] = 2.2;
            fd_harmonic[0] = 4.3;
            fd_harmonic[1] = 4.1;
            fd_harmonic[2] = 3.4;
            fd_harmonic[3] = 2.8;
            fd_harmonic[4] = 2.2;
#endif
            fd_35D40J[0] = 4.4;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.6;
            fd_35D40J[4] = 2.3;

            fd_harmonic_35D40J[0] = 4.4;
            fd_harmonic_35D40J[1] = 4.2;
            fd_harmonic_35D40J[2] = 3.6;
            fd_harmonic_35D40J[3] = 3.2;
            fd_harmonic_35D40J[4] = 2.7;
        }
        else if (freq >= 40 && freq < 45)
        {
#ifdef EMP_430
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.0;
            fd_harmonic[1] = 3.9;
            fd_harmonic[2] = 3.4;
            fd_harmonic[3] = 2.9;
            fd_harmonic[4] = 2.5;
#elif (EMP_161 || EMP_360)
            fd[0] = 4.0;
            fd[1] = 3.8;
            fd[2] = 3.0;
            fd[3] = 3.0;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.2;
            fd_harmonic[1] = 4.0;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.0;
            fd_harmonic[4] = 2.5;
#else
            fd[0] = 3.8;
            fd[1] = 3.6;
            fd[2] = 3.4;
            fd[3] = 2.8;
            fd[4] = 2.2;
            fd_harmonic[0] = 4.3;
            fd_harmonic[1] = 4.1;
            fd_harmonic[2] = 3.4;
            fd_harmonic[3] = 2.8;
            fd_harmonic[4] = 2.2;
#endif
            fd_35D40J[0] = 4.4;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.6;
            fd_35D40J[4] = 2.3;

            fd_harmonic_35D40J[0] = 4.4;
            fd_harmonic_35D40J[1] = 4.2;
            fd_harmonic_35D40J[2] = 3.6;
            fd_harmonic_35D40J[3] = 3.2;
            fd_harmonic_35D40J[4] = 2.7;
        }
        else if (freq >= 45 && freq < 50)
        {

#ifdef EMP_430
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.7;
            fd_harmonic[1] = 4.5;
            fd_harmonic[2] = 4.0;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 3.1;
#elif (EMP_161 || EMP_360)
            fd[0] = 4.0;
            fd[1] = 3.8;
            fd[2] = 3.0;
            fd[3] = 3.0;
            fd[4] = 2.5;
            fd_harmonic[0] = 4.5;
            fd_harmonic[1] = 4.5;
            fd_harmonic[2] = 4.0;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 2.5;
#else
            fd[0] = 3.8;
            fd[1] = 3.6;
            fd[2] = 3.4;
            fd[3] = 2.8;
            fd[4] = 2.2;
            fd_harmonic[0] = 4.7;
            fd_harmonic[1] = 4.5;
            fd_harmonic[2] = 4.0;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 3.1;
#endif
            fd_35D40J[0] = 4.4;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.6;
            fd_35D40J[4] = 2.3;

            fd_harmonic_35D40J[0] = 4.9;
            fd_harmonic_35D40J[1] = 4.3;
            fd_harmonic_35D40J[2] = 3.8;
            fd_harmonic_35D40J[3] = 3.0;
            fd_harmonic_35D40J[4] = 2.6;

        }
        else if (freq >= 50 && freq < 55)
        {
#ifdef EMP_430
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.5;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 4.7;
            fd_harmonic[2] = 4.3;
            fd_harmonic[3] = 3.7;
            fd_harmonic[4] = 2.9;
#elif (EMP_161 || EMP_360)
            fd[0] = 4.0;
            fd[1] = 3.8;
            fd[2] = 3.0;
            fd[3] = 3.0;
            fd[4] = 2.5;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 4.5;
            fd_harmonic[2] = 4.0;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 3.0;
#else
            fd[0] = 4.3;
            fd[1] = 3.8;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.3;
            fd_harmonic[0] = 5.2;
            fd_harmonic[1] = 4.7;
            fd_harmonic[2] = 4.0;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 2.9;
#endif

            fd_35D40J[0] = 4.4;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.6;
            fd_35D40J[4] = 2.3;

            fd_harmonic_35D40J[0] = 5.0;
            fd_harmonic_35D40J[1] = 4.5;
            fd_harmonic_35D40J[2] = 4.0;
            fd_harmonic_35D40J[3] = 3.3;
            fd_harmonic_35D40J[4] = 2.7;
        }
        else if (freq>=55 && freq<65)
        {
#ifdef EMP_430
            fd[0] = 3.5;
            fd[1] = 3.5;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.5;
#elif (EMP_161 || EMP_360)
            fd[0] = 4.0;
            fd[1] = 3.8;
            fd[2] = 3.0;
            fd[3] = 3.0;
            fd[4] = 2.5;
#else
            fd[0] = 4.3;
            fd[1] = 3.8;
            fd[2] = 3.1;
            fd[3] = 2.8;
            fd[4] = 2.3;
#endif

            fd_harmonic[0] = 3.5;
            fd_harmonic[1] = 3.5;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 3.5;

            fd_35D40J[0] = 4.4;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.6;
            fd_35D40J[4] = 2.3;

            fd_harmonic_35D40J[0] = 4.4;
            fd_harmonic_35D40J[1] = 3.9;
            fd_harmonic_35D40J[2] = 3.3;
            fd_harmonic_35D40J[3] = 2.6;
            fd_harmonic_35D40J[4] = 2.3;
        }
        else if (freq>=65 && freq<75)
        {
#ifdef EMP_430
            fd[0] = 4.3;
            fd[1] = 4.3;
            fd[2] = 3.8;
            fd[3] = 3.3;
            fd[4] = 2.6;
#elif (EMP_161 || EMP_360)
            fd[0] = 4.8;
            fd[1] = 4.5;
            fd[2] = 3.5;
            fd[3] = 2.6;
            fd[4] = 2.2;
#else
            fd[0] = 4.8;
            fd[1] = 4.0;
            fd[2] = 3.2;
            fd[3] = 2.6;
            fd[4] = 2.1;
#endif

            fd_harmonic[0] = 3.5;
            fd_harmonic[1] = 3.5;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.5;
            fd_harmonic[4] = 3.5;

            fd_35D40J[0] = 4.8;
            fd_35D40J[1] = 3.7;
            fd_35D40J[2] = 3.2;
            fd_35D40J[3] = 2.4;
            fd_35D40J[4] = 2.2;

            fd_harmonic_35D40J[0] = 4.8;
            fd_harmonic_35D40J[1] = 3.7;
            fd_harmonic_35D40J[2] = 3.2;
            fd_harmonic_35D40J[3] = 2.4;
            fd_harmonic_35D40J[4] = 2.2;
        }
        else if (freq>=75 && freq<90)
        {
#ifdef EMP_430
            fd[0] = 4.3;
            fd[1] = 4.3;
            fd[2] = 3.8;
            fd[3] = 3.3;
            fd[4] = 2.6;
#elif (EMP_161 || EMP_360)
            fd[0] = 5.3;
            fd[1] = 4.8;
            fd[2] = 4.0;
            fd[3] = 3.5;
            fd[4] = 2.5;
#else
            fd[0] = 5.0;
            fd[1] = 4.4;
            fd[2] = 3.2;
            fd[3] = 2.8;
            fd[4] = 2.2;
#endif

            fd_harmonic[0] = 4.0;
            fd_harmonic[1] = 4.0;
            fd_harmonic[2] = 4.0;
            fd_harmonic[3] = 4.0;
            fd_harmonic[4] = 4.0;

            fd_35D40J[0] = 4.9;
            fd_35D40J[1] = 3.9;
            fd_35D40J[2] = 3.3;
            fd_35D40J[3] = 2.4;
            fd_35D40J[4] = 2.0;

            fd_harmonic_35D40J[0] = 4.9;
            fd_harmonic_35D40J[1] = 3.9;
            fd_harmonic_35D40J[2] = 3.3;
            fd_harmonic_35D40J[3] = 2.4;
            fd_harmonic_35D40J[4] = 2.0;
        }
        else if (freq>=90 && freq<100)
        {
#ifdef EMP_322
            fd[0] = 5.3;
            fd[1] = 4.6;
            fd[2] = 3.2;
            fd[3] = 3.1;
            fd[4] = 2.7;
            fd_harmonic[0] = 4.6;
            fd_harmonic[1] = 4.6;
            fd_harmonic[2] = 4.6;
            fd_harmonic[3] = 4.6;
            fd_harmonic[4] = 4.6;
            fd_harmonic_35D40J[0] = 5.2;
            fd_harmonic_35D40J[1] = 4.7;
            fd_harmonic_35D40J[2] = 4.0;
            fd_harmonic_35D40J[3] = 3.5;
            fd_harmonic_35D40J[4] = 2.9;
#elif (EMP_161 || EMP_360)
            fd[0] = 5.3;
            fd[1] = 4.8;
            fd[2] = 4.0;
            fd[3] = 3.5;
            fd[4] = 2.5;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 5.0;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 5.0;
            fd_harmonic_35D40J[0] = 5.2;
            fd_harmonic_35D40J[1] = 4.7;
            fd_harmonic_35D40J[2] = 4.0;
            fd_harmonic_35D40J[3] = 3.5;
            fd_harmonic_35D40J[4] = 2.9;
#elif EMP_430
            fd[0] = 4.3;
            fd[1] = 4.3;
            fd[2] = 3.8;
            fd[3] = 3.3;
            fd[4] = 2.6;
            fd_harmonic[0] = 4.6;
            fd_harmonic[1] = 4.6;
            fd_harmonic[2] = 4.6;
            fd_harmonic[3] = 4.6;
            fd_harmonic[4] = 4.6;

            fd_harmonic_35D40J[0] = 4.6;
            fd_harmonic_35D40J[1] = 4.2;
            fd_harmonic_35D40J[2] = 3.8;
            fd_harmonic_35D40J[3] = 3.0;
            fd_harmonic_35D40J[4] = 2.6;
#else
            fd[0] = 5.3;
            fd[1] = 4.6;
            fd[2] = 3.2;
            fd[3] = 3.1;
            fd[4] = 2.7;
            fd_harmonic[0] = 4.6;
            fd_harmonic[1] = 4.6;
            fd_harmonic[2] = 4.6;
            fd_harmonic[3] = 4.6;
            fd_harmonic[4] = 4.6;

            fd_harmonic_35D40J[0] = 5.1;
            fd_harmonic_35D40J[1] = 4.2;
            fd_harmonic_35D40J[2] = 3.2;
            fd_harmonic_35D40J[3] = 2.3;
            fd_harmonic_35D40J[4] = 2.0;
#endif

            fd_35D40J[0] = 5.1;
            fd_35D40J[1] = 4.2;
            fd_35D40J[2] = 3.2;
            fd_35D40J[3] = 2.3;
            fd_35D40J[4] = 2.0;
        }
        else if(freq >=100 && freq <110)
        {
#ifdef EMP_322
            fd[0] = 5.4;
            fd[1] = 4.3;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.6;
            fd_harmonic[0] = 4.6;
            fd_harmonic[1] = 4.6;
            fd_harmonic[2] = 4.6;
            fd_harmonic[3] = 4.6;
            fd_harmonic[4] = 4.6;
            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
#elif (EMP_161 || EMP_360)
            fd[0] = 5.8;
            fd[1] = 5.2;
            fd[2] = 4.5;
            fd[3] = 3.5;
            fd[4] = 2.5;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 5.0;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 5.0;
            fd_harmonic_35D40J[0] = 5.2;
            fd_harmonic_35D40J[1] = 4.7;
            fd_harmonic_35D40J[2] = 4.0;
            fd_harmonic_35D40J[3] = 3.5;
            fd_harmonic_35D40J[4] = 2.9;
#else
            fd[0] = 5.5;
            fd[1] = 5.0;
            fd[2] = 4.6;
            fd[3] = 3.9;
            fd[4] = 3.2;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 5.0;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 5.0;
            fd_harmonic_35D40J[0] = 5.3;
            fd_harmonic_35D40J[1] = 4.4;
            fd_harmonic_35D40J[2] = 3.7;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.3;
#endif

            fd_35D40J[0] = 5.3;
            fd_35D40J[1] = 4.4;
            fd_35D40J[2] = 3.7;
            fd_35D40J[3] = 2.8;
            fd_35D40J[4] = 2.3;
		  }
        else if (freq>=110)
        {
#ifdef EMP_322
            fd[0] = 5.4;
            fd[1] = 4.3;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.6;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 5.0;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 5.0;
            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
#elif (EMP_161 || EMP_360)
            fd[0] = 6.0;
            fd[1] = 4.6;//5.4;
            fd[2] = 4.1;//4.5;
            fd[3] = 4.0;
            fd[4] = 3.0;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 5.0;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 5.0;

            fd_harmonic_35D40J[0] = 5.2;
            fd_harmonic_35D40J[1] = 4.7;
            fd_harmonic_35D40J[2] = 4.0;
            fd_harmonic_35D40J[3] = 3.5;
            fd_harmonic_35D40J[4] = 2.9;
#else
            fd[0] = 5.4;
            fd[1] = 4.3;
            fd[2] = 3.0;
            fd[3] = 2.8;
            fd[4] = 2.6;
            fd_harmonic[0] = 5.0;
            fd_harmonic[1] = 5.0;
            fd_harmonic[2] = 5.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 5.0;

            fd_harmonic_35D40J[0] = 5.3;
            fd_harmonic_35D40J[1] = 4.4;
            fd_harmonic_35D40J[2] = 3.7;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.3;
#endif

            fd_35D40J[0] = 5.3;
            fd_35D40J[1] = 4.4;
            fd_35D40J[2] = 3.7;
            fd_35D40J[3] = 2.8;
            fd_35D40J[4] = 2.3;
        }
    }
    else  if (probeR >= 20)		//低频微凸
    {
        if (freq < 60)
        {
            fd[0] = 4.5;
            fd[1] = 4.1;
            fd[2] = 3.3;
            fd[3] = 2.8;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;

            for(i = 0; i < 5; i++)
                fd_35D40J[i] = fd[i];
        }
        else if (freq>=60 && freq<85)
        {
            fd[0] = 5.0;
            fd[1] = 4.0;
            fd[2] = 3.4;
            fd[3] = 2.6;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;

            for(i = 0; i < 5; i++)
                fd_35D40J[i] = fd[i];
        }
        else if (freq < 135)
        {
            fd[0] = 5.0;
            fd[1] = 4.0;
            fd[2] = 3.0;
            fd[3] = 2.6;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;

            for(i = 0; i < 5; i++)
                fd_35D40J[i] = fd[i];
        }
        else
        {
            fd[0] = 5.0;
            fd[1] = 4.0;
            fd[2] = 3.0;
            fd[3] = 2.6;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;

            for(i = 0; i < 5; i++)
                fd_35D40J[i] = fd[i];
        }
    }
    else
    {
        if (freq < 55)
        {
            fd[0] = 4.0;
            fd[1] = 3.6;
            fd[2] = 3.4;
            fd[3] = 2.9;
            fd[4] = 2.6;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
        }
        else if (freq>=55 && freq<75)
        {
            fd[0] = 4.5;
            fd[1] = 3.6;
            fd[2] = 3.4;
            fd[3] = 2.6;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
        }
        else if (freq>=75 && freq<90)
        {
            fd[0] = 5.2;
            fd[1] = 4.0;
            fd[2] = 3.0;
            fd[3] = 2.4;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
        }
        else if (freq>=90 && freq<110)
        {
            fd[0] = 5.2;
            fd[1] = 4.0;
            fd[2] = 3.7;
            fd[3] = 3.0;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;

            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
        }
        else if (freq>=110)
        {
            fd[0] = 5.2;
            fd[1] = 4.0;
            fd[2] = 3.6;
            fd[3] = 2.8;
            fd[4] = 2.4;
            fd_harmonic[0] = 3.6;
            fd_harmonic[1] = 3.6;
            fd_harmonic[2] = 3.5;
            fd_harmonic[3] = 3.1;
            fd_harmonic[4] = 3.0;
            fd_harmonic_35D40J[0] = 4.3;
            fd_harmonic_35D40J[1] = 4.1;
            fd_harmonic_35D40J[2] = 3.4;
            fd_harmonic_35D40J[3] = 2.8;
            fd_harmonic_35D40J[4] = 2.2;
        }
        for(i = 0; i < 5; i++)
            fd_35D40J[i] = fd[i];
    }

#if (defined(EMP_340) || defined(EMP_430) || defined(EMP_161)||defined(EMP_360) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
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
            PRINTF("----- project test --Convex--%d----demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {

         //    fd[i] =  float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
           // fd_harmonic[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);

           // fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
           // fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
            PRINTF("-Convex--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
#else
	for (i = 0; i < 5; i++)
	{
		fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
		fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
		PRINTF("-Convex--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
	}

#endif
#endif

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
}