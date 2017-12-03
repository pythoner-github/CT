#ifndef CALC_2D_CONVEX_H
#define CALC_2D_CONVEX_H

#include "Calc2D.h"

class Calc2DConvex: public Calc2D
{
	public:
		Calc2DConvex();
		~Calc2DConvex();

		//virtual void CalcFocPos();
        virtual void CalcFilter();
        void CalcDefaultDemodFd();
        virtual void CalcFilterFreqCompound(int order);
		float Get2DTis(int scanAngleIndex, float focPos, int freqReceive, int power, int depthIndex);
		float GetCfmTis(int dopFreq, float focPos, int prfIndex, int power, int boxLineBegin, int boxLineEnd);
		float GetPwTis(int dopFreq, int prfIndex);
		void CalcWeightingEmit(int pulseWidth);
		void CalcWeightingEmitColor(int pulseWidth);

	protected:	
		void CalcFilterBandPass(int order);
        void CalcDynamicFilter(int order);
		void CalcDynamicDemod(int order);
	
	private:
		static float TIS_CFM_BASIC[2][11][11]; ///< color doppler freq--focus pos's level--prf
		static float TIS_CFM_POWER[10];
		static float TIS_CFM_BOX_LINES[8];
		static float TIS_2D_BASIC[11][8]; ///< 2d focus pos's level--scan agnle's level
		static float TIS_2D_FREQ[9];
		static float TIS_2D_POWER[10];
		static float TIS_2D_DEPTH[20];
		static float TIS_PW_BASIC[2][15]; ///< pw doppler freq--prf

};
#endif

