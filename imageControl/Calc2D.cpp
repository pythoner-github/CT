#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include "imageControl/Calc2D.h"
#include "Def.h"
#include "imageControl/FpgaCtrl2D.h"
#include "imageProc/ModeStatus.h"
#include "base/CalcTime.h"
#include "probe/ProbeMan.h"
#include "display/HintArea.h"
#include "sysMan/ViewSuperuser.h"
#include "ViewMain.h"
///> dynamic range
const int Calc2D::DYNAMIC_DATA[MAX_DYNAMIC_INDEX] = {197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197};
const Calc2D::LogPair Calc2D::m_logPara[MAX_DYNAMIC_INDEX] = {
    {40, 25}, {45, 24}, {50, 23}, {55, 18}, {60, 16}, {65, 12}, {70, 8}, {78, 2},
    {80, 0}, {82, -2}, {85, -4}, {88, -6}, {90, -7}, {93, -8}, {96, -9}, {100, -10}};

///> tgc
const int Calc2D::TGC_X[8] = {0, 15, 30, 60, 90, 130, 170, 210};    //240点 8段
const int Calc2D::TGC_CTL_DEFAULT[4] = {90,140,12,60};
#ifdef EMP_430
//A60 tgc curve
const int Calc2D::TGC_FIEXD_35C60E[TGC_DOTS] = {
//black-white
    30, 32, 35, 38, 41, 43, 46, 49,
    52, 54, 57, 60, 62, 65, 68, 70,
    73, 75, 78, 80, 83, 85, 88, 90,
    93, 95, 97, 100, 102, 104, 107, 109,
    111, 113, 116, 118, 120, 122, 124, 126,
    128, 130, 132, 134, 136, 138, 140, 142,
    144, 146, 148, 150, 151, 153, 155, 157,
    158, 160, 162, 163, 165, 166, 168, 170,
    171, 173, 174, 176, 177, 178, 180, 181,
    183, 184, 185, 186, 188, 189, 190, 191,
    193, 194, 195, 196, 197, 198, 199, 200,
    202, 203, 204, 205, 206, 206, 207, 208,
    209, 210, 211, 212, 213, 214, 214, 215,
    216, 217, 217, 218, 219, 220, 220, 221,
    222, 222, 223, 224, 224, 225, 226, 226,
    227, 227, 228, 228, 229, 229, 230, 230,
    231, 231, 232, 232, 233, 233, 234, 234,
    235, 235, 236, 236, 236, 237, 237, 238,
    238, 238, 239, 239, 239, 240, 240, 241,
    241, 241, 242, 242, 242, 243, 243, 243,
    243, 244, 244, 244, 245, 245, 245, 246,
    246, 246, 246, 247, 247, 247, 247, 248,
    248, 248, 249, 249, 249, 249, 250, 250,
    250, 250, 251, 251, 251, 251, 251, 252,
    252, 252, 252, 252, 253, 253, 253, 253,
    253, 254, 254, 254, 254, 254, 254, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 256, 256, 256, 256, 256, 256,
    256, 256, 256, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255
};
//高频微凸
const int Calc2D::TGC_FIEXD_65C15D[TGC_DOTS] = {
    65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 86, 88, 90, 92, 94, 96, 98,
    100, 102, 103, 105, 107, 109, 111, 112, 114, 116, 118, 119, 121, 123, 125,
    126, 128, 130, 131, 133, 134, 136, 138, 139, 141, 142, 144, 146, 147, 149,
    150, 152, 153, 155, 156, 158, 159, 160, 162, 163, 165, 166, 167, 169, 170,
    171, 173, 174, 175, 177, 178, 179, 180, 182, 183, 184, 185, 186, 187, 189,
    190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204,
    205, 206, 207, 208, 209, 210, 211, 212, 213, 213, 214, 215, 216, 217, 217,
    218, 219, 220, 221, 221, 222, 223, 223, 224, 225, 225, 226, 227, 227, 228,
    229, 229, 230, 230, 231, 231, 232, 233, 233, 234, 234, 235, 235, 236, 236,
    236, 237, 237, 238, 238, 239, 239, 239, 240, 240, 241, 241, 241, 242, 242,
    242, 243, 243, 243, 244, 244, 244, 244, 245, 245, 245, 246, 246, 246, 246,
    247, 247, 247, 247, 247, 248, 248, 248, 248, 248, 249, 249, 249, 249, 249,
    249, 250, 250, 250, 250, 250, 250, 251, 251, 251, 251, 251, 251, 251, 252,
    252, 252, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 254,
    254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 256, 256, 256,
    256, 256, 257, 257, 257, 257, 258, 258, 258, 258, 259, 259
};
//线阵
//#ifdef EMP_430
const int Calc2D::TGC_FIEXD_65L40E[TGC_DOTS] = {
    84, 90, 95, 100, 106, 111, 115, 115,
    115, 115, 115, 115, 115, 115, 115, 116,
    117, 121, 124, 127, 131, 134, 137, 140,
    143, 145, 148, 150, 153, 155, 158, 160,
    162, 164, 166, 168, 170, 172, 173, 175,
    177, 178, 180, 181, 182, 184, 185, 186,
    187, 189, 190, 191, 192, 192, 193, 194,
    195, 196, 197, 197, 198, 199, 199, 200,
    200, 201, 201, 202, 202, 202, 203, 203,
    204, 204, 204, 204, 205, 205, 205, 205,
    206, 206, 206, 206, 206, 206, 206, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 210,
    210, 210, 210, 210, 210, 210, 210, 211,
    211, 211, 211, 211, 211, 211, 211, 212,
    212, 212, 212, 212, 212, 212, 212, 213,
    213, 213, 213, 213, 213, 213, 214, 214,
    214, 214, 214, 214, 214, 214, 215, 215,
    215, 215, 215, 215, 215, 215, 215, 215,
    216, 216, 216, 216, 216, 216, 216, 216,
    216, 216, 216, 216, 216, 217, 217, 217,
    217, 217, 217, 217, 217, 217, 217, 217,
    217, 217, 217, 217, 217, 217, 217, 217,
    217, 217, 217, 218, 218, 218, 218, 218
};
/*
#else
const int Calc2D::TGC_FIEXD_65L40E[TGC_DOTS] = {

    44, 50, 55, 60, 66, 71, 75, 80,
    85, 89, 94, 98, 102, 106, 110, 114,
    117, 121, 124, 127, 131, 134, 137, 140,
    143, 145, 148, 150, 153, 155, 158, 160,
    162, 164, 166, 168, 170, 172, 173, 175,
    177, 178, 180, 181, 182, 184, 185, 186,
    187, 189, 190, 191, 192, 192, 193, 194,
    195, 196, 197, 197, 198, 199, 199, 200,
    200, 201, 201, 202, 202, 202, 203, 203,
    204, 204, 204, 204, 205, 205, 205, 205,
    206, 206, 206, 206, 206, 206, 206, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 210,
    210, 210, 210, 210, 210, 210, 210, 211,
    211, 211, 211, 211, 211, 211, 211, 212,
    212, 212, 212, 212, 212, 212, 212, 213,
    213, 213, 213, 213, 213, 213, 214, 214,
    214, 214, 214, 214, 214, 214, 215, 215,
    215, 215, 215, 215, 215, 215, 215, 215,
    216, 216, 216, 216, 216, 216, 216, 216,
    216, 216, 216, 216, 216, 217, 217, 217,
    217, 217, 217, 217, 217, 217, 217, 217,
    217, 217, 217, 217, 217, 217, 217, 217,
    217, 217, 217, 218, 218, 218, 218, 218
};
#endif
*/
//腔体
const int Calc2D::TGC_FIEXD_TV[TGC_DOTS] = {
    44, 50, 55, 60, 66, 71, 75, 80,
    85, 89, 94, 98, 102, 106, 110, 114,
    117, 121, 124, 127, 131, 134, 137, 140,
    143, 145, 148, 150, 153, 155, 158, 160,
    162, 164, 166, 168, 170, 172, 173, 175,
    177, 178, 180, 181, 182, 184, 185, 186,
    187, 189, 190, 191, 192, 192, 193, 194,
    195, 196, 197, 197, 198, 199, 199, 200,
    200, 201, 201, 202, 202, 202, 203, 203,
    204, 204, 204, 204, 205, 205, 205, 205,
    206, 206, 206, 206, 206, 206, 206, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 207, 207,
    207, 207, 207, 207, 207, 207, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 208, 208, 208, 208,
    208, 208, 208, 208, 209, 209, 209, 209,
    209, 209, 209, 209, 209, 209, 209, 210,
    210, 210, 210, 210, 210, 210, 210, 211,
    211, 211, 211, 211, 211, 211, 211, 212,
    212, 212, 212, 212, 212, 212, 212, 213,
    213, 213, 213, 213, 213, 213, 214, 214,
    214, 214, 214, 214, 214, 214, 215, 215,
    215, 215, 215, 215, 215, 215, 215, 215,
    216, 216, 216, 216, 216, 216, 216, 216,
    216, 216, 216, 216, 216, 217, 217, 217,
    217, 217, 217, 217, 217, 217, 217, 217,
    217, 217, 217, 217, 217, 217, 217, 217,
    217, 217, 217, 218, 218, 218, 218, 218

};
#endif
///> focus delay
const int Calc2D::APERTURE_SPAN_C[12] = {18, 24, 32, 40, 48, 56, 64, 80, 128, 156, 200, 320};
//{6, 12, 14, 18, 22, 26, 30, 34, 38, 42, 64, 320}; //{6, 12, 14, 18, 22, 26, 30, 34, 38, 42, 46, 64};    //{36, 42, 48, 56, 64, 64, 64, 64, 64, 64, 64, 64};//{6, 12, 14, 18, 22, 26, 30, 34, 38, 42, 46, 64};
const int Calc2D::APERTURE_SPAN_L[12] = {40,48, 64, 80, 96, 128,    196,    256,    400,    512,    1024,   2048};
const int Calc2D::EMIT_CH_NUM_C[20] =   {6, 8, 10, 12, 14, 16, 18,  20, 22, 24, 26, 30, 34, 38, 42, 48, 56, 64, 64, 64};
//{14,16, 18, 24, 30, 34, 38,   46, 50, 58, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};//{20,22, 24, 26, 30, 34, 38,   46, 50, 58, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};

//variable
struct Calc2D::CalcPara* Calc2D::m_calcPara = NULL;

#ifdef EMP_PROJECT
struct Calc2D::ProjectCalcPara* Calc2D::m_projectCalcPara = NULL;
#endif

//filter parament
const unsigned char Calc2D::m_filterDynamic[30720] = //480*32*2
{
#include "res/filter/Convex/dync_lp_12M_0M_025_480seg.h"
};
const unsigned char Calc2D::m_filterMatch[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg.h"
};

const unsigned char Calc2D::m_filterMatch_harmonic_1[30720] = //480*32*2
{
    //#include "../res/filter/Convex/DynMatchFilter_480seg_harmonic.h"
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_12.h"
};

const unsigned char Calc2D::m_filterMatch_harmonic_2[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_23.h"
};
const unsigned char Calc2D::m_filterMatch_harmonic_3[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_34.h"
};
const unsigned char Calc2D::m_filterMatch_harmonic_4[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_45.h"
};
const unsigned char Calc2D::m_filterMatch_harmonic_5[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_56.h"
   // #include "../res/filter/Convex/DynMatchFilter_480seg_harmonic.h"
};
const unsigned char Calc2D::m_filterMatch_harmonic_6[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_67.h"
};
const unsigned char Calc2D::m_filterMatch_harmonic_7[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_78.h"
};
const unsigned char Calc2D::m_filterMatch_harmonic_8[30720] = //480*32*2
{
#include "res/filter/Convex/DynMatchFilter_480seg_harmonic_78.h"
};
const short Calc2D::m_bp_compensation[32] =
{
36, 13, -14, -45, -38, -8, 66, 76, 5, -156, -182, 13, 368, 441, 18, -694, -913, -198, 1050, 1588, 612, -1290, -2349, -1264, 1270, 3010, 2047, -924, -3381, -2755, 265,3539
};

bool Calc2D::m_singleAperture = FALSE;

FpgaCtrl2D Calc2D::m_fpga;
//=========================================================
Calc2D::Calc2D()
{
    m_scanRange[0] = 100;
    m_scanRange[1] = 120;
    m_speed = 22;
    m_pulseWidth = 0;
    m_pulseCycle = 1;
    m_channedNum = 32;
    m_timer = 0;
    m_bandPassFilterIndex = 12;
    for(int i = 0; i < 5; i++)
    {
        m_fcDynamicFilter[i] = 2.0;
        m_dynamicDemodFd[i] = 0;
        m_fcBandPassFilter[i] = 2.0;
        m_fcBandPassFilter1[i] = 2.0;
        m_fcBandPassFilter2[i] = 5.0;
        m_fcDynamicFilterBaseFreq[i] = m_fcDynamicFilter[i];
        m_fcBandPassFilterBaseFreq[i] = m_fcBandPassFilter[i];
        m_fcBandPassFilterBaseFreq1[i] = m_fcBandPassFilter1[i];
        m_fcBandPassFilterBaseFreq2[i] = m_fcBandPassFilter2[i];
        m_dynamicDemodFdBaseFreq[i] = m_dynamicDemodFd[i];
    }
}

Calc2D::~Calc2D()
{

}

///> public
/*
 * @brief get scan range of image
 */
void Calc2D::GetScanRange(int range[2])
{
    range[0] = m_scanRange[0];
    range[1] = m_scanRange[1];
}

/*
 * @brief get valid scan range of image, different from GetScanRange when probe is linear(especially when image width
 * is out of display area).
 *
 * @para range[2][out] display scan range returned
 * @para widthInDots[in] image display width in dots, only used in linear probe
 */
void Calc2D::CalcDisplayScanRange(int range[2], int widthInDots)
{
    GetScanRange(range);
}
/*
 * @brief change foucs sum
 *
 * note  这里么没有重新计算，焦点分段（FocusChange),因为每次变焦点数目都会改变焦点位置，而焦点位置里面已经会调用FocusChange
 */
void Calc2D::CalcFocSum()
{
    //send foucs sum to imging system
    m_fpga.SendFocusSum(m_calcPara->focSum);
}

/*
 * @brief calc sample in 60Mhz and sample in 128 dots/60Mhz
 */
void Calc2D::CalcSample()
{
    const int DOTS_64 = 64;
    const int SAMPLE_NUM = 65536;
    const int FREQ = SAMPLE_FREQ; //Mhz

    int depth = m_calcPara->depth;
    int depthDots = m_calcPara->depthDots;
    double speed = m_calcPara->soundSpeed;
#ifdef EMP_355
    int sample60 = (int)(speed * SAMPLE_NUM * depthDots / 2 / FREQ / depth + 0.5);  //60MHz采样频率, 采样数据
#else
    int sample60 = (int)(INIT_SCALE * speed * SAMPLE_NUM * depthDots / 2 / FREQ / depth + 0.5); //60MHz采样频率, 采样数据
#endif
    //printf("######################calc sample: depthDots = %d", depthDots);
    int sampleAperture = (int)(speed * SAMPLE_NUM * DOTS_64 / 2 / FREQ / depth + 0.5);  //60MHz采样频率64点, 变迹

    ///> send sample128 and sample60
    m_fpga.SendSample48m(sample60);
    m_fpga.SendSampleApertureBW(sampleAperture);
    m_fpga.SendSampleApertureColor(sampleAperture);
}

/*
 * @brief calc max period
 */
int Calc2D::CalcMaxPeriod()
{
    int depth = m_calcPara->depth;
    int maxPeriod = (int)(depth * 2 / m_calcPara->soundSpeed + 0.5); //us

    if (maxPeriod < 70)
        maxPeriod = 70;

    ///> send max period(note: add 15 to cut light ring on the bottom of image)
    //在5的基础上加到10，因为相控阵边缘扫描线各阵元最大延迟差大于一般的探头。
    //取出所有fifo数据需要更长的时间。
    maxPeriod += 10;
    //solve the problem that there are sever lines in the 52mm when change the depth to 54 mm.
    //lhm 2013.10.15
    maxPeriod +=20;
    PRINTF("-------------------------depth = %d, maxPeriod = %d\n", depth, maxPeriod);
    m_fpga.SendMaxPeriod(maxPeriod);

    return maxPeriod;
}

int Calc2D::CalcTgcMaxperiod()
{
    float speed = m_calcPara->soundSpeed;
    int maxDepth = m_calcPara->depthMax;

    int maxperiod = (int)(maxDepth * 2 / speed + 0.5); //us
    m_fpga.SendTgcMaxPeriod(maxperiod);

    return maxperiod;
}
/*
 * @brief calc scan range, according scan angle
 *
 * @para levelTotal level sum of scan angle, more then 0
 * note: scan range start from 0 for each probe.
 */
void Calc2D::CalcScanRange()
{
    int lines = m_calcPara->probeLines;
    int index = m_calcPara->angleIndex;
    int indexMax = m_calcPara->angleIndexMax;

    //printf("lines:%d\n", lines);
    PRINTF("index:%d indexMax:%d\n", index, indexMax);

    ///> "/2*2" assure scanRange[0] is even, "3/4 * " is to enlarge angle
    m_scanRange[0] =(int)(lines* (3/(float)4 * index / (indexMax * 2))) / 2*2;
    //m_scanRange[0] = (lines* (3/(float)4 * index / (indexMax * 2)) / 2*2);
    m_scanRange[1] = (lines - 1) - m_scanRange[0];

    PRINTF("scanRange(%d, %d)\n", m_scanRange[0], m_scanRange[1]);
    ///send scan range
    m_fpga.SendScanrange(m_scanRange[0], m_scanRange[1]);
}
float Calc2D::CalcRealScanAngle()
{
    int lines = m_calcPara->probeLines;
    int angleTotal = 0;

    if (m_calcPara->efviCtrl)
    {
        angleTotal = m_calcPara->extendedAngle/100;
    }
    else
    {
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        {

            angleTotal = m_projectCalcPara->probeAngle / 100;
        }
        else
            angleTotal = m_calcPara->probeWidth/100;
#else
        angleTotal = m_calcPara->probeWidth/100;
#endif

  }
    float angle  = angleTotal * (m_scanRange[1] - m_scanRange[0] + 1) / lines;

    return angle;
}

void Calc2D::CalcMBP(int mbp)
{
    m_fpgaPW.SendMBP(mbp);
}

void Calc2D::CalcFreqCompoundCtrl(bool on)
{
    m_fpga.SendFreqCompund((unsigned int)on);
}

int Calc2D::CalcFPS(void)
{
    //const int tx = 20;  // us
    int maxperiod = CalcMaxPeriod();  // us
    int lines = m_scanRange[1] - m_scanRange[0] + 1;
    int mbp = m_calcPara->mbp;

    int fps = (mbp*1000000)/((maxperiod + TX) * lines);

    return fps;
}

void Calc2D::CalcCwLinesDelay(void)
{
    bool isCw = m_calcPara->modeCw;
#ifdef EMP_355
   if(ViewMain::GetInstance()->GetModeIsFlag())
#else
    if (isCw)
#endif
    {
#if 0
        PReceiveDelayCw();
#else
        PReceiveDelayCalcCW(m_delayPhase);
#endif
    }
}

void Calc2D::CalcSpaceCompoundCtrl(bool on)
{
    m_fpga.SendSpaceCompound((unsigned int)on);
}
void Calc2D::CalcFreqCompoundCoef(void)
{
    int coef0;
    int coef1;
    int coef2;
    unsigned short value[512];

    for(int i = 0; i < 512; i++)
    {
        coef0 = 8;
        coef1 = 6 + 6*(i-256)/512;
        coef2 = 6 - 6*(i-256)/512;
        value[i]   =   (coef0 + (coef1<<5) + (coef2<<10));
    }

    m_fpga.SendFreqCompoundCoef(value, 512);
}

void Calc2D::CalcWeightingEmit(int pulseWidth)
{
    unsigned char weightingValue[APERTURE_HALF*2];
    int value = 600;

    WeightingEmit(pulseWidth, value, weightingValue);
    m_fpga.SendPulseWidth(weightingValue, APERTURE_HALF*2);
}

/*
 * @breif calc weighting emit when freq compound
 * @para pulsewidth[in]
 * @para order freq number
 *       order=1; f1
 *       order=2; f2
 */
void Calc2D::CalcWeightingEmitFreqCompound(int pulseWidth, int order)
{
    unsigned char weightingValue[APERTURE_HALF*2];
    int value = 600;

    WeightingEmit(pulseWidth, value, weightingValue);
    m_fpga.SendPulseWidthFreqCompound(weightingValue, APERTURE_HALF*2, order);
}

void Calc2D::CalcWeightingEmitColor(int pulseWidth)
{
    unsigned char weightingValue[APERTURE_HALF*2];
    int value = 600;

    WeightingEmit(pulseWidth, value, weightingValue);
    m_fpga.SendColorPulseWidth(weightingValue, APERTURE_HALF*2);
}

// virtual 2D
void Calc2D::CalcEmitDelay()
{
    int focSum = m_calcPara->focSum;
    int size = sizeof(EMIT_CH_NUM_C) / sizeof(int);
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;

    int i;
    for (i = 0; i < focSum; i ++)
    {
        CEmitDelay(i, EMIT_CH_NUM_C, size, compound);
    }

    CEmitDelayPw((float)m_calcPara->focPos[0], EMIT_CH_NUM_C, size);
}
void Calc2D::CalcEmitDelayPw(float focPos)
{
    int size = sizeof(EMIT_CH_NUM_C) / sizeof(int);
    CEmitDelayPw(focPos, EMIT_CH_NUM_C, size);
}
void Calc2D::CalcEmitDelayCfm(float focPos)
{
    int size = sizeof(EMIT_CH_NUM_C) / sizeof(int);
    CEmitDelayCfm(focPos, EMIT_CH_NUM_C, size);
}
void Calc2D::CalcReceiveDelay()
{
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;
    CReceiveDelay(compound);
}

void Calc2D::CalcReceiveDelayColor()
{
    CReceiveDelayColor();
}

void Calc2D::CalcReceiveAperture()
{
    bool compound = m_calcPara->compoundSpaceCtrl || m_calcPara->compoundFreqCtrl;
#ifdef EMP_430
    CReceiveApertureU16(compound);
#else
    CReceiveAperture(compound);
#endif
    //CReceiveApertureU16(compound);
}
void Calc2D::CalcTgc(int gain, int tgcY[],AbsUpdate2D* ptrUpdate, int section)
{
    ///> calc
    if ((section == 0) || (section == 1))
        Tgc(TGC_X, gain, tgcY, TGC_CTL_DEFAULT, ptrUpdate, section);
    else
        TgcColor(TGC_X, gain, tgcY, TGC_CTL_DEFAULT, ptrUpdate, section);
}

void Calc2D::CalcTgcDigital(int gain, int section, int maxValue)
{
    //digital tgc cycle
    int depth = m_calcPara->depth;
#ifdef EMP_355
    int dots = IMG_H * INIT_SCALE;
#else
    int dots = IMG_H;
#endif
    int freq = SAMPLE_FREQ;
    float speed = SOUND_SPEED;
    unsigned short tgc;
    unsigned short value;
    const int size = 512;
    int cycle = depth * 2 * freq / speed / dots + 0.5;
    unsigned short digitalTgc[size];
    int delayClockNum = 600;    //delay 600 clocks for synchronizing focus delay
    int delayDot = ((float)delayClockNum / freq) * dots / (depth * 2 / speed) + 0.5;
    if (delayDot > dots)
        delayDot = dots;

    float scale = m_calcPara->imgScale / (float)10;
    int size1 = TGC_DOTS / scale;

    //digital tgc data
    int max;
    int min;
    if ((section == 0) || (section == 1)) //2D or M
    {
#ifdef EMP_430
        max = 300;
        min = 4;
#elif defined EMP_355
        max = 500;
        min = 4;
#else
        max = 160;
        min = 4; //64;
#endif
        for (int i = 0; i < dots; i++)
        {
            if (i < delayDot)
                tgc = m_tgcFlexibel[0];
            else
            {
#ifdef EMP_430
                int y = (int) (i -delayDot) * size1 / dots;
                if (y >= TGC_DOTS)
                    y = TGC_DOTS - 1;
                tgc = m_tgcFlexibel[y];
#else
                tgc = m_tgcFlexibel[(i - delayDot) * TGC_DOTS / dots];
#endif
            }

            value = min + (max - min) * tgc / 255;
            if (value > max)
                value = max;
            digitalTgc[i] = value;
        }

#ifdef EMP_430
        //解决tgc在调节的时候突变太大，导致有黑白条问题。
        int digitalTgcTemp[size];
        int sum = 0;

        for (int i = 0; i < 10; i++)
            digitalTgcTemp[i] = digitalTgc[i];

        for(int i = 10; i < dots -10; i++)
        {
            sum = 0;
            for (int j = -10; j < 10; j++)
                sum += digitalTgc[i + j];
            digitalTgcTemp[i] = sum/20;
        }

        for (int i = 0; i < dots - 10; i++)
        {
            digitalTgc[i] = digitalTgcTemp[i];
        }
#endif
#ifdef EMP_355
        PRINTF("tgc:\n");
        for(int i = dots; i < size; i++)
        {
            digitalTgc[i] = digitalTgc[dots - 1];
            PRINTF("(%d, %d), ", i, digitalTgc[i]);
        }
        PRINTF("\n\n");
#else
        memset((unsigned char*)(digitalTgc + dots), 0, (size - dots) * sizeof(unsigned short));
#endif
        m_fpga.SendTgcDigital2D(digitalTgc, size);
        m_fpga.SendTgcDigitalCycle2D(cycle);
    }
    else if (section == 2) //PW
    {
#if (defined(EMP_430))
      {
          max = 160;
          min = 128;

          value = 510; //min + (max - min) * gain / (float)maxValue;
          for (int i = 0; i < size; i ++)
          {
              digitalTgc[i] = value; //500
          }
      }
#else
#if (defined(EMP_340) || defined(EMP_355) || defined(EMP_322))
        max = 160;
        min = 128;
#else
        max = 128;
        min = 128;
#endif
        value = min + (max - min) * gain / (float)maxValue;
        for (int i = 0; i < size; i ++)
        {
            digitalTgc[i] = value; //500
        }
#endif
        m_fpga.SendTgcDigitalPw(digitalTgc, size);
        m_fpga.SendTgcDigitalCyclePw(cycle);
    }
    else  //cfm
    {
#if (defined(EMP_430))
        {
            int zero_num;
            zero_num = size / depth * 10;
            max = 160;
            min = 100;
            value = 510;
            for (int i = 0; i < zero_num; i ++)
                //digitalTgc[i] = 446/(zero_num - size / depth * 4) * i + 510 - zero_num * 446 / (zero_num - size / depth * 4);//64;//100;// (value - 128)/zero_num * i + 18;
                digitalTgc[i] = 64;// (value - 128)/zero_num * i + 18;
            for (int i = zero_num; i < size; i ++)
            {
                digitalTgc[i] = value;
            }
        }
#else
#if (defined(EMP_340) || defined(EMP_355) || defined(EMP_322))
        max = 160;
        min = 100;
#else
        max = 128;
        min = 128;
#endif
        value = min + (max - min) * gain / (float)maxValue;
        for (int i = 0; i < size; i ++)
        {
            digitalTgc[i] = value;
        }
#endif
        m_fpga.SendTgcDigitalCfm(digitalTgc, size);
        m_fpga.SendTgcDigitalCycleCfm(cycle);
    }
}

void Calc2D::CalcFocPos()
{
    int start = 0;

    FocChange(start);

    ///> 2D pulse number according to last focus pos
    //m_fpga.Send2DPulseNum(3);

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
#if (defined(EMP_340) || defined(EMP_322))
    if(((strcmp(para.model, "35D40J")) == 0) || ((strcmp(para.model, "30P16A")) == 0))
    {
        m_fpga.Send2DPulseNum(1);
    }
    else
    {
        m_fpga.Send2DPulseNum(2);

    }
#elif (defined(EMP_430))
    if((strcmp(para.model, "35D40J")) == 0)
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

    CalcEmitDelay();
}

bool Calc2D::CalcFocPulse()
{
    int freq = m_calcPara->freq.emit;
    int power = m_calcPara->power;
    PRINTF("++++++++++++++++CALC FOC PULSE: emit = %d\n", freq);

    return FocPulse(power, freq);
}

void Calc2D::CalcPulse(int freq)
{
    int power = m_calcPara->power;
    PRINTF("++++++++++++++++CALC FOC PULSE: emit = %d\n", freq);
    FocPulse(power, freq);
}

void Calc2D::CalcFocPulseFreqCompound(int order)
{
    int freq = m_calcPara->freq.emit;
    int power = m_calcPara->power;
    PRINTF("++++++++++++++++CALC FOC PULSE: emit = %d\n", freq);

    FocPulseFreqCompound(power, freq, order);
}

/*
 * @brief calc log curve of convex probe. This function has been deprecated.
 *
 * @para freq frequency*20 of probe
 */
void Calc2D::CalcLog()
{
    int freq = m_calcPara->freq.receive;
    float value;

    if (freq <= 55)
        value = 3.2;
    else if ((freq >= 75) && (freq <= 85))
        value = 3.0;
    else
        value = 2.8;

    Log(value);

}

/*
 * @brief calc image y start offset dot in portrait direction
 *
 * @retval y start offset of image, unit:mm
 */
float Calc2D::CalcImgStartDepth()
{
#ifdef PROJECT
 float r;
    double theta;
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

        r = m_projectCalcPara->probeR / 100.0;
         theta = m_projectCalcPara->probeAngle / 100 / 2 * PI / 180;
    }
    else
    {
        r = m_calcPara->probeR / 100;
         theta = m_calcPara->probeWidth / 100 / 2 * PI / 180;
    }
#else
    float r = m_calcPara->probeR /100;
    double theta = m_calcPara->probeWidth / 100 / 2 * PI / 180;
#endif
    float offset = r * (1 - cos(theta));

    return offset;
}

/*
 * @brief find Tis in 2D mode, if input is out of range, it will be cut to the nearest one
 *
 * @para scanAngleIndex[in] index of scan angle. range (0-7)
 * @para focPos[in] real focus position of last one, unit(mm)
 * @para freqReceive[in] (freq real emit frequency) * 20, unit (MHZ)
 * @para powerIndex[in] index of output power
 * @para depthIndex[in] index of image scale in depth
 *
 * @retval tis find in 2D tis table
 */
float Calc2D::Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int powerIndex, int depthIndex)
{
    return 0.0;
}

/*
 * @brief find Tis in CFM mode, if input is out of range, it will be cut to the nearest one
 *
 * @para[in] dopFreq (real color doppler frequency) * 20, unit(MHZ)
 * @para[in] focPos real focus position of last one, unit(mm)
 * @para[in] prfIndex index of PRF, range(0-10)
 * @para[in] powerIndex index of output power
 * @para[in] boxLineBegin begin scan line of current box
 * @para[in] boxLineEnd end scan line of current box
 *
 * @retval Tis find in CFM Tis table
 */
float Calc2D::GetCfmTis(int dopFreq, float focPos, int prfIndex, int powerIndex, int boxLineBegin, int boxLineEnd)
{
    return 0.0;
}

/*
 * @brief find Tis in spectrum mode, if input is out of range, it will be cut to the nearest one
 *
 * @para[in] dopFreq (real color doppler frequency) * 20, unit(MHZ)
 * @para[in] prfIndex index of PRF, range(0-14)
 */
float Calc2D::GetPwTis(int dopFreq, int prfIndex)
{
    return 0.0;
}

void Calc2D::CalcExtendedImagingSample(void)
{
    ExtendedImagingSampleC();
}

void Calc2D::EnterSingleAperture(bool on)
{
    m_singleAperture = on;
    CalcReceiveAperture();

}

void Calc2D::CalcExtendedImaging(bool on)
{
    if (on)
    {
       // m_fpga.SendExtendImaging(0);
        m_fpga.SendExtendImaging(1);
        CalcExtendedImagingSample();
    }
    else
    {
        m_fpga.SendExtendImaging(0);
    }
}

///> protected
/*
 * @brief calculate BW tgc(fixed and adjustable) curve, then send it to imaging system.
 *
 * @para tgcX[8] section of tgc curve(曲线分段点)
 * @para tgcY[9] [0]-[8] is value of tgc in each section. [9] is the total gain
 * @para fixedTgcControl [0]-[3] is the value to control calculating fixed tgc curve.
 */
void Calc2D::Tgc(const int tgcX[8], int gain, int tgcYKey[8], const int fixedTgcControl[4], AbsUpdate2D* ptrUpdate, int section)
{
#ifdef EMP_430
    int tgcFixedTemp[TGC_DOTS];
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    char probeType = 'C';
    probeType = para.type;
    switch(probeType)
    {
        case 'C':
        case 'T':
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;
        //低频微凸
        case 'N':
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;
       //高频微凸
        case 'n':
        case 'c':
           for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_65C15D[i];
            break;

        case 'l':
        case 'L':
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_65L40E[i];
            break;
        //腔体
        case 't':
             for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_TV[i];
            break;
        //相控阵
        case 'P':
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;

        default:
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;
    }
#endif

    int tgcFixed[TGC_DOTS];
    int tgcFlexibel[TGC_DOTS];
    int tgcY[8];
    float tempTgc;
    float temp1 = 0;
    float temp2 = 0;
    float temp;
    float tgcYAll;
    float scale = m_calcPara->imgScale / (float)10;

    const int MAX = 128;
    int A = 90; //x=0
    int B = 200; //x=7
    float k = B - A;
    float weighting = 0;;
    int x;
    float theta;
    for (x = 0; x < 8; x ++)
    {
        theta = (tgcYKey[x] / (float)255) * (PI / (float)2);
        tgcY[x] = sin(theta) * 255;
        //tgcY[x] = 181 * log10((tgcYKey[x] + 10) / (float)10);
        weighting = (k * log10((x+1)*10/(float)8) + A) / (float)MAX; //使8个滑动电位器排程竖线就能获得排程斜线的效果.
        tgcY[x] = tgcY[x] * weighting;
    }

    //fixed tgc
    int a = fixedTgcControl[0];
    int b = fixedTgcControl[1];
    int c = fixedTgcControl[2];
    int d = fixedTgcControl[3];

    ///> calculate tgc, 240 dots total.
    int k0;
    for( x = 0; x < TGC_DOTS; x++)
    {
#if 0
        // fixed tgc
        if((x/scale > c) && (x/scale<TGC_DOTS))
            k0 = ((float)(255 - b)) / a * x / scale + b;
        else
            k0 = ((float)(255 - b) * c / a + b - d) / c * x / scale + d;
#else
        if((x > c) && (x < TGC_DOTS))
            k0 = ((float)(255 - b)) / a * x + b;
        else
            k0 = ((float)(255 - b) * c / a + b - d) / c * x + d;
#endif
        if (k0>255)
          k0 = 255;

        tgcFixed[x] = k0;

        //adjustable tgc
        if(x < tgcX[0])
            tempTgc = tgcY[0];
        else if (x < (tgcX[0] + tgcX[1]) / 2)
        {
            if(tgcY[1] == tgcY[0])
            {
                tempTgc = tgcY[0];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[1] - tgcY[0]) / ((tgcX[1] - tgcX[0]) * (tgcX[1] - tgcX[0]));
                tempTgc = (x - tgcX[0]) *  (x - tgcX[0]) * temp / 200.0 + tgcY[0];
            }
        }
        else if (x  <  tgcX[1])
        {
            if(tgcY[1] == tgcY[0])
            {
                tempTgc = tgcY[0];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[1] - tgcY[0]) / ((tgcX[1] - tgcX[0]) * (tgcX[1] - tgcX[0]));
                tempTgc = (x - tgcX[1]) * (x - tgcX[1]) * temp / 200.0 + tgcY[1];
            }
        }
        else if (x < (tgcX[1] + tgcX[2]) / 2)
        {
            if(tgcY[2] == tgcY[1])
            {
                tempTgc = tgcY[1];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[2] - tgcY[1]) / ((tgcX[2] - tgcX[1]) * (tgcX[2] - tgcX[1]));
                tempTgc = (x - tgcX[1]) * (x - tgcX[1]) * temp / 200.0 + tgcY[1];
            }
        }
        else if (x  <  tgcX[2])
        {
            if(tgcY[2] == tgcY[1])
            {
                tempTgc = tgcY[1];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[2] - tgcY[1]) / ((tgcX[2] - tgcX[1]) * (tgcX[2] - tgcX[1]));
                tempTgc = (x - tgcX[2]) * (x - tgcX[2]) * temp / 200.0 + tgcY[2];
            }
        }

        else if (x  <  (tgcX[2] + tgcX[3]) / 2)
        {
            if(tgcY[3] == tgcY[2])
            {
                tempTgc = tgcY[2];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[3] - tgcY[2]) / ((tgcX[3] - tgcX[2]) * (tgcX[3] - tgcX[2]));
                tempTgc = (x - tgcX[2]) * (x - tgcX[2]) * temp / 200.0 + tgcY[2];
            }
        }
        else if (x  <  tgcX[3])
        {
            if(tgcY[3] == tgcY[2])
            {
                tempTgc = tgcY[2];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[3] - tgcY[2]) / ((tgcX[3] - tgcX[2]) * (tgcX[3] - tgcX[2]));
                tempTgc = (x - tgcX[3]) * (x - tgcX[3]) * temp / 200.0 + tgcY[3];
            }
        }
        else if (x  <  (tgcX[3] + tgcX[4]) / 2)
        {
            if(tgcY[4] == tgcY[3])
            {
                tempTgc = tgcY[3];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[4] - tgcY[3]) / ((tgcX[4] - tgcX[3]) * (tgcX[4] - tgcX[3]));
                tempTgc = (x - tgcX[3]) * (x - tgcX[3]) * temp / 200.0 + tgcY[3];
            }
        }
        else if (x  <  tgcX[4])
        {
            if(tgcY[4] == tgcY[3])
            {
                tempTgc = tgcY[3];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[4] - tgcY[3]) / ((tgcX[4] - tgcX[3]) * (tgcX[4] - tgcX[3]));
                tempTgc = (x - tgcX[4]) * (x - tgcX[4]) * temp / 200.0 + tgcY[4];
            }
        }
        else if (x  <  (tgcX[4] + tgcX[5]) / 2)
        {
            if(tgcY[5]==tgcY[4])
            {
                tempTgc = tgcY[4];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[5] - tgcY[4]) / ((tgcX[5] - tgcX[4]) * (tgcX[5] - tgcX[4]));
                tempTgc = (x - tgcX[4]) * (x - tgcX[4]) * temp / 200.0 + tgcY[4];
            }
        }
        else if (x  <  tgcX[5])
        {
            if(tgcY[5] == tgcY[4])
            {
                tempTgc = tgcY[4];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[5] - tgcY[4]) / ((tgcX[5] - tgcX[4]) * (tgcX[5] - tgcX[4]));
                tempTgc = (x - tgcX[5]) * (x - tgcX[5]) * temp / 200.0 + tgcY[5];
            }
        }
        else if (x  <  (tgcX[5] + tgcX[6]) / 2)
        {
            if(tgcY[6] == tgcY[5])
            {
                tempTgc = tgcY[5];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[6] - tgcY[5]) / ((tgcX[6] - tgcX[5]) * (tgcX[6] - tgcX[5]));
                tempTgc = (x - tgcX[5]) * (x - tgcX[5]) * temp / 200.0 + tgcY[5];
            }
        }
        else if (x  <  tgcX[6])
        {
            if(tgcY[6] == tgcY[5])
            {
                tempTgc = tgcY[5];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[6] - tgcY[5]) / ((tgcX[6] - tgcX[5]) * (tgcX[6] - tgcX[5]));
                tempTgc = (x - tgcX[6]) * (x - tgcX[6]) * temp / 200.0 + tgcY[6];
            }
        }
        else if (x  <  (tgcX[6] + tgcX[7]) / 2)
        {
            if(tgcY[7] == tgcY[6])
            {
                tempTgc = tgcY[6];
            }
            else
            {
                temp = 400.0 * (float)(tgcY[7] - tgcY[6]) / ((tgcX[7] - tgcX[6]) * (tgcX[7] - tgcX[6]));
                tempTgc = (x - tgcX[6]) * (x - tgcX[6]) * temp / 200.0 + tgcY[6];
            }
        }
        else if (x  <  tgcX[7])
        {
            if(tgcY[7] == tgcY[6])
            {
                tempTgc = tgcY[6];
            }
            else
            {
                temp = (-400.0) * (float)(tgcY[7] - tgcY[6]) / ((tgcX[7] - tgcX[6]) * (tgcX[7] - tgcX[6]));
                tempTgc = (x - tgcX[7]) * (x - tgcX[7]) * temp / 200.0 + tgcY[7];
            }
        }
        else
        {
            tempTgc =  tgcY[7];
        }

        //total adjustable tgc
        tgcYAll =  gain * tempTgc / 255.0;

        if(x == 0)
        {
            temp1 = tgcYAll;
            temp2 = tgcYAll;
        }

        tempTgc = (temp1 + temp2 + tgcYAll) / 3.0;

        temp2 = temp1;
        temp1 = tempTgc;
        if (tempTgc > 255)
            tempTgc = 255;
        else if (tempTgc < 0)
            tempTgc = 0;

        tgcFlexibel[x] = (int)tempTgc;
        m_tgcFlexibel[x] = (int)tempTgc;
    }

    // total tgc
    memset(m_tgcTotal2D, 0, TGC_DOTS+TGC_EXTRA_DOTS);
#ifdef EMP_430
    memset(m_tgcFlexibel, 0, TGC_DOTS+TGC_EXTRA_DOTS);
#endif
    unsigned int tempTotal = 0;
    int size = TGC_DOTS / scale;
    int y;
    int lastTgc;

#ifdef EMP_430
    int maxDepth = m_calcPara->depthMax;

    //for(x = 0; x < size - 2; x++)
    for(x = 0; x < size; x++)
    {
        //convex probe 只对凸阵探头做该处理
        int x1 = x * maxDepth / 220.0;
        if (x1 >= TGC_DOTS)
            x1 = TGC_DOTS -1;
        y = x * scale;
#ifdef EMP_430
        tempTotal = (unsigned int) (((float)tgcFlexibel[y] * tgcFixedTemp[x1] * 1.0)/255);
#else
        tempTotal = (unsigned int) (((float)tgcFlexibel[y] * tgcFixedTemp[x1] * 1.2)/255);
#endif
        if (tempTotal > 255)
            tempTotal = 255;
        else if(tempTotal < 0)
            tempTotal = 0;

        m_tgcTotal2D[x] = tempTotal;
        m_tgcFlexibel[x] = tempTotal;
    }
    for(x = size; x < TGC_DOTS + TGC_EXTRA_DOTS; x++)
    {
        tempTotal = 0;

        m_tgcTotal2D[x] = tempTotal;

        m_tgcFlexibel[x] = tempTotal;
    }
#else
    //printf("\n");
    lastTgc = (unsigned int)(((float)tgcFlexibel[(int)((size-1)*scale)] * tgcFixed[size-1])/255);
    for(x = 0; x < TGC_DOTS + TGC_EXTRA_DOTS; x++)
    {
        y = x * scale;
        if (x < size)
        {
            tempTotal = (unsigned int)(((float)tgcFlexibel[y] * tgcFixed[x])/255);
        }
        else
        {
            tempTotal = lastTgc;
        }

        if(tempTotal > 255)
            tempTotal = 255;
        else if(tempTotal < 0)
            tempTotal = 0;

        m_tgcTotal2D[x] = tempTotal;
        //printf("%d, ", tempTotal);
    }
#endif

    // send tgc
    if (section == 0) //2D
        m_fpga.SendTgc2D(m_tgcTotal2D, TGC_DOTS+TGC_EXTRA_DOTS);
    else if (section == 1) //M
        m_fpga.SendTgcPw(m_tgcTotal2D, TGC_DOTS+TGC_EXTRA_DOTS);

    // display tgc curve
    if (ptrUpdate != NULL)
        ptrUpdate->TgcCurve(m_tgcTotal2D, size);
}

/*
 * @brief calculate color tgc(fixed and adjustable) curve, then send it to imaging system.
 *
 * @para tgcX[8] section of tgc curve(曲线分段点)
 * @para tgcY[9] [0]-[8] is value of tgc in each section. [9] is the total gain
 * @para fixedTgcControl [0]-[3] is the value to control calculating fixed tgc curve.
 */
void Calc2D::TgcColor(const int tgcX[8], int gain, int tgcYKey[8], const int fixedTgcControl[4], AbsUpdate2D* ptrUpdate, int section)
{
#ifdef EMP_430
    int smoothNum = 20;
    int tgcFixedTemp[TGC_DOTS];

    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    char probeType = 'C';
    float adjustValue = 1.0;
    probeType = para.type;
    switch(probeType)
    {
        case 'C':
        case 'T':
            adjustValue = 1.0;//1.4;
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;
        //低频微凸
        case 'N':
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;
       //高频微凸
        case 'n':
        case 'c':
            adjustValue = 1.0;//1.6;
           for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_65C15D[i];
            break;

        case 'l':
        case 'L':
            adjustValue = 1.0;//1.4;
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_65L40E[i];
            break;
        //腔体
        case 't':
            adjustValue = 1.0;//1.5;
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_65C15D[i];
            break;
        //相控阵
        case 'P':
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;

        default:
            for (int i = 0; i < TGC_DOTS; i++)
                tgcFixedTemp[i] = TGC_FIEXD_35C60E[i];
            break;
    }
#endif
    int tgcFixed[TGC_DOTS];
    int tgcFlexibel[TGC_DOTS];
    int tgcY[8];
    float scale = m_calcPara->imgScale / (float)10;

    const int MAX = 128;
    int A = 90; //x=0
    int B = 200; //x=7
    float k = B - A;
    float weighting = 0;;
    int x;
    float theta;
    for (x = 0; x < 8; x ++)
    {
        theta = (tgcYKey[x] / (float)255) * (PI / (float)2);
        tgcY[x] = sin(theta) * 255;
#ifdef EMP_430
        tgcY[x] = 181 * log10((tgcYKey[x] +10)/ (float)10);
#else
        weighting = (k * log10((x+1)*10/(float)8) + A) / (float)MAX; //使8个滑动电位器排程竖线就能获得排程斜线的效果.
        tgcY[x] = tgcY[x] * weighting;
#endif
    }

    //fixed tgc
    int a = fixedTgcControl[0];
    int b = fixedTgcControl[1];
    int c = fixedTgcControl[2];
    int d = fixedTgcControl[3];

    ///> calculate tgc, 240 dots total.
    int k0;

    for( x = 0; x < TGC_DOTS; x++)
    {
        tgcFlexibel[x] = gain;
#ifndef EMP_430
// fixed tgc
        if(x > c)
            k0 = ((float)(255 - b)) / a * x + b;
        else
            k0 = ((float)(255 - b) * c / a + b - d) / c * x + d;

        if (k0>255)
          k0 = 255;
        tgcFixed[x] = k0;
#endif
    }

    // total tgc
    memset(m_tgcTotal2D, 0, TGC_DOTS+TGC_EXTRA_DOTS);
    unsigned int tempTotal = 0;
    int size = TGC_DOTS / scale;
    int y;
    int lastTgc;

#ifdef EMP_430
    int maxDepth = m_calcPara->depthMax;

    for(x = 0; x < size; x++)
    {
        int x1 = x * maxDepth / 220.0;
        if (x1 >= TGC_DOTS)
            x1 = TGC_DOTS - 1;

        y = x * scale;
        tempTotal = (unsigned int) (((float)tgcFlexibel[y] * tgcFixedTemp[x1] * adjustValue)/255);

        if (tempTotal > 220)
            tempTotal = 220;
        else if(tempTotal < 0)
            tempTotal = 0;

        m_tgcTotal2D[x] = tempTotal;
    }
    for(x = size; x < TGC_DOTS; x++)
    {
        tempTotal = 0;

        m_tgcTotal2D[x] = tempTotal;
    }

    unsigned char digitalTgcTemp[TGC_DOTS];
    int sum = 0;

    for (int i = 0; i < 10; i++)
        digitalTgcTemp[i] = m_tgcTotal2D[i];

    for(int i = 10; i < TGC_DOTS -10; i++)
    {
        sum = 0;
        for (int j = -10; j < 10; j++)
            sum += m_tgcTotal2D[i + j];
        digitalTgcTemp[i] = sum/20;
    }

    for (int i = TGC_DOTS - 10; i < TGC_DOTS; i++)
    {
        digitalTgcTemp[i] = m_tgcTotal2D[i];
    }

    if (section == 2)
    {
        m_fpga.SendTgcPw(digitalTgcTemp, TGC_DOTS);
    }
    else
    {
        m_fpga.SendTgcCfm(digitalTgcTemp, TGC_DOTS);
    }

    // display tgc curve
    if (ptrUpdate != NULL)
        ptrUpdate->TgcCurve(digitalTgcTemp, size);
#if 0
    printf("start:  \n\n");
    for(int i = 0; i < TGC_DOTS; i++)
    {
        printf("%d, ", digitalTgcTemp[i]);
        if(((i+1)%30) == 0)
        {
            printf("\n\n");
        }
    }
    printf("end! \n\n");
#endif
#else
    lastTgc = (unsigned int)(((float)tgcFlexibel[(int)((size-1)*scale)] * tgcFixed[size-1])/255);
    for(x = 0; x < TGC_DOTS + TGC_EXTRA_DOTS; x++)
    {
        y = x * scale;
        if (x < size)
        {
            tempTotal = (unsigned int)(((float)tgcFlexibel[y] * tgcFixed[x])/255);
        }
        else
        {
            tempTotal = lastTgc;
        }

        if(tempTotal > 255)
            tempTotal = 255;
        else if(tempTotal < 0)
            tempTotal = 0;

        m_tgcTotal2D[x] = tempTotal;
    }

   if (section == 2)
    {
        m_fpga.SendTgcPw(m_tgcTotal2D, TGC_DOTS);
    }
    else
    {
        m_fpga.SendTgcCfm(m_tgcTotal2D, TGC_DOTS);
    }
#endif

}

/*
 * @brief calc focus section and send to imging system
 *
 * @para start start offset of the first focus
 */
void Calc2D::FocChange(int start)
{
    ///> para needed by calc
    int focusSum = m_calcPara->focSum;
    int depth = m_calcPara->depth;
    int depthDots = m_calcPara->depthDots;
    int focusPos[FOC_MAX];
    for(int i = 0; i < FOC_MAX; i++)
    {
        focusPos[i] = m_calcPara->focPos[i];
    }

    ///>
    int value = depthDots + 4;// + 20; //+20 to avoid the ring at the bottom of image
    int focusBegin[4];
    int focusEnd[4];
    int offset = start;

    focusBegin[0] = 0 + offset;
    focusEnd[focusSum - 1] = value + offset;

    double scale = (float)depth / depthDots;

    for (int i = 1; i < focusSum; i ++)
    {
        focusEnd[i-1] = focusBegin[i]
            = (unsigned int)((focusPos[i] + focusPos[i-1]) / (scale * 2)) + offset;
    }

    ///> send focus position
    m_fpga.SendFocusPosition((unsigned int*)focusBegin, (unsigned int*)focusEnd);
}

/*
 * @brief calculate log curve of given value "value".
 *
 * @para value parameter to calculate log curve.
 */
void Calc2D::Log(float dynamicRange, float gain, int size, unsigned char *pLogCurveBuf)
{
    float D = 20.0 * (256.0 - 2.0) / (float)dynamicRange;
    float G = 256.0 -1- D * (log10(32767) - (float)gain / 20.0);

    double spect_max = 32767.0 * pow(10, -(gain) / 20.0);
    double spect_min = 32767.0 * pow(10, -(gain + dynamicRange) / 20.0);

    for (int i = 0; i < size; i++)
    {
        if (i <= spect_min)
        {
            pLogCurveBuf[i] = 1;
        }
        else if (i >= spect_max)
        {
            pLogCurveBuf[i] = 255;
        }
        else
        {
            pLogCurveBuf[i] = int(D * log10(i) + G + 0.5);
        }
    }
}

/*
 * @brief calculate log curve of given value "value". This is old calc.
 *
 * @para value parameter to calculate log curve.
 */
void Calc2D::Log(double value, int size, unsigned char *pLogCurveBuf)
{
    int i;
    const int COMPARE = 8192 * 2; //8192;
    int j,temp;

    for(i = 0; i < size; i++)
    {
        j=i*12/4;
        if(j<COMPARE)
        {
            temp = (unsigned char)((255 / value) * (log(j / ((COMPARE-1) / (pow(10, value) - 1)) + 1) / log(10)));
            if(temp>255)    temp = 255;
            pLogCurveBuf[i] = temp;
        }
        else
        {
            pLogCurveBuf[i] = 255;
        }
    }
}

void Calc2D::Log(double value)
{
    const int SIZE = 8192;
    unsigned char logCurve[SIZE];

    Log(value, SIZE, logCurve);

    ///> send log to imaging system
    m_fpga.SendLogAddrData(SIZE / sizeof(unsigned short), (unsigned short* )logCurve);
}

/*
 * @brief calc emit delay of convex or TV
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::CEmitDelay(int focusIndex, const int CH_NUM[], int size, bool compound)
{
    float steer = 0.0;
    float focPos = (float)m_calcPara->focPos[focusIndex];
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {
        if (focusIndex == 0)
        {
            unsigned short delay[MAX_LINES * APERTURE_HALF * 2]; ///< all emit delay
            CEmitDelayCalcExtended(focPos, CH_NUM, size, delay);
            m_fpga.SendEmitDelayEven(delay, MAX_LINES * APERTURE_HALF * 2, focusIndex, MAX_LINES*APERTURE_HALF*2);
        }
    }
    else
    {
        unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay
        CEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEven(delay, APERTURE_HALF * 2, focusIndex, APERTURE_HALF*2);
        m_fpga.SendEmitDelayOdd(delay+APERTURE_HALF*2, APERTURE_HALF * 2, focusIndex, APERTURE_HALF*2);
    }
    if (compound)
    {
        unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay
        // left
        steer = 0- m_calcPara->compoundAngle * PI / 180.0;
        CEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenSpaceCompound1(delay, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
        m_fpga.SendEmitDelayOddSpaceCompound1(delay+APERTURE_HALF*2, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);

        //right
        steer = m_calcPara->compoundAngle * PI / 180.0;
        CEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenSpaceCompound2(delay, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
        m_fpga.SendEmitDelayOddSpaceCompound2(delay+APERTURE_HALF*2, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
    }
}
/*
 * @brief calc emit delay for spectrum mode
 *
 * @para focPos focus position of pw, unit:mm
 * @para CH_NUM aperture of pw
 * @para size of CH_NUM
 */
void Calc2D::CEmitDelayPw(float focPos, const int CH_NUM[], int size)
{
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {}
    else
    {

        unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay

        float steer = 0.0;
        CEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenPW(delay, APERTURE_HALF * 2, 0, 0);
        m_fpga.SendEmitDelayOddPW(delay+APERTURE_HALF*2, APERTURE_HALF * 2, 0, 0);
#if 0
        unsigned short delay[APERTURE_HALF * 2]; ///< all emit delay
        CEmitDelayCalc(focPos, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenPW(delay, APERTURE_HALF * 2, 0, 0);
        m_fpga.SendEmitDelayOddPW(delay, APERTURE_HALF * 2, 0, 0);
#endif
    }
}

/*
 * @brief calc emit delay for color flow mode
 *
 * @para focPos focus position of pw, unit:mm
 * @para CH_NUM aperture of pw
 * @para size of CH_NUM
 */
void Calc2D::CEmitDelayCfm(float focPos, const int CH_NUM[], int size)
{
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {}
    else
    {
        unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay
        float steer = 0.0;
        CEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenCFM(delay, APERTURE_HALF * 2, 0, 0);
        m_fpga.SendEmitDelayOddCFM(delay + APERTURE_HALF*2, APERTURE_HALF * 2, 0, 0);
#if 0
        //printf("******emitdelay cfm\n");
        unsigned short delay[APERTURE_HALF * 2]; ///< all emit delay
        CEmitDelayCalc(focPos, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenCFM(delay, APERTURE_HALF * 2, 0, 0);
        m_fpga.SendEmitDelayOddCFM(delay, APERTURE_HALF * 2, 0, 0);
#endif
    }
}
/*
 * @brief calc receive delay of convex and TV, for black-white
 */
void Calc2D::CReceiveDelay(bool compound)
{
    // calc control parameter
    CCalcReceiveDelayCtrlParam(compound);

    float steer;
    //计算动态聚焦
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {
        int  delayAn[MAX_LINES * APERTURE_HALF * 2]; ///< save all MBP's an
        short delayBn[MAX_LINES * APERTURE_HALF * 2]; ///< save all MBP's Bn
        CReceiveDelayCalcExtended(delayAn, delayBn);
        m_fpga.SendReceiveDelayAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBn(delayBn, sizeof(delayBn) / sizeof(short));
    }
    else
    {
        int  delayAn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's an
        short delayBn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's Bn
        steer = 0.0;
        CReceiveDelayCalc(steer, delayAn, delayBn);
#if 0
        CReceiveDelayCalc(delayAn, delayBn);
#endif
        m_fpga.SendReceiveDelayAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBn(delayBn, sizeof(delayBn) / sizeof(short));
        m_fpga.SendReceiveDelayColorAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayColorBn(delayBn, sizeof(delayBn) / sizeof(short));
    }
    if (compound)
    {
        // compound1

        int  delayAn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's an
        short delayBn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's Bn

        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        CReceiveDelayCalc(steer, delayAn, delayBn);
        m_fpga.SendReceiveDelayAnSpaceCompound1((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBnSpaceCompound1(delayBn, sizeof(delayBn) / sizeof(short));

        // compound2
        steer = m_calcPara->compoundAngle * PI / 180.0;
        CReceiveDelayCalc(steer, delayAn, delayBn);
        m_fpga.SendReceiveDelayAnSpaceCompound2((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBnSpaceCompound2(delayBn, sizeof(delayBn) / sizeof(short));
    }
    //计算变迹孔径
#ifdef EMP_430
    CReceiveApertureU16(compound);
#else
    CReceiveAperture(compound);
#endif
    //CReceiveApertureU16(compound);
}
/*
 * @brief calc receive delay of convex and TV for color(PW, CFM, PDI)
 */
void Calc2D::CReceiveDelayColor()
{
}

void Calc2D::CReceiveAperture(bool compound)
{
    float steer = 0.0;

    // calc control parameter
    CCalcReceiveDelayCtrlParam(compound);

    // calc aperture
    unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]; ///< save all aperture value
    steer = 0.0;
    CReceiveApertureCalc(steer, delayAperture);
    m_fpga.SendReceiveAperture(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));
    m_fpga.SendReceiveApertureColor(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));

    if (compound)
    {
        // compound1
        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        CReceiveApertureCalc(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound1(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));

        // compound2
        steer = m_calcPara->compoundAngle * PI / 180.0;
        CReceiveApertureCalc(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound2(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));
    }
}

void Calc2D::CReceiveApertureU16(bool compound)
{
    float steer = 0.0;

    // calc control parameter
    CCalcReceiveDelayCtrlParam(compound);

    // calc aperture
    short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]; ///< save all aperture value
    steer = 0.0;
    CReceiveApertureCalcU16(steer, delayAperture);
    m_fpga.SendReceiveApertureU16(delayAperture, sizeof(delayAperture) / sizeof(short));
    m_fpga.SendReceiveApertureColorU16(delayAperture, sizeof(delayAperture) / sizeof(short));

    if (compound)
    {
        // compound1
        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        CReceiveApertureCalcU16(steer, delayAperture);

        m_fpga.SendReceiveApertureSpaceCompound1U16(delayAperture, sizeof(delayAperture) / sizeof(short));

        // compound2
        steer = m_calcPara->compoundAngle * PI / 180.0;
        CReceiveApertureCalcU16(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound2U16(delayAperture, sizeof(delayAperture) / sizeof(short));
    }

}

void Calc2D::CCalcReceiveDelayCtrlParam(bool compound)
{
    float steer = 0.0;
    short focStartTime;
    short wrOffset;
    short startK;

    // focus para BW
    steer = 0.0;
    CFocusParaCalc(steer, focStartTime, wrOffset, startK);
    m_fpga.SendPhaseFocStart(focStartTime);
    m_fpga.SendPhaseWrOffset(wrOffset);
    m_fpga.SendPhaseStartK(startK);

    // focus para color
    m_fpga.SendPhaseFocStartColor(focStartTime);
    m_fpga.SendPhaseWrOffsetColor(wrOffset);
    m_fpga.SendPhaseStartKColor(startK);

    if (compound)
    {
        //compound focus para
        steer = 0-m_calcPara->compoundAngle * PI / 180.0;
        CFocusParaCalc(steer, focStartTime, wrOffset, startK);
        m_fpga.SendSpaceCompound1FocStart(focStartTime);
        m_fpga.SendSpaceCompound1WrOffset(wrOffset);
        m_fpga.SendSpaceCompound1StartK(startK);

        steer = m_calcPara->compoundAngle * PI / 180.0;
        CFocusParaCalc(steer, focStartTime, wrOffset, startK);
        m_fpga.SendSpaceCompound2FocStart(focStartTime);
        m_fpga.SendSpaceCompound2WrOffset(wrOffset);
        m_fpga.SendSpaceCompound2StartK(startK);

        // new adder
        steer = m_calcPara->compoundAngle * PI / 180.0;
        unsigned int value[COMPOUND_SIZE];
        CCompoundParaAdderCalc(steer, value, COMPOUND_SIZE);
        m_fpga.SendSpaceCompoundAdder((unsigned short*)value, COMPOUND_SIZE * sizeof(int)/sizeof(short));

        // adder
        float angle = 0.0;
        int adderForLine;
        int adderForDot;
        int startSamp;
        CompoundParaCalc(steer, angle, adderForLine, adderForDot, startSamp);
        m_fpga.SendSpaceCompoundAdderLine(adderForLine);
        m_fpga.SendSpaceCompoundAdderDot(adderForDot);
        m_fpga.SendSpaceCompoundStartSamp(startSamp);
        m_fpga.SendSpaceCompound1AdderLine(adderForLine);
        m_fpga.SendSpaceCompound1AdderDot(adderForDot);
        m_fpga.SendSpaceCompound1StartSamp(startSamp);
        m_fpga.SendSpaceCompound2AdderLine(adderForLine);
        m_fpga.SendSpaceCompound2AdderDot(adderForDot);
        m_fpga.SendSpaceCompound2StartSamp(startSamp);

        // coef
        CompoundRate();
    }

}

/*
 * @brief calc emit delay of linear probe, for black-white
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::LEmitDelay(int focusIndex, const int CH_NUM[], int size, bool compound)
{
    float focPos = (float)m_calcPara->focPos[focusIndex];
    float steer = 0.0;
    unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {
        if (focusIndex == 0)
        {
            steer = 0.0;
            unsigned short delayTp[MAX_LINES * APERTURE_HALF * 2]; ///< all emit delay
            LEmitDelayCalcExtended(focPos, steer, CH_NUM, size, delayTp);
            m_fpga.SendEmitDelayEven(delayTp, MAX_LINES*APERTURE_HALF * 2, focusIndex, MAX_LINES*APERTURE_HALF*2);
        }
    }
    else
    {
        steer = m_calcPara->steerAngle * PI / 180.0;
        LEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEven(delay, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
        m_fpga.SendEmitDelayOdd(delay+APERTURE_HALF*2, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
    }

    if (compound)
    {
        // left
        steer = 0- m_calcPara->compoundAngle * PI / 180.0;
        LEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenSpaceCompound1(delay, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
        m_fpga.SendEmitDelayOddSpaceCompound1(delay+APERTURE_HALF*2, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);

        //right
        steer = m_calcPara->compoundAngle * PI / 180.0;
        LEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenSpaceCompound2(delay, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
        m_fpga.SendEmitDelayOddSpaceCompound2(delay+APERTURE_HALF*2, APERTURE_HALF*2, focusIndex, APERTURE_HALF*2);
    }
}
/*
 * @brief calc emit delay of linear probe, for spectrum mode
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::LEmitDelayPw(float focPos, const int CH_NUM[], int size)
{
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {}
    else
    {
        //printf("*********L emitdelay pw\n");
        unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay
        float steer = m_calcPara->steerAngleColor * PI / 180.0;
        LEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenPW(delay, APERTURE_HALF * 2, 0, 0);
        m_fpga.SendEmitDelayOddPW(delay+APERTURE_HALF*2, APERTURE_HALF * 2, 0, 0);
    }
}

/*
 * @brief calc emit delay of linear probe
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::LEmitDelayCfm(float focPos, const int CH_NUM[], int size)
{
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {}
    else
    {
        //printf("*********L emitdelay cfm\n");
        unsigned short delay[APERTURE_HALF * 4]; ///< all emit delay
        float steer = m_calcPara->steerAngleColor * PI / 180.0;

        LEmitDelayCalc(focPos, steer, CH_NUM, size, delay);
        m_fpga.SendEmitDelayEvenCFM(delay, APERTURE_HALF * 2, 0, 0);
        m_fpga.SendEmitDelayOddCFM(delay + APERTURE_HALF*2, APERTURE_HALF * 2, 0, 0);
    }
}

void Calc2D::LReceiveDelay(bool compound)
{
    float steer;

    //BW receive delay
    int  delayAn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's an
    short delayBn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's Bn

    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {
        steer = 0.0;
        int  delayAnTp[MAX_LINES * APERTURE_HALF * 2]; ///< save all MBP's an
        short delayBnTp[MAX_LINES * APERTURE_HALF * 2]; ///< save all MBP's Bn
        LReceiveDelayCalcExtended(steer, delayAnTp, delayBnTp);
        m_fpga.SendReceiveDelayAn((short*)delayAnTp, sizeof(delayAnTp) / sizeof(short));
        m_fpga.SendReceiveDelayBn(delayBnTp, sizeof(delayBnTp) / sizeof(short));
    }
    else
    {
        steer = m_calcPara->steerAngle * PI / 180.0;
        LReceiveDelayCalc(steer, delayAn, delayBn);
        m_fpga.SendReceiveDelayAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBn(delayBn, sizeof(delayBn) / sizeof(short));
/**
 *为了解决开扩展成像或者梯形成像时候图像左侧出现黑条的问题，发送了TP/EFVI之后，不能再发送2D/CFM/PW的时间延迟计算，这样会覆盖。
 */
        // color receive delay
        steer = m_calcPara->steerAngleColor * PI / 180.0;
        LReceiveDelayCalc(steer, delayAn, delayBn);
        m_fpga.SendReceiveDelayColorAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayColorBn(delayBn, sizeof(delayBn) / sizeof(short));
    }

    if (compound)
    {
        // compound1
        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        LReceiveDelayCalc(steer, delayAn, delayBn);
        m_fpga.SendReceiveDelayAnSpaceCompound1((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBnSpaceCompound1(delayBn, sizeof(delayBn) / sizeof(short));

        // compound2
        steer = m_calcPara->compoundAngle * PI / 180.0;
        LReceiveDelayCalc(steer, delayAn, delayBn);
        m_fpga.SendReceiveDelayAnSpaceCompound2((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayBnSpaceCompound2(delayBn, sizeof(delayBn) / sizeof(short));
    }

    // calc receive aperture
#ifdef EMP_430
    LReceiveApertureU16(compound);
#else
    LReceiveAperture(compound);
#endif
}

void Calc2D::LReceiveDelayColor(bool compound)
{
    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {}
    else
    {
        // color receive delay
        float steer = 0.0;

        //BW receive delay
        int  delayAn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's an
        short delayBn[MAX_MBP * APERTURE_HALF * 2]; ///< save all MBP's Bn

        steer = m_calcPara->steerAngleColor * PI / 180.0;
        LReceiveDelayCalc(steer, delayAn, delayBn);

        m_fpga.SendReceiveDelayColorAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
        m_fpga.SendReceiveDelayColorBn(delayBn, sizeof(delayBn) / sizeof(short));

        if (compound)
        {
            // compound1
            steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
            LReceiveDelayCalc(steer, delayAn, delayBn);
            m_fpga.SendReceiveDelayAnSpaceCompound1((short*)delayAn, sizeof(delayAn) / sizeof(short));
            m_fpga.SendReceiveDelayBnSpaceCompound1(delayBn, sizeof(delayBn) / sizeof(short));

            // compound2
            steer = m_calcPara->compoundAngle * PI / 180.0;
            LReceiveDelayCalc(steer, delayAn, delayBn);
            m_fpga.SendReceiveDelayAnSpaceCompound2((short*)delayAn, sizeof(delayAn) / sizeof(short));
            m_fpga.SendReceiveDelayBnSpaceCompound2(delayBn, sizeof(delayBn) / sizeof(short));
        }

        // calc receive aperture
#ifdef EMP_430
        LReceiveApertureU16(compound);
#else
        LReceiveAperture(compound);
#endif
    }
}

//变迹做了归一化，所以发送数据的时候必须改成16发送
void Calc2D::LReceiveApertureU16(bool compound)
{
    // calc control parameter
    LCalcReceiveDelayCtrlParam(compound);

    float steer;
    steer = m_calcPara->steerAngle * PI / 180.0;
    short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]; ///< save all aperture value
    LReceiveApertureCalcU16(steer, delayAperture);
    m_fpga.SendReceiveApertureU16(delayAperture, sizeof(delayAperture) / sizeof(short));

    steer = m_calcPara->steerAngleColor * PI / 180.0;
    LReceiveApertureCalcU16(steer, delayAperture);
    m_fpga.SendReceiveApertureColorU16(delayAperture, sizeof(delayAperture) / sizeof(short));

    if (compound)
    {
        // compound1
        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        LReceiveApertureCalcU16(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound1U16(delayAperture, sizeof(delayAperture) / sizeof(short));

        // compound2
        steer = m_calcPara->compoundAngle * PI / 180.0;
        LReceiveApertureCalcU16(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound2U16(delayAperture, sizeof(delayAperture) / sizeof(short));
    }
}

void Calc2D::LReceiveAperture(bool compound)
{
    // calc control parameter
    LCalcReceiveDelayCtrlParam(compound);

    float steer;
    steer = m_calcPara->steerAngle * PI / 180.0;
    unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]; ///< save all aperture value
    LReceiveApertureCalc(steer, delayAperture);
    m_fpga.SendReceiveAperture(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));

    steer = m_calcPara->steerAngleColor * PI / 180.0;
    LReceiveApertureCalc(steer, delayAperture);
    m_fpga.SendReceiveApertureColor(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));

    if (compound)
    {
        // compound1
        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        LReceiveApertureCalc(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound1(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));

        // compound2
        steer = m_calcPara->compoundAngle * PI / 180.0;
        LReceiveApertureCalc(steer, delayAperture);
        m_fpga.SendReceiveApertureSpaceCompound2(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));
    }
}

void Calc2D::LCalcReceiveDelayCtrlParam(bool compound)
{
    float steer = 0.0;
    short focStartTime;
    short wrOffset;
    short startK = 0;

    //focus para BW
    steer = m_calcPara->steerAngle * PI / 180.0;
    LFocusParaCalc(steer, focStartTime, wrOffset, startK);
    m_fpga.SendPhaseFocStart(focStartTime);
    m_fpga.SendPhaseWrOffset(wrOffset);
    m_fpga.SendPhaseStartK(startK);

    //focus para color
    steer = m_calcPara->steerAngleColor * PI / 180.0;
    LFocusParaCalc(steer, focStartTime, wrOffset, startK);
    m_fpga.SendPhaseFocStartColor(focStartTime);
    m_fpga.SendPhaseWrOffsetColor(wrOffset+20);
    m_fpga.SendPhaseStartKColor(startK);

    if (compound)
    {
        //compound focus para
        steer = 0-m_calcPara->compoundAngle * PI / 180.0;
        LFocusParaCalc(steer, focStartTime, wrOffset, startK);
        m_fpga.SendSpaceCompound1FocStart(focStartTime);
        m_fpga.SendSpaceCompound1WrOffset(wrOffset);
        m_fpga.SendSpaceCompound1StartK(startK);

        steer = m_calcPara->compoundAngle * PI / 180.0;
        LFocusParaCalc(steer, focStartTime, wrOffset, startK);
        m_fpga.SendSpaceCompound2FocStart(focStartTime);
        m_fpga.SendSpaceCompound2WrOffset(wrOffset);
        m_fpga.SendSpaceCompound2StartK(startK);

        // adder
        int adderForLine;
        int adderForDot;
        int startSamp;
        float angle = 0.0;

        //steer = 0.0;
        steer = m_calcPara->steerAngle * PI / 180.0;
        angle = m_calcPara->compoundAngle * PI / 180.0;
        CompoundParaCalc(steer, angle, adderForLine, adderForDot, startSamp);
        m_fpga.SendSpaceCompoundAdderLine(adderForLine);
        m_fpga.SendSpaceCompoundAdderDot(adderForDot);
        m_fpga.SendSpaceCompoundStartSamp(startSamp);

        steer = 0 - m_calcPara->compoundAngle * PI / 180.0;
        angle = 0.0;
        CompoundParaCalc(steer, angle, adderForLine, adderForDot, startSamp);
        m_fpga.SendSpaceCompound1AdderLine(adderForLine);
        m_fpga.SendSpaceCompound1AdderDot(adderForDot);
        m_fpga.SendSpaceCompound1StartSamp(startSamp);

        steer = m_calcPara->compoundAngle * PI / 180.0;
        angle = 0.0;
        CompoundParaCalc(steer, angle, adderForLine, adderForDot, startSamp);
        m_fpga.SendSpaceCompound2AdderLine(adderForLine);
        m_fpga.SendSpaceCompound2AdderDot(adderForDot);
        m_fpga.SendSpaceCompound2StartSamp(startSamp);

        // new adder
        steer = m_calcPara->compoundAngle * PI / 180.0;
        unsigned int value[COMPOUND_SIZE];
        LCompoundParaAdderCalc(steer, value, COMPOUND_SIZE);
        m_fpga.SendSpaceCompoundAdder((unsigned short*)value, COMPOUND_SIZE * sizeof(int)/sizeof(short));

        // coef
        CompoundRate();
    }
}

/*
 * @brief calc emit delay of phase probe, for black-white
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::PEmitDelay(int focusIndex, const int CH_NUM[], int size)
{
    float focPos = (float)m_calcPara->focPos[focusIndex];
    unsigned short delayEmitPhase[APERTURE_HALF * 2 * PHASELINE]; ///< all emit delay
    //PEmitDelayCalc(focPos, CH_NUM, size, delay);
    PEmitDelayCalc(focPos, C_BW, CH_NUM, size, delayEmitPhase);

/*
    //test 2012-9-25
    int i, j;
    for (i = 0; i < PHASELINE; i ++)
    {
        for (j = 0; j < 40-5; j ++)
        {
            delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
        }

        for (j = 56-5; j < 64; j ++)
        {
            delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
        }
    }
    //end test
*/
#if 0
    bool isCw = m_calcPara->modeCw;
    int cwLine = m_calcPara->cwLine;
    if (isCw)
    {
        int i, j;
        for (i = 0; i < PHASELINE; i ++)
        {
            for (j = 0; j < 32; j ++)
            {
                delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
            }

        }

       m_fpga.SendEmitDelayEvenPW(delayEmitPhase, sizeof(delayEmitPhase) / sizeof (unsigned short), APERTURE_HALF*2*PHASELINE, 1);

       //receive
       PReceiveDelayCalcCW(delayEmitPhase);

    }
    else
        // send emit delay to imaging system
        m_fpga.SendEmitDelayEvenPW(delayEmitPhase, sizeof(delayEmitPhase) / sizeof (unsigned short), focusIndex, APERTURE_HALF*2*PHASELINE);
        //m_fpga.SendEmitDelayEven(delay, sizeof(delay) / sizeof (unsigned short), focusIndex, APERTURE_HALF*2*PHASELINE);
#endif
    m_fpga.SendEmitDelayEven(delayEmitPhase, sizeof(delayEmitPhase) / sizeof (unsigned short), focusIndex, APERTURE_HALF*2*PHASELINE);
}

//test for changing send channel num
void Calc2D::ChangeChanelNum(int num)
{
  m_channedNum = num;
}

/*
 * @brief calc emit delay of phase probe, for spectrum mode
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::PEmitDelayPw(float focPos, const int CH_NUM[], int size)
{
    unsigned short delayEmitPhase[APERTURE_HALF * 2 * PHASELINE];
    //PEmitDelayCalc(focPos, CH_NUM, size, m_delayPhase);
    PEmitDelayCalc(focPos, C_PW, CH_NUM, size, m_delayPhase);

    bool isCw = m_calcPara->modeCw;
    int cwLine = m_calcPara->cwLine;
    if (isCw)
    {
        PRINTF("-cw=%d, cwLine=%d\n", isCw, cwLine);
        //emit
        memcpy(delayEmitPhase, m_delayPhase, APERTURE_HALF*2*PHASELINE*sizeof(unsigned short));
        int i;
        int j;
        for (i = 0; i < PHASELINE; i ++)
        {
#ifdef EMP_360
            for (j = 0; j <24; j ++)
            {
                delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
            }
            for (j = 32; j < 64; j ++)
            {
                delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
            }
#else
            for (j = 0; j <32/*(64 - m_channedNum)*//*40+8*/; j ++)
            {
                delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
            }
            for (j = 56+8; j < 64; j ++)
            {
                delayEmitPhase[i*APERTURE_HALF*2 + j] = 0;
            }
#endif

#if 0
            if (i == cwLine)
            {
                PRINTF("cwLine = %d\n", cwLine);
                for (j = 0; j < APERTURE_HALF*2; j ++)
                {
                    PRINTF("[%d] = %d\n", j, delayEmitPhase[i*APERTURE_HALF*2+j]);
                }
            }
#endif
        }
        m_fpga.SendEmitDelayEvenPW(delayEmitPhase, sizeof(delayEmitPhase) / sizeof (unsigned short), APERTURE_HALF*2*PHASELINE, 1);

#if 0
        //send ph value delay
        PReceiveDelayCw();
#else
        //receive
        PReceiveDelayCalcCW(m_delayPhase);
#endif
    }
    else
    {
        m_fpga.SendEmitDelayEvenPW(m_delayPhase, sizeof(m_delayPhase) / sizeof (unsigned short), APERTURE_HALF*2*PHASELINE, 1);
    }

}

/*
 * @brief calc emit delay of phase probe
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 */
void Calc2D::PEmitDelayCfm(float focPos, const int CH_NUM[], int size)
{
    unsigned short delay[APERTURE_HALF * 2 * PHASELINE]; ///< all emit delay
    PEmitDelayCalc(focPos, C_CFM, CH_NUM, size, delay);
    m_fpga.SendEmitDelayEvenCFM(delay, sizeof(delay) / sizeof (unsigned short), APERTURE_HALF*2*PHASELINE, 1);
}

void Calc2D::PReceiveDelay(bool compound)
{
    //计算动态聚焦
    int  delayAn[APERTURE_HALF * 2 * PHASELINE]; ///< save all MBP's an
    short delayBn[APERTURE_HALF * 2 * PHASELINE]; ///< save all MBP's Bn
    PReceiveDelayCalc(delayAn, delayBn);
    m_fpga.SendReceiveDelayAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
    m_fpga.SendReceiveDelayBn(delayBn, sizeof(delayBn) / sizeof(short));

    // FPGA都使用黑白的参数工作,彩色不需发送
    //PReceiveDelayCalc(delayAn, delayBn);
    //m_fpga.SendReceiveDelayColorAn((short*)delayAn, sizeof(delayAn) / sizeof(short));
    //m_fpga.SendReceiveDelayColorBn(delayBn, sizeof(delayBn) / sizeof(short)); //color Bn is not needed

    PReceiveAperture(compound);
}

void Calc2D::PReceiveDelayColor(void)
{
}

void Calc2D::PReceiveAperture(bool compound)
{
    // calc control parameter
    PCalcReceiveDelayCtrlParam(compound);

    // calc receive aperture
    unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]; ///< save all aperture value
    PReceiveApertureCalc(delayAperture);
    m_fpga.SendReceiveAperture(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));
    m_fpga.SendReceiveApertureColor(delayAperture, sizeof(delayAperture) / sizeof(unsigned char));
}

void Calc2D::PReceiveApertureU16(bool compound)
{
    // calc control parameter
    PCalcReceiveDelayCtrlParam(compound);

    short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]; ///< save all aperture value

    // calc receive aperture
    PReceiveApertureCalcU16(delayAperture);
    m_fpga.SendReceiveApertureU16(delayAperture, sizeof(delayAperture) / sizeof(short));
    m_fpga.SendReceiveApertureColorU16(delayAperture, sizeof(delayAperture) / sizeof(short));
}

void Calc2D::PCalcReceiveDelayCtrlParam(bool compound)
{
        const int FREQ = SAMPLE_FREQ; //mhz
     float probeAngle;
     probeAngle = m_calcPara->probeWidth / 100.0;
    float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth = m_projectCalcPara->probeAngle;
    else
        probeWidth = m_calcPara->probeWidthPhase;
#else
        probeWidth = m_calcPara->probeWidthPhase;
#endif
    int probeArray = m_calcPara->probeArray;
    float interval = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1));
    float angle = probeAngle / (float)2 * PI / 180.0;
    float speed = m_calcPara->soundSpeedTsi;

    // focus para BW
    short startK = 0;
    short wrOffset;
    float focusBegin = 10;
    short focStartTime = (int)(focusBegin * 2.0 * (float)FREQ / speed + 0.5);
    if (fabs(angle) < ZERO)
        wrOffset = 10;
    else
#ifdef EMP_355
        wrOffset = 10 + APERTURE_HALF * interval / sin(angle)  * (1.0 - cos(angle)) * 1000 / speed / (float)NS_PER_UNIT_RECIVE;
#else
       wrOffset = 10 + APERTURE_HALF * interval / sin(angle)  * (1.0 - cos(angle)) * 1000 / speed / (float)NS_PER_UNIT;
#endif
    m_fpga.SendPhaseFocStart(focStartTime);
    m_fpga.SendPhaseWrOffset(wrOffset);
    m_fpga.SendPhaseStartK(startK);

    // focus para color
    m_fpga.SendPhaseFocStartColor(focStartTime);
    m_fpga.SendPhaseWrOffsetColor(wrOffset);
    m_fpga.SendPhaseStartKColor(startK);

    if (compound)
    {
        // new adder
        float steer = 0.0;
        steer = m_calcPara->compoundAngle * PI / 180.0;
        unsigned int value[COMPOUND_SIZE];
        CCompoundParaAdderCalc(steer, value, COMPOUND_SIZE);
        m_fpga.SendSpaceCompoundAdder((unsigned short*)value, COMPOUND_SIZE * sizeof(int)/sizeof(short));

        // adder
        int adderForLine;
        int adderForDot;
        int startSamp;
        steer = 0.0;
        float angle = 0.0;
        CompoundParaCalc(steer, angle, adderForLine, adderForDot, startSamp);
        m_fpga.SendSpaceCompoundAdderLine(adderForLine);
        m_fpga.SendSpaceCompoundAdderDot(adderForDot);
        m_fpga.SendSpaceCompoundStartSamp(startSamp);
        m_fpga.SendSpaceCompound1AdderLine(adderForLine);
        m_fpga.SendSpaceCompound1AdderDot(adderForDot);
        m_fpga.SendSpaceCompound1StartSamp(startSamp);
        m_fpga.SendSpaceCompound2AdderLine(adderForLine);
        m_fpga.SendSpaceCompound2AdderDot(adderForDot);
        m_fpga.SendSpaceCompound2StartSamp(startSamp);

        // coef
        CompoundRate();
    }

}

/*
 * @brief calc width of focus pulse, when freq compound(f0)
 *
 * @para power[in] percent of output power
 * @para freqEmit[in] emit freq * 20
 *
 * @retval if focus pulse width is changed, TRUE-changed, FALSE-not changed
 */
bool Calc2D::FocPulse(int power, int freqEmit)
{
#ifdef EMP_355
    float width = (float)CLOCK_EMIT * 20 / freqEmit / 2;
#else
    float width = (float)SAMPLE_FREQ * 20 / freqEmit / 2;
#endif
    int pulseCycle = (int)(width + 0.5);
    int pulseWidth = (width * power / (float)100 + 0.5);
    //m_pulseCycle = pulseCycle;

    if (pulseCycle == 0)
        pulseCycle = 1;

    if (pulseWidth == 0)
        pulseWidth = 1;

    m_pulseCycle = pulseCycle;
    PRINTF("PULSEWIDTH ==============================emitFreq = %d soundPower = %d  cycle = %d    pulseWidth = %d\n  ", freqEmit, power, pulseCycle, pulseWidth);

    //send foc pulse
    m_fpga.SendPulseCycle(pulseCycle);

    //send foc pulse
    CalcWeightingEmit(pulseWidth);

    //send foc pulse for test
#ifdef EMP_355
    m_fpga.SendPulseWidthBak(pulseWidth);
#endif

    if (pulseWidth != m_pulseWidth)
    {
        m_pulseWidth = pulseWidth;
        return TRUE;
    }
    else
    {
        m_pulseWidth = pulseWidth;
        return FALSE;
    }
}

/*
 * @brief calc pulse freq for freq compound
 * @para power[in] percent of output power
 * @para freqEmit[in] emit freq * 20
 * @para order[in]
 *       order=1: f1
 *       order=2: f2
 */
int  Calc2D::FocPulseFreqCompound(int power, int freqEmit, int order)
{
#ifdef EMP_355
    float width = (float)CLOCK_EMIT * 20 / freqEmit / 2;
#else
    float width = (float)SAMPLE_FREQ * 20 / freqEmit / 2;
#endif
    int pulseCycle = (int)(width + 0.5);
    int pulseWidth = (width * power / (float)100 + 0.5);
    int ret = pulseCycle;

    if (pulseCycle == 0)
        pulseCycle = 1;

    if (pulseWidth == 0)
        pulseWidth = 1;

    //send foc pulse
    m_fpga.SendPulseCycleFreqCompound(pulseCycle, order);
    CalcWeightingEmitFreqCompound(pulseWidth, order);
#ifdef EMP_355
    m_fpga.SendPulseWidthFreqCompound(pulseWidth, order);
#endif

    return ret;

}

/*
 * @brief select high filter by index,  all high filters you can select is here.
 *
 * @para freqCompound[in] order of freq compound, 0:normal freq, 1:compound freq1, 2:compound freq2
 */
void Calc2D::BandPassFilterSelect(int freqCompound)
{
#if 1
    int filterAddr, segAddr;
    int freqInterval = 480/12;
    float fcCur;
    const int size = FREQ_SECTION*32* sizeof(unsigned short);
    unsigned char *ptrFilter = new unsigned char[size];

    //printf("\n");
    //printf("bandPassfilter\n");
    for(int i=0; i<FREQ_SECTION; i++)
    {
        fcCur = (float)(m_freqStep[i]*SAMPLE_FREQ)/((float)(1024*4));
#if 1
        if (fcCur > 12)
            fcCur = 12;
#else
        fcCur = 10;
#endif

        if (m_calcPara->harmonic)
        {
#if 0
            if(i < 16)
                segAddr = (12-m_fcBPFilter_1)*freqInterval;
            else if( (i <32) && (i >= 16))
                segAddr = (12-m_fcBPFilter_2)*freqInterval;
            else if((i < 48) && (i >= 32))
                segAddr = (12-m_fcBPFilter_3)*freqInterval;
            else
                segAddr = (12-m_fcBPFilter_1)*freqInterval;
#endif
            if(i < 16)
            {
                fcCur = float((m_fcBandPassFilter[1] - m_fcBandPassFilter[0]) / 15.0 * (float) i + m_fcBandPassFilter[0]);
                if (fcCur > 12)
                    fcCur = 12;
                segAddr = (12 - fcCur )*freqInterval;
            }
            else if( (i <32) && (i >= 16))
            {
                fcCur = float((m_fcBandPassFilter[2] - m_fcBandPassFilter[1])/ 15.0 * (float) i +m_fcBandPassFilter[1] - (m_fcBandPassFilter[2] - m_fcBandPassFilter[1]) * 16 / 15.0);
                if (fcCur > 12)
                    fcCur = 12;
                segAddr = (12 - fcCur)*freqInterval;
            }
            else if((i < 48) && (i >= 32))
            {
                fcCur = float((m_fcBandPassFilter[3] - m_fcBandPassFilter[2])/ 15.0 * (float)i + m_fcBandPassFilter[2] - (m_fcBandPassFilter[3] - m_fcBandPassFilter[2]) * 32 / 15.0);
                if (fcCur > 12)
                    fcCur = 12;
                segAddr = (12 - fcCur)*freqInterval;
            }
            else
            {
                fcCur = float((m_fcBandPassFilter[4] - m_fcBandPassFilter[3])/ 15.0 * (float)i + m_fcBandPassFilter[3] - (m_fcBandPassFilter[4] - m_fcBandPassFilter[3]) * 48.0 / 15.0);
                if (fcCur > 12)
                    fcCur = 12;
                segAddr = (12 - fcCur)*freqInterval;
            }
        }
        else
        {
            segAddr = (12-fcCur)*freqInterval;
        }
        filterAddr = segAddr*32*2;

        //printf("%d ", m_freqStep[i]);
        //printf("%d ", filterAddr);
        int list = m_bandPassFilterIndex / 10;
        if (m_calcPara->harmonic)
        {
            switch(list)
            {
                case 1:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_1+filterAddr, 32*2);
                    break;
                case 2:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_2+filterAddr, 32*2);
                    break;
                case 3:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_3+filterAddr, 32*2);
                    break;
                case 4:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_4+filterAddr, 32*2);
                    break;
                case 5:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_5+filterAddr, 32*2);
                    break;
                case 6:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_6+filterAddr, 32*2);
                    break;
                case 7:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_7+filterAddr, 32*2);
                    break;
                case 8:
                    memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_8+filterAddr, 32*2);
                    break;
            }
        }
        else
        {
            memcpy(ptrFilter+i*32*2, m_filterMatch+filterAddr, 32*2);
        }
    }
#if 0
    char str_filter[256];
    HintArea::GetInstance()->ClearHint();
    if(m_calcPara->harmonic)
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilter[0], m_fcBandPassFilter[1], m_fcBandPassFilter[2], m_fcBandPassFilter[3], m_fcBandPassFilter[4], m_dynamicDemodFd[0], m_dynamicDemodFd[1], m_dynamicDemodFd[2], m_dynamicDemodFd[3], m_dynamicDemodFd[4], m_fcDynamicFilter[0], m_fcDynamicFilter[1], m_fcDynamicFilter[2], m_fcDynamicFilter[3], m_fcDynamicFilter[4], m_bandPassFilterIndex);
    }
    else
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilterBaseFreq[0], m_fcBandPassFilterBaseFreq[1], m_fcBandPassFilterBaseFreq[2], m_fcBandPassFilterBaseFreq[3], m_fcBandPassFilterBaseFreq[4], m_dynamicDemodFdBaseFreq[0], m_dynamicDemodFdBaseFreq[1], m_dynamicDemodFdBaseFreq[2], m_dynamicDemodFdBaseFreq[3], m_dynamicDemodFdBaseFreq[4], m_fcDynamicFilterBaseFreq[0], m_fcDynamicFilterBaseFreq[1], m_fcDynamicFilterBaseFreq[2], m_fcDynamicFilterBaseFreq[3], m_fcDynamicFilterBaseFreq[4], m_bandPassFilterIndex);
    }
    HintArea::GetInstance()->UpdateHint(str_filter);
#endif
    //printf("\n");
    // printf("\n");
    //  printf("bandPassfilter\n");
    //   for(int i = 0; i < size; i++)
    //      printf("%d, ", ptrFilter[i]);

    ///> clear and send high filter
    int cnt = sizeof(unsigned short);
    if (m_calcPara->harmonic)
    {
        if (freqCompound == 0)
            m_fpga.SendMatchFilter((unsigned short*)m_bandPassFilter, size/cnt);
        else if (freqCompound == 1)
            m_fpga.SendMatchFilterFreqCompound1((unsigned short*)m_bandPassFilter, size/cnt);
        else if (freqCompound == 2)
            m_fpga.SendMatchFilterFreqCompound2((unsigned short*)m_bandPassFilter, size/cnt);
        else
            m_fpga.SendMatchFilter((unsigned short*)m_bandPassFilter, size/cnt);
    }
    else
    {
        if (freqCompound == 0)
            m_fpga.SendMatchFilter((unsigned short*)ptrFilter, size/cnt);
        else if (freqCompound == 1)
            m_fpga.SendMatchFilterFreqCompound1((unsigned short*)ptrFilter, size/cnt);
        else if (freqCompound == 2)
            m_fpga.SendMatchFilterFreqCompound2((unsigned short*)ptrFilter, size/cnt);
        else
            m_fpga.SendMatchFilter((unsigned short*)ptrFilter, size/cnt);
    }
#endif
}

void Calc2D::BandPassFilterSelect_test(float freq[5], int freqDepth[5], int freqCompound)
{
    int filterAddr, segAddr;
    int temp;
    int depth = m_calcPara->depth;
    int freqInterval = 480/12;
    float fcCur;
    const int size = FREQ_SECTION*32* sizeof(unsigned short);
    unsigned char *ptrFilter = new unsigned char[size];
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            m_fcBandPassFilter[i] = freq[i];
            m_depthBandPassFilter[i] = freqDepth[i];
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            m_fcBandPassFilterBaseFreq[i] = freq[i];
            m_depthBandPassFilter[i] = freqDepth[i];
        }
    }

    for(int i=0; i<FREQ_SECTION; i++)
    {
        fcCur = (float)(m_freqStep[i]*SAMPLE_FREQ)/((float)(1024*4));
        if (fcCur > 12)
            fcCur = 12;

        temp = i * depth / FREQ_SECTION;
        if(m_calcPara->harmonic)
        {
            if (temp < freqDepth[0])
                fcCur = m_fcBandPassFilter[0];
            else if (temp < freqDepth[1])
                fcCur = m_fcBandPassFilter[0] - (temp-freqDepth[0])*(m_fcBandPassFilter[0]-m_fcBandPassFilter[1])/(freqDepth[1]-freqDepth[0]);
            else if (temp <freqDepth[2])
                fcCur = m_fcBandPassFilter[1] - (temp-freqDepth[1])*(m_fcBandPassFilter[1]-m_fcBandPassFilter[2])/(freqDepth[2]-freqDepth[1]);
            else if (temp <freqDepth[3])
                fcCur = m_fcBandPassFilter[2] - (temp-freqDepth[2])*(m_fcBandPassFilter[2]-m_fcBandPassFilter[3])/(freqDepth[3]-freqDepth[2]);
            else if (temp <freqDepth[4])
                fcCur = m_fcBandPassFilter[3] - (temp-freqDepth[3])*(m_fcBandPassFilter[3]-m_fcBandPassFilter[4])/(freqDepth[4]-freqDepth[3]);
            else
                fcCur = m_fcBandPassFilter[4];
        }
        else
        {
            if (temp < freqDepth[0])
                fcCur = m_fcBandPassFilterBaseFreq[0];
            else if (temp < freqDepth[1])
                fcCur = m_fcBandPassFilterBaseFreq[0] - (temp-freqDepth[0])*(m_fcBandPassFilterBaseFreq[0]-m_fcBandPassFilterBaseFreq[1])/(freqDepth[1]-freqDepth[0]);
            else if (temp <freqDepth[2])
                fcCur = m_fcBandPassFilterBaseFreq[1] - (temp-freqDepth[1])*(m_fcBandPassFilterBaseFreq[1]-m_fcBandPassFilterBaseFreq[2])/(freqDepth[2]-freqDepth[1]);
            else if (temp <freqDepth[3])
                fcCur = m_fcBandPassFilterBaseFreq[2] - (temp-freqDepth[2])*(m_fcBandPassFilterBaseFreq[2]-m_fcBandPassFilterBaseFreq[3])/(freqDepth[3]-freqDepth[2]);
            else if (temp <freqDepth[4])
                fcCur = m_fcBandPassFilterBaseFreq[3] - (temp-freqDepth[3])*(m_fcBandPassFilterBaseFreq[3]-m_fcBandPassFilterBaseFreq[4])/(freqDepth[4]-freqDepth[3]);
            else
                fcCur = m_fcBandPassFilterBaseFreq[4];
        }

        if (fcCur > 12)
            fcCur = 12;
        segAddr = (12 - fcCur)*freqInterval;

        filterAddr = segAddr*32*2;

        int list = m_bandPassFilterIndex / 10;
        //  printf("list:%d\n", list);
       // if (m_calcPara->harmonic)
       // {
        switch(list)
        {
            case 1:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_1+filterAddr, 32*2);
                break;
            case 2:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_2+filterAddr, 32*2);
                break;
            case 3:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_3+filterAddr, 32*2);
                break;
            case 4:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_4+filterAddr, 32*2);
                break;
            case 5:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_5+filterAddr, 32*2);
                break;
            case 6:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_6+filterAddr, 32*2);
                break;
            case 7:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_7+filterAddr, 32*2);
                break;
            case 8:
                memcpy(m_bandPassFilter+i*32*2, m_filterMatch_harmonic_8+filterAddr, 32*2);
                break;
        }
                //   }
                //   else
                //   {
                //memcpy(ptrFilter+i*32*2, m_filterMatch+filterAddr, 32*2);
                //   }
    }
#if 0
    char str_filter[256];
    HintArea::GetInstance()->ClearHint();
    if(m_calcPara->harmonic)
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  ===fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilter[0], m_fcBandPassFilter[1], m_fcBandPassFilter[2], m_fcBandPassFilter[3], m_fcBandPassFilter[4], m_dynamicDemodFd[0], m_dynamicDemodFd[1], m_dynamicDemodFd[2], m_dynamicDemodFd[3], m_dynamicDemodFd[4], m_fcDynamicFilter[0], m_fcDynamicFilter[1], m_fcDynamicFilter[2], m_fcDynamicFilter[3], m_fcDynamicFilter[4], m_bandPassFilterIndex);
    }
    else
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilterBaseFreq[0], m_fcBandPassFilterBaseFreq[1], m_fcBandPassFilterBaseFreq[2], m_fcBandPassFilterBaseFreq[3], m_fcBandPassFilterBaseFreq[4], m_dynamicDemodFdBaseFreq[0], m_dynamicDemodFdBaseFreq[1], m_dynamicDemodFdBaseFreq[2], m_dynamicDemodFdBaseFreq[3], m_dynamicDemodFdBaseFreq[4], m_fcDynamicFilterBaseFreq[0], m_fcDynamicFilterBaseFreq[1], m_fcDynamicFilterBaseFreq[2], m_fcDynamicFilterBaseFreq[3], m_fcDynamicFilterBaseFreq[4], m_bandPassFilterIndex);
    }
    HintArea::GetInstance()->UpdateHint(str_filter);
#endif
    ///> clear and send high filter
    int cnt = sizeof(unsigned short);
    if (freqCompound == 0)
        m_fpga.SendMatchFilter((unsigned short*)m_bandPassFilter, size/cnt);
    else if (freqCompound == 1)
        m_fpga.SendMatchFilterFreqCompound1((unsigned short*)m_bandPassFilter, size/cnt);
    else if (freqCompound == 2)
        m_fpga.SendMatchFilterFreqCompound2((unsigned short*)m_bandPassFilter, size/cnt);
    else
        m_fpga.SendMatchFilter((unsigned short*)m_bandPassFilter, size/cnt);
}

void Calc2D::BandPassFilterSelect_test(float freq1[5], float freq2[5], int freqDepth[5], int freqCompound)
{
    const int size = FREQ_SECTION * 32;
    unsigned short *ptrFilter = new unsigned short[size];
    memset(ptrFilter, 0, sizeof(ptrFilter));
#ifdef EMP_355
    float fs = SAMPLE_FREQ;
#else
    float fs = 60;
#endif
    int order = 62;
    int windowType = 0;

    int depth = m_calcPara->depth;
    int i, temp;
    float w1, w2;

    float bandPassFreq1[5];
    float bandPassFreq2[5];

    for(i = 0; i < 5; i++)
    {
        if(m_calcPara->harmonic)
        {
            m_fcBandPassFilter1[i] = freq1[i];
            m_fcBandPassFilter2[i] = freq2[i];
        }
        else
        {
            m_fcBandPassFilterBaseFreq1[i] = freq1[i];
            m_fcBandPassFilterBaseFreq2[i] = freq2[i];
        }
        m_depthBandPassFilter[i] = freqDepth[i];
        bandPassFreq1[i] = freq1[i];
        bandPassFreq2[i] = freq2[i];
    }

    for(i = 0; i < FREQ_SECTION; i++)
    {

        temp = i * depth / FREQ_SECTION;
        if (temp < freqDepth[0])
        {
            w1 = bandPassFreq1[0];
            w2 = bandPassFreq2[0];
        }
        else if (temp < freqDepth[1])
        {
            w1 = bandPassFreq1[0] - (temp-freqDepth[0])*(bandPassFreq1[0]-bandPassFreq1[1])/(freqDepth[1]-freqDepth[0]);
            w2 = bandPassFreq2[0] - (temp-freqDepth[0])*(bandPassFreq2[0]-bandPassFreq2[1])/(freqDepth[1]-freqDepth[0]);
        }
        else if (temp <freqDepth[2])
        {
            w1 = bandPassFreq1[1] - (temp-freqDepth[1])*(bandPassFreq1[1]-bandPassFreq1[2])/(freqDepth[2]-freqDepth[1]);
            w2 = bandPassFreq2[1] - (temp-freqDepth[1])*(bandPassFreq2[1]-bandPassFreq2[2])/(freqDepth[2]-freqDepth[1]);
        }
        else if (temp <freqDepth[3])
        {
            w1 = bandPassFreq1[2] - (temp-freqDepth[2])*(bandPassFreq1[2]-bandPassFreq1[3])/(freqDepth[3]-freqDepth[2]);
            w2 = bandPassFreq2[2] - (temp-freqDepth[2])*(bandPassFreq2[2]-bandPassFreq2[3])/(freqDepth[3]-freqDepth[2]);
        }
        else if (temp <freqDepth[4])
        {
            w1 = bandPassFreq1[3] - (temp-freqDepth[3])*(bandPassFreq1[3]-bandPassFreq1[4])/(freqDepth[4]-freqDepth[3]);
            w2 = bandPassFreq2[3] - (temp-freqDepth[3])*(bandPassFreq2[3]-bandPassFreq2[4])/(freqDepth[4]-freqDepth[3]);
        }
        else
        {
            w1 = bandPassFreq1[4];
            w2 = bandPassFreq2[4];
        }

        BandPassFilterFunction(ptrFilter + i*32, w1, w2, order, fs, windowType);
        //printf("--%d, w1 = %.2f, w2 = %.2f\n", i, w1, w2);
    }

    ///> clear and send high filter
    if (freqCompound == 0)
        m_fpga.SendMatchFilter(ptrFilter, size);
    else if (freqCompound == 1)
        m_fpga.SendMatchFilterFreqCompound1(ptrFilter, size);
    else if (freqCompound == 2)
        m_fpga.SendMatchFilterFreqCompound2(ptrFilter, size);
    else
        m_fpga.SendMatchFilter(ptrFilter, size);

        delete []ptrFilter;
}

void BandPassFilterFunction(unsigned short* h, float w0_in, float w1_in, int order, float fs, int windowType)
{
    int i, M;
    float w0, w1;
    M = order + 1;
    w0 = w0_in / (fs / 2) * PI;
    w1 = w1_in / (fs / 2) * PI;

    float *hd = new float[M];
    memset(hd, 0, sizeof(hd));
    float *hd0 = new float[M];
    memset(hd0, 0, sizeof(hd0));
    float *hd1 = new float[M];
    memset(hd, 0, sizeof(hd1));

    float *w_win = new float[M];
    memset(w_win, 0, sizeof(w_win));
    float *coefVal = new float[M];
    memset(coefVal, 0, sizeof(coefVal));

    Ideal_lowPassFilterFunc(hd0, w0, M);
    Ideal_lowPassFilterFunc(hd1, w1, M);

    switch(windowType)
    {
        case 0:
            HammingWindowFunc(w_win, M);
            break;
        case 1:
            HammingWindowFunc(w_win, M);
            break;
        case 2:
            HammingWindowFunc(w_win, M);
            break;
        default:
            HammingWindowFunc(w_win, M);
            break;
    }

    for (i = 0; i < M; i++)
    {
        coefVal[i] = (hd1[i] - hd0[i]) * w_win[i];
    }

    float a[M], b[M];
    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(b));
    float H_max = 0;

    if (M%2)
    {
        H_max = 0.0;
        for(i = 0; i <= (M-1)/2; i++)
        {
            if(i == 0)
                a[i] = coefVal[(M-1)/2];
            else
                a[i] = 2 * coefVal[(M-1)/2-i];

            H_max = H_max + a[i] * cos((w0+w1)/2*i);
        }

        for (i = 0; i < (M+1)/2; i++)
        {
            h[i] = (unsigned short)(coefVal[i] / H_max * 8192);
        }
    }
    else
    {
        H_max = 0.0;
        for(i = 1; i <= M/2; i++)
        {
            b[i-1] = 2 * coefVal[M/2-i];
            H_max = H_max + b[i-1] * cos((w0+w1)/2*(i-1.0/2));
        }

        for (i = 0; i < (M+1)/2; i++)
        {
            h[i] = (unsigned short)(coefVal[i] / H_max * 8192);
        }
    }

    delete []hd;
    delete []hd0;
    delete []hd1;
    delete []w_win;
    delete []coefVal;

}

void Calc2D::LowPassFilterFunction(unsigned short* coef, float wc_in, int order, float fs, int windowType)
{
    int M, i;
    float wc;
    M = order + 1;
    wc = wc_in / (fs / 2) * PI;

    float *hd = new float[M];
    memset(hd, 0, sizeof(hd));
    float *w_win = new float[M];
    memset(w_win, 0, sizeof(hd));

    float *coefVal = new float[M];
    memset(coefVal, 0, sizeof(coefVal));

    Ideal_lowPassFilterFunc(hd, wc, M);

    switch(windowType)
    {
        case 0:
            HammingWindowFunc(w_win, M);
            break;
        case 1:
            HammingWindowFunc(w_win, M);
            break;
        case 2:
            HammingWindowFunc(w_win, M);
            break;
        default:
            HammingWindowFunc(w_win, M);
            break;
    }

    float sum = 0.0;
    for(i = 0; i < M; i++)
    {
        coefVal[i] = hd[i] * w_win[i];
        sum = sum + coefVal[i];
    }

    for(i = 0; i < (M+1)/2; i++)
    {
        coef[i] = (unsigned short)(coefVal[i] / sum * 8192) ;
    }

    delete []hd;
    delete []w_win;
    delete []coefVal;
}

void Ideal_lowPassFilterFunc(float *hd, float wc, int M)
{
    int i;
    float fc;
    float tmp, alfa;
    fc = wc / PI;
    alfa = (M -1) / 2.0;

    for (i = 0; i < M; i++)
    {
        tmp = fc * (i - alfa);
        if (abs(tmp) < eps)
            tmp = 1;
        else
            tmp = sin(tmp * PI) / (tmp * PI);

        hd[i] = fc * tmp;
    }
}

void HammingWindowFunc(float *y, int L)
{
    int i;
    for(i = 0; i < L; i++)
    {
        y[i] = (0.54 - 0.46 * cos(2*PI*i/(L-1)));
    }
}

void Calc2D::ChangeFreqBPFilter_1(float freq)
{
    m_fcBPFilter_1 = freq;
    BandPassFilterSelect(0);
}
void Calc2D::ChangeFreqBPFilter_2(float freq)
{
    m_fcBPFilter_2 = freq;
    BandPassFilterSelect(0);
}

void Calc2D::ChangeFreqBPFilter_3(float freq)
{
    m_fcBPFilter_3 = freq;
    BandPassFilterSelect(0);
}

void Calc2D::ChangeFreqBPFilter_4(float freq)
{
    m_fcBPFilter_4 = freq;
    BandPassFilterSelect(0);
}

void Calc2D::ChangeFreqBPFilter_5(float freq)
{
    m_fcBPFilter_5 = freq;
    BandPassFilterSelect(0);
}

void Calc2D::ChangeFreqBandPassFilter(float data, int segment)
{
    m_fcBandPassFilter[segment - 1] = data;
    BandPassFilterSelect_test(m_fcBandPassFilter, m_depthBandPassFilter, 0);
}

void Calc2D::DefaultFreqBPFilter(const int fc[5])
{
    m_fcBPFilter_1 = float (m_calcPara->freq.emit) / 10.0;
    //printf("calc2D:freq.emit:%d\n", m_calcPara->freq.emit);
    m_fcBPFilter_2 = m_fcBPFilter_1;
    m_fcBPFilter_3 = m_fcBPFilter_1;
    m_fcBPFilter_4 = m_fcBPFilter_1;
    m_fcBPFilter_5 = m_fcBPFilter_1;
#if 1
    for(int i = 0; i < 5; i++)
    {
        m_fcBandPassFilter[i] = float (fc[i] / 10.0);
       // printf("i:%d fc:%d\n", i, fc[i]);
    }
#endif
    BandPassFilterSelect_test(m_fcBandPassFilter, m_depthBandPassFilter, 0);
}

//----------------optimization test-----------------------------
void Calc2D::DefaultFreqBPFilter(const int fc1[5], const int fc2[5])
{
    for(int i = 0; i < 5; i++)
    {
        m_fcBandPassFilter1[i] = float (fc1[i] / 10.0);
        m_fcBandPassFilter2[i] = float (fc2[i] / 10.0);
    }
    BandPassFilterSelect_test(m_fcBandPassFilter1, m_fcBandPassFilter2, m_depthBandPassFilter, 0);
}

void Calc2D::ChangeBPFilter(int num)
{
    m_bandPassFilterIndex = num * 10 + (num + 1);
    //BandPassFilterSelect_test(m_fcBandPassFilter, m_depthBandPassFilter, 0);
    if(m_calcPara->harmonic)
    {
        BandPassFilterSelect_test(m_fcBandPassFilter1, m_fcBandPassFilter2, m_depthBandPassFilter, 0);
    }
    else
    {
        BandPassFilterSelect_test(m_fcBandPassFilterBaseFreq1, m_fcBandPassFilterBaseFreq2, m_depthBandPassFilter, 0);
    }
}

void Calc2D::Valid2DImage(void)
{
    int start2D;
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if(((strcmp(para.model, "65C10J")) == 0)||(strcmp(para.model, "65C15E")==0))
        start2D = 740- 46 - 80;
    else
        start2D = 740 - 46;
    //int depth = m_calcPara->depth;
    //float speed = m_calcPara->soundSpeed;
    //int dots = 460;//m_calcPara->depthDots;
    //int value = depth * 2 * SAMPLE_FREQ / speed / (dots * INIT_SCALE);
    int startColor;
#ifdef EMP_355
    startColor = 740 - 46 - 32 + 100-70;// - 5 * value;
#else
    startColor = 740 - 46 - 32;// - 5 * value;
#endif
    m_fpga.SendValid2DImage(start2D, startColor);
}

/*
 * @brief send dynamic freq num to imaging system
 *
 * @para index number of dynamic freq
 */
void Calc2D::DynamicDemod(float freq[5], int freqDepth[5], bool enableHarmonic)
{
    int cycle;
    unsigned short freqStep[FREQ_SECTION];
    int data[5];
    int i;
    m_enableHarmonic = enableHarmonic;

    if(m_calcPara->harmonic)
    {
        for(i = 0; i < 5; i++)
        {
            m_dynamicDemodFd[i] = freq[i];
            m_depthDynamicDemod[i] = freqDepth[i];
        }
    }
    else
    {
        for(i = 0; i < 5; i++)
        {
            m_depthDynamicDemod[i] = freqDepth[i];
            m_dynamicDemodFdBaseFreq[i] = freq[i];
        }
    }

    if(m_calcPara->harmonic)
        DynamicDemodCalc(m_dynamicDemodFd, freqDepth, enableHarmonic, cycle, freqStep);
    else
        DynamicDemodCalc(m_dynamicDemodFdBaseFreq, freqDepth, enableHarmonic, cycle, freqStep);
#if 0
    char str_filter[256];
    HintArea::GetInstance()->ClearHint();
    if(m_calcPara->harmonic)
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilter[0], m_fcBandPassFilter[1], m_fcBandPassFilter[2], m_fcBandPassFilter[3], m_fcBandPassFilter[4], m_dynamicDemodFd[0], m_dynamicDemodFd[1], m_dynamicDemodFd[2], m_dynamicDemodFd[3], m_dynamicDemodFd[4], m_fcDynamicFilter[0], m_fcDynamicFilter[1], m_fcDynamicFilter[2], m_fcDynamicFilter[3], m_fcDynamicFilter[4], m_bandPassFilterIndex);
    }
    else
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilterBaseFreq[0], m_fcBandPassFilterBaseFreq[1], m_fcBandPassFilterBaseFreq[2], m_fcBandPassFilterBaseFreq[3], m_fcBandPassFilterBaseFreq[4], m_dynamicDemodFdBaseFreq[0], m_dynamicDemodFdBaseFreq[1], m_dynamicDemodFdBaseFreq[2], m_dynamicDemodFdBaseFreq[3], m_dynamicDemodFdBaseFreq[4], m_fcDynamicFilterBaseFreq[0], m_fcDynamicFilterBaseFreq[1], m_fcDynamicFilterBaseFreq[2], m_fcDynamicFilterBaseFreq[3], m_fcDynamicFilterBaseFreq[4], m_bandPassFilterIndex);
    }
    HintArea::GetInstance()->UpdateHint(str_filter);
#endif
    ///> send demod combined datas.
    m_fpga.SendDemodCombineBuf(freqStep, FREQ_SECTION);
    m_fpga.SendDemodCombine(cycle);
}

void Calc2D::DynamicDemod_test(float freq[5], int freqDepth[5], bool enableHarmonic)
{
    int cycle;
    unsigned short freqStep[FREQ_SECTION];
    int data[5];
    int i;
    m_enableHarmonic = enableHarmonic;
    if(m_calcPara->harmonic)
    {
        for(i = 0; i < 5; i++)
        {
            m_dynamicDemodFd[i] = freq[i];
            m_depthDynamicDemod[i] = freqDepth[i];
        }
    }
    else
    {
        for(i = 0; i < 5; i++)
        {
            m_depthDynamicDemod[i] = freqDepth[i];
            m_dynamicDemodFdBaseFreq[i] = freq[i];
        }
    }
    if(m_calcPara->harmonic)
        DynamicDemodCalc_test(m_dynamicDemodFd, freqDepth, enableHarmonic, cycle, freqStep);
    else
        DynamicDemodCalc_test(m_dynamicDemodFdBaseFreq, freqDepth, enableHarmonic, cycle, freqStep);
#if 0
    char str_filter[256];
    HintArea::GetInstance()->ClearHint();
    if(m_calcPara->harmonic)
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilter[0], m_fcBandPassFilter[1], m_fcBandPassFilter[2], m_fcBandPassFilter[3], m_fcBandPassFilter[4], m_dynamicDemodFd[0], m_dynamicDemodFd[1], m_dynamicDemodFd[2], m_dynamicDemodFd[3], m_dynamicDemodFd[4], m_fcDynamicFilter[0], m_fcDynamicFilter[1], m_fcDynamicFilter[2], m_fcDynamicFilter[3], m_fcDynamicFilter[4], m_bandPassFilterIndex);
    }
    else
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilterBaseFreq[0], m_fcBandPassFilterBaseFreq[1], m_fcBandPassFilterBaseFreq[2], m_fcBandPassFilterBaseFreq[3], m_fcBandPassFilterBaseFreq[4], m_dynamicDemodFdBaseFreq[0], m_dynamicDemodFdBaseFreq[1], m_dynamicDemodFdBaseFreq[2], m_dynamicDemodFdBaseFreq[3], m_dynamicDemodFdBaseFreq[4], m_fcDynamicFilterBaseFreq[0], m_fcDynamicFilterBaseFreq[1], m_fcDynamicFilterBaseFreq[2], m_fcDynamicFilterBaseFreq[3], m_fcDynamicFilterBaseFreq[4], m_bandPassFilterIndex);
    }
    HintArea::GetInstance()->UpdateHint(str_filter);
#endif
    ///> send demod combined datas.
    m_fpga.SendDemodCombineBuf(freqStep, FREQ_SECTION);
    m_fpga.SendDemodCombine(cycle);
}

void Calc2D::ChangeFdDynamicDemod_test(float data, int segment)
{
    int fd;
    m_dynamicDemodFd[segment - 1] += data;
    fd = m_dynamicDemodFd[segment - 1] * 10;
    if(fd < 0)
        m_dynamicDemodFd[segment - 1] = 0.2;

    //for(int i = 0; i < 5; i++)
    //   printf("fd[%d]:%.1f\n", i, m_dynamicDemodFd[i]);
    DynamicDemod_test(m_dynamicDemodFd, m_depthDynamicDemod, m_enableHarmonic);
}

void Calc2D::SetZeroToFdDynamicDemod_test(float data)
{
    for (int i = 0; i < 5; i++)
        m_dynamicDemodFd[i] = data;
}

void Calc2D::DynamicDemodFreqCompound(float freq[5], int freqDepth[5], bool enableHarmonic, int order)
{
    int cycle;
    unsigned short freqStep[FREQ_SECTION];
    DynamicDemodCalc(freq, freqDepth, enableHarmonic, cycle, freqStep);

    ///> send demod combined datas.
    m_fpga.SendDemodBufFreqCompound(freqStep, FREQ_SECTION, order);
}
/*
 * @brief find filter data send dynamic filter to imaging system. send (ptr) filter data from  offset, count is (size)
 *        每个频率有64段滤波参数，每段32个word，频率范围0～12Mhz分为480段可选参数库
 *
 * @para freqCompound[in] 0:normal freq, 1:compound freq1, 2:compound freq1
 * @note filter (0.25-12Mhz) is devide into 480 section, step is 0.25. depth(0-depthMax) is devide into 64 section. 63 section of detph is denoted
 *       to color doppler
 */
void Calc2D::DynamicFilter(int freqCompound)
{
    int filterAddr, segAddr;
    int freqInterval = 480/12;
    float fcCur;
    const int size = FREQ_SECTION*32* sizeof(unsigned short);
    unsigned char *ptrFilter = new unsigned char[size];

    // BW
    for(int i=0; i<FREQ_SECTION; i++)
    {
        fcCur = (float)(m_freqStep[i]*SAMPLE_FREQ)/((float)(1024*4))*2.0/4.0;
        if (fcCur < 2.0)
            fcCur = 2.0;
        if (fcCur > 12)
            fcCur = 12;
        segAddr = (12-fcCur)*freqInterval;
        filterAddr = segAddr*32*2;
        memcpy(ptrFilter+i*32*2, m_filterDynamic+filterAddr, 32*2);
    }

    ///> clear and send dynamic to imaging system
    if (freqCompound == 0)
        m_fpga.SendFilter((unsigned short*)ptrFilter, size / sizeof(unsigned short));
    else if (freqCompound == 1)
        m_fpga.SendFilterCompoundFreq1((unsigned short*)ptrFilter, size / sizeof(unsigned short));
    else if (freqCompound == 2)
        m_fpga.SendFilterCompoundFreq2((unsigned short*)ptrFilter, size / sizeof(unsigned short));
    else
        m_fpga.SendFilter((unsigned short*)ptrFilter, size / sizeof(unsigned short));

    // color(freq_section = 1)
    if (freqCompound == 0)
    {
        fcCur = 1.0;
        segAddr = (12-fcCur)*freqInterval;
        filterAddr = segAddr*32*2;
        memcpy(ptrFilter, m_filterDynamic+filterAddr, 32*2);//12MHz ~ 0MHz
        m_fpga.SendFilterColor((unsigned short*)ptrFilter, 32*2);
    }

    delete []ptrFilter;
}

/*
void Calc2D::DynamicFilter_test(float freq[5], int freqDepth[5], int freqCompound)
{
    int filterAddr, segAddr;
    int freqInterval = 480/12;
    float fcCur;
    const int size = FREQ_SECTION*32* sizeof(unsigned short);
    unsigned char *ptrFilter = new unsigned char[size];
    int temp;
    int depth = m_calcPara->depth;
    if(m_calcPara->harmonic)
    {
        for(int i = 0; i < 5; i++)
        {
            m_fcDynamicFilter[i] = freq[i];
            m_depthDynamicFilter[i] = freqDepth[i];
        }

    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            m_fcDynamicFilterBaseFreq[i] = freq[i];
            m_depthDynamicFilter[i] = freqDepth[i];
        }
    }
    // BW
    for(int i=0; i<FREQ_SECTION; i++)
    {
        //fcCur = (float)(m_freqStep[i]/2.0*SAMPLE_FREQ)/((float)(1024*4))*2.0/4.0;
        //fcCur = (float)(m_freqStep[i]*SAMPLE_FREQ)/((float)(1024*4))*2.0/4.0;
       // if (fcCur < 2.0)
        //    fcCur = 2.0;
       // if (fcCur > 12)
       //     fcCur = 12;

        temp = i * depth / FREQ_SECTION;
        if(m_calcPara->harmonic)
        {
        if (temp < freqDepth[0])
            fcCur = m_fcDynamicFilter[0];
        else if (temp < freqDepth[1])
            fcCur = m_fcDynamicFilter[0] - (temp-freqDepth[0])*(m_fcDynamicFilter[0]-m_fcDynamicFilter[1])/(freqDepth[1]-freqDepth[0]);
        else if (temp <freqDepth[2])
            fcCur = m_fcDynamicFilter[1] - (temp-freqDepth[1])*(m_fcDynamicFilter[1]-m_fcDynamicFilter[2])/(freqDepth[2]-freqDepth[1]);
        else if (temp <freqDepth[3])
            fcCur = m_fcDynamicFilter[2] - (temp-freqDepth[2])*(m_fcDynamicFilter[2]-m_fcDynamicFilter[3])/(freqDepth[3]-freqDepth[2]);
        else if (temp <freqDepth[4])
            fcCur = m_fcDynamicFilter[3] - (temp-freqDepth[3])*(m_fcDynamicFilter[3]-m_fcDynamicFilter[4])/(freqDepth[4]-freqDepth[3]);
        else
            fcCur = m_fcDynamicFilter[4];
        }
        else
        {
            if (temp < freqDepth[0])
                fcCur = m_fcDynamicFilterBaseFreq[0];
            else if (temp < freqDepth[1])
                fcCur = m_fcDynamicFilterBaseFreq[0] - (temp-freqDepth[0])*(m_fcDynamicFilterBaseFreq[0]-m_fcDynamicFilterBaseFreq[1])/(freqDepth[1]-freqDepth[0]);
            else if (temp <freqDepth[2])
                fcCur = m_fcDynamicFilterBaseFreq[1] - (temp-freqDepth[1])*(m_fcDynamicFilterBaseFreq[1]-m_fcDynamicFilterBaseFreq[2])/(freqDepth[2]-freqDepth[1]);
            else if (temp <freqDepth[3])
                fcCur = m_fcDynamicFilterBaseFreq[2] - (temp-freqDepth[2])*(m_fcDynamicFilterBaseFreq[2]-m_fcDynamicFilterBaseFreq[3])/(freqDepth[3]-freqDepth[2]);
            else if (temp <freqDepth[4])
                fcCur = m_fcDynamicFilterBaseFreq[3] - (temp-freqDepth[3])*(m_fcDynamicFilterBaseFreq[3]-m_fcDynamicFilterBaseFreq[4])/(freqDepth[4]-freqDepth[3]);
            else
                fcCur = m_fcDynamicFilterBaseFreq[4];
        }

        if (fcCur < 2.0)
            fcCur = 2.0;
        if (fcCur > 12)
            fcCur = 12;
        segAddr = (12 - fcCur)*freqInterval;

        filterAddr = segAddr*32*2;
        memcpy(ptrFilter+i*32*2, m_filterDynamic+filterAddr, 32*2);
    }
#if 0
    char str_filter[256];
    HintArea::GetInstance()->ClearHint();
    if(m_calcPara->harmonic)
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilter[0], m_fcBandPassFilter[1], m_fcBandPassFilter[2], m_fcBandPassFilter[3], m_fcBandPassFilter[4], m_dynamicDemodFd[0], m_dynamicDemodFd[1], m_dynamicDemodFd[2], m_dynamicDemodFd[3], m_dynamicDemodFd[4], m_fcDynamicFilter[0], m_fcDynamicFilter[1], m_fcDynamicFilter[2], m_fcDynamicFilter[3], m_fcDynamicFilter[4], m_bandPassFilterIndex);
    }
    else
    {
        sprintf(str_filter, "=BP (%.1f, %.1f, %.1f, %.1f, %.1f)  =fd(%.1f, %.1f, %.1f, %.1f, %.1f)  =LP (%.1f, %.1f, %.1f, %.1f, %.1f) =the %d list", m_fcBandPassFilterBaseFreq[0], m_fcBandPassFilterBaseFreq[1], m_fcBandPassFilterBaseFreq[2], m_fcBandPassFilterBaseFreq[3], m_fcBandPassFilterBaseFreq[4], m_dynamicDemodFdBaseFreq[0], m_dynamicDemodFdBaseFreq[1], m_dynamicDemodFdBaseFreq[2], m_dynamicDemodFdBaseFreq[3], m_dynamicDemodFdBaseFreq[4], m_fcDynamicFilterBaseFreq[0], m_fcDynamicFilterBaseFreq[1], m_fcDynamicFilterBaseFreq[2], m_fcDynamicFilterBaseFreq[3], m_fcDynamicFilterBaseFreq[4], m_bandPassFilterIndex);
    }
    HintArea::GetInstance()->UpdateHint(str_filter);
#endif
    ///> clear and send dynamic to imaging system
    if (freqCompound == 0)
        m_fpga.SendFilter((unsigned short*)ptrFilter, size / sizeof(unsigned short));
    else if (freqCompound == 1)
        m_fpga.SendFilterCompoundFreq1((unsigned short*)ptrFilter, size / sizeof(unsigned short));
    else if (freqCompound == 2)
        m_fpga.SendFilterCompoundFreq2((unsigned short*)ptrFilter, size / sizeof(unsigned short));
    else
        m_fpga.SendFilter((unsigned short*)ptrFilter, size / sizeof(unsigned short));

#if 0
    // color(freq_section = 1)
    if (freqCompound == 0)
    {
        fcCur = 1.0;
        segAddr = (12-fcCur)*freqInterval;
        filterAddr = segAddr*32*2;
        memcpy(ptrFilter, m_filterDynamic+filterAddr, 32*2);//12MHz ~ 0MHz
        for(int i = 0; i < 32*2; i++)
            PRINTF("%d, ", m_filterDynamic[filterAddr+i]);
        m_fpga.SendFilterColor((unsigned short*)ptrFilter, 32*2);
    }
#endif
    delete []ptrFilter;
}
*/

void Calc2D::DynamicFilter_test(float freq[5], int freqDepth[5], int freqCompound)
{
    const int size = FREQ_SECTION * 32;
    unsigned short *ptrFilter = new unsigned short[size];
    memset(ptrFilter, 0, sizeof(ptrFilter));
#ifdef EMP_355
    float fs = SAMPLE_FREQ;
#else
    float fs = 60;
#endif
    int order = 62;
    int windowType = 0;

    int i, temp;
    int depth = m_calcPara->depth;
    float wp;

#ifdef EMP_PROJECT
    int dots = IMG_H;
        int sample_freq = SAMPLE_FREQ;
        float speed = SOUND_SPEED;
        wp = sample_freq / 2.0 /(depth * 2.0 / speed * sample_freq / dots);
        float scale = 1.0;

     if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        {
                scale = m_projectCalcPara->wpScale;
        }
        wp = wp *scale;
#endif

    float lowPassFreq[5];
    if(m_calcPara->harmonic)
    {
        for(i = 0; i < 5; i++)
        {
            m_fcDynamicFilter[i] = freq[i];
            m_depthDynamicFilter[i] = freqDepth[i];
            lowPassFreq[i] = freq[i];
        }
    }
    else
    {
        for(i = 0; i < 5; i++)
        {
            m_fcDynamicFilterBaseFreq[i] = freq[i];
            m_depthDynamicFilter[i] = freqDepth[i];
            lowPassFreq[i] = freq[i];
        }
    }

    for(i = 0; i < FREQ_SECTION; i++)
    {
#ifndef EMP_PROJECT
        temp = i * depth / FREQ_SECTION;
        if (temp < freqDepth[0])
        {
            wp = lowPassFreq[0];
        }
        else if (temp < freqDepth[1])
        {
            wp = lowPassFreq[0] - (temp-freqDepth[0])*(lowPassFreq[0]-lowPassFreq[1])/(freqDepth[1]-freqDepth[0]);

        }
        else if (temp <freqDepth[2])
        {
            wp = lowPassFreq[1] - (temp-freqDepth[1])*(lowPassFreq[1]-lowPassFreq[2])/(freqDepth[2]-freqDepth[1]);
        }
        else if (temp <freqDepth[3])
        {
            wp = lowPassFreq[2] - (temp-freqDepth[2])*(lowPassFreq[2]-lowPassFreq[3])/(freqDepth[3]-freqDepth[2]);
        }
        else if (temp <freqDepth[4])
        {
            wp = lowPassFreq[3] - (temp-freqDepth[3])*(lowPassFreq[3]-lowPassFreq[4])/(freqDepth[4]-freqDepth[3]);
        }
        else
        {
            wp = lowPassFreq[4];
        }
#endif
        LowPassFilterFunction(ptrFilter + i*32, wp, order, fs, windowType);
    }

    ///> clear and send dynamic to imaging system
    if (freqCompound == 0)
        m_fpga.SendFilter(ptrFilter, size);
    else if (freqCompound == 1)
        m_fpga.SendFilterCompoundFreq1(ptrFilter, size);
    else if (freqCompound == 2)
        m_fpga.SendFilterCompoundFreq2(ptrFilter, size);
    else
        m_fpga.SendFilter(ptrFilter, size);

    delete []ptrFilter;

}

void Calc2D::ChangeFreqDynamicFilter(float data, int segment)
{
    m_fcDynamicFilter[segment - 1] = data;
    DynamicFilter_test(m_fcDynamicFilter, m_depthDynamicFilter, 0);
}

void Calc2D::DefaultFreqDynamicFilter(const int fc[5])
{
   for(int i = 0; i < 5; i++)
   {
        m_fcDynamicFilter[i] = float(fc[i] / 10.0);
       //printf("i:%d fc:%d\n", i, fc[i]);
    }
    DynamicFilter_test(m_fcDynamicFilter, m_depthDynamicFilter, 0);
}

void Calc2D::PrintPara()
{
    PRINTF("bandpassfc(%.1f, %.1f, %.1f, %.1f, %.1f)\n", m_fcBandPassFilter[0], m_fcBandPassFilter[1], m_fcBandPassFilter[2] , m_fcBandPassFilter[3], m_fcBandPassFilter[4]);
    PRINTF("\n");
    PRINTF("dynamicfc(%.1f, %.1f, %.1f, %.1f, %.1f)\n", m_fcDynamicFilter[0], m_fcDynamicFilter[1], m_fcDynamicFilter[2] , m_fcDynamicFilter[3], m_fcDynamicFilter[4]);

    PRINTF("\n");
    PRINTF("demod fd(%.1f, %.1f, %.1f, %.1f, %.1f)\n", m_dynamicDemodFd[0], m_dynamicDemodFd[1], m_dynamicDemodFd[2] , m_dynamicDemodFd[3], m_dynamicDemodFd[4]);
    PRINTF("\n");
}
void Calc2D::CalcFilterChangeDepth()
{
    if(m_calcPara->harmonic)
    {
        DynamicDemod_test(m_dynamicDemodFd, m_depthDynamicDemod, m_enableHarmonic);
        //BandPassFilterSelect_test(m_fcBandPassFilter, m_depthBandPassFilter, 0);
        BandPassFilterSelect_test(m_fcBandPassFilter1, m_fcBandPassFilter2, m_depthBandPassFilter, 0);
        DynamicFilter_test(m_fcDynamicFilter, m_depthDynamicFilter, 0);
    }
    else
    {
        DynamicDemod_test(m_dynamicDemodFdBaseFreq, m_depthDynamicDemod, m_enableHarmonic);
        //BandPassFilterSelect_test(m_fcBandPassFilterBaseFreq, m_depthBandPassFilter, 0);
        BandPassFilterSelect_test(m_fcBandPassFilterBaseFreq1, m_fcBandPassFilterBaseFreq2, m_depthBandPassFilter, 0);
        DynamicFilter_test(m_fcDynamicFilterBaseFreq, m_depthDynamicFilter, 0);
    }

    for (int i = 0; i < 5; i++)
    {
        if (m_calcPara->harmonic)
            PRINTF("--Depth Change--THI--%d, bp1 = %.1f, bp2 = %.1f, demod = %.1f, lp = %.1f, \n", i, m_fcBandPassFilter1[i], m_fcBandPassFilter2[i], m_dynamicDemodFd[i], m_fcDynamicFilter[i]);
        else
            PRINTF("--Depth Change--Base Freq--%d, bp1 = %.1f, bp2 = %.1f, demod = %.1f, lp = %.1f, \n", i, m_fcBandPassFilterBaseFreq1[i], m_fcBandPassFilterBaseFreq2[i], m_dynamicDemodFdBaseFreq[i], m_fcDynamicFilterBaseFreq[i]);
    }
}

void Calc2D::ExtendedImagingSampleC(void)
{
    const int SAMPLE_NUM = 65536;
    const int FREQ = SAMPLE_FREQ; //Mhz

    int depth = m_calcPara->depth;
    int depthDots = m_calcPara->depthDots;
    double speed = m_calcPara->soundSpeed;

    const int SIZE = 256;
    unsigned short buffer[SIZE];
#ifdef EMP_355
    unsigned short sample60 = (int)(speed * SAMPLE_NUM * depthDots / 2 / FREQ / depth + 0.5);
#else
    unsigned short sample60 = (int)(INIT_SCALE * speed * SAMPLE_NUM * depthDots / 2 / FREQ / depth + 0.5);
#endif
    for (int i = 0; i < SIZE; i ++)
    {
        buffer[i] = sample60;
    }

    m_fpga.SendExtendImagingSample(buffer, SIZE);
}
void Calc2D::ExtendedImagingSampleL(void)
{
    const int SAMPLE_NUM = 65536;
    const int FREQ = SAMPLE_FREQ;
    const int SIZE = MAX_LINES;
    int depthDots = m_calcPara->depthDots;
    int depth = m_calcPara->depth;
    double speed = m_calcPara->soundSpeed;
    float probeWidth;

#ifdef EMP_PROJECT
 if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth =(float) m_projectCalcPara->probeAngle /100.0;
    else
        probeWidth =(float) m_calcPara->probeWidth / 100.0;
#else
        probeWidth =(float) m_calcPara->probeWidth / 100.0;
#endif

    int lines = m_calcPara->probeLines;
    float angle = fabs(m_calcPara->tpSteer) * PI / 180;
    float widthHalf = probeWidth / 2.0;
    unsigned short buffer[SIZE];
    float pos2center;
    float steer;
    float oHeight = widthHalf / tan(angle);

    for (int lineNo = 0; lineNo < SIZE; lineNo++)
    {
        pos2center = 0 - widthHalf + ((MAX_LINES + lineNo - 1) % MAX_LINES) * probeWidth / (lines - 1);

        if (fabs(m_calcPara->tpSteer) == ZERO)
            steer = 0.0;
        else
            steer = atan(pos2center/oHeight);

#ifdef EMP_355
        buffer[(MAX_LINES+lineNo-1) % MAX_LINES] = (int)(cos(steer) * speed * SAMPLE_NUM * depthDots / 2 / FREQ / depth + 0.5);
#else
        buffer[(MAX_LINES+lineNo-1) % MAX_LINES] = (int)(cos(steer) * INIT_SCALE * speed * SAMPLE_NUM * depthDots / 2 / FREQ / depth + 0.5);
#endif
    }

    m_fpga.SendExtendImagingSample(buffer, SIZE);
}

int Calc2D::GetFocIndex(float focPos)
{
    int focIndex = 0;

    if (focPos <= 0.5)
        focIndex = 0;
    else if ((focPos > 0.5) && (focPos <= 1.2))
        focIndex = 1;
    else if ((focPos > 1.2) && (focPos <= 2.5))
        focIndex = 2;
    else if ((focPos > 2.5) && (focPos <= 3.5))
        focIndex = 3;
    else if ((focPos > 3.5) && (focPos <= 5.0))
        focIndex = 4;
    else if ((focPos > 5.0) && (focPos <= 6.0))
        focIndex = 5;
    else if ((focPos > 6.0) && (focPos <= 7.2))
        focIndex = 6;
    else if ((focPos > 7.2) && (focPos <= 8.5))
        focIndex = 7;
    else if ((focPos > 8.5) && (focPos <= 9.8))
        focIndex = 8;
    else if ((focPos > 9.8) && (focPos <= 11.0))
        focIndex = 9;
    else if ((focPos > 11.0) && (focPos <= 12.4))
        focIndex = 10;
    else if (focPos > 12.4)
        focIndex = 10;

    return focIndex;
}
int Calc2D::GetPowerIndex(int power)
{
    int powerIndex = (power > 1)? (power-1)/10 : 0;

    if (powerIndex > 9)
        powerIndex = 9;
    else if (powerIndex < 0)
        powerIndex = 0;

    return powerIndex;
}
/*
 * @brief get receive freq index of 2D
 *
 * @para freqReceive[in] real receive freq * 20
 */
int Calc2D::Get2DFreqReceiveIndex(int freqReceive)
{
    int freqIndex = 0;

    if (freqReceive <= 40)
        freqIndex = 0;
    else if ((freqReceive > 40) && (freqReceive <= 50))
        freqIndex = 1;
    else if ((freqReceive > 50) && (freqReceive <= 60))
        freqIndex = 2;
    else if ((freqReceive > 60) && (freqReceive <= 70))
        freqIndex = 3;
    else if ((freqReceive > 70) && (freqReceive <= 80))
        freqIndex = 4;
    else if ((freqReceive > 80) && (freqReceive <= 90))
        freqIndex = 5;
    else if ((freqReceive > 90) && (freqReceive <= 100))
        freqIndex = 6;
    else if ((freqReceive > 100) && (freqReceive <= 110))
        freqIndex = 7;
    else if ((freqReceive > 110) && (freqReceive <= 120))
        freqIndex = 8;
    else if (freqReceive > 120)
        freqIndex = 8;

    return freqIndex;
}

int Calc2D::GetColorFreqIndex(int freqColor)
{
    int freqIndex = 0;

    if (freqColor <= 54)
        freqIndex = 0;
    else if ((freqColor > 54) && (freqColor <= 66))
        freqIndex = 1;
    else if (freqColor > 66)
        freqIndex = 1;

    return freqIndex;
}
int Calc2D::GetCfmLinesIndex(int boxLineBegin, int boxLineEnd)
{
    int boxlines = boxLineEnd - boxLineBegin + 1;
    int index = 0;

    if (boxlines <= 29)
        index = 0;
    else if ((boxlines > 29) && (boxlines <= 59))
        index = 1;
    else if ((boxlines > 59) && (boxlines <= 89))
        index = 2;
    else if ((boxlines > 89) && (boxlines <= 119))
        index = 3;
    else if ((boxlines > 119) && (boxlines <= 147))
        index = 4;
    else if ((boxlines > 147) && (boxlines <= 177))
        index = 5;
    else if ((boxlines > 177) && (boxlines <= 207))
        index = 6;
    else if ((boxlines > 207) && (boxlines <= 237))
        index = 7;
    else if (boxlines > 237)
        index = 7;

    return index;

}
int Calc2D::Get2DScanAngleIndex(int scanAngleIndex)
{
    if (scanAngleIndex > 7)
        scanAngleIndex = 7;
    else if (scanAngleIndex < 0)
        scanAngleIndex = 0;

    return scanAngleIndex;
}
int Calc2D::Get2DDepthIndex(int depthIndex)
{
    if (depthIndex > 19)
        depthIndex = 19;
    else if (depthIndex < 0)
        depthIndex = 0;

    return depthIndex;
}
int Calc2D::GetPwPrfIndex(int prfIndex)
{
    if (prfIndex > 14)
        prfIndex = 14;
    else if (prfIndex < 0)
        prfIndex = 0;

    return prfIndex;
}
int Calc2D::GetCfmPrfIndex(int prfIndex)
{
    if (prfIndex > 10)
        prfIndex = 10;
    else if (prfIndex < 0)
        prfIndex = 0;

    return prfIndex;
}

///> private

/*
 * @brief calc emit delay of convex or TV
 *
 * @para focusIndex index of focus, range if (0 ~ FOC_MAX-1)代表第几个焦点
 * @para steerAngle[in] steer angle, used when steer or compound
 * @para CH_NUM[in] aperture control
 * @para chSize[in] sizeo of CH_NUM
 * @para delay[out] emit delay value
 */
void Calc2D::CEmitDelayCalc(float probeFocPos, float steerAngle, const int CH_NUM[], int size, unsigned short delay[APERTURE_HALF * 4])
{
    int i;
    float activeMaxLength, activeLength, perAngle;
    int sendCh = APERTURE_HALF * 2;
    int noSendCh = 4;
    ///> extern para used in calc计算中使用的外部参数
    float probeR;
    float probeWidth;

#ifdef EMP_PROJECT
if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

        probeR = m_projectCalcPara->probeR;
        probeWidth =(float) m_projectCalcPara->probeAngle/100.0;
    }
    else
    {
        probeR = m_calcPara->probeR;
    probeWidth = (float)m_calcPara->probeWidth / 100.0;
    }

#else
        probeR = m_calcPara->probeR;
    probeWidth = (float)m_calcPara->probeWidth / 100.0;
   #endif

 probeR = probeR /100.0;
    int probeArray = m_calcPara->probeArray;
    double speed = m_calcPara->soundSpeedTsi / 1000;
    float steer = steerAngle;
    float focPos = probeFocPos;
    int temp;
   //计算实际发射孔径
    perAngle = ((float)probeWidth) / ((float)probeArray-1) * PI / 180.0;

#ifdef EMP_PROJECT
    int sendChNum;
    float F_num_Tx = 4.0;

    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

            F_num_Tx =  m_projectCalcPara->txFNum;;

    }
    else
    {
        if((int)probeR <= 30)
        {
            F_num_Tx = 4.0;
            //  sendCh = (int)(4 * sqrt(1 + focPos * focPos / (5 * 5 + 1)));    //双曲线
            //  if(sendCh > APERTURE_HALF * 2)  sendCh = APERTURE_HALF * 2;
        }
        else
        {
            F_num_Tx = 8.0;
            //  sendChNum = (focPos + 1) / 10;
            //  if(sendChNum > 19)
            //      sendChNum = 19;
            //  ASSERT(sendChNum < size);
            //      sendCh = CH_NUM[sendChNum];
        }
    }
    //  noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

//  float F_num_Tx = 4.0;
    printf("tx_f_num = %3.2f\n", F_num_Tx);
    float AptSize_mm = focPos / F_num_Tx;
    int   AptSize_ele = AptSize_mm/probeR/perAngle;

    sendCh = AptSize_ele;
#else
int sendChNum;
    if((int)probeR <= 30)
    {
        sendCh = (int)(4 * sqrt(1 + focPos * focPos / (5 * 5 + 1)));    //双曲线
        if(sendCh > APERTURE_HALF * 2)  sendCh = APERTURE_HALF * 2;
    }
    else
    {
        sendChNum = (focPos + 1) / 10;
        if(sendChNum > 19)
            sendChNum = 19;
        ASSERT(sendChNum < size);
        sendCh = CH_NUM[sendChNum];
    }
#endif

    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    float b;
    float ofset2center;
    activeMaxLength = focPos;
    int mbp = m_calcPara->mbp;

    int PULSE_NUM;
    ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if((strcmp(para.model, "35D40J")) == 0)
    {
        PULSE_NUM = 1;
        PRINTF("-----35D40J---pulse num = 1\n");
    }
    else
    {
         PULSE_NUM = 2;
    }

    for (int odd_even = 1; odd_even < 3; odd_even ++)
    {
        ofset2center = -0.25+odd_even*0.5;
        if(mbp>1)
            b = (float)(( APERTURE_HALF  - 0.5 )* perAngle);
        else
            b = (float)(( APERTURE_HALF  - ofset2center)* perAngle);

        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength = sqrt((double)(4 * probeR * probeR * sin(b/2) * sin(b/2)
                        - 4 * probeR * focPos * sin(b/2) * sin(steer - b/2)
                        + focPos * focPos));
#ifdef EMP_355
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
#else
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
#endif

#if (defined(EMP_340) || defined(EMP_322))
            temp += 180 - (int)(60 / (m_calcPara->freq.emit / 20.0)) * PULSE_NUM;
#elif defined(EMP_355)
            temp += 220 -40;//- (int)(60 / (m_calcPara->freq.emit / 20.0)) * PULSE_NUM;
#else
            temp += 160-40;
#endif

            if(temp < 0)    temp = 0;
#ifdef EMP_355
            delay[(odd_even-1)*APERTURE_HALF*2 + APERTURE_HALF * 2 - 1 - i] = temp;
#else
            if(temp > 255)  temp = 255;
            delay[(odd_even-1)*APERTURE_HALF*2 + APERTURE_HALF * 2 - 1 - i] = (unsigned char)temp;
#endif
            b = b - perAngle;
        }

        for(i = 0; i < noSendCh; i++)
        {
            delay[(odd_even-1)*APERTURE_HALF*2 + i] = 0;
            delay[(odd_even-1)*APERTURE_HALF*2 + APERTURE_HALF * 2 - i - 1] = 0;
        }
    }

#if 0
   for (i = 0; i < APERTURE_HALF * 2; i ++)
   {
       PRINTF("calc delay:::::::::::::::delay[%d] = %d\n", i, delay[i]);
   }
#endif
}

/*
 * @brief calc receive delay of convex and TV probe
 * @para An[out] parameter An's calculate result
 * @para Bn[out] parameter Bn's calculate result
 */
void Calc2D::CReceiveDelayCalc(float steerAngle, int An[MAX_MBP * APERTURE_HALF * 2], short Bn[MAX_MBP * APERTURE_HALF * 2])
{
    const int FREQ = SAMPLE_FREQ; //Mhz
    float offsetToCenter = 0.0;
    float b, absb, perAngle, temp;
    double speed = m_calcPara->soundSpeedTsi;
    float probeR;
    float probeWidth;

#ifdef EMP_PROJECT
 if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
     probeR = m_projectCalcPara->probeR;;
        probeWidth = m_projectCalcPara->probeAngle;
    }
    else
    {   probeR = (float)m_calcPara->probeR;
    probeWidth = m_calcPara->probeWidth;
    }

#else
     probeR = (float)m_calcPara->probeR;
    probeWidth = m_calcPara->probeWidth;
#endif
    probeR =(float) probeR /100.0;
    int probeArray = m_calcPara->probeArray;
    perAngle = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1)) * PI / 180.0;

    int beamNo;
    int i;
    int delayFAn[APERTURE_HALF * 2];
    short delayFBn[APERTURE_HALF * 2];
    for (beamNo = 0; beamNo < MAX_MBP; beamNo ++)
    {
        offsetToCenter = -0.25 + 0.5 * beamNo;
        b = ( APERTURE_HALF - offsetToCenter) * perAngle;

        for (i = 0; i < APERTURE_HALF * 2; i ++)
        {
            absb = fabs(b);
            temp = 4.0 * probeR * probeR * sin(absb/2) * sin(absb/2) * ((float)FREQ/speed) * ((float)FREQ/speed);
            delayFAn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            temp = 4.0 * probeR * sin(b/2) * sin(b/2 - steerAngle) * ((float)FREQ/speed);
            delayFBn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            b = b - perAngle;
        }

        memcpy(An+beamNo*APERTURE_HALF*2, delayFAn, sizeof(delayFAn));
        memcpy(Bn+beamNo*APERTURE_HALF*2, delayFBn, sizeof(delayFBn));
    }
}

/*
 * @brief  calc receive aperture for Convex or TV
 */
void Calc2D::CReceiveApertureCalcU16(float steerAngle,  short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2])
{
    int  i, j;
    int endPix;
    int delayDb[APERTURE_HALF * 2];
    double scale;

    //变迹孔径
    endPix = APERTURE_DOTS;
    int firstpeak = 8192;//255;
    int span_no = 0;
    float tempDepth,perAngle;

    scale = ((float)m_calcPara->depth / APERTURE_DOTS);
    float probeWidth;
     probeWidth = m_calcPara->probeWidth;
    int probeArray = m_calcPara->probeArray;
    perAngle = ((float)probeWidth / 100.0) / ((float)probeArray-1) * PI / 180.0;
    const int MAX_SUM_VALUE = 8192*2;
    float span;
    int sumEven, sumOdd, sum_half;
    for( j = 0; j < endPix; j++ )
    {
        tempDepth = ((float)j) * scale;

        span_no = tempDepth / 10;
        if(span_no < 11)
        {
            //分段线性插值
            span = APERTURE_SPAN_C[span_no] + (tempDepth - ((int)(tempDepth / 10)) * 10 ) * (APERTURE_SPAN_C[span_no + 1]- APERTURE_SPAN_C[span_no]) / 10;
        }
        else
        {
            span = APERTURE_SPAN_C[11];
        }

        ProbeSocket::ProbePara para;
        ProbeMan::GetInstance()->GetCurProbe(para);

        const char* type = para.model;
        if (strcmp(type, "35C60E") == 0)
            span /=2.5;
        /*
        span = tempDepth/5.0/perAngle/probeR;
        if(span>64) span = 64;
        span = tempDepth/((float)m_calcPara->depth)/perAngle*3.0;
        if(span<6)  span = 6;
        if(span>80) span = 80;
        */
        sumEven = 0;
        sumOdd = 0;
        sum_half = 0;
        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            //抛物线
            int temp = (int)((float)firstpeak - (float)( ((float)(i - (APERTURE_HALF -1)) * (i - (APERTURE_HALF -1))) * ((float)firstpeak) / (span * span) + 0.5));

            if (temp < 0) temp = 0;
            //else if (temp > 255) temp = 255;

            if (m_singleAperture && (i != APERTURE_HALF))
                temp = 0;
            delayDb[i] = temp;
            sumEven += delayDb[i];
        }
        if(sum_half == 0)
            sum_half = 1;
        if(sum_half == 0)
            sum_half = 1;
        if(sumEven == 0)
            sumEven= 1;

        //归一化处理
        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
#if 0
            if (i%2)
                delayDb[i] = delayDb[i] * MAX_SUM_VALUE / sumOdd;
            else
                delayDb[i] = delayDb[i] * MAX_SUM_VALUE / sumEven;
#endif
            delayDb[i] = delayDb[i] * MAX_SUM_VALUE / sumEven;
        }
        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            //保存变迹
            delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[i];
        }
    }
}

/*
 * @brief  calc receive aperture for Convex or TV
 */
void Calc2D::CReceiveApertureCalc(float steerAngle, unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2])
{
    int  i, j;
    int endPix;
    int delayDb[APERTURE_HALF * 2];
    double scale;

    //变迹孔径
    endPix = APERTURE_DOTS;
    int firstpeak = 255;
    int span_no = 0;
    float tempDepth,perAngle;

    scale = ((float)m_calcPara->depth / APERTURE_DOTS);

    float probeWidth;
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeWidth =(float) m_projectCalcPara->probeAngle;
    }
    else
    {

        probeWidth = (float)m_calcPara->probeWidth;

    }
#else
    probeWidth = (float)m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    perAngle = ((float)probeWidth / 100.0) / ((float)probeArray-1) * PI / 180.0;
    float span;
    for( j = 0; j < endPix; j++ )
    {
        tempDepth = ((float)j) * scale;

        span_no = tempDepth / 10;
        if(span_no < 11)
        {
            //分段线性插值
            span = APERTURE_SPAN_C[span_no] + (tempDepth - ((int)(tempDepth / 10)) * 10 ) * (APERTURE_SPAN_C[span_no + 1]- APERTURE_SPAN_C[span_no]) / 10;
        }
        else
        {
            span = APERTURE_SPAN_C[11];
        }

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            //抛物线
            int temp = (int)((float)firstpeak - (float)( ((float)(i - (APERTURE_HALF -1)) * (i - (APERTURE_HALF -1))) * ((float)firstpeak) / (span * span) + 0.5));

            if (temp < 0) temp = 0;
            else if (temp > 255) temp = 255;

            if (m_singleAperture && (i != APERTURE_HALF))
                temp = 0;
            delayDb[i] = temp;
        }

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            //保存变迹
            delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[i];
        }
    }
}

void Calc2D::CCompoundParaAdderCalc(const float steer, unsigned int *addr, int size)
{
    int probeArray = m_calcPara->probeArray;
    float probeWidth;
    float probeR;
#ifdef EMP_PROJECT
 if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeWidth =(float) m_calcPara->probeWidth/100.0;
        probeR = m_projectCalcPara->probeR;
    }
    else
    {
        probeWidth =(float) m_calcPara->probeWidth/100.0;
        probeR = m_calcPara->probeR;
    }

#else
        probeWidth =(float) m_calcPara->probeWidth/100.0;
    probeR = m_calcPara->probeR;
#endif
    int dots = m_calcPara->depthDots;
    int depth = m_calcPara->depth;
    int lines = m_calcPara->probeLines;

    int correct = 0;
    if (steer < 0.0)
        correct = - 0.5;
    else
        correct = 0.5;
#ifdef EMP_355
    float dotInterval = (float)depth / (float)dots;
#else
    float dotInterval = (float)depth / (dots * INIT_SCALE);
#endif
    float angleInterval = (float)probeWidth / (lines - 1) * PI / 180;

    float h;
    float r = (float)probeR;
    r = r / 100.0;
    float BC;

    int dotOffset = r * (1 - cos(fabs(steer))) / dotInterval / 2 + 0.5;
    int lineOffset;
    int dotaddrNormal;
    int dotaddrSteer;
    float theta;
    int start = 0;//for compound
    for (int i = 0; i < size; i ++)
    {
        h = i * dotInterval;
        BC = sqrt((r+h) * (r+h) - r*r*sin(steer)*sin(steer)) - r * cos(steer);
        theta = asin(sin(steer) * BC / (r+h));

        lineOffset = theta / angleInterval + 0.5;
        dotaddrNormal = i + start + dotOffset;
        dotaddrSteer = BC / dotInterval + 0.5 + start;
        *(addr + i) = ((lineOffset & 0x1ff) << 20) + (dotaddrSteer << 10) + dotaddrNormal;
    }
}

/*
 * @brief calc emit delay of linear probe
 *
 * @para probeFocPos[in], current focus position of probe
 * @para steerAngle[in] steer angle, used when steer or compound
 * @para CH_NUM[in] aperture control
 * @para chSize[in] sizeo of CH_NUM
 * @para delay[out] emit delay value: even(0-APERTURE_HALF * 2); odd(APERTURE_HALF * 2-APERTURE_HALF * 4)
 */
void Calc2D::LEmitDelayCalc(float probeFocPos, float steerAngle, const int CH_NUM[], int chSize, unsigned short delay[APERTURE_HALF * 4])
{
    int i;
    float activeMaxLength, activeLength, interval;
    int sendCh = APERTURE_HALF * 2;
    int noSendCh = 4;

    ///> extern para used in calc计算中使用的外部参数
   float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

        probeWidth = (float)m_projectCalcPara->probeAngle / 100.0;
    }
    else
    {
        probeWidth =(float) m_calcPara->probeWidth / 100.0;
    }
#else
        probeWidth =(float) m_calcPara->probeWidth / 100.0;
#endif
     int probeArray = m_calcPara->probeArray;
    double speed = m_calcPara->soundSpeedTsi / 1000;
    float steer = steerAngle;
    //float focPos = probeFocPos / cos(fabs(steer));
    float focPos = probeFocPos;
    //calc emit delay计算实际发射孔径
    float sqFocPos = focPos * focPos;
    interval = (float)((float)probeWidth) / ((float)(probeArray - 1));

    int temp;
    float b;
    int sendChNum;

    for(i = 0; i < APERTURE_HALF*4; i++)
    {
        delay[i] = 0;
    }

    sendChNum = (focPos + 1) / 10;

    if(sendChNum > 19)
        sendChNum = 19;

    if (sendChNum > (chSize - 1))
    {
        sendCh = 64;
        PRINTF("send ch num no = %d \n", sendChNum);
    }
    else
        sendCh = CH_NUM[sendChNum];

    //sendCh *= 1.5;
    //if(sendCh>64)   sendCh = 64;

    PRINTF("send ch num = %d \n", sendCh);
    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    int mbp;
    float ofset2center;
    activeMaxLength = focPos;
    const int PULSE_NUM = 1;

    for (int odd_even = 1; odd_even < 3; odd_even ++)
    {
        ofset2center = -0.25+odd_even*0.5;

        mbp  = m_calcPara->mbp;
        if(mbp>1)
            b = (float)(( APERTURE_HALF  - 0.5 )*interval);
        else
            b = (float)(( APERTURE_HALF  - ofset2center )*interval);

        int offset_init_delay;
#ifdef EMP_355
        offset_init_delay = APERTURE_HALF * interval * sin(fabs(steer)) / speed / NS_PER_UNIT_EMIT / 2;
#else
        offset_init_delay = APERTURE_HALF * interval * sin(fabs(steer)) / speed / NS_PER_UNIT / 2;
#endif
    for( i =  0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength= (float)sqrt ( (float) ( sqFocPos + b * b - 2 * focPos * b * sin(steer) ) );
#ifdef EMP_355
        temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
#else
        temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
#endif
           // temp += (160-40);

#if (defined(EMP_340) || defined(EMP_322))
           temp += 180 - (int)(60 / (m_calcPara->freq.emit / 20.0)) * PULSE_NUM + offset_init_delay;
#elif (defined(EMP_355))
           temp += (260-40+offset_init_delay);
#else
           temp += (160-40+offset_init_delay);
#endif

            if(temp < 0)    temp = 0;
#ifdef EMP_355
            delay[(odd_even-1)*APERTURE_HALF*2 + APERTURE_HALF * 2 - 1 - i] = temp;
#else
            if(temp > 255)  temp = 0;   //255;
            delay[(odd_even-1)*APERTURE_HALF*2 + APERTURE_HALF * 2 - 1 - i] = (unsigned char)temp;
#endif
            b = b - interval;
        }

        for(i=0; i<noSendCh; i++)
        {
            delay[(odd_even-1)*APERTURE_HALF*2+i] = 0;
            delay[(odd_even-1)*APERTURE_HALF*2+APERTURE_HALF*2-i-1] = 0;
        }
    }
}

/*
 * @brief calc receive delay of linear probe
 * @para steer[in] steer angle
 * @para An[out] parameter An's calculate result
 * @para Bn[out] parameter Bn's calculate result
 */
void Calc2D::LReceiveDelayCalc(float steerAngle, int An[MAX_MBP * APERTURE_HALF * 2], short Bn[MAX_MBP * APERTURE_HALF * 2])
{
    const int FREQ = SAMPLE_FREQ;
    float offsetToCenter = 0.0;
    float b, interval, temp;
    double speed = m_calcPara->soundSpeedTsi;
    float probeWidth;
#ifdef EMP_PROJECT
   if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth = m_projectCalcPara->probeAngle;
    else
        probeWidth = m_calcPara->probeWidth;
#else
        probeWidth = m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    interval = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1));

    float steer = steerAngle;
    int beamNo;
    int i;
    int delayFAn[APERTURE_HALF * 2];
    short delayFBn[APERTURE_HALF * 2];
    for (beamNo = 0; beamNo < MAX_MBP; beamNo ++)
    {
        offsetToCenter = -0.25 + 0.5 * beamNo;
        b = ( APERTURE_HALF - offsetToCenter) * interval;

        for (i = 0; i < APERTURE_HALF * 2; i ++)
        {
            temp =  ((float)FREQ/speed) * b * ((float)FREQ/speed) * b;
            delayFAn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            temp =  -2.0 * ((float)FREQ/speed) * b * sin(steer);
            delayFBn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            b = b - interval;
        }

        memcpy(An+beamNo*APERTURE_HALF*2, delayFAn, sizeof(delayFAn));
        memcpy(Bn+beamNo*APERTURE_HALF*2, delayFBn, sizeof(delayFBn));
    }
}

void Calc2D::LReceiveApertureCalcU16(float steerAngle, short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2])
{
    int  i, j;
    int delayDb[APERTURE_HALF * 2];
    int endPix;
    double scale;

    endPix = APERTURE_DOTS;
    float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth = m_projectCalcPara->probeAngle;
    else
        probeWidth= m_calcPara->probeWidth;
#else
        probeWidth= m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    float steer = steerAngle;
    float interval = ((float)probeWidth / 100) / (float)(probeArray - 1);
    const int MAX_SUM_VALUE = 8192*2;
    scale = ((float)m_calcPara->depth / APERTURE_DOTS); //mm/s
    float span;
    int span_no;

    //初始变迹 0~startPix-1
    int aaa,bbb;
    int   trackcenteroffset=0;
    float tempdepth;
    int temp;
    aaa = 8;
    bbb = 8;
    int sumEven = 0;
    int sumOdd = 0;
    for( j = 0; j < endPix; j++ )
    {
        sumEven = 0;
        sumOdd = 0;
        tempdepth = ((float)j+1) * scale;
        trackcenteroffset = tempdepth*sin(steer) / interval /3;
        if(trackcenteroffset>=31) trackcenteroffset = 31;
        else if(trackcenteroffset<= (-31))  trackcenteroffset = -31;

        span_no = tempdepth / 10;
        if(span_no < 11)
        {
            //分段线性插值
            span = APERTURE_SPAN_L[span_no] + (tempdepth - ((int)(tempdepth / 10)) * 10 ) * (APERTURE_SPAN_L[span_no + 1]- APERTURE_SPAN_L[span_no]) / 10;
        }
        else
        {
            span = APERTURE_SPAN_L[11];
        }
        span = span*1.5;
        /*
        span = aaa*sqrt(1.0+((float)(j*j)/((float)(bbb*bbb)))); //aaa=8; bbb=8;
        span = tempdepth/interval;
        span = span*4/3;
        */
        if(span <=2) span = 2;

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            temp = 255.0*exp(  ( 0- (i-(APERTURE_HALF -1-trackcenteroffset))*(i-(APERTURE_HALF -1-trackcenteroffset) )*4     )/(span*span));        //高斯函数
            if (temp < 0)           temp = 0;
            //else if (temp > 255)  temp = 255;

            if (m_singleAperture && (i != APERTURE_HALF))
                temp = 0;

           delayDb[i] = temp;
#if 0
            if (i %2)
                sumOdd += delayDb[i];
            else
                sumEven += delayDb[i];
#endif
            sumEven += delayDb[i];
        }

        if(sumEven == 0)
            sumEven = 1;
        if(sumOdd == 0)
            sumOdd = 1;

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
#if 0
            if (i%2)
                delayDb[i] = delayDb[i] * MAX_SUM_VALUE / sumOdd;
            else
                delayDb[i] = delayDb[i] * MAX_SUM_VALUE / sumEven;
#endif
            delayDb[i] = delayDb[i] * MAX_SUM_VALUE / sumEven;
        }

        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            //保存变迹
            delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[APERTURE_HALF * 2 - 1 - i];
        }
    }
}

void Calc2D::LReceiveApertureCalc(float steerAngle, unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2])
{
    int  i, j;
    int delayDb[APERTURE_HALF * 2];
    int endPix;
    double scale;

    endPix = APERTURE_DOTS;
    float  probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth = m_projectCalcPara->probeAngle;
    else
        probeWidth = m_calcPara->probeWidth;
#else
        probeWidth = m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    float steer = steerAngle;
    float interval = ((float)probeWidth / 100) / (float)(probeArray - 1);

    scale = ((float)m_calcPara->depth / APERTURE_DOTS); //mm/s
    float span;
    int span_no;

    //初始变迹 0~startPix-1
    int aaa,bbb;
    int   trackcenteroffset=0;
    float tempdepth;
    int temp;
    aaa = 8;
    bbb = 8;
    for( j = 0; j < endPix; j++ )
    {
        tempdepth = ((float)j+1) * scale;
        trackcenteroffset = tempdepth*sin(steer) / interval /3;
        if(trackcenteroffset>=31) trackcenteroffset = 31;
        else if(trackcenteroffset<= (-31))  trackcenteroffset = -31;

        span_no = tempdepth / 10;
        if(span_no < 11)
        {
            //分段线性插值
            span = APERTURE_SPAN_L[span_no] + (tempdepth - ((int)(tempdepth / 10)) * 10 ) * (APERTURE_SPAN_L[span_no + 1]- APERTURE_SPAN_L[span_no]) / 10;
        }
        else
        {
            span = APERTURE_SPAN_L[11];
        }
#ifdef EMP_355
         ProbeSocket::ProbePara para;
    ProbeMan::GetInstance()->GetCurProbe(para);
    if (strcmp("55L60J",para.model)==0)
             {
                span = span*0.3;
        //printf("\n\n\nspan * = 0.3, %d\n\n\n",span);
             }
    else
             {
                span = span*1.5;
        //printf("\n\n\nspan * = 0.3, %d\n\n\n",span);
             }
 #else
                 span = span*1.5;
#endif
        /*
        span = aaa*sqrt(1.0+((float)(j*j)/((float)(bbb*bbb)))); //aaa=8; bbb=8;
        span = tempdepth/interval;
        span = span*4/3;
        */
        if(span <=2) span = 2;

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            temp = 255.0*exp(  ( 0- (i-(APERTURE_HALF -1-trackcenteroffset))*(i-(APERTURE_HALF -1-trackcenteroffset) )*4     )/(span*span));        //高斯函数
            if (temp < 0)           temp = 0;
            else if (temp > 255)    temp = 255;

            if (m_singleAperture && (i != APERTURE_HALF))
                temp = 0;

           delayDb[i] = temp;
        }
        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            //保存变迹
            delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[APERTURE_HALF * 2 - 1 - i];
        }
    }
}

/*
 * @brief calc emit delay of phase probe
 *
 * @para probeFocPos[in], current focus position of probe
 * @para mode[in] image mode
 * @para CH_NUM[in]
 * @para chSize[in]
 */
void Calc2D::PEmitDelayCalc(float probeFocPos, EMODE mode, const int CH_NUM[], int chSize, unsigned short delay[APERTURE_HALF * 2 * PHASELINE])
{

    float activeMaxLength, activeLength, interval;
    int sendCh = APERTURE_HALF * 2;
    int noSendCh = 4;

    ///> extern para used in calc计算中使用的外部参数
    float probeWidth;
#ifdef EMP_PROJECT
   if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth =(float) m_projectCalcPara->probeAngle /100.0;
    else
        probeWidth =(float) m_calcPara->probeWidthPhase / 100.0;
#else
        probeWidth =(float) m_calcPara->probeWidthPhase / 100.0;
    #endif
    int probeArray = m_calcPara->probeArray;
    int probeLines = m_calcPara->probeLines;
    double speed = m_calcPara->soundSpeedTsi / 1000;
    float probeAngle;// = m_calcPara->probeWidth / 100.0;

    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {
        probeAngle = (float)m_calcPara->extendedAngle/100.0;
    }
    else
    {
        probeAngle = (float)m_calcPara->probeWidth/100.0;
    }

    float focPos = probeFocPos;
    //calc emit delay计算实际发射孔径
    float sqFocPos = focPos * focPos;
    interval = (float)((float)probeWidth) / ((float)(probeArray - 1));

    int temp;
    float b;
    int sendChNum;
#ifdef EMP_PROJECT

   float F_num_Tx = 5.0;
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

            F_num_Tx =  m_projectCalcPara->txFNum;

    }
    float AptSize_mm = focPos / F_num_Tx;
    int   AptSize_ele = AptSize_mm/interval;

    sendCh = AptSize_ele;
    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    int i;
    for(i = 0; i < APERTURE_HALF*2*PHASELINE; i++)
    {
        delay[i] = 0;
    }

    activeMaxLength = focPos;
    int lines = probeLines;
    int j;
    float angle;
    PRINTF("probeLines=%d\n", lines);
    for (j = 0; j < lines; j ++)
    {
        angle = (0 - probeAngle / 2.0 + j * probeAngle / (float)(probeLines - 1)) * PI / 180.0;
        b = ( APERTURE_HALF - 0.5) * interval;

        for( i =  0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength= (float)sqrt ( (float) ( sqFocPos + b * b - 2 * focPos * b * sin(angle) ) );
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
            temp += 250;

            if(temp < 0)    temp = 0;
            if(temp > 1000) temp = 0;

            if((i<noSendCh) ||( i>(APERTURE_HALF*2-1-noSendCh)))
            {

                delay[j * APERTURE_HALF*2  + APERTURE_HALF * 2 - 1 - i] = 0;
            }
            else
                delay[j * APERTURE_HALF*2  + APERTURE_HALF * 2 - 1 - i] = temp;
            //delay[j * APERTURE_HALF*2  + i] = temp;
            b = b - interval;
        }
    }
#else

    int i;
    for(i = 0; i < APERTURE_HALF*2*PHASELINE; i++)
    {
        delay[i] = 0;
    }

    activeMaxLength = focPos;
    int lines = probeLines;
    int j;
    float angle;
#if 0
    bool isCw = m_calcPara->modeCw;
    if (ModeStatus::IsCWImgMode() && isCw)
    {
        int cwLine = m_calcPara->cwLine;
        lines = cwLine;
        j = lines;
        angle = (0 - probeAngle / 2.0 + j * probeAngle / (float)(probeLines - 1)) * PI / 180.0;
        b = ( APERTURE_HALF - 0.5) * interval;

        for( i =  0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength= (float)sqrt ( (float) ( sqFocPos + b * b - 2 * focPos * b * sin(angle) ) );
#ifdef EMP_355
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
#else
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
#endif
            temp += 250;

            if(temp < 0)    temp = 0;
            if(temp > 1000) temp = 0;

            delay[j * APERTURE_HALF*2  + APERTURE_HALF * 2 - 1 - i] = temp;
            b = b - interval;
        }

    }
    else
    {
        lines = probeLines;
#endif
        PRINTF("probeLines=%d\n", lines);
        for (j = 0; j < lines; j ++)
        {
            angle = (0 - probeAngle / 2.0 + j * probeAngle / (float)(probeLines - 1)) * PI / 180.0;
            b = ( APERTURE_HALF - 0.5) * interval;

            for( i =  0; i < APERTURE_HALF * 2; i++ )
            {
                activeLength= (float)sqrt ( (float) ( sqFocPos + b * b - 2 * focPos * b * sin(angle) ) );
#ifdef EMP_355
                temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
#else
                temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
#endif
                temp += 250;

                if(temp < 0)    temp = 0;
                if(temp > 1000) temp = 0;

                delay[j * APERTURE_HALF*2  + APERTURE_HALF * 2 - 1 - i] = temp;
                //delay[j * APERTURE_HALF*2  + i] = temp;
                b = b - interval;
            }
        }
#endif
}

/*
 * @brief calc receive delay of phase probe
 * @para An[out] parameter An's calculate result
 * @para Bn[out] parameter Bn's calculate result
 */
void Calc2D::PReceiveDelayCalc(int An[APERTURE_HALF * 2 * PHASELINE], short Bn[APERTURE_HALF * 2 * PHASELINE])
{
    const int FREQ = SAMPLE_FREQ; //Mhz
    float offsetToCenter = 0.0;
    float b, interval, temp;
    double speed = m_calcPara->soundSpeedTsi;
    float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth =(float) m_projectCalcPara->probeAngle /100.0;
    else
        probeWidth =(float) m_calcPara->probeWidthPhase / 100.0;
#else
        probeWidth =(float) m_calcPara->probeWidthPhase / 100.0;
#endif

    int probeArray = m_calcPara->probeArray;
        int probeLines = m_calcPara->probeLines;
        int probeAngle;// = m_calcPara->probeWidth / 100.0;

    if (m_calcPara->efviCtrl || m_calcPara->tpViewCtrl)
    {
        probeAngle = m_calcPara->extendedAngle/100;
    }
    else
    {
        probeAngle = m_calcPara->probeWidth/100;

    }

    interval = ((float)probeWidth) / ((float)(probeArray - 1));

    float angle;
    int beamNo;
    int i;
    int lineNo;
    int delayFAn[APERTURE_HALF * 2];
    short delayFBn[APERTURE_HALF * 2];
    for (beamNo = 0; beamNo < PHASELINE; beamNo ++)
    {
        //An
    //  offsetToCenter = -0.25 + 0.5 * beamNo;
        offsetToCenter = 0.5;
        b = ( APERTURE_HALF - offsetToCenter) * interval;
        for (i = 0; i < APERTURE_HALF * 2; i ++)
        {
            temp =  ((float)FREQ/speed) * b * ((float)FREQ/speed) * b;
            delayFAn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            b = b - interval;
        }
        memcpy(An+beamNo*APERTURE_HALF*2, delayFAn, sizeof(delayFAn));
    }

    // Bn
    for (lineNo = 0; lineNo < probeLines; lineNo ++)
    {
        angle = (0 - probeAngle / 2.0 + ((probeLines+lineNo-1)%probeLines) * probeAngle / (float)(probeLines - 1))  * PI / 180.0;
        offsetToCenter = 0.5;
        b = (APERTURE_HALF - offsetToCenter) * interval;
        for (i = 0; i < APERTURE_HALF * 2; i ++)
        {
            temp =  -2.0 * ((float)FREQ/speed) * b * sin(angle);
            delayFBn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            b = b - interval;
        }
        memcpy(Bn+lineNo*APERTURE_HALF*2, delayFBn, sizeof(delayFBn));
    }
}

void Calc2D::PReceiveApertureCalc(unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2])
{
    int  i, j;
    int delayDb[APERTURE_HALF * 2];
    int endPix;
    double scale;

    endPix = APERTURE_DOTS;
    float  probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth =(float) m_projectCalcPara->probeAngle;
    else
        probeWidth =(float) m_calcPara->probeWidthPhase;
#else
        probeWidth =(float) m_calcPara->probeWidthPhase;
#endif
    int probeArray = m_calcPara->probeArray;
    float steer = 0.0; //m_calcPara->steerAngle * PI / 180.0;

    float interval = ((float)probeWidth / 100) / (float)(probeArray - 1);

    scale = ((float)m_calcPara->depth / APERTURE_DOTS); //mm/s
    float span;

    //初始变迹 0~startPix-1
    int aaa,bbb;
    int   trackcenteroffset=0;
    float tempdepth;
    int temp;
    aaa = 8;
    bbb = 8;
    bool isCw = m_calcPara->modeCw;

    for( j = 0; j < endPix; j++ )
    {
        tempdepth = ((float)j+1) * scale;
        trackcenteroffset = tempdepth*sin(steer) / interval /3;
        if(trackcenteroffset>=31) trackcenteroffset = 31;
        else if(trackcenteroffset<= (-31))  trackcenteroffset = -31;

        span = aaa*sqrt(1.0+((float)(j*j)/((float)(bbb*bbb)))); //aaa=8; bbb=8;
        span = tempdepth/interval;
        span = span*4/3;
        if(span <=2) span = 2;

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            temp = 255.0*exp(  ( 0- (i-(APERTURE_HALF -1-trackcenteroffset))*(i-(APERTURE_HALF -1-trackcenteroffset) )*4     )/(span*span));        //高斯函数
            if (temp < 0)           temp = 0;
            else if (temp > 255)    temp = 255;

            if (m_singleAperture && (i != APERTURE_HALF))
                temp = 0;

            delayDb[i] = temp;
        }
        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            //保存变迹
            delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[APERTURE_HALF * 2 - 1 - i];
        }

#if 0
        if(!isCw)
        {
            //the firt time is send aperture
            //test
            for (i = 0; i < APERTURE_HALF*2; i++)
            {
                delayAperture[APERTURE_HALF*2*j + i] = 0;
            }
        }
        else
        {
            for( i = 0; i < APERTURE_HALF * 2; i++ )
            {
                //保存变迹
                delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[APERTURE_HALF * 2 - 1 - i];
            }
        }
#endif
    }
}
void Calc2D::PReceiveApertureCalcU16(short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2])
{
    int  i, j;
    int delayDb[APERTURE_HALF * 2];
    int endPix;
    double scale;
    const int MAX_SUM_VALUE = 8192*2;
    endPix = APERTURE_DOTS;
    float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth =(float) m_projectCalcPara->probeAngle;
    else
        probeWidth =(float) m_calcPara->probeWidthPhase;
#else
        probeWidth =(float) m_calcPara->probeWidthPhase;
#endif
    int probeArray = m_calcPara->probeArray;
    float steer = 0.0; //m_calcPara->steerAngle * PI / 180.0;

    float interval = ((float)probeWidth / 100) / (float)(probeArray - 1);

    scale = ((float)m_calcPara->depth / APERTURE_DOTS); //mm/s
    float span;

    //初始变迹 0~startPix-1
    int aaa,bbb;
    int   trackcenteroffset=0;
    float tempdepth;
    int temp;
    aaa = 8;
    bbb = 8;
    bool isCw = m_calcPara->modeCw;

    int sumDelay;
    for( j = 0; j < endPix; j++ )
    {
        sumDelay = 0;
        tempdepth = ((float)j+1) * scale;
        trackcenteroffset = tempdepth*sin(steer) / interval /3;
        if(trackcenteroffset>=31) trackcenteroffset = 31;
        else if(trackcenteroffset<= (-31))  trackcenteroffset = -31;

        span = aaa*sqrt(1.0+((float)(j*j)/((float)(bbb*bbb)))); //aaa=8; bbb=8;
        span = tempdepth/interval;
        span = span*4/3;
        if(span <=2) span = 2;

        for( i = 0; i < APERTURE_HALF*2; i++ )
        {
            temp = 255.0*exp(  ( 0- (i-(APERTURE_HALF -1-trackcenteroffset))*(i-(APERTURE_HALF -1-trackcenteroffset) )*4     )/(span*span));        //高斯函数
            if (temp < 0)           temp = 0;
            //else if (temp > 255)  temp = 255;

            if (m_singleAperture && (i != APERTURE_HALF))
                temp = 0;

            delayDb[i] = temp;
            sumDelay += delayDb[i];
        }

        if(sumDelay == 0)
            sumDelay = 1;
        for( i = 0; i < APERTURE_HALF * 2; i++)
            delayDb[i] =  delayDb[i] * MAX_SUM_VALUE / sumDelay;

        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            //保存变迹
            delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[APERTURE_HALF * 2 - 1 - i];
        }

#if 0
        if(!isCw)
        {
            //the firt time is send aperture
            //test
            for (i = 0; i < APERTURE_HALF*2; i++)
            {
                delayAperture[APERTURE_HALF*2*j + i] = 0;
            }
        }
        else
        {
            for( i = 0; i < APERTURE_HALF * 2; i++ )
            {
                //保存变迹
                delayAperture[APERTURE_HALF * 2 * j + i] = delayDb[APERTURE_HALF * 2 - 1 - i];
            }
        }
#endif
    }
}

void Calc2D::PReceiveDelayCalcCW(unsigned short delayEmit[APERTURE_HALF * 2 * PHASELINE])
{
    int lines = m_calcPara->cwLine;

    unsigned short delay[APERTURE_HALF*2];
    //unsigned short cwEmit[APERTURE_PHASE];
    unsigned short cwReceive[APERTURE_PHASE];
    // get line delay
    memcpy(delay, delayEmit+APERTURE_HALF*2*lines, APERTURE_HALF*2*sizeof(unsigned short));

    // emit
    //memcpy(cwEmit, delay, APERTURE_PHASE*sizeof(unsigned short));
    // send emit to fpga

    // receive
    memcpy(cwReceive, delay+8, APERTURE_PHASE*sizeof(unsigned short));

    // get min value of receive
    int min = 0xFFFF;
    for (int i = 0; i < APERTURE_PHASE; i ++)
    {
        if (cwReceive[i] < min)
        {
            min = cwReceive[i];
        }
    }

    // get max value of (receive - min) / pulseWidth
    int max = 0;
    for (int i = 0; i < APERTURE_PHASE; i ++)
    {
#if 0
        cwReceive[i] = (cwReceive[i]- min) / m_pulseCycle;
        if (cwReceive[i] > max)
        {
            max = cwReceive[i];
        }
#else
       if (cwReceive[i] > max)
        {
            max = cwReceive[i];
        }
       cwReceive[i] = (cwReceive[i]- min) % (2 * m_pulseCycle);

       PRINTF("------des cwReceive[%d]=%d  max=%d  min=%d  m_pulseCycle=%d\n", i, cwReceive[i], max, min, m_pulseCycle);
#endif
    }

    // 对24 /16做归一化
    const int DIVIDER = 24; //16;
    unsigned char clkNum[APERTURE_PHASE];
    for (int i = 0; i < APERTURE_PHASE; i ++)
    {
        //clkNum[i] = DIVIDER * cwReceive[i] / (max+1);
        clkNum[i] = DIVIDER * cwReceive[i] / (2 * m_pulseCycle);
        PRINTF("------calc clkNum cwReceive[%d]=%d  m_pulseCycle=%d  clkNum[%d]=%d\n", i, cwReceive[i],  m_pulseCycle, i, clkNum[i]);
    }

    // send receive to fpga
    m_fpga.SendCWReceive(clkNum, APERTURE_PHASE);
}

/*
 * @brief calc weighting emit value
 * @para pulseWidth[in] base pulse width
 * @para weightingValue[out] result of calc
 */
void Calc2D::WeightingEmit(int pulseWidth, int spanValue, unsigned char weightingValue[APERTURE_HALF * 2])
{
    int i;
    float value;

    for (i = 0; i < APERTURE_HALF * 2; i ++)
    {
        value =  1.0 * exp(0 - (i - (APERTURE_HALF-1)) * (i - (APERTURE_HALF-1)) / (float)spanValue);       // L -- 200;  C -- 500
        weightingValue[i] = pulseWidth * value + 0.4;
        //PRINTF("(((((((((((((((((((((((((((CALC2d: weighting emit %d = %d\n", i, weightingValue[i]);
    }
}

/*
 * @brief Calc compound parameter(adderline, adderdot, startsamp)
 * @para steer[in] steer angle
 * @para angle[in] compound angle
 * @para line[out] adderline
 * @para dot[out] adderdot
 * @para start[out] startsamp
 */
void Calc2D::CompoundParaCalc(const float steer, const float angle, int& line, int &dot, int& start)
{
    const int value = 1024;
    float probeWidth;
#ifdef EMP_PROJECT
   if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth = m_projectCalcPara->probeAngle;
    else
        probeWidth= m_calcPara->probeWidth;
#else
        probeWidth= m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    int dots = m_calcPara->depthDots;
    int depth = m_calcPara->depth;
    //float speed = m_calcPara->soundSpeedTsi;
    float interval = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1));
    int correct = 0;
    if (steer < 0.0)
        correct = - 0.5;
    else
        correct = 0.5;
#ifdef EMP_355
    line = 0 - (depth / (float)dots * tan(steer) * 2.0 / interval * value + correct);
#else
    line = 0 - (depth / (dots * INIT_SCALE) * tan(steer) * 2.0 / interval * value + correct);
#endif
    dot = 1.0 / cos(steer) * value + 0.5;
    if (fabs(angle) < ZERO)
        start = 0;
    else
        start = 1;//(APERTURE_HALF * interval / sin(fabs(angle)) * (1.0 - cos(fabs(angle))) * 1000 / speed / (float)NS_PER_UNIT + 10) / temp;
    PRINTF("--- line = %d, dot = %d, sampStart = %d\n", line, dot, start);
}

void Calc2D::LCompoundParaAdderCalc(const float steer, unsigned int *addr, int size)
{
    const int value = 1024;
    float probeWidth;
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
        probeWidth = m_projectCalcPara->probeAngle;
    else
        probeWidth = m_calcPara->probeWidth;
#else
        probeWidth = m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    int dots = m_calcPara->depthDots;
    int depth = m_calcPara->depth;
    //float speed = m_calcPara->soundSpeedTsi;
    float interval = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1));

    int correct = 0;
    if (steer < 0.0)
        correct = - 0.5;
    else
        correct = 0.5;
#ifdef EMP_355
    int line = (depth / (float)dots * tan(steer) * 2.0 / interval * value + correct);
#else
    int line = (depth / (dots * INIT_SCALE) * tan(steer) * 2.0 / interval * value + correct);
#endif
    int dot = 1.0 / cos(steer) * value + 0.5;
    int start = 2;
    PRINTF("--- line = %d, dot = %d, sampStart = %d\n", line, dot, start);

    int i;
    int cntLine = 0;
    int cntDot = 0;
    int lineOffset;
    int dotaddrNormal;
    int dotaddrSteer;
    for (i = 0; i < size; i ++)
    {
        cntLine += line;
        cntDot += dot;

        lineOffset = cntLine / 1024 + 0.5;
        dotaddrNormal = i + start - 1;
        dotaddrSteer = cntDot/1024 + 0.5 - 1;
        *(addr + i) = ((lineOffset & 0x1ff) << 20) + (dotaddrSteer << 10) + dotaddrNormal;
    }
}

void Calc2D::CFocusParaCalc(const float steer, short& startTime, short& wrOffset, short& startK)
{
    const int FREQ = SAMPLE_FREQ; //mhz
    float probeWidth;
    int probeArray = m_calcPara->probeArray;
    float speed = m_calcPara->soundSpeedTsi;
    float probeR;
#ifdef EMP_PROJECT
   if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeR = m_projectCalcPara->probeR;
        probeWidth = m_projectCalcPara->probeAngle;;
    }
    else
    {
        probeR = m_calcPara->probeR;
        probeWidth = m_calcPara->probeWidth;
    }
#else
    probeR = m_calcPara->probeR;
    probeWidth = m_calcPara->probeWidth;
#endif
    probeR = probeR /100.0;
    float focusBegin = 0;

    startTime = (int)(focusBegin * 2.0 * (float)FREQ / speed + 0.5);
#ifdef EMP_355
    wrOffset = 10 + probeR * (1.0 - cos(fabs(steer))) * 1000 / speed / (float)NS_PER_UNIT_RECIVE;
#else
     wrOffset = 10 + probeR * (1.0 - cos(fabs(steer))) * 1000 / speed / (float)NS_PER_UNIT;
#endif
    startK = 0;
    PRINTF("---width = %d\n", probeWidth);
    PRINTF("---steer = %f, focusBegin = %f, startTime = %d, wrOffset = %d, startK = %d\n", steer, focusBegin, startTime, wrOffset, startK);
}

void Calc2D::LFocusParaCalc(const float steer, short& startTime, short& wrOffset, short& startK)
{
    const int FREQ = SAMPLE_FREQ; //mhz
    float probeWidth;
#ifdef EMP_PROJECT
 if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeWidth = m_projectCalcPara->probeAngle;;
    }
    else
        probeWidth = m_calcPara->probeWidth;
#else
        probeWidth = m_calcPara->probeWidth;
#endif
    int probeArray = m_calcPara->probeArray;
    float speed = m_calcPara->soundSpeedTsi;
    float interval = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1));
    float focusBegin = interval * APERTURE_HALF * 1 * sin(fabs(steer)) + 0.3; //mm

    startTime = (int)(focusBegin * 2.0 * (float)FREQ / speed + 0.5);
    if (fabs(steer) < ZERO)
        wrOffset = 10;
    else
#ifdef EMP_355
        wrOffset = 20 + APERTURE_HALF * interval / sin(fabs(steer))  * (1.0 - cos(fabs(steer))) * 1000 / speed / (float)NS_PER_UNIT_RECIVE;
#else
         wrOffset = 20 + APERTURE_HALF * interval / sin(fabs(steer))  * (1.0 - cos(fabs(steer))) * 1000 / speed / (float)NS_PER_UNIT;
#endif
    startK = 0;
    PRINTF("---width = %d\n", probeWidth);
    PRINTF("---steer = %f, focusBegin = %f, startTime = %d, wrOffset = %d, startK = %d\n", steer, focusBegin, startTime, wrOffset, startK);
}

void Calc2D::DynamicDemodCalc_test(float freq[5], int freqDepth[5], bool enableHarmonic, int& cycleFilterChg, unsigned short sinStep[64])
{
    float speed = m_calcPara->soundSpeed;
    int depth = m_calcPara->depth;
    int i;
    int temp;
    float freqDemod;
    for (i = 0; i < FREQ_SECTION; i ++)
    {
        temp = i * depth / FREQ_SECTION;
#if 0
        if (temp < freqDepth[0])
            freqDemod = freq[0] - (temp-0)*(freq[0]-freq[1])/(freqDepth[0]-0);
        else if (temp <freqDepth[1])
            freqDemod = freq[1] - (temp-freqDepth[0])*(freq[1]-freq[2])/(freqDepth[1]-freqDepth[0]);
        else if (temp <freqDepth[2])
            freqDemod = freq[2] - (temp-freqDepth[1])*(freq[2]-freq[3])/(freqDepth[2]-freqDepth[1]);
        else if (temp <freqDepth[3])
            freqDemod = freq[3] - (temp-freqDepth[2])*(freq[3]-freq[4])/(freqDepth[3]-freqDepth[2]);
        else
            freqDemod = freq[4];
#endif

        if (temp < freqDepth[0])
            freqDemod = freq[0];
        else if (temp < freqDepth[1])
            freqDemod = freq[0] - (temp-freqDepth[0])*(freq[0]-freq[1])/(freqDepth[1]-freqDepth[0]);
        else if (temp <freqDepth[2])
            freqDemod = freq[1] - (temp-freqDepth[1])*(freq[1]-freq[2])/(freqDepth[2]-freqDepth[1]);
        else if (temp <freqDepth[3])
            freqDemod = freq[2] - (temp-freqDepth[2])*(freq[2]-freq[3])/(freqDepth[3]-freqDepth[2]);
        else if (temp <freqDepth[4])
            freqDemod = freq[3] - (temp-freqDepth[3])*(freq[3]-freq[4])/(freqDepth[4]-freqDepth[3]);
        else
            freqDemod = freq[4];

        sinStep[i] = 1024*4*freqDemod/SAMPLE_FREQ+0.5;
        m_freqStep[i] = sinStep[i];
    }

    cycleFilterChg = depth*2*SAMPLE_FREQ/speed/64 + 0.5;
    if(cycleFilterChg > 511) cycleFilterChg = 511;

}
void Calc2D::DynamicDemodCalc(float freq[5], int freqDepth[5], bool enableHarmonic, int& cycleFilterChg, unsigned short sinStep[64])
{
    float speed = m_calcPara->soundSpeed;
    int depth = m_calcPara->depth;
    int i;
    int temp;
    float freqDemod;
#if 0
    if (m_calcPara->harmonic && enableHarmonic)
    {
        //printf("\n");
        freqDemod = m_calcPara->freq.emit / (float)20;
        //printf("thi is on sinStep\n");
        //printf("freqDemod:%.1f\n", freqDemod);
        for (i = 0; i < FREQ_SECTION; i ++)
        {
            sinStep[i] = 1024*4*freqDemod*2/SAMPLE_FREQ +0.5;
            m_freqStep[i] = sinStep[i];// / 2;
            //printf("%d, ", sinStep[i]);
        }
        //printf("\n");
    }
    else
    {
        //printf("\n");
        //printf("thi is off sinStep\n");
        for (i = 0; i < FREQ_SECTION; i ++)
        {
            temp = i * depth / FREQ_SECTION;
            if (temp < freqDepth[0])
                freqDemod = freq[0] - (temp-0)*(freq[0]-freq[1])/(freqDepth[0]-0);
            else if (temp <freqDepth[1])
                freqDemod = freq[1] - (temp-freqDepth[0])*(freq[1]-freq[2])/(freqDepth[1]-freqDepth[0]);
            else if (temp <freqDepth[2])
                freqDemod = freq[2] - (temp-freqDepth[1])*(freq[2]-freq[3])/(freqDepth[2]-freqDepth[1]);
            else if (temp <freqDepth[3])
                freqDemod = freq[3] - (temp-freqDepth[2])*(freq[3]-freq[4])/(freqDepth[3]-freqDepth[2]);
            else
                freqDemod = freq[4];

            sinStep[i] = 1024*4*freqDemod/SAMPLE_FREQ+0.5;
            m_freqStep[i] = sinStep[i];
            //printf("%d, ", sinStep[i]);
        }

        //printf("\n");
    }
#endif
    for (i = 0; i < FREQ_SECTION; i ++)
    {
        temp = i * depth / FREQ_SECTION;

#if 0
        if (temp < freqDepth[0])
            freqDemod = freq[0] - (temp-0)*(freq[0]-freq[1])/(freqDepth[0]-0);
        else if (temp <freqDepth[1])
            freqDemod = freq[1] - (temp-freqDepth[0])*(freq[1]-freq[2])/(freqDepth[1]-freqDepth[0]);
        else if (temp <freqDepth[2])
            freqDemod = freq[2] - (temp-freqDepth[1])*(freq[2]-freq[3])/(freqDepth[2]-freqDepth[1]);
        else if (temp <freqDepth[3])
            freqDemod = freq[3] - (temp-freqDepth[2])*(freq[3]-freq[4])/(freqDepth[3]-freqDepth[2]);
        else
            freqDemod = freq[4];
#endif

        if (temp < freqDepth[0])
            freqDemod = freq[0];
        else if (temp < freqDepth[1])
            freqDemod = freq[0] - (temp-freqDepth[0])*(freq[0]-freq[1])/(freqDepth[1]-freqDepth[0]);
        else if (temp <freqDepth[2])
            freqDemod = freq[1] - (temp-freqDepth[1])*(freq[1]-freq[2])/(freqDepth[2]-freqDepth[1]);
        else if (temp <freqDepth[3])
            freqDemod = freq[2] - (temp-freqDepth[2])*(freq[2]-freq[3])/(freqDepth[3]-freqDepth[2]);
        else if (temp <freqDepth[4])
            freqDemod = freq[3] - (temp-freqDepth[3])*(freq[3]-freq[4])/(freqDepth[4]-freqDepth[3]);
        else
            freqDemod = freq[4];

        sinStep[i] = 1024*4*freqDemod/SAMPLE_FREQ+0.5;
        m_freqStep[i] = sinStep[i];
    }

    cycleFilterChg = depth*2*SAMPLE_FREQ/speed/64 + 0.5;
    if(cycleFilterChg > 511) cycleFilterChg = 511;
}

void Calc2D::CEmitDelayCalcExtended(float probeFocPos, const int CH_NUM[], int size, unsigned short delay[MAX_LINES * APERTURE_HALF * 2])
{
    unsigned short delayTemp[APERTURE_HALF * 2];
    int i;
    float activeMaxLength, activeLength, perAngle;
    int sendCh = APERTURE_HALF * 2;
    int noSendCh = 4;
    ///> extern para used in calc计算中使用的外部参数
    float probeR;
    float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeWidth = m_projectCalcPara->probeAngle;;
        probeR = m_projectCalcPara->probeR;;
    }
     else
     {
         probeR = m_calcPara->probeR;
     probeWidth = m_calcPara->probeWidth;
     }
#else
         probeR = m_calcPara->probeR;
     probeWidth = m_calcPara->probeWidth;
#endif
     probeR = probeR /100.0;
    int probeArray = m_calcPara->probeArray;
    double speed = m_calcPara->soundSpeedTsi / 1000;
    float focPos = probeFocPos;
    int temp;

   //计算实际发射孔径
    perAngle = ((float)probeWidth / 100.0) / ((float)probeArray-1) * PI / 180.0;

#ifdef EMP_PROJECT

    int sendChNum;
    float F_num_Tx = 5.0;

    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

            F_num_Tx =  m_projectCalcPara->txFNum;;

    }
    else
    {
        if((int)probeR <= 30)
        {
            F_num_Tx = 5.0;
            //  sendCh = (int)(4 * sqrt(1 + focPos * focPos / (5 * 5 + 1)));    //双曲线
            //  if(sendCh > APERTURE_HALF * 2)  sendCh = APERTURE_HALF * 2;
        }
        else
        {
            F_num_Tx = 5.0;
            //  sendChNum = (focPos + 1) / 10;
            //  if(sendChNum > 19)
            //      sendChNum = 19;
            //  ASSERT(sendChNum < size);
            //      sendCh = CH_NUM[sendChNum];
        }
    }
    //  noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

//  float F_num_Tx = 4.0;
    printf("tx_f_num = %3.2f\n", F_num_Tx);
    float AptSize_mm = focPos / F_num_Tx;
    int   AptSize_ele = AptSize_mm/probeR/perAngle;

    sendCh = AptSize_ele;
    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    float extAngle = m_calcPara->extendedAngle / 100 * PI / 180;
    float lineAngle;
    float aptCenterAngle;
    float aptLineOffset;
    for (int lineNo = 0; lineNo < MAX_LINES; lineNo ++)
    {
        lineAngle = (lineNo - (float)(MAX_LINES - 1) / 2.0) * extAngle / (MAX_LINES - 1);
        aptCenterAngle = (lineNo - (float)(MAX_LINES - 1) / 2.0) * (probeWidth / 100.0) / (MAX_LINES - 1) * PI / 180;
        aptLineOffset = lineAngle - aptCenterAngle;

        float b;
        int mbp = m_calcPara->mbp;
        if(mbp>1)
        {
            b = (float)(( APERTURE_HALF  - 0.5 )* perAngle) - aptLineOffset;
        }
        else
        {
            b = (float)(( APERTURE_HALF  - 0.25 )* perAngle) - aptLineOffset;
        }

        activeMaxLength = focPos;
        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength = sqrt((double)(4 * probeR * (probeR + focPos) * sin(b/2) * sin(b/2) + focPos * focPos));
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
            temp += 100+160-40;
            if(temp < 0)    temp = 0;
            //if(temp > 255)    temp = 255;

            if ((lineNo % 2) == 1)
                delayTemp[i] = (unsigned char)temp;
            else
                delayTemp[APERTURE_HALF * 2 - 1 - i] = (unsigned char)temp;

            b = b - perAngle;
        }

        for(i = 0; i < noSendCh; i++)
        {
            delayTemp[i] = 0;
            delayTemp[APERTURE_HALF * 2 - i - 1] = 0;
        }

        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            delay[lineNo*APERTURE_HALF*2+i] = delayTemp[i];
        }

    }

#else
    int sendChNum;
    if((int)probeR <= 30)
    {
        sendCh = (int)(4 * sqrt(1 + focPos * focPos / (5 * 5 + 1)));    //双曲线
        if(sendCh > APERTURE_HALF * 2)  sendCh = APERTURE_HALF * 2;
    }
    else
    {
        sendChNum = (focPos + 1) / 10;
        if(sendChNum > 19)
            sendChNum = 19;
        ASSERT(sendChNum < size);
        sendCh = CH_NUM[sendChNum];
    }

    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    float extAngle = m_calcPara->extendedAngle / 100 * PI / 180;
    float lineAngle;
    float aptCenterAngle;
    float aptLineOffset;
    for (int lineNo = 0; lineNo < MAX_LINES; lineNo ++)
    {
        lineAngle = (lineNo - (float)(MAX_LINES - 1) / 2.0) * extAngle / (MAX_LINES - 1);
        aptCenterAngle = (lineNo - (float)(MAX_LINES - 1) / 2.0) * (probeWidth / 100.0) / (MAX_LINES - 1) * PI / 180;
        aptLineOffset = lineAngle - aptCenterAngle;

        float b;
        int mbp = m_calcPara->mbp;
        if(mbp>1)
        {
            b = (float)(( APERTURE_HALF  - 0.5 )* perAngle) - aptLineOffset;
        }
        else
        {
            b = (float)(( APERTURE_HALF  - 0.25 )* perAngle) - aptLineOffset;
        }

        activeMaxLength = focPos;
        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength = sqrt((double)(4 * probeR * (probeR + focPos) * sin(b/2) * sin(b/2) + focPos * focPos));
#ifdef EMP_355
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
#else
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
#endif
            temp += 100+160-40;
            if(temp < 0)    temp = 0;
            //if(temp > 255)    temp = 255;

            if ((lineNo % 2) == 1)
                delayTemp[i] = (unsigned char)temp;
            else
                delayTemp[APERTURE_HALF * 2 - 1 - i] = (unsigned char)temp;

            b = b - perAngle;
        }

        for(i = 0; i < noSendCh; i++)
        {
            delayTemp[i] = 0;
            delayTemp[APERTURE_HALF * 2 - i - 1] = 0;
        }

        for( i = 0; i < APERTURE_HALF * 2; i++ )
        {
            delay[lineNo*APERTURE_HALF*2+i] = delayTemp[i];
        }

    }
#endif
}

void Calc2D::CReceiveDelayCalcExtended(int An[MAX_LINES * APERTURE_HALF * 2], short Bn[MAX_LINES * APERTURE_HALF * 2])
{
    const int FREQ = SAMPLE_FREQ; //Mhz
    float offsetToCenter = 0.0;
    float b, absb, perAngle, temp;
    double speed = m_calcPara->soundSpeedTsi;
    float probeR;
    float probeWidth;
#ifdef EMP_PROJECT
  if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeWidth = m_projectCalcPara->probeAngle;;
        probeR = m_projectCalcPara->probeR;
    }
     else
     {
         probeR = (float)m_calcPara->probeR;
         probeWidth = m_calcPara->probeWidth;
     }
#else
         probeR = (float)m_calcPara->probeR;
         probeWidth = m_calcPara->probeWidth;
#endif
  probeR = probeR /100.0;
     int probeArray = m_calcPara->probeArray;
     perAngle = (((float)probeWidth) / 100.0) / ((float)(probeArray - 1)) * PI / 180.0;

    int lineNo;
    int i;
    int delayFAn[APERTURE_HALF * 2];
    short delayFBn[APERTURE_HALF * 2];
    float extAngle = m_calcPara->extendedAngle / 100 * PI / 180;
    float lineAngle;
    float aptCenterAngle;
    float aptLineOffset;

    for (lineNo = 0; lineNo < MAX_LINES; lineNo ++)
    {
        lineAngle = (lineNo - (float)(MAX_LINES - 1) / 2.0) * extAngle / (MAX_LINES - 1);
        aptCenterAngle = (lineNo - (float)(MAX_LINES - 1) / 2.0) * (probeWidth / 100.0) / (MAX_LINES - 1) * PI / 180;
        aptLineOffset = lineAngle - aptCenterAngle;
        //if (lineNo == 0)

        if (lineNo%2==0)
            offsetToCenter = 0.75;
        else
            offsetToCenter = 0.25;
        b = ( APERTURE_HALF - offsetToCenter) * perAngle - aptLineOffset;

        for (i = 0; i < APERTURE_HALF * 2; i ++)
        {
            absb = fabs(b);
            temp = 4.0 * probeR * probeR * sin(absb/2) * sin(absb/2) * ((float)FREQ/speed) * ((float)FREQ/speed);
            delayFAn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            temp = 4.0 * probeR * sin(absb/2) * sin(absb/2) * ((float)FREQ/speed);
            delayFBn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            b = b - perAngle;
        }

        memcpy(An+lineNo*APERTURE_HALF*2, delayFAn, sizeof(delayFAn));
        memcpy(Bn+lineNo*APERTURE_HALF*2, delayFBn, sizeof(delayFBn));

#if 0
        if (lineNo == 0)
        {
            for( i = 0; i < APERTURE_HALF * 2; i++ )
            {
                PRINTF("receive An[%d] = %d, Bn = %d\n", i, An[lineNo*APERTURE_HALF*2+i], Bn[lineNo*APERTURE_HALF*2+i]);
            }
        }
#endif
    }
}

void Calc2D::LEmitDelayCalcExtended(float probeFocPos, float steerAngle, const int CH_NUM[], int chSize, unsigned short delay[MAX_LINES * APERTURE_HALF * 4])
{
    int i;
    float activeMaxLength, activeLength, interval;
    int sendCh = APERTURE_HALF * 2;
    int noSendCh = 4;

    ///> extern para used in calc计算中使用的外部参数

    float  probeWidth;
#ifdef EMP_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {
        probeWidth =(float) m_projectCalcPara->probeAngle/100.0;
    }
    else
        probeWidth =(float) m_calcPara->probeWidth/100.0;
#else
        probeWidth =(float) m_calcPara->probeWidth/100.0;
#endif
    int probeArray = m_calcPara->probeArray;
    double speed = m_calcPara->soundSpeedTsi / 1000;
    //float steer = steerAngle;
    //float focPos = probeFocPos / cos(fabs(steer));
    float focPos = probeFocPos;
    //calc emit delay计算实际发射孔径
    float sqFocPos = focPos * focPos;
    interval = (float)((float)probeWidth) / ((float)(probeArray - 1));

    int temp;
    float b;
    int sendChNum;
#ifdef EMP_PROJECT

   float F_num_Tx = 5.0;

    if(ViewSuperuser::GetInstance()->GetProjectModeStatus())
    {

            F_num_Tx =  m_projectCalcPara->txFNum;;

    }

//  float F_num_Tx = 4.0;
    printf("tx_f_num = %3.2f\n", F_num_Tx);
    float AptSize_mm = focPos / F_num_Tx;
    int   AptSize_ele = AptSize_mm/interval;

    sendCh = AptSize_ele;

    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    //int mbp;
    //int ofset2center;
    activeMaxLength = focPos;

    int lines = m_calcPara->probeLines;
    float angle = fabs(m_calcPara->tpSteer) * PI / 180;
    float widthHalf = probeWidth / 2.0;
    float oHeight = widthHalf / tan(angle);
    float pos2center;
    float tpSteer;

    for (int lineNo = 0; lineNo < MAX_LINES; lineNo ++)
    {
        pos2center = 0 - widthHalf + lineNo * probeWidth / (lines - 1);

        if (fabs(angle) == ZERO)
            tpSteer = 0.0;
        else
            tpSteer = atan(pos2center/oHeight);

        b = (float)(( APERTURE_HALF  - 0.5 )*interval);
        for( i =  0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength= (float)sqrt ( (float) ( sqFocPos + b * b - 2 * focPos * b * sin(tpSteer) ) );
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
            temp += 120;

            if(temp < 0)    temp = 0;
            if(temp > 1000) temp = 0;   //255;
            delay[lineNo * APERTURE_HALF*2 + APERTURE_HALF * 2 - 1 - i] = temp;
            b = b - interval;
        }

#if 0
        if (lineNo == 0)
        {
            for( i = 0; i < APERTURE_HALF * 2; i++ )
            {
                printf("delay[%d] = %d\n", i, delay[lineNo * APERTURE_HALF*2 + i]);
            }
        }
#endif
#if 1
        for(i=0; i<noSendCh; i++)
        {
            delay[lineNo*APERTURE_HALF*2+i] = 0;
            delay[lineNo*APERTURE_HALF*2+APERTURE_HALF*2-i-1] = 0;
        }
#endif
    }

#else

    for(i = 0; i < APERTURE_HALF*2; i++)
    {
        delay[i] = 0;
    }

    sendChNum = (focPos + 1) / 10;

    if(sendChNum > 19)
        sendChNum = 19;

    if (sendChNum > (chSize - 1))
    {
        sendCh = 64;
        PRINTF("send ch num no = %d \n", sendChNum);
    }
    else
        sendCh = CH_NUM[sendChNum];
    noSendCh = (APERTURE_HALF * 2 - sendCh) / 2;

    //int mbp;
    //int ofset2center;
    activeMaxLength = focPos;

    int lines = m_calcPara->probeLines;
    float angle = fabs(m_calcPara->tpSteer) * PI / 180;
    float widthHalf = probeWidth / 2.0;
    float oHeight = widthHalf / tan(angle);
    float pos2center;
    float tpSteer;

    for (int lineNo = 0; lineNo < MAX_LINES; lineNo ++)
    {
        pos2center = 0 - widthHalf + lineNo * probeWidth / (lines - 1);

        if (fabs(angle) == ZERO)
            tpSteer = 0.0;
        else
            tpSteer = atan(pos2center/oHeight);

        b = (float)(( APERTURE_HALF  - 0.5 )*interval);
        for( i =  0; i < APERTURE_HALF * 2; i++ )
        {
            activeLength= (float)sqrt ( (float) ( sqFocPos + b * b - 2 * focPos * b * sin(tpSteer) ) );
#ifdef EMP_355
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT_EMIT );
#else
            temp = ( ( (activeMaxLength - activeLength ) / speed) / NS_PER_UNIT );
#endif
            temp += 120;

            if(temp < 0)    temp = 0;
            if(temp > 1000) temp = 0;   //255;
            delay[lineNo * APERTURE_HALF*2 + APERTURE_HALF * 2 - 1 - i] = temp;
            b = b - interval;
        }

#if 0
        if (lineNo == 0)
        {
            for( i = 0; i < APERTURE_HALF * 2; i++ )
            {
                printf("delay[%d] = %d\n", i, delay[lineNo * APERTURE_HALF*2 + i]);
            }
        }
#endif
#if 1
        for(i=0; i<noSendCh; i++)
        {
            delay[lineNo*APERTURE_HALF*2+i] = 0;
            delay[lineNo*APERTURE_HALF*2+APERTURE_HALF*2-i-1] = 0;
        }
#endif
    }
#endif
}

void Calc2D::LReceiveDelayCalcExtended(float steerAngle, int An[MAX_LINES * APERTURE_HALF * 2], short Bn[MAX_LINES * APERTURE_HALF * 2])
{
    const int FREQ = SAMPLE_FREQ;
    float offsetToCenter = 0.0;
    float b, interval, temp;
    double speed = m_calcPara->soundSpeedTsi;
    float probeWidth;
        probeWidth =(float) m_calcPara->probeWidth/ 100.0;
    int probeArray = m_calcPara->probeArray;
    interval = (((float)probeWidth)) / ((float)(probeArray - 1));
    int lineNo;
    int i;
    int delayFAn[APERTURE_HALF * 2];
    short delayFBn[APERTURE_HALF * 2];

    int lines = m_calcPara->probeLines;
    float angle = fabs(m_calcPara->tpSteer) * PI / 180;
    float widthHalf = probeWidth / 2.0;
    float oHeight = widthHalf / tan(angle);
    float pos2center;
    float tpSteer;

    for (lineNo = 0; lineNo < MAX_LINES; lineNo ++)
    {
        pos2center = 0 - widthHalf + ((MAX_LINES + lineNo - 1) % MAX_LINES) * probeWidth / (lines - 1);

        if (fabs(m_calcPara->tpSteer) == ZERO)
            tpSteer = 0.0;
        else
            tpSteer = atan(pos2center/oHeight);

        if (lineNo%2==0)
            offsetToCenter = 0.75;
        else
            offsetToCenter = 0.25;

        b = ( APERTURE_HALF - offsetToCenter) * interval;

        for (i = 0; i < APERTURE_HALF * 2; i ++)
        {
            temp =  ((float)FREQ/speed) * b * ((float)FREQ/speed) * b;
            delayFAn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            temp =  -2.0 * ((float)FREQ/speed) * b * sin(tpSteer);
            delayFBn[APERTURE_HALF*2-1-i] = (int)(temp + 0.5);

            b = b - interval;
        }

        memcpy(An+lineNo*APERTURE_HALF*2, delayFAn, sizeof(delayFAn));
        memcpy(Bn+lineNo*APERTURE_HALF*2, delayFBn, sizeof(delayFBn));
    }
}

void Calc2D::CompoundRate(void)
{
    // coef
    int compoundRate0=8;
    int compoundRate1=6;
    int compoundRate2=6;
    int compoundSubValue=12;
    int angle = m_calcPara->compoundAngle;

    if (angle < 5)
    {
        compoundRate0=6;
        compoundRate1=5;
        compoundRate2=5;
        compoundSubValue=2;
    }
    else if (angle < 10)
    {
        compoundRate0=7;
        compoundRate1=6;
        compoundRate2=6;
        compoundSubValue=4;
    }
    else
    {
        compoundRate0=7;
        compoundRate1=7;
        compoundRate2=7;
        compoundSubValue=6;

    }
    m_fpga.SendSpaceCompoundCoef(compoundRate0);
    m_fpga.SendSpaceCompound1Coef(compoundRate1);
    m_fpga.SendSpaceCompound2Coef(compoundRate2);
    m_fpga.SendSpaceCompoundBase(compoundSubValue);
}

void Calc2D::CalcDynamicRange(int index)
{
    const int SIZE = 8192;
    unsigned char logCurve[SIZE];

    if (index == 7)
            Log(2.8, SIZE, logCurve);
        else
            Log(m_logPara[index].dynamicRange, m_logPara[index].gain, SIZE, logCurve);

    m_fpga.SendLogAddrData(SIZE / sizeof(unsigned short), (unsigned short* )logCurve);

    //dynamic range
    //voltage reference
    m_fpga.SendDynamicRange(DYNAMIC_DATA[index]);
}

void Calc2D::CalcVoltageRefer(int voltage)
{
    //voltage reference
    m_fpga.SendDynamicRange(voltage);
}

#if 0
gboolean PReceiveCalcCw(gpointer data)
{
    Calc2D cl;
    cl.PReceiveDelayCalcCW(Calc2D::m_delayPhase);
    return FALSE;
}

void Calc2D::PReceiveDelayCw()
{
    if (m_timer > 0)
    {
        g_source_remove(m_timer);
        m_timer = 0;
    }
    m_timer = g_timeout_add(30, PReceiveCalcCw, NULL);
}
#endif
void Calc2D::CalcDefaultDemodFd()
{
}