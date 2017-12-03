#ifndef CALC_2D_PHASE_H
#define CALC_2D_PHASE_H

#include "Calc2DConvex.h"

class Calc2DPhase: public Calc2DConvex
{
	public:
		Calc2DPhase();
		~Calc2DPhase();

		void CalcEmitDelay();
		void CalcEmitDelayPw(float focPos);
		void CalcEmitDelayCfm(float focPos);
        void CalcDefaultDemodFd();

		void CalcReceiveDelay();
		void CalcReceiveDelayColor();
        void CalcReceiveAperture();

		void CalcFilter();
		void CalcFilterFreqCompound(int order);

    protected:
		void CalcFilterBandPass(int order);
        void CalcDynamicFilter(int order);
		void CalcDynamicDemod(int order);        

    private:
		static const int EMIT_CH_NUM_P[20]; ///< control calculating emit delay

};
#endif

