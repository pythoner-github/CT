/**
* @brief CFMDSC_Rate.h: interface for the CCFMDSC_Rate class.
* CCFMDSC_Rate is a derive class of CDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2010-4-2
*/

#ifndef CFMDSC_Rate_H
#define CFMDSC_Rate_H

#include "DSC.h"

class CCFMDSC_Rate : public CDSC  
{
public:
	static CCFMDSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void UpdateCFMLDensity(void);
	virtual void UpdateCFMBaseline(void);
};

#endif //CFMDSC_Rate_H
