#ifndef DRAWIMT_H
#define DRAWIMT_H


#include <math.h>
#include "MeasureDraw.h"
#include "UpdateMeasure.h"

class CDrawIMT
{
    public:
        CDrawIMT();
        ~CDrawIMT();
        static CDrawIMT* GetInstance();
        void GetArea(POINT& area_start, POINT& area_end);
        void GetWH(int& w, int& h);
        void GetResultAttr(UpdateMeasure::ResultAttr& attr);
        void StartTimer(void);
        void RemoveTimer(void);
        void UpdateIMT(void);
        void GetDirIMT(bool& dir);
        void GetRealIMTStatus(bool& imtstatus);

        void GetIMTStatus(bool& imtstatus);
        bool GetIMTRealStatus() { return m_RealIMTStatus;}
        bool UpdateIMTStatus() { return m_IMTStatus;}
        void ClearInAdventIMT(void);

    private:
        void SetRoiMem(void);
        void SetInAdventIMTMem(void);
        void GetUpDownStatus(void);
     

        unsigned int m_on_off;
        POINT m_area_start;
        POINT m_area_end;
        int g_imtTimer;
        unsigned int roi_w;
        unsigned int roi_h;
        unsigned int *pIntima;
        unsigned int *pAdventitia;
        unsigned char *pRoiPre;
        unsigned char *pRoi;
        bool m_dir;
        bool m_RealIMTStatus;
        bool m_IMTStatus;
        bool m_UpDownStatus;
        UpdateMeasure::ResultAttr m_attr;

        MeasureDraw m_draw;
        UpdateMeasure m_update;
        static CDrawIMT* m_ptrInstance;
};
#endif
