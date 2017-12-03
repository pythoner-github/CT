/**
* @brief CFMDSC_R.h: interface for the CCFMDSC_R class.
* CCFMDSC_R is a derive class of CDSC. Receive cfm data is R, not RGB, and a full cfm line have PKTSIZE bytes, not 
* PKTSIZE*3 bytes. The section parameter is invalid.
*
* @version: 1.0
* @author: hehao
* @date: 2010-5-14
*/

#ifndef CFMDSC_R_H
#define CFMDSC_R_H

#include "DSC.h"

class CCFMDSC_R : public CDSC  
{
public:
	static CCFMDSC_R *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart, 
		unsigned int sampEnd, float zoomScale);	
	virtual void UpdateCFMLDensity(void);
	virtual void UpdateCFMBaseline(void);
};

#endif //CFMDSC_R_H
