#ifndef KNOB_CFM_H
#define KNOB_CFM_H

#include "Def.h"

enum EKnobCfm{CFM_SCALE, CFM_BASELINE, CFM_WALL_FILTER, CFM_STEER, CFM_SENSITIVE, CFM_DOP_FREQ, CFM_SIMULT, CFM_FLOW_OPT, CFM_INVERT, CFM_FOC_POS, CFM_LINE_DENSITY, CFM_MAP, CFM_SCAN_LINES, CFM_TURBO};
//CFM_TISSUE, CFM_NOISE, CFM_RATE, CFM_VARIANCE, CFM_GAIN_GATE

void KnobCfmCreate();

///> knob menu need to be sync
void SyncKnobCfm(EKnobCfm type, const char* s, EKnobReturn status, bool draw=true);

///> callback func
EKnobReturn CfmChgScale(EKnobOper oper);

EKnobReturn CfmChgBaseline(EKnobOper oper);

EKnobReturn CfmChgWallFilter(EKnobOper oper);

EKnobReturn CfmChgLineDensity(EKnobOper oper);

EKnobReturn CfmChgSensitive(EKnobOper oper);

EKnobReturn CfmChgDopFreq(EKnobOper oper);

EKnobReturn CfmChgMap(EKnobOper oper);

EKnobReturn CfmChgSimult(EKnobOper oper);

EKnobReturn CfmChgTurbo(EKnobOper oper);

EKnobReturn CfmChgInvert(EKnobOper oper);

EKnobReturn CfmChgFlowOpt(EKnobOper oper);

EKnobReturn CfmChgColorFoc(EKnobOper oper);

///////////////////////////  test ///////////////////////////
EKnobReturn CfmChgTissue(EKnobOper oper);

EKnobReturn CfmChgNoise(EKnobOper oper);

EKnobReturn CfmChgRate(EKnobOper oper);

EKnobReturn CfmChgVariance(EKnobOper oper);

EKnobReturn CfmChgPktsize(EKnobOper oper);

EKnobReturn CfmChgGainGate(EKnobOper oper);

EKnobReturn CfmChgScanLines(EKnobOper oper);

#endif

