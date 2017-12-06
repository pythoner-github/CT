#ifndef FORMAT_CFM_H
#define FORMAT_CFM_H

#include "imageProc/Format2D.h"
#include "imageProc/FpgaReceive.h"
#include "AbsUpdateCfm.h"
#include "imageControl/ImgCfm.h"

class FormatCfm
{
    public:
        ///> new type
        enum EFormatCfm{B, BB, B4}; ///< 2d display format, "B"=one B, "BB"=two B, "B4"=four B,

        static FormatCfm* GetInstance();
        ~FormatCfm();

        void ChangeFormat(enum EFormatCfm format);
        bool SwitchBB(bool left, int &current);
        bool SwitchBBForFreeze(int lastRealB);
        int SwitchBB(void);
        int SwitchB4();
        int SwitchB4ForFreeze(int curB, int lastRealB);

        EFormatCfm GetFormat();

        // special measure
        void SetFormatForSnap(EFormatCfm format) { m_formatSnap = format; }

    private:
        FormatCfm();

        static FormatCfm* m_ptrInstance;

        DscMan* m_ptrDsc;
        DSCCONTROLATTRIBUTES* m_ptrDscPara;

        AbsUpdateCfm* m_ptrUpdate;
        Replay* m_ptrReplay;
        ImgCfm* m_ptrImg;

        EFormatCfm m_format; ///< current cfm format
        int m_curB; ///< curent B image. BB(0-left, 1-right), 2B(0-upleft, 1-upright, 2-downleft, 3-downright)
        EFormatCfm m_formatSnap;
};

#endif