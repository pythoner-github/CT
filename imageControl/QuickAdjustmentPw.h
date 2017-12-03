/***************************************************************
   * Copyright (C), EMP Tech. Co., Ltd.
   * FileName: QuickAdjustmentPw.h
   * Author: weifang       Version : 1.1        Date:2017.01.11
   * Description:
      * pw一键优化实现
   * Version:  1.1
   * Function List:
      * 1. SetDataPw
      * 2. AnalysisPw
   * History:
     * <author>  <time>   <version >   <desc>
     * 1.   weifang    2014/07/24     0.0     build this moudle
     * 2.   weifang    2014/09/19     1.0.2
     * 2.   weifang    2017/01/11     1.1
*****************************************************************/
#pragma once
class QuickAdjustmentPw
{
	public:
		~QuickAdjustmentPw();
		static QuickAdjustmentPw* GetInstance();

        static bool m_shutInitialPw;
        static bool m_onQAPw;

		void QuickAdjustmentPwOn();
		void QuickAdjustmentPwOff();
        void SendDataToQAPw(unsigned char *pImgData);
        void SetParaChangeMode();
        void SetParaChangeKnob(int pwFlag);

	private:

		QuickAdjustmentPw();

		// extern member
		static QuickAdjustmentPw* m_ptrInstance;

        // quick adjustment pw control para
        int m_spectrumSpeed;
        float m_speedRatio;
        int m_pwLines;  //计算一个心动周期的pw线数，设定prf=9.0kHz
        int m_lineNo;
        bool m_readPw;
        unsigned char *m_pPwBuf;
        bool m_shutPwChangeKnob;
        int m_cout;

		// pw calculate para
		float m_prfChg;
		int m_baselineChg;
		int m_refPRF;

        void CalcQAPw(unsigned char *pImgData);
        float MeanMaxMinCal(float *pSrc, int size, int flag);
		void AnalysisPw(unsigned char *srcData, int height, int width);
        void SaveImage(unsigned char *pBits, int size);

};