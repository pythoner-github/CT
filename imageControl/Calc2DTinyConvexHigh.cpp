/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: Calc2DTinyConvexHigh.cpp 65C15D
 * @brief: derived from class Calc2DTinyConvexHigh, deal with calculation of convex probe("65C15D").
 *
 * version: V1.0
 * date: 2010-1-7
 * @author: zhanglei
 */

#include "Calc2DTinyConvexHigh.h"
#include "Def.h"
#include "Img2D.h"
#include "TopArea.h"

const int Calc2DTinyConvexHigh::EMIT_CH_NUM[20] =	{20,24, 28, 36, 46, 56, 64,	64, 64,	64,	64,	64,	64,	64,	64,	64,	64,	64,	64,	64};

float Calc2DTinyConvexHigh::TIS_CFM_BASIC[2][12][11]= 
{
	{//4.9Mhz
		{0.0229, 0.0459, 0.0917, 0.1215, 0.1835, 0.2064, 0.2362, 0.2912, 0.4082, 0.5343, 0.6490}, 
		{0.0229, 0.0459, 0.0917, 0.1215, 0.1835, 0.2064, 0.2362, 0.2912, 0.4082, 0.5343, 0.6490}, 
		{0.0300, 0.0600, 0.1200, 0.1589, 0.2399, 0.2699, 0.3089, 0.3808, 0.5338, 0.6987, 0.8486}, 
		{0.0406, 0.0811, 0.1623, 0.2150, 0.3246, 0.3651, 0.4179, 0.5153, 0.7222, 0.9453, 1.1482}, 
		{0.0406, 0.0811, 0.1623, 0.2150, 0.3246, 0.3651, 0.4179, 0.5153, 0.7222, 0.9453, 1.1482}, 
		{0.0582, 0.1164, 0.2328, 0.3085, 0.4657, 0.5239, 0.5996, 0.7393, 1.0362, 1.3563, 1.6474}, 
		{0.0503, 0.1005, 0.2011, 0.2664, 0.4022, 0.4525, 0.5178, 0.6385, 0.8949, 1.1714, 1.4227}, 
		{0.0653, 0.1305, 0.2611, 0.3459, 0.5221, 0.5874, 0.6723, 0.8289, 1.1618, 1.5207, 1.8471}, 
		{0.0653, 0.1305, 0.2611, 0.3459, 0.5221, 0.5874, 0.6723, 0.8289, 1.1618, 1.5207, 1.8471}, 
		{0.0759, 0.1517, 0.3034, 0.4020, 0.6068, 0.6827, 0.7813, 0.9633, 1.3501, 1.7673, 2.1466}, 
		{0.0759, 0.1517, 0.3034, 0.4020, 0.6068, 0.6827, 0.7813, 0.9633, 1.3501, 1.7673, 2.1466}, 
		{0.0935, 0.1870, 0.3740, 0.4955, 0.7479, 0.8414, 0.9630, 1.1873, 1.6641, 2.1783, 2.6458}, 
	}, 
	{//5.8Mhz
		{0.0271, 0.0543, 0.1004, 0.1628, 0.2089, 0.2442, 0.2903, 0.3527, 0.4883, 0.6430, 0.7597}, 
		{0.0271, 0.0543, 0.1004, 0.1628, 0.2089, 0.2442, 0.2903, 0.3527, 0.4883, 0.6430, 0.7597}, 
		{0.0355, 0.0710, 0.1313, 0.2129, 0.2732, 0.3193, 0.3796, 0.4612, 0.6386, 0.8408, 0.9934}, 
		{0.0480, 0.0960, 0.1776, 0.2880, 0.3696, 0.4320, 0.5136, 0.6240, 0.8640, 1.1376, 1.3440}, 
		{0.0480, 0.0960, 0.1776, 0.2880, 0.3696, 0.4320, 0.5136, 0.6240, 0.8640, 1.1376, 1.3440}, 
		{0.0689, 0.1377, 0.2548, 0.4132, 0.5303, 0.6198, 0.7369, 0.8953, 1.2397, 1.6322, 1.9283}, 
		{0.0595, 0.1190, 0.2201, 0.3569, 0.4580, 0.5353, 0.6364, 0.7732, 1.0706, 1.4096, 1.6654}, 
		{0.0772, 0.1544, 0.2857, 0.4633, 0.5946, 0.6950, 0.8262, 1.0038, 1.3899, 1.8301, 2.1621}, 
		{0.0772, 0.1544, 0.2857, 0.4633, 0.5946, 0.6950, 0.8262, 1.0038, 1.3899, 1.8301, 2.1621}, 
		{0.0897, 0.1795, 0.3320, 0.5384, 0.6910, 0.8077, 0.9602, 1.1666, 1.6153, 2.1268, 2.5127}, 
		{0.0897, 0.1795, 0.3320, 0.5384, 0.6910, 0.8077, 0.9602, 1.1666, 1.6153, 2.1268, 2.5127}, 
		{0.1106, 0.2212, 0.4093, 0.6637, 0.8517, 0.9955, 1.1835, 1.4379, 1.9910, 2.6214, 3.0970}, 
	}, 
};
float Calc2DTinyConvexHigh::TIS_CFM_POWER[10] =
{
	0, 0, 0.20, 0.20, 0.40, 0.40, 0.74, 0.74, 1.00, 1.00, 	
};
float Calc2DTinyConvexHigh::TIS_CFM_BOX_LINES[8] =
{
	0.9069, 1.0000, 0.9759, 0.8862, 0.8034, 0.7483, 0.6793, 0.6310,	
};
float Calc2DTinyConvexHigh::TIS_PW_BASIC[2][15] = //freq-prf
{ 
	{0.0630, 0.0988, 0.1261, 0.1891, 0.2312, 0.2795, 0.3152, 0.3510, 0.3993, 0.4413, 0.4834, 0.5254, 0.5737, 0.6095, 0.6515},	
	{0.0671, 0.1243, 0.1566, 0.2064, 0.2561, 0.3232, 0.3556, 0.4227, 0.4724, 0.5221, 0.5719, 0.6042, 0.6713, 0.7037, 0.7708}, 
};

///>
Calc2DTinyConvexHigh::Calc2DTinyConvexHigh()
{

}

Calc2DTinyConvexHigh::~Calc2DTinyConvexHigh()
{

}

void Calc2DTinyConvexHigh::CalcEmitDelay()
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
float Calc2DTinyConvexHigh::Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex)
{
	return 0.0;
}
float Calc2DTinyConvexHigh::GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd)
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
float Calc2DTinyConvexHigh::GetPwTis(int dopFreq, int prfIndex)
{
	prfIndex = GetPwPrfIndex(prfIndex);	
	int freqIndex = GetColorFreqIndex(dopFreq);

	float ti = TIS_PW_BASIC[freqIndex][prfIndex];
	PRINTF("PW TI = %f\n", ti);

	return ti;	
}
void Calc2DTinyConvexHigh::CalcFilter()
{
	CalcDynamicDemod(0);
	CalcFilterBandPass(0);
	CalcDynamicFilter(0);
}

void Calc2DTinyConvexHigh::CalcFilterFreqCompound(int order)
{
	CalcDynamicDemod(order);
	CalcFilterBandPass(order);
	CalcDynamicFilter(order);
}

///> private
int Calc2DTinyConvexHigh::GetFocIndex(float focPos)
{
	int focIndex = 0;

	if (focPos <= 0.2)
		focIndex = 0;
	else if ((focPos > 0.2) && (focPos <= 0.4))
		focIndex = 1;
	else if ((focPos > 0.4) && (focPos <= 0.9))
		focIndex = 2;
	else if ((focPos > 0.9) && (focPos <= 1.4))
		focIndex = 3;
	else if ((focPos > 1.4) && (focPos <= 1.9))
		focIndex = 4;
	else if ((focPos > 1.9) && (focPos <= 2.4))
		focIndex = 5;
	else if ((focPos > 2.4) && (focPos <= 2.9))
		focIndex = 6;
	else if ((focPos > 2.9) && (focPos <= 3.4))
		focIndex = 7;
	else if ((focPos > 3.4) && (focPos <= 3.9))
		focIndex = 8;
	else if ((focPos > 3.9) && (focPos <= 4.4))
		focIndex = 9;
	else if ((focPos > 4.4) && (focPos <= 4.9))
		focIndex = 10;
	else if ((focPos > 4.9) && (focPos <= 5.4))
		focIndex = 11;
	else if (focPos > 5.4)
		focIndex = 11;	

	return focIndex;
}

int Calc2DTinyConvexHigh::Get2DFreqReceiveIndex(int freq)
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

int Calc2DTinyConvexHigh::GetColorFreqIndex(int freq)
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
void Calc2DTinyConvexHigh::CalcFilterBandPass(int order)
{
    int probeR = m_calcPara->probeR /100;
    int depth[5];
    int probeIndex = 0;
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    string probe_type = TopArea::GetInstance()->GetProbeType();
    
    if (probeR < 20)
	{
		depth[0] = 40;
		depth[1] = 60;
		depth[2] = 80;
		depth[3] = 100;
		depth[4] = 120;
	}

    for (int i = 0; i < NUM_PROBE; ++i) 
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

#if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) ||defined(EMP_161) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
    float fc_bandpass_filter1[5], fc_bandpass_filter2[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
		{
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i] / 10.0);
			//printf("-Tiny Convex High--%d---THI---w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
		}
    }
    else
    {
        for(int i = 0; i < 5; i++)
		{
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
			//printf("-Tiny Convex High--%d-----w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
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

void Calc2DTinyConvexHigh::CalcDynamicFilter(int order)
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
    int probeR = m_calcPara->probeR /100;
	int depth[5];
    
    if (probeR < 20)
    {
        depth[0] = 40;
        depth[1] = 60;
        depth[2] = 80;
        depth[3] = 100;
        depth[4] = 120;
    }

    float fc_dynamic_filter[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
		{
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
			//printf("-Tiny Convex High--%d--THI---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
		}
    }
    else
    {
        for(int i = 0; i < 5; i++)
		{
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
			//printf("-Tiny Convex High--%d---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
		}
    }
    DynamicFilter_test(fc_dynamic_filter, depth, order);

    //DynamicFilter(order);
}    
void Calc2DTinyConvexHigh::CalcDynamicDemod(int order)
{
	int i;
	int probeR = m_calcPara->probeR /100;
    int freq = m_calcPara->freq.receive;
    int depth[5];
    float fd[5];
    float fd_harmonic[5];

    for(int i = 0; i < 5; i++)
        fd_harmonic[i] = (float)freq / 10.0;
    if (probeR < 20)
    {
        depth[0] = 40;
        depth[1] = 60;
        depth[2] = 80;
        depth[3] = 100;
        depth[4] = 120;
    }

    if (probeR < 20)		//高频微凸
    {
        if (freq < 90)
        {
#ifdef EMP_430
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.5;
            fd[3] = 5.0;
            fd[4] = 4.0;
            fd_harmonic[0] = 7.3;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 6.1;
            fd_harmonic[3] = 5.4;
            fd_harmonic[4] = 4.0;
#elif EMP_360
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.0;
            fd[3] = 4.0;
            fd[4] = 3.5;
            fd_harmonic[0] = 8.0;
            fd_harmonic[1] = 7.0;
            fd_harmonic[2] = 6.0;
            fd_harmonic[3] = 5.0;
            fd_harmonic[4] = 3.5;
#else
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.4;
            fd[3] = 5.0;
            fd[4] = 4.6;
            fd_harmonic[0] = 7.3;
            fd_harmonic[1] = 6.5;
            fd_harmonic[2] = 6.1;
            fd_harmonic[3] = 5.4;
            fd_harmonic[4] = 4.0;
#endif

        }
        else if (freq >= 90 && freq < 100)
        {
#ifdef EMP_430
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.5;
            fd[3] = 5.0;
            fd[4] = 4.0;
            fd_harmonic[0] = 8.5;
            fd_harmonic[1] = 7.1;
            fd_harmonic[2] = 6.5;
            fd_harmonic[3] = 5.5;
            fd_harmonic[4] = 4.5;
#elif EMP_360
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.0;
            fd[3] = 4.0;
            fd[4] = 3.5;
            fd_harmonic[0] = 9.0;
            fd_harmonic[1] = 8.0;
            fd_harmonic[2] = 7.0;
            fd_harmonic[3] = 6.0;
            fd_harmonic[4] = 4.5;
#else
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.4;
            fd[3] = 5.0;
            fd[4] = 4.6;
            fd_harmonic[0] = 8.5;
            fd_harmonic[1] = 7.1;
            fd_harmonic[2] = 6.5;
            fd_harmonic[3] = 5.5;
            fd_harmonic[4] = 4.5;
#endif

        }
        else if (freq >= 100 && freq < 110)
        {
#ifdef EMP_430
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.5;
            fd[3] = 5.0;
            fd[4] = 4.0;
            fd_harmonic[0] = 9.6;
            fd_harmonic[1] = 8.9;
            fd_harmonic[2] = 7.6;
            fd_harmonic[3] = 6.5;
            fd_harmonic[4] = 5.0;
#elif EMP_360
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.0;
            fd[3] = 4.0;
            fd[4] = 3.5;
            fd_harmonic[0] = 10.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.5;
            fd_harmonic[3] = 6.5;
            fd_harmonic[4] = 4.5;
#else
            fd[0] = 6.5;
            fd[1] = 6.0;
            fd[2] = 5.4;
            fd[3] = 5.0;
            fd[4] = 4.6;
            fd_harmonic[0] = 9.6;
            fd_harmonic[1] = 8.9;
            fd_harmonic[2] = 7.6;
            fd_harmonic[3] = 6.5;
            fd_harmonic[4] = 5.0;
#endif
        }
        else if (freq>=110 && freq<135)
        {
#ifdef EMP_430
            fd[0] = 7.0;
            fd[1] = 6.5;
            fd[2] = 6.0;
            fd[3] = 5.5;
            fd[4] = 4.5;
            fd_harmonic[0] = 11.7;
            fd_harmonic[1] = 10.2;
            fd_harmonic[2] = 7.7;
            fd_harmonic[3] = 7.0;
            fd_harmonic[4] = 5.0;
#elif EMP_360
            fd[0] = 7.5;
            fd[1] = 7.0;
            fd[2] = 6.0;
            fd[3] = 5.0;
            fd[4] = 4.0;
            fd_harmonic[0] = 10.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.5;
            fd_harmonic[3] = 6.5;
            fd_harmonic[4] = 4.5;
#else
            fd[0] = 7.5;
            fd[1] = 7.0;
            fd[2] = 6.4;
            fd[3] = 5.2;
            fd[4] = 4.8;
            fd_harmonic[0] = 11.7;
            fd_harmonic[1] = 10.2;
            fd_harmonic[2] = 7.7;
            fd_harmonic[3] = 7.0;
            fd_harmonic[4] = 5.0;
#endif

        }
        else if (freq>=135 && freq<165)
        {
#ifdef EMP_430
            fd[0] = 8.0;
            fd[1] = 7.5;
            fd[2] = 7.0;
            fd[3] = 6.5;
            fd[4] = 5.5;
            fd_harmonic[0] = 11.7;
            fd_harmonic[1] = 10.2;
            fd_harmonic[2] = 7.7;
            fd_harmonic[3] = 7.0;
            fd_harmonic[4] = 5.0;
#elif EMP_360
            fd[0] = 9.0;
            fd[1] = 8.0;
            fd[2] = 7.0;
            fd[3] = 5.5;
            fd[4] = 4.0;
            fd_harmonic[0] = 10.0;
            fd_harmonic[1] = 9.0;
            fd_harmonic[2] = 7.5;
            fd_harmonic[3] = 6.5;
            fd_harmonic[4] = 4.5;
#else
            fd[0] = 8.0;
            fd[1] = 7.2;
            fd[2] = 6.5;
            fd[3] = 5.5;
            fd[4] = 4.8;
            fd_harmonic[0] = 11.7;
            fd_harmonic[1] = 10.2;
            fd_harmonic[2] = 7.7;
            fd_harmonic[3] = 7.0;
            fd_harmonic[4] = 5.0;
#endif
        }
        else if (freq>=165)
        {
            fd[0] = 9.0;
            fd[1] = 7.8;
            fd[2] = 7.2;
            fd[3] = 6.8;
            fd[4] = 4.8;
            fd_harmonic[0] = 11.7;
            fd_harmonic[1] = 10.2;
            fd_harmonic[2] = 7.7;
            fd_harmonic[3] = 7.0;
            fd_harmonic[4] = 5.0;
        }
    }

	// 65C15D---G70
#if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) ||defined(EMP_161) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
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

            fd[i] =  m_projectCalcPara->bandPassW[i] / 10.0;	
            fd_harmonic[i] =  m_projectCalcPara->bandPassW[i] / 10.0;	
            PRINTF("----- project test --TinyConvexHigh--%d----demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
    else 
    {  	
        for (i = 0; i < 5; i++)
        {      
           // fd[i] =  float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] ) / 10.0;
           // fd_harmonic[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i] ) / 10.0;

            // fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
            // fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
            //printf("-Tiny Convex High--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
#else
    for (i = 0; i < 5; i++)
    {      
        fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
        fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
        //printf("-Tiny Convex High--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
    }
#endif
#endif

    if(m_calcPara->harmonic)
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
