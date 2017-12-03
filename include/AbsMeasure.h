#ifndef ABS_MEASURE_H
#define ABS_MEASURE_H

#include "Def.h"

class AbsMeasure
{
	public:
		AbsMeasure() {}
		virtual ~AbsMeasure() {}

		virtual void PressLeft(POINT p) = 0;
		virtual void PressRight(POINT p) = 0;
		virtual void MouseMove(POINT p) = 0;
		virtual void Esc() {}
		virtual void Change() {}
		virtual void Value(EKnobOper opr) {}
};
#endif
