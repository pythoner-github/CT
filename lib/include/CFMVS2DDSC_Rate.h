#ifndef CFMVS2DDSC_Rate_H
#define CFMVS2DDSC_Rate_H

#include "CFMDSC_Rate.h"

class CCFMVS2DDSC_Rate : public CCFMDSC_Rate
{
public:
	static CCFMVS2DDSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void UpdateCFMBaseline(void);
};

#endif //CFMVS2DDSC_Rate_H
