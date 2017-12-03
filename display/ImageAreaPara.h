#ifndef IMAGE_AREA_PARA_H
#define IMAGE_AREA_PARA_H

#include <string>
#include "ImageArea.h"

using std::string;

class ImageAreaPara
{
public:
    ~ImageAreaPara();
    static ImageAreaPara* GetInstance();
    int GetGenFpsReplay();
    int GetGenFps();

    void DrawGeneralPara();
    void Draw2DPara();
    void DrawMPara();
    void DrawPwPara();
    void DrawCwPara();
    void DrawCfmPara();
    void DrawPdiPara();

    void UpdateGenPwr(int data, bool draw = TRUE);
    void UpdateGenZoomScale(double data, bool draw = TRUE);
#ifndef EMP_355
    void UpdateGenFps(int data, bool draw = TRUE);
#else
    void UpdateGenFps(int receiveFps, int processFps, bool draw = TRUE);
#endif
    void UpdateVolume(int data, bool draw = TRUE);

    void Update2DGain(int data, bool draw = TRUE);
    void Update2DDynamicRange(int data, bool draw = TRUE);
    void Update2DLineDensity(const char* data, bool draw = TRUE);
    void Update2DMBP(int data, bool draw = TRUE);
    void Update2DTHI(bool data, bool draw = TRUE);
    void Update2DTSI(const char* data, bool draw = TRUE);

    void UpdateMGain(int data, bool draw = TRUE);
    void UpdateMDynamicRange(int data, bool draw = TRUE);

    void UpdateCwGain(int data, bool draw = TRUE);
    void UpdatePwGain(int data, bool draw = TRUE);
    void UpdatePwWF(int data, bool draw = TRUE);
    void UpdatePwSV(double data, bool draw = TRUE);
    void UpdatePwPRF(double data, bool draw = TRUE);
    void UpdatePwAngle(int data, bool draw = TRUE);
    void UpdatePwSVPos(double data, bool draw = TRUE);
    void UpdatePwHPRFStatus(bool on, bool draw = TRUE);

    //void UpdateCwGain(int data, bool draw = TRUE);
    void UpdateCwWF(int data, bool draw = TRUE);
    void UpdateCwPRF(double data, bool draw = TRUE);
    void UpdateCwAngle(int data, bool draw = TRUE);
    void UpdateCwSVPos(double data, bool draw = TRUE);

    void UpdateCfmGain(int data, bool draw = TRUE);
    void UpdateCfmWF(int data, bool draw = TRUE);
    void UpdateCfmPRF(int data, bool draw = TRUE);
    void UpdateCfmPersist(int data, bool draw = TRUE);
    void UpdateCfmSensitive(int data, bool draw = TRUE);

    void UpdatePdiGain(int data, bool draw = TRUE);
    void UpdatePdiWF(int data, bool draw = TRUE);
    void UpdatePdiPersist(int data, bool draw = TRUE);
    void UpdatePdiSensitive(int data, bool draw = TRUE);
    void UpdateAutoOptimize(bool data, bool draw = TRUE);    
    void UpdateFreq(const char* freq, bool draw=TRUE);
    void UpdateCfmDopplerFreq(const char* freq, bool draw=TRUE);
    void UpdatePdiDopplerFreq(const char* freq, bool draw=TRUE);
    void UpdateDepth(int depth, bool draw=TRUE);
	void FillRectangle(int x, int y, int w, int h);
		
#ifdef TRANSDUCER
    void UpdateTransducer(int data,bool draw =TRUE);
    void DrawTransducer(void);
    void ReDrawTransducer(void);
#endif

private:
    ImageAreaPara();

    void DrawPwGenPara(void);
    void DrawPwGen2Para(void);
		
    static ImageAreaPara* m_ptrInstance;
    static char * Level[2];	
		
    ImageArea* m_ptrImg;
		
    char m_bufText[100];
    char m_freq[100];
	char m_freqDoppler[100];
    int m_depth;

    int m_pwr;
    double m_zoomScale;
    int m_fps;
    int m_fpsBak;
    int m_fpsForReplay;
#ifdef TRANSDUCER
    int m_transducer;
    int m_transducerBak;
#endif
    int m_gain2D;
    int m_dynamicRange;
    string m_lineDensity;
    int m_mbp;
    int m_gainM;
    bool m_thi;
    string m_tsi;

    int m_gainPw;
    int m_wfPw;
    double m_svPw;
    double m_PRFPw;
    int m_anglePw;
    int m_volumePw;
    double m_svPosPw;
    bool m_HPRFStatus;

    int m_gainCw;
    int m_wfCw;
    double m_PRFCw;
    int m_angleCw;
    double m_svPosCw;
		
    int m_gainCfm;
    int m_wfCfm;
    int m_prfCfm;
    int m_persistCfm;
    int m_sensitiveCfm;

    int m_gainPdi;
    bool m_optimize;

    ImageArea::DrawAttr m_attr;
	static const int m_eareH = 20;
	static const int m_eareWl = 220;
	static const int m_eareWs = 40;

};
#endif
