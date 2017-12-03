#ifndef ABS_CALC2D_H
#define ABS_CALC2D_H

#include "AbsUpdate2D.h"
#include "FpgaCtrl2D.h"
#include "FpgaPw.h"
#include "ProbeSocket.h"


class Calc2D
{
	public:
		enum EMODE{C_BW, C_PW, C_CFM}; //calc mode

		Calc2D();
		~Calc2D();

		///> type
		static const int FOC_MAX = 4; ///< max num of focus
#ifdef EMP_355
 	    static const float INIT_SCALE = 0.95;//1.0;
#else
        static const float INIT_SCALE = 1.0;//0.95;
#endif
		static const int MAX_DYNAMIC_INDEX = 16; ///<max index of dynamic range
		static const int CLOCK_EMIT = 100; ///<100Mhz 发射模块的工作时钟
        static const int TX = 20; ///< 发射时间, unit:us

		struct CalcPara
/*
        ///> cw
        static const int APERTURE_HALF = 32; ///< half of aperture(channel)
        static const int PHASELINE = 256;
        static const int APERTURE_PHASE = 24; //16;
        unsigned short m_delayPhase[APERTURE_HALF*2*PHASELINE];
*/
		{
			int probeArray; ///< array of probe探头阵元数
			int probeLines; ///< scan lines of probe探头扫描线数
			int probeWidth; ///< linear width/convex angle of probe凸阵角度/线阵宽度 (real width(cm) * 100), unit: mm(L)
            int probeWidthPhase; ///< width of phase
			int probeR; ///< radius of probe探头半径, unit: mm
			int focSum; ///< sum of focus
			int focPos[FOC_MAX]; ///< focus position unit:mm
			double soundSpeed; ///< ultrasound speed. unit: km/s.
			double soundSpeedTsi; ///< ultrasound speed according to current tsi type. unit: km/s.
			int depth; ///< curr depth
			int depthMax; ///< max depth of this probe
			int depthDots; ///< dots in height(成像点数)
			int imgScaleIndex; ///< level of image salce(eg: if imageScale can be 1.0 1.2 1.4 1.6, then imageScaleIndex is 2 when imageScale = 1.4)
			int imgScaleIndexMax; ///< max of image scale index
			int imgScale; ///< real image scale * 10
			int angleIndex; ///< current level of scan angle, start from 1
			int angleIndexMax; ///< max level of scan angle
			ProbeSocket::FreqPair freq; ///< real emit frequency * 20, unit:MHz
			int power;///< real sound power
			bool lineDensity; ///< real line density
			int edgeEnhance; ///< level of line edge enhancement
			bool harmonic; ///< current harmonic status
			int steerAngle; ///< steer angle of 2D linear probe
			int steerAngleColor; ///< steer angle of 2D linear probe
			int mLine; ///< number of M line
			int mSpeedIndex; ///< level of M speed
			int mbp;//value of mbp
			bool compoundSpaceCtrl;
			int compoundAngle; // angle of space compound, unit:degree
			bool compoundFreqCtrl;
            bool modeCw;
            int cwLine;
            bool tpViewCtrl;
            bool efviCtrl;
            int tpSteer;
            int extendedAngle;
            int thiFreq; ///< harmonic frequency * 20, unit: MHZ
		};

#ifdef EMP_PROJECT
	  struct ProjectCalcPara
        {
			bool onDemodParaPrintf;
			int filterSecIndex;
            float bandPassW[5];
            float txFNum;
            float wpScale;
            int d2Pluse;
            int pwPluse;
            int cfmPluse;
            int probeR;
            int probeAngle;
        };
		static struct ProjectCalcPara* m_projectCalcPara;
		void SetProjectCalcPara(struct ProjectCalcPara* calc) {m_projectCalcPara = calc;}
#endif
		///> func
		void SetCalcPara(struct CalcPara* calc) {m_calcPara = calc;}
		void GetScanRange(int range[2]); ///< get scan range of current image
		virtual void CalcDisplayScanRange(int range[2], int widthInDots); ///< get valid scan range of current display
		void Valid2DImage(void);

		void CalcFocSum(); ///<para: foucs sum, focus position, depth dots, depth, probe type(offset)
		void CalcSample(); ///<para
		int CalcMaxPeriod();
        int CalcTgcMaxperiod();
		void CalcScanRange();
		//int CalcRealScanAngle();
		float CalcRealScanAngle();
		void CalcMBP(int mbp);
        void CalcDynamicRange(int index);
        void CalcVoltageRefer(int voltage);
		void CalcLineDensity();
		void CalcEdgeEnhance();
		void CalcEnableEmit();
		double CalcScale2D();
		double CalcWidthBetweenLines();
        void CalcPhaseCtrl(bool phase);
		void CalcFreqCompoundCtrl(bool on);
		void CalcSpaceCompoundCtrl(bool on);
		void CalcFreqCompoundCoef(void);
        int CalcFPS(void);
        void CalcCwLinesDelay(void);

		///> M mode
		void CalcMLine();
		void CalcMSpeed();
		double CalcScaleM();

        //CW
        void ChangeChanelNum(int num);

		///> virtual 2D mode
		void CalcWeightingEmit(int pulseWidth);
		void CalcWeightingEmitFreqCompound(int pulseWidth, int order);
		void CalcWeightingEmitColor(int pulseWidth);
		virtual void CalcEmitDelay(); ///<para: probe r/width/array, focus position, speed, probe type
		virtual void CalcEmitDelayPw(float focPos); ///< calc PW focus emit delay, same as CalcEmitDelay
		virtual void CalcEmitDelayCfm(float focPos); ///< calc CFM focus emit delay, same as CalcEmitDelay
		virtual void CalcReceiveDelay(); ///<para: probe r/width/array, focus position, speed, probe type. delay+aperture
		virtual void CalcReceiveDelayColor(); ///< calc color(PW,CFM) receive delay, same as CalcReceiveDelay, delay+aperture
		virtual void CalcReceiveAperture(); ///< calc BW+color(PW,CFM) receive aperture only
		virtual void CalcTgc(int gain, int tgcY[],AbsUpdate2D* ptrUpdate, int section); ///<para: probe type setion: 0-b, 1-pw/m, 2-cfm
        virtual void CalcTgcDigital(int gain, int section, int maxValue);
		virtual void CalcFocPos(); ///<para: foucs sum, focus position, depth dots, depth, probe type(offset)
		virtual bool CalcFocPulse(); ///<para: power, freq, probe type
		virtual void CalcFocPulseFreqCompound(int order);
		virtual void CalcLog(); ///<para:probe type
		virtual void CalcFilter() = 0; ///< probe type, freq
        virtual void CalcDefaultDemodFd();
        virtual void CalcFilterFreqCompound(int order) = 0; ///< probe type, freq
		virtual float CalcImgStartDepth(); ///< image y start offset dot in portrait direction
		virtual float Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int powerIndex, int depthIndex);
		virtual float GetCfmTis(int dopFreq, float focPos, int prfIndex, int powerIndex, int boxLineBegin, int boxLineEnd);
		virtual float GetPwTis(int dopFreq, int prfIndex);
        virtual void CalcExtendedImagingSample(void);
        void CalcExtendedImaging(bool on);

        void EnterSingleAperture(bool on);

        //test 20140717
        void ChangeFreqDynamicFilter(float data, int segment);
        void DefaultFreqDynamicFilter(const int fc[5]);
        void ChangeFreqBPFilter_1(float freq);
        void ChangeFreqBPFilter_2(float freq);
        void ChangeFreqBPFilter_3(float freq);
        void ChangeFreqBPFilter_4(float freq);
        void ChangeFreqBPFilter_5(float freq);
        void ChangeFreqBandPassFilter(float data, int segment);
        void DefaultFreqBPFilter(const int fc[5]);
        void ChangeBPFilter(int num);
        void ChangeFdDynamicDemod_test(float data, int segment);
        void SetZeroToFdDynamicDemod_test(float data);
        void CalcPulse(int freq);
        void BandPassFilterSelect_test(float freq[5], int freqDepth[5], int freqCompound);

		//optimization test 2014.12.02
        void DefaultFreqBPFilter(const int fc1[5], const int fc2[5]);
		void BandPassFilterSelect_test(float freq1[5], float freq2[5], int freqDepth[5], int freqCompound);
		void LowPassFilterFunction(unsigned short* coef, float wc_in, int order, float fs, int windowType);

        void DynamicFilter_test(float freq[5], int freqDepth[5], int freqCompound);
        void CalcFilterChangeDepth();
        void PrintPara();

	protected:
        static const int APERTURE_HALF = 32; ///< half of aperture(channel)
        static const int PHASELINE = 256;
        static const int APERTURE_PHASE = 24; //16;

		///> extern
		static FpgaCtrl2D m_fpga;
		FpgaPw m_fpgaPW;
		static struct CalcPara* m_calcPara;

		///> tgc curve section
		static const int TGC_X[8];
        static const int TGC_CTL_DEFAULT[4];

		///> log value
		static const double LOG_VALUE_DEFAULT = 2.7;

		///> filter
		static const int DYNAMIC_FILTER_SIZE = 45056;
		static const unsigned char m_filterDynamic[30720];
        static const unsigned char m_filterMatch[30720];
        static const unsigned char m_filterMatch_harmonic_1[30720];
        static const unsigned char m_filterMatch_harmonic_2[30720];
        static const unsigned char m_filterMatch_harmonic_3[30720];
        static const unsigned char m_filterMatch_harmonic_4[30720];
        static const unsigned char m_filterMatch_harmonic_5[30720];
        static const unsigned char m_filterMatch_harmonic_6[30720];
        static const unsigned char m_filterMatch_harmonic_7[30720];
        static const unsigned char m_filterMatch_harmonic_8[30720];

        //bandpassfilter data
        static const short m_bp_compensation[32];
        float m_fcBPFilter_1;
        float m_fcBPFilter_2;
        float m_fcBPFilter_3;
        float m_fcBPFilter_4;
        float m_fcBPFilter_5;


        ///> internal
		int m_pulseCycle;
        int m_timer;

		void Tgc(const int tgcX[8], int gain, int tgcYKey[8], const int fixedTgcControl[4], AbsUpdate2D* ptrUpdate, int section);
		void TgcColor(const int tgcX[8], int gain, int tgcYKey[8], const int fixedTgcControl[4], AbsUpdate2D* ptrUpdate, int section);
		void FocChange(int start);
                void Log(double value);
		void Log(double value, int size, unsigned char *pLogCurveBuf);
		void Log(float dynamicRange, float gain, int size, unsigned char *pLogCurveBuf);

		void CEmitDelay(int focusIndex, const int CH_NUM[], int size, bool compound);
        void CEmitDelayPw(float focPos, const int CH_NUM[], int size);
        void CEmitDelayCfm(float focPos, const int CH_NUM[], int size);
		void CReceiveDelay(bool compound);
		void CReceiveDelayColor();
		void CReceiveAperture(bool compound);
		void CReceiveApertureU16(bool compound);
        void CCalcReceiveDelayCtrlParam(bool compound);
		void LEmitDelay(int focusIndex, const int CH_NUM[], int size, bool compound);
		void LEmitDelayPw(float focPos, const int CH_NUM[], int size);
        void LEmitDelayCfm(float focPos, const int CH_NUM[], int size);
		void LReceiveDelay(bool compound);
		//void LReceiveDelayColor(void);
		void LReceiveDelayColor(bool compound);
		void LReceiveAperture(bool compound);
		void LReceiveApertureU16(bool compound);
        void LCalcReceiveDelayCtrlParam(bool compound);
		void PEmitDelay(int focusIndex, const int CH_NUM[], int size);
		void PEmitDelayPw(float focPos, const int CH_NUM[], int size);
        void PEmitDelayCfm(float focPos, const int CH_NUM[], int size);
		void PReceiveDelay(bool compound);
		void PReceiveDelayColor(void);
		void PReceiveAperture(bool compound);
        void PReceiveApertureU16(bool compound);
        void PCalcReceiveDelayCtrlParam(bool compound);
		void WeightingEmit(int pulseWidth, int spanValue, unsigned char weightingValue[APERTURE_HALF * 2]);

		bool FocPulse(int power, int freqEmit);
        int  FocPulseFreqCompound(int power, int freqEmit, int order);
		void BandPassFilterSelect(int freqCompound);
		void DynamicDemod(float freq[5], int freqDepth[5], bool enableHarmonic);
		void DynamicDemodFreqCompound(float freq[5], int freqDepth[5], bool enableHarmonic, int order);
        void DynamicFilter(int freqCompound);
        void ExtendedImagingSampleC(void);
        void ExtendedImagingSampleL(void);

		virtual int GetFocIndex(float focPos);
		virtual int GetPowerIndex(int power);
		virtual int Get2DFreqReceiveIndex(int freqReceive);
		virtual int GetColorFreqIndex(int freqColor);
		virtual int GetCfmLinesIndex(int boxLineBegin, int boxLineEnd);
		int Get2DScanAngleIndex(int scanAngleIndex);
		int Get2DDepthIndex(int depthIndex);
		int GetPwPrfIndex(int prfIndex);
		int GetCfmPrfIndex(int prfIndex);

    public:
		void DynamicDemodCalc_test(float freq[5], int freqDepth[5], bool enableHarmonic,int& cycleFilterChg, unsigned short sinStep[64]);
        void DynamicDemod_test(float freq[5], int freqDepth[5], bool enableHarmonic);
        ///>dynamic range
		typedef struct tagLogPair
		{
			int dynamicRange;
			int gain;
		} LogPair;
		static const LogPair m_logPara[MAX_DYNAMIC_INDEX];
                static const int DYNAMIC_DATA[MAX_DYNAMIC_INDEX];

    private:
        //test
		///> calc focus delay (emit and recevie), and aperture(receive)
		static const float NS_PER_UNIT = 16.67; ///<60Mhz
		static const float NS_PER_UNIT_RECIVE = 20; ///<60Mhz using to calculate recive delay
		static const float NS_PER_UNIT_EMIT = 10; ///<60Mhz using to calculate emition delay
#ifdef EMP_355
		static const int SAMPLE_FREQ = 50; ///<50Mhz  AD采样时钟及解调工作时钟
#else
		static const int SAMPLE_FREQ = 60; ///<60Mhz
#endif
	//	static const int CLOCK_EMIT = 100; ///<100Mhz 发射模块的工作时钟
		static const int FOCUS_DOTS = 504; ///< dots of calc focus delay
		static const int APERTURE_DOTS = 64; ///< dots of calc aperture
		static const int MAX_UNIT = 500; ///< (int)700/NS_PER_UNIT;//最大延迟时间
		static const int APERTURE_SPAN_C[12]; ///< control calculating aperture
		static const int APERTURE_SPAN_L[12]; ///< control calculating aperture
		static const int EMIT_CH_NUM_C[20]; ///< control calculating emit delay

		///> tgc
#ifdef EMP_355
		static const int TGC_DOTS = 240;
#else
		static const int TGC_DOTS = 240;
#endif
        static const int TGC_EXTRA_DOTS = 15; //fpga访问可能会越界（240）
        unsigned char m_tgcTotal2D[TGC_DOTS + TGC_EXTRA_DOTS];
        unsigned char m_tgcFlexibel[TGC_DOTS + TGC_EXTRA_DOTS] ;
#ifdef EMP_430
        static const int TGC_FIEXD_35C60E[TGC_DOTS];
        static const int TGC_FIEXD_65C15D[TGC_DOTS];
        static const int TGC_FIEXD_65L40E[TGC_DOTS];
        static const int TGC_FIEXD_TV[TGC_DOTS];
#endif
		static const int MAX_LINES = 256;
		static const int MAX_MBP = 4;
        static const int FREQ_SECTION = 64;

        ///> single aperture
        static bool m_singleAperture;

        static const int COMPOUND_SIZE = 512;

        unsigned short m_freqStep[FREQ_SECTION];

		///> para of M mode
		int m_scanRange[2]; ///< range of scan line, between:[0] ~ [1]
		int m_speed; ///< scan speed in M mode
		int m_pulseWidth;

        ///> cw
        unsigned short m_delayPhase[APERTURE_HALF*2*PHASELINE];
        int m_channedNum;

        //test
        float m_dynamicDemodFd[5];
        float m_fcDynamicFilter[5];
		float m_fcBandPassFilter[5];
        float m_fcBandPassFilter1[5];
        float m_fcBandPassFilter2[5];

        int m_depthDynamicDemod[5];
        int m_bandPassFilterIndex;
        unsigned char m_bandPassFilter[FREQ_SECTION * 32 * 2];
        int m_depthBandPassFilter[5];
        int m_depthDynamicFilter[5];
        bool m_enableHarmonic;

        //base frequence
        float m_dynamicDemodFdBaseFreq[5];
        float m_fcDynamicFilterBaseFreq[5];
        float m_fcBandPassFilterBaseFreq[5];
	    float m_fcBandPassFilterBaseFreq1[5];
	    float m_fcBandPassFilterBaseFreq2[5];
        int m_depthDynamicDemodBaseFreq[5];

        void CEmitDelayCalc(float probeFocPos, float steerAngle, const int CH_NUM[], int size, unsigned short delay[APERTURE_HALF * 4]);
        void CReceiveDelayCalc(float steerAngle, int An[MAX_MBP * APERTURE_HALF * 2], short Bn[MAX_MBP * APERTURE_HALF * 2]);
		void CReceiveApertureCalc(float steerAngle, unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF]);
        void CReceiveApertureCalcU16(float steerAngle, short delayAperture[APERTURE_DOTS * APERTURE_HALF]);
        void CCompoundParaAdderCalc(const float steer, unsigned int *addr, int size);
		void CFocusParaCalc(const float steer, short& startTime, short& wrOffset, short& startK);
		void LEmitDelayCalc(float probeFocPos, float steerAngle, const int CH_NUM[], int chSize, unsigned short delay[APERTURE_HALF * 4]);
        void LReceiveDelayCalc(float steerAngle, int An[MAX_MBP * APERTURE_HALF * 2], short Bn[MAX_MBP * APERTURE_HALF * 2]);
		void LReceiveApertureCalc(float steerAngle, unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]);
		void LReceiveApertureCalcU16(float steerAngle, short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]);
        void LCompoundParaAdderCalc(const float steer, unsigned int *addr, int size);
		void LFocusParaCalc(const float steer, short& startTime, short& wrOffset, short& startK);
		void PEmitDelayCalc(float probeFocPos, EMODE mode, const int CH_NUM[], int chSize, unsigned short delay[APERTURE_HALF * 2 * PHASELINE]);
        void PReceiveDelayCalc(int An[APERTURE_HALF * 2 * PHASELINE], short Bn[MAX_MBP * APERTURE_HALF * 2 * PHASELINE]);
		void PReceiveApertureCalc(unsigned char delayAperture[APERTURE_DOTS * APERTURE_HALF]);
        void PReceiveApertureCalcU16(short delayAperture[APERTURE_DOTS * APERTURE_HALF * 2]);
        void PReceiveDelayCalcCW(unsigned short delayEmit[APERTURE_HALF * 2 * PHASELINE]);
        void PReceiveDelayCw();

		void CompoundParaCalc(const float steer, const float angle, int& line, int &dot, int& start);
		void DynamicDemodCalc(float freq[5], int freqDepth[5], bool enableHarmonic,int& cycleFilterChg, unsigned short sinStep[64]);
        void CompoundRate(void);

        void CEmitDelayCalcExtended(float probeFocPos, const int CH_NUM[], int size, unsigned short delay[MAX_LINES * APERTURE_HALF * 2]);
        void CReceiveDelayCalcExtended(int An[MAX_LINES * APERTURE_HALF * 2], short Bn[MAX_LINES * APERTURE_HALF * 2]);
        void LEmitDelayCalcExtended(float probeFocPos, float steerAngle, const int CH_NUM[], int size, unsigned short delay[MAX_LINES * APERTURE_HALF * 4]);
        void LReceiveDelayCalcExtended(float steerAngle, int An[MAX_MBP * APERTURE_HALF * 2], short Bn[MAX_MBP * APERTURE_HALF * 2]);
};

///> inline func
inline void Calc2D::CalcLineDensity()
{
	// send to imging system
	m_fpga.SendLineDensity(m_calcPara->lineDensity);
}
inline void Calc2D::CalcEdgeEnhance()
{
}
inline void Calc2D::CalcEnableEmit()
{
	// send command to enable emit
	m_fpga.SendBeginEmit();
}

/*
 * @brief calc scale of measure
 */
inline double Calc2D::CalcScale2D()
{
	int depth = m_calcPara->depth;
	int depthDots = m_calcPara->depthDots;
#ifdef EMP_355
	double scale = (float)depth / depthDots;
#else
	double scale = (float)depth / depthDots / INIT_SCALE;
#endif
	PRINTF("Calc2D: depth = %d    depthDots = %d         scale = %f\n", depth, depthDots, scale);

	return (scale);
}

/*
 * @brief get width between two lines
 *
 * @retval width, unit:mm
 */
inline double Calc2D::CalcWidthBetweenLines()
{
	double width = (float)m_calcPara->probeWidth / 100 / m_calcPara->probeLines;

	return width;
}

inline void Calc2D::CalcPhaseCtrl(bool phase)
{
    if (phase)
        m_fpga.SendPhaseControl(1);
    else
        m_fpga.SendPhaseControl(0);
}

inline void Calc2D::CalcMLine()
{
	//send m line
	m_fpga.SendBmLine(m_calcPara->mLine);

}
inline void Calc2D::CalcMSpeed()
{
	m_speed = 35 + 19 * m_calcPara->mSpeedIndex; //22

	//send m speed
	m_fpga.SendBmSpeed(255 - m_speed);
}

inline double Calc2D::CalcScaleM()
{
    double mCycle = (double)(m_speed * 200.0 + 150.0) / 1000 / 1000; //s

    return mCycle;
}
  void BandPassFilterFunction(unsigned short* h, float w0_in, float w1_in, int order, float fs, int windowType);
  void Ideal_lowPassFilterFunc(float *hd, float wc, int M);
  void HammingWindowFunc(float *y, int L);
#endif