/**
* @brief PWDSC.h: interface for the CPWDSC class.
* CPWDSC is a derive class of CDSC.
*
* @version: 1.0
* @author: hehao
* @date: 2009-7-1
*/

#ifndef PWDSC_H
#define PWDSC_H

#include "DSC.h"

class CPWDSC : public CDSC  
{
public:
	static CPWDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart, 
		unsigned int sampEnd, float zoomScale);	
	virtual void UpdatePWDisplayFormat(void);
	virtual void ClearPWLine(void);
	virtual int GetPWHeight(void);
	virtual vector<POINT> GetTraceData(void);
	virtual vector<POINT> GetTraceMaxVelocity(void);
	virtual void GetTraceCycle(int &begin, int &end);
	virtual void UpdateTrace(void);
};

#endif //PWDSC_H

