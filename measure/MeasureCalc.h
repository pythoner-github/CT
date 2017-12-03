// -*- c++ -*-
#ifndef MEASURE_CALC_H
#define MEASURE_CALC_H

#include <vector>
#include "Def.h"

using std::vector;

class MeasureCalc
{
	public:
		MeasureCalc();
		~MeasureCalc();

		///> 2D
		double D2CalcDist(POINT p1, POINT p2);
		double D2CalcSquDist(POINT p1, POINT p2);
		double D2CalcLenTrack(POINT p1, POINT p2);
        double D2CalcAreaDot(POINT p1, POINT p2, POINT start);
		double D2CalcAreaTrack(POINT p1, POINT p2, POINT start);
        double D2CalcEllipse(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start, int sign);//计算椭圆面积、周长、体积
        double D2CalcPerimeterEllipse(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start);
        double D2CalcAreaRec(POINT p1, POINT p2);
        double CalcVol(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start, double dist);
        double D2CalcEllipse2(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start, POINT z_start, POINT z_end);
        double D2CalcVolSpheroid(POINT radius_start, POINT rasius_end);
        double D2CalcSimpson(float length, float diamSquSum);
		double D2CalcAL(float length, float area);
		double D2CalcAngle(POINT vertex, POINT left, POINT right);
		double D2CalcHipAlpha(double angle1, double angle2);
		double D2CalcHipBeta(double angle1, double angle2);
		double D2CalcAngusty(double dist1, double dist2);
		double CalcRatio(double dist1, double dist2);
		///> M
		double MCalcDepthDist(POINT p1, POINT p2);
		double MCalcLvTei(double dist);//M模式下Teicholz方法计算左室容积
		double CalcEf(double edv, double esv);
		double MCalcTime(POINT p1, POINT p2);
		double MCalcVel(POINT p1, POINT p2);
		double MCalcDepth(POINT p1, POINT p2);
		double MCalcHR(double time);
		double MCalcSlope(POINT p1, POINT p2);
		///> D
		double DCalcVel(POINT p);
		double DCalcRI(double ps, double ed);
		double DCalcSD(double ps, double ed);
		double DCalcTime(POINT p1, POINT p2);
		double DCalcAccel(POINT p1, POINT p2);
		bool GetPsEdInVec(vector<POINT> vec, POINT &psP, POINT &edP, int baseLine, int auto_manual, bool auto_calc=false);
        double DCalcTraceAutoOther(char buf[], vector<POINT> vec, POINT ps, POINT ed);
        double DCalcTraceOther(char buf[], vector<POINT> vec, POINT ps, POINT ed);
        double DCalcTrace(char buf[], vector<POINT> vec, POINT &ps, POINT &ed, int auto_manual);
        double DCalcTraceAuto(char buf[], vector<POINT> vec, POINT &ps, POINT &ed, int auto_manual);
        void DCalcPI(vector<POINT> vec, POINT &ps, POINT &ed, double &psValue, double &edValue, double &tamaxValue, double &piValue);
        double DCalcPISelf(double ps, double ed, double tamax);
        //	double DCalcPI(vector<POINT> vec);
        double DCalcTamax(vector<POINT> vec);
        double DCalcHRAuto(double time);
        double DCalcHR(double time);
        double DCalcPGmax(double Vmax);
        double DCalcPGmean(vector<POINT> vec);
        double DCalcINTEGRALTrack(vector<POINT> track);

    private:
        double GetScale2D(POINT p);
        void GetValueFromTrace(vector<POINT> vec, int baseLine, int &ps, int &ed, int &vmean);
		bool GetVmean2FromTrace(vector<POINT> vec, int baseLine, int &vmean2);
		bool GetVmeanFromTrace(vector<POINT> vec, int baseLine, int &vmean);
		int GetHRCycleNum();
};
#endif
