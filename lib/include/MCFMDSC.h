#ifndef MCFMDSC_H
#define MCFMDSC_H

#include "CFMDSC_R.h"

class CMCFMDSC : public CCFMDSC_R
{
public:
	static CMCFMDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	virtual void UpdateMDisplayFormat(void);
	virtual void ClearMLine(void);
};

#endif //MDSC_H

