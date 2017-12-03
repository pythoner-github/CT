/**
* @brief DSC.h: interface for the CDSC class.
* CDSC is a abstract base class.
*
* @version: 1.0
* @author: hehao
* @date: 2009-6-10
*/

#ifndef DSC_H
#define DSC_H

#include <DSCTypes.h>
#include <vector>
using namespace std;

class CDSC
{
public:
	enum EFOVSTATUS
	{
		PREPARE,
		CAPTURE,
		VIEW,
		REVIEW
	};

	virtual void Destroy(void)=0;
	virtual bool GetUSBDatas(const unsigned char *pDatas);
	virtual void Update(void);

	virtual void SetGlobalZoomState(POINT zoomPoint, float zoomScale);
	virtual RECT SetPIPZoomState(POINT PIPPoint, float zoomScale);
	virtual void SetLocalZoomState(unsigned int scanLStart, unsigned int scanLEnd, unsigned int sampStart,
		unsigned int sampEnd, float zoomScale);

	virtual void UpdateProbeWidth(void);
	virtual void UpdateProbeR(void);
	virtual void UpdateProbeType(void);
	virtual void UpdateCurMaxScanLines(void);
	virtual void UpdateSamplePoints(void);
	virtual void UpdateScale(void);
	virtual void UpdatePWDisplayFormat(void);
	virtual void UpdateMDisplayFormat(void);
	virtual void UpdateCFMLDensity(void);
	virtual void ClearPWLine(void);
	virtual void ClearMLine(void);
	virtual int GetPWHeight(void);
	virtual vector<POINT> GetTraceData(void);
	virtual vector<POINT> GetTraceMaxVelocity(void);
	virtual void GetTraceCycle(int &begin, int &end);
	virtual void UpdateTrace(void);
	virtual void SetCMMLinePosition(POINT startPos, POINT endPos);
	virtual void SetCMMLinePosition(POINT startPos, POINT endPos, int type);
	virtual void UpdateCFMBaseline(void);
	virtual void UpdateCFMPDIDirection(void);
	virtual void UpdateBBFlag(void);
	virtual void UpdateB4Flag(void);
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
    virtual void UpdateTpViewStatus(void);
};
#endif	//DSC_H