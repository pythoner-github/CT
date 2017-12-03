#ifndef KEY_FUNC_H
#define KEY_FUNC_H

#include "AbsCommand.h"
#include "ExamItem.h"
#include "ProbeSocket.h"
#include "MenuArea.h"

class KeySwitchIM: public AbsCommand
{
	public:
		bool Execute();
		bool ExcuteChange(bool isswitch);
		static bool m_imOn;	//TRUE: input method on; FALSE: input method off
};

class KeyPowerOff: public AbsCommand
{
	public:
		bool Execute();
};
      
class KeyTSI: public AbsCommand
{
	public:
		bool Execute();
	
    private:
		bool Do();
		bool Undo();
		static int keyNum ;
};
        
class KeyAutoOptimize: public AbsCommand
{
	public:
		bool Execute();
        static void AutoOff(void);

	private:
		bool Do();
		bool Undo();
		void ImgOptimize(ProbeSocket::ProbePara p, ExamItem::ParaItem i);
		void BackupPara();

		static ExamItem::ParaItem m_itemPara;
		static bool m_autoOn; // TRUE: enter optimize; FALSE: exit optimize
};

class KeyMeasure: public AbsCommand
{
	public:
		bool Execute();
};
class KeyCalc: public AbsCommand
{
	public:
		bool Execute();
};

class KeySystem : public AbsCommand
{
public:
    bool Execute();
};

class KeyCursor: public AbsCommand
{
	public:
		bool Execute();
};

class KeyText: public AbsCommand
{
	public:
		bool Execute();
};

class KeyArrow: public AbsCommand
{
	public:
		bool Execute();
};

class KeyBodyMark: public AbsCommand
{
	public:
		bool Execute();
};

class KeySaveSnap: public AbsCommand
{
	public:
		bool Execute();
};

class KeyReview: public AbsCommand
{
	public:
		bool Execute();
		static void HideMenuReview();
		static void SetDefaultIDAndPath();
		static void SetDemoIDAndPath();
		static void ExitMenuReivew();

//	private:
		static bool m_menuRead;
};

class KeyPIP: public AbsCommand
{
    public:
        bool Execute();
};

class KeyLocalZoom: public AbsCommand
{
	public:
		bool Execute();
        bool ExitLocalZoom();
};

class KeyFocus: public AbsCommand

{
	public:
		bool Execute();
};

class KeyDepth: public AbsCommand
{
	public:
		bool ExecuteAdd(); //ADD
		bool ExecuteSub(); //SUB
        bool Execute();

    private:
        void DepthExe(EKnobOper oper);
};

class KeyFreq: public AbsCommand
{
	public:
		bool Execute(); 
};

class KeyChroma: public AbsCommand
{
	public:
		bool Execute(); 
};

class KeyClearScreen: public AbsCommand
{
	public:
		bool Execute();
		bool UnFreezeExecute(bool update=false);
		bool ModeExecute(bool update=false);

	private:
		void DeleteDataForClearScreen();
};

class KeyEndExam: public AbsCommand
{
	public:
		bool Execute();
};

class KeyCenterLine: public AbsCommand
{
	public:
		bool Execute();
		void Clear();
};

class KeyMenu: public AbsCommand
{
	public:
		bool Execute();
};

class KeyBiopsy: public AbsCommand
{
	public:
		bool Execute();
};

extern int g_count11;

//global function
void ChangeTis();
void DarkFucusLight();
#endif
