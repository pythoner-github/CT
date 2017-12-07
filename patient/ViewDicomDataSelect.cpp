#include <dirent.h>
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "patient/ViewDicomDataSelect.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyDef.h"
#include "patient/FileMan.h"
#include "periDevice/DCMMan.h"
#include "patient/ViewQueryRetrieve.h"
#include "display/ViewDialog.h"
#include "patient/PatientInfo.h"
#include "calcPeople/ViewReport.h"
#include "display/gui_global.h"
#include <glib.h>
#include <glib/gstdio.h>
#include "display/gui_func.h"
#include "periDevice/PeripheralMan.h"
#include "patient/ImgMan.h"
#include "patient/Database.h"
#include "patient/VideoMan.h"
#include "sysMan/SysOptions.h"
#include "periDevice/ViewPrintPreview.h"
#include "imageProc/Replay.h"

ViewDicomDataSelect* ViewDicomDataSelect::m_ptrInstance = NULL;

PatientInfo ViewDicomDataSelect::m_patientInfo;
ViewDicomDataSelect::ViewDicomDataSelect()
{
    m_listBranch = NULL;
    m_studyNo = 0;
}

ViewDicomDataSelect::~ViewDicomDataSelect()
{
    if (m_ptrInstance != NULL)
    delete m_ptrInstance;
}

ViewDicomDataSelect* ViewDicomDataSelect::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewDicomDataSelect;

    return m_ptrInstance;
}
#ifndef VET
void ViewDicomDataSelect::CreateWindow(GtkWindow *parent)
{
    GtkWidget *fixed;
    GtkWidget *dataSrc;
    GtkWidget *dataDe;
    GtkTreeModel *modelSrc;

    GtkWidget *label_list;
    GtkWidget *label_quality;
    GtkWidget *label_cancel;
    GtkWidget *image_cancel;
    GtkWidget *button_cancel;
    GtkWidget *label_import;
    GtkWidget *image_import;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, 640, 480);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Browse DICOMDIR"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), parent);
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (m_window), fixed);

    label_list = gtk_label_new (_("<b>DICOMDIR List</b>"));
    gtk_widget_show (label_list);
    gtk_fixed_put (GTK_FIXED (fixed), label_list, 30, 10);
    gtk_label_set_use_markup (GTK_LABEL (label_list), TRUE);
    gtk_widget_set_size_request (label_list, 150, 30);
    gtk_misc_set_alignment (GTK_MISC (label_list), 0, 0.5);

    dataSrc = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (dataSrc);
    gtk_fixed_put (GTK_FIXED (fixed), dataSrc, 20+10, 10+30);
    gtk_widget_set_size_request (dataSrc, 200+45, 300+20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (dataSrc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (dataSrc), GTK_SHADOW_IN);

    GtkTreeModel *modelRoot = create_root_model();
    m_treeSrc = gtk_tree_view_new_with_model (modelRoot);
    g_object_unref (modelRoot);
    add_column(GTK_TREE_VIEW(m_treeSrc));
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (m_treeSrc), FALSE);
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (m_treeSrc), FALSE);
    gtk_container_add (GTK_CONTAINER (dataSrc), m_treeSrc);

    label_quality = gtk_label_new (_("<b>Property Description</b>"));
    gtk_widget_show (label_quality);
    gtk_fixed_put (GTK_FIXED (fixed), label_quality, 300, 10);
    gtk_label_set_use_markup (GTK_LABEL (label_quality), TRUE);
    gtk_widget_set_size_request (label_quality, 150, 30);
    gtk_misc_set_alignment (GTK_MISC (label_quality), 0, 0.5);

    dataDe = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (dataDe);
    gtk_fixed_put (GTK_FIXED (fixed), dataDe, 300, 10+30);
    gtk_widget_set_size_request (dataDe, 250+60, 300+20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (dataDe), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (dataDe), GTK_SHADOW_IN);

    m_treeDe = create_treeview_list();
    gtk_widget_show (m_treeDe);
    gtk_container_add (GTK_CONTAINER (dataDe), m_treeDe);

    label_import = gtk_label_new_with_mnemonic (_("Import"));
    image_import= gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
    m_button_import = create_button_icon(label_import, image_import);
    gtk_fixed_put (GTK_FIXED (fixed), m_button_import, 250+40, 410);
    //gtk_widget_set_size_request (m_button_import, 100, 35);
    g_signal_connect(m_button_import, "clicked", G_CALLBACK(HandleButtonImportClicked), this);

    label_cancel = gtk_label_new_with_mnemonic (_("Exit"));
    image_cancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    button_cancel = create_button_icon(label_cancel, image_cancel);
    gtk_fixed_put (GTK_FIXED (fixed), button_cancel, 320+130, 410);
    g_signal_connect(button_cancel, "clicked", G_CALLBACK(HandleButtonCancelClicked), this);

    UpdateDicomDirList();
    gtk_widget_show_all(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

}

void ViewDicomDataSelect::UpdateDicomDirList(void)
{
    string destDirStorageMedia = UDISK_PATH;
    EDCMReturnStatus status = CDCMMan::GetMe()->BrowseDICOMDIR(destDirStorageMedia,vecStudyLevel);
    if(status == DCMSUCCESS)
    {
        int size = vecStudyLevel.size();
        if(size == 0)
        {
            gtk_widget_set_sensitive(m_button_import,false);
            return ;
        }
        else
            gtk_widget_set_sensitive(m_button_import,true);

        GtkTreeModel *model;
        GtkTreeIter iter;

        model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeSrc));
        gtk_tree_store_clear(GTK_TREE_STORE(model));

        for(int i=0;i<size;i++)
        {
            gtk_tree_store_append(GTK_TREE_STORE(model), &iter,NULL);
            gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
                    0, vecStudyLevel[i].levelName.c_str(),
                    1,i,
                    -1);
            if(vecStudyLevel[i].vecSeriesLevel.size()>0)
            {
                for(int j=0;j<vecStudyLevel[i].vecSeriesLevel.size();j++)
                {

                    GtkTreeIter iter_branch0;
                    gtk_tree_store_append(GTK_TREE_STORE(model),&iter_branch0,&iter);
                    gtk_tree_store_set(GTK_TREE_STORE(model),&iter_branch0,
                            0, vecStudyLevel[i].vecSeriesLevel[j].levelName.c_str(),
                            1,j,
                            -1);
                    if(vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel.size()>0)
                    {
                        for(int k=0;k<vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel.size();k++)
                        {
                            GtkTreeIter iter_branch1;
                            gtk_tree_store_append(GTK_TREE_STORE(model),&iter_branch1,&iter_branch0);
                            gtk_tree_store_set(GTK_TREE_STORE(model),&iter_branch1,
                                    0,vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel[k].levelName.c_str(),
                                    1,k,
                                    -1);
                            gtk_tree_model_iter_next(model, &iter_branch1);
                        }
                    }
                    gtk_tree_model_iter_next(model, &iter_branch0);
                }
            }
            gtk_tree_model_iter_next(model, &iter);
        }
    }
}

void ViewDicomDataSelect::add_column(GtkTreeView *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
    g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

GtkTreeModel* ViewDicomDataSelect::create_root_model()
{
    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(2, G_TYPE_STRING,G_TYPE_INT);
#if 0
    string destDirStorageMedia = UDISK_PATH;
    EDCMReturnStatus status = CDCMMan::GetMe()->BrowseDICOMDIR(destDirStorageMedia,vecStudyLevel);
    int size = vecStudyLevel.size();
    if(size == 0)
        return NULL;
    for(int i=0;i<size;i++)
    {
        gtk_tree_store_append(store, &iter,NULL);
        gtk_tree_store_set(store, &iter,
                0, vecStudyLevel[i].levelName.c_str(),
                1,vecStudyLevel[i].index,
                -1);
#if 0
        if(vecStudyLevel[i].vecSeriesLevel.size()>0)
        {
            for(int j=0;j<vecStudyLevel[i].vecSeriesLevel.size();j++)
            {
                GtkTreeIter iter_branch0;
                gtk_tree_store_append(store,&iter_branch0,&iter);
                gtk_tree_store_set(store,&iter_branch0,
                        0, vecStudyLevel[i].vecSeriesLevel[j].levelName,
                        1,vecStudyLevel[i].vecSeriesLevel[j].index,
                        -1);
                if(vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel.size()>0)
                {
                    for(int k=0;k<vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel.size();k++)
                    {
                        GtkTreeIter iter_branch1;
                        gtk_tree_store_append(store,&iter_branch1,&iter_branch0);
                        gtk_tree_store_set(store,&iter_branch1,
                                0,vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel[k].levelName,
                                1,vecStudyLevel[i].vecSeriesLevel[j].vecFileLevel[k].index,
                                -1);
                    }
                }
            }
        }
#endif
        gtk_tree_model_iter_next(model, &iter);
    }
#endif
    return GTK_TREE_MODEL (store);
}

GtkWidget* ViewDicomDataSelect::create_treeview_list()
{
    GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Property"), renderer, "text", COL_QUALITY, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 150, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Value"), renderer, "text", COL_VALUE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 450, NULL);

    model = create_model_list();
    if (model != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model);

    return treeview;
}

GtkTreeModel* ViewDicomDataSelect::create_model_list()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COL,
            G_TYPE_STRING,
            G_TYPE_STRING);

    return GTK_TREE_MODEL (store);
}

void ViewDicomDataSelect::TreeSelectionChanged(GtkTreeSelection *selection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreePath *path;
    int index;
    int depth;

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
        return ;
    gtk_tree_selection_select_iter(selection,&iter);
    gtk_tree_model_get (model, &iter, 1, &index, -1);
    path = gtk_tree_model_get_path(model,&iter);
    depth = gtk_tree_path_get_depth(path);
    switch(depth)
    {
        case 1:
            {
                gtk_tree_model_get (model, &iter, 1, &index, -1);
                m_studyIndex = index;
                InsertInfo1(index);
            break;
            }
        case 2:
            {
                gtk_tree_model_get (model, &iter, 1, &index, -1);
                int index0 = index;
                gtk_tree_path_up(path);
                gtk_tree_model_get_iter(model,&iter,path);
                gtk_tree_model_get (model, &iter, 1, &index, -1);
                int index1 = index;
                m_studyIndex = index1;
                InsertInfo2(index1,index0);
                break;
            }
        case 3:
            {
                gtk_tree_model_get (model, &iter, 1, &index, -1);
                m_studyIndex = index;
                int index0 = index;
                gtk_tree_path_up(path);
                gtk_tree_model_get_iter(model,&iter,path);
                gtk_tree_model_get (model, &iter, 1, &index, -1);
                int index1 = index;
                gtk_tree_path_up(path);
                gtk_tree_model_get_iter(model,&iter,path);
                gtk_tree_model_get (model, &iter, 1, &index, -1);
                int index2 = index;
                m_studyIndex = index2;
                InsertInfo3(index2,index1,index0);
                break;
            }
    }
}
void ViewDicomDataSelect::InsertInfo1(int index)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    string patientName;
    ViewQueryRetrieve::ChangePersonNameFormatForShow(vecStudyLevel[index].patientName, patientName);

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeDe));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Patient Name"),
            COL_VALUE,patientName.c_str(),
            -1);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Patient ID"),
            COL_VALUE,vecStudyLevel[index].patientID.c_str(),
            -1);

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Exam Date"),
            COL_VALUE,vecStudyLevel[index].studyDate.c_str(),
            -1);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Exam Time"),
            COL_VALUE,vecStudyLevel[index].studyTime.c_str(),
            -1);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Accession Number"),
            COL_VALUE,vecStudyLevel[index].accessionNumber.c_str(),
            -1);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Description"),
            COL_VALUE,vecStudyLevel[index].studyDescription.c_str(),
            -1);

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("Instance UID"),
            COL_VALUE,vecStudyLevel[index].studyInstanceUID.c_str(),
            -1);
}

void ViewDicomDataSelect::InsertInfo2(int index1,int index2)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeDe));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("type"),
            COL_VALUE,vecStudyLevel[index1].vecSeriesLevel[index2].type.c_str(),
            -1);
    gtk_tree_model_iter_next(model, &iter);
}

void ViewDicomDataSelect::InsertInfo3(int index1,int index2,int index3)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeDe));
    gtk_list_store_clear(GTK_LIST_STORE(model));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_QUALITY,_("filePath"),
            COL_VALUE,vecStudyLevel[index1].vecSeriesLevel[index2].vecFileLevel[index3].filePath.c_str(),
            -1);
}

void ViewDicomDataSelect::PROGRESSSTATUS(int nPos)
{
    double frac;
    frac = nPos*0.01;

    gdk_threads_enter();
    ViewDialog::GetInstance()->SetProgressBar(frac);
    while(gtk_events_pending())
        gtk_main_iteration();
    gdk_threads_leave();

}
gboolean ImportStudyForDicom(gpointer data)
{
    ViewDicomDataSelect *tmp;
    tmp = (ViewDicomDataSelect *)data;
    tmp->ImportStudy();
    return FALSE;
}

unsigned int ViewDicomDataSelect::GetDCMStudyElement(DCMSTUDYELEMENT element)
{
    PatientInfo::Info info;
    info.p.id = element.stPatientID;
    string firstName, midName, lastName;
    ViewQueryRetrieve::ChangePersonNameFormat(element.stPatientName, firstName, lastName, midName);
    info.p.name.last = lastName;
    info.p.name.first = firstName;
    info.p.name.mid = midName;

    if(element.stPatientSex.c_str() !=NULL)
    {
        string sex = element.stPatientSex;
        if(strcmp(sex.c_str(),"F")==0)
            info.p.sex = 0;
        else if(strcmp(sex.c_str() , "M")==0)
            info.p.sex = 1;
        else if(strcmp(sex.c_str() , "O")==0)
            info.p.sex = 2;
        else if(sex.length()==0)
            info.p.sex = -1;
        else
            info.p.sex = -1;
    }
    else
        info.p.sex = -1;

    if(strlen(element.stPatientAge.c_str())!=0)
    {
        string age = element.stPatientAge;
        int age_len = strlen(age.c_str());
        char str[256]="\0";
        sprintf(str,"%c%c%c",age[0],age[1],age[2]);
        info.p.age = atoi(str);

        char str_tmp[2];
        sprintf(str_tmp,"%c",age[age_len-1]);
        if(strcmp(str_tmp,"Y")==0)
            info.p.ageUnit = 0;
        else if(strcmp(str_tmp,"M")==0)
            info.p.ageUnit = 1;
        else if(strcmp(str_tmp,"D")==0)
            info.p.ageUnit = 2;
    }
    else
    {
        info.p.age = 0;
        info.p.ageUnit = 0;
    }

    if(strlen(element.stPatientBirthDate.c_str())!=0)
    {
        string birthDate = element.stPatientBirthDate;
        char birth_year[100] = "\0";
        char birth_month[100] = "\0";
        char birth_day[100] = "\0";
        sprintf(birth_year,"%c%c%c%c",birthDate[0],birthDate[1],birthDate[2],birthDate[3]);
        sprintf(birth_month,"%c%c",birthDate[4],birthDate[5]);
        sprintf(birth_day,"%c%c",birthDate[6],birthDate[7]);
        info.p.birthDate.year = atoi(birth_year);
        info.p.birthDate.month = atoi(birth_month);
        info.p.birthDate.day = atoi(birth_day);
    }
    else
    {
        info.p.birthDate.year =0;
        info.p.birthDate.month = 0;
        info.p.birthDate.day = 0;
    }

    info.p.telephone ="";
    info.p.address = "";
    info.p.comment = "";

    //Size
    char sizeT[256];
    strcpy(sizeT,element.stPatientSize.c_str());
    DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

    //weight
    char weightT[256];
    strcpy(weightT,element.stPatientWeight.c_str());
    DotToCommaLocaleNumeric(weightT, sizeof(weightT));

    if(sizeT!= 0)
        info.e.height =(int)(atof(sizeT)*100);
    else
        info.e.height = 0.0;

    if(weightT!= 0)
    info.e.weight = atof(weightT);
    else
        info.e.weight = 0.0;

    info.e.BSA =0.0;

    if(strlen(element.stStudyDate.c_str())!=0)
    {
        string studyDate = element.stStudyDate;
        char exam_year[100] = "\0";
        char exam_month[100] = "\0";
        char exam_day[100] = "\0";

        sprintf(exam_year,"%c%c%c%c",studyDate[0],studyDate[1],studyDate[2],studyDate[3]);
        sprintf(exam_month,"%c%c",studyDate[4],studyDate[5]);
        sprintf(exam_day,"%c%c",studyDate[6],studyDate[7]);
        info.e.examDate.year = atoi(exam_year);
        info.e.examDate.month = atoi(exam_month);
        info.e.examDate.day = atoi(exam_day);
    }
    else
    {
        info.e.examDate.year = 0;
        info.e.examDate.month =0;
        info.e.examDate.day = 0;
    }
    if(strlen(element.stStudyTime.c_str())!=0)
    {
        string studyTime= element.stStudyTime;
        char exam_hour[100] = "\0";
        char exam_minute[100] = "\0";
        char exam_second[100] = "\0";

        sprintf(exam_hour,"%c%c",studyTime[0],studyTime[1]);
        sprintf(exam_minute,"%c%c",studyTime[2],studyTime[3]);
        sprintf(exam_second,"%c%c",studyTime[4],studyTime[5]);
        info.e.examTime.hour = atoi(exam_hour);
        info.e.examTime.second = atoi(exam_second);
        info.e.examTime.minute = atoi(exam_minute);
    }
    else
    {
        info.e.examTime.hour = 0;
        info.e.examTime.second = 0;
        info.e.examTime.minute = 0;
    }
    ViewQueryRetrieve::ChangePersonNameFormatForShow(element.stStudyDoctor, info.e.examDoctor);
    info.e.comment = element.stStudyDescription;
    info.e.reportDoctor = "";

    //ob
    info.ob.LMPDate.year = 0;
    info.ob.LMPDate.month = 0;
    info.ob.LMPDate.day = 0;

    info.ob.OvulateDate.year = 0;
    info.ob.OvulateDate.month = 0;
    info.ob.OvulateDate.day = 0;

    info.ob.pregCount = -1;
    info.ob.abnormalPregCount = -1;
    info.ob.fetusCount = -1;
    info.ob.deliveryCount = -1;
    info.ob.miscarryCount = -1;

    //CE
    info.car.HR = -1;
    info.car.pressureHigh = -1;
    info.car.pressureLow = -1;

    //URO
    info.uro.PSA = 0;

    m_ptrInstance->m_info = info;
    m_ptrInstance->m_studyNo= element.stStudyNo;

    Database db;
    db.GetExamIDCurrent(info.e.examNum);
    m_ptrInstance->m_studyNo = atoi(info.e.examNum.c_str())+1;
    return m_ptrInstance->m_studyNo;
}

void ViewDicomDataSelect::CreateExamDir(int examNum)
{
    gchar *path;
    gchar buf[100];

    char strExamNum[100];
    sprintf(strExamNum, "%d", examNum);
    path = g_build_path(G_DIR_SEPARATOR_S, STORE_PATH, strExamNum, NULL);
    if (g_access(path, F_OK) == 0)
    {
        PRINTF("-----------------------folder of store path %d is exist, path = %s\n", examNum, path);
        sprintf(buf, "rm -rf %s\n", path);
        FileMan fm;
        if (fm.DelDirectory(buf) == -1)
        {
            perror("rmdir fail!");
            PRINTF("delete folder of store path 0 error\n");
            g_free(path);
            return;
        }
    }

    if (g_mkdir(path, 0755) == -1)
    {
        PRINTF("create folder of store path %d error\n", examNum);
    }

    g_free(path);

}

string ViewDicomDataSelect::GetDCMImageElement(DCMIMAGEELEMENT element,unsigned char *pImageBuf, unsigned long bufLen)
{
    m_ptrInstance->CreateExamDir(m_ptrInstance->m_studyNo);

    time_t at;
    at = time(&at);

    struct tm* ct;
    ct = localtime(&at);

    //char fileName[256];
    char *fileName = (char*)malloc(256);
    memset(fileName, 0, 15);
    sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10, m_ptrInstance->m_imageNum);
    m_ptrInstance->m_imageNum++;
    if(element.imgFrameNumber>1)
    {
        SysOptions so;
        int format = so.GetCineFormat();

        m_ptrInstance->deq.clear();
        int videoFormat = so.GetCineFormat();
        VideoMan::VideoItem item;
        //item.path =(string)fileName;
        if(videoFormat == 0)
            item.format = VideoMan::AVI;
        else
            item.format = VideoMan::CINE;
        item.data = pImageBuf;

        for(int i = 0;i<bufLen/(IMG_AREA_W*IMG_AREA_H*IMG_BPP);i++)
        {

            Image::AutoTracePara para;
            Image image((unsigned int*)pImageBuf, para);
            pImageBuf=pImageBuf+IMG_AREA_W*IMG_AREA_H*IMG_BPP;
            m_ptrInstance->deq.push_back(image);
        }
        item.deq = &(m_ptrInstance->deq);
        item.begin = 0;
        item.frames = element.imgFrameNumber;
        item.frame_rate = Replay::GetInstance()->GetImgFps();
        item.width = element.imgWidth;
        item.height = element.imgHeight;
        VideoMan::GetInstance()->SaveVideoForRetrieve(m_ptrInstance->m_studyNo, fileName, STORE_PATH, &item);
        m_ptrInstance->deq.clear();

        switch(format)
        {
            case VideoMan::CINE:
                strcat(fileName, ".cine");
                break;
            case VideoMan::AVI:
                strcat(fileName, ".avi");
                break;
            default:
                strcat(fileName, ".avi");
                break;
        }
    }
    else if (element.imgFrameNumber == 1)
    {
        SysOptions so;
        int format = so.GetImageFormat();

        ImgMan::ImgItem item;
        item.width = element.imgWidth;
        item.height = element.imgHeight;
        item.data = pImageBuf;
        ScanMode::SpecialMeasurePara para;
        ScanMode::GetInstance()->GetSpecialMeasurePara(&para);
        item.para = para;

        //ImgMan::ImgItem item_frm;
        int format_bak = ImgMan::GetInstance()->GetImgFormat();

        if(ImgMan::GetInstance()->SaveSnapForRetrieve(m_ptrInstance->m_studyNo, fileName, STORE_PATH, &item) == 0)
        {
            ImgMan::GetInstance()->SetImgFormat(ImgMan::FRM);
            ImgMan::GetInstance()->SaveSnapForRetrieve(m_ptrInstance->m_studyNo, fileName, STORE_PATH, &item);
            ImgMan::GetInstance()->SetImgFormat(format_bak);
        }
        switch(format)
        {
            case ImgMan::FRM:
                strcat(fileName, ".frm");
                break;
            case ImgMan::EMP:
                strcat(fileName, ".emp");
                break;
            case ImgMan::BMP:
                strcat(fileName, ".bmp");
                break;
            case ImgMan::JPEG:
                strcat(fileName, ".jpg");
                break;
            default:
                strcat(fileName, ".bmp");
                break;
        }
    }
    string part = element.imgStudyPart;
    if(strcmp(part.c_str(),"ABDOMEN") ==0)
        m_ptrInstance->m_info.e.examType = "Abdomen";
    else if(strcmp(part.c_str(),"HEART") ==0)
        m_ptrInstance->m_info.e.examType = "Cardiac";
    else if(strcmp(part.c_str(),"HIP") ==0)
        m_ptrInstance->m_info.e.examType = "Hip Joint";
    else if(strcmp(part.c_str(),"BREAST") ==0)
        m_ptrInstance->m_info.e.examType = "Mammary Glands";
    else
        m_ptrInstance->m_info.e.examType = "Abdomen";

    string imgFileName = (string)fileName;
    free(fileName);
    return imgFileName;
}

void ViewDicomDataSelect::GetDCMSRElement(DCMSRELEMENT element)
{
    m_ptrInstance->m_srIndication = element.srIndication;
    m_ptrInstance->m_srComments = element.srComments;
    m_ptrInstance->m_srReferenceImageFilename = element.srReferenceImageFilename;

}

void ViewDicomDataSelect::ImportStudy(void)
{
#if 1
    PeripheralMan *ptr = PeripheralMan::GetInstance();
    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewDicomDataSelect::GetInstance()->GetWindow()),
                ViewDialog::ERROR,
                _("No USB storage found!"),
                NULL);
        return;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewDicomDataSelect::GetInstance()->GetWindow()),
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"),
                    NULL);
            return;
        }
    }
#endif
    string destDirStorageMedia = UDISK_PATH;
    Replay::GetInstance()->ClearCurReplayData();
    Replay::GetInstance()->DisplayReplayBar();

    m_ptrInstance->m_srIndication = "";
    m_ptrInstance->m_srComments = "";
    m_ptrInstance->m_srReferenceImageFilename.clear();
    EDCMReturnStatus status = CDCMMan::GetMe()->ImportStudy(m_studyIndex,destDirStorageMedia,GetDCMStudyElement,GetDCMImageElement,GetDCMSRElement,PROGRESSSTATUS);

    if(status == DCMSUCCESS)
    {
        string errmsg;
        string studyNo;
        Database db;
        if (db.ArchivePat(m_ptrInstance->m_info,errmsg))
        {
            db.GetExamIDCurrent(m_ptrInstance->m_info.e.examNum);
        }
        else
        {
            PRINTF("Achieve unsuccessfully!\n");
        }
        ViewQueryRetrieve::GetInstance()->SetRetrieveFlag(false);
        m_patientInfo.SetInfoQA(m_ptrInstance->m_info);

        gchar deststr[512];
        sprintf(deststr, "%s/%d", STORE_PATH, m_ptrInstance->m_studyNo);
        ViewPrintPreview::GetInstance()->CreatePagesForRetrieve(deststr,m_ptrInstance->m_srIndication,m_ptrInstance->m_srComments,m_ptrInstance->m_srReferenceImageFilename);

        //m_patientInfo.ClearAll();
        ViewReport::GetInstance()->ClearIndicationandComments();

        ViewDialog::GetInstance()->Destroy();
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("Import data successfully!"),
                NULL);
        //printf("Send Successfully\n");
        ptr->UmountUsbStorage();
    }
    else if(status == DCMSTUDYEXISTED)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("Study Info has existed!"),
                NULL);
    }
    else if (status == DCMNOENOUGHSPACE)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("No enough space!"),
                NULL);
    }

    else if (status == DCMNONEXISTDICOMDIR)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("DICOMDIR is not existed!"),
                NULL);
    }
    else if (status == DCMNONENTIREDICOMDIR)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("File has lost in DICOMDIR !"),
                NULL);
    }
    else if (status == DCMIMPORTFAILURE)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("Import data unsuccessfully!"),
                NULL);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::INFO,
                _("Import data unsuccessfully!"),
                NULL);
    }
}

void ViewDicomDataSelect::ButtonImportClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    gboolean exist;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeSrc));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeSrc));
    if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
    {
        g_timeout_add(100, ImportStudyForDicom, this);
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::PROGRESS,
                NULL,
                NULL);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),
                ViewDialog::ERROR,
                _("No record is selected!"),
                NULL);
        return;

    }
}

void ViewDicomDataSelect::ButtonCancelClicked(GtkButton *button)
{
    DestroyWindow();
}

gboolean ViewDicomDataSelect::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewDicomDataSelect::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        m_window = NULL;
    }
    //PeripheralMan *ptr = PeripheralMan::GetInstance();
   // ptr->UmountUsbStorage();

}

static gboolean DestroyWin(gpointer data)
{
    ViewDicomDataSelect *tmp;
    tmp = (ViewDicomDataSelect *)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewDicomDataSelect::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100, DestroyWin, this);
            FakeEscKey();
            break;

        default:
            break;
    }
}

#endif