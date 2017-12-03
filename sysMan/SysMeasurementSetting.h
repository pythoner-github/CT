#ifndef __SYSMEASUREMENTSETTING_H__
#define __SYSMEASUREMENTSETTING_H__

#include "../base/IniFile.h"

struct TraceItem{
	unsigned char psSet;
	unsigned char edSet;
	unsigned char riSet;
	unsigned char sdSet;
	unsigned char tamaxSet;
	unsigned char piSet;
	unsigned char timeSet;
	unsigned char hrSet;
	unsigned char pgmaxSet;
	unsigned char pgmeanSet;
};

extern TraceItem g_traceCalcItem;

extern void GetTraceItem(struct TraceItem &calcItem);

class SysMeasurementSetting
{
public:
    SysMeasurementSetting();
    ~SysMeasurementSetting();

    int GetMeasureResult(void);
    int GetMeasureLineDisplay(void);
    int GetMeasureCursorSize(void);
    int GetMeasureSequence(void);
    int GetMeasureLineDensity(void);
    int GetMeasureColorCur(void);
    int GetMeasureColorConfirm(void);
    int GetTraceMethod(void);
    int GetReportResult(void);
    bool GetAutoCalcPS(void);
    bool GetAutoCalcED(void);
    bool GetAutoCalcRI(void);
    bool GetAutoCalcSD(void);
    bool GetAutoCalcTAmax(void);
    bool GetAutoCalcPI(void);
    bool GetAutoCalcTime(void);
    bool GetAutoCalcHR(void);
    bool GetAutoCalcPGmax(void);
    bool GetAutoCalcPGmean(void);
    int GetHeartBeatCycle(void);
    int GetUnitDist(void);
    int GetUnitArea(void);
    int GetUnitVol(void);
    int GetUnitTime(void);
    int GetUnitVel(void);
    int GetUnitSlope(void);
    int GetUnitAccel(void);
    int GetUnitEfw(void);

    // @param measureReulst font size: 0-10 1-15
    void SetMeasureResult(int mearesult);
    // @param measureLineDisplay: 0-OFF 1-ON
    void SetMeasureLineDisplay(int measureLineDisplay);
    // @param measureCursor: 0-Big 1-Mid 2-Small
    void SetMeasureCursorSize(int measureCursorSize);
    // @param measureSequence: 0-Default 1-Repeat 2-None
    void SetMeasureSequence(int measureSequence);
    // @param measureLineDensity: 0-High 1-Mid 2-Low
    void SetMeasureLineDensity(int measureLineDensity);
    // @param measureColorCur: 0-Red, 1-Yellow, 2-Green, 3-Blue, 4-White
    void SetMeasureColorCur(int measureColor);
    // @param measureColorConfirm: 0-Red, 1-Yellow, 2-Green, 3-Blue, 4-White
    void SetMeasureColorConfirm(int measureColor);
    // @param TraceMethod 0-Point 1-Track 2-Auto
    void SetTraceMethod(int manualTrace);
    // @param reportResult 0-last 1-average
    void SetReportResult(int reportResult);




    void SetAutoCalcPS(bool ps);
    void SetAutoCalcED(bool ed);
    void SetAutoCalcRI(bool ri);
    void SetAutoCalcSD(bool sd);
    void SetAutoCalcTAmax(bool tamax);
    void SetAutoCalcPI(bool pi);
    void SetAutoCalcTime(bool time);
    void SetAutoCalcHR(bool hr);
    void SetAutoCalcPGmax(bool pgmax);
    void SetAutoCalcPGmean(bool pgmean);
    void SetHeartBeatCycle(int cycle);

    void SetUnitDist(int unit);
    void SetUnitArea(int unit);
    void SetUnitVol(int unit);
    void SetUnitTime(int unit);
    void SetUnitVel(int unit);
    void SetUnitSlope(int unit);
    void SetUnitAccel(int unit);
    void SetUnitEfw(int unit);

    void SyncFile(void);
    void DefaultFactory(void);
    void UpdateTraceItemSetting(void);

private:
    IniFile *ptrIni;
};


#endif


