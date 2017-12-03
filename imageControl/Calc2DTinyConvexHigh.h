#ifndef CALC_2D_TINY_CONVEX_HIGH_H
#define CALC_2D_TINY_CONVEX_HIGH_H

#include "Calc2DTV.h"

class Calc2DTinyConvexHigh: public Calc2DTV
{
	public:
		Calc2DTinyConvexHigh();
		~Calc2DTinyConvexHigh();

		virtual void CalcEmitDelay(); 
		float Get2DTis(int scanAngleIndex, float focPos, int freq, int power, int depthIndex);
		float GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd);
		float GetPwTis(int dopFreq, int prfIndex);
		void CalcFilter();
		void CalcFilterFreqCompound(int order);

    protected:
		void CalcFilterBandPass(int order);
        void CalcDynamicFilter(int order);
		void CalcDynamicDemod(int order);        

	private:
		static const int EMIT_CH_NUM[20]; ///< control calculating emit delay
		static float TIS_CFM_BASIC[2][12][11]; ///< color doppler freq--focus pos's level--prf
		static float TIS_CFM_POWER[10];
		static float TIS_CFM_BOX_LINES[8];
		static float TIS_PW_BASIC[2][15]; ///< pw doppler freq--prf
		
		int GetFocIndex(float focPos);
		int Get2DFreqReceiveIndex(int freq);
		int GetColorFreqIndex(int freq);
};
#endif

