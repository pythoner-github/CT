// -*- c++ -*-
#ifndef UPDATE_PW_H
#define UPDATE_PW_H

#include "Def.h"
#include "AbsUpdatePw.h"
#include "ImageAreaPara.h"
#include "ImageAreaDraw.h"
#include "MenuArea.h"
#include <vector>

using std::vector;
/*
 * @brief abstract class, update view realative to imging pw para, this class should be realize by GUI.
 */
class UpdatePw: public AbsUpdatePw
{
public:
    UpdatePw();
    ~UpdatePw() {}

    /*
     * @brief update Pw cursor and sample volume(SV) when it changed
     *
     * @para line PW line
     * @para dotBegin begin of SV in dots
     * @para dotEnd end of SV in dots
     */
    void SampleVolume(int line, int dotBegin, int dotEnd, vector<int> HPRFEmitPos);
    void SampleVolumeClear();
    
    void SampleVolumeCW(int line, int dotPos);
    void SampleVolumeCWClear();

	void SpectrumInvert(bool on);
    /*
	 * @brief update Pw total gain when it changed
	 * @para gain value of pw gain 
     */
    void GainPw(int gain);
   
    void GainCw(int gain);

    void DopFreq(int freq, EKnobReturn status);
    void  SVLength(int len, EKnobReturn status);
    void  SVPos(double depth);
    /*
	 * @brief update when correct angle is changed
	 *
	 * @para angle real angle, when angle is larger than 60 or less than -60, value display changed color to red
	 */
	void SetValueCorrectAngle(int angle);
	void CorrectAngle(int angle, EKnobReturn status);
	void PRF(int data, EKnobReturn status);
	void PRFMax(int data, EKnobReturn status);
    void PRFRange(int data, EKnobReturn status);
	void Baseline(int index, int maxIndex, EKnobReturn status);
	void Steer(int angle, EKnobReturn status);
	void WallFilter(int index, EKnobReturn status);
	void DynamicRange(int index, EKnobReturn status);
	void Speed(int index, EKnobReturn status);
	void SoundVolume(int index, EKnobReturn status);
	void Simult(bool on, EKnobReturn status);
	void HPRF(bool on, EKnobReturn status);
	void GrayMap(int index, EKnobReturn status);
	void SoundStatus(int index, EKnobReturn status);
	void Noise(int data, EKnobReturn status);
	void Log(int value, EKnobReturn status);
    void AutoCalc(bool on);
    void AutoStatus(bool on);

	void DisablePRF();
	// for test
    void SoundFilter(int data, EKnobReturn status);

    void EnterPwMode();
    void EnterPwImg();
    void UpdatePwMode(void);
    void UpdateCwMode(void);
    void UpdateCwMenu(void);
    void UpdatePwMenu(void);
    void UpdatePwImg();
    void EnterPwCfmFromPw();
    void EnterCwCfmFromCfm();
    void EnterPwCfmFromCfm();
    void UpdateCwCfmMode(void);
    void UpdateCwCfmMenu();
    void UpdatePwCfmMode(void);
    void UpdatePwCfmMenu(void);
    void UpdatePwCfmImg(void);
    void EnterPwPdiFromPw();
    void EnterCwPdiFromPdi();
    void UpdateCwPdiMode();
    void EnterPwPdiFromPdi();
    void UpdatePwPdiMode();

    void ChangeFormatPw(int format);

	// pw post process
	void TimeRes(int index, EKnobReturn status);

	void TimeMarkPw(bool on);

private:
    ImageAreaPara *m_ptrImgPara;
    ImageAreaDraw *m_ptrImgDraw;
    MenuArea* m_ptrMenu;
    char m_str[20];
    bool m_timeMark;
};

#endif
