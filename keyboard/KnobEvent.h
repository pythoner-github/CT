// -*- c++ -*-
/**
* @brief KnobEvent.h: interface for the CKnobEvent class,
* it achieved part of knob operation, and it derived from AbsKeyboardEvent.
*
* emperor
* @version: 1.0
* @author: hehao
* @date: 2009-9-5
*/

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
