/**
* @brief PWCFMDSC_Rate_Rate.h: interface for the CPWCFMDSC_Rate_Rate class.
* CPWCFMDSC_Rate is a derive class of CPWDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-7-13
*/

#ifndef PWCFMDSC_Rate_H
#define PWCFMDSC_Rate_H

#include "PWDSC.h"

class CPWCFMDSC_Rate : public CPWDSC  
{
public:
	static CPWCFMDSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void UpdateCFMLDensity(void);
	virtual void UpdateCFMBaseline(void);
};

#endif //PWCFMDSC_Rate_H

