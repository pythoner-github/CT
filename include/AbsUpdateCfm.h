#ifndef ABS_UPDATE_IMG_CFM_H
#define ABS_UPDATE_IMG_CFM_H

#include "Def.h"

class AbsUpdateCfm
{
	public:
		virtual ~AbsUpdateCfm() {}

		virtual void SetBoxStatus(bool changeSize) = 0; ///< TRUE, change box size; FALSE, change box position
		virtual void Box(int lineBegin, int lineEnd, int dotBegin, int dotEnd) = 0;
		virtual void BoxClear() = 0;
		virtual void BoxReDraw() = 0;
		virtual void GainCfm(int gain) = 0;
		virtual void PRF(int data, EKnobReturn status) = 0;
        virtual void PRFRange(int data, EKnobReturn status) = 0;
		virtual void Baseline(int index, EKnobReturn status) = 0;
		virtual void WallFilter(int index, EKnobReturn status) = 0;
		virtual void LineDensity(int index, EKnobReturn status) = 0;
		virtual void PacketSize(int index, EKnobReturn status) = 0;
		virtual void ColorMap(int index, EKnobReturn status)= 0;
		virtual void Turbo(int index, EKnobReturn status) = 0;
		virtual void SetVelRange(double max, double min, double maxKHz, double minKHz) = 0;
		virtual void ColorInvert(bool data, EKnobReturn status) = 0;
		virtual void FlowOpt(int index, EKnobReturn status) = 0;
        virtual void FocPosRatio(int index, EKnobReturn status) = 0;

		virtual void EnterCfm(void) = 0;
		virtual void EnterCfmMenu(void) = 0;
		virtual void EnterCfmKnob(void) = 0;
		virtual void EnterCfmImg(bool update = false) = 0;

		virtual void EnterPdi() = 0;
		virtual void EnterCfmVs2D(void) = 0;

		virtual void ChangeFormatCfm(int format) = 0;
		virtual void ChangeCurrentImgBB(int current) = 0;

		/*
		 * @brief change current image in 4B format
		 *
		 * @para current B
		 *  0-lu, 1-ru, 2-ld, 3-rd in 4B('l'=left, 'r'=right, 'u'=up, 'd'=down)
		 */
		virtual void ChangeCurrentImg4B(int current) = 0;
		virtual void ChangeCurrentImg4BForFreeze(int current) = 0;

		virtual void ColorTable(unsigned char table[][4], int len, int width, int bitDepth) = 0;

		// post process
		virtual void BackgroundHide(bool on) = 0;

		virtual void BCWidth(bool on) = 0;

		virtual void CfmCompare(bool on) = 0;

		virtual void PdiDirStatus(bool on) = 0;

		virtual void Threshold(int index, EKnobReturn status) = 0;

		virtual void Persist(int index, EKnobReturn status) = 0;

		virtual void Reject(int index, EKnobReturn status) = 0;

		virtual void Smooth(int index, EKnobReturn status) = 0;

		virtual void Artifact(int index, EKnobReturn status) = 0;

		///////////////////////  test  ///////////////////////

		virtual void TissueGate(int index, EKnobReturn status) = 0;

		virtual void NoiseGate(int index, EKnobReturn status) = 0;

		virtual void SpeedGate(int index, EKnobReturn status) = 0;

		virtual void VarianceGate(int index, EKnobReturn status) = 0;

		virtual void GainGate(int index, EKnobReturn status) = 0;

        virtual void ScanLines() = 0;

		//////////////////////////////////////////////////////
};
#endif