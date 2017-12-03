#ifndef MULTI_FUNC_MODE
#define MULTI_FUNC_MODE

#include "AbsMultiFunc.h"
#include "../imageControl/KnobM.h"
#include "../imageControl/Img2D.h"
#include "../imageProc/Replay.h"
#include "KeyValueOpr.h"
#include "LightDef.h"
#include "../imageProc/ScanMode.h"
#include "../imageProc/ModeStatus.h"
#include "LightDef.h"
#include "GlobalClassMan.h"
#include "ImgProcPw.h"
#include "MenuPW.h"

//AbsUpdateMix* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();

void OverturnMouseOpr(int &offsetX, int &offsetY);

extern MenuPW g_menuPW;
/*
 * @brief none 
 */ 
class MultiFuncNone: public AbsMultiFunc
{
    public:
    void Do();
    void Mouse(int offsetX, int offsetY);
    void KeyLeft();
    void KeyRight();
    void KeyUp();
    void KeyDown();
    void Update();
    void Value(EKnobOper opr);
    void Undo();
};

/*
 * @brief Replay
 */ 
class MultiFuncReplay: public AbsMultiFunc
{
    public:
        MultiFuncReplay()
        {
            if(g_menuPW.GetAutoTraceStatus())
                ImgProcPw::GetInstance()->SetAutoCalc(TRUE);
            m_count = 0;
        }
        void Update();
        void Mouse(int offsetX, int offsetY);
        void Do();
        void KeyLeft();
        void KeyRight();
    private:
        int m_count;
};

/*
 * @brief M init
 */
class MultiFuncMInit: public AbsMultiFunc
{
	public:
		void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
};

class MultiFuncM: public AbsMultiFunc
{
	public:
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
};

/*
 * @brief pw init
 */
class MultiFuncPwInit: public AbsMultiFunc
{
	public:
		void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		void Value(EKnobOper oper);
};

/*
 * @brief pw
 */
class MultiFuncPw: public AbsMultiFunc
{
	public:
		void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		void Value(EKnobOper oper);
};

/*
 * @brief cfm box pos
 */
class MultiFuncCfm: public AbsMultiFunc
{
	public:
		void Do();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
};

/*
 * @brief pw+cfm init
 */
class MultiFuncPwCfmInit: public AbsMultiFunc
{
	public:
		MultiFuncPwCfmInit()
		{
			m_change = 0;
			ImgCfm::GetInstance()->SetBoxStatus(false);
		}
		void Do();
		virtual void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		void Value(EKnobOper oper); 

	private:
		char m_change; ///< 0-SV; 1-BOX pos; 2-BOX size
};

/*
 * @brief pw+cfm 
 */
class MultiFuncPwCfm: public MultiFuncPwCfmInit
{
	public:
		void Update();
};

/*
 * @brief pw+pdi init 
 */
class MultiFuncPwPdiInit: public MultiFuncPwCfmInit
{

	public:
		void Update();
};

/*
 * @brief pw+pdi 
 */
class MultiFuncPwPdi: public MultiFuncPwCfmInit
{
	public:
		void Update();
};


/*
 * @brief Anatomic M mode
 */
class MultiFuncAnatomicM: public AbsMultiFunc
{
	public:
		MultiFuncAnatomicM()
		{
			m_moveStatus = FALSE;
            KnobAnatomicMCreate();
		}
		~MultiFuncAnatomicM()
		{
		}

		void Mouse(int offsetX, int offsetY); 
		void Do(); 
		void KeyUp();
		void KeyDown();
		void KeyLeft();
		void KeyRight();

	private:
		bool m_moveStatus;
};

/*
 * @brief EFOV mode
 */
class MultiFuncEFOV: public AbsMultiFunc
{
	public:
		MultiFuncEFOV()
		{
		}
		~MultiFuncEFOV()
		{
		}

		void Update();
		void Mouse(int offsetX, int offsetY); 
		void Undo(); 
        void Do();
		void KeyLeft();
		void KeyRight();

	private:
};

/*
 * @brief cw init
 */
class MultiFuncCwInit: public AbsMultiFunc
{
	public:
		void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		CalcPw* m_ptrCalc;
};


/*
 * @brief cw
 */
class MultiFuncCw: public AbsMultiFunc
{
    private:
		//CalcPw* m_ptrCalc;

	public:
		void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		CalcPw* m_ptrCalc;


};

/*
 * @brief cw+cfm init
 */
class MultiFuncCwCfmInit: public AbsMultiFunc
{
	public:
		MultiFuncCwCfmInit()
		{
			m_change = 0;
			ImgCfm::GetInstance()->SetBoxStatus(false);
		}
		void Do();
		virtual void Update();
		void Mouse(int offsetX, int offsetY);
		void KeyLeft();
		void KeyRight();
		void KeyUp();
		void KeyDown();
		CalcPw* m_ptrCalc;
	private:
		char m_change; ///< 0-SV; 1-BOX pos; 2-BOX size

	//	CalcPw* m_ptrCalc;
};

/*
 * @brief cw+cfm 
 */
class MultiFuncCwCfm: public MultiFuncCwCfmInit
{

	public:
		void Update();
};

/*
 * @brief cw+pdi init 
 */
class MultiFuncCwPdiInit: public MultiFuncCwCfmInit
{

	public:
		void Update();
};

/*
 * @brief cw+pdi 
 */
class MultiFuncCwPdi: public MultiFuncCwCfmInit
{  private:
		CalcPw* m_ptrCalc;

	public:
		void Update();
};


#endif
