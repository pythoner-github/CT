#ifndef FREEZE_MODE_H
#define FREEZE_MODE_H

#include "AbsUpdateMix.h"
#include "Replay.h"
#include "Def.h"

class FreezeMode
{
	public:
        ///< freeze mode, REPLAY:冻结，曾经回放过，不论当前停在哪一帧; AUTOREPLAY：自动回放中，FREEZE：冻结，未曾回放过。
		enum EFreezeMode{FREEZE, UNFREEZE, REPLAY, AUTOREPLAY}; 

		static FreezeMode* GetInstance();
		~FreezeMode();
		
		void PressFreeze();
		void PressUnFreeze();


		EKnobReturn ChangeAutoReplay();
		void EnterAutoReplayView();
		void EnterAutoReplayReview();
		void ExitAutoReplay();
		void SetReplayMode();

		void EnterReplayMode();
		void ExitReplayMode();
		void Freeze();
		void UnFreeze();
		EFreezeMode GetFreezeMode() {return m_freezeMode; }
        void UpdateHospitalandpart(int date_format, const char *hospital_name);
        void FreezeEFOV();
        void UnFreezeEFOV();
        void Freeze4D();
        void UnFreeze4D();
        void SetUnFreezeForbid(bool on);
		bool GetUnFreezeForbidStatus(){return m_unFreezeForbidStatus;}

	private:
		FreezeMode();
		static FreezeMode* m_ptrInstance;

		// interface
		AbsUpdateMix* m_ptrUpdate;
		Replay* m_ptrReplay;

		EFreezeMode m_freezeMode;
	    bool m_unFreezeForbidStatus;

};
#endif
