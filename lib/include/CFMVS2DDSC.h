/**
* @brief CFMVS2DDSC.h: interface for the CCFMVS2DDSC class.
* CCFMVS2DDSC is a derive class of CCFMDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef CFMVS2DDSC_H
#define CFMVS2DDSC_H

#include "CFMDSC.h"

class CCFMVS2DDSC : public CCFMDSC  
{
public:
	static CCFMVS2DDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
};

#endif //CFMVS2DDSC_H
