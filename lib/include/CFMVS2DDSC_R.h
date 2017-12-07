#ifndef CFMVS2DDSC_R_H
#define CFMVS2DDSC_R_H

#include "CFMDSC_R.h"

class CCFMVS2DDSC_R : public CCFMDSC_R
{
public:
	static CCFMVS2DDSC_R *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	virtual void UpdateCFMBaseline(void);
};

#endif //CFMVS2DDSC_R_H
