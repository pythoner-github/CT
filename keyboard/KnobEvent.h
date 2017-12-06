#ifndef KNOB_EVENT_H
#define KNOB_EVENT_H

#include "AbsKeyboardEvent.h"

class CKnobEvent: public AbsKeyboardEvent
{
public:
    virtual void KnobEvent(unsigned char keyValue, unsigned char offset);
    static void FEvent (unsigned char keyValue);
    virtual ~CKnobEvent(){};
};

#endif //KNOB_EVENT_H