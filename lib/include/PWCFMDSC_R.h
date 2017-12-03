/**
* @brief PWCFMDSC_R.h: interface for the CPWCFMDSC_R class.
* CPWCFMDSC_R is a derive class of CPWDSC. Receive cfm data is R, not RGB, and a full cfm line have PKTSIZE bytes, not 
* PKTSIZE*3 bytes. The section parameter is invalid.
*
* @version: 1.0
* @author: hehao
* @date: 2010-5-14
*/

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

