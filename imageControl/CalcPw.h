#ifndef ABS_CALC_PW_H
#define ABS_CALC_PW_H

#include "FpgaPw.h"
#include "FpgaCtrl2D.h"
#include "AbsUpdate2D.h"
#include <vector>
using namespace std;

class CalcPw
{
	public:
		CalcPw();
		~CalcPw();

		struct CalcPara
		{
			int spectrumSpeed;
			int PRFIndex;
			int colorFreq; // = (real color freq * 20) MHz
			int correctAngle; // unit is degree
			int spectrumDots; // pixels in one line after DSC
		};

		void SetCalcPara(CalcPara* calc) {m_calcPara = calc;}

        void CalcGainCw(int dataCw);
		void CalcPwLine(int line);
		void CalcSvPos(int dotBegin, int dotEnd);
		void CalcNoise(int data);
		void CalcDynamicRange(int range);
		void CalcBaseline(int baseline);
		void CalcLog(int value);
		void CalcWallFilter(unsigned int *filter, int size);
		void CalcSpeed(int data);
		void CalcSoundVolume(int data);
		void CalcSoundFilter(int index);
		void CalcHPRF(int data);
		void CalcSoundStatus(int data);
		void CalcBandPassFilter(int pulseNum);
        void CalcBandPassFilter_test(int pulseNum, int bplist);
        void CalcBandPassFilter_new(int halfpulseCylce, float fc_offset);

		void CalcColorGray(int index);
		void CalcColorEmitDelay(); // calc color focus emit delay
		void CalcColorReceiveDelay();
		void CalcInvert(bool on);

		void CalcSoundInterpolation(int prf, int spectrumSpeed);
		int CalcMaxPeriod(int prf);
		int CalcMaxPeriodForHprf(int prf);
		int CalcFocPulse(int freq, int power, int &pulseWidth);

		double CalcScaleTime(int spectrumSpeed, int prf); //s/p
		double CalcScaleVel(int prf, int colorFreq, int angle, double soundSpeed, int pixles); //(m/s)/p
		double CalcMaxVel(int prf, int colorFreq, int angle, double soundSpeed); // cm/s

		void CalcHDotSample(int depth, double soundSpeed);
		void CalcHDotSampleForHprf(int depth, int prf);

		int GetMaxSoundFilter(void);
		int GetHPRFIndex(float svBegin, float svEnd);
        int SVPosPRFToHPRF(float &depthBegin, float &depthEnd, int prf, int prfIndex=0);
		vector<int> GetHPRFEmitPos(int prf, int svPos, double soundSpeed, float scale);
		int GetPulseCycleNum();

		void CalcRGBSampleDots(int dots);
		void CalcRGBSampleCoef(unsigned short* table, int size);
		void CalcSoundStopDot(int prf);
		void CalcPwImgCtrl(bool on);
		void CalcPwImgCtrlPara(int offLineNum);
		void CalcCwImgCtrl(bool on);

	private:
		static FpgaPw m_fpga;
		static FpgaCtrl2D m_fpga2D;
#if (defined(EMP_355))
		static const int MAX_SOUND_FILTER = 25+8;
        static const int SOUND_FILTER_ORDER = 254;
		static const short SOUND_FILTER[MAX_SOUND_FILTER][SOUND_FILTER_ORDER+4];
#else
		static const int MAX_SOUND_FILTER = 13;
		static const unsigned short SOUND_FILTER[MAX_SOUND_FILTER][16];
#endif
		static const int HPRF_CORRECT = 5; //us
#ifdef EMP_430
        static const int FOC_LOW_TIME = 60; //us
#else
        static const int FOC_LOW_TIME = 20;
#endif
#ifdef EMP_355
		static const int CLOCK_EMIT = 100; ///<100Mhz 发射模块的工作时钟
		static const int SAMPLE_FREQ = 50; ///<50Mhz  AD采样时钟及解调工作时钟
#else
		static const int SAMPLE_FREQ = 60; ///<50Mhz  AD采样时钟及解调工作时钟
		static const int CLOCK_EMIT = 60; ///<60Mhz 发射模块的工作时钟
#endif
		CalcPara* m_calcPara;
		int m_maxPeriod;
		int m_soundSpeed;
		int m_pulseCycle;

		//void Tgc(int tgcX[8], int gain, int tgcY[8], const int fixedTgcControl[4], AbsUpdate2D* ptrUpdate);
		double GetMaxPeriod(int prf);
		int GetHPRFEmitNum(float emitT, float recvT);
};
#endif