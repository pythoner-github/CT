#ifndef FORMAT_PW_H
#define FORMAT_PW_H

#include "ImgPw.h"
#include "AbsUpdatePw.h"
#include "Replay.h"
#include "DSCTypes.h"
#include "DscMan.h"

/*
 * @brief change M display format or switch current image in all format except M_TOTAL
 */
class FormatPw
{
	public:
		///> new type
		enum EFormatPw{P_TOTAL, BP11_UD, BP21_UD, BP12_UD, BP11_LR}; ///< M display format, "UD"=up down, "LR"=left right, "11"=1:1, "12"=1:2

		static FormatPw* GetInstance();
		~FormatPw();

		void ChangeFormat(enum EFormatPw format);

		EFormatPw GetFormat();
		int GetSpectrumDots() {return m_pwDots; PRINTF("----------------M_PWDOTS = %d\n", m_pwDots);}

        ///> special measure
        void SetFormatForSnap(EFormatPw format) { m_formatSnap = format;}

	private:
		FormatPw();

		static FormatPw* m_ptrInstance;

		DscMan* m_ptrDsc;
		DSCCONTROLATTRIBUTES* m_ptrDscPara;
		
		AbsUpdatePw* m_ptrUpdate;
		ImgPw* m_ptrImg;
		Replay* m_ptrReplay;

		EFormatPw m_format;
		EFormatPw m_formatSnap;

		int m_pwDots;
};
#endif
