#ifndef KNOB_PW_H
#define KNOB_PW_H

#include "Def.h"

enum EKnobPw {PW_SV_LEN, PW_ANGLE, PW_SCALE, PW_BASELINE, PW_STEER, PW_DOP_FREQ, PW_WALL_FILTER, PW_SPEED, PW_DYN, PW_NOISE, PW_SIMULT, PW_HPRF, PW_SOUNDSTATUS, PW_MAP, PW_LOG};
void KnobPwCreate();

///> knob menu need to be sync
void SyncKnobPw(EKnobPw type, const char* s, EKnobReturn status, bool draw=true);

///> callback func
EKnobReturn PwChgSVLength(EKnobOper oper);

EKnobReturn PwChgAngle(EKnobOper oper);

EKnobReturn PwChgAngleFast(EKnobOper oper);

EKnobReturn PwPressAngleFast(void);

EKnobReturn PwChgScale(EKnobOper oper);

EKnobReturn PwChgBaseline(EKnobOper oper);

EKnobReturn PwChgSteer(EKnobOper oper);

EKnobReturn PwChgWallFilter(EKnobOper oper);

EKnobReturn PwChgDopFreq(EKnobOper oper);

EKnobReturn PwChgSpeed(EKnobOper oper);

EKnobReturn PwChgNoise(EKnobOper oper);

EKnobReturn PwChgSimult(EKnobOper oper);

EKnobReturn PwChgDynamicRange(EKnobOper oper);

EKnobReturn PwChgMap(EKnobOper oper);

EKnobReturn PwChgHPRF(EKnobOper oper);

EKnobReturn PwChgSoundStatus(EKnobOper oper);

EKnobReturn PwChgLog(EKnobOper oper);

// for test
EKnobReturn PwChgSoundFilter(EKnobOper oper);

#endif
