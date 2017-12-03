#ifndef ABS_UPDATE_IMG_2D_H
#define ABS_UPDATE_IMG_2D_H

#include "Def.h"

/*
 * @brief abstract class, update view realative to imging 2d para, this class should be realize by GUI.
 */
class AbsUpdate2D
{
	public:
	virtual ~AbsUpdate2D() {}

	virtual void ProbeType(const char* type) = 0;

    virtual void UserDefineExamItem(const char* useritem) = 0;
 #ifdef VET
	virtual void ExamItemByProbe(const char *type) = 0;
#endif

    virtual void ExamItem(int index) = 0;

	/*
	 * @brief update view when freq is changed 
	 *
	 * @para freq freq = realFreq * 20, unit:Mhz
	 */
	virtual void Freq(int freq, EKnobReturn status) = 0;

	/*
	 * @brief update view when depth is changed
	 *
	 * @para depth realDepth, unit:mm
	 */
	virtual void Depth(int depth) = 0;

	/*
	 * @brief update view when focus sum and focus position is changed
	 *
	 * @para focSum focus sum
	 * @para focPos[] focus position, focPos[0] is the first focus position, unit:mm
	 */
	virtual void FocInfo(int focSum, int focPos[]) = 0;

	/*
	 * @brief update tgc curve when tgc is changed
	 *
	 * @para tgc pointer pointed to real tgc data
	 * @para tgcSize size of tgc data
	 */
	virtual void TgcCurve(unsigned char* tgc, int tgcSize) = 0;
	
	/*
	 * @brief update 2d and m total gain when total gain is changed
	 */
    virtual void Gain2D(int total) = 0;
    
	virtual void GainM(int total) = 0;

#ifdef TRANSDUCER
     virtual void Transducer(int transducer)=0;
#endif

	/*
	 * @brief update scan angle
	 *
	 * @para angleIndex level of angle
	 */
	virtual void ScanAngle(int angleIndex, EKnobReturn status) = 0;

	/*
	 * @brief update view when dynamic range is changed
	 *
	 * @para range dynamic range, unit: dB
	 */
	virtual void DynamicRange(int range, EKnobReturn status) = 0;

	/*
	 * @brief update view when line density is changed
	 *
	 * @para densityIndex level of density
	 *  0-low density
	 *  1-high density
	 */
	virtual void LineDensity(const char* density, EKnobReturn status) = 0;


	/*
	 * @brief update view when sound power if changed
	 *
	 * @power (percent of power), display power% 
	 */
	virtual void SoundPower(int power, EKnobReturn status) = 0;

	/*
	 * @brief update view when harmonic status is changed
	 *
	 * @para on status of harmonic, 
	 *  on=TURE harmonic is on
	 *  on=FALSE harmonic is off
	 */
    virtual void Harmonic(bool on, EKnobReturn status) = 0;
    
    virtual void  HarmonicStatus(bool on, EKnobReturn status) = 0;
  
    virtual void  HarmonicFreq(int harFreq, EKnobReturn status) = 0;
	
    virtual void MBP(int mbp, EKnobReturn status) = 0;

	virtual void Tsi(const char *data, EKnobReturn status) = 0;
	 
	virtual void Agc(int index, EKnobReturn status) = 0;
	/*
	 * @brief update view when edge enhancement is changed
	 *
	 * @para edgeIndex level of edge enhancement
	 */
	virtual void EdgeEnhance(int edgeIndex, EKnobReturn status) = 0;

	virtual void Steer(int angle, EKnobReturn status) = 0;
	/*
	 * @brief update view when position of M line is changed
	 *
	 * @para line current m line
	 */
	virtual void MLine(int line) = 0;
	virtual void ClearMLine() = 0;
	virtual void ReDrawMLine() = 0;

	/*
	 * @brief update view when M speed is changed
	 *
	 * @para speedIndex level of M speed
	 */
	virtual void MSpeed(int speedIndex, EKnobReturn status) = 0;

	/*
	 * @brief set probe para when probe is changed
	 *
	 * @para type probe type, 'C'-convex, 'T'-TV, 'L'-Linear, 'P'-phase
	 * @para line scan lines number of probe
	 * @para width (width of linear or angle of convex/TV) * 100
	 * @para r radius of probe of convex/TV
	 */
	virtual void ProbePara(int type, int lines, int width, int r) = 0;

	/*
	 * @brief update view when enter mode 2D
	 */
	virtual void Enter2DMode() = 0; ///<enter 2d mode
	virtual void Enter2DMenu(void) = 0;
	virtual void Enter2DKnob(void) = 0;
	virtual void Enter2DImg(bool update = false) = 0;

    virtual void UpdateEFOVZoomScale(void) = 0;
	virtual void EFOVZoom(float scale, EKnobReturn status) = 0; 
	virtual void EFOVRotate(int angle, EKnobReturn status) = 0;
    virtual void EnterEFOVPrepare(void) = 0;
    virtual void EnterEFOVCapture(void) = 0;
    virtual void EnterEFOVView(void) = 0;
    virtual void EnterEFOVReview(void) = 0;
	virtual void EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y) = 0;
	virtual void ClearEFOVZoomMiniatureBox(void) = 0;
	virtual void EFOVZoomBox(int start_x, int start_y, int end_x, int end_y) = 0;
	virtual void ClearEFOVZoomBox(void) = 0;
	virtual void EFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right) = 0;
	virtual void ClearEFOVViewBox(void) = 0;
	virtual void EFOVReplay(bool on, EKnobReturn status) = 0; 
        virtual void ClearEFOVImg(void) = 0;
	/*
	 * @brief update view when enter M mode 
	 */
	virtual void EnterMMode() = 0;
	virtual void EnterMImg() = 0;

	/*
	 * @brief update view when update M mode
	 */
	virtual void UpdateM() = 0; ///< enter M mode, M image
	virtual void UpdateMMenu(void) = 0;
	virtual void UpdateMKnob(void) = 0;
	virtual void UpdateMImg(void) = 0;
	virtual void EnterAnatomicM() = 0;

	/*
	 * @brief change format of 2D
	 *
	 * @para 0-B, 1-BB, 2-4B
	 */ 
    virtual void ChangeFormat2D(int format) = 0; 

	/*
	 * @brief change current image in BB format
	 *
	 * @para current B
	 *  0-left, 1-right in BB
	 */
	virtual void ChangeCurrentImgBB(int current) = 0; 
	
	/*
	 * @brief change current image in 4B format
	 *
	 * @para current B
	 *  0-lu, 1-ru, 2-ld, 3-rd in 4B('l'=left, 'r'=right, 'u'=up, 'd'=down)
	 */
	virtual void ChangeCurrentImg4B(int current) = 0; 

	/*
	 * @brief change display format of M to "format"
	 *
	 * @para format 0-total M, 
	 *		1-up B dowm M 1:1, 
	 *		2-up B right M 2:1, 
	 *		3-up B down M 1:2, 
	 *		4-left B right M 1:1,  
	 */
	virtual void ChangeFormatM(int format) = 0; 

	virtual void LeftRight(bool on) = 0;
	
	virtual void UpDown(bool on) = 0;

	//virtual void Polarity(bool on) = 0;
    virtual void Polarity(bool on, EKnobReturn status) = 0;
	
	/*
	 * @brief change noise reject status
	 *
	 * @para status 0-OFF 1-ON
	 */
	virtual void NoiseReject(bool on) = 0;

	virtual void Rotate(int data, EKnobReturn status) = 0;

	virtual void FrameAver(int data, EKnobReturn status) = 0;

	virtual void LineAver(int data, EKnobReturn status) = 0;

	virtual void Smooth(int data, EKnobReturn status) = 0;
	
	virtual void Gamma(int data, EKnobReturn status) = 0;

	virtual void Chroma(const char* data, EKnobReturn status) = 0;
	
	virtual void ImgEhn(int data, EKnobReturn status) = 0;

    virtual void CompoundSpace(int angle, EKnobReturn status) = 0;

    virtual void CompoundFreq(bool on, EKnobReturn status) = 0;

    virtual void TpView(int index, EKnobReturn status) = 0;

    virtual void EFVI(bool on, EKnobReturn status) = 0;


	// M
	virtual void TimeMarkM(bool on) = 0;
	
	virtual void EnhanceM(int data, EKnobReturn status) = 0;
	
	virtual void SmoothM(int data, EKnobReturn status) = 0;
	
	virtual void TIS(float data) = 0;

	//zoom
	virtual void ZoomScale(double scale) = 0;

    virtual void GlobalZoomScaleY(void) = 0;

	virtual void MoveFlag(POINT p) = 0;

	virtual	void ClearMoveFlag() = 0;

	virtual void PIPMagnifier(POINT p) = 0;

	virtual void ClearPIPMagnifier() = 0;

	virtual void PIPBox(POINT p1, POINT p2) = 0;
	
	virtual void ClearPIPBox() = 0;

	virtual void EnterLocalZoom(void) = 0;

	virtual void ExitLocalZoom(void) = 0;

	virtual void SetLocalZoomBoxStatus(bool changeSize) = 0;

	virtual void LocalZoomBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd) = 0;

	virtual void ClearLocalZoomBox() = 0;

	virtual void GrayReject(int rejecPoint) = 0;

	/*
	 * @brief display gray trans curve and adjustable point
	 *
	 * @para colorTable: pointer pointed to curve data
	 * @para size: size of curve data 
	 * @para curPointX: current adjustable point
	 * @para curPointY: value of current adjustable point
	 * @para maxPoint: max sum of adjustable points
	 */
	virtual void GrayTrans(unsigned char* colorTable, int size, int curPointY, unsigned char* basePoint, int maxPoint) = 0;

	virtual void ClearGrayCurve() = 0;

	virtual void BWColorTable(RGBTRIPLE *table, int size) = 0;

    virtual void AutoOptimize(bool on) = 0;    

    virtual void ScanLines(int scanLinesIndex, EKnobReturn status) = 0;
};
#endif

