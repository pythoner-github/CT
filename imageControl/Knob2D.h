// -*- c++ -*-
#ifndef KNOB_2D_H
#define KNOB_2D_H

#include "Def.h"
#if (defined(EMP_322) || defined(EMP_313))
enum EKnobD2{ D2_ANGLE, D2_SPACE_COMPOUND, D2_FREQ_COMPOUND, D2_DYN, D2_LINE_DENSITY, D2_STEER, D2_TP_VIEW, D2_EFVI, D2_EDGE, D2_MBP, D2_SOUND_POWER, D2_SCAN_LINES, D2_POLARITY, D2_THI};
#elif (defined(EMP_460) || defined(EMP_355))
enum EKnobD2{D2_FREQ, D2_DYN, D2_FOCUS, D2_SPACE_COMPOUND, D2_CHROMA, D2_FREQ_COMPOUND, D2_ANGLE, D2_STEER, D2_TP_VIEW, D2_TSI, D2_LINE_DENSITY, D2_EDGE, D2_EFVI, D2_SCAN_LINES, D2_POLARITY, D2_SOUND_POWER,D2_THI};
#elif defined(EMP_3410)
enum EKnobD2{D2_FREQ, D2_DYN, D2_FOCUS, D2_SPACE_COMPOUND, D2_CHROMA, D2_ANGLE, D2_STEER, D2_TP_VIEW, D2_TSI, D2_LINE_DENSITY, D2_EDGE, D2_EFVI, D2_SCAN_LINES, D2_POLARITY, D2_FREQ_COMPOUND, D2_THI};
#else
//enum EKnobD2{D2_FREQ, D2_ANGLE, D2_SPACE_COMPOUND, D2_FREQ_COMPOUND, D2_CHROMA, D2_DYN, D2_THI, D2_TSI, D2_LINE_DENSITY, D2_EDGE, D2_FOCUS};
//enum EKnobD2{D2_FREQ, D2_ANGLE, D2_SPACE_COMPOUND, D2_FREQ_COMPOUND, D2_CHROMA, D2_DYN, D2_STEER, D2_TP_VIEW, D2_EFVI,D2_TSI, D2_LINE_DENSITY, D2_EDGE, D2_FOCUS, D2_SCAN_LINES, D2_POLARITY, D2_THI};
enum EKnobD2{D2_FREQ, D2_DYN, D2_FOCUS, D2_SPACE_COMPOUND, D2_CHROMA, D2_FREQ_COMPOUND, D2_ANGLE, D2_STEER, D2_TP_VIEW, D2_TSI, D2_LINE_DENSITY, D2_EDGE, D2_EFVI, D2_SCAN_LINES, D2_POLARITY, D2_THI};
#endif
void KnobD2Create();

///> knob menu need to be sync
void SyncKnobD2(EKnobD2 type, const char* s, EKnobReturn status, bool draw=true);

void SyncKnobD2StatusTHI(EKnobD2 type, const char* s, bool draw=true);

///> callback func
EKnobReturn D2ChgFreq(EKnobOper oper);

EKnobReturn D2PresssChgStatusTHI(void);

EKnobReturn D2ChgPwr(EKnobOper oper);
#if 0
EKnobReturn D2ChgScanAngle(EKnobOper oper);

EKnobReturn D2ChgDynamicRange(EKnobOper oper);

EKnobReturn D2ChgChroma(EKnobOper oper);
EKnobReturn D2PressChroma(void);

EKnobReturn D2ChgTHI(EKnobOper oper);

EKnobReturn D2ChgTSI(EKnobOper oper);

EKnobReturn D2ChgLineDensity(EKnobOper oper);

EKnobReturn D2ChgAGC(EKnobOper oper);

EKnobReturn D2ChgMBP(EKnobOper oper);

EKnobReturn D2ChgEdge(EKnobOper oper);

EKnobReturn D2ChgSteer(EKnobOper oper);

EKnobReturn D2PressFocus(void);

EKnobReturn D2ChgSpaceCompound(EKnobOper oper);

EKnobReturn D2ChgFreqCompound(EKnobOper oper);
#endif


EKnobReturn D2PressChgScanAngle(void);
EKnobReturn D2ChgScanAngle(EKnobOper oper);

EKnobReturn D2PressChgDynamicRange(void);
EKnobReturn D2ChgDynamicRange(EKnobOper oper);

EKnobReturn D2ChgChroma(EKnobOper oper);
EKnobReturn D2PressChroma(void);

EKnobReturn D2PressChgTHI(void);
EKnobReturn D2ChgTHI(EKnobOper oper);

EKnobReturn D2ChgTSI(EKnobOper oper);

EKnobReturn D2PressChgLineDensity(void);
EKnobReturn D2ChgLineDensity(EKnobOper oper);

EKnobReturn D2ChgAGC(EKnobOper oper);

EKnobReturn D2ChgMBP(EKnobOper oper);

EKnobReturn D2PressChgEdge(void);
EKnobReturn D2ChgEdge(EKnobOper oper);

EKnobReturn D2ChgSteer(EKnobOper oper);
EKnobReturn D2PressChgSteer(void);

EKnobReturn D2PressFocus(void);

EKnobReturn D2PressChgSpaceCompound(void); 
EKnobReturn D2ChgSpaceCompound(EKnobOper oper);

EKnobReturn D2PressChgFreqCompound(void);
EKnobReturn D2ChgFreqCompound(EKnobOper oper);

EKnobReturn ChgFocusNum(EKnobOper  oper);

EKnobReturn D2ChgFocPos(EKnobOper  oper);

EKnobReturn D2PressChgMBP(void);

EKnobReturn D2PressChgSoundPower(void);

void ParepareForTpView(void);
EKnobReturn D2ChgTpView(EKnobOper oper);
EKnobReturn D2PressChgTpView();

EKnobReturn D2ChgEFVI(EKnobOper oper);
EKnobReturn D2PressChgEFVI();

EKnobReturn D2PressDepth(EKnobOper oper);
	
EKnobReturn D2PressChgScanLine(void);
EKnobReturn D2ChgScanLine(EKnobOper oper);

EKnobReturn D2ChgPolarity(EKnobOper oper);
#endif
