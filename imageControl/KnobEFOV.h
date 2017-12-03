#ifndef _KNOBEFOV_H_
#define _KNOBEFOV_H_

#include "Def.h"

enum EKnobEFOVPrepare {EFOV_ROI};
enum EKnobEFOV {EFOV_ZOOM, EFOV_ROTATE};
enum EKnobEFOVReview {EFOV_REPLAY, EFOV_SAVE};

void KnobEFOVNoneCreate();
void KnobEFOVPrepareCreate();
void KnobEFOVCaptureCreate();
void KnobEFOVViewCreate();
void KnobEFOVReviewCreate();

void SyncKnobEFOV(EKnobEFOV type, const char *s, EKnobReturn status);
void SyncKnobEFOVReview(EKnobEFOVReview type, const char* s, EKnobReturn status);
EKnobReturn KnobEFOVZoom(EKnobOper oper);
EKnobReturn KnobEFOVRotate(EKnobOper oper);
EKnobReturn KnobEFOVReviewPress(void);
EKnobReturn KnobEFOVReplayPress(void);
EKnobReturn KnobEFOVSavePress(void);
EKnobReturn KnobEFOVReadPress(void);
EKnobReturn KnobEFOVROISize(EKnobOper oper);
EKnobReturn KnobEFOVPressChgRotate(void);
EKnobReturn KnobEFOVPressChgZoom(void);
EKnobReturn KnobEFOVPressChgROISize(void);

///>for g30
EKnobReturn KnobEFOVReviewPressForG30(EKnobOper oper);
EKnobReturn KnobEFOVReplayPressForG30(EKnobOper oper);
EKnobReturn KnobEFOVSavePressForG30(EKnobOper oper);

#endif /* _KNOBEFOV_H_ */
