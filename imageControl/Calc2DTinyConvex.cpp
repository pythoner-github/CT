#include "imageControl/Calc2DTinyConvex.h"
#include "Def.h"
#include "imageControl/Img2D.h"
#include "display/TopArea.h"

const int Calc2DTinyConvex::EMIT_CH_NUM[20] =   {20,22, 24, 26, 30, 34, 38, 46, 50, 58, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};

///>
Calc2DTinyConvex::Calc2DTinyConvex()
{

}

Calc2DTinyConvex::~Calc2DTinyConvex()
{

}

void Calc2DTinyConvex::CalcEmitDelay()
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

void Calc2DTinyConvex::CalcFocPos()
{
    int start = 0;

    FocChange(start);

    ///> 2D pulse number according to last focus pos
    int pos = m_calcPara->focPos[m_calcPara->focSum-1];
#ifdef CT_355
 m_fpga.Send2DPulseNum(1);
#else
    if (pos > 120) //mm
        m_fpga.Send2DPulseNum(1);
    else
        m_fpga.Send2DPulseNum(2);
#endif
    CalcEmitDelay();
}

void Calc2DTinyConvex::CalcDefaultDemodFd()
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

void Calc2DTinyConvex::CalcFilter()
{
    CalcDynamicDemod(0);
    CalcFilterBandPass(0);
    CalcDynamicFilter(0);
}

void Calc2DTinyConvex::CalcFilterFreqCompound(int order)
{
    CalcDynamicDemod(order);
    CalcFilterBandPass(order);
    CalcDynamicFilter(order);
}

///> private
void Calc2DTinyConvex::CalcFilterBandPass(int order)
{
    int depth[5];
    int probeIndex = 0;
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    string probe_type = TopArea::GetInstance()->GetProbeType();

#if defined(CT_340)
    depth[0] = 30;
    depth[1] = 80;
    depth[2] = 120;
    depth[3] = 160;
    depth[4] = 220;
#elif defined(CT_355)
    depth[0] = 20;
    depth[1] = 60;
    depth[2] = 100;
    depth[3] = 160;
    depth[4] = 220;
#else
   depth[0] = 100;
    depth[1] = 120;
    depth[2] = 160;
    depth[3] = 200;
    depth[4] = 250;
#endif

    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

#if (defined(CT_340) || defined(CT_430) || defined(CT_360) || (defined(CT_161)) || defined(CT_355) || defined(CT_322) || defined(CT_440))
    float fc_bandpass_filter1[5], fc_bandpass_filter2[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i] / 10.0);
            PRINTF("-Tiny convex--%d---THI---w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            PRINTF("-Tiny convex--%d-----w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
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
void Calc2DTinyConvex::CalcDynamicFilter(int order)
{
    int depth[5];
    int probeIndex = 0;
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    string probe_type = TopArea::GetInstance()->GetProbeType();
#if (defined(CT_355)||defined(CT_340))
    depth[0] = 40;
    depth[1] = 80;
    depth[2] = 120;
    depth[3] = 180;
    depth[4] = 220;
#else
    depth[0] = 100;
    depth[1] = 120;
    depth[2] = 160;
    depth[3] = 200;
    depth[4] = 250;
#endif

    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

    float fc_dynamic_filter[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);
            PRINTF("-Tiny Convex--%d--THI---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            PRINTF("-Tiny Convex--%d---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
        }
    }
    DynamicFilter_test(fc_dynamic_filter, depth, order);

    //DynamicFilter(order);
}
void Calc2DTinyConvex::CalcDynamicDemod(int order)
{
    //int probeR = m_calcPara->probeR;
    int i;
    int freq = m_calcPara->freq.receive;
    int depth[5];
    float fd[5];
    float fd_harmonic[5];
#if (defined(CT_340)||defined(CT_355))
    depth[0] = 20;
    depth[1] = 60;
    depth[2] = 100;
    depth[3] = 160;
    depth[4] = 220;
#else
    depth[0] = 100;
    depth[1] = 120;
    depth[2] = 160;
    depth[3] = 200;
    depth[4] = 250;
#endif

    if(freq < 45)
    {
#ifdef CT_161 || CT_360
        fd[0] = 4.0;
        fd[1] = 3.5;
        fd[2] = 3.0;
        fd[3] = 2.5;
        fd[4] = 2.2;
        fd_harmonic[0] = 4.0;
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
    }
    else if(freq >= 45 && freq < 50)
    {
#ifdef CT_161 || CT_360
        fd[0] = 4.0;
        fd[1] = 3.5;
        fd[2] = 3.0;
        fd[3] = 2.5;
        fd[4] = 2.2;
        fd_harmonic[0] = 4.5;
        fd_harmonic[1] = 4.3;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.5;
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
    }
    else if(freq >= 50 && freq < 60)
    {
#ifdef CT_161 || CT_360
        fd[0] = 4.0;
        fd[1] = 3.5;
        fd[2] = 3.0;
        fd[3] = 2.5;
        fd[4] = 2.2;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 4.8;
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
    }
    else if(freq >= 60 && freq < 70)
    {
#ifdef CT_161 || CT_360
        fd[0] = 4.5;
        fd[1] = 4.2;
        fd[2] = 3.5;
        fd[3] = 2.5;
        fd[4] = 2.2;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 4.8;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 2.5;
#else
        fd[0] = 4.3;
        fd[1] = 3.8;
        fd[2] = 3.1;
        fd[3] = 2.8;
        fd[4] = 2.3;
        fd_harmonic[0] = 3.5;
        fd_harmonic[1] = 3.5;
        fd_harmonic[2] = 3.5;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 3.5;
#endif
    }
    else if (freq>= 70 && freq < 80)
    {
#ifdef CT_161 || CT_360
        fd[0] = 4.5;
        fd[1] = 4.0;
        fd[2] = 3.5;
        fd[3] = 2.5;
        fd[4] = 2.2;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 4.8;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 2.5;
#else
        fd[0] = 4.5;
        fd[1] = 4.0;
        fd[2] = 3.6;
        fd[3] = 3.2;
        fd[4] = 2.8;
        fd_harmonic[0] = 3.5;
        fd_harmonic[1] = 3.5;
        fd_harmonic[2] = 3.5;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 3.5;
#endif
    }
    else if (freq >=80 && freq < 90)
    {
#ifdef CT_161 || CT_360
        fd[0] = 5.0;
        fd[1] = 4.0;
        fd[2] = 3.5;
        fd[3] = 2.5;
        fd[4] = 2.5;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 4.8;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 2.5;
#else
        fd[0] = 5.6;
        fd[1] = 5.0;
        fd[2] = 4.6;
        fd[3] = 3.6;
        fd[4] = 2.8;
        fd_harmonic[0] = 4.0;
        fd_harmonic[1] = 4.0;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 4.0;
        fd_harmonic[4] = 4.0;
#endif
    }
    else if (freq >= 90 && freq < 100)
    {
#ifdef CT_161 || CT_360
        fd[0] = 5.0;
        fd[1] = 4.0;
        fd[2] = 3.5;
        fd[3] = 2.5;
        fd[4] = 2.5;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 4.8;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 2.5;
#else
        fd[0] = 5.6;
        fd[1] = 5.0;
        fd[2] = 4.6;
        fd[3] = 3.6;
        fd[4] = 2.8;
        fd_harmonic[0] = 4.6;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.6;
        fd_harmonic[3] = 4.6;
        fd_harmonic[4] = 4.6;
#endif
    }
    else if (freq >= 100 && freq < 135)
    {
#ifdef CT_161 || CT_360
        fd[0] = 5.0;
        fd[1] = 4.0;
        fd[2] = 3.5;
        fd[3] = 3.0;
        fd[4] = 2.5;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 4.8;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.5;
        fd_harmonic[4] = 2.5;
#else
        fd[0] = 5.6;
        fd[1] = 5.0;
        fd[2] = 4.6;
        fd[3] = 3.6;
        fd[4] = 2.8;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 5.0;
        fd_harmonic[2] = 5.0;
        fd_harmonic[3] = 5.0;
        fd_harmonic[4] = 5.0;
#endif
    }
    else
    {
        fd[0] = 5.6;
        fd[1] = 5.0;
        fd[2] = 4.6;
        fd[3] = 3.6;
        fd[4] = 2.8;
        fd_harmonic[0] = 5.0;
        fd_harmonic[1] = 5.0;
        fd_harmonic[2] = 5.0;
        fd_harmonic[3] = 5.0;
        fd_harmonic[4] = 5.0;
    }

    // 35C20H---G70
#if (defined(CT_340) || defined(CT_430) || defined(CT_360) || (defined(CT_161)) || defined(CT_355) || defined(CT_322))
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int probeIndex = 0;
    string probe_type = TopArea::GetInstance()->GetProbeType();
    for (i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

#ifdef CT_PROJECT
    if (m_projectCalcPara->filterSecIndex)
    {
        for(i = 0; i < 5; i++)
        {

            fd[i] =  float(m_projectCalcPara->bandPassW[i] / 10.0);
            fd_harmonic[i] =  float(m_projectCalcPara->bandPassW[i] / 10.0);

            PRINTF("----- project test --TinyConvex--%d----demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
           // fd[i] =  float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i]) / 10.0;
           // fd_harmonic[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i]) / 10.0;

            // fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
            // fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
            PRINTF("-Tiny Convex--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }
    }

#else

    for (i = 0; i < 5; i++)
    {
        fd[i] =  float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
        fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
        PRINTF("-Tiny Convex--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
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