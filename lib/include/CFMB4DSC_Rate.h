#ifndef CFMB4DSC_Rate_H
#define CFMB4DSC_Rate_H

#include "CFMDSC_Rate.h"

class CCFMB4DSC_Rate : public CCFMDSC_Rate
{
public:
	static CCFMB4DSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateB4Flag(void);
};

#endif //CFMB4DSC_Rate_H
