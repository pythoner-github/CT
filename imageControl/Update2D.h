// -*- c++ -*-
#ifndef UPDATE_2D_H
#define UPDATE_2D_H

#include "../include/AbsUpdate2D.h"
#include "Def.h"
#include "ImageAreaPara.h"
#include "../display/ImageAreaDraw.h"
#include "MenuArea.h"
#include <vector>

using std::vector;

class Update2D: public AbsUpdate2D
{
public:
    Update2D();

	void ProbeType(const char* type);
#ifdef VET
	void ExamItemByProbe(const char *type);
#endif
    void UserDefineExamItem(const char* useritem);

    void ExamItem(int index);

    void Freq(int freq, EKnobReturn status);

    void Depth(int depth);

    void FocInfo(int focSum, int focPos[]);

    void TgcCurve(unsigned char* tgc, int tgcSize);

    void Gain2D(int total);

#ifdef TRANSDUCER  
    void Transducer(int transducer); 
#endif

	void GainM(int total);

    void ScanAngle(int angleIndex, EKnobReturn status);

    void ScanLines(int scanLinesIndex, EKnobReturn status);

    void DynamicRange(int range, EKnobReturn status);

    void LineDensity(const char* density, EKnobReturn status);

    void SoundPower(int power, EKnobReturn status);

    void Harmonic(bool on, EKnobReturn status);

    void  HarmonicStatus(bool on, EKnobReturn status);

    void HarmonicFreq(int harFreq, EKnobReturn status);

	void MBP(int mbp, EKnobReturn status);

	void Tsi(const char* data, EKnobReturn status);

	void Agc(int index, EKnobReturn status);
    
	void EdgeEnhance(int edgeIndex, EKnobReturn status);

	void Steer(int angle, EKnobReturn status);

    void MLine(int line);

	void ClearMLine();

	void ReDrawMLine();

    void MSpeed(int speedIndex, EKnobReturn status);

    void ProbePara(int type, int lines, int width, int r);

    void Enter2DMode(void); ///<enter 2d mode
    void Enter2DMenu(void);
    void Enter2DKnob(void);
    void Enter2DImg(bool update = false);
// EFOV
    void UpdateEFOVZoomScale(void);
	void EFOVZoom(float scale, EKnobReturn status); 
	void EFOVRotate(int angle, EKnobReturn status);
    void EnterEFOVPrepare(void);
    void EnterEFOVCapture(void);
    void EnterEFOVView(void);
    void EnterEFOVReview(void);
	void EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y);
	void ClearEFOVZoomMiniatureBox(void);
	void EFOVZoomBox(int start_x, int start_y, int end_x, int end_y);
	void ClearEFOVZoomBox(void);
	void EFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right);
	void ClearEFOVViewBox(void);
	void EFOVReplay(bool on, EKnobReturn status); 
        void ClearEFOVImg();

    void EnterMMode(void); ///< enter M mode
	void EnterMImg();

    void UpdateM(void); ///< update M mode
    void UpdateMMenu(void);
    void UpdateMKnob(void);
    void UpdateMImg(void);
	void EnterAnatomicM();

    void ChangeFormat2D(int format);

    void ChangeCurrentImgBB(int current);
	
    void ChangeCurrentImg4B(int current);

    void ChangeFormatM(int format);

	void LeftRight(bool on);
	
	void UpDown(bool on);

	//void Polarity(bool on);
    void Polarity(bool on, EKnobReturn status);

	void NoiseReject(bool on);

	void Rotate(int data, EKnobReturn status);

	void FrameAver(int data, EKnobReturn status);

	void LineAver(int data, EKnobReturn status);

	void Smooth(int data, EKnobReturn status);

	void Gamma(int data, EKnobReturn status);

	void Chroma(const char* data, EKnobReturn status);

	void ImgEhn(int data, EKnobReturn status);
	
    void CompoundSpace(int angle, EKnobReturn status);

	void CompoundFreq(bool on, EKnobReturn status);

    void TpView(int index, EKnobReturn status);
 
    void EFVI(bool on, EKnobReturn status); 
    
    // M
    void TimeMarkM(bool on);
	
	void EnhanceM(int data, EKnobReturn status);
	
	void SmoothM(int data, EKnobReturn status);

	void TIS(float data);

	// Zoom
	void ZoomScale(double scale);

    void GlobalZoomScaleY(void);

	void MoveFlag(POINT p);

	void ClearMoveFlag();

	void PIPMagnifier(POINT p);
	
	void ClearPIPMagnifier();

	void PIPBox(POINT p1, POINT p2);

	void ClearPIPBox();

	void EnterLocalZoom(void);
	void ExitLocalZoom(void);

	void SetLocalZoomBoxStatus(bool changeSize);
	void LocalZoomBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd);

	void ClearLocalZoomBox();

	void GrayReject(int rejecPoint);

	void GrayTrans(unsigned char* colorTable, int size, int curPointY, unsigned char* basePoint, int maxPoint);

	void ClearGrayCurve();

	void BWColorTable(RGBTRIPLE *table, int size);

    static void SetCineRemoveImg(int count);

    static void CopyImgDataToReplay(void);

    void AutoOptimize(bool on);    

private:
    ImageAreaPara *m_ptrImgPara;
    ImageAreaDraw *m_ptrImgDraw;
    MenuArea* m_ptrMenu;
    char m_str[20];
    bool m_timeMark;
    int m_tgcTimeout;
};

#endif
