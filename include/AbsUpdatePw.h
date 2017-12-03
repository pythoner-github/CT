#ifndef ABS_UPDATE_IMG_PW_H
#define ABS_UPDATE_IMG_PW_H

#include "Def.h"
#include <vector>

/*
 * @brief abstract class, update view realative to imging pw para, this class should be realize by GUI.
 */
class AbsUpdatePw
{
	public:
		virtual ~AbsUpdatePw() {}

		/*
		 * @brief update Pw cursor and sample volume(SV) in image area when it changed
		 *
		 * @para line PW line
		 * @para dotBegin begin of SV in dots
		 * @para dotEnd end of SV in dots
		 */
		virtual void SampleVolume(int line, int dotBegin, int dotEnd, std::vector<int> HPRFEmitPos) = 0;
		virtual void SampleVolumeClear() = 0;
        //virtual void SampleVolumeReDraw() = 0;
		virtual void SampleVolumeCW(int line, int dotPos) = 0;
		virtual void SampleVolumeCWClear() = 0;

		virtual void SpectrumInvert(bool on) = 0;
		/*
		 * @brief update Pw gain when it changed
		 * @para gain gain of pw
		 */
        virtual void GainCw(int gain) = 0;
		virtual void GainPw(int gain) = 0;
		virtual void DopFreq(int freq, EKnobReturn status) = 0;

		/*
		 * @brief update sv length in knob area when it is changed
		 */
		virtual void  SVLength(int len, EKnobReturn status) = 0;
		/*
		 * @depth unit:mm
		 */
		virtual void  SVPos(double depth) = 0;
		/*
		 * @brief update angle in knob area when it changed
		 *
		 * @para angle real angle, when angle is larger than 60 or less than -60, value display changed color to red
		 */
		virtual void SetValueCorrectAngle(int angle) = 0;
		virtual void CorrectAngle(int angle, EKnobReturn status) = 0;
		virtual void PRF(int data, EKnobReturn status) = 0;
        virtual void PRFRange(int data, EKnobReturn status) = 0;
		virtual void Baseline(int index, int maxIndex, EKnobReturn status) = 0;
        virtual void Steer(int angle, EKnobReturn status) = 0;
		virtual void WallFilter(int index, EKnobReturn status) = 0;
		virtual void DynamicRange(int index, EKnobReturn status) = 0;
		virtual void Speed(int index, EKnobReturn status) = 0;
		virtual void SoundVolume(int index, EKnobReturn status) = 0;
		virtual void Simult(bool on, EKnobReturn status) = 0;
		virtual void HPRF(bool on, EKnobReturn status) = 0;
		virtual void GrayMap(int index, EKnobReturn status) = 0;
		virtual void SoundStatus(int index, EKnobReturn status) = 0;
		virtual void Noise(int data, EKnobReturn status) = 0;
		virtual void Log(int value, EKnobReturn status) = 0;
        virtual void AutoCalc(bool on) = 0;
        virtual void AutoStatus(bool on) = 0;

		// for test
		virtual void SoundFilter(int data, EKnobReturn status) = 0;

		virtual void EnterPwMode() = 0;
		virtual void EnterPwImg() = 0;
		virtual void UpdatePwMode() = 0;
		virtual void UpdatePwMenu() = 0;
		virtual void UpdatePwImg() = 0;
		virtual void EnterPwCfmFromPw() = 0;
		virtual void EnterPwCfmFromCfm() = 0;
		virtual void UpdatePwCfmMode(void) = 0;
		virtual void UpdatePwCfmMenu(void) = 0;
		virtual void UpdatePwCfmImg(void) = 0;

		virtual void EnterPwPdiFromPw() = 0;
		virtual void EnterPwPdiFromPdi() = 0;
		virtual void UpdatePwPdiMode() = 0;


        //cw
        virtual void UpdateCwMenu() = 0;
        virtual void UpdateCwCfmMenu(void) = 0;
        virtual void UpdateCwMode() = 0;
        virtual void EnterCwCfmFromCfm() = 0;
        virtual void UpdateCwCfmMode(void) = 0;
        virtual void EnterCwPdiFromPdi() = 0;
        virtual void UpdateCwPdiMode() = 0;

		virtual void ChangeFormatPw(int format) = 0;

		// pw post process
		virtual void TimeRes(int index, EKnobReturn status) = 0;
		virtual void TimeMarkPw(bool on) = 0;
};

#endif
