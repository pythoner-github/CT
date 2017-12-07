#ifndef PWCFMDSC_R_H
#define PWCFMDSC_R_H

#include "PWDSC.h"

class CPWCFMDSC_R : public CPWDSC
{
public:
	static CPWCFMDSC_R *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	virtual void UpdateCFMLDensity(void);
	virtual void UpdateCFMBaseline(void);
};

#endif //PWCFMDSC_R_H

