#ifndef ZOOM_H
#define ZOOM_H

#include <DSCTypes.h>
#include <DSC.h>
#include "AbsUpdate2D.h"
#include "imageProc/DscMan.h"

class Zoom
{
	public:
		~Zoom();
		static Zoom* GetInstance();

		void SetDsc(CDSC* ptrDsc) { m_ptrDsc = ptrDsc;}

		// global zoom
		void GlobalZoomCtrl(bool on);
		bool GlobalZoomScale(EKnobOper oper);
		bool GlobalZoomScroll(int offsetX, int offsetY);
        void GetGlobalZoomInfo(double &scale, int &offsetInOriginal);

		// PIP
		void PIPZoomCtrl(bool on);
		bool PIPZoomScale(EKnobOper oper);
		bool PIPZoomPos(int offsetX, int offsetY);

		// local zoom
		void LocalZoomInit();
		void LocalZoomCtrl(bool on);
		bool LocalZoomBoxPos(int offsetX, int offsetY);
		bool LocalZoomBoxSize(int offsetX, int offsetY, bool lrOverturn, bool udOverturn);
        void UpdateLocalZoom(double scale);
        bool ReSendLocalZoomInfo(void);
        bool GetLocalZoomStatus(void);
        void GetScanLineRange(int lineRange[2]);
        void GetScanDotRange(int dotRange[2]); ///<相对于未放大前的图像位置
        int GetScanDotBeginDepth(void);
        void GetLocalZoomScale(double &scale); ///<局部放大本身的倍率。
		void ClearLocalZoom();
		void RedrawLocalZoom();
		void SetLocalZoomBoxStatus(bool changeSize);

	private:
		Zoom();
		static Zoom* m_ptrInstance;

		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		AbsUpdate2D* m_ptrUpdate;
		DscMan* m_ptrDscMan;
		CDSC* m_ptrDsc;

		static const int MAX_GLOBAL_SCALE = 8;
		static const double GLOBAL_SCALE[MAX_GLOBAL_SCALE];

		static const int MAX_PIP_SCALE = 8;
		static const double PIP_SCALE[MAX_PIP_SCALE];

        static const int INTERLINE = 16;//60;

		bool m_globalCtrl;
		int m_globalScale;
		POINT m_globalPos;
		int m_globalOffsetX;
		int m_globalOffsetY;

		bool m_PIPCtrl;
		int m_PIPScale;
		POINT m_PIPPos;

		bool m_localCtrl;
		int m_localLineBegin;
		int m_localLineEnd;
		int m_localDotBegin;
		int m_localDotEnd;
        double m_localScale;
        guint m_timeout;

		void SetGlobalInfo(POINT p, int scaleIndex);
		void SetGlobalPos(POINT p);

        void SetPIPInfo(POINT p, int scaleIndex);
        void SetPIPPos(POINT p);

		void SetLocalInfo(int lineBegin, int lineEnd, int dotBegin, int dotEnd);
        static gboolean HandleSetPos(gpointer data);
};
#endif