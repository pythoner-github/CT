#ifndef CFMDSC_H
#define CFMDSC_H

#include "DSC.h"

class CCFMDSC : public CDSC
{
public:
	static CCFMDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void UpdateCFMLDensity(void);
};

#endif //CFMDSC_H
