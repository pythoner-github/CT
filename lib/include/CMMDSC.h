/**
* @brief CMMDSC.h: interface for the CCMMDSC class.
* CCMMDSC is a derive class of CMDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-7-1
*/

#ifndef CMMDSC_H
#define CMMDSC_H

#include "MDSC.h"

class CCMMDSC : public CMDSC  
{
public:
	static CCMMDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame, 
			POINT startPos, POINT endPos);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart, 
		unsigned int sampEnd, float zoomScale);	
	virtual void UpdateMDisplayFormat(void);
	virtual void ClearMLine(void);
	virtual void SetCMMLinePosition(POINT startPos, POINT endPos);
};

#endif //CMMDSC_H

