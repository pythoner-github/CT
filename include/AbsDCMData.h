/**
* @brief AbsDCMData.h: interface for the CAbsDCMData class.
* CAbsDCMData is abstract base class used to get datas of DICOM 
* image that be send.
*
* @version: 1.0
* @author: hehao
* @date: 2009-9-27
*/

#ifndef ABS_DCMDATA_H
#define ABS_DCMDATA_H
#include "DCMDef.h"


class CAbsDCMData
{
public:
	virtual ~CAbsDCMData(){};
	virtual bool GetDCMData(bool (*)(DCMELEMENT &))=0;

};

#endif //ABS_DCMDATA_H
