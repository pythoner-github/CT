#ifndef FORMAT_M_H
#define FORMAT_M_H

#include "Img2D.h"
#include "Replay.h"
#include "DSCTypes.h"
#include "DscMan.h"


/*
 * @brief change M display format or switch current image in all format except M_TOTAL
 */
class FormatM
{
	public:
		///> new type
		enum EFormatM{M_TOTAL, BM11_UD, BM21_UD, BM12_UD, BM11_LR}; ///< M display format, "UD"=up down, "LR"=left right, "11"=1:1, "12"=1:2

		static FormatM* GetInstance();
		~FormatM();

		void ChangeFormat(enum EFormatM format);
		EFormatM GetFormat();

		int GetBImgHeightDot(void);
		POINT GetBCenterPoint(void);

        ///> special measure
        void SetFormatForSnap(EFormatM format) { m_formatSnap = format; }

	private:
		FormatM();

		static FormatM* m_ptrInstance;

		DscMan* m_ptrDsc;
		DSCCONTROLATTRIBUTES* m_ptrDscPara;		
		
		AbsUpdate2D* m_ptrUpdate;
		Img2D* m_ptrImg;
		Replay* m_ptrReplay;

		EFormatM m_format;
        EFormatM m_formatSnap;
};
#endif
