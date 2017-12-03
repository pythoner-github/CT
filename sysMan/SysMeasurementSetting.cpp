#include <string>
#include "Def.h"
#include "SysMeasurementSetting.h"

using std::string;

TraceItem g_traceCalcItem;

void GetTraceItem(struct TraceItem &calcItem)
{
	calcItem= g_traceCalcItem; 
}

SysMeasurementSetting::SysMeasurementSetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysMeasurementSetting::~SysMeasurementSetting()
{
    if (ptrIni != NULL) {
	delete ptrIni;
	ptrIni = NULL;
    }
}

int SysMeasurementSetting::GetMeasureResult(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureResult");
}
 
int SysMeasurementSetting::GetMeasureLineDisplay(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureLineDisplay");
}

int SysMeasurementSetting::GetMeasureCursorSize(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureCursorSize");
}

int SysMeasurementSetting::GetMeasureSequence(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureSequence");
}

int SysMeasurementSetting::GetMeasureLineDensity(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureLineDensity");
}

int SysMeasurementSetting::GetMeasureColorCur(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureColorCur");
}

int SysMeasurementSetting::GetMeasureColorConfirm(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "MeasureColorConfirm");
}

int SysMeasurementSetting::GetTraceMethod(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "TraceMethod");
}

int SysMeasurementSetting::GetReportResult(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "ReportResult");
}

bool SysMeasurementSetting::GetAutoCalcPS(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "PS");
}
bool SysMeasurementSetting::GetAutoCalcED(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "ED");
}
bool SysMeasurementSetting::GetAutoCalcRI(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "RI");
}
bool SysMeasurementSetting::GetAutoCalcSD(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "SD");
}
bool SysMeasurementSetting::GetAutoCalcTAmax(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "TAmax");
}
bool SysMeasurementSetting::GetAutoCalcPI(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "PI");
}
bool SysMeasurementSetting::GetAutoCalcTime(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "Time");
}
bool SysMeasurementSetting::GetAutoCalcHR(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "HR");
}
bool SysMeasurementSetting::GetAutoCalcPGmax(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "PGmax");
}
bool SysMeasurementSetting::GetAutoCalcPGmean(void)
{
    return ptrIni->ReadBool("MeasurementSetting", "PGmean");
}

int SysMeasurementSetting::GetHeartBeatCycle(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "HeartBeatCycle");
}

int SysMeasurementSetting::GetUnitDist(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitDist");
}

int SysMeasurementSetting::GetUnitArea(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitArea");
}

int SysMeasurementSetting::GetUnitVol(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitVol");
}

int SysMeasurementSetting::GetUnitTime(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitTime");
}

int SysMeasurementSetting::GetUnitVel(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitVel");
}

int SysMeasurementSetting::GetUnitSlope(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitSlope");
}

int SysMeasurementSetting::GetUnitAccel(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitAccel");
}

int SysMeasurementSetting::GetUnitEfw(void)
{
    return ptrIni->ReadInt("MeasurementSetting", "UnitEfw");
}

void SysMeasurementSetting::SetMeasureResult(int mearesult)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureResult", mearesult);
}

void SysMeasurementSetting::SetMeasureLineDisplay(int measureLineDisplay)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureLineDisplay", measureLineDisplay);
}

void SysMeasurementSetting::SetMeasureSequence(int measureSequence)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureSequence", measureSequence);    
}

void SysMeasurementSetting::SetMeasureCursorSize(int measureCursor)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureCursorSize", measureCursor);
}

void SysMeasurementSetting::SetMeasureLineDensity(int measureLineDensity)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureLineDensity", measureLineDensity);
}

void SysMeasurementSetting::SetMeasureColorCur(int measureColorCur)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureColorCur", measureColorCur);
}

void SysMeasurementSetting::SetMeasureColorConfirm(int measureColorConfirm)
{
    ptrIni->WriteInt("MeasurementSetting", "MeasureColorConfirm", measureColorConfirm);
}

void SysMeasurementSetting::SetTraceMethod(int manualTrace)
{
    ptrIni->WriteInt("MeasurementSetting", "TraceMethod", manualTrace);
}

void SysMeasurementSetting::SetReportResult(int reportResult)
{
    ptrIni->WriteInt("MeasurementSetting", "ReportResult", reportResult);
}

void SysMeasurementSetting::SetAutoCalcPS(bool ps)
{
    ptrIni->WriteBool("MeasurementSetting", "PS", ps);
}
void SysMeasurementSetting::SetAutoCalcED(bool ed)
{
    ptrIni->WriteBool("MeasurementSetting", "ED", ed);
}
void SysMeasurementSetting::SetAutoCalcRI(bool ri)
{
    ptrIni->WriteBool("MeasurementSetting", "RI", ri);
}
void SysMeasurementSetting::SetAutoCalcSD(bool sd)
{
    ptrIni->WriteBool("MeasurementSetting", "SD", sd);
}
void SysMeasurementSetting::SetAutoCalcTAmax(bool tamax)
{
    ptrIni->WriteBool("MeasurementSetting", "TAmax", tamax);
}
void SysMeasurementSetting::SetAutoCalcPI(bool pi)
{
    ptrIni->WriteBool("MeasurementSetting", "PI", pi);
}
void SysMeasurementSetting::SetAutoCalcTime(bool time)
{
    ptrIni->WriteBool("MeasurementSetting", "Time", time);
}
void SysMeasurementSetting::SetAutoCalcHR(bool hr)
{
    ptrIni->WriteBool("MeasurementSetting", "HR", hr);
}
void SysMeasurementSetting::SetAutoCalcPGmax(bool pgmax)
{
    ptrIni->WriteBool("MeasurementSetting", "PGmax", pgmax);
}
void SysMeasurementSetting::SetAutoCalcPGmean(bool pgmean)
{
    ptrIni->WriteBool("MeasurementSetting", "PGmean", pgmean);
}

void SysMeasurementSetting::SetHeartBeatCycle(int cycle)
{
    ptrIni->WriteInt("MeasurementSetting", "HeartBeatCycle", cycle);
}


void SysMeasurementSetting::SetUnitDist(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitDist", unit);
}

void SysMeasurementSetting::SetUnitArea(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitArea", unit);
}

void SysMeasurementSetting::SetUnitVol(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitVol", unit);
}

void SysMeasurementSetting::SetUnitTime(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitTime", unit);
}

void SysMeasurementSetting::SetUnitVel(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitVel", unit);
}

void SysMeasurementSetting::SetUnitSlope(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitSlope", unit);
}

void SysMeasurementSetting::SetUnitAccel(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitAccel", unit);
}

void SysMeasurementSetting::SetUnitEfw(int unit)
{
    ptrIni->WriteInt("MeasurementSetting", "UnitEfw", unit);
}

void SysMeasurementSetting::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}

void SysMeasurementSetting::DefaultFactory(void)
{
    int measureLineDisplay=0;
    int measureSequence = 0;
    int measureCursorSize=2;
    int measureLineDensity=1;
    int measureColorCur=2;
    int measureColorConfirm=4;
    int traceInMeasure=0;
    int manualTrace=0;
    int reportResult = 0;
    bool ps = false;
    bool ed = false;
    bool ri = false;
    bool sd = false;
    bool tamax = false;
    bool pi = false;
    bool time = false;
    bool hr = false;
    bool pgmax = false;
    bool pgmean = false;
	int heartBeatCycle = 2;
    int dist=0;
    int area=0;
    int vol=0;
    int tm=0;
    int vel=0;
    int accel=0;
    int efw=0;

    ptrIni->WriteInt("MeasurementSetting", "MeasureLineDisplay", measureLineDisplay);
    ptrIni->WriteInt("MeasurementSetting", "MeasureSequence", measureSequence);
    ptrIni->WriteInt("MeasurementSetting", "MeasureCursorSize", measureCursorSize);
    ptrIni->WriteInt("MeasurementSetting", "MeasureLineDensity", measureLineDensity);
    ptrIni->WriteInt("MeasurementSetting", "MeasureColorCur", measureColorCur);
    ptrIni->WriteInt("MeasurementSetting", "MeasureColorConfirm", measureColorConfirm);
    ptrIni->WriteInt("MeasurementSetting", "TraceInMeasure", traceInMeasure);
    ptrIni->WriteInt("MeasurementSetting", "TraceMethod", manualTrace);
    ptrIni->WriteInt("MeasurementSetting", "ReportResult", reportResult);

    ptrIni->WriteBool("MeasurementSetting", "PS", ps);
    ptrIni->WriteBool("MeasurementSetting", "ED", ed);
    ptrIni->WriteBool("MeasurementSetting", "RI", ri);
    ptrIni->WriteBool("MeasurementSetting", "SD", sd);
    ptrIni->WriteBool("MeasurementSetting", "TAmax", tamax);
    ptrIni->WriteBool("MeasurementSetting", "PI", pi);
    ptrIni->WriteBool("MeasurementSetting", "Time", time);
    ptrIni->WriteBool("MeasurementSetting", "HR", hr);
    ptrIni->WriteBool("MeasurementSetting", "PGmax", pgmax);
    ptrIni->WriteBool("MeasurementSetting", "PGmean", pgmean);

    ptrIni->WriteInt("MeasurementSetting", "UnitDist", dist);
    ptrIni->WriteInt("MeasurementSetting", "UnitArea", area);
    ptrIni->WriteInt("MeasurementSetting", "UnitVol", vol);
    ptrIni->WriteInt("MeasurementSetting", "UnitTime", tm);
    ptrIni->WriteInt("MeasurementSetting", "UnitVel", vel);
    ptrIni->WriteInt("MeasurementSetting", "UnitAccel", accel);
    ptrIni->WriteInt("MeasurementSetting", "UnitEfw", efw);

    ptrIni->WriteInt("MeasurementSetting", "HeartBeatCycle", heartBeatCycle);
}

void SysMeasurementSetting::UpdateTraceItemSetting(void)
{
	g_traceCalcItem.psSet = GetAutoCalcPS();
	g_traceCalcItem.edSet = GetAutoCalcED();
	g_traceCalcItem.riSet = GetAutoCalcRI();
	g_traceCalcItem.sdSet = GetAutoCalcSD();
	g_traceCalcItem.tamaxSet = GetAutoCalcTAmax();
	g_traceCalcItem.piSet = GetAutoCalcPI();
	g_traceCalcItem.timeSet = GetAutoCalcTime();
	g_traceCalcItem.hrSet = GetAutoCalcHR();
	g_traceCalcItem.pgmaxSet = GetAutoCalcPGmax();
	g_traceCalcItem.pgmeanSet = GetAutoCalcPGmean();
}
