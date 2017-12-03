#ifndef CALC_2D_TV_H
#define CALC_2D_TV_H

#include "Calc2D.h"


class Calc2DTV: public Calc2D
{
	public:
		Calc2DTV();
		~Calc2DTV();

		virtual void CalcEmitDelay(); 
		void CalcTgc(int gain, int tgcY[],AbsUpdate2D* ptrUpdate, int section);
		void CalcFocPos();
        void CalcDefaultDemodFd();
		void CalcFilter();
		void CalcFilterFreqCompound(int order);

		virtual float Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex);
		virtual float GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd);
		virtual float GetPwTis(int dopFreq, int prfIndex);

	protected:
		void CalcFilterBandPass(int order);
        void CalcDynamicFilter(int order);
		void CalcDynamicDemod(int order);

	private:
		static const int EMIT_CH_NUM[20]; ///< control calculating emit delay

		static float TIS_CFM_BASIC[2][11][11]; ///< color doppler freq--focus pos's level--prf
		static float TIS_CFM_POWER[10];
		static float TIS_CFM_BOX_LINES[8];
		static float TIS_2D_BASIC[11][8]; ///< 2d focus pos's level--scan agnle's level
		static float TIS_2D_FREQ[9];
		static float TIS_2D_POWER[10];
		static float TIS_2D_DEPTH[20];
		static float TIS_PW_BASIC[2][15]; ///< pw doppler freq--prf
		
		virtual int Get2DFreqReceiveIndex(int freq);
		virtual int GetColorFreqIndex(int freq);
};
#endif

