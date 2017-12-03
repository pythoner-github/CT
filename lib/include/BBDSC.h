/**
* @brief BBDSC.h: interface for the CBBDSC class.
* CBBDSC is a derive class of CDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef BBDSC_H
#define BBDSC_H

#include "DSC.h"

class CBBDSC : public CDSC  
{
public:
	static CBBDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart, 
		unsigned int sampEnd, float zoomScale);	
	virtual void UpdateBBFlag(void);
};

#endif //BBDSC_H

