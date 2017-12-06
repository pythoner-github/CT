#ifndef ABS_DCMDATA_H
#define ABS_DCMDATA_H
#include "periDevice/DCMDef.h"

class CAbsDCMData
{
public:
    virtual ~CAbsDCMData(){};
    virtual bool GetDCMData(bool (*)(DCMELEMENT &))=0;

};

#endif //ABS_DCMDATA_H