/**
* @brief CFMBBDSC_Rate.h: interface for the CCFMBBDSC_Rate class.
* CCFMBBDSC_Rate is a derive class of CCFMDSC_Rate.
*
* @version: 1.0
* @author: hehao
* @date: 2010-4-2
*/

#ifndef CFMBBDSC_Rate_H
#define CFMBBDSC_Rate_H

#include "CFMDSC_Rate.h"

class CCFMBBDSC_Rate : public CCFMDSC_Rate  
{
public:
	static CCFMBBDSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateBBFlag(void);
};

#endif //CFMBBDSC_Rate_H
