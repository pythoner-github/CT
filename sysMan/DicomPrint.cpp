#include "DicomPrint.h"
#include <string.h>

#include <stdio.h>

#include <glib/gi18n.h>


#include <stdlib.h>
#include "SysGeneralSetting.h"
#include "../include/Def.h"
#include "ViewDialog.h"
#include "ViewSystem.h"
#define DISTANCE 230
#define WIDTH   270+80
#define HEIGHT   45

#define SCALE_WIDTH  1
#define SCALE_HEIGHT 1.2
DicomPrint *DicomPrint::m_pInstance = NULL;

DicomPrint::DicomPrint()
{
    char path[256];
    sprintf(path,"%s/%s",CFG_RES_PATH,SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

DicomPrint::~DicomPrint()
{
}

DicomPrint *DicomPrint::GetInstance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new DicomPrint();
    }
    return m_pInstance;
}

static void HandleButtonPrintAddClicked(GtkButton *button, DicomPrint *data)
{
    switch(DicomPrint::GetInstance()->m_currentState)
    {
        case E_INIT:
            {
                if(FALSE == DicomPrint::GetInstance()->AddPrintConfiguration())
                {
                    return;
                }
                ButtonCallBack::GetInstance()->ButtonPrintAddClicked(button,data->m_combobox_print_device,data->m_entry_print_AE,data->m_entry_print_port,data->m_entry_print_service_name,data->m_treeview_print);
                DicomPrint::GetInstance()->StateMachine(E_INIT);
            }
            break;
        case E_ONLYREAD:
            {
                DicomPrint::GetInstance()->StateMachine(E_ONLYWRITE);
            }
            break;
        case E_ONLYWRITE:
            {
                DicomPrint::GetInstance()->SetPrintConfiguration();
                DicomPrint::GetInstance()->StateMachine(E_ONLYREAD);
            }
            break;
    }
}

static void HandleButtonPrintClearClicked(GtkButton *button, DicomPrint *data)
{
    PRINTF("current = %d\n",DicomPrint::GetInstance()->m_currentState);
    switch(DicomPrint::GetInstance()->m_currentState)
    {
        case E_INIT:
            {
                DicomPrint::GetInstance()->StateMachine(E_INIT);
                ButtonCallBack::GetInstance()->ButtonPrintClearClicked(button,data->m_combobox_print_device,data->m_entry_print_service_name,data->m_entry_print_AE,data->m_entry_print_port);
            }
            break;
        case E_ONLYREAD:
            {
                DicomPrint::GetInstance()->StateMachine(E_INIT);
            }
            break;
        case E_ONLYWRITE:
            {
                DicomPrint::GetInstance()->StateMachine(E_ONLYREAD);
            }
            break;
    }
}

static void HandleButtonPrintDeleteClicked(GtkButton *button, DicomPrint *data)
{
    bool ret;
    if(DicomPrint::GetInstance()->m_currentState != E_ONLYWRITE)
    {
        ret = ButtonCallBack::GetInstance()->ButtonPrintDeleteClicked(button,data->m_treeview_print);
        if (ret == TRUE)
            DicomPrint::GetInstance()->StateMachine(E_INIT);
    }
}

static void HandleButtonPrintDefaultClicked(GtkButton *button, DicomPrint *data)
{
    if(DicomPrint::GetInstance()->m_currentState != E_ONLYWRITE)
    {
        ButtonCallBack::GetInstance()->ButtonPrintDefaultClicked(button,data->m_treeview_print);
    }
}

static void HandleButtonPrintConnectClicked(GtkButton *button, DicomPrint *data)
{
    if(DicomPrint::GetInstance()->m_currentState != E_ONLYWRITE)
    {
        ButtonCallBack::GetInstance()->ButtonPrintConnectClicked(button,data->m_treeview_print);
    }
}

static void HandlePrintDeviceChanged(GtkComboBox *combobox,DicomPrint *data)
{
    if (data)
        ButtonCallBack::GetInstance()->PrintDeviceChanged(combobox,data->m_combobox_print_device,data->m_entry_print_service_name);
}

void DicomPrint::HandleStringInsertChar(char *src,char c,int position)
{
	int i = 0;
	int len = strlen(src);
	if(position < 0 || position > len)
		{
			return;
		}
	i = len;
	while(i >= position)
	{

		src[i+1] = src[i];
		i--;
	}
	src[i+1] = c;
	src[len+2] = '\0';

}

void DicomPrint::DigitsLimit(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    string text;
    int old_value;
    int new_value;

    if (g_ascii_isdigit(*new_text))
    {
        text = gtk_entry_get_text(GTK_ENTRY(editable));
        printf("text = %s\n",text.c_str());
        printf("new_text = %c\n",*new_text);
        printf("position = %d\n",*position);
        if(text.length() != 0)
        {
	   HandleStringInsertChar((char *)text.c_str(),*new_text,*position);
	    new_value = atoi(text.c_str());
            if(new_value >= 0 && new_value <= 1024)
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
}

void DicomPrint::CharacterLimit(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (g_ascii_islower(*new_text) ||g_ascii_isupper(*new_text))
    {

    }
    else
    {
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }
}

void DicomPrint::setListFreeze(gboolean enable)
{
    bool buttonEnable;
    m_enableList = enable;

    buttonEnable= enable ? FALSE : TRUE;

    gtk_widget_set_sensitive(m_delete_button,buttonEnable);
    gtk_widget_set_sensitive(m_default_button,buttonEnable);
    gtk_widget_set_sensitive(m_connect_button,buttonEnable);
}

void DicomPrint::init_print_setting()
{
    vector<string> group_device_service = CDCMMan::GetMe()->GetAllDevice();
    bool exist = false;
    GtkTreeModel *model;
    GtkTreeIter iter;
    device_index_t deviceIndex;
    model = gtk_combo_box_get_model(GTK_COMBO_BOX (m_combobox_print_device));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        char *path_string = gtk_tree_model_get_string_from_iter(model,&iter);
        int path_num = atoi(path_string);

        exist = gtk_tree_model_iter_next(model, &iter);
        gtk_combo_box_remove_text (GTK_COMBO_BOX (m_combobox_print_device),path_num);
    }
    device_index.clear();
    for(int i = 0;i<group_device_service.size();i++)
    {
        gtk_combo_box_insert_text (GTK_COMBO_BOX (m_combobox_print_device),i,group_device_service[i].c_str());

        if(group_device_service[i].size() > 30)
        {
            strncpy(deviceIndex.buf,group_device_service[i].c_str(),30);
        }else
        {
            strncpy(deviceIndex.buf,group_device_service[i].c_str(),group_device_service[i].size()+1);

        }
        deviceIndex.index = i;
        device_index.push_back(deviceIndex);
    }
    CDCMMan::GetMe()->GetAllPrintService(GetSingleServiceAttribute, this);
}

bool DicomPrint::AddPrintConfiguration()
{
    char *device;
    string temp;
    DCMPRINTCONFIGELEMENT element;
    const gchar *text;
	const char *ae = gtk_entry_get_text(GTK_ENTRY(m_entry_print_AE));
    const char *port = gtk_entry_get_text(GTK_ENTRY(m_entry_print_port));
    device = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_print_device));

    if(ae[0] == '\0' ||port[0] == '\0'||device[0] == '\0')
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::INFO,
                _("Device can not be empty!"),
                NULL);
        return FALSE;

    }

    element.pcColor = (DCM::EDCMPrintColor)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxColor));
    element.pcFormat = (DCM::EDCMPrintFormat)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxFormat));
    element.pcOrientation = (DCM::EDCMOrientation)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxOrientation));
    element.pcMagnification = (DCM::EDCMMagnification)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxMagnification));
    element.pcMediumType = (DCM::EDCMMediumType)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxMediumType));
    element.pcFilmSize = (DCM::EDCMFilmSize)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxFilmSize));
    element.pcDestination = (DCM::EDCMDestination)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxDestination));
    element.pcPriorities = (DCM::EDCMPriorities)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxPrioritys));
    element.pcCopies = (char)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxCopies))+1;
    temp =  EntryGetTextMinDensity();
    strncpy(element.pcMinDensity,temp.c_str(),10);

    temp =  EntryGetTextMaxDensity();
    strncpy(element.pcMaxDensity,temp.c_str(),10);

    text = gtk_entry_get_text(GTK_ENTRY(m_EntryConfInfo));
    strncpy(element.pcConfigurationInformation,text,101);

    if(FALSE == (CDCMMan::GetMe()->AddPrintServiceConfiguration(device, element)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::INFO,
                _("Fail to add the device configuration!"),
                NULL);

        return FALSE;

    }
    return TRUE;
}

bool DicomPrint::SetPrintConfiguration()
{
    char device[30];
    string temp;
    //DCMPRINTCONFIGELEMENT element;
    const char *text;
    strncpy(device,combo_box_get_text(GTK_COMBO_BOX(m_combobox_print_device)),30);

    element.pcColor = (DCM::EDCMPrintColor)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxColor));
    element.pcFormat = (DCM::EDCMPrintFormat)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxFormat));
    element.pcOrientation = (DCM::EDCMOrientation)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxOrientation));
    element.pcMagnification = (DCM::EDCMMagnification)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxMagnification));
    element.pcMediumType = (DCM::EDCMMediumType)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxMediumType));
    element.pcFilmSize = (DCM::EDCMFilmSize)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxFilmSize));
    element.pcDestination = (DCM::EDCMDestination)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxDestination));
    element.pcPriorities = (DCM::EDCMPriorities)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxPrioritys));
    element.pcCopies =(char)gtk_combo_box_get_active (GTK_COMBO_BOX(m_comboBoxCopies))+1;

    temp =  EntryGetTextMinDensity();
    strncpy(element.pcMinDensity,temp.c_str(),10);
    temp =  EntryGetTextMaxDensity();
    strncpy(element.pcMaxDensity,temp.c_str(),10);
    text = gtk_entry_get_text(GTK_ENTRY(m_EntryConfInfo));
    if(text != NULL)
    {
        strncpy(element.pcConfigurationInformation,text,101);
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::INFO,
                _("ConfigurationInformation cant not be NULL!"),
                NULL);
        return FALSE;

    }

    if(FALSE == (CDCMMan::GetMe()->SetPrintServiceConfiguration(device, element)))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::INFO,
                _("Fail to update the device configuration!"),
                NULL);

        return FALSE;

    }
    return TRUE;
}

void DicomPrint::UpdatePrintConfiguration(DCMPRINTCONFIGELEMENT &element)
{

    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxColor),element.pcColor);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxFormat),element.pcFormat);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxOrientation),element.pcOrientation);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxMagnification),element.pcMagnification);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxMediumType),element.pcMediumType);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxFilmSize),element.pcFilmSize);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxPrioritys),element.pcPriorities);
	gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxDestination),element.pcDestination);
    gtk_combo_box_set_active (GTK_COMBO_BOX(m_comboBoxCopies),element.pcCopies -1);
    gtk_entry_set_text (GTK_ENTRY(m_entryMinDensity),element.pcMinDensity);
    gtk_entry_set_text (GTK_ENTRY(m_entryMaxDensity),element.pcMaxDensity);
    gtk_entry_set_text (GTK_ENTRY(m_EntryConfInfo),element.pcConfigurationInformation);
}

GtkWidget *DicomPrint::createNotePrint()
{
    GtkWidget *fixed_print;
    GtkWidget *frame_print_equipment;
    GtkWidget *  label_print_equipment;
    GtkWidget *fixed;
    widgetInfo_t widgetInfo;
    buttonInfo_t buttonInfo;
    GtkWidget *frame_print_property;
    GtkWidget  *label_print_property;
    GtkWidget *fixed_print_property;

    GtkWidget *frame_service_list;
    GtkWidget *label_service_list ;
    GtkWidget *fixed_service_list;
    GtkTreeSelection *selected;

    fixed_print = gtk_fixed_new();

    gtk_widget_show(fixed_print);

	frame_print_equipment = gtk_frame_new (NULL);
    gtk_widget_show (frame_print_equipment);
    gtk_fixed_put (GTK_FIXED (fixed_print), frame_print_equipment, 0, 0);
    gtk_widget_set_size_request (frame_print_equipment, ((900.0-10)/1.25)*SCALE_WIDTH, (60/1.33)*SCALE_HEIGHT);
    gtk_frame_set_label_align (GTK_FRAME (frame_print_equipment), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print_equipment), GTK_SHADOW_IN);

 label_print_equipment = gtk_label_new (_("<b>Device Property</b>"));
//	label_worklist_equipment = gtk_label_new ("Service Property");
    gtk_widget_show (label_print_equipment);
    gtk_frame_set_label_widget (GTK_FRAME (frame_print_equipment), label_print_equipment);
    gtk_label_set_use_markup (GTK_LABEL (label_print_equipment), TRUE);

fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame_print_equipment), fixed);
//==================Device=======================
	strncpy(widgetInfo.labelTitle,_("<b>Device :</b>"),50);
	widgetInfo.label_width = (100.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.label_x = 0;
	widgetInfo.label_y = 0;
	widgetInfo.entry_width = (110.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_x = (100.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_y = 0;
	 widgetInfo.parent = fixed;
	createComboLabel(widgetInfo);
m_combobox_print_device = m_combobox_device;
g_signal_connect(G_OBJECT(m_combobox_print_device),"changed",G_CALLBACK(HandlePrintDeviceChanged),this);
//gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"scp");
//gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"hehe");
//==================Service Name=======================
strncpy(widgetInfo.labelTitle,_("<b>Service Name :</b>"),50);
	widgetInfo.label_width = (160.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.label_x = ((0+DISTANCE-30.0)/1.25)*SCALE_WIDTH;
	widgetInfo.label_y = 0;
	widgetInfo.entry_width = (110.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_x = ((130.0+DISTANCE)/1.25)*SCALE_WIDTH;
	widgetInfo.entry_y = 0;
	 widgetInfo.parent = fixed;

	createEntryLabel(widgetInfo);
m_entry_print_service_name   = m_entry_print_name;
//==================AE Title=======================
strncpy(widgetInfo.labelTitle,_("<b>AE Title :</b>"),50);
	widgetInfo.label_width = (130.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.label_x = (0+DISTANCE+DISTANCE-30.0)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = 0;
	widgetInfo.entry_width = (110.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_x = ((160.0+DISTANCE+DISTANCE-60)/1.25)*SCALE_WIDTH;
	widgetInfo.entry_y = 0;
	 widgetInfo.parent = fixed;

    createEntryLabel(widgetInfo);
    m_entry_print_AE   = m_entry_print_name;

//==================Port=======================
strncpy(widgetInfo.labelTitle,_("<b>Port :</b>"),50);
	widgetInfo.label_width = (130.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.label_x = ((0+DISTANCE+DISTANCE+DISTANCE-60.0)/1.25)*SCALE_WIDTH;
	widgetInfo.label_y = 0;
	widgetInfo.entry_width = (110.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_x = ((160.0+DISTANCE+DISTANCE-30+DISTANCE-60)/1.25)*SCALE_WIDTH;
	widgetInfo.entry_y = 0;
	 widgetInfo.parent = fixed;

	createEntryLabel(widgetInfo);
	m_entry_print_port = m_entry_print_name;
//==================add=======================
	strncpy(buttonInfo.label,_("Add"),50);
	buttonInfo.x = (50.0/1.25)*SCALE_WIDTH;
	buttonInfo.y = (270.0/1.33)*SCALE_HEIGHT;
	buttonInfo.width = (110.0/1.25)*SCALE_WIDTH;
	buttonInfo.height = (35.0/1.33)*SCALE_HEIGHT;
	buttonInfo.parent = fixed_print;
	createButton(buttonInfo);
    m_button_add = m_button_print_add;
g_signal_connect(m_button_add, "clicked", G_CALLBACK(HandleButtonPrintAddClicked), this);
//==================clear=======================
	strncpy(buttonInfo.label,_("Clear"),50);
	buttonInfo.x = (250.0/1.25)*SCALE_WIDTH;
	buttonInfo.y = (270.0/1.33)*SCALE_HEIGHT;
	buttonInfo.width = (110.0/1.25)*SCALE_WIDTH;
	buttonInfo.height = (35.0/1.33)*SCALE_HEIGHT;
	buttonInfo.parent = fixed_print;
	createButton(buttonInfo);
    m_button_clear = m_button_print_add;
g_signal_connect(m_button_clear, "clicked", G_CALLBACK(HandleButtonPrintClearClicked), this);

frame_print_property = gtk_frame_new (NULL);
    gtk_widget_show (frame_print_property);
    gtk_fixed_put (GTK_FIXED (fixed_print), frame_print_property, 0, 60.0/1.33*SCALE_HEIGHT);
    gtk_widget_set_size_request (frame_print_property, ((900.0-10)/1.25)*SCALE_WIDTH, ((300.0-90)/1.33)*SCALE_HEIGHT);
    gtk_frame_set_label_align (GTK_FRAME (frame_print_property), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print_property), GTK_SHADOW_IN);

// label_worklist_equipment = gtk_label_new (_("<b>Device Property</b>"));
	label_print_property = gtk_label_new (_("<b>Print Property</b>"));
    gtk_widget_show (label_print_property);
    gtk_frame_set_label_widget (GTK_FRAME (frame_print_property), label_print_property);
    gtk_label_set_use_markup (GTK_LABEL (label_print_property), TRUE);

	fixed_print_property = gtk_fixed_new ();
    gtk_widget_show (fixed_print_property);
    gtk_container_add (GTK_CONTAINER (frame_print_property), fixed_print_property );
	m_fixed_print_property = fixed_print_property;
//==================Color=======================
	strncpy(widgetInfo.labelTitle,_("<b>Color :</b>"),50);
	widgetInfo.label_width = (138.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_y = (20.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_width = (110.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_x = ((122.0+18)/1.25)*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0/1.33)*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxColor = m_combobox_device;
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Grayscale");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Color");
//==================Format=======================
	strncpy(widgetInfo.labelTitle,_("<b>Format :</b>"),50);
	widgetInfo.label_width = (118.0/1.25)*SCALE_WIDTH;
	widgetInfo.label_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_width = (110.0/1.25)*SCALE_WIDTH;
	widgetInfo.entry_height = (30.0/1.33)*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0/1.33)*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxFormat = m_combobox_device;
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 1x1");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 1x2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 2x2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 2x3");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 3x3");

	gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 3x4");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 3x5");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 4x4");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 4x5");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"STD 4x6");



//==================Orientation=======================
	strncpy(widgetInfo.labelTitle,_("<b>Orientation :</b>"),50);
	widgetInfo.label_width = 118.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+2*WIDTH)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = 20.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+2*WIDTH)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = 20.0/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxOrientation = m_combobox_device;

gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Portrait");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Landscape");


//==================Magnification=======================
	strncpy(widgetInfo.labelTitle,_("<b>Magnification :</b>"),50);
	widgetInfo.label_width = 140.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = 2.0/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (122.0+18)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+HEIGHT)/1.25*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxMagnification = m_combobox_device;
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Replicate");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Bilinear");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Cubic");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"None");

//==================Min Density=======================
strncpy(widgetInfo.labelTitle,_("<b>Min Denisty :</b>"),50);
	widgetInfo.label_width = 118.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;

	createEntryLabel(widgetInfo);
	g_signal_connect (G_OBJECT (m_entry_print_name), "insert-text", G_CALLBACK(HandleDigitsLimit), this);
    m_entryMinDensity = m_entry_print_name;;
//==================Max Density=======================
strncpy(widgetInfo.labelTitle,_("<b>Max Denisty :</b>"),50);
	widgetInfo.label_width = 128.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+2*WIDTH - 10)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+2*WIDTH)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;

	createEntryLabel(widgetInfo);
	g_signal_connect (G_OBJECT (m_entry_print_name), "insert-text", G_CALLBACK(HandleDigitsLimit), this);
    m_entryMaxDensity = m_entry_print_name;
//==================Medium Type=======================
	strncpy(widgetInfo.labelTitle,_("<b>Medium Type :</b>"),50);
	widgetInfo.label_width = 134.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = 2.0/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+2*HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (122.0+18)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+2*HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxMediumType = m_combobox_device;

gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Paper");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Clear Film");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Blue Film");


//==================Film Size=======================
	strncpy(widgetInfo.labelTitle,_("<b>Film Size :</b>"),50);
	widgetInfo.label_width = 118.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+2*HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+2*HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxFilmSize = m_combobox_device;
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"8INx10IN");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"10INx12IN");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"10INx14IN");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"14INx14IN");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"14INx17IN");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"24CMx24CM");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"24CMx30CM");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"A4");


//==================Destination=======================
strncpy(widgetInfo.labelTitle,_("<b>Destination :</b>"),50);
	widgetInfo.label_width = 118.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+2*WIDTH)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+2*HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+2*WIDTH)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+2*HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxDestination = m_combobox_device;
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Magazine");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Processor");



//==================Prioritys=======================
strncpy(widgetInfo.labelTitle,_("<b>Prioritys :</b>"),50);
	widgetInfo.label_width = 138.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = 2.0/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+3*HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (122.0+18)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+3*HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"High");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Middle");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"Low");
    m_comboBoxPrioritys = m_combobox_device;
//==================Copies=======================
strncpy(widgetInfo.labelTitle,_("<b>Copies :</b>"),50);
	widgetInfo.label_width = 118.0/1.25*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+3*HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+WIDTH-70)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+3*HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createComboLabel(widgetInfo);
    m_comboBoxCopies = m_combobox_device;
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"1");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"2");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"3");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"4");
gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_device),"5");

//==================Configuration Information=======================
strncpy(widgetInfo.labelTitle,_("<b>Configuration Info :</b>"),50);
	widgetInfo.label_width = 227.0/1.22*SCALE_WIDTH;
	widgetInfo.label_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.label_x = (2.0+2*WIDTH-110-4)/1.25*SCALE_WIDTH;
	widgetInfo.label_y = (20.0+3*HEIGHT)/1.33*SCALE_HEIGHT;
	widgetInfo.entry_width = 110.0/1.25*SCALE_WIDTH;
	widgetInfo.entry_height = 30.0/1.33*SCALE_HEIGHT;
	widgetInfo.entry_x = (120.0+2*WIDTH)/1.25*SCALE_WIDTH;
	widgetInfo.entry_y = (20.0+3*HEIGHT)/1.33*SCALE_HEIGHT;
	 widgetInfo.parent = fixed_print_property;
	createEntryLabel(widgetInfo);
	//g_signal_connect (G_OBJECT (m_entry_worklist_name), "insert-text", G_CALLBACK(HandleCharacterLimit), this);
    m_EntryConfInfo = m_entry_print_name;
frame_service_list = gtk_frame_new (NULL);
    gtk_widget_show (frame_service_list);
    gtk_fixed_put (GTK_FIXED (fixed_print), frame_service_list, 0, 300.0/1.33*SCALE_HEIGHT);
    gtk_widget_set_size_request (frame_service_list, (900.0-10)/1.25*SCALE_WIDTH, 170/1.33*SCALE_HEIGHT);
    gtk_frame_set_label_align (GTK_FRAME (frame_service_list), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_service_list), GTK_SHADOW_IN);

// label_worklist_equipment = gtk_label_new (_("<b>Device Property</b>"));
	label_service_list = gtk_label_new (_("<b>Service List</b>"));
    gtk_widget_show (label_service_list);
    gtk_frame_set_label_widget (GTK_FRAME (frame_service_list), label_service_list);
    gtk_label_set_use_markup (GTK_LABEL (label_service_list), TRUE);

	fixed_service_list = gtk_fixed_new ();
    gtk_widget_show (fixed_service_list);
    gtk_container_add (GTK_CONTAINER (frame_service_list), fixed_service_list );



GtkWidget *scrollWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show (scrollWin);
    gtk_fixed_put (GTK_FIXED (fixed_service_list), scrollWin, 0, 0);
    gtk_widget_set_size_request (scrollWin, (900.0-20)/1.25*SCALE_WIDTH, (130.0-30)/1.33*SCALE_HEIGHT);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollWin), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrollWin), GTK_SHADOW_IN);

    m_treeview_print = create_service_treeview();
	gtk_container_add(GTK_CONTAINER(scrollWin), m_treeview_print);
	gtk_widget_set_size_request (m_treeview_print, 250/1.25*SCALE_WIDTH, 120/1.33*SCALE_HEIGHT);
    gtk_widget_show (m_treeview_print);
	selected= gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_print));
	g_signal_connect (G_OBJECT (selected),"changed", G_CALLBACK(HandleListClicked), this);
	gtk_widget_add_events(m_treeview_print,GDK_BUTTON_PRESS_MASK|GDK_BUTTON_MOTION_MASK);
	g_signal_connect (G_OBJECT (m_treeview_print),"button-press-event", G_CALLBACK(HandleIsEnableList), this);
//==================delete=======================
	strncpy(buttonInfo.label,_("Delete"),50);
	buttonInfo.x = 50.0/1.25*SCALE_WIDTH;
	buttonInfo.y = 100.0/1.33*SCALE_HEIGHT;
	buttonInfo.width = 110.0/1.25*SCALE_WIDTH;
	buttonInfo.height = 35.0/1.33*SCALE_HEIGHT;
	buttonInfo.parent = fixed_service_list;
	createButton(buttonInfo);
	m_delete_button = m_button_print_add;


g_signal_connect(m_button_print_add, "clicked", G_CALLBACK(HandleButtonPrintDeleteClicked), this);
//==================Default=======================
	strncpy(buttonInfo.label,_("Default"),50);
	buttonInfo.x = (180.0+50)/1.25*SCALE_WIDTH;
	buttonInfo.y = 100.0/1.33*SCALE_HEIGHT;
	buttonInfo.width = 110.0/1.25*SCALE_WIDTH;
	buttonInfo.height = 35.0/1.33*SCALE_HEIGHT;
	buttonInfo.parent = fixed_service_list;
	createButton(buttonInfo);
	m_default_button = m_button_print_add;
g_signal_connect(m_button_print_add, "clicked", G_CALLBACK(HandleButtonPrintDefaultClicked), this);
//==================Connect Test=======================
	strncpy(buttonInfo.label,_("Connect Test"),50);
	buttonInfo.x = (310.0+100)/1.25*SCALE_WIDTH;
	buttonInfo.y = 100.0/1.33*SCALE_HEIGHT;
	buttonInfo.width = 170.0/1.25*SCALE_WIDTH;
	buttonInfo.height = 35.0/1.33*SCALE_HEIGHT;
	buttonInfo.parent = fixed_service_list;
	createButton(buttonInfo);
	m_connect_button = m_button_print_add;
g_signal_connect(m_button_print_add, "clicked", G_CALLBACK(HandleButtonPrintConnectClicked), this);
SetPrintPropertyDefault();

	setListFreeze(FALSE);
    StateMachine(E_INIT);
//init_worklist_setting();
	return fixed_print;
}

void DicomPrint::GetSingleServiceAttribute(string device, string serviceName,string aeTitle, int port, bool isDefault,void *data)
{
    char tmp_port[256];
    char tmp_isDefault[20];
    char tmp_device[256];
    sprintf(tmp_port,"%d",port);
    sprintf(tmp_device,"%s",device.c_str());

    if (isDefault)
        sprintf(tmp_isDefault,"%s",_("Yes"));
    else
        sprintf(tmp_isDefault,"%s"," ");


    PRINTF("---------GetSingleServiceAttribute:%s %d\n",device.c_str(),tmp->m_page_num);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gboolean exist = FALSE;
    char *tmp_device1;


    model = gtk_tree_view_get_model(GTK_TREE_VIEW(DicomPrint::GetInstance()->m_treeview_print));
    exist = gtk_tree_model_get_iter_first(model, &iter);
    while(exist)
    {
        gtk_tree_model_get(model,&iter,
                COL_STORAGE_DEVICE,&tmp_device1,
                -1);

        if (strcmp(tmp_device,tmp_device1)==0)
        {
            //printf("Add failed\n");
            return;
        }
        exist = gtk_tree_model_iter_next(model, &iter);
    }
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            COL_STORAGE_DEVICE, device.c_str(),
            COL_SERVICE_NAME,serviceName.c_str(),
            COL_AE_TITLE,aeTitle.c_str(),
            COL_PORT,tmp_port,
            COL_DEFAULT,tmp_isDefault,
            -1);

    gtk_tree_model_iter_next(model, &iter);
}

GtkWidget* DicomPrint::create_service_treeview()
{
    GtkWidget *treeview;
    GtkTreeModel *model = NULL;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Device"), renderer, "text", COL_STORAGE_DEVICE, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Service Name"), renderer, "text", COL_SERVICE_NAME, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 200, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("AE Title"), renderer, "text", COL_AE_TITLE, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Port"), renderer, "text", COL_PORT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Default"), renderer, "text", COL_DEFAULT, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    g_object_set(G_OBJECT(column),  "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

    model = create_service_model();
    if (model != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model);

    return treeview;
}

GtkTreeModel* DicomPrint::create_service_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(NUM_COLS,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING);

    return GTK_TREE_MODEL (store);
}

void DicomPrint::createComboLabel(widgetInfo_t widgetInfo)
{
    GtkWidget *label_print_device;
    label_print_device = gtk_label_new (widgetInfo.labelTitle);

    gtk_fixed_put (GTK_FIXED (widgetInfo.parent), label_print_device, widgetInfo.label_x, widgetInfo.label_y);
    gtk_label_set_use_markup (GTK_LABEL (label_print_device), TRUE);
    gtk_widget_set_size_request (label_print_device, widgetInfo.label_width, widgetInfo.label_height);
    gtk_misc_set_alignment (GTK_MISC(label_print_device), 1, 0.5);
    gtk_widget_show (label_print_device);

    m_combobox_device = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_device);
    gtk_fixed_put (GTK_FIXED (widgetInfo.parent), m_combobox_device, widgetInfo.entry_x, widgetInfo.entry_y);
    gtk_widget_set_size_request (m_combobox_device, widgetInfo.entry_width, widgetInfo.entry_height);
}

void DicomPrint::createEntryLabel(widgetInfo_t widgetInfo)
{
    GtkWidget *label_print_name;

    label_print_name = gtk_label_new (widgetInfo.labelTitle);
    gtk_widget_show (label_print_name);
    gtk_fixed_put (GTK_FIXED (widgetInfo.parent), label_print_name, widgetInfo.label_x, widgetInfo.label_y);
    gtk_label_set_use_markup (GTK_LABEL (label_print_name), TRUE);
    gtk_widget_set_size_request (label_print_name, widgetInfo.label_width, widgetInfo.label_height);
    gtk_misc_set_alignment (GTK_MISC(label_print_name), 1, 0.5);

    m_entry_print_name = gtk_entry_new ();
    gtk_widget_show (m_entry_print_name);
    gtk_fixed_put (GTK_FIXED (widgetInfo.parent), m_entry_print_name, widgetInfo.entry_x, widgetInfo.entry_y);
    gtk_widget_set_size_request (m_entry_print_name, widgetInfo.entry_width, widgetInfo.entry_height);
    gtk_entry_set_max_length (GTK_ENTRY (m_entry_print_name), 45);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_print_name), 9679);
}

void DicomPrint::StateMachine(PropertyState currentState)
{
    GtkTreeSelection *selected;
    switch(currentState)
    {
        case E_INIT:
            {
                UnlockDeviceProperty();
                UnlockPrintProperty();
                gtk_button_set_label(GTK_BUTTON(m_button_add),_("Add"));
                gtk_button_set_label(GTK_BUTTON(m_button_clear),_("Clear"));

                selected= gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_print));
                gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(selected));

                setListFreeze(FALSE);
                SetPrintPropertyDefault();
            }
            break;
        case E_ONLYREAD:
            {
                LockDeviceProperty();
                LockPrintProperty();
                gtk_button_set_label(GTK_BUTTON(m_button_add),_("Edit"));
                gtk_button_set_label(GTK_BUTTON(m_button_clear),_("Clear"));

                gtk_entry_set_text (GTK_ENTRY(m_entry_print_service_name),service);
                gtk_entry_set_text (GTK_ENTRY(m_entry_print_AE),ae);
                gtk_entry_set_text (GTK_ENTRY(m_entry_print_port),port);

                DicomPrint::GetInstance()->UpdatePrintConfiguration(element);
                setListFreeze(FALSE);
            }
            break;
        case E_ONLYWRITE:
            {
                LockDeviceProperty();
                UnlockPrintProperty();
                gtk_button_set_label(GTK_BUTTON(m_button_add),_("Save"));
                gtk_button_set_label(GTK_BUTTON(m_button_clear),_("Cancel"));



                setListFreeze(TRUE);
            }
            break;

    }
    m_currentState = currentState;

}

void DicomPrint::LockDeviceProperty()
{
    gtk_widget_set_sensitive(m_combobox_print_device,FALSE);
    gtk_widget_set_sensitive(m_entry_print_service_name,FALSE);
    gtk_widget_set_sensitive(m_entry_print_AE,FALSE);
    gtk_widget_set_sensitive(m_entry_print_port,FALSE);

}

void DicomPrint::UnlockDeviceProperty()
{
    gtk_widget_set_sensitive(m_combobox_print_device,TRUE);
    gtk_widget_set_sensitive(m_entry_print_service_name,TRUE);
    gtk_widget_set_sensitive(m_entry_print_AE,TRUE);
    gtk_widget_set_sensitive(m_entry_print_port,TRUE);

}

void DicomPrint::LockPrintProperty()
{

    //gtk_widget_set_sensitive(m_fixed_print_property,FALSE);
    gtk_widget_set_sensitive(m_comboBoxColor,FALSE);
    gtk_widget_set_sensitive(m_comboBoxFormat,FALSE);
    gtk_widget_set_sensitive(m_comboBoxOrientation,FALSE);
    gtk_widget_set_sensitive(m_comboBoxMagnification,FALSE);
    gtk_widget_set_sensitive(m_comboBoxMediumType,FALSE);
    gtk_widget_set_sensitive(m_comboBoxFilmSize,FALSE);
    gtk_widget_set_sensitive(m_comboBoxDestination,FALSE);
    gtk_widget_set_sensitive(m_comboBoxPrioritys,FALSE);
    gtk_widget_set_sensitive(m_comboBoxCopies,FALSE);
    gtk_widget_set_sensitive(m_EntryConfInfo,FALSE);
    gtk_widget_set_sensitive(m_entryMinDensity,FALSE);
    gtk_widget_set_sensitive(m_entryMaxDensity,FALSE);
}

void DicomPrint::UnlockPrintProperty()
{
    gtk_widget_set_sensitive(m_comboBoxColor,TRUE);
    gtk_widget_set_sensitive(m_comboBoxFormat,TRUE);
    gtk_widget_set_sensitive(m_comboBoxOrientation,TRUE);
    gtk_widget_set_sensitive(m_comboBoxMagnification,TRUE);
    gtk_widget_set_sensitive(m_comboBoxMediumType,TRUE);
    gtk_widget_set_sensitive(m_comboBoxFilmSize,TRUE);
    gtk_widget_set_sensitive(m_comboBoxDestination,TRUE);
    gtk_widget_set_sensitive(m_comboBoxPrioritys,TRUE);
    gtk_widget_set_sensitive(m_comboBoxCopies,TRUE);
    gtk_widget_set_sensitive(m_EntryConfInfo,TRUE);
    gtk_widget_set_sensitive(m_entryMinDensity,TRUE);
    gtk_widget_set_sensitive(m_entryMaxDensity,TRUE);
}

void DicomPrint::SetPrintPropertyDefault()
{
    int i = 0;
    property_t default_buf[9] = {{1,m_comboBoxColor},
        {0,m_comboBoxFormat},
        {0,m_comboBoxOrientation},
        {2,m_comboBoxMagnification},
        {2,m_comboBoxMediumType},
        {0,m_comboBoxFilmSize},
        {1,m_comboBoxDestination},
        {1,m_comboBoxPrioritys},
        {0,m_comboBoxCopies}};
    for(i = 0;i < 9;i++)
    {
        gtk_combo_box_set_active (GTK_COMBO_BOX(default_buf[i].comboText),
                default_buf[i].active);
    }
	gtk_entry_set_text (GTK_ENTRY(m_entryMinDensity),"");
	gtk_entry_set_text (GTK_ENTRY(m_entryMaxDensity),"");
	gtk_entry_set_text (GTK_ENTRY(m_EntryConfInfo),"");

}

void DicomPrint::SetDevicePropertyDefault()
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_print_device),-1);
    gtk_entry_set_text(GTK_ENTRY(m_entry_print_name),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_print_AE),"");
    gtk_entry_set_text(GTK_ENTRY(m_entry_print_port),"");

}

gchar *DicomPrint::combo_box_get_text(GtkComboBox *comboBox)
{
    gchar *text;

    text = gtk_combo_box_get_active_text(comboBox);

    PRINTF("text = %s\n",text);

    return text;
}

void DicomPrint::createButton(buttonInfo_t buttonInfo)
{
    m_button_print_add = gtk_button_new_with_mnemonic (buttonInfo.label);
    gtk_widget_show (m_button_print_add);
    gtk_fixed_put (GTK_FIXED (buttonInfo.parent), m_button_print_add, buttonInfo.x, buttonInfo.y);
    gtk_widget_set_size_request (m_button_print_add, buttonInfo.width, buttonInfo.height);
    //g_signal_connect(button_print_add, "clicked", G_CALLBACK(HandleButtonPrintAddClicked), this);
}

string DicomPrint::entry_get_text(GtkEntry *entry)
{
    string text;
    text = gtk_entry_get_text(GTK_ENTRY(entry));
    PRINTF("text = %s\n",text.c_str());

    return text;
}

string DicomPrint::EntryGetTextMinDensity()
{
    string text;
    text = entry_get_text(GTK_ENTRY(m_entryMinDensity));
    return text;
}

string DicomPrint::EntryGetTextMaxDensity()
{
    string text;
    text = entry_get_text(GTK_ENTRY(m_entryMaxDensity));
    return text;
}
