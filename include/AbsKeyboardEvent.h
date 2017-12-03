// -*- c++ -*-
/**
 * @brief AbsKeyboardEvent.h: abstract base class, it define interface event
 * of Key Knob Slider and Mouse, and every window will override these interface.
 *
 * emperor
 * @version: 1.0
 * @author: hehao
 * @date: 2009-9-5
 */

#ifndef ABS_KEYBOARDEVENT_H
#define ABS_KEYBOARDEVENT_H

class AbsKeyboardEvent
{
    public:
	virtual void KeyEvent(unsigned char keyValue){};
	virtual void KnobEvent(unsigned char keyValue, unsigned char offset){};
	virtual void SliderEvent(unsigned char keyValue, unsigned char offset){};
	virtual void MouseEvent(char offsetX, char offsetY){};
	virtual ~AbsKeyboardEvent(){};
};
#endif //ABS_KEYBOARDEVENT_H
