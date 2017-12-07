#ifndef CMMExDSC_H
#define CMMExDSC_H

#include "DSC.h"

class CCMMExDSC : public CDSC
{
public:
	static CCMMExDSC *Create(const DSCCONTROLATTRIBUTES &attrs, const UPDATEFRAME pUpdateFrame,
		POINT startPos[3], POINT endPos[3]);
	virtual void Destroy(void);
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);
	virtual void ClearMLine(void);

	//para type: 0:A sample line, 1:B sample line, 2:C sample line
	virtual void SetCMMLinePosition(POINT startPos, POINT endPos, int type);
};

#endif //CMMExDSC_H

