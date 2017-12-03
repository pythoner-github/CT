// -*- c++ -*- 
#ifndef FAKE_X_EVENT_H
#define FAKE_X_EVENT_H

#include "AbsKeyboardEvent.h"

class FakeXEvent: public AbsKeyboardEvent {
public:
    virtual void KeyEvent(unsigned char keyValue);
    virtual void MouseEvent(char offsetX, char offsetY);
    virtual ~FakeXEvent(){};
};

#endif
