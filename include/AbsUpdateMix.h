#ifndef ABS_UPDATE_MIX_H
#define ABS_UPDATE_MIX_H

#include "Def.h"

class AbsUpdateMix
{
	public:
		virtual ~AbsUpdateMix() {}

		virtual void Freeze() = 0;

		virtual void UnFreeze() = 0;

		virtual void EnterMeasure() = 0;
		
		virtual void ExitMeasure() = 0;
		
		virtual void EnterCalc() = 0;

		virtual void ExitCalc() = 0;

		virtual void EnterSystem() = 0;
    
		virtual void ExitSystem() = 0;

		virtual void ReplayBar(int cur, int total, int trimLeft, int trimRight) = 0;

        virtual void ClearReplayBar() = 0;
#if (defined (EMP_322) || defined(EMP_313))
        virtual void SpeedCtrl(int indexSpeed) = 0;
#endif
        virtual void ReplayCtrl(bool on) = 0;

		virtual void ReplayCtrl(bool on, int indexSpeed, EKnobReturn status) = 0;		

		virtual void ReplayTrimLeft(int data) = 0;		
		
		virtual void ReplayTrimRight(int data) = 0;		
};
#endif
