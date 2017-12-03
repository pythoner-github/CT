/**
* @brief AbsUpdateArrow.h: interface for the CAbsUpdateArrow class.
* CAbsUpdateArrow is an interface class, it only define a set of actions
* for drawing arrow, don't implement.
*
* @version: 1.0
* @author: hehao
* @date: 2009-8-25
*/

#ifndef ABS_UPDATE_ARROW_H
#define ABS_UPDATE_ARROW_H

#include "Def.h"

class CAbsUpdateArrow
{
public:
	virtual ~CAbsUpdateArrow(){}
	virtual void DrawArrow(POINT pos, unsigned int direction, double scale, 
		unsigned int colorIndex, unsigned int shape)=0;
};

#endif //ABS_UPDATE_ARROW_H
