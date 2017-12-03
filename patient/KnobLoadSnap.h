#ifndef KNOB_LOADSNAP_H
#define KNOB_LOADSNAP_H

#include "Def.h"

#if (defined (EMP_322) || defined(EMP_313))
    enum EKnobReview {REVIEW_NEXT, REVIEW_PREVIOUS, REVIEW_SLIDE_PLAY, REVIEW_SLIDE_SPEED};
#else
    enum EKnobReview {REVIEW_NEXT, REVIEW_SLIDE_PLAY, REVIEW_SLIDE_SPEED};
#endif

void KnobLoadSnapCreate();
void SyncKnobReview(EKnobReview type, const char* s, EKnobReturn status, bool update = TRUE);

EKnobReturn ChgReviewSnap(EKnobOper oper);
EKnobReturn ReviewNextSnap(void);
EKnobReturn ReviewPreviousSnap(void);
EKnobReturn PressReviewSlidePlay(void);
EKnobReturn ReviewSlidePlay(EKnobOper oper);
EKnobReturn PressReviewSlideSpeed(void);
EKnobReturn ReviewSlideSpeed(EKnobOper oper);

#endif
