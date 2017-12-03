/**
* @brief CFMBBDSC.h: interface for the CCFMBBDSC class.
* CCFMBBDSC is a derive class of CCFMDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef CFMBBDSC_H
#define CFMBBDSC_H

#include "CFMDSC.h"

class CCFMBBDSC : public CCFMDSC  
{
public:
	static CCFMBBDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateBBFlag(void);
};

#endif //CFMBBDSC_H
