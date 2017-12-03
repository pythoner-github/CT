#ifndef REPLAY_H
#define REPLAY_H

#include <deque>
#include <gtk/gtk.h>
#include "Image.h"
#include "AbsUpdateMix.h"
#include "AbsUpdate2D.h"
#include "Def.h"
#include "DscMan.h"

using std::deque;

gboolean ReplayCallBack(gpointer data);

class Replay
{
	public:
		~Replay();

		static Replay* GetInstance();

		void Init();
		void SetReplayFps(int fps);
		//void ChangeReplayStatus();
        EKnobReturn ChangeSpeed(EKnobOper oper);
#if (defined(EMP_460) || defined(EMP_355))
		void SetTrimLeft(EKnobOper oper);
		void SetTrimRight(EKnobOper oper);
		void ResetTrim(EKnobOper oper);
#else
		void SetTrimLeft();
		void SetTrimRight();
		void ResetTrim();
#endif
		bool AutoReplayStartView();
		bool AutoReplayStartReview();
		void AutoReplayEnd();

		bool SetAreaNum(int num);
        bool SwitchArea(int current, bool unfreeze);
		void AddOneImg(Image img);

        void ClearCurReplayData();
		void PrepareForEndReplay();
		void PrepareForReplay();
		void DisplayReplayBar();

		unsigned int* ReviewOneImg();

        bool ReviewNoImgBB(int curB);
        bool ReviewNoImgB4(int curB);
        unsigned int* ViewOneImg();
		unsigned int* ReviewOneImgOnly();
		unsigned int* ViewOneImgOnly();
		unsigned int* GetCurrentImg();

        unsigned int* GetSelectImgForIMT();
        unsigned int* GetSelectNextImgForIMT();

        unsigned int* GetLastImgForIMT();
        unsigned int* GetNextImgForIMT();
        bool GetCineStatusForIMT(void) { return m_enterCine;}
        bool GetSnapStatusForIMT(void) { return m_enterSnap;}
        int GetImgFps(void) {return m_fps;}

		bool GetCineImg(deque<Image>** ptrDeq, int &cineBegin, int &cineCount, int &cineWidth, int &cineHeight, int &fps);

		void EnterViewStatus();
		void EnterReviewStatus();

		// cine read
		void AddOneCineImg(Image img);
		void EnterReadCine();
		void ExitReadCine();
		void SetUpdateFrameFunc(UPDATEFRAME drawImg);
		void SetDefaultFrameFunc(void);

		// snap read
		void EnterReadSnap();
		void ExitReadSnap();
		bool GetReadSnapStatus();

        // pw trace
        const Image::AutoTracePara* GetCurrentTraceData(void);
        const std::vector<POINT> GetInitTraceData();
        const std::vector<POINT> GetTraceData(int x1, int x2);

        void UpdateReplayCtrl(bool on, EKnobReturn ret);

		//EFOV review
		void EnterEFOVReview(void);
		void ExitEFOVReview(void);
		EKnobReturn ChangeEFOVReviewDir(EKnobOper oper);
		void GetEFOVCineImg(unsigned char **data, int &size);
		void SetEFOVCineImg(unsigned char *data, int size);
		void EFOVAutoReviewStart();
		void EFOVAutoReviewEnd();
		void EFOVAutoReview();
		void ChangeEFOVAutoReviewFlag();
		bool IsEFOVAutoReplay(void) { return m_efovAutoReviewFlag; }
		bool EFOVVideoOutOfRange(void);
	private:
		Replay();

		static Replay* m_ptrInstance;
		static const int TOTAL_REPLAY = 1024;//500;
		static const int MAX_AREA = 4;
#if (defined(EMP_460) || defined(EMP_355))
		static const int MAX_SPEED = 4;
#else
		static const int MAX_SPEED = 3;
#endif
		static const float REPLAY_SPEED[MAX_SPEED]; //multi sum

		AbsUpdateMix* m_ptrUpdate;
		AbsUpdate2D* m_ptrUpdate2D;
		UPDATEFRAME m_ptrUpdateFrameFreeze;
		DscMan* m_ptrDsc;

		deque<Image> m_deq[MAX_AREA]; ///< vector to save image  of each area
		int m_maxCount[MAX_AREA]; ///< max number of images can be saved in each area, start from 1
		int m_imgIndex[MAX_AREA]; ///< current image index of each area, start from 0
		int m_areaIndex; ///< current area to save image, start from 0
		int m_areaNum; ///< number of area, start from 1

		int m_speedIndex; ///< index speed of replay, range(0-2)
		int m_fps; ///< frame per second when real-time display, unit frame/s
		int m_trimLeft;
		int m_trimRight;
		guint m_tagTimer;
		bool m_viewMode;

		int m_maxCountBak[MAX_AREA];
		int m_areaIndexBak;
		int m_areaNumBak;
		int m_fpsBak;
		bool m_enterCine;
		bool m_enterSnap;

        bool m_discard;

		void ClearAll();

		// read snap
		void BakImgForSnap();
		void RestoreImgForSnap();
		int GetReplayTime(int speedIndex); //unit: ms

		int m_efovFrameNo;
		bool m_efovAutoReviewFlag;
		guint m_tagTimerEFOV;
};

inline void Replay::SetReplayFps(int fps)
{
	m_fps = fps;
    if (m_fps <= 0)
        m_fps = 60;
}

#endif