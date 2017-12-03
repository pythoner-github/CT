// -*- c++ -*-
#ifndef MEASURE_M_H
#define MEASURE_M_H

#include "AbsMeasure.h"
#include "UpdateMeasure.h"
#include "MeasureCalc.h"
#include "MeasureDraw.h"
#include "MeasureMan.h"

class MMeasureTime: public AbsMeasure
{
public:
    MMeasureTime(const SingleItemInfo *ptrSingleItemInfo);
    ~MMeasureTime();
    
    void Init();
    void PressLeft(POINT p);
    void PressRight(POINT p);
    void MouseMove(POINT p);
    void Change();
    void Esc();
    
private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan * m_ptrMan;
    
    int m_step;
    int m_method;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_time;
    double m_hr;
    int m_item;
    const SingleItemInfo *m_itemInfo;
};

class MMeasureVel: public AbsMeasure
{
public:
    MMeasureVel(const SingleItemInfo *ptrSingleItemInfo);
    ~MMeasureVel();
    
    void Init();
    void PressLeft(POINT p);
    void PressRight(POINT p);
    void MouseMove(POINT p);
    void Change();
    void Esc();

private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan * m_ptrMan;

    int m_step;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_result;
    int m_method;//0:速度测量，1：斜率测量
    int m_item;
    const SingleItemInfo *m_itemInfo;
};

class MMeasureDistSlope: public AbsMeasure
{
public:
    MMeasureDistSlope(const MultiItemInfo *ptrMultiItemInfo);
    ~MMeasureDistSlope();
    
    void Init();
    void PressLeft(POINT p);
    void PressRight(POINT p);
    void MouseMove(POINT p);
    void Change();
    void Esc();

private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan * m_ptrMan;

    int m_step;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_dist;
    double m_slope;
    const MultiItemInfo *m_itemInfo;

    void Result(int saveR, UpdateMeasure::ResultAttr& attr);
};

class MMeasureDepth: public AbsMeasure
{
public:
    MMeasureDepth(const SingleItemInfo *ptrSingleItemInfo);
    ~MMeasureDepth();
    
    void Init();
    void PressLeft(POINT p);
    void PressRight(POINT p);
    void MouseMove(POINT p);
    void Change();
    void Esc();

private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan * m_ptrMan;

    int m_step;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_dist;
    int m_item;
    const SingleItemInfo *m_itemInfo;
};

class MMeasureHR: public AbsMeasure
{
public:
    MMeasureHR();
    ~MMeasureHR() {};
    
    void Init();
    void PressLeft(POINT p);
    void PressRight(POINT p);
    void MouseMove(POINT p);
    void Change();
    void Esc();

private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan * m_ptrMan;

    int m_step;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_hr;
};

//射血分数测量－M模式Teicholz方法
class MMeasureEFTei: public AbsMeasure
{
    public:
	MMeasureEFTei(const SingleItemInfo *ptrSingleItemInfo);
	~MMeasureEFTei();

	void Init();
	void PressLeft(POINT p); ///< do
	void PressRight(POINT p); ///< undo
	void MouseMove(POINT p);
	void Change();
	void Esc();

    private:
	MeasureCalc m_calc;
	MeasureDraw m_draw;
	UpdateMeasure m_update;
	MeasureMan* m_ptrMan;

    int m_item;
    const SingleItemInfo *m_itemInfo;

	int m_step; ///< current status(step)
	bool m_isDraw;
	POINT m_lvdd_start;
	POINT m_lvdd_end;
	POINT m_lvds_start;
	POINT m_lvds_end;
	POINT m_tempP;
	POINT m_p1;
	double m_lvdd;//左室舒张直径
	double m_lvds;//左室收缩直径
	double m_lvvd;//左室舒张体积
	double m_lvvs;//左室收缩体积
	double m_ef;
};

#if 0
class MMeasureSlope: public AbsMeasure
{
public:
    MMeasureSlope();
    ~MMeasureSlope() {};
    
    void Init();
    void PressLeft(POINT p);
    void PressRight(POINT p);
    void MouseMove(POINT p);
private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan * m_ptrMan;

    int m_step;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
};
#endif
#endif
