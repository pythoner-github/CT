/**
* @brief BDSC.h: interface for the CBDSC class.
* CBDSC is a derive class of CDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef BDSC_H
#define BDSC_H
#include "DSC.h"

class CBDSC:public CDSC
{
public:
	static CBDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void Update(void);

	virtual void SetGlobalZoomState(POINT zoomPoint, float zoomScale);
	virtual RECT SetPIPZoomState(POINT PIPPoint, float zoomScale);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart, 
		unsigned int sampEnd, float zoomScale);	
    virtual void UpdateTpViewStatus(void);    
};

#endif //BDSC_H
