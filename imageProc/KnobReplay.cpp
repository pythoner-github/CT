#include <deque>
#include "imageProc/KnobReplay.h"
#include "display/KnobMenu.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/Replay.h"
#include "Def.h"
#include "display/gui_global.h"
#include "patient/VideoMan.h"
#include "periDevice/PeripheralMan.h"
#include "sysMan/SysOptions.h"
#include "display/HintArea.h"
#include "display/ViewDialog.h"
#include "ViewMain.h"
#include "imageProc/ModeStatus.h"
#include "sysMan/ViewSuperuser.h"

using std::deque;
#if (defined (CT_322) || defined(CT_313))
KnobMenu::KnobItem KnobReplayMenu[6] = {
    {N_("Replay"), "", MIN, NULL, ReplayChgCtrl},
    {N_("Replay Speed"), "", MIN, NULL, PressChgSpeed},
    {N_("Trim Left"),"", PRESS, NULL, ReplayChgTrimLeft},
    {N_("Trim Right"),"", PRESS, NULL, ReplayChgTrimRight},
    {N_("Reset Trim"),N_("Press"), PRESS, NULL, ReplayChgResetTrim},
    {N_("Save Cine"), N_("Press"), PRESS, NULL, ReplayChgSaveCine},
   // {"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL}
};
#elif (defined (CT_460) || defined(CT_355))
KnobMenu::KnobItem KnobReplayMenu[5] = {
    {N_("Replay"), "", MIN, ReplayChgSpeed, NULL},
    {N_("Trim Left"),"", PRESS, ReplayChgTrimLeft460, NULL},
    {N_("Trim Right"),"", PRESS, ReplayChgTrimRight460, NULL},
    {N_("Reset Trim"),N_("Press"), PRESS, ReplayChgResetTrim460, NULL},
    {N_("Save Cine"), N_("Press"), PRESS, ReplayChgSaveCine460, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL}
};
#else
KnobMenu::KnobItem KnobReplayMenu[5] = {
    {N_("Replay/Speed"), "", MIN, ReplayChgSpeed, ReplayChgCtrl},
    {N_("Trim Left"),"", PRESS, NULL, ReplayChgTrimLeft},
    {N_("Trim Right"),"", PRESS, NULL, ReplayChgTrimRight},
    {N_("Reset Trim"),N_("Press"), PRESS, NULL, ReplayChgResetTrim},
    {N_("Save Cine"), N_("Press"), PRESS, NULL, ReplayChgSaveCine},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL},
    //{"", "", ERROR, NULL, NULL}
};
#endif

KnobMenu::KnobItem DemoKnobReplayMenu[5] = {
    {N_("Replay/Speed"), "", MIN, ReplayChgSpeed, ReplayChgCtrl},
    //{N_("Trim Left"),"", PRESS, NULL, ReplayChgTrimLeft},
    //{N_("Trim Right"),"", PRESS, NULL, ReplayChgTrimRight},
    //{N_("Reset Trim"),N_("Press"), PRESS, NULL, ReplayChgResetTrim},
    //{N_("Save Cine"), N_("Press"), PRESS, NULL, ReplayChgSaveCine},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

const char * Toggle_Replay[2] =
{
    N_("OFF"),
    N_("ON")
};

void KnobReplayCreate()
{
    if(ViewSuperuser::GetInstance()->GetDemoStatus())
    {
        KnobMenu::GetInstance()->SetItem(DemoKnobReplayMenu, sizeof(DemoKnobReplayMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::REPLAY);
    }
    else
    {
        KnobReplayMenu[REPLAY_CINE].status = PRESS;
        KnobMenu::GetInstance()->SetItem(KnobReplayMenu, sizeof(KnobReplayMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::REPLAY);
    }
}

void KnobCineCreate()
{
    KnobReplayCreate();
}

//sync
void SyncKnobReplay(EKnobReplay type, const char* s, EKnobReturn status, bool draw)
{
    if(ViewSuperuser::GetInstance()->GetDemoStatus())
    {
        if(type == REPLAY_CTRL)
        {
            sprintf(DemoKnobReplayMenu[type].value, N_("%s"), s);
            DemoKnobReplayMenu[type].status = status;
        }
    }
    else
    {
        sprintf(KnobReplayMenu[type].value, N_("%s"), s);
        KnobReplayMenu[type].status = status;
    }
    if (draw)
        KnobMenu::GetInstance()->Update();
}

///> callback function
EKnobReturn ReplayChgCtrl(void)
{
    EKnobReturn ret;
    ret = FreezeMode::GetInstance()->ChangeAutoReplay();
    return ret;
}

EKnobReturn ReplayChgSpeed(EKnobOper oper)
{
    EKnobReturn ret;
    ret = Replay::GetInstance()->ChangeSpeed(oper);
    return ret;
}

EKnobReturn PressChgSpeed(void)
{
    EKnobReturn ret;
    ret = Replay::GetInstance()->ChangeSpeed(ROTATE);
    return ret;
}

EKnobReturn ReplayChgTrimLeft(void)
{
#ifndef CT_460
#ifndef CT_355
    Replay::GetInstance()->SetTrimLeft();
#endif
#endif
    return PRESS;
}

EKnobReturn ReplayChgTrimRight(void)
{
#ifndef CT_460
#ifndef CT_355
    Replay::GetInstance()->SetTrimRight();
#endif
#endif
    return PRESS;
}

EKnobReturn ReplayChgResetTrim(void)
{
#ifndef CT_460
#ifndef CT_355
    Replay::GetInstance()->ResetTrim();
#endif
#endif
    return PRESS;
}

#if (defined(CT_460) || defined(CT_355))
EKnobReturn ReplayChgTrimLeft460(EKnobOper oper)
{
    Replay::GetInstance()->SetTrimLeft(oper);
    return PRESS;
}

EKnobReturn ReplayChgTrimRight460(EKnobOper oper)
{
    Replay::GetInstance()->SetTrimRight(oper);
    return PRESS;
}

EKnobReturn ReplayChgResetTrim460(EKnobOper oper)
{
    Replay::GetInstance()->ResetTrim(oper);
    return PRESS;
}
#endif

static VideoMan::VideoItem item;
static gboolean DelayToSaveCine(gpointer data)
{
    // save to image manage
    if(VideoMan::GetInstance()->GetVideoStorage() == VideoMan::UDISK)
    {
        PeripheralMan* ptrPeripheralMan = PeripheralMan::GetInstance();

        if(!ptrPeripheralMan->CheckUsbStorageState())
        {
            HintArea::GetInstance()->UpdateHint(_("Error: No USB device Plug-in!"), 2);
            free(data);
        //  free(item.data);
            return ERROR;
        }
        if(!ptrPeripheralMan->MountUsbStorage())
        {
            HintArea::GetInstance()->UpdateHint(_("Error: Failed to mount USB device!"), 2);
            free(data);
        //  free(item.data);
            return ERROR;
        }

        VideoMan::GetInstance()->SaveVideo(0, (char*)data, UDISK_PATH, &item);

        ptrPeripheralMan->UmountUsbStorage();
    }
    else
        VideoMan::GetInstance()->SaveVideo(0, (char*)data, STORE_PATH, &item);

    if(data !=NULL)
        free(data);

    FreezeMode::GetInstance()->SetUnFreezeForbid(FALSE);
    return FALSE;
}

static int SaveCine(gpointer data)
{
    PRINTF("SaveCine file name: %s\n", (char*)data);
    if (FreezeMode::GetInstance()->GetUnFreezeForbidStatus())
        return ERROR;
    FreezeMode::GetInstance()->SetUnFreezeForbid(TRUE);

    HintArea::GetInstance()->UpdateHint(_("Saving..."), 0);

    g_timeout_add(100, DelayToSaveCine, data);

    return OK;
}

EKnobReturn ReplayChgSaveCine(void)
{
    //get current image para
//  VideoMan::VideoItem item;
    if (!Replay::GetInstance()->GetCineImg(&(item.deq), item.begin, item.frames, item.width, item.height, item.frame_rate))
    {
        HintArea::GetInstance()->UpdateHint(_("[Replay]: No image can be saved."), 1);
        return ERROR;
    }
//int size = item.deq->size();
//printf("size = %d item.begin = %d item.end = %d item.width = %d item.height = %d\n",size,item.begin,item.frames,item.width,item.height);
    SysOptions so;
    if(!so.GetCineAutoName())
    {
        if(!ModeStatus::IsFreezeMode())
            FreezeMode::GetInstance()->PressFreeze();
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::FILENAME,
                _("Please input the file name(without suffix)"),
                &SaveCine);
    }
    else
    {
        time_t at;
        at = time(&at);

        struct tm* ct;
        ct = localtime(&at);

        char *fileName = (char*)malloc(256);
    //  char fileName[15];
        memset(fileName, 0, 255);
        sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10);

        SaveCine(fileName);
    }

    return PRESS;
}
#if (defined(CT_460) || defined(CT_355))
EKnobReturn ReplayChgSaveCine460(EKnobOper oper)
{
    //get current image para
//  VideoMan::VideoItem item;
    if (!Replay::GetInstance()->GetCineImg(&(item.deq), item.begin, item.frames, item.width, item.height, item.frame_rate))
    {
        HintArea::GetInstance()->UpdateHint(_("[Replay]: No image can be saved."), 1);
        return ERROR;
    }
//int size = item.deq->size();
//printf("size = %d item.begin = %d item.end = %d item.width = %d item.height = %d\n",size,item.begin,item.frames,item.width,item.height);
    SysOptions so;
    if(!so.GetCineAutoName())
    {
        if(!ModeStatus::IsFreezeMode())
            FreezeMode::GetInstance()->PressFreeze();
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::FILENAME,
                _("Please input the file name(without suffix)"),
                &SaveCine);
    }
    else
    {
        time_t at;
        at = time(&at);

        struct tm* ct;
        ct = localtime(&at);

        char *fileName = (char*)malloc(256);
    //  char fileName[15];
        memset(fileName, 0, 255);
        sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10);

        SaveCine(fileName);
    }

    return PRESS;
}
#endif