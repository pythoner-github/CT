#ifndef CFMB4DSC_H
#define CFMB4DSC_H

#include "CFMDSC.h"

class CCFMB4DSC : public CCFMDSC
{
public:
	static CCFMB4DSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateB4Flag(void);
};

#endif //CFMB4DSC_H
