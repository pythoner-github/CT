#include <gtk/gtk.h>
#include "display/gui_global.h"
#include "display/gui_func.h"
#include "measure/MenuMeasure.h"
#include "display/MenuTree.h"
#include "measure/MeasureFactory.h"
#include "measure/DrawHistogram.h"
#include "sysMan/MeasureSetting.h"

static void MeasureBasic(int type)
{
    if(type >= BASIC_MEA_START && type <=BASIC_MEA_END)
        MeasureFactory::GetInstance()->Create((EMeasureFactory)type);
}

static void ExpandChildMenu(int type)
{
	g_menuMeasure.ExpandMenu();
}

GUIMenuNew g_measureAllDefaultItem[] ={
	{NULL,D2_DIST_DOT, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
  	{NULL,D2_DIST_LINE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_LEN_TRACK, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_LEN_DOT, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_AREA_TRACK, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_AREA_DOT, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_AREA_ELLIPSE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_AREA_RECTANGLE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
    {NULL,D2_VOLUME_3DIST, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_VOLUME_ELLIPSE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_VOLUME_ELLIPSE_1DIST, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_VOLUME_1DIST, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_ANGLE_DOT, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_ANGLE_LINE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_DEPTH, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,M_TIME, MEA_M, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,M_SLOPE, MEA_M, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,M_VEL, MEA_M, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,M_HR, MEA_M, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,M_DEPTH, MEA_M, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_AUTO_TRACK, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_MANUAL_TRACK, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_VEL, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_RI, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_SD, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_TIME, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_ACCEL, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_PI, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_HR, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_PGMAX, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D_PGMEAN, MEA_D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_ANGUSTY_DIST, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
  	{NULL,D2_RATIO_DIST, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_ANGUSTY_AREA, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_RATIO_AREA, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_RATIO_D_P, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_RATIO_VOL, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_RATIO_ANGLE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_EF, MEA_M, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_PROFILE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_HISTOGRAM, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_IMT, MEA_2D, TEXT_ONLY, true,{ExpandChildMenu}},
	{NULL,D2_IMT_ABOVE, MEA_2D, TEXT_ONLY, true,{MeasureBasic}},
	{NULL,D2_IMT_BELOW, MEA_2D, TEXT_ONLY, true,{MeasureBasic}}
};
MenuMeasure g_menuMeasure;

MenuMeasure::MenuMeasure()
: m_sizeDefaultItems(0), m_examItem("AdultAbdo"), m_scanMode(ScanMode::D2), m_completeMeasureFlag(NULL)
{
	m_sizeDefaultItems = sizeof(g_measureAllDefaultItem) / sizeof(GUIMenuNew);

	int count = 0;
	for (count = 0; count < m_sizeDefaultItems; count++)
	{
		if (g_measureAllDefaultItem[count].pf.ptrNone == ExpandChildMenu)
		{
			g_measureAllDefaultItem[count].child.push_back(g_measureAllDefaultItem[count+1]);
			g_measureAllDefaultItem[count].child.push_back(g_measureAllDefaultItem[count+2]);
		}
		m_vecAllMeasureItem.push_back(g_measureAllDefaultItem[count]);
	}

	UpdateAllMeasureItems();
}

MenuMeasure::~MenuMeasure()
{
	m_vecAllMeasureItem.clear();
	m_vecCurExamItemMeasureItem.clear();
	m_vecCurScanModeMeasureItem.clear();
	if (m_completeMeasureFlag != NULL)
	{
		delete m_completeMeasureFlag;
		m_completeMeasureFlag = NULL;
	}
}

GtkWidget * MenuMeasure::Create()
{
	m_table = gtk_table_new(18, 6, TRUE);
    gtk_table_set_row_spacing(GTK_TABLE(m_table), 0, 5);
	m_scrolledWindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_scrolledWindow), GTK_SHADOW_IN);
    m_treeView = m_treeMenu.Create(m_vecCurScanModeMeasureItem);

    gtk_widget_modify_base(m_treeView, GTK_STATE_NORMAL, g_deep);
    gtk_container_add (GTK_CONTAINER (m_scrolledWindow), m_treeView);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_scrolledWindow, 0, 6, 1, 18);
    gtk_widget_modify_bg(m_scrolledWindow, GTK_STATE_NORMAL, g_deep);

	return m_table;
}

void MenuMeasure::ChangeAllMeasureItems()
{
	UpdateAllMeasureItems();
	UpdateCurrentExamItemMeasureItems(m_examItem);
	UpdateCurrentScanModeMeasureItems(m_scanMode);
	m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeMeasureItem);
}

void MenuMeasure::UpdateAllMeasureItems()
{
	if (m_completeMeasureFlag != NULL)
	{
		delete m_completeMeasureFlag;
		m_completeMeasureFlag = NULL;
	}

	m_completeMeasureFlag = new bool[m_vecAllMeasureItem.size()];
	for (int j = 0; j < (int)m_vecAllMeasureItem.size(); j++)
	{
		m_completeMeasureFlag[j] = false;
	}
}

void MenuMeasure::UpdateLabel(void)
{
    m_treeMenu.Update(m_treeView, m_vecCurScanModeMeasureItem);
}

void MenuMeasure::ClearAllData()
{
	if (m_completeMeasureFlag != NULL)
	{
		for (int j = 0; j < (int)m_vecAllMeasureItem.size(); j++)
		{
			m_completeMeasureFlag[j] = false;
		}
	}
	UpdateCurrentScanModeMeasureItems(m_scanMode);
	m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeMeasureItem);
}

void MenuMeasure::SetCompleteMeasureFlag(int etype)
{
	if (m_completeMeasureFlag != NULL && (int)m_vecAllMeasureItem.size() > etype)

	{
		m_completeMeasureFlag[etype] = true;
	}
}

void MenuMeasure::Hide(void)
{
    gtk_widget_hide_all(m_table);
}

void MenuMeasure::Show(void)
{
    gtk_widget_show_all(m_table);

    EMeasureSequenceType sequence = (EMeasureSequenceType)MeasureSetting::GetInstance()->GetMeasureSequence(m_examItem.c_str());
    if (sequence == MEA_SEQ_REPEAT)
    {
		m_treeMenu.SelectFirst(m_treeView);

    }
}

void MenuMeasure::ChangeExamItem(string item)
{
	m_examItem = item;
	UpdateCurrentExamItemMeasureItems(m_examItem);
	UpdateCurrentScanModeMeasureItems(m_scanMode);

    m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeMeasureItem);
}

void MenuMeasure::ChangeScanMode(ScanMode::EScanMode mode)
{
	bool isChange = false;
	if (GetMeasureMode(mode) != GetMeasureMode(m_scanMode))
		isChange = true;

	m_scanMode = mode;
	if (isChange)
	{
		UpdateCurrentScanModeMeasureItems(m_scanMode);
		m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeMeasureItem);
	}
}

EMeasureMode MenuMeasure::GetMeasureMode(ScanMode::EScanMode mode)
{
	EMeasureMode meaMode;
	switch (mode)
	{
		case ScanMode::D2:
		case ScanMode::M_INIT:
		case ScanMode::PW_INIT:
		case ScanMode::CW_INIT:
		case ScanMode::PWCFM_INIT:
		case ScanMode::PWPDI_INIT:
		case ScanMode::CWCFM_INIT:
		case ScanMode::CWPDI_INIT:
		case ScanMode::CFM_VS_2D:
		case ScanMode::PDI_VS_2D:
		case ScanMode::EFOV:
		case ScanMode::CFM:
		case ScanMode::PDI:
			meaMode = MEA_2D;
			break;
		case ScanMode::M:
		case ScanMode::ANATOMIC_M:
			meaMode = MEA_M;
			break;
		case ScanMode::PW:
		case ScanMode::PW_SIMULT:
		case ScanMode::CW:
		case ScanMode::PWCFM:
		case ScanMode::PWCFM_SIMULT:
		case ScanMode::PWPDI:
		case ScanMode::PWPDI_SIMULT:
		case ScanMode::CWCFM:
		case ScanMode::CWPDI:
			meaMode = MEA_D;
			break;
		default:
			meaMode = MEA_2D;
			break;
	}

	return meaMode;
}

void MenuMeasure::UpdateCurrentExamItemMeasureItems(string item)
{
	m_vecCurExamItemMeasureItem.clear();

	vector<int> vecItemCalc;
	MeasureSetting::GetInstance()->GetMeasureListEtype((char *)item.c_str(), vecItemCalc);

	for (int i = 0; i < (int)vecItemCalc.size(); i++)
	{
		if (vecItemCalc[i] < 0 || vecItemCalc[i] > (int)m_vecAllMeasureItem.size() - 1)
			continue;

		m_vecCurExamItemMeasureItem.push_back(m_vecAllMeasureItem[vecItemCalc[i]]);
    }
}

void MenuMeasure::UpdateCurrentScanModeMeasureItems(ScanMode::EScanMode mode)
{
	EMeasureMode meaMode = GetMeasureMode(mode);

	m_vecCurScanModeMeasureItem.clear();
	for (int i = 0; i < (int)m_vecCurExamItemMeasureItem.size(); i++)
	{
		if (m_vecCurExamItemMeasureItem[i].mode == meaMode)
		{
			m_vecCurScanModeMeasureItem.push_back(m_vecCurExamItemMeasureItem[i]);
			vector<GUIMenuNew>::iterator iter = m_vecCurScanModeMeasureItem.end() - 1;
			UpdateRendererType(iter);
			int size = iter->child.size();
			if (size > 0)
			{
				iter = iter->child.begin();
				for (int s = 0; s < size; s++)
				{
					UpdateRendererType(iter);
					iter++;
				}
			}
		}
	}
}

void MenuMeasure::UpdateRendererType(vector<GUIMenuNew>::iterator iter)
{
	if (m_completeMeasureFlag != NULL && (int)m_vecAllMeasureItem.size() > (iter->etype))
	{
		if (m_completeMeasureFlag[iter->etype])
		{
			iter->type = TOGGLE;
		}
	}
}

void MenuMeasure::ExpandMenu()
{
	m_treeMenu.ExpandMenu(m_treeView);
}

void MenuMeasure::SelectNext()
{
	EMeasureSequenceType sequence = (EMeasureSequenceType)MeasureSetting::GetInstance()->GetMeasureSequence(m_examItem.c_str());
	if (sequence != MEA_SEQ_REPEAT)
	{
		MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
		m_treeMenu.SelectNext(m_treeView, sequence);
	}
}

void MenuMeasure::CompleteCurrentMeasure()
{
	int parentEtype;
	int etype = m_treeMenu.SetCompleteMeasureFlag(m_treeView, parentEtype);
	SetCompleteMeasureFlag(etype);

	if (parentEtype != -1)
		SetCompleteMeasureFlag(parentEtype);
}