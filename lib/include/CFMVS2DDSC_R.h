/**
* @brief CFMVS2DDSC_R.h: interface for the CCFMVS2DDSC_R class.
* CCFMVS2DDSC_R is a derive class of CCFMDSC_R. Receive cfm data is R, not RGB, and a full cfm line have PKTSIZE bytes, 
* not PKTSIZE*3 bytes. The section parameter is invalid.
*
* @version: 1.0
* @author: hehao
* @date: 2010-5-14
*/

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
