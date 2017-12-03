/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: MeasureCalc.cpp
 * @brief: do somthing measure used when measuring 
 * 
 * version: V1.0
 * date: 2009-7-9
 * @author: zhanglei
 */

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "MeasureDef.h"
#include "MeasureCalc.h"
#include "../imageControl/Img2D.h"
#include "ImgPw.h"
#include "SysMeasurementSetting.h"
#include "UpdateMeasure.h"
#include "MeasureDraw.h"
#include "../display/ImageAreaDraw.h"

using namespace std;

extern bool g_calcPwStatus;

MeasureCalc::MeasureCalc()
{
}
MeasureCalc::~MeasureCalc()
{
}

double MeasureCalc::GetScale2D(POINT p)
{
	int field = 0;
	double scaleNeed;
	double scale[4];

	Img2D::GetInstance()->GetAllScale2D(scale);
	field = ImageAreaDraw::GetInstance()->GetPointPos(p.x, p.y);
	if (field < 0)
		scaleNeed = Img2D::GetInstance()->GetScale2D();
	else
		scaleNeed = scale[field];

	return(scaleNeed);
}

///> 2D
double MeasureCalc::D2CalcDist(POINT p1, POINT p2)
{
	double scale = GetScale2D(p1);

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;

	double result = 0.0;
	result = scale * sqrt(dx*dx + dy*dy) / 10.0;

	return result;
}

double MeasureCalc::D2CalcSquDist(POINT p1, POINT p2)
{
	double scale = GetScale2D(p1);

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;

	double result = 0.0;
	result = scale * scale * (dx*dx + dy*dy) / 100.0;

	return result;
}

double MeasureCalc::D2CalcLenTrack(POINT p1, POINT p2)
{
	double scale = GetScale2D(p1);

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;

	double result = 0.0;
	result = scale * sqrt(dx*dx + dy*dy) / 10.0;

	return result;
}

double MeasureCalc::DCalcINTEGRALTrack(vector<POINT> track)
{
    double scale = ImgPw::GetInstance()->GetScaleTime();
    double scale1 = ImgPw::GetInstance()->GetScaleVel();
    int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
    double area1=0.0;

    for(unsigned int i = 0;i<(track.size()-1); i++)
    { 
        double time = fabs((track[i+1].x -track[i].x)*scale);
        double vel = fabs((baseLine-track[i+1].y)*scale1);

        if(((baseLine-track[i+1].y)>0)&&((baseLine-track[i].y)>0)&&(track[i+1].x!=track[i].x))
            area1 +=(time *vel);
        if(((baseLine-track[i+1].y)<0)&&((baseLine-track[i].y)<0)&&(track[i+1].x!=track[i].x))
            area1 -=(time *vel);
    }
return area1;
}

double MeasureCalc::D2CalcAreaTrack(POINT p1, POINT p2, POINT start)
{
	double scale = GetScale2D(p1);
	double area;
	area = (double)(scale * (p2.y + p1.y - 2*start.y) / 2.0) * (scale * (double)(p2.x - p1.x)) / 100.0;
	return area;
}

double MeasureCalc::D2CalcAreaDot(POINT p1, POINT p2, POINT start)
{    	
	double scale = GetScale2D(p1);
	double area;
	area = (scale * (p2.y + p1.y - 2 * start.y) / 2.0) * (scale * (p2.x - p1.x)) / 100.0;
	return area;
}

//计算椭圆面积、周长、体积
//sign = 1:面积，sign = 2:周长， sign = 3:体积
double MeasureCalc::D2CalcEllipse(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start, int sign)
{
	double result;
	double pox, poy;
	double la_start_tmpx, la_end_tmpx, sa_start_tmpx, la_start_tmpy, la_end_tmpy, sa_start_tmpy;
	double long_axis, short_axis;

	la_start_tmpx = (double)long_axis_start.x;
	la_end_tmpx = (double)long_axis_end.x;
	sa_start_tmpx = (double)short_axis_start.x;
	la_start_tmpy = (double)long_axis_start.y;
	la_end_tmpy = (double)long_axis_end.y;
	sa_start_tmpy = (double)short_axis_start.y;

	double scale = GetScale2D(long_axis_start);
	pox = (la_start_tmpx + la_end_tmpx) / 2.0;
	poy = (la_start_tmpy + la_end_tmpy) / 2.0;
	long_axis = scale * sqrt((la_end_tmpx - la_start_tmpx)*(la_end_tmpx - la_start_tmpx) + 
			(la_end_tmpy - la_start_tmpy)*(la_end_tmpy - la_start_tmpy)) / 2.0;
	short_axis = scale * sqrt((sa_start_tmpy - poy)*(sa_start_tmpy - poy) + 
			(sa_start_tmpx - pox)*(sa_start_tmpx - pox)) ;
	if (sign == 1)
	{
		result = PI * long_axis * short_axis / 100.0;
	}
	else if (sign == 2)
	{
		if(long_axis >= short_axis)
			result = (2 * PI * short_axis + 4 * (long_axis - short_axis)) / 10.0;//周长公式需要再查证
		else
			result = (2 * PI * long_axis + 4 * (short_axis - long_axis)) / 10.0;
	}
	else if (sign == 3)
	{
		if(long_axis >= short_axis)
			result = ((double)4 / (double)3) * PI * long_axis * short_axis * short_axis / 1000.0;
		else
			result = ((double)4 / (double)3) * PI * short_axis * long_axis * long_axis / 1000.0;
	}
	else
	{
		result = 0;
	}

	return result;
}


double MeasureCalc::D2CalcAreaRec(POINT p1, POINT p2)
{    	
	double area;
	double scale = GetScale2D(p1);
	area = (double)( abs(p2.x - p1.x) * scale * abs(p2.y - p1.y) * scale / 100.0 );
	return area;
}

double MeasureCalc::CalcVol(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start, double dist)
{	
    double long_axis, short_axis, z_axis;
    double center_x, center_y;
    center_x = (double)(long_axis_start.x + long_axis_end.x)/ 2.0;
    center_y = (double)(long_axis_start.y + long_axis_end.y)/ 2.0;

    double scale = GetScale2D(long_axis_start);
    
    long_axis = scale * sqrt((double)((long_axis_end.x - long_axis_start.x)*(long_axis_end.x - long_axis_start.x)) + (double)((long_axis_end.y - long_axis_start.y)*(long_axis_end.y - long_axis_start.y)))/2.0;

    short_axis = scale * sqrt((double)((short_axis_start.x - center_x)*(short_axis_start.x - center_x)) + (double)((short_axis_start.y - center_y)*(short_axis_start.y - center_y)));
    z_axis = dist / 2; 
    double vol = (double)4/(double)3 *PI * long_axis * short_axis * z_axis / 100.0;

    return vol;
}


double MeasureCalc::D2CalcEllipse2(POINT long_axis_start, POINT long_axis_end, POINT short_axis_start, POINT z_start, POINT z_end)
{
	double vol;
	double pox, poy;
	double la_start_tmpx, la_end_tmpx, sa_start_tmpx, la_start_tmpy, la_end_tmpy, sa_start_tmpy;
	double long_axis, short_axis, z_axis;

	la_start_tmpx = (double)long_axis_start.x;
	la_end_tmpx = (double)long_axis_end.x;
	sa_start_tmpx = (double)short_axis_start.x;
	la_start_tmpy = (double)long_axis_start.y;
	la_end_tmpy = (double)long_axis_end.y;
	sa_start_tmpy = (double)short_axis_start.y;


	double scale = GetScale2D(long_axis_start);
	pox = (la_start_tmpx + la_end_tmpy) / 2.0;
	poy = (la_start_tmpy + la_end_tmpy) / 2.0;
	long_axis = scale * ( sqrt(pow((la_end_tmpy - la_start_tmpy), 2.0) + pow((la_end_tmpx - la_start_tmpx), 2.0)) ) / 2.0;
	short_axis = scale * (sqrt(pow((sa_start_tmpy - poy), 2.0) + pow((sa_start_tmpx - pox), 2.0)));
	z_axis = scale * (sqrt(pow((z_end.x - z_start.x), 2.0) + pow((z_end.y - z_start.y), 2.0)));

	vol = ((double)4 / (double)3) * PI * long_axis * short_axis * z_axis / 1000.0;
	return vol;
}

double MeasureCalc::D2CalcVolSpheroid(POINT radius_start, POINT radius_end)
{
	double radius, vol;
	double scale = GetScale2D(radius_start);

	radius = scale * (sqrt(pow((radius_end.x - radius_start.x), 2.0) + pow((radius_end.y - radius_start.y), 2.0)));
	vol = ((double)4 / (double)3) * PI * radius * radius * radius / 1000.0;
	return vol;
}

double MeasureCalc::D2CalcSimpson(float length, float diamSquSum)
{
	return(PI * (double)length * (double)diamSquSum / 80);
}

double MeasureCalc::D2CalcAL(float length, float area)
{
	if (length > ZERO)
		return((double)(8*area * area) / (3 * PI * (double)length));
	else
		return (double)INVALID_VAL;;
}

double MeasureCalc::D2CalcAngle(POINT vertex, POINT left, POINT right)
{
	double length1, length2, cos_angle, angle, tan_angle;
	int length1_squ,length2_squ,length3_squ;

	length1_squ = (left.x-vertex.x) * (left.x-vertex.x) + (left.y-vertex.y) * (left.y-vertex.y);
	length2_squ = (right.x-vertex.x) * (right.x-vertex.x) + (right.y-vertex.y) * (right.y-vertex.y);
	length3_squ = (right.x-left.x) * (right.x-left.x) + (right.y-left.y) * (right.y-left.y);
	length1 = sqrt((double)length1_squ);
	length2 = sqrt((double)length2_squ);
	//	if (((length1<-0.0001) || (length1>0.0001)) && ((length2<-0.0001) || (length2>0.0001)))
    if ((fabs(length1) > ZERO) && (fabs(length2) > ZERO))
        cos_angle = (double)(length1_squ + length2_squ - length3_squ) / (2.0 * length1 * length2);
    else if ((length1 == 0)&&(length2 != 0))
        cos_angle = (double) ((right.x - vertex.x) / length2); //2012.09.07 lihuamei 
    else if ((length2 == 0) && (length1 != 0))
        cos_angle = (double) ((left.x - vertex.x) / length1); //2012.09.07 lihuamei 
    else
        cos_angle = 1;
    if (cos_angle > 1)
        angle = 0;
    else if (cos_angle < -1)
        angle = 180;
    else
        angle = acos(cos_angle) * 180.0 /PI;

	return angle;
}

double MeasureCalc::D2CalcHipAlpha(double angle1, double angle2)
{
	double alpha = angle2 - angle1;
	while(alpha >= 360)
		alpha -= 360;
	while(alpha < 0)
		alpha += 360;

	if(alpha > 180)
		alpha = 360 - alpha;
	return alpha;
}

double MeasureCalc::D2CalcHipBeta(double angle1, double angle2)
{
	double beta = 180 - (angle2 - angle1);
	while(beta >= 360)
		beta -= 360;
	while(beta < 0)
		beta += 360;
	if(beta > 180)
		beta = 360 - beta;
	return beta;
}

double MeasureCalc::D2CalcAngusty(double dist1, double dist2)
{
    double dist_tmp, angusty;
	dist_tmp = (dist1>dist2) ? (dist1-dist2) : (dist2-dist1);
	if (dist2 > ZERO)
		angusty = dist_tmp / dist2;
	else
	{
        angusty =(double) INVALID_VAL;
	}
	return angusty;
}

double MeasureCalc::CalcRatio(double dist1, double dist2)
{
	double ratio;
	if (dist2 > ZERO)
		ratio = dist1 / dist2;
	else
	{
		ratio = (double)INVALID_VAL;
	}

	return ratio;
}

///> M
double MeasureCalc::MCalcDepthDist(POINT p1, POINT p2)
{
	double scale = Img2D::GetInstance()->GetScaleMDepth();

	int dy = abs(p2.y - p1.y);

	double result = 0.0;
	result = scale * dy / 10.0;

	return result;
}


double MeasureCalc::MCalcLvTei(double dist)//M模式Teicholz方法计算左心室容积
{
	double vol;
	double distTmp;

	distTmp = dist;
	if (dist > ZERO)
		vol = (7.0*distTmp*distTmp*distTmp)/(2.4+distTmp);
	else
		vol = 0.0;

	return vol;	
}

double MeasureCalc::CalcEf(double edv, double esv)
{
	double ef;
	if (edv > ZERO)
		ef = (edv - esv) * 100.0 / edv;
	else
	{
        ef = (double)INVALID_VAL;

	}

	return ef;
}

double MeasureCalc::MCalcTime(POINT p1, POINT p2)
{
	double scale = Img2D::GetInstance()->GetScaleMTime();

	double result = scale * abs(p1.x - p2.x);

	return result;
}

double MeasureCalc::MCalcVel(POINT p1, POINT p2)
{
	double scale_depth = Img2D::GetInstance()->GetScaleMDepth();
	double scale_time = Img2D::GetInstance()->GetScaleMTime();

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;

	double result = 0.0;
	if (dx == 0)
		result = 0;
	else
		result = fabs((scale_depth * (double)dy / 10.0) / (scale_time * (double)dx));//单位化成cm和s

	return result;
}

double MeasureCalc::MCalcDepth(POINT p1, POINT p2)
{
	double scale = Img2D::GetInstance()->GetScaleMDepth();

	int dy = abs(p2.y - p1.y);

	double result = 0.0;
	result = scale * dy / 10.0;

	return result;
}

//心率计算，最后还应该加入由用户选择的几个心动周期的参数
double MeasureCalc::MCalcHR(double time)
{
	double result;
	SysMeasurementSetting sys;
	int cycle = sys.GetHeartBeatCycle();

	if (fabs(time) > ZERO)
		result = 60.0 / time * cycle;
	else
		result = (double)INVALID_VAL;


	return result;
}

double MeasureCalc::MCalcSlope(POINT p1, POINT p2)
{
	double scale_depth = Img2D::GetInstance()->GetScaleMDepth();
	double scale_time = Img2D::GetInstance()->GetScaleMTime();

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;

	double result = 0.0;
	if (dx == 0)
		result = 0;
	else 
		result = fabs((scale_depth * (double)dy / 10.0) / (scale_time * (double)dx));

	return result;
}

///> D
double MeasureCalc::DCalcVel(POINT p)
{
	double scale = ImgPw::GetInstance()->GetScaleVel();
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();

	double vel = 0.0;
    vel = scale * (baseLine - p.y);
//printf("______________-vel = %f\n", vel);
	return vel;
}

double MeasureCalc::DCalcRI(double ps, double ed)
{
	double ri;

	if (fabs(ps) > ZERO)
		ri = (ps - ed) / ps;
	else
		ri = (double)INVALID_VAL;


	return ri;
}

double MeasureCalc::DCalcSD(double ps, double ed)
{
	double sd;

	if (fabs(ed) > ZERO)
		sd = ps / ed;
	else
		sd = (double)INVALID_VAL;
;

	return sd;
}

double MeasureCalc::DCalcTime(POINT p1, POINT p2)
{
	double scale = ImgPw::GetInstance()->GetScaleTime();

	double time = fabs((p2.x - p1.x) * scale);

	return time;
}

double MeasureCalc::DCalcAccel(POINT p1, POINT p2)
{
	double scaleTime = ImgPw::GetInstance()->GetScaleTime();
	double scaleVel = ImgPw::GetInstance()->GetScaleVel();

	double accel;
	if ((p2.x != p1.x) && (scaleTime > ZERO))
		accel = ((p1.y - p2.y) * scaleVel) / ((p2.x - p1.x) * scaleTime);
	else
		accel = (double)INVALID_VAL;

	return accel;
}

double MeasureCalc::DCalcTraceAutoOther(char buf[], vector<POINT> vec, POINT ps, POINT ed)
{
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
	POINT psPoint, edPoint; 
	double psv=0.0, edv=0.0, sd=0.0, ri=0.0, tamax=0.0, pi=0.0, time=0.0, hr=0.0, pgmax=0.0, pgmean=0.0;
	int vecSize;
	TraceItem TraceCalcItem;

	vecSize = vec.size();
	if (vecSize < 2)//至少有2个描迹点才进行计算
		return 0.0;

	GetTraceItem(TraceCalcItem);
	if (!GetPsEdInVec(vec, psPoint, edPoint, baseLine, 0, true))
		return 0.0;

    if((ps.x == 0) && (ps.y == 0))
    {
        edPoint = ed;
    }

    if((ed.x == 0) && (ed.y == 0))
    {
        psPoint = ps;
    }

	psv = TraceCalcItem.psSet ? DCalcVel(psPoint) : (double)INVALID_VAL;
	if (fabs(psv) <= ZERO)
	{
		psv = TraceCalcItem.psSet ? 0.0 : (double)INVALID_VAL;
		edv = TraceCalcItem.edSet ? 0.0 : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? 0.0 : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? 0.0 : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? 0.0 : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? 0.0 : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? 0.0 : (double)INVALID_VAL;
		hr = TraceCalcItem.hrSet ? 0.0 : (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? 0.0 : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? 0.0 : (double)INVALID_VAL;
	}
	else
	{
		edv = TraceCalcItem.edSet ? DCalcVel(edPoint) : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? DCalcRI(psv, edv) : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? DCalcSD(psv, edv) : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? DCalcTamax(vec) : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? DCalcPISelf(psv, edv, tamax) : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? DCalcTime(vec[0], vec[vecSize - 1]) : (double)INVALID_VAL;
		if (((unsigned int)time != INVALID_VAL) || (time > ZERO))
			hr = TraceCalcItem.hrSet ? DCalcHRAuto(time) : (double)INVALID_VAL;
		else
			hr = (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? DCalcPGmax(psv) : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? DCalcPGmean(vec) : (double)INVALID_VAL;
	}
	//	UpdateMeasure update;
	MeasureDraw draw;
	char buf_tmp[128]= {'\0'};
	//	UpdateMeasure::ResultAttr attr;

	//	attr.cursorType = draw.GetCursorType();
	//	attr.curColor = draw.GetCurColor();
	//	attr.confirmColor = draw.GetConfirmColor();
    //
    //	测量翻译
#if 1
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "%s= %3.2fcm/s", _("PS"), psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("ED"), edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("RI"), ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("S/D"), sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("TAMAX"), tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("PI"), pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\n%s= %3.2fs", _("Time"), time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\n%s= %3.2fbpm", _("HR"), hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg",_("PGmax"), pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg", _("PGmean"), pgmean); strcat(buf, buf_tmp);}
#else
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "PS= %3.2fcm/s", psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\nED= %3.2fcm/s", edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\nRI= %3.2f", ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\nS/D= %3.2f", sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\nTAMAX= %3.2fcm/s", tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\nPI= %3.2f", pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\nTime= %3.2fs", time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\nHR= %3.2fbpm", hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\nPGmax= %3.2fmmHg", pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\nPGmean= %3.2fmmHg", pgmean); strcat(buf, buf_tmp);}
#endif
	//	update.DTrace(buf, attr, display);

	draw.DrawTraceTag(ps, FALSE);
	draw.DrawTraceTag(ed, FALSE);

    g_calcPwStatus = true;
	return 0.0;
}
//calc trace according to ps and ed 
double MeasureCalc::DCalcTraceOther(char buf[], vector<POINT> vec, POINT ps, POINT ed)
{
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
	POINT psPoint, edPoint; 
	double psv=0.0, edv=0.0, sd=0.0, ri=0.0, tamax=0.0, pi=0.0, time=0.0, hr=0.0, pgmax=0.0, pgmean=0.0;
	int vecSize;
	TraceItem TraceCalcItem;

	vecSize = vec.size();
	if (vecSize < 2)//至少有2个描迹点才进行计算
		return 0.0;

	GetTraceItem(TraceCalcItem);
	if (!GetPsEdInVec(vec, psPoint, edPoint, baseLine, 0))
		return 0.0;

    if((ps.x == 0) && (ps.y == 0))
    {
        edPoint = ed;
    }

    if((ed.x == 0) && (ed.y == 0))
    {
        psPoint = ps;
    }

	psv = TraceCalcItem.psSet ? DCalcVel(psPoint) : (double)INVALID_VAL;
	if (fabs(psv) <= ZERO)
	{
		psv = TraceCalcItem.psSet ? 0.0 : (double)INVALID_VAL;
		edv = TraceCalcItem.edSet ? 0.0 : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? 0.0 : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? 0.0 : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? 0.0 : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? 0.0 : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? 0.0 : (double)INVALID_VAL;
		hr = TraceCalcItem.hrSet ? 0.0 : (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? 0.0 : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? 0.0 : (double)INVALID_VAL;
	}
	else
	{
		edv = TraceCalcItem.edSet ? DCalcVel(edPoint) : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? DCalcRI(psv, edv) : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? DCalcSD(psv, edv) : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? DCalcTamax(vec) : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? DCalcPISelf(psv, edv, tamax) : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? DCalcTime(vec[0], vec[vecSize - 1]) : (double)INVALID_VAL;
		if (((unsigned int)time != INVALID_VAL) || (time > ZERO))
			hr = TraceCalcItem.hrSet ? DCalcHR(time) : (double)INVALID_VAL;
		else
			hr = (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? DCalcPGmax(psv) : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? DCalcPGmean(vec) : (double)INVALID_VAL;
	}
	//	UpdateMeasure update;
	MeasureDraw draw;
	char buf_tmp[128]= {'\0'};
	//	UpdateMeasure::ResultAttr attr;

	//	attr.cursorType = draw.GetCursorType();
	//	attr.curColor = draw.GetCurColor();
	//	attr.confirmColor = draw.GetConfirmColor();
    //
    //	测量翻译
#if 1
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "%s= %3.2fcm/s", _("PS"), psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("ED"), edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("RI"), ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("S/D"), sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("TAMAX"), tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("PI"), pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\n%s= %3.2fs", _("Time"), time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\n%s= %3.2fbpm", _("HR"), hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg",_("PGmax"), pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg", _("PGmean"), pgmean); strcat(buf, buf_tmp);}
#else
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "PS= %3.2fcm/s", psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\nED= %3.2fcm/s", edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\nRI= %3.2f", ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\nS/D= %3.2f", sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\nTAMAX= %3.2fcm/s", tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\nPI= %3.2f", pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\nTime= %3.2fs", time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\nHR= %3.2fbpm", hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\nPGmax= %3.2fmmHg", pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\nPGmean= %3.2fmmHg", pgmean); strcat(buf, buf_tmp);}
#endif
	//	update.DTrace(buf, attr, display);

	draw.DrawTraceTag(ps, FALSE);
	draw.DrawTraceTag(ed, FALSE);

    g_calcPwStatus = true;
	return 0.0;
}

//D描迹测量（自动计算,自动计算是算法计算一个周期）
double MeasureCalc::DCalcTraceAuto(char buf[], vector<POINT> vec, POINT &ps, POINT &ed, int auto_manual)
{
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
	POINT psPoint, edPoint; 
	double psv=0.0, edv=0.0, sd=0.0, ri=0.0, tamax=0.0, pi=0.0, time=0.0, hr=0.0, pgmax=0.0, pgmean=0.0;
	int vecSize;
	TraceItem TraceCalcItem;

	vecSize = vec.size();
	if (vecSize < 2)//至少有2个描迹点才进行计算
		return 0.0;

	GetTraceItem(TraceCalcItem);
	if (!GetPsEdInVec(vec, psPoint, edPoint, baseLine, auto_manual, true))
    {
        ps = psPoint;
        ed= edPoint;
		 return 0.0;
    }
	ps = psPoint;
	ed = edPoint;

	psv = TraceCalcItem.psSet ? DCalcVel(psPoint) : (double)INVALID_VAL;
	if (fabs(psv) <= ZERO)
	{
		psv = TraceCalcItem.psSet ? 0.0 : (double)INVALID_VAL;
		edv = TraceCalcItem.edSet ? 0.0 : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? 0.0 : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? 0.0 : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? 0.0 : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? 0.0 : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? 0.0 : (double)INVALID_VAL;
		hr = TraceCalcItem.hrSet ? 0.0 : (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? 0.0 : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? 0.0 : (double)INVALID_VAL;
	}
	else
	{
		edv = TraceCalcItem.edSet ? DCalcVel(edPoint) : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? DCalcRI(psv, edv) : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? DCalcSD(psv, edv) : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? DCalcTamax(vec) : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? DCalcPISelf(psv, edv, tamax) : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? DCalcTime(vec[0], vec[vecSize - 1]) : (double)INVALID_VAL;
		if (((unsigned int)time != INVALID_VAL) || (time > ZERO))
			hr = TraceCalcItem.hrSet ? DCalcHRAuto(time) : (double)INVALID_VAL;
		else
			hr = (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? DCalcPGmax(psv) : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? DCalcPGmean(vec) : (double)INVALID_VAL;
	}
    MeasureDraw draw;
    char buf_tmp[128]= {'\0'};
    //
    //	测量翻译
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "%s= %3.2fcm/s", _("PS"), psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("ED"), edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("RI"), ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("S/D"), sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("TAMAX"), tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("PI"), pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\n%s= %3.2fs", _("Time"), time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\n%s= %3.2fbpm", _("HR"), hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg",_("PGmax"), pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg", _("PGmean"), pgmean); strcat(buf, buf_tmp);}

    if(ps.x != 0)
        draw.DrawTraceTag(ps, FALSE);
    if(ed.x != 0)
        draw.DrawTraceTag(ed, FALSE);

	return 0.0;
}

//D描迹测量（手动描迹、自动描迹、 半自动计算, 周期数是系统预设下设置）
double MeasureCalc::DCalcTrace(char buf[], vector<POINT> vec, POINT &ps, POINT &ed, int auto_manual)
{
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
	POINT psPoint, edPoint; 
    psPoint.y = edPoint.y = baseLine;
    psPoint.x = edPoint.y = 0;
	double psv=0.0, edv=0.0, sd=0.0, ri=0.0, tamax=0.0, pi=0.0, time=0.0, hr=0.0, pgmax=0.0, pgmean=0.0;
	int vecSize;
	TraceItem TraceCalcItem;

	vecSize = vec.size();
    if (vecSize < 2)//至少有2个描迹点才进行计算
    {
        ps = psPoint;
        ed = edPoint;
        return 0.0;
    }

	GetTraceItem(TraceCalcItem);
	if (!GetPsEdInVec(vec, psPoint, edPoint, baseLine, auto_manual))
    {
        ps = psPoint;
        ed = edPoint;
		return 0.0;
    }

    ps = psPoint;
	 ed = edPoint;

	psv = TraceCalcItem.psSet ? DCalcVel(psPoint) : (double)INVALID_VAL;
	if (fabs(psv) <= ZERO)
	{
		psv = TraceCalcItem.psSet ? 0.0 : (double)INVALID_VAL;
		edv = TraceCalcItem.edSet ? 0.0 : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? 0.0 : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? 0.0 : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? 0.0 : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? 0.0 : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? 0.0 : (double)INVALID_VAL;
		hr = TraceCalcItem.hrSet ? 0.0 : (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? 0.0 : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? 0.0 : (double)INVALID_VAL;
	}
	else
	{
		edv = TraceCalcItem.edSet ? DCalcVel(edPoint) : (double)INVALID_VAL;
		ri = TraceCalcItem.riSet ? DCalcRI(psv, edv) : (double)INVALID_VAL;
		sd = TraceCalcItem.sdSet ? DCalcSD(psv, edv) : (double)INVALID_VAL;
		tamax = TraceCalcItem.tamaxSet ? DCalcTamax(vec) : (double)INVALID_VAL;
		pi = TraceCalcItem.piSet ? DCalcPISelf(psv, edv, tamax) : (double)INVALID_VAL;
		time = TraceCalcItem.timeSet ? DCalcTime(vec[0], vec[vecSize - 1]) : (double)INVALID_VAL;
		if (((unsigned int)time != INVALID_VAL) || (time > ZERO))
			hr = TraceCalcItem.hrSet ? DCalcHR(time) : (double)INVALID_VAL;
		else
			hr = (double)INVALID_VAL;
		pgmax = TraceCalcItem.pgmaxSet ? DCalcPGmax(psv) : (double)INVALID_VAL;
		pgmean = TraceCalcItem.pgmeanSet ? DCalcPGmean(vec) : (double)INVALID_VAL;
	}
	//	UpdateMeasure update;
	MeasureDraw draw;
	char buf_tmp[128]= {'\0'};
	//	UpdateMeasure::ResultAttr attr;

	//	attr.cursorType = draw.GetCursorType();
	//	attr.curColor = draw.GetCurColor();
	//	attr.confirmColor = draw.GetConfirmColor();
    //
    //	测量翻译
#if 1
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "%s= %3.2fcm/s", _("PS"), psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("ED"), edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("RI"), ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("S/D"), sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\n%s= %3.2fcm/s", _("TAMAX"), tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\n%s= %3.2f", _("PI"), pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\n%s= %3.2fs", _("Time"), time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\n%s= %3.2fbpm", _("HR"), hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg",_("PGmax"), pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\n%s= %3.2fmmHg", _("PGmean"), pgmean); strcat(buf, buf_tmp);}
#else
	if (TraceCalcItem.psSet) {sprintf(buf_tmp, "PS= %3.2fcm/s", psv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.edSet) {sprintf(buf_tmp, "\nED= %3.2fcm/s", edv); strcat(buf, buf_tmp);}
	if (TraceCalcItem.riSet) {sprintf(buf_tmp, "\nRI= %3.2f", ri); strcat(buf, buf_tmp);}
	if (TraceCalcItem.sdSet) {sprintf(buf_tmp, "\nS/D= %3.2f", sd); strcat(buf, buf_tmp);}
	if (TraceCalcItem.tamaxSet) {sprintf(buf_tmp, "\nTAMAX= %3.2fcm/s", tamax/10.0); strcat(buf, buf_tmp);}
	if (TraceCalcItem.piSet) {sprintf(buf_tmp, "\nPI= %3.2f", pi); strcat(buf, buf_tmp);}
	if (TraceCalcItem.timeSet) {sprintf(buf_tmp, "\nTime= %3.2fs", time); strcat(buf, buf_tmp);}
	if (TraceCalcItem.hrSet) {sprintf(buf_tmp, "\nHR= %3.2fbpm", hr); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmaxSet) {sprintf(buf_tmp, "\nPGmax= %3.2fmmHg", pgmax); strcat(buf, buf_tmp);}
	if (TraceCalcItem.pgmeanSet) {sprintf(buf_tmp, "\nPGmean= %3.2fmmHg", pgmean); strcat(buf, buf_tmp);}
#endif
	//	update.DTrace(buf, attr, display);

	draw.DrawTraceTag(ps, FALSE);
	draw.DrawTraceTag(ed, FALSE);
    
    g_calcPwStatus = true;

	return 0.0;
}

bool MeasureCalc::GetPsEdInVec(vector<POINT> vec, POINT &psP, POINT &edP, int baseLine, int auto_manual, bool auto_calc)
{
	int vecSize;
	int i;

	vecSize = vec.size();
	if (vecSize < 2)
		return false;
   int cycle = 1;
   if(!auto_calc)
   {
       SysMeasurementSetting sys;
       cycle = sys.GetHeartBeatCycle();
   }

   int vecSizeEveryCycle =(int)((double)vecSize/cycle + 0.5);

	psP = vec[0];
	edP = vec[0];
    int psPosition = 0;
    ///>至少留下3/16周期，因为要从3/16中找到ed
    for (i=1; i<vecSize*13/16; i++)
	{
        PRINTF("i:%d, value:%d, baseLine:%d \n", i, vec[i].y, baseLine);
		if (abs(baseLine - psP.y) < abs(baseLine - vec[i].y))
		{
			psP = vec[i];
			psPosition = i;
		}
	}

    //printf("positon:%d size(%d, %d)\n",psPosition, vecSize, vecSizeEveryCycle);
    if(psPosition == vecSize - 1 )
    {
        psP.x = edP.x = 0;
        return false;
    }

	edP = psP;
    int edPEnd = psPosition + vecSizeEveryCycle;
    if(edPEnd > vecSize)
        edPEnd = vecSize;
    int edPBegin = edPEnd - vecSizeEveryCycle*3/16;
    if(edPBegin < 0)
        edPBegin = 0;

    ///>ps位置从后往前找，找1/8个周期
	unsigned char sign = 0;
    edP = vec[edPEnd - 1];
    for (i=edPEnd - 2; i>edPBegin; i--)
    {
        if (((baseLine - edP.y)*(baseLine - vec[i].y) < 0) && (sign == 0))
            sign = 1;
        //printf("sign:%d baseLine=%d, ed.y=%d, vec[%d].y=%d\n", sign, baseLine, edP.y, vec[i].y);
        if (sign == 1)
        {
            edP = (abs(baseLine - edP.y) < abs(baseLine - vec[i].y)) ? vec[i] : edP;
        }
        else
        {
            edP = (abs(baseLine - edP.y) > abs(baseLine - vec[i].y)) ? vec[i] : edP;
        }
    }
    ///> 当没有图像的时候，算法计算的值是非零值,此时的ps/ed应该无效
    if((psP.y == edP.y) || (edP.x == psP.x))
    {
        psP.x = edP.x = 0;
        return false;
    }

	return true;
}

void MeasureCalc::DCalcPI(vector<POINT> vec, POINT &ps, POINT &ed, double &psValue, double &edValue, double &tamaxValue, double &piValue)
{
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
	POINT psPoint, edPoint; 
	int vecSize;

	vecSize = vec.size();
	if (vecSize < 2)
	{
		psValue = 0.0;
		edValue = 0.0;
		tamaxValue = 0.0;
		piValue = 0.0;
		return;
	}
	if (!GetPsEdInVec(vec, psPoint, edPoint, baseLine, 1))
		return;
	ps = psPoint;
	ed = edPoint;

	psValue = DCalcVel(psPoint);
	if (fabs(psValue) < ZERO)
	{
		psValue = 0.0;
		edValue = 0.0;
		tamaxValue = 0.0;
		piValue = 0.0;
	}
	else
	{
		edValue = DCalcVel(edPoint);
		tamaxValue = DCalcTamax(vec);
		if (fabs(tamaxValue) < ZERO)
			piValue = (double)INVALID_VAL;

		else
			piValue = DCalcPISelf(psValue, edValue, tamaxValue);
	}

	return;
}

double MeasureCalc::DCalcPISelf(double ps, double ed, double tamax)
{
	if (fabs(tamax) < ZERO)
		return (double)INVALID_VAL;
	else
		return((ps - ed) / tamax);
}

double MeasureCalc::DCalcTamax(vector<POINT> vec)
{
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();
	double scaleVel = ImgPw::GetInstance()->GetScaleVel();
	double tamax;
	int Vmean;
	if (GetVmeanFromTrace(vec, baseLine, Vmean))
		tamax = scaleVel * Vmean;
	else
		tamax = 0.0;

	return tamax;
}
/*
 * @brief calculate hear rate
 *
 * @para p1, p2[in] start point and end point
 * @ret[out] hear rate value, unit: b/min
 */
//心率计算 自动计算周期=1
double MeasureCalc::DCalcHRAuto(double time)
{
	int cycle = 1;
	double result;
	if ((int)time == INVALID_VAL)
		result = (double)INVALID_VAL;
	else
		result = (fabs(time) > ZERO) ? (60.0 / time * cycle) : 0.0;

	return result;
}
/*
 * @brief calculate hear rate
 *
 * @para p1, p2[in] start point and end point
 * @ret[out] hear rate value, unit: b/min
 */
//心率计算，最后还应该加入由用户选择的几个心动周期的参数
double MeasureCalc::DCalcHR(double time)
{
	SysMeasurementSetting sys;
	int cycle = sys.GetHeartBeatCycle();
	double result;
	if ((int)time == INVALID_VAL)
		result = (double)INVALID_VAL;
	else
		result = (fabs(time) > ZERO) ? (60.0 / time * cycle) : 0.0;

	return result;
}

/*
 * @brief calculate max pressure gradient
 *
 * @para p1[in] point that max velocity is located
 * @ret value of max pressure gradient, unit mmHg
 */
double MeasureCalc::DCalcPGmax(double Vmax)
{
	double PGmax = 4 * Vmax * Vmax / 10000;

	return PGmax;
}

double MeasureCalc::DCalcPGmean(vector<POINT> vec)
{
	double scaleVel = ImgPw::GetInstance()->GetScaleVel();
	int baseLine = ImgPw::GetInstance()->GetBaseLineForCalc();

	int Vmean2;
	double PGmean;
	if (GetVmean2FromTrace(vec, baseLine, Vmean2))
		PGmean = 4 * Vmean2 * scaleVel * scaleVel / (100*100); ///< Vmean2 = (v1*v1 + v2*v2 + ...+vn*vn) / n
	else
		PGmean = 0.0;

	return PGmean;
} 

///> private
/*
 * @brief get some specitial value from trace curve
 *
 * @para vec[in] vector point in trace saved in it
 * @para baseLine[in] baseline of spectrum
 * @para ps[out] ps value in trace
 * @para ed[out] ed value in trace
 * @para vmean[out] mean velocity in trace
 */
void MeasureCalc::GetValueFromTrace(vector<POINT> vec, int baseLine, int &ps, int &ed, int &vmean)
{
	ps = 0;
	ed = 0;
	vmean = 0;
}

/*
 * @brief get Vmean2 value from trace
 *
 * @para vec[in]  vector point in trace saved in it
 * @para baseLine[in] baseline of spectrum
 * @para vmean2[out] integral of velocity square in trace, Vmean2 = (v1*v1 + v2*v2 + ...+vn*vn) / n
 */
bool MeasureCalc::GetVmean2FromTrace(vector<POINT> vec, int baseLine, int &vmean2)
{
	int i;
	int sizeVec = vec.size();

	if (sizeVec == 0)
		return FALSE;
	else if (sizeVec == 1)	
	{
		vmean2 = (vec[0].y - baseLine) * (vec[0].y - baseLine);	
		return TRUE;
	}

	int* vel;
	int count = 0;
	int v;
	int xInterval;

	vel = (int *)malloc(sizeVec*sizeof(int));
	if (vel == NULL)
		return FALSE;

	vmean2 = 0;
	for (i = 0; i < (sizeVec - 1); i ++)
	{
		*(vel + i) = (vec[i].y + vec[i+1].y) / 2;
		xInterval = abs(vec[i+1].x - vec[i].x);
		v = *(vel + i) - baseLine;
		vmean2 += v * v * xInterval;	
		count += xInterval;
	}
	vmean2 = vmean2 / count;	
	free(vel);
	return TRUE;
}

/*
 * @brief get Vmean value from trace
 *
 * @para vec[in]  vector point in trace saved in it
 * @para baseLine[in] baseline of spectrum
 * @para vmean[out] integral of velocity in trace, Vmean = (v1 + v2 + ...+vn) / n
 */
bool MeasureCalc::GetVmeanFromTrace(vector<POINT> vec, int baseLine, int &vmean)
{
	int i;
	int sizeVec = vec.size();

	if (sizeVec == 0)
		return FALSE;
	else if (sizeVec == 1)
	{
		//vmean = (vec[0].y - baseLine);	
		vmean = ( baseLine - vec[0].y);	
		return TRUE;
	}

	int* vel;
	int count = 0;
	int v;
	int xInterval;

	vel = (int *)malloc(sizeVec*sizeof(int));
	if (vel == NULL)
		return FALSE;

	vmean = 0;
	for (i = 0; i < (sizeVec - 1); i++)
	{
		*(vel + i) = (vec[i].y + vec[i+1].y) / 2;
		xInterval = abs(vec[i+1].x - vec[i].x);
		v = baseLine - *(vel + i);
		vmean += v * xInterval;
		count += xInterval;
	}
	vmean = vmean / count;	
	free(vel);
	return TRUE;
}

int MeasureCalc::GetHRCycleNum()
{
	return 2;
}

