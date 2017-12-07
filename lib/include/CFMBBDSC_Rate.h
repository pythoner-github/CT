#ifndef CFMBBDSC_Rate_H
#define CFMBBDSC_Rate_H

#include "CFMDSC_Rate.h"

class CCFMBBDSC_Rate : public CCFMDSC_Rate
{
public:
	static CCFMBBDSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateBBFlag(void);
};

#endif //CFMBBDSC_Rate_H
