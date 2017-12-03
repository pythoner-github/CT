/**
* @brief CFMB4DSC.h: interface for the CCFMB4DSC class.
* CCFMB4DSC is a derive class of CCFMDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef CFMB4DSC_H
#define CFMB4DSC_H

#include "CFMDSC.h"

class CCFMB4DSC : public CCFMDSC  
{
public:
	static CCFMB4DSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	void UpdateB4Flag(void);
};

#endif //CFMB4DSC_H
