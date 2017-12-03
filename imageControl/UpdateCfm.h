// -*- c++ -*-
#ifndef UPDATE_CFM_H
#define UPDATE_CFM_H

#include "AbsUpdateCfm.h"
#include "Def.h"
#include "MenuArea.h"

class ImageAreaDraw;
class ImageAreaPara;

class UpdateCfm: public AbsUpdateCfm
{
public:
    UpdateCfm();
    ~UpdateCfm() {}

	void SetBoxStatus(bool changeSize); 
    void Box(int lineBegin, int lineEnd, int dotBegin, int dotEnd);
    void BoxClear();
    void BoxReDraw();
    void GainCfm(int gain);
    void PRF(int data, EKnobReturn status);
    void PRFRange(int data, EKnobReturn status);
    void Baseline(int index, EKnobReturn status);
    void WallFilter(int index, EKnobReturn status);
    void LineDensity(int index, EKnobReturn status);
	void PacketSize(int index, EKnobReturn status);
    void ColorMap(int index, EKnobReturn status);
    void Turbo(int index, EKnobReturn status);
    void ScanLines();
    void SetVelRange(double max, double min, double maxKHz, double minKHz);
	void ColorInvert(bool data, EKnobReturn status);
	void FlowOpt(int index, EKnobReturn status);
	void FocPosRatio(int index, EKnobReturn status);

	///////////////////////  test  ///////////////////////
	void TissueGate(int index, EKnobReturn status);
	void NoiseGate(int index, EKnobReturn status);
	void SpeedGate(int index, EKnobReturn status);
	void VarianceGate(int index, EKnobReturn status);
	void GainGate(int index, EKnobReturn status);
	//////////////////////////////////////////////////////

    void EnterCfm(void);
    void EnterCfmMenu(void);
    void EnterCfmKnob(void);
    void EnterCfmImg(bool update = false);
    void EnterPdi(void);
	void EnterCfmVs2D(void);
    void ChangeFormatCfm(int format);
    void ChangeCurrentImgBB(int current); 
	
    /*
     * @brief change current image in 4B format
     *
     * @para current B
     *  0-lu, 1-ru, 2-ld, 3-rd in 4B('l'=left, 'r'=right, 'u'=up, 'd'=down)
     */
    void ChangeCurrentImg4B(int current); 
    void ChangeCurrentImg4BForFreeze(int current); 

	void ColorTable(unsigned char table[][4], int len, int width, int bitDepth);

	// post process
	void BackgroundHide(bool on);

	void BCWidth(bool on);

	void CfmCompare(bool on);

	void PdiDirStatus(bool on);

	void Threshold(int index, EKnobReturn status);

	void Persist(int index, EKnobReturn status);

	void Reject(int index, EKnobReturn status);

	void Smooth(int index, EKnobReturn status);

	void Artifact(int index, EKnobReturn status);

private:
    ImageAreaPara *m_ptrImgPara;
    ImageAreaDraw *m_ptrImgDraw;
    MenuArea* m_ptrMenu;
    char m_str[20];
};
#endif
