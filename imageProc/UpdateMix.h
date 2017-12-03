#ifndef UPDATE_MIX_H
#define UPDATE_MIX_H

#include "AbsUpdateMix.h"
#include "MenuArea.h"

class UpdateMix: public AbsUpdateMix
{
	public:
		UpdateMix();

		void Freeze();

		void UnFreeze();

		void EnterMeasure();
		
		void ExitMeasure();
		
		void EnterCalc();
		
		void ExitCalc();

		void EnterSystem(void);
		
		void ExitSystem(void);

		void ReplayBar(int cur, int total, int trimLeft, int trimRight);
		
		void ClearReplayBar();

		void ReplayCtrl(bool on );
#if (defined (EMP_322) || defined(EMP_313))
		void SpeedCtrl(int indexSpeed);
#endif
        void ReplayCtrl(bool on, int indexSpeed, EKnobReturn status);

        void ReplayTrimLeft(int data);		
		
		void ReplayTrimRight(int data);		

	private:
		MenuArea* m_ptrMenu;		
		char m_str[20];
};

#endif
