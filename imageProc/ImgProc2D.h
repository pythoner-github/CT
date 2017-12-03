#ifndef __IMGPROC2D_H__
#define __IMGPROC2D_H__

#include <string>
#include "DSCTypes.h"
#include "DSC.h"
#include "ExamItem.h"
#include "AbsUpdate2D.h"

using std::string;

/**
* @brief processing 2D image (just for the image which processed with DSC )
*/
class ImgProc2D
{
	public:
		///>
		~ImgProc2D();

		static const int MAX_CHROMA = 8;
		static const string CHROMA_TYPE[MAX_CHROMA];
		static const int COLOR_MAP_CHROM[MAX_CHROMA];
		static const int MAX_ROTATE = 4;
		static const int Rotate[MAX_ROTATE]; //0, 90, 180, 270
		static const int MAX_REJECT = 2;
		static const int MAX_FRAME = 8;
		static const int MAX_LINE = 8;
		static const int MAX_SMOOTH = 8;
		static const int MAX_IMG_EHN = 6;
		static const int MAX_EFOV_ROTATE = 360;
		static const int MAX_EFOV_ZOOM_INDEX = 7;
		static const int MAX_EFOV_COLOR = 256;
        static const int MAX_TRANS_CURVE = 7;//6;//  12;
        static const double TRANS_CURVE_RATIO[MAX_TRANS_CURVE];
		enum SIZE{SMALL, MID, BIG};

		static ImgProc2D* GetInstance();
		void InitOptimize(ExamItem::Para2D* d2);
		void Init(ExamItem::Para2D* d2);
		void GetCurPara(ExamItem::ParaItem* ptrParaItem);

		///> change the para for GUI
		void ChangeLR();
		void ChangeUD();
		void ChangePolarity(bool status);
		void ChangeNoiseReject(bool status);

		enum EKnobReturn ChangeFrameAver(enum EKnobOper oper);
		enum EKnobReturn ChangeRotate(enum EKnobOper oper);
		enum EKnobReturn ChangeLineAver(enum EKnobOper oper);
		enum EKnobReturn ChangeSmooth(enum EKnobOper oper);
		enum EKnobReturn ChangeGamma(enum EKnobOper oper);
		enum EKnobReturn ChangeChroma(enum EKnobOper oper);
        enum EKnobReturn PressChroma(void);
		enum EKnobReturn ChangeImgEhn(enum EKnobOper oper);

		bool ChangeGrayTransPointX(enum EKnobOper oper);
		bool ChangeGrayTransPointY(enum EKnobOper oper);
		bool ResetGrayTrans();
		bool ChangeGrayReject(enum EKnobOper oper);
		void DisplayGrayTrans(bool on);
		void DisplayGrayReject(bool on);

		bool GetChromaStatus(); // return TRUE = on
		bool GetLRStatus();
		bool GetUDStatus();
		int GetRotate(void);
		void SetRotate(int data, EKnobReturn ret);
		int GetImgEhnLevel();

		enum EKnobReturn ChangeEFOVRotate(enum EKnobOper oper);
		enum EKnobReturn ChangeEFOVZoomScale(enum EKnobOper oper);
		bool ChangeEFOVZoomPos(enum EKnobOper oper);
		void SetEFOVRotate(int data, EKnobReturn ret);
		void SetEFOVZoom(int scaleIndex, int pos, EKnobReturn ret);
		//void SetImgEhnLevel(int level);
		void SetImgEhnLevel(int level, bool update=true);
		void RecoverImgEhnLevel();
		//void SetFrameAverLevel(int level);
		void SetFrameAverLevel(int level,bool update=true);
		void RecoverFrameAverLevel();
		//void SetLineAverLevel(int level);
		void SetLineAverLevel(int level, bool update=true);
		void RecoverLineAverLevel();
		void GetEFOVSpeed(int &speed_flag, int &speed_maxflag, float &speed);
		EFOVRECT<int> GetEFOVBoxRect();
		void EnterEFOVView();
		void GetEFOVSpeedColormap(RGBTRIPLE table[MAX_EFOV_COLOR]);
		enum EKnobReturn ChangeEFOVROISize(enum EKnobOper oper);
		EKnobReturn GetKnobEFOVROISize(ImgProc2D::SIZE size);
		SIZE GetEFOVROISize(void) { return m_roiEFOVSize; }
		bool GetUpDownStatusForIMT(void) { return m_UD;} //get updown mark for IMT measure
		int GetGrayTranIndex(void) { return m_CurCurveIndex; }
        bool GetEnhanceBiopsy()
        {
        return m_biopsyEnhance;
        }
        void EnhanceBiopsy(bool on);

	private:
		ImgProc2D();

		///> extern member
		static ImgProc2D* m_ptrInstance;

		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		AbsUpdate2D* m_ptrUpdate;

		unsigned char* m_ptrColormap;

		///> static const
		static const int MAX_GAMMA = 8;
		static const int MAX_GRAY = 256;

		static const int FrameAver[MAX_FRAME];
		static const int LineAver[MAX_LINE];
		static const double Gamma[MAX_GAMMA];
		static const int MAX_TRANS_POINT = 9;
		static const float m_zoomEFOVScale[MAX_EFOV_ZOOM_INDEX];
	    bool m_biopsyEnhance;

		bool m_LR;
		bool m_UD;
		bool m_Polarity;
		int m_Rotate; ///< 0~2
		int m_FrameAver;	///< 0~7
		int m_FrameAverBak;	///< 0~7
		int m_LineAver;	///< 0~3
		int m_LineAverBak;	///< 0~3
		int m_Smooth;	///< 0~7
		bool m_NoiseReject;
		int m_RejectPoint;	///< 0~255
		int m_ImgEhn; ///<0-5
		int m_ImgEhnBak; ///<0-5
		int m_rotateEFOV;	///<-360~360
        int m_zoomEFOVIndex;	///<0~5
        unsigned int m_zoomEFOVPos;
        RECT m_zoomEFOVBox;
        SIZE m_roiEFOVSize;

		int m_Chroma;	///< 0~5
		int m_ChromaBak;	///< 0~5

		unsigned char m_ColorTransTable[MAX_GRAY];	///< 0~255
		unsigned char m_ColorTable[MAX_GRAY];	///< 0~255
		unsigned char m_CurveBasePoint[MAX_TRANS_POINT];///< base point, used to adjust type of gray curve
        int m_CurrentPoint; ///< current point to be changed, among m_CurveBasePoint
        int m_CurCurveIndex;
        int m_Gamma;	///< 0~7

		///>
		void CustomGammaChange(unsigned char* BaseValue, unsigned char* CurveValue);
		void ChangeColormap();

		///> set para to DSC of 2D mode
		void SetLR(bool on);
		void SetUD(bool on);
		void SetPolarity(bool on);
		//void SetFrameAver(int data, EKnobReturn ret);
		void SetFrameAver(int data, EKnobReturn ret,bool update=true);
		//void SetLineAver(int data, EKnobReturn ret);
		void SetLineAver(int data, EKnobReturn ret, bool update=true);
		void SetSmooth(int data, EKnobReturn ret);
		void SetNoiseReject(bool on);
		void SetColorTable(RGBTRIPLE table[], int size);
		void InitColorTable();
		//void SetImgEhn(int data, EKnobReturn ret);
		void SetImgEhn(int data, EKnobReturn ret, bool update=true);

        void UpdateDsc(void);
};

#endif //__IMGPROC2D_H__