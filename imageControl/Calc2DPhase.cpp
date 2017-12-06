#include "imageControl/Calc2DPhase.h"
#include "Def.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/ScanMode.h"
#include "probe/ProbeSocket.h"
#include "imageControl/Img2D.h"
#include "display/TopArea.h"

const int Calc2DPhase::EMIT_CH_NUM_P[20] =  {8,10, 12, 14, 18, 32, 64,  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};

///>pulic
Calc2DPhase::Calc2DPhase()
{
}
Calc2DPhase::~Calc2DPhase()
{

}

void Calc2DPhase::CalcEmitDelay()
{
    int focSum = m_calcPara->focSum;
    int size = sizeof(EMIT_CH_NUM_P) / sizeof(int);

    int i;
    for (i = 0; i < focSum; i ++)
    {
        PEmitDelay(i, EMIT_CH_NUM_P, size);
    }

    PEmitDelayPw((float)m_calcPara->focPos[0], EMIT_CH_NUM_P, size);

#if 0
    ModeStatus modeStatus;
    ScanMode::EScanMode mode = modeStatus.GetScanMode();

    if ((mode == ScanMode::CW) || (mode == ScanMode::CWCFM) || (mode == ScanMode::CWPDI))

        PEmitDelayPw((float)m_calcPara->focPos[0], EMIT_CH_NUM_P, size);
    else
    {

        int i;
        for (i = 0; i < focSum; i ++)
        {
            PEmitDelay(i, EMIT_CH_NUM_P, size);
        }
    }

    //PEmitDelayPw((float)m_calcPara->focPos[0], EMIT_CH_NUM_P, size);
#endif
}
void Calc2DPhase::CalcEmitDelayPw(float focPos)
{
    int size = sizeof(EMIT_CH_NUM_P) / sizeof(int);
    PEmitDelayPw(focPos, EMIT_CH_NUM_P, size);
}
void Calc2DPhase::CalcEmitDelayCfm(float focPos)
{
    int size = sizeof(EMIT_CH_NUM_P) / sizeof(int);
    PEmitDelayCfm(focPos, EMIT_CH_NUM_P, size);
}

void Calc2DPhase::CalcReceiveDelay()
{
    bool compound = m_calcPara->compoundFreqCtrl || m_calcPara->compoundSpaceCtrl;

    bool isCw = m_calcPara->modeCw;

    if (!isCw)
    {
        PReceiveDelay(compound);
    }
}
void Calc2DPhase::CalcReceiveDelayColor()
{
    PReceiveDelayColor();
}

void Calc2DPhase::CalcReceiveAperture()
{
    bool compound = m_calcPara->compoundFreqCtrl || m_calcPara->compoundSpaceCtrl;
    PReceiveAperture(compound);
}

void Calc2DPhase::CalcDefaultDemodFd()
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

void Calc2DPhase::CalcFilter()
{
    CalcDynamicDemod(0);
    CalcFilterBandPass(0);
    CalcDynamicFilter(0);
}

void Calc2DPhase::CalcFilterFreqCompound(int order)
{
    CalcDynamicDemod(order);
    CalcFilterBandPass(order);
    CalcDynamicFilter(order);
}

void Calc2DPhase::CalcFilterBandPass(int order)
{
    int harmonicIndex = Img2D::GetInstance()->GetHarmonicFreqIndex();
    int probeIndex = 0;
    string probe_type = TopArea::GetInstance()->GetProbeType();
    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
            probeIndex = i;
    }

    //int probeR = m_calcPara->probeR;
    int freq = m_calcPara->freq.receive;
    int freqIndex = Img2D::GetInstance()->GetFreqIndex();
    int depth[5];
    float fd[5];
    bool harmonic = m_calcPara->harmonic;
    float fd_harmonic[5];

    if(m_calcPara->harmonic)
    {
        depth[0] = 50;
        depth[1] = 100;
        depth[2] = 150;
        depth[3] = 200;
        depth[4] = 250;
    }
    else
    {
        depth[0] = 100;
        depth[1] = 120;
        depth[2] = 160;
        depth[3] = 200;
        depth[4] = 250;
    }

 #if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) || defined(EMP_161) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
    float fc_bandpass_filter1[5], fc_bandpass_filter2[5];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i] / 10.0);
            //printf("-phase--%d---THI---w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            fc_bandpass_filter1[i] = float(ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            fc_bandpass_filter2[i] = float(ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            //printf("-phase--%d-----w1 = %.2f, w2 = %.2f\n", i, fc_bandpass_filter1[i], fc_bandpass_filter2[i]);
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

void Calc2DPhase::CalcDynamicFilter(int order)
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

    if(m_calcPara->harmonic)
    {
        depth[0] = 50;
        depth[1] = 100;
        depth[2] = 150;
        depth[3] = 200;
        depth[4] = 250;

    }
    else
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
            //printf("-phase--%d--THI---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            fc_dynamic_filter[i] = float(ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
            //printf("-phase--%d---low pass filter = %.2f\n", i, fc_dynamic_filter[i]);
        }
    }

    DynamicFilter_test(fc_dynamic_filter, depth, order);
}

void Calc2DPhase::CalcDynamicDemod(int order)
{
    //int probeR = m_calcPara->probeR;
    int freq = m_calcPara->freq.receive;
    int depth[5];
    float fd[5];
    bool harmonic = m_calcPara->harmonic;
    float fd_harmonic[5];
#if 0
    fd_harmonic[0] = float(freq / 10.0);
    fd_harmonic[1] = float(freq / 10.0);
    fd_harmonic[2] = float(freq / 10.0);
    fd_harmonic[3] = float(freq / 10.0);
    fd_harmonic[4] = float(freq / 10.0);
#endif

    if(harmonic)
    {
        depth[0] = 50;
        depth[1] = 100;
        depth[2] = 150;
        depth[3] = 200;
        depth[4] = 250;
    }
    else
    {
        depth[0] = 100;
        depth[1] = 120;
        depth[2] = 160;
        depth[3] = 200;
        depth[4] = 250;
    }

    if (freq < 45)
    {
        fd[0] = 4.5;//5.8;
        fd[1] = 3.9;//5.2;
        fd[2] = 3.3;//5.0;
        fd[3] = 2.8;//4.8;
        fd[4] = 2.5;//4.4;
        fd_harmonic[0] = 4.6;//5.3;
        fd_harmonic[1] = 4.3;//4.6;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.2;
        fd_harmonic[4] = 3.0;
    }
    else if (freq >=45 && freq <50 )
    {
        fd[0] = 4.5;//5.8;
        fd[1] = 3.9;//5.2;
        fd[2] = 3.3;//5.0;
        fd[3] = 2.8;//4.8;
        fd[4] = 2.5;//4.4;
        fd_harmonic[0] = 5.2;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.2;
        fd_harmonic[3] = 3.4;
        fd_harmonic[4] = 3.2;
    }
    else if (freq >= 50 && freq < 55)
    {
        fd[0] = 4.5;//5.8;
        fd[1] = 3.9;//5.2;
        fd[2] = 3.3;//5.0;
        fd[3] = 2.8;//4.8;
        fd[4] = 2.5;//4.4;
        fd_harmonic[0] = 5.4;
        fd_harmonic[1] = 4.8;
        fd_harmonic[2] = 4.4;
        fd_harmonic[3] = 4.0;
        fd_harmonic[4] = 2.4;
    }
    else if(freq >= 55 && freq < 70)
    {
        fd[0] = 4.7;
        fd[1] = 4.1;
        fd[2] = 3.6;
        fd[3] = 3.2;
        fd[4] = 2.8;
        fd_harmonic[0] = 5.3;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.2;
        fd_harmonic[4] = 3.0;
    }
    else if (freq>=70 && freq<75)
    {
        fd[0] = 4.7;
        fd[1] = 3.9;
        fd[2] = 3.2;
        fd[3] = 3.0;
        fd[4] = 3.0;
        fd_harmonic[0] = 5.3;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.2;
        fd_harmonic[4] = 3.0;
    }
    else if (freq>=75 && freq<90)
    {
        fd[0] = 5.0;
        fd[1] = 4.4;
        fd[2] = 3.7;
        fd[3] = 3.3;
        fd[4] = 3.2;
        fd_harmonic[0] = 5.3;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.2;
        fd_harmonic[4] = 3.0;
    }
    else if (freq>=90 && freq<110)
    {
        fd[0] = 5.8;
        fd[1] = 4.8;
        fd[2] = 4.3;
        fd[3] = 4.0;
        fd[4] = 3.5;
        fd_harmonic[0] = 5.3;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.2;
        fd_harmonic[4] = 3.0;
    }
    else if (freq>=110)
    {
        fd[0] = 5.8;
        fd[1] = 4.8;
        fd[2] = 4.3;
        fd[3] = 4.0;
        fd[4] = 3.5;
        fd_harmonic[0] = 5.3;
        fd_harmonic[1] = 4.6;
        fd_harmonic[2] = 4.0;
        fd_harmonic[3] = 3.2;
        fd_harmonic[4] = 3.0;
    }

    // 30P16A---G70
 #if (defined(EMP_340) || defined(EMP_430) || defined(EMP_360) || defined(EMP_161) || defined(EMP_355) || defined(EMP_322) || defined(EMP_440))
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

                PRINTF("----- project test --Phase--%d----demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
            }
        }
        else
        {

            for (i = 0; i < 5; i++)
            {
                //fd[i] =  float(ProbeSocket::BAND_PASS_FILTER_FC_BASE_FREQ[probeIndex][freqIndex][i] / 10.0);
                // fd_harmonic[i] = float(ProbeSocket::BAND_PASS_FILTER_FC[probeIndex][harmonicIndex][i] / 10.0);

                //  fd[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
                //fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
                //printf("-phase--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
            }
        }

#else
        for (i = 0; i < 5; i++)
        {
            fd[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[probeIndex][freqIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[probeIndex][freqIndex][i]) / 20.0);
            fd_harmonic[i] = float((ProbeSocket::BAND_PASS_FILTER_FC1[probeIndex][harmonicIndex][i] + ProbeSocket::BAND_PASS_FILTER_FC2[probeIndex][harmonicIndex][i]) / 20.0);
            //printf("-phase--%d---demod freq basic = %.2f, THI= %.2f\n", i, fd[i], fd_harmonic[i]);
        }

#endif
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