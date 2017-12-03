// -*- c++ -*-
#ifndef _BIOPSY_LINE_H_
#define _BIOPSY_LINE_H_

#include "display/ImageAreaDraw.h"
#include "Def.h"

class BiopsyLine {
public:
    ~BiopsyLine();
    static BiopsyLine* GetInstance();

    bool Create(void);//
    void WritePara(void);
    void Draw(void);//
    void Clear(void);//
    void Redraw(void);
    void SetDotSize(int size);
    void SetDefault(void);
    void AngleChange(EKnobOper oper);//
   // void PositionChange(EKnobOper oper);
	void PositionChange(int offsetX);//
    bool InitPara(void);//
    void GetPara(unsigned int &biop_x, unsigned int &biop_y);

	void AngleSwitch(void);//2016.08.08
    void ClearSingleLine(void);
    void ClearDoubleLine(void);
    void BiopsyEnhance();

private:
    BiopsyLine(void);
    static BiopsyLine* m_ptrInstance;

   // bool InitPara(void);
    void InitBiopsy(void);

    char m_probeType;
    int m_probeRadius;

   /* unsigned int m_biopX;
    unsigned int m_biopY;*/
	//-----------------------------------------------------//
    /*int m_biopX;
    int m_biopY;*/
    double  m_biopX;
    double  m_biopY;
	//-------------------------------------------------------//
	//------2016.10.31-----------------------------------------------------//
	/*int m_biopX1;
	int m_biopY1;
	int m_biopX2;
	int m_biopY2;*/
	double  m_biopX1;
	double  m_biopY1;
	double  m_biopX2;
	double  m_biopY2;
    double  m_biopAngle1;
    double  m_biopAngle2;
	//---------------------------------------------------------//
    unsigned int m_size;
    ImageAreaDraw *m_ptrImgDraw;

    int m_baseVal_x;
    int m_baseVal_y;
    int m_adjustRange_x;
    int m_adjustRange_y;

    //float Angle;
    double  Angle;

    int m_adjustAngle;
    int m_baseAngle;

};
#endif