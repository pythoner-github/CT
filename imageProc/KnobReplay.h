#ifndef KNOB_REPLAY_H
#define KNOB_REPLAY_H

#include "Def.h"
#if (defined (EMP_322) || defined(EMP_313))
enum EKnobReplay {REPLAY_CTRL, REPLAY_SPEED, REPLAY_TRIM_LEFT, REPLAY_TRIM_RIGHT, REPLAY_RESET_TRIM, REPLAY_CINE};
#else
enum EKnobReplay {REPLAY_CTRL, REPLAY_TRIM_LEFT, REPLAY_TRIM_RIGHT, REPLAY_RESET_TRIM, REPLAY_CINE};
#endif
void KnobReplayCreate();

void KnobCineCreate();

void SyncKnobReplay(EKnobReplay type, const char* s, EKnobReturn status, bool draw=true);

EKnobReturn ReplayChgCtrl(void);

EKnobReturn ReplayChgSpeed(EKnobOper oper);

EKnobReturn PressChgSpeed(void);

EKnobReturn ReplayChgTrimLeft(void);

EKnobReturn ReplayChgTrimRight(void);

EKnobReturn ReplayChgResetTrim(void);

EKnobReturn ReplayChgSaveCine(void);

#if (defined(EMP_460) || defined(EMP_355))
EKnobReturn ReplayChgTrimLeft460(EKnobOper);

EKnobReturn ReplayChgTrimRight460(EKnobOper);

EKnobReturn ReplayChgResetTrim460(EKnobOper);

EKnobReturn ReplayChgSaveCine460(EKnobOper);
#endif

#endif
