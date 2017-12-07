#ifndef PWCFMDSC_H
#define PWCFMDSC_H

#include "PWDSC.h"

class CPWCFMDSC : public CPWDSC
{
public:
	static CPWCFMDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void UpdateCFMLDensity(void);
};

#endif //PWCFMDSC_H

