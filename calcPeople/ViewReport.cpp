#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <math.h>
#include <pango-1.0/pango/pangoft2.h>
#include FT_FREETYPE_H

#include "keyboard/KeyValueOpr.h"
#include "display/gui_global.h"
#include "keyboard/KeyDef.h"
#include "patient/PatientInfo.h"
#include "display/ViewCalendar.h"
#include "calcPeople/ViewReport.h"
#include "periDevice/ViewPrintPreview.h"
#include "periDevice/Usblp.h"
#include "display/gui_func.h"
#include "ViewMain.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysCalculateSetting.h"
#include "measure/MeasureMan.h"
#include "calcPeople/MeasureTable.h"
#include "calcPeople/MeaCalcFun.h"
#include "calcPeople/ViewTemplet.h"
#include "patient/ImgMan.h"
#include "display/TopArea.h"
#include "probe/ProbeSelect.h"
#include "sysMan/ViewSystem.h"
#include "calcPeople/MenuCalcNew.h"
#include "periDevice/DCMMan.h"
#include "patient/ImgMan.h"
#include "sysMan/SysGeneralSetting.h"

#include "patient/CreateBitmap.h"
#include "keyboard/MultiFuncFactory.h"
extern pthread_t pidInitTable;
extern gboolean ReportInit;

//static void* ThreadLoadData1(void* argv);
//static void* ThreadLoadData2(void* argv);
//static void* ThreadLoadImage(void* argv);

static const char str_Des[7][3][50] = {
    {N_("FHR ≤ 1 times"), N_("FHR ≥ 15 times/m, time ≥ 15s, 1~4 times"), N_("FHR ≥ 15 times/m, time ≥ 15s, ≥ 5 times")},
    {N_("No FM"), N_("1~2 times FM"), N_("FM ≥ 3 times")},
    {N_("No FBM or time ≤ 30s"), N_("FBM ≥ 1 times,time 30-60s"), N_("FBM ≥ 1 times,time ≥ 60s")},
    {N_("Limbs stretch, no bend, fingers loose"), N_("Limbs or spine stretch-bend ≥ 1 times"), N_("Limbs and spine stretch-bend ≥ 1 times")},
    {N_("Placental grade is 3"), N_("Placental on posterior wall, no grade"), N_("Placental grade ≤ 2")},
    {N_("AF < 1cm"), N_("1cm < AF < 2cm"), N_("AF > 2cm")},
    {N_("Normal, chronic asphyxia risk low"), N_("Chronic asphyxia risk suspicious"), N_("Chronic asphyxia risk high")},
};

ViewReport* ViewReport::m_ptrInstance = NULL;

ViewReport::ViewReport()
{
    m_notebook = NULL;
    m_showFlag = FALSE;
    m_pixmapGC = NULL;
    for(int i=0;i<7;i++)
        m_grade[i] = 0;
    row_height = 22;
   DefaultCalcArr();
}

ViewReport::~ViewReport()
{
    if(m_ptrInstance != NULL)
        delete m_ptrInstance;

    DefaultCalcArr();
}

ViewReport* ViewReport::GetInstance()
{
    if(m_ptrInstance == NULL)
        m_ptrInstance = new ViewReport;

    return m_ptrInstance;
}

extern KeyValueOpr keyInterface;
void ViewReport::CreateWindow(void)
{
    GtkWidget *btnAdd, *imageAdd, *labelAdd;
//  GtkWidget *btnExport, *alignmentExport, *hboxExport, *imageExport, *labelExport;
    GtkWidget *btnExit, *imageExit, *labelExit;
//  GtkWidget *btnSave, *imageSave, *labelSave;
    GtkWidget *btnPrint, *imagePrint, *labelPrint;
    GtkWidget *btnEdit, *imageEdit, *labelEdit;

    GtkWidget *framePatInfo, *labelFrame, *tablePatInfo;
    GtkWidget *labelHos, *labelName, *labelID, *labelSID, *labelDate, *labelItem, *labelRDr, *labelUDr, *labelSex, *labelAge;

#ifdef VET
    GtkWidget *labelOwnerName;
    GtkWidget *labelAnimalName;
    GtkWidget *labelSpecies;
#endif
    //BeforeEnterDialog();
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

    //create window
    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_window, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_title (GTK_WINDOW (m_window), _("Report"));
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_decorated (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));

    m_fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (m_window), m_fixed);

    //create button area
    labelAdd = gtk_label_new_with_mnemonic (_("Add\nImage"));
    imageAdd = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
    btnAdd = create_button_icon (labelAdd, imageAdd);
    gtk_fixed_put (GTK_FIXED (m_fixed), btnAdd, 494, 688);
    g_signal_connect (G_OBJECT(btnAdd), "clicked", G_CALLBACK(HandleBtnAddImageClicked), this);

    labelEdit = gtk_label_new_with_mnemonic (_("Edit\nReport"));
    imageEdit = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_BUTTON);
    btnEdit = create_button_icon (labelEdit, imageEdit);
    gtk_fixed_put (GTK_FIXED (m_fixed), btnEdit, 624, 688);
    g_signal_connect(G_OBJECT(btnEdit), "clicked", G_CALLBACK(HandleBtnEditClicked), this);

    labelPrint = gtk_label_new_with_mnemonic (_("Print\nPreview"));
    imagePrint = gtk_image_new_from_stock ("gtk-print", GTK_ICON_SIZE_BUTTON);
    btnPrint = create_button_icon (labelPrint, imagePrint);
    gtk_fixed_put (GTK_FIXED (m_fixed), btnPrint, 754, 688);
    g_signal_connect(G_OBJECT(btnPrint), "clicked", G_CALLBACK(HandleBtnPrintClicked), this);

    labelExit = gtk_label_new_with_mnemonic (_("Exit"));
    imageExit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
    btnExit = create_button_icon (labelExit, imageExit);
    gtk_fixed_put (GTK_FIXED (m_fixed), btnExit, 884, 688);
    g_signal_connect(G_OBJECT(btnExit), "clicked", G_CALLBACK(HandleBtnExitClicked), this);

    //create patient info area
    framePatInfo = gtk_frame_new (NULL);
    gtk_fixed_put (GTK_FIXED (m_fixed), framePatInfo, 20, 20);
    gtk_widget_set_size_request (framePatInfo, 984, 160);
    gtk_frame_set_label_align (GTK_FRAME (framePatInfo), 0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (framePatInfo), GTK_SHADOW_IN);

#ifdef VET
    labelFrame = gtk_label_new (_("Animal Information"));
#else
    labelFrame = gtk_label_new (_("Patient Information"));
#endif
    gtk_frame_set_label_widget (GTK_FRAME (framePatInfo), labelFrame);
    gtk_label_set_single_line_mode (GTK_LABEL (labelFrame), TRUE);

    tablePatInfo = gtk_table_new (4, 8, TRUE);
    gtk_container_add (GTK_CONTAINER (framePatInfo), tablePatInfo);
    gtk_container_set_border_width (GTK_CONTAINER (tablePatInfo), 10);
    gtk_table_set_row_spacings (GTK_TABLE (tablePatInfo), 6);

    labelHos = gtk_label_new (_("Hospital:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelHos, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelHos), 0, 0.5);

    m_entryHos = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryHos, 1, 4, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryHos), FALSE);

//hlx
#ifdef VET
    labelAnimalName = gtk_label_new (_("Animal Name:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelAnimalName, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelAnimalName), 0, 0.5);

    m_entryAnimalName = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryAnimalName, 1, /*3*/2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryAnimalName), FALSE);

    labelOwnerName = gtk_label_new(_("Owner:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelOwnerName, /*0, 1, */2, 3, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelOwnerName), 1, 0.5);

    m_entryOwnerName = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryOwnerName, 3, 4, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryOwnerName), FALSE);

    labelSpecies = gtk_label_new (_("Species:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelSpecies, 4, 5, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelSpecies), 1, 0.5);

    m_entrySpecies = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entrySpecies, 5, 6, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entrySpecies), FALSE);

    labelSex = gtk_label_new (_("Gender:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelSex, 6, 7, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelSex), 1, 0.5);

    m_entrySex = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entrySex, 7, 8, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entrySex), FALSE);
#else
    labelName = gtk_label_new (_("Name:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelName, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelName), 0, 0.5);

    m_entryName = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryName, 1, 3, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryName), FALSE);

    labelSex = gtk_label_new (_("Gender:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelSex, 3, 4, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelSex), 1, 0.5);

    m_entrySex = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entrySex, 4, 5, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entrySex), FALSE);

    labelAge = gtk_label_new (_("Age:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelAge, 5, 6, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelAge), 1, 0.5);

    m_entryAge = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryAge, 6, 7, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryAge), FALSE);
#endif
#ifdef VET
    labelID = gtk_label_new (_("ID:"));
#else
    labelID = gtk_label_new (_("Patient ID:"));
#endif
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelID, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelID), 0, 0.5);

    m_entryID = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryID, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryID), FALSE);

    labelSID = gtk_label_new (_("Exam ID:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelSID, 2, 3, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelSID), 1, 0.5);

    m_entrySID = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entrySID, 3, 4, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entrySID), FALSE);

#ifdef VET
    labelAge = gtk_label_new (_("Age:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelAge, 4, 5, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelAge), 1, 0.5);

    m_entryAge = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryAge, 5, 6, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryAge), FALSE);

    labelItem = gtk_label_new (_("Exam Type:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelItem, 6, 7, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelItem), 1, 0.5);

    m_entryItem = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryItem, 7, 8, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryItem), FALSE);
#else
    labelItem = gtk_label_new (_("Exam Type:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelItem, 4, 5, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelItem), 1, 0.5);

    m_entryItem = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryItem, 5, 7, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    //gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryItem, 5, 6, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryItem), FALSE);
#endif

    labelDate = gtk_label_new (_("Exam Date:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelDate, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelDate), 0, 0.5);

    m_entryDate = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryDate, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryDate), FALSE);

    labelUDr = gtk_label_new (_("Diagnostician:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelUDr, 2, 3, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelUDr), 1, 0.5);

    m_entryUDr = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryUDr, 3, 5, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryUDr), FALSE);

    labelRDr = gtk_label_new (_("Physician:"));
    gtk_table_attach (GTK_TABLE (tablePatInfo), labelRDr, 5, 6, 3, 4, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (labelRDr), 1, 0.5);

    m_entryRDr = gtk_entry_new ();
    gtk_table_attach (GTK_TABLE (tablePatInfo), m_entryRDr, 6, 8, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_editable_set_editable (GTK_EDITABLE (m_entryRDr), FALSE);

    //create notebook
    m_notebook = gtk_notebook_new ();
    gtk_fixed_put (GTK_FIXED (m_fixed), m_notebook, 20, 190);
    gtk_widget_set_size_request (m_notebook, 984, 488);

    m_viewportABD = CreateSW2NB(m_notebook, NB_ABD, _("Abdomen"));
    m_viewportUR = CreateSW2NB(m_notebook, NB_UR, _("Urology"));
    m_viewportAE = CreateSW2NB(m_notebook, NB_AE, _("Cardiac"));
//hlx
#ifdef VET
    m_viewportAnOB = CreateSW2NB(m_notebook, NB_ANOB, _("Obstetrics"));
#else
    m_viewportOB = CreateSW2NB(m_notebook, NB_OB, _("Obstetrics"));
#endif
//修改 动物超 在动物超中没有妇科
//hlx
#ifdef VET
#else
    m_viewportGYN = CreateSW2NB(m_notebook, NB_GYN, _("Gynecology"));
#endif

    m_viewportSP = CreateSW2NB(m_notebook, NB_SP, _("Small Part"));
#if (defined(EMP_313) || defined(EMP_322))
    m_viewportFE = CreateSW2NB(m_notebook, NB_FE, _("Fetal Cardio"));
#else
    m_viewportVES = CreateSW2NB(m_notebook, NB_VES, _("Vascular"));
    m_viewportFE = CreateSW2NB(m_notebook, NB_FE, _("Fetal Cardio"));
    m_viewportTCD = CreateSW2NB(m_notebook, NB_TCD, _("TCD"));
#endif
    m_viewportORT = CreateSW2NB(m_notebook, NB_ORT, _("Orthopedic"));

//添加 动物超 添加肌腱科别
//hlx
#ifdef VET
    m_viewportTD = CreateSW2NB(m_notebook, NB_TD, _("Tendon"));
#endif

    g_signal_connect(G_OBJECT(m_notebook), "switch-page", G_CALLBACK(HandleNotebookChanged), this);

    //edit reprot
    m_frameEdit = CreateEditView();
    gtk_fixed_put (GTK_FIXED (m_fixed), m_frameEdit, 20, 513);

    //add image
    m_frameImage = CreateImageView();
    gtk_fixed_put (GTK_FIXED (m_fixed), m_frameImage, 20, 513);
//#ifdef VET
//#else
    LoadImage();
//#endif
#if 1
    SetPatientInfo();
    gtk_widget_show_all (m_window);
    gtk_widget_hide_all(m_frameEdit);
    gtk_widget_hide_all(m_frameImage);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
#endif

    if(!ReportInit)
        pthread_join(pidInitTable, NULL);   //Waiting for the init thread

#ifdef VET

     int index = NB_ABD;
//  LoadMeasureData(NB_ABD);

#else

    int index = 0;
    ExamItem::EItem item = ProbeSelect::GetItemIndex();
//  printf("item = %d\n", (int)item);
    switch(item)
    {
        case ExamItem::ABDO_ADULT:
        case ExamItem::ABDO_LIVER:
        case ExamItem::ABDO_KID:
        case ExamItem::CAR_KID:
        case ExamItem::USER1:
        case ExamItem::USER2:
        case ExamItem::USER3:
        case ExamItem::USER4:
            index = NB_ABD;
            break;
        case ExamItem::KIDNEY:
        case ExamItem::BLADDER:
            index = NB_UR;
            break;
        case ExamItem::CAR_ADULT:
            index = NB_AE;
            break;
        case ExamItem::EARLY_PREG:
        case ExamItem::LATER_PREG:
            index = NB_OB;
            break;
        case ExamItem::GYN:
            index = NB_GYN;
            break;
        case ExamItem::SMALL_PART:
        case ExamItem::GLANDS:
        case ExamItem::THYROID:
        case ExamItem::EYE:
            index = NB_SP;
            break;
#if (defined(EMP_322) || defined(EMP_313))
    case ExamItem::CAR_FETUS:
            index = NB_FE;
            break;
    case ExamItem::HIP_JOINT:
    case ExamItem::JOINT_CAVITY:
    case ExamItem::MENISCUS:
    case ExamItem::SPINE:
            index = NB_ORT;
            break;
#else
        case ExamItem::CAROTID:
        case ExamItem::JUGULAR:
        case ExamItem::PERI_ARTERY:
        case ExamItem::PERI_VEIN:
            index = NB_VES;
            break;
        case ExamItem::CAR_FETUS:
            index = NB_FE;
            break;
        case ExamItem::TCD:
            index = NB_TCD;
            break;
        case ExamItem::HIP_JOINT:
        case ExamItem::JOINT_CAVITY:
        case ExamItem::MENISCUS:
        case ExamItem::SPINE:
            index = NB_ORT;
            break;
#endif
        default:
            index = NB_ABD;
            break;
    }
#endif
    PRINTF("index = %d\n", index);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_notebook), index);
    LoadMeasureData();
    ShowMeasureData(index);
//#endif
}

void ViewReport::ShowMeasureData(guint page_num)
{
    gtk_widget_hide_all(m_viewportABD);
    gtk_widget_hide_all(m_viewportUR);
    gtk_widget_hide_all(m_viewportAE);
#ifdef VET
    gtk_widget_hide_all(m_viewportAnOB);
#else
    gtk_widget_hide_all(m_viewportOB);
#endif
#ifdef VET
#else
    gtk_widget_hide_all(m_viewportGYN);
#endif
    gtk_widget_hide_all(m_viewportSP);
#if (defined(EMP_313) || defined(EMP_322))
#else
    gtk_widget_hide_all(m_viewportTCD);
    gtk_widget_hide_all(m_viewportVES);
#endif
    gtk_widget_hide_all(m_viewportFE);
    gtk_widget_hide_all(m_viewportORT);
#ifdef VET
    gtk_widget_hide_all(m_viewportTD);
#endif
    switch (page_num)
    {
        case NB_ABD:
            gtk_widget_show_all(m_viewportABD);
            break;
        case NB_UR:
            gtk_widget_show_all(m_viewportUR);
            break;
        case NB_AE:
            gtk_widget_show_all(m_viewportAE);
            break;
#ifdef VET
        case NB_ANOB:
            gtk_widget_show_all(m_viewportAnOB);
            break;
#else
        case NB_OB:
            gtk_widget_show_all(m_viewportOB);
            break;
#endif
#ifdef VET
#else
        case NB_GYN:
            gtk_widget_show_all(m_viewportGYN);
            break;
#endif
        case NB_SP:
            gtk_widget_show_all(m_viewportSP);
            break;
#if (defined(EMP_313) || defined(EMP_322))
#else
        case NB_TCD:
            gtk_widget_show_all(m_viewportTCD);
            break;
        case NB_VES:
            gtk_widget_show_all(m_viewportVES);
            break;
#endif
        case NB_FE:
            gtk_widget_show_all(m_viewportFE);
            break;
        case NB_ORT:
            gtk_widget_show_all(m_viewportORT);
            break;
#ifdef VET
        case NB_TD:
            gtk_widget_show_all(m_viewportTD);
            break;
#endif
    }
}

#ifdef VET
void ViewReport::LoadMeasureData(void)
{
    GtkWidget *fixed;
    fixed = NULL;

//  switch (page_num)
//  {
    //  case NB_ABD:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportABD)))
            {
                fixed = CreateNotePageABD();
                gtk_container_add (GTK_CONTAINER (m_viewportABD), fixed);
            }
        //  break;
    //  case NB_UR:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportUR)))
            {
                fixed = CreateNotePageUR();
                gtk_container_add (GTK_CONTAINER (m_viewportUR), fixed);
            }
        //  break;
    //  case NB_AE:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportAE)))
            {
                fixed = CreateNotePageAE();
                gtk_container_add (GTK_CONTAINER (m_viewportAE), fixed);
            }
    //      break;
//修改 动物超  在动物超中的产科和人用超的产科有很大的不一样
//hlx
    //  case NB_ANOB:
            if (!gtk_bin_get_child(GTK_BIN(m_viewportAnOB)))
            {
                fixed = CreateNotePageAnOB();
                gtk_container_add(GTK_CONTAINER(m_viewportAnOB), fixed);
            }
          //  break;
       // case NB_SP:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportSP)))
            {
                fixed = CreateNotePageSP();
                gtk_container_add (GTK_CONTAINER (m_viewportSP), fixed);
            }
         //   break;
    //  case NB_TCD:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportTCD)))
            {
                fixed = CreateNotePageTCD();
                gtk_container_add (GTK_CONTAINER (m_viewportTCD), fixed);
            }
          //  break;
      //  case NB_VES:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportVES)))
            {
                fixed = CreateNotePageVES();
                gtk_container_add (GTK_CONTAINER (m_viewportVES), fixed);
            }
         //   break;
       // case NB_FE:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportFE)))
            {
                fixed = CreateNotePageFE();
                gtk_container_add (GTK_CONTAINER (m_viewportFE), fixed);
            }
          //  break;
      //  case NB_ORT:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportORT)))
            {
                fixed = CreateNotePageORT();
                gtk_container_add (GTK_CONTAINER (m_viewportORT), fixed);
            }
        //    break;
//添加 动物超 肌腱科的报告
    //  case NB_TD:
            if(!gtk_bin_get_child(GTK_BIN(m_viewportTD)))
            {
                fixed = CreateNotePageTD();
                gtk_container_add(GTK_CONTAINER(m_viewportTD), fixed);
            }
        //  break;
    //  default:
        //  fixed = NULL;
        //  break;
//  }
//  gtk_widget_show_all (fixed);
}
#else
void ViewReport::LoadMeasureData(void)
{
    GtkWidget *fixed = NULL;
    if(!gtk_bin_get_child(GTK_BIN(m_viewportABD)))
    {
        fixed = CreateNotePageABD();
        gtk_container_add (GTK_CONTAINER (m_viewportABD), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportUR)))
    {
        fixed = CreateNotePageUR();
        gtk_container_add (GTK_CONTAINER (m_viewportUR), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportAE)))
    {
        fixed = CreateNotePageAE();
        gtk_container_add (GTK_CONTAINER (m_viewportAE), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportOB)))
    {
        fixed = CreateNotePageOB();
        gtk_container_add (GTK_CONTAINER (m_viewportOB), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportGYN)))
    {
        fixed = CreateNotePageGYN();
        gtk_container_add (GTK_CONTAINER (m_viewportGYN), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportSP)))
    {
        fixed = CreateNotePageSP();
        gtk_container_add (GTK_CONTAINER (m_viewportSP), fixed);
    }
#if (defined(EMP_313) || defined(EMP_322))
#else
    if(!gtk_bin_get_child(GTK_BIN(m_viewportTCD)))
    {
        fixed = CreateNotePageTCD();
        gtk_container_add (GTK_CONTAINER (m_viewportTCD), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportVES)))
    {
        fixed = CreateNotePageVES();
        gtk_container_add (GTK_CONTAINER (m_viewportVES), fixed);
    }
#endif
    if(!gtk_bin_get_child(GTK_BIN(m_viewportFE)))
    {
        fixed = CreateNotePageFE();
        gtk_container_add (GTK_CONTAINER (m_viewportFE), fixed);
    }
    if(!gtk_bin_get_child(GTK_BIN(m_viewportORT)))
    {
        fixed = CreateNotePageORT();
        gtk_container_add (GTK_CONTAINER (m_viewportORT), fixed);
    }
}
#endif

#ifdef VET
void ViewReport::LoadMeasureDataForArchive(void)
{
    CreateNotePageABD();
    CreateNotePageUR();
    CreateNotePageAE();
    CreateNotePageAnOB();
    CreateNotePageSP();
    CreateNotePageTCD();
    CreateNotePageVES();
    CreateNotePageFE();
    CreateNotePageORT();
    CreateNotePageTD();
}
#else
void ViewReport::LoadMeasureDataForArchive(void)
{
    CreateNotePageABD();
    CreateNotePageUR();
    CreateNotePageAE();
    CreateNotePageOB();
    CreateNotePageGYN();
    CreateNotePageSP();
#if (defined(EMP_313) || defined(EMP_322))
#else
    CreateNotePageTCD();
    CreateNotePageVES();
#endif
    CreateNotePageFE();
    CreateNotePageORT();
}
#endif

void ViewReport::LoadImage()
{
    int total = m_vecName.size();
    GtkWidget *vbox;

//  PatientInfo::Info info;
//  g_patientInfo.GetInfo(info);
//  int sid = atoi(info.e.examNum.c_str());
    int sid = 0;

    GtkWidget *table = gtk_table_new(1, total, TRUE);
    gtk_container_add (GTK_CONTAINER (m_viewportImage), table);

    m_vecChkBtn.clear();
    for(int i=0; i<total; i++)
    {
        vbox = CreateImageItem(sid, i);
        gtk_table_attach(GTK_TABLE(table), vbox, i, 1+i, 0, 2, (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 5, 5);
    }

    gtk_widget_show_all (table);
}

void ViewReport::DestroyWindow(void)
{
    m_vecNameTmp = m_vecNameSel;

    if(GTK_IS_WIDGET(m_window))
    {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        m_window = NULL;
    }
}

static gboolean ExitWindow(gpointer data)
{
    ViewReport *tmp;
    tmp = (ViewReport *)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewReport::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue)
    {
        case KEY_ESC:
        case KEY_REPORT:
            g_timeout_add(100, ExitWindow, this);
            FakeEscKey();
            break;

        default:
            break;
    }
}

bool ViewReport::Sort(const string s1, const string s2)
{
    int i;
    int len;
    char str1[255], str2[255];

    len = strlen(s1.c_str());
    for(i=0; i<len; i++)
    {
        if(s1.c_str()[i]=='.')
            break;
    }
    strncpy(str1, s1.c_str(), i);
    str1[i] = '\0';

    len = strlen(s2.c_str());
    for(i=0; i<len; i++)
    {
        if(s2.c_str()[i]=='.')
            break;
    }
    strncpy(str2, s2.c_str(), i);
    str2[i] = '\0';

    return atof(str1) > atof(str2);
//  return atoi(s1.c_str()) < atoi(s2.c_str());
}

void ViewReport::LoadSnapInfo()
{
//  PatientInfo::Info info;
//  g_patientInfo.GetInfo(info);
//  int sid = atoi(info.e.examNum.c_str());
    int sid = 0;

    m_vecName.clear();
    ImgMan::GetInstance()->LoadSnap(sid, STORE_PATH, &m_vecName);

    sort(m_vecName.begin(), m_vecName.end(), Sort);
}

GtkWidget* ViewReport::CreateImageItem(int sid, int id)
{
    GtkWidget *image, *chkbtn;
    ImgMan::ImgItem img_item;
    if(ImgMan::GetInstance()->ReadSnap(sid, m_vecName[id].c_str(), STORE_PATH, &img_item) != 0)
    {
        PRINTF("%s: ReadSnap Error!\n", __FUNCTION__);
        return NULL;
    }

    GdkPixbuf *pixbuf = NULL;
    pixbuf = gdk_pixbuf_scale_simple(img_item.pixbuf, SNAP_W, SNAP_H, GDK_INTERP_BILINEAR);
    g_object_unref(img_item.pixbuf);

    if(pixbuf != NULL)
    {
        image = gtk_image_new();
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
        g_object_unref(pixbuf);
    }

    GtkWidget *vbox = gtk_vbox_new (FALSE, 2);

    GtkWidget *button = gtk_button_new ();
    gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);
//  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(HandleBtnAddImageClicked), this);
    g_object_set_data(G_OBJECT(button), "id", GINT_TO_POINTER(id));
    gtk_button_set_image(GTK_BUTTON(button), image);
//  gtk_widget_set_tooltip_text(button, m_vecName[id].c_str());
    gchar *basename = g_path_get_basename(m_vecName[id].c_str());
    gtk_widget_set_tooltip_text(button, basename);
    g_free(basename);

    GtkWidget *hbox = gtk_hbox_new (TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    char tmp[4];
    sprintf(tmp, "%d", id+1);
    chkbtn = gtk_check_button_new_with_label (tmp);
//  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(chkbtn)), GTK_STATE_NORMAL, g_black);
//  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(chkbtn)), GTK_STATE_ACTIVE, g_black);
//  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(chkbtn)), GTK_STATE_PRELIGHT, g_black);
    gtk_box_pack_start (GTK_BOX (hbox), chkbtn, FALSE, FALSE, 0);
    g_object_set_data(G_OBJECT(chkbtn), "id", GINT_TO_POINTER(id));
    g_signal_connect(G_OBJECT(chkbtn), "clicked", G_CALLBACK(HandleChkBtnImageClicked), this);

    m_vecChkBtn.push_back(chkbtn);

    return vbox;
}

static void TestShowVec(vector<string> vec)
{
    vector<string>::iterator ite;
    for(ite = vec.begin(); ite < vec.end(); ite++)
        PRINTF("%s: Name = %s\n", __FUNCTION__, (*ite).c_str());
}

string ViewReport::GetExamDoctor()
{
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    string examDoctor = info.e.examDoctor;
    return examDoctor;
}

string ViewReport::GetHospitalName()
{
    SysGeneralSetting sgs;
    string hospital;
    sgs.GetHospital(hospital);
    return hospital;
}

void ViewReport::BtnAddOKClicked(GtkButton *button)
{
    m_vecNameSel = m_vecNameTmp;
    CDCMMan::GetMe()->EditSR(GetSRElement());
    CDCMMan::GetMe()->SetSRVerifyFlag(true);
    string name = GetExamDoctor();
    if(name =="")
        name = "VerifierName";
    CDCMMan::GetMe()->SetSRVerifierName(name);
    string organization = GetHospitalName();
    if(organization =="")
        organization = "VerifyOrganization";
    CDCMMan::GetMe()->SetSRVerifyOrganization(organization);

    HideImageView();
}

void ViewReport::BtnAddCancelClicked(GtkButton *button)
{
    m_vecNameTmp = m_vecNameSel;
    HideImageView();
}

void ViewReport::ChkBtnImageClicked(GtkButton *button)
{
    if(m_showFlag)
        return;

    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "id"));

    if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
    {
        vector<string>::iterator it = find(m_vecNameTmp.begin(), m_vecNameTmp.end(), m_vecName[id].c_str());
        if(it != m_vecNameTmp.end())
            m_vecNameTmp.erase(it);
    }
    else
    {
        if(m_vecNameTmp.size() < MAX_SEL)
        {
            vector<string>::iterator it = find(m_vecNameTmp.begin(), m_vecNameTmp.end(), m_vecName[id].c_str());
            if(it == m_vecNameTmp.end())
                m_vecNameTmp.push_back(m_vecName[id].c_str());
        }
        else
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
        }
    }
}

GtkWidget* ViewReport::CreateImageView(void)
{
    LoadSnapInfo();

    GtkWidget *frame = gtk_frame_new (NULL);
//  gtk_fixed_put (GTK_FIXED (m_fixed), frame, 30, 510);
    gtk_widget_set_size_request (frame, 984, 165);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);

#ifdef REPORTPICS_6
    GtkWidget *label = gtk_label_new (_("Select Image(Maximum of 6)"));
#else
    GtkWidget *label = gtk_label_new (_("Select Image(Maximum of 2)"));
#endif
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_single_line_mode (GTK_LABEL (label), TRUE);

    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (sw, 830, 140);
    gtk_fixed_put(GTK_FIXED(fixed), sw, 10, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_ALWAYS, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_IN);

    m_viewportImage = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (m_viewportImage);
    gtk_container_add (GTK_CONTAINER (sw), m_viewportImage);

    GtkWidget *labelOK = gtk_label_new_with_mnemonic (_("OK"));
    GtkWidget *imageOK = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
    GtkWidget *btnOK = create_button_icon (labelOK, imageOK);
    gtk_fixed_put(GTK_FIXED(fixed), btnOK, 850, 10);
    g_signal_connect(G_OBJECT(btnOK), "clicked", G_CALLBACK(HandleBtnAddOKClicked), this);

    GtkWidget *labelCancel = gtk_label_new_with_mnemonic (_("Cancel"));
    GtkWidget *imageCancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    GtkWidget *btnCancel = create_button_icon (labelCancel, imageCancel);
    gtk_fixed_put(GTK_FIXED(fixed), btnCancel, 850, 80);
    g_signal_connect(G_OBJECT(btnCancel), "clicked", G_CALLBACK(HandleBtnAddCancelClicked), this);

    return frame;
}

void ViewReport::ShowImageView(void)
{
    if(m_vecNameSel.size() > 0)
    {
        unsigned int s = m_vecNameSel.size();
        for(guint i=0; i<s; i++)
        {
            vector<string>::iterator it = find(m_vecName.begin(), m_vecName.end(), m_vecNameSel[i].c_str());
            if(it == m_vecName.end())
            {
                vector<string>::iterator itt = m_vecNameSel.begin() + i;
                m_vecNameSel.erase(itt);
            }
        }
        m_vecNameTmp = m_vecNameSel;
        for(guint i=0; i<m_vecName.size(); i++)
        {
            vector<string>::iterator it = find(m_vecNameSel.begin(), m_vecNameSel.end(), m_vecName[i].c_str());
        //  g_signal_handlers_block_by_func (G_OBJECT(m_vecChkBtn[i]), (void*)HandleChkBtnImageClicked, NULL);
            if(it != m_vecNameSel.end())
            {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vecChkBtn[i]), TRUE);
                PRINTF("Found at %d\n", i);
            }
            else
            {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_vecChkBtn[i]), FALSE);
            }
        //  g_signal_handlers_unblock_by_func (G_OBJECT(m_vecChkBtn[i]), (void*)HandleChkBtnImageClicked, NULL);
        }
    }

    gtk_widget_set_size_request(m_notebook, m_notebook->allocation.width, 313);
    gtk_widget_show_all(m_frameImage);
}

void ViewReport::HideImageView(void)
{
    gtk_widget_set_size_request(m_notebook, m_notebook->allocation.width, 488);
    gtk_widget_hide_all(m_frameImage);
}

#ifdef VET
void ViewReport::BtnAddImageClicked(GtkButton *button)
{
    LoadImage();
    HideEditView();
    m_showFlag = TRUE;
    ShowImageView();
    m_showFlag = FALSE;
}
#else
void ViewReport::BtnAddImageClicked(GtkButton *button)
{
    HideEditView();
    m_showFlag = TRUE;
    ShowImageView();
    m_showFlag = FALSE;
}
#endif

void ViewReport::BtnEditOKClicked(GtkButton *button)
{
    char *text;
    GtkTextIter iter_start, iter_end;

    gtk_text_buffer_get_start_iter(m_buffer_indication, &iter_start);
    gtk_text_buffer_get_end_iter(m_buffer_indication, &iter_end);
    text = gtk_text_buffer_get_text(m_buffer_indication, &iter_start, &iter_end, FALSE);
    m_strReport[0] = text;

    gtk_text_buffer_get_start_iter(m_buffer_comments, &iter_start);
    gtk_text_buffer_get_end_iter(m_buffer_comments, &iter_end);
    text = gtk_text_buffer_get_text(m_buffer_comments, &iter_start, &iter_end, FALSE);
    m_strReport[1] = text;

    CDCMMan::GetMe()->EditSR(GetSRElement());
    CDCMMan::GetMe()->SetSRVerifyFlag(true);
    string name = GetExamDoctor();
    if(name =="")
        name = "VerifierName";
    CDCMMan::GetMe()->SetSRVerifierName(name);
    string organization = GetHospitalName();
    if(organization =="")
        organization = "VerifyOrganization";
    CDCMMan::GetMe()->SetSRVerifyOrganization(organization);

    HideEditView();
}

void ViewReport::BtnEditCancelClicked(GtkButton *button)
{
    HideEditView();
}

void ViewReport::ShowEditView(void)
{
    gtk_widget_set_size_request (m_notebook, m_notebook->allocation.width, 313);
    gtk_widget_show_all(m_frameEdit);

//  SysGeneralSetting sysGS;
//  if(sysGS.GetLanguage() != ZH)
//      gtk_widget_hide(m_btnTransfer);

    SetIndication(m_strReport[0].c_str());
    SetComments(m_strReport[1].c_str());

    gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(m_swEdit)), 0.0);
}

void ViewReport::HideEditView(void)
{
    gtk_widget_set_size_request (m_notebook, m_notebook->allocation.width, 488);
    gtk_widget_hide_all(m_frameEdit);
}

void ViewReport::BtnEditClicked(GtkButton *button)
{
    HideImageView();
    ShowEditView();
}

GtkWidget* ViewReport::CreateEditView(void)
{
    GtkWidget *viewport;
    GtkWidget *fixed_edit_report;
    GtkWidget *sw_indication;
//    GtkWidget *m_textview_indication;
    GtkWidget *sw_comments;
//    GtkWidget *m_textview_comments;
    GtkWidget *button_clear_indication;
    GtkWidget *button_clear_comments;
    GtkWidget *label_indication;
    GtkWidget *label_comments;

    m_frameEdit = gtk_frame_new (NULL);
//  gtk_fixed_put (GTK_FIXED (m_fixed), m_frameEdit, 30, 510);
    gtk_widget_set_size_request (m_frameEdit, 984, 165);
    gtk_frame_set_label_align (GTK_FRAME (m_frameEdit), 0, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (m_frameEdit), GTK_SHADOW_IN);

    GtkWidget *label = gtk_label_new (_("Edit Report"));
    gtk_frame_set_label_widget (GTK_FRAME (m_frameEdit), label);
    gtk_label_set_single_line_mode (GTK_LABEL (label), TRUE);

    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add (GTK_CONTAINER (m_frameEdit), fixed);

    m_swEdit = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (m_swEdit, 830, 130);
    gtk_fixed_put(GTK_FIXED(fixed), m_swEdit, 10, 10);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_swEdit), GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_swEdit), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
//    gtk_scrolled_window_set_placement(GTK_SCROLLED_WINDOW (m_swEdit), GTK_CORNER_TOP_RIGHT);

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (m_swEdit), viewport);

    fixed_edit_report = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (viewport), fixed_edit_report);

    label_indication = gtk_label_new (_("<b>Descriptor:</b>"));
    //label_indication = gtk_label_new (_("<b>Indication:</b>"));
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), label_indication, 10, 5);
    gtk_widget_set_size_request (label_indication, -1, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_indication), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_indication), 0, 0.5);

    sw_indication = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), sw_indication, 10, 40);
    gtk_widget_set_size_request (sw_indication, 790, 80);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw_indication), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw_indication), GTK_SHADOW_IN);

    m_textview_indication = gtk_text_view_new ();
    m_buffer_indication = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_indication));
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_indication), GTK_WRAP_WORD_CHAR);
    gtk_container_add (GTK_CONTAINER (sw_indication), m_textview_indication);
    g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_indication))), "insert-text", G_CALLBACK(HandleTextIndicationInsert), this);

#ifndef VET
    m_btnTransfer = gtk_button_new_with_mnemonic (_("Transfer templet"));
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), m_btnTransfer, 550-50, 5);
    gtk_widget_set_size_request (m_btnTransfer, 160, 30);
    g_signal_connect(G_OBJECT(m_btnTransfer), "clicked", G_CALLBACK(HandleBtnTransferClicked), this);
#endif

    button_clear_indication = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), button_clear_indication, 680, 5);
    gtk_widget_set_size_request (button_clear_indication, 120, 30);
    g_signal_connect(G_OBJECT(button_clear_indication), "clicked", G_CALLBACK(HandleBtnClearIndicationClicked), this);

    label_comments = gtk_label_new (_("<b>Comments:</b>"));
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), label_comments, 10, 125);
    gtk_widget_set_size_request (label_comments, -1, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_comments), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_comments), 0, 0.5);

    sw_comments = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), sw_comments, 10, 160);
    gtk_widget_set_size_request (sw_comments, 790, 80);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw_comments), GTK_SHADOW_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw_comments), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    m_textview_comments = gtk_text_view_new ();
    m_buffer_comments = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comments));
//    gtk_widget_set_size_request (m_textview_comments, 930, -1);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_comments), GTK_WRAP_WORD_CHAR);
    gtk_container_add (GTK_CONTAINER (sw_comments), m_textview_comments);
    g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comments))), "insert-text", G_CALLBACK(HandleTextCommentInsert), this);

    button_clear_comments = gtk_button_new_with_mnemonic (_("Clear"));
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), button_clear_comments, 680, 125);
    gtk_widget_set_size_request (button_clear_comments, 120, 30);
    g_signal_connect(G_OBJECT(button_clear_comments), "clicked", G_CALLBACK(HandleBtnClearCommentsClicked), this);

    GtkWidget *labelOK = gtk_label_new_with_mnemonic (_("OK"));
    GtkWidget *imageOK = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
    GtkWidget *btnOK = create_button_icon (labelOK, imageOK);
    gtk_fixed_put(GTK_FIXED(fixed), btnOK, 850, 10);
    g_signal_connect(G_OBJECT(btnOK), "clicked", G_CALLBACK(HandleBtnEditOKClicked), this);

    GtkWidget *labelCancel = gtk_label_new_with_mnemonic (_("Cancel"));
    GtkWidget *imageCancel = gtk_image_new_from_stock ("gtk-cancel", GTK_ICON_SIZE_BUTTON);
    GtkWidget *btnCancel = create_button_icon (labelCancel, imageCancel);
    gtk_fixed_put(GTK_FIXED(fixed), btnCancel, 850, 80);
    g_signal_connect(G_OBJECT(btnCancel), "clicked", G_CALLBACK(HandleBtnEditCancelClicked), this);

#if 0
    //占空间，无实际作用
    label_comments = gtk_label_new (" ");
    gtk_fixed_put (GTK_FIXED (fixed_edit_report), label_comments, 10, 260);
    gtk_widget_set_size_request (label_comments, -1, 10);
#endif

    return m_frameEdit;
}

void ViewReport::SetIndication(const char* text)
{
    gtk_text_buffer_set_text(m_buffer_indication, text, -1);
}

void ViewReport::InsertIndication(const char* text)
{
    GtkTextIter iter_end;

    gtk_text_buffer_get_end_iter(m_buffer_indication, &iter_end);
    gtk_text_buffer_insert(m_buffer_indication, &iter_end, text, -1);

    //插入换行
    gtk_text_buffer_get_end_iter(m_buffer_indication, &iter_end);
    gtk_text_buffer_insert(m_buffer_indication, &iter_end, "\n\n", -1);
}

void ViewReport::SetComments(const char* text)
{
    gtk_text_buffer_set_text(m_buffer_comments, text, -1);
}

void ViewReport::InsertComments(const char* text)
{
    GtkTextIter iter_end;

    gtk_text_buffer_get_end_iter(m_buffer_comments, &iter_end);
    gtk_text_buffer_insert(m_buffer_comments, &iter_end, text, -1);

    //插入换行
    gtk_text_buffer_get_end_iter(m_buffer_comments, &iter_end);
    gtk_text_buffer_insert(m_buffer_comments, &iter_end, "\n\n", -1);
}

void ViewReport::BtnTransferClicked(GtkButton *button)
{
    ViewTemplet::GetInstance()->CreateWindow();
}

void ViewReport::BtnClearIndicationClicked(GtkButton *button)
{
    GtkTextBuffer *buffer_indication = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_indication));
    gtk_text_buffer_set_text(buffer_indication, "", -1);
}

void ViewReport::BtnClearCommentsClicked(GtkButton *button)
{
    GtkTextBuffer *buffer_comments = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comments));
    gtk_text_buffer_set_text(buffer_comments, "", -1);
}

void ViewReport::BtnPrintClicked(GtkButton *button)
{
    PRINTF("Print Clicked\n");
 /*
  * SysGeneralSetting sys;
    int lang = sys.GetLanguage();

    PRINTF("Print Vec Sel is: \n");
    TestShowVec(m_vecNameSel);

    print_report(0, 0, lang);
   */
    ViewPrintPreview::GetInstance()->CreateWindow2();
}

#ifdef VET
void ViewReport::NotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
    ShowMeasureData(page_num);
    //LoadMeasureData(page_num);

}
#else
void ViewReport::NotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
    ShowMeasureData(page_num);
}
#endif
bool ViewReport::ArchiveReport(guint exam_id)
{
    gchar deststr[512];
/*
 SysGeneralSetting sys;
    int lang = sys.GetLanguage();

    PRINTF("Print Vec Sel is: \n");
    TestShowVec(m_vecNameSel);
    print_report(0, 0, lang);
     */
    sprintf(deststr, "%s/%d", STORE_PATH, 0);
    ViewPrintPreview::GetInstance()->CreatePages(deststr);

    m_vecNameSel.clear();
    m_vecNameTmp.clear();

    for(int i=0;i<7;i++)
        m_grade[i] = 0;

    return true;
}

/*
 * Return: a newly-allocated string that must be freed, if the report has not archived, return NULL
 */
char* ViewReport::LoadReport(guint exam_id)
{
    gchar *strPath, strID[10];
    gchar *basename = g_path_get_basename (PRINTIMAGE);

    sprintf(strID, "%d", exam_id);
    strPath = g_build_filename(STORE_PATH, strID, basename, NULL);
    if(CheckFileName(strPath) != 1)
    {
        g_free(basename);
        return NULL;
    }

    g_free(basename);
    return strPath;
}

char* ViewReport::GetArchivePath(guint exam_id)
{
    gchar *strPath, strID[10];

    sprintf(strID, "%d", exam_id);
    strPath = g_build_filename(STORE_PATH, strID, "/", NULL);

    return strPath;
}

void ViewReport::BtnSaveClicked(GtkButton *button)
{
    if (g_keyInterface.Size() == 1)
        SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
}

void ViewReport::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

void ViewReport::SetPatientInfo(void)
{
//  char str[255];
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    PRINTF("%s, %s, %s)\n", info.p.name.first.c_str(), info.p.name.mid.c_str(), info.p.name.last.c_str());

    string name;
    string hospital;
    string sex;
    string examDate;
    string age;
    SysGeneralSetting sgs;
    int lang = sgs.GetLanguage();
#ifdef VET
    name = info.p.animal_name;
    string species;
    string owner = info.p.owner_name;
    GetAnimalSpeciesString(info.p.species, species);
#else
    if (!lang)
        name = info.p.name.first + " " + info.p.name.mid + " " + info.p.name.last;
    else
        name = info.p.name.last + " " + info.p.name.first + " " + info.p.name.mid;
#endif
    sgs.GetHospital(hospital);
    gtk_entry_set_text (GTK_ENTRY(m_entryHos), hospital.c_str());
    gtk_entry_set_text (GTK_ENTRY(m_entryName), name.c_str());
    gtk_entry_set_text (GTK_ENTRY(m_entryID), info.p.id.c_str());
    gtk_entry_set_text (GTK_ENTRY(m_entrySID), info.e.examNum.c_str());
#ifdef VET
    gtk_entry_set_text (GTK_ENTRY(m_entryAnimalName), name.c_str());
    gtk_entry_set_text (GTK_ENTRY(m_entryOwnerName), owner.c_str());
    gtk_entry_set_text (GTK_ENTRY(m_entrySpecies), species.c_str());
#else
    gtk_entry_set_text (GTK_ENTRY(m_entryName), name.c_str());
#endif
//  int date_format = sgs.GetDateFormat();
//  if(date_format == 0)
//      sprintf(str, "%d-%d-%d", info.e.examDate.year, info.e.examDate.month, info.e.examDate.day);
//  else if(date_format == 1)
//      sprintf(str, "%d-%d-%d", info.e.examDate.month, info.e.examDate.day, info.e.examDate.year);
//  else
//      sprintf(str, "%d-%d-%d", info.e.examDate.day, info.e.examDate.month, info.e.examDate.year);
    GetDateString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, examDate);
    gtk_entry_set_text (GTK_ENTRY(m_entryDate), examDate.c_str());
//  if(!info.e.examType.empty())
//      gtk_entry_set_text (GTK_ENTRY(m_entryItem), info.e.examType.c_str());
//  else
//  {
    string s;
    TopArea::GetInstance()->GetCheckPart(s);
    gtk_entry_set_text (GTK_ENTRY(m_entryItem), _(s.c_str()));
    //  }
    gtk_entry_set_text (GTK_ENTRY(m_entryUDr), info.e.examDoctor.c_str());
    gtk_entry_set_text (GTK_ENTRY(m_entryRDr), info.e.reportDoctor.c_str());
//  switch(info.p.sex)
//  {
//      case 0: sprintf(str, _("Female"));  break;
//      case 1: sprintf(str, _("Male"));    break;
//      case 2: sprintf(str, _("Other"));   break;
//  }
    GetSexString(info.p.sex, sex);
    gtk_entry_set_text (GTK_ENTRY(m_entrySex), sex.c_str());
//  switch(info.p.ageUnit)
//  {
//      case 0: //year
//          sprintf(str, "%d%s", info.p.age, _("Y"));
//          break;
//      case 1: //month
//          sprintf(str, "%d%s", info.p.age, _("M"));
//          break;
//      case 2: //day
//          sprintf(str, "%d%s", info.p.age, _("D"));
//          break;
//      default:
//          sprintf(str, "%d%s", info.p.age, _("Y"));
//          break;
//  }
    GenAgeStr(info.p.age, info.p.ageUnit, age);
    gtk_entry_set_text (GTK_ENTRY(m_entryAge), age.c_str());
}

GtkWidget* ViewReport::CreateSW2NB(GtkWidget *notebook, gint num, const char *title)
{
    GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (sw);
    gtk_container_add (GTK_CONTAINER (notebook), sw);
    gtk_notebook_set_tab_label_packing (GTK_NOTEBOOK (notebook), sw,
            TRUE, TRUE, GTK_PACK_START);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_IN);

    GtkWidget *label= gtk_label_new (title);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), num), label);

    GtkWidget *viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_hide (viewport);
    gtk_container_add (GTK_CONTAINER (sw), viewport);
    return viewport;
}

CalcNode_t* CalcArrListHead = NULL;
void ViewReport::DefaultCalcArr(void)
{
    CalcNode_t *p = CalcArrListHead;
    while(p != NULL)
    {
        CalcArrListHead = p->next;
        free(p);
        p = CalcArrListHead;
    }
    CalcArrListHead = NULL;
    PRINTF("DefaultCalcArr()\n");
}

void ViewReport::ShowAllCalcArr(void)
{
    int count = 0;
    CalcNode_t *p = CalcArrListHead;
    while(p != NULL)
    {
        printf("ShowAllCalcArr: id = %d, title = %s, value = %s\n", p->id, p->title, p->value);
        p = p->next;
        count++;
    }
    printf("ShowAllCalcArr: count = %d\n", count);
    return;
}

int ViewReport::GetCountFromCalcArr(void)
{
    int count = 0;
    CalcNode_t *p = CalcArrListHead;
    while(p != NULL)
    {
        if(p->check)
        {
            count++;
        }
        p = p->next;
    }
    return count;
}

bool ViewReport::GetItemFromCalcArr(int seq, CalcNode_t *result)
{
    int count = 0;
    CalcNode_t *p = CalcArrListHead;
    memset(result, 0, sizeof(CalcNode_t));
    while(p != NULL)
    {
        if(p->check)
        {
            if(seq == count)
            {
                memcpy(result, p, sizeof(CalcNode_t));
                return true;
            }
            count++;
        }
        p = p->next;
    }
    return false;
}

int ViewReport::SearchIDFromCalcArr(gint stamp, gint row)
{
  //  ShowAllCalcArr();
    CalcNode_t *p = CalcArrListHead;
    while(p != NULL)
    {
        if(stamp == p->stamp && row == p->row)
        {
            return p->id;
        }
        else
        {
            p = p->next;
        }
    }
    PRINTF("failed SearchIDFromCalcArr( stamp = %d, row = %d)\n", stamp, row);
    return -1;
}

bool ViewReport::SetCheckToCalcArr(int id, char *title, bool fixed)
{
    CalcNode_t *p = CalcArrListHead;
    while(p != NULL)
    {
        if((p->id == id)&&(!strcmp(p->title, title)))
        {
            p->check = fixed;
            return true;
        }
        p = p->next;
    }
    return false;
}
bool ViewReport::GetCheckFromCalcArr(int id, const char *title)
{
    if(id >= USER_START)
        return true;
    else
        return ViewSystem::GetInstance()->CheckFlagFromReportTemplet(id);
}

void ViewReport::SetIDToCalcArr(int stamp, int row, bool fixed, int id, int section, const char *title, const char *value, const char *gw, const char *edcb)
{
    CalcNode_t *p = CalcArrListHead;
    while(NULL != p)
    {
        if((((id >= OB_AFI_LUQ)&&(id <= OB_AFI_RLQ))||(id == p->id)) && !strcmp(title, p->title))
        {
            p->row = row;
            p->stamp = stamp;
            p->check = fixed;
            if(value)strcpy(p->value, value);
            if(gw)strcpy(p->gw, gw);
            if(edcb)strcpy(p->edcb, edcb);
            return;
        }
        p = p->next;
    }
    p = (CalcNode_t*)malloc(sizeof(CalcNode_t));
    if(NULL != p)
    {
        memset(p, 0, sizeof(CalcNode_t));
        p->id = id;
        p->section = section;
        p->next = NULL;
        p->row = row;
        p->stamp = stamp;
        p->check = fixed;
        if(title)strcpy(p->title, title);
        if(value)strcpy(p->value, value);
        if(gw)strcpy(p->gw, gw);
        if(edcb)strcpy(p->edcb, edcb);

        if(NULL == CalcArrListHead)
        {
            CalcArrListHead = p;
        }
        else
        {
            CalcNode_t *q = CalcArrListHead;
            while(q->next)
            {
                q = q->next;
            }
            q->next = p;
        }
    }
}

void CheckBoxToggleClicked(GtkCellRendererToggle *cell, gchar *path_str, gpointer data)
{
    GtkTreeIter iter;
    gboolean fixed;
    gint id;
    GtkTreeModel *model = (GtkTreeModel *)data;
    GtkTreePath *path = gtk_tree_path_new_from_string (path_str);

    /* get toggled iter */
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get (model, &iter, 0, &fixed, -1);

    /* do something with the value */
    fixed ^= 1;

    gchar *title = NULL, *value= NULL, *gw= NULL, *edcb= NULL;
    gtk_tree_model_get (model, &iter, 1, &title, 2, &value, -1);
    ViewReport *ptrRes = ViewReport::GetInstance();
    id = ptrRes->SearchIDFromCalcArr(iter.stamp, atoi((char*)path_str));
#ifdef EMP_322
    if(!((id >= OB_AFI_LUQ && id <= OB_AFI_RLQ)||(id >= OB_RA_DIMEN && id <= OB_MPA_DIAM)||(id >= OB_EAR && id <= OB_SCAPULA)))

#else
    if(!((id >= OB_AFI_LUQ && id <= OB_AFI_RLQ)||(id >= OB_RA_DIMEN && id <= OB_THORACIC_AO)||(id >= OB_EAR && id <= OB_SCAPULA)))
#endif
    {
        if(gtk_tree_model_get_n_columns (model) >= 7)
            gtk_tree_model_get (model, &iter, 6, &gw, 7, &edcb, -1);
    }

    if(ptrRes->SetCheckToCalcArr(id, title, fixed))
    {
        gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, fixed,-1);
    }

    /* clean up */
    if(path)gtk_tree_path_free(path);
    if(title)g_free(title);
    if(value)g_free(value);
    if(gw)g_free(gw);
    if(edcb)g_free(edcb);
}

gint ViewReport::AddResultToObTree( GtkWidget *treeview, ResultObTable result, int fetal, int row)
{
    gchar  Value[256], v[3][256];
    memset(v, 0, sizeof(v));
    memset(Value, 0, sizeof(Value));

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);

    sprintf(Value, "%.2f%s", result.data[0], result.units);

    for(gint t = 0; t<3; t++)
    {
        if( (result.data[1+t] != INVALID_VAL) && (result.data[1+t] != INVALID_VAL_GW) )
        {
            sprintf(v[t], "%.2f", result.data[1+t]);
        }
        else
        {
            sprintf(v[t], "%s", "  ");
        }
    }
    bool fixed = GetCheckFromCalcArr(result.id, result.title);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            C_OB_COL, fixed,
            T_OB_COL, result.title,
            V_OB_COL, Value,
            V1_OB_COL, v[0],
            V2_OB_COL, v[1],
            V3_OB_COL, v[2],
                    -1);

    if(result.GW)gtk_list_store_set(GTK_LIST_STORE(model), &iter, GW_OB_COL, result.GW, -1);
    if(result.EDCB)gtk_list_store_set(GTK_LIST_STORE(model), &iter, EDCB_OB_COL, result.EDCB, -1);

    SetIDToCalcArr(iter.stamp,  row, fixed, result.id, result.section, result.title, Value, result.GW, result.EDCB);
    return ++row;
}

gint ViewReport::AddResultToTree(GtkWidget *treeview, ResultTable result, bool side, gint row)
{
    char  Value[256], v[3][256];
    memset(v, 0, sizeof(v));
    memset(Value, 0, sizeof(Value));

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    //printf("row= %d, title=%s, value = %.2f, v1 = %.2f, v2 = %.2f, v3 = %.2f\n", row, result.title, result.data[0], result.data[1], result.data[2], result.data[3]);

    sprintf(Value, "%.2f%s", result.data[0], result.units);

    for(gint t = 0; t<3; t++)
    {
        if( (result.data[1+t] != INVALID_VAL) && (result.data[1+t] != INVALID_VAL_GW) )
        {
            sprintf(v[t], "%.2f", result.data[1+t]);
        }
        else
        {
            sprintf(v[t], "%s", "  ");
        }
    }

    bool fixed = GetCheckFromCalcArr(result.id, result.title);
    //printf("title = %s, id = %d, fixed = %s\n", result.title, result.id, fixed ? "True" : "False");
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    C_L_GEN_COL, fixed,
                    T_L_GEN_COL, result.title,
                    V_L_GEN_COL, Value,
                    V1_L_GEN_COL, v[0],
                    V2_L_GEN_COL, v[1],
                    V3_L_GEN_COL, v[2],
                    -1);

    SetIDToCalcArr(iter.stamp,  row, fixed, result.id, result.section, result.title, Value, NULL, NULL);

    return ++row;
}

void ViewReport::MapHTable(int table[][CALC_MAX], int currItem, int selectItem, int secStart, int *position, CalcInfoP* ptrInfo)
{
    int i = 0, k = 0;
    int max = 0;
    const CalcResultInfo *ptrRes;

    while((*ptrInfo)[i])
    {
        max = 0;
        ptrRes = (*ptrInfo)[i];
        i++;

        k = 0;
        while (ptrRes->pars[k] != NO_PAR)
        {
            max = max > ptrRes->pars[k] ? max : ptrRes->pars[k];
            k++;
        }

    if (max == currItem)
        {
            table[selectItem-secStart][*position] = ptrRes->item;
            (*position)++;
            if (ptrRes->ptrHCalcInfo != NULL)
                MapHTable(table, ptrRes->item, selectItem, secStart, position, (CalcInfoP*)(ptrRes->ptrHCalcInfo));
        }
    }
}
void ViewReport::InitMapTable(int table[][CALC_MAX], int start, int end, const SingleItemInfo info[])
{
    int i, j, k, max;
    CalcInfoP *ptrInfo;
    const CalcResultInfo *ptrRes;
    bool flag;
    int position = 0;

    for(i=0; i<end-start; i++)
        {
        position = 0;
        flag = FALSE;
        ptrInfo = (CalcInfoP*)(info[i].ptrCalcInfo);
        if(ptrInfo)
        {
            j = 0;
            while((*ptrInfo)[j])
            {
                max = 0;
                ptrRes = (*ptrInfo)[j];
                j++;
                //查找计算相关测量项的最大枚举值
                for(k=0; k<CALC_PAR_NUMS; k++)
                {
                    if(ptrRes->pars[k] >= CALC_START)
                        continue;
                    if(ptrRes->pars[k] == NO_PAR)
                        break;
                    max = max > ptrRes->pars[k] ? max : ptrRes->pars[k];
                }
                //判断最大枚举值是否为当前测量项的枚举值
                if(max == i+start)
                {
                    table[max-start][position] = ptrRes->item;
                    position++;
                    if (ptrRes->ptrHCalcInfo != NULL)
                        MapHTable(table, ptrRes->item, max, start, &position, (CalcInfoP*)(ptrRes->ptrHCalcInfo));
                }

            }
        }
    }
}

int GetMeaScale(int type, int n)
{
/*  switch(type)
    {
        case P12T:
            if(n==0 || n==2)
                return 10;
            else if(n==1)
                return 1;
            break;
        case RI:
            if(n==0 || n==1)
                return 10;
            else if(n==2)
                return 1;
        case SD:
            if(n==0 || n==1)
                return 10;
            else if(n==2 || n==3)
                return 1;
        case PGMAX:
            if(n==0)
                return 10;
            else if(n==1)
                return 100;

        case SIMPSONS:
            if(n==0)
                return 100;
            else if(n==1)
                return 10;
            else
                return 1;

        case AL:
            if(n==0)
                return 100;
            else if(n==1)
                return 10;
            else
                return 1;

        default:
            return MeaUnitScale[type];
    }
*/
    return 1;
}

/*
 * @brief Check the item flag type
 * @retval 0: general, 1: GW, 2: EDCB, 3: EFW
 */
int ViewReport::CheckItemType(int i)
{
//hlx
#ifdef VET
    if (i>=ANOB_DOG_GSD_GW && i<=ANOB_EQUINE_EED_EDCB)
    {
        if (i==ANOB_DOG_GSD_GW || i==ANOB_DOG_CRL_GW || i==ANOB_DOG_HD_GW || i==ANOB_DOG_BD_GW||
                i==ANOB_CAT_HD_GW || i==ANOB_CAT_BD_GW ||
                i==ANOB_SHEEP_CRL_GW ||
                i==ANOB_SWINE_HL_GW || i==ANOB_SWINE_SL_GW||
                i==ANOB_BOVINE_KCRL_GW || i==ANOB_BOVINE_WCRL_GW||i==ANOB_BOVINE_HCRL_GW||ANOB_BOVINE_BBD_GW||
                i==ANOB_BOVINE_BTD_GW||i==ANOB_BOVINE_BUD_GW||
                i==ANOB_EQUINE_GSD_GW||i==ANOB_EQUINE_ERD_GW||i==ANOB_EQUINE_ESD_GW||i==ANOB_EQUINE_EED_GW)
            return 1;
        else
            return 2;
    }
#else
    if(i>=OB_EFW_HADLOCK1 && i<=OB_EFW_TOKYO) //EFW
        return 3;
    //  else if(i>=OB_CER_GW && i<=OB_OFD_EDCB) //GW or EDCB
    else if(i>=OB_CER_GW && i<=OB_FTA_EDCB) //GW or EDCB
    {
        if(i==OB_CER_GW || i==OB_HL_GW || i==OB_BPD_GW || i==OB_FL_GW || i==OB_OOD_GW || i==OB_TIBIA_GW ||
                i==OB_TTD_GW || i==OB_ULNA_GW || i==OB_CRL_GW || i==OB_GS_GW || i==OB_AC_GW || i==OB_HC_GW ||
                i==OB_TAD_GW || i==OB_APAD_GW || i==OB_THD_GW || i==OB_OFD_GW || i==OB_FTA_GW )
            return 1;
        else if(i==OB_CER_EDCB || i==OB_HL_EDCB || i==OB_BPD_EDCB || i==OB_FL_EDCB || i==OB_OOD_EDCB || i==OB_TIBIA_EDCB ||
                i==OB_TTD_EDCB || i==OB_ULNA_EDCB || i==OB_CRL_EDCB || i==OB_GS_EDCB || i==OB_AC_EDCB || i==OB_HC_EDCB ||
                i==OB_TAD_EDCB || i==OB_APAD_EDCB || i==OB_THD_EDCB || i==OB_OFD_EDCB || i==OB_FTA_EDCB )
            return 2;
        else
            return 0;
    }
#endif
    return 0;
}
GtkTreeView* ViewReport::create_ob_treeview(GtkTreeModel *model)
{
    GtkWidget *tree = gtk_tree_view_new_with_model(model);
    g_object_set(G_OBJECT(tree),
            "can-focus", FALSE,
            "enable-search", FALSE,
            "headers-clickable", FALSE,
            "headers-visible", FALSE,
            "rules-hint", TRUE,
            NULL);

    gint i;
    gint w[10] = {155, 100, 65, 65, 65, 65, 65};

    for(i=0; i<N_OB_COLS; i++)
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        g_object_set(G_OBJECT(renderer),
                "height", 30,
                NULL);

        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
        g_object_set(G_OBJECT(column),
                "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
                "fixed-width", w[i],
                NULL);
    }

    return GTK_TREE_VIEW(tree);
}

GtkTreeView* ViewReport::create_ob_afi_treeview(GtkTreeModel *model)
{
    GtkWidget *tree = gtk_tree_view_new_with_model(model);
    g_object_set(G_OBJECT(tree),
            "can-focus", FALSE,
            "enable-search", FALSE,
            "headers-clickable", FALSE,
            "headers-visible", FALSE,
            "rules-hint", TRUE,
            NULL);

    gint i;
    gint w[10] = {155, 100, 65, 65, 65, 65, 65};

    for(i=0; i<N_OB_AFI_COLS; i++)
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        g_object_set(G_OBJECT(renderer),
                "height", 30,
                NULL);

        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
        g_object_set(G_OBJECT(column),
                "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
                "fixed-width", w[i],
                NULL);
    }

    return GTK_TREE_VIEW(tree);
}
GtkWidget* ViewReport::CreateObTable(int start, int end, int map_table[][CALC_MAX], int map_start, int fetal)
{
    MeaResult *ptrRes = MeaResult::GetInstance();
    ResultObTable result;
    int ret;
    int i, j, k, m, n;
    double value[MEA_MULTI], allvalue[MEA_MULTI][MEA_TIMES];
    const SingleItemInfo *sInfo;
    const MultiItemInfo *mInfo;
    char GW[20], EDCB[20], strTmp[100];
    int scale = 1;
    gint rows = 0;
    GtkListStore *store;
    GtkTreeModel *model = NULL;
    GtkWidget *treeview = NULL;

    //修改 动物超 产科
    //hlx
#ifdef VET
    store = gtk_list_store_new(N_OB_COLS,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING);
#else
    if(start != OB_AFI_LUQ && start != OB_RA_DIMEN && start != OB_EAR)
    {
        store = gtk_list_store_new(N_OB_COLS,
                G_TYPE_BOOLEAN,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING);
    }
    else
    {
        store = gtk_list_store_new(N_OB_AFI_COLS,
                G_TYPE_BOOLEAN,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING);
    }
#endif
    model = GTK_TREE_MODEL (store);
    m_treeview_ob = treeview = gtk_tree_view_new();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

    if (model != NULL)
    {
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    }

    for(i=start; i<end; i++)
    {
        //clear data
        for(m=0; m<MEA_MULTI; m++)
        {

            value[m] = INVALID_VAL;
            for(n=0; n<MEA_TIMES; n++)
                allvalue[m][n] = INVALID_VAL;
        }
        for(m=0; m<MEA_TIMES+1; m++)
            result.data[m] = INVALID_VAL;
        result.GW = result.EDCB = NULL;

        //get value
        ret = ptrRes->GetValue(i, value, allvalue, IN_REPORT_MEAN, fetal);
        if(ret!=MEA_SUCCESS)
            continue;

        ret = ptrRes->GetValue(i, value, allvalue, IN_REPORT_ALL, fetal);
        if(ret!=MEA_SUCCESS)
            continue;

        MeasureMan::GetInstance()->GetMeasureString(i, &sInfo, &mInfo);
//for user
        if ((i < USER_END) && (i >= USER_START))
        {
            if(IsETypeInSection(i, OB_M) != true)
                continue;
        }

#ifdef VET

        result.id = i;
        result.section = ANOB_M;
#else

        result.id = i;
        result.section = OB_M;
        if(2 == fetal)result.id = (result.id - OB_MEA_START) + MAX_CALC_COUNT; //fetal 2, the id  should add to the array end.
#endif
        if((*sInfo).unitItem < UNIT_END)
        {
            string units;
            double coeffi =1.0;
            MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (*sInfo).unitItem);
            scale = GetMeaScale((*sInfo).meaType, 0);
            result.data[0] = value[0]*coeffi / scale;
            result.title= _((*sInfo).title);
            result.units = units.c_str();
            for(j=0; j<MEA_TIMES; j++)
            {
                if( (allvalue[0][j] != INVALID_VAL) && (allvalue[0][j] != INVALID_VAL_GW) )
                {
                    result.data[j+1] = allvalue[0][j]*coeffi/scale;
                }
            }
            //hlx

#ifdef VET
            j = 0;
#else
            for(j=0; j<CALC_MAX; j++)
#endif
            {
                if(CheckItemType(map_table[i-map_start][j]) == 1)
                {
                    double calc_val;
#ifdef VET
                    if(ptrRes->CalcGetValue(&calc_val, map_table[i-map_start][j], ANOB_M, fetal)==MEA_SUCCESS)
#else
                        if(ptrRes->CalcGetValue(&calc_val, map_table[i-map_start][j], OB_M, fetal)==MEA_SUCCESS)
#endif
                        {
                            sprintf(GW, "%dw%dd", ((int)calc_val)/7, ((int)calc_val)%7);
                            result.GW = GW;

                            if(map_table[i-map_start][1] != -1)
                            {
#ifdef VET
                                if(ptrRes->CalcGetValue(&calc_val, map_table[i-map_start][j+1], ANOB_M, fetal)==MEA_SUCCESS)
#else
                                    if(ptrRes->CalcGetValue(&calc_val, map_table[i-map_start][j+1], OB_M, fetal)==MEA_SUCCESS)
#endif
                                    {
                                        SysGeneralSetting sysSetting;
                                        m_dateFormat = sysSetting.GetDateFormat();

                                        if (m_dateFormat == 0)
                                            sprintf(EDCB, "%d/%d/%d", ((int)calc_val)/10000, ((int)calc_val)%10000/100, (int)calc_val%100);
                                        else if(m_dateFormat == 1)
                                            sprintf(EDCB, "%d/%d/%d", ((int)calc_val)%10000/100, ((int)calc_val)%100, (int)calc_val/10000);
                                        else if(m_dateFormat == 2)
                                            sprintf(EDCB, "%d/%d/%d", ((int)calc_val)%100, ((int)calc_val)%10000/100, (int)calc_val/10000);
                                        else
                                            sprintf(EDCB, "%d/%d/%d", ((int)calc_val)/10000, ((int)calc_val)%10000/100, (int)calc_val%100);

                                        result.EDCB = EDCB;
                                    }
                            }
                        }
                }
            }
            rows = AddResultToObTree(m_treeview_ob, result, fetal, rows);
        }
        else
        {
        //  for(k=0; k<MEA_MULTI; k++)
            k = 0;
            while ((*mInfo).titleUnit[k].title[0] != '\0')
            {
                scale = GetMeaScale((*mInfo).meaType, k);
                sprintf(strTmp, "%s %s", _((*mInfo).itemTitle), _((*mInfo).titleUnit[k].title));
                if(strcmp((*mInfo).itemTitle, "Sac") == 0 )
                {
                    sprintf(strTmp, "%s", _((*mInfo).titleUnit[k].title));
                }
                else
                {
                    sprintf(strTmp, "%s %s", _((*mInfo).itemTitle), (*mInfo).titleUnit[k].title);

                }
                string units;
                double coeffi = 1.0;
                MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (*mInfo).titleUnit[k].unitItem);
                result.title = strTmp;//(*mInfo).titleUnit[k].title;
                result.units = units.c_str();
                result.data[0] = value[k]*coeffi/scale;
                for(j=0; j<MEA_TIMES; j++)
                {
                    if( (allvalue[k][j] != INVALID_VAL) && (allvalue[k][j] != INVALID_VAL_GW) )
                    {
                        //  PRINTF("T%dM%d=%.2f%s  ", j, k, allvalue[k][j], result.units);
                        PRINTF("T%dM%d=%.2f%s  ", j, k, allvalue[k][j], result.units);
                        result.data[j+1] = allvalue[k][j]*coeffi/scale;
                    }
                }
                rows = AddResultToObTree(m_treeview_ob, result, fetal, rows);

                k++;
            }
        }

        int calc_start;
        const PtrOfCalcInfo *ptrCalcInfo;

#ifdef VET
        calc_start = ANOB_CALC_START;
        ptrCalcInfo = AnOBCalcInfoPtr;
#else
        calc_start = OB_CALC_START;
        ptrCalcInfo = OBCalcInfoPtr;
#endif
        for(j=0; j<CALC_MAX; j++)
        {
            int temp = 0;
            if(strcmp(result.title, _("AFI1"))== 0)
            {
                temp = i+3;
            }
            else if(strcmp(result.title, _("AFI2"))== 0)
            {
                temp = i+2;
            }
            else if(strcmp(result.title, _("AFI3"))== 0)
            {
                temp = i+1;
            }
            else
            {
                temp = i;
            }

            if(map_table[temp-map_start][j] == -1)
                    break;

            //Filter EFW, GW, EDCB type
            if(CheckItemType(map_table[temp-map_start][j])==0)
            {
                int calcItem = map_table[temp-map_start][j] - calc_start;
                for(m=0; m<MEA_TIMES+1; m++)
                    result.data[m] = INVALID_VAL;
                string units;
                double coeffi = 1.0;
                MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (ptrCalcInfo + calcItem)->ptrCalcInfo->unitItem);
                result.title = _((ptrCalcInfo + calcItem)->ptrCalcInfo->title);
                result.units = units.c_str();
                result.GW = result.EDCB = NULL;
                double calc_val;
#ifdef VET
                if(ptrRes->CalcGetValue(&calc_val, map_table[temp-map_start][j], ANOB_M, fetal)==MEA_SUCCESS)
#else
                    if(ptrRes->CalcGetValue(&calc_val, map_table[temp-map_start][j], OB_M, fetal)==MEA_SUCCESS)
#endif
                    {
                        result.data[0] = calc_val*coeffi;
                        if((result.id == OB_AFI_LUQ)||(result.id == OB_AFI_LLQ)||(result.id == OB_AFI_RUQ)||(result.id == OB_AFI_RLQ))
                        {
                            bool exist = false;

                            for(int k = 1; k<(temp-i+1); k++)
                            {
                                //get value
                                ret = ptrRes->GetValue(i+k, value, allvalue, IN_REPORT_ALL, fetal);
                                if(ret == MEA_SUCCESS)
                                {
                                    exist = true;
                                    break;
                                }
                                ret = ptrRes->GetValue(i+k, value, allvalue, IN_REPORT_MEAN, fetal);
                                if(ret == MEA_SUCCESS)
                                {
                                    exist = true;
                                    break;
                                }
                            }
                            if(!exist)
                            {
                                rows = AddResultToObTree(m_treeview_ob, result, fetal, rows);
                            }
                        }
                        else
                            rows = AddResultToObTree(m_treeview_ob, result, fetal, rows);
                    }
            }
        }
    }

    if(rows > 0)
    {
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        //gint wide[] = {50, 155, 80, 55, 55, 55, 55, 55};
        gint wide[] = {20, 160, 85, 60, 60, 60, 60, 75};

        renderer = gtk_cell_renderer_toggle_new ();
        column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", C_OB_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (CheckBoxToggleClicked), (gpointer)model);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", wide[0], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("", renderer, "text", T_OB_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[1], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("Value"), renderer, "text", V_OB_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[2], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("1"), renderer, "text", V1_OB_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[3], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("2"), renderer, "text", V2_OB_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[4], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("3"), renderer, "text", V3_OB_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[5], NULL);
    //  gtk_cell_renderer_get_size(renderer, treeview, NULL, NULL, NULL, NULL, row_height);
    //  printf("ob=====row_height = %d\n", row_height);
        if(start != OB_AFI_LUQ && start != OB_RA_DIMEN && start != OB_EAR)
        {
            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(("GW"), renderer, "text", GW_OB_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[6], NULL);

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(("EDD"), renderer, "text", EDCB_OB_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[7], NULL);
        }
    }
    else
    {
        treeview = NULL;
    }
    g_object_unref(store);

    return GTK_WIDGET(treeview);
}

int ViewReport::GetSectionForDepartment(const char *department)
{
    int section = -1;//ABDO_M;

    if (strcmp(department, "Abdomen-General") == 0 || strcmp(department, "Abdomen-Arterial") == 0 || strcmp(department, "Abdomen-Venous") == 0)
        section = ABDO_M;
    else if (strcmp(department, "Urology") == 0)
        section = UR_M;
    else if (strcmp(department, "Cardiac") == 0)
        section = ADULT_M;
    else if (strcmp(department, "Fetal Cardio") == 0)
        section = FETAL_M;
    else if (strcmp(department, "Obstetrics-General") == 0 || strcmp(department, "Obstetrics-FetalHeart") == 0)
        section = OB_M;
    else if (strcmp(department, "Gynecology") == 0)
        section = GYN_M;
    else if (strcmp(department, "SmallPart-Glands") == 0 || strcmp(department, "SmallPart-Testicle") == 0
            || strcmp(department, "SmallPart-Thyroid") == 0 || strcmp(department, "SmallPart-Eye") == 0)
        section = SP_M;
    else if (strcmp(department, "Vascular-Carotid") == 0 || strcmp(department, "Vascular-LE") == 0 || strcmp(department, "Vascular-UE") == 0)
        section = VS_M;
    else if (strcmp(department, "TCD") == 0)
        section = TCD_M;
    else if (strcmp(department, "Orthopedic") == 0)
        section = ORTHO_M;
#ifdef VET
    else if (strcmp(department, "AnOB") == 0)
        section = ANOB_M;
    else if (strcmp(department, "TD") == 0)
        section = TD_M;
#endif

    return section;
}

bool ViewReport::IsETypeInSection(int etype, int Section)
{
         string department;
    CalcSetting::GetInstance()->GetDepartmentForCustomMeasure(etype, department);

    int sectionT = GetSectionForDepartment(department.c_str());
    if (sectionT == Section)
        return true;
    else
        return false;
}

GtkWidget* ViewReport::CreateGeneralTable(int section, int start, int end, int map_table[][CALC_MAX], int map_start)
{
    MeaResult *ptrRes = MeaResult::GetInstance();
    ResultTable result;
    int ret;
    int i, j, k, m, n;
    double value[MEA_MULTI], allvalue[MEA_MULTI][MEA_TIMES];
    const SingleItemInfo *sInfo;
    const MultiItemInfo *mInfo;
    char strTmp[100];
    int scale = 1;

    GtkWidget *lab_left, *lab_right, *vbox_left, *vbox_right;
    GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
    GtkListStore *store= gtk_list_store_new(N_GEN_COLS,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING);

    GtkTreeModel *model = NULL;

    model = GTK_TREE_MODEL (store);
    m_treeview_left = gtk_tree_view_new();

    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (m_treeview_left), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (m_treeview_left), TRUE);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (m_treeview_left));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

    if (model != NULL)
    {
        gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeview_left), model);
    }

    GtkListStore *store2= gtk_list_store_new(N_GEN_COLS,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING);

    GtkTreeModel *model2 = NULL;

    model2 = GTK_TREE_MODEL (store2);
    m_treeview_right = gtk_tree_view_new();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (m_treeview_right), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (m_treeview_right), TRUE);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (m_treeview_right));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

    if (model2 != NULL)
    {
        gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeview_right), model2);
    }

    int rows = 0;
    int rows_left = 0, rows_right = 0;
    for(i=start; i<end; i++)
    {
        //clear data
        for(m=0; m<MEA_MULTI; m++)
        {
            value[m] = INVALID_VAL;
            for(n=0; n<MEA_TIMES; n++)
                allvalue[m][n] = INVALID_VAL;
        }
        for(m=0; m<MEA_TIMES+1; m++)
            result.data[m] = INVALID_VAL;

        ret = ptrRes->GetValue(i, value, allvalue, IN_REPORT_MEAN);
        if(ret!=MEA_SUCCESS)
            continue;

        ret = ptrRes->GetValue(i, value, allvalue, IN_REPORT_ALL);
        if(ret!=MEA_SUCCESS)
            continue;

    //  result.title = MeasureMan::GetInstance()->GetMeasureString(i);
        MeasureMan::GetInstance()->GetMeasureString(i, &sInfo, &mInfo);

        //for user
         if ((i < USER_END) && (i >= USER_START))
        {
            if(IsETypeInSection(i, section) != true)
                continue;
        }

        result.id = i;
        result.section = section;
        if((*sInfo).unitItem < UNIT_END)
        {
            string units;
            double coeffi = 1.0;
            MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (*sInfo).unitItem);
            //  PRINTF("---------------------SingleItemInfo\n");
            scale = GetMeaScale((*sInfo).meaType, 0);
            //  PRINTF("scale = %d\n", scale);
            result.data[0] = value[0]*coeffi/scale;
            sprintf(strTmp, "%s", _((*sInfo).title));
            result.title = strTmp;
            //result.title = (*sInfo).title;
            result.units = units.c_str();

            for(j=0; j<MEA_TIMES; j++)
            {
                if( (allvalue[0][j] != INVALID_VAL) && (allvalue[0][j] != INVALID_VAL_GW) && (fabs(allvalue[0][j]) > ZERO))
                {
                    //  PRINTF("T%d=%.2f%s  ", j, allvalue[0][j], result.units);
                    result.data[j+1] = allvalue[0][j]*coeffi/scale;
                }
            }

            if(rows%2)
            {
                rows_right = AddResultToTree(m_treeview_right, result, m_side, rows_right);
            }
            else
            {
                rows_left = AddResultToTree(m_treeview_left, result, m_side, rows_left);
            }
            rows = rows_left+rows_right;
        }
        else
        {
            k = 0;
            while ((*mInfo).titleUnit[k].title[0] != '\0')
            {
                string units;
                double coeffi = 1.0;
                MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (*mInfo).titleUnit[k].unitItem);
                scale = GetMeaScale((*mInfo).meaType, k);
                sprintf(strTmp, "%s %s", _((*mInfo).itemTitle), _((*mInfo).titleUnit[k].title));
                result.title = strTmp;//(*mInfo).titleUnit[k].title;
                result.units = units.c_str();
                result.data[0] = value[k]*coeffi/scale;
                for(j=0; j<MEA_TIMES; j++)
                {
                    if( (allvalue[k][j] != INVALID_VAL) && (allvalue[k][j] != INVALID_VAL_GW) && (fabs(allvalue[k][j]) > ZERO))
                    {
                        //  PRINTF("T%dM%d=%.2f%s  ", j, k, allvalue[k][j], result.units);
                        result.data[j+1] = allvalue[k][j]*coeffi/scale;
                    }

                }

                //printf("test2:i = %d, title=%s, value = %.2f, v1 = %.2f, v2 = %.2f, v3 = %.2f\n",i, result.title, result.data[0], result.data[1], result.data[2], result.data[3]);
                if(section == ORTHO_M)
                {
                    if(rows_left > 2)
                    {
                        rows_right = AddResultToTree(m_treeview_right, result, m_side, rows_right);
                    }
                    else
                    {
                        rows_left = AddResultToTree(m_treeview_left, result, m_side, rows_left);
                    }
                }
                else
                {
                    if(rows%2)
                    {
                        rows_right = AddResultToTree(m_treeview_right, result, m_side, rows_right);
                    }
                    else
                    {
                        rows_left = AddResultToTree(m_treeview_left, result, m_side, rows_left);
                    }
                }
                rows = rows_left+rows_right;

                k++;
            }

        }

        //get calculate data
        int calc_start;
        const PtrOfCalcInfo *ptrCalcInfo;

        if(section == ORTHO_M)
        {
            gchar tmp[256];
            if ((value[0]>=60) && (value[1]<55)) //type = Ia
                sprintf(tmp,"  %s       %s" , "Type","Ia");
            else if ((value[0]>=60) && (value[1]>=55)&&(value[1]<77)) //type = Ib
                sprintf(tmp,"  %s       %s" , "Type","Ib");
            else if ((value[0]>=50) && (value[0]<60) && (value[1]>=55)&&(value[1]<77))//type = IIa or IIb
                sprintf(tmp,"  %s       %s" , "Type","IIa, IIb");
            else if ((value[0]>=43) && (value[0]<50) && (value[1]>=0) && (value[1]<=77)) //type = IId
                sprintf(tmp,"  %s       %s" , "Type","IIc");
            else if ((value[0]>=43) && (value[0]<50) && (value[1]>77)) //type = IId
                sprintf(tmp,"  %s       %s" , "Type","IId");
            else if ((value[0]>=0) && (value[0]<43) && (value[1]>77)) //type = III or IV
                sprintf(tmp,"  %s       %s" , "Type","III, IV");
            else
                sprintf(tmp,"  %s       %s" , "Type","XXX");

            if(rows > 2)
            {
                lab_right= create_label(tmp, 0, 30, NULL, NULL);
                rows_right++;
            }
            else
            {
                lab_left = create_label(tmp, 0, 30, NULL, NULL);
                rows_left++;
            }
            rows = rows_left+rows_right;
        }
        else
        {
            switch(section)
            {
                case ABDO_M:
                    calc_start = ABD_CALC_START;
                    ptrCalcInfo = AbdoCalcInfoPtr;
                    break;
                case ADULT_M:
                    calc_start = ADULT_CALC_START;
                    ptrCalcInfo = AdultCalcInfoPtr;
                    break;
                case UR_M:
                    calc_start = UR_CALC_START;
                    ptrCalcInfo = URCalcInfoPtr;
                    break;
                case GYN_M:
                    calc_start = GYN_CALC_START;
                    ptrCalcInfo = GYNCalcInfoPtr;
                    break;
                case SP_M:
                    calc_start = SP_CALC_START;
                    ptrCalcInfo = SPCalcInfoPtr;
                    break;
#ifndef EMP_322
                case VS_M:
                    calc_start = VS_CALC_START;
                    ptrCalcInfo = VSCalcInfoPtr;
                    break;
#endif
                case FETAL_M:
                    calc_start = FETAL_CALC_START;
                    ptrCalcInfo = FetalCalcInfoPtr;
                    break;
                default:
                    ptrCalcInfo = NULL;
                    break;
            }

            if(ptrCalcInfo == NULL) continue;

            for(j=0; j<CALC_MAX; j++)
            {
                if(map_table[i-map_start][j] == -1)
                {
                    break;
                }
                else
                {
                    int calcItem = map_table[i-map_start][j] - calc_start;
                    for(m=0; m<MEA_TIMES+1; m++)result.data[m] = INVALID_VAL;
                    string units;
                    double coeffi = 1.0;
                    MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units, (ptrCalcInfo + calcItem)->ptrCalcInfo->unitItem);
                    result.title = _((ptrCalcInfo + calcItem)->ptrCalcInfo->title);
                    result.units = units.c_str();
                //  PRINTF("i=%d\n", i-map_start);
                    double calc_val;
                    if(ptrRes->CalcGetValue(&calc_val, map_table[i-map_start][j], section)==MEA_SUCCESS)
                    {
                    //  PRINTF("\n##################title= %s\n calc_val = %.2f\n", result.title, calc_val);
                    //  result.title = "";
                        result.data[0] = calc_val*coeffi;

                        if(rows%2)
                        {
                            rows_right = AddResultToTree(m_treeview_right, result, false, rows_right);
                        }
                        else
                        {
                            rows_left = AddResultToTree(m_treeview_left, result, false, rows_left);
                        }
                        rows = rows_left+rows_right;
                    }
                }
            }
        }

    }
    if(rows_left > 0)
    {
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        //gint wide[] = {50, 155, 80, 55, 55, 55};
        gint wide[] = {20, 165, 90, 60, 60, 60};

        renderer = gtk_cell_renderer_toggle_new ();
        column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", C_L_GEN_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_left), column);
        g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (CheckBoxToggleClicked), (gpointer)model);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", wide[0], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("", renderer, "text", T_L_GEN_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_left), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[1], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("Value"), renderer, "text", V_L_GEN_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_left), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[2], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("1"), renderer, "text", V1_L_GEN_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_left), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[3], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("2"), renderer, "text", V2_L_GEN_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_left), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[4], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("3"), renderer, "text", V3_L_GEN_COL, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_left), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[5], NULL);

        gtk_cell_renderer_get_size(renderer, m_treeview_left, NULL, NULL, NULL, NULL, &row_height);

        if((section == ORTHO_M)&&((i < USER_END) && (i >= USER_START)))
        {
            vbox_left = gtk_vbox_new(FALSE, 2);
            gtk_box_pack_start (GTK_BOX (vbox_left), m_treeview_left, FALSE, FALSE, 0);
            if(lab_left)
            {
                gtk_box_pack_start (GTK_BOX (vbox_left), lab_left, FALSE, FALSE, 0);
                gtk_misc_set_alignment(GTK_MISC(lab_left), 0.0, 0.5);
            }
            gtk_box_pack_start (GTK_BOX (hbox), vbox_left, FALSE, FALSE, 0);
        }
        else
        {
            gtk_box_pack_start (GTK_BOX (hbox), m_treeview_left, FALSE, FALSE, 0);
        }

        if(rows_right > 0)
        {
            renderer = gtk_cell_renderer_toggle_new ();
            column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", C_L_GEN_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_right), column);
            g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK(CheckBoxToggleClicked), (gpointer)model2);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", wide[0], NULL);

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes("", renderer, "text", T_L_GEN_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_right), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[1], NULL);

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(("Value"), renderer, "text", V_L_GEN_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_right), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[2], NULL);

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(("1"), renderer, "text", V1_L_GEN_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_right), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[3], NULL);

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(("2"), renderer, "text", V2_L_GEN_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_right), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[4], NULL);

            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes(("3"), renderer, "text", V3_L_GEN_COL, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeview_right), column);
            g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[5], NULL);

            if((section == ORTHO_M)&&((i < USER_END) && (i >= USER_START)))
            {
                vbox_right = gtk_vbox_new(FALSE, 2);
                gtk_box_pack_start (GTK_BOX (vbox_right), m_treeview_right, FALSE, FALSE, 0);
                if(lab_right)
                {
                    gtk_box_pack_start (GTK_BOX (vbox_right), lab_right, FALSE, FALSE, 0);
                    gtk_misc_set_alignment(GTK_MISC(lab_right), 0.0, 0.5);
                }
                gtk_box_pack_start (GTK_BOX (hbox), vbox_right, FALSE, FALSE, 0);
            }
            else
            {
                gtk_box_pack_start (GTK_BOX (hbox), m_treeview_right, FALSE, FALSE, 0);
            }
        }
    }
    else
    {
        hbox = NULL;
    }

    return GTK_WIDGET(hbox);
}

int ViewReport::AddResultToTree2(GtkWidget * treeview, char *title, char *value, int id, int row)
{
    bool fixed = GetCheckFromCalcArr(id, title);
   //printf("title = %s, id = %d, fixed = %s\n", title, id, fixed ? "True" : "False");

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    0, fixed,
                    1, title,
                    2, value,
                    -1);

    SetIDToCalcArr(iter.stamp,  row, fixed, id, OB_M, title, value, NULL, NULL);
    return ++row;
}

int ViewReport::AddResultToTree3(GtkWidget * treeview, char *title, char *value, char *sd, int id, int row)
{
    bool fixed = GetCheckFromCalcArr(id, title);
   //printf("title = %s, id = %d, fixed = %s\n", title, id, fixed ? "True" : "False");

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW(treeview));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                    0, fixed,
                    1, title,
                    2, value,
                    3, sd,
                    -1);

    SetIDToCalcArr(iter.stamp,  row, fixed, id, OB_M, title, value, NULL, NULL);
    return ++row;
}

void ViewReport::AddAverRes(GtkWidget *fixed, guint *y, const char *data)
{
    guint x1 = 20, x2 = 40;
    int h = 30;
    double calc_val[2];
    char title[100], value[100], sd[100];
    MeaResult *ptrRes = MeaResult::GetInstance();
    gint rows = 0, id = 0;
    GtkListStore *store= gtk_list_store_new(4,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING);

    GtkTreeModel *model = GTK_TREE_MODEL (store);
    GtkWidget *treeview = gtk_tree_view_new();

    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

    if (model != NULL)
    {
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    }

    for(gint i=0; i<2; i++)
    {
        if(ptrRes->CalcGetAgwValue(calc_val, 0+i*1)==MEA_SUCCESS)
        {
            if(0 == i)
                id = AVERRES_START_ID;
            else
                id = AVERRES_START_ID + i*2;
            sprintf(title, "%s", "AUA");
            sprintf(value, " %dw%dd", ((int)calc_val[0])/7, ((int)calc_val[0])%7);
            rows = AddResultToTree3(treeview, title, value, "± 1w", id, rows);

            if(0 == i)
                id = AVERRES_START_ID + 1;
            else
                id = AVERRES_START_ID + i*2 +1;
            sprintf(title, "%s", "EDD(AUA)");
            SysGeneralSetting sysSetting;
            m_dateFormat = sysSetting.GetDateFormat();
            //printf("------------------EDD(AUA)  m_dateFormat = %d", m_dateFormat);
            if (m_dateFormat == 0)
                sprintf(value, "%d/%d/%d", ((int)calc_val[1])/10000, ((int)calc_val[1])%10000/100, (int)calc_val[1]%100);
            else if(m_dateFormat == 1)
                sprintf(value, "%d/%d/%d", ((int)calc_val[1])%10000/100, ((int)calc_val[1])%100, (int)calc_val[1]/10000);
            else if(m_dateFormat == 2)
                sprintf(value, "%d/%d/%d", ((int)calc_val[1])%100, ((int)calc_val[1])%10000/100, (int)calc_val[1]/10000);
            else
                sprintf(value, "%d/%d/%d", ((int)calc_val[1])/10000, ((int)calc_val[1])%10000/100, (int)calc_val[1]%100);

            rows = AddResultToTree3(treeview, title, value, "± 1w", id, rows);
        }
    }

    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    if(info.ob.LMPDate.year != 0 && info.ob.LMPDate.month != 0 && info.ob.LMPDate.day != 0)
    {
        calc_val[0] = MeaCalcFun::CalcGW_LMP(info.ob.LMPDate.year, info.ob.LMPDate.month, info.ob.LMPDate.day);
        sprintf(title, "%s", "GA(LMP)");
        sprintf(value, "%dw%dd",  ((int)calc_val[0])/7, ((int)calc_val[0])%7);
        id = AVERRES_START_ID +4;
        rows = AddResultToTree3(treeview, title, value, "± 1w", id, rows);

        char edcb[20];
        MeaCalcFun::CalcEDCB_LMP(info.ob.LMPDate.year, info.ob.LMPDate.month, info.ob.LMPDate.day, edcb);
        sprintf(title, "%s", "EDD(LMP)");
        sprintf(value, "%s",  edcb);
        id = AVERRES_START_ID +5;
        rows = AddResultToTree3(treeview, title, value, "± 1w", id, rows);

    }

    g_object_unref (model);

    if(rows > 0)
    {
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        gint wide[] = {20, 170, 90, 90};

        GtkWidget* titlebutton = create_label(_("<i><b>Average</b></i>"), 0, h, g_white, NULL);
        gtk_misc_set_alignment(GTK_MISC(titlebutton), 0.0, 0.5);
        gtk_fixed_put(GTK_FIXED(fixed), titlebutton, x1, *y);

        renderer = gtk_cell_renderer_toggle_new ();
        column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (CheckBoxToggleClicked), (gpointer)model);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", wide[0], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[1], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("Value"), renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[2], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes((_("SD")), renderer, "text", 3, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[3], NULL);

        gtk_fixed_put(GTK_FIXED(fixed), treeview, x2, *y+h);
        *y += (rows)*h + h + h/2;
    }
}
#ifdef VET
//添加肌腱 病灶面积 结果显示
//hlx
void ViewReport::AddTDRes(GtkWidget *fixed, guint *y, const char *title)
{
    guint x1 = 120;
    int h = 30;
    int w = 200;
    char tmp[100];
    double value;
    double affect;
    double total;
    GtkWidget *table = NULL;
    GtkWidget *lab;
    MeaResult *ptrRes = MeaResult::GetInstance();

    //if (ptrRes->CalcGetTDValue(&value) == MEA_SUCCESS)
    if (ptrRes->CalcGetTDValue(&total, &affect, &value) == MEA_SUCCESS)
    {
        if(!table)
            table = gtk_table_new(1, 8, FALSE);

        sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span> = %.2f%%</b>", _("Ratio"), value);
        lab = create_label(tmp, w, h, g_white, NULL);
        gtk_misc_set_alignment(GTK_MISC(lab), 0.0, 0.5);
        gtk_table_attach_defaults(GTK_TABLE(table), lab, 0+0*2, 1+0*2, 0, 1);

        if (value >=1 && value <=15)
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _("Slight"));
        }
        else if (value >=16 && value <= 25)
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _("Moderate"));
        }
        else if (value >25)
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _("Severe"));
        }
        else
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _(" "));
        }

        if (value >=1 && value <=15)
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _("Slight"));
        }
        else if (value >=16 && value <= 25)
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _("Moderate"));
        }
        else if (value >25)
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _("Severe"));
        }
        else
        {
            sprintf(tmp, "<b><span fgcolor=\"#00FFFF\">%s</span></b>", _(" "));
        }

        lab = create_label(tmp, w, h, g_white, NULL);
        gtk_misc_set_alignment(GTK_MISC(lab), 0.0, 0.5);
        gtk_table_attach_defaults(GTK_TABLE(table), lab, 0+1*2, 1+1*2, 0, 1);
    }

    if(table)
    {
        gtk_fixed_put(GTK_FIXED(fixed), table, x1, *y);
        *y += 2*h;
    }

}
#endif

void ViewReport::AddOBEfwTable(GtkWidget *fixed, guint *y, const char *labstr, int fetal)
{
    gint rows = 0, id = 0;
    GtkListStore *store= gtk_list_store_new(4,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING);

    GtkTreeModel *model = GTK_TREE_MODEL (store);
    GtkWidget *treeview = gtk_tree_view_new();

    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

    if (model != NULL)
    {
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    }

    guint x1 = 20, x2 = 40;
    int h = 30;
    MeaResult *ptrRes = MeaResult::GetInstance();
    const CalcResultInfo *EfwInfo[] = { &OBEfwHadlock1, &OBEfwHadlock2, &OBEfwHadlock3, &OBEfwHadlock4,
                                        &OBEfwShepard, &OBEfwHansmenn, &OBEfwTokyo };
    const char method[][20] = { "Hadlock1", "Hadlock2", "Hadlock3", "Hadlock4", "Shepard", "Hansmenn", "Tokyo" };

    int i = GetCurEfwMethod();

    double calc_val;
    if(ptrRes->CalcGetValue(&calc_val, EfwInfo[i]->item, OB_M, fetal)==MEA_SUCCESS)
    {
        char title[100], value[100], sd[100];

        if(1 == fetal)
        {
            id = EFW_START_ID;
        }
        else
        {
            id = EFW_START_ID + 1;
        }

        string units;
        double coeffi = 1.0;
        MeasureMan::GetInstance()->GetMeasureUnit(coeffi, units,EfwInfo[i]->unitItem);
        sprintf(title, "%s%s%s%s", _("EFW"), "(", method[i], ")");
        sprintf(value, "%.2f%s", calc_val*coeffi, units.c_str());
        sprintf(sd, "± %.2f%s", calc_val*0.1*coeffi, units.c_str());
        //sprintf(value, "%.2f%s", calc_val*1000, "g"); //units[EfwInfo[i]->unitItem]);
        //sprintf(sd, "± %.2f%s", calc_val*1000*0.1, "g");
        rows = AddResultToTree3(treeview, title, value, sd, id, rows);
    }

    g_object_unref (model);

    if(rows > 0)
    {
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        gint wide[] = {20, 170, 90, 90};

        GtkWidget* lab = create_label(labstr, 0, h, g_blue, NULL);
        gtk_misc_set_alignment(GTK_MISC(lab), 0.0, 0.5);
        gtk_fixed_put(GTK_FIXED(fixed), lab , x1, *y);

        renderer = gtk_cell_renderer_toggle_new ();
        column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (CheckBoxToggleClicked), (gpointer)model);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", wide[0], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[1], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("Value"), renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[2], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes((_("SD")), renderer, "text", 3, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[3], NULL);

        gtk_fixed_put(GTK_FIXED(fixed), treeview, x2, *y+h);
        *y += (rows)*h + h+ h/2;
    }
}

void ViewReport::AddTable(GtkWidget *fixed, guint *y, const char *title, int section, int start, int end, int map_table[][CALC_MAX], int fetal)
{
    guint rows = 0;
    guint x1 = 20;
    guint x2 = 40;
    int h = 30;
    int map_start;

    switch(section)
    {
        case ABDO_M:    map_start = ABD_MEA_START;      break;
        case ADULT_M:   map_start = ADULT_MEA_START;    break;
        case UR_M:      map_start = UR_MEA_START;       break;
        case OB_M:      map_start = OB_MEA_START;       break;
        case GYN_M:     map_start = GYN_MEA_START;      break;
        case SP_M:      map_start = SP_MEA_START;       break;
#ifndef EMP_322
        case VS_M:      map_start = VS_MEA_START;       break;
#endif
        case FETAL_M:   map_start = FETAL_MEA_START;    break;
#ifndef EMP_322
        case TCD_M:     map_start = TCD_MEA_START;      break;
#endif
        case ORTHO_M:   map_start = ORTHO_MEA_START;    break;
#ifdef VET
        case TD_M:      map_start = TD_MEA_START;   break;
        case ANOB_M:    map_start = ANOB_MEA_START; break;
#endif
    }

    if(m_TableUser == map_table)
    {
        map_start = USER_START;
    }

    GtkWidget *lab = NULL;
    GtkWidget *treeview = NULL;
#ifdef VET
  if((section == OB_M) || (section == ANOB_M))
        treeview = CreateObTable(start, end, map_table, map_start, fetal);
    else
        treeview = CreateGeneralTable(section, start, end, map_table, map_start);
#else
    if(section == OB_M)
        treeview = CreateObTable(start, end, map_table, map_start, fetal);
    else
        treeview = CreateGeneralTable(section, start, end, map_table, map_start);
#endif

    if(treeview)
    {
        if(section == OB_M)
            lab = create_label(title, 0, h, NULL, NULL);
        else
            lab = create_label(title, 0, h, g_blue, NULL);
        gtk_misc_set_alignment(GTK_MISC(lab), 0.0, 0.5);

        GtkTreeIter iter;
        GtkTreeModel *model;

#ifdef VET
        if((section == OB_M) || (section == ANOB_M))
        {
            model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_ob));
        }
        else
        {
            model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_left));
        }
#else
        if(section == OB_M)
        {
            model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_ob));
        }
        else
        {
            model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_left));
        }
#endif

        gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
        while(valid)
        {
            rows++;
            valid = gtk_tree_model_iter_next(model, &iter);
        }

        if(rows > 0)
        {
            gtk_fixed_put(GTK_FIXED(fixed), lab, x1, *y);
            *y += h;
            gtk_fixed_put(GTK_FIXED(fixed), treeview, x2, *y);
            h = row_height; // table row is not enough 30, about 21*30 ~~ 28*23
            *y += rows*h + h; //rows*h is table hight, h is next table start location
        }
    }
}

void ViewReport::UpdateGrowthCurve(int type)
{
    MeaResult *ptrRes = MeaResult::GetInstance();
    double calc_val, value[MEA_MULTI], allvalue[MEA_MULTI][MEA_TIMES];
    int item;

    m_dataGC[0] = m_dataGC[1] = 0;

    switch(type)
    {
        case 0: //GS-Rempen
            item = OB_GS;
            break;
        case 1: //CRL-Hadlock
        case 2: //CRL-Robinson
        case 3: //CRL-Hansmenn
            item = OB_CRL;
            break;
        case 4: //BPD-Hadlock
        case 5: //BPD-Lasser
        case 6: //BPD-Merz
        case 7: //BPD-Rempen
            item = OB_BPD;
            break;
        case 8: //HC-Merz
        case 9: //HC-Hadlock
            item = OB_HC;
            break;
        case 10:    //AC-Hadlock
        case 11:    //AC-Merz
            item = OB_AC;
            break;
        case 12:    //FL-Hadlock
        case 13:    //FL-Merz
        case 14:    //FL-Jeanty
            item = OB_FL;
            break;
        case 15:    //HL-Jeanty
            item = OB_HL;
            break;
        case 16:    //EFW-Hadlock
        case 17:    //EFW-Hansmenn
            {
                for(int f=0; f<2; f++)
                {
                    int t = 0;
                    double efw_all = 0.0;
                    double efw[EFW_END-EFW_START-1];
                    for(int i=EFW_START+1; i<EFW_END; i++)
                    {
                        if(ptrRes->CalcGetValue(&calc_val, OB_EFW_HADLOCK1+i-1, OB_M, f+1)==MEA_SUCCESS)
                        {
                            //  printf("calc_val=%f, o=%f\n", (int)(calc_val*100+0.5)/100.0, calc_val);
                            efw[i-1] = (int)(calc_val*100+0.5)/100.0;
                            //  PRINTF("efw[%d]=%.2f\n", i-1, efw[i-1]);
                            efw_all += efw[i-1];
                            t++;
                        }
                        else
                            continue;
                    }
                    //  PRINTF("t = %d\n", t);
                    if(t > 0)
                    {
                        m_dataGC[f] = ((efw_all / (float)t) + 0.0005) * 1000;
                    }
                }
            }
        //  PRINTF("Growth Currve EFW: F1 = %d, F2 = %d\n", m_dataGC[0], m_dataGC[1]);
            return;

        default:
            return;
    }
    for(int m=0; m<MEA_MULTI; m++)
    {
        value[m] = INVALID_VAL;
        for(int n=0; n<MEA_TIMES; n++)
            allvalue[m][n] = INVALID_VAL;
    }
    if(ptrRes->GetValue(item, value, allvalue, IN_AVERAGE, FETAL1)==MEA_SUCCESS)
        m_dataGC[0] = (value[0]*10 + 0.05) * 10;
    if(ptrRes->GetValue(item, value, allvalue, IN_AVERAGE, FETAL2)==MEA_SUCCESS)
        m_dataGC[1] = (value[0]*10 + 0.05) * 10;
//  PRINTF("Growth Currve F1 = %d, F2 = %d\n", m_dataGC[0], m_dataGC[1]);
}

static void draw_dash_line(GdkDrawable *drawable, GdkGC *gc, GdkFunction mode, int x0, int y0, int x1, int y1)
{
    int x, y, count;
    int dist_x, dist_y;
    int length;

    dist_x = x1 - x0;
    dist_y = y1 - y0;
    length = (int)( sqrt(dist_x*dist_x + dist_y*dist_y) );

    gdk_gc_set_function (gc, mode);
    if (length > 10)
    {
        count = (length/10)*10;
        while(count >= 0)
        {
            x = x0 + (int)( (float)dist_x * (float)count / (float)length);
            y = y0 + (int)( (float)dist_y * (float)count / (float)length);

            gdk_draw_point (drawable, gc, x, y);
            gdk_draw_point (drawable, gc, x+1, y);
            gdk_draw_point (drawable, gc, x, y+1);
            gdk_draw_point (drawable, gc, x+1, y+1);

            count -= 10;
        }
    }
}

void ViewReport::DrawGrowthCurve(int ObTable[][3], int StartDate, int MeasureNum[], int GWDate, char sign)
{
//  PRINTF("%s: StartDate=%d, MeasureNum 0=%d, 1=%d, GWDate=%d\n", __FUNCTION__, StartDate, MeasureNum[0], MeasureNum[1], GWDate);
#if 1
    //X轴代表天数，Y轴代表所测指标的长度
    int i, j, x, y;
    int FormerX = 0;
    int FormerY = 0;
    //X,Y轴均为280个象素，分成10格，起始坐标为(320,320)
    volatile int Coordinate_Len=280, Coordinate_Start_X=320, Coordinate_Start_Y=320;
    //  volatile int Table_data[280];
    volatile int Length_X, Length_Y;
    //Unit_Len_X为X坐标上每格代表的数据长度，Unit_Len_X1为X坐标上每格的象素长度
    volatile int Unit_Len_X, Unit_Len_Y, Unit_Len_X1, Unit_Len_Y1;
    volatile int StartNum_X, StartNum_Y;//坐标轴起始数据
    volatile int DataNum = 0;
    gchar  strtemp[50];

    GdkGC *gc = gdk_gc_new(m_pixmapGC);
    gdk_gc_set_foreground(gc, g_black);
    PangoLayout *layout = gtk_widget_create_pango_layout(m_window, "");
    pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);

    for(i=0;i<280;i++)
    {
        if(ObTable[i][1]!=10000)
            DataNum = i+1;
        else
            break;
    }
    //=============计算坐标轴数值范围，画坐标轴====================//
    Length_Y = ObTable[DataNum-1][2] - ObTable[0][0];
    Length_X = DataNum;

    Unit_Len_X1 = Coordinate_Len/10;
    Unit_Len_Y1 = Coordinate_Len/10;

    if(Length_X>100)
    {
        Length_X = (Length_X/100+1)*100+30;
        if(StartDate+Length_X>300)
            Length_X = Length_X - ((StartDate/10+1)*10+Length_X-350);
    }
    else if(Length_X>10)
    {
        Length_X = (Length_X/10+2)*10+10;
        if(StartDate+Length_X>300)
            Length_X = Length_X - ((StartDate/10+1)*10+Length_X-330);
    }

    if(StartDate+Length_X>300)
        Length_X = Length_X - ((StartDate/10+1)*10+Length_X-300);

    Unit_Len_X = Length_X/10;

    if(Length_Y>100)
    {
        Length_Y = (Length_Y/100+2)*100;
    }
    else if(Length_Y>10)
    {
        Length_Y = (Length_Y/10+2)*10;
    }

    Unit_Len_Y = Length_Y/10;

    draw_dash_line (m_pixmapGC, gc, GDK_COPY, Coordinate_Start_X, Coordinate_Start_Y, Coordinate_Start_X+Coordinate_Len, Coordinate_Start_Y);
    for(i=1;i<=10;i++)
    {
        draw_dash_line (m_pixmapGC, gc, GDK_COPY, Coordinate_Start_X+i*Unit_Len_X1-1, Coordinate_Start_Y-5, Coordinate_Start_X+i*Unit_Len_X1-1, Coordinate_Start_Y);
        for(j=1;j<21;j++)
        {
            //坐标轴点
            gdk_draw_point (m_pixmapGC, gc, Coordinate_Start_X+i*Unit_Len_X1-1, Coordinate_Start_Y-j*Unit_Len_Y1/2);
        }
    }
    draw_dash_line (m_pixmapGC, gc, GDK_COPY, Coordinate_Start_X, Coordinate_Start_Y-Coordinate_Len, Coordinate_Start_X, Coordinate_Start_Y);
    for(i=1;i<=10;i++)
    {
        draw_dash_line (m_pixmapGC, gc, GDK_COPY, Coordinate_Start_X, Coordinate_Start_Y-i*Unit_Len_Y1, Coordinate_Start_X+5, Coordinate_Start_Y-i*Unit_Len_Y1);
        for(j=1;j<21;j++)
        {
            gdk_draw_point (m_pixmapGC, gc, Coordinate_Start_X+j*Unit_Len_X1/2, Coordinate_Start_Y-i*Unit_Len_Y1);
        }
    }

    StartNum_X = StartDate/Unit_Len_X;
    StartNum_X = StartNum_X*Unit_Len_X;
    StartNum_Y = ObTable[0][0]/Unit_Len_Y;
    StartNum_Y = StartNum_Y*Unit_Len_Y;

    for(i=0;i<6;i++)//标横轴的坐标值
    {
        sprintf(strtemp, "%3d", (int)(StartNum_X+i*Unit_Len_X*2));
        pango_layout_set_text(layout, strtemp, strlen(strtemp));
        gdk_draw_layout(m_pixmapGC, gc, Coordinate_Start_X+i*Unit_Len_X1*2-16,Coordinate_Start_Y+10, layout);
    }
    sprintf(strtemp, "(day)");
    pango_layout_set_text(layout, strtemp, strlen(strtemp));
    gdk_draw_layout(m_pixmapGC, gc, Coordinate_Start_X+i*Unit_Len_X1*2-40, Coordinate_Start_Y-10, layout);

    if(sign==0)//标纵轴的坐标值
    {
        for(i=0;i<11;i++)
        {
            sprintf(strtemp, "%3d.%1d", ((int)(StartNum_Y+i*Unit_Len_Y))/10, ((int)(StartNum_Y+i*Unit_Len_Y))%10);
            pango_layout_set_text(layout, strtemp, strlen(strtemp));
            gdk_draw_layout(m_pixmapGC, gc, Coordinate_Start_X-40, Coordinate_Start_Y-i*Unit_Len_Y1-8, layout);
        }
    }
    else
    {
        for(i=0;i<11;i++)
        {
            sprintf(strtemp, "%1d.%1d%1d", ((int)(StartNum_Y+i*Unit_Len_Y))/1000, ((int)(StartNum_Y+i*Unit_Len_Y))%1000/100,((int)(StartNum_Y+i*Unit_Len_Y))%100/10);
            pango_layout_set_text(layout, strtemp, strlen(strtemp));
            gdk_draw_layout(m_pixmapGC, gc, Coordinate_Start_X-40, Coordinate_Start_Y-i*Unit_Len_Y1-8, layout);
        }
    }
    if(sign==0)
        sprintf(strtemp, "(mm)");
    else
        sprintf(strtemp, "(kg)");
    pango_layout_set_text(layout, strtemp, strlen(strtemp));
    gdk_draw_layout(m_pixmapGC, gc, Coordinate_Start_X-20, Coordinate_Start_Y-i*Unit_Len_Y1, layout);

    for(i=0;i<DataNum;i++) //以下为画曲线，以上为画坐标
    {
        x = Coordinate_Start_X + (i+StartDate-StartNum_X)*Unit_Len_X1/Unit_Len_X;
        y = Coordinate_Start_Y - (ObTable[i][1]-StartNum_Y)*Unit_Len_Y1/Unit_Len_Y;
        gdk_draw_point (m_pixmapGC, gc, x, y);

        if(i>0)
            draw_dash_line (m_pixmapGC, gc, GDK_COPY, FormerX, FormerY, x, y);

        FormerX = x;
        FormerY = y;
    }

    for(i=0;i<DataNum;i++)//下限曲线
    {
        x = Coordinate_Start_X + (i+StartDate-StartNum_X)*Unit_Len_X1/Unit_Len_X;
        y = Coordinate_Start_Y - (ObTable[i][0]-StartNum_Y)*Unit_Len_Y1/Unit_Len_Y;

        if(ObTable[i][0]!=0)
        {
            gdk_draw_point (m_pixmapGC, gc, x, y);

            if(i>0)
                draw_dash_line (m_pixmapGC, gc, GDK_COPY, FormerX, FormerY, x, y);

        }

        FormerX = x;
        FormerY = y;
    }

    for(i=0;i<DataNum;i++)//上限曲线
    {
        x = Coordinate_Start_X + (i+StartDate-StartNum_X)*Unit_Len_X1/Unit_Len_X;
        y = Coordinate_Start_Y - (ObTable[i][2]-StartNum_Y)*Unit_Len_Y1/Unit_Len_Y;
        gdk_draw_point (m_pixmapGC, gc, x, y);

        if(i>0)
            draw_dash_line (m_pixmapGC, gc, GDK_COPY, FormerX, FormerY, x, y);

        FormerX = x;
        FormerY = y;
    }

    //标测量值的坐标点和LMP和上下限曲线及横轴的坐标点
    x = Coordinate_Start_X + (GWDate-StartNum_X)*Unit_Len_X1/Unit_Len_X;
    y = Coordinate_Start_Y - (MeasureNum[0]-StartNum_Y)*Unit_Len_Y1/Unit_Len_Y;
    gdk_gc_set_foreground(gc, g_blue);
    if(MeasureNum[0]!=0  && GWDate!=0 )
    {
        sprintf(strtemp, "%s 1 :", _("Fetus"));
        pango_layout_set_text(layout, strtemp, strlen(strtemp));
        gdk_draw_layout(m_pixmapGC, gc, 70, 80, layout);
        if(sign==0)
        //  sprintf(strtemp, "▲(%2dw%1dd,%.1fmm)",  GWDate/7,GWDate%7,MeasureNum[0]);
            sprintf(strtemp, "%2dw%1dd, %2d.%1dmm",  GWDate/7,GWDate%7,MeasureNum[0]/10,MeasureNum[0]%10);
        else
        //  sprintf(strtemp, "▲(%2dw%1dd,%.2fkg)", GWDate/7,GWDate%7,MeasureNum[0]);
            sprintf(strtemp, "%2dw%1dd, %1d.%1d%1dkg", GWDate/7,GWDate%7,MeasureNum[0]/1000,(MeasureNum[0]%1000)/100,(MeasureNum[0]%100)/10);
        pango_layout_set_text(layout, strtemp, strlen(strtemp));
        gdk_draw_layout(m_pixmapGC, gc, 80, 100, layout);
    //  printf("MeasureNum[0]= %d, StartNum_Y=%d, Length_Y=%d, GWDate=%d, StartDate=%d, DataNum=%d\n", MeasureNum[0], StartNum_Y, Length_Y, GWDate, StartDate, DataNum);
        if(MeasureNum[0]>=StartNum_Y && MeasureNum[0]<StartNum_Y+Length_Y &&
                GWDate>=StartDate && GWDate<StartDate+DataNum)
        {
            sprintf(strtemp, "F1");
            pango_layout_set_text(layout, strtemp, strlen(strtemp));
            gdk_draw_layout(m_pixmapGC, gc, x+5, y-10, layout);
        //  gdk_draw_layout(m_pixmapGC, gc, x, y-15, layout);
            gdk_draw_line(m_pixmapGC, gc, Coordinate_Start_X+1, y, x, y);
            gdk_draw_line(m_pixmapGC, gc, x, y, x, Coordinate_Start_Y-1);
        }
    }

    x = Coordinate_Start_X + (GWDate-StartNum_X)*Unit_Len_X1/Unit_Len_X;
    y = Coordinate_Start_Y - (MeasureNum[1]-StartNum_Y)*Unit_Len_Y1/Unit_Len_Y;
    gdk_gc_set_foreground(gc, g_skyBlue);
    if(MeasureNum[1]!=0 && GWDate!=0)
    {
        sprintf(strtemp, "%s 2 :", _("Fetus"));
        pango_layout_set_text(layout, strtemp, strlen(strtemp));
        gdk_draw_layout(m_pixmapGC, gc, 69, 130, layout);
        if(sign==0)
        //  sprintf(strtemp, "△(%2dw%1dd,%.1fmm)", GWDate/7,GWDate%7,MeasureNum[1]);
            sprintf(strtemp, "%2dw%1dd, %2d.%1dmm", GWDate/7,GWDate%7,MeasureNum[1]/10, MeasureNum[1]%10);
        else
        //  sprintf(strtemp, "▲(%2dw%1dd,%.2fkg)", GWDate/7,GWDate%7,MeasureNum[1]);
            sprintf(strtemp, "%2dw%1dd, %1d.%1d%1dkg", GWDate/7,GWDate%7,MeasureNum[1]/1000,(MeasureNum[1]%1000)/100,(MeasureNum[1]%100)/10);
        pango_layout_set_text(layout, strtemp, strlen(strtemp));
        gdk_draw_layout(m_pixmapGC, gc, 80, 150, layout);
        if(MeasureNum[1]>=StartNum_Y && MeasureNum[1]<StartNum_Y+Length_Y &&
                GWDate>=StartDate && GWDate<StartDate+DataNum)
        {
            sprintf(strtemp, "F2");
            pango_layout_set_text(layout, strtemp, strlen(strtemp));
            gdk_draw_layout(m_pixmapGC, gc, x+5, y-10, layout);
        //  gdk_draw_layout(m_pixmapGC, gc, x+5, y, layout);
            gdk_gc_set_function(gc, GDK_COPY);
            gdk_draw_line(m_pixmapGC, gc, Coordinate_Start_X+1, y, x, y);
            gdk_draw_line(m_pixmapGC, gc, x, y, x, Coordinate_Start_Y-1);
        }
    }

    g_object_unref(layout);
#endif
}

void ViewReport::GrowthCurveChanged(GtkComboBox *combobox)
{
    int type = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
    UpdateGrowthCurve(type);
    gtk_widget_queue_draw(m_drawingareaGC);
}

void ViewReport::GrowthCurveConfigure(GtkWidget *widget, GdkEventConfigure *event)
{
    if (m_pixmapGC){
        gdk_pixmap_unref (m_pixmapGC);
    }
    m_pixmapGC = gdk_pixmap_new (widget->window,
            widget->allocation.width,
            widget->allocation.height,
            -1);

    GdkGC *gc = gdk_gc_new (m_pixmapGC);
    gdk_gc_set_foreground (gc, g_white);
    gdk_draw_rectangle (m_pixmapGC, gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);

    g_object_unref(gc);
}

void ViewReport::GrowthCurveExpose(GtkWidget *widget, GdkEventExpose *event)
{
    GdkGC *gc = gdk_gc_new(m_pixmapGC);
    gdk_gc_set_foreground (gc, g_gray);
    gdk_draw_rectangle (m_pixmapGC, gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);
    g_object_unref(gc);

    //Get LMP date
    PatientInfo::Info info;
    g_patientInfo.GetInfo(info);
    int gw = MeaCalcFun::CalcGW_LMP(info.ob.LMPDate.year, info.ob.LMPDate.month, info.ob.LMPDate.day);
//  printf("GrowthCurve:  LMP=%d/%d/%d, gw=%d\n", info.ob.LMPDate.year, info.ob.LMPDate.month, info.ob.LMPDate.day, gw);

    int flag = gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboboxGC));
    switch(flag)
    {
        case 0:     DrawGrowthCurve (gs_rempen1, 32, m_dataGC, gw, 0);      break;
        case 1:     DrawGrowthCurve (crl_hadlock1, 43, m_dataGC,gw, 0);     break;
        case 2:     DrawGrowthCurve (crl_robinson1, 43, m_dataGC, gw, 0);   break;
        case 3:     DrawGrowthCurve (crl_hansmenn1, 43, m_dataGC, gw, 0);   break;
        case 4:     DrawGrowthCurve (bpd_hadlock1, 84, m_dataGC, gw, 0);    break;
        case 5:     DrawGrowthCurve (bpd_lasser1, 84, m_dataGC, gw, 0);     break;
        case 6:     DrawGrowthCurve (bpd_merz1, 84, m_dataGC, gw, 0);       break;
        case 7:     DrawGrowthCurve (bpd_rempen1, 84, m_dataGC, gw, 0);     break;
        case 8:     DrawGrowthCurve (hc_merz1, 84, m_dataGC, gw, 0);        break;
        case 9:     DrawGrowthCurve (hc_merz1 ,84, m_dataGC, gw, 0);        break;
        case 10:    DrawGrowthCurve (ac_hadlock1, 84, m_dataGC, gw, 0);     break;
        case 11:    DrawGrowthCurve (ac_hadlock1, 84, m_dataGC, gw, 0);     break;
        case 12:    DrawGrowthCurve (fl_hadlock1, 84, m_dataGC, gw, 0);     break;
        case 13:    DrawGrowthCurve (fl_merz1, 84, m_dataGC, gw, 0);        break;
        case 14:    DrawGrowthCurve (fl_jeanty1, 84, m_dataGC, gw, 0);      break;
        case 15:    DrawGrowthCurve (hl_jeanty1, 91, m_dataGC, gw, 0);      break;
        case 16:    DrawGrowthCurve (EFW_hadlock1, 70, m_dataGC, gw, 1);    break;
        case 17:    DrawGrowthCurve (EFW_hansmenn1, 70, m_dataGC, gw, 1);   break;
        default:
            break;
    }
    gdk_draw_pixmap (widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                     m_pixmapGC,
                     event->area.x, event->area.y,
                     event->area.x, event->area.y,
                     event->area.width, event->area.height);
}

void ViewReport::AddGrowthCurve(GtkWidget *fixed, guint *y)
{
    guint x1 = 20;
    guint x2 = 80;
    guint h = 30;
    GtkTreeIter iter;
    GtkListStore *store;
    GtkWidget *title;

    gchar strGrowthCurve[18][25]= { {"GS-Rempen"},
                                    {"CRL-Hadlock"}, {"CRL-Robinson"},  {"CRL-Hansmenn"},
                                    {"BPD-Hadlock"}, {"BPD-Lasser"}, {"BPD-Merz"}, {"BPD-Rempen"},
                                    {"HC-Merz"}, {"HC-Hadlock"},
                                    {"AC-Hadlock"}, {"AC-Merz"},
                                    {"FL-Hadlock"}, {"FL-Merz"}, {"FL-Jeanty"},
                                    {"HL-Jeanty"},
                                    {"EFW-Hadlock"}, {"EFW-Hansmenn"} };

    title = create_label(_("<i><b>Growth Curve</b></i>"), 0, h, g_white, NULL);
    gtk_misc_set_alignment(GTK_MISC(title), 0.0, 0.5);
    gtk_fixed_put(GTK_FIXED(fixed), title, x1, *y);

    store = gtk_list_store_new(1, G_TYPE_STRING);
    for(int i=0;i<18;i++)
    {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set(store, &iter, 0, strGrowthCurve[i], -1);
    }
    m_comboboxGC = create_combobox(150, h, "text", GTK_TREE_MODEL(store));
    g_signal_connect (G_OBJECT(m_comboboxGC), "changed", G_CALLBACK(HandleGrowthCurveChanged), this);
    gtk_fixed_put(GTK_FIXED(fixed), m_comboboxGC, 700, *y-5);
    *y += h;

    m_drawingareaGC= gtk_drawing_area_new();
    gtk_widget_set_size_request(m_drawingareaGC, 770, 350);
    gtk_widget_modify_bg (m_drawingareaGC, GTK_STATE_NORMAL, g_white);
    g_signal_connect (G_OBJECT(m_drawingareaGC), "configure_event", G_CALLBACK(HandleGrowthCurveConfigure), this);
    g_signal_connect (G_OBJECT(m_drawingareaGC), "expose_event", G_CALLBACK(HandleGrowthCurveExpose), this);
    gtk_fixed_put(GTK_FIXED(fixed), m_drawingareaGC, x2, *y);

    *y += 350;

    UpdateGrowthCurve(0);
}

void ViewReport::UpdateTotalGrade(int flag)
{
    if(flag == -1)
    {
        for(int i=0; i<6; i++)
            gtk_entry_set_text(GTK_ENTRY(m_entryDes[i]), _(str_Des[i][m_grade[i]]));
    }
    else
    {
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[flag]), _(str_Des[flag][m_grade[flag]]));
    }

    //Get AF value
//  int AFres = 10;//test
    double AFres;
    if(MeaResult::GetInstance()->CalcGetValue(&AFres, OB_CALC_AFI, OB_M, FETAL_BY_SET)!=MEA_SUCCESS)
        return;
    if(AFres != 0)
    {
        char str_total[10];
        m_grade[6] = m_grade[0] + m_grade[1] + m_grade[2] + m_grade[3] + m_grade[4] + m_grade[5];
        sprintf(str_total, "%d", m_grade[6]);
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[8]), str_total);
        if(m_grade[6] >= 7 && m_grade[6] <= 12)
            gtk_entry_set_text(GTK_ENTRY(m_entryDes[6]), _(str_Des[6][0]));
        else if(m_grade[6] >= 3 && m_grade[6] <= 6)
            gtk_entry_set_text(GTK_ENTRY(m_entryDes[6]), _(str_Des[6][1]));
        else if(m_grade[6] >= 0 && m_grade[6] <= 2)
            gtk_entry_set_text(GTK_ENTRY(m_entryDes[6]), _(str_Des[6][2]));
    }
}

void ViewReport::SpinValueChanged(GtkSpinButton *spin)
{
    int row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(spin), "row"));
    int val = atoi(gtk_entry_get_text(GTK_ENTRY(spin)));

    m_grade[row] = val;
    UpdateTotalGrade(row);
}

void ViewReport::AddBiophysicalProfile(GtkWidget *fixed, guint *y)
{
    gint i;
    guint x1 = 20;
    guint x2 = 80;
    guint h = 30;
    GtkWidget *formula;
    GtkAdjustment *adj[5];

    char str[9][50]= {N_("Formula: Vintzileos"), N_("FHR:"), N_("FM:"), N_("FBM:"), N_("FT:"), N_("PL:"), N_("AF:"), N_("Total:")};

    GtkWidget *title = create_label(_("<i><b>Fetal Biophysical Profile</b></i>"), 0, h, g_white, NULL);
    gtk_misc_set_alignment(GTK_MISC(title), 0.0, 0.5);
    gtk_fixed_put(GTK_FIXED(fixed), title, x1, *y);
    *y += h;

    GtkWidget *table = gtk_table_new(9, 3, FALSE);
    gtk_table_set_col_spacing (GTK_TABLE(table), 1, 30);
    gtk_fixed_put(GTK_FIXED(fixed), table, x2, *y);

    formula = create_label(_(str[0]), 0, h, g_white, NULL);
    gtk_widget_set_usize(formula, -1, h);
    gtk_misc_set_alignment(GTK_MISC(formula), 0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), formula, 0, 3, 0, 1);

    for(i=0; i<7; i++)
    {
        GtkWidget *lab = create_label(_(str[i+1]), 0, h, g_white, NULL);
        gtk_misc_set_alignment(GTK_MISC(lab), 0, 0.5);
        gtk_widget_set_usize(lab, 160, h);
        gtk_table_attach_defaults(GTK_TABLE(table), lab, 0, 1, i+1, i+2);
    }

    for(i=0; i<5; i++)
        adj[i] = (GtkAdjustment *) gtk_adjustment_new (m_grade[i], 0, 2, 1, 1, 0);

    for(i=0; i<5; i++)
    {
        GtkWidget *spin = gtk_spin_button_new(adj[i], 1, 0);
        gtk_entry_set_editable (GTK_ENTRY(spin), FALSE);
        gtk_widget_set_usize(spin, 60, h-5);
        g_object_set_data(G_OBJECT(spin), "row", GINT_TO_POINTER(i));
        g_signal_connect(G_OBJECT(spin), "value_changed", G_CALLBACK(HandleSpinValueChanged), this);
        gtk_table_attach_defaults(GTK_TABLE(table), spin, 1, 2, i+1, i+2);
    }

    for(i=0; i<7; i++)
    {
        m_entryDes[i] = gtk_entry_new();
        gtk_entry_set_editable (GTK_ENTRY(m_entryDes[i]), FALSE);
        gtk_widget_set_sensitive (m_entryDes[i], FALSE);
        gtk_widget_set_usize (m_entryDes[i], 400, h);
        gtk_widget_modify_text (m_entryDes[i], GTK_STATE_INSENSITIVE, g_black);
        gtk_table_attach_defaults(GTK_TABLE(table), m_entryDes[i], 2, 3, i+1, i+2);
    }

    GtkWidget *lab = create_label(_("Single Explain:    2-point:Normal  1-point:Slight Abnormal  0-point:Obvious Abnormal"),
            0, h, g_white, NULL);
    gtk_misc_set_alignment(GTK_MISC(lab), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), lab, 0, 3, 8, 9);

    //AF
    m_entryDes[7] = gtk_entry_new();
    gtk_entry_set_editable (GTK_ENTRY(m_entryDes[7]), FALSE);
    gtk_widget_set_sensitive (m_entryDes[7], FALSE);
    gtk_widget_set_usize(m_entryDes[7], 60, h);
    gtk_widget_modify_text (m_entryDes[7], GTK_STATE_INSENSITIVE, g_black);
    gtk_table_attach_defaults(GTK_TABLE(table), m_entryDes[7], 1, 2, 6, 7);

    //Total
    m_entryDes[8] = gtk_entry_new();
    gtk_entry_set_editable (GTK_ENTRY(m_entryDes[8]), FALSE);
    gtk_widget_set_sensitive (m_entryDes[8], FALSE);
    gtk_widget_set_usize(m_entryDes[8], 60, h);
    gtk_widget_modify_text (m_entryDes[8], GTK_STATE_INSENSITIVE, g_black);
    gtk_table_attach_defaults(GTK_TABLE(table), m_entryDes[8], 1, 2, 7, 8);

    //Get AF value
//int AFres = 10;   //test
    double AFres;
    if(MeaResult::GetInstance()->CalcGetValue(&AFres, OB_CALC_AFI, OB_M)!=MEA_SUCCESS)
        AFres = 0;
    AFres = AFres * 10;
    //printf("AFres = %f\n", AFres);
    if(AFres > 20)
    {
        m_grade[5] = 2;
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[7]), "2");
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[5]), _(str_Des[6][0]));
    }
    else if(AFres<=20 && AFres>=10)
    {
        m_grade[5] = 1;
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[7]), "1");
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[5]), _(str_Des[6][1]));
    }
    else if(AFres<10 && AFres!=0)
    {
        m_grade[5] = 0;
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[7]), "0");
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[5]), _(str_Des[6][2]));
    }
    else if(AFres==0)
    {
        gtk_entry_set_text(GTK_ENTRY(m_entryDes[5]), _("No measured"));
    }

    UpdateTotalGrade(-1);

    gint rows;
    g_object_get(G_OBJECT(table), "n-rows", &rows, NULL);
    *y += rows * h;
}

void ViewReport::AddPSAD(GtkWidget *fixed, guint *y)
{
    gint rows = 0, id = 0;
    GtkListStore *store= gtk_list_store_new(3,
            G_TYPE_BOOLEAN,
            G_TYPE_STRING,
            G_TYPE_STRING);

    GtkTreeModel *model = GTK_TREE_MODEL (store);
    GtkWidget *treeview = gtk_tree_view_new();

    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_NONE);

    if (model != NULL)
    {
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    }

    guint x1 = 20, x2 = 40;
    int h = 30;
    double calc_val[2];
    MeaResult *ptrRes = MeaResult::GetInstance();
    ptrRes->CalcGetSPSA(calc_val[0], calc_val[1]);

    for(gint i = 0; i < 2; i++)
    {
        if(calc_val[i] != INVALID_VAL)
        {
            char title[100], value[100];
            if(i==0)
            {
                id = PSAD_START_ID;
                sprintf(title, "%s", "SPSA");
                sprintf(value, " %.2fng/ml", calc_val[i]);
            }
            else
            {
                id = PSAD_START_ID + 1;
                sprintf(title, "%s", "PSAD");
                sprintf(value, "%.2f", calc_val[i]);
            }
            rows = AddResultToTree2(treeview, title, value, id, rows);
        }
    }

    g_object_unref (model);

    if(rows > 0)
    {
        GtkCellRenderer *renderer;
        GtkTreeViewColumn *column;
        gint wide[] = {20, 170, 90};

        GtkWidget* lab = create_label("PSAD&&SPSA", 0, h, g_blue, NULL);
        gtk_misc_set_alignment(GTK_MISC(lab), 0.0, 0.5);

        renderer = gtk_cell_renderer_toggle_new ();
        column = gtk_tree_view_column_new_with_attributes ("", renderer, "active", 0, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_signal_connect (G_OBJECT(renderer), "toggled", G_CALLBACK (CheckBoxToggleClicked), (gpointer)model);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", wide[0], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 1, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[1], NULL);

        renderer = gtk_cell_renderer_text_new();
        column = gtk_tree_view_column_new_with_attributes(("Value"), renderer, "text", 2, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
        g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width",  wide[2], NULL);

        gtk_fixed_put(GTK_FIXED(fixed), lab, x1, *y);
        *y += h;
        gtk_fixed_put(GTK_FIXED(fixed), treeview, x2, *y);
        h = row_height; // table row is not enough 30, aboult 21*30 ~~ 28*23
        *y += rows*h + h; //rows*h is table hight, h is next table start location
    }
}

void ViewReport::InitTableData()
{
    int i, j;

    //ABD
    for(i=0; i<ABD_MEA_END-ABD_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableAbd[i][j] = -1;
    InitMapTable(m_TableAbd, ABD_MEA_START, ABD_MEA_END, AbdoInfo);

    //UR
    for(i=0; i<UR_MEA_END-UR_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableUr[i][j] = -1;
    InitMapTable(m_TableUr, UR_MEA_START, UR_MEA_END, URInfo);

    //AE
    for(i=0; i<ADULT_MEA_END-ADULT_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableAdult[i][j] = -1;
    InitMapTable(m_TableAdult, ADULT_MEA_START, ADULT_MEA_END, AdultInfo);

    //OB
    for(i=0; i<OB_MEA_END-OB_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableOb[i][j] = -1;
    InitMapTable(m_TableOb, OB_MEA_START, OB_MEA_END, OBInfo);

    //GYN
    for(i=0; i<GYN_MEA_END-GYN_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableGyn[i][j] = -1;
    InitMapTable(m_TableGyn, GYN_MEA_START, GYN_MEA_END, GYNInfo);

    //SP
    for(i=0; i<SP_MEA_END-SP_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableSp[i][j] = -1;
    InitMapTable(m_TableSp, SP_MEA_START, SP_MEA_END, SPInfo);

    //VES
    for(i=0; i<VS_MEA_END-VS_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableVs[i][j] = -1;
    InitMapTable(m_TableVs, VS_MEA_START, VS_MEA_END, VSInfo);

    //FETAL
    for(i=0; i<FETAL_MEA_END-FETAL_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableFetal[i][j] = -1;
    InitMapTable(m_TableFetal, FETAL_MEA_START, FETAL_MEA_END, FetalInfo);

    //TCD
    for(i=0; i<TCD_MEA_END-TCD_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableTcd[i][j] = -1;
    InitMapTable(m_TableTcd, TCD_MEA_START, TCD_MEA_END, TCDInfo);

    //ORTHO
    for(i=0; i<ORTHO_MEA_END-ORTHO_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableOrtho[i][j] = -1;
    InitMapTable(m_TableOrtho, ORTHO_MEA_START, ORTHO_MEA_END, OrthoInfo);
#ifdef VET
//添加 动物超 TD
//hlx
    for(i=0; i<TD_MEA_END-TD_MEA_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableTD[i][j] = -1;
    InitMapTable(m_TableTD, TD_MEA_START, TD_MEA_END, TDInfo);
//添加 动物超 ANOB
//hlx
    for(i=0;i<ANOB_MEA_END-ANOB_MEA_START; i++)
        for(j=0;j<CALC_MAX;j++)
            m_TableAnOB[i][j] = -1;
    InitMapTable(m_TableAnOB, ANOB_MEA_START, ANOB_MEA_END, AnOBInfo);
#endif

    //USER
  for(i=0; i<USER_END-USER_START; i++)
        for(j=0; j<CALC_MAX; j++)
            m_TableUser[i][j] = -1;
    InitMapTable(m_TableUser, USER_START, USER_END,userDef_new);

}

void ViewReport::ClearIndicationandComments(void)
{
    m_strReport[0] = "";
    m_strReport[1] = "";
}

GtkWidget* ViewReport::CreateNotePageABD()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create ABD Note\n");

    //General
    AddTable(fixed, &y, _("<i><b>General</b></i>"), ABDO_M, ABD_MEA_START, ABD_DIST_AO, m_TableAbd);
    //Arterial
    AddTable(fixed, &y, _("<i><b>Arterial</b></i>"), ABDO_M, ABD_DIST_AO, ABD_PROX_IVC, m_TableAbd);
    //Venous
    AddTable(fixed, &y, _("<i><b>Venous</b></i>"), ABDO_M, ABD_PROX_IVC, ABD_MEA_END, m_TableAbd);
    AddTable(fixed, &y,  _("<i><b>User</b></i>"), ABDO_M, USER_START, USER_END, m_TableUser);
    return fixed;
}

GtkWidget* ViewReport::CreateNotePageUR()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create UR Note\n");

    //Bladder
    AddTable(fixed, &y, _("<i><b>Bladder</b></i>"), UR_M, UR_MEA_START, UR_PROSTATE_H, m_TableUr);
    //Prostate
    AddTable(fixed, &y, _("<i><b>Prostate</b></i>"), UR_M, UR_PROSTATE_H, UR_L_KID_H, m_TableUr);
    //Show PSA
    AddPSAD(fixed, &y);
    //Left Renal
    AddTable(fixed, &y, _("<i><b>Left Renal</b></i>"), UR_M, UR_L_KID_H, UR_R_KID_H, m_TableUr);
    //Right Renal
    AddTable(fixed, &y, _("<i><b>Right Renal</b></i>"), UR_M, UR_R_KID_H, UR_MEA_END, m_TableUr);
    AddTable(fixed, &y,  _("<i><b>User</b></i>"), UR_M, USER_START, USER_END, m_TableUser);
    return fixed;
}

GtkWidget* ViewReport::CreateNotePageAE()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create Adult Note\n");

    //Dimensions(2D)
    AddTable(fixed, &y, _("<i><b>Dimensions(2D)</b></i>"), ADULT_M, ADULT_MEA_START, ADULT_AOR_DIAM, m_TableAdult);
    //Vessels(2D)
    AddTable(fixed, &y, _("<i><b>Vessels(2D)</b></i>"), ADULT_M, ADULT_AOR_DIAM, ADULT_A2CD, m_TableAdult);
    //EF&Volume(2D)
    AddTable(fixed, &y, _("<i><b>EF &amp; Volume(2D)</b></i>"), ADULT_M, ADULT_A2CD, ADULT_AV_AREA, m_TableAdult);
    //Valves(2D)
    AddTable(fixed, &y, _("<i><b>Valves(2D)</b></i>"), ADULT_M, ADULT_AV_AREA, ADULT_RVAWD_MM, m_TableAdult);
    //Dimensions(M)
    AddTable(fixed, &y, _("<i><b>Dimensions(M)</b></i>"), ADULT_M, ADULT_RVAWD_MM, ADULT_LV_ET, m_TableAdult);
    //Aortic Valve(M)
    AddTable(fixed, &y, _("<i><b>Aortic Valve(M)</b></i>"), ADULT_M, ADULT_LV_ET, ADULT_MV_DE_EXC, m_TableAdult);
    //Mitral Valve(M)
    AddTable(fixed, &y, _("<i><b>Mitral Valve(M)</b></i>"), ADULT_M, ADULT_MV_DE_EXC, ADULT_TV_DE_EXC, m_TableAdult);
    //Tricuspid Valve(M)
    AddTable(fixed, &y, _("<i><b>Tricuspid Valve(M)</b></i>"), ADULT_M, ADULT_TV_DE_EXC, ADULT_LATE_DIAS_SLOPE, m_TableAdult);
    //Pulmonic Valve(M)
    AddTable(fixed, &y, _("<i><b>Pulmonic Valve(M)</b></i>"), ADULT_M, ADULT_LATE_DIAS_SLOPE, ADULT_AV_ACC_T, m_TableAdult);
    //Aortic Valve(D)
    AddTable(fixed, &y, _("<i><b>Aortic Valve(D)</b></i>"), ADULT_M, ADULT_AV_ACC_T, ADULT_HR_MV, m_TableAdult);
    //Mitral Valve(D)
    AddTable(fixed, &y, _("<i><b>Mitral Valve(D)</b></i>"), ADULT_M, ADULT_HR_MV, ADULT_HR_TV, m_TableAdult);
    //Tricuspid Valve(D)
    AddTable(fixed, &y, _("<i><b>Tricuspid Valve(D)</b></i>"), ADULT_M, ADULT_HR_TV, ADULT_HR_PV, m_TableAdult);
    //Pulmonic Valve(D)
    AddTable(fixed, &y, _("<i><b>Pulmonic Valve(D)</b></i>"), ADULT_M, ADULT_HR_PV, ADULT_MEA_END, m_TableAdult);
    AddTable(fixed, &y,  _("<i><b>User</b></i>"),ADULT_M, USER_START, USER_END, m_TableUser);
    return fixed;
}

GtkWidget* ViewReport::CreateNotePageOB()
{
    gchar title[255];
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create OB Note\n");

    //Average result
    AddAverRes(fixed, &y, "<i><b>Average</b></i>");
    //Fetal 1
    //Advanced
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("Advanced"), _("(Fetus 1)"));
    AddTable(fixed, &y, title, OB_M, OB_MEA_START, OB_AFI_LUQ, m_TableOb, 1);
    //AFI&BPP
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("AFI"), _("(Fetus 1)"));
    AddTable(fixed, &y, title, OB_M, OB_AFI_LUQ, OB_CRL, m_TableOb, 1);
    //Early
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("Early"), _("(Fetus 1)"));
    AddTable(fixed, &y, title, OB_M, OB_CRL, OB_RA_DIMEN, m_TableOb, 1);
    //Fetal Heart
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("Fetal Heart"), _("(Fetus 1)"));
    AddTable(fixed, &y, title, OB_M, OB_RA_DIMEN, OB_GS, m_TableOb, 1);
    //General
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("General"), _("(Fetus 1)"));
    AddTable(fixed, &y, title, OB_M, OB_GS, OB_EAR, m_TableOb, 1);
    //EFW
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("EFW"), _("(Fetus 1)"));
    AddOBEfwTable(fixed, &y, title, 1);
    //Other
    sprintf(title, "<i><b><span fgcolor=\"#0000FF\">%s%s</span></b></i>", _("Other"), _("(Fetus 1)"));
    AddTable(fixed, &y, title, OB_M, OB_EAR, OB_MEA_END, m_TableOb, 1);

    //Fetal 2
    //Advanced
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("Advanced"), _("(Fetus 2)"));
    AddTable(fixed, &y, title, OB_M, OB_MEA_START, OB_AFI_LUQ, m_TableOb, 2);
    //AFI&BPP
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("AFI"), _("(Fetus 2)"));
    AddTable(fixed, &y, title, OB_M, OB_AFI_LUQ, OB_CRL, m_TableOb, 2);
    //Early
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("Early"), _("(Fetus 2)"));
    AddTable(fixed, &y, title, OB_M, OB_CRL, OB_RA_DIMEN, m_TableOb, 2);
    //Fetal Heart
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("Fetal Heart"), _("(Fetus 2)"));
    AddTable(fixed, &y, title, OB_M, OB_RA_DIMEN, OB_GS, m_TableOb, 2);
    //General
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("General"), _("(Fetus 2)"));
    AddTable(fixed, &y, title, OB_M, OB_GS, OB_EAR, m_TableOb, 2);
    //EFW
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("EFW"), _("(Fetus 2)"));
    AddOBEfwTable(fixed, &y, title, 2);
    //Other
    sprintf(title, "<i><b><span fgcolor=\"#00FFFF\">%s%s</span></b></i>", _("Other"), _("(Fetus 2)"));
    AddTable(fixed, &y, title, OB_M, OB_EAR, OB_MEA_END, m_TableOb, 2);

    //Growth Curve
    y += 20;
    AddGrowthCurve(fixed, &y);
    //Biophysical Profile
    y += 20;
    AddBiophysicalProfile(fixed, &y);
    AddTable(fixed, &y, _("<i><b>User</b></i>"), OB_M, USER_START, USER_END, m_TableUser);
    return fixed;
}

GtkWidget* ViewReport::CreateNotePageGYN()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create GYN Note\n");

    //General
    AddTable(fixed, &y, _("<i><b>General</b></i>"), GYN_M, GYN_MEA_START, GYN_L_FOLL1, m_TableGyn);
    //Fertility
    AddTable(fixed, &y, _("<i><b>Fertility</b></i>"), GYN_M, GYN_L_FOLL1, GYN_FIBROID1, m_TableGyn);
    //Fibroid
    AddTable(fixed, &y, _("<i><b>Fibroid</b></i>"), GYN_M, GYN_FIBROID1, GYN_MEA_END, m_TableGyn);
    AddTable(fixed, &y,  _("<i><b>User</b></i>"), GYN_M, USER_START, USER_END, m_TableUser);
    return fixed;
}

GtkWidget* ViewReport::CreateNotePageSP()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create SP Note\n");

    //Breast
    AddTable(fixed, &y, _("<i><b>Breast</b></i>"), SP_M, SP_MEA_START, SP_L_TESTIS_L, m_TableSp);
    //Testicle
    AddTable(fixed, &y, _("<i><b>Testicle</b></i>"), SP_M, SP_L_TESTIS_L, SP_ISTHMUS_AP, m_TableSp);
    //Thyroid
    AddTable(fixed, &y, _("<i><b>Thyroid</b></i>"), SP_M, SP_ISTHMUS_AP, SP_L_EYE_OA, m_TableSp);
    //Eye
    AddTable(fixed, &y, _("<i><b>Eyeball</b></i>"), SP_M, SP_L_EYE_OA, SP_MEA_END, m_TableSp);
    AddTable(fixed, &y, _("<i><b>User</b></i>"), SP_M, USER_START, USER_END, m_TableUser);
    return fixed;
}
#ifdef EMP_322
#else
GtkWidget* ViewReport::CreateNotePageVES()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create VES Note\n");

    //Carotid
    AddTable(fixed, &y, _("<i><b>Carotid</b></i>"), VS_M, VS_MEA_START, VS_L_CFA, m_TableVs);
    //LE Art
    AddTable(fixed, &y, _("<i><b>LE Art</b></i>"), VS_M, VS_L_CFA, VS_L_ANTECUBE, m_TableVs);
    //UE Art
    AddTable(fixed, &y, _("<i><b>UE Art</b></i>"), VS_M, VS_L_ANTECUBE, VS_MEA_END, m_TableVs);
    AddTable(fixed, &y,  _("<i><b>User</b></i>"), VS_M, USER_START, USER_END, m_TableUser);
    return fixed;
}
#endif

GtkWidget* ViewReport::CreateNotePageFE()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create Fetal Note\n");

    //Dimen(2D)
    AddTable(fixed, &y, _("<i><b>Dimen(2D)</b></i>"), FETAL_M, FETAL_MEA_START, FETAL_AOR_DIAM, m_TableFetal);
    //Vessels(2D)
    AddTable(fixed, &y, _("<i><b>Vessels(2D)</b></i>"), FETAL_M, FETAL_AOR_DIAM, FETAL_AO_AN_DIAM, m_TableFetal);
    //Valves(2D)
    AddTable(fixed, &y, _("<i><b>Valves(2D)</b></i>"), FETAL_M, FETAL_AO_AN_DIAM, FETAL_RVAWD_MM, m_TableFetal);
    //Dimen(M)
    AddTable(fixed, &y, _("<i><b>Dimen(M)</b></i>"), FETAL_M, FETAL_RVAWD_MM, FETAL_AOR_DIAM_MM, m_TableFetal);
    //Vessels(M)
    AddTable(fixed, &y, _("<i><b>Vessels(M)</b></i>"), FETAL_M, FETAL_AOR_DIAM_MM, FETAL_LV_ET_MM, m_TableFetal);
    //AV MV(M)
    AddTable(fixed, &y, _("<i><b>AV &amp; MV(M)</b></i>"), FETAL_M, FETAL_LV_ET_MM, FETAL_LATE_DIAS_SLOPE_MM, m_TableFetal);
    //PV TV(M)
    AddTable(fixed, &y, _("<i><b>PV &amp; TV(M)</b></i>"), FETAL_M, FETAL_LATE_DIAS_SLOPE_MM, FETAL_PLACENTA, m_TableFetal);
    //Utero Placenta(D)
    AddTable(fixed, &y, _("<i><b>Utero &amp; Placenta(D)</b></i>"), FETAL_M, FETAL_PLACENTA, FETAL_FROAMEN_OVALE, m_TableFetal);
    //Vessels(D)
    AddTable(fixed, &y, _("<i><b>Vessels(D)</b></i>"), FETAL_M, FETAL_FROAMEN_OVALE, FETAL_LVOT_ACC_TIME, m_TableFetal);
    //AV MV(D)
    AddTable(fixed, &y, _("<i><b>AV &amp; MV(D)</b></i>"), FETAL_M, FETAL_LVOT_ACC_TIME, FETAL_RVOT_ACC_TIME, m_TableFetal);
    //PV TV(D)
    AddTable(fixed, &y, _("<i><b>PV &amp; TV(D)</b></i>"), FETAL_M, FETAL_RVOT_ACC_TIME, FETAL_THORACIC_AO, m_TableFetal);
    //Peripheral Vasc(D)

    AddTable(fixed, &y, _("<i><b>Peripheral Vasc(D)</b></i>"), FETAL_M, FETAL_THORACIC_AO, FETAL_MEA_END, m_TableFetal);
    AddTable(fixed, &y,  _("<i><b>User</b></i>"), FETAL_M, USER_START, USER_END, m_TableUser);

    return fixed;
}
#ifdef EMP_322
#else
GtkWidget* ViewReport::CreateNotePageTCD()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create TCD Note\n");

    AddTable(fixed, &y, " ", TCD_M, TCD_MEA_START, TCD_MEA_END, m_TableTcd);

 AddTable(fixed, &y,  _("<i><b>User</b></i>"), TCD_M, USER_START, USER_END, m_TableUser);

    return fixed;
}
#endif
GtkWidget* ViewReport::CreateNotePageORT()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new ();

    PRINTF("Create Ortho Note\n");

    AddTable(fixed, &y, " ", ORTHO_M, ORTHO_MEA_START, ORTHO_MEA_END, m_TableOrtho);
    y += 30;
    AddTable(fixed, &y,  _("<i><b>User</b></i>"), ORTHO_M, USER_START, USER_END, m_TableUser);
    return fixed;
}
#ifdef VET
//添加 动物超 TD报告函数
//hlx
GtkWidget* ViewReport::CreateNotePageTD()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new();

    PRINTF("Create TD Note\n");
    AddTDRes(fixed, &y, "<i><b>Test</b></i>");
//Lesion1~~Lension11
    AddTable(fixed, &y, _("<i><b>Lesion 1</b></i>"),    TD_M,  TD_MEA_START,    TD_MEA_START+4,  m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 2</b></i>"),    TD_M,  TD_MEA_START+4,  TD_MEA_START+8,  m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 3</b></i>"),    TD_M,  TD_MEA_START+8,  TD_MEA_START+12, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 4</b></i>"),    TD_M,  TD_MEA_START+12, TD_MEA_START+16, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 5</b></i>"),    TD_M,  TD_MEA_START+16, TD_MEA_START+20, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 6</b></i>"),    TD_M,  TD_MEA_START+20, TD_MEA_START+24, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 7</b></i>"),    TD_M,  TD_MEA_START+24, TD_MEA_START+28, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 8</b></i>"),    TD_M,  TD_MEA_START+28, TD_MEA_START+32, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 9</b></i>"),    TD_M,  TD_MEA_START+32, TD_MEA_START+36, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 10</b></i>"),   TD_M,  TD_MEA_START+36, TD_MEA_START+40, m_TableTD);
    AddTable(fixed, &y, _("<i><b>Lesion 11</b></i>"),   TD_M,  TD_MEA_START+40, TD_MEA_END,      m_TableTD);
//conclusion
//  AddTable(fixed, &y, "<i><b>Concusion</b></i>",  TD_M,  TD_MEA_START+40, TD_MEA_END,      m_TableTD);
    return fixed;
}
#endif

#ifdef VET
//添加 动物超 ANOB报告函数
//hlx
GtkWidget* ViewReport::CreateNotePageAnOB()
{
    guint y = 30;
    GtkWidget *fixed = gtk_fixed_new();

    PRINTF("Create ANOB NOTE\n");

    AddTable(fixed, &y, _("<i><b>Dog</b></i>"),     ANOB_M,  ANOB_DOG_GSD,   ANOB_DOG_BD+1,      m_TableAnOB, 1);
        //AddTable(fixed, &y, title, OB_M, OB_GS, OB_EAR, m_TableOb, 1);
    AddTable(fixed, &y, _("<i><b>Cat</b></i>"),     ANOB_M,  ANOB_CAT_HD,    ANOB_CAT_BD+1,      m_TableAnOB, 1);
    AddTable(fixed, &y, _("<i><b>Sheep</b></i>"),   ANOB_M,  ANOB_SHEEP_CRL, ANOB_SHEEP_CRL+1,   m_TableAnOB, 1);
    AddTable(fixed, &y, _("<i><b>Swine</b></i>"),   ANOB_M,  ANOB_SWINE_HL,  ANOB_SWINE_SL+1,    m_TableAnOB, 1);
    AddTable(fixed, &y, _("<i><b>Bovine</b></i>"),  ANOB_M,  ANOB_BOVINE_KCRL,ANOB_BOVINE_BUD+1, m_TableAnOB, 1);
    AddTable(fixed, &y, _("<i><b>Equine</b></i>"),  ANOB_M,  ANOB_EQUINE_GSD,ANOB_EQUINE_EED+1,  m_TableAnOB, 1);

    return fixed;
}
#endif

void ViewReport::EventBoxCalendarPress(GtkWidget *widget, GdkEventButton *event)
{
    ViewCalendar::GetInstance()->CreateWindow(m_window, ViewCalendar::START, this);
}

const char* ViewReport::GetCheckPart()
{
    if(!m_notebook)
    {
        if(GTK_IS_NOTEBOOK(m_notebook))
        {
            int n = gtk_notebook_get_current_page(GTK_NOTEBOOK(m_notebook));
            GtkWidget *label = gtk_notebook_get_tab_label(GTK_NOTEBOOK(m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), n));
            return gtk_label_get_text(GTK_LABEL(label));
        }
    }
    return _("ABD");
}

vector<string> ViewReport::GetSelectImage(void)
{
    unsigned int i;
    char *path;
    vector<string> vec;
    vec.clear();

    for(i=0; i<m_vecNameSel.size(); i++)
    {
        path = g_build_filename(STORE_PATH, "0", m_vecNameSel[i].c_str(), NULL);
        vec.push_back(path);
        g_free(path);
    }

    return vec;
}

void ViewReport::TextIndicationInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len)
{
    char *str;
    g_object_get(G_OBJECT(textbuffer), "text", &str, NULL);

    string strInsert = text;
    string strAll = str + strInsert;
//  printf("all str=%s\nnew str=%s\nold str=%s\n", strAll.c_str(), strInsert.c_str(), str);

    int width, height;
    PangoLayout *layout = GetStringSizeInReport(strAll.c_str(), &width, &height, 14);
    g_object_unref(layout);

//  printf("height=%d\n", height);
    if(height > 355-40)
        g_signal_stop_emission_by_name((gpointer)textbuffer, "insert_text");
}

void ViewReport::TextCommentInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len)
{
    char *str;
    g_object_get(G_OBJECT(textbuffer), "text", &str, NULL);

    string strInsert = text;
    string strAll = str + strInsert;
//  printf("all str=%s\nnew str=%s\nold str=%s\n", strAll.c_str(), strInsert.c_str(), str);

    int width, height;
    PangoLayout *layout = GetStringSizeInReport(strAll.c_str(), &width, &height, 14);
    g_object_unref(layout);

//  printf("height=%d\n", height);
    if(height > 195-40)
        g_signal_stop_emission_by_name((gpointer)textbuffer, "insert_text");
}

DCMSRELEMENT ViewReport::GetSRElement()
{
    DCMSRELEMENT SR;
    SR.srIndication = m_strReport[0];
    SR.srComments = m_strReport[1];
    SR.srReferenceImageFilename = m_vecNameSel;
    return SR;
}