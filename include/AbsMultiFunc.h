#ifndef ABS_MULTI_FUNC
#define ABS_MULTI_FUNC

#include "Def.h"


class AbsMultiFunc
{
	public:
		virtual void Do() {}
		virtual void Undo() {}
		virtual void Update() { PRINTF("ABS uPDATE\n");}
		virtual void Value(EKnobOper opr) {}
		virtual void Mouse(int offsetX, int offsetY) {}
		virtual void KeyLeft() {}
		virtual void KeyRight() {}
		virtual void KeyUp() {}
		virtual void KeyDown() {}
		virtual void Esc() {}
		virtual ~AbsMultiFunc(){}
};

#endif

