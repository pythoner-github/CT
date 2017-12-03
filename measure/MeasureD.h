#ifndef MEASURE_D_H
#define MEASURE_D_H

#include "Def.h"
#include "AbsMeasure.h"
#include "UpdateMeasure.h"
#include "MeasureCalc.h"
#include "MeasureDraw.h"
#include "MeasureMan.h"
//轨迹法手动描迹
class DMeasureManualTrack: public AbsMeasure
{
    public:
	DMeasureManualTrack();
	~DMeasureManualTrack();

	void Init();
	void PressLeft(POINT p); ///< do
	void PressRight(POINT p); ///< undo
	void MouseMove(POINT p);
    void Value(EKnobOper opr);
    void Change();
	void Esc();

    private:
	double m_ps;
	double m_ed;
	double m_sd;
	double m_ri;
	double m_tamax;
	double m_pi;
	int m_hr;
	POINT m_psTmp;
	POINT m_psP;
	POINT m_edP;

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

	const unsigned char MOUSE_INTERVAL;
	unsigned char m_mouse_count;
};

class DMeasureManualDot: public AbsMeasure
{
    public:
	DMeasureManualDot();
	~DMeasureManualDot();

	void Init();
	void PressLeft(POINT p); ///< do
	void PressRight(POINT p); ///< undo
	void MouseMove(POINT p);
	void Change();
	void Esc();

    private:
	double m_ps;
	double m_ed;
	double m_sd;
	double m_ri;
	double m_tamax;
	double m_pi;
	int m_hr;
	POINT m_psTmp;
	vector<POINT> m_dot;//存储描点位置用于擦除   
	POINT m_psP;
	POINT m_edP;

	MeasureCalc m_calc;
	MeasureDraw m_draw;
	UpdateMeasure m_update;
	MeasureMan* m_ptrMan;

	int m_step; ///< current status(step)
	bool m_isDraw;
	POINT m_tempP;
	POINT m_p1;
};

class DMeasureManualAuto: public AbsMeasure
{
    public:
	DMeasureManualAuto();
	~DMeasureManualAuto();

	void Init();
	void PressLeft(POINT p); ///< do
	void PressRight(POINT p); ///< undo
	void MouseMove(POINT p);
    void Value(EKnobOper opr);
    void Change();
	void Esc();

    private:
	POINT m_startP;
	POINT m_endP;
	vector<POINT> m_track;//存储轨迹用于擦除
	POINT m_psP;
	POINT m_edP;
    POINT m_cursor;
    bool m_cursorTypeChg;
    bool m_auto;

	MeasureCalc m_calc;
	MeasureDraw m_draw;
	UpdateMeasure m_update;
	MeasureMan* m_ptrMan;

	int m_step; ///< current status(step)
    bool m_isDraw;
    //const static int MAX_OPT = 4;

    void AutoTraceInit();
    void AutoCalc(); //全自动
    void SemiAutoCalc();//半自动
};


class DMeasureIntegralTrack: public AbsMeasure
{
    public:
	DMeasureIntegralTrack(const SingleItemInfo *ptrSingleItemInfo);
	~DMeasureIntegralTrack();

	void Init();
	void PressLeft(POINT p); ///< do
	void PressRight(POINT p); ///< undo
	void MouseMove(POINT p);
	void Change();
	void Esc();

    private:
	double m_integral_area;
	double m_perimeter;
	vector<POINT> m_track;//存储轨迹用于擦除  

	MeasureCalc m_calc;
	MeasureDraw m_draw;
	UpdateMeasure m_update;
	MeasureMan* m_ptrMan;

    int m_step; ///< current status(step)
    bool m_isDraw;
    POINT m_line_tempP;
    POINT m_tempP;
    POINT m_back;
    POINT m_p1;
    int m_item;
    const SingleItemInfo *m_itemInfo;

	const unsigned char MOUSE_INTERVAL;
	unsigned char m_mouse_count;
};




/*
 * @brief measure velocity in doppler mode
 */
class DMeasureVel :public AbsMeasure
{
	public:
		DMeasureVel(const SingleItemInfo *ptrSingleItemInfo);
		~DMeasureVel();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
		void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
		UpdateMeasure m_update;
		MeasureMan* m_ptrMan;

		int m_method;
		bool m_isDraw;
		POINT m_tempP;
		double m_vel;
		double m_PGmax;
		int m_item;
		const SingleItemInfo *m_itemInfo;
};

/*
 * @brief measure PGmax in doppler mode
 */
class DMeasurePGmax :public AbsMeasure
{
	public:
		DMeasurePGmax(const MultiItemInfo *ptrMultiItemInfo);
		~DMeasurePGmax();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
		void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
		UpdateMeasure m_update;
		MeasureMan* m_ptrMan;

		int m_method;
		bool m_isDraw;
		POINT m_tempP;
		double m_vel;
		double m_PGmax;
		int m_item;
		const MultiItemInfo *m_itemInfo;
};

/*
 * @brief measure resistance index in doppler mode
 */
class DMeasureRI :public AbsMeasure
{
	public:
		DMeasureRI(const MultiItemInfo *ptrMultiItemInfo);
		~DMeasureRI();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
		void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
		UpdateMeasure m_update;
		MeasureMan* m_ptrMan;

		int m_step;
		bool m_isDraw;
		POINT m_tempP;
		POINT m_p1;
		double m_ps;
		double m_ed;
		double m_ri;
		double m_sd;
		char m_name[20];
		int m_item;
		int m_method; //0: PS+ED  1: PS+ED+RI  2: PS+ED+RI+S/D
		const MultiItemInfo *m_itemInfo;
};

/*
 * @brief measure SD(velocity ratio of systole and diastole) in doppler mode
 */
class DMeasureSD :public AbsMeasure
{
	public:
		DMeasureSD();
		~DMeasureSD();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
		void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
		UpdateMeasure m_update;
		MeasureMan* m_ptrMan;

		int m_step;
		bool m_isDraw;
		POINT m_tempP;
		POINT m_p1;
		double m_ps;
		double m_ed;
		double m_sd;
};

/*
 * @brief measure time in doppler mode
 */
class DMeasureTime :public AbsMeasure
{
	public:
		DMeasureTime(const SingleItemInfo *ptrSingleItemInfo);
		~DMeasureTime();

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

		int m_step;
		int m_method;
		bool m_isDraw;
		POINT m_tempP;
		POINT m_p1;
		double m_time;
		int m_item;
		const SingleItemInfo *m_itemInfo;
};

#if 1
/*
 * @brief measure time in doppler mode
 */
class DMeasureHR :public AbsMeasure
{
	public:
		DMeasureHR(const MultiItemInfo *ptrMultiItemInfo);
		~DMeasureHR();

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

		int m_step;
		int m_method;
		bool m_isDraw;
		POINT m_tempP;
		POINT m_p1;
		double m_time;
		double m_hr;
		int m_item;
		const MultiItemInfo *m_itemInfo;
};
#endif

#if 0
class DMeasureHR :public AbsMeasure
{
	public:
		DMeasureHR(const SingleItemInfo *ptrSingleItemInfo);
		~DMeasureHR();

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
#endif

/*
 * @brief measure accel and slope in doppler mode
 */
class DMeasureAccel :public AbsMeasure
{
	public:
		DMeasureAccel(const SingleItemInfo *ptrSingleItemInfo);
		~DMeasureAccel();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
		void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
		UpdateMeasure m_update;
		MeasureMan* m_ptrMan;

		int m_step;
		bool m_isDraw;
		POINT m_tempP;
		POINT m_p1;
		double m_result;
		int m_item;
		const SingleItemInfo *m_itemInfo;
};


/*
 * @brief measure P1/2t in doppler mode
 */
class DMeasureP12t :public AbsMeasure
{
	public:
		DMeasureP12t(const MultiItemInfo *ptrMultiItemInfo);
		~DMeasureP12t();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
		void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
		UpdateMeasure m_update;
		MeasureMan* m_ptrMan;

		int m_step;
		bool m_isDraw;
		POINT m_tempP;
		POINT m_p1;
		double m_Vmax;
		double m_slope;
		double m_p12t;
		int m_item;
		const MultiItemInfo *m_itemInfo;
		
    void Result(int saveR, UpdateMeasure::ResultAttr& attr);
};


/*
 * @brief measure PI(pulse index) in doppler mode
 * >method = 0: PI测量
 * >method = 1: PG mean测量
 */
class DMeasurePI :public AbsMeasure//应该再加入鼠标反向运动时自动往前擦除轨迹线
{
	public:
		DMeasurePI(int method,const SingleItemInfo *ptrSingleItemInfo);
		~DMeasurePI();

		void Init();
		void PressLeft(POINT p); ///< do
		void PressRight(POINT p); ///< undo
		void MouseMove(POINT p);
        void Value(EKnobOper opr);
        void Esc();

	private:
		MeasureCalc m_calc;
		MeasureDraw m_draw;
        UpdateMeasure m_update;
        MeasureMan* m_ptrMan;

        int m_item;
        const SingleItemInfo *m_itemInfo;

		int m_method;
		int m_step;
		bool m_isDraw;
		POINT m_tempP;
		vector<POINT> m_vec;
        vector<POINT> m_trackTemp;//临时存储擦除了的轨迹,用于重绘  
        POINT m_psP;
		POINT m_edP;
		double m_ps;
		double m_ed;
		double m_tamax;
		double m_pi;
		double m_pgmean;
};

/*
 * @brief measure PGmean(mean pressure gradient) in doppler mode
 */
// class DMeasurePGmean :public AbsMeasure
// {
// 	public:
// 		DMeasurePGmean();
// 		~DMeasurePGmean();

// 		void Init();
// 		void PressLeft(POINT p); ///< do
// 		void PressRight(POINT p); ///< undo
// 		void MouseMove(POINT p);
// 		void Esc();

// 	private:
// 		MeasureCalc m_calc;
// 		MeasureDraw m_draw;
// 		UpdateMeasure m_update;
// 		MeasureMan* m_ptrMan;

// 		int m_step;
// 		bool m_isDraw;
// 		POINT m_tempP;
// 		vector<POINT> m_vec;
// 		double m_pg;
// };

#endif
