#ifndef CFMVS2DDSC_H
#define CFMVS2DDSC_H

#include "CFMDSC.h"

class CCFMVS2DDSC : public CCFMDSC
{
public:
	static CCFMVS2DDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
};

#endif //CFMVS2DDSC_H
