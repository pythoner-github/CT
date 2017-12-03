#ifndef MULTI_FUNC_VALUE
#define MULTI_FUNC_VALUE

#include "AbsMultiFunc.h"
#include "NoteArea.h"
#include "MeasureDraw.h"
#include "ImgProc2D.h"
#include "Zoom.h"
#include "MeasureFactory.h"
#include "HintArea.h"
#include "../imageProc/PartFuncMan.h"
#include "../include/Def.h"
#include "../imageControl/Knob2D.h"
#include "../imageProc/GlobalClassMan.h"
#include "LightDef.h"
#include "../imageProc/ModeStatus.h"
/*
 * @brief focus
 */
class MultiFuncFocus: public AbsMultiFunc
{
    public:
        MultiFuncFocus()
        {
            Img2D* ptrImg = Img2D::GetInstance();
            if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || Zoom::GetInstance()->GetLocalZoomStatus()|| ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus())
                HintArea::GetInstance()->UpdateHint(_("[Focus]: Mouse to move focus. When zoom, compound imaging, Tp-View or EFVI is on, it only has one focus."));
            else
#ifndef EMP_355
                HintArea::GetInstance()->UpdateHint(_("[Focus]: Mouse to move focus, <Value> to change focus sum."));
#endif
			g_ptrKeyFunc = ChgFocusNum;
        }


        ~MultiFuncFocus()
       
        {
            HintArea::GetInstance()->ClearHint();
            g_ptrKeyFunc = NULL;
#if (defined (EMP_322)|| defined(EMP_313))
            g_keyInterface.CtrlLight(FALSE,LIGHT_FOCUS);
#endif
        }
        void Value(EKnobOper opr);
        void Mouse(int offsetX, int offsetY);
        void Undo();
        void KeyUp();
        void KeyDown();
};

/*
 * @brief Depth
*/
class MultiFuncDepth: public AbsMultiFunc
{
	public:
		MultiFuncDepth()
		{   
#ifndef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[Depth]: <Value> to change depth size."));
#endif
			g_ptrKeyFunc = D2PressDepth;

        }

        ~MultiFuncDepth()
        {
            HintArea::GetInstance()->ClearHint();
            g_ptrKeyFunc = NULL;
#if (defined (EMP_322)|| defined(EMP_313))
            g_keyInterface.CtrlLight(FALSE,LIGHT_DEPTH);
#endif

        }
        void Value(EKnobOper opr);
        void Undo();
};


/*
 * @brief local zoom
 */
class MultiFuncLocalZoom: public AbsMultiFunc
{
    public:
        MultiFuncLocalZoom()
        {
#if (defined (EMP_322) || defined(EMP_313))
        HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Mouse to move box, <Set> to change box size and to zoom."));
#else
        HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Mouse to move box, <Set> to change box size, <Update> to zoom."));
#endif
            m_changeSize = FALSE;
            Zoom::GetInstance()->LocalZoomInit();
			Zoom::GetInstance()->SetLocalZoomBoxStatus(m_changeSize);
            n1 = 0;
        }
		~MultiFuncLocalZoom()
		{

			HintArea::GetInstance()->ClearHint();
			Zoom::GetInstance()->ClearLocalZoom();
            if((MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)&&(!(Zoom::GetInstance()->GetLocalZoomStatus())))
            {
#if (defined (EMP_322) || defined(EMP_313))
            g_keyInterface.CtrlLight(FALSE,LIGHT_ZOOM);
#endif
            }
		}

		void Do();
		void Undo();
		void Update();
		void Mouse(int offsetX, int offsetY); ///< change sample box's position
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();

	private:
		int n1;
        bool m_changeSize; //true: change size, false: move pos
};

/*
 * @brief global zoom
 */
class MultiFuncGlobalZoom: public AbsMultiFunc
{
	public:
		MultiFuncGlobalZoom()
		{
#ifdef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: <Auto> to change scale, Mouse to scroll."));
#else
            HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: <Value> to change scale, Mouse to scroll."));
#endif
			Zoom::GetInstance()->GlobalZoomCtrl(TRUE);
            g_ptrKeyFunc = GlobalZoom;

		}
		~MultiFuncGlobalZoom()
        {
            HintArea::GetInstance()->ClearHint();
            Zoom::GetInstance()->GlobalZoomCtrl(FALSE);
            g_ptrKeyFunc = NULL;

		}

		void Value(EKnobOper opr); ///< change scale
		void Mouse(int offsetX, int offsetY); ///< change sample box's position
		void Undo(); 
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
};

/*
 * @brief PIP zoom
 */
class MultiFuncPIPZoom: public AbsMultiFunc
{
	public:
		MultiFuncPIPZoom()
		{
#ifdef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[PIP]: <Auto> to change scale, Mouse to Move Magnifier."));
#else
            HintArea::GetInstance()->UpdateHint(_("[PIP]: <Value> to change scale, Mouse to Move Magnifier."));
#endif
			Zoom::GetInstance()->PIPZoomCtrl(TRUE);
            g_ptrKeyFunc = PIPZoom;

		}
		~MultiFuncPIPZoom()
        { 
            HintArea::GetInstance()->ClearHint();
            Zoom::GetInstance()->PIPZoomCtrl(FALSE);
            g_ptrKeyFunc = NULL;

		}

		void Value(EKnobOper opr); ///< change scale
		void Mouse(int offsetX, int offsetY); ///< change sample box's position
		void Undo(); 
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
};


/*
 * @brief measure
 */

class MultiFuncMeasure: public AbsMultiFunc
{
    public:
        MultiFuncMeasure()
        {
            PRINTF("enter measure mode\n");
          //  m_ptrUpdateMea = GlobalClassMan::GetInstance()->GetUpdateMix();
        }
        ~MultiFuncMeasure()
        {
            PRINTF("exit measure mode\n");
            if (g_ptrAbsMeasure != NULL)
            {
                delete g_ptrAbsMeasure;
                g_ptrAbsMeasure  = NULL;
            }

        }
        void Undo();
        void Do();
        void Update();
        void Value(EKnobOper opr);
        void Mouse(int offsetX, int offsetY); 
        void KeyLeft();
        void KeyRight();
        void KeyUp();
        void KeyDown();

	private:
        POINT m_p;
        MeasureDraw m_draw;
        //AbsUpdateMix* m_ptrUpdateMea;

		POINT GetPoint(int offsetX, int offsetY);
};

#if 0
/*
 * @brief calc
 */

class MultiFuncCalc: public AbsMultiFunc
{
    public:
        MultiFuncCalc()
        {
            PRINTF("enter calc mode\n");
            m_ptrUpdateCal = GlobalClassMan::GetInstance()->GetUpdateMix();
        }
        ~MultiFuncCalc()
        { 
            PRINTF("exit calc mode\n");
            if (g_ptrAbsMeasure != NULL)
            {
                delete g_ptrAbsMeasure;
                g_ptrAbsMeasure  = NULL;
            }

            m_ptrUpdateCal->ExitCalc();
#if defined (EMP_322)
            g_keyInterface.CtrlLight(FALSE,LIGHT_CALC);
#endif
        }
        void Undo();

	private:
        AbsUpdateMix* m_ptrUpdateCal;

};
#endif
/*
 * @brief text comment
 */ 
class MultiFuncText: public AbsMultiFunc
{
	public:
		MultiFuncText()
		{
			PRINTF("enter text mode\n");
			NoteArea::GetInstance()->Enter();
		}
		~MultiFuncText()
		{
			PRINTF("exit text mode\n");
			NoteArea::GetInstance()->Hide();
		}
        void Undo();
};

/*
 * @brief gray transform 
 */ 
class MultiFuncGrayTrans: public AbsMultiFunc
{
	public: 	
        MultiFuncGrayTrans()
		{
#if (defined(EMP_322) || defined(EMP_313))
            HintArea::GetInstance()->UpdateHint(_("[Gray Transform]: Mouse to move dot, <Value> to select dot, <Set> to select curve."));
#elif defined EMP_355
            HintArea::GetInstance()->UpdateHint(_("[Gray Transform]: Mouse to move dot, <Auto> to select dot, <Update> to select curve."));
#else
            HintArea::GetInstance()->UpdateHint(_("[Gray Transform]: Mouse to move dot, <Value> to select dot, <Update> to select curve."));

#endif
            ImgProc2D::GetInstance()->DisplayGrayTrans(TRUE);
            g_ptrKeyFunc = GrayTransport;
		}

		~MultiFuncGrayTrans()
		{
            HintArea::GetInstance()->ClearHint();
            ImgProc2D::GetInstance()->DisplayGrayTrans(FALSE);
            g_ptrKeyFunc = NULL;
		}
		void Mouse(int offsetX, int offsetY); 
		void Value(EKnobOper opr);
        void Do();
		void Undo();
		void Update();
		void KeyUp();
		void KeyDown();
};

/*
 * @brief gray reject
 */ 
class MultiFuncGrayReject: public AbsMultiFunc
{
	public:
		MultiFuncGrayReject()
		{
			HintArea::GetInstance()->UpdateHint(_("[Gray Reject]: Mouse to move position."));
			ImgProc2D::GetInstance()->DisplayGrayReject(TRUE);
		}

		~MultiFuncGrayReject()
		{
			HintArea::GetInstance()->ClearHint();
			ImgProc2D::GetInstance()->DisplayGrayReject(FALSE);
		}
		void Mouse(int offsetX, int offsetY); 
		void Undo(); 
		void KeyLeft();
		void KeyRight();
};
/*
 * @brief freq. reject
 */ 
class MultiFuncFreq: public AbsMultiFunc
{
    public:
        MultiFuncFreq()
        {
            HintArea::GetInstance()->UpdateHint(_("[Freq.]: <Value> change freq. size"));
            g_ptrKeyFunc = D2ChgFreq;

        }

        ~MultiFuncFreq()
        {
            HintArea::GetInstance()->ClearHint();
            g_ptrKeyFunc = NULL;
#if (defined (EMP_322)|| defined(EMP_313))
            g_keyInterface.CtrlLight(FALSE,LIGHT_FREQ);
#endif
        }
        void Undo();
};

/*
 * @brief change chroma  调节伪彩
 */ 
class MultiFuncChroma: public AbsMultiFunc
{
    public:
        MultiFuncChroma()
        {
            HintArea::GetInstance()->UpdateHint(_("[Chroma  Reject]: <Value> change chroma "));
            g_ptrKeyFunc = D2ChgChroma;

        }

        ~MultiFuncChroma()
        {
            HintArea::GetInstance()->ClearHint();
            g_ptrKeyFunc = NULL;
#if defined (EMP_322)
            g_keyInterface.CtrlLight(FALSE,LIGHT_CHROMA);
#endif
        }
        void Undo();
};

/*@brief biopsy verify*/

/*class MultiFuncBiopsyVerify: public AbsMultiFunc,public FakeXEvent
{
	public:
		MultiFuncBiopsyVerify()
		{
            HintArea::GetInstance()->UpdateHint(_("[Verify]: <Value> to adjust angle, Mouse to adjust position."));
		}
		~MultiFuncBiopsyVerify()
        { 
            HintArea::GetInstance()->ClearHint();
		}

		void Value(EKnobOper opr); ///< verify angle
		void Mouse(int offsetX, int offsetY); ///< verify position
		void Undo(); 
		void Do();

		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		//------2016.09.11--------------------------------//
		void KeyEvent(unsigned char keyValue);
		//------------------------------------------//
};*/

/*@brief biopsy */

/*class MultiFuncBiopsy: public AbsMultiFunc,public FakeXEvent
{
    public:
    void Mouse(int offsetX, int offsetY);
	void Undo();
	void Do();
	//------2016.09.11--------------------------------//
	void KeyEvent(unsigned char keyValue);
	//------------------------------------------//

};*/
class MultiFuncBiopsyVerify: public AbsMultiFunc
{
	public:
		MultiFuncBiopsyVerify()
		{
#ifdef EMP_355
            HintArea::GetInstance()->UpdateHint(_("[Verify]: <Auto> to adjust angle, Mouse to adjust position."));
        #else
 HintArea::GetInstance()->UpdateHint(_("[Verify]: <Value> to adjust angle, Mouse to adjust position."));
#endif
		}
		~MultiFuncBiopsyVerify()
        { 
            HintArea::GetInstance()->ClearHint();
		}

		void Value(EKnobOper opr); ///< verify angle
		void Mouse(int offsetX, int offsetY); ///< verify position
		void Undo(); 
		void Do();

		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
};

/*@brief biopsy */

class MultiFuncBiopsy: public AbsMultiFunc
{
    public:
    void Mouse(int offsetX, int offsetY);
	void Undo();
	void Do();

};

#endif
