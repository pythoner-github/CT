#ifndef CFMBBDSC_H
#define CFMBBDSC_H

#include "CFMDSC.h"

class CCFMBBDSC : public CCFMDSC
{
public:
	static CCFMBBDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateBBFlag(void);
};

#endif //CFMBBDSC_H
