/**
* @brief CFMB4DSC_Rate.h: interface for the CCFMB4DSC_Rate class.
* CCFMB4DSC_Rate is a derive class of CCFMDSC_Rate.
*
* @version: 1.0
* @author: hehao
* @date: 20010-4-2
*/

#ifndef CFMB4DSC_Rate_H
#define CFMB4DSC_Rate_H

#include "CFMDSC_Rate.h"

class CCFMB4DSC_Rate : public CCFMDSC_Rate  
{
public:
	static CCFMB4DSC_Rate *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateB4Flag(void);
};

#endif //CFMB4DSC_Rate_H
