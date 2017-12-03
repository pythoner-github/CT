#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "bodymark/MenuBDMK.h"
#include "Def.h"
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "bodymark/BodyMark.h"
#include "bodymark/MultiFuncBodyMark.h"
#include "probe/ExamItem.h"
#include "probe/ProbeSelect.h"

#ifdef VET
	#define BDMK_PATH "res/bdmk_vet"
	static const char* examItem[] = {N_("Animal"), N_("Part")};
#else
#define BDMK_PATH "res/bdmk_scaled"

#if (defined(EMP_322) || defined(EMP_313))
static const char* examItem[] = {N_("Abdomen"), N_("Obstetrics"), N_("Gynecology"), N_("Urology"), N_("Cardiac"), N_("Small Part"), N_("Orthopedic"), N_("EXTRA")};
#else
static const char* examItem[] = {N_("Abdomen"), N_("Obstetrics"), N_("Gynecology"), N_("Urology"), N_("Cardiac"), N_("Small Part"), N_("Vascular"), N_("Orthopedic"), N_("EXTRA")};
#endif

#endif
using std::string;

MenuBDMK g_menuBDMK;

MenuBDMK::MenuBDMK(void)
{
	m_fixed = NULL;
	m_combobox = NULL;
	m_vecName.clear();
	m_study = 0;
	m_numTotal = 0;
	m_pageCur = 0;
	m_dir = NULL;
}

void MenuBDMK::Hide(void)
{
    if(m_dir)
        g_dir_close(m_dir);
    m_dir = NULL;

    gtk_combo_box_popdown(GTK_COMBO_BOX(m_combobox));
    gtk_widget_hide_all(m_fixed);
}

void MenuBDMK::Show(void)
{
    gtk_widget_show_all(m_fixed);

	//根据不同的探头部位设置不同科别
	guchar study;
	ExamItem::EItem item = ProbeSelect::GetItemIndex();
//	PRINTF("item = %d\n", (int)item);

#ifdef VET

    study = ANIMAL;
#else
    switch(item)
	{
		case ExamItem::ABDO_ADULT:
		case ExamItem::ABDO_LIVER:
		case ExamItem::ABDO_KID:
		case ExamItem::USER1:
		case ExamItem::USER2:
		case ExamItem::USER3:
		case ExamItem::USER4:
			study = ABD;
			break;
		case ExamItem::EARLY_PREG:
		case ExamItem::LATER_PREG:
			study = OB;
			break;
		case ExamItem::GYN:
			study = GYN;
			break;
		case ExamItem::KIDNEY:
		case ExamItem::BLADDER:
			study = UR;
			break;
		case ExamItem::CAR_KID:
		case ExamItem::CAR_ADULT:
		case ExamItem::CAR_FETUS:
			study = CAR;
			break;
		case ExamItem::SMALL_PART:
		case ExamItem::GLANDS:
		case ExamItem::THYROID:
		case ExamItem::EYE:
#ifdef EMP_355
		case ExamItem::MSK:
#endif
            study = SP;
            break;
#if (defined(EMP_322) || defined(EMP_313))
        case ExamItem::HIP_JOINT:
        case ExamItem::JOINT_CAVITY:
        case ExamItem::MENISCUS:
        case ExamItem::SPINE:

            study = ORT;
            break;
#else
        case ExamItem::CAROTID:
        case ExamItem::JUGULAR:
        case ExamItem::PERI_ARTERY:
        case ExamItem::PERI_VEIN:
            study = VES;
            break;
        case ExamItem::HIP_JOINT:
        case ExamItem::JOINT_CAVITY:
        case ExamItem::MENISCUS:
        case ExamItem::SPINE:

            study = ORT;
            break;
#endif
	//	case ExamItem::TCD:
		default:
			study = ABD;
			break;
	}
#endif
    ChangeBDMKStudy(study);
    gtk_combo_box_set_active (GTK_COMBO_BOX (m_combobox), study);
}

void MenuBDMK::UpdateLabel(void)
{
	GtkTreeIter iter;
	GtkListStore *store = gtk_list_store_new (1, G_TYPE_STRING);
	for(unsigned int i=0; i<G_N_ELEMENTS(examItem); i++)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, _(examItem[i]), -1);
	}
	gtk_combo_box_set_model(GTK_COMBO_BOX(m_combobox), GTK_TREE_MODEL(store));
    g_object_unref (store);

    if(m_labelHide != NULL)
        gtk_label_set_text(GTK_LABEL(m_labelHide), _("Hide BodyMark"));
}

GtkWidget* MenuBDMK::Create(void)
{
    m_fixed = gtk_fixed_new ();
    gtk_widget_set_size_request (m_fixed, MENU_AREA_W, MENU_AREA_H);

	GtkTreeIter iter;
	GtkListStore *store = gtk_list_store_new (1, G_TYPE_STRING);
	for(unsigned int i=0; i<G_N_ELEMENTS(examItem); i++)
	{
		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter, 0, _(examItem[i]), -1);
	}
	m_combobox = create_combobox (120, 30, "text", GTK_TREE_MODEL(store));
    gtk_fixed_put (GTK_FIXED (m_fixed), m_combobox, 30, 10);
	gtk_widget_show (m_combobox);
	g_signal_connect(G_OBJECT(m_combobox), "changed", G_CALLBACK(HandleStudyChanged), this);

	GtkWidget *table = gtk_table_new(MAX_ROWS, MAX_IMAGE/MAX_ROWS, TRUE);
    gtk_widget_set_size_request (table, MENU_AREA_W-20, 445); //if MAX_ROWS=5, height=445, if MAX_ROWS=6, height=520
    gtk_fixed_put (GTK_FIXED (m_fixed), table, 10, 45);
	gtk_container_set_border_width (GTK_CONTAINER (table), 2);
	gtk_table_set_row_spacings (GTK_TABLE (table), 10);
	gtk_table_set_col_spacings (GTK_TABLE (table), 10);
	gtk_widget_show (table);

	int k = MAX_IMAGE/MAX_ROWS;
	for(int i=0; i<MAX_ROWS; i++)
	{
		for(int j=0; j<k; j++)
		{
			m_btn[j+i*k] = gtk_button_new();
			gtk_table_attach_defaults(GTK_TABLE(table), m_btn[j+i*k], j, j+1, i, i+1);
			g_signal_connect(G_OBJECT(m_btn[j+i*k]), "clicked", G_CALLBACK(HandleBDMKClicked), this);
			g_object_set_data(G_OBJECT(m_btn[j+i*k]), "SNO", GINT_TO_POINTER(j+i*k));
		}
	}

	/*m_labelHide = create_label(_("Hide BodyMark"), 0, 0, g_white, NULL);
	GtkWidget *btnHide = create_button(m_labelHide, 120, 30, g_deep);
    gtk_fixed_put (GTK_FIXED (m_fixed), btnHide, 30, 495);
	g_signal_connect(G_OBJECT(btnHide), "clicked", G_CALLBACK(HandleHideClicked), this);
*/
	GtkWidget *hbox = gtk_hbox_new (TRUE, 0);
    gtk_fixed_put (GTK_FIXED (m_fixed), hbox, 0, 535);
    gtk_widget_set_size_request (hbox, MENU_AREA_W, 30);

	GtkWidget *labelLeft = create_label("◀", 0, 0, g_white, NULL);
	m_btnLeft = create_button(labelLeft, 60, 0, g_deep);
	gtk_box_pack_start (GTK_BOX (hbox), m_btnLeft, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnLeft), "clicked", G_CALLBACK(HandleLeftClicked), this);
	gtk_widget_modify_bg(m_btnLeft, GTK_STATE_INSENSITIVE, g_deep);

	m_labelPage = create_label("[ 0 / 0 ]", 0, 0, g_black, NULL);
	gtk_box_pack_start (GTK_BOX (hbox), m_labelPage, TRUE, TRUE, 0);

	GtkWidget *labelRight = create_label("▶", 0, 0, g_white, NULL);
	m_btnRight = create_button(labelRight, 60, 0, g_deep);
	gtk_box_pack_start (GTK_BOX (hbox), m_btnRight, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(m_btnRight), "clicked", G_CALLBACK(HandleRightClicked), this);
	gtk_widget_modify_bg(m_btnRight, GTK_STATE_INSENSITIVE, g_deep);

    return m_fixed;
}

void MenuBDMK::SetPageValue(guint page, guint total_page)
{
	char str[20];

	sprintf(str, "[ %d / %d ]", page, total_page);
	gtk_label_set_text(GTK_LABEL(m_labelPage), str);

	if(page <= 1 && total_page <= 1)
	{
		gtk_button_set_label(GTK_BUTTON(m_btnLeft), "");
		gtk_button_set_label(GTK_BUTTON(m_btnRight), "");
		gtk_widget_set_sensitive (m_btnLeft, FALSE);
		gtk_widget_set_sensitive (m_btnRight, FALSE);
	}
	else if(page == 1 && total_page > 1)
	{
		gtk_button_set_label(GTK_BUTTON(m_btnLeft), "");
		gtk_button_set_label(GTK_BUTTON(m_btnRight), "▶");
		gtk_widget_set_sensitive (m_btnLeft, FALSE);
		gtk_widget_set_sensitive (m_btnRight, TRUE);
	}
	else if(page > 1 && total_page > page)
	{
		gtk_button_set_label(GTK_BUTTON(m_btnLeft), "◀");
		gtk_button_set_label(GTK_BUTTON(m_btnRight), "▶");
		gtk_widget_set_sensitive (m_btnLeft, TRUE);
		gtk_widget_set_sensitive (m_btnRight, TRUE);
	}
	else if(page > 1 && total_page == page)
	{
		gtk_button_set_label(GTK_BUTTON(m_btnLeft), "◀");
		gtk_button_set_label(GTK_BUTTON(m_btnRight), "");
		gtk_widget_set_sensitive (m_btnLeft, TRUE);
		gtk_widget_set_sensitive (m_btnRight, FALSE);
	}
}

void MenuBDMK::HideAllButton()
{
	int i;
	for(i=0; i<MAX_IMAGE; i++)
		gtk_widget_hide(m_btn[i]);
}

gint MenuBDMK::GetFileNum(const char *file)
{
	int i;
	char num[10];
	int len = strlen(file);

	memset(num, 0, strlen(num));
	for(i=0; i<len; i++)
	{
		if(file[i] == '.')
			break;
		num[i] = file[i];
	}
	num[i+1] = '\0';

	return atoi(num);
}

bool MenuBDMK::Sort(const string s1, const string s2)
{
	return atof(s1.c_str()) < atof(s2.c_str());
}

gint MenuBDMK::SortFileName(gconstpointer p1, gconstpointer p2)
{
	int a, b;

	a = GetFileNum((const char*)p1);
	b = GetFileNum((const char*)p2);

	return (a > b ? +1 : a == b ? 0 : -1);
}

void MenuBDMK::UpdateImage(const char* path, int page)
{
	int i, j;
	int nth, size;
	char *filepath;
	GtkWidget *image;

	size = m_vecName.size();
	nth = (page-1) * MAX_IMAGE;
	for(i=nth, j=0; i<size; i++, j++)
	{
		if(j >= MAX_IMAGE)
			break;
		filepath = g_build_filename(path, m_vecName[i].c_str(), NULL);
		if(!g_file_test(filepath, G_FILE_TEST_IS_DIR))
		{
			image = gtk_image_new();
			gtk_widget_show(image);
			gtk_image_set_from_file(GTK_IMAGE(image), filepath);
			gtk_button_set_image(GTK_BUTTON(m_btn[j]), image);
			gtk_widget_show(m_btn[j]);
		}
		g_free(filepath);
	}
}

void MenuBDMK::ChangeBDMKStudy(guchar study)
{
	gchar *path;
	string str;
	const gchar *name;

	//clear the list
	m_vecName.clear();

	// hide all image displayed
	HideAllButton();

	//read filename

#ifdef VET

    switch(study)

    {
        case ANIMAL:	path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;
        case PART:	    path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Part", NULL);	    break;
        default:
                        path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;

    }
#else
	switch(study)
	{
		case ABD:	path = GetBdmkPath("ABD");	break;
		case OB:	path = GetBdmkPath("OB");	break;
		case GYN:	path = GetBdmkPath("GYN");	break;
		case UR:	path = GetBdmkPath("UR");	break;
		case CAR:	path = GetBdmkPath("CAR");	break;
		case SP:	path = GetBdmkPath("SP");	break;
#if (defined(EMP_322) || defined(EMP_313))
		case ORT:	path = GetBdmkPath("ORT");	break;
		case EXTRA:	path = GetBdmkPath("EXTRA");break;
#else
		case VES:	path = GetBdmkPath("VES");	break;
		case ORT:	path = GetBdmkPath("ORT");	break;
		case EXTRA:	path = GetBdmkPath("EXTRA");break;
#endif
		default:
					path = GetBdmkPath("ABD");
	}
#endif
	m_study = study;
	PRINTF("%s: BDMK path %s\n", __FUNCTION__, path);
	m_dir = g_dir_open(path, 0, NULL);
	if(!m_dir)
	{
		PRINTF("Error: Cannot open dir '%s'\n", path);
		g_free(path);
		return;
	}

	name = g_dir_read_name(m_dir);
	if(!name)
	{
		PRINTF("Error: The dir '%s' is empty!\n", path);
		g_free(path);
		return;
	}
	while(name != NULL)
	{
		/* Ignore hidden files that start with a '.' */
		if(name[0] != '.')
		{
			str = name;
			m_vecName.push_back(str);
		}
		name = g_dir_read_name(m_dir);
	}
	g_dir_close(m_dir);
	m_dir = NULL;

	m_pageCur = 1;
	m_numTotal = m_vecName.size();
	SetPageValue(m_pageCur, (m_numTotal/MAX_IMAGE)+1);

	//sort list by filename
	sort(m_vecName.begin(), m_vecName.end(), Sort);

	UpdateImage(path, m_pageCur);
	g_free(path);
}

void MenuBDMK::StudyChanged(GtkComboBox *combobox)
{
	guchar study;
	gchar *str = gtk_combo_box_get_active_text(combobox);
#ifdef VET
    if(!strcmp(str, _(examItem[0])))  study = ANIMAL;
	else if(!strcmp(str, _(examItem[1])))	study = PART;
#else

	if(!strcmp(str, _(examItem[0])))  study = ABD;
	else if(!strcmp(str, _(examItem[1])))	study = OB;
	else if(!strcmp(str, _(examItem[2])))	study = GYN;
	else if(!strcmp(str, _(examItem[3])))	study = UR;
	else if(!strcmp(str, _(examItem[4])))	study = CAR;
	else if(!strcmp(str, _(examItem[5])))	study = SP;
#if (defined(EMP_322)|| defined(EMP_313))
	else if(!strcmp(str, _(examItem[6])))	study = ORT;
	else if(!strcmp(str, _(examItem[7])))	study = EXTRA;
#else
	else if(!strcmp(str, _(examItem[6])))	study = VES;
	else if(!strcmp(str, _(examItem[7])))	study = ORT;
	else if(!strcmp(str, _(examItem[8])))	study = EXTRA;
#endif
    else	study = ABD;
#endif

	ChangeBDMKStudy(study);
}

char* MenuBDMK::GetBdmkPath(const char *item)
{
	char *path;
	path = g_build_filename(CFG_RES_PATH, BDMK_PATH, item, NULL);
	if(g_file_test(path, G_FILE_TEST_IS_DIR))
	{
		return path;
	}
	else
	{
		GString *strUp = g_string_new(item);
		GString *strDown = g_string_ascii_down(strUp); //After called g_string_ascii_down(), strUp==strDown, so just must free one
		path = g_build_filename(CFG_RES_PATH, BDMK_PATH, strDown->str, NULL);
		g_string_free(strDown, TRUE);
		return path;
	}
}

void MenuBDMK::BDMKClicked(GtkButton *button)
{
	char *path;
	char *abspath;
	int no = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "SNO"));
#ifdef VET
    switch(m_study)
    {

        case ANIMAL:	path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;

        case PART:	    path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Part", NULL);	    break;

        default:
                        path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;
    }
#else
    switch(m_study)
	{
		case ABD:	path = GetBdmkPath("ABD");	break;
		case OB:	path = GetBdmkPath("OB");	break;
		case GYN:	path = GetBdmkPath("GYN");	break;
		case UR:	path = GetBdmkPath("UR");	break;
		case CAR:	path = GetBdmkPath("CAR");	break;
		case SP:	path = GetBdmkPath("SP");	break;
#if (defined(EMP_322) || defined(EMP_313))
		case ORT:	path = GetBdmkPath("ORT");	break;
        case EXTRA:	path = GetBdmkPath("EXTRA");break;
#else
        case VES:	path = GetBdmkPath("VES");	break;
        case ORT:	path = GetBdmkPath("ORT");	break;
        case EXTRA:	path = GetBdmkPath("EXTRA");break;
#endif
        default:
                    path = GetBdmkPath("ABD");
	}

#endif

	abspath = g_build_filename(path, m_vecName[no+(m_pageCur-1)*MAX_IMAGE].c_str(), NULL);
	if(!g_file_test(abspath, G_FILE_TEST_IS_DIR))
	{
		m_pixbufSel = gdk_pixbuf_new_from_file(abspath, NULL);

		//add code to draw bdmk to image area
		MultiFuncBodyMark::SetNewBodyMark(m_pixbufSel);
	}
#if 0
	//GdkPixbuf *pixbuf;
	image = gtk_button_get_image(button);
	if(gtk_image_get_storage_type(GTK_IMAGE(image)) != GTK_IMAGE_EMPTY)
	{
		pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));	//default pixbuf

		//add code to draw bdmk to image area
		BodyMark::SetBodyMark(pixbuf);
		MultiFuncBodyMark::BeginMoveProbeMark();
	}
#endif
	g_free(path);
	g_free(abspath);
}

void MenuBDMK::HideClicked(GtkButton *button)
{
	BodyMark::HideBodyMark();
}

void MenuBDMK::LeftClicked(GtkButton *button)
{
	m_pageCur --;
	SetPageValue(m_pageCur, (m_numTotal/MAX_IMAGE)+1);

	gchar *path;
#ifdef VET
    switch(m_study)
    {
        case ANIMAL:	path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;
        case PART:	    path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Part", NULL);	    break;
        default:
                        path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;
    }
#else
	switch(m_study)
	{
		case ABD:	path = GetBdmkPath("ABD");	break;
		case OB:	path = GetBdmkPath("OB");	break;
		case GYN:	path = GetBdmkPath("GYN");	break;
		case UR:	path = GetBdmkPath("UR");	break;
		case CAR:	path = GetBdmkPath("CAR");	break;
		case SP:	path = GetBdmkPath("SP");	break;
#if (defined(EMP_322) || defined(EMP_313))
		case ORT:	path = GetBdmkPath("ORT");	break;
        case EXTRA:	path = GetBdmkPath("EXTRA");break;
#else
        case VES:	path = GetBdmkPath("VES");	break;
        case ORT:	path = GetBdmkPath("ORT");	break;
        case EXTRA:	path = GetBdmkPath("EXTRA");break;
#endif
        default:
                    path = GetBdmkPath("ABD");
    }
#endif
	HideAllButton();
	UpdateImage(path, m_pageCur);
	g_free(path);
}

void MenuBDMK::RightClicked(GtkButton *button)
{
	m_pageCur ++;
	SetPageValue(m_pageCur, (m_numTotal/MAX_IMAGE)+1);

	gchar *path;
#ifdef VET
    switch(m_study)
    {
        case ANIMAL:	path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;
        case PART:	    path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Part", NULL);	    break;
        default:
                        path = g_build_filename(CFG_RES_PATH, BDMK_PATH, "Animal", NULL);	break;
    }

#else
	switch(m_study)
	{
		case ABD:	path = GetBdmkPath("ABD");	break;
		case OB:	path = GetBdmkPath("OB");	break;
		case GYN:	path = GetBdmkPath("GYN");	break;
		case UR:	path = GetBdmkPath("UR");	break;
		case CAR:	path = GetBdmkPath("CAR");	break;
		case SP:	path = GetBdmkPath("SP");	break;
#if (defined(EMP_322) || defined(EMP_313))
		case ORT:	path = GetBdmkPath("ORT");	break;
		case EXTRA:	path = GetBdmkPath("EXTRA");break;
#else
        case VES:	path = GetBdmkPath("VES");	break;
        case ORT:	path = GetBdmkPath("ORT");	break;
        case EXTRA:	path = GetBdmkPath("EXTRA");break;
#endif
		default:
					path = GetBdmkPath("ABD");
	}
#endif
	HideAllButton();
	UpdateImage(path, m_pageCur);
	g_free(path);
}