/**
* @brief EFOVDSC.h: interface for the CEFOVDSC class.
* CEFOVDSC is a derive class of CDSC. Only for linear probe.
*
* @version: 1.0
* @author: hehao
* @date: 2011-5-9
*/

#ifndef EFOVDSC_H
#define EFOVDSC_H
#include "DSC.h"

class CEFOVDSC:public CDSC
{
public:
	static CEFOVDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void Update(void);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart, 
		unsigned int sampEnd, float zoomScale);	
	virtual void SetEFOVStatus(EFOVSTATUS status);
	virtual void RotateEFOVImage(int level);
	virtual void GetEFOVSpeed(int &speed_flag, int &speed_maxflag, float &speed);
	virtual EFOVRECT<int> GetEFOVBoxRect(void);
	virtual int GetEFOVTotalFrameNo(void);
	virtual RECT SetEFOVZoomState(int xCenter, float zoomScale);
	virtual void SetEFOVReviewFrameNo(int no);
	virtual EFOVSTATUS GetEFOVStatus(void);
	virtual void SaveEFOVCine(unsigned char **data, int &size);
	virtual void ReadEFOVCine(unsigned char *data, int size);
	virtual float GetEFOVScale(void);
	virtual RECT GetEFOVZoomBoxRect(void);
        virtual void SetROISize(int size);
};

#endif //EFOVDSC_H
