#ifndef B4DSC_H
#define B4DSC_H

#include "DSC.h"

class CB4DSC : public CDSC
{
public:
	static CB4DSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	virtual void UpdateB4Flag(void);
};

#endif //B4DSC_H

