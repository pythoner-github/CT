#ifndef IMG_PW_H
#define IMG_PW_H

#include "imageControl/CalcPw.h"
#include "Def.h"
#include "AbsUpdatePw.h"
#include "imageControl/Img2D.h"
#include "probe/ProbeMan.h"
#include "probe/ExamItem.h"

class ImgPw
{
    public:
        //enum Operation{SUB, ADD};
        enum ESpectrumMode{SPECTRUM_PW, SPECTRUM_CW};
        static const int MAX_PRF = 25;
        static const int MAX_PRF_CW = 33;
        static const unsigned int PW_PRF[MAX_PRF]; // unit:hz
        static const unsigned int CW_PRF[MAX_PRF]; // unit:hz
        static const int MAX_WALL_FILTER = 8;
        static const unsigned int WALL_FILTER_FREQ[MAX_PRF][MAX_WALL_FILTER]; //unit: hz
        static const unsigned int WALL_FILTER_FREQ_CW[MAX_PRF_CW][MAX_WALL_FILTER]; //unit: hz
        static const float INTERVAL = 50000.0;
        static const int MAX_ANGLE = 85;

        //temp
        static const double SV_CORRECT_DIST = 3.6;//-0.8;

        ~ImgPw();

        static ImgPw* GetInstance();

        void SetCalcPw(CalcPw* calc);
        void InitProbeOptimize(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
        void InitProbe(ProbeSocket::ProbePara* ptrPara, ExamItem::ParaItem* ptrParaItem);
        void GetCurPara(ExamItem::ParaItem* ptrParaItem);

        void EnterPw();
        void EnterCw();

        // sample volume
        void ResetSv(); ///< reset sample volume
        void ResetSvAccordingColor(); ///< reset sample volume
        void ReSendSv();
        void ClearSv();
        void ReDrawSv();
        bool ChangePwSV(int offsetX, int offsetY);
        EKnobReturn ChangeSVLength(EKnobOper oper);
        void GetPwInfo(int &line, int &dotBegin, int &dotEnd, int &dotSvLen);
        void SetPwInfo(int line, int dotBegin, int dotEnd);
        void SetBalanceStatus(bool balance) { m_balance = balance; }

        void ChangeInvert();
        void ChangeSoundPower(EKnobOper oper);
        void ChangeTgcPw(int tgcY[8]); ///< change micro-tgc
        EKnobReturn ChangeGainPw(EKnobOper oper);
        EKnobReturn ChangeGainCw(EKnobOper oper);

        EKnobReturn ChangeDopplerFreq(EKnobOper oper);
        void UpdateDopplerFreq();
        EKnobReturn ChangeCorrectAngle(EKnobOper oper);
        EKnobReturn ChangeCorrectAngleFast(EKnobOper oper);
        EKnobReturn ChangeScale(EKnobOper oper);
        EKnobReturn ChangeScaleSimult2(EKnobOper oper);
        EKnobReturn ChangeScaleSimult3(EKnobOper oper);
        EKnobReturn ChangeBaseline(EKnobOper oper);
        EKnobReturn ChangeWallFilter(EKnobOper oper);
        EKnobReturn ChangeDynamicRange(EKnobOper oper);
        EKnobReturn ChangeSpeed(EKnobOper oper);
        EKnobReturn ChangeSoundVolume(EKnobOper oper);
        //EKnobReturn ChangeHPRF(EKnobOper oper);
        EKnobReturn ChangeSoundStatus(EKnobOper oper);
        EKnobReturn ChangeGrayMap(EKnobOper oper);
        EKnobReturn ChangeNoiseThreshold(EKnobOper oper);
        EKnobReturn ChangeLog(EKnobOper oper);
        EKnobReturn ChangeHPRF(bool);
        EKnobReturn ChangeSimult2(bool on);
        EKnobReturn ChangeSimult3(bool on);
        EKnobReturn ChangeLineDensity(EKnobOper oper);
        EKnobReturn ChangeSteer(EKnobOper oper);
        void ChangeMBP(void);
        void ChangeTSI(void);///< must call after ChangeTSI of Img2D
        void ChangeDepth();
        void ChangePwTis();
        void SetRGBSampleDots(int dots);
        void SetScale(int prfIndex);

        double GetScaleTime(); ///< s/pixel
        double GetScaleVel(); ///< (cm/s)/pixel
        double GetMaxVel(); ///< cm/s
        double GetScaleVelKHz(); ///< (KHz)/pixel
        double GetMaxVelKHz(); ///< KHz
        int GetPwDots();
        int GetColorFreq();
        int GetNameColorFreq();
        double GetSvSamplePos(); //unit:mm
        bool GetHPRFStatus();
        bool GetSimut3Status(void) { return m_simult3; }
        bool GetSimut2Status(void) { return m_simult2; }
        int GetBaseLineForCalc();
        int GetRealSpectrumSpeed();
        int GetRealPRF(); // unit: hz
        void CalcPwInfoDot(int posDot, int svLen, int &dotBegin, int &dotEnd, int &dotSvLen);
        int GetSteerAngle(int &currentAngle);

        // for test
        EKnobReturn ChangeSoundFilter(EKnobOper oper);
        void DelaySendPwPara(void);
        void OnPwImgCtrl(bool on);
        void OnCwImgCtrl(bool on);

        ///> special measure
        void SetScaleForSpecialMeasure(double scaleVel, double scaleTime, int baselineCalc);
        void SetSpectrumModeCW(bool isCW);
        bool GetCwImgCtrl() { return m_cwImgCtrl; }

        void ChangeDopplerSoundStatus(bool data);
        void SVCorrect(int begin, int end);
        EKnobOper GetOper(){return m_oper;}
    //--------------------- Quick Adjustment Pw ----------------------//
    void QAPwInitPara(int &prf);
    void QAPwSetPara(float prfChg, int baseline);
    void QAPwCalcEmission(int onEmission);
    int GetKnobSpectrumSpeed();
    void QAPwSetGain();
       int GetPRFValue(int prfIndex);

    private:
        static const int MAX_SPEED_INDEX = 6;
        static int SPECTRUM_SPEED[MAX_SPEED_INDEX];
        static const int MAX_GRAY_SCALE = 6;
        static const int MAX_NOISE = 121;//118 change by yangyq
        static const int MIN_NOISE = 2;//add by yangyq
        static const int MAX_BASELINE_INDEX = 8; // range is (-MAX_BASELINE, MAX_BASELINE)
        static const int MAX_BASELINE = 128;
#ifdef EMP_430
        static const int MAX_GAIN_PW = 300;
        static const int FOC_LOW_TIME = 60;//20;//60;
#elif defined EMP_355
        static const int MAX_GAIN_PW = 400;
        static const int FOC_LOW_TIME = 20;
#else
        static const int MAX_GAIN_PW = 1084;
        static const int FOC_LOW_TIME = 20;
#endif
        //static const int MAX_SOUND_VOLUME = 53;//63;
        static const int MAX_SOUND_VOLUME = 20;//43;//30;//63;
        static const int MAX_SV = 200;///<unit (mm*10)
        static const int MIN_SV = 5;///<unit (mm*10)
        static const int POS_PRF_SUM = 9;
        static const int SV_POS_PRF[2][POS_PRF_SUM];
        static const int DYNAMIC_RANGE[2];
        static const int MAX_LOG = 511;
        static const unsigned int WALL_FILTER[MAX_WALL_FILTER][7];
        static const int MAX_SIMULT2_PRF_INDEX = 10;
        static const unsigned int SIMULT2_PRF[MAX_SIMULT2_PRF_INDEX]; // unit:hz
        static const int MAX_SIMULT3_PRF_INDEX = 10;
        static const unsigned int SIMULT3_PRF[MAX_SIMULT3_PRF_INDEX]; // unit:hz
        static const int MAX_STEER = 7;
        static const int STEER_ANGLE[MAX_STEER];
        static const float PW_FOCUS[3];

        //fast correct angle
        static const int MAX_ANGLE_FAST = 3;
        static const int CORRECT_ANGLE_FAST[MAX_ANGLE_FAST];
        int m_corAngleFastIndex;

        EKnobOper m_oper;
        ImgPw();

        void DopplerFreq(int freq, EKnobReturn ret);
        void GainPw(int data);
        void GainCw(int data);
        void DynamicRange(int data, EKnobReturn ret);
        void Baseline(int index, EKnobReturn ret);
        void WallFilter(int index, EKnobReturn ret);

        void SampleVolume(int pwLine, int svPos, int svLen);
        void SendSampleVolume(int pwLine, int svPos, int svLen);
        void DrawSampleVolume(int pwLine, int svPos, int svLen);
        void SVPos(int pwLine, int svPos, int svLen);
        void SVLen(int pwLine, int svPos, int svLen, EKnobReturn ret);
        void SVCalc(int pwLine, int svPos, int svLen, int &dotBegin, int &dotEnd, bool SendFpga);
        void GetPwLineRange(int &begin, int &end);
        void GetSvPosRange(int &begin, int &end);

        void SpectrumSpeed(int speedIndex, int prfIndex, EKnobReturn ret);
        void SoundVolume(int data, EKnobReturn ret);
        void Simult2(bool on, EKnobReturn ret);
        void Simult3(bool on, EKnobReturn ret);
        void HPRF(bool on, EKnobReturn ret);
        void CorrectAngle(int data, EKnobReturn ret);
        EKnobReturn PRFHPRF(int index);
        EKnobReturn PRF(int index);
        EKnobReturn PRFSimult2(int index);
        EKnobReturn PRFSimult3(int index);
        bool CalcPRFNormal(int prf, float samplePos);
        bool CalcPRFSimult2(int prf, float samplePos);
        bool CalcPRFSimult3(int prfSimult, int samplePos);
        void CalcPRF(int prf, float samplePos);
        void WhenPRFChanged(int prfIndex);
        void SoundStatus(bool data, EKnobReturn ret);
        void GrayMap(int index, EKnobReturn ret);
        void NoiseThreshold(int data, EKnobReturn ret);
        void SoundFilter(int index, EKnobReturn ret);
        void Invert(bool on);
        void Steer(int angle, EKnobReturn ret);
        int SwitchHPRF(bool on, float svPos);
        vector<int> GetHPRFEmitPos();
        int GetSpeed(int speedIndex, int prf);
        void Log(int value, EKnobReturn ret);
        int GetPRFIndex(unsigned int prf); //unit:hz
        int GetSimult2PRFIndex(unsigned int prf); //unit:hz
        int GetSimult3PRFIndex(unsigned int prf); //unit:hz
        int CalcTraceFs(int wallFilterIndex, int prfIndex, int baseline, int angleCorrect);
        void SetTraceFs(int fs);
        void SetTraceBaseline(int baseline);
        void SetTraceDynRange(int dynRange);
        void SetTraceGain(int gain);
        void SetTraceNoiseThreshold(int noiseThreshold);
        int CalcFocPulse(int freq, int power);
      //  void SVCorrect(int begin, int end);
        //void BalanceCfmInfo(int pwLine, int pwDotBegin, int pwDotEnd);
        void BalanceCfmInfo(int pwLine, int pwSvPos, int pwSvLen);
        void FocPos(int svDotPos);
        int GetOffLineNum(int spectrumSpeed, int prf);
        int GetPwLineOneImg(int spectrumSpeed, int prf);
       int SVPosCorrect(int svPosDots);
       int GetPwFocus(float focusPos);
       int GetHPRFValue(int prfIndex);
       void UpdateSV(int line, int dotBegin, int dotEnd, vector<int> HPRFEmitPos);
       void UpdateSVClear(void);
       bool CalcPRFMaxIndex(float samplePos);

        static ImgPw* m_ptrInstance;
        AbsUpdatePw* m_ptrUpdate;
        DSCCONTROLATTRIBUTES* m_ptrDscPara;
        CalcPw* m_ptrCalc;
        CalcPw::CalcPara m_calcPara;
        Img2D* m_ptrImg2D;
        CalcTime m_ctGainPw;

        int m_tGainPw;
        bool m_HPRF; ///< status high pulse repeat freq, TRUE is on, FALSE is off
        int m_pwLine; ///< pw line
        int m_svPos; ///< position of sample volume in dot on pw line
        int m_svPosBefore; ///< position of sample volume before correct in dot
        int m_svLen; ///< length of sample volume, = real * 10, unit: mm
        int m_tgc[8]; ///< 8 tgc
        int m_gainPw; ///< gain of Pw mdoe
        int m_gainCw;
        int m_freqIndex; ///< index of current color doppler emitting frequency
        int m_angle; ///< correct angle, unit: degree
        int m_PRFIndex; ///< index of PRF value
        int m_PRFIndexBak; ///< bak when HPRF switch in on and off
        int m_PRFIndexTempBak; ///< prf index's bak, used when hprf'S prf index is changed
        int m_baselineIndex; ///<
        int m_baseline; ///< baseline's real position, relative to (y start of spectrum + spectrum / 2)
        int m_wallFilterIndex; ///< index of wall filter file
        int m_dynamicRangeIndex; ///< index of spectrum dynamic range
        int m_speedIndex; ///< speed of spectrum
        int m_soundVolumeIndex; ///< index of sound volume value
        bool m_soundStatus; ///< status of sound, 0:off, 1:no
        int m_soundFilterIndex; ///<
        int m_log; ///< log value
        bool m_invert;
        int m_grayMap;
        int m_noiseThreshould;

        int m_offSetX;
        int m_offSetY;

        double m_scaleTime; ///< s/pixel
        double m_scaleVel; ///< (mm/s)/pixel

        int m_freq[ProbeSocket::MAX_DOP_FREQ]; ///< real doppler freq * 20
        int m_freqSum; ///< doppler freq sum of current probe
        double m_depthToSample;
        int m_maxPRFIndex;
        bool m_balance;

        ///>simult 2
        bool m_simult2;
        int m_simult2PRFIndex; ///< prf index in simult 2
        int m_maxSimult2PRFIndex; ///< max prf index in simult 2

        //simult 3
        bool m_simult3;
        int m_simult3PRFIndex;
        int m_maxSimult3PRFIndex;

        // remove error data of spectrum when svn is changed
        int m_linesRemove;
        bool m_pwImgCtrl;
        bool m_cwImgCtrl;
        int m_timer;
        bool m_sendPwLine;

        ///> special measure
        double m_scaleVelSnap;
        double m_scaleTimeSnap;
        int m_baselineSnap;
        ESpectrumMode m_specMode;

        ///> steer
        int m_steerIndexBak;
        int m_steerIndex;

        ///> focus
        int m_pwFocusIndex;

        ///> test
        int m_pulseCycle;
        int m_bandPassFilterList;

        ///>test
    public:
        void ChangeBandPassFilter(EKnobOper oper);
        int GetBandPassFilterList() { return m_bandPassFilterList;}
        int GetMaxPRFIndex(){return m_maxPRFIndex;}
};

#endif