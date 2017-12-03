#ifndef KNOB_M_H
#define KNOB_M_H

#include "Def.h"
#if ((defined (EMP_322) || defined(EMP_313)))
enum EKnobM {M_SPEED, M_MBP, M_SOUND_POWER};
#else
enum EKnobM {M_FREQ, M_SPEED, M_CHROMA};
#endif
void KnobMCreate();
void KnobAnatomicMCreate();

///> knob menu need to be sync
void SyncKnobM(EKnobM type, const char* s, EKnobReturn status, bool draw=true);

EKnobReturn MChgFreq(EKnobOper oper);
EKnobReturn MPressChgStatusTHI(void);

EKnobReturn MChgSpeed(EKnobOper oper);
EKnobReturn MPressChgSpeed(void);

EKnobReturn MChgChroma(EKnobOper oper);
EKnobReturn MPressChroma(void);

EKnobReturn MPressChgMBP(void);

EKnobReturn MPressChgSoundPower(void);


#endif
