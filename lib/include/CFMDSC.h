/**
* @brief CFMDSC.h: interface for the CCFMDSC class.
* CCFMDSC is a derive class of CDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef CFMDSC_H
#define CFMDSC_H

#include "DSC.h"

class CCFMDSC : public CDSC  
{
public:
	static CCFMDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void UpdateCFMLDensity(void);
};

#endif //CFMDSC_H
