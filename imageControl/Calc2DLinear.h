#ifndef CALC_2D_LINEAR_H
#define CALC_2D_LINEAR_H

#include "Calc2D.h"

class Calc2DLinear: public Calc2D
{
	public:
		Calc2DLinear();
		~Calc2DLinear();

		void CalcEmitDelay();
		void CalcEmitDelayPw(float focPos);
		void CalcEmitDelayCfm(float focPos);
		void CalcReceiveDelay();
		void CalcReceiveDelayColor();
        void CalcReceiveAperture();
		void CalcTgc(int gain, int tgcY[],AbsUpdate2D* ptrUpdate, int section);
		void CalcFocPos();
		bool CalcFocPulse();
        void CalcFocPulseFreqCompound(int order);
		void CalcLog();
		void CalcFilter();
        void CalcDefaultDemodFd();
		void CalcFilterFreqCompound(int order);
		float CalcImgStartDepth();
		void CalcDisplayScanRange(int range[2], int widthInDots);
		void CalcWeightingEmit(int pulseWidth);
		void CalcWeightingEmitColor(int pulseWidth);
        void CalcExtendedImagingSample(void);
		
		float Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex);
		float GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd);
		float GetPwTis(int dopFreq, int prfIndex);

	private:
		static int EMIT_CH_NUM_L[20]; ///< control calculating emit delay
		static const int EMIT_CH_NUM_55L60G[20]; ///< control calculating emit delay
		static const int EMIT_CH_NUM_70L40J[20]; ///< control calculating emit delay
		static const int EMIT_CH_NUM_10L25K[20]; ///< control calculating emit delay
        static int EMIT_CH_NUM_L_COLOR[20]; 
		static const int EMIT_CH_NUM_L_COLOR_55L60G[20]; ///< control calculating emit delay
		static const int EMIT_CH_NUM_L_COLOR_70L40J[20]; 
        static const int TGC_CTL[4];
		
		static float TIS_CFM_BASIC[2][12][11]; ///< color doppler freq--focus pos's level--prf
		static float TIS_CFM_POWER[10];
		static float TIS_CFM_BOX_LINES[8];
		static float TIS_2D_BASIC[13]; ///< 2d focus pos's level--scan agnle's level
		static float TIS_2D_FREQ[5];
		static float TIS_2D_POWER[10];
		static float TIS_2D_DEPTH[20];
		static float TIS_PW_BASIC[2][15]; ///< pw doppler freq--prf
		
		void CalcFilterBandPass(int order);
        void CalcDynamicFilter(int order);
		void CalcDynamicDemod(int order);

		int GetFocIndex(float focPos);
		int Get2DFreqReceiveIndex(int freq);
		int GetColorFreqIndex(int freqColor);
};
#endif

