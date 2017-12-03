#ifndef CALC_2D_TINY_CONVEX_H
#define CALC_2D_TINY_CONVEX_H

#include "Calc2DConvex.h"

class Calc2DTinyConvex: public Calc2DConvex
{
	public:
		Calc2DTinyConvex();
		~Calc2DTinyConvex();

        void CalcDefaultDemodFd();
		virtual void CalcEmitDelay(); 
		virtual void CalcFocPos();
		virtual void CalcFilter();
		virtual void CalcFilterFreqCompound(int order);

    protected:
		void CalcFilterBandPass(int order);
        void CalcDynamicFilter(int order);
		void CalcDynamicDemod(int order);

	private:
		static const int EMIT_CH_NUM[20]; ///< control calculating emit delay
};
#endif

