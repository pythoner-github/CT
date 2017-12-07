#ifndef CFMBBDSC_R_H
#define CFMBBDSC_R_H

#include "CFMDSC_R.h"

class CCFMBBDSC_R : public CCFMDSC_R
{
public:
	static CCFMBBDSC_R *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	void UpdateBBFlag(void);
};

#endif //CFMBBDSC_R_H
