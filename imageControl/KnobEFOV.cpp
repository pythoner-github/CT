#include "imageControl/KnobEFOV.h"
#include "display/gui_global.h"
#include "display/KnobMenu.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/Replay.h"
#include "sysMan/SysOptions.h"
#include "display/HintArea.h"
#include "display/ViewDialog.h"
#include "display/MenuArea.h"
#include "patient/VideoMan.h"
#include "ViewMain.h"
#include <stdlib.h>
#include "imageProc/KnobReplay.h"

extern const char * EFOVROISizeName[3];
#if defined (EMP_322) || defined(EMP_313)
KnobMenu::KnobItem KnobEFOVNone[6] = {
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVPrepareMenu[6] = {
    {N_("ROI Size"), "", MIN, KnobEFOVROISize, KnobEFOVPressChgROISize},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVViewMenu[6] = {
    {N_("Zoom"), "", MIN, KnobEFOVZoom, KnobEFOVPressChgZoom},
    {N_("Rotate"), "", MIN, KnobEFOVRotate, KnobEFOVPressChgRotate},
    {N_("Review"), N_("Press"), PRESS, NULL, KnobEFOVReviewPress},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVReviewMenu[6] = {
    {N_("Replay"), "", PRESS, NULL, KnobEFOVReplayPress},
    {N_("Save Cine"), N_("Press"), ERROR, NULL, KnobEFOVSavePress},
    // {N_("ReadCine"), N_("Press"), PRESS, NULL, KnobEFOVReadPress},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};
#elif defined(EMP_355)
KnobMenu::KnobItem KnobEFOVNone[5] = {
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVPrepareMenu[5] = {
    {N_("ROI Size"), "", MIN, KnobEFOVROISize, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVViewMenu[5] = {
    {N_("Zoom"), "", MIN, KnobEFOVZoom, NULL},
    {N_("Rotate"), "", MIN, KnobEFOVRotate, NULL},
    {N_("Review"), N_("Press"), PRESS,  KnobEFOVReviewPressForG30, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVReviewMenu[5] = {
    {N_("Replay"), "", PRESS, KnobEFOVReplayPressForG30, NULL},
    {N_("Save Cine"), "", PRESS, KnobEFOVSavePressForG30, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};
#else
KnobMenu::KnobItem KnobEFOVNone[5] = {
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVPrepareMenu[5] = {
    {N_("ROI Size"), "", MIN, KnobEFOVROISize, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVViewMenu[5] = {
    {N_("Zoom"), "", MIN, KnobEFOVZoom, NULL},
    {N_("Rotate"), "", MIN, KnobEFOVRotate, NULL},
    {N_("Review"), N_("Press"), PRESS, NULL, KnobEFOVReviewPress},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};

KnobMenu::KnobItem KnobEFOVReviewMenu[5] = {
    {N_("Replay"), "", PRESS, NULL, KnobEFOVReplayPress},
    {N_("Save Cine"), N_("Press"), ERROR, NULL, KnobEFOVSavePress},
    // {N_("ReadCine"), N_("Press"), PRESS, NULL, KnobEFOVReadPress},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL},
    {"", "", ERROR, NULL, NULL}
};
#endif
void KnobEFOVNoneCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobEFOVNone, sizeof(KnobEFOVNone)/sizeof(KnobMenu::KnobItem), KnobMenu::EFOV);
}

void KnobEFOVPrepareCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobEFOVPrepareMenu, sizeof(KnobEFOVPrepareMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::EFOV);

    ImgProc2D::SIZE size = ImgProc2D::GetInstance()->GetEFOVROISize();
    EKnobReturn ret = ImgProc2D::GetInstance()->GetKnobEFOVROISize(size);
    KnobMenu::GetInstance()->SetValue(EFOV_ROI, (char *)(EFOVROISizeName[size]), ret);
}

void KnobEFOVCaptureCreate()
{
    KnobEFOVNoneCreate();
}

void KnobEFOVViewCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobEFOVViewMenu, sizeof(KnobEFOVViewMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::EFOV);
}

void KnobEFOVReviewCreate()
{
    KnobMenu::GetInstance()->SetItem(KnobEFOVReviewMenu, sizeof(KnobEFOVReviewMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::EFOV);
}

void SyncKnobEFOV(EKnobEFOV type, const char* s, EKnobReturn status)
{
    sprintf(KnobEFOVViewMenu[type].value, "%s", s);
    KnobEFOVViewMenu[type].status = status;
    KnobMenu::GetInstance()->Update();
}

void SyncKnobEFOVReview(EKnobEFOVReview type, const char* s, EKnobReturn status)
{
    sprintf(KnobEFOVReviewMenu[type].value, "%s", s);
    KnobEFOVReviewMenu[type].status = status;
    KnobMenu::GetInstance()->Update();
}

EKnobReturn KnobEFOVPressChgZoom(void)
{
    if (MenuArea::GetInstance()->GetMenuType() != MenuArea::EFOV)
        MenuArea::GetInstance()->ShowEFOVView();
    EKnobReturn ret = ImgProc2D::GetInstance()->ChangeEFOVZoomScale(ROTATE);
    return ret;
}
EKnobReturn KnobEFOVZoom(EKnobOper oper)
{
    if (MenuArea::GetInstance()->GetMenuType() != MenuArea::EFOV)
        MenuArea::GetInstance()->ShowEFOVView();
    EKnobReturn ret = ImgProc2D::GetInstance()->ChangeEFOVZoomScale(oper);
    return ret;
}

EKnobReturn KnobEFOVPressChgRotate(void)
{
    if (MenuArea::GetInstance()->GetMenuType() != MenuArea::EFOV)
        MenuArea::GetInstance()->ShowEFOVView();
    EKnobReturn ret = ImgProc2D::GetInstance()->ChangeEFOVRotate(ROTATE);
    return ret;
}
EKnobReturn KnobEFOVRotate(EKnobOper oper)
{
    if (MenuArea::GetInstance()->GetMenuType() != MenuArea::EFOV)
        MenuArea::GetInstance()->ShowEFOVView();
    EKnobReturn ret = ImgProc2D::GetInstance()->ChangeEFOVRotate(oper);
    return ret;
}

EKnobReturn KnobEFOVReviewPressForG30(EKnobOper oper)
{
	Replay::GetInstance()->EnterEFOVReview();
	return PRESS;
}

EKnobReturn KnobEFOVReplayPressForG30(EKnobOper oper)
{
	Replay::GetInstance()->ChangeEFOVAutoReviewFlag();
	return PRESS;
}
EKnobReturn KnobEFOVReviewPress(void)
{
	Replay::GetInstance()->EnterEFOVReview();
	return PRESS;
}

EKnobReturn KnobEFOVReplayPress(void)
{
	Replay::GetInstance()->ChangeEFOVAutoReviewFlag();
	return PRESS;
}

static gboolean DelayToSaveCine(gpointer data)
{
	VideoMan::GetInstance()->SaveEFOVVideo(0, (char*)data, STORE_PATH);
	free(data);
	return FALSE;
}

static int SaveCine(gpointer data)
{
    ReplayChgSaveCine();
    return OK;
}

EKnobReturn KnobEFOVSavePressForG30(EKnobOper oper)
{
    if (Replay::GetInstance()->EFOVVideoOutOfRange())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::QUESTION,
                _("eView cine is oversized. Intercept the cine automatically?"),
                &SaveCine);
        return PRESS;
    } else {
        return ReplayChgSaveCine();
    }
}

EKnobReturn KnobEFOVSavePress(void)
{
    if (Replay::GetInstance()->EFOVVideoOutOfRange())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
                ViewDialog::QUESTION,
                _("eView cine is oversized. Intercept the cine automatically?"),
                &SaveCine);
        return PRESS;
    } else {
        return ReplayChgSaveCine();
    }
	// Replay::GetInstance()->EFOVAutoReviewEnd();

	// SysOptions so;
	// if(!so.GetCineAutoName()) {
	// 	ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
	// 			ViewDialog::FILENAME,
	// 			_("Please input the file name(without suffix)"),
	// 			&SaveCine);
	// } else {
	// 	time_t at;
	// 	at = time(&at);

	// 	struct tm* ct;
	// 	ct = localtime(&at);

	// 	char *fileName = (char*)malloc(256);
	// 	memset(fileName, 0, 255);
	// 	sprintf(fileName, "%d%d%d%d%d%d%d%d%d%d%d", (ct->tm_year+1900), (ct->tm_mon+1)/10, (ct->tm_mon+1)%10, (ct->tm_mday)/10, (ct->tm_mday)%10, (ct->tm_hour)/10, (ct->tm_hour)%10, (ct->tm_min)/10, (ct->tm_min)%10, (ct->tm_sec)/10, (ct->tm_sec)%10);
	// 	SaveCine(fileName);
	// }
	// return PRESS;
}

EKnobReturn KnobEFOVReadPress(void)
{
    //     Replay::GetInstance()->EFOVAutoReviewEnd();

    // const char *path = "/mnt/harddisk/store/0/EFOV.efov";
    // struct stat file_stat;

    // if (stat(path, &file_stat) == -1) {
    //     printf("Get file stat error!\n");
    //     return PRESS;
    // }

    // const off_t size = file_stat.st_size;
    // unsigned char *buf = (unsigned char *)malloc(size);
    // if (buf == NULL)
    //     return PRESS;

    // FILE *fs = fopen(path, "r");
    // if(fread(buf, sizeof(unsigned char), size, fs) != size) {
    //     printf("fread error!\n");
    //     fclose(fs);
    //     free(buf);
    //     return PRESS;
    // }
    // Replay::GetInstance()->SetEFOVCineImg(buf, size);
    // fclose(fs);
    // free(buf);
    return PRESS;
}

EKnobReturn KnobEFOVROISize(EKnobOper oper)
{
    EKnobReturn ret = ImgProc2D::GetInstance()->ChangeEFOVROISize(oper);
    return ret;
}
EKnobReturn KnobEFOVPressChgROISize(void)
{
    EKnobReturn ret = ImgProc2D::GetInstance()->ChangeEFOVROISize(ROTATE);
    return ret;
}