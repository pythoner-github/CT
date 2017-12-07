#ifndef IMAGE_AREA_2D_H
#define IMAGE_AREA_2D_H

#include "imageProc/ModeStatus.h"
#include <vector>
#include "Def.h"
#include "imageProc/Image.h"

class ImageArea;
using std::vector;

#define SCALE_W 8
#define SCALE_H 4

#define TGC_W 15

#define IMAGE_TMP 45

#define GRAYSCALE_W 12
#define GRAYSCALE_H 256

#define COLORSCALE_W 20
#define COLORSCALE_H 256

class ImageAreaDraw
{
public:
    enum SCALEUNIT{CMPS, KHZ};  // cm/s and KHz
    enum EFormat { SINGLEB, DUAL, QUAD, L1R1, U1D2, U2D1, U1D1 };

    static ImageAreaDraw * GetInstance();
    ~ImageAreaDraw();
    void Update();
    void GetProbePara(int type, int lines, int width, int r);
    void ClearTextArea(bool update=false);
    void UpdateAngle(int angle, bool isCw, bool draw, bool update=true);
    void SetAngleValue(int angle);
    void GetBoundary(int &up, int &down, int &left, int &right);
    int GetPointPos(int x, int y);

    void GetAnatomicBoundary(int &up, int &down, int &left, int &right);
// 2D
    void DrawScanDirection(bool update=true);
    void SetSDFreeze(bool update=true);
    void DrawScaleY(bool update=true);
    void SetScaleYFreeze(void);
    void SetGrayTable(RGBTRIPLE *table, int size, bool update=true);
    void DrawGrayScale(bool update=true);
    void DrawFocus(int focSum, int focPos[], bool update=true);
    void ReDrawFocus(bool update=true);
    void ClearFocus(bool update=true);
    void DrawTGCLine(unsigned char* tgc, int tgcSize, bool update=true);
    void ClearTGCLine(bool update=true);
    void DrawCenterLine(bool update=true);
    void ClearCenterLine(bool update=true);
    void ReDrawBiopsyLine(bool update=true);
    void ReDrawBiopsyDoubleLine(bool update=true);//2016.11.05--hy
    void ClearBiopsyLine(bool update=true);
    void ClearBiopsyDoubleLine(bool update=true);//2016.11.03
    void DrawBiopsyLine(int xDist, int yDist, int size, bool update=true);
   // void DrawBiopsyLine(int xDist, int yDist, bool update=true);
    void DrawBiopsyLine(double xDist, double yDist, bool update=true);

    /*void DrawBiopsyDoubleLine(int xDist0, int yDist0, int xDist1, int yDist1, bool update=true);*///2016.11.03-hy
    void DrawBiopsyDoubleLine(double xDist0, double yDist0, double xDist1, double yDist1, bool update=true);//2016.11.03-hy

    void DrawReplayBar(int cur_img, int total_img, int left, int right);
    void ClearReplayBar(void);
    void SetSteerAngle(int angle);
    void Set2DSteerAngle(int angle);

// M
    void DrawBMLine(int curBMLine, bool update=true);
    void ClearBMLine(bool update=true);
    void ReDrawBMLine(bool update=true);
    void DrawScaleYM(bool update=true);
    void DrawScaleXM(bool update=true);
    void DrawGridM(bool update=true);
    void ClearGridM(bool update=true);
    void DrawAnatomicalM(POINT head, POINT tail, bool draw, bool update=true);
    void ReDrawAnatomicalM(bool update=true);
// PW
    void DrawSV(int curPwLine, int dotBegin, int dotEnd, vector<int> HPRFEmitPos, bool update=true);
    void ClearSV(bool update=true);
    void ReDrawSV(bool update=true);
    void DrawSVCW(int curPwLine, int dotPos, bool update=true);
    void ClearSVCW(bool update=true);
    void ReDrawSVCW(bool update=true);
    void SetPwScaleUnit(SCALEUNIT unit, bool update=true);
    void SetPwBaseLine(int curLevel, int maxLevel, bool update=true);
    void DrawScaleYPw(bool update=true);
    void DrawScaleXPw(bool update=true);
    void DrawGridPw(bool update=true);
    void ClearGridPw(bool update=true);
    void DrawPwTraceAuto(bool auto_calc, const Image::AutoTracePara *para);
    void DrawPwTrace(bool auto_calc, const Image::AutoTracePara *para);
     void ClearAutoCalcPara(bool auto_calc=true);
    void DrawTraceTag(const Image::AutoTracePara* para, POINT &cycStart, POINT &cycEnd, POINT &ps, POINT &ed);
    void GetPwTracePara(Image::AutoTracePara &para);
    Image::AutoTracePara* NewPwTracePara(void);
    void SetPsEdValue(POINT ps, POINT ed, int curOpt);
// CFM
    void SetColorTable(unsigned char table[][4], int len, int width, int bitDepth, bool update=true);
    void DrawColorScale(bool update=true);
    void DrawVelScaleRange(void);
    void SetColorScaleRange(double max, double min, double maxKHz, double minKHz);
    void SetColorScaleUnit(SCALEUNIT unit);
    void SetCfmBoxDashStatus(bool isDash);
    void DrawCfmBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool update=true);
    void ClearCfmBox(bool update=true);
    void ReDrawCfmBox(bool update=true);
    void ReDrawCfmBoxForFreeze(bool update=true);
    void SetCfmBoxFreeze(void);
    void SetCfmBoxFreeze(int preB);
    void UnSetCfmBoxFreeze(void);

// OTHER
    void DrawPIPBox(int x, int y, int width, int height, bool update=true);
    void ClearPIPBox(bool update=true);
    void ReDrawPIPBox(bool update=true);
    void DrawGrayRejectCurve(unsigned char Point, bool update=true);
    void DrawGrayTransCurve(unsigned char *CurveValue, unsigned char *BasePoint, unsigned char Current, bool update=true);
    void ClearGrayCurve(bool update=true);
    void SetLocalZoomBoxDashStatus(bool isDash);
    void DrawLocalZoom(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool update=true);
    void ClearLocalZoom(bool update=true);
    void ReDrawLocalZoom(bool update=true);
    void DrawMagnifier(int x, int y, bool update=true);
    void ClearMagnifier(bool update=true);
    void ReDrawMagnifier(bool update=true);
    void DrawMoveFlag(int x, int y, bool update=true);
    void ClearMoveFlag(bool update=true);
// EFOV
    void DrawEFOVZoomMiniatureBox(int start_x,  int start_y, int end_x, int end_y);
    void ReDrawEFOVZoomMiniatureBox(void);
    void ClearEFOVZoomMiniatureBox(void);
    void EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y, bool draw);
    void DrawEFOVZoomBox(int start_x,  int start_y, int end_x, int end_y);
    void ReDrawEFOVZoomBox(void);
    void ClearEFOVZoomBox(void);
    void EFOVZoomBox(int start_x,  int start_y, int end_x, int end_y, bool draw);
    void DrawEFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right);
    void ReDrawEFOVViewBox(void);
    void ClearEFOVViewBox(void);
    void EFOVViewBox(POINT up_left, POINT down_left, POINT up_right, POINT down_right, bool draw);

    void DrawEFOV2DScaleY(bool prepare_mode);
    void DrawEFOVScaleY(void);
    void DrawEFOVScaleX(void);
    void EFOVScaleY(int scale_x, int scale_y, int height, double ratio);
    void EFOVScaleX(int scale_x, int scale_y, int height, double ratio);
    char* GetCalcAutoPara();
#ifdef CT_355
    void SetClearStatus(bool on);
#endif
private:
    ImageAreaDraw();
    void ChangeFormat2D(Format2D::EFormat2D format, int curB, double ratio2D, bool LRStatus, bool UDStatus);
    void ChangeFormatM(FormatM::EFormatM format);
    void ChangeFormatMInit(void);
    void ChangeFormatAnatomicM(FormatM::EFormatM format);
    void ChangeFormatPw(FormatPw::EFormatPw format);
    void ChangeFormatPwInit(void);
    void ChangeFormatCfm(FormatCfm::EFormatCfm format, int curB, double ratio2D, bool LRStatus, bool UDStatus);
    void ChangeFormatPwCfmInit(void);
    void ChangeFormatPwCfm(FormatPw::EFormatPw format);
    void ChangeFormatColorVs2D(void);

    void DrawVScaleY(bool U2D, int curB);
    void DrawHScaleY(bool L2R);
    void HorFocus(vector<int> &focPos, bool draw, bool L2R);
    void VerFocus(vector<int> &focPos, bool draw, bool U2D);
    void CenterLine(int rotate);

    int PwScaleCalc(double vel);
    void ScanDirection(int x, int y, bool CurB);
    void BMLine(int curBMLine, bool LRStatus, bool UDStatus, GdkPoint pointBM, bool draw);
    void PwBaseline(int x, int y, GdkColor *color);
    void GridM(double ratioX, double ratioY, GdkPoint pointScaleYM, bool draw);
    void GridPw(float maxVel, int maxLevel, int curLevel, float ratioX, float ratioY, bool draw);
    void Focus(vector<int> &focPos, bool draw);
    void SampleVolumn(int curPwLine, int dotBegin, int dotEnd, vector<int> HPRFEmitPos, bool LRStatus, bool UDStatus, GdkPoint pointPw, bool isCw, bool draw);
    void CfmBox(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool LRStatus, bool UDStatus, GdkPoint pointCfm, bool draw, bool dash = false);
    void LocalZoom(int lineBegin, int lineEnd, int dotBegin, int dotEnd, bool LRStatus, bool UDStatus, GdkPoint pointLZ, bool draw, bool dash = false);
    void BiopsyLine(int xDist, int yDist, bool LRStatus, bool UDStatus, int rotate, int size, bool draw);
    //void BiopsyLine(int xDist, int yDist, bool LRStatus, bool UDStatus, int rotate, bool draw);
void BiopsyLine(double xDist, double yDist, bool LRStatus, bool UDStatus, int rotate, bool draw);

    void CalcScanDirection(int &xPos, int &yPos);

    //    void PwTrace(const vector<POINT>& trace);
    //    void PwCycle(int begin, int end);
#ifdef CT_355
    bool m_clear;
#endif
    POINT m_EFOVZoomMiniatureBox[2];
    bool m_drawEFOVZoomMiniature;

    POINT m_EFOVZoomBox[2];
    bool m_drawEFOVZoom;
    POINT m_EFOVViewBox[4];
    bool m_drawEFOVView;

    static ImageAreaDraw * m_ptrInstance;

    int m_probeType;
    int m_probeLines;
    double m_probeWidth;
    int m_probeR;

    ImageArea* m_ptrImg;
    int m_correctAngle;
    int m_steerAngle;
    int m_steerAngle2D;
    int m_scaleYSpacing;

    ScanMode::EScanMode m_scanMode;
    Format2D::EFormat2D m_format2D;
    FormatM::EFormatM m_formatM;
    FormatPw::EFormatPw m_formatPw;
    FormatCfm::EFormatCfm m_formatCfm;
    int m_curB;// curent B image. BB(0-left, 1-right), 2B(0-upleft, 1-upright, 2-downleft, 3-downright)
    bool m_LRStatus;
    bool m_UDStatus;
    int m_rotate;

    double m_ratio2D;
    double m_ratio2DAll[4];
    double m_zoomScale;
    int m_zoomOffset;

    int m_countSD;
    vector<GdkPoint> m_pointSD;
    bool m_drawSD[4];
    struct prePointSD {
        int x;
        int y;
        bool LRStatus;
        bool UDStatus;
    };
    prePointSD m_prePointSD[4];

    int m_countScaleY;
    int m_heightScaleY;
    int m_widthScaleY;
    vector<GdkPoint> m_pointScaleY;
    vector<GdkPoint> m_clearScaleY;
    int m_heightClearScaleY;
    int m_widthClearScaleY;

    struct focInfo{
        int focSum;
        vector<int> focPos;
    };
    focInfo m_preFocInfo;
//    vector<focInfo> m_focInfo;
    GdkPoint m_pointFocus;
    bool m_focusDraw;

    // Biopsy
    struct preBiopsy{
    /*int xDist;
    int yDist;*/
    double xDist;
    double yDist;
    bool LRStatus;
    bool UDStatus;
    int rotate;
    };
    preBiopsy m_preBiopsy;
    bool m_drawBiopsy;
    //-----2016.11.03---hy-----------------------------------//
    bool m_drawBiopsyDouble;
    preBiopsy m_preBiopsyDouble0;
    preBiopsy m_preBiopsyDouble1;
    //-------------------------------------------------------//
    // center line
    int m_preRotate;

    GdkPoint m_pointBM;
    int m_widthImgBM;
    int m_heightImgBM;
    bool m_mLineDraw;
    bool m_drawMline;
//    int m_mLine;
    struct PreMLine{
    int BMLine;
    bool LRStatus;
    bool UDStatus;
    GdkPoint pointBM;
    };
    PreMLine m_preMLine;

    GdkPoint m_pointScaleYM;
    GdkPoint m_prePointScaleYM;
    float m_heightScaleYM;
    double m_ratioYM;
    double m_ratioYMPre;

    GdkPoint m_pointScaleXM;
    float m_widthScaleXM;
    double m_ratioXM;
    double m_ratioXMPre;

    bool m_drawGridM;

    // Anatomical M
    POINT m_headMPre;
    POINT m_tailMPre;
    // pw
    GdkPoint m_pointPw;
    double m_zoomRatioPw;
    int m_heightImgPw;
    int m_widthImgPw;
    struct PreSV{
    int pwLine;
    int dotBegin;
    int dotEnd;
    bool LRStatus;
    bool UDStatus;
    GdkPoint pointPw;
    vector<int> HPRFEmitPos;
    };
    PreSV m_preSV;
    //   int m_preSV[3]; // pwLine, dotBegin, dotEnd
    bool m_SVDraw;
    bool m_drawSV;

    GdkPoint m_pointScaleYPw;
    int m_widthScaleYPw;
    int m_heightScaleYPw;
    float m_ratioYPw;

//    char m_pwScaleUnit[10];
    SCALEUNIT m_pwUnit;

    float m_pwMaxVel;
    int m_pwMaxLevel;
    int m_pwCurLevel;
//    int m_pwDots;
    bool m_drawGridPw;
    float m_pwMaxVelPre;
    int m_pwMaxLevelPre;
    int m_pwCurLevelPre;
    float m_ratioYPwPre;
    float m_ratioXPwPre;
    bool m_drawPwBaseline;
    GdkPoint m_pwBaseline;

    GdkPoint m_pointScaleXPw;
    float m_ratioXPw;
    float m_widthScaleXPw;
    Image::AutoTracePara m_pwTracePara;
//  vector<POINT> m_pwTrace;
//  vector<POINT> m_maxTrace;
    //vector<POINT> m_validTrace;
    POINT m_psP;
    POINT m_edP;
    int m_curOpt;

    // cfm
    GdkPoint m_pointCfm;
    double m_zoomRatioCfm;
    int m_widthImgCfm;
    int m_heightImgCfm;

    SCALEUNIT m_cfmUnit;

    struct PreBoxPara{
        int lineBegin;
        int lineEnd;
        int dotBegin;
        int dotEnd;
    bool LRStatus;
    bool UDStatus;
    GdkPoint point;
    };
    PreBoxPara m_preBox[4];
    PreBoxPara m_curBox;
    bool m_drawBox[4];
    bool m_dashCfmBox;
//    int m_preBox[4];  // lineBegin, lineEnd, dotBegin, dotEnd
//    bool m_boxDraw;
    double m_colRange[4];

    struct ColorScale{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    };
    vector<ColorScale> m_colorTable;
    int m_colorScaleLen;
    int m_colorScaleWidth;

    // other
    int m_PIPBox[4]; //x, y, width, height
    bool m_PIPBoxDraw;

    GdkPoint m_pointLZ;
    PreBoxPara m_preLZ;
    bool m_LZDraw;
    bool m_inLZMode;
    int m_LZLineRange[2];
    int m_LZDotRange[2];
    int m_LZDotBeginDepth;
    double m_LZScale;
    bool m_dashLZBox;

    RGBTRIPLE m_grayTable[256];
    int m_graySize;

    POINT m_magnifier;
    bool m_magnifierDraw;

    POINT m_moveFlag;
    bool m_moveFlagDraw;
    EFormat m_format;

    char* m_autoPara;

};
#endif