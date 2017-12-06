#ifndef MEASURE_2D_H
#define MEASURE_2D_H

#include <string>
#include "Def.h"
#include "AbsMeasure.h"
#include "measure/UpdateMeasure.h"
#include "measure/MeasureCalc.h"
#include "measure/MeasureDraw.h"
#include "measure/MeasureMan.h"
#include "imageProc/ModeStatus.h"

using std::string;

///>两点法测距离
//>深度方向上测距离
class D2MeasureDistDot: public AbsMeasure
{
    public:
    D2MeasureDistDot(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureDistDot();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_dist;
    int m_item;
    const SingleItemInfo *m_itemInfo;
    int m_2DRotate;
};

//两线法测距离
class D2MeasureDistLine: public AbsMeasure
{
    public:
    D2MeasureDistLine(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureDistLine();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;

    POINT m_p11;//测量范围左上顶点坐标
    POINT m_p22;//测量范围右下顶点坐标
    //两线的端点
    POINT m_line1_start;
    POINT m_line1_end;
    POINT m_line2_start;
    POINT m_line2_end;

    int m_item;
    const SingleItemInfo *m_itemInfo;
    double m_tan;
    double m_dist;

};

//轨迹法测长度
class D2MeasureLengthTrack: public AbsMeasure
{
    public:
    D2MeasureLengthTrack(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureLengthTrack();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Change();
    void Esc();
    void Value(EKnobOper opr);

    private:
    double m_length;
    vector<POINT> m_track;//存储轨迹用于擦除
    vector<POINT> m_trackTemp;//临时存储擦除了的轨迹,用于重绘

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_line_tempP;
    POINT m_tempP;
    POINT m_p1;
    int m_item;
    const SingleItemInfo *m_itemInfo;

    const unsigned char MOUSE_INTERVAL;
    unsigned char m_mouse_count;
};

class D2MeasureLengthDot: public AbsMeasure
{
    public:
    D2MeasureLengthDot(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureLengthDot();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Change();
    void Esc();

    private:
    double m_length;
    vector<POINT> m_dot;//存储描点位置用于擦除

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_item;
    const SingleItemInfo *m_itemInfo;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
};

class D2MeasureAreaTrack: public AbsMeasure
{
    public:
    D2MeasureAreaTrack(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureAreaTrack();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Value(EKnobOper opr);
    void Change();
    void Esc();

    private:
    double m_area;
    double m_perimeter;
    vector<POINT> m_track;//存储轨迹用于擦除
    vector<POINT> m_trackTemp;//临时存储擦除了的轨迹,用于重绘

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_line_tempP;
    POINT m_tempP;
    POINT m_p1;
    int m_item;
    const SingleItemInfo *m_itemInfo;

    const unsigned char MOUSE_INTERVAL;
    unsigned char m_mouse_count;
};

class D2MeasureSimpson: public AbsMeasure
{
    public:
    D2MeasureSimpson(const MultiItemInfo *ptrMultiItemInfo);
    ~D2MeasureSimpson();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Value(EKnobOper opr);
    void Change();
    void Esc();
    int LenEnd(POINT mousePoint, int vecPosi);
    int DiamLine(POINT lenStart, POINT lenEnd, int lenEndPosi, bool isCur);

    private:
    vector<POINT> m_track;//存储轨迹用于擦除
    vector<POINT> m_trackTemp;//临时存储擦除了的轨迹,用于重绘

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    double m_area;
    double m_length;
    double m_vol;

    int m_step; ///< current status(step)
    bool m_isDraw;
    unsigned int m_lenPosi;
    POINT m_line_tempP;
    POINT m_tempP;
    POINT m_p1;
    POINT m_lenStart;
    int m_item;
    int m_mouseSign;
    const MultiItemInfo *m_itemInfo;

    const unsigned char MOUSE_INTERVAL;
    unsigned char m_mouse_count;

    int m_directX;
    int m_directY;
    int m_direct;
    unsigned char m_slowMouse;
};

class D2MeasureAL: public AbsMeasure
{
    public:
    D2MeasureAL(const MultiItemInfo *ptrMultiItemInfo);
    ~D2MeasureAL();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Value(EKnobOper opr);
    void Change();
    void Esc();

    private:
    vector<POINT> m_track;//存储轨迹用于擦除
    vector<POINT> m_trackTemp;//临时存储擦除了的轨迹,用于重绘

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    double m_area;
    double m_length;
    double m_vol;

    int m_step; ///< current status(step)
    bool m_isDraw;
    int m_lenPosi;
    POINT m_line_tempP;
    POINT m_tempP;
    POINT m_p1;
    POINT m_lenStart;
    int m_item;
    int m_mouseSign;
    const MultiItemInfo *m_itemInfo;

    const unsigned char MOUSE_INTERVAL;
    unsigned char m_mouse_count;
};

class D2MeasureAreaDot: public AbsMeasure
{
    public:
    D2MeasureAreaDot(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureAreaDot();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Change();
    void Esc();

    private:
    double m_area;
    double m_perimeter;
    vector<POINT> m_dot;//存储描点位置用于擦除

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_item;
    const SingleItemInfo *m_itemInfo;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_dot_tempP;
    POINT m_p1;
};

/*
class D2MeasureAreaEllipse: public AbsMeasure
{
    public:
    D2MeasureAreaEllipse();
    ~D2MeasureAreaEllipse();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_temp_real;
    POINT m_l_start;    //长轴起始点
    POINT m_l_end;  //长轴终止点
    POINT m_s_start;        //短轴起始点
    double m_perimeter;
    double m_area;
};
*/

class D2MeasureAreaRec: public AbsMeasure
{
    public:
    D2MeasureAreaRec(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureAreaRec();

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
    POINT m_tempP;
    POINT m_p1;
    double m_area;
};

class D2MeasureVol3Axis: public AbsMeasure
{
    public:
    //D2MeasureVol3Axis(const char *name, int item);
    D2MeasureVol3Axis(const char *name, const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureVol3Axis();

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
    POINT m_tempP;
    POINT m_p1;
    POINT m_x_start;
    POINT m_x_end;
    POINT m_y_start;
    POINT m_y_end;
    POINT m_z_start;
    POINT m_z_end;
    double m_x_len;
    double m_y_len;
    double m_z_len;
    double m_vol;
    //int m_item;
    char m_name[20];
};

class D2MeasureSac3Axis: public AbsMeasure
{
    public:
    D2MeasureSac3Axis(const MultiItemInfo *ptrMultiItemInfo);
    ~D2MeasureSac3Axis();

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
    const MultiItemInfo *m_itemInfo;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    POINT m_x_start;
    POINT m_x_end;
    POINT m_y_start;
    POINT m_y_end;
    POINT m_z_start;
    POINT m_z_end;
    double m_x_len;
    double m_y_len;
    double m_z_len;
};

class D2MeasureVolEllipse1: public AbsMeasure
{
    public:
    D2MeasureVolEllipse1(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureVolEllipse1();

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
    double m_vol;
    double m_area;
    double m_perimeter;
    int m_item;
    const SingleItemInfo *m_itemInfo;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_temp_real;
    POINT m_l_start;    //长轴起始点
    POINT m_l_end;      //长轴终止点
    POINT m_s_start;    //短轴起始点
    double  m_SA_P;     //轴长
//  int m_method;

//  void CalcResult(void);
};

class D2MeasureVolEllipse2: public AbsMeasure
{
    public:
    D2MeasureVolEllipse2(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureVolEllipse2();

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
    POINT m_tempP;
    POINT m_temp_real;
    POINT m_p1;
    POINT m_l_start;    //长轴起始点
    POINT m_l_end;      //长轴终止点
    POINT m_s_start;    //短轴起始点
    double  m_SA_P;     //轴长
    double m_area;
    double m_dist;
    double m_vol;
};

class D2MeasureVolSpheroid: public AbsMeasure
{
    public:
    D2MeasureVolSpheroid(const MultiItemInfo *ptrMultiItemInfo);
    ~D2MeasureVolSpheroid();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_diam;
    double m_vol;
    const MultiItemInfo *m_itemInfo;
};

class D2MeasureAngleSingle: public AbsMeasure
{
    public:
    D2MeasureAngleSingle(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureAngleSingle();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_vertex;
    POINT m_left;
    POINT m_right;
    double m_angle;
    const SingleItemInfo *m_itemInfo;
};

class D2MeasureAngle: public AbsMeasure
{
    public:
    D2MeasureAngle(int method,const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureAngle();

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
    POINT m_tempP;
    POINT m_vertex;
    POINT m_left;
    POINT m_right;
    double m_dist_l;
    double m_dist_r;
    double m_angle;
    int m_method;
};

#if 1
class D2MeasureHip: public AbsMeasure
{
    public:
    D2MeasureHip(const MultiItemInfo *ptrMultiItemInfo);
    ~D2MeasureHip();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Value(EKnobOper opr);
    void Change();
    void Esc();

    private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    int m_len;
    bool m_isDraw;
    POINT m_tempP;
    POINT m_bl_p1, m_bl_p2;
    POINT m_rl_p1, m_rl_p2;
    POINT m_il_p1, m_il_p2;
    double m_theta;
    double m_alpha;//α
    double m_beta;//β
    struct Record{
        POINT point;
        double angle;
    }m_blRecord, m_rlRecord, m_ilRecord;

    const MultiItemInfo *m_itemInfo;
};
#endif

class D2MeasureDepthDist: public AbsMeasure
{
    public:
    D2MeasureDepthDist();
    ~D2MeasureDepthDist();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    double m_dist;
    int m_method;
};

//两点法距离狭窄比
class D2MeasureRatioDistDot: public AbsMeasure
{
    public:
    D2MeasureRatioDistDot(int method,const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureRatioDistDot();

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
    POINT m_tempP;
    POINT m_dist1_start;
    POINT m_dist1_end;
    POINT m_dist2_start;
    POINT m_dist2_end;
    double m_dist1;
    double m_dist2;
    double m_ratio;
    double m_angusty;
    int m_method;
};

//距离周长比例测量
class D2MeasureRatioDistPeri: public AbsMeasure
{
    public:
    D2MeasureRatioDistPeri(const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureRatioDistPeri();

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
    POINT m_p1;
    POINT m_tempP;
    POINT m_tempP1;
    POINT m_dist_start;
    POINT m_dist_end;
    POINT m_l_start;    //长轴起始点
    POINT m_l_end;  //长轴终止点
    POINT m_s_start;        //短轴起始点
    POINT m_temp_real;
    double m_dist;
    double m_perimeter;
    double m_ratio;
};

/*
//面积比例测量（使用轨迹法）
class D2MeasureRatioAreaTrack: public AbsMeasure
{
    public:
    D2MeasureRatioAreaTrack();
    ~D2MeasureRatioAreaTrack();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Change();
    void Esc();

    private:
    double m_area1;
    double m_area2;
    double m_ratio;
    vector<POINT> Track1;//存储轨迹用于擦除
    vector<POINT> Track2;//存储轨迹用于擦除

    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_line_tempP;
    POINT m_tempP;
    POINT m_p1;
    POINT m_trace1_start;
    POINT m_trace1_end;
    POINT m_trace2_start;
    POINT m_trace2_end;

    const unsigned char MOUSE_INTERVAL;
    unsigned char m_mouse_count;
};
*/

class D2MeasureRatioArea: public AbsMeasure
{
    public:
    D2MeasureRatioArea(int method,const SingleItemInfo *ptrSingleItemInfo);
    ~D2MeasureRatioArea();

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
    POINT m_tempP;
    POINT m_temp_real;
    POINT m_l1_start;   //长轴起始点
    POINT m_l1_end; //长轴终止点
    POINT m_s1_start;       //短轴起始点
    POINT m_l2_start;   //长轴起始点
    POINT m_l2_end; //长轴终止点
    POINT m_s2_start;       //短轴起始点
    double m_area1;
    double m_area2;
    double m_vol1;
    double m_vol2;
    double m_angusty;
    double m_ratio;
    int m_method;
};

class D2MeasureRatioAngle: public AbsMeasure
{
    public:
    D2MeasureRatioAngle(const MultiItemInfo *ptrMultiItemInfo);
    ~D2MeasureRatioAngle();

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

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_vertex1;
    POINT m_left1;
    POINT m_right1;
    POINT m_vertex2;
    POINT m_left2;
    POINT m_right2;
    double m_angle1;
    double m_angle2;
    double m_ratio;
    const MultiItemInfo *m_itemInfo;
};

class D2MeasureHistogram: public AbsMeasure
{
    public:
    D2MeasureHistogram();
    ~D2MeasureHistogram();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Change();
    void DrawHistogram(char *image_p);
    void Esc();

    private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    POINT m_area_start;
    POINT m_area_end;
    unsigned int m_sum[256];
};
/*
class DrawHistogram
{
    public:
    DrawHistogram();
    ~DrawHistogram();

    static void SetArea(POINT area_start, POINT area_end);
    static void HistogramDraw(unsigned char *image_p);

    private:
    static POINT m_area_start;
    static POINT m_area_end;
    static int m_sum[256];
    static MeasureDraw m_draw;
};
*/
class D2MeasureProfile: public AbsMeasure
{
    public:
    D2MeasureProfile();
    ~D2MeasureProfile();

    void Init();
    void PressLeft(POINT p); ///< do
    void PressRight(POINT p); ///< undo
    void MouseMove(POINT p);
    void Change();
    void Esc();
    void DrawProfile(POINT start, POINT end, unsigned char *image_p);

    private:
    MeasureCalc m_calc;
    MeasureDraw m_draw;
    UpdateMeasure m_update;
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_tempP;
    POINT m_p1;
    POINT m_start;
    POINT m_end;
};

POINT CycleVertex( POINT p1, POINT p2 );

class D2MeasureIMT: public AbsMeasure
{
        public:
            D2MeasureIMT(bool dir);
            ~D2MeasureIMT();
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
    MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    bool m_ImtStatus;
    bool m_RealIMTStatus;
    bool m_dir;
    POINT m_tempP;
    POINT m_p1;
    static POINT m_tempP_old;
    static POINT m_p1_old;
    static unsigned int *pIntima;
    static unsigned int *pAdventitia;
    unsigned char *pRoiPre;
    unsigned char *pRoi;

    static int roi_width;
    static int roi_height;
    POINT start;
    POINT end;
};
#endif