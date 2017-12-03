// -*- c++ -*-
#ifndef UPDATE_MEASURE_H
#define UPDATE_MEASURE_H

#include <string>
#include <deque>
#include "MeasureDef.h"
#include "IMT.h"
#include "Def.h"

using std::string;
using std::deque;
class ImageArea;
// class ViewMeasureResult;
class UpdateMeasure
{
public:
    UpdateMeasure();
    ~UpdateMeasure();

    struct ResultAttr {
	int cursorType;
	int curColor;		// color index
	int confirmColor;	// color index
    };
    struct Result {
	string text;
	int lines;
	ResultAttr attr;
    };

    void ClearPwResultUnFreeze(int cursorType);
    void ClearFirst();
    void ClearLast();
    void ClearAll(bool update=true);
    void ClearMeasure(void);
    void AddResult(string result, int cursorType=0, int colorConfirm=2); // for test
    int GetResultCount(void);

    ///< 2D
    void CatHighCalc(const CalcResultInfo *calcInfo, int *calcOrder, double dataMea[], char buf[], int meaLen);
    void GenDisplaySingle(const SingleItemInfo *info, double data[], ResultAttr& attr, bool inMeasure, int unit_coeffi);
    void GenDisplayMulti(const MultiItemInfo *info, double dataMea[], ResultAttr& attr, bool inMeasure, int unit_coeffi[]);
    void D2Hip(double angle1, double angle2, ResultAttr& attr, bool inMeasure=true);

    void D2Dist(double length, ResultAttr& attr, bool inMeasure);

    void D2LenTrack(double length, ResultAttr& attr, bool inMeasure=true);
    //    void D2AreaTrack(double perimeter, double area, ResultAttr& attr, bool inMeasure=true);
    void D2AreaDot(double area, ResultAttr& attr, bool inMeasure=true);
    //    void D2AreaEllipse(double perimeter, double area, ResultAttr& attr, bool inMeasure=true);
    void D2AreaRec(double area, ResultAttr& attr, bool inMeasure=true);
    void D2Vol3Axis(char *name, double axis_x, double axis_y, double axis_z, double vol, ResultAttr& attr, bool inMeasure=true);
    void D2VolEllipse1(double vol, ResultAttr& attr, bool inMeasure=true);
    void D2VolEllipse2(double area, double dist, double vol, ResultAttr& attr, bool inMeasure=true);
    void D2VolSpheroid(double radius, double vol, ResultAttr& attr, bool inMeasure=true);
    void D2Angle3Dot(double dist_left, double dist_right, double angle, ResultAttr& attr, bool inMeasure=true);
    void D2Angle2Line(double angle, ResultAttr& attr, bool inMeasure=true);
    void D2RatioDist(double dist1, double dist2, double ratio, ResultAttr& attr, bool inMeasure=true);
    void D2RatioDistPeri(double dist, double perimeter, double ratio, ResultAttr& attr, bool inMeasure=true);
    void D2AngustyDist(double dist1, double dist2, double angusty, ResultAttr& attr, bool inMeasure=true);
    void D2RatioAreaTrack(double area1, double area2, double ratio, bool inMeasure=true);
    void D2AngustyArea(double area1, double area2, double angusty, ResultAttr& attr, bool inMeasure=true);
    void D2RatioArea(double area1, double area2, double ratio, ResultAttr& attr, bool inMeasure=true);
    void D2RatioVol(double vol1, double vol2, double ratio, ResultAttr& attr, bool inMeasure=true);
    void D2RatioAngle(double angle1, double angle2, double ratio, ResultAttr& attr, bool inMeasure=true);
    void MEF(double lvidd, double lvids, double lvvd, double lvvs, double ef, ResultAttr& attr, bool inMeasure=true);
    ///< M
    //    void MTime(double time, bool inMeasure=true);
    void MVel(double time, ResultAttr& attr, bool inMeasure=true);
    void MDepth(double depth, ResultAttr& attr, bool inMeasure=true);
    //    void MHeartRate(double time, double rate, bool inMeasure=true);
    void MSlope(double rate, ResultAttr& attr, bool inMeasure=true);

    ///< D
    //    void DVel(double vel, bool inMeasure=true);
    //	void DPSED(char *name, double ps, double ed, bool inMeasure=true);
    //    void DRI(char *name, double ps, double ed, double ri, bool inMeasure=true);
    void DSD(double ps, double ed, double sd, ResultAttr& attr, bool inMeasure=true);
    void DPI(double ps, double ed, double tamax, double pi, ResultAttr& attr, bool inMeasure=true);
    void DManual(double ps, double ed, double sd, double ri, double m_tamax, double m_pi, int m_hr, ResultAttr& attr, bool inMeasure);
    bool DTrace(char buf[], ResultAttr& attr, bool inMeasure);
    bool DTraceAdjust(char buf[], ResultAttr& attr, bool inMeasure=true);
    //    void DHR(double hr, bool inMeasure=true);
    void DIntegralArea(double integralarea, ResultAttr& attr, bool inMeasure=true);

    //	void DPGmax(const char *name, double vel, double pg, bool inMeasure=true);
    void DPGmean(double pg, ResultAttr& attr, bool inMeasure=true);
    void IMTResult(SIMTPARA imt_para,ResultAttr& attr,  bool update=true);
    void UpdateResultForChgFont(void);
    void PutResult(void);
    void ClearCurColorPwResult();
private:
    void RefreshResult(bool update=true);
    void UpdateResult(string result, int cursorType=0, int curColor=4); // for test
    int CountMeasureLines(void);
    int m_resultX;
    int m_resultY;
    int m_dateFormat;
		void NewLineSign(unsigned char newLineSign[2], int sign);

    static deque<Result> m_dequeResult;
    ImageArea* m_ptrImg;
    // ViewMeasureResult *m_ptrView;
};
#endif