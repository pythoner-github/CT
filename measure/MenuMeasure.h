// -*- c++ -*-
#ifndef MENU_MEASURE_H
#define MENU_MEASURE_H

#include "MenuTree.h"
#include "ExamItem.h"
#include "ScanMode.h"
#include "MeasureDef.h"
class MenuMeasure {
public:
    MenuMeasure(void);
    virtual  ~MenuMeasure();
    GtkWidget* Create();
    void UpdateLabel(void);
    void Hide(void);
    void Show(void);
    void ChangeExamItem(string item);
    void ChangeScanMode(ScanMode::EScanMode mode);
    void ChangeAllMeasureItems(void);
    void ExpandMenu(void);
    void SelectNext(void);
    void ClearAllData(void); //When new patient, must call the function.
    void SetCompleteMeasureFlag(int etype);
    void CompleteCurrentMeasure();
    string GetExamItem(void) { return m_examItem; }


private:
    void UpdateCurrentExamItemMeasureItems(string item);
    void UpdateCurrentScanModeMeasureItems(ScanMode::EScanMode mode);
    void UpdateAllMeasureItems(void);
    void UpdateRendererType(vector<GUIMenuNew>::iterator iter);
    EMeasureMode GetMeasureMode(ScanMode::EScanMode mode);
    EMeasureMode GetMeasureModeFromMeasureType(int measureType);
    int GetUnitItemFromMeasureType(int measureType);

    vector<GUIMenuNew> m_vecAllMeasureItem; 
    vector<GUIMenuNew> m_vecCurExamItemMeasureItem;
    vector<GUIMenuNew> m_vecCurScanModeMeasureItem;
    int m_sizeDefaultItems;
    GtkWidget *m_table;
    GtkWidget *m_scrolledWindow;
    TreeMenu m_treeMenu;
    GtkWidget *m_treeView;
    string m_examItem;	//name of exam item.
    ScanMode::EScanMode m_scanMode;
    bool *m_completeMeasureFlag;

};

extern MenuMeasure g_menuMeasure;

#endif
