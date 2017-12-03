/**
* @brief MenuCalcNew.h: The file is interface for calc menu creating.
*
* @version: 1.0
* @author: hehao
* @date: 2014-11-28
*/

#ifndef __MENUCALC_NEW_H__
#define __MENUCALC_NEW_H__

#include "MenuTree.h"
#include "ExamItem.h"
#include "ScanMode.h"
#include "MeasureDef.h"

extern SingleItemInfo userDef_new[100];

class MenuCalcNew
{
	public:
		MenuCalcNew(void);
		virtual ~MenuCalcNew(void);

		GtkWidget * Create(void);
		void UpdateLabel(void);
		void Show(void);
		void Hide(void);
        int GetCurrentFetal(void) { return m_fetalOrder; }
        void ResetFetalOrder(void) { m_fetalOrder = 0; }
        void ChangeExamItem(string item);
        void ChangeScanMode(ScanMode::EScanMode mode);
        void ChangeAllCalcItems(void);
        void ExpandMenu(void);
        void SelectNext(void);
        void ClearAllData(void); //When new patient, must call the function.
        void SetCompleteMeasureFlag(int etype);
        void CompleteCurrentMeasure();
        string GetExamItem(void) { return m_examItem; }
        void UpdateEfwItem(int efw);
        bool IsFlagExist(void);
        void ClearAllFlag(void);
    private:
        void UpdateCurrentExamItemCalcItems(string item);
        void UpdateCurrentScanModeCalcItems(ScanMode::EScanMode mode);
        void UpdateAllCalcItems(void);
        void UpdateRendererType(vector<GUIMenuNew>::iterator iter);
        bool IsOBItem(int etype);

        bool ShowMultiplyFetal(void);
		EMeasureMode GetMeasureMode(ScanMode::EScanMode mode);
		EMeasureMode GetMeasureModeFromMeasureType(int measureType);
		int GetUnitItemFromMeasureType(int measureType);
		void ClearUserDefineCalcItem();
		void ClearUserDefineSingleItemInfo(void);
		void Fetal1Pressed(GtkButton *widget);
		void Fetal2Pressed(GtkButton *widget);
		static void HandleFetal1Pressed(GtkButton *widget, MenuCalcNew *data)
		{ data->Fetal1Pressed(widget); }
		static void HandleFetal2Pressed(GtkButton *widget, MenuCalcNew *data)
		{ data->Fetal2Pressed(widget); }

		vector<GUIMenuNew> m_vecAllCalcItem; 
		vector<GUIMenuNew> m_vecCurExamItemCalcItem;
		vector<GUIMenuNew> m_vecCurScanModeCalcItem;
		int m_sizeDefaultItems;
		GtkWidget * m_table;
		TreeMenu m_treeMenu;
		GtkWidget *m_scrolledWindow;
		GtkWidget *m_treeView;
		GtkWidget *m_buttonFetal1;
		GtkWidget *m_buttonFetal2;
		static int m_fetalOrder;
		//ExamItem::EItem m_examItem;
		string m_examItem;	//name of exam item.
		ScanMode::EScanMode m_scanMode;
		bool *m_completeMeasureFlag;
		bool m_isExistedOBItem;
};

extern MenuCalcNew g_menuCalc;
#endif //__MENUCALC_NEW_H__
