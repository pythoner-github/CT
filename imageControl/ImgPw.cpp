#include <math.h>
#include "imageControl/ImgPw.h"
#include "imageControl/Img2D.h"
#include "imageProc/GlobalClassMan.h"
#include "display/HintArea.h"
#include "imageProc/ScanMode.h"
#include "imageProc/Zoom.h"
#include "periDevice/IoCtrl.h"
#include "probe/ProbeMan.h"
#include "ViewMain.h"

int soundstatus=0;
int ImgPw::SPECTRUM_SPEED[MAX_SPEED_INDEX] = {48, 64, 80, 96, 112, 127};//{24, 32, 40, 48, 56,60}; //PRF=8000hz
ImgPw* ImgPw::m_ptrInstance = NULL;
//const int ImgPw::SV_POS_PRF[] = {54, 60, 65, 70, 75, 80, 87, 94, 103, 112, 124, 138, 155, 177, 204, 241, 292}; // sv pos unit:mm, prf index begin from 18 to 2
const int ImgPw::SV_POS_PRF[2][POS_PRF_SUM] =
{
    {70, 100, 120, 138, 150, 177, 204, 241, 1000}, //sv pos unit:mm
    //{300, 300, 300, 300, 300, 300, 300, 300, 1000}, //sv pos unit:mm
    {8000, 6000, 5000, 4500, 4000, 3500, 3000, 2500, 2000}, // prf unit:hz
    //{8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000}, // prf unit:hz
};

const int ImgPw::DYNAMIC_RANGE[2] = {5, 120};// {1, 121};

#ifdef EMP_355
const unsigned int ImgPw::WALL_FILTER_FREQ[MAX_PRF][MAX_WALL_FILTER] =  //hz
{
    {7,10,14,17,21,28,35,52}, //350
    {10,15,20,25,30,40,50,75}, //500
    {15,22,30,37,45,60,75,112}, //750
    {20,30,40,50,60,80,100,150}, //1000
    {30,45,60,75,90,120,150,225}, //1500
    {40,60,80,100,120,160,200,300}, //2000
    {50,75,100,125,150,200,250,375}, //2500
    {60,90,120,150,180,240,300,450}, //3000
    {70,105,140,175,210,280,350,525}, //3500
    {80,120,160,200,240,320,400,600}, //4000
    {90,135,180,225,270,360,450,675}, //4500
    {100,150,200,250,300,400,500,750}, //5000
    {110,165,220,275,330,440,550,825}, //5500
    {120,180,240,300,360,480,600,900}, //6000
    {130,195,260,325,390,520,650,975}, //6500
    {140,210,280,350,420,560,700,1050}, //7000
    {150,225,300,375,450,600,750,1125}, //7500
    {160,240,320,400,480,640,800,1200}, //8000
    {170, 255, 340, 425, 510, 680, 850, 1275}, //8500
    {180, 270, 360, 450, 540, 720, 900, 1350}, //9000
    {190, 285, 380, 475, 570, 760, 950, 1425}, //9500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10000
};
const unsigned int ImgPw::WALL_FILTER_FREQ_CW[MAX_PRF_CW][MAX_WALL_FILTER] =  //hz
{
    {7,10,14,17,21,28,35,52}, //350
    {10,15,20,25,30,40,50,75}, //500
    {15,22,30,37,45,60,75,112}, //750
    {20,30,40,50,60,80,100,150}, //1000
    {30,45,60,75,90,120,150,225}, //1500
    {40,60,80,100,120,160,200,300}, //2000
    {50,75,100,125,150,200,250,375}, //2500
    {60,90,120,150,180,240,300,450}, //3000
    {70,105,140,175,210,280,350,525}, //3500
    {80,120,160,200,240,320,400,600}, //4000
    {90,135,180,225,270,360,450,675}, //4500
    {100,150,200,250,300,400,500,750}, //5000
    {110,165,220,275,330,440,550,825}, //5500
    {120,180,240,300,360,480,600,900}, //6000
    {130,195,260,325,390,520,650,975}, //6500
    {140,210,280,350,420,560,700,1050}, //7000
    {150,225,300,375,450,600,750,1125}, //7500
    {160,240,320,400,480,640,800,1200}, //8000
    {170, 255, 340, 425, 510, 680, 850, 1275}, //8500
    {180, 270, 360, 450, 540, 720, 900, 1350}, //9000
    {190, 285, 380, 475, 570, 760, 950, 1425}, //9500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10200
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //11000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //11500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //12000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //12500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //13000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //13500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //14000
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //14500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //15000
};
#else
const unsigned int ImgPw::WALL_FILTER_FREQ[MAX_PRF][MAX_WALL_FILTER] =  //hz
{
    {7,10,14,17,21,28,35,52}, //350
    {10,15,20,25,30,40,50,75}, //500
    {15,22,30,37,45,60,75,112}, //750
    {20,30,40,50,60,80,100,150}, //1000
    {30,45,60,75,90,120,150,225}, //1500
    {40,60,80,100,120,160,200,300}, //2000
    {50,75,100,125,150,200,250,375}, //2500
    {60,90,120,150,180,240,300,450}, //3000
    {70,105,140,175,210,280,350,525}, //3500
    {80,120,160,200,240,320,400,600}, //4000
    {90,135,180,225,270,360,450,675}, //4500
    {100,150,200,250,300,400,500,750}, //5000
    {110,165,220,275,330,440,550,825}, //5500
    {120,180,240,300,360,480,600,900}, //6000
    {130,195,260,325,390,520,650,975}, //6500
    {140,210,280,350,420,560,700,1050}, //7000
    {150,225,300,375,450,600,750,1125}, //7500
    {160,240,320,400,480,640,800,1200}, //8000
    {170, 255, 340, 425, 510, 680, 850, 1275}, //8500
    {180, 270, 360, 450, 540, 720, 900, 1350}, //9000
    {190, 285, 380, 475, 570, 760, 950, 1425}, //9500
    {200, 300, 400, 500, 600, 800, 1000, 1500}, //10000
};
#endif
const unsigned int ImgPw::PW_PRF[MAX_PRF] = //hz
{
    350, 500, 750, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500,
    6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000,10200, 10500, 11000,
};

const unsigned int ImgPw::CW_PRF[MAX_PRF] = //hz
{
   2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 8000, 9000,
    10000,12000,14000,16000, 18000, 20000, 25000, 30000, 35000, 40000, 45000, 50000,
};

const unsigned int ImgPw::WALL_FILTER[MAX_WALL_FILTER][7] =
{
#include "res/filter/Pw/wall_filter.h"
};

const unsigned int ImgPw::SIMULT2_PRF[MAX_SIMULT2_PRF_INDEX] = //hz
{
    1000, 1500, 2000, 2500, 3000, 3500, 5000, 6000, 7000, 9000,
};

const unsigned int ImgPw::SIMULT3_PRF[MAX_SIMULT3_PRF_INDEX] = //hz
{
    1000, 1500, 2000, 2500, 3000, 3500, 5000, 6000, 7000, 9000,
};

const int ImgPw::STEER_ANGLE[MAX_STEER] = {-15, -10, -5,  0, 5, 10, 15};
const float ImgPw::PW_FOCUS[3] = {20.0, 80.0, 150.0}; //mm

const int ImgPw::CORRECT_ANGLE_FAST[MAX_ANGLE_FAST] = {-60, 0, 60};

gboolean ApplySVChange(gpointer data)
{
    ImgPw::GetInstance()->DelaySendPwPara();

    //Img2D::GetInstance()->SetCwLine(pwLine);

    return FALSE;
}

ImgPw* ImgPw::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ImgPw;

    return m_ptrInstance;
}
ImgPw::ImgPw()
{
    GlobalClassMan* ptrGcm = GlobalClassMan::GetInstance();
    m_ptrUpdate = ptrGcm->GetUpdatePw();

    DscMan* ptrDscMan = DscMan::GetInstance();
    m_ptrDscPara = ptrDscMan->GetDscPara();

    m_ptrCalc = NULL;
    m_ptrImg2D = Img2D::GetInstance();

    m_ctGainPw.Begin();
    m_tGainPw = 0;
#ifdef EMP_355
    m_HPRF = TRUE;
#else
    m_HPRF = FALSE;
#endif
    m_pwLine = 128;
    m_svPos = 200;
    m_svPosBefore = m_svPos;
    m_svLen = 5;
    m_gainPw = 70;
    m_gainCw = 50;
    m_freqIndex = 0;
    m_angle = 0;
    m_PRFIndex = 0;
    m_PRFIndexBak = m_PRFIndex;
    m_PRFIndexTempBak = m_PRFIndex;
    m_baselineIndex = 0;
    m_baseline = IMG_H * 3 / 4;
    m_wallFilterIndex = 0;
    m_dynamicRangeIndex = 36;
    m_speedIndex = 1;
    m_soundVolumeIndex = 0;
    m_log = 294;
    m_invert = FALSE;
    m_soundStatus = FALSE;
    m_soundFilterIndex = 3;
    m_grayMap = 0;
    m_offSetX = 0;
    m_offSetY = 0;
#ifdef EMP_355
   m_oper = ADD;
#endif
    int i;
    for (i = 0; i < ProbeSocket::MAX_DOP_FREQ; i ++)
        m_freq[i] = 0;
    m_freqSum = 0;
    m_depthToSample = 10.0;
    m_maxPRFIndex = MAX_PRF-1;
    m_balance = FALSE;

    m_scaleTime = 0.0;
    m_scaleVel = 0.0;

    m_simult2 = FALSE;
    m_simult2PRFIndex = 4;
    m_maxSimult2PRFIndex = MAX_SIMULT2_PRF_INDEX - 1;

    m_simult3 = FALSE;
    m_simult3PRFIndex = 4;
    m_maxSimult3PRFIndex = MAX_SIMULT2_PRF_INDEX - 1;

    m_pwImgCtrl = TRUE;
    m_timer = 0;
    m_sendPwLine = TRUE;
    m_cwImgCtrl =  TRUE;

    for (i = 0; i < 8; i ++)
        m_tgc[i] = 128;

    m_scaleVelSnap = 0.0;
    m_scaleTimeSnap = 0.0;
    m_baselineSnap = 0;

    m_specMode = SPECTRUM_PW;

    m_steerIndexBak = 0;
    m_steerIndex = 0;
    m_pwFocusIndex = -1;

    ///>test
    m_bandPassFilterList = 1;
}

ImgPw::~ImgPw()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

void ImgPw::SetCalcPw(CalcPw* calc)
{
    m_ptrCalc = calc;

    m_ptrCalc->SetCalcPara(&m_calcPara);
}

void ImgPw::InitProbeOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    PRINTF("======================optimize PRINTF IMGPW send para to fpga======================\n");
    EKnobReturn ret;

    ASSERT((ptrPara!=NULL) && (ptrParaItem!=NULL));
    // get doppler freq
    int i;
    m_freqSum = 0;
    for (i = 0; i < ProbeSocket::MAX_DOP_FREQ; i ++)
    {
        if (ptrPara->freqDop[i] != 0)
        {
            m_freq[i] = ptrPara->freqDop[i];
            m_freqSum ++;
        }
    }

    // apply doppler freq
    //m_freqIndex = 0;
    m_freqIndex = ptrParaItem->spectrum.freq;
    if (m_freqIndex == 0)
        ret = MIN;
    else if (m_freqIndex == (m_freqSum - 1))
        ret = MAX;
    else
        ret = OK;
    DopplerFreq(m_freq[m_freqIndex], ret);

    //gain
    m_gainCw = ptrParaItem->spectrum.gain;
    GainCw(m_gainCw);
    m_gainPw = ptrParaItem->spectrum.gain;
    GainPw(m_gainPw);

    //dynamic range
    m_dynamicRangeIndex = ptrParaItem->spectrum.dynamicRange;
    if (m_dynamicRangeIndex == DYNAMIC_RANGE[0])
        ret = MIN;
    else if (m_dynamicRangeIndex == DYNAMIC_RANGE[1])
        ret = MAX;
    else
        ret = OK;
    DynamicRange(m_dynamicRangeIndex, ret);

    //sv
    int range[2];
    GetPwLineRange(range[0], range[1]);
    m_pwLine = (range[0] + range[1]) / 2;
    m_svPosBefore = 200;
    m_svPos = SVPosCorrect(m_svPosBefore);
    m_svLen = ptrParaItem->spectrum.SV;
    SampleVolume(m_pwLine, m_svPos, m_svLen);

    // PRF, realize in HPRF
    m_PRFIndex = ptrParaItem->spectrum.PRF;
    m_PRFIndexBak = m_PRFIndex;
    HPRF(m_HPRF, ret);

    //wall filter
    m_wallFilterIndex = ptrParaItem->spectrum.wallFilter;
    WallFilter(m_wallFilterIndex, OK);

    // invert
    m_invert = ptrParaItem->spectrum.invert;
    Invert(m_invert);

    // correct angle
    m_angle = ptrParaItem->spectrum.correctAngle;
    if (m_angle == -MAX_ANGLE)
        ret = MIN;
    else if (m_angle == MAX_ANGLE)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->SetValueCorrectAngle(m_angle);
    m_ptrUpdate->CorrectAngle(m_angle, ret);

    //fast anlge index
    if(m_angle < 0)
        m_corAngleFastIndex = 0;
    else if(m_angle > 0)
        m_corAngleFastIndex = 2;
    else
        m_corAngleFastIndex = 1;

    //baseline
    //m_baselineIndex = 0;
    m_baselineIndex = ptrParaItem->spectrum.baseline;
    if (m_baselineIndex == - MAX_BASELINE_INDEX)
        ret = MIN;
    else if (m_baselineIndex == MAX_BASELINE_INDEX)
        ret = MAX;
    else
        ret = OK;
    Baseline(m_baselineIndex, ret);

    //spectrum speed
    //m_speedIndex = 3;
    m_speedIndex = ptrParaItem->spectrum.speed;
    if (m_speedIndex == 0)
        ret = MIN;
    else if (m_speedIndex == (MAX_SPEED_INDEX - 1))
        ret = MAX;
    else
        ret = OK;
    SpectrumSpeed(m_speedIndex, m_PRFIndex, ret);

    //volume
    m_soundVolumeIndex = ptrParaItem->spectrum.soundVolume;
#ifdef EMP_430
    m_soundStatus = FALSE;
#else
    m_soundStatus = TRUE;
#endif
    soundstatus=m_soundStatus;

    if (!m_soundStatus)
        ret = MIN;
    else if (m_soundStatus)
        ret = MAX;
    SoundStatus(m_soundStatus, ret);

   // noise threshould
    m_noiseThreshould = ptrParaItem->spectrum.noiseThread;//20;
    if (m_noiseThreshould == 0)
        ret = MIN;
    else if (m_noiseThreshould == (MAX_NOISE - 1))
        ret = MAX;
    else
        ret = OK;
    NoiseThreshold(m_noiseThreshould, ret);

  // pw steer
    //m_steerIndex = 3;
    m_steerIndex = ptrParaItem->color.steer;
    m_steerIndexBak = m_steerIndex;
    if (m_steerIndex == 0)
        ret = MIN;
    else if (m_steerIndex == (MAX_STEER - 1))
        ret = MAX;
    else
        ret = OK;
    Steer(STEER_ANGLE[m_steerIndex], ret);
}

void ImgPw::InitProbe(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem)
{
    PRINTF("======================PRINTF IMGPW send para to fpga======================\n");
    EKnobReturn ret;

    ASSERT((ptrPara!=NULL) && (ptrParaItem!=NULL));
    InitProbeOptimize(ptrPara, ptrParaItem);

    // pw img display on
    OnPwImgCtrl(TRUE);
    m_sendPwLine = TRUE;

    // tgc

    if(ModeStatus::IsCWImgMode())
        GainCw(m_gainCw);
    else if(ModeStatus::IsPWImgMode())
        GainPw(m_gainPw);

    //log
    m_log = 294;
    Log(m_log, OK);

    //sv
    int range[2];
    GetPwLineRange(range[0], range[1]);
    m_pwLine = (range[0] + range[1]) / 2;
    m_svPosBefore = 200;
    m_svPos = SVPosCorrect(m_svPosBefore);
    m_svLen = ptrParaItem->spectrum.SV;
    SampleVolume(m_pwLine, m_svPos, m_svLen);

    // pw  map
    m_grayMap = 0;
    if (m_grayMap == 0)
        ret = MIN;
    else if (m_grayMap == (MAX_GRAY_SCALE - 1))
        ret = MAX;
    else
        ret = OK;
    GrayMap(m_grayMap, ret);

     //simult
    m_simult2 = FALSE;
    if (!m_simult2)
        ret = MIN;
    else if (m_simult2)
        ret = MAX;
    Simult2(m_simult2, ret);

    // high pulse repeat frequency
#ifdef EMP_355
    m_HPRF = TRUE;
#else
    m_HPRF = FALSE;
#endif
    if (!m_HPRF)
        ret = MIN;
    else if (m_HPRF)
        ret = MAX;
    HPRF(m_HPRF, ret);

    // pw focus pos
    FocPos(m_svPos);
}

void ImgPw::GetCurPara(ExamItem::ParaItem* ptrParaItem)
{
    ptrParaItem->spectrum.freq = m_freqIndex; //< index of color freq
    ptrParaItem->spectrum.gain = m_gainPw;
    ptrParaItem->spectrum.dynamicRange = m_dynamicRangeIndex;
    ptrParaItem->spectrum.PRF = m_PRFIndex;
    ptrParaItem->spectrum.wallFilter = m_wallFilterIndex;
    ptrParaItem->spectrum.invert = m_invert;
    ptrParaItem->spectrum.SV = m_svLen;
    ptrParaItem->spectrum.correctAngle = m_angle;
    //steer is equal in cfm and pw mode
    ptrParaItem->color.steer = m_steerIndex;
    ptrParaItem->spectrum.baseline = m_baselineIndex;
    ptrParaItem->spectrum.soundVolume = m_soundVolumeIndex;
    ptrParaItem->spectrum.noiseThread = m_noiseThreshould;
    ptrParaItem->spectrum.speed = m_speedIndex;
}

void ImgPw::EnterPw()
{
    EKnobReturn ret;
    //m_PRFIndex=8;
    // 控制FPGA开始传输PW图像
    OnPwImgCtrl(TRUE);
    if (m_speedIndex == 0)
        ret = MIN;
    else if (m_speedIndex == (MAX_SPEED_INDEX - 1))
        ret = MAX;
    else
        ret = OK;

    // resend pw speed and prf value to FPGA
//  SpectrumSpeed(m_speedIndex, m_PRFIndex, ret);
#ifdef EMP_355
if(ViewMain::GetInstance()->GetModeIsFlag())
    {
        ret = OK;
        HPRF(true, ret);
    }
    else
    {
        if(m_PRFIndex >m_maxPRFIndex)
            ret = PRF(m_maxPRFIndex);
        else
            ret = PRF(m_PRFIndex);
        ImgPw::GetInstance()->ChangeHPRF(FALSE);
    }
#else
    ret = PRF(m_PRFIndex);
#endif

    // gain M和PW模式共用统一TGC地址，因此进入时需重新发送TGC
    GainPw(m_gainPw);
}

void ImgPw::EnterCw()
{
    EKnobReturn ret;
    m_speedIndex=3;

    if (m_speedIndex == 0)
        ret = MIN;
    else if (m_speedIndex == (MAX_SPEED_INDEX - 1))
        ret = MAX;
    else
        ret = OK;

    // resend pw speed and prf value to FPGA
    SpectrumSpeed(m_speedIndex, m_PRFIndex, ret);
    // ret = PRF(m_PRFIndex);
    //

    //  doppler freq
    m_freqIndex = 0;
    if (m_freqIndex == 0)
        ret = MIN;
    else if (m_freqIndex == (m_freqSum - 1))
        ret = MAX;
    else
        ret = OK;
    DopplerFreq(m_freq[m_freqIndex], ret);

    // gain M和PW模式共用统一TGC地址，因此进入时需重新发送TGC
    GainCw(m_gainCw);

    m_ptrImg2D->ModeCwCtrl(TRUE);
    FocPos(m_svPos);

    OnCwImgCtrl(TRUE);
}
/*
 * @brief reset sv(scan line, dots region)
 */
void ImgPw::ResetSv()
{
    int range[2];
    GetPwLineRange(range[0], range[1]);
    m_pwLine = (range[1] + range[0]) / 2;
    m_svPosBefore = 200;
    m_svPos = SVPosCorrect(m_svPosBefore);
    m_svLen = 10;

    SampleVolume(m_pwLine, m_svPos, m_svLen);
}

void ImgPw::ResetSvAccordingColor()
{
    int cfmLine[2], cfmDot[2];
    ImgCfm::GetInstance()->GetBoxInfo(cfmLine[0], cfmLine[1], cfmDot[0], cfmDot[1]);
    m_pwLine = (cfmLine[0] + cfmLine[1]) / 2;
    m_svPosBefore = (cfmDot[0] + cfmDot[1]) / 2;
    m_svPos = SVPosCorrect(m_svPosBefore);
    SampleVolume(m_pwLine, m_svPos, m_svLen);
}

/*
 * @brief send sv para agian according to current status(example: local zoom)
 */
void ImgPw::ReSendSv()
{
    // must send focus delay
    m_pwFocusIndex = -1;

    // if out of range
    int range[2];
    GetPwLineRange(range[0], range[1]);
    if ((m_pwLine < range[0]) || (m_pwLine > range[1]))
    {
        m_pwLine = (range[1] + range[0]) / 2;
    }

    GetSvPosRange(range[0], range[1]);
    if ((m_svPosBefore < range[0]) || (m_svPosBefore > range[1]))
    {
        m_svPosBefore = (range[0] + range[1] ) / 2;
    }
    m_svPos = SVPosCorrect(m_svPosBefore);
    SampleVolume(m_pwLine, m_svPos, m_svLen);
}

void ImgPw::ChangeInvert()
{
    m_invert = !m_invert;

    Invert(m_invert);
}
void ImgPw::ChangeTgcPw(int tgcY[8])
{
    memcpy(m_tgc, tgcY, 8*sizeof(int));

    m_ptrImg2D->CalcTgc(m_gainPw*MAX_GAIN_PW/100, m_tgc, 2);
    m_ptrImg2D->CalcTgcDigital(m_gainPw*MAX_GAIN_PW/100, 2, MAX_GAIN_PW);
}

EKnobReturn ImgPw::ChangeGainPw(EKnobOper oper)
{
    int gain = m_gainPw;
    int step = 1;

    float tu = m_ctGainPw.End();
    m_ctGainPw.Begin();

    if(tu < INTERVAL)
    {
        if(m_tGainPw >= 3)
            step = 5;
        else
            m_tGainPw ++;
    }
    else
    {
        m_tGainPw = 0;
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

    m_gainPw = gain;

    EKnobReturn ret = OK;
    if (gain == 0)
        ret = MIN;
    else if (gain == 100)
        ret = MAX;
    else
        ret = OK;

    GainPw(m_gainPw);

    return (ret);
}

EKnobReturn ImgPw::ChangeGainCw(EKnobOper oper)
{
    int gain = m_gainCw;
    int step = 1;

    float tu = m_ctGainPw.End();
    m_ctGainPw.Begin();

    if(tu < INTERVAL)
    {
        if(m_tGainPw >= 3)
            step = 5;
        else
            m_tGainPw ++;
    }
    else
    {
        m_tGainPw = 0;
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

    m_gainCw = gain;

    EKnobReturn ret = OK;
    if (gain == 0)
        ret = MIN;
    else if (gain == 100)
        ret = MAX;
    else
        ret = OK;

    GainCw(m_gainCw);

    return (ret);
}

EKnobReturn ImgPw::ChangeDopplerFreq(EKnobOper oper)
{
    int index = m_freqIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (index < (m_freqSum - 1))
        {
            index += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= step;
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

    m_freqIndex = index;
    int freq = m_freq[m_freqIndex];
    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (m_freqSum - 1))
        ret = MAX;
    else
        ret = OK;

    DopplerFreq(freq, ret);

    return (ret);
}

void ImgPw::UpdateDopplerFreq()
{
    EKnobReturn ret;
    if (m_freqIndex == 0)
        ret = MIN;
    else if (m_freqIndex == (m_freqSum - 1))
        ret = MAX;
    else
        ret = OK;

    m_ptrUpdate->DopFreq(m_freq[m_freqIndex], ret);
}
EKnobReturn ImgPw::ChangeCorrectAngle(EKnobOper oper)
{
    int data = m_angle;
    int step = 1;

    float tu = m_ctGainPw.End();
    m_ctGainPw.Begin();
    if(tu < INTERVAL)
        step = 5;
    else
        step = 1;

    if (oper == ADD)
    {
        if ((data+step) < MAX_ANGLE)
        {
            data += step;
        }
        else
        {
            data = -MAX_ANGLE;
        }
    }
    else if (oper == SUB)
    {
        if ((data-step) > -MAX_ANGLE)
        {
            data -= step;
        }
        else
        {
            data = MAX_ANGLE;
        }
    }
    else
    {
        return ERROR;
    }

    m_angle = data;

    //fast anlge index
    if(data < 0)
        m_corAngleFastIndex = -1;
    else if(data > 0)
        m_corAngleFastIndex = 1;
    else
        m_corAngleFastIndex = 0;

    PRINTF("data: %d   index:%d\n", data, m_corAngleFastIndex);

    EKnobReturn ret = OK;
    if (data == -MAX_ANGLE)
        ret = MIN;
    else if (data == MAX_ANGLE)
        ret = MAX;
    else
        ret = OK;

    CorrectAngle(m_angle, ret);

    return (ret);
}
EKnobReturn ImgPw::ChangeCorrectAngleFast(EKnobOper oper)
{
    int data = m_angle;
    int index = m_corAngleFastIndex;
    int step = 30;

    if (oper == ADD)
    {
        if ((data + step) <= MAX_ANGLE)
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
        if ((data - step) >= -MAX_ANGLE)
        {
            data -= step;
        }
        else
        {
            return (MIN);
        }
    }
    else if (oper == ROTATE)
    {
       if(index < MAX_ANGLE_FAST - 1)
           index += 1;
       else
           index = 0;
       m_corAngleFastIndex = index;
       data = CORRECT_ANGLE_FAST[index];
    }
    else
    {
        return ERROR;
    }

    m_angle = data;

    EKnobReturn ret = OK;
    if (data == -MAX_ANGLE)
        ret = MIN;
    else if (data == MAX_ANGLE)
        ret = MAX;
    else
        ret = OK;

    CorrectAngle(m_angle, ret);

    return (ret);
}

void ImgPw::SetScale(int prfIndex)
{
    m_PRFIndex = prfIndex;
}

EKnobReturn ImgPw::ChangeScale(EKnobOper oper)
{
    EKnobReturn ret;
#ifdef EMP_355
    bool mode = ViewMain::GetInstance()->GetModeIsFlag();
    if(!mode)
    {
        if (m_HPRF)
        {
            HintArea::GetInstance()->UpdateHint(_("[Scale]: Can not be changed when HPRF is on."), 2);
            return ERROR;
        }

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
        ret = PRF(m_PRFIndex);
    }
    else
    {
        m_oper = oper;
        int data = m_PRFIndex;
        int step = 1;

        int prf = GetHPRFValue(m_PRFIndex);
        if (oper == ADD)
        {
           // if (data < m_maxPRFIndex)
            if(prf < GetPRFValue(m_maxPRFIndex))
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
            if (prf > 350)
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
        m_HPRF = TRUE;
        ret = MAX;
        HPRF(m_HPRF, ret);
    }
#else
    if (m_HPRF)
    {
        HintArea::GetInstance()->UpdateHint(_("[Scale]: Can not be changed when HPRF is on."), 2);
        return ERROR;
    }

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
    ret = PRF(m_PRFIndex);
#endif
    return (ret);
}

EKnobReturn ImgPw::ChangeScaleSimult2(EKnobOper oper)
{
    if (!m_simult2)
    {
        PRINTF("simult prf can not be changed when simult is off\n");
        return ERROR;
    }
    int data = m_simult2PRFIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < m_maxSimult2PRFIndex)
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

    m_simult2PRFIndex = data;
    EKnobReturn ret;
    ret = PRFSimult2(m_simult2PRFIndex);

    return (ret);

}

EKnobReturn ImgPw::ChangeScaleSimult3(EKnobOper oper)
{
    if (!m_simult3)
    {
        PRINTF("simult prf can not be changed when simult is off\n");
        return ERROR;
    }

    PRINTF("---change simult 3: index = %d, maxIndex = %d\n", m_simult3PRFIndex, m_maxSimult3PRFIndex);
    int data = m_simult3PRFIndex;
    int step = 1;

    if (oper == ADD)
    {
        if ((data+step) <= m_maxSimult3PRFIndex)
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
    m_simult3PRFIndex = data;

    EKnobReturn ret = PRFSimult3(m_simult3PRFIndex);
    return (ret);
}
EKnobReturn ImgPw::ChangeBaseline(EKnobOper oper)
{
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
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data > -MAX_BASELINE_INDEX)
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
    if (data == -MAX_BASELINE_INDEX)
        ret = MIN;
    else if (data == MAX_BASELINE_INDEX)
        ret = MAX;
    else
        ret = OK;

    Baseline(m_baselineIndex, ret);

    return (ret);
}
EKnobReturn ImgPw::ChangeWallFilter(EKnobOper oper)
{
    int data = m_wallFilterIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_WALL_FILTER-1))
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
    m_wallFilterIndex = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_WALL_FILTER-1))
        ret = MAX;
    else
        ret = OK;

    WallFilter(m_wallFilterIndex, ret);

    return (ret);
}
EKnobReturn ImgPw::ChangeDynamicRange(EKnobOper oper)
{
    int data = m_dynamicRangeIndex;
    int step = 5;

    if (oper == ADD)
    {
        if ((data + step) <= DYNAMIC_RANGE[1])
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
        if ((data - step) >= DYNAMIC_RANGE[0])
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

    m_dynamicRangeIndex = data;

    EKnobReturn ret = OK;
    if (data == DYNAMIC_RANGE[0])
        ret = MIN;
    else if (data == DYNAMIC_RANGE[1])
        ret = MAX;
    else
        ret = OK;

    DynamicRange(m_dynamicRangeIndex, ret);

    return (ret);
}
EKnobReturn ImgPw::ChangeSpeed(EKnobOper oper)
{
    int data = m_speedIndex;
    int step = 1;

    if (oper == ADD)
    {
        if (data < MAX_SPEED_INDEX - 1)
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
    m_speedIndex = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == MAX_SPEED_INDEX - 1)
        ret = MAX;
    else
        ret = OK;

    SpectrumSpeed(m_speedIndex, m_PRFIndex, ret);

    return (ret);
}
EKnobReturn ImgPw::ChangeSoundVolume(EKnobOper oper)
{
    int data = m_soundVolumeIndex;
    int step = 1;
    if (oper == ADD)
    {
        if (data < MAX_SOUND_VOLUME)
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
    m_soundVolumeIndex = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == MAX_SOUND_VOLUME)
        ret = MAX;
    else
        ret = OK;

    SoundVolume(m_soundVolumeIndex, ret);

    return (ret);
}

/*
 * @brief HPRF can work only when simult is off
 */
EKnobReturn ImgPw::ChangeHPRF(bool on)
{
    if ((m_simult3 || m_simult2) && on)
    {
        HintArea::GetInstance()->UpdateHint(_("[HPRF]: Can not be on when simult is on."), 2);
        return ERROR;
    }

    if ((m_specMode == SPECTRUM_CW) && on)
    {

        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }

    m_HPRF = on;

    EKnobReturn ret = OK;
    if (m_HPRF == FALSE)
        ret = MIN;
    else if (m_HPRF == TRUE)
        ret = MAX;
    else
        ret = OK;

    HPRF(m_HPRF, ret);

    return (ret);
}

EKnobReturn ImgPw::ChangeSimult2(bool on)
{
    bool data = on;
    EKnobReturn ret = OK;
    if (data == FALSE)
        ret = MIN;
    else if (data == TRUE)
        ret = MAX;
    else
        ret = OK;

    m_simult2 = data;
    Simult2(m_simult2, ret);
    return (ret);
}

EKnobReturn ImgPw::ChangeSimult3(bool on)
{
    bool data = on;

    EKnobReturn ret = OK;
    if (data == FALSE)
        ret = MIN;
    else if (data == TRUE)
        ret = MAX;
    else
        ret = OK;

    m_simult3 = data;
    Simult3(m_simult3, ret);

    return (ret);
}

EKnobReturn ImgPw::ChangeLineDensity(EKnobOper oper)
{
    EKnobReturn ret = OK;

    if (m_ptrImg2D != NULL)
    {
        ret = m_ptrImg2D->ChangeLineDensity(oper);
        if (m_simult3)
        {
            PRFSimult3(m_simult3PRFIndex);
        }
    }
    return ret;
}

void ImgPw::ChangeMBP(void)
{
    m_ptrImg2D->ChangeMBP();

    if (m_simult3)
    {
        PRFSimult3(m_simult3PRFIndex);
    }
}

void ImgPw::ChangeTSI(void)
{
    FocPos(m_svPos);
}

//#endif
/*
 * @brief HPRF can work only when simult is off
 */
/*EKnobReturn ImgPw::ChangeHPRF(bool on)
{
    if ((m_simult3 || m_simult2) && on)
    {
        HintArea::GetInstance()->UpdateHint(_("[HPRF]: Can not be on when simult is on."), 2);
        return ERROR;
    }

    if ((m_specMode == SPECTRUM_CW) && on)
    {
        HintArea::GetInstance()->UpdateHint(_("[HPRF]: Can not be on in cw mode."), 2);
        return ERROR;
    }

    m_HPRF = on;

    EKnobReturn ret = OK;
    if (m_HPRF == FALSE)
        ret = MIN;
    else if (m_HPRF == TRUE)
        ret = MAX;
    else
        ret = OK;

    HPRF(m_HPRF, ret);

    return (ret);
}

EKnobReturn ImgPw::ChangeSimult2(bool on)
{
    bool data = on;
    EKnobReturn ret = OK;
    if (data == FALSE)
        ret = MIN;
    else if (data == TRUE)
        ret = MAX;
    else
        ret = OK;

    m_simult2 = data;
    Simult2(m_simult2, ret);
    return (ret);
}

EKnobReturn ImgPw::ChangeSimult3(bool on)
{
    bool data = on;

    EKnobReturn ret = OK;
    if (data == FALSE)
        ret = MIN;
    else if (data == TRUE)
        ret = MAX;
    else
        ret = OK;

    m_simult3 = data;
    Simult3(m_simult3, ret);

    return (ret);
}

EKnobReturn ImgPw::ChangeLineDensity(EKnobOper oper)
{
    EKnobReturn ret = OK;

    if (m_ptrImg2D != NULL)
    {
        ret = m_ptrImg2D->ChangeLineDensity(oper);
        if (m_simult3)
        {
            PRFSimult3(m_simult3PRFIndex);
        }
    }
    return ret;
}
void ImgPw::ChangeMBP(void)
{
    m_ptrImg2D->ChangeMBP();

    if (m_simult3)
    {
        PRFSimult3(m_simult3PRFIndex);
    }
}

void ImgPw::ChangeTSI(void)
{
    FocPos(m_svPos);
}
*/

// sound volume is valid, only when sound status is on
//int soundstatus=0;
EKnobReturn ImgPw::ChangeSoundStatus(EKnobOper oper)
{
    bool data = m_soundStatus;
    if (oper == ADD)
    {
        if (data == FALSE)
        {
            data = TRUE;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (data == TRUE)
        {
            data =  FALSE;
        }
        else
        {
            return (MIN);
        }
    }
    else if (oper == ROTATE)
    {
        if (data)
            data = FALSE;
        else
            data = TRUE;
    }
    else
    {
        return ERROR;
    }

    m_soundStatus = data;

    EKnobReturn ret = OK;
    if (!data)
        ret = MIN;
    else if (data)
        ret = MAX;
    else
        ret = OK;

    SoundStatus(m_soundStatus, ret);
    soundstatus=m_soundStatus;
    return (ret);
}
EKnobReturn ImgPw::ChangeGrayMap(EKnobOper oper)
{
    int data = m_grayMap;
    int step = 1;

    if (oper == ADD)
    {
        if (data < (MAX_GRAY_SCALE-1))
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
    m_grayMap = data;

    EKnobReturn ret = OK;
    if (data == 0)
        ret = MIN;
    else if (data == (MAX_GRAY_SCALE - 1))
        ret = MAX;
    else
        ret = OK;

    GrayMap(m_grayMap, ret);

    return (ret);
}

EKnobReturn ImgPw::ChangeNoiseThreshold(EKnobOper oper)
{
    int index = m_noiseThreshould;
    int step = 2;

    if (oper == ADD)
    {
        if ((index+step) < (MAX_NOISE-1))
        {
            index += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if ((index-step) > 0)
        {
            index -= step;
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

    m_noiseThreshould = index;

    EKnobReturn ret = OK;
    if (index == MIN_NOISE)
        ret = MIN;
    else if (index == (MAX_NOISE-3))
        ret = MAX;
    else
        ret = OK;

    NoiseThreshold(m_noiseThreshould, ret);

    return (ret);
}

EKnobReturn ImgPw::ChangeLog(EKnobOper oper)
{
    int index = m_log;
    int step = 10;

    if (oper == ADD)
    {
        if ((index+step) < (MAX_LOG-1))
        {
            index += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if ((index-step) > 1)
        {
            index -= step;
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

    m_log = index;

    EKnobReturn ret = OK;
    if (index == 1)
        ret = MIN;
    else if (index == (MAX_LOG-1))
        ret = MAX;
    else
        ret = OK;

    Log(m_log, ret);

    return (ret);
}

void ImgPw::ChangeDepth()
{
    SendSampleVolume(m_pwLine, m_svPos, m_svLen);
    if(ModeStatus::IsSimultMode())
    {
        if(ModeStatus::IsCwImgMode())
            GainCw(m_gainCw);
        else
            GainPw(m_gainPw);
    }
}

EKnobReturn ImgPw::ChangeSteer(EKnobOper oper)
{
    m_steerIndexBak = m_steerIndex;
    int index = m_steerIndex;
    int angle;

    ///> change speed of sound
    if (oper == ADD)
    {
        if (index < (MAX_STEER - 1))
        {
            index ++;
        }
        else
        {
            PRINTF("ImgPw: steer reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index --;
        }
        else
        {
            PRINTF("ImgPw: steer reach min value!\n");
            return (MIN);
        }
    }
    else if (oper == ROTATE)
    {
        if (index < (MAX_STEER - 1))
            index ++;
        else
            index = 0;
    }
    else
    {
        return ERROR;
    }

    m_steerIndex = index;
    angle = STEER_ANGLE[m_steerIndex];

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_STEER - 1))
        ret = MAX;
    else
        ret = OK;

    Steer(angle, ret);
    return (ret);
}

void ImgPw::ChangePwTis()
{
    int freq = GetNameColorFreq();
    ///> Tis
    m_ptrImg2D->SetPwTis(freq, m_PRFIndex);

}

/*
 * @brief calc and send sample dots for PW rgb post process
 *
 * @para dots[in] real sample dots to dsc and fpga
 */
void ImgPw::SetRGBSampleDots(int dots)
{
    const int MAX = 256;
    unsigned short *table = (unsigned short*)malloc (dots*BITS16);
    int i;
    float step = (float)MAX / dots;

    PRINTF("===========================================\n");

    for (i = 0; i < dots; i ++)
    {
        table[i] = (0 + i * step) * (float)128;
        //PRINTF("==");
        //PRINTF("%d",table[i]);
    }

    PRINTF("dots after decimation is dots = %d\n",dots)
    PRINTF("===========================================\n");

    m_ptrCalc->CalcRGBSampleDots(dots);
    m_ptrCalc->CalcRGBSampleCoef(table, dots);

    free(table);
}

void ImgPw::ClearSv()
{
    //m_ptrUpdate->SampleVolumeClear();
   UpdateSVClear();
}

void ImgPw::ReDrawSv()
{
    // redraw sv
    int dotBegin;
    int dotEnd;

    SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, FALSE);
    //m_ptrUpdate->SampleVolume(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
   UpdateSV(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
}

bool ImgPw::ChangePwSV(int offsetX, int offsetY)
{
    m_offSetX =  offsetX;
    m_offSetY = offsetY;
    int line = m_pwLine;
    int pos = m_svPosBefore;
    ModeStatus ms;
    int mode = ms.GetScanMode();
    int stepLine;
    int stepDot;
    stepLine = offsetX;
    stepDot = -offsetY;

    int range[2];

    GetPwLineRange(range[0], range[1]);

    line += stepLine;
    if (line > range[1])
        line = range[1];
    else if (line < range[0])
        line = range[0];
    m_pwLine = line;

    GetSvPosRange(range[0], range[1]);
    pos += stepDot;
    if (pos > range[1])
        pos = range[1];
    else if (pos < range[0])
        pos = range[0];
    m_svPosBefore = pos;

    //m_sendPwLine = FALSE;
    m_svPos = SVPosCorrect(m_svPosBefore);
    SVPos(m_pwLine, m_svPos, m_svLen);

    return (TRUE);
}
EKnobReturn ImgPw::ChangeSVLength(EKnobOper oper)
{
    EKnobReturn ret = OK;
#ifdef EMP_355
  if(!ViewMain::GetInstance()->GetModeIsFlag())
    {
        int index = m_svLen;
        int step = 5;

        if (oper == ADD)
        {
            if ((index+step) <= MAX_SV)
            {
                index += step;
            }
            else
            {
                return (MAX);
            }
        }
        else if (oper == SUB)
        {
            if ((index-step) >= MIN_SV)
            {
                index -= step;
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

        m_svLen = index;
        if (index == MIN_SV)
            ret = MIN;
        else if (index == (MAX_SV))
            ret = MAX;
        else
            ret = OK;

        SVLen(m_pwLine, m_svPos, m_svLen, ret);
        return (ret);
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("Operation is invalid."), 1);
        return ERROR;
    }
#else
    int index = m_svLen;
    int step = 5;

    if (oper == ADD)
    {
        if ((index+step) <= MAX_SV)
        {
            index += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if ((index-step) >= MIN_SV)
        {
            index -= step;
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

    m_svLen = index;

    if (index == MIN_SV)
        ret = MIN;
    else if (index == (MAX_SV))
        ret = MAX;
    else
        ret = OK;

    SVLen(m_pwLine, m_svPos, m_svLen, ret);
#endif
    return (ret);
}

void ImgPw::GetPwInfo(int &line, int &dotBegin, int &dotEnd, int &dotSvLen)
{
    line = m_pwLine;
    CalcPwInfoDot(m_svPosBefore, m_svLen, dotBegin, dotEnd, dotSvLen);
}
void ImgPw::SetPwInfo(int line, int dotBegin, int dotEnd)
{
    // pw line
    m_pwLine = line;

    // pw sv pos
    double scale = m_ptrImg2D->GetScale2DInImgHDot();
    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
        double scaleZoom;
        Zoom::GetInstance()->GetLocalZoomScale(scaleZoom);
        scale = scale / scaleZoom;
    }
    int svLenDot = (float)m_svLen / 10 / scale;
    m_svPosBefore = dotBegin + svLenDot / 2;
    m_svPos = SVPosCorrect(m_svPosBefore);

    SVPos(m_pwLine, m_svPos, m_svLen);
}

double ImgPw::GetScaleTime()
{
    // special measure mode
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
    {
        return m_scaleTimeSnap;
    }

    // normal mode
    int speed = GetSpeed(m_speedIndex, GetPRFValue(m_PRFIndex));
    m_scaleTime = m_ptrCalc->CalcScaleTime(speed, GetPRFValue(m_PRFIndex));
    return m_scaleTime;
}

/*
 * @brief get scale of velocity
 *
 * @ret val scale of velocity, unit:(cm/s)/p
 */
double ImgPw::GetScaleVel()
{
    // special measure mode
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
    {
        return m_scaleVelSnap;
    }

    // normal mode
    double speed = Img2D::GetInstance()->GetSoundSpeed() * 1000;
    //double speed = SOUND_SPEED * 1000; //s
    // get dots value from dsc
    int dots = GetPwDots() / 2;

    int freq = GetColorFreq();

    m_scaleVel = m_ptrCalc->CalcScaleVel(GetPRFValue(m_PRFIndex), freq, m_angle, speed, dots);

    return m_scaleVel;
}

int ImgPw::GetBaseLineForCalc()
{
    // special measure mode
    if (ScanMode::GetInstance()->IsSpecialMeasureStatus())
    {
        return m_baselineSnap;
    }

    // normal mode
    ModeStatus s;
    FormatPw::EFormatPw formatPw = s.GetFormatPw();
    int offset;
    float pwImgWidth;

    switch(formatPw)
    {
        case FormatPw::P_TOTAL:
            offset = IMG_H / 2;
            pwImgWidth = IMG_H;
            break;
        case FormatPw::BP11_UD:
            offset = IMG_H * 3 / 4;
            pwImgWidth = IMG_H / 2;
            break;
        case FormatPw::BP21_UD:
            offset = IMG_H * 5 / 6;
            pwImgWidth = IMG_H * 1 / 3;
            break;
        case FormatPw::BP12_UD:
            offset = IMG_H * 2 / 3;
            pwImgWidth = IMG_H * 2 / 3;
            break;
        case FormatPw::BP11_LR:
            offset = IMG_H / 2;
            pwImgWidth = IMG_H;
            break;
        default:
            offset = IMG_H * 3 / 4;
            pwImgWidth = IMG_H / 2;
            break;
    }

    int baseline = pwImgWidth / 2 * m_baselineIndex / MAX_BASELINE_INDEX;
    return (offset - baseline);
}

int ImgPw::GetRealSpectrumSpeed()
{
    return GetSpeed(m_speedIndex, GetPRFValue(m_PRFIndex));
}

int ImgPw::GetRealPRF()
{
    return GetPRFValue(m_PRFIndex);
}

void ImgPw::CalcPwInfoDot(int posDot, int svLen, int &dotBegin, int &dotEnd, int &dotSvLen)
{
    double scale = m_ptrImg2D->GetScale2DInImgHDot();
    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
        double scaleZoom;
        Zoom::GetInstance()->GetLocalZoomScale(scaleZoom);
        scale = scale / scaleZoom;
    }

    dotSvLen = (float)svLen / 10 / scale;
    dotBegin = posDot - dotSvLen/2;
    dotEnd = posDot + dotSvLen/2;
}

int ImgPw::GetSteerAngle(int &currentAngle)
{
    currentAngle = STEER_ANGLE[m_steerIndex];
    return STEER_ANGLE[m_steerIndexBak];
}

double ImgPw::GetMaxVel()
{
    double speed = SOUND_SPEED * 1000;
    int freq = GetColorFreq();

    double maxVel = m_ptrCalc->CalcMaxVel(GetPRFValue(m_PRFIndex), freq, m_angle, speed);
    //PRINTF("MAX vel = %f\n", maxVel);

    return maxVel;
}

double ImgPw::GetScaleVelKHz() ///< (KHz)/pixel
{
    // get dots value from dsc
    int dots = GetPwDots() / 2;
    double scaleVel = GetPRFValue(m_PRFIndex) / (float)dots / 1000;

    PRINTF("scaleVelKHz = %f\n", scaleVel);

    return scaleVel;

}
double ImgPw::GetMaxVelKHz() ///< KHz
{
    double vel = GetPRFValue(m_PRFIndex) / (float)1000;

    return vel;
}
int ImgPw::GetPwDots()
{
    return FormatPw::GetInstance()->GetSpectrumDots();
}
int ImgPw::GetColorFreq()
{
    int pulseNum;

    if (m_ptrCalc != NULL)
        pulseNum = m_ptrCalc->GetPulseCycleNum() + 1;
    else
        pulseNum = 1;
    int freq = ((float)60 / pulseNum / 2) * 20; //unit: MHZ*20

    //PRINTF("--------------corrected freq = %d\n", freq);
    return freq;
}

// only for temp use
int ImgPw::GetNameColorFreq()
{
    return m_freq[m_freqIndex];
}

double ImgPw::GetSvSamplePos()
{
    return m_depthToSample;
}

bool ImgPw::GetHPRFStatus()
{
    return m_HPRF;
}

void ImgPw::DopplerFreq(int freq, EKnobReturn ret)
{
    int power = Img2D::GetInstance()->GetOutputPower();
     m_pulseCycle = CalcFocPulse(freq, power);
#ifdef EMP_355
    float fc_offset = 2.4;
    m_ptrCalc->CalcBandPassFilter_new(m_pulseCycle, fc_offset);
#else
     m_ptrCalc->CalcBandPassFilter_test(m_pulseCycle, m_bandPassFilterList);
#endif

    m_ptrUpdate->DopFreq(freq, ret);
}

void ImgPw::GainPw(int data)
{
    SetTraceGain(data);

    //update view
    m_ptrUpdate->GainPw(data);
#ifdef EMP_430
    data = data/2 + 50;
#endif
    m_ptrImg2D->CalcTgc(data*MAX_GAIN_PW/100, m_tgc, 2);
    m_ptrImg2D->CalcTgcDigital(data*MAX_GAIN_PW/100, 2, MAX_GAIN_PW);

}
void ImgPw::GainCw(int data)
{
    SetTraceGain(data);
    int dataCw;
    if(data >=50)
    {
        if(data>90)
        {
            dataCw=752;
        }
        else
        {
            dataCw=128+16*(data-50);
        }
    }
    else
    {
        dataCw=128-2*(50-data);
    }

    m_ptrCalc->CalcGainCw(dataCw);
    m_ptrUpdate->GainCw(data);
}

void ImgPw::DynamicRange(int data, EKnobReturn ret)
{
    SetTraceDynRange(data);
    m_ptrCalc->CalcDynamicRange(data);
    m_ptrUpdate->DynamicRange(data, ret);
}

void ImgPw::Baseline(int index, EKnobReturn ret)
{
    int maxPwDots = MAX_BASELINE;

    // send to fpga
    m_baseline = ((float)index / MAX_BASELINE_INDEX) * maxPwDots;
    if(m_baseline >= maxPwDots)
        m_baseline = maxPwDots-1;

    SetTraceBaseline(m_baseline);
    m_ptrCalc->CalcBaseline(m_baseline);
    m_ptrUpdate->Baseline(index, MAX_BASELINE_INDEX, ret);
}

void ImgPw::WallFilter(int index, EKnobReturn ret)
{
    m_ptrCalc->CalcWallFilter((unsigned int*)WALL_FILTER[index], 7);
    m_ptrUpdate->WallFilter(index, ret);

    PRINTF("=====================================send wall filter coefficients");

    // pw trace
    CalcTraceFs(index, m_PRFIndex, m_baseline, m_angle);
}

/*
 * @breif calc and draw sv
 * @para pwLine[in] pw sample line
 * @para svPos[in] sv pos in dots
 * @para svLen[in] length of sv in mm*10
 */
void ImgPw::SampleVolume(int pwLine, int svPos, int svLen)
{
    SendSampleVolume(pwLine, svPos, svLen);
    DrawSampleVolume(pwLine, svPos, svLen);
}

/*
 * @brief send sample volume to fpga, only send, not draw sv
 * @para pwLine[in] pw sample line
 * @para svPos[in] sv pos in dots
 * @para svLen[in] length of sv in mm*10
 */
void ImgPw::SendSampleVolume(int pwLine, int svPos, int svLen)
{
    // pw line
    m_ptrCalc->CalcPwLine(pwLine);
    m_ptrImg2D->SetCwLine(pwLine);

    // sv pos
    SVPos(pwLine, svPos, svLen);
}

void ImgPw::DrawSampleVolume(int pwLine, int svPos, int svLen)
{
    int dotBegin;
    int dotEnd;
    EKnobReturn ret;

    SVCalc(pwLine, svPos, svLen, dotBegin, dotEnd, FALSE);

    //update view
    if (svLen == MIN_SV)
        ret = MIN;
    else if (svLen == MAX_SV)
        ret = MAX;
    else
        ret = OK;

    UpdateSV(pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
    m_ptrUpdate->SVLength(svLen, ret);

    double pos = svPos * m_ptrImg2D->GetScale2DInImgHDot();
    m_ptrUpdate->SVPos(pos);
}
/*
 * @brief calc when svn pos is changed
 * @para pwLine[in] pw sample line
 * @para svPos[in] sv pos in dots
 * @para svLen[in] length of sv in mm*10
 */
void ImgPw::SVPos(int pwLine, int svPos, int svLen)
{
    m_sendPwLine = FALSE;
    int dotBegin;
    int dotEnd;
    //send to fpga
    SVCalc(pwLine, svPos, svLen, dotBegin, dotEnd, m_sendPwLine);
    PRINTF("svn pos: dotBegin = %d, dotEnd = %d\n", dotBegin, dotEnd);

    if(ModeStatus::IsCWImgMode())
    {
        //test move
        IoCtrl iofreeze;
        iofreeze.Freeze();
        OnCwImgCtrl(FALSE);
    }

    m_ptrImg2D->SetCwLine(pwLine);

    // change hprf when sv is changed
    double pos = svPos * m_ptrImg2D->GetScale2DInImgHDot();
    if (m_HPRF)
    {
        SwitchHPRF(m_HPRF, pos);
    }
    else
    {
        CalcPRF(GetPRFValue(m_PRFIndex), m_depthToSample);
    }

    // change pw focus

    FocPos(svPos);

    if(ModeStatus::IsCWImgMode())
    {
        //test
        IoCtrl iofreeze;
        iofreeze.Unfreeze();
        //triggle
        OnCwImgCtrl(TRUE);
   }

    //update view
    PRINTF("svPos: svPos = %d, dotBegin = %d, dotEnd = %d\n", svPos, dotBegin, dotEnd);
    UpdateSV(pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
    m_ptrUpdate->SVPos(pos);

    // balance cfm box
    if (m_balance)
        BalanceCfmInfo(pwLine, m_svPosBefore, svLen);

    if (m_timer > 0)
    {
        g_source_remove(m_timer);
        m_timer = 0;
    }
    m_timer = g_timeout_add(30, ApplySVChange, NULL);
}
void ImgPw::SVLen(int pwLine, int svPos, int svLen, EKnobReturn ret)
{
    int dotBegin;
    int dotEnd;

    SVCalc(pwLine, svPos, svLen, dotBegin, dotEnd, TRUE);

    //send to fpga
    if(m_HPRF)
    {
        SwitchHPRF(m_HPRF, svPos*m_ptrImg2D->GetScale2DInImgHDot());
    }
    else
    {
        CalcPRF(GetPRFValue(m_PRFIndex), m_depthToSample);
    }

    //update view
    m_ptrUpdate->SVLength(svLen, ret);
    UpdateSV(pwLine, dotBegin, dotEnd, GetHPRFEmitPos());

    // balance cfm box
    if (m_balance)
        BalanceCfmInfo(pwLine, m_svPosBefore, svLen);
}

/*
 * @brief calc sv range (begin, end) when svPos or svLen is changed
 *       pw的采样率随着SV的位置变化而变化，采样深度与2D不同，是在SV所在位置上适当加深得来的，
 *       因此，SV BEGIN 和SV END 是相对于PW采样比例，而不是2D图像的比例的。
 * @para pwLine[in] sample line of pw
 * @para svPos[in] sv pos in dot, pixel
 * @para svLen[in] sv length in depth, mm
 * @para dotBegin[out] sv begin in dot, value send to fpga
 * @para dotEnd[out] sv end in dot, value send to fpga
 */
void ImgPw::SVCalc(int pwLine, int svPos, int svLen, int &dotBegin, int &dotEnd, bool SendFpga)
{
    // calc sv len dot in scale 2D, used for decide range of sv
    double scale = m_ptrImg2D->GetScale2DInImgHDot();
    PRINTF("scale:%lf\n", scale);
    int svLenDot = (float)svLen / 10 / scale;
    dotBegin = svPos - svLenDot/2;
    dotEnd = svPos + svLenDot/2;
#ifdef EMP_355
 if (dotEnd >= m_ptrImg2D->GetDepthDots()) // out of range
    {
        dotEnd = m_ptrImg2D->GetDepthDots();
        dotBegin = m_ptrImg2D->GetDepthDots() - svLenDot;
        m_svPosBefore = m_ptrImg2D->GetDepthDots() - svLenDot/2;
        m_svPos = SVPosCorrect(m_svPosBefore);
    }
#else
    if (dotEnd >= IMG_H) // out of range
    {
        dotEnd = IMG_H;
        dotBegin = IMG_H - svLenDot;
        m_svPosBefore = IMG_H - svLenDot/2;
        m_svPos = SVPosCorrect(m_svPosBefore);
    }
#endif
    if (dotBegin < 0)
    {
        dotBegin = 0;
        dotEnd = svLenDot;
        m_svPosBefore = svLenDot/2;
        m_svPos = SVPosCorrect(m_svPosBefore);
    }

    double pos = m_svPosBefore * scale;
    m_ptrUpdate->SVPos(pos);

    // calc svBegin and svEnd
    double svPosDetph = m_svPosBefore * scale; //mm
    double svBeginDepth = dotBegin * scale;
    double svEndDetph = svPosDetph + (float)svLen / 10 / 2; //mm

    double speed = m_ptrImg2D->GetSoundSpeed(); //km/s
    double depthToSample;
    if(svEndDetph < 25)
        depthToSample = 25;
    else
        depthToSample = svEndDetph;

    //calc prf is not connection to current depth
   // depthToSample = Img2D::GetInstance()->GetDepth();
    m_depthToSample = depthToSample;
#ifdef EMP_355
    double newScale =  depthToSample / m_ptrImg2D->GetDepthDots(); //mm/p
#else
    double newScale =  depthToSample / IMG_H; //mm/p
#endif
    //int newSvPosDot = svPosDetph / newScale;
    int newDotBegin = svBeginDepth/newScale;
    int newsvLenDot = (float)svLen / 10 / newScale;

    int newDotEnd = newDotBegin + newsvLenDot;// newSvPosDot + newsvLenDot / 2;

    PRINTF("================SVCalc verify: dotBegin = %d, dotEnd = %d  \n",newDotBegin,newDotEnd);

    // send to fpga
    if(!m_HPRF)
        m_ptrCalc->CalcHDotSample(depthToSample, speed);

    if (SendFpga)
    {
#ifdef EMP_355
    // send sample begin and end
        if(!ViewMain::GetInstance()->GetModeIsFlag())
        {
            m_ptrCalc->CalcPwLine(pwLine);
            SVCorrect(newDotBegin, newDotEnd);
        }
#else
        m_ptrCalc->CalcPwLine(pwLine);
        // send sample begin and end
        SVCorrect(newDotBegin, newDotEnd);
#endif
    }
}

/*
 * @brief change spectrum speed
 *
 * @para data[in] index of spectrum speed
 * @para ret[in] status of Knob spectrum speed
 */
void ImgPw::GetPwLineRange(int &begin, int &end)
{
    int interval = 0;
    int scanRange[2];
    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
        Zoom::GetInstance()->GetScanLineRange(scanRange);
    }
    else
    {
        Img2D::GetInstance()->GetDisplayScanRange(scanRange);
    }

    begin = scanRange[0] + interval;
    end = scanRange[1] - interval;
}

/*
 * @brief 获得当前图像上的移动点范围。
 */
void ImgPw::GetSvPosRange(int &begin, int &end)
{
    //int offset = m_ptrImg2D->GetImgStartDots();
    int offset = 0;
    double scale = m_ptrImg2D->GetScale2DInImgHDot();

    if (Zoom::GetInstance()->GetLocalZoomStatus())
    {
        double scaleZoom;
        Zoom::GetInstance()->GetLocalZoomScale(scaleZoom);
        scale = scale / scaleZoom;
    }

    int svLenDot = (float)m_svLen / 10 / scale;
    begin = svLenDot / 2;
#ifdef EMP_355
    end = m_ptrImg2D->GetDepthDots() - offset - svLenDot / 2;
#else
    end = IMG_H - offset - svLenDot / 2;
#endif

    PRINTF("get sv pos range: m_svLen = %d, scale = %f, offset = %d, svLenDot = %d, begin = %d, end = %d\n", m_svLen, scale, offset, svLenDot, begin, end);
}
void ImgPw::SpectrumSpeed(int speedIndex, int prfIndex, EKnobReturn ret)
{

#ifdef EMP_355
   bool mode = ViewMain::GetInstance()->GetModeIsFlag();
   int num;
    if(mode)
        num = GetPwLineOneImg(m_speedIndex, GetHPRFValue(m_PRFIndex));
    else
        num = GetPwLineOneImg(m_speedIndex, GetPRFValue(m_PRFIndex));
    m_ptrDscPara->dcaPWSpeed = num;
    PRINTF("---------------DSC spectrum speed num = %d\n", m_ptrDscPara->dcaPWSpeed);
    int prf;
    if(mode)
    {
        prf = GetHPRFValue(prfIndex);
    }
    else
    {
        prf = GetPRFValue(prfIndex);

    }
#else
    // set pw speed in dsc
    int num = GetPwLineOneImg(m_speedIndex, GetPRFValue(m_PRFIndex));
    m_ptrDscPara->dcaPWSpeed = num;
    PRINTF("---------------DSC spectrum speed num = %d\n", m_ptrDscPara->dcaPWSpeed);

    int prf = GetPRFValue(prfIndex);
#endif

    int speed = GetSpeed(speedIndex, prf);
    m_ptrCalc->CalcSpeed(speed);
    m_ptrUpdate->Speed(speedIndex, ret);

    //send dsc
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.interval = (float)speed / prf;

    //PRINTF("spectrum speed = %d\n", speed);
    m_ptrCalc->CalcSoundInterpolation(prf, speed);
}

void ImgPw::SoundStatus(bool data, EKnobReturn ret)
{
    m_ptrCalc->CalcSoundStatus((int)data);
    m_ptrUpdate->SoundStatus((int)data, ret);
#ifdef EMP_355
    bool mode = ViewMain::GetInstance()->GetModeIsFlag();
#endif

    // sound volume
    if (data)
    {
        usleep(1);
        SoundVolume(m_soundVolumeIndex, OK);
        int speed = GetSpeed(m_speedIndex, GetPRFValue(m_PRFIndex));
#ifdef EMP_355
        if(mode)
            m_ptrCalc->CalcSoundInterpolation(GetHPRFValue(m_PRFIndex), speed);
        else
            m_ptrCalc->CalcSoundInterpolation(GetPRFValue(m_PRFIndex), speed);
#else
        m_ptrCalc->CalcSoundInterpolation(GetPRFValue(m_PRFIndex), speed);
#endif
    }
}
//public fun
void ImgPw::ChangeDopplerSoundStatus(bool data)
{
#if 0
    //close HV
    if(data)
        ProbeMan::GetInstance()->ActiveHV(TRUE);
    else
        ProbeMan::GetInstance()->ActiveHV(FALSE);
#endif
    EKnobReturn ret = OK;
    if (!data)
        ret = MIN;
    else if (data)
        ret = MAX;
    else
        ret = OK;
    m_soundStatus=data;
    SoundStatus(data, ret);
}

void ImgPw::SoundVolume(int data, EKnobReturn ret)
{
    m_ptrCalc->CalcSoundVolume(data);

    // only for test
    m_ptrUpdate->SoundVolume(data, ret);
}

void ImgPw::CorrectAngle(int data, EKnobReturn ret)
{
    CalcTraceFs(m_wallFilterIndex, m_PRFIndex, m_baseline, data);
    m_ptrUpdate->CorrectAngle(data, ret);
}

void ImgPw::Simult2(bool on, EKnobReturn ret)
{
    // change prf
    if (on)
    {
        m_simult2PRFIndex = GetSimult2PRFIndex(PW_PRF[m_PRFIndex]);
        PRINTF("-------------simult2 on: prf = %d\n", SIMULT2_PRF[m_simult2PRFIndex]);
        PRFSimult2(m_simult2PRFIndex);
    }
    else
    {
        m_ptrImg2D->ExitSimult();
        PRINTF("-------------simult2 off: prf = %d\n", PW_PRF[m_PRFIndex]);
        PRF(m_PRFIndex);
    }

    // update
    m_ptrUpdate->Simult(on, ret);
}

void ImgPw::Simult3(bool on, EKnobReturn ret)
{
    // change prf
    if (on)
    {
        m_simult3PRFIndex = GetSimult3PRFIndex(GetPRFValue(m_PRFIndex));
        PRFSimult3(m_simult3PRFIndex);
    }
    else
    {
        m_ptrImg2D->ExitSimult();
        ImgCfm::GetInstance()->ExitSimult3();
        PRF(m_PRFIndex);
    }

    // update
    m_ptrUpdate->Simult(on, ret);
}

void ImgPw::HPRF(bool on, EKnobReturn ret)
{
    m_HPRF = on;
#ifdef EMP_355
bool mode = ViewMain::GetInstance()->GetModeIsFlag();
    if(!mode)
    {
        //switch HPRF when on is true, switch PRF when on is false.
        if (on)
        {
            m_PRFIndexBak = m_PRFIndex;
            m_PRFIndexTempBak = m_PRFIndex;
            ScanMode::GetInstance()->EnterHPRF(TRUE);
        }
        else
        {
            //m_PRFIndex = m_PRFIndexBak;
            ScanMode::GetInstance()->EnterHPRF(FALSE);
        }

        int dotBegin;
        int dotEnd;
        SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, FALSE);
        SwitchHPRF(on, m_svPos*m_ptrImg2D->GetScale2DInImgHDot());

        //update view
        UpdateSV(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
        //m_ptrUpdate->HPRF(on, ret);
    }
    else
    {
        //switch HPRF when on is true, switch PRF when on is false.
        m_PRFIndexBak = m_PRFIndex;
        m_PRFIndexTempBak = m_PRFIndex;
        ScanMode::GetInstance()->EnterHPRF(TRUE);
        int dotBegin;
        int dotEnd;
        SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, FALSE);
        int num = SwitchHPRF(on, m_svPos*m_ptrImg2D->GetScale2DInImgHDot());

        if(num)
        {
            if(m_PRFIndex > m_maxPRFIndex)
                m_PRFIndex = m_maxPRFIndex;
            PRF(m_PRFIndex);
            //update view
            UpdateSV(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
            int prf = GetHPRFValue(m_PRFIndex);
            EKnobReturn ret1;
            if(prf >= GetPRFValue(m_maxPRFIndex))
                ret1 = MAX;
            else if(prf ==350)
                ret1 = MIN;
            else
                ret1 =OK;

            m_ptrUpdate->PRF(prf, ret1);
        }   //m_ptrUpdate->HPRF(on, ret);
    }
#else
    //switch HPRF when on is true, switch PRF when on is false.
    if (on)
    {
        m_PRFIndexBak = m_PRFIndex;
        m_PRFIndexTempBak = m_PRFIndex;
        ScanMode::GetInstance()->EnterHPRF(TRUE);
    }
    else
    {
        //m_PRFIndex = m_PRFIndexBak;
        ScanMode::GetInstance()->EnterHPRF(FALSE);
    }

    int dotBegin;
    int dotEnd;
    SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, FALSE);
    SwitchHPRF(on, m_svPos*m_ptrImg2D->GetScale2DInImgHDot());

    //update view
    UpdateSV(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
#endif
    m_ptrUpdate->HPRF(on, ret);
}

EKnobReturn ImgPw::PRFHPRF(int index)
{
    m_ptrCalc->CalcMaxPeriod(GetPRFValue(index));

    // update vie
    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (MAX_PRF - 1))
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(GetPRFValue(index), ret);

    // update para
    WhenPRFChanged(index);

    return ret;
}

EKnobReturn ImgPw::PRF(int index)
{
    PRINTF("----------prf normal: PRF = %d, m_depthToSample = %f\n", PW_PRF[index], m_depthToSample);

    IoCtrl io;
    io.Freeze();
    EKnobReturn ret = OK;

#ifdef EMP_355
    bool mode = ViewMain::GetInstance()->GetModeIsFlag();
    if(!mode)
    {
        bool retVal = CalcPRFNormal(GetPRFValue(index), m_depthToSample);
        if (m_PRFIndex == 0) // must use m_PRFIndex, PRF mybe call recursive in ClusterSizeNormal functon
            ret = MIN;
        else if (m_PRFIndex == m_maxPRFIndex)
            ret = MAX;
        else
            ret = OK;
        m_ptrUpdate->PRF(GetPRFValue(m_PRFIndex), ret);

        if (retVal)
        {
            PRINTF("----------------------prf normal: index = %d\n", index);
            WhenPRFChanged(index);
        }
    }
    else
    {
        WhenPRFChanged(index);
    }
#else
    // calPwc
    bool retVal = CalcPRFNormal(GetPRFValue(index), m_depthToSample);
    if (m_PRFIndex == 0) // must use m_PRFIndex, PRF mybe call recursive in ClusterSizeNormal functon
        ret = MIN;
    else if (m_PRFIndex == m_maxPRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(GetPRFValue(m_PRFIndex), ret);

    // update para
    if (retVal)
    {
        PRINTF("----------------------prf normal: index = %d\n", index);
        WhenPRFChanged(index);
    }
#endif
    ScanMode::GetInstance()->SetPWPulseNum();

    io.Unfreeze();

    return ret;
}

/*
 * @brief do something when simult prf changed, prf range according to simult-prf, don't calc maxperiod
 */
EKnobReturn ImgPw::PRFSimult2(int index)
{
    PRINTF("----------prf simult2: SIMULT2_PRF = %d, m_depthToSample = %f\n", SIMULT2_PRF[index], m_depthToSample);
    // calc maxperiod
    bool retVal = CalcPRFSimult2(SIMULT2_PRF[index], m_depthToSample);
    EKnobReturn ret = OK;
    if (m_simult2PRFIndex == 0)
        ret = MIN;
    else if (m_simult2PRFIndex == m_maxSimult2PRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(SIMULT2_PRF[m_simult2PRFIndex], ret);

    // update prf index in normal mode
    if (retVal)
    {
        m_PRFIndex = GetPRFIndex(SIMULT2_PRF[index]);
        PRINTF("----------------------prf simult2: index = %d\n", m_PRFIndex);
        WhenPRFChanged(m_PRFIndex);
    }

    return ret;
}

/*
 * @brief deal something when prf index if change in simult mode
 */
EKnobReturn ImgPw::PRFSimult3(int index)
{
    //cluster size
    bool retVal = CalcPRFSimult3(SIMULT3_PRF[index], m_depthToSample);
    EKnobReturn ret = OK;
    if (m_simult3PRFIndex == 0)
        ret = MIN;
    else if (m_simult3PRFIndex == m_maxSimult3PRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRF(SIMULT3_PRF[m_simult3PRFIndex], ret);

    // update CFM prf index in normal mode
    if (retVal)
    {
        m_PRFIndex = GetPRFIndex(SIMULT3_PRF[index]);
        PRINTF("----------------simut3 prf = %d\n", PW_PRF[m_PRFIndex]);
        WhenPRFChanged(m_PRFIndex);

        // cfm chang when simult3
        ImgCfm::GetInstance()->PRFSimult3(SIMULT3_PRF[index], ret);
    }

    return ret;
}

bool ImgPw::CalcPRFNormal(int prf, float samplePos)
{
    CalcPRFMaxIndex(samplePos);

    //因为m_PRFIndex在上面的函数中可能会变化，此时计算的maxPeriof必须和变化了的prfIndex对应
    //该处理放到上面的函数内部进行处理
    //m_ptrCalc->CalcMaxPeriod(prf);

    // view
    EKnobReturn ret = OK;
    if (m_PRFIndex == 0)
        ret = MIN;
    else if (m_PRFIndex == m_maxPRFIndex)
        ret = MAX;
    else
        ret = OK;

    m_ptrUpdate->PRFRange(GetPRFValue(m_PRFIndex), ret);

    PRINTF("prf = %d, prfMaxIndex = %d\n", prf, m_maxPRFIndex);
    return TRUE;
}

bool ImgPw::CalcPRFMaxIndex(float samplePos)
{
    int prfMax;
    double speed = m_ptrImg2D->GetSoundSpeed();
    float tDepth = 2 * samplePos / speed + FOC_LOW_TIME + 5;//20 + 5 ; //20us

    ModeStatus s1;
    ScanMode::EScanMode mode = s1.GetScanMode();
    //if (ModeStatus::IsCWImgMode())
    //  prfMax = 50000;
    // else
    // cw the same as pw on m_maxPRFIndex
    prfMax = 1000000 / tDepth;
    m_maxPRFIndex = GetPRFIndex(prfMax);

    PRINTF("prfIndex = %d, prfMax = %d, prfMaxIndex = %d\n", prfMax, m_PRFIndex, m_maxPRFIndex);
    if (m_PRFIndex > m_maxPRFIndex)
    {
        m_PRFIndex = m_maxPRFIndex;
        PRF(m_PRFIndex);
        return FALSE;
    }

    m_ptrCalc->CalcMaxPeriod(GetPRFValue(m_PRFIndex));
}

bool ImgPw::CalcPRFSimult2(int prfSimult, float samplePos)
{
    // calc
    float depth2D = Img2D::GetInstance()->GetDepth();
    float depthSv = samplePos;
    float speed = Img2D::GetInstance()->GetSoundSpeed();
    int maxPeriod2D = depth2D * 2 / speed + 5 ; //us
    int maxPeriodCfm = maxPeriod2D;
    int maxPeriodPw;// = maxPeriod2D;
    int prf = prfSimult;

    maxPeriodPw = depthSv * 2 /speed + 5; //us

    int mbp = 1;
    if (m_ptrImg2D != NULL)
    {
        mbp = m_ptrImg2D->GetMBPFpgaColor();
    }

    int maxValue = 100;
    if (mbp <= 2)
        maxValue = 80;
    else
        maxValue = 100;

    if(maxPeriod2D < maxValue)
        maxPeriod2D = maxValue;

    PRINTF("================ duplex: calculate maxPeriod =============\n");
    PRINTF(" maxPeriod2D = %d maxPeriodPw = %d, maxPeriodCfm = %d \n",maxPeriod2D,maxPeriodPw,maxPeriodCfm);
    //deal the problem prf is out of range
    int maxPRF = (float)1000000 / (maxPeriod2D + FOC_LOW_TIME + maxPeriodPw + FOC_LOW_TIME); //hz
    m_maxSimult2PRFIndex = GetSimult2PRFIndex(maxPRF);
    if (m_simult2PRFIndex > m_maxSimult2PRFIndex)
    {
        m_simult2PRFIndex = m_maxSimult2PRFIndex;
        PRFSimult2(m_simult2PRFIndex);
        return FALSE;
    }

    int clusterSize =  ((float)1000000/prf - maxPeriodPw - FOC_LOW_TIME) / (maxPeriod2D + FOC_LOW_TIME);
    maxPeriodPw = 1000000/(float)prf - (maxPeriod2D + FOC_LOW_TIME) * clusterSize - FOC_LOW_TIME;

    PRINTF("========================= duplex parameters===================\n");
    PRINTF("maxPRF = %d,clusterSize = %d,  maxPeriod2D = %d, maxPeriodCFM = %d, maxPeriodPW = %d\n",maxPRF,clusterSize,maxPeriod2D,maxPeriodCfm,maxPeriodPw);
    PRINTF("=========== duplex PRFtime = %d\n",maxPeriodPw+60+(maxPeriod2D+60)*clusterSize);

    // send to fpga
    FpgaCfm fpgaCfm;
    FpgaPw fpgaPw;
    FpgaCtrl2D fpga2D;
    fpga2D.SendMaxPeriod(maxPeriod2D);
    fpgaPw.SendMaxPeriod(maxPeriodPw);
    fpgaCfm.SendClusterSize(clusterSize);//useful
    fpgaCfm.SendMaxPeriod(maxPeriodCfm);

    // view
    EKnobReturn ret = OK;
    if (m_simult2PRFIndex == 0)
        ret = MIN;
    else if (m_simult2PRFIndex == m_maxSimult2PRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRFRange(SIMULT2_PRF[m_simult2PRFIndex], ret);

    return TRUE;
}

/*
 * @brief calc cluster size in simult 3 mode, relate para: 2D depth, sv depth, prf of simult
 * @para prf[in] prf in simult mode
 * @para depth[in] depth of of sv unit:mm
 */
bool ImgPw::CalcPRFSimult3(int prfSimult, int samplePos)
{
    // calc
    float depth2D = m_ptrImg2D->GetDepth();
    float speed = Img2D::GetInstance()->GetSoundSpeed();
#ifdef EMP_430
    int maxPeriod2D = depth2D * 2 / speed +10;// 5 ; //us
    int maxPeriodPw = samplePos * 2 / speed + 10;//5 ;//us
#else
    int maxPeriod2D = depth2D * 2 / speed +5;// 20;// 5 ; //us
    int maxPeriodPw = samplePos * 2 / speed + 5;// 20;//5 ;//us
#endif
    int maxPeriodCfm = maxPeriod2D;
    int prf = prfSimult;

//  PRINTF("================triplex: calculate maxPeriod=============\n");
//  PRINTF(" maxPeriod2D = %d maxPeriodPw = %d, maxPeriodCfm = %d \n",maxPeriod2D,maxPeriodPw,maxPeriodCfm);

    int mbp = 1;
    if (m_ptrImg2D != NULL)
    {
        mbp = m_ptrImg2D->GetMBPFpgaColor();
        PRINTF("mbp = %d\n", mbp);
    }

    int maxValue = 100;
    if (mbp <= 2)
        maxValue = 80;
    else
        maxValue = 100;

    if(maxPeriodCfm < maxValue)
        maxPeriodCfm = maxValue;

    maxPeriod2D = maxPeriodCfm;

    // if or not is out of range
    int maxPRF = (float)1000000 / (maxPeriod2D + FOC_LOW_TIME + maxPeriodPw + FOC_LOW_TIME); //hzi
    m_maxSimult3PRFIndex = GetSimult3PRFIndex(maxPRF);
    if (m_simult3PRFIndex > m_maxSimult3PRFIndex)
    {
        m_simult3PRFIndex = m_maxSimult3PRFIndex;
        PRFSimult3(m_simult3PRFIndex);
        return FALSE;
    }

    // calc para and send to fpga
    int clusterSize =  ((float)1000000/prf - maxPeriodPw - FOC_LOW_TIME) / (maxPeriod2D + FOC_LOW_TIME);
    maxPeriodPw = 1000000/(float)prf - (maxPeriod2D + FOC_LOW_TIME) * clusterSize - FOC_LOW_TIME;

    PRINTF("=========================triplex parameters===================\n");
    PRINTF("maxPRF = %d,clusterSize = %d,  maxPeriod2D = %d, maxPeriodCFM = %d, maxPeriodPW = %d\n",maxPRF,clusterSize,maxPeriod2D,maxPeriodCfm,maxPeriodPw);

    //send to fpga
    FpgaCfm fpgaCfm;
    FpgaPw fpgaPw;
    FpgaCtrl2D fpga2D;
    fpga2D.SendMaxPeriod(maxPeriod2D);
    fpgaPw.SendMaxPeriod(maxPeriodPw);
    fpgaCfm.SendClusterSize(clusterSize);
    fpgaCfm.SendMaxPeriod(maxPeriodCfm);

    // view
    EKnobReturn ret = OK;
    if (m_simult3PRFIndex == 0)
        ret = MIN;
    else if (m_simult3PRFIndex == m_maxSimult3PRFIndex)
        ret = MAX;
    else
        ret = OK;
    m_ptrUpdate->PRFRange(SIMULT3_PRF[m_simult3PRFIndex], ret);

    return TRUE;
}

void ImgPw::CalcPRF(int prf, float samplePos)
{
    if (m_simult2)
        CalcPRFSimult2(prf,samplePos);
    else if (m_simult3)
        CalcPRFSimult3(prf,samplePos);
    else
        CalcPRFNormal(prf, samplePos);
}

void ImgPw::WhenPRFChanged(int index)
{
    EKnobReturn ret = OK;

    // sound stop dot
#ifdef EMP_355
 if(ViewMain::GetInstance()->GetModeIsFlag())
    {
        m_ptrCalc->CalcSoundStopDot(GetHPRFValue(index));
    }
    else
    {
        // sound stop dot
        m_ptrCalc->CalcSoundStopDot(GetPRFValue(index));
    }
#else
    m_ptrCalc->CalcSoundStopDot(GetPRFValue(index));
#endif

    if (m_wallFilterIndex == (MAX_WALL_FILTER-1))
        ret = MAX;
    else if (m_wallFilterIndex == 0)
        ret = MIN;
    else
        ret = OK;

    m_ptrUpdate->WallFilter(m_wallFilterIndex, ret);
    // pw trace
    CalcTraceFs(m_wallFilterIndex, index, m_baseline, m_angle);

    if (m_speedIndex == 0)
        ret = MIN;
    else if (m_speedIndex == (MAX_SPEED_INDEX-1))
        ret = MAX;
    else
        ret = OK;
    SpectrumSpeed(m_speedIndex, index, ret);
}
void ImgPw::GrayMap(int index, EKnobReturn ret)
{
    m_ptrUpdate->GrayMap(index, ret);
}

void ImgPw::NoiseThreshold(int data, EKnobReturn ret)
{
    SetTraceNoiseThreshold(data);

    // send to fpga
    m_ptrCalc->CalcNoise(data);

    m_ptrUpdate->Noise(data, ret);
}

void ImgPw::SoundFilter(int index, EKnobReturn ret)
{
    // send to fpga
    m_ptrCalc->CalcSoundFilter(index);

    m_ptrUpdate->SoundFilter(index, ret);
}

void ImgPw::Invert(bool on)
{
    // send to dsc
    //  m_ptrDscPara->dcaPWIPAttrs.ipaUDOverturn = on;

    // send to fpga
    m_ptrCalc->CalcInvert(on);

    // update view
    m_ptrUpdate->SpectrumInvert(on);
}

/*
 * @brief change steer angle
 *
 */
void ImgPw::Steer(int angle, EKnobReturn ret)
{
    // send to dsc
    m_ptrDscPara->dcaCFMSteer = angle;

    // send to fpga
    m_ptrImg2D->SetSteerColor(angle);
    m_ptrImg2D->CalcReceiveDelayColor();

    // send to display
    m_ptrUpdate->Steer(angle, ret);

    // draw sample volume
    int current, before;
    double angleCurrent, angleBefore, dAngle;
    current = angle;
    before = STEER_ANGLE[m_steerIndexBak];

    // correct angle
    if (current > 0)
        m_angle = -abs(m_angle);
    else if (current < 0)
        m_angle = abs(m_angle);

    EKnobReturn ret1 = OK;
    if (m_angle == -MAX_ANGLE)
        ret1 = MIN;
    else if (m_angle== MAX_ANGLE)
        ret1 = MAX;
    else
        ret1 = OK;
    CorrectAngle(m_angle, ret1);

    // steer sv, make center of sv is not changed according to screen
    angleCurrent = current * PI / 180;
    angleBefore = before * PI / 180;
    dAngle = angleCurrent - angleBefore;
    double scale = m_ptrImg2D->GetScale2DInImgHDot();
    float focPos = m_svPosBefore * scale;
    float wLines = m_ptrImg2D->GetWidthBetweenLinesL();
    int nLines = focPos * sin(dAngle) / wLines;
    int nDots = m_svPosBefore * fabs(cos(angleBefore)) / cos(angleCurrent) - m_svPosBefore;

    int rangeLine[2], rangeSvPos[2];
    GetPwLineRange(rangeLine[0], rangeLine[1]);
    GetSvPosRange(rangeSvPos[0], rangeSvPos[1]);
    if (((m_pwLine - nLines) >= rangeLine[0]) && (m_pwLine - nLines) <= rangeLine[1])
        m_pwLine -= nLines;
    if (((m_svPosBefore + nDots) >= rangeSvPos[0]) && ((m_svPosBefore + nDots) <= rangeSvPos[1]))
    {
        m_svPosBefore += nDots;
        m_svPos = SVPosCorrect(m_svPosBefore);
    }
    SendSampleVolume(m_pwLine, m_svPos, m_svLen);

    // change focus
    FocPos(m_svPos);
}

void ImgPw::ChangeSoundPower(EKnobOper oper)
{
    int power = m_ptrImg2D->GetOutputPower();
    CalcFocPulse(m_freq[m_freqIndex], power);
}

// for test
EKnobReturn ImgPw::ChangeSoundFilter(EKnobOper oper)
{
    int index = m_soundFilterIndex;
    int step = 1;
    int max = m_ptrCalc->GetMaxSoundFilter();

    if (oper == ADD)
    {
        if (index < (max - 1))
        {
            index += step;
        }
        else
        {
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (index > 0)
        {
            index -= step;
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

    m_soundFilterIndex = index;

    EKnobReturn ret = OK;
    if (index == 0)
        ret = MIN;
    else if (index == (max - 1))
        ret = MAX;
    else
        ret = OK;

    SoundFilter(m_soundFilterIndex, ret);

    return (ret);
}

/** @brief switch between HPRF and PRF.
 *  @param on switch to HPRF when on is true, switch to PRF when on is false.
 *  @param svPos depth of sv, unit:mm.
 */
int ImgPw::SwitchHPRF(bool on, float svPos)
{
    int num = 0;
    int dotBegin;
    int dotEnd;
#ifdef EMP_355
 if(on)
    {
        float svBegin = svPos - (float)m_svLen/20;
        float svEnd = svBegin + (float)m_svLen/10;

        PRINTF("=================111111SV Depth HPRF 1: svBegin = %3f, svEnd = %3f \n",svBegin,svEnd);
        //test
        bool mode = ViewMain::GetInstance()->GetModeIsFlag();
        if(mode)
            num = m_ptrCalc->SVPosPRFToHPRF(svBegin, svEnd, GetHPRFValue(m_PRFIndex), m_PRFIndex);
        else
        {
            num = m_ptrCalc->SVPosPRFToHPRF(svBegin, svEnd, GetPRFValue(m_PRFIndex), m_PRFIndex);
            PRINTF("=================2222222SV Depth HPRF 1: svBegin = %3f, svEnd = %3f \n",svBegin,svEnd);

            // recalc
            float speed = m_ptrImg2D->GetSoundSpeed();
            if (svEnd < 25)
                m_depthToSample = 25;
            else
                m_depthToSample = svEnd;

            m_ptrCalc->CalcHDotSample(m_depthToSample, speed);
            //reCalc end

            double scale = m_depthToSample / (float)m_ptrImg2D->GetDepthDots();
            dotBegin = svBegin / scale;
            dotEnd = svEnd / scale;
            SVCorrect(dotBegin, dotEnd);

            PRINTF("============== PRFindex HPRF : m_PRFIndex = %d, m_PRFIndexTempBak = %d \n",m_PRFIndex,m_PRFIndexTempBak);
            if (m_PRFIndex != m_PRFIndexTempBak)
            {
                m_PRFIndexTempBak = m_PRFIndex;
                PRFHPRF(m_PRFIndex);
            }

        }
        m_ptrCalc->CalcHPRF(num);

    }
    else
    {
        //send to fpga
        SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, TRUE);
        PRF(m_PRFIndex);
        m_ptrCalc->CalcHPRF(num);
    }
#else
    if(on)
    {
        float svBegin = svPos - (float)m_svLen/20;
        float svEnd = svBegin + (float)m_svLen/10;

        PRINTF("=================111111SV Depth HPRF 1: svBegin = %3f, svEnd = %3f \n",svBegin,svEnd);
        //test

        num = m_ptrCalc->SVPosPRFToHPRF(svBegin, svEnd, GetPRFValue(m_PRFIndex));

        // recalc
        float speed = m_ptrImg2D->GetSoundSpeed();
        if (svEnd < 25)
            m_depthToSample = 25;
        else
            m_depthToSample = svEnd;

        m_ptrCalc->CalcHDotSample(m_depthToSample, speed);
        //reCalc end

        double scale = m_depthToSample / (float)IMG_H;
        dotBegin = svBegin / scale;
        dotEnd = svEnd / scale;
        SVCorrect(dotBegin, dotEnd);

        PRINTF("============== PRFindex HPRF : m_PRFIndex = %d, m_PRFIndexTempBak = %d \n",m_PRFIndex,m_PRFIndexTempBak);
        if (m_PRFIndex != m_PRFIndexTempBak)
        {
            m_PRFIndexTempBak = m_PRFIndex;
            PRFHPRF(m_PRFIndex);
        }
    }
    else
    {
        //send to fpga
        SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, TRUE);
        PRF(m_PRFIndex);
    }

    m_ptrCalc->CalcHPRF(num);
#endif
    return num;
}

/** @brief get emit position of HPRF.
 *  @return array include all emits' position, unit:dot on pw line.
 */
vector<int> ImgPw::GetHPRFEmitPos(void)
{
    vector<int> emitDots;
    double soundSpeed = m_ptrImg2D->GetSoundSpeed();

    if(m_HPRF)
        emitDots = m_ptrCalc->GetHPRFEmitPos(GetPRFValue(m_PRFIndex), m_svPos, soundSpeed, m_ptrImg2D->GetScale2DInImgHDot());

    return emitDots;
}

/*
 * @brief get proper spectrum speed according to current speed and prf
 *
 * @para speedIndex index of spectrum speed
 * @para prfIndex index of PRF
 *
 * @retval real speed calculated
 */
int ImgPw::GetSpeed(int speedIndex, int prf)
{
    int speed = SPECTRUM_SPEED[speedIndex];
    int speedNew = prf * speed / 8000;//8000;

    //PRINTF("(((((((((((((((((((((speedNew = %d\n", speedNew);
    speedNew = speedNew;// / 2 * 2;
    if (speedNew > 127)
        speedNew = 127;
    if (speedNew < 1)
        speedNew = 1;

//  PRINTF("revised speedNew = %d\n", speedNew);

    return speedNew;
}
void ImgPw::Log(int value, EKnobReturn ret)
{
    m_ptrCalc->CalcLog(value);

    m_ptrUpdate->Log(value, ret);
}

/*
 * @brief get PRF index, which PRF is similar to prf
 *
 * @para prf reference of PRF, unit: hz
 *
 * @retval index of PRF
 */
int ImgPw::GetPRFIndex(unsigned int prf) //unit:hz
{
    int i;

    for (i = 0; i < MAX_PRF; i ++)
    {
        if (GetPRFValue(i) == prf)
        {
            break;
        }
        else if (GetPRFValue(i) > prf)
        {
            i--;
            break;
        }
    }

    if ( i > MAX_PRF-1)
        i = MAX_PRF-1;
    else if (i < 0)
        i = 0;

    PRINTF("get index of prf, prf = %d\n", GetPRFValue(i));

    return i;
}

int ImgPw::GetSimult2PRFIndex(unsigned int prf) //unit:hz
{
    int i;

    for (i = 0; i < MAX_SIMULT2_PRF_INDEX; i ++)
    {
        if (SIMULT2_PRF[i] == prf)
        {
            break;
        }
        else if (SIMULT2_PRF[i] > prf)
        {
            i--;
            break;
        }
    }

    if ( i > MAX_SIMULT2_PRF_INDEX - 1)
        i = MAX_SIMULT2_PRF_INDEX - 1;
    else if ( i < 0)
        i = 0;

    return i;
}

int ImgPw::GetSimult3PRFIndex(unsigned int prf) //unit:hz
{
    int i;

    for (i = 0; i < MAX_SIMULT3_PRF_INDEX; i ++)
    {
        if (SIMULT3_PRF[i] == prf)
        {
            break;
        }
        else if (SIMULT3_PRF[i] > prf)
        {
            i--;
            break;
        }
    }

    if ( i > MAX_SIMULT3_PRF_INDEX - 1)
        i = MAX_SIMULT3_PRF_INDEX - 1;
    else if (i < 0)
        i = 0;

    return i;
}

int ImgPw::CalcTraceFs(int wallFilterIndex, int prfIndex, int baseline, int angleCorrect)
{
    double speed = SOUND_SPEED * 1000;
    int wf;
#ifdef EMP_355
    if(ViewMain::GetInstance()->GetModeIsFlag())
        wf = WALL_FILTER_FREQ_CW[m_PRFIndex][wallFilterIndex];//unit:hz
    else
        wf = WALL_FILTER_FREQ[m_PRFIndex][wallFilterIndex];//unit:hz
#else
    wf = WALL_FILTER_FREQ[m_PRFIndex][wallFilterIndex];//unit:hz
#endif
    int freq = GetColorFreq();

    double vel = m_ptrCalc->CalcMaxVel(wf, freq, angleCorrect, speed);
    double scaleVel = GetScaleVel();

    int height = m_ptrDscPara->dcaPWSamplePoints;
    if(DscMan::GetInstance()->GetDsc() != NULL && DscMan::GetInstance()->GetDsc()->GetPWHeight() > 0)
        height =DscMan::GetInstance()->GetDsc()->GetPWHeight();

    float scalePos = (float)m_ptrDscPara->dcaPWSamplePoints / height;
    int fs = (int)((vel / scaleVel) * scalePos);

    SetTraceFs(fs);
    return fs;
}

void ImgPw::SetTraceFs(int fs)
{
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.fs = fs;
}

void ImgPw::SetTraceBaseline(int baseline)
{
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.baseline = baseline;
}

void ImgPw::SetTraceDynRange(int dynRange)
{
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.dynRange = dynRange;
}

void ImgPw::SetTraceGain(int gain)
{
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.gain = gain;
}

void ImgPw::SetTraceNoiseThreshold(int noiseThreshold)
{
    m_ptrDscPara->dcaPWIPAttrs.ipaTrace.noiseThreshold = noiseThreshold;
}

int ImgPw::CalcFocPulse(int freq, int power)
{
    int pulseCycle;
    int pulseWidth;

    pulseCycle = m_ptrCalc->CalcFocPulse(freq, power, pulseWidth);
    GlobalClassMan::GetInstance()->GetCalc2D()->CalcWeightingEmitColor(pulseWidth);

    return pulseCycle;
}

void ImgPw::SVCorrect(int begin, int end)
{
#ifdef EMP_355
    int offset = 0;//-15;
#else
    int offset = -15;
#endif
    PRINTF("*******///////// offset dots = %d \n", offset);
    m_ptrCalc->CalcSvPos(begin-offset, end-offset);
}

void ImgPw::BalanceCfmInfo(int pwLine, int pwSvPos, int pwSvLen)
{
    // deal when out of range
    ImgCfm::GetInstance()->WhenBoxIsOutRange(FALSE);

    // get cfm info
    int cfmLine[2], cfmDot[2];
    ImgCfm::GetInstance()->GetBoxInfo(cfmLine[0], cfmLine[1], cfmDot[0], cfmDot[1]);

    // get pw info
    int pwDotBegin, pwDotEnd, pwDotSvLen;
    CalcPwInfoDot(pwSvPos, pwSvLen, pwDotBegin, pwDotEnd, pwDotSvLen);

    PRINTF("balace cfm before: cfm lineb = %d, linee = %d, pw line = %d\n",cfmLine[0], cfmLine[1], pwLine);
    bool outRange = FALSE;
    int offset = 0;
    if (cfmLine[0] > pwLine)
    {
        // 防止经MBP校正后BOX的复位问题
        int range[2];
        GetPwLineRange(range[0], range[1]);
        int mbp = m_ptrImg2D->GetMBP();
        int temp = pwLine / mbp * mbp;

        if (temp >= range[0])
        {
            offset = cfmLine[1] - cfmLine[0];
            cfmLine[0] = pwLine;
            cfmLine[1] = cfmLine[0] + offset;
            outRange = TRUE;
        }
    }
    else if (cfmLine[1] < pwLine)
    {
        offset = cfmLine[1] - cfmLine[0];
        cfmLine[1] = pwLine;
        cfmLine[0] = cfmLine[1] - offset;
        outRange = TRUE;
    }

    int pwDotOffset = pwDotEnd - pwDotBegin;
    offset = cfmDot[1] - cfmDot[0];
    if (offset < pwDotOffset)
        offset = pwDotOffset;
    if (cfmDot[0] > pwDotBegin)
    {
        cfmDot[0] = pwDotBegin;
        cfmDot[1] = cfmDot[0] + offset;
#ifdef EMP_355
    if (cfmDot[1] > m_ptrImg2D->GetDepthDots())
        cfmDot[1] = m_ptrImg2D->GetDepthDots();
#else
    if (cfmDot[1] > IMG_H)
        cfmDot[1] = IMG_H;
#endif
        outRange = TRUE;
    }
    if (cfmDot[1] < pwDotEnd)
    {
        cfmDot[1] = pwDotEnd;
        cfmDot[0] = cfmDot[1] - offset;
        outRange = TRUE;
    }

    PRINTF("balace cfm after: cfm lineb = %d, linee = %d, pw line = %d\n",cfmLine[0], cfmLine[1], pwLine);

    if (outRange)
        ImgCfm::GetInstance()->SetBoxInfo(cfmLine[0], cfmLine[1], cfmDot[0], cfmDot[1]);
}

void ImgPw::FocPos(int svDotPos)
{
    float scale = m_ptrImg2D->GetScale2DInImgHDot();
    float pos = svDotPos * scale;

    //recalc the focus pos when the focus pos beyound old
    // inorder to improve the frame in cw mode
    int index = GetPwFocus(pos);
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    char type = m_ptrImg2D->ReviseProbeType(para.type);
#if 1
    if(ModeStatus::IsCWImgMode())
    {
        if (type == 'P')
        {
            if (m_pwFocusIndex != index)
            {
                m_ptrImg2D->CalcEmitDelayPw(PW_FOCUS[index]);
                m_pwFocusIndex = index;
            }
            else
            {
                //m_ptrImg2D->CalcEmitDelayPw(PW_FOCUS[index]);
            }
        }
        else
            m_ptrImg2D->CalcEmitDelayPw(pos);
    }
    else
    {
        if (type == 'P')
        {
            if (m_pwFocusIndex != index)
            {
                PRINTF("%s: calcEmit, index = %d\n", __FUNCTION__, index);
                m_ptrImg2D->CalcEmitDelayPw(PW_FOCUS[index]);
                m_pwFocusIndex = index;
            }
            else
            {
                //m_ptrImg2D->CalcEmitDelayPw(PW_FOCUS[index]);
            }
        }
        else
            m_ptrImg2D->CalcEmitDelayPw(pos);
    }
#endif
}

void ImgPw::DelaySendPwPara(void)
{
    OnPwImgCtrl(FALSE);
    int dotBegin;
    int dotEnd;
    m_sendPwLine = TRUE;
    //if(!m_HPRF)
    SVCalc(m_pwLine, m_svPos, m_svLen, dotBegin, dotEnd, m_sendPwLine);
    OnPwImgCtrl(TRUE);
}

void ImgPw::OnPwImgCtrl(bool on)
{
    if (on)
    {
        m_pwImgCtrl = TRUE;
    }
    else
    {
        m_pwImgCtrl = FALSE;
        int num = GetOffLineNum(m_speedIndex, GetPRFValue(m_PRFIndex));
        m_ptrCalc->CalcPwImgCtrlPara(num);
    }

    m_ptrCalc->CalcPwImgCtrl(m_pwImgCtrl);
}

void ImgPw::OnCwImgCtrl(bool on)
{
    m_cwImgCtrl = on;
    m_ptrCalc->CalcCwImgCtrl(on);
//    if (on)
//    ChangePwSV(0, 0); //send ph value to fpga
}

void ImgPw::SetScaleForSpecialMeasure(double scaleVel, double scaleTime, int baselineCalc)
{
    m_scaleVelSnap = scaleVel;
    m_scaleTimeSnap = scaleTime;
    m_baselineSnap = baselineCalc;
}

void ImgPw::SetSpectrumModeCW(bool isCW)
{
    if (isCW)
        m_specMode = SPECTRUM_CW;
    else
        m_specMode = SPECTRUM_PW;
}

int ImgPw::GetOffLineNum(int spectrumSpeedIndex, int prf)
{
    const int PRF = 6000;
    const int SPEED = GetSpeed(3, PRF);
    const int LINES = 6;
    int speed = GetSpeed(spectrumSpeedIndex, prf);

    int num = SPEED * prf * LINES /(float)(PRF * speed) + 0.5;
    return num;
}

int ImgPw::GetPwLineOneImg(int spectrumSpeedIndex, int prf)
{
    const int PRF = 6000;
    const int SPEED = GetSpeed(3, PRF);
    int speed = GetSpeed(spectrumSpeedIndex, prf);

    int num = SPEED * prf * 4 /(float)(PRF * speed) + 0.5;
    if (spectrumSpeedIndex == 0)
        num = 12;
    return num;
}

/*
 * @breif correct svPos(in dots) when local zoom
 */
int ImgPw::SVPosCorrect(int svPosDots)
{
   int posDots = svPosDots;

   Zoom* ptrZoom = Zoom::GetInstance();
   if (ptrZoom->GetLocalZoomStatus())
   {
       int dotRange[2];
       double scale;
       ptrZoom->GetScanDotRange(dotRange);
       ptrZoom->GetLocalZoomScale(scale);
       posDots = (float)posDots / scale + dotRange[0];
   }

   return posDots;
}

/*
 * divde pw focus three section
 * @para focPos[in] current pw focus pos, unit:mm
 */
int ImgPw::GetPwFocus(float focusPos)
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

int ImgPw::GetPRFValue(int prfIndex)
{
    ASSERT(prfIndex < MAX_PRF);

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    //解决当scale调节到使得PW_PRF[index]等于0，死机问题。
    //愿意是返回的prf值要在别的计算公式中充当分母 2014.03.11 lhma
    int temp;
    static int prfBak;
    temp = PW_PRF[prfIndex];
    if(para.type == 'P')
    {
        if(temp > PW_PRF[MAX_PRF-1])
        {
            temp = PW_PRF[MAX_PRF - 1];
        }
    }
    //return CW_PRF[prfIndex];
    if (temp != 0)
        prfBak = temp;
    else
        temp = prfBak;
    return temp;
}

int ImgPw::GetHPRFValue(int prfIndex)
{
    int prf;
    if(prfIndex ==0)
        prf = 350;
    else if (prfIndex ==1)
        prf = 500;
    else if(prfIndex ==2)
        prf = 750;
    else if((prfIndex > 2)&&(prfIndex < 22))
        prf = (500*(prfIndex-3))+1000;
    else if(prfIndex == 22)
        prf = 10200;
    else
        prf = (500*(prfIndex-4)) +1000;
    return prf;
}

void ImgPw::UpdateSV(int line, int dotBegin, int dotEnd, vector<int> HPRFEmitPos)
{
#ifdef EMP_355
    if (m_specMode == SPECTRUM_PW)
    {
        if(!ViewMain::GetInstance()->GetModeIsFlag())
            m_ptrUpdate->SampleVolume(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
        else
            m_ptrUpdate->SampleVolumeCW(m_pwLine, dotBegin);
    }
#else
    if (m_specMode == SPECTRUM_PW)
        m_ptrUpdate->SampleVolume(m_pwLine, dotBegin, dotEnd, GetHPRFEmitPos());
    else
        m_ptrUpdate->SampleVolumeCW(m_pwLine, dotBegin);
#endif
}
void ImgPw::UpdateSVClear(void)
{
#ifdef EMP_355
   if (m_specMode == SPECTRUM_PW)
    {
        if(!ViewMain::GetInstance()->GetModeIsFlag())
        m_ptrUpdate->SampleVolumeClear();
    else
        m_ptrUpdate->SampleVolumeCWClear();
    }
#else
    if (m_specMode == SPECTRUM_PW)
        m_ptrUpdate->SampleVolumeClear();
    else
        m_ptrUpdate->SampleVolumeCWClear();
#endif
}

///>test
void ImgPw::ChangeBandPassFilter(EKnobOper oper)
{
#if 0
    int step = 1;
    if(oper == ADD)
    {
        if((m_pulseCycle + step)< 24)
            m_pulseCycle += step;
    }
    else if(oper == SUB)
    {
        if(m_pulseCycle - step > 0)
            m_pulseCycle -= step;
    }

    m_ptrCalc->CalcBandPassFilter(m_pulseCycle);
#endif
    int step = 1;
    if(oper == ADD)
    {
        if(m_bandPassFilterList+step < 9)
            m_bandPassFilterList += step;
    }
    else if(oper == SUB)
    {
        if(m_bandPassFilterList-step > 0)
            m_bandPassFilterList -= step;
    }

    m_ptrCalc->CalcBandPassFilter_test(m_pulseCycle, m_bandPassFilterList);

    ImgCfm::GetInstance()->UpdateHint();
}

//------------------------- Quick Adjustment Pw ------------------------------//
void ImgPw::QAPwInitPara(int &prf)
{
    // PRF调节到最大
    EKnobReturn ret;
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);

    gdk_threads_enter();
    if (mode == ScanMode::PW_SIMULT)
    {
        switch(para.type)
        {
            case 'C':
                m_simult2PRFIndex = m_maxSimult2PRFIndex;
                break;
            case 'L':
                m_simult2PRFIndex = m_maxSimult2PRFIndex;
                break;
            default:
                m_simult2PRFIndex = m_maxSimult2PRFIndex;
                break;
        }
        PRINTF("-Init--m_simult2PRFIndex = %d\n", m_simult2PRFIndex);
        ret = PRFSimult2(m_simult2PRFIndex);
        prf = SIMULT2_PRF[m_simult2PRFIndex];
    }
    else if ((mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
    {
        switch(para.type)
        {
            case 'C':
                m_simult3PRFIndex = m_maxSimult3PRFIndex;
                break;
            case 'L':
                m_simult3PRFIndex = m_maxSimult3PRFIndex;
                break;
            default:
                m_simult3PRFIndex = m_maxSimult3PRFIndex;
                break;
        }
        PRINTF("-Init--m_simult3PRFIndex = %d\n", m_simult3PRFIndex);
        ret = PRFSimult3(m_simult3PRFIndex);
        prf = SIMULT3_PRF[m_simult3PRFIndex];
    }
    else
    {
        switch(para.type)
        {
            case 'C':
                m_PRFIndex = m_maxPRFIndex-2;
                break;
            case 'L':
                m_PRFIndex = m_maxPRFIndex-4;
                break;
            default:
                m_PRFIndex = m_maxPRFIndex-4;
                break;
        }
        PRINTF("-Init--m_PRFIndex = %d\n", m_PRFIndex);
        ret = PRF(m_PRFIndex);
        prf = PW_PRF[m_PRFIndex];
    }
    gdk_threads_leave();

    usleep(100000);
    // 基线调节到中间位置
    gdk_threads_enter();
    m_baselineIndex = 0;
    Baseline(m_baselineIndex, OK);
    gdk_threads_leave();

    // pw-gain很小时，影响数据分析，增大gain
    int data;
    if (m_gainPw < 40)
    {
        data = 40;
        SetTraceGain(data);
        m_ptrImg2D->CalcTgc(data*MAX_GAIN_PW/100, m_tgc, 2);
        m_ptrImg2D->CalcTgcDigital(data*MAX_GAIN_PW/100, 2, MAX_GAIN_PW);
    }
}

void ImgPw::QAPwCalcEmission(int onEmission)
{
    int power;
    if (onEmission)
    {
        power = m_ptrImg2D->GetOutputPower();
    }
    else
        power = 0;

    CalcFocPulse(m_freq[m_freqIndex], power);
}

static gboolean TimeoutClearHintArea(gpointer data)
{
    HintArea::GetInstance()->ClearHint();
    return FALSE;
}

void ImgPw::QAPwSetPara(float prfChg, int baseline)
{
    ModeStatus s;
    ScanMode::EScanMode mode = s.GetScanMode();

    int i;
    float minDistPRF = 11000.0;
    float distPRF;
    // PRF调节
    EKnobReturn ret;
    if (ModeStatus::IsUnFreezeMode())
    {
        gdk_threads_enter();
        if (mode == ScanMode::PW_SIMULT)
        {
            for (i = m_maxSimult2PRFIndex; i >=0; i--)
            {
                distPRF = fabs(prfChg - SIMULT2_PRF[i]);
                if (distPRF < minDistPRF)
                {
                    minDistPRF = distPRF;
                    m_simult2PRFIndex = i;
                }
            }
            ret = PRFSimult2(m_simult2PRFIndex);
        }
        else if ((mode == ScanMode::PWCFM_SIMULT) || (mode == ScanMode::PWPDI_SIMULT))
        {
            for (i = m_maxSimult3PRFIndex; i >=0; i--)
            {
                distPRF = fabs(prfChg - SIMULT3_PRF[i]);
                if (distPRF < minDistPRF)
                {
                    minDistPRF = distPRF;
                    m_simult3PRFIndex = i;
                }
            }
            ret = PRFSimult3(m_simult3PRFIndex);
        }
        else
        {
            for (i = m_maxPRFIndex; i >=0; i--)
            {
                distPRF = fabs(prfChg - PW_PRF[i]);
                if (distPRF < minDistPRF)
                {
                    minDistPRF = distPRF;
                    m_PRFIndex = i;
                }
            }
            ret = PRF(m_PRFIndex);
        }
        gdk_threads_leave();
    }

    usleep(100000);

    if (ModeStatus::IsUnFreezeMode())
    {
        // 基线调节
        gdk_threads_enter();
        m_baselineIndex = baseline;
        Baseline(m_baselineIndex, OK);
        gdk_threads_leave();
        DscMan::GetInstance()->GetWriteLock();
        DscMan::GetInstance()->GetDsc()->ClearPWLine();
        DscMan::GetInstance()->ReadWriteUnlock();

        // 恢复pw的增益
        if (m_gainPw < 25)
        {
            m_gainPw = 25;
            GainPw(m_gainPw);
        }
    }
    g_timeout_add(500, TimeoutClearHintArea, NULL);
 //   HintArea::GetInstance()->ClearHint();
}

int ImgPw::GetKnobSpectrumSpeed()
{
    int speed = SPECTRUM_SPEED[m_speedIndex];
    return speed;
}