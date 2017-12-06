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