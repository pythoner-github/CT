/**
* @brief PWCFMDSC.h: interface for the CPWCFMDSC class.
* CPWCFMDSC is a derive class of CPWDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-7-13
*/

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

