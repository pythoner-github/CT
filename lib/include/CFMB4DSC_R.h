#ifndef CFMB4DSC_R_H
#define CFMB4DSC_R_H

#include "CFMDSC_R.h"

class CCFMB4DSC_R : public CCFMDSC_R
{
public:
	static CCFMB4DSC_R *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	void UpdateB4Flag(void);
};

#endif //CFMB4DSC_R_H
