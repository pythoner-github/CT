#include "periDevice/DCMMan.h"
#include <string.h>
#include <libavutil/time.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <vector>
#include "imageControl/Update2D.h"
#include "probe/ExamItem.h"
#include "Def.h"
#include "display/MenuArea.h"
#include "sysMan/UpgradeMan.h"
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "sysMan/ViewSystem.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/KeyDef.h"
#include "periDevice/PeripheralMan.h"
#include "ViewMain.h"
#include "display/ViewDialog.h"
#include "periDevice/NetworkMan.h"
#include "display/KnobMenu.h"
#include "display/TopArea.h"
#include "sysMan/ScreenSaver.h"
#include "patient/PatientInfo.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/SysOptions.h"
#include "sysMan/SysNoteSetting.h"
#include "sysMan/SysDicomSetting.h"
#include "sysMan/SysMeasurementSetting.h"
#include "sysMan/SysCalculateSetting.h"
#include "sysMan/SysUserDefinedKey.h"
#include "patient/ImgMan.h"
#include "patient/VideoMan.h"
#include "calcPeople/MeaCalcFun.h"
#include "imageControl/ImgPw.h"
#include "imageControl/ImgCfm.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/ImgProcCfm.h"
#include "sysMan/UserDefineKey.h"
#include "keyboard/KeyValueOpr.h"
#include "periDevice/ViewPrinterAdd.h"
#include "probe/ProbeSelect.h"

#include "periDevice/Printer.h"
#include "measure/MeasureMan.h"
#include "sysMan/CalcSetting.h"
#include "sysMan/MeasureSetting.h"
#include "patient/FileMan.h"
#include "sysMan/UserSelect.h"
#include "patient/ViewUdiskDataSelect.h"
#include "sysMan/ConfigToUSB.h"
#include "sysMan/ConfigToHost.h"

#include "sysMan/DicomLocalSetting.h"
#include "sysMan/DicomServerSetting.h"
#include "sysMan/DicomServiceSetting.h"
#include "periDevice/DCMRegister.h"
#include "probe/ExamItem.h"
#include <EmpAuthorization.h>
#include "calcPeople/MenuCalcNew.h"
#include "measure/MenuMeasure.h"
#include "Init.h"
#include <errno.h>
#include "patient/ViewNewPat.h"
using std::vector;
using std::string;

extern bool g_authorizationOn;

int _system_(const char *cmd)
{
    char str_cmd[20];
    int status;
    pid_t pid;
    if(cmd == NULL)
        return (-1);

    pid = vfork();
    if(pid < 0)
    {
        status = -1;
        perror("vfork error");
    }
    else if(pid == 0)
    {
        if(execl("/bin/sh", "sh", "-c", cmd, (char*)0) == -1)
        {
            PRINTF("execl error-%s", strerror(errno));
            _exit(127);
        }
    }
    else
    {
        while(waitpid(pid, &status, 0) < 0)
        {
            if(errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }

    return status;
}

#ifdef VET
static string KB_LIST[NUM_KB]=
{
   // "Abdomen","Urology", "Cardiac","Obstetrics","Gynecology","Small Part","Vascular","Fetal Cardio","TCD","Orthopedic"
    "Abdomen","Urology", "Cardiac","Obstetrics","Tendon","Small Part","Vascular","Fetal Cardio","TCD","Orthopedic"
};
#else

static string KB_LIST[NUM_KB]=
{
    "Abdomen","Urology", "Cardiac","Obstetrics","Gynecology","Small Part","Vascular","Fetal Cardio","TCD","Orthopedic"
};
#endif
#ifdef VET
char user_configure[USERCONFIG_LEN] = "VetItemPara.ini";
#else
char user_configure[USERCONFIG_LEN] = "ItemPara.ini";
#endif
static common_printer common_printer_data[] = {
    {"HP",    "LaserJet Series"},
    {"HP",    "DeskJet Series"},
    {"HP",    "New DeskJet Series"},
    {"EPSON", "9-Pin Series"},
    {"EPSON", "24-Pin Series"},
    {"EPSON", "Stylus Color Series"},
    {"EPSON", "New Stylus Color Series"},
    {"EPSON", "Stylus Photo Series"},
    {"EPSON", "New Stylus Photo Series"},
};
#ifdef VET
#ifdef CT_355
static ExamPara ExamItemArray[] = {
        {"", N_("Dog"),  ExamItem::DOG},
        {"", N_("Cat"),  ExamItem::CAT},
        {"", N_("Bovine"),  ExamItem::BOVINE},
        {"", N_("Equine"),  ExamItem::EQUINE},
        {"", N_("Swine"),  ExamItem::SWINE},
        {"", N_("Sheep"),  ExamItem::SHEEP},
        };
#else
static ExamPara ExamItemArray[] = {
        {"", N_("User1"),  ExamItem::USER1},
        {"", N_("User2"),  ExamItem::USER2},
        {"", N_("User3"),  ExamItem::USER3},
        {"", N_("User4"),  ExamItem::USER4},
        {"", N_("User5"),  ExamItem::USER5},
        {"", N_("User6"),  ExamItem::USER6},
        {"", N_("User7"),  ExamItem::USER7},
        {"", N_("User8"),  ExamItem::USER8},
        {"", N_("User9"),  ExamItem::USER9},
        {"", N_("User10"), ExamItem::USER10}
        };
#endif
#else
const static ExamPara ExamItemArray[] = {
    {N_("Abdomen"), N_("Adult Abdomen"), ExamItem::ABDO_ADULT},
    {N_("Abdomen"), N_("Adult Liver"), ExamItem::ABDO_LIVER},
    {N_("Abdomen"), N_("Kid Abdomen"), ExamItem::ABDO_KID},

    {N_("Cardiac"), N_("Adult Cardio"), ExamItem::CAR_ADULT},
    {N_("Cardiac"), N_("Kid Cardio"), ExamItem::CAR_KID},

    {N_("Small Part"), N_("Mammary Glands"), ExamItem::GLANDS},
    {N_("Small Part"), N_("Thyroid"), ExamItem::THYROID},
    {N_("Small Part"), N_("Eye Ball"), ExamItem::EYE},
    //{N_("Small Part"), N_("Small Part"), ExamItem::SMALL_PART},
    {N_("Small Part"), N_("Testicle"), ExamItem::SMALL_PART},

    {N_("Gynecology"), N_("Gynecology"), ExamItem::GYN},

    {N_("Obstetrics"), N_("Early Pregnancy"), ExamItem::EARLY_PREG},
    {N_("Obstetrics"), N_("Middle-late Pregnancy"), ExamItem::LATER_PREG},
    {N_("Obstetrics"), N_("Fetal Cardio"), ExamItem::CAR_FETUS},

    {N_("Urology"), N_("Kidney Ureter"), ExamItem::KIDNEY},
    {N_("Urology"), N_("Bladder Prostate"), ExamItem::BLADDER},

    {N_("Vascular"), N_("Carotid"), ExamItem::CAROTID},
    {N_("Vascular"), N_("Jugular"), ExamItem::JUGULAR},
    {N_("Vascular"), N_("Periphery Artery"), ExamItem::PERI_ARTERY},
    {N_("Vascular"), N_("Periphery Vein"), ExamItem::PERI_VEIN},

    {N_("Orthopedic"), N_("Hip Joint"), ExamItem::HIP_JOINT},
    {N_("Orthopedic"), N_("Meniscus"), ExamItem::MENISCUS},
    {N_("Orthopedic"), N_("Joint Cavity"), ExamItem::JOINT_CAVITY},
    {N_("Orthopedic"), N_("Spine"), ExamItem::SPINE},
    {N_("Small Part"), N_("MSK"), ExamItem::MSK},
    {N_("TCD"), "TCD", ExamItem::TCD},

    {N_("User Defined"), N_("User defined 1"), ExamItem::USER1},
    {N_("User Defined"), N_("User defined 2"), ExamItem::USER2},
    {N_("User Defined"), N_("User defined 3"), ExamItem::USER3},
    {N_("User Defined"), N_("User defined 4"), ExamItem::USER4}
};
#endif
static string KB_ADDR[NUM_KB]=
{
    "ABD","UR", "CAR","OB","GYN","SP","VES", "EXTRA","TCD","ORT"
};

ViewSystem::ViewSystem(void)
{
    m_p1Selected = true;
    m_p1_func_index = 0;
    m_p2_func_index = 0;
    m_imageItemSensitive = false;
    m_itemIndex = -1;
    m_bmIndex = 0;
    m_bpwIndex = 0;
    m_page_num = 0;
    m_calc_page_num = 0;
    m_itemName = NULL;
    m_save_or_new_flag = 0;
    m_str_index = NULL;
    m_current_note_page = 0;
    m_frameRegisterInfo = NULL;
    m_powerOffFlag = false;
    m_vgaInterl=true;
}

ViewSystem* ViewSystem::m_ptrInstance = NULL;

ViewSystem::~ViewSystem()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewSystem* ViewSystem::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewSystem;

    return m_ptrInstance;
}

GtkWidget* ViewSystem::GetWindow(void)
{
    return m_window;
}

#if 1 //add by jhuang
#include "calcPeople/MeasureDef.h"
#include "calcPeople/ViewReport.h"
#ifdef VET
const char *sectionstr[][LANG_MAX] =
{//{ ENGLISH, CHINESE, RU, PL,ES, FR DE }
    {"Abdomen", "\u8179\u90e8", "\u0436\u0438\u0432\u043e\u0442\u0410\u043a", "Jama brzuszna", "Abdomen","Abdomen", "Abdomen",      },  //"Abdomen"
    {"Cardiac", "\u5fc3\u810f\u79d1", "\u041a\u0430\u0440\u0434\u0438\u0430\u043b\u044c\u043d\u044b\u0439", "Serce", "Cardiaque", "Cardiaco", "Kardial",                 },  //"Cardiac"
    {"Urology", "\u6ccc\u5c3f\u79d1", "\u0423\u0440\u043e\u043b\u043e\u0433\u0438\u044f", "Urologia", "Urologie", "Urologico","Urologie",   },  //"Urology"
    {"Obstetrics", "\u4ea7\u79d1", "\u0410\u043a\u0443\u0448\u0435\u0440\u0441\u0442\u0432\u043e", "Po\u0142o\u017cnictwo",  "Obstétrique",  "Obstetrico",  "Geburtshilfe"   },  //"Obstetrics"
   {"Tendon", "\u808c\u8171", "\u0441\u0443\u0445\u043e\u0436\u0438\u043b\u0438\u0435", "Tendon", "Tendon","Tendon",  "Sehne",                                                   },//"Tendon"
    {"Small Part", "\u5c0f\u5668\u5b98", "\u0414\u0435\u0442\u043e\u043b\u044c\u043d\u043e", "Ma\u0142e narz\u0105dy", "Petite partie", "Pequeñas partes", "Small-Parts",             },  //"Small Part"
    {"Fetal Cardio", "\u80ce\u513f\u5fc3\u810f", "\u041a\u0430\u0440\u0434\u0438\u043e. \u043f\u043b\u043e\u0434\u0430", "Kardio. P\u0142odu","Cardio f\u0153tal",  "Fetal Cardio","fötale Kardio"    },  //"Fetal Cardio"
    {"Vascular", "\u8840\u7ba1", "\u0421\u043e\u0441\u0443\u0434\u044b", "Naczynia","Vasculaire", "Vascular", "Vaskulär",                        },  //"Vascular"
#if 1 // "Others" take place for "TCD" and "Orthopedic"
    //{"TCD", "\u7ecf\u9885\u591a\u666e\u52d2", "TCD", "TCD", "TCD","TCD",  "TCD",                                                                   },  //"TCD"
   {"Others", "其他", "Другие", "Inne", "Autres",  "Otro", "Andere",                                                                   },  //"Others"

#else
    {"TCD", "\u7ecf\u9885\u591a\u666e\u52d2", "TCD", "TCD", "TCD","TCD",  "TCD",                                                                   },  //"TCD"
    {"Orthopedic", "\u77eb\u5f62\u5916\u79d1", "\u041e\u0440\u0442\u043e\u043f\u0435\u0434\u0438\u044f", "Ortopedia",  "Orthopédie",   "Ortopedico", "Orthopädisch",                }   //"Orthopedic"
#endif
};
#else

#ifdef CT_322
const char *sectionstr[][LANG_MAX] =
{//{ ENGLISH, CHINESE, RU, PL,SE, FR, DE }
    {"Abdomen", "\u6210\u4eba\u8179\u90e8", "\u0410\u0431\u0434\u043e\u043c.", "Jama brzuszna","Abdomen", "Abdomen", "Abdomen",      },  //"Abdomen"
    {"Cardiac", "\u5fc3\u810f\u79d1", "\u041a\u0430\u0440\u0434\u0438\u0430\u043b\u044c\u043d\u044b\u0439", "Serce","Cardiaque",  "Cardiaco", "Kardial",                 },  //"Cardiac"
    {"Urology", "\u6ccc\u5c3f\u79d1", "\u0423\u0440\u043e\u043b\u043e\u0433\u0438\u044f", "Urologia","Urologie", "Urologico", "Urologie",   },  //"Urology"
    {"Obstetrics", "\u4ea7\u79d1", "\u0410\u043a\u0443\u0448\u0435\u0440\u0441\u0442\u0432\u043e", "Po\u0142o\u017cnictwo", "Obstétrique", "Obstetrico",  "Geburtshilfe",     },  //"Obstetrics"
    {"Gynecology", "\u5987\u79d1", "\u0413\u0438\u043d\u0435\u043a\u043e\u043b\u043e\u0433\u0438\u044f", "Ginekologia","Gynécologie", "Gynecologico",  "Gynäkologie",        },  //"Gynecology"
    {"Small Part", "\u5c0f\u5668\u5b98", "\u0414\u0435\u0442\u043e\u043b\u044c\u043d\u043e", "Ma\u0142e narz\u0105dy", "Petite partie", "Pequeñas partes","Small-Parts",             },  //"Small Part"
    {"Fetal Cardio", "\u80ce\u513f\u5fc3\u810f", "\u041a\u0430\u0440\u0434\u0438\u043e. \u043f\u043b\u043e\u0434\u0430", "Kardio. P\u0142odu","Cardio f\u0153tal", "Cardio Fetal", "fötale Kardio"    },  //"Fetal Cardio"
    {"Others", "其他", "Другие", "Inne", "Autres",  "Otro","Andere",                                                                   },  //"Others"

};
#else
const char *sectionstr[][LANG_MAX] =
{//{ ENGLISH, CHINESE, RU, PL,ES, FR DE }
    {"Abdomen", "\u6210\u4eba\u8179\u90e8", "\u0410\u0431\u0434\u043e\u043c.", "Jama brzuszna", "Abdomen","Abdomen", "Abdomen",      },  //"Abdomen"
    {"Cardiac", "\u5fc3\u810f\u79d1", "\u041a\u0430\u0440\u0434\u0438\u0430\u043b\u044c\u043d\u044b\u0439", "Serce","Cardiaque", "Cardiaco",  "Kardial",                 },  //"Cardiac"
    {"Urology", "\u6ccc\u5c3f\u79d1", "\u0423\u0440\u043e\u043b\u043e\u0433\u0438\u044f", "Urologia", "Urologie", "Urologico", "Urologie",   },  //"Urology"
    {"Obstetrics", "\u4ea7\u79d1", "\u0410\u043a\u0443\u0448\u0435\u0440\u0441\u0442\u0432\u043e", "Po\u0142o\u017cnictwo", "Obstétrique","Obstetrico",  "Geburtshilfe",     },  //"Obstetrics"
    {"Gynecology", "\u5987\u79d1", "\u0413\u0438\u043d\u0435\u043a\u043e\u043b\u043e\u0433\u0438\u044f", "Ginekologia", "Gynécologie", "Gynecologico",  "Gynäkologie",        },  //"Gynecology"
    {"Small Part", "\u5c0f\u5668\u5b98", "\u0414\u0435\u0442\u043e\u043b\u044c\u043d\u043e", "Ma\u0142e narz\u0105dy", "Petite partie", "Pequeñas partes",  "Small-Parts",             },  //"Small Part"
  {"Fetal Cardio", "\u80ce\u513f\u5fc3\u810f", "\u041a\u0430\u0440\u0434\u0438\u043e. \u043f\u043b\u043e\u0434\u0430", "Kardio. P\u0142odu", "Cardio f\u0153tal","Cardio Fetal", "fötale Kardio"    },  //"Fetal Cardio"
    {"Vascular", "\u8840\u7ba1", "\u0421\u043e\u0441\u0443\u0434\u044b", "Naczynia","Vasculaire","Vascular", "Vaskulär",                        },  //"Vascular"
 #if 1 // "Others" take place for "TCD" and "Orthopedic"
    //{"TCD", "\u7ecf\u9885\u591a\u666e\u52d2", "TCD", "TCD", "TCD",  "TCD",                                                                   },  //"TCD"

    {"Others", "其他", "Другие", "Inne", "Autres","Otro",  "Andere",                                                                   },  //"Others"
#else
    {"TCD", "\u7ecf\u9885\u591a\u666e\u52d2", "TCD", "TCD","TCD", "TCD",  "TCD",                                                                   },  //"TCD"
    {"Orthopedic", "\u77eb\u5f62\u5916\u79d1", "\u041e\u0440\u0442\u043e\u043f\u0435\u0434\u0438\u044f", "Ortopedia", "Orthopédie", "Ortopedico", "Orthopädisch",                }   //"Orthopedic"
#endif

};
#endif
#endif
const char *defaultstr = "System Default";
const char *defaultflag ="\u221a"; //62; //251; // \u221a;//
const char *templet1str[LANG_MAX] = ////{ ENGLISH, CHINESE, RU, PL, FR,es, DE}
    {"Template 1",  "\u6a21\u677f1", "\u0428\u0430\u0431\u043b\u043e\u043d 1", "szablon 1", "Modššle 1",  "Templete 1",  "Template 1",     };//"templet"
const char *Customstr[LANG_MAX] = ////{ ENGLISH, CHINESE, RU, PL, ,FR,es, DE}
    {"Custom Template", "\u81ea\u5B9a\u4e49\u6a21\u677f", "\u041f\u043e\u043b\u044c\u0437\u043e\u0432\u0430\u0442\u0435\u043b\u044c\u0441\u043a\u0438\u0435 \u0448\u0430\u0431\u043b\u043e\u043d\u044b","Custom Template","Modššle auto-dšŠfini", "Custom Templete","Custom Template",      };//"Custom Template"

char ShowArr[MAX_CALCARR_COUNT+1] = {0x66};
GtkWidget* CheckButtonArr[MAX_CALCARR_COUNT] = {NULL};
sqlite3 *CustomReport_db = NULL;

#define CustomReport_PATH       "./res/CustomReport.db"
#define CustomReport_PATH_BAK   "./res/bak_CustomReport.db"
#define DEFALT_SCREEN_WIDTH     (950) //(SCREEN_WIDTH)
#define GRAY_WIDTH              (20)
#define TREE_WINDOW_WIDTH       (260)
#define SHOW_WINDOW_WIDTH       ((DEFALT_SCREEN_WIDTH -40) - TREE_WINDOW_WIDTH - 3*GRAY_WIDTH)
#define DEF_FLAG_LEN            (strlen(defaultflag))
#define CHECK_VALUE             (0x74)
#define UNCHECK_VALUE           (0x66)
#define MAX_SECTION             (sizeof(sectionstr)/sizeof(sectionstr[0]))

#ifdef CT_322
#define OTHERS_M                (ORTHO_M)
#else
#define OTHERS_M                (TCD_M)
#endif
bool ViewSystem::StrCmpSectionString(int section, const char *string, int *language)
{
    int maxsection = MAX_SECTION;
    PRINTF("maxSection==%d\n",maxsection);
    if(section < maxsection && section >= 0)
    {
        for(int lan = 0; lan < LANG_MAX; lan++)
        {
                if(strcmp(sectionstr[section][lan], string) == 0)
                {

                    if(language != NULL)
                        *language = lan;
                    return true;
                }
        }
    }
    return false;
}

bool ViewSystem::StrCmpCustomString(const char *string, int *language)
{
    for(int lan = 0; lan < LANG_MAX; lan++)
    {
        if(strcmp(Customstr[lan], string) == 0)
        {
            if(language != NULL)
                *language = lan;
            return true;
        }
    }
    return false;
}

bool ViewSystem::StrCmpTemplet1String(const char *string, int *language)
{
    for(int lan = 0; lan < LANG_MAX; lan++)
    {
        if(strcmp(templet1str[lan], string) == 0)
        {
            if(language != NULL)
                *language = lan;
            return true;
        }
    }
    return false;
}

bool ViewSystem::OpenDB(void)
{
    if(sqlite3_open(CustomReport_PATH, &CustomReport_db) != SQLITE_OK)return false;
    return true;
}

bool ViewSystem::CloseDB(void)
{
    if (sqlite3_close(CustomReport_db) != SQLITE_OK)
    {
        PRINTF("Database Close Error:%s\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }
    CustomReport_db = 0;
    return true;
}

bool ViewSystem::InitDB(void)
{
    if(access(CustomReport_PATH, F_OK))
    {
        sqlite3 *db = 0;
        if (sqlite3_open(CustomReport_PATH, &db) != SQLITE_OK)
        {
            PRINTF("Open CustomReport  Database Error!\n");
            return false;
        }
        char *errmsg = NULL, buffer[1024*10], buf[1024];
        memset(buffer, 0, sizeof(buffer));
        //create table
        strcpy(buffer, "BEGIN TRANSACTION;\nCREATE TABLE CustomReport(sections, templet, childsection, record);\nCREATE TABLE test_cp(sections, templet, childsection, record);\n");
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", defaultstr, defaultstr, defaultstr, templet1str[init_language]);
        strcat(buffer, buf);

        int maxsection = MAX_SECTION;
        for(int k = 0; k < maxsection; k++)
        {
            int start, end, count;
            char record[1024];
            memset(record, CHECK_VALUE, sizeof(record));
            memset(buf, 0, sizeof(buf));

            InitRecordFromShowArr(sectionstr[k][init_language], &start, &end);
            count = end - start;
            if(StrCmpSectionString(OB_M, sectionstr[k][init_language], NULL))
            {
                //Fetal 2
                count += OB_CALC_COUNT;
                //EFW
                count += EFW_COUNT;
                //average
                count += AverRes_COUNT;
            }
            if(StrCmpSectionString(UR_M, sectionstr[k][init_language], NULL))
            {
                //PSAD
                count += PSAD_COUNT;
            }
            if(StrCmpSectionString(OTHERS_M, sectionstr[k][init_language], NULL))
            {
                //Ohters
                count += OTHERS_COUNT;
            }
            record[count] = '\0';
            sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", (char *)Customstr[init_language], (char *)templet1str[init_language], (char *)sectionstr[k][init_language], record);
            strcat(buffer, buf);
        }

        strcat(buffer, "COMMIT;\n");
        if (sqlite3_exec(db, buffer, 0, 0, &errmsg) != SQLITE_OK)
        {
            PRINTF("Init CustomReport db  Error: %s\n", sqlite3_errmsg(db));
            sqlite3_free(errmsg);
            return false;
        }
        sqlite3_close(db);
    }
    return true;
}

bool ViewSystem::LocalizationDB(void)
{
    char *errmsg = NULL;
    sqlite3_stmt *stmt_f1=NULL;
    const char *sections = NULL, *templet = NULL, *childsection = NULL;
    string sql_f1 = "SELECT DISTINCT sections FROM CustomReport";

    char buffer[1024*10];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "BEGIN TRANSACTION;\nCREATE TABLE CustomReport(sections, templet, childsection, record);\nCREATE TABLE test_cp(sections, templet, childsection, record);\n");

    if(access(CustomReport_PATH, F_OK)) return true;
    OpenDB();

    if (sqlite3_prepare(CustomReport_db, sql_f1.c_str(), sql_f1.size(), &stmt_f1, 0) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
        PRINTF("LocalizationDB 1:%s\n", sqlite3_errmsg(CustomReport_db));
        CloseDB();
        return false;
    }

    while (sqlite3_step(stmt_f1) != SQLITE_DONE)
    {
        int language;
        const char *f1 = (const char *)sqlite3_column_text(stmt_f1, 0);
        if (strcmp(f1, "") == 0) continue;

        if(StrCmpCustomString(f1, &language))
        {
            if(language > LANG_MAX)
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
                sqlite3_finalize(stmt_f1);
                CloseDB();
                return false;
            }
#if 0
            if(language == init_language)
            {
                sqlite3_finalize(stmt_f1);
                CloseDB();
                return true;
            }
#endif
            sections = Customstr[init_language];
        }
        else
        {
            sections = f1;
        }

        sqlite3_stmt *stmt_f2 = NULL;
        string sql_f2 = "SELECT DISTINCT templet FROM CustomReport WHERE sections = ?";
        if (sqlite3_prepare(CustomReport_db, sql_f2.c_str(), sql_f2.size(), &stmt_f2, 0) != SQLITE_OK)
        {
            CloseDB();
            return false;
        }
        if (sqlite3_bind_text(stmt_f2, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK)
        {
            sqlite3_finalize(stmt_f2);
            CloseDB();
            return false;
        }

        while (sqlite3_step(stmt_f2) != SQLITE_DONE)
        {
            const char *f2 = (const char *)sqlite3_column_text(stmt_f2, 0);
            if (strcmp(f2, "") == 0)continue;

            if(StrCmpTemplet1String(f2, &language))
            {
                if(language > LANG_MAX)
                {
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
                    sqlite3_finalize(stmt_f2);
                    sqlite3_finalize(stmt_f1);
                    CloseDB();
                    return false;
                }
#if 0
                if(language == init_language)
                {
                    sqlite3_finalize(stmt_f2);
                    sqlite3_finalize(stmt_f1);
                    CloseDB();
                    return true;
                }
#endif
                templet = templet1str[init_language];
            }
            else
            {
                templet = f2;
            }

            sqlite3_stmt *stmt_f3 = NULL;
            string sql_f3 = "SELECT DISTINCT childsection FROM CustomReport WHERE sections = ? AND templet = ?";
            if(sqlite3_prepare(CustomReport_db, sql_f3.c_str(), sql_f3.size(), &stmt_f3, 0) != SQLITE_OK)
            {
                CloseDB();
                return false;
            }
            if(sqlite3_bind_text(stmt_f3, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK)
            {
                PRINTF("LocalizationDB 5:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                CloseDB();
                return false;
            }
            if(sqlite3_bind_text(stmt_f3, 2, f2, strlen(f2), SQLITE_TRANSIENT) != SQLITE_OK)
            {
                PRINTF("LocalizationDB 6:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                CloseDB();
                return false;
            }
            while(sqlite3_step(stmt_f3) != SQLITE_DONE)
            {
                const char *f3 = (const char *)sqlite3_column_text(stmt_f3, 0);
                if (strcmp(f3, "") == 0)continue;

                int section, maxsection = MAX_SECTION;
                for(section = 0; section < maxsection; section++)
                {
                    if(StrCmpSectionString(section, f3, &language))
                    {
                        if(language > LANG_MAX)
                        {
                            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
                            sqlite3_finalize(stmt_f3);
                            sqlite3_finalize(stmt_f2);
                            sqlite3_finalize(stmt_f1);
                            CloseDB();
                            return false;
                        }
#if 0
                        if(language == init_language)
                        {
                            sqlite3_finalize(stmt_f3);
                            sqlite3_finalize(stmt_f2);
                            sqlite3_finalize(stmt_f1);
                            CloseDB();
                            return true;
                        }
#endif
                        childsection = sectionstr[section][init_language];
                        break;
                    }
                }

                if(!strcmp(f3, defaultstr)) childsection = defaultstr;

                string record;
                ostringstream stream;
                stream << "SELECT record FROM CustomReport WHERE sections = '" << f1 << "' AND templet = '" << f2 << "' AND childsection = '" << f3 << "'";
                sqlite3_stmt *stmt = NULL;
                string sql = stream.str();

                if (sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK)
                {
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
                    PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
                    CloseDB();
                    return false;
                }

                while (sqlite3_step(stmt) != SQLITE_DONE)
                {
                    if (strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
                        record.assign((const char *)sqlite3_column_text(stmt, 0));
                }
                sqlite3_finalize(stmt);

                //write record
                char buf[1024];
                memset(buf, 0, sizeof(buf));

                if(StrCmpTemplet1String(record.c_str(), NULL))
                {
                    sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", sections, templet, childsection, templet1str[init_language]);
                }
                else
                {
                    sprintf(buf, "INSERT INTO \"CustomReport\" VALUES('%s','%s','%s','%s');\n", sections, templet, childsection, record.c_str());
                }
                strcat(buffer, buf);
            }
            sqlite3_finalize(stmt_f3);
        }
        sqlite3_finalize(stmt_f2);
    }
    sqlite3_finalize(stmt_f1);

    CloseDB();
    remove(CustomReport_PATH_BAK);

    sqlite3 *CustomReport_db_bak;
    if (sqlite3_open(CustomReport_PATH_BAK, &CustomReport_db_bak) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
        PRINTF("LocalizationDB 7:%s\n", sqlite3_errmsg(CustomReport_db_bak));
        return false;
    }

    strcat(buffer, "COMMIT;\n");
    if (sqlite3_exec(CustomReport_db_bak, buffer, 0, 0, &errmsg) != SQLITE_OK)  // costed 100ms, slower than sqlite3.6
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
        PRINTF("Init CustomReport_db_bak Error: %s\n", sqlite3_errmsg(CustomReport_db_bak));
        sqlite3_free(errmsg);
        return false;
    }

    sqlite3_close(CustomReport_db_bak);
    if(rename(CustomReport_PATH_BAK, CustomReport_PATH) != 0)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL);
        PRINTF("rename(%s, %s) error\n", CustomReport_PATH_BAK, CustomReport_PATH);
        return false;
    }
    return true;
}

GtkTreeModel *ViewSystem:: CreateTreeModel(void)
{
    InitDB();
    LocalizationDB();
    OpenDB();

    int count = 0;
    string recordname;
    ostringstream stream;
    stream << "SELECT record FROM CustomReport WHERE sections = '" << defaultstr << "' AND templet = '" << defaultstr << "' AND childsection = '" << defaultstr << "'";
    sqlite3_stmt *stmt = NULL;
    string sql = stream.str();

    if(sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        if(strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
            recordname.assign((const char *)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);

    if(recordname.length() > 0)
    {
        ostringstream stream2;
        stream2 << "SELECT DISTINCT childsection FROM CustomReport WHERE sections = '" << Customstr[init_language] <<"' AND templet = '" << recordname.c_str() << "'";
        sqlite3_stmt *stmt2 = NULL;
        string sql2 = stream2.str();

        if(sqlite3_prepare(CustomReport_db, sql2.c_str(), sql2.size(), &stmt2, 0) != SQLITE_OK)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
            return false;
        }
        while (sqlite3_step(stmt2) != SQLITE_DONE)
        {
            const char *pp = (const char *)sqlite3_column_text(stmt2, 0);
            if (strcmp(pp, "") == 0) continue;
            count ++;
        }
        sqlite3_finalize(stmt2);
    }

    char templatestr[256];
    if(0 == count)
    {
        sprintf(templatestr, "%s", templet1str[init_language]);
        ostringstream stream3;
        stream3 << "UPDATE CustomReport SET record = '" << templatestr << "' WHERE sections = '" << defaultstr << "' AND templet = '" << defaultstr << "' AND childsection = '" << defaultstr << "'";
        string sql3 = stream3.str();
        if (sqlite3_exec(CustomReport_db, sql3.c_str(), 0, 0, NULL) != SQLITE_OK)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL);
            return false;
        }
    }
    else
    {
        sprintf(templatestr, "%s", recordname.c_str());
    }

    GtkTreeIter first_iter;
    GtkTreeIter second_iter;
    GtkTreeStore *store = gtk_tree_store_new(1, G_TYPE_STRING);

    sqlite3_stmt *stmt_f1=NULL;
    string sql_f1 = "SELECT DISTINCT sections FROM CustomReport";
    if (sqlite3_prepare(CustomReport_db, sql_f1.c_str(), sql_f1.size(), &stmt_f1, 0) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
        PRINTF("CreateTreeModel 1:%s\n", sqlite3_errmsg(CustomReport_db));
        return NULL;
    }

    while(sqlite3_step(stmt_f1) != SQLITE_DONE)
    {
        const char *f1 = (const char *)sqlite3_column_text(stmt_f1, 0);
        if (strcmp(f1, "") == 0) continue;
        if(!strcmp(f1, defaultstr))continue;//delet default system
        gtk_tree_store_append(store, &topIter, NULL);
        gtk_tree_store_set(store, &topIter, 0, f1, -1);

        sqlite3_stmt *stmt_f2 = NULL;
        string sql_f2 = "SELECT DISTINCT templet FROM CustomReport WHERE sections = ?";
        if (sqlite3_prepare(CustomReport_db, sql_f2.c_str(), sql_f2.size(), &stmt_f2, 0) != SQLITE_OK)
        {
            PRINTF("CreateTreeModel 2:%s\n", sqlite3_errmsg(CustomReport_db));
            return NULL;
        }
        if (sqlite3_bind_text(stmt_f2, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK)
        {
            PRINTF("CreateTreeModel 3:%s\n", sqlite3_errmsg(CustomReport_db));
            sqlite3_finalize(stmt_f2);
            return NULL;
        }

        while (sqlite3_step(stmt_f2) != SQLITE_DONE)
        {
            const char *f2 = (const char *)sqlite3_column_text(stmt_f2, 0);
            if (strcmp(f2, "") == 0)continue;
            gtk_tree_store_append(store, &first_iter, &topIter);

            if(!strcmp(f2, templatestr))
            {
                char buffer[256];
                sprintf(buffer, "%s", defaultflag);
                strcat(buffer, " ");
                strcat(buffer, f2);
                gtk_tree_store_set(store, &first_iter, 0, buffer, -1);
            }
            else
            {
                gtk_tree_store_set(store, &first_iter, 0, f2, -1);
            }

            sqlite3_stmt *stmt_f3 = NULL;
            string sql_f3 = "SELECT DISTINCT childsection FROM CustomReport WHERE sections = ? AND templet = ?";
            if (sqlite3_prepare(CustomReport_db, sql_f3.c_str(), sql_f3.size(), &stmt_f3, 0) != SQLITE_OK)
            {
                PRINTF("CreateTreeModel 4:%s\n", sqlite3_errmsg(CustomReport_db));
                return NULL;
            }
            if (sqlite3_bind_text(stmt_f3, 1, f1, strlen(f1), SQLITE_TRANSIENT) != SQLITE_OK)
            {
                PRINTF("CreateTreeModel 5:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                return NULL;
            }
            if (sqlite3_bind_text(stmt_f3, 2, f2, strlen(f2), SQLITE_TRANSIENT) != SQLITE_OK)
            {
                PRINTF("CreateTreeModel 6:%s\n", sqlite3_errmsg(CustomReport_db));
                sqlite3_finalize(stmt_f3);
                return NULL;
            }
            while (sqlite3_step(stmt_f3) != SQLITE_DONE)
            {
                const char *f3 = (const char *)sqlite3_column_text(stmt_f3, 0);
                if (strcmp(f3, "") == 0)continue;
                gtk_tree_store_append(store, &second_iter, &first_iter);
                gtk_tree_store_set(store, &second_iter, 0, f3, -1);
            }
            sqlite3_finalize(stmt_f3);
        }
        sqlite3_finalize(stmt_f2);
    }
    sqlite3_finalize(stmt_f1);
    CloseDB();

    if(0 == count)
    {
        //ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Blank template cannot apply,switch to template 1!"), NULL);
    }
    return GTK_TREE_MODEL(store);
}

void ViewSystem::TreeSelectionChanged(GtkTreeSelection *selection)
{
    GtkWidget *window = NULL;
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)return;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    int tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    char *sections = NULL, *templet = NULL, *childsection = NULL, *temp = NULL;
    ostringstream stream;

    if(tree_depth == 1)
    {
        gtk_widget_show(button_add);
        gtk_widget_hide(button_save);
        gtk_widget_hide(button_delete);
        gtk_widget_hide(label_childsection);
        gtk_widget_hide(m_combobox_childsection);
        gtk_widget_hide(label_templet);
        gtk_widget_hide(m_entry_templet);
        gtk_widget_hide(button_ok);
        gtk_widget_hide(button_cancel);
        if(show_window)gtk_widget_hide_all (show_window);
        gtk_widget_set_sensitive(m_button_apply, FALSE);
    }
    else if(tree_depth == 2)
    {
        gtk_tree_model_get(model, &iter, 0, &temp, -1);

        if(NULL != strstr(temp, defaultflag) || StrCmpTemplet1String(temp, NULL))
        {
            gtk_widget_hide(button_delete);
        }
        else
        {
            gtk_widget_show(button_delete);
        }

        gtk_widget_show(button_add);
        gtk_widget_hide(button_save);
        gtk_widget_hide(label_childsection);
        gtk_widget_hide(m_combobox_childsection);
        gtk_widget_hide(label_templet);
        gtk_widget_hide(m_entry_templet);
        gtk_widget_hide(button_ok);
        gtk_widget_hide(button_cancel);
        if(show_window)gtk_widget_hide_all (show_window);
        gtk_widget_set_sensitive(m_button_apply, TRUE);
    }
    else if (tree_depth == 3)
    {
        GtkTreeIter iter_parent0;
        GtkTreeIter iter_parent1;
        gtk_tree_model_get(model, &iter, 0, &childsection, -1);
        gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
        gtk_tree_model_get(model, &iter_parent0, 0, &temp, -1);
        gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
        gtk_tree_model_get(model, &iter_parent1, 0, &sections, -1);

        if(NULL != strstr(temp, defaultflag))
        {
            templet = &temp[DEF_FLAG_LEN + 1];
        }
        else
        {
            templet = temp;
        }

        ReadRecordFromDB(sections, templet, childsection);
        if(show_window)gtk_widget_hide_all(show_window);

        if(StrCmpSectionString(ABDO_M, childsection, NULL))
        {
            window = CreatWindowABD();
        }
        if(StrCmpSectionString(ADULT_M, childsection, NULL))
        {
            window =  CreatWindowAE();
        }
        if(StrCmpSectionString(UR_M, childsection, NULL))
        {
            window =  CreatWindowUR();
        }
#ifdef VET
        if(StrCmpSectionString(ANOB_M, childsection, NULL))
        {
            window =  CreatWindowAnOB();
        }
#else
        if(StrCmpSectionString(OB_M, childsection, NULL))
        {
            window =  CreatWindowOB();
        }
#endif
#ifdef VET
#else
        if(StrCmpSectionString(GYN_M, childsection, NULL))
        {
            window =  CreatWindowGYN();
        }
#endif
        if(StrCmpSectionString(SP_M, childsection, NULL))
        {
            window =  CreatWindowSP();
        }
#if(defined(CT_313) || defined(CT_322))
        if(StrCmpSectionString(FETAL_M, childsection, NULL))
        {
            window =  CreatWindowFE();
        }
#else//TCD// VES
        if(StrCmpSectionString(VS_M, childsection, NULL))
        {
            window =  CreatWindowFE();
        }
        if(StrCmpSectionString(FETAL_M, childsection, NULL))
        {
            window =  CreatWindowVES();
        }
#endif
        if(StrCmpSectionString(OTHERS_M, childsection, NULL))
        {

            window =  CreatWindowOTHERS();
        }
#ifdef VET
        if(StrCmpSectionString(TD_M, childsection, NULL))
        {
            window =  CreatWindowTD();
        }
#endif
        if(window)
        {
            show_window = window;
            gtk_widget_show_all (show_window);
        }

        int start, end;
        if(InitRecordFromShowArr(childsection, &start, &end))
        {
            for(int id = start; id < end; id++)
            {
                if(CheckButtonArr[id])
                {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
                }
            }
        }
#ifdef VET
#else
        if(StrCmpSectionString(OB_M, childsection, NULL))
        {
            //Fetal 2
            for(int id = MAX_CALC_COUNT; id < MAX_CALC_COUNT+OB_CALC_COUNT; id++)
            {
                if(CheckButtonArr[id])
                {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
                }
            }
            //EFW
            for(int id = EFW_START_ID; id < EFW_START_ID + EFW_COUNT; id++)
            {
                if(CheckButtonArr[id])
                {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
                }
            }

            //Average
            for(int id = AVERRES_START_ID; id < AVERRES_START_ID + AverRes_COUNT; id++)
            {
                if(CheckButtonArr[id])
                {
                        if(id == AVERRES_START_ID + 2 || id == AVERRES_START_ID + 3)
                            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id+2]));
                        else
                            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
                }
            }
        }
#endif
        //PSAD
        if(StrCmpSectionString(UR_M, childsection, NULL))
        {
            for(int id = PSAD_START_ID; id < PSAD_START_ID + PSAD_COUNT; id++)
            {
                if(CheckButtonArr[id])
                {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
                }
            }
        }

         //Others
        if(StrCmpSectionString(OTHERS_M, childsection, NULL))
        {
            for(int id = OTHERS_START_ID; id < OTHERS_END_ID; id++)
            {
                if(CheckButtonArr[id])
                {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(CheckButtonArr[id]), (CHECK_VALUE == ShowArr[id]));
                }
            }
        }

        if(StrCmpTemplet1String(temp, NULL))
        {
            gtk_widget_hide(button_delete);
        }
        else
        {
            gtk_widget_show(button_delete);
        }

        gtk_widget_hide(button_add);
        gtk_widget_show(button_save);
        gtk_widget_hide(label_childsection);
        gtk_widget_hide(m_combobox_childsection);
        gtk_widget_hide(label_templet);
        gtk_widget_hide(m_entry_templet);
        gtk_widget_hide(button_ok);
        gtk_widget_hide(button_cancel);
        gtk_widget_set_sensitive(m_button_apply, FALSE);
    }
}

void ViewSystem::ChkBtnClicked(GtkButton *button)
{
    for(int id = 0; id < MAX_CALCARR_COUNT; id++)
    {
        if((GtkWidget*)button == CheckButtonArr[id])
        {
            //average
            if(id == AVERRES_START_ID + 2  || id == AVERRES_START_ID + 3)
            {
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
                {
                    ShowArr[id + 2] = CHECK_VALUE;
                }
                else
                {
                    ShowArr[id + 2] = UNCHECK_VALUE;
                }
            }
            else
            {
                if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
                {
                    ShowArr[id] = CHECK_VALUE;
                }
                else
                {
                    ShowArr[id] = UNCHECK_VALUE;
                }

                //average
                if(id == AVERRES_START_ID || id == AVERRES_START_ID + 1)
                {
                    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
                    {
                        ShowArr[id + 2] = CHECK_VALUE;
                    }
                    else
                    {
                        ShowArr[id + 2] = UNCHECK_VALUE;
                    }
                }
            }
        }
    }
}

GtkWidget* ViewSystem::AddFrameByTitle(char *title, int section, int startid, int endid, int *h)
{
    GtkWidget* checkbutton;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fixed;
    frame = gtk_frame_new (NULL);
    label = gtk_label_new (title);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    int x = 0, y = 0, tab_x = 10, tab_y = 10, count = endid - startid;
    int wide = (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH- 4*tab_x)/3, high = 30;

    for(int t = 0; t < count; t++)
    {
        if(!(t%3))
        {
            x = tab_x;
            if(t /3)
            {
                y += high;
            }
            else
            {
                y += tab_y;
            }
        }
        else
        {
            x += wide + tab_x;
        }

        int id = startid + t;

        switch (section)
        {
            case ABDO_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(AbdoInfo[id- ABD_MEA_START].title));
                break;
            case ADULT_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(AdultInfo[id - ADULT_MEA_START].title));
                break;
            case UR_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(URInfo[id - UR_MEA_START].title));
                break;
#ifdef VET
            case ANOB_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(AnOBInfo[id - ANOB_MEA_START].title));
                break;
#endif
            case OB_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(OBInfo[id - OB_MEA_START].title));
                break;
            case GYN_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(GYNInfo[id - GYN_MEA_START].title));
                break;
            case SP_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(SPInfo[id - SP_MEA_START].title));
                break;
            case VS_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(VSInfo[id - VS_MEA_START].title));
                break;
            case FETAL_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(FetalInfo[id - FETAL_MEA_START].title));
                break;
            case TCD_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(TCDInfo[id - TCD_MEA_START].title));
                break;
            case ORTHO_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(OrthoInfo[id - ORTHO_MEA_START].title));
                break;
#ifdef VET
            case TD_M:
                checkbutton = gtk_check_button_new_with_mnemonic (_(TDInfo[id - TD_MEA_START].title));
                break;
#endif
            default:
                break;
        }

        gtk_widget_show (checkbutton);
        gtk_fixed_put (GTK_FIXED (fixed), checkbutton, x, y);
        gtk_widget_set_size_request (checkbutton, wide, high);
        g_signal_connect(G_OBJECT(checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
        if(id < MAX_CALCARR_COUNT)CheckButtonArr[id] = checkbutton;

    }

    *h = y + high + tab_y*2;

    gtk_widget_show (frame);
    gtk_widget_set_size_request (frame, (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH), *h);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    return frame;
}

GtkWidget* ViewSystem::AddFrameByTitle2(char *title, int section, int startid, int endid, int *h)
{
    GtkWidget* checkbutton;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fixed;

    frame = gtk_frame_new (NULL);
    label = gtk_label_new (title);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    int x = 0, y = 0, tab_x = 10, tab_y = 10, count = endid - startid;
    int wide = (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH- 4*tab_x)/3, high = 30;

    for(int t = 0; t < count; t++)
    {
        if(!(t%3))
        {
            x = tab_x;
            if(t /3)
            {
                y += high;
            }
            else
            {
                y += tab_y;
            }
        }
        else
        {
            x += wide + tab_x;
        }
        int id = startid + t;
        checkbutton = gtk_check_button_new_with_mnemonic (_(OBInfo[id - OB_MEA_START].title));

        gtk_widget_show (checkbutton);
        gtk_fixed_put (GTK_FIXED (fixed), checkbutton, x, y);
        gtk_widget_set_size_request (checkbutton, wide, high);
        g_signal_connect(G_OBJECT(checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
        id = (id - OB_MEA_START) + MAX_CALC_COUNT; //fetal 2, the id  should add to the array end.
        if(id < MAX_CALCARR_COUNT)CheckButtonArr[id] = checkbutton;
    }

    *h = y + high + tab_y*2;

    gtk_widget_show (frame);
    gtk_widget_set_size_request (frame, (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH), *h);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    return frame;
}

GtkWidget* ViewSystem::AddFrameForOthers(const char *title, int *h)
{
    GtkWidget* checkbutton;
    GtkWidget* label;
    GtkWidget* frame;
    GtkWidget* fixed;
    frame = gtk_frame_new (NULL);
    label = gtk_label_new (title);
    gtk_widget_show (label);
    gtk_frame_set_label_widget (GTK_FRAME (frame), label);
    gtk_label_set_use_markup (GTK_LABEL (label), TRUE);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (frame), fixed);

    int x = 0, y = 0, tab_x = 10, tab_y = 10;
    int wide =(SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH- 4*tab_x)/3, high = 30;

    int count = 0, idstart;
    const char *labstr[6];
    char efwstr[2][256];
    sprintf(efwstr[0], "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 1)"));
    sprintf(efwstr[1], "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 2)"));

    char averagestr[256];
    sprintf(averagestr, "<i><b>%s</b></i>", _("Average"));

    memset(labstr, 0, sizeof(labstr));
    if(!strcmp(title, _("PSAD")))
    {
        count = PSAD_COUNT;
        idstart = PSAD_START_ID;
        labstr[0] = _("SPSA");
        labstr[1] = _("PSAD");
    }
    else if(!strcmp(title, averagestr))
    {
         int k = 0;
        count = AverRes_COUNT - 2;
        idstart = AVERRES_START_ID;
        labstr[k++] = _("AUA");
        labstr[k++] = _("EDD(AUA)");
        labstr[k++] = _("GA(LMP)");
        labstr[k++] = _("EDD(LMP)");
    }
    else if(!strcmp(title, _(efwstr[0])))
    {
        count = EFW_COUNT/2;
        idstart = EFW_START_ID;
        labstr[0] = _("EFW");
    }
    else if(!strcmp(title, _(efwstr[1])))
    {
        count = EFW_COUNT/2;
        idstart = EFW_START_ID + 1;
        labstr[0] = _("EFW");
    }
    else if(!strcmp(title, _("Others")))
    {
        count = OTHERS_COUNT;
        idstart = OTHERS_START_ID;
        labstr[0] = _("Show Comments");
        labstr[1] = _("Show Descriptor");
    }

    for(int t = 0; t < count; t++)
    {
        if(!(t%3))
        {
            x = tab_x;
            if(t /3)
            {
                y += high;
            }
            else
            {
                y += tab_y;
            }
        }
        else
        {
            x += wide + tab_x;
        }
        int id = idstart + t;
        checkbutton = gtk_check_button_new_with_mnemonic (labstr[t]);
        gtk_widget_show (checkbutton);
        gtk_fixed_put (GTK_FIXED (fixed), checkbutton, x, y);
        gtk_widget_set_size_request (checkbutton, wide, high);
        g_signal_connect(G_OBJECT(checkbutton), "clicked", G_CALLBACK(HandleChkBtnClicked), this);
        if(id < MAX_CALCARR_COUNT)CheckButtonArr[id] = checkbutton;
    }

    *h = y + high + tab_y*2;

    gtk_widget_show (frame);
    gtk_widget_set_size_request (frame, (SHOW_WINDOW_WIDTH - 3*GRAY_WIDTH), *h);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    return frame;
}

GtkWidget *ViewSystem::CreatWindowABD(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowABD)
        return m_WindowABD;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

    //add frames
    frame = AddFrameByTitle(_("<i><b>General</b></i>"), ABDO_M, ABD_MEA_START,  ABD_DIST_AO, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
#ifndef CT_322
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Arterial</b></i>"),  ABDO_M, ABD_DIST_AO, ABD_PROX_IVC, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Venous</b></i>"), ABDO_M, ABD_PROX_IVC, ABD_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
#endif

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowABD = scrolledwindow;
}

GtkWidget *ViewSystem::CreatWindowAE(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowAE)
        return m_WindowAE;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

    //add frames
    frame = AddFrameByTitle(_("<i><b>Dimensions(2D)</b></i>"), ADULT_M, ADULT_MEA_START, ADULT_AOR_DIAM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Vessels(2D)</b></i>"), ADULT_M, ADULT_AOR_DIAM, ADULT_A2CD, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>EF &amp; Volume(2D)</b></i>"), ADULT_M, ADULT_A2CD, ADULT_AV_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Valves(2D)</b></i>"), ADULT_M, ADULT_AV_AREA, ADULT_RVAWD_MM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Dimensions(M)</b></i>"), ADULT_M, ADULT_RVAWD_MM, ADULT_LV_ET, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Aortic Valve(M)</b></i>"), ADULT_M, ADULT_LV_ET, ADULT_MV_DE_EXC, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle( _("<i><b>Mitral Valve(M)</b></i>"), ADULT_M, ADULT_MV_DE_EXC, ADULT_TV_DE_EXC, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Tricuspid Valve(M)</b></i>"), ADULT_M, ADULT_TV_DE_EXC, ADULT_LATE_DIAS_SLOPE, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Pulmonic Valve(M)</b></i>"), ADULT_M, ADULT_LATE_DIAS_SLOPE, ADULT_AV_ACC_T, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
#ifndef CT_322
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Aortic Valve(D)</b></i>"), ADULT_M, ADULT_AV_ACC_T, ADULT_HR_MV, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Mitral Valve(D)</b></i>"), ADULT_M, ADULT_HR_MV, ADULT_HR_TV, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Tricuspid Valve(D)</b></i>"), ADULT_M, ADULT_HR_TV, ADULT_HR_PV, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Pulmonic Valve(D)</b></i>"), ADULT_M, ADULT_HR_PV, ADULT_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
#endif
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowAE = scrolledwindow;
}

GtkWidget *ViewSystem::CreatWindowUR(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowUR)
        return m_WindowUR;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

    //add frames
    frame = AddFrameByTitle(_("<i><b>Bladder</b></i>"), UR_M, UR_MEA_START, UR_PROSTATE_VOL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Prostate</b></i>"), UR_M, UR_PROSTATE_VOL, UR_L_KID_VOL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Left Renal</b></i>"), UR_M, UR_L_KID_VOL, UR_R_KID_VOL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle( _("<i><b>Right Renal</b></i>"), UR_M, UR_R_KID_VOL, UR_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    // Show PSAD
    frame = AddFrameForOthers(_("PSAD"), &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowUR = scrolledwindow;
}
#ifdef VET
GtkWidget *ViewSystem::CreatWindowAnOB(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowANOB)
        return m_WindowANOB;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

    char title[256];

    sprintf(title, "<i><b>%s</b></i>", _(" Dog "));
    frame = AddFrameByTitle(title, ANOB_M, ANOB_MEA_START, ANOB_CAT_HD, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s</b></i>", _(" Cat "));
    frame = AddFrameByTitle(title, ANOB_M, ANOB_CAT_HD, ANOB_SHEEP_CRL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s</b></i>", _(" Sheep "));
    frame = AddFrameByTitle(title, ANOB_M, ANOB_SHEEP_CRL, ANOB_SWINE_HL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s</b></i>", _(" Swine "));
    frame = AddFrameByTitle(title, ANOB_M,ANOB_SWINE_HL, ANOB_BOVINE_KCRL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s</b></i>", _(" Bovine "));
    frame = AddFrameByTitle( title, ANOB_M, ANOB_BOVINE_KCRL, ANOB_EQUINE_GSD, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s</b></i>", _(" Equine "));
    frame = AddFrameByTitle(title, ANOB_M, ANOB_EQUINE_GSD, ANOB_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowANOB = scrolledwindow;
}
#endif

GtkWidget *ViewSystem::CreatWindowOB(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowOB)
        return m_WindowOB;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

    char title[256];
    //add frames
    sprintf(title, "<i><b>%s</b></i>", _("Average"));
    frame = AddFrameForOthers(title, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    //Fetal 1
    sprintf(title, "<i><b>%s%s</b></i>", _("Advanced"), _("(Fetus 1)"));
    frame = AddFrameByTitle(title, OB_M, OB_MEA_START, OB_AFI, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("AFI"), _("(Fetus 1)"));
    frame = AddFrameByTitle(title, OB_M, OB_AFI, OB_CRL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("Early"), _("(Fetus 1)"));
    frame = AddFrameByTitle(title, OB_M, OB_CRL, OB_RA_DIMEN, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("Fetal Heart"), _("(Fetus 1)"));
    frame = AddFrameByTitle( title, OB_M, OB_RA_DIMEN, OB_GS, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("General"), _("(Fetus 1)"));
    frame = AddFrameByTitle(title, OB_M, OB_GS, OB_EAR, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    //EFW
    sprintf(title, "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 1)"));
    //AddOBEfwTable(fixed, &y, title, 1);
    frame = AddFrameForOthers(title, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("Other"), _("(Fetus 1)"));
    frame = AddFrameByTitle(title, OB_M, OB_EAR, OB_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    //Fetal 2
    sprintf(title, "<i><b>%s%s</b></i>", _("Advanced"), _("(Fetus 2)"));
    frame = AddFrameByTitle2(title, OB_M, OB_MEA_START, OB_AFI, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("AFI"), _("(Fetus 2)"));
    frame = AddFrameByTitle2(title, OB_M, OB_AFI, OB_CRL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("Early"), _("(Fetus 2)"));
    frame = AddFrameByTitle2(title, OB_M, OB_CRL, OB_RA_DIMEN, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("Fetal Heart"), _("(Fetus 2)"));
    frame = AddFrameByTitle2( title, OB_M, OB_RA_DIMEN, OB_GS, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("General"), _("(Fetus 2)"));
    frame = AddFrameByTitle2(title, OB_M, OB_GS, OB_EAR, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    //EFW
    sprintf(title, "<i><b>%s%s</b></i>", _("EFW"), _("(Fetus 2)"));
    frame = AddFrameForOthers(title, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    sprintf(title, "<i><b>%s%s</b></i>", _("Other"), _("(Fetus 2)"));
    frame = AddFrameByTitle2(title, OB_M, OB_EAR, OB_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowOB = scrolledwindow;
}

GtkWidget *ViewSystem::CreatWindowGYN(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowGYN)
        return m_WindowGYN;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

//add frames
    frame = AddFrameByTitle(_("<i><b>General</b></i>"), GYN_M, GYN_MEA_START, GYN_L_FOLL1, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Fertility</b></i>"), GYN_M, GYN_L_FOLL1, GYN_FIBROID1, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Fibroid</b></i>"), GYN_M, GYN_FIBROID1, GYN_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowGYN = scrolledwindow;
}

GtkWidget *ViewSystem::CreatWindowSP(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowSP)
        return m_WindowSP;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

//add frames
    frame = AddFrameByTitle(_("<i><b>Breast</b></i>"), SP_M, SP_MEA_START, SP_L_TESTIS_VOL, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Testicle</b></i>"), SP_M, SP_L_TESTIS_VOL, SP_ISTHMUS_AP, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Thyroid</b></i>"), SP_M, SP_ISTHMUS_AP, SP_L_EYE_OA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Eyeball</b></i>"), SP_M, SP_L_EYE_OA, SP_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowSP = scrolledwindow;
}

GtkWidget *ViewSystem::CreatWindowFE(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowFE)
        return m_WindowFE;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

//add frames
    frame = AddFrameByTitle(_("<i><b>Dimen(2D)</b></i>"), FETAL_M, FETAL_MEA_START, FETAL_AOR_DIAM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Vessels(2D)</b></i>"), FETAL_M, FETAL_AOR_DIAM, FETAL_AO_AN_DIAM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Valves(2D)</b></i>"), FETAL_M, FETAL_AO_AN_DIAM, FETAL_RVAWD_MM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Dimen(M)</b></i>"), FETAL_M, FETAL_RVAWD_MM, FETAL_AOR_DIAM_MM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Vessels(M)</b></i>"), FETAL_M, FETAL_AOR_DIAM_MM, FETAL_LV_ET_MM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>AV &amp; MV(M)</b></i>"), FETAL_M, FETAL_LV_ET_MM, FETAL_LATE_DIAS_SLOPE_MM, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>PV &amp; TV(M)</b></i>"), FETAL_M, FETAL_LATE_DIAS_SLOPE_MM, FETAL_PLACENTA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
#ifndef CT_322
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Utero &amp; Placenta(D)</b></i>"), FETAL_M, FETAL_PLACENTA, FETAL_FROAMEN_OVALE, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Vessels(D)</b></i>"), FETAL_M, FETAL_FROAMEN_OVALE, FETAL_LVOT_ACC_TIME, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>AV &amp; MV(D)</b></i>"), FETAL_M, FETAL_LVOT_ACC_TIME, FETAL_RVOT_ACC_TIME, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>PV &amp; TV(D)</b></i>"), FETAL_M, FETAL_RVOT_ACC_TIME, FETAL_THORACIC_AO, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>Peripheral Vasc(D)</b></i>"), FETAL_M, FETAL_THORACIC_AO, FETAL_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
#endif
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowFE = scrolledwindow;
}

GtkWidget *ViewSystem::CreatWindowOTHERS(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowOTHERS)
        return m_WindowOTHERS;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

//add frames
#if(defined(CT_313) || defined(CT_322))
#else//TCD// VES
    frame = AddFrameByTitle(_("TCD"), TCD_M, TCD_MEA_START, TCD_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;
#endif
//add frames
    frame = AddFrameByTitle(_("Orthopedic"), ORTHO_M, ORTHO_MEA_START, ORTHO_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

//others
    frame = AddFrameForOthers(_("Others"), &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowOTHERS = scrolledwindow;
}
#ifdef VET
GtkWidget *ViewSystem::CreatWindowTD(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowTD)
        return m_WindowTD;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

//add frames
    frame = AddFrameByTitle(_("<i><b>Tendon Le1</b></i>"), TD_M,  TD_MEA_START, TD_LES2_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le2</b></i>"), TD_M, TD_LES2_AREA  , TD_LES3_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le3</b></i>"), TD_M, TD_LES3_AREA, TD_LES4_AREA,  &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le4</b></i>"),TD_M, TD_LES4_AREA, TD_LES5_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le5</b></i>"), TD_M, TD_LES5_AREA, TD_LES6_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le6</b></i>"), TD_M, TD_LES6_AREA, TD_LES7_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le7</b></i>"), TD_M, TD_LES7_AREA, TD_LES8_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le8</b></i>"), TD_M, TD_LES8_AREA, TD_LES9_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le9</b></i>"), TD_M, TD_LES9_AREA, TD_LES10_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le10</b></i>"), TD_M, TD_LES10_AREA, TD_LES11_AREA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    frame = AddFrameByTitle(_("<i><b>Tendon Le11</b></i>"), TD_M, TD_LES11_AREA, TD_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 5;

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowTD = scrolledwindow;
}

#endif
GtkWidget *ViewSystem::CreatWindowVES(void)
{
    GtkWidget* scrolledwindow;
    GtkWidget* frame;
    GtkWidget *viewport;
    GtkWidget* fixed;
    int x = GRAY_WIDTH, y = 20, high = 0;

    if(m_WindowVES)
        return m_WindowVES;

    viewport = gtk_viewport_new (NULL, NULL);
    gtk_widget_show (viewport);

    fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);
    gtk_container_add (GTK_CONTAINER (viewport), fixed);

//add frames
    frame = AddFrameByTitle(_("<i><b>Carotid</b></i>"), VS_M, VS_MEA_START, VS_L_CFA, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>LE Art</b></i>"), VS_M,  VS_L_CFA, VS_L_ANTECUBE, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);
    y += high + 20;

    frame = AddFrameByTitle(_("<i><b>UE Art</b></i>"), VS_M, VS_L_ANTECUBE, VS_MEA_END, &high);
    gtk_fixed_put (GTK_FIXED (fixed), frame, x, y);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolledwindow, SHOW_WINDOW_WIDTH, 430);
    gtk_fixed_put (GTK_FIXED (m_FixedReportSet), scrolledwindow, (TREE_WINDOW_WIDTH + 2*GRAY_WIDTH), 20);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

    return m_WindowVES = scrolledwindow;
}

bool ViewSystem::UniqueItem(const char *sections, const char *templet, const char *childsection)
{
    sqlite3_stmt *stmt = NULL;
    ostringstream stream;
    int result = 0;

    stream << "SELECT COUNT(*) AS num_result FROM CustomReport " << "WHERE sections LIKE '" << sections
                << "' " << "AND templet LIKE '" << templet << "' " << "AND childsection LIKE '" << childsection << "' ";
    string sql = stream.str();

    OpenDB();
    if (sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK)
    {
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        if (strcmp(sqlite3_column_name(stmt, 0), "num_result") == 0)
            result = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    CloseDB();

    if (result)
        return false;
    else
        return true;
}

GtkTreeIter ViewSystem::InsertUniqueComment(GtkTreeModel *model, const char *str)
{
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_get_iter_first(model, &tmp_iter);
    while (has_node == TRUE)
    {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strcmp(strtmp, str) == 0)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}

void ViewSystem::ChangeCommentExamBox(int probe_type, char *check_part)
{
    int comment_probe = GetCommentProbeType();
    if(comment_probe == probe_type)
    {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_comment), check_part);
    }
}

void ViewSystem::ChangeCommentExamBoxDelete(int probe_type)
{
    int comment_probe = GetCommentProbeType();
    if(comment_probe == probe_type)
    {
        ExamItem examItem;
        vector<int> itemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[comment_probe].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));

        vecExamItem_comment.clear();
        vector<int>::iterator iter_index;

#if 0
        ExamItem::ParaItem paraItem;
        for(int i=0;i<10;i++)
        {
            examItem.ReadExamItemPara(comment_probe, i,  &paraItem,NULL);
            ExamItemArray[i].name=paraItem.alias;
            PRINTF("---name=%s\n", ExamItemArray[i].name.c_str());
        }
#endif
        for (iter_index = itemIndex.begin(); iter_index != itemIndex.end(); iter_index++)
        {
            vecExamItem_comment.push_back(ExamItemArray[*iter_index].name);

        }
        CreateDefineItem_comment(comment_probe, vecExamItem_comment);

        int exam_size(0);
        exam_size = vecExamItem_comment.size();

        for(int i = exam_size; i >= 0; i--)
        {
            gtk_combo_box_remove_text(GTK_COMBO_BOX (m_combobox_exam_comment), i);
        }

        for (int i = 0; i <exam_size; i++)
        {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_comment), _(vecExamItem_comment[i].c_str()));
        }
    }
}

GtkTreeIter ViewSystem::InsertUnique(GtkTreeModel *model, GtkTreeIter *iter, const char *str)
{
    GtkTreeIter tmp_iter;
    char *strtmp = NULL;
    gboolean has_node = gtk_tree_model_iter_children(model, &tmp_iter, iter);
    while (has_node == TRUE)
    {
        gtk_tree_model_get(model, &tmp_iter, 0, &strtmp, -1);
        if (strcmp(strtmp, str) == 0)
            return tmp_iter;
        else
            has_node = gtk_tree_model_iter_next(model, &tmp_iter);
    }
    return tmp_iter;
}

void ViewSystem::EntrytempletInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));
    if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)))
    {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}
void ViewSystem::BtnNewCheckPartOkClicked(GtkButton *button)
{
    if(m_save_or_new_flag == 1)
    {
        get_current_and_save_image_para(NULL);
    }
    else if (m_save_or_new_flag == 2)
    {
        image_default_setting();
    }
    else
    {
        char *check_part = (char*) gtk_entry_get_text(GTK_ENTRY(m_entry_new_check_part));
        AddCheckPart(check_part);
    }
    gtk_widget_hide(m_frame_new_check_part);
}

void ViewSystem::BtnNewCheckPartCancelClicked(GtkButton *button)
{
    gtk_widget_hide(m_frame_new_check_part);
}
void ViewSystem::BtnOkClicked(GtkButton *button)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    char *sections, *templet, *temp, *new_string, buf[256];
    ostringstream stream;

    if (tree_depth == 1)
    {
        gtk_tree_model_get(model, &iter, 0, &sections, -1);
        templet = (char *)gtk_entry_get_text(GTK_ENTRY(m_entry_templet));

        memset(buf, 0, sizeof(buf));
        if(strlen(templet))
        {
            int k, t;
            char buffer[256];
            sprintf(buffer, "%s", templet);

            for(k = 0,t = 0; '\0' != buffer[k]; k++)
            {
                // ascii code 32 is space
                if(32 != buffer[k])
                {
                    buf[t++] = buffer[k];
                }
                else if(0 != t)
                {
                    if(32 != buf[t-1])buf[t++] = buffer[k];
                }
            }

            if(t > 1)
            {
                if(32 == buf[t-1])buf[t-1] = '\0';
            }
            buf[t] = '\0';
        }
        new_string = buf;

        if(!(strlen(new_string) > 0) || (!UniqueItem(sections, new_string)))
        {
            if(strlen(new_string) > 0)
            {
                GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
                gtk_tree_selection_select_iter(selected_node, &iter_tmp);
            }
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _("Template name has existed or the input format is incorrect.Please input again!"), NULL);
            return;
        }
        if (gtk_tree_model_iter_has_child(model, &iter) == FALSE)
            stream << "UPDATE CustomReport SET templet = '" << new_string << "' WHERE sections = '" << sections << "'";
        else
            stream << "INSERT INTO CustomReport VALUES('" << sections << "', '" << new_string << "', '', '')";
    }
    else if (tree_depth == 2)
    {
        GtkTreeIter parent;
        gtk_tree_model_get(model, &iter, 0, &temp, -1);
        gtk_tree_model_iter_parent(model, &parent, &iter);
        gtk_tree_model_get(model, &parent, 0, &sections, -1);

        if(NULL != strstr(temp, defaultflag))
        {
            templet = &temp[DEF_FLAG_LEN + 1];
        }
        else
        {
            templet = temp;
        }

        unsigned int child = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_childsection));
        new_string = (char*)sectionstr[child][init_language];
        for(int language = 0; language < LANG_MAX; language++)
        {
            if(!UniqueItem(sections, templet, sectionstr[child][language]))
            {
                GtkTreeIter iter_tmp = InsertUnique(model, &iter, new_string);
                gtk_tree_selection_select_iter(selected_node, &iter_tmp);
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _("Selected exam type has existed!"), NULL);
                return;
            }
        }

        if (gtk_tree_model_iter_has_child(model, &iter) == FALSE)
            stream << "UPDATE CustomReport SET childsection = '" << new_string  << "' WHERE sections = '" << sections  << "' AND templet = '" << templet << "'";
        else
            stream << "INSERT INTO CustomReport VALUES('"  << sections << "', '"  << templet << "', '"  << sectionstr[child][init_language] << "', '')";
    }

    char *errmsg = NULL;
    string sql = stream.str();
    OpenDB();
    if (sqlite3_exec(CustomReport_db, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
        PRINTF("INSERT or UPDATE error:%s!\n", errmsg);
        return;
    }
    CloseDB();

    GtkTreeIter iter_new;
    gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
    GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_TreeviewReportSet), new_path);
    GtkTreeViewColumn *column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_TreeviewReportSet), 0);
    gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_TreeviewReportSet),  new_path, column_tree_view,  renderer,  TRUE);
    gtk_tree_path_free (new_path);

    {
        GtkTreeIter iter;
        GtkTreeModel *model;
        GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
        if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        gint tree_depth = gtk_tree_path_get_depth(path);
        gtk_tree_path_free (path);

        if(tree_depth == 1)
        {
            gtk_widget_show(button_add);
            gtk_widget_hide(button_delete);
            gtk_widget_hide(button_save);
        }
        else if(tree_depth == 2)
        {
            gtk_widget_show(button_add);
            gtk_widget_show(button_delete);
            gtk_widget_hide(button_save);
        }
        else
        {
            gtk_widget_hide(button_add);
            gtk_widget_show(button_delete);
            gtk_widget_show(button_save);
        }
    }
    gtk_widget_hide(label_childsection);
    gtk_widget_hide(m_combobox_childsection);
    gtk_widget_hide(label_templet);
    gtk_widget_hide(m_entry_templet);
    gtk_widget_hide(button_ok);
    gtk_widget_hide(button_cancel);
}

void ViewSystem::BtnCancelClicked(GtkButton *button)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    if(tree_depth == 1)
    {
        gtk_widget_show(button_add);
        gtk_widget_hide(button_delete);
    }
    else if(tree_depth == 2)
    {
        gtk_widget_show(button_add);
        gtk_widget_show(button_delete);
    }
    else
    {
        gtk_widget_hide(button_add);
        gtk_widget_show(button_delete);
    }

    gtk_widget_hide(button_save);
    gtk_widget_hide(label_childsection);
    gtk_widget_hide(m_combobox_childsection);

    gtk_widget_hide(label_templet);
    gtk_widget_hide(m_entry_templet);
    gtk_widget_hide(button_ok);
    gtk_widget_hide(button_cancel);
}

void ViewSystem::BtnAddClicked(GtkButton *button)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    if(tree_depth == 2)
    {
        gtk_widget_hide(button_add);
        gtk_widget_hide(button_save);
        gtk_widget_hide(button_delete);
        gtk_widget_hide(label_templet);
        gtk_widget_hide(m_entry_templet);

        gtk_widget_show(label_childsection);
        gtk_widget_show(m_combobox_childsection);
        gtk_widget_show(button_ok);
        gtk_widget_show(button_cancel);
    }
    else if(tree_depth == 1)
    {
        gtk_widget_hide(button_add);
        gtk_widget_hide(button_save);
        gtk_widget_hide(button_delete);
        gtk_widget_hide(label_childsection);
        gtk_widget_hide(m_combobox_childsection);

        gtk_widget_show(label_templet);
        gtk_widget_show(m_entry_templet);
        gtk_widget_grab_focus(m_entry_templet);
        gtk_widget_show(button_ok);
        gtk_widget_show(button_cancel);
    }

}

void ViewSystem::BtnSave2Clicked(GtkButton *button)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) return;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    char *sections, *templet, *childsection, *temp, record[1024];
    int start, end;
    if (tree_depth == 3)
    {
        GtkTreeIter iter_parent0;
        GtkTreeIter iter_parent1;
        gtk_tree_model_get(model, &iter, 0, &childsection, -1);
        gtk_tree_model_iter_parent(model, &iter_parent0, &iter);
        gtk_tree_model_get(model, &iter_parent0, 0, &temp, -1);
        gtk_tree_model_iter_parent(model, &iter_parent1, &iter_parent0);
        gtk_tree_model_get(model, &iter_parent1, 0, &sections, -1);

        if(NULL != strstr(temp, defaultflag))
        {
            templet = &temp[DEF_FLAG_LEN + 1];
        }
        else
        {
            templet = temp;
        }

        ostringstream stream;
        memset(record, 0, sizeof(record));
        if(!InitRecordFromShowArr(childsection, &start, &end))return;
        memcpy(record, &ShowArr[start], end - start);

        if(StrCmpSectionString(OB_M, childsection, NULL))
        {
            //Fetal 2
            memcpy(&record[OB_CALC_COUNT], &ShowArr[MAX_CALC_COUNT],  OB_CALC_COUNT);
            //EFW
            memcpy(&record[(OB_CALC_COUNT)*2], &ShowArr[EFW_START_ID], EFW_COUNT);
            //average
            memcpy(&record[(OB_CALC_COUNT)*2 + EFW_COUNT], &ShowArr[AVERRES_START_ID], AverRes_COUNT);
        }
        if(StrCmpSectionString(UR_M, childsection, NULL))
        {
            //PSAD
            memcpy(&record[end-start], &ShowArr[PSAD_START_ID], PSAD_COUNT);
        }
        if(StrCmpSectionString(OTHERS_M, childsection, NULL))
        {
            //Ohters
            memcpy(&record[end-start], &ShowArr[OTHERS_START_ID], OTHERS_COUNT);
        }

        stream << "UPDATE CustomReport SET record = '" << record << "' WHERE sections = '" << sections << "' AND templet = '" << templet << "' AND childsection = '" << childsection << "'";
        string sql = stream.str();
        char *errmsg = NULL;
        OpenDB();

        if (sqlite3_exec(CustomReport_db, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL);
            PRINTF("save error:%s!\n", errmsg);
        }
        CloseDB();
    }
}

void ViewSystem::BtnRecoveryClicked(GtkButton *button)
{
    bool ret = false;
    if ( !remove(CustomReport_PATH) && InitDB()) ret = true;
    if(show_window)gtk_widget_hide_all(show_window);

    gtk_widget_show(button_add);
    gtk_widget_hide(button_delete);
    gtk_widget_hide(button_save);
    gtk_widget_hide(label_childsection);
    gtk_widget_hide(m_combobox_childsection);

    gtk_widget_hide(label_templet);
    gtk_widget_hide(m_entry_templet);
    gtk_widget_hide(button_ok);
    gtk_widget_hide(button_cancel);

    GtkTreeModel *modelnew = CreateTreeModel();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_TreeviewReportSet), modelnew);
    g_object_unref (modelnew);

    GtkTreePath *path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_TreeviewReportSet), path, NULL, TRUE);
    gtk_tree_path_free(path);

    if(ret)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window),  ViewDialog::INFO,  _("Recovery succeeded!"), NULL); //Recovery succeed! Restart request!
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Recovery failed!"), NULL);
    }
}

void ViewSystem::BtnDeleteClicked(GtkButton *button)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) return;
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);

    char *sections, *templet, *childsection, *temp;
    ostringstream stream;

    if (tree_depth == 1)
    {
        return;
    }
    else if (tree_depth == 2)
    {
        GtkTreeIter parent;
        gtk_tree_model_get(model, &iter, 0, &temp, -1);
        gtk_tree_model_iter_parent(model, &parent, &iter);
        gtk_tree_model_get(model, &parent, 0, &sections, -1);

        if(NULL != strstr(temp, defaultflag))
        {
            templet = &temp[DEF_FLAG_LEN + 1];
        }
        else
        {
            templet = temp;
        }

        int nChild = gtk_tree_model_iter_n_children(model, &parent);
        if (nChild == 1)
            stream << "UPDATE CustomReport SET templet = '' " << "WHERE sections = '" << sections << "'";
        else if (nChild > 1)
            stream << "DELETE FROM CustomReport WHERE sections = '" << sections << "' AND templet = '" << templet << "'";
        else
            PRINTF("no node to delete\n");
    }
    else if (tree_depth == 3)
    {
        GtkTreeIter parent1, parent2;
        gtk_tree_model_get(model, &iter, 0, &childsection, -1);
        gtk_tree_model_iter_parent(model, &parent1, &iter);
        gtk_tree_model_get(model, &parent1, 0, &temp, -1);
        gtk_tree_model_iter_parent(model, &parent2, &parent1);
        gtk_tree_model_get(model, &parent2, 0, &sections, -1);

        if(NULL != strstr(temp, defaultflag))
        {
            templet = &temp[DEF_FLAG_LEN + 1];
        }
        else
        {
            templet = temp;
        }

        int nChild = gtk_tree_model_iter_n_children(model, &parent1);
        if (nChild == 1)
            stream << "UPDATE CustomReport SET childsection = '' " << "WHERE sections = '" << sections << "' AND templet = '" << templet << "'";
        else if (nChild > 1)
            stream << "DELETE FROM CustomReport WHERE sections = '" << sections << "' AND templet = '" << templet << "' AND childsection = '" << childsection << "'";
        else
           PRINTF("no node to delete\n");
    }

    char *errmsg = NULL;
    string sql = stream.str();

    OpenDB();
    if (sqlite3_exec(CustomReport_db, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,  _("Database error!"), NULL);
        PRINTF("DELETE ERROR:%s!\n", errmsg);
        return;
    }
    CloseDB();

    if (gtk_tree_model_iter_has_child(model, &iter) == FALSE)
    {
        //null windows
        if(show_window)gtk_widget_show_all (show_window);
    }

    GtkTreeIter parent_iter;
    gtk_tree_model_iter_parent(model, &parent_iter, &iter);
    GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);
    gtk_tree_store_remove(GTK_TREE_STORE(model), &topIter);

    GtkTreeModel *modelnew = CreateTreeModel();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_TreeviewReportSet), modelnew);
    g_object_unref (modelnew);

    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_TreeviewReportSet), parent_path);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_TreeviewReportSet), parent_path, NULL, TRUE);
    gint parent_depth = gtk_tree_path_get_depth(parent_path);
    gtk_tree_path_free (parent_path);

    if(show_window)gtk_widget_hide_all(show_window);

    if(parent_depth == 1)
    {
        gtk_widget_hide(button_delete);
    }
    else if(parent_depth == 2)
    {
        gtk_widget_show(button_delete);
    }

    gtk_widget_show(button_add);
    gtk_widget_hide(button_save);
    gtk_widget_hide(label_childsection);
    gtk_widget_hide(m_combobox_childsection);

    gtk_widget_hide(label_templet);
    gtk_widget_hide(m_entry_templet);
    gtk_widget_hide(button_ok);
    gtk_widget_hide(button_cancel);
}

void ViewSystem::InitReportVar(void)
{
    show_window = NULL;

    m_WindowABD = NULL;
    m_WindowUR = NULL;
    m_WindowAE = NULL;
    m_WindowOB = NULL;
    m_WindowGYN = NULL;
    m_WindowSP = NULL;
    m_WindowFE = NULL;
    m_WindowOTHERS = NULL;
    m_WindowVES = NULL;
    m_WindowTD = NULL;
    m_WindowANOB = NULL;

    m_FixedReportSet = NULL;
    m_TreeviewReportSet = NULL;
    memset(ShowArr, UNCHECK_VALUE, sizeof(ShowArr));
    memset(CheckButtonArr, 0, sizeof(CheckButtonArr));

    SysGeneralSetting sys;
    init_language = sys.GetLanguage();
}

GtkWidget *ViewSystem::create_set_report(void)
{
    InitReportVar();
    GtkWidget* fixed = gtk_fixed_new ();
    gtk_widget_show (fixed);

    GtkWidget* scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_fixed_put (GTK_FIXED (fixed), scrolledwindow, GRAY_WIDTH, 20);
    gtk_widget_set_size_request (scrolledwindow, TREE_WINDOW_WIDTH, 430);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_OUT);
    gtk_widget_show (scrolledwindow);

    GtkTreeModel *model = CreateTreeModel();
    m_TreeviewReportSet = gtk_tree_view_new_with_model(model);
    g_object_unref (model);

    gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_TreeviewReportSet), FALSE);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (m_TreeviewReportSet), TRUE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(m_TreeviewReportSet), FALSE);
    gtk_tree_selection_set_mode (gtk_tree_view_get_selection(GTK_TREE_VIEW (m_TreeviewReportSet)), GTK_SELECTION_SINGLE);
    GtkTreePath *path = gtk_tree_path_new_first();
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_TreeviewReportSet), path, NULL, TRUE);
    gtk_tree_path_free(path);
    g_signal_connect(G_OBJECT(gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet))), "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
  gtk_widget_add_events(GTK_WIDGET(m_TreeviewReportSet), (gtk_widget_get_events(GTK_WIDGET(m_TreeviewReportSet)) | GDK_BUTTON_RELEASE_MASK));

   g_signal_connect_after(m_TreeviewReportSet, "button_release_event", G_CALLBACK(HandleExamDepartmentBtnClicked), this);

    renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(m_TreeviewReportSet), column);

    gtk_widget_show (m_TreeviewReportSet);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), m_TreeviewReportSet);

    int y = 465, w = 148, h = 40;

    //create buttons: delete, add, save, recovery
    GtkWidget *button_recovery= gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_recovery);
    gtk_fixed_put (GTK_FIXED (fixed), button_recovery, 10, y);
    gtk_widget_set_size_request (button_recovery, w, h);

    button_add = gtk_button_new_with_mnemonic (_("Insert"));
    gtk_widget_show (button_add);
    gtk_fixed_put (GTK_FIXED (fixed), button_add, 170, y);
    gtk_widget_set_size_request (button_add, w, h);

    button_save = gtk_button_new_with_mnemonic (_("Save"));
    gtk_widget_hide (button_save);
    gtk_fixed_put (GTK_FIXED (fixed), button_save, 170, y);
    gtk_widget_set_size_request (button_save, w, h);

    button_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_hide (button_delete);
    gtk_fixed_put (GTK_FIXED (fixed), button_delete, 330, y);
    gtk_widget_set_size_request (button_delete, w, h);

    g_signal_connect ((gpointer) button_recovery, "clicked", G_CALLBACK (on_button_recovery_clicked), this);
    g_signal_connect ((gpointer) button_add, "clicked", G_CALLBACK (on_button_add_clicked), this);
    g_signal_connect ((gpointer) button_save, "clicked", G_CALLBACK (on_button_save2_clicked), this);
    g_signal_connect ((gpointer) button_delete, "clicked", G_CALLBACK (on_button_delete_clicked), this);

    //create insert template
    label_templet = gtk_label_new (_("<b>Please Input New Name:</b>"));
    gtk_widget_hide (label_templet);
    gtk_fixed_put (GTK_FIXED (fixed), label_templet, 170, y);
    gtk_widget_set_size_request (label_templet, 200, h);
    gtk_label_set_use_markup (GTK_LABEL (label_templet), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_templet), 0.9, 0.5);

    m_entry_templet = gtk_entry_new ();
    gtk_widget_hide (m_entry_templet);
    gtk_fixed_put (GTK_FIXED (fixed), m_entry_templet, 380, y);
    gtk_widget_set_size_request (m_entry_templet, w, h);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_templet), 9679);
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_templet), 46);
    g_signal_connect(G_OBJECT(m_entry_templet), "insert_text", G_CALLBACK(on_entry_templet_insert), this);

    //create insert childsection
    label_childsection = gtk_label_new (_("<b>Please Select Exam Type:</b>"));
    gtk_widget_hide (label_childsection);
    gtk_fixed_put (GTK_FIXED (fixed), label_childsection, 170, y);
    gtk_widget_set_size_request (label_childsection, 200, h);
    gtk_label_set_use_markup (GTK_LABEL (label_childsection), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_childsection), 0.9, 0.5);

    m_combobox_childsection = gtk_combo_box_new_text ();
    gtk_widget_hide (m_combobox_childsection);
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_childsection, 380, y);
    gtk_widget_set_size_request (m_combobox_childsection, w, h);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Abdomen"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Cardiac"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Urology"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Obstetrics"));
#ifndef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Gynecology"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Small Part"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Fetal Cardio"));
#if (defined(CT_313) || defined(CT_322))
#else//TCD// VES
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Vascular"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Others"));
#ifdef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_childsection), _("Tendon"));
#endif
    gtk_combo_box_set_active((GtkComboBox*)(m_combobox_childsection), 0);

    //create buttons: ok, cancel
    button_ok = gtk_button_new_with_mnemonic (_("OK"));
    gtk_widget_hide (button_ok);
    gtk_fixed_put (GTK_FIXED (fixed), button_ok, 540, y);
    gtk_widget_set_size_request (button_ok, w, h);

    button_cancel = gtk_button_new_with_mnemonic (_("Cancel"));
    gtk_widget_hide (button_cancel);
    gtk_fixed_put (GTK_FIXED (fixed), button_cancel, 700, y);
    gtk_widget_set_size_request (button_cancel, w, h);

    g_signal_connect ((gpointer) button_ok, "clicked", G_CALLBACK (on_button_ok_clicked), this);
    g_signal_connect ((gpointer) button_cancel, "clicked", G_CALLBACK (on_button_cancel_clicked), this);
    return fixed;
}

bool ViewSystem::InitRecordFromShowArr(const char *childsection, int *start, int *end)
{
    bool ret = false;
    if(!childsection ||!start ||!end) return ret;

    if(StrCmpSectionString(ABDO_M, childsection, NULL))
    {
        *start = ABD_MEA_START;
        *end = ABD_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(ADULT_M, childsection, NULL))
    {
        *start = ADULT_MEA_START;
        *end = ADULT_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(UR_M, childsection, NULL))
    {
        *start = UR_MEA_START;
        *end = UR_MEA_END;
        ret = true;
    }
#ifdef VET

 if(StrCmpSectionString(ANOB_M, childsection, NULL))
    {
        *start = ANOB_MEA_START;
        *end = ANOB_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(TD_M, childsection, NULL))
    {
        *start = TD_MEA_START;
        *end = TD_MEA_END;
        ret = true;
    }
#else
    if(StrCmpSectionString(OB_M, childsection, NULL))
    {
        *start = OB_MEA_START;
        *end = OB_MEA_END;
        ret = true;
    }
    if(StrCmpSectionString(GYN_M, childsection, NULL))
    {
        *start = GYN_MEA_START;
        *end = GYN_MEA_END;
        ret = true;
    }
#endif
    if(StrCmpSectionString(SP_M, childsection, NULL))
    {
        *start = SP_MEA_START;
        *end = SP_MEA_END;
        ret = true;
    }
#ifdef CT_322
#else
    if(StrCmpSectionString(VS_M, childsection, NULL))
    {
         *start = VS_MEA_START;
        *end = VS_MEA_END;
        ret = true;
    }
#endif
    if(StrCmpSectionString(FETAL_M, childsection, NULL))
    {
         *start = FETAL_MEA_START;
        *end = FETAL_MEA_END;
        ret = true;
    }
#ifdef CT_322
 if(StrCmpSectionString(OTHERS_M, childsection, NULL))
    {
        *start = ORTHO_MEA_START;
        //*end = TCD_MEA_END;
        *end = ORTHO_MEA_END;
        ret = true;
    }
#else
    if(StrCmpSectionString(OTHERS_M, childsection, NULL))
    {
        *start = TCD_MEA_START;
        //*end = TCD_MEA_END;
        *end = ORTHO_MEA_END;
        ret = true;
    }
#endif
    return ret;
}
bool ViewSystem::ReadRecordFromDB(const char * sections, const char* templet, const char* childsection)
{
    if(!sections || !templet ||!childsection)return false;

    ostringstream stream;
    stream << "SELECT record FROM CustomReport WHERE sections = '" << sections << "' AND templet = '" << templet << "' AND childsection = '" << childsection << "'";
    sqlite3_stmt *stmt = NULL;
    string sql = stream.str();
    string record;
    OpenDB();

    if (sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(CustomReport_db));
        return false;
    }

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        if (strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
            record.assign((const char *)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    CloseDB();

    if(record.length() == 0)return false;

    int start,  end;
    //memset(ShowArr, UNCHECK_VALUE, sizeof(ShowArr));
    if(InitRecordFromShowArr(childsection, &start, &end))
    {
        memcpy(&ShowArr[start], record.c_str(), end-start);
    }

    if(StrCmpSectionString(OB_M, childsection, NULL))
    {
        char recordstr[1024];
        strcpy(recordstr, record.c_str());
        //Fetal 2
        memcpy(&ShowArr[MAX_CALC_COUNT], &recordstr[OB_CALC_COUNT], OB_CALC_COUNT);
        //EFW
        memcpy(&ShowArr[EFW_START_ID], &recordstr[(OB_CALC_COUNT)*2], EFW_COUNT);
        //Average
        memcpy(&ShowArr[AVERRES_START_ID], &recordstr[(OB_CALC_COUNT)*2 + EFW_COUNT], AverRes_COUNT);
    }
    if(StrCmpSectionString(UR_M, childsection, NULL))
    {
        char recordstr[1024];
        strcpy(recordstr, record.c_str());
        //PSAD
        memcpy(&ShowArr[PSAD_START_ID], &recordstr[end-start], PSAD_COUNT);
    }
    if(StrCmpSectionString(OTHERS_M, childsection, NULL))
    {
        char recordstr[1024];
        strcpy(recordstr, record.c_str());
        //Ohters
        memcpy(&ShowArr[OTHERS_START_ID], &recordstr[end-start], OTHERS_COUNT);
    }

    int id;
    for(id = 0; id < MAX_CALCARR_COUNT; id++)
    {
        if(ShowArr[id] != CHECK_VALUE)
        {
            ShowArr[id] = UNCHECK_VALUE;
        }
    }
    ShowArr[id] = '\0';
    return true;
}

void ViewSystem::apply_report_setting(void)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *templet;

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE)return;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);
    gtk_tree_model_get(model, &iter, 0, &templet, -1);

    if((tree_depth <= 1) || (tree_depth >= 3))return;
    if(NULL != strstr(templet, defaultflag))return;

    int count = 0;
    if(strlen(templet) > 0)
    {
        OpenDB();
        ostringstream stream2;
        stream2 << "SELECT DISTINCT childsection FROM CustomReport WHERE sections = '" << Customstr[init_language] <<"' AND templet = '" << templet << "'";
        sqlite3_stmt *stmt2 = NULL;
        string sql2 = stream2.str();

        if(sqlite3_prepare(CustomReport_db, sql2.c_str(), sql2.size(), &stmt2, 0) != SQLITE_OK)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
            return;
        }
        while (sqlite3_step(stmt2) != SQLITE_DONE)
        {
            const char *pp = (const char *)sqlite3_column_text(stmt2, 0);
            if (strcmp(pp, "") == 0) continue;
            count ++;
        }
        sqlite3_finalize(stmt2);

        if(count > 0)
        {
            ostringstream stream3;
            stream3 << "UPDATE CustomReport SET record = '" << templet << "' WHERE sections = '" << defaultstr << "' AND templet = '" << defaultstr << "' AND childsection = '" << defaultstr << "'";
            string sql3 = stream3.str();
            if (sqlite3_exec(CustomReport_db, sql3.c_str(), 0, 0, NULL) != SQLITE_OK)
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Database error!"), NULL);
            }

            GtkTreeIter parent_iter;
            gtk_tree_model_iter_parent(model, &parent_iter, &iter);
            GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);

            GtkTreeModel *modelnew = CreateTreeModel();
            gtk_tree_view_set_model(GTK_TREE_VIEW(m_TreeviewReportSet), modelnew);
            g_object_unref (modelnew);
            gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_TreeviewReportSet), parent_path);
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_TreeviewReportSet), parent_path, NULL, TRUE);
            gtk_tree_path_free (parent_path);
            if(show_window)gtk_widget_hide_all(show_window);
        }
        else
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Blank template can't apply!"), NULL);
        }
        CloseDB();
    }
}

bool ViewSystem::CheckFlagFromReportTemplet(int id)
{
    const char *templet;

    SysGeneralSetting sys;
    init_language = sys.GetLanguage();

    InitDB();
    //LocalizationDB(); // 110~120ms
    OpenDB();

    ostringstream stream;
    stream << "SELECT record FROM CustomReport WHERE sections = '" << defaultstr << "' AND templet = '" << defaultstr << "' AND childsection = '" << defaultstr << "'";
    sqlite3_stmt *stmt = NULL;
    string sql = stream.str();
    string record;

    if(sqlite3_prepare(CustomReport_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,   _("Database error!"), NULL);
        if ((id == OTHERS_COMMENT_SHOW_ID) || (id == OTHERS_INDICTION_SHOW_ID))
        {
            return true;
        }
        return false;
    }

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        if(strcmp(sqlite3_column_name(stmt, 0), "record") == 0)
            record.assign((const char *)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);

    if(record.length() > 0)
    {
        templet = record.c_str();
    }
    else
    {
        templet = templet1str[init_language];
    }
    CloseDB();

    memset(ShowArr, UNCHECK_VALUE, sizeof(ShowArr));
    int maxsection = MAX_SECTION;

    for(int t = 0; t < maxsection; t++)
    {
        bool ret = false;
        ret = ReadRecordFromDB(Customstr[init_language], templet, sectionstr[t][init_language]);

        if(false == ret && OTHERS_M == t && ((id == OTHERS_COMMENT_SHOW_ID)||(id == OTHERS_INDICTION_SHOW_ID)))
        {
            return true;
        }
    }

    if(CHECK_VALUE == ShowArr[id])
        return true;
    else
        return false;
}
#endif //add by jhuang

void ViewSystem::CreateWindow(void)
{
    GtkWidget *fixed_main;
    //    GtkWidget *button_title;
    //    GtkWidget *label_title;
    //GtkWidget *notebook;
    GtkWidget *fixed_general;
    GtkWidget *fixed_options;
    GtkWidget *fixed_image;
    GtkWidget *fixed_dicom;
    //GtkWidget *fixed_tvout;
    GtkWidget *fixed_comment;
    GtkWidget *fixed_measure;
    GtkWidget *fixed_kbcal;
    GtkWidget *fixed_calculate;

#if not defined(CT_313)
    GtkWidget *fixed_key_config;
    GtkWidget *label_key_config;
#endif
    GtkWidget *fixed_info;
    GtkWidget *label_general;
    GtkWidget *label_options;

    GtkWidget *label_image_preset;
    GtkWidget *label_measure;
    GtkWidget *label_calculate;
    GtkWidget *label_comment;
    GtkWidget *label_tvout;
    GtkWidget *label_dicom;
    GtkWidget *label_system;

    GtkWidget *button_apply;
    GtkWidget *image_apply;
    GtkWidget *label_apply;

    GtkWidget *button_save;
    GtkWidget *image_save;
    GtkWidget *label_save;

    GtkWidget *button_exit;
    GtkWidget *image_exit;
    GtkWidget *label_exit;

    GtkWidget *label_kbcal;

    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
    int w = DEFALT_SCREEN_WIDTH;

    m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   // gtk_widget_set_size_request (m_window,840, 640);
    gtk_widget_set_size_request (m_window, DEFALT_SCREEN_WIDTH, 640);
    gtk_window_set_position (GTK_WINDOW (m_window), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_window), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_window), _("System Setup"));
    gtk_window_set_transient_for(GTK_WINDOW(m_window), GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()));
    g_signal_connect (G_OBJECT(m_window), "delete-event", G_CALLBACK(on_window_delete_event), this);

    fixed_main = gtk_fixed_new ();
    gtk_widget_show (fixed_main);
    gtk_container_add (GTK_CONTAINER (m_window), fixed_main);

    // button_title = gtk_button_new ();
    // gtk_widget_show (button_title);
    // gtk_fixed_put (GTK_FIXED (fixed_main), button_title, 0, 0);
    // gtk_widget_set_size_request (button_title, 800, 30);
    // GTK_WIDGET_UNSET_FLAGS (button_title, GTK_CAN_FOCUS);
    // gtk_button_set_focus_on_click (GTK_BUTTON (button_title), FALSE);

    // label_title = gtk_label_new (_("System Setup"));
    // gtk_widget_show (label_title);
    // gtk_container_add (GTK_CONTAINER (button_title), label_title);
    // gtk_label_set_justify (GTK_LABEL (label_title), GTK_JUSTIFY_CENTER);

    image_apply = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
    label_apply = gtk_label_new_with_mnemonic (_("Apply"));
    m_button_apply = button_apply = create_button_icon (label_apply, image_apply);
    gtk_widget_modify_bg(m_button_apply, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_fixed_put (GTK_FIXED (fixed_main), button_apply, 500, 560);

    image_save = gtk_image_new_from_stock ("gtk-save", GTK_ICON_SIZE_BUTTON);
    label_save = gtk_label_new_with_mnemonic (_("Save & Exit"));
    m_button_save = button_save = create_button_icon(label_save, image_save);
    gtk_widget_modify_bg(m_button_save, GTK_STATE_INSENSITIVE, g_deepGray);
    gtk_fixed_put (GTK_FIXED (fixed_main), button_save, 630, 560);

    image_exit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
    label_exit = gtk_label_new_with_mnemonic (_("Exit"));
    m_button_exit = button_exit = create_button_icon(label_exit, image_exit);
    gtk_widget_modify_bg(m_button_exit, GTK_STATE_INSENSITIVE, g_deepGray);
    //gtk_fixed_put (GTK_FIXED (fixed_main), button_exit, 700, 560);
    gtk_fixed_put (GTK_FIXED (fixed_main), button_exit, 760, 560);

    m_notebook = gtk_notebook_new ();
    gtk_widget_show (m_notebook);
    gtk_fixed_put (GTK_FIXED (fixed_main), m_notebook, 20, 10);
    //gtk_widget_set_size_request (m_notebook, 800, 540);
    gtk_widget_set_size_request (m_notebook, w - 40, 540);

    g_signal_connect ((gpointer) button_apply, "clicked",
            G_CALLBACK (on_button_apply_clicked),
            this);
    g_signal_connect ((gpointer) button_save, "clicked",
            G_CALLBACK (on_button_save_clicked),
            this);

    g_signal_connect ((gpointer) button_exit, "clicked",
            G_CALLBACK (on_button_exit_clicked),
            this);

    int notebook_counter = 0; //add by jhuang

    // create note general
    fixed_general = create_note_general();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_general);
    init_general_setting(NULL);

    label_general = gtk_label_new (_("General"));
    gtk_widget_show (label_general);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_general);

    // create note options
    fixed_options = create_note_options();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_options);
    init_option_setting(NULL);

    label_options = gtk_label_new (_("Options"));
    gtk_widget_show (label_options);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_options);

    // create note image preset
    fixed_image = create_note_image();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_image);
    init_image_setting();

    label_image_preset = gtk_label_new (_("Image Preset"));
    gtk_widget_show (label_image_preset);
    m_flag_notebook_image = notebook_counter++;
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), m_flag_notebook_image), label_image_preset);

    // create measure preset
    fixed_measure = create_note_measure();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_measure);
    init_measure_setting(NULL);

    label_measure = gtk_label_new (_("Measure"));
    gtk_widget_show (label_measure);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_measure);

// create kbcal
//fixed_kbcal = create_note_kbcal();
    fixed_kbcal = create_note_calc_measure();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_kbcal);

    label_kbcal = gtk_label_new (_("Calc And Caliper"));
    gtk_widget_show (label_kbcal);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_kbcal);

#ifndef VET
    // create calculate table
    fixed_calculate = create_note_calc();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_calculate);
    init_calc_setting(NULL);

    label_calculate = gtk_label_new (_("OB "));
    gtk_widget_show (label_calculate);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_calculate);
#endif
    // create note comment setting
    fixed_comment = create_note_comment();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_comment);
    init_comment_setting(NULL);

    label_comment = gtk_label_new (_("Comment"));
    gtk_widget_show (label_comment);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_comment);

    /// create tvout
    //fixed_tvout = create_note_tvout();
    m_fixed_tvout = gtk_fixed_new();
    gtk_widget_show(m_fixed_tvout);
    gtk_container_add (GTK_CONTAINER (m_notebook), m_fixed_tvout);
    //init_tvout_setting(NULL);

    label_tvout = gtk_label_new (_("Peripheral"));
    //label_tvout = gtk_label_new (_("TV Out"));
    gtk_widget_show (label_tvout);
    m_flag_notebook_tvout = notebook_counter++;
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), m_flag_notebook_tvout), label_tvout);

    /// create reoprt set
    GtkWidget *label_setreport;
    m_FixedReportSet = create_set_report();
    gtk_container_add (GTK_CONTAINER (m_notebook), m_FixedReportSet);
    label_setreport = gtk_label_new (_("Custom Report"));
    gtk_widget_show (label_setreport);
    m_flag_notebook_coustomreport = notebook_counter++;
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), m_flag_notebook_coustomreport), label_setreport);
#ifdef CT_3410
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[0]))
#else
        if(CDCMRegister::GetMe()->IsAuthorize())
#endif
    {
        // create note dicom
        fixed_dicom = create_note_dicom();
        gtk_container_add (GTK_CONTAINER (m_notebook), fixed_dicom);
        // init_dicom_setting();
        label_dicom = gtk_label_new (_("DICOM"));
        gtk_widget_show (label_dicom);
        gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_dicom);
    }
#if defined(CT_313)
    // create note info
    fixed_info = create_note_info();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_info);
    init_info_setting();

    label_system = gtk_label_new (_("System Info"));
    gtk_widget_show (label_system);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_system);

#else
    // create note key config
    fixed_key_config = create_note_key_config();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_key_config);
    init_key_config();
#if defined(CT_322)
    label_key_config = gtk_label_new (_("Value-Gain"));

#elif (defined(CT_460) || defined(CT_355))
    label_key_config = gtk_label_new (_("P1-P3"));
#else
    label_key_config = gtk_label_new (_("P1-P2"));
#endif

    gtk_widget_show (label_key_config);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_key_config);

    // create note info
    fixed_info = create_note_info();
    gtk_container_add (GTK_CONTAINER (m_notebook), fixed_info);
    init_info_setting();
#ifdef CT_3410
    label_system = gtk_label_new (_("Other"));
#else
    label_system = gtk_label_new (_("System Info"));
#endif
    gtk_widget_show (label_system);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_notebook), notebook_counter++), label_system);
 #endif

    g_signal_connect(G_OBJECT(m_notebook), "switch-page", G_CALLBACK(on_notebook_switch_page), this);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_notebook), m_flag_notebook_image);

#if 0
    image_apply = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
    label_apply = gtk_label_new_with_mnemonic (_("Apply"));
    button_apply = create_button_icon (label_apply, image_apply);
    gtk_fixed_put (GTK_FIXED (fixed_main), button_apply, 440, 560);
    gtk_widget_set_sensitive(button_apply, FALSE);

    image_save = gtk_image_new_from_stock ("gtk-save", GTK_ICON_SIZE_BUTTON);
    label_save = gtk_label_new_with_mnemonic (_("Save & Exit"));
    button_save = create_button_icon(label_save, image_save);
    gtk_fixed_put (GTK_FIXED (fixed_main), button_save, 570, 560);
    gtk_widget_set_sensitive(button_save, FALSE);

    image_exit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
    label_exit = gtk_label_new_with_mnemonic (_("Exit"));
    button_exit = create_button_icon(label_exit, image_exit);
    gtk_fixed_put (GTK_FIXED (fixed_main), button_exit, 700, 560);

    g_signal_connect ((gpointer) button_apply, "clicked",
            G_CALLBACK (on_button_apply_clicked),
            this);
    g_signal_connect ((gpointer) button_save, "clicked",
            G_CALLBACK (on_button_save_clicked),
            this);
    g_signal_connect ((gpointer) button_exit, "clicked",
            G_CALLBACK (on_button_exit_clicked),
            this);
#endif
    gtk_widget_show(m_window);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
    return ;
}

gboolean ViewSystem::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    // UserSelect::GetInstance()->close_username();
    return FALSE;
}

static gboolean PowerOff(gpointer data)
{
    execlp("poweroff", "poweroff", NULL);   //system() is deprecated; when memory is lack, it will be not run!
    perror("PowerOff Failed");

    return FALSE;
}

static gboolean ExitWindowSystem(gpointer data)
{
    ViewSystem *tmp;
    tmp = (ViewSystem*)data;
    tmp->DestroyWindow();

    return FALSE;
}

void ViewSystem::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_window)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_window);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        m_window = NULL;
    }

    ///>注册了新功能，关机重启
    if(m_powerOffFlag)
        g_timeout_add(500, PowerOff, this);
}

GtkWidget* ViewSystem::create_note_general(void)
{
    GtkWidget *fixed_general;
    //    GtkWidget *entry_hospital;
    GtkWidget *label_hospital;
    //    GtkWidget *combobox_language;
    //    GtkWidget *combobox_screen_saver;
    GtkWidget *label_screen_saver;
    GtkWidget *label_date_format;
    GtkWidget *label_language;
#ifdef CT_355
    GtkWidget *combobox_video;
    GtkWidget *label_video;
    GtkWidget *label_vga;
#endif
    GtkWidget *frame_datetime;
    GtkWidget *fixed_datetime;
    // GtkWidget *calendar;
    GtkObject *spinbutton_hour_adj;
    // GtkWidget *spinbutton_hour;
    GtkObject *spinbutton_minute_adj;
    // GtkWidget *spinbutton_minute;
    GtkObject *spinbutton_second_adj;
    // GtkWidget *spinbutton_second;
    GtkWidget *label_time;
    GtkWidget *label_datetime;
    GtkWidget *label_time_format;
    GtkWidget *button_adjust_time;
    //GtkWidget *frame_print;

    GtkWidget *button_default;

    fixed_general = gtk_fixed_new ();
    gtk_widget_show (fixed_general);

    m_entry_hospital = gtk_entry_new ();
    gtk_widget_show (m_entry_hospital);
    gtk_fixed_put (GTK_FIXED (fixed_general), m_entry_hospital, 190+30, 20);
    gtk_widget_set_size_request (m_entry_hospital, 250, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_hospital), 9679);
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_hospital), 46);
    g_signal_connect(G_OBJECT(m_entry_hospital), "insert_text", G_CALLBACK(on_entry_hospital_insert), this);

    label_hospital = gtk_label_new (_("<b>Hospital Name:</b>"));
    gtk_widget_show (label_hospital);
    gtk_fixed_put (GTK_FIXED (fixed_general), label_hospital, 20+30, 20);
    gtk_widget_set_size_request (label_hospital, 170, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_hospital), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_hospital), 0.5, 0.5);

    label_language = gtk_label_new (_("<b>Language:</b>"));
    gtk_widget_show (label_language);
    gtk_fixed_put (GTK_FIXED (fixed_general), label_language, 30+20, 350);
    gtk_widget_set_size_request (label_language, 120, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_language), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_language), 0.9, 0.5);

    m_combobox_language = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_language);
    gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_language, 150+20, 350);
    gtk_widget_set_size_request (m_combobox_language, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "English");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "中文");

#ifdef CT_355
    label_vga=gtk_label_new(_("<b>VGA Source:</b>"));
    gtk_widget_show(label_vga);
    gtk_fixed_put(GTK_FIXED(fixed_general),label_vga,30+20,390);
    gtk_widget_set_size_request (label_vga, 135, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_vga), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_vga), 0.9, 0.5);
    m_combobox_vga= gtk_combo_box_new_text();
    gtk_widget_show (m_combobox_vga);
    gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_vga, 165+20, 390);
    gtk_widget_set_size_request (m_combobox_vga, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_vga), _("Internal"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_vga), _("External"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_vga), 0);
    g_signal_connect((gpointer)m_combobox_vga, "changed", G_CALLBACK (on_vga_changed), this);
#endif

#ifdef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "Русский язык");
#else
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "Русский язык");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "Polski");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "Français");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "Español");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_language), "Deutsch");
#endif

    g_signal_connect((gpointer)m_combobox_language, "changed", G_CALLBACK (on_language_changed), this);

    label_date_format = gtk_label_new (_("<b>Date Format:</b>"));
    gtk_widget_show (label_date_format);
   // gtk_fixed_put (GTK_FIXED (fixed_general), label_date_format, 270, 350);
    gtk_fixed_put (GTK_FIXED (fixed_general), label_date_format, 310-30, 350);
    gtk_widget_set_size_request (label_date_format, 120+30, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_date_format), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_date_format), 0.9, 0.5);

    m_combobox_date_format = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_date_format);
    gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_date_format, 410+20, 350);
    gtk_widget_set_size_request (m_combobox_date_format, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_date_format), _("Y/M/D"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_date_format), _("M/D/Y"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_date_format), _("D/M/Y"));

    label_screen_saver = gtk_label_new (_("<b>Screen saver:</b>"));
    gtk_widget_show (label_screen_saver);
    //gtk_fixed_put (GTK_FIXED (fixed_general), label_screen_saver, 510, 350);
    gtk_fixed_put (GTK_FIXED (fixed_general), label_screen_saver, 560-30, 350);
    gtk_widget_set_size_request (label_screen_saver, 120 +20+30, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_screen_saver), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_screen_saver), 0.9, 0.5);

    m_combobox_screen_saver = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_screen_saver);
   // gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_screen_saver, 646 + 4, 350);
    gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_screen_saver, 660+40, 350);
    gtk_widget_set_size_request (m_combobox_screen_saver, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("none"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("5 Min."));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("10 Min."));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("20 Min."));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("30 Min."));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("45 Min."));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_screen_saver), _("60 Min."));

    // label_video = gtk_label_new (_("<b>Video Standard:</b>"));
    // gtk_widget_show (label_video);
    // gtk_fixed_put (GTK_FIXED (fixed_general), label_video, 25, 400);
    // gtk_widget_set_size_request (label_video, 125, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_video), TRUE);
    // gtk_misc_set_alignment (GTK_MISC (label_video), 0.9, 0.5);

    // m_combobox_video = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_video);
    // gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_video, 150, 400);
    // gtk_widget_set_size_request (m_combobox_video, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video), _("PAL"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video), _("NTSC"));
    // gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video), 0);

    // label_connector_type = gtk_label_new (_("<b>TV Connector:</b>"));
    // gtk_widget_show (label_connector_type);
    // gtk_fixed_put (GTK_FIXED (fixed_general), label_connector_type, 275, 400);
    // gtk_widget_set_size_request (label_connector_type, 125, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_connector_type), TRUE);

    // m_combobox_connect_type = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_connect_type);
    // gtk_fixed_put (GTK_FIXED (fixed_general), m_combobox_connect_type, 390, 400);
    // gtk_widget_set_size_request (m_combobox_connect_type, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_connect_type), _("S-Video"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_connect_type), _("Composite"));

    // label_connector_hint = gtk_label_new (_(" (Restart to complete changes)"));
    // gtk_widget_show (label_connector_hint);
    // gtk_fixed_put (GTK_FIXED (fixed_general), label_connector_hint, 500, 400);
    // gtk_widget_set_size_request (label_connector_hint, 210, 30);
    // gtk_misc_set_alignment (GTK_MISC (label_connector_hint), 0, 0.5);

    frame_datetime = gtk_frame_new (NULL);
    gtk_widget_show (frame_datetime);
    gtk_fixed_put (GTK_FIXED (fixed_general), frame_datetime, 20+30, 70);
    gtk_widget_set_size_request (frame_datetime, 600, 240);
    gtk_frame_set_label_align (GTK_FRAME (frame_datetime), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_datetime), GTK_SHADOW_IN);

    fixed_datetime = gtk_fixed_new ();
    gtk_widget_show (fixed_datetime);
    gtk_container_add (GTK_CONTAINER (frame_datetime), fixed_datetime);

    m_calendar = gtk_calendar_new ();
    gtk_widget_show (m_calendar);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), m_calendar, 30, 15);
    gtk_widget_set_size_request (m_calendar, 280, 190);

    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);

    spinbutton_hour_adj = gtk_adjustment_new (0, 0, 23, 1, 1, 0);
    m_spinbutton_hour = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_hour_adj), 1, 0);
    gtk_widget_show (m_spinbutton_hour);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), m_spinbutton_hour, 330, 60);
    gtk_widget_set_size_request (m_spinbutton_hour, 80, 25);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbutton_hour), now_tm->tm_hour);
    g_signal_connect(G_OBJECT(m_spinbutton_hour), "insert_text", G_CALLBACK(on_spinbutton_insert_hour), this);

    spinbutton_minute_adj = gtk_adjustment_new (0, 0, 59, 1, 1, 0);
    m_spinbutton_minute = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_minute_adj), 1, 0);
    gtk_widget_show (m_spinbutton_minute);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), m_spinbutton_minute, 410, 60);
    gtk_widget_set_size_request (m_spinbutton_minute, 80, 25);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbutton_minute), now_tm->tm_min);
    g_signal_connect(G_OBJECT(m_spinbutton_minute), "insert_text", G_CALLBACK(on_spinbutton_insert_time), this);

    spinbutton_second_adj = gtk_adjustment_new (0, 0, 59, 1, 1, 0);
    m_spinbutton_second = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_second_adj), 1, 0);
    gtk_widget_show (m_spinbutton_second);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), m_spinbutton_second, 490, 60);
    gtk_widget_set_size_request (m_spinbutton_second, 80, 25);
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(m_spinbutton_second), now_tm->tm_sec);
    g_signal_connect(G_OBJECT(m_spinbutton_second), "insert_text", G_CALLBACK(on_spinbutton_insert_time), this);

    label_time = gtk_label_new (_("Time Setting:"));
    gtk_widget_show (label_time);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), label_time, 330, 16);
    gtk_widget_set_size_request (label_time, 100+16, 45);
    gtk_misc_set_alignment (GTK_MISC (label_time), 0, 0.5);

    label_time_format = gtk_label_new (_("HH:MM:SS"));
    gtk_widget_show (label_time_format);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), label_time_format, 330, 90);
    gtk_widget_set_size_request (label_time_format, 100, 20);
    gtk_misc_set_alignment (GTK_MISC (label_time_format), 0, 0.5);

    label_datetime = gtk_label_new (_("<b>Date and Time</b>"));
    gtk_widget_show (label_datetime);
    gtk_frame_set_label_widget (GTK_FRAME (frame_datetime), label_datetime);
    gtk_label_set_use_markup (GTK_LABEL (label_datetime), TRUE);

    button_adjust_time = gtk_button_new_with_mnemonic (_("Adjust Time&Date"));
    gtk_widget_show (button_adjust_time);
    gtk_fixed_put (GTK_FIXED (fixed_datetime), button_adjust_time, 330, 160);
    gtk_widget_set_size_request (button_adjust_time, 145, 40);
    g_signal_connect ((gpointer) button_adjust_time, "clicked",
            G_CALLBACK (on_button_adjust_time_clicked),
            this);
#if 0
    frame_print = gtk_frame_new (NULL);
    gtk_widget_show (frame_print);
    gtk_fixed_put (GTK_FIXED (fixed_general), frame_print, 510, 70);
    gtk_widget_set_size_request (frame_print, 340, 240);
    gtk_frame_set_label_align (GTK_FRAME (frame_print), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print), GTK_SHADOW_IN);

    fixed_printer = gtk_fixed_new ();
    gtk_widget_show (fixed_printer);
    gtk_container_add (GTK_CONTAINER (frame_print), fixed_printer);

    m_radiobutton_common = gtk_radio_button_new_with_mnemonic (NULL, _("Common"));
    gtk_widget_show (m_radiobutton_common);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_radiobutton_common, 10, 10);
    gtk_widget_set_size_request (m_radiobutton_common, 100, 30);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_common), radiobutton_printer_group);
    radiobutton_printer_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_common));
    g_signal_connect((gpointer)m_radiobutton_common, "toggled", G_CALLBACK (on_common_radio_button_toggled), this);

    m_radiobutton_specific = gtk_radio_button_new_with_mnemonic (NULL, _("Specific"));
    gtk_widget_show (m_radiobutton_specific);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_radiobutton_specific, 110, 10);
    gtk_widget_set_size_request (m_radiobutton_specific, 100, 30);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_specific), radiobutton_printer_group);
    radiobutton_printer_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_specific));
    g_signal_connect((gpointer)m_radiobutton_specific, "toggled", G_CALLBACK (on_specific_radio_button_toggled), this);

    m_scrolledwindow_common = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (m_scrolledwindow_common, 320, 170);
    gtk_widget_show (m_scrolledwindow_common);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_scrolledwindow_common, 10, 40);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_scrolledwindow_common), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_scrolledwindow_common), GTK_SHADOW_IN);

    m_treeview_common_print = create_common_print_treeview();
    gtk_widget_show (m_treeview_common_print);
    m_selected_common_printer = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_common_print));
    gtk_tree_selection_set_mode(m_selected_common_printer, GTK_SELECTION_SINGLE);
    gtk_container_add (GTK_CONTAINER (m_scrolledwindow_common), m_treeview_common_print);

    m_fixed_specific = gtk_fixed_new ();
    gtk_widget_show (m_fixed_specific);
    gtk_widget_set_size_request (m_fixed_specific, 320, 170);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_fixed_specific, 10, 40);

    scrolledwindow_specific = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_specific);
    gtk_fixed_put(GTK_FIXED(m_fixed_specific), scrolledwindow_specific, 5, 5);
    gtk_widget_set_size_request(scrolledwindow_specific, 320, 145);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_specific), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_specific), GTK_SHADOW_IN);

    m_treeview_specific_print = create_specific_print_treeview();
    gtk_widget_show (m_treeview_specific_print);
    m_selected_specific_printer = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_specific_print));
    gtk_tree_selection_set_mode(m_selected_specific_printer, GTK_SELECTION_SINGLE);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_specific), m_treeview_specific_print);

    GtkWidget *button_add_printer = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_add_printer);
    gtk_fixed_put (GTK_FIXED (m_fixed_specific), button_add_printer, 115, 150);
    gtk_widget_set_size_request (button_add_printer, 100, 30);
    g_signal_connect((gpointer)button_add_printer, "clicked", G_CALLBACK (on_button_add_printer_clicked), this);

    GtkWidget *button_del_printer = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_del_printer);
    gtk_fixed_put (GTK_FIXED (m_fixed_specific), button_del_printer, 225, 150);
    gtk_widget_set_size_request (button_del_printer, 100, 30);
    g_signal_connect((gpointer)button_del_printer, "clicked", G_CALLBACK (on_button_del_printer_clicked), this);

    label_print = gtk_label_new (_("<b>Printer</b>"));
    gtk_widget_show (label_print);
    gtk_frame_set_label_widget (GTK_FRAME (frame_print), label_print);
    gtk_label_set_use_markup (GTK_LABEL (label_print), TRUE);

    g_signal_connect((gpointer)m_selected_common_printer, "changed", G_CALLBACK (on_common_treeview_selection_changed), this);
    g_signal_connect((gpointer)m_selected_specific_printer, "changed", G_CALLBACK (on_specific_treeview_selection_changed), this);
#endif
    button_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_default);
     gtk_fixed_put (GTK_FIXED (fixed_general), button_default, 50, 420+20);
    gtk_widget_set_size_request (button_default, 120+28, 56-16);
    g_signal_connect ((gpointer) button_default, "clicked", G_CALLBACK (on_button_general_default_clicked), this);

    // g_signal_connect ((gpointer) m_combobox_language, "changed",
    //            G_CALLBACK (on_combobox_language_changed),
    //            this);
    // g_signal_connect ((gpointer) combobox_screen_saver, "changed",
    //            G_CALLBACK (on_combobox_screen_saver_changed),
    //            NULL);
    return fixed_general;
}

void ViewSystem::CommonRadioToggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press) {
        gtk_widget_show_all(m_scrolledwindow_common);
        gtk_widget_hide_all(m_fixed_specific);
    }
}

void ViewSystem::SpecificRadioToggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press) {
        gtk_widget_show_all(m_fixed_specific);
        gtk_widget_hide_all(m_scrolledwindow_common);
    }
}

void ViewSystem::SpinbuttonInsertHour(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int hour = atoi(old_str.c_str());
    if (hour < 0 || hour > 23)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
}

void ViewSystem::SpinbuttonInsertTime(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int time = atoi(old_str.c_str());
    if (time < 0 || time > 59)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
}

void ViewSystem::BtnAdjustTimeClicked(GtkButton *button)
{
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(m_calendar), &year, &month, &day);

    int hour, minute, second;
    hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_hour));
    minute = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_minute));
    second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_second));

    char setdatetime[50];
    char hctosys[20];
    sprintf(setdatetime, "hwclock --set --date=\"%d/%d/%d %d:%d:%d\"", month+1, day, year, hour, minute, second);
    //同步时间
    sprintf(hctosys, "hwclock --hctosys");
#if 0
    system(setdatetime);
    system(hctosys);
#else
    _system_(setdatetime);
    _system_(hctosys);
#endif
}

void ViewSystem::CommonTreeviewSelectionChanged(GtkTreeSelection *treeselection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    bool common_selected = gtk_tree_selection_get_selected(m_selected_common_printer, &model, &iter);
    if (common_selected) {
        PRINTF("** unselect specific **\n");
        gtk_tree_selection_unselect_all (GTK_TREE_SELECTION(m_selected_specific_printer));
    }
}

void ViewSystem::ModebmRadioToggled(GtkToggleButton *togglebutton)
{
    int index_display_format = 0;
    SysOptions sysOp;
    index_display_format = sysOp.GetDisplayFormatM();

    if (index_display_format == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_total), TRUE);
    else if (index_display_format == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_11), TRUE);
    else if (index_display_format == 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_21), TRUE);
    else if (index_display_format == 3)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_12), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_lr_11), TRUE);
}

void ViewSystem::ModebpwRadioToggled(GtkToggleButton *togglebutton)
{
    int index_display_format = 0;
    SysOptions sysOp;
    index_display_format = sysOp.GetDisplayFormatPW();
    if (index_display_format == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_total_pw), TRUE);
    else if (index_display_format == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_11_pw), TRUE);
    else if (index_display_format == 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_21_pw), TRUE);
    else if (index_display_format == 3)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_12_pw), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_lr_11_pw), TRUE);
}

void ViewSystem::OnRadiobtnDisplay_total(GtkToggleButton *togglebutton)
{
    SysOptions sysOp;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm)))
    {
        m_bmIndex = 0;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw)))
    {
        m_bpwIndex = 0;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();
}

void ViewSystem::OnRadiobtnDisplay_ud11(GtkToggleButton *togglebutton)
{
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm)))
    {
        m_bmIndex = 1;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw)))
    {
        m_bpwIndex = 1;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();

}

void ViewSystem::OnRadiobtnDisplay_ud21(GtkToggleButton *togglebutton)
{
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm)))
    {
        m_bmIndex = 2;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw)))
    {
        m_bpwIndex = 2;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();

}

void ViewSystem::OnRadiobtnDisplay_ud12(GtkToggleButton *togglebutton)
{
    SysOptions sysOp;

   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm)))
    {
        m_bmIndex = 3;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw)))
    {
        m_bpwIndex = 3;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();
}

void ViewSystem::OnRadiobtnDisplay_lr11(GtkToggleButton *togglebutton)
{
    SysOptions sysOp;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm)))
    {
        m_bmIndex = 4;
        sysOp.SetDisplayFormatM(m_bmIndex);
    }

    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw)))
    {
        m_bpwIndex = 4;
        sysOp.SetDisplayFormatPW(m_bpwIndex);
    }
    sysOp.SyncFile();
}

void ViewSystem::SpecificTreeviewSelectionChanged(GtkTreeSelection *treeselection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    bool specific_selected = gtk_tree_selection_get_selected(m_selected_specific_printer, &model, &iter);
    if (specific_selected) {
        PRINTF("** unselect common **\n");
        gtk_tree_selection_unselect_all (GTK_TREE_SELECTION(m_selected_common_printer));
    }
}

void ViewSystem::ChangeNoteBookPage(int page)
{
    m_current_note_page = page;
    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_notebook), m_current_note_page);
}
void ViewSystem::ChangeDicomNoteBookPage(int page)
{

    gtk_notebook_set_current_page(GTK_NOTEBOOK(m_dicom_notebook), page);
}

void ViewSystem::notebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
    m_current_note_page = page_num;
    if(page_num == m_flag_notebook_coustomreport)
    {
        GtkTreeIter iter;
        GtkTreeModel *model;

        GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_TreeviewReportSet));
        if (gtk_tree_selection_get_selected(selected_node, &model, &iter) == TRUE)
        {
            GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
            gint tree_depth = gtk_tree_path_get_depth(path);
            gtk_tree_path_free (path);

            if(2 == tree_depth)
                gtk_widget_set_sensitive(m_button_apply, TRUE);
            else
                gtk_widget_set_sensitive(m_button_apply, FALSE);
            gtk_widget_set_sensitive(m_button_save, TRUE);
            gtk_widget_set_sensitive(m_button_exit, TRUE);
        }
    }
    else if(page_num == m_flag_notebook_image)
    {
        gtk_widget_set_sensitive(m_button_apply, FALSE);
        gtk_widget_set_sensitive(m_button_save, FALSE);
    }
    else if(page_num == m_flag_notebook_tvout)
    {
        create_note_tvout();
        init_tvout_setting(NULL);
        CreatePrinter();
        init_printer(NULL);
        gtk_widget_set_sensitive(m_button_apply, TRUE);
        gtk_widget_set_sensitive(m_button_save, TRUE);
        gtk_widget_set_sensitive(m_button_exit, TRUE);
    }
    else
    {
        gtk_widget_set_sensitive(m_button_apply, TRUE);
        gtk_widget_set_sensitive(m_button_save, TRUE);
        gtk_widget_set_sensitive(m_button_exit, TRUE);
    }
}

void ViewSystem::BtnAddPrinterClicked(GtkButton *button)
{
    ViewPrinterAdd::GetInstance()->CreateWindow(GTK_WINDOW(m_window));
}

void ViewSystem::BtnDelPrinterClicked(GtkButton *button)
{
    string prnt_name = specific_printer_selection();
    if (!prnt_name.empty()) {
        Printer prnt;
        prnt.DeletePrinter(prnt_name.c_str());
        PeripheralMan::GetInstance()->SwitchPrinterDriver();
    }
    update_specific_printer_model();
}

int ViewSystem::common_printer_selection(void)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(m_selected_common_printer, &model, &iter) != TRUE)
        return -1;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gchar *index_str = gtk_tree_path_to_string (path);
    return atoi(index_str);
}

string ViewSystem::specific_printer_selection(void)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    string ret_val;

    if (gtk_tree_selection_get_selected(m_selected_specific_printer, &model, &iter) != TRUE)
        return ret_val;

    char *value;
    gtk_tree_model_get(model, &iter, PRT_NAME, &value, -1);
    if (value)
        ret_val = value;
    return ret_val;
}
void ViewSystem::update_specific_printer_model(void)
{
    GtkTreeModel* model = create_specific_print_model();
    if (model != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(m_treeview_specific_print), model);
    g_object_unref (model); /* destroy model automatically with view */
}
 //  int date_format;
void ViewSystem::init_general_setting(SysGeneralSetting* sysGeneralSetting)
{
    if (sysGeneralSetting == NULL)
        sysGeneralSetting = new SysGeneralSetting;

    string hospital_name;
    sysGeneralSetting->GetHospital(hospital_name);
    gtk_entry_set_text(GTK_ENTRY(m_entry_hospital), hospital_name.c_str());

    int index_lang = sysGeneralSetting->GetLanguage();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_language), index_lang);

    int date_format = sysGeneralSetting->GetDateFormat();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), date_format);
    int screen_saver = sysGeneralSetting->GetScreenProtect();
    unsigned char index_screen_saver = 0;
    switch (screen_saver) {
        case 0: index_screen_saver = 0; break;
        case 5: index_screen_saver = 1; break;
        case 10: index_screen_saver = 2; break;
        case 20: index_screen_saver = 3; break;
        case 30: index_screen_saver = 4; break;
        case 45: index_screen_saver = 5; break;
        case 60: index_screen_saver = 6; break;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_screen_saver), index_screen_saver);
#if 0
    sysGeneralSetting = new SysGeneralSetting;
    if (sysGeneralSetting->GetPrinterMethod() == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_common), TRUE);
        gtk_widget_show_all(m_scrolledwindow_common);
        gtk_widget_hide_all(m_fixed_specific);
        char printer_path[3];
        int index_printer = sysGeneralSetting->GetPrinter();
        sprintf(printer_path, "%d", index_printer);
        GtkTreePath *path = gtk_tree_path_new_from_string(printer_path);
        gtk_tree_selection_select_path(m_selected_common_printer, path);
    } else if (sysGeneralSetting->GetPrinterMethod() == 1) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_specific), TRUE);
        gtk_widget_hide_all(m_scrolledwindow_common);
        gtk_widget_show_all(m_fixed_specific);
    }
#endif
    delete sysGeneralSetting;
}
extern bool review_pic;
void ViewSystem::save_general_setting(void)
{
    SysGeneralSetting sysGeneralSetting;

   const char * hospital_name = gtk_entry_get_text(GTK_ENTRY(m_entry_hospital));
    if (hospital_name != NULL)                     //in order to avoid segment default
    {
        sysGeneralSetting.SetHospital(hospital_name);
    }
    int langage = sysGeneralSetting.GetLanguage();
    int index_lang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_language));
    if(langage != index_lang)
    {
        char str_cmd[256];

        if (ZH == index_lang)
        {
            setenv("LANG", "zh_CN.UTF-8", 1);
            setenv("LANGUAGE", "zh_CN:zh", 1);
            setlocale(LC_ALL, "zh_CN.UTF-8");
            sprintf(str_cmd, "setxkbmap -layout %s", "us");
            _system_(str_cmd);
            //system("setxkbmap -layout us");
        }
        else if (RU == index_lang)
        {
            setenv("LANG", "ru_RU.UTF-8", 1);
            setenv("LANGUAGE", "ru_RU:ru", 1);
            setlocale(LC_ALL, "ru_RU.UTF-8");
            //system("setxkbmap -layout ru");
            sprintf(str_cmd, "setxkbmap -layout %s", "ru");
            _system_(str_cmd);
        }
        else if (PL == index_lang)
        {
            setenv("LANG", "pl_PL.UTF-8", 1);
            setenv("LANGUAGE", "pl_PL:pl", 1);
            setlocale(LC_ALL, "pl_PL.UTF-8");
            //system("setxkbmap -layout pl qwertz");
            sprintf(str_cmd, "setxkbmap -layout %s", "pl qwertz");
            _system_(str_cmd);
        }
        else if (ES == index_lang)
        {
            setenv("LANG", "es_ES.UTF-8", 1);
            setenv("LANGUAGE", "es_ES:es", 1);
            setlocale(LC_ALL, "es_ES.UTF-8");
            //system("setxkbmap -layout es");
            sprintf(str_cmd, "setxkbmap -layout %s", "es");
            _system_(str_cmd);
        }

        else if (FR == index_lang)
        {
            setenv("LANG", "fr_FR.UTF-8", 1);
            setenv("LANGUAGE", "fr_FR:fr", 1);
            setlocale(LC_ALL, "fr_FR.UTF-8");
            //system("setxkbmap -layout fr");
            sprintf(str_cmd, "setxkbmap -layout %s", "fr");
            _system_(str_cmd);
        }
        else if (DE == index_lang)
        {
            setenv("LANG", "de_DE.UTF-8", 1);
            setenv("LANGUAGE", "de_DE:de", 1);
            setlocale(LC_ALL, "de_DE.UTF-8");
            //system("setxkbmap -layout de");
            sprintf(str_cmd, "setxkbmap -layout %s", "de");
            _system_(str_cmd);
        }
        else
        {
            setenv("LANG", "en_US.UTF-8", 1);
            setenv("LANGUAGE", "en_US:en", 1);
            setlocale(LC_ALL, "en_US.UTF-8");
            //system("setxkbmap -layout us");
            sprintf(str_cmd, "setxkbmap -layout %s", "us");
            _system_(str_cmd);
        }
        // setlocale(LC_NUMERIC, "en_US.UTF-8");
        // setlocale(LC_NUMERIC, "en_US.UTF-8");
        ChangeKeymap();
        sysGeneralSetting.SetLanguage(index_lang);
    }

    int date_format = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_date_format));
    sysGeneralSetting.SetDateFormat(date_format);
    MenuArea::GetInstance()->UpdateLabel();
    KnobMenu::GetInstance()->Update();
    if(!review_pic)
    {
        UpdateHospitalandpart(date_format, hospital_name);
    }

    int ScreenSaverIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_screen_saver));
    sysGeneralSetting.SetScreenProtect(ScreenSaverIndex);

    unsigned char ScreenSaver = 0;
    switch (ScreenSaverIndex) {
        case 0: ScreenSaver = 0; break;
        case 1: ScreenSaver = 5; break;
        case 2: ScreenSaver = 10; break;
        case 3: ScreenSaver = 20; break;
        case 4: ScreenSaver = 30; break;
        case 5: ScreenSaver = 45; break;
        case 6: ScreenSaver = 60; break;
    }
    ScreenSaver::GetInstance()->SetPeriod(ScreenSaver * 60);

#if 0
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_common))) {
        int printIndex = common_printer_selection();
        if (printIndex != -1) {
            sysGeneralSetting.SetPrinterMethod(0);
            sysGeneralSetting.SetPrinter(printIndex);
        }
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_specific))) {
        string default_print = specific_printer_selection();
        if (!default_print.empty()) {
            Printer print;
            sysGeneralSetting.SetPrinterMethod(1);
            print.SetDefault(default_print.c_str());
            PeripheralMan::GetInstance()->SwitchPrinterDriver();
            update_specific_printer_model();
        }
    }
#endif
    sysGeneralSetting.SyncFile();
    //update calc menu
    g_menuCalc.UpdateLabel();
    g_menuMeasure.UpdateLabel();
    ViewNewPat::GetInstance()->UpdateNameLength();
}

GtkWidget* ViewSystem::create_note_options(void)
{
    GtkWidget *fixed_options;

    //GtkWidget *frame_biopsy_line;
    //GtkWidget *hbox_biopsy;
    //GtkWidget *m_radiobutton_biopsy_on;
    //GSList *radiobutton_biopsy_group = NULL;
    //GtkWidget *radiobutton_biopsy_off;
    //GtkWidget *label_biopsy_line;

    GtkWidget *frame_TI;
    GtkWidget *hbox_TI;
    //GtkWidget *m_radiobutton_TIC;
    GSList *radiobutton_TI_group = NULL;
    //GtkWidget *m_radiobutton_TIB;
    //GtkWidget *m_radiobutton_TIS;

    GtkWidget *label_TI;
    GtkWidget *frame_center_line;
    GtkWidget *hbox_center;

    GtkWidget *frame_display_format;
    GtkWidget *label_display_foramt;

    GSList *radiobutton_center_group = NULL;

    //GtkWidget *m_radiobutton_center_on;
    //GtkWidget *m_radiobutton_center_off;
    GtkWidget *label_center_line;
    GtkWidget *frame_kb_sound;
    GtkWidget *hbox_kb_sound;
    //GtkWidget *m_radiobutton_kb_sound_on;

    GSList *radiobutton_kb_sound_group = NULL;
    //GtkWidget *m_radiobutton_kb_sound_off;

    GtkWidget *label_kb_sound;
    GtkWidget *button_default_options;
    GtkWidget *frame_image_options;
    GtkWidget *table_img_options;

    GtkWidget *hbox_display_format;
    GSList *radiobtn_display_format_group_bm = NULL;
    GSList *radiobtn_display_format_group_bpw = NULL;

    //GtkWidget *combobox_img_format;
    //GtkWidget *combobox_img_medium;
    //GtkWidget *combobox_img_file;

    GtkWidget *label_img_file;
    //GtkWidget *label_img_medium;

    GtkWidget *label_img_format;
    GtkWidget *label_image_options;
    GtkWidget *frame_video_options;
    GtkWidget *table_video_options;

    //GtkWidget *combobox_video_format;
    //GtkWidget *combobox_video_medium;

    GtkWidget *label_video_file;
    //GtkWidget *label_video_medium;
    GtkWidget *label_video_format;
    //GtkWidget *combobox_video_file;

    GtkWidget *label_video_options;
    GtkWidget *fixed_display_format;
    GtkWidget *label_mode_bm;
    GtkWidget *label_mode_bpw;

    fixed_options = gtk_fixed_new ();
    gtk_widget_show (fixed_options);

    // biopsy line
    // frame_biopsy_line = gtk_frame_new (NULL);
    // gtk_widget_show (frame_biopsy_line);
    // gtk_fixed_put (GTK_FIXED (fixed_options), frame_biopsy_line, 30, 30);
    // gtk_widget_set_size_request (frame_biopsy_line, 170, 60);
    // gtk_frame_set_label_align (GTK_FRAME (frame_biopsy_line), 0.5, 0.5);
    // gtk_frame_set_shadow_type (GTK_FRAME (frame_biopsy_line), GTK_SHADOW_IN);

    // label_biopsy_line = gtk_label_new (_("<b>Biopsy Line</b>"));
    // gtk_widget_show (label_biopsy_line);
    // gtk_frame_set_label_widget (GTK_FRAME (frame_biopsy_line), label_biopsy_line);
    // gtk_label_set_use_markup (GTK_LABEL (label_biopsy_line), TRUE);

    // hbox_biopsy = gtk_hbox_new (TRUE, 0);
    // gtk_widget_show (hbox_biopsy);
    // gtk_container_add (GTK_CONTAINER (frame_biopsy_line), hbox_biopsy);

    // m_radiobutton_biopsy_on = gtk_radio_button_new_with_mnemonic (NULL, _("ON"));
    // gtk_widget_show (m_radiobutton_biopsy_on);
    // gtk_box_pack_start (GTK_BOX (hbox_biopsy), m_radiobutton_biopsy_on, FALSE, FALSE, 0);
    // gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_biopsy_on), radiobutton_biopsy_group);
    // radiobutton_biopsy_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_biopsy_on));

    // m_radiobutton_biopsy_off = gtk_radio_button_new_with_mnemonic (NULL, _("OFF"));
    // gtk_widget_show (m_radiobutton_biopsy_off);
    // gtk_box_pack_start (GTK_BOX (hbox_biopsy), m_radiobutton_biopsy_off, FALSE, FALSE, 0);
    // gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_biopsy_off), radiobutton_biopsy_group);
    // radiobutton_biopsy_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_biopsy_off));

    // TI
    frame_TI = gtk_frame_new (NULL);
    gtk_widget_show (frame_TI);
    //gtk_fixed_put (GTK_FIXED (fixed_options), frame_TI, 30, 30);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_TI, 50, 30);
    gtk_widget_set_size_request (frame_TI, 320, 60);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_TI, 50, 30);
    gtk_widget_set_size_request (frame_TI, 170, 60);
#endif
    gtk_frame_set_label_align (GTK_FRAME (frame_TI), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_TI), GTK_SHADOW_IN);
    gtk_widget_set_sensitive(frame_TI, FALSE);

    label_TI = gtk_label_new (_("<b>TI</b>"));
    gtk_widget_show (label_TI);
    gtk_frame_set_label_widget (GTK_FRAME (frame_TI), label_TI);
    gtk_label_set_use_markup (GTK_LABEL (label_TI), TRUE);

    hbox_TI = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_TI);
    gtk_container_add (GTK_CONTAINER (frame_TI), hbox_TI);

    m_radiobutton_TIC = gtk_radio_button_new_with_mnemonic (NULL, _("TIC"));
    gtk_widget_show (m_radiobutton_TIC);
    gtk_box_pack_start (GTK_BOX (hbox_TI), m_radiobutton_TIC, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_TIC), radiobutton_TI_group);
    radiobutton_TI_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_TIC));

    m_radiobutton_TIB = gtk_radio_button_new_with_mnemonic (NULL, _("TIB"));
    gtk_widget_show (m_radiobutton_TIB);
    gtk_box_pack_start (GTK_BOX (hbox_TI), m_radiobutton_TIB, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_TIB), radiobutton_TI_group);
    radiobutton_TI_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_TIB));

    m_radiobutton_TIS = gtk_radio_button_new_with_mnemonic (NULL, _("TIS"));
    gtk_widget_show (m_radiobutton_TIS);
    gtk_box_pack_start (GTK_BOX (hbox_TI), m_radiobutton_TIS, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_TIS), radiobutton_TI_group);
    radiobutton_TI_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_TIS));

    // center line
    frame_center_line = gtk_frame_new (NULL);
    gtk_widget_show (frame_center_line);
    //gtk_fixed_put (GTK_FIXED (fixed_options), frame_center_line, 30, 120); //lihuamei 2012.09.28
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_center_line, 450, 30);
    gtk_widget_set_size_request (frame_center_line, 320, 60);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_center_line, 500+50, 30);
    gtk_widget_set_size_request (frame_center_line, 170, 60);
#endif
    gtk_frame_set_label_align (GTK_FRAME (frame_center_line), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_center_line), GTK_SHADOW_IN);

    label_center_line = gtk_label_new (_("<b>Center Line</b>"));
    gtk_widget_show (label_center_line);
    gtk_frame_set_label_widget (GTK_FRAME (frame_center_line), label_center_line);
    gtk_label_set_use_markup (GTK_LABEL (label_center_line), TRUE);

    hbox_center = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_center);
    gtk_container_add (GTK_CONTAINER (frame_center_line), hbox_center);

    m_radiobutton_center_on = gtk_radio_button_new_with_mnemonic (NULL, _("ON"));
    gtk_widget_show (m_radiobutton_center_on);
    gtk_box_pack_start (GTK_BOX (hbox_center), m_radiobutton_center_on, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_center_on), radiobutton_center_group);
    radiobutton_center_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_center_on));

    m_radiobutton_center_off = gtk_radio_button_new_with_mnemonic (NULL, _("OFF"));
    gtk_widget_show (m_radiobutton_center_off);
    gtk_box_pack_start (GTK_BOX (hbox_center), m_radiobutton_center_off, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_center_off), radiobutton_center_group);
    radiobutton_center_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_center_off));

    // sound of keyboard
    frame_kb_sound = gtk_frame_new (NULL);
    gtk_widget_show (frame_kb_sound);
   // gtk_fixed_put (GTK_FIXED (fixed_options), frame_kb_sound, 240, 30);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_kb_sound, 450, 120);
    gtk_widget_set_size_request (frame_kb_sound, 320, 60);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_kb_sound, 300, 30);
    gtk_widget_set_size_request (frame_kb_sound, 170, 60);
#endif
    gtk_frame_set_label_align (GTK_FRAME (frame_kb_sound), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_kb_sound), GTK_SHADOW_IN);

    label_kb_sound = gtk_label_new (_("<b>Keyboard Sound</b>"));
    gtk_widget_show (label_kb_sound);
    gtk_frame_set_label_widget (GTK_FRAME (frame_kb_sound), label_kb_sound);
    gtk_label_set_use_markup (GTK_LABEL (label_kb_sound), TRUE);

    hbox_kb_sound = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_kb_sound);
    gtk_container_add (GTK_CONTAINER (frame_kb_sound), hbox_kb_sound);

    m_radiobutton_kb_sound_on = gtk_radio_button_new_with_mnemonic (NULL, _("ON"));
    gtk_widget_show (m_radiobutton_kb_sound_on);
    gtk_box_pack_start (GTK_BOX (hbox_kb_sound), m_radiobutton_kb_sound_on, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_kb_sound_on), radiobutton_kb_sound_group);
    radiobutton_kb_sound_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_kb_sound_on));

    m_radiobutton_kb_sound_off = gtk_radio_button_new_with_mnemonic (NULL, _("OFF"));
    gtk_widget_show (m_radiobutton_kb_sound_off);
    gtk_box_pack_start (GTK_BOX (hbox_kb_sound), m_radiobutton_kb_sound_off, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_kb_sound_off), radiobutton_kb_sound_group);
    radiobutton_kb_sound_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_kb_sound_off));

// mouse speed
    GtkWidget *frame_mouse_speed = gtk_frame_new (_("<b>Mouse Speed</b>"));
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_mouse_speed, 50, 120);
    gtk_widget_set_size_request (frame_mouse_speed, 320, 60);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_mouse_speed, 660, 120);
    gtk_widget_set_size_request (frame_mouse_speed, 160, 160);
#endif
    gtk_frame_set_label_align (GTK_FRAME (frame_mouse_speed), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_mouse_speed), GTK_SHADOW_IN);
    gtk_label_set_use_markup (GTK_LABEL(GTK_FRAME (frame_mouse_speed)->label_widget), TRUE);
    GtkWidget *fixed_mouse_speed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(frame_mouse_speed), fixed_mouse_speed);
#ifdef CT_322
    GtkWidget *vbox_mouse_speed = gtk_hbox_new (TRUE, 45);
    gtk_fixed_put (GTK_FIXED (fixed_mouse_speed), vbox_mouse_speed, 24, 10);
#else
    GtkWidget *vbox_mouse_speed = gtk_vbox_new (TRUE, 20);
    gtk_fixed_put (GTK_FIXED (fixed_mouse_speed), vbox_mouse_speed, 30, 20);
#endif

    m_radiobutton_mouse_speed_low = gtk_radio_button_new_with_mnemonic (NULL, _("Low"));
    gtk_box_pack_start (GTK_BOX (vbox_mouse_speed), m_radiobutton_mouse_speed_low, FALSE, FALSE, 0);

    GSList *mouse_speed_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_mouse_speed_low));
    m_radiobutton_mouse_speed_middle = gtk_radio_button_new_with_mnemonic (mouse_speed_group, _("Middle"));
    gtk_box_pack_start (GTK_BOX (vbox_mouse_speed), m_radiobutton_mouse_speed_middle, FALSE, FALSE, 0);

    mouse_speed_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_mouse_speed_low));
    m_radiobutton_mouse_speed_high = gtk_radio_button_new_with_mnemonic (mouse_speed_group, _("High"));
    gtk_box_pack_start (GTK_BOX (vbox_mouse_speed), m_radiobutton_mouse_speed_high, FALSE, FALSE, 0);
    gtk_widget_show_all(frame_mouse_speed);

    // display format
#if 0
    GtkWidget *frame_mode = gtk_frame_new(NULL);
    gtk_widget_show(frame_mode);
   // gtk_fixed_put(GTK_FIXED(fixed_options), frame_mode, 30, 120);
    gtk_fixed_put(GTK_FIXED(fixed_options), frame_mode, 50, 120);
    gtk_widget_set_size_request(frame_mode, 100, 160);
    gtk_frame_set_label_align (GTK_FRAME (frame_mode), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_mode), GTK_SHADOW_IN);
    GtkWidget *label_mode = gtk_label_new_with_mnemonic(_("<b>MODE</b>"));
    gtk_widget_show(label_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_mode), label_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_mode), TRUE);

    GtkWidget *fixed_mode = gtk_fixed_new();
    gtk_widget_show(fixed_mode);
    gtk_container_add(GTK_CONTAINER(frame_mode), fixed_mode);

    m_radiobtn_bm = gtk_radio_button_new_with_mnemonic(NULL, _("B/M"));
    gtk_widget_show (m_radiobtn_bm);
    gtk_fixed_put (GTK_FIXED (fixed_mode), m_radiobtn_bm, 8, 10);
    gtk_widget_set_size_request (m_radiobtn_bm, 80, 30);
    GSList *radiobutton_mode_group = NULL;
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_bm), radiobutton_mode_group);
    radiobutton_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_bm));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm), FALSE);
    g_signal_connect((gpointer)m_radiobtn_bm, "toggled", G_CALLBACK (on_modebm_radio_button_toggled), this);

    m_radiobtn_bpw = gtk_radio_button_new_with_mnemonic(NULL, _("B/PW"));
    gtk_widget_show (m_radiobtn_bpw);
    gtk_fixed_put (GTK_FIXED (fixed_mode), m_radiobtn_bpw, 8, 45);
    gtk_widget_set_size_request (m_radiobtn_bpw, 80, 30);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_bpw), radiobutton_mode_group);
    radiobutton_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_bpw));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobtn_bpw), FALSE);
    g_signal_connect((gpointer)m_radiobtn_bpw, "toggled", G_CALLBACK (on_modebpw_radio_button_toggled), this);
#endif
    //12.28

    frame_display_format = gtk_frame_new (NULL);
    gtk_widget_show (frame_display_format);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_display_format, 50, 210);
    gtk_widget_set_size_request (frame_display_format, 720, 90);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_display_format, 50, 120);
    gtk_widget_set_size_request (frame_display_format, 420+150, 160);
#endif
    gtk_frame_set_label_align (GTK_FRAME (frame_display_format), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_display_format), GTK_SHADOW_IN);

    label_display_foramt = gtk_label_new (_("<b>Display Format</b>"));
    gtk_widget_show (label_display_foramt);
    gtk_frame_set_label_widget (GTK_FRAME (frame_display_format), label_display_foramt);
    gtk_label_set_use_markup (GTK_LABEL (label_display_foramt), TRUE);

    fixed_display_format = gtk_fixed_new ();
    gtk_widget_show (fixed_display_format);
    gtk_container_add (GTK_CONTAINER (frame_display_format), fixed_display_format);
#if 0
    hbox_display_format = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_display_format);
    gtk_container_add (GTK_CONTAINER (frame_display_format), hbox_display_format);
#endif
    m_radiobtn_total = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobtn_total);
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_total, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_total, 70, 8);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_total), radiobtn_display_format_group_bm);
    radiobtn_display_format_group_bm = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_total));
    // g_signal_connect((gpointer)m_radiobtn_total, "toggled", G_CALLBACK (on_display_total_radio_button_toggled), this);

    GtkWidget *vbox_format_1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_1);
    gtk_container_add (GTK_CONTAINER (m_radiobtn_total), vbox_format_1);

    GtkWidget *image_format_1 = create_pixmap ("./res/btn_format/1.jpg");
    gtk_widget_show (image_format_1);
    gtk_box_pack_start (GTK_BOX (vbox_format_1), image_format_1, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_1), 5, 10);
#if 0
    GtkWidget *label_format_1 = gtk_label_new (_("TOTAL"));
    gtk_widget_modify_fg(label_format_1, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_1, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_1);
    gtk_box_pack_start (GTK_BOX (vbox_format_1), label_format_1, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_1), TRUE);
#endif
    m_radiobtn_ud_11 = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobtn_ud_11);
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_ud_11, FALSE, FALSE, 0);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_11, 200, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_11, 170, 8);
#endif
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_ud_11), radiobtn_display_format_group_bm);
    radiobtn_display_format_group_bm = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_ud_11));
    // g_signal_connect((gpointer)m_radiobtn_ud_11, "toggled", G_CALLBACK (on_display_ud11_radio_button_toggled), this);

    GtkWidget *vbox_format_2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_2);
    gtk_container_add (GTK_CONTAINER (m_radiobtn_ud_11), vbox_format_2);

    GtkWidget *image_format_2 = create_pixmap ("./res/btn_format/2.jpg");
    gtk_widget_show (image_format_2);
    gtk_box_pack_start (GTK_BOX (vbox_format_2), image_format_2, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_2), 5, 10);

#if 0
    GtkWidget *label_format_2 = gtk_label_new (_("UD_11"));
    gtk_widget_modify_fg(label_format_2, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_2, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_2);
    gtk_box_pack_start (GTK_BOX (vbox_format_2), label_format_2, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_2), TRUE);
#endif

    m_radiobtn_ud_21 = gtk_radio_button_new(NULL);
    gtk_widget_show (m_radiobtn_ud_21);
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_ud_21, FALSE, FALSE, 0);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_21, 330, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_21, 270, 8);
#endif
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_ud_21), radiobtn_display_format_group_bm);
    radiobtn_display_format_group_bm = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_ud_21));
    // g_signal_connect((gpointer)m_radiobtn_ud_21, "toggled", G_CALLBACK (on_display_ud21_radio_button_toggled), this);

    GtkWidget *vbox_format_3 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_3);
    gtk_container_add (GTK_CONTAINER (m_radiobtn_ud_21), vbox_format_3);

    GtkWidget *image_format_3 = create_pixmap ("./res/btn_format/3.jpg");
    gtk_widget_show (image_format_3);
    gtk_box_pack_start (GTK_BOX (vbox_format_3), image_format_3, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_3), 5, 10);

#if 0
    GtkWidget *label_format_3 = gtk_label_new (_("UD_21"));
    gtk_widget_modify_fg(label_format_3, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_3, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_3);
    gtk_box_pack_start (GTK_BOX (vbox_format_3), label_format_3, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_3), TRUE);
#endif

    m_radiobtn_ud_12 = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobtn_ud_12);
    //gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_ud_12, FALSE, FALSE, 0);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_12, 460, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_12, 370, 8);
#endif
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_ud_12), radiobtn_display_format_group_bm);
    radiobtn_display_format_group_bm = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_ud_12));
    // g_signal_connect((gpointer)m_radiobtn_ud_12, "toggled", G_CALLBACK (on_display_ud12_radio_button_toggled), this);

    GtkWidget *vbox_format_4 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_4);
    gtk_container_add (GTK_CONTAINER (m_radiobtn_ud_12), vbox_format_4);

    GtkWidget *image_format_4 = create_pixmap ("./res/btn_format/4.jpg");
    gtk_widget_show (image_format_4);
    gtk_box_pack_start (GTK_BOX (vbox_format_4), image_format_4, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_4), 5, 10);
#if 0
    GtkWidget *label_format_4 = gtk_label_new (_("UD_12"));
    gtk_widget_modify_fg(label_format_4, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_4, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_4);
    gtk_box_pack_start (GTK_BOX (vbox_format_4), label_format_4, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_4), TRUE);
#endif
    m_radiobtn_lr_11 = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobtn_lr_11);
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_lr_11, FALSE, FALSE, 0);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_lr_11, 590, 8);
#else
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_lr_11, 470, 8);
#endif
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_lr_11), radiobtn_display_format_group_bm);
    radiobtn_display_format_group_bm = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_lr_11));
    // g_signal_connect((gpointer)m_radiobtn_lr_11, "toggled", G_CALLBACK (on_display_lr11_radio_button_toggled), this);

    GtkWidget *vbox_format_5 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_5);
    gtk_container_add (GTK_CONTAINER (m_radiobtn_lr_11), vbox_format_5);

    GtkWidget *image_format_5 = create_pixmap ("./res/btn_format/5.jpg");
    gtk_widget_show (image_format_5);
    gtk_box_pack_start (GTK_BOX (vbox_format_5), image_format_5, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_5), 5, 10);

    label_mode_bm = gtk_label_new (_("<b>M: </b>"));
    gtk_widget_show (label_mode_bm);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), label_mode_bm, 10, 23);
    gtk_widget_set_size_request (label_mode_bm, 50, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_mode_bm), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_mode_bm), 0, 0.5);
    //pw
    m_radiobtn_total_pw = gtk_radio_button_new (NULL);
#ifdef CT_322
    gtk_widget_hide (m_radiobtn_total_pw);
#else
    gtk_widget_show (m_radiobtn_total_pw);
#endif
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_total, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_total_pw, 70, 70);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_total_pw), radiobtn_display_format_group_bpw);
    radiobtn_display_format_group_bpw = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_total_pw));

    GtkWidget *vbox_format_1_pw = gtk_vbox_new (FALSE, 0);
#ifdef CT_322
    gtk_widget_hide (vbox_format_1_pw);
#else
    gtk_widget_show (vbox_format_1_pw);
#endif
    gtk_container_add (GTK_CONTAINER (m_radiobtn_total_pw), vbox_format_1_pw);

    GtkWidget *image_format_1_pw = create_pixmap ("./res/btn_format/1.jpg");
#ifdef CT_322
    gtk_widget_hide (image_format_1_pw);

#else
    gtk_widget_show (image_format_1_pw);
#endif
    gtk_box_pack_start (GTK_BOX (vbox_format_1_pw), image_format_1_pw, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_1_pw), 5, 10);
#if 0
    GtkWidget *label_format_1 = gtk_label_new (_("TOTAL"));
    gtk_widget_modify_fg(label_format_1, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_1, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_1);
    gtk_box_pack_start (GTK_BOX (vbox_format_1), label_format_1, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_1), TRUE);
#endif

    m_radiobtn_ud_11_pw = gtk_radio_button_new (NULL);
#ifdef CT_322
    gtk_widget_hide (m_radiobtn_ud_11_pw);
#else
    gtk_widget_show (m_radiobtn_ud_11_pw);
#endif
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_ud_11, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_11_pw, 170, 70);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_ud_11_pw), radiobtn_display_format_group_bpw);
    radiobtn_display_format_group_bpw = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_ud_11_pw));

    GtkWidget *vbox_format_2_pw = gtk_vbox_new (FALSE, 0);
#ifdef CT_322
    gtk_widget_hide (vbox_format_2_pw);
#else
    gtk_widget_show (vbox_format_2_pw);
#endif
    gtk_container_add (GTK_CONTAINER (m_radiobtn_ud_11_pw), vbox_format_2_pw);

    GtkWidget *image_format_2_pw = create_pixmap ("./res/btn_format/2.jpg");
#ifdef CT_322
    gtk_widget_hide (image_format_2_pw);
#else
    gtk_widget_show (image_format_2_pw);
#endif
    gtk_box_pack_start (GTK_BOX (vbox_format_2_pw), image_format_2_pw, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_2_pw), 5, 10);

#if 0
    GtkWidget *label_format_2 = gtk_label_new (_("UD_11"));
    gtk_widget_modify_fg(label_format_2, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_2, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_2);
    gtk_box_pack_start (GTK_BOX (vbox_format_2), label_format_2, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_2), TRUE);
#endif

    m_radiobtn_ud_21_pw = gtk_radio_button_new(NULL);
#ifdef CT_322
    gtk_widget_hide (m_radiobtn_ud_21_pw);
#else
    gtk_widget_show (m_radiobtn_ud_21_pw);
#endif
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_ud_21, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_21_pw, 270, 70);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_ud_21_pw), radiobtn_display_format_group_bpw);
    radiobtn_display_format_group_bpw = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_ud_21_pw));

    GtkWidget *vbox_format_3_pw = gtk_vbox_new (FALSE, 0);
#ifdef CT_322
    gtk_widget_hide (vbox_format_3_pw);
#else
    gtk_widget_show (vbox_format_3_pw);
#endif
    gtk_container_add (GTK_CONTAINER (m_radiobtn_ud_21_pw), vbox_format_3_pw);

    GtkWidget *image_format_3_pw = create_pixmap ("./res/btn_format/3.jpg");
#ifdef CT_322
    gtk_widget_hide (image_format_3_pw);
#else
    gtk_widget_show (image_format_3_pw);
#endif
    gtk_box_pack_start (GTK_BOX (vbox_format_3_pw), image_format_3_pw, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_3_pw), 5, 10);

#if 0
    GtkWidget *label_format_3 = gtk_label_new (_("UD_21"));
    gtk_widget_modify_fg(label_format_3, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_3, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_3);
    gtk_box_pack_start (GTK_BOX (vbox_format_3), label_format_3, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_3), TRUE);
#endif

    m_radiobtn_ud_12_pw = gtk_radio_button_new (NULL);
#ifdef CT_322
    gtk_widget_hide (m_radiobtn_ud_12_pw);
#else
    gtk_widget_show (m_radiobtn_ud_12_pw);
#endif
    //gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_ud_12, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_ud_12_pw, 370, 70);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_ud_12_pw), radiobtn_display_format_group_bpw);
    radiobtn_display_format_group_bpw = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_ud_12_pw));

    GtkWidget *vbox_format_4_pw = gtk_vbox_new (FALSE, 0);
#ifdef CT_322
    gtk_widget_hide (vbox_format_4_pw);
#else
    gtk_widget_show (vbox_format_4_pw);
#endif
    gtk_container_add (GTK_CONTAINER (m_radiobtn_ud_12_pw), vbox_format_4_pw);

    GtkWidget *image_format_4_pw = create_pixmap ("./res/btn_format/4.jpg");
#ifdef CT_322
    gtk_widget_hide (image_format_4_pw);
#else
    gtk_widget_show (image_format_4_pw);
#endif

    gtk_box_pack_start (GTK_BOX (vbox_format_4_pw), image_format_4_pw, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_4_pw), 5, 10);
#if 0
    GtkWidget *label_format_4 = gtk_label_new (_("UD_12"));
    gtk_widget_modify_fg(label_format_4, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_4, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_4);
    gtk_box_pack_start (GTK_BOX (vbox_format_4), label_format_4, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_4), TRUE);
#endif
    m_radiobtn_lr_11_pw = gtk_radio_button_new (NULL);
#ifdef CT_322
    gtk_widget_hide (m_radiobtn_lr_11_pw);
#else
    gtk_widget_show (m_radiobtn_lr_11_pw);
#endif
    // gtk_box_pack_start (GTK_BOX (hbox_display_format), m_radiobtn_lr_11, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED (fixed_display_format), m_radiobtn_lr_11_pw, 470, 70);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobtn_lr_11_pw), radiobtn_display_format_group_bpw);
    radiobtn_display_format_group_bpw = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobtn_lr_11_pw));

    GtkWidget *vbox_format_5_pw = gtk_vbox_new (FALSE, 0);
#ifdef CT_322
    gtk_widget_hide (vbox_format_5_pw);
#else
    gtk_widget_show (vbox_format_5_pw);
#endif
    gtk_container_add (GTK_CONTAINER (m_radiobtn_lr_11_pw), vbox_format_5_pw);

    GtkWidget *image_format_5_pw = create_pixmap ("./res/btn_format/5.jpg");
#ifdef CT_322
    gtk_widget_hide (image_format_5_pw);
#else
    gtk_widget_show (image_format_5_pw);
#endif
    gtk_box_pack_start (GTK_BOX (vbox_format_5_pw), image_format_5_pw, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_5_pw), 5, 10);

    label_mode_bpw = gtk_label_new (_("<b>PW/CW: </b>"));
#ifdef CT_322
    gtk_widget_hide (label_mode_bpw);
#else
    gtk_widget_show (label_mode_bpw);
#endif
    gtk_fixed_put (GTK_FIXED (fixed_display_format), label_mode_bpw, 10, 85);
    gtk_widget_set_size_request (label_mode_bpw, 60, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_mode_bpw), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_mode_bpw), 0, 0.5);

#if 0
    GtkWidget *label_format_5 = gtk_label_new (_("LR_11"));
    gtk_widget_modify_fg(label_format_5, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_5, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_5);
    gtk_box_pack_start (GTK_BOX (vbox_format_5), label_format_5, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_5), TRUE);
#endif

    // factory default
    button_default_options = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_default_options);
    //gtk_fixed_put (GTK_FIXED (fixed_options), button_default_options, 30, 380);
    //gtk_fixed_put (GTK_FIXED (fixed_options), button_default_options, 30, 420);
    gtk_fixed_put (GTK_FIXED (fixed_options), button_default_options, 50, 420+20);
    gtk_widget_set_size_request (button_default_options, 140+8, 40);
    g_signal_connect ((gpointer) button_default_options, "clicked", G_CALLBACK (on_button_options_default_clicked), this);

    // image option
    frame_image_options = gtk_frame_new (NULL);
    gtk_widget_show (frame_image_options);
    // gtk_fixed_put (GTK_FIXED (fixed_options), frame_image_options, 30, 310);
#ifdef CT_322
  gtk_fixed_put (GTK_FIXED (fixed_options), frame_image_options, 50, 330);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_image_options, 50, 310);
#endif

    //gtk_widget_set_size_request (frame_image_options, 320, 100);
    gtk_widget_set_size_request (frame_image_options, 320, 100);
    gtk_frame_set_label_align (GTK_FRAME (frame_image_options), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_image_options), GTK_SHADOW_IN);

    label_image_options = gtk_label_new (_("<b>Image Options</b>"));
    gtk_widget_show (label_image_options);
    gtk_frame_set_label_widget (GTK_FRAME (frame_image_options), label_image_options);
    gtk_label_set_use_markup (GTK_LABEL (label_image_options), TRUE);

    SysGeneralSetting sysGS;
    if(ES == sysGS.GetLanguage())
        table_img_options = gtk_table_new (3, 2, FALSE);
    else
        table_img_options = gtk_table_new (3, 2, TRUE);
    gtk_widget_show (table_img_options);
    gtk_container_add (GTK_CONTAINER (frame_image_options), table_img_options);
    gtk_container_set_border_width (GTK_CONTAINER (table_img_options), 10);
    gtk_table_set_row_spacings (GTK_TABLE (table_img_options), 5);

    m_combobox_img_format = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_img_format);
    gtk_table_attach (GTK_TABLE (table_img_options), m_combobox_img_format, 1, 2, 0, 1,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_format), _("BMP"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_format), _("JPEG"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_format), _("EMP"));

#if 0
    m_combobox_img_medium = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_img_medium);
    gtk_table_attach (GTK_TABLE (table_img_options), m_combobox_img_medium, 1, 2, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_medium), _("Hard disk"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_medium), _("U disk"));
#endif

    m_combobox_img_file = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_img_file);
    gtk_table_attach (GTK_TABLE (table_img_options), m_combobox_img_file, 1, 2, 1, 2,
            (GtkAttachOptions) (GTK_EXPAND|GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_file), _("Manual"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_img_file), _("Auto"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_file), 0);

    label_img_file = gtk_label_new (_("<b>File name:</b>"));
    gtk_widget_show (label_img_file);
    gtk_table_attach (GTK_TABLE (table_img_options), label_img_file, 0, 1, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_img_file), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_img_file), 0, 0.5);

#if 0
    label_img_medium = gtk_label_new (_("<b>Storage medium:</b>"));
    gtk_widget_show (label_img_medium);
    gtk_table_attach (GTK_TABLE (table_img_options), label_img_medium, 0, 1, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_img_medium), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_img_medium), 0, 0.5);
#endif

    label_img_format = gtk_label_new (_("<b>Storage format: </b>"));
    gtk_widget_show (label_img_format);
    gtk_table_attach (GTK_TABLE (table_img_options), label_img_format, 0, 1, 0, 1,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_img_format), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_img_format), 0, 0.5);

    // video option
    frame_video_options = gtk_frame_new (NULL);
    gtk_widget_show (frame_video_options);
    // gtk_fixed_put (GTK_FIXED (fixed_options), frame_video_options, 390, 310);
#ifdef CT_322
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_video_options, 450, 330);
    gtk_widget_set_size_request (frame_video_options, 320, 100);
#else
    gtk_fixed_put (GTK_FIXED (fixed_options), frame_video_options, 450, 310);
    gtk_widget_set_size_request (frame_video_options, 320-10, 100);
#endif
    //gtk_widget_set_size_request (frame_video_options, 320, 100);
    gtk_frame_set_label_align (GTK_FRAME (frame_video_options), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_video_options), GTK_SHADOW_IN);

    label_video_options = gtk_label_new (_("<b>Video Options</b>"));
    gtk_widget_show (label_video_options);
    gtk_frame_set_label_widget (GTK_FRAME (frame_video_options), label_video_options);
    gtk_label_set_use_markup (GTK_LABEL (label_video_options), TRUE);

    if(ES ==sysGS.GetLanguage())
        table_video_options = gtk_table_new (3, 2, FALSE);
    else
        table_video_options = gtk_table_new (3, 2, TRUE);
    gtk_widget_show (table_video_options);
    gtk_container_add (GTK_CONTAINER (frame_video_options), table_video_options);
    gtk_container_set_border_width (GTK_CONTAINER (table_video_options), 10);
    gtk_table_set_row_spacings (GTK_TABLE (table_video_options), 5);

    m_combobox_video_format = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_video_format);
    gtk_table_attach (GTK_TABLE (table_video_options), m_combobox_video_format, 1, 2, 0, 1,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video_format), _("AVI"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video_format), _("CINE"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_format), 1);

#if 0
    m_combobox_video_medium = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_video_medium);
    gtk_table_attach (GTK_TABLE (table_video_options), m_combobox_video_medium, 1, 2, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video_medium), _("Hard disk"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video_medium), _("U disk"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_medium), 1);
#endif

    label_video_file = gtk_label_new (_("<b>File name:</b>"));
    gtk_widget_show (label_video_file);
    gtk_table_attach (GTK_TABLE (table_video_options), label_video_file, 0, 1, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_video_file), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_video_file), 0, 0.5);

#if 0
    label_video_medium = gtk_label_new (_("<b>Storage medium:</b>"));
    gtk_widget_show (label_video_medium);
    gtk_table_attach (GTK_TABLE (table_video_options), label_video_medium, 0, 1, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_video_medium), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_video_medium), 0, 0.5);
#endif

    label_video_format = gtk_label_new (_("<b>Storage format: </b>"));
    gtk_widget_show (label_video_format);
    gtk_table_attach (GTK_TABLE (table_video_options), label_video_format, 0, 1, 0, 1,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_video_format), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_video_format), 0, 0.5);

    m_combobox_video_file = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_video_file);
    gtk_table_attach (GTK_TABLE (table_video_options), m_combobox_video_file, 1, 2, 1, 2,
            (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video_file), _("Manual"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video_file), _("Auto"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_file), 0);

    return fixed_options;
}

void ViewSystem::init_option_setting(SysOptions* sysOptions)
{
    if (sysOptions == NULL)
        sysOptions = new SysOptions;

    // int index_biopsy_display = sysOptions->GetBiopsyLine();
    // if (index_biopsy_display == 0)
    //  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_biopsy_on), TRUE);
    // else
    //  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_biopsy_off), TRUE);

    int index_center_display = sysOptions->GetCenterLine();
    if (index_center_display == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_center_on), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_center_off), TRUE);

    int index_TI_type = sysOptions->GetTI();
    if (index_TI_type == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_TIC), TRUE);
    else if (index_TI_type == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_TIB), TRUE);
    else if (index_TI_type == 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_TIS), TRUE);

    int index_keyboard_sound = sysOptions->GetKeyboardSound();
    if (index_keyboard_sound == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_kb_sound_on), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_kb_sound_off), TRUE);

    int index_display_format = 0;
    index_display_format = sysOptions->GetDisplayFormatM();
//  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobtn_bm), TRUE);
    m_bmIndex = index_display_format;
    m_bpwIndex = sysOptions->GetDisplayFormatPW();

    if (index_display_format == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_total), TRUE);
    else if (index_display_format == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_11), TRUE);
    else if (index_display_format == 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_21), TRUE);
    else if (index_display_format == 3)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_12), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_lr_11), TRUE);

    if ( m_bpwIndex == 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_total_pw), TRUE);
    else if ( m_bpwIndex == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_11_pw), TRUE);
    else if (m_bpwIndex == 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_21_pw), TRUE);
    else if (m_bpwIndex == 3)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_ud_12_pw), TRUE);
    else
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobtn_lr_11_pw), TRUE);

    int index_img_format = sysOptions->GetImageFormat();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_format), index_img_format);
    //    int index_img_medium = sysOptions->GetImageMedia();
    //    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_medium), index_img_medium);
    int index_img_file = sysOptions->GetImageAutoName();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_img_file), index_img_file);
    int index_video_format = sysOptions->GetCineFormat();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_format), index_video_format);
    //    int index_video_medium = sysOptions->GetCineMedia();
    //    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_medium), index_video_medium);
    int index_video_file = sysOptions->GetCineAutoName();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video_file), index_video_file);

    int mouse_speed = sysOptions->GetMouseSpeed();
    if (mouse_speed <= 0)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_high), TRUE);
    else if (mouse_speed == 1)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_middle), TRUE);
    else if (mouse_speed >= 2)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_low), TRUE);

    delete sysOptions;
}

void ViewSystem::save_option_setting(void)
{
    SysOptions sysOptions;

    // unsigned char biopsyIndex;
    // if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_biopsy_on)))
    //  biopsyIndex = 0;
    // else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_biopsy_off)))
    //  biopsyIndex = 1;
    // sysOptions.SetBiopsyLine(biopsyIndex);

    unsigned char centerLineIndex = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_center_on)))
        centerLineIndex = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_center_off)))
        centerLineIndex = 1;
    sysOptions.SetCenterLine(centerLineIndex);

    unsigned char TIIndex = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_TIC)))
        TIIndex = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_TIB)))
        TIIndex = 1;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_TIS)))
        TIIndex = 2;
    sysOptions.SetTI(TIIndex);

    unsigned char kbSoundIndex = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_kb_sound_on)))
    {
        kbSoundIndex = 0;
    }
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_kb_sound_off)))
        kbSoundIndex = 1;
    sysOptions.SetKeyboardSound(kbSoundIndex);
    if (kbSoundIndex)
    {
        KeyboardSound(false);
    }
    else
    {
        KeyboardSound(true);
    }
    int mouseSpeed;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_low)))
        mouseSpeed = 2;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_middle)))
        mouseSpeed = 1;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_mouse_speed_high)))
        mouseSpeed = 0;
    sysOptions.SetMouseSpeed(mouseSpeed);
    g_keyInterface.SetMouseSpeed(mouseSpeed);

    unsigned char m_bmIndex = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_total)))
    {
        m_bmIndex = 0;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_ud_11)))
    {
        m_bmIndex = 1;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_ud_21)))
    {
        m_bmIndex = 2;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_ud_12)))
    {
        m_bmIndex = 3;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_lr_11)))
    {
        m_bmIndex = 4;
    }

    sysOptions.SetDisplayFormatM(m_bmIndex);

    unsigned char m_bpwIndex = 0;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_total_pw)))
    {
        m_bpwIndex = 0;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_ud_11_pw)))
    {
        m_bpwIndex = 1;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_ud_21_pw)))
    {
        m_bpwIndex = 2;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_ud_12_pw)))
    {
        m_bpwIndex = 3;
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobtn_lr_11_pw)))
    {
        m_bpwIndex = 4;
    }
    sysOptions.SetDisplayFormatPW(m_bpwIndex);

    unsigned char imgFormatIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_img_format));
    sysOptions.SetImageFormat(imgFormatIndex);
    //    unsigned char imgMediumIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_img_medium));
    //    sysOptions.SetImageMedia(imgMediumIndex);
    unsigned char imgFileIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_img_file));
    sysOptions.SetImageAutoName(imgFileIndex);
    unsigned char videoFormatIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video_format));
    sysOptions.SetCineFormat(videoFormatIndex);
    //    unsigned char videoMediumIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video_medium));
    //    sysOptions.SetCineMedia(videoMediumIndex);
    unsigned char videoFileIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video_file));
    sysOptions.SetCineAutoName(videoFileIndex);

    sysOptions.SyncFile();

    ImgMan::GetInstance()->SetImgFormat(imgFormatIndex);
    //ImgMan::GetInstance()->SetImgStorage(imgMediumIndex);
    ImgMan::GetInstance()->SetImgNameMode(imgFileIndex);
    VideoMan::GetInstance()->SetVideoFormat(videoFormatIndex);
    //VideoMan::GetInstance()->SetVideoStorage(videoMediumIndex);
    VideoMan::GetInstance()->SetVideoNameMode(videoFileIndex);
}

GtkWidget* ViewSystem::create_note_image(void)
{
    GtkWidget *fixed_image;
    GtkWidget *label_probe_type;
    // GtkWidget *combobox_probe_type;
    GtkWidget *scrolledwindow_exam_type;
    // GtkWidget *treeview_exam_type;

    GtkWidget *button_delete_user;
    GtkWidget *label_delete;
    GtkWidget *img_delete;
    GtkWidget *m_image_button_add;
    GtkWidget *m_image_button_delete;
#ifndef VET
    GtkWidget *button_edit_dept;
    GtkWidget *button_return_dept;
    GtkWidget *button_get_current;
    GtkWidget *button_default_image;
#endif

    GtkWidget *button_import_from_USB;
    GtkWidget *button_export_to_USB;
    GtkWidget *label_user_select;
    GtkWidget *scrolledwindow_img_set;
    GtkWidget *viewport_img_set;
    GtkWidget *fixed_img_set;
    GtkWidget *frame_comment;
    GtkWidget *fixed_comment;
    GtkWidget *table_comment;
    GtkWidget *label_mbp;
    // GtkWidget *combobox_mbp;
    GtkWidget *label_ao_power;
    // GtkWidget *combobox_ao_power;
    // GtkWidget *label_note;
    // GtkWidget *combobox_note;
    // GtkWidget *label_bdmk;
    // GtkWidget *combobox_bdmk;
    GtkWidget *label_comment;
    GtkWidget *frame_2d_m_mode;
    GtkWidget *fixed_2d_m_mode;
    GtkWidget *table_2d_m_mode;
    GtkWidget *hseparator1;
    GtkObject *spinbutton_2d_gain_adj;
    // GtkWidget *spinbutton_2d_gain;
    GtkWidget *label_focus_sum;
    GtkWidget *label_steer;
    GtkWidget *label_agc;
    // GtkWidget *combobox_agc;
    GtkWidget *label_edge_enh;
    GtkWidget *label_chroma;
    // GtkWidget *combobox_edge_enh;
    // GtkWidget *combobox_steer;
    // GtkWidget *combobox_focus_sum;
    // GtkWidget *combobox_focus_pos;
    // GtkWidget *combobox_depth;
    GtkWidget *label_focus_pos;
    GtkWidget *label_depth;
    // GtkWidget *combobox_chroma;
    GtkWidget *label_lr;
    GtkWidget *label_ud;
    // GtkWidget *combobox_lr;
    // GtkWidget *combobox_ud;
    GtkWidget *label_polarity;
    // GtkWidget *combobox_rotation;
    // GtkWidget *combobox_polarity;
    // GtkWidget *label_rotation;
    GtkWidget *label_line_aver;
    // GtkWidget *combobox_frame;
    GtkWidget *label_frame;
    // GtkWidget *combobox_line_aver;
    GtkWidget *label_restric;
    // GtkWidget *combobox_restric;
    // GtkWidget *combobox_thi;
    GtkWidget *label_tsi;
    // GtkWidget *combobox_tsi;
    GtkWidget *label_imgEnh;
    GtkWidget *label_m_gain;
    GtkObject *spinbutton_m_gain_adj;
    // GtkWidget *spinbutton_m_gain;
    GtkWidget *label_thi;
    GtkWidget *hseparator2;
    GtkWidget *label_scan_range;
    // GtkWidget *combobox_scan_range;
    GtkWidget *label_dynamic_range;
    // GtkWidget *combobox_dynamic_range;
    GtkWidget *label_2d_freq;
    // GtkWidget *combobox_2d_freq;
    GtkWidget *label_2d_gain;
    GtkWidget *label_2d_line_density;
    // GtkWidget *combobox_2d_line_density;
    GtkWidget *label_2d_smooth;
    // GtkWidget *combobox_2d_smooth;
    GtkWidget *label_2d_m_mode;
    GtkWidget *label_gray_transform;
    GtkWidget *label_space_compound;
    GtkWidget *label_freq_compound;
    GtkWidget *label_thi_freq;
#if not defined(CT_322)
#if not defined(CT_313)
    GtkWidget *frame_pw_mode;
    GtkWidget *fixed_pw_mode;
    GtkWidget *table_pw_mode;
    GtkWidget *label_pw_gain;
    GtkObject *spinbutton_pw_gain_adj;
    // GtkWidget *spinbutton_pw_gain;
    GtkWidget *label_pw_scale_prf;

    // GtkWidget *combobox_pw_scale_prf;
    // GtkWidget *combobox_time_resolution;
    GtkWidget *label_time_resolution;
    GtkWidget *hseparator3;
    // GtkWidget *combobox_pw_freq;

    GtkWidget *label_pw_freq;
    GtkWidget *label_pw_wallfilter;
    GtkWidget *label_pw_angle;
    GtkObject *spinbutton_pw_angle_adj;
    // GtkWidget *combobox_pw_wallfilter;
    GtkWidget *label_pw_invert;
    // GtkWidget *combobox_pw_invert;
    GtkWidget *label_pw_mode;
    GtkWidget *frame_cfm_mode;
    GtkWidget *fixed_cfm_mode;
    GtkWidget *table_cfm_mode;
    GtkWidget *label_color_gain;
    GtkWidget *label_cfm_scale_prf;
    GtkWidget *label_cfm_wallfilter;
    GtkWidget *label_cfm_line_density;
    // GtkWidget *combobox_cfm_line_density;
    // GtkWidget *combobox_cfm_scale_prf;
    GtkObject *spinbutton_cfm_gain_adj;
    // GtkWidget *spinbutton_cfm_gain;
    // GtkWidget *combobox_cfm_wallfilter;

    GtkWidget *label_sensitivity;
    GtkWidget *label_variance;
    // GtkWidget *combobox_variance;
    GtkWidget *label_cfm_invert;
    // GtkWidget *combobox_cfm_invert;
    GtkWidget *label_persistence;
    // GtkWidget *combobox_persistence;
    GtkWidget *label_cfm_smooth;
    // GtkWidget *combobox_cfm_smooth;
    // GtkWidget *combobox_color_rejection;
    GtkWidget *label_color_rejection;
    // GtkWidget *label_pdi_gain;
    // GtkObject *spinbutton_pdi_gain_adj;
    // GtkWidget *spinbutton_pdi_gain;
    // GtkWidget *combobox_sensitivity;
    GtkWidget *hseparator4;
    GtkWidget *label_cfm_mode;
#endif
#endif
    GtkWidget *bin_entry_dia;

    SysGeneralSetting sysGS;

    int i;
    char buf[50];

    fixed_image = gtk_fixed_new ();
    gtk_widget_show (fixed_image);

    //user select
    label_user_select = gtk_label_new (_("<b>User Select: </b>"));

    gtk_misc_set_alignment (GTK_MISC(label_user_select), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_user_select), TRUE);
    gtk_widget_show (label_user_select);
    gtk_fixed_put (GTK_FIXED (fixed_image), label_user_select, 340+20, 10+5);
    //gtk_fixed_put (GTK_FIXED (fixed_image), label_user_select, 10+20, 10);
    gtk_widget_set_size_request (label_user_select, 100, 30);

    m_comboboxentry_user_select = gtk_combo_box_entry_new_text ();
    gtk_widget_show (m_comboboxentry_user_select);
    gtk_fixed_put (GTK_FIXED (fixed_image), m_comboboxentry_user_select, 330+110+20, 10+5);
    //gtk_fixed_put (GTK_FIXED (fixed_image), m_comboboxentry_user_select, 110+20, 10);
    gtk_widget_set_size_request (m_comboboxentry_user_select, 120+100, 30);
    bin_entry_dia = gtk_bin_get_child (GTK_BIN(m_comboboxentry_user_select));
    gtk_entry_set_max_length(GTK_ENTRY(bin_entry_dia), 40); //25 in order to display Умолчан системы
    g_signal_connect(G_OBJECT(bin_entry_dia), "insert_text", G_CALLBACK(on_entry_name_insert), this);
    g_signal_connect(G_OBJECT(bin_entry_dia), "focus-out-event", G_CALLBACK(HandleUserSelectFocusOut), this);

    img_delete = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
    button_delete_user = create_button_icon_only(img_delete, 0, 0, NULL);
    gtk_widget_show (button_delete_user);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_delete_user, 100+330+220+40, 10+5);
    //gtk_fixed_put (GTK_FIXED (fixed_image), button_delete_user, 220+40, 10);
    gtk_widget_set_size_request (button_delete_user, 30, 30);
    g_signal_connect(button_delete_user, "clicked", G_CALLBACK(HandleImageBtnDeleteUser), this);

    UserSelect::GetInstance()->read_default_username(m_comboboxentry_user_select);
    UserSelect::GetInstance()->read_username_db(USERNAME_DB, m_comboboxentry_user_select);
    int num = UserSelect::GetInstance()->get_active_user();
    UserSelect::GetInstance()->set_active_user(m_comboboxentry_user_select, num);

    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    int index_username = exam.ReadDefaultUserIndex(&ini);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_user_select), index_username);
    g_signal_connect(GTK_COMBO_BOX(m_comboboxentry_user_select), "changed", G_CALLBACK(HandleUserChanged), this);

    string default_probe;
    default_probe=exam.ReadDefaultProbe(&ini);
    for (i = 0; i < NUM_PROBE; i++)
    {
        if(strcmp(default_probe.c_str(), PROBE_LIST[i].c_str())==0)
        {
            save_itemIndex(i);
            break;
        }
    }

    // probe type
    label_probe_type = gtk_label_new (_("<b>Probe Type: </b>"));
    gtk_misc_set_alignment (GTK_MISC(label_probe_type), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_probe_type), TRUE);
    gtk_widget_show (label_probe_type);
    gtk_fixed_put (GTK_FIXED (fixed_image), label_probe_type, 10+20, 10);
    //gtk_fixed_put (GTK_FIXED (fixed_image), label_probe_type, 10+20, 40+10);
    //gtk_fixed_put (GTK_FIXED (fixed_image), label_probe_type, 30, 40);
    gtk_widget_set_size_request (label_probe_type, 100+25, 30);

    m_combobox_probe_type = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_probe_type);
    gtk_fixed_put (GTK_FIXED (fixed_image), m_combobox_probe_type, 110+20, 10);
    //gtk_fixed_put (GTK_FIXED (fixed_image), m_combobox_probe_type, 110+20, 40+10);
    gtk_widget_set_size_request (m_combobox_probe_type, 120+25+25, 30);
    for (i = 0; i < NUM_PROBE; i++) {
/*
        char newProbeName[256];
        ProbeMan::GetInstance()->VerifyProbeName(PROBE_LIST[i].c_str(), newProbeName);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_probe_type), newProbeName);
        */

        char path[100] = {"0"};
        sprintf(path, "%s/%s", CFG_RES_PATH, PROBE_ALIAS_FILE);
        IniFile ini(path);
        char alias[56] = {"0"};
        ProbeMan::GetInstance()->ReadProbeAlias(&ini, PROBE_LIST[i].c_str(), alias);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_probe_type), alias);
    }
#ifdef VET
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_type), 0);
#else
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_type), -1);
#endif
    g_signal_connect(m_combobox_probe_type, "changed", G_CALLBACK(HandleProbeTypeChanged), this);

    // exam type
    scrolledwindow_exam_type = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_exam_type);
#ifdef VET
     gtk_fixed_put (GTK_FIXED (fixed_image), scrolledwindow_exam_type, 10+20, 70+30-40);
#else
    gtk_fixed_put (GTK_FIXED (fixed_image), scrolledwindow_exam_type, 10+20, 70+10-40);
#endif
#ifdef VET
    gtk_widget_set_size_request (scrolledwindow_exam_type, 220, 345-50-60+40+40);
#else
    gtk_widget_set_size_request (scrolledwindow_exam_type, 220+50, 275+48+40+40+77-30);
#endif
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_exam_type), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
#ifdef VET
    ExamItem examItem;
    vector<int> itemIndex;
    examItem.GetItemListOfProbe((char*)PROBE_LIST[0].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
    GtkTreeModel *model = create_exam_item_model(itemIndex);
#else
    vector<int> test;
    GtkTreeModel *model = create_exam_item_model(test);
#endif
    m_treeview_exam_type = gtk_tree_view_new_with_model(model);

//#ifdef VET
  //  add_columns(GTK_TREE_VIEW(m_treeview_exam_type));
//#else
    add_exam_item_column(GTK_TREE_VIEW(m_treeview_exam_type));
//#endif
    gtk_container_add (GTK_CONTAINER (scrolledwindow_exam_type), m_treeview_exam_type);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
#ifdef VET
    GtkTreePath *path_tmp;
    if(m_str_index == NULL)
        path_tmp = gtk_tree_path_new_from_string("0");
    else
        path_tmp = gtk_tree_path_new_from_string(m_str_index);
    gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
#endif

#ifndef VET
    g_object_set(renderer, "editable", TRUE, NULL);
    g_signal_connect(renderer, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(renderer, "edited", G_CALLBACK(HandleCellRendererRename), this);
#endif
    g_signal_connect(select, "changed", G_CALLBACK(HandleExamTypeChanged), this);
    gtk_widget_add_events(GTK_WIDGET(m_treeview_exam_type), (gtk_widget_get_events(GTK_WIDGET(m_treeview_exam_type)) | GDK_BUTTON_RELEASE_MASK));
    g_signal_connect_after(m_treeview_exam_type, "button_release_event", G_CALLBACK(HandleExamDepartmentBtnClicked), this);
    //g_signal_connect(m_treeview_exam_type, "test-expand-row", G_CALLBACK(HandleExamTypeExpandBefore), this);
    gtk_widget_show (m_treeview_exam_type);
#if 0
    GtkTreeIter iter_new;
    gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
    GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_TreeviewReportSet), new_path);
#endif
       //GtkTreePath *path = gtk_tree_path_new_from_string();
#ifndef VET
    m_image_button_add = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
    m_button_add = create_button_icon_only(m_image_button_add, 0, 0, NULL);
    gtk_widget_hide (m_button_add);
    gtk_fixed_put (GTK_FIXED (fixed_image), m_button_add, 260, 50+30-2-40);
    gtk_widget_set_size_request (m_button_add, 30-5, 30-5);
    g_signal_connect(m_button_add, "clicked", G_CALLBACK(HandleAddItemClicked), this);

    m_image_button_delete = gtk_image_new_from_stock ("gtk-remove", GTK_ICON_SIZE_BUTTON);
    m_button_delete_item = create_button_icon_only(m_image_button_delete, 0, 0, NULL);
    gtk_widget_hide (m_button_delete_item);
    gtk_fixed_put (GTK_FIXED (fixed_image), m_button_delete_item, 295-10, 50+30-2-40);
    gtk_widget_set_size_request (m_button_delete_item, 30-5, 30-5);
    g_signal_connect(m_button_delete_item, "clicked", G_CALLBACK(HandleDeleteItemClicked), this);
#endif

#if 0
    button_edit_dept = gtk_button_new_with_mnemonic (_("Edit"));
    gtk_widget_show (button_edit_dept);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_edit_dept, 80, 345+20);//415);
    gtk_widget_set_size_request (button_edit_dept, 148, 40-10);
    g_signal_connect(button_edit_dept, "clicked", G_CALLBACK(HandleImageBtnEditDept), this);

    button_return_dept = gtk_button_new_with_mnemonic (_("Return"));
    gtk_widget_hide (button_return_dept);
    //gtk_fixed_put (GTK_FIXED (fixed_image), button_import_from_USB, 50, 355);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_return_dept, 80, 385+20);//415);
    gtk_widget_set_size_request (button_return_dept, 148, 40-10);
    g_signal_connect(button_return_dept, "clicked", G_CALLBACK(HandleImageBtnReturnDept), this);

    // get current
    button_get_current = gtk_button_new_with_mnemonic (_("Get Current"));
    gtk_widget_show (button_get_current);
    //gtk_fixed_put (GTK_FIXED (fixed_image), button_get_current, 42, 425);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_get_current, 80, 385+20);//465);
    gtk_widget_set_size_request (button_get_current, 136+12, 30);//+10);
    g_signal_connect(button_get_current, "clicked", G_CALLBACK(HandleImageBtnGetCurrent), this);

    // factory default
    button_default_image = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_default_image);
    //gtk_fixed_put (GTK_FIXED (fixed_image), button_default_image, 42, 460);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_default_image, 80, 425+20);//465);
    gtk_widget_set_size_request (button_default_image, 136+12, 30);//+10);
    g_signal_connect(button_default_image, "clicked", G_CALLBACK(HandleImageBtnDefault), this);

#else
    const int dir = 20;
    //new checkpart
    m_frame_new_check_part = gtk_frame_new (NULL);
    gtk_widget_hide (m_frame_new_check_part);
    gtk_fixed_put (GTK_FIXED (fixed_image), m_frame_new_check_part, 330+20+40, 70);//90
    gtk_widget_set_size_request (m_frame_new_check_part, 560-40-80, 155+40); //550
    gtk_frame_set_shadow_type (GTK_FRAME (m_frame_new_check_part), GTK_SHADOW_IN);
    GtkWidget *fixed_new_check_part = gtk_fixed_new ();
    gtk_widget_show (fixed_new_check_part);
    gtk_container_add (GTK_CONTAINER (m_frame_new_check_part), fixed_new_check_part);

    m_label_check_part = gtk_label_new (_("Exam Type:"));
    gtk_misc_set_alignment (GTK_MISC(m_label_check_part), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (m_label_check_part), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_check_part), PANGO_WRAP_WORD);
    gtk_widget_show (m_label_check_part);
    gtk_fixed_put (GTK_FIXED (fixed_new_check_part), m_label_check_part, 22+10+20, 10+5+20-5);
    //gtk_widget_set_size_request (m_label_check_part, 80, 30);
    gtk_widget_set_size_request(m_label_check_part, 338, 30);

    m_entry_new_check_part = gtk_entry_new ();
    gtk_widget_show (m_entry_new_check_part);
    gtk_fixed_put (GTK_FIXED (fixed_new_check_part), m_entry_new_check_part, 100+22+10+30, 10+5+20-5);
    gtk_widget_set_size_request (m_entry_new_check_part, 200, 30);
    //g_signal_connect(G_OBJECT(m_entry_new_check_part), "insert_text", G_CALLBACK(on_entry_new_check_part), this);
    //g_signal_connect(G_OBJECT(m_entry_new_check_part), "focus-out-event", G_CALLBACK(HandleNewCheckPartFocusOut), this);

    //确定
    GtkWidget *imageOK = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelOK = gtk_label_new_with_mnemonic (_("OK"));
    GtkWidget* button_ok = create_button_icon(labelOK, imageOK);
    gtk_widget_show (button_ok);
    gtk_fixed_put (GTK_FIXED (fixed_new_check_part), button_ok, 22+10+20, 120-50+30);
    g_signal_connect(button_ok, "clicked", G_CALLBACK(HandleNewCheckPartBtnOk), this);

    //取消
    GtkWidget *imageCancel = gtk_image_new_from_stock (GTK_STOCK_CANCEL, GTK_ICON_SIZE_BUTTON);
    GtkWidget *labelCancel = gtk_label_new_with_mnemonic (_("Cancel"));
    GtkWidget* button_cancel = create_button_icon(labelCancel, imageCancel);
    gtk_widget_show (button_cancel);
    gtk_fixed_put (GTK_FIXED (fixed_new_check_part), button_cancel, 520-22-136-12-20-80, 120-50+30);
    //gtk_widget_set_size_request (button_cancel, 136+12, 30+10);
    g_signal_connect(button_cancel, "clicked", G_CALLBACK(HandleNewCheckPartBtnCancel), this);

    // save
    button_get_current = gtk_button_new_with_mnemonic (_("Save"));
    gtk_widget_show (button_get_current);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_get_current, 340+20+10, 415-30-10-50-50-40+20+20+20);
    gtk_widget_set_size_request (button_get_current, 136+12, 30+10+10);
    g_signal_connect(button_get_current, "clicked", G_CALLBACK(HandleImageBtnSave), this);

    // new
    GtkWidget *button_new = gtk_button_new_with_mnemonic (_("New"));
    gtk_widget_show (button_new);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_new, 340+20+10+136+12+dir, 415-30-10-50-50-40+20+20+20);
    gtk_widget_set_size_request (button_new, 136+12, 30+10+10);
    g_signal_connect(button_new, "clicked", G_CALLBACK(HandleImageBtnNew), this);

    //delete
    GtkWidget *button_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_delete);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_delete, 340+20+10+136+12+dir+136+12+dir, 415-30-10-50-50-40+20+20+20);
    gtk_widget_set_size_request (button_delete, 136+12, 30+10+10);
    g_signal_connect(button_delete, "clicked", G_CALLBACK(HandleDeleteItemClicked), this);

    // factory default
    button_default_image = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_default_image);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_default_image, 340+20+10+136+12+dir+136+12+dir, 415+20+20);
    //gtk_fixed_put (GTK_FIXED (fixed_image), button_default_image, 340+20+10, 415);
    gtk_widget_set_size_request (button_default_image, 136+12, 30+10-10);
    g_signal_connect(button_default_image, "clicked", G_CALLBACK(HandleImageBtnDefault), this);
#if 0
   // export to USB
    button_export_to_USB = gtk_button_new_with_mnemonic (_("Export To USB"));
    gtk_widget_show (button_export_to_USB);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_export_to_USB, 340+20+10+136+12+dir, 415-85);
    gtk_widget_set_size_request (button_export_to_USB, 136+12, 30+10+10);
    g_signal_connect(button_export_to_USB, "clicked", G_CALLBACK(HandleImageBtnExportToUSB), this);

    // import from USB
    button_import_from_USB = gtk_button_new_with_mnemonic (_("Import From USB"));
    gtk_widget_show (button_import_from_USB);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_import_from_USB, 340+20+10+136+12+dir+136+12+dir, 415-85);
    gtk_widget_set_size_request (button_import_from_USB, 136+12, 30+10+10);
    g_signal_connect(button_import_from_USB, "clicked", G_CALLBACK(HandleImageBtnImportFromUSB), this);
#else
    // export to USB
    button_export_to_USB = gtk_button_new_with_mnemonic (_("Export To USB"));
    gtk_widget_show (button_export_to_USB);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_export_to_USB, 340+20+10, 415-80+20+20);
    gtk_widget_set_size_request (button_export_to_USB, 232, 30+10+10);
    g_signal_connect(button_export_to_USB, "clicked", G_CALLBACK(HandleImageBtnExportToUSB), this);

    // import from USB
    button_import_from_USB = gtk_button_new_with_mnemonic (_("Import From USB"));
    gtk_widget_show (button_import_from_USB);
    gtk_fixed_put (GTK_FIXED (fixed_image), button_import_from_USB, 340+20+10+232+dir, 415-80+20+20);
    gtk_widget_set_size_request (button_import_from_USB, 232, 30+10+10);
    g_signal_connect(button_import_from_USB, "clicked", G_CALLBACK(HandleImageBtnImportFromUSB), this);
#endif
#endif

    // scroll window
    scrolledwindow_img_set = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_img_set);
    gtk_fixed_put (GTK_FIXED (fixed_image), scrolledwindow_img_set, 300+30, 10);
    gtk_widget_set_size_request (scrolledwindow_img_set, 560, 480);
    //gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_img_set), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_img_set), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_img_set), GTK_SHADOW_OUT);

    viewport_img_set = gtk_viewport_new (NULL, NULL);
    gtk_widget_hide (viewport_img_set);
    //gtk_widget_show (viewport_img_set);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_img_set), viewport_img_set);

    fixed_img_set = gtk_fixed_new ();
    gtk_widget_show (fixed_img_set);
    gtk_container_add (GTK_CONTAINER (viewport_img_set), fixed_img_set);

    // comment
    frame_comment = gtk_frame_new (NULL);
    gtk_widget_show (frame_comment);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_comment, 5, 5);
    gtk_widget_set_size_request (frame_comment, 520, 70);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_comment), GTK_SHADOW_OUT);

    label_comment = gtk_label_new (_("<b>Common</b>"));
    gtk_widget_show (label_comment);
    gtk_frame_set_label_widget (GTK_FRAME (frame_comment), label_comment);
    gtk_label_set_use_markup (GTK_LABEL (label_comment), TRUE);

    fixed_comment = gtk_fixed_new ();
    gtk_widget_show (fixed_comment);
    gtk_container_add (GTK_CONTAINER (frame_comment), fixed_comment);

    table_comment = gtk_table_new (1, 4, FALSE);
    gtk_widget_show (table_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), table_comment, 20, 5);
    gtk_widget_set_size_request (table_comment, 480, 30);
    gtk_table_set_row_spacings (GTK_TABLE(table_comment), 10);

    // MBP
    label_mbp = gtk_label_new (_("MBP :"));
    gtk_widget_show (label_mbp);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), label_mbp, 5, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), label_mbp, 0, 1, 0, 1);
    gtk_widget_set_size_request (label_mbp, -1, 30);

    m_combobox_mbp = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_mbp);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_mbp, 105, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), m_combobox_mbp, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_combobox_mbp, 100, 30);
    for (i = 0; i < Img2D::MAX_MBP_INDEX; i ++) {
        sprintf(buf, "%d", Img2D::MBP[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_mbp), buf);
    }

    label_ao_power = gtk_label_new (_("AO / Power :"));
    gtk_widget_show (label_ao_power);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), label_ao_power, 270, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), label_ao_power, 2, 3, 0, 1);
    gtk_widget_set_size_request (label_ao_power, -1, 30);

    m_combobox_ao_power = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ao_power);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_ao_power, 370, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_comment), m_combobox_ao_power, 3, 4, 0, 1);
    gtk_widget_set_size_request (m_combobox_ao_power, 100, 30);
    for (i = 0; i < Img2D::MAX_POWER_INDEX; i ++) {
        sprintf(buf, "%d", Img2D::POWER_DATA[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ao_power), buf);
    }

    // label_note = gtk_label_new (_("Note :"));
    // gtk_widget_show (label_note);
    // gtk_fixed_put (GTK_FIXED (fixed_comment), label_note, 5, 50);
    // gtk_widget_set_size_request (label_note, 100, 30);

    // combobox_note = gtk_combo_box_new_text ();
    // gtk_widget_show (combobox_note);
    // gtk_fixed_put (GTK_FIXED (fixed_comment), combobox_note, 105, 50);
    // gtk_widget_set_size_request (combobox_note, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_note), _("test"));

    // label_bdmk = gtk_label_new (_("Body Mark:"));
    // gtk_widget_show (label_bdmk);
    // gtk_fixed_put (GTK_FIXED (fixed_comment), label_bdmk, 250, 50);
    // gtk_widget_set_size_request (label_bdmk, 100, 30);

    // combobox_bdmk = gtk_combo_box_new_text ();
    // gtk_widget_show (combobox_bdmk);
    // gtk_fixed_put (GTK_FIXED (fixed_comment), combobox_bdmk, 350, 50);
    // gtk_widget_set_size_request (combobox_bdmk, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (combobox_bdmk), _("test"));

    // 2d and m mode
    frame_2d_m_mode = gtk_frame_new (NULL);
    gtk_widget_show (frame_2d_m_mode);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_2d_m_mode, 5, 90);//90
    gtk_widget_set_size_request (frame_2d_m_mode, 520, 605); //550
    //gtk_widget_set_size_request (frame_2d_m_mode, 520, 605 - 40); //550
    gtk_frame_set_shadow_type (GTK_FRAME (frame_2d_m_mode), GTK_SHADOW_OUT);

    fixed_2d_m_mode = gtk_fixed_new ();
    gtk_widget_show (fixed_2d_m_mode);
    gtk_container_add (GTK_CONTAINER (frame_2d_m_mode), fixed_2d_m_mode);

    label_2d_m_mode = gtk_label_new (_("<b>2D and M mode</b>"));
    gtk_widget_show (label_2d_m_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_2d_m_mode), label_2d_m_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_2d_m_mode), TRUE);

    table_2d_m_mode = gtk_table_new (16, 4, FALSE); //14
    //table_2d_m_mode = gtk_table_new (15, 4, FALSE); //14
    gtk_widget_show (table_2d_m_mode);
    gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), table_2d_m_mode, 20, 5);
    gtk_widget_set_size_request (table_2d_m_mode, 480, 520);//520
    gtk_table_set_row_spacings (GTK_TABLE(table_2d_m_mode), 10);

    // 2D gain
    label_2d_gain = gtk_label_new (_("2D Gain:"));
    gtk_widget_show (label_2d_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_gain, 5, 10);
    gtk_widget_set_size_request (label_2d_gain, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_gain, 0, 1, 0, 1);

    spinbutton_2d_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_2d_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_2d_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_2d_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_spinbutton_2d_gain, 105, 12);
    gtk_widget_set_size_request (m_spinbutton_2d_gain, -1, 27);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_spinbutton_2d_gain, 1, 2, 0, 1);
    g_signal_connect(G_OBJECT(m_spinbutton_2d_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    // 2D freq index
    label_2d_freq = gtk_label_new (_("Freq. :"));
    gtk_widget_show (label_2d_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_freq, 270, 10);
    gtk_widget_set_size_request (label_2d_freq, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_freq, 2, 3, 0, 1);

    m_combobox_2d_freq = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_2d_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_2d_freq, 370, 10);
    gtk_widget_set_size_request (m_combobox_2d_freq, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_2d_freq, 3, 4, 0, 1);

    // focus sum
    label_focus_sum = gtk_label_new (_("Focus sum:"));
    gtk_widget_show (label_focus_sum);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_focus_sum, 5, 50);
    gtk_widget_set_size_request (label_focus_sum, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_focus_sum , 0, 1, 1, 2);

    m_combobox_focus_sum = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_focus_sum);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_focus_sum, 105, 50);
    gtk_widget_set_size_request (m_combobox_focus_sum, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_focus_sum, 1, 2, 1, 2);
    for (i = 1; i <= Img2D::MAX_FOCUS; i ++) {
        sprintf(buf, "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_focus_sum), buf);
    }
    g_signal_connect (G_OBJECT(m_combobox_focus_sum), "changed", G_CALLBACK (HandleComboFocusSum), this);

    // focus position
    label_focus_pos = gtk_label_new (_("Focus Pos. Index:"));
    gtk_widget_show (label_focus_pos);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_focus_pos, 260, 50);
    gtk_widget_set_size_request (label_focus_pos, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_focus_pos, 2, 3, 1, 2);

    m_combobox_focus_pos = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_focus_pos);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_focus_pos, 370, 50);
    gtk_widget_set_size_request (m_combobox_focus_pos, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_focus_pos, 3, 4, 1, 2);

    // scan range
    label_scan_range = gtk_label_new (_("Scan Range:"));
    gtk_widget_show (label_scan_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_scan_range, 5, 90);
    gtk_widget_set_size_request (label_scan_range, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_scan_range, 0, 1, 2, 3);

    m_combobox_scan_range = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_scan_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_scan_range, 105, 90);
    gtk_widget_set_size_request (m_combobox_scan_range, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_scan_range, 1, 2, 2, 3);
    for (i = 0; i < Img2D::MAX_ANGLE; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_scan_range), buf);
    }

    // dyanamic range
    label_dynamic_range = gtk_label_new (_("Dynamic Range:"));
    gtk_widget_show (label_dynamic_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_dynamic_range, 260, 90);
    gtk_widget_set_size_request (label_dynamic_range, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_dynamic_range, 2, 3, 2, 3);

    m_combobox_dynamic_range = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_dynamic_range);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_dynamic_range, 370, 90);
    gtk_widget_set_size_request (m_combobox_dynamic_range, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_dynamic_range, 3, 4, 2, 3);
    for (i = 0; i < Calc2D::MAX_DYNAMIC_INDEX; i ++)    {
        sprintf(buf , "%ddB", Img2D::DYNAMIC_DATA_D[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_dynamic_range), buf);
    }

    // line density
    label_2d_line_density = gtk_label_new (_("Line Density:"));
    gtk_widget_show (label_2d_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_line_density, 5, 130);
    gtk_widget_set_size_request (label_2d_line_density, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_line_density, 0, 1, 3, 4);

    m_combobox_2d_line_density = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_2d_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_2d_line_density, 105, 130);
    gtk_widget_set_size_request (m_combobox_2d_line_density, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_2d_line_density, 1, 2, 3, 4);
    for (i = 0; i < Img2D::MAX_LINE_DENSITY; i ++) {
        sprintf(buf , "%s", _(Img2D::LINE_DENSITY_DISPLAY[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_2d_line_density), buf);
    }

    // depth
    label_depth = gtk_label_new (_("Depth Scale:"));
    gtk_widget_show (label_depth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_depth, 270, 130);
    gtk_widget_set_size_request (label_depth, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_depth, 2, 3, 3, 4);

    m_combobox_depth = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_depth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_depth, 370, 130);
    gtk_widget_set_size_request (m_combobox_depth, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_depth, 3, 4, 3, 4);
    for (i = 0; i < Img2D::MAX_SCALE_INDEX; i ++) {
        sprintf(buf, "%.1f", (float)Img2D::IMG_SCALE[i] / 10);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_depth), buf);
    }

    // steer
    label_steer = gtk_label_new (_("Steer:"));
    gtk_widget_show (label_steer);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_steer, 5, 170);
    gtk_widget_set_size_request (label_steer, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_steer, 0, 1, 4, 5);

    m_combobox_steer = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_steer);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_steer, 105, 170);
    gtk_widget_set_size_request (m_combobox_steer, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_steer, 1, 2, 4, 5);
    for (i = 0; i < Img2D::MAX_STEER; i ++) {
        sprintf(buf, "%d°", Img2D::STEER_ANGLE[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_steer), buf);
    }

    // AGC
    label_agc = gtk_label_new (_("AGC:"));
    gtk_widget_show (label_agc);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_agc, 270, 170);
    gtk_widget_set_size_request (label_agc, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_agc, 2, 3, 4, 5);

    m_combobox_agc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_agc);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_agc, 370, 170);
    gtk_widget_set_size_request (m_combobox_agc, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_agc, 3, 4, 4, 5);
    for (i = 0; i < Img2D::MAX_AGC; i++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_agc), buf);
    }

    // edge enhancement
    label_edge_enh = gtk_label_new (_("Edge enh. :"));
    gtk_widget_show (label_edge_enh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_edge_enh, 5, 210);
    gtk_widget_set_size_request (label_edge_enh, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_edge_enh, 0, 1, 5, 6);

    m_combobox_edge_enh = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_edge_enh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_edge_enh, 105, 210);
    gtk_widget_set_size_request (m_combobox_edge_enh, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_edge_enh, 1, 2, 5, 6);
    for (i = 0; i < Img2D::MAX_EDGE; i++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_edge_enh), buf);
    }

    // chroma
    label_chroma = gtk_label_new (_("Chroma:"));
    gtk_widget_show (label_chroma);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_chroma, 270, 210);
    gtk_widget_set_size_request (label_chroma, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_chroma, 2, 3, 5, 6);

    m_combobox_chroma = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_chroma);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_chroma, 370, 210);
    gtk_widget_set_size_request (m_combobox_chroma, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_chroma, 3, 4, 5, 6);
    for (i = 0; i < ImgProc2D::MAX_CHROMA; i++) {
        sprintf(buf , "%s", _(ImgProc2D::CHROMA_TYPE[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_chroma), buf);
    }

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), hseparator1, 16, 250);
    //gtk_widget_set_size_request (hseparator1, 472, 16);
    gtk_table_attach (GTK_TABLE (table_2d_m_mode), hseparator1, 0, 4, 6, 7,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_SHRINK), 0, 0);
    gtk_widget_set_size_request (hseparator1, -1, 5);

    // left-right
    label_lr = gtk_label_new (_("L / R:"));
    gtk_widget_show (label_lr);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_lr, 5, 270);
    gtk_widget_set_size_request (label_lr, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_lr, 0, 1, 7, 8);

    m_combobox_lr = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_lr);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_lr, 105, 270);
    gtk_widget_set_size_request (m_combobox_lr, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_lr, 1, 2, 7, 8);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_lr), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_lr), _("ON"));

    // up-down
    label_ud = gtk_label_new (_("U / D:"));
    gtk_widget_show (label_ud);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_ud, 270, 270);
    gtk_widget_set_size_request (label_ud, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_ud, 2, 3, 7, 8);

    m_combobox_ud = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ud);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_ud, 370, 270);
    gtk_widget_set_size_request (m_combobox_ud, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_ud, 3, 4, 7, 8);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ud), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ud), _("ON"));

    // polarity
    label_polarity = gtk_label_new (_("Polarity:"));
    gtk_widget_show (label_polarity);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_polarity, 5, 310);
    gtk_widget_set_size_request (label_polarity, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_polarity, 0, 1, 8, 9);

    m_combobox_polarity = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_polarity);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_polarity, 105, 310);
    gtk_widget_set_size_request (m_combobox_polarity, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_polarity, 1, 2, 8, 9);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_polarity), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_polarity), _("ON"));

#if 0
    // rotate
    label_rotation = gtk_label_new (_("Rotation:"));
    gtk_widget_show (label_rotation);
    gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_rotation, 270, 310);
    gtk_widget_set_size_request (label_rotation, 100, 30);

    m_combobox_rotation = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_rotation);
    gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_rotation, 370, 310);
    gtk_widget_set_size_request (m_combobox_rotation, 100, 30);
    for (i = 0; i < ImgProc2D::MAX_ROTATE; i ++) {
        sprintf(buf , "%d°", ImgProc2D::Rotate[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_rotation), buf);
    }
#endif

    // frame aver
    label_frame = gtk_label_new (_("Frame aver. :"));
    gtk_widget_show (label_frame);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_frame, 5, 350);
    gtk_widget_set_size_request (label_frame, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_frame, 2, 3, 8, 9);
    m_combobox_frame = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_frame);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_frame, 105, 350);
    gtk_widget_set_size_request (m_combobox_frame, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_frame, 3, 4, 8, 9);
    for (i = 0; i < ImgProc2D::MAX_FRAME; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_frame), buf);
    }

    // line aver
    label_line_aver = gtk_label_new (_("Line aver. :"));
    gtk_widget_show (label_line_aver);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_line_aver, 270, 350);
    gtk_widget_set_size_request (label_line_aver, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_line_aver, 0, 1, 9, 10);

    m_combobox_line_aver = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_line_aver);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_line_aver, 370, 350);
    gtk_widget_set_size_request (m_combobox_line_aver, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_line_aver, 1, 2, 9, 10);
    for (i = 0; i < ImgProc2D::MAX_LINE; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_line_aver), buf);
    }

    // smooth
    label_2d_smooth = gtk_label_new (_("Smooth:"));
    gtk_widget_show (label_2d_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_2d_smooth, 5, 390);
    gtk_widget_set_size_request (label_2d_smooth, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_2d_smooth, 2, 3, 9, 10);

    m_combobox_2d_smooth = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_2d_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_2d_smooth, 105, 390);
    gtk_widget_set_size_request (m_combobox_2d_smooth, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_2d_smooth, 3, 4, 9, 10);
    for (i = 0; i < ImgProc2D::MAX_SMOOTH; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_2d_smooth), buf);
    }

    // restric
    label_restric = gtk_label_new (_("Reject:"));
    gtk_widget_show (label_restric);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_restric, 270, 390);
    gtk_widget_set_size_request (label_restric, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_restric, 0, 1, 10, 11);

    m_combobox_restric = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_restric);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_restric, 370, 390);
    gtk_widget_set_size_request (m_combobox_restric, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_restric, 1, 2, 10, 11);
    for (i = 0; i < ImgProc2D::MAX_REJECT; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_restric), buf);
    }

   // gray transform
    label_gray_transform = gtk_label_new (_("GrayTrans:"));
    gtk_widget_show (label_gray_transform);
    gtk_widget_set_size_request (label_gray_transform, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_gray_transform, 2, 3, 10, 11);

    m_combobox_gray_trans = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_gray_trans);
    gtk_widget_set_size_request (m_combobox_gray_trans, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_gray_trans, 3, 4, 10, 11);
    //float value = 1.0;
    //sprintf(buf , "%.1f", value);
    //gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_gray_trans), 0, "C0");
    for (i = 0; i < ImgProc2D::MAX_TRANS_CURVE; i ++)
    {
        sprintf(buf , "C%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_gray_trans), buf);
    }

    hseparator2 = gtk_hseparator_new ();
    gtk_widget_show (hseparator2);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), hseparator2, 10, 430);
    //gtk_widget_set_size_request (hseparator2, 470, 16);
    gtk_table_attach (GTK_TABLE (table_2d_m_mode), hseparator2, 0, 4, 11, 12,
            (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_SHRINK), 0, 0);
    gtk_widget_set_size_request (hseparator2, -1, 5);

    // THI
    label_thi = gtk_label_new (_("THI:"));
    gtk_widget_show (label_thi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_thi, 5, 450);
    gtk_widget_set_size_request (label_thi, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_thi, 0, 1, 12, 13);

    m_combobox_thi = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_thi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_thi, 105, 450);
    gtk_widget_set_size_request (m_combobox_thi, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_thi, 1, 2, 12, 13);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_thi), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_thi), _("ON"));

    // TSI
    label_tsi = gtk_label_new (_("TSI:"));
    gtk_widget_show (label_tsi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_tsi, 270, 450);
    gtk_widget_set_size_request (label_tsi, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_tsi, 2, 3, 12, 13);

    m_combobox_tsi = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_tsi);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_tsi, 370, 450);
    gtk_widget_set_size_request (m_combobox_tsi, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_tsi, 3, 4, 12, 13);
    for (i = 0; i < Img2D::MAX_TSI; i ++) {
        sprintf(buf , "%s", _(Img2D::TSI_DISPLAY[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_tsi), buf);
    }

    // image enhance
    label_imgEnh = gtk_label_new (_("iPurity:"));
    gtk_widget_show (label_imgEnh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_imgEnh, 5, 490);
    gtk_widget_set_size_request (label_imgEnh, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_imgEnh, 0, 1, 13, 14);

    m_combobox_imgEnh = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_imgEnh);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_combobox_imgEnh, 105, 490);
    gtk_widget_set_size_request (m_combobox_imgEnh, 100, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_combobox_imgEnh, 1, 2, 13, 14);
    for (i = 0; i < ImgProc2D::MAX_IMG_EHN; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_imgEnh), buf);
    }

    // M gain
    label_m_gain = gtk_label_new (_("M Gain:"));
    gtk_widget_show (label_m_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), label_m_gain, 270, 490);
    gtk_widget_set_size_request (label_m_gain, -1, 30);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), label_m_gain, 2, 3, 13, 14);

    spinbutton_m_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_m_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_m_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_m_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_2d_m_mode), m_spinbutton_m_gain, 370, 490);
    gtk_widget_set_size_request (m_spinbutton_m_gain, 100, 27);
    gtk_table_attach_defaults (GTK_TABLE (table_2d_m_mode), m_spinbutton_m_gain, 3, 4, 13, 14);
    g_signal_connect(G_OBJECT(m_spinbutton_m_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    //space compound
    label_space_compound = gtk_label_new(_("SpaceCompound:"));
    gtk_widget_show(label_space_compound);
    gtk_widget_set_size_request(label_space_compound, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), label_space_compound, 0, 1, 14, 15);
    m_combobox_space_compound = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_space_compound);
    gtk_widget_set_size_request(m_combobox_space_compound, -1, 30);
    gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_space_compound), 0, _("OFF"));
    for(i = 1; i <Img2D::MAX_SPACE_COMPOUND; i ++)
    {
        sprintf(buf, "%d", i);
        gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_space_compound), buf);
    }
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), m_combobox_space_compound, 1, 2, 14, 15);

    //freq compound
    label_freq_compound = gtk_label_new(_("FreqCompound:"));
    gtk_widget_show(label_freq_compound);
    gtk_widget_set_size_request(label_freq_compound, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), label_freq_compound, 2, 3, 14, 15);
    m_combobox_freq_compound = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_freq_compound);
    gtk_widget_set_size_request(m_combobox_freq_compound, -1, 30);
    gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_freq_compound), 0, _("OFF"));
    gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_freq_compound), 1, _("ON"));
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), m_combobox_freq_compound, 3, 4, 14, 15);

    //thi freq
    label_thi_freq = gtk_label_new(_("FH:"));
    gtk_widget_show(label_thi_freq);
    gtk_widget_set_size_request(label_thi_freq, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), label_thi_freq, 0, 1, 15, 16);
    m_combobox_thi_freq = gtk_combo_box_new_text();
    gtk_widget_show(m_combobox_thi_freq);
    gtk_widget_set_size_request(m_combobox_thi_freq, -1, 30);
    gtk_table_attach_defaults(GTK_TABLE(table_2d_m_mode), m_combobox_thi_freq, 1, 2, 15, 16);

    // pw mode
#if not defined(CT_322)
#ifndef  CT_313
    frame_pw_mode = gtk_frame_new (NULL);
    gtk_widget_show (frame_pw_mode);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_pw_mode, 5, 715); //660
    //gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_pw_mode, 5, 715 - 40); //660
    gtk_widget_set_size_request (frame_pw_mode, 520, 200);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_pw_mode), GTK_SHADOW_OUT);

    fixed_pw_mode = gtk_fixed_new ();
    gtk_widget_show (fixed_pw_mode);
    gtk_container_add (GTK_CONTAINER (frame_pw_mode), fixed_pw_mode);

    label_pw_mode = gtk_label_new (_("<b>PW mode</b>"));
    gtk_widget_show (label_pw_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_pw_mode), label_pw_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_pw_mode), TRUE);

    table_pw_mode = gtk_table_new (5, 4, FALSE);
    gtk_widget_show (table_pw_mode);
    gtk_fixed_put (GTK_FIXED (fixed_pw_mode), table_pw_mode, 20, 5);
    gtk_widget_set_size_request (table_pw_mode, 480, 170);
    gtk_table_set_row_spacings (GTK_TABLE(table_pw_mode), 10);

    // doppler freq
    label_pw_freq = gtk_label_new (_("Freq. :"));
    gtk_widget_show (label_pw_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_freq, 5, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_freq, 0, 1, 0, 1);
    gtk_widget_set_size_request (label_pw_freq, -1, 30);

    m_combobox_pw_freq = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_freq);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_freq, 105, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_freq, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_combobox_pw_freq, 100, 30);

    // pw gain
    label_pw_gain = gtk_label_new (_("PW Gain:"));
    gtk_widget_show (label_pw_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_gain, 270, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_gain, 2, 3, 0, 1);
    gtk_widget_set_size_request (label_pw_gain, -1, 30);

    spinbutton_pw_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_pw_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_pw_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_pw_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_spinbutton_pw_gain, 370, 12);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_spinbutton_pw_gain, 3, 4, 0, 1);
    gtk_widget_set_size_request (m_spinbutton_pw_gain, 100, 27);
    g_signal_connect(G_OBJECT(m_spinbutton_pw_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

#if 0
    for (i = 0; i < ProbeSocket::FREQ_DOPPLER_SUM[probeIndex]; i ++)
    {
        sprintf(buf , "%.1f", (float)ProbeSocket::FREQ_DOPPLER[probeIndex][i] / 20);
        gtk_combo_box_append_text (GTK_COMBO_BOX(m_combobox_pw_freq), buf);
    }
#endif

    // pw scale
    label_pw_scale_prf = gtk_label_new (_("Scale PRF:"));
    gtk_widget_show (label_pw_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_scale_prf, 5, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_scale_prf, 0, 1, 1, 2);
    gtk_widget_set_size_request (label_pw_scale_prf, -1, 30);

    m_combobox_pw_scale_prf = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_scale_prf, 105, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_scale_prf, 1, 2, 1, 2);
    gtk_widget_set_size_request (m_combobox_pw_scale_prf, 100, 30);
    for (i = 0; i < ImgPw::MAX_PRF; i ++) {
        sprintf(buf , "%dHz", ImgPw::PW_PRF[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_scale_prf), buf);
    }
    g_signal_connect (G_OBJECT(m_combobox_pw_scale_prf ), "changed", G_CALLBACK (HandleComboPwPrf), this);

    // wall filter
    label_pw_wallfilter = gtk_label_new (_("Wall Filter:"));
    gtk_widget_show (label_pw_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_wallfilter, 270, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_wallfilter, 2, 3, 1, 2);
    gtk_widget_set_size_request (label_pw_wallfilter, -1, 30);

    m_combobox_pw_wallfilter = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_wallfilter, 370, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_wallfilter, 3, 4, 1, 2);
    gtk_widget_set_size_request (m_combobox_pw_wallfilter, 100, 30);

    // angle
    label_pw_angle = gtk_label_new (_("Angle:"));
    gtk_widget_show (label_pw_angle);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_angle, 0, 1, 2, 3);
    gtk_widget_set_size_request (label_pw_angle , -1, 30);

    spinbutton_pw_angle_adj = gtk_adjustment_new (0, -85, 85, 5, 1, 0);
    m_spinbutton_pw_angle = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_pw_angle_adj), 1, 0);
    gtk_widget_show (m_spinbutton_pw_angle);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_spinbutton_pw_angle, 1, 2, 2, 3);
    gtk_widget_set_size_request (m_spinbutton_pw_angle, 100, 27);
    g_signal_connect(G_OBJECT(m_spinbutton_pw_angle), "insert_text", G_CALLBACK(on_spinbutton_insert_angle), this);
    g_signal_connect(G_OBJECT(m_spinbutton_pw_angle), "output", G_CALLBACK(on_spinbutton_output_angle), this);

    hseparator3 = gtk_hseparator_new ();
    gtk_widget_show (hseparator3);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), hseparator3, 8, 88);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), hseparator3, 0, 4, 3, 4);
    gtk_widget_set_size_request (hseparator3, -1, 5);

    // invert
    label_pw_invert = gtk_label_new (_("Spectrum Invert:"));
    gtk_widget_show (label_pw_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_pw_invert, 5, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_pw_invert, 0, 1, 4, 5);
    gtk_widget_set_size_request (label_pw_invert, -1, 30);

    m_combobox_pw_invert = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_pw_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_pw_invert, 105, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_pw_invert, 1, 2, 4, 5);
    gtk_widget_set_size_request (m_combobox_pw_invert, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_invert), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_invert), _("ON"));

    // time resolustion
    label_time_resolution = gtk_label_new (_("Time Resolution:"));
    gtk_widget_show (label_time_resolution);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), label_time_resolution, 260, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), label_time_resolution, 2, 3, 4, 5);
    gtk_widget_set_size_request (label_time_resolution, -1, 30);

    m_combobox_time_resolution = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_time_resolution);
    //gtk_fixed_put (GTK_FIXED (fixed_pw_mode), m_combobox_time_resolution, 370, 110);
    gtk_table_attach_defaults (GTK_TABLE (table_pw_mode), m_combobox_time_resolution, 3, 4, 4, 5);
    gtk_widget_set_size_request (m_combobox_time_resolution, 100, 30);
    for (i = 0; i < ImgProcPw::MAX_TIME_RES; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_time_resolution), buf);
    }

    // cfm mode
    frame_cfm_mode = gtk_frame_new (NULL);
    gtk_widget_show (frame_cfm_mode);
    gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_cfm_mode, 5, 935); //880
    //gtk_fixed_put (GTK_FIXED (fixed_img_set), frame_cfm_mode, 5, 935 - 40); //880
    gtk_widget_set_size_request (frame_cfm_mode, 520, 260);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_cfm_mode), GTK_SHADOW_OUT);

    fixed_cfm_mode = gtk_fixed_new ();
    gtk_widget_show (fixed_cfm_mode);
    gtk_container_add (GTK_CONTAINER (frame_cfm_mode), fixed_cfm_mode);

    label_cfm_mode = gtk_label_new (_("<b>CFM mode</b>"));
    gtk_widget_show (label_cfm_mode);
    gtk_frame_set_label_widget (GTK_FRAME (frame_cfm_mode), label_cfm_mode);
    gtk_label_set_use_markup (GTK_LABEL (label_cfm_mode), TRUE);

    table_cfm_mode = gtk_table_new (6, 4, FALSE);
    gtk_widget_show (table_cfm_mode);
    gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), table_cfm_mode, 20, 5);
    gtk_widget_set_size_request (table_cfm_mode, 480, 230);
    gtk_table_set_row_spacings (GTK_TABLE(table_cfm_mode), 10);

    // cfm gain
    label_color_gain = gtk_label_new (_("Color Gain:"));
    gtk_widget_show (label_color_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_color_gain, 5, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_color_gain, 0, 1, 0, 1);
    gtk_widget_set_size_request (label_color_gain, -1, 30);

    spinbutton_cfm_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    m_spinbutton_cfm_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_cfm_gain_adj), 1, 0);
    gtk_widget_show (m_spinbutton_cfm_gain);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_spinbutton_cfm_gain, 105, 12);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_spinbutton_cfm_gain, 1, 2, 0, 1);
    gtk_widget_set_size_request (m_spinbutton_cfm_gain, 100, 27);
    g_signal_connect(G_OBJECT(m_spinbutton_cfm_gain), "insert_text", G_CALLBACK(on_spinbutton_insert_gain), this);

    // cfm scale
    label_cfm_scale_prf = gtk_label_new (_("Scale PRF:"));
    gtk_widget_show (label_cfm_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_scale_prf, 270, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_scale_prf, 2, 3, 0, 1);
    gtk_widget_set_size_request (label_cfm_scale_prf, -1, 30);

    m_combobox_cfm_scale_prf = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_scale_prf);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_scale_prf, 370, 10);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_scale_prf, 3, 4, 0, 1);
    gtk_widget_set_size_request (m_combobox_cfm_scale_prf, 100, 30);
    for (i = 0; i < ImgCfm::MAX_PRF_INDEX; i ++) {
        sprintf(buf , "%dHz", ImgCfm::CFM_PRF[i]);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_scale_prf), buf);
    }
    g_signal_connect (G_OBJECT(m_combobox_cfm_scale_prf), "changed", G_CALLBACK (HandleComboCfmPrf), this);

    // sensitive
    label_sensitivity = gtk_label_new (_("Sensitivity:"));
    gtk_widget_show (label_sensitivity);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_sensitivity, 5, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_sensitivity, 0, 1, 1, 2);
    gtk_widget_set_size_request (label_sensitivity, -1, 30);
    m_combobox_sensitivity = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_sensitivity);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_sensitivity, 105, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_sensitivity, 1, 2, 1, 2);
    gtk_widget_set_size_request (m_combobox_sensitivity, 100, 30);

    g_signal_connect (G_OBJECT(m_combobox_sensitivity), "changed", G_CALLBACK (HandleComboCfmSensitivity), this);

    // cfm wall filter
    label_cfm_wallfilter = gtk_label_new (_("Wall Filter:"));
    gtk_widget_show (label_cfm_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_wallfilter, 5, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_wallfilter, 2, 3, 1, 2);
    gtk_widget_set_size_request (label_cfm_wallfilter, -1, 30);

    m_combobox_cfm_wallfilter = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_wallfilter);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_wallfilter, 105, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_wallfilter, 3, 4, 1, 2);
    gtk_widget_set_size_request (m_combobox_cfm_wallfilter, 100, 30);

    // color line density
    label_cfm_line_density = gtk_label_new (_("Line Density:"));
    gtk_widget_show (label_cfm_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_line_density, 270, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_line_density, 0, 1, 2, 3);
    gtk_widget_set_size_request (label_cfm_line_density, -1, 30);

    m_combobox_cfm_line_density = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_line_density);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_line_density, 370, 50);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_line_density, 1, 2, 2, 3);
    gtk_widget_set_size_request (m_combobox_cfm_line_density, 100, 30);
    for (i = 0; i < ImgCfm::MAX_LINE_DENSITY; i ++) {
        sprintf(buf , "%s", _(ImgCfm::LINE_DENSITY_DISPLAY[i].c_str()));
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_line_density), buf);
    }

    // variance
    label_variance = gtk_label_new (_("Variance:"));
    gtk_widget_show (label_variance);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_variance, 270, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_variance, 2, 3, 2, 3);
    gtk_widget_set_size_request (label_variance, -1, 30);

    m_combobox_variance = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_variance);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_variance, 370, 90);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_variance, 3, 4, 2, 3);
    gtk_widget_set_size_request (m_combobox_variance, -1, 30);
    for (i = 0; i < ImgCfm::MAX_TURB; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_variance), buf);
    }

    hseparator4 = gtk_hseparator_new ();
    gtk_widget_show (hseparator4);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), hseparator4, 5, 130);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), hseparator4, 0, 4, 3, 4);
    gtk_widget_set_size_request (hseparator4, -1, 5);

    // invert
    label_cfm_invert = gtk_label_new (_("Color Invert:"));
    gtk_widget_show (label_cfm_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_invert, 5, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_invert, 0, 1, 4, 5);
    gtk_widget_set_size_request (label_cfm_invert, -1, 30);

    m_combobox_cfm_invert = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_invert);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_invert, 105, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_invert, 1, 2, 4, 5);
    gtk_widget_set_size_request (m_combobox_cfm_invert, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_invert), _("OFF"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_invert), _("ON"));

    // persistence
    label_persistence = gtk_label_new (_("Persistence:"));
    gtk_widget_show (label_persistence);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_persistence, 270, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_persistence, 2, 3, 4, 5);
    gtk_widget_set_size_request (label_persistence, -1, 30);

    m_combobox_persistence = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_persistence);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_persistence, 370, 150);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_persistence, 3, 4, 4, 5);
    gtk_widget_set_size_request (m_combobox_persistence, 100, 30);
    for (i = 0; i < ImgProcCfm::MAX_PERSIST; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_persistence), buf);
    }

    // color rejection
    label_color_rejection = gtk_label_new (_("Color Reject:"));
    gtk_widget_show (label_color_rejection);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_color_rejection, 5, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_color_rejection, 0, 1, 5, 6);
    gtk_widget_set_size_request (label_color_rejection, -1, 30);

    m_combobox_color_rejection = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_color_rejection);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_color_rejection, 105, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_color_rejection, 1, 2, 5, 6);
    gtk_widget_set_size_request (m_combobox_color_rejection, 100, 30);
    for (i = 0; i < ImgProcCfm::MAX_REJECT; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_color_rejection), buf);
    }

    // cfm smooth
    label_cfm_smooth = gtk_label_new (_("Smooth:"));
    gtk_widget_show (label_cfm_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_cfm_smooth, 270, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), label_cfm_smooth, 2, 3, 5, 6);
    gtk_widget_set_size_request (label_cfm_smooth, -1, 30);

    m_combobox_cfm_smooth = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_cfm_smooth);
    //gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_combobox_cfm_smooth, 370, 190);
    gtk_table_attach_defaults (GTK_TABLE (table_cfm_mode), m_combobox_cfm_smooth, 3, 4, 5, 6);
    gtk_widget_set_size_request (m_combobox_cfm_smooth, 100, 30);
    for (i = 0; i < ImgProcCfm::MAX_SMOOTH; i ++) {
        sprintf(buf , "%d", i);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_smooth), buf);
    }
#endif
#endif
    // pdi gain
    // label_pdi_gain = gtk_label_new (_("PDI Gain:"));
    // gtk_widget_show (label_pdi_gain);
    // gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), label_pdi_gain, 5, 230);
    // gtk_widget_set_size_request (label_pdi_gain, 100, 30);

    // spinbutton_pdi_gain_adj = gtk_adjustment_new (0, 0, 100, 1, 1, 0);
    // m_spinbutton_pdi_gain = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton_pdi_gain_adj), 1, 0);
    // gtk_widget_show (m_spinbutton_pdi_gain);
    // gtk_fixed_put (GTK_FIXED (fixed_cfm_mode), m_spinbutton_pdi_gain, 105, 232);
    // gtk_widget_set_size_request (m_spinbutton_pdi_gain, 100, 27);

    return fixed_image;
}

#ifndef VET
void ViewSystem::RenameItemClicked(GtkButton *button)
{

    g_object_set(renderer, "editable", TRUE, NULL);

   // g_signal_connect(renderer, "editable", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(renderer, "edited", G_CALLBACK(HandleCellRendererRename), this);
}
#endif
void ViewSystem::EntryItemInsert(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path)
{
    if(GTK_IS_ENTRY(editable))
    {
        g_signal_connect(G_OBJECT(editable), "insert_text", G_CALLBACK(on_entry_insert_custom_item), this);
    }
}

void ViewSystem::EntryItemLengthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
   gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

    if(old_text_length + new_text_length >20) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}
void ViewSystem::CreateDefineItem(int probeIndex, vector<ExamPara>& vecExamItem)
{
    char probelist[256];
    char useritem[256];
    char department[256];
    char firstgenitem[256];
    char src_group[256];
    char userselect[256];
    const char* username = GetUserName();
    ExamItem exam;
    char usernamechar[256];
    strcpy(usernamechar, username);
    char userselectname[256];;
    exam.TransUserSelectForEng(usernamechar, userselectname);

    vector<string> useritemgroup;
    ExamItem examitem;
    useritemgroup = examitem.GetCurrentUserGroup();

    int group_length(0);
    group_length = useritemgroup.size();
    PRINTF("probeIndex=%d,group_length=%d\n",probeIndex, group_length);
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        examitem.GetUserItem(src_group, userselect, probelist, useritem, department, firstgenitem);
        ExamPara exampara;
#ifndef VET
        exampara.dept_name=_(department);
#endif
        exampara.name=useritem;
        exampara.index=ExamItem::USERNAME;
        PRINTF("probelist=%s, PROBE_LIST=%s\n",probelist,PROBE_LIST[probeIndex].c_str());
        if(strcmp(userselectname, userselect) == 0)
        {
            if((probeIndex>=0)&&(probeIndex<=NUM_PROBE))
            {
                if (strcmp(probelist, PROBE_LIST[probeIndex].c_str())==0)
                {
                    vecExamItem.push_back(exampara);
                }
            }
        }
    }
}

void ViewSystem::CreateDefineItemFormUsbToHost(char *name)
{
    char path_defined[256];
    if(strcmp(name, "System Default") ==0)
/*
#ifdef CT_355
        sprintf(path_defined, "%s%s%s", "/mnt/udisk/emp-data/userconfig/", name, ".ini");
        //sprintf(path_defined, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, "SystemDefault", ".ini");
    else
        sprintf(path_defined, "%s%s%s", "/mnt/udisk/emp-data/userconfig/", name, ".ini");
#else
*/
        sprintf(path_defined, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, "SystemDefault", ".ini");
    else
        sprintf(path_defined, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, name, ".ini");
//#endif
    FILE *fp;
    if((fp = fopen(path_defined, "r")) !=NULL)
    {
        remove(path_defined);
        fclose(fp);
    }
    int fd = open(path_defined, O_RDWR |O_CREAT |O_TRUNC, 0666);
    close(fd);
    IniFile ini_defined(path_defined);
    IniFile *ptrIni1= &ini_defined;

    char probelist[256];
    char useritem[256];
    char department[256];
    char firstgenitem[256];
    char src_group[256];
    char userselect[256];
    ExamItem exam;

    vector<string> useritemgroup;
    ExamItem examitem;
    char path[256];

    if(strcmp(name, "System Default") ==0)
        sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
    else
        sprintf(path, "%s%s%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "userconfig/", name,".ini");

    IniFile ini(path);
    IniFile *ptrIni= &ini;
    useritemgroup = ptrIni->GetGroupName();

    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        strcpy(userselect,(char*)ptrIni->ReadString(src_group, "UserSelect").c_str());
        if(strcmp(userselect , "") !=0)
        {
            strcpy(probelist,(char*)ptrIni->ReadString(src_group, "ProbeType").c_str());
            strcpy(useritem,(char*)ptrIni->ReadString(src_group, "ExamItem").c_str());
#ifndef VET
            strcpy(department,(char*)ptrIni->ReadString(src_group, "Department").c_str());
            strcpy(firstgenitem,(char*)ptrIni->ReadString(src_group, "GenFirstItem").c_str());
#endif
            ptrIni1->WriteString(src_group, "UserSelect",name);
            ptrIni1->WriteString(src_group, "ProbeType",probelist);
            ptrIni1->WriteString(src_group, "ExamItem", useritem);
#ifndef VET
            ptrIni1->WriteString(src_group, "Department", department);
            ptrIni1->WriteString(src_group, "GenFirstItem", firstgenitem);
#endif
            ptrIni1->SyncConfigFile();
        }
    }
}

void ViewSystem::CreateDefineItem_comment(int probeIndex, vector<string>& vecExamItem_comment)
{
    char probelist[256];
    char useritem[256];
    char department[256];
    char firstgenitem[256];
    char src_group[256];
    char userselect[256];
   // const char* username = GetUserName();
     char path_userselect[256];
    sprintf(path_userselect, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini_userselect(path_userselect);
    ExamItem examitem;
    string userselectname;
    userselectname = examitem.ReadDefaultUserSelect(&ini_userselect);

    vector<string> useritemgroup;
    useritemgroup = examitem.GetDefaultUserGroup();

    int group_length(0);
    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        examitem.GetUserItem(src_group, userselect, probelist, useritem, department, firstgenitem);
        ExamPara exampara;
        exampara.dept_name=_(department);
        exampara.name=useritem;
        exampara.index=ExamItem::USERNAME;
        PRINTF("probelist=%s, PROBE_LIST=%s\n",probelist,PROBE_LIST[probeIndex].c_str());
        if(strcmp(userselectname.c_str(), userselect) == 0)
        {
            if((probeIndex>=0)&&(probeIndex<=NUM_PROBE))
            {
                if (strcmp(probelist, PROBE_LIST[probeIndex].c_str())==0)
                {
                    vecExamItem_comment.push_back(useritem);
                }
            }
        }
    }
}

bool ViewSystem::RenameNotice(int probe_type_index, char *new_text, char *dialognotice, int group_length)
{
    char probelist[256];
    char useritem[256];
    char department[256];
    char firstgenitem[256];
    char src_group[256];
    char userselect[256];
    const char* username = GetUserName();
    char usernamechar[256];
    strcpy(usernamechar, username);
    char userselectname[256];
    ExamItem examitem;
    examitem.TransUserSelectForEng(usernamechar, userselectname);

    vector<string> useritemgroup;
    useritemgroup = examitem.GetCurrentUserGroup();

    group_length = useritemgroup.size();
    for (int i= 0 ; i <  group_length; i++)
    {
        sprintf(src_group ,"%s", useritemgroup[i].c_str());
        examitem.GetUserItem(src_group, userselect, probelist,useritem,department, firstgenitem);
        if(strcmp(userselectname, userselect) == 0)
        {
            if((probe_type_index>=0)&&(probe_type_index<=NUM_PROBE))
            {
                if (strcmp(probelist, PROBE_LIST[probe_type_index].c_str())==0)
                {
                    if (strcmp(useritem,new_text)==0)
                    {
                        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, dialognotice, NULL); //重命名失败!该结点已存在
                        return FALSE;
                    }
                }
            }
        }
    }

}

void ViewSystem::TransEnglish(char *strname, char str[256],char *str_indexname,char str_index[256])

{
    string newtext=_("New Item");
    string smallpart=_("Small Part");
    string gyney=_("Gynecology");
    string abdomen=_("Abdomen");
    string cardiac=_("Cardiac");
    string ortho=_("Orthopedic");
    string vas=_("Vascular");
    string ob=_("Obstetrics");
    string ur=_("Urology");

    string adbo=_("Adult Abdomen");
    string gyn=_("Gynecology");
    string earlypreg=_("Early Pregnancy");
    string kidney=_("Kidney Ureter");
    string hipjoint=_("Hip Joint");
    string mammary=_("Mammary Glands");
    string kidabdo=_("Kid Abdomen");
    string adultcar=_("Adult Cardio");
    string fetus=_("Fetal Cardio");
    string thyroid=_("Thyroid");
    string carotid=_("Carotid");

        if (strcmp(strname, smallpart.c_str())==0)
        {
            strcpy(str, "Small Part");
        }
        else if (strcmp(strname, gyney.c_str())==0)
        {
            strcpy(str, "Gynecology");
        }
        else if (strcmp(strname,abdomen.c_str())==0)
        {
            strcpy(str, "Abdomen");
        }
        else if (strcmp(strname, cardiac.c_str())==0)
        {
            strcpy(str, "Cardiac");
        }
        else if (strcmp(strname, ortho.c_str())==0)
        {
            strcpy(str, "Orthopedic");
        }
        else if (strcmp(strname, vas.c_str())==0)
        {
            strcpy(str, "Vascular");
        }
        else if (strcmp(strname,ob.c_str())==0)
        {
            strcpy(str, "Obstetrics");
        }
        else if (strcmp(strname, ur.c_str())==0)
        {
            strcpy(str, "Urology");
        }

        if (strcmp(str_indexname, newtext.c_str())==0)
        {
            strcpy(str_index, "New Item");
        }

        else if (strcmp(str_indexname, adbo.c_str())==0)
        {
            strcpy(str_index, "Adult Abdomen");
        }
        else if (strcmp(str_indexname, gyn.c_str())==0)
        {
            strcpy(str_index, "Gynecology");
        }
        else if (strcmp(str_indexname,earlypreg.c_str())==0)
        {
            strcpy(str_index, "Early Pregnancy");
        }
        else if (strcmp(str_indexname, kidney.c_str())==0)
        {
            strcpy(str_index, "Kidney Ureter");
        }
        else if (strcmp(str_indexname,hipjoint.c_str())==0)
        {
            strcpy(str_index, "Hip Joint");
        }
        else if (strcmp(str_indexname,mammary.c_str())==0)
        {
            strcpy(str_index, "Mammary Glands");
        }
        else if (strcmp(str_indexname,kidabdo.c_str())==0)
        {
            strcpy(str_index, "Kid Abdomen");
        }
        else if (strcmp(str_indexname,adultcar.c_str())==0)
        {
            strcpy(str_index, "Adult Cardio");
        }
        else if (strcmp(str_indexname,fetus.c_str())==0)
        {
            strcpy(str_index, "Fetal Cardio");

        }
        else if (strcmp(str_indexname,thyroid.c_str())==0)
        {
            strcpy(str_index, "Thyroid");

        }
        else if (strcmp(str_indexname,carotid.c_str())==0)
        {
            strcpy(str_index, "Carotid");
        }

}
void ViewSystem::BtnComboVideoChanged()
{
    FpgaCtrl2D* fpga2d=new FpgaCtrl2D;
    gint data=gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video));
   // fpga2d->VideoStandard((INT16U)(128+data));
    fpga2d->VideoStandard((INT16U)(data));
}

void ViewSystem::CellRendererRename(GtkCellRendererText *renderer, gchar *path_str, gchar *new_text)
{
    char *old_text;

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_exam_type));
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &old_text, -1);

    // gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free(path);

    //如果重命名前后renderertext中的文本没有改变,则返回
    if (strcmp(new_text, old_text) == 0)
    {

        return;
    }
    if (strcmp(new_text, "") == 0) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("The new item name can not be empty!"), NULL); //结点名称不能为空

        return;
    }

    //except the user defined1 to user defined4
#ifdef VET
    int size = sizeof(ExamItemArray)/sizeof(ExamPara);
#else
    int size = sizeof(ExamItemArray)/sizeof(ExamPara) - 4;
#endif
    for(int i = 0; i < size/*24*/; i++)
    {
        if (new_text==ExamItemArray[i].name)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Fail to rename! The item name has existed!"), NULL); //重命名失败!该结点已存在
            return;
        }
        if (old_text==ExamItemArray[i].name)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Operation is invaild."), NULL); //
            return;
        }
    }

    int probe_type_index = GetProbeType();
    char *dialognotice=_("Fail to rename! The item name has existed!");
    int group_length(0);
    bool renameflag=TRUE;
    renameflag=RenameNotice(probe_type_index, new_text, dialognotice, group_length);
    if(renameflag)
    {
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);
        GtkTreeIter iter_parent;
        GtkTreeIter iter_child;

        char *strname;
        char *str_indexname;
        gtk_tree_model_iter_parent(model, &iter_parent, &iter);

        gtk_tree_model_get(model, &iter_parent, 0, &strname, -1);
        gtk_tree_model_iter_children(model, &iter_child, &iter_parent);
        gtk_tree_model_get(model, &iter_child, 0, &str_indexname, -1);

        char str[256];
        char str_index[256];

        TransEnglish(strname, str, str_indexname, str_index);

        ExamItem examItem;
        int itemIndex;
        int probeIndex;
        probeIndex = GetProbeType();

        if ((probeIndex >= 0))
        {
            examItem.DeleteNewItemFile(probeIndex, old_text, str);
            examItem.DeleteItemParaFile(probeIndex, old_text, str);
            examItem.DeleteNewItemForCommentFile(probeIndex, old_text, str);
            examItem.DeleteNewItemForCalcFile(probeIndex, old_text, str);
            examItem.DeleteNewItemForMeasureFile(probeIndex, old_text, str);

            examItem.WriteNewItemFile(probeIndex, new_text, str,str_index);
            examItem.WriteDefinedItemPara(probeIndex, new_text, str, str_index);
            examItem.WriteNewItemToCommentFile(probeIndex, new_text, str);
            examItem.WriteNewItemToCalcFile(probeIndex, new_text, str);
            examItem.WriteNewItemToMeasureFile(probeIndex, new_text, str);
        }
    }
}

void ViewSystem::AddCheckPart(char *checkpart)
{
    if(strcmp(checkpart, "") == 0)
    {
        PRINTF("chekc_part is null\n");
        return;
    }

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
    GtkTreeModel *model;
    GtkTreeIter iter;

#ifdef VET
    gtk_tree_selection_get_selected(selected_node, &model, &iter);
#else
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a department before inserting!"), NULL); //请先选择待插入结点的父结点
        return;
    }
#endif
    ///>先检查新建的item是否为固定的检查模式或者科别
    //except the user defined1 to user defined4
#ifdef VET
    int size = sizeof(ExamItemArray)/sizeof(ExamPara);
#else
    int size = sizeof(ExamItemArray)/sizeof(ExamPara) - 4;
#endif
    for(int i = 0; i < size; i++)
    {
        if (checkpart == ExamItemArray[i].name)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Fail to rename! The item name has existed!"), NULL); //重命名失败!该结点已存在
            return;
        }
    }
    for(int i = 0; i < NUM_KB; i++)
    {
        if (strcmp(checkpart, KB_ADDR[i].c_str()) == 0)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("Fail to rename! The item name has existed!"), NULL);
            return;
        }
    }
    int probe_type_index = GetProbeType();
    char *dialognotice=_("Fail to rename! The item name has existed!");
    int group_length(0);
    bool renameflag=TRUE;

    renameflag=RenameNotice(probe_type_index, checkpart, dialognotice, group_length);
    if(renameflag)
    {
#ifndef VET
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        gint tree_depth = gtk_tree_path_get_depth(path);
        gtk_tree_path_free (path);

        if(tree_depth > 1)
        {
            GtkTreeIter parent_iter;
            gtk_tree_model_iter_parent(model, &parent_iter, &iter);
            iter = parent_iter;
            tree_depth = 1;
        }

        char *strname;
        char *str_indexname;
        GtkTreeIter childern;

        gtk_tree_model_get(model, &iter, 0, &strname, -1);
        gtk_tree_model_iter_children(model, &childern, &iter);
        gtk_tree_model_get(model, &childern, 0, &str_indexname, -1);
#endif
        char str[256];
        char str_index[256];
#ifndef VET
        TransEnglish(strname, str, str_indexname, str_index);
#endif
        ExamItem examItem;
        int itemIndex;
        int probeIndex;
        probeIndex = GetProbeType();

        //先写入新建名称到文件
        if ((probeIndex >= 0))
        {
            examItem.WriteNewItemFile(probeIndex, checkpart, str, str_index);
            examItem.WriteNewItemToCommentFile(probeIndex, checkpart, str);
            examItem.WriteNewItemToCalcFile(probeIndex, checkpart, str);
            examItem.WriteNewItemToMeasureFile(probeIndex, checkpart, str);
        }
        //change exam box in comment and calc

        char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path1);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        if(strcmp(_(username.c_str()), GetUserName()) == 0)
        {
            CalcSetting::GetInstance()->ChangeExamBox(checkpart);
            MeasureSetting::GetInstance()->ChangeExamBox(checkpart);
            int probe_type_index = GetProbeType();
            ChangeCommentExamBox(probe_type_index, checkpart);
        }
#ifdef VET
        vector<int> vecItemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[probeIndex].c_str(), ((vector<ExamItem::EItem>*) &vecItemIndex));
        GtkTreeModel *new_model1 = create_exam_item_model(vecItemIndex);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), new_model1);
        //高亮插入的词条，并更新滚动条
        GtkTreeSelection *new_selection;
        GtkTreeIter iter_new;
        new_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
        iter_new= InsertUniqueComment(new_model1, checkpart);
        gtk_tree_selection_select_iter(new_selection, &iter_new);

        GtkTreePath *path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_exam_type), path_scroll, NULL, TRUE, 1.0, 1.0);
        gtk_tree_path_free (path_scroll);

#else
        GtkTreeIter iter_new;
        gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, checkpart, -1);
        GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);

        //刷新treeview
        vector<int> vecItemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[probeIndex].c_str(), ((vector<ExamItem::EItem>*) &vecItemIndex));
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), create_exam_item_model(vecItemIndex));
        //gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeview_exam_type), new_path);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), new_path, NULL, TRUE);

        gtk_tree_path_free (new_path);
#endif
        //保存当前参数值到新建检查科别
        if ((probeIndex >= 0))
        {
            get_current_and_save_image_para(checkpart);
        }

    }
}

void ViewSystem::AddItemClicked(GtkButton *button)
{
     char *new_string = _("New Item");//临时插入结点

    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a department before inserting!"), NULL); //请先选择待插入结点的父结点
        return;
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free (path);
    if(tree_depth==1)
    {
        int probe_type_index = GetProbeType();
        int group_length(0);
        char *dialognotice= _("Fail to insert! 'New Item' has existed in the probe type. please rename it before inserting!");
        RenameNotice(probe_type_index, new_string, dialognotice, group_length);
        if(group_length>MAXNUMBER)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                    _("The defined items have reached the maximum!"), NULL);
            return;
        }

        char *strname;
        char *str_indexname;
        GtkTreeIter childern;

        gtk_tree_model_get(model, &iter, 0, &strname, -1);
        gtk_tree_model_iter_children(model, &childern, &iter);
        gtk_tree_model_get(model, &childern, 0, &str_indexname, -1);

        char str[256];
        char str_index[256];
        TransEnglish(strname, str, str_indexname, str_index);

        ExamItem examItem;
        int itemIndex;
        int probeIndex;
        probeIndex = GetProbeType();
        if ((probeIndex >= 0))
        {
            examItem.WriteNewItemFile(probeIndex, new_string, str,str_index);
            examItem.WriteDefinedItemPara(probeIndex, new_string, str, str_index);
            examItem.WriteNewItemToCommentFile(probeIndex, new_string, str);
            examItem.WriteNewItemToCalcFile(probeIndex, new_string, str);
            examItem.WriteNewItemToMeasureFile(probeIndex, new_string, str);

        }
    }
    else if(tree_depth==2)
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Fail to insert! Please select a department before inserting!"), NULL);
        return;
    }

    GtkTreeIter iter_new;
    gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, &iter);
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
    GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);

    //刷新treeview
    int probeIndex;
    probeIndex = GetProbeType();

    ExamItem examItem;
    vector<int> itemIndex;
    examItem.GetItemListOfProbe((char*)PROBE_LIST[probeIndex].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), create_exam_item_model(itemIndex));
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeview_exam_type), new_path);
    //设置结点为编辑状态
    GtkTreeViewColumn *column_tree_view;
    column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeview_exam_type), 0);

    gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeview_exam_type),
            new_path,
            column_tree_view,
            renderer,
            TRUE);
    gtk_tree_path_free (new_path);
}

void ViewSystem::DeleteItemClicked(GtkButton *button)
{
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));

    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selected_node, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                          _("Please select one item to be delete!"), NULL); //请先选择待删除的结点!
        return;
    }

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint tree_depth = gtk_tree_path_get_depth(path);
    gtk_tree_path_free(path);

#ifndef VET
    if (tree_depth == 1) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Fail to delete! Can not delete the department!"), NULL); //删除失败!不能删除根结点!
        return;
    }
    if (tree_depth == 2)
#endif
    {
        char *str0name, *str1name;
        string str0;
        string str1;
        GtkTreeIter parent;
        gtk_tree_model_get(model, &iter, 0, &str0name, -1);
#ifndef VET
        gtk_tree_model_iter_parent(model, &parent, &iter);
        gtk_tree_model_get(model, &parent, 0, &str1name, -1);
#endif
        str0=str0name;
#ifndef VET
        str1=str1name;
#endif
        ExamItem examItem;
        int itemIndex;
        int probeIndex;
        probeIndex = GetProbeType();
        if ((probeIndex >= 0))
        {
#ifdef VET
            int size = sizeof(ExamItemArray)/sizeof(ExamPara);
#else
            int size = sizeof(ExamItemArray)/sizeof(ExamPara) - 4;
#endif
            for(int i = 0; i < size/*24*/; i++)
                {
                   if (strcmp(str0.c_str(),_(ExamItemArray[i].name.c_str()))==0)
                    {
                        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                                _("Fail to delete! Can not delete the defaulted exam item!"), NULL);
                        return;
                    }
                }

            string useritemname;
            char path[256];
            sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
            IniFile ini(path);
            IniFile *ptrIni= &ini;

            bool userflag= ptrIni->ReadBool("UserItemFlag", "UserFlag");
            useritemname = ptrIni->ReadString("UserItemName", "ItemName");
            string userselect = ptrIni->ReadString("UserSelect", "DefaultUser");
            string userprobe = ptrIni->ReadString("ProbeModel", "ProbeModel");
            const char* username = GetUserName();

            if((strcmp(useritemname.c_str(),str0.c_str())==0)&&(userflag==true)&&(strcmp(userprobe.c_str(),PROBE_LIST[probeIndex].c_str())==0)&&(strcmp(username,  _(userselect.c_str()))==0))
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                        _("Fail to delete! The item is under using!"), NULL); //删除失败!
                return;
            }
            else
            {
                examItem.DeleteNewItemFile(probeIndex, str0.c_str(), str1.c_str());
                examItem.DeleteItemParaFile(probeIndex, str0.c_str(), str1.c_str());

                //记录当前所选结点的父结点,刷新删除节点之后的树
                examItem.DeleteNewItemForCommentFile(probeIndex, str0.c_str(), str1.c_str());
                examItem.DeleteNewItemForCalcFile(probeIndex, str0.c_str(), str1.c_str());
                examItem.DeleteNewItemForMeasureFile(probeIndex, str0.c_str(), str1.c_str());
                char path1[256];
                sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
                IniFile ini1(path1);
                ExamItem exam;
                string username;
                username = exam.ReadDefaultUserSelect(&ini1);
                if(strcmp(_(username.c_str()), GetUserName()) == 0)
                {

                    CalcSetting::GetInstance()->ChangeExamBoxDelete();
                    MeasureSetting::GetInstance()->ChangeExamBoxDelete();
                    ChangeCommentExamBoxDelete(probeIndex);
                }
#ifndef VET
                GtkTreeIter parent_iter;
                GtkTreeIter child_iter;
                gtk_tree_model_iter_parent(model, &parent_iter, &iter);
                GtkTreePath *parent_path = gtk_tree_model_get_path(model, &parent_iter);
#endif
                ExamItem examItem;
                vector<int> itemIndex;
                examItem.GetItemListOfProbe((char*)PROBE_LIST[probeIndex].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
                gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), create_exam_item_model(itemIndex));

#ifndef VET
                //展开被删除结点的父结点
                gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
                gboolean has_node = gtk_tree_model_iter_children(model, &child_iter, &parent_iter);
                if(has_node)
                {
                    parent_path = gtk_tree_model_get_path(model, &child_iter);
                    char *str_path = gtk_tree_path_to_string(parent_path);
                    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), parent_path, NULL, TRUE);
                }

                gtk_tree_path_free (parent_path);
#endif
            }
        }
    }
}

void ViewSystem::SpinbuttonInsertGain(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int gain = atoi(old_str.c_str());
    if (gain < 0 || gain > 100)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
}

void ViewSystem::SpinbuttonInsertAngle(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    string new_str = new_text;
    string s1 = "°";
    string::size_type pos = 0;
    if((pos=new_str.find(s1, 0)) == string::npos)
    {
        if (!g_ascii_isdigit(*new_text))
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    old_str.insert(*position, new_str);
    int angle = atoi(old_str.c_str());
    if (angle < -85 || angle > 85)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
}

gboolean ViewSystem::SpinbuttonOutputAngle(GtkSpinButton *spin)
{
    GtkAdjustment *adj;
    gchar *text;
    int value;
    adj = gtk_spin_button_get_adjustment (spin);
    value = (int)gtk_adjustment_get_value (adj);
    text = g_strdup_printf ("%d°", value);
    gtk_entry_set_text (GTK_ENTRY (spin), text);
    g_free (text);

    return TRUE;
}

void ViewSystem::UserChanged(GtkComboBox *widget)
{
    int select;
    select = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    if (select >= 0)
    {
        gchar * name = gtk_combo_box_get_active_text (GTK_COMBO_BOX(widget));
        memset(user_configure, 0, USERCONFIG_LEN);
        if (select > 0)
        {
            sprintf(user_configure, "%s%s%s", "userconfig/",name, ".ini");
        }
        else
        {
#ifdef VET
            sprintf(user_configure, "%s", "VetItemPara.ini");
#else
            sprintf(user_configure, "%s", "ItemPara.ini");
#endif
        }
        set_image_item_sensitive(false);
        save_itemIndex(-1);

        // rebuild the exam type treeview, in order to disppear the select box in gtk
        int index = GetProbeType();
        if (index == -1)
            return;
        ExamItem examItem;
        vector<int> itemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
        GtkTreeModel *model = create_exam_item_model(itemIndex);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));

    }
}

void ViewSystem::UpdateUserItem(void)
{
 // gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_type), 0);
      const char* name = GetUserName();
    memset(user_configure, 0, USERCONFIG_LEN);
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))
    {
        sprintf(user_configure, "%s%s%s", "userconfig/",name, ".ini");
    }
    else
    {
#ifdef VET
        sprintf(user_configure, "%s", "VetItemPara.ini");
#else
        sprintf(user_configure, "%s", "ItemPara.ini");
#endif
    }
    set_image_item_sensitive(false);
    save_itemIndex(-1);

        // rebuild the exam type treeview, in order to disppear the select box in gtk
        int index = GetProbeType();
        if (index == -1)
            return;
        ExamItem examItem;
        vector<int> itemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
        GtkTreeModel *model = create_exam_item_model(itemIndex);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));

}

void ViewSystem::ExamCommentChanged(GtkComboBox *widget)
{
    GtkTreeModel *model1 = create_item_comment_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), model1);

#ifdef VET
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), 0);

#else
    int department_index = DepartmentIndex();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), department_index);
#endif
    g_signal_connect(m_combobox_department_comment, "changed", G_CALLBACK(HandleDepartmentCommentChanged), this);

    g_object_set(m_cellrenderer_comment_text, "editable", TRUE, NULL);
    g_signal_connect(m_cellrenderer_comment_text, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(m_cellrenderer_comment_text, "edited", G_CALLBACK(HandleCellRendererRenameSelectComment), this);

}
void ViewSystem::DepartmentCommentChanged(GtkComboBox *widget)
{
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));

    if(index == -1)
    return;

    GtkTreeModel *model = create_item_comment_model(index);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment), model);

 }

GtkTreeModel* ViewSystem::create_item_comment_model1()
{
    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

    PRINTF("index1=%d, exam_type_name=%s\n", index1, exam_type_name);
    if(!exam_type_name)
        return NULL;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);

    vector<ExamPara> vecItemComment1;
    vecItemComment1.clear();
    CreateItemList_Comment1(PROBE_LIST[index1] + "-" +exam_type, vecItemComment1);
    int item_size(0);
    item_size = vecItemComment1.size();

    if(vecItemComment1.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_INT);

    vector<ExamPara>::iterator iterItem;
    for (iterItem = vecItemComment1.begin(); iterItem != vecItemComment1.end(); iterItem++) {

        gtk_tree_store_append(store, &iter, NULL);

        SysGeneralSetting sysGS;
        if(sysGS.GetLanguage() ==ZH)
        {

            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, _(iterItem->name.c_str()),
                    INDEX_COLUMN, iterItem->index,
                    -1);
        }
        else
        {
          gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, iterItem->name.c_str(),
                    INDEX_COLUMN, iterItem->index,
                    -1);
        }
next:
            continue;
        }

    return GTK_TREE_MODEL(store);
}

GtkTreeModel* ViewSystem::create_item_comment_model(int index)
{

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile new_ini(path1);
    IniFile *new_ptrIni= &new_ini;
    int number;
    int  department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
    if(department_index== -1)
        return NULL;
    char department[256];
    DepartmentName(department, department_index);

    number = new_ptrIni->ReadInt(department, "Number");
    vector<ExamPara> vecItemComment;
    vecItemComment.clear();
    CreateItemList_Comment(department, vecItemComment, number);

    int item_size(0);
    item_size = vecItemComment.size();

    if(vecItemComment.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_INT);

    vector<ExamPara>::iterator iterItem;
    for (iterItem = vecItemComment.begin(); iterItem != vecItemComment.end(); iterItem++) {

        gtk_tree_store_append(store, &iter, NULL);

        SysGeneralSetting sysGS;
        if(sysGS.GetLanguage() ==ZH)
        {

            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN,  _(iterItem->name.c_str()),
                    INDEX_COLUMN, iterItem->index,
                    -1);
            // PRINTF("------------------------%s\n", iterItem->name.c_str());
        }
        else
        {
            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN,  iterItem->name.c_str(),
                    INDEX_COLUMN, iterItem->index,
                    -1);

        }
next:
        continue;
        }

    return GTK_TREE_MODEL(store);
}
/*
void ViewSystem::Add_Columns_Comment(GtkTreeView *treeView)
{
    //column for menu name
    GtkCellRenderer *renderer_text;
    GtkCellRenderer *renderer_toggle;
    GtkTreeViewColumn *column_text;
    GtkTreeViewColumn *column_toggle;

    renderer_text = gtk_cell_renderer_text_new(); // GtkCellRendererText
    column_text = gtk_tree_view_column_new_with_attributes("ITEM_NAME", // column name
            renderer_text,    // the cell renderer type
            "text", NAME_COLUMN, // renderer attribute
            NULL);
    g_object_set(G_OBJECT(column_text),
            "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
            "fixed_width", 180,
            NULL);

    renderer_toggle = gtk_cell_renderer_toggle_new();
    column_toggle = gtk_tree_view_column_new_with_attributes("TOGGLE",
            renderer_toggle,
            "visible", RENDERER_TYPE_COLUMN,
            "active", VALUE_COLUMN,
            NULL);
    g_object_set(G_OBJECT(column_toggle),
            "sizing", GTK_TREE_VIEW_COLUMN_FIXED,
            "fixed_width", 50,
            NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column_text);
}

GtkWidget* ViewSystem::CreateCommentTreeView()
{

    // creating the view
    GtkWidget *treeview = gtk_tree_view_new_with_model(NULL);
    Add_Columns_Comment(GTK_TREE_VIEW(treeview));

    // creating a model
    GtkTreeModel *model;
    model = CreateModel(treeview, pItem);

    // set view property //
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(treeview), TRUE);
    // gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(treeview), TRUE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
    //gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), TRUE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(treeview), TRUE);

    // selection handling \//
    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);
#if O
    g_signal_connect(select, "changed", G_CALLBACK(HandleTreeSelectionChanged), this);
    g_signal_connect(treeview, "row-expanded", G_CALLBACK(HandleTreeViewExpanded), this);
    g_signal_connect(treeview, "row-collapsed", G_CALLBACK(HandleTreeViewCollapse), this);
#endif
    //g_signal_connect(treeview, "row-active", G_CALLBACK(HandleTreeRowActivated), this);

    // gtk_tree_view_collapse_all(GTK_TREE_VIEW(treeview));
    gtk_tree_view_expand_all(GTK_TREE_VIEW(treeview));

    //    g_object_unref(model);
    return treeview;
}

*/

int ViewSystem::DepartmentIndex()
{
     int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
     gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

     if(!exam_type_name)
         return -1;

     char exam_type[256];
     ExamItem exam;
     exam.TransItemName(exam_type_name, exam_type);
     string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
     //printf("exam_type_name=%s, exam_type=%s, probe_exam=%s\n", exam_type_name, exam_type, probe_exam.c_str());

     char path[256];
     sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
     IniFile ini1(path);
     string username;
     username = exam.ReadDefaultUserSelect(&ini1);
     char path1[256];
     if(strcmp(username.c_str(), "System Default") == 0)
     {
        sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
   /* IniFile ini(path);
    IniFile *ptrIni= &ini;

     char path1[256];
     sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    */ IniFile ini(path1);
     IniFile *ptrIni= &ini;

     string department_string = ptrIni->ReadString(probe_exam.c_str(), "Department");
     const char *department = department_string.c_str();
     //printf("department=%s\n", department);
#ifdef VET
     if(strcmp(department, "Abdomen") ==0)
         return 0;
     else if(strcmp(department, "Urology") == 0)
         return 1;
     else if(strcmp(department, "Cardiac") == 0)
         return 2;
     else if(strcmp(department, "Obstetrics") == 0)
         return 3;
     else if(strcmp(department, "Small Part") == 0)
         return 4;
     else if(strcmp(department, "Vascular") == 0)
         return 5;
     else if(strcmp(department, "Orthopedic") == 0)
         return 6;
     else
         return 0;
#else
     if(strcmp(department, "Abdomen") ==0)
         return 0;
     else if(strcmp(department, "Urology") == 0)
         return 1;
     else if(strcmp(department, "Cardiac") == 0)
         return 2;
     else if(strcmp(department, "Obstetrics") == 0)
         return 3;
     else if(strcmp(department, "Gynecology") == 0)
        return 4;
    else if(strcmp(department, "Small Part") == 0)
        return 5;
#ifdef CT_322
     else if(strcmp(department, "Orthopedic") == 0)
        return 6;
#else
    else if(strcmp(department, "Vascular") == 0)
        return 6;
    else if(strcmp(department, "Orthopedic") == 0)
        return 7;
#endif
     else
        return 0;
#endif
}

void ViewSystem::DepartmentName(char department[256], int index)
{
#ifdef VET
    if(index == 0)
        strcpy(department, "Abdomen");
    else if(index == 1)
        strcpy(department, "Urology");
    else if(index == 2)
        strcpy(department, "Cardiac");
    else if(index == 3)
        strcpy(department, "Obstetrics");
    else if(index == 4)
        strcpy(department, "Small Part");
    else if(index == 5)
        strcpy(department, "Vascular");
    else if(index == 6)
        strcpy(department, "Orthopedic");
    else
        strcpy(department, "Tendon");
#else
    if(index == 0)
        strcpy(department, "Abdomen");
    else if(index == 1)
        strcpy(department, "Urology");
    else if(index == 2)
        strcpy(department, "Cardiac");
    else if(index == 3)
        strcpy(department, "Obstetrics");
    else if(index == 4)
        strcpy(department, "Gynecology");
    else if(index == 5)
        strcpy(department, "Small Part");
#ifdef CT_322
    else
        strcpy(department, "Orthopedic");
#else
    else if(index == 6)
        strcpy(department, "Vascular");
    else
        strcpy(department, "Orthopedic");
#endif
#endif
}

void ViewSystem::CreateItemList_Delete_Comment1(char *select_name, string probe_exam,vector<ExamPara>& vecItemComment1)
{

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    const char *probeExam = probe_exam.c_str();
    int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    SysGeneralSetting sysGS;
    for(int i=1;i<=number;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(probeExam, NoteNumber).c_str();
        exampara.index=ExamItem::USERNAME;
        if(sysGS.GetLanguage() ==ZH)
        {
            if(strcmp(select_name, _(exampara.name.c_str()))!=0)
                vecItemComment1.push_back(exampara);
        }
        else
        {
            if(strcmp(select_name, exampara.name.c_str())!=0)
                vecItemComment1.push_back(exampara);
        }
    }
}

void ViewSystem::CreateItemList_Delete_Comment(char *select_name, char *department, vector<ExamPara>& vecDeleteComment)
{
   char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ExamItem examitem;

      int number;
    number = ptrIni->ReadInt(department, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber).c_str();
        exampara.index=ExamItem::USERNAME;
        SysGeneralSetting sysGS;
        if(strcmp(select_name, _(exampara.name.c_str()))!=0)
            vecDeleteComment.push_back(exampara);
    }
}

void ViewSystem::CreateItemList_Comment1(string probe_exam,vector<ExamPara>& vecItemComment1)
{
       char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path1);
    ExamItem exam;
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni= &ini;

    const char *probeExam = probe_exam.c_str();

      int number;
    number = ptrIni->ReadInt(probeExam, "Number");

    if(number ==0)
        return;

    for(int i=1;i<=number;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
        exampara.dept_name="";
        exampara.name = ptrIni->ReadString(probeExam, NoteNumber).c_str();
        exampara.index=ExamItem::USERNAME;

        vecItemComment1.push_back(exampara);
    }
}

void ViewSystem::CreateItemList_Note(char *department,vector<ExamPara>& vecItemComment, int number)
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, DEFAULT_NOTE_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ExamItem examitem;

    for(int i=1;i<=number;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
       exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber).c_str();
       // exampara.index = ;
          exampara.index=ExamItem::USERNAME;

        vecItemComment.push_back(exampara);
    }
}

void ViewSystem::CreateItemList_Comment(char *department,vector<ExamPara>& vecItemComment, int number)
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    ExamItem examitem;

    for(int i=1;i<=number;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);

        ExamPara exampara;
       exampara.dept_name="";
        exampara.name = ptrIni->ReadString(department, NoteNumber).c_str();
       // exampara.index = ;
          exampara.index=ExamItem::USERNAME;

        vecItemComment.push_back(exampara);
    }
}

void ViewSystem::ProbeCommentChanged(GtkComboBox *widget)
{
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
    // int index = GetProbeComment();
    if (index == -1)
    {
        return;
    }
    ExamItem examItem;
    vector<int> itemIndex;
    examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));

    create_exam_comment_model(itemIndex);
}

void ViewSystem::ProbeTypeChanged(GtkComboBox *widget)
{
   int probeIndex = 0;
    int examIndex = 0;
    int i;
    char *itemName;
    char buf[50];
    int index = GetProbeType();
    if (index == -1)
    {
        return;
    }
    ExamItem examItem;
    vector<int> itemIndex;
    examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));

    GtkTreeModel *model = create_exam_item_model(itemIndex);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);

    GtkTreePath *path_tmp;
    PRINTF("path2222:%s\n", m_str_index);

    if(m_str_index == NULL)
    {
#ifdef VET
        path_tmp = gtk_tree_path_new_from_string("0");
#else
        path_tmp = gtk_tree_path_new_from_string("0:0");
#endif
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
    }
    else
    {
        path_tmp = gtk_tree_path_new_from_string(m_str_index);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
    }
    GetImageNoteSelection(probeIndex, examIndex,itemName);
    gchar* probeType;
    probeType = (char *)(PROBE_LIST[gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_type))].c_str());

    if ((probeIndex != -1) && (probeIndex < NUM_PROBE))
    {
        // update probe 2D freq
        ClearComboBox(GTK_COMBO_BOX(m_combobox_2d_freq));
        for (i = 0; i < ProbeSocket::FREQ2D_SUM[probeIndex]; i ++)
        {
            sprintf(buf , "%.1fMHz", (float)ProbeSocket::FREQ2D[probeIndex][i].emit / 20);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_2d_freq), buf);
        }
#if not defined(CT_322)
#if not defined(CT_313)
        // update doppler freq
        ClearComboBox(GTK_COMBO_BOX(m_combobox_pw_freq));
        for (i = 0; i < ProbeSocket::FREQ_DOPPLER_SUM[probeIndex]; i ++)
        {
            sprintf(buf , "%.1f", (float)ProbeSocket::FREQ_DOPPLER[probeIndex][i] / 20);
            gtk_combo_box_append_text (GTK_COMBO_BOX(m_combobox_pw_freq), buf);
        }
#endif
#endif
        ClearComboBox(GTK_COMBO_BOX(m_combobox_thi_freq));
        for (i = 0; i < ProbeSocket::FREQ_THI_SUM[probeIndex]; i ++)
        {
            if(ProbeSocket::FREQ_THI[i] != 0)
            {
                sprintf(buf , "%.1fMHz", (float)ProbeSocket::FREQ_THI[probeIndex][i] / 10);
                gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_thi_freq), buf);
            }
        }

    }

    if (probeType != NULL)
    {
        if (strcmp(probeType, "30P16A") == 0)
        {
            ClearComboBox(GTK_COMBO_BOX(m_combobox_space_compound));
            gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_space_compound), 0, _("OFF"));
        }
        else
        {
            ClearComboBox(GTK_COMBO_BOX(m_combobox_space_compound));
            gtk_combo_box_insert_text(GTK_COMBO_BOX(m_combobox_space_compound), 0, _("OFF"));
            for(i = 1; i <Img2D::MAX_SPACE_COMPOUND; i ++)
            {
                sprintf(buf, "%d", i);
                gtk_combo_box_append_text(GTK_COMBO_BOX(m_combobox_space_compound), buf);
            }
        }
    }
}

void ViewSystem::ShowList(const char *name)
{
    gtk_combo_box_append_text (GTK_COMBO_BOX(m_comboboxentry_user_select), name);
}

void ViewSystem::set_active_user(gint num)
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_user_select), num);
}

int ViewSystem::get_active_user(void)
{
    return gtk_combo_box_get_active(GTK_COMBO_BOX(m_comboboxentry_user_select));
}

const gchar* ViewSystem::GetUserName(void)
{
    return gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxentry_user_select))));

}

int ViewSystem::GetProbeType(void)
{
    return gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_type));
}

void ViewSystem::image_para_combo_box_set(GtkWidget *combobox, int value)
{
    if (value < 0)
        gtk_widget_set_sensitive(combobox, FALSE);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), value);
}

void ViewSystem::SetImagePara(const ExamItem::ParaItem &item)
{
    image_para_combo_box_set((m_combobox_mbp), item.common.MBP);
    image_para_combo_box_set((m_combobox_ao_power), item.common.powerIndex);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_2d_gain), (double)item.d2.gain2D);
    image_para_combo_box_set((m_combobox_agc), item.d2.AGC);
    image_para_combo_box_set((m_combobox_edge_enh), item.d2.edgeEnhance);
    image_para_combo_box_set((m_combobox_steer), item.d2.steerIndex);
    if(item.d2.focSum<=0)
        image_para_combo_box_set((m_combobox_focus_sum), (item.d2.focSum));
    else
        image_para_combo_box_set((m_combobox_focus_sum), (item.d2.focSum-1));
    if(item.d2.focPosIndex<0)
        image_para_combo_box_set((m_combobox_focus_pos), item.d2.focPosIndex+1);
    else
        image_para_combo_box_set((m_combobox_focus_pos), item.d2.focPosIndex);
    image_para_combo_box_set((m_combobox_depth), item.d2.imgScale);
    image_para_combo_box_set((m_combobox_chroma), item.d2.chroma);
    image_para_combo_box_set((m_combobox_lr), item.d2.leftRight);
    image_para_combo_box_set((m_combobox_ud), item.d2.upDown);
    //image_para_combo_box_set((m_combobox_rotation), item.d2.rotate);
    image_para_combo_box_set((m_combobox_polarity), item.d2.polarity);
    image_para_combo_box_set((m_combobox_frame), item.d2.frameAver);
    image_para_combo_box_set((m_combobox_line_aver), item.d2.lineAver);
    image_para_combo_box_set((m_combobox_restric), item.d2.noiseReject);
    image_para_combo_box_set((m_combobox_gray_trans), item.d2.grayTransIndex);
    image_para_combo_box_set((m_combobox_thi), item.d2.harmonic);
    image_para_combo_box_set((m_combobox_tsi), item.d2.TSI);
    image_para_combo_box_set((m_combobox_imgEnh), item.d2.imgEhn);
    image_para_combo_box_set((m_combobox_scan_range), item.d2.scanAngle);
    image_para_combo_box_set((m_combobox_dynamic_range), item.d2.dynamicRange);
    image_para_combo_box_set((m_combobox_2d_freq), item.d2.freqIndex);
    image_para_combo_box_set((m_combobox_thi_freq), item.d2.thiFreqIndex);
    image_para_combo_box_set((m_combobox_2d_line_density), item.d2.lineDensity);
    image_para_combo_box_set((m_combobox_2d_smooth), item.d2.smooth);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_m_gain), (double)item.d2.gainM);
    image_para_combo_box_set((m_combobox_space_compound), item.d2.spaceCompoundIndex);
    image_para_combo_box_set((m_combobox_freq_compound), item.d2.freqCompoundIndex);
#if not defined(CT_322)
#if not defined(CT_313)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_gain), (double)item.spectrum.gain);
    image_para_combo_box_set((m_combobox_pw_scale_prf), item.spectrum.PRF);
    image_para_combo_box_set((m_combobox_time_resolution), item.spectrum.timeSmooth);
    image_para_combo_box_set((m_combobox_pw_freq), item.spectrum.freq);
    image_para_combo_box_set((m_combobox_pw_wallfilter), item.spectrum.wallFilter);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_angle), (double)item.spectrum.correctAngle);
    image_para_combo_box_set((m_combobox_pw_invert), item.spectrum.invert);

    image_para_combo_box_set((m_combobox_cfm_line_density), item.color.lineDensity);
    image_para_combo_box_set((m_combobox_cfm_scale_prf), item.color.PRFIndex);
    image_para_combo_box_set((m_combobox_sensitivity), item.color.sensitive); //must before after prf index
    image_para_combo_box_set((m_combobox_cfm_wallfilter), item.color.wallFilter);//must after sensitive
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_cfm_gain), (double)item.color.gain);
    image_para_combo_box_set((m_combobox_variance), item.color.turb);
    image_para_combo_box_set((m_combobox_cfm_invert), item.color.invert);
    image_para_combo_box_set((m_combobox_persistence), item.color.persist);
    image_para_combo_box_set((m_combobox_cfm_smooth), item.color.smooth);
    image_para_combo_box_set((m_combobox_color_rejection), item.color.reject);
#endif
#endif
}

void ViewSystem::save_image_para(ExamItem::ParaItem &item,  char *itemName)
{
    const char* name = GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))
    {
        UserSelect::GetInstance()->write_username(m_comboboxentry_user_select, USERNAME_DB, name);
    }

    ExamItem examItem;
    int probeIndex = 0;
    int itemIndex = 0;
    ///>item 无效 lhm
    char *item_name = NULL;
    GetImageNoteSelection(probeIndex, itemIndex, item_name);
    PRINTF("probeIndex:%d itemIndex:%d itemName:%s\n", probeIndex, itemIndex, itemName);
    if(itemName == NULL)
        itemName = item_name;

    if ((probeIndex >= 0) && (itemIndex >= 0))
    {
        examItem.WriteExamItemPara(probeIndex, itemIndex, &item, itemName);
    }
}

void ViewSystem::GetImagePara(ExamItem::ParaItem &item)
{
    string gain;
    string angle;
    item.common.MBP = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_mbp));
    item.common.powerIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ao_power));
    // item.d2.gain2D = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_2d_gain));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_2d_gain));
    item.d2.gain2D = atoi(gain.c_str());
    item.d2.AGC = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_agc));
    item.d2.edgeEnhance = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_edge_enh));
    item.d2.steerIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_steer));
    item.d2.focSum = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_focus_sum)) + 1;
    item.d2.focPosIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_focus_pos));
    item.d2.imgScale = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_depth));
    item.d2.chroma = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_chroma));
    item.d2.leftRight = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_lr));
    item.d2.upDown = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ud));
    //item.d2.rotate = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_rotation));
    item.d2.polarity = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_polarity));
    item.d2.frameAver = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_frame));
    item.d2.lineAver = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_line_aver));
    item.d2.noiseReject = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_restric));
    item.d2.grayTransIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_gray_trans));
    item.d2.harmonic = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_thi));
    item.d2.TSI = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_tsi));
    item.d2.imgEhn = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_imgEnh));
    item.d2.scanAngle = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_scan_range));
    item.d2.dynamicRange = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_dynamic_range));
    item.d2.freqIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_2d_freq));
    item.d2.thiFreqIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_thi_freq));
    item.d2.lineDensity = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_2d_line_density));
    item.d2.smooth = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_2d_smooth));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_m_gain));
    item.d2.gainM = atoi(gain.c_str());
    item.d2.spaceCompoundIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_space_compound));
    item.d2.freqCompoundIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_freq_compound));
#if not defined(CT_322)
#if not defined(CT_313)
    // item.spectrum.gain = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_pw_gain));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_pw_gain));
    item.spectrum.gain = atoi(gain.c_str());
    item.spectrum.PRF = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_scale_prf));
    item.spectrum.timeSmooth = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_time_resolution));
    item.spectrum.freq = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_freq));
    item.spectrum.wallFilter = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_wallfilter));
    angle = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_pw_angle));
    item.spectrum.correctAngle = atoi(angle.c_str());
    item.spectrum.invert = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_invert));

    // item.color.gain = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_spinbutton_cfm_gain));
    gain = gtk_entry_get_text(GTK_ENTRY(m_spinbutton_cfm_gain));
    item.color.gain = atoi(gain.c_str());
    item.color.lineDensity = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_line_density));
    item.color.PRFIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_scale_prf));
    item.color.wallFilter = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));
    item.color.turb = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_variance));
    item.color.invert = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_invert));
    item.color.persist = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_persistence));
    item.color.smooth = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_smooth));
    item.color.reject = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_color_rejection));
    item.color.sensitive = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sensitivity));
#endif
#endif
}

void ViewSystem::image_para_restrict(int probeIndex)
{
    switch (probeIndex) {
        case 0:                     // 35C40K
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
            break;
        case 1:                     // 75L40K
            gtk_widget_set_sensitive(m_combobox_scan_range, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            break;
        case 2:                     // 65C10K
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            break;
        case 3:
#ifdef CT_430
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
#else
            // 35C20H
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
#endif
            break;
        case 4:
#if defined(CT_161)
            //30P16A
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            gtk_widget_set_sensitive(m_combobox_space_compound, FALSE);
#elif defined(CT_430)
            //35D50D
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
#elif defined(CT_440)
            //30P16A
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            gtk_widget_set_sensitive(m_combobox_space_compound, FALSE);
#else
            //65C15D
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
#endif
            break;
        case 5:
#ifdef CT_322
            //90L40J
            gtk_widget_set_sensitive(m_combobox_scan_range, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
#else
            // 30P16A
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            gtk_widget_set_sensitive(m_combobox_space_compound, FALSE);
#endif
            break;
        case 6:
#ifdef CT_340
            //90L25K
            gtk_widget_set_sensitive(m_combobox_scan_range, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
#else
            //35D40J
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
#endif
            break;
        case 7:
#ifdef CT_322
            //30P16A
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            gtk_widget_set_sensitive(m_combobox_space_compound, FALSE);
#else
            //10L25K
            gtk_widget_set_sensitive(m_combobox_scan_range, FALSE);
//            gtk_widget_set_sensitive(m_combobox_thi, FALSE);
#endif
            break;
        case 8:
            //35D40J
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
            break;
        case 9:
            //65C10H
            gtk_widget_set_sensitive(m_combobox_steer, FALSE);
            //gtk_widget_set_sensitive(m_combobox_thi, FALSE);
            break;
    }
}

void ViewSystem::save_itemIndex(int itemIndex)
{
    m_itemIndex = itemIndex;
}

void ViewSystem::save_itemName(char *itemName)
{
    m_itemName=itemName;

}

int ViewSystem::get_itemIndex(void)
{
    return m_itemIndex;
}

void ViewSystem::get_itemName(char *itemName)
{
     strcpy(itemName,m_itemName);
}

void ViewSystem::tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data)
{
    const char *str="";
    static char lastStr[30] = "";
    bool d_press = (bool)user_data;
    str = gtk_tree_path_to_string(path);
    if (d_press)
    {
        gtk_tree_view_collapse_row(tree_view,
                gtk_tree_path_new_from_string(str));
        return;
    }
    if((!strstr(str, ":")) && (strcmp(lastStr, str)))
    {
        if(strcmp(lastStr, ""))
            gtk_tree_view_collapse_row(tree_view,
                   gtk_tree_path_new_from_string(lastStr));
        strcpy(lastStr, str);
    }

    gtk_tree_view_expand_row(tree_view, path, FALSE);
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(tree_view), path, NULL, FALSE);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(tree_view), path, NULL, TRUE, 0.5, 0.5);
 }

gboolean ViewSystem::BtnExamDepartmentClicked(GtkWidget *widget, GdkEventButton *event)
{
    if (event->button == 3)
        return FALSE;
    else if (event->button == 2)
        return FALSE;
    else if (event->button == 1)
    {
        if (event->type == GDK_BUTTON_RELEASE)
        {
            GtkTreeIter iter;
            GtkTreeModel *model;

            GtkTreeSelection *selection;
            selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

            if (gtk_tree_selection_get_selected(selection, &model, &iter) == TRUE)
            {
                GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
                int tree_depth = gtk_tree_path_get_depth(path);
                gtk_tree_path_free (path);

                if(gtk_tree_model_iter_has_child(model, &iter))
                {
                    GtkTreePath *path=gtk_tree_model_get_path(model, &iter);
                    tree_auto_scroll(GTK_TREE_VIEW(widget), &iter, path, (gpointer)0);
                    gtk_tree_path_free(path);
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}

void ViewSystem::ExamTypeChanged(GtkTreeSelection *selection)
{
    set_image_item_sensitive(false);

    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
        return ;

    ExamItem::EItem itemIndex;
    gtk_tree_model_get(model, &iter,
            INDEX_COLUMN, &itemIndex,
            -1);
    char *itemName;
    gtk_tree_model_get(model, &iter,
            0, &itemName,
            -1);
    save_itemName(itemName);
    save_itemIndex(itemIndex);

    if (itemIndex != -1) {
        set_image_item_sensitive(true);

        int probeIndex = GetProbeType();
        if (probeIndex != -1) {
            ExamItem::ParaItem paraItem;
            ExamItem examItem;
            examItem.ReadExamItemPara(probeIndex, (int)itemIndex, &paraItem, itemName);
            SetImagePara(paraItem);
            image_para_restrict(probeIndex);
        }
    }
  //  else {
//      set_image_item_sensitive(false);
//}
}

bool ViewSystem::ExamTypeTestRowExpandBefore(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path)
{
    gtk_tree_view_collapse_all(tree_view);
    return FALSE; //必须要有返回值,才能触发事件
}

void ViewSystem::init_image_setting(void)
{
   string probe_type = TopArea::GetInstance()->GetProbeType();
    for (int i = 0; i < NUM_PROBE; ++i)
    {
        if (strcmp(probe_type.c_str(), PROBE_LIST[i].c_str()) == 0)
        {
            //char newProbeName[256];
            //ProbeMan::GetInstance()->VerifyProbeName(probe_type.c_str(), newProbeName);
            gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_type), i);
        }
    }
    set_image_item_sensitive(false);
}

void ViewSystem::init_image_para(void)
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_mbp), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ao_power), -1);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_2d_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_agc), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_edge_enh), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_steer), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_focus_sum), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_focus_pos), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_depth), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_chroma), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_lr), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ud), -1);
    //gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_rotation), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_polarity), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_frame), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_line_aver), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_restric), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_gray_trans), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_thi), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_tsi), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_imgEnh), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_scan_range), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_dynamic_range), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_2d_freq), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_thi_freq), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_2d_line_density), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_2d_smooth), -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_m_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_space_compound), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_freq_compound), -1);
#if not defined(CT_322)
#if not defined(CT_313)
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_scale_prf), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_time_resolution), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_freq), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_wallfilter), -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_pw_angle), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_pw_invert), -1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_line_density), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_scale_prf), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sensitivity), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter), -1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_spinbutton_cfm_gain), (double)0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_variance), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_invert), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_persistence), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_smooth), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_color_rejection), -1);
#endif
#endif
}

void ViewSystem::set_image_item_sensitive(bool status)
{
    m_imageItemSensitive = status;
    if (!status)
        init_image_para();

    gtk_widget_set_sensitive(m_combobox_mbp, status);
    gtk_widget_set_sensitive(m_combobox_ao_power, status);
    gtk_widget_set_sensitive(m_spinbutton_2d_gain, status);
    gtk_widget_set_sensitive(m_combobox_agc, status);
    gtk_widget_set_sensitive(m_combobox_edge_enh, status);
    gtk_widget_set_sensitive(m_combobox_steer, status);
    gtk_widget_set_sensitive(m_combobox_focus_sum, status);
    gtk_widget_set_sensitive(m_combobox_focus_pos, status);
    gtk_widget_set_sensitive(m_combobox_depth, status);
    gtk_widget_set_sensitive(m_combobox_chroma, status);
    gtk_widget_set_sensitive(m_combobox_lr, status);
    gtk_widget_set_sensitive(m_combobox_ud, status);
    //gtk_widget_set_sensitive(m_combobox_rotation, status);
    gtk_widget_set_sensitive(m_combobox_polarity, status);
    gtk_widget_set_sensitive(m_combobox_frame, status);
    gtk_widget_set_sensitive(m_combobox_line_aver, status);
    gtk_widget_set_sensitive(m_combobox_restric, status);
    gtk_widget_set_sensitive(m_combobox_gray_trans, status);
    gtk_widget_set_sensitive(m_combobox_thi, status);
    gtk_widget_set_sensitive(m_combobox_tsi, status);
    gtk_widget_set_sensitive(m_combobox_imgEnh, status);
    gtk_widget_set_sensitive(m_spinbutton_m_gain, status);
    gtk_widget_set_sensitive(m_combobox_scan_range, status);
    gtk_widget_set_sensitive(m_combobox_dynamic_range, status);
    gtk_widget_set_sensitive(m_combobox_2d_freq, status);
    gtk_widget_set_sensitive(m_combobox_thi_freq, status);
    gtk_widget_set_sensitive(m_combobox_2d_line_density, status);
    gtk_widget_set_sensitive(m_combobox_2d_smooth, status);
    gtk_widget_set_sensitive(m_combobox_space_compound, status);
    gtk_widget_set_sensitive(m_combobox_freq_compound, status);

#if not defined(CT_322)
#if not defined(CT_313)
    gtk_widget_set_sensitive(m_spinbutton_pw_gain, status);
    gtk_widget_set_sensitive(m_combobox_pw_scale_prf, status);
    gtk_widget_set_sensitive(m_combobox_time_resolution, status);
    gtk_widget_set_sensitive(m_combobox_pw_freq, status);
    gtk_widget_set_sensitive(m_combobox_pw_wallfilter, status);
    gtk_widget_set_sensitive(m_spinbutton_pw_angle, status);
    gtk_widget_set_sensitive(m_combobox_pw_invert, status);
    gtk_widget_set_sensitive(m_combobox_cfm_line_density, status);
    gtk_widget_set_sensitive(m_combobox_cfm_wallfilter, status);
    gtk_widget_set_sensitive(m_combobox_sensitivity, status);
    gtk_widget_set_sensitive(m_combobox_cfm_scale_prf, status);
    gtk_widget_set_sensitive(m_spinbutton_cfm_gain, status);
    gtk_widget_set_sensitive(m_combobox_variance, status);
    gtk_widget_set_sensitive(m_combobox_cfm_invert, status);
    gtk_widget_set_sensitive(m_combobox_persistence, status);
    gtk_widget_set_sensitive(m_combobox_cfm_smooth, status);
    gtk_widget_set_sensitive(m_combobox_color_rejection, status);
#endif
#endif
}

void ViewSystem::save_image_setting(void)
{
    const char* name = GetUserName();
    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))

    {
        UserSelect::GetInstance()->write_username(m_comboboxentry_user_select, USERNAME_DB, name);

    }

    ExamItem examItem;
    ExamItem::ParaItem paraItem;
    GetImagePara(paraItem);

    int probeIndex = 0;
    int itemIndex = 0;
    char *itemName = NULL;
    GetImageNoteSelection(probeIndex, itemIndex, itemName);
    if ((probeIndex >= 0) && (itemIndex >= 0))
    {
        examItem.WriteExamItemPara(probeIndex, itemIndex, &paraItem, itemName);
    }

}

void ViewSystem::CreatePrinter()
{
    GtkWidget* frame_print;
    GtkWidget *fixed_printer;
    GSList *radiobutton_printer_group = NULL;
    GtkWidget *scrolledwindow_specific;
    GtkWidget *label_print;
    static const int DIR_MAP = 20;

    frame_print = gtk_frame_new (NULL);
    gtk_widget_show (frame_print);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), frame_print, 20, 140 - 70 + 180 + 10 + 20);
    gtk_widget_set_size_request (frame_print, 615, 190);
    gtk_frame_set_label_align (GTK_FRAME (frame_print), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_print), GTK_SHADOW_IN);

    fixed_printer = gtk_fixed_new ();
    gtk_widget_show (fixed_printer);
    gtk_container_add (GTK_CONTAINER (frame_print), fixed_printer);

    m_radiobutton_common = gtk_radio_button_new_with_mnemonic (NULL, _("Common"));
    gtk_widget_show (m_radiobutton_common);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_radiobutton_common, 320 + 10 + 20 + 20 + 10 + DIR_MAP, 10);
    gtk_widget_set_size_request (m_radiobutton_common, 100, 30);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_common), radiobutton_printer_group);
    radiobutton_printer_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_common));
    g_signal_connect((gpointer)m_radiobutton_common, "toggled", G_CALLBACK (on_common_radio_button_toggled), this);

    m_radiobutton_specific = gtk_radio_button_new_with_mnemonic (NULL, _("Specific"));
    gtk_widget_show (m_radiobutton_specific);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_radiobutton_specific, 320 + 10 + 20 + 20 + 10 + DIR_MAP, 10 + 30 + 10);
    gtk_widget_set_size_request (m_radiobutton_specific, 100, 30);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_specific), radiobutton_printer_group);
    radiobutton_printer_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_specific));
    g_signal_connect((gpointer)m_radiobutton_specific, "toggled", G_CALLBACK (on_specific_radio_button_toggled), this);

    m_scrolledwindow_common = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (m_scrolledwindow_common, 320+20, 145);
    gtk_widget_show (m_scrolledwindow_common);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_scrolledwindow_common, 10 + DIR_MAP, 10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_scrolledwindow_common), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_scrolledwindow_common), GTK_SHADOW_IN);

    m_treeview_common_print = create_common_print_treeview();
    gtk_widget_show (m_treeview_common_print);
    m_selected_common_printer = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_common_print));
    gtk_tree_selection_set_mode(m_selected_common_printer, GTK_SELECTION_SINGLE);
    gtk_container_add (GTK_CONTAINER (m_scrolledwindow_common), m_treeview_common_print);

    m_fixed_specific = gtk_fixed_new ();
    gtk_widget_show (m_fixed_specific);
    gtk_widget_set_size_request (m_fixed_specific, 320+20, 170);
    gtk_fixed_put (GTK_FIXED (fixed_printer), m_fixed_specific, 10 + DIR_MAP, 10);

    scrolledwindow_specific = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_specific);
    gtk_fixed_put(GTK_FIXED(m_fixed_specific), scrolledwindow_specific, 5, 5);
    gtk_widget_set_size_request(scrolledwindow_specific, 320, 145);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_specific), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_specific), GTK_SHADOW_IN);

    m_treeview_specific_print = create_specific_print_treeview();
    gtk_widget_show (m_treeview_specific_print);
    m_selected_specific_printer = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_specific_print));
    gtk_tree_selection_set_mode(m_selected_specific_printer, GTK_SELECTION_SINGLE);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_specific), m_treeview_specific_print);

    GtkWidget *button_add_printer = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (button_add_printer);
    gtk_fixed_put (GTK_FIXED (m_fixed_specific), button_add_printer, 320 + 10 + 20 + 20 + 10, 30 + 10 + 30 + 10);
    gtk_widget_set_size_request (button_add_printer, 100+30, 30);
    g_signal_connect((gpointer)button_add_printer, "clicked", G_CALLBACK (on_button_add_printer_clicked), this);

    GtkWidget *button_del_printer = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (button_del_printer);
    gtk_fixed_put (GTK_FIXED (m_fixed_specific), button_del_printer, 320 + 10 + 20 + 20 + 10, 30 + 10 + 30 + 10 + 30 + 10);
    gtk_widget_set_size_request (button_del_printer, 100+30, 30);
    g_signal_connect((gpointer)button_del_printer, "clicked", G_CALLBACK (on_button_del_printer_clicked), this);

    label_print = gtk_label_new (_("<b>Printer</b>"));
    gtk_widget_show (label_print);
    gtk_frame_set_label_widget (GTK_FRAME (frame_print), label_print);
    gtk_label_set_use_markup (GTK_LABEL (label_print), TRUE);

    g_signal_connect((gpointer)m_selected_common_printer, "changed", G_CALLBACK (on_common_treeview_selection_changed), this);
    g_signal_connect((gpointer)m_selected_specific_printer, "changed", G_CALLBACK (on_specific_treeview_selection_changed), this);

}

void ViewSystem::init_printer(SysGeneralSetting *sysGeneralSetting)
{
    if(sysGeneralSetting == NULL)
        sysGeneralSetting = new SysGeneralSetting;
    if (sysGeneralSetting->GetPrinterMethod() == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_common), TRUE);
        gtk_widget_show_all(m_scrolledwindow_common);
        gtk_widget_hide_all(m_fixed_specific);
        char printer_path[3];
        int index_printer = sysGeneralSetting->GetPrinter();
        sprintf(printer_path, "%d", index_printer);
        GtkTreePath *path = gtk_tree_path_new_from_string(printer_path);
        gtk_tree_selection_select_path(m_selected_common_printer, path);
    } else if (sysGeneralSetting->GetPrinterMethod() == 1) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobutton_specific), TRUE);
        gtk_widget_hide_all(m_scrolledwindow_common);
        gtk_widget_show_all(m_fixed_specific);
    }
    delete sysGeneralSetting;
}

void ViewSystem::create_note_tvout(void)
{
    GtkWidget *label_video;
    GtkWidget *label_connector_type;
    GtkWidget *label_connector_hint;

    GtkWidget *frame_tvout_setting;
    GtkWidget *frame_tvout;
    GtkWidget *hbox_tvout;
    GSList *radiobutton_format_group = NULL;
    GtkWidget *vbox_format_1;
    GtkWidget *image_format_1;
    GtkWidget *label_format_1;
    GtkWidget *vbox_format_2;
    GtkWidget *image_format_2;
    GtkWidget *label_format_2;
    GtkWidget *vbox_format_3;
    GtkWidget *image_format_3;
    GtkWidget *label_format_3;
    GtkWidget *label_tvout_format;
    GtkWidget *button_default;
    static const int ADJUST_PIXEL_X = 30;
    static const int ADJUST_PIXEL_Y = 70;
    static const int FRAME_TVOUT_SETTING_W = DEFALT_SCREEN_WIDTH - 40 - 15;
    static const int FRAME_TVOUT_SETTING_H = 140 - ADJUST_PIXEL_Y + 180 - 30 + 10 ;
    label_video = gtk_label_new (_("<b>Video Standard:</b>"));

    gtk_widget_show (label_video);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), label_video, 50 - ADJUST_PIXEL_X, 30 - 5);
    gtk_widget_set_size_request (label_video, 125, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_video), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_video), 0.9, 0.5);

    m_combobox_video = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_video);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), m_combobox_video, 175 - ADJUST_PIXEL_X, 30 - 5);

    gtk_widget_set_size_request (m_combobox_video, 120, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video), _("NTSC"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_video), _("PAL"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video), 0);
#ifdef CT_355
     g_signal_connect ((gpointer) m_combobox_video, "changed",
                  G_CALLBACK (on_combobox_video_changed),
                  this);
#endif

#if 0
    label_connector_type = gtk_label_new (_("<b>TV Connector:</b>"));
    gtk_widget_show (label_connector_type);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), label_connector_type, 175 - ADJUST_PIXEL_X + 120 + 5, 30 - 5);

    gtk_widget_set_size_request (label_connector_type, 125, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_connector_type), TRUE);

    m_combobox_connect_type = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_connect_type);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), m_combobox_connect_type, 175 - ADJUST_PIXEL_X + 120 + 5 + 125, 30 - 5);

    gtk_widget_set_size_request (m_combobox_connect_type, 120, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_connect_type), _("S-Video"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_connect_type), _("Composite"));

    label_connector_hint = gtk_label_new (_(" (Restart to complete changes)"));

#ifdef CT3D
    gtk_widget_hide (label_connector_hint);
#else
    gtk_widget_show (label_connector_hint);
#endif
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), label_connector_hint, 175 - ADJUST_PIXEL_X + 120 + 5 + 125 + 120 + 5, 30 - 5);
    gtk_widget_set_size_request (label_connector_hint, 210 + 240, 30);
    gtk_misc_set_alignment (GTK_MISC (label_connector_hint), 0, 0.5);
#endif

    frame_tvout = gtk_frame_new (NULL);
    gtk_widget_show (frame_tvout);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), frame_tvout, 50 - ADJUST_PIXEL_X, 140 - ADJUST_PIXEL_Y);
    gtk_widget_set_size_request (frame_tvout, 600 + 15, 180);
    gtk_frame_set_label_align (GTK_FRAME (frame_tvout), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_tvout), GTK_SHADOW_IN);

    hbox_tvout = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_tvout);
    gtk_container_add (GTK_CONTAINER (frame_tvout), hbox_tvout);

    m_radiobutton_format_1 = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobutton_format_1);
    gtk_box_pack_start (GTK_BOX (hbox_tvout), m_radiobutton_format_1, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_format_1), radiobutton_format_group);
    radiobutton_format_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_format_1));

    vbox_format_1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_1);
    gtk_container_add (GTK_CONTAINER (m_radiobutton_format_1), vbox_format_1);

    image_format_1 = create_pixmap ("./res/btn_format/tv-out-1.png");
    gtk_widget_show (image_format_1);
    gtk_box_pack_start (GTK_BOX (vbox_format_1), image_format_1, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_1), 5, 10);

    label_format_1 = gtk_label_new (_("Full Screen"));
    gtk_widget_modify_fg(label_format_1, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_1, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_1);
    gtk_box_pack_start (GTK_BOX (vbox_format_1), label_format_1, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_1), TRUE);

    m_radiobutton_format_2 = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobutton_format_2);
    gtk_box_pack_start (GTK_BOX (hbox_tvout), m_radiobutton_format_2, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_format_2), radiobutton_format_group);
    radiobutton_format_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_format_2));

    vbox_format_2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_2);
    gtk_container_add (GTK_CONTAINER (m_radiobutton_format_2), vbox_format_2);

    image_format_2 = create_pixmap ("./res/btn_format/tv-out-2.png");
    gtk_widget_show (image_format_2);
    gtk_box_pack_start (GTK_BOX (vbox_format_2), image_format_2, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_2), 5, 10);

    label_format_2 = gtk_label_new (_("Hide Menu Area"));
    gtk_widget_modify_fg(label_format_2, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_2, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_2);
    gtk_box_pack_start (GTK_BOX (vbox_format_2), label_format_2, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_2), TRUE);

    m_radiobutton_format_3 = gtk_radio_button_new (NULL);
    gtk_widget_show (m_radiobutton_format_3);
    gtk_box_pack_start (GTK_BOX (hbox_tvout), m_radiobutton_format_3, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_format_3), radiobutton_format_group);
    radiobutton_format_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_format_3));

    vbox_format_3 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox_format_3);
    gtk_container_add (GTK_CONTAINER (m_radiobutton_format_3), vbox_format_3);

    image_format_3 = create_pixmap ("./res/btn_format/tv-out-3.png");
    gtk_widget_show (image_format_3);
    gtk_box_pack_start (GTK_BOX (vbox_format_3), image_format_3, TRUE, TRUE, 0);
    gtk_misc_set_padding (GTK_MISC (image_format_3), 5, 10);

    label_format_3 = gtk_label_new (_("Image Area Only"));
    gtk_widget_modify_fg(label_format_3, GTK_STATE_PRELIGHT, g_white);
    gtk_widget_modify_fg(label_format_3, GTK_STATE_ACTIVE, g_white);
    gtk_widget_show (label_format_3);
    gtk_box_pack_start (GTK_BOX (vbox_format_3), label_format_3, FALSE, FALSE, 0);
    gtk_label_set_use_markup (GTK_LABEL (label_format_3), TRUE);

    SysGeneralSetting sg;
    if (sg.GetConnectorType())
        label_tvout_format = gtk_label_new (_("<b>TV Out Format (Restart to complete changes)</b>"));
    else
        label_tvout_format = gtk_label_new (_("<b>TV Out Format</b>"));

    gtk_widget_show (label_tvout_format);
    gtk_frame_set_label_widget (GTK_FRAME (frame_tvout), label_tvout_format);
    gtk_label_set_use_markup (GTK_LABEL (label_tvout_format), TRUE);

    button_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_default);
    gtk_fixed_put (GTK_FIXED (m_fixed_tvout), button_default, 340+20+10+136+12+20+136+12+20, 415+20);
    gtk_widget_set_size_request (button_default, 140+8, 40);
    g_signal_connect ((gpointer) button_default, "clicked", G_CALLBACK (on_button_tvout_default_clicked), this);
}

void ViewSystem::init_tvout_setting(SysGeneralSetting *sysGeneralSetting)
{
    if (sysGeneralSetting == NULL)
        sysGeneralSetting = new SysGeneralSetting;

    int index_video_mode = sysGeneralSetting->GetVideoMode();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_video), index_video_mode);
#if 0
    int index_connector_type = sysGeneralSetting->GetConnectorType();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_connect_type), index_connector_type);
#endif

    int tvout_format = sysGeneralSetting->GetVideoFormat();
    switch (tvout_format) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_format_1), TRUE);
            break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_format_2), TRUE);
            break;
        case 2:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_format_3), TRUE);
            break;
    }
    delete sysGeneralSetting;
}

void ViewSystem::save_tvout_setting(void)
{
    SysGeneralSetting sysGeneralSetting;

#if 0
    int ConnectorType = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_connect_type));
    sysGeneralSetting.SetConnectorType(ConnectorType);
#endif

    int VideoModeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_video));
    int tvout_format = 0;
    string command;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_1))) {
        tvout_format = 0;
        command = "--mode 1024x768 --pos 0x0";
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_2))) {
        tvout_format = 1;
        command = "--mode 848x660 --pos 180x0";
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_format_3))) {
        tvout_format = 2;
        command = "--mode 848x560 --pos 180x100";
    }

    if(sysGeneralSetting.GetVideoMode() != VideoModeIndex || sysGeneralSetting.GetVideoFormat() != tvout_format) {
        if (VideoModeIndex) {
            //command = "xrandr --output TV --set TV_FORMAT NTSC-M " + command;
            command = "xrandr --output TV1 --set mode NTSC-M " + command;
        } else {
            //command = "xrandr --output TV --set TV_FORMAT PAL " + command;
            command = "xrandr --output TV1 --set mode PAL " + command;
        }

        if(!sysGeneralSetting.GetConnectorType()) {
            //system(command.c_str());
            _system_(command.c_str());
        }

        sysGeneralSetting.SetVideoMode(VideoModeIndex);
        sysGeneralSetting.SetVideoFormat(tvout_format);
    }
    //save printer
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_common))) {
        int printIndex = common_printer_selection();
        if (printIndex != -1) {
            sysGeneralSetting.SetPrinterMethod(0);
            sysGeneralSetting.SetPrinter(printIndex);
        }
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_specific))) {
        string default_print = specific_printer_selection();
        if (!default_print.empty()) {
            Printer print;
            sysGeneralSetting.SetPrinterMethod(1);
            print.SetDefault(default_print.c_str());
            PeripheralMan::GetInstance()->SwitchPrinterDriver();
            update_specific_printer_model();
        }
    }

    sysGeneralSetting.SyncFile();
}

void ViewSystem::BtnTVOutDefaultClicked(GtkButton *button)
{
    SysGeneralSetting *sysGeneralSetting = new SysGeneralSetting;

    char printer_path[3];
    int index_printer = sysGeneralSetting->GetPrinter();
    sprintf(printer_path, "%d", index_printer);
    GtkTreePath *path = gtk_tree_path_new_from_string(printer_path);
    gtk_tree_selection_select_path(m_selected_common_printer, path);

    sysGeneralSetting->TVOutDefaultFactory();
    init_tvout_setting(sysGeneralSetting);
}

GtkWidget* ViewSystem::create_note_calc_measure(void)
{
    GtkWidget *fixed_calc_measure;
    GtkWidget *fixed_calc;
    GtkWidget *label_calc;
    GtkWidget *fixed_measure;
    GtkWidget *label_measure;

    fixed_calc_measure = gtk_fixed_new ();
    gtk_widget_show (fixed_calc_measure);

    m_calc_notebook = gtk_notebook_new();
    gtk_fixed_put (GTK_FIXED (fixed_calc_measure), m_calc_notebook, 40, 20);
    gtk_widget_set_size_request (m_calc_notebook, 850, 470);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (m_calc_notebook),GTK_POS_LEFT);
    gtk_widget_show (m_calc_notebook);

    int calc_notebook_page = 0;

    fixed_calc = CalcSetting::GetInstance()->CreateCalcWindow(m_window);
    gtk_container_add (GTK_CONTAINER (m_calc_notebook), fixed_calc);

    label_calc = gtk_label_new (_("Calculate"));
    gtk_widget_show (label_calc);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_calc_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_calc_notebook), calc_notebook_page++), label_calc);

    fixed_measure = MeasureSetting::GetInstance()->CreateMeasureWindow(m_window);
    gtk_container_add (GTK_CONTAINER (m_calc_notebook), fixed_measure);

    label_measure = gtk_label_new (_("Measure"));
    gtk_widget_show (label_measure);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_calc_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_calc_notebook), calc_notebook_page++), label_measure);

    g_signal_connect(G_OBJECT(m_calc_notebook), "switch-page", G_CALLBACK(on_calc_notebook_switch_page), this);

    return fixed_calc_measure;
}

GtkWidget* ViewSystem::create_note_dicom(void)
{
    GtkWidget *fixed_dicom;
    GtkWidget *fixed_local;
    GtkWidget *label_local;

    GtkWidget *fixed_server;
    GtkWidget *label_server;

    GtkWidget *fixed_service;
    GtkWidget *label_service;

    fixed_dicom = gtk_fixed_new ();
    gtk_widget_show (fixed_dicom);

    m_dicom_notebook = gtk_notebook_new ();
    gtk_fixed_put (GTK_FIXED (fixed_dicom), m_dicom_notebook, 40, 20);
    gtk_widget_set_size_request (m_dicom_notebook, 850, 470);
    // gtk_widget_set_size_request (m_notebook, w - 40, 540);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (m_dicom_notebook),GTK_POS_LEFT);
    gtk_widget_show (m_dicom_notebook);

    int dicom_notebook_page = 0;

    // create note local
   // fixed_local = Create_note_local();
    fixed_local = DicomLocalSetting::GetInstance()->CreateDicomWindow(m_window);
    gtk_container_add (GTK_CONTAINER (m_dicom_notebook), fixed_local);
    DicomLocalSetting::GetInstance()->init_local_setting();

    label_local = gtk_label_new (_("Local"));
    gtk_widget_show (label_local);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_dicom_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_dicom_notebook), dicom_notebook_page++), label_local);

    // create note server
    fixed_server = DicomServerSetting::GetInstance()->CreateDicomWindow(m_window);
    gtk_container_add (GTK_CONTAINER (m_dicom_notebook), fixed_server);
   // DicomServerSetting::GetInstance()-> init_server_setting();

    label_server = gtk_label_new (_("Server"));
    gtk_widget_show (label_server);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_dicom_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_dicom_notebook), dicom_notebook_page++), label_server);

    //create note service
    fixed_service = DicomServiceSetting::GetInstance()->CreateDicomWindow(m_window);
    gtk_container_add (GTK_CONTAINER (m_dicom_notebook), fixed_service);
   // DicomServiceSetting::GetInstance()->init_service_setting();

    label_service = gtk_label_new (_("Service"));
    gtk_widget_show (label_service);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (m_dicom_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (m_dicom_notebook), dicom_notebook_page++), label_service);

    g_signal_connect(G_OBJECT(m_dicom_notebook), "switch-page", G_CALLBACK(on_dicom_notebook_switch_page), this);

    return fixed_dicom;
}

void ViewSystem::DicomnotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
    m_page_num = page_num;
    switch(page_num)
    {
        case 0:
            DicomLocalSetting::GetInstance()->init_local_setting();
            break;
        case 1:
            DicomServerSetting::GetInstance()-> init_server_setting();
            break;
        case 2:
            DicomServiceSetting::GetInstance()->init_service_setting();
            break;
        default:
            break;
    }
}

void ViewSystem::CalcnotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num)
{
}
void ViewSystem::VgaChanged(GtkComboBox *box)
{
  int indexVga= gtk_combo_box_get_active(box);
    FpgaCtrl2D* fpga=new FpgaCtrl2D();
    fpga->VGADisplay((INT16U)(indexVga));
    if (0==indexVga)
        m_vgaInterl=true;
    else
        m_vgaInterl=false;
    delete fpga;
}

void ViewSystem::LanguageChanged(GtkComboBox *box)
{
  int indexLang = gtk_combo_box_get_active(box);
  if (indexLang == ZH)
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), 0);  //set date format to y:m:d)
  else
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), 2);  //set date format to d:m:y)
}

#if 0
void ViewSystem::init_dicom_setting(void)
{
#if 0
    SysDicomSetting *sysDicomSetting = new SysDicomSetting;

    string local_ip, local_netmask, local_gateway, local_AE;
    string remote_ip, remote_port, remote_AE;

    sysDicomSetting->GetLocalIP(local_ip);
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_ip), local_ip.c_str());
    sysDicomSetting->GetLocalNetMask(local_netmask);
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_mask), local_netmask.c_str());
    sysDicomSetting->GetLocalGateWay(local_gateway);
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_gateway), local_gateway.c_str());
    sysDicomSetting->GetLocalAE(local_AE);
    gtk_entry_set_text(GTK_ENTRY(m_entry_equipment_ae), local_AE.c_str());
    sysDicomSetting->GetRemoteIP(remote_ip);
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_dicom), remote_ip.c_str());
    sysDicomSetting->GetRemotePort(remote_port);
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_port), remote_port.c_str());
    sysDicomSetting->GetRemoteAE(remote_AE);
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_ae), remote_AE.c_str());

    delete sysDicomSetting;
#endif
}

void ViewSystem::save_dicom_setting(void)
{
#if 0
    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetLocalIP(gtk_entry_get_text(GTK_ENTRY(m_entry_network_ip)));
    sysDicomSetting.SetLocalNetMask(gtk_entry_get_text(GTK_ENTRY(m_entry_network_mask)));
    sysDicomSetting.SetLocalGateWay(gtk_entry_get_text(GTK_ENTRY(m_entry_network_gateway)));
    sysDicomSetting.SetLocalAE(gtk_entry_get_text(GTK_ENTRY(m_entry_equipment_ae)));
    sysDicomSetting.SetRemoteIP(gtk_entry_get_text(GTK_ENTRY(m_entry_host_dicom)));
    sysDicomSetting.SetRemotePort(gtk_entry_get_text(GTK_ENTRY(m_entry_host_port)));
    sysDicomSetting.SetRemoteAE(gtk_entry_get_text(GTK_ENTRY(m_entry_host_ae)));
    sysDicomSetting.SyncFile();
#endif
}
#endif

GtkWidget* ViewSystem::create_note_comment(void)
{
    //GtkWidget *fixed_comment;
    GtkWidget *frame_comment_font;
    GtkWidget *hbox_comment_font;
    GSList *radiobutton_font_group = NULL;
    GtkWidget *label_comment_font;
    GtkWidget *frame_bodymark_size;
    GtkWidget *hbox_bodymark_size;
    GSList *radiobutton_bodymark_group = NULL;
    GtkWidget *label_bodymark_size;
    GtkWidget *section_frame;

    GtkWidget *button_comment_default;
    GtkWidget *label_comment_color;
    GtkWidget *label_bodymark_color;
    GtkWidget *frame_fixed_comment;
    fixed_comment = gtk_fixed_new ();
    gtk_widget_show (fixed_comment);

    GtkWidget *label_probe_comment = gtk_label_new (_("Probe Type: "));
    gtk_misc_set_alignment (GTK_MISC(label_probe_comment), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_probe_comment), TRUE);
    gtk_widget_show (label_probe_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), label_probe_comment, 10+20+20, 10+10+5);
    gtk_widget_set_size_request (label_probe_comment, 100, 30);

    int probe_comment_index = 0;
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;

    string default_probe;
    default_probe=exam.ReadDefaultProbe(&ini);
    for (int j = 0; j < NUM_PROBE; j++)
    {
        if(strcmp(default_probe.c_str(), PROBE_LIST[j].c_str())==0)
        {
            probe_comment_index = j;
            break;
        }
        else
            probe_comment_index = 0;
    }

    m_combobox_probe_comment = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_probe_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_probe_comment, 110+20+20+20-30, 10+10+5);
    gtk_widget_set_size_request (m_combobox_probe_comment, 120, 30);
    for (int i = 0; i < NUM_PROBE; i++) {
/*
         char newProbeName[256];
        ProbeMan::GetInstance()->VerifyProbeName(PROBE_LIST[i].c_str(), newProbeName);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_probe_comment), newProbeName);
*/
        char path[100] = {"0"};
        sprintf(path, "%s/%s", CFG_RES_PATH, PROBE_ALIAS_FILE);
        IniFile ini(path);
        char alias[56] = {"0"};
        ProbeMan::GetInstance()->ReadProbeAlias(&ini, PROBE_LIST[i].c_str(), alias);
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_probe_comment), alias);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_comment), probe_comment_index);//-1);
    g_signal_connect(m_combobox_probe_comment, "changed", G_CALLBACK(HandleProbeCommentChanged), this);

    GtkWidget *label_exam_comment = gtk_label_new (_("Exam Type:"));
    gtk_misc_set_alignment (GTK_MISC(label_exam_comment), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_exam_comment), TRUE);
    gtk_widget_show (label_exam_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), label_exam_comment, 200+200-100+30-5-17, 10+10+5);
    gtk_widget_set_size_request (label_exam_comment, 100+17, 30);

    m_combobox_exam_comment = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_exam_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_exam_comment, 300+210-100-30+40-5, 10+10+5);
    gtk_widget_set_size_request (m_combobox_exam_comment, 120, 30);

    ExamItem examItem;
    vector<int> itemIndex;
    examItem.GetItemListOfProbe((char*)PROBE_LIST[probe_comment_index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
    create_exam_comment_model(itemIndex);
    //gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_comment), 0);//-1);
    g_signal_connect(m_combobox_exam_comment, "changed", G_CALLBACK(HandleExamCommentChanged), this);

    int department_index = DepartmentIndex();
    GtkWidget *label_department_comment = gtk_label_new (_("Select Lexicon:"));
    gtk_misc_set_alignment (GTK_MISC(label_department_comment), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_department_comment), TRUE);
    gtk_widget_show (label_department_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), label_department_comment, 90, 10+20+50-10);//300+310-50+40, 10+10+5);
    gtk_widget_set_size_request (label_department_comment, 100, 30);

    m_combobox_department_comment = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_department_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_department_comment, 190, 10+20+50-10);//300+310+150+10-100+30, 10+10+5);
    gtk_widget_set_size_request (m_combobox_department_comment, 120, 30);

    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Abdomen"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Urology"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Cardiac"));

    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Obstetrics"));
#ifndef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Gynecology"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Small Part"));
#ifndef CT_322
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Vascular"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Orthopedic"));

#ifdef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Tendon"));

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), 0);//0);

#else
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), department_index);//0);
#endif
    g_signal_connect(m_combobox_department_comment, "changed", G_CALLBACK(HandleDepartmentCommentChanged), this);

    section_frame = gtk_frame_new (NULL);
    gtk_widget_show (section_frame);
    gtk_fixed_put (GTK_FIXED (fixed_comment), section_frame, 50, 100-40);
    gtk_widget_set_size_request (section_frame, 600+200, 488-244+30+20+30-10);
    gtk_frame_set_shadow_type (GTK_FRAME (section_frame), GTK_SHADOW_IN);

    scrolledwindow_item_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), scrolledwindow_item_comment, 50+40, 10+20+50-5+40-5);
    gtk_widget_set_size_request (scrolledwindow_item_comment, 120+100, 200+63+20-40+10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkTreeModel *model = create_item_comment_model(0);
    m_treeview_item_comment = gtk_tree_view_new_with_model(model);
    add_columns_comment(GTK_TREE_VIEW(m_treeview_item_comment));

    gtk_widget_modify_base(m_treeview_item_comment, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_comment), m_treeview_item_comment);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    g_object_set(m_cellrenderer_comment_text, "editable", TRUE, NULL);
    g_signal_connect(m_cellrenderer_comment_text, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(m_cellrenderer_comment_text, "edited", G_CALLBACK(HandleCellRendererRenameSelectComment), this);

    gtk_widget_show (m_treeview_item_comment);

    scrolledwindow_item_comment1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_comment1);
    gtk_fixed_put (GTK_FIXED (fixed_comment), scrolledwindow_item_comment1, 50+40+220+40+80+40, 10+20+50-5+40-5);//-3-2);
    gtk_widget_set_size_request (scrolledwindow_item_comment1, 120+100, 200+63+20-40+10);//200+63+20+5+3+2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_comment1), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    GtkTreeModel *model1 = create_item_comment_model1();
    m_treeview_item_comment1 = gtk_tree_view_new_with_model(model1);
    add_columns_comment1(GTK_TREE_VIEW(m_treeview_item_comment1));

    gtk_widget_modify_base(m_treeview_item_comment1, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_comment1), m_treeview_item_comment1);

    GtkTreeSelection *select1;
    select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    g_object_set(m_cellrenderer_comment_text1, "editable", TRUE, NULL);
    g_signal_connect(m_cellrenderer_comment_text1, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(m_cellrenderer_comment_text1, "edited", G_CALLBACK(HandleCellRendererRenameComment), this);
    gtk_widget_show (m_treeview_item_comment1);

   m_button_select_one = gtk_button_new_with_mnemonic (_(">"));
    gtk_widget_show (m_button_select_one);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_select_one, 350-15, 137);//70+30+20+30-5);
    gtk_widget_set_size_request (m_button_select_one, 80+30, 30);
    g_signal_connect(m_button_select_one, "clicked", G_CALLBACK(HandleButtonSelectOneCommentClicked), this);

    m_button_select_all = gtk_button_new_with_mnemonic (_(">>"));
    gtk_widget_show (m_button_select_all);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_select_all, 350-15, 194);//140+20+100-30);
    gtk_widget_set_size_request (m_button_select_all, 80+30, 30);
    g_signal_connect(m_button_select_all, "clicked", G_CALLBACK(HandleButtonSelectAllCommentClicked), this);

    m_button_comment_add = gtk_button_new_with_mnemonic (_("Add"));
    gtk_widget_show (m_button_comment_add);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_comment_add, 350-15, 251);//130-50+40+200+220+112+50+10, 70+58+58+58+5);//130);
    gtk_widget_set_size_request (m_button_comment_add, 80+30, 30);
    g_signal_connect(m_button_comment_add, "clicked", G_CALLBACK(HandleButtonAddClicked), this);

     m_button_comment_delete_select = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (m_button_comment_delete_select);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_comment_delete_select, 350-15, 308);//260+110+40-25-20, 158-63+38+38+38+20+20);//130-50+40+200+220+112+50+10, 70+58+58+58+5);//130);
    gtk_widget_set_size_request (m_button_comment_delete_select, 80+30, 30);
    g_signal_connect(m_button_comment_delete_select, "clicked", G_CALLBACK(HandleButtonDeleteSelectClicked), this);

    m_button_comment_up = gtk_button_new_with_mnemonic (_("Up"));
    gtk_widget_show (m_button_comment_up);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_comment_up, 730-15, 137);//180);
    gtk_widget_set_size_request (m_button_comment_up, 80+30, 30);
    g_signal_connect(m_button_comment_up, "clicked", G_CALLBACK(HandleButtonUpClicked), this);

    m_button_comment_down = gtk_button_new_with_mnemonic (_("Down"));
    gtk_widget_show (m_button_comment_down);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_comment_down, 730-15, 194);//230);
    gtk_widget_set_size_request (m_button_comment_down, 80+30, 30);
    g_signal_connect(m_button_comment_down, "clicked", G_CALLBACK(HandleButtonDownClicked), this);

       m_button_comment_delete = gtk_button_new_with_mnemonic (_("Delete"));
    gtk_widget_show (m_button_comment_delete);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_comment_delete, 730-15, 251);//260+110+40-25-20, 158-63+38+38+38+20+20);//180);
    gtk_widget_set_size_request (m_button_comment_delete, 80+30, 30);
    g_signal_connect(m_button_comment_delete, "clicked", G_CALLBACK(HandleButtonDeleteClicked), this);

    m_button_comment_delete_all = gtk_button_new_with_mnemonic (_("All Delete"));
    gtk_widget_show (m_button_comment_delete_all);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_button_comment_delete_all,  730-15, 308);//260+110+40-25-20, 158-63+38+38+38+38+20+20);//230);
    gtk_widget_set_size_request (m_button_comment_delete_all, 80+30, 30);
    g_signal_connect(m_button_comment_delete_all, "clicked", G_CALLBACK(HandleButtonDeleteAllClicked), this);

    frame_comment_font = gtk_frame_new (NULL);
    gtk_widget_show (frame_comment_font);
    gtk_fixed_put (GTK_FIXED (fixed_comment), frame_comment_font, 50, 30+330+10+30-10);
    gtk_widget_set_size_request (frame_comment_font, 240, 70-20);
    gtk_frame_set_label_align (GTK_FRAME (frame_comment_font), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_comment_font), GTK_SHADOW_IN);

    hbox_comment_font = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_comment_font);
    gtk_container_add (GTK_CONTAINER (frame_comment_font), hbox_comment_font);

    m_radiobutton_font_big = gtk_radio_button_new_with_mnemonic (NULL, _("Big"));
    gtk_widget_show (m_radiobutton_font_big);
    gtk_box_pack_start (GTK_BOX (hbox_comment_font), m_radiobutton_font_big, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_font_big), radiobutton_font_group);
    radiobutton_font_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_font_big));

    m_radiobutton_font_mid = gtk_radio_button_new_with_mnemonic (NULL, _("Mid"));
    gtk_widget_show (m_radiobutton_font_mid);
    gtk_box_pack_start (GTK_BOX (hbox_comment_font), m_radiobutton_font_mid, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_font_mid), radiobutton_font_group);
    radiobutton_font_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_font_mid));

    m_radiobutton_font_small = gtk_radio_button_new_with_mnemonic (NULL, _("Small"));
    gtk_widget_show (m_radiobutton_font_small);
    gtk_box_pack_start (GTK_BOX (hbox_comment_font), m_radiobutton_font_small, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_font_small), radiobutton_font_group);
    radiobutton_font_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_font_small));

    label_comment_font = gtk_label_new (_("<b>Font Size</b>"));
    gtk_widget_show (label_comment_font);
    gtk_frame_set_label_widget (GTK_FRAME (frame_comment_font), label_comment_font);
    gtk_label_set_use_markup (GTK_LABEL (label_comment_font), TRUE);

    frame_bodymark_size = gtk_frame_new (NULL);
    gtk_widget_show (frame_bodymark_size);
    gtk_fixed_put (GTK_FIXED (fixed_comment), frame_bodymark_size, 50, 420+10+30-10);

    gtk_widget_set_size_request (frame_bodymark_size, 240, 70-20);
    gtk_frame_set_label_align (GTK_FRAME (frame_bodymark_size), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_bodymark_size), GTK_SHADOW_IN);

    hbox_bodymark_size = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_bodymark_size);
    gtk_container_add (GTK_CONTAINER (frame_bodymark_size), hbox_bodymark_size);

    m_radiobutton_bodymark_big = gtk_radio_button_new_with_mnemonic (NULL, _("Big"));
    gtk_widget_show (m_radiobutton_bodymark_big);
    gtk_box_pack_start (GTK_BOX (hbox_bodymark_size), m_radiobutton_bodymark_big, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_bodymark_big), radiobutton_bodymark_group);
    radiobutton_bodymark_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_bodymark_big));

    m_radiobutton_bodymark_mid = gtk_radio_button_new_with_mnemonic (NULL, _("Mid"));
    gtk_widget_show (m_radiobutton_bodymark_mid);
    gtk_box_pack_start (GTK_BOX (hbox_bodymark_size), m_radiobutton_bodymark_mid, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_bodymark_mid), radiobutton_bodymark_group);
    radiobutton_bodymark_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_bodymark_mid));

    m_radiobutton_bodymark_small = gtk_radio_button_new_with_mnemonic (NULL, _("Small"));
    gtk_widget_show (m_radiobutton_bodymark_small);
    gtk_box_pack_start (GTK_BOX (hbox_bodymark_size), m_radiobutton_bodymark_small, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_bodymark_small), radiobutton_bodymark_group);
    radiobutton_bodymark_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_bodymark_small));

    label_bodymark_size = gtk_label_new (_("<b>Bodymark Size</b>"));
    gtk_widget_show (label_bodymark_size);
    gtk_frame_set_label_widget (GTK_FRAME (frame_bodymark_size), label_bodymark_size);
    gtk_label_set_use_markup (GTK_LABEL (label_bodymark_size), TRUE);

    label_comment_color = gtk_label_new (_("Font Color: "));
    gtk_widget_show (label_comment_color);
   // gtk_fixed_put (GTK_FIXED (fixed_comment), label_comment_color, 300, 50);
    gtk_fixed_put (GTK_FIXED (fixed_comment), label_comment_color, 370, 50+330+10+30-10);

    gtk_widget_set_size_request (label_comment_color, 130+40+20+20, 30);

    m_combobox_comment_color = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_comment_color);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_comment_color, 430+12, 50);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_comment_color, 500+40+20+20, 50+330+10+30-10);
    gtk_widget_set_size_request (m_combobox_comment_color, 130, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_comment_color), _("Red"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_comment_color), _("Green"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_comment_color), _("Blue"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_comment_color), _("White"));

    label_bodymark_color = gtk_label_new (_("Probe Mark Color: "));
    gtk_widget_show (label_bodymark_color);
   // gtk_fixed_put (GTK_FIXED (fixed_comment), label_bodymark_color, 300, 160);
    gtk_fixed_put (GTK_FIXED (fixed_comment), label_bodymark_color, 370, 160+330-50+30-10);
    gtk_widget_set_size_request (label_bodymark_color, 130+40+20+20, 30);

    m_combobox_bodymark_color = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_bodymark_color);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_bodymark_color, 430+12, 160);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_bodymark_color, 500+40+20+20,100+330+10+30-10);
    gtk_widget_set_size_request (m_combobox_bodymark_color, 130, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bodymark_color), _("White"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bodymark_color), _("Gray"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bodymark_color), _("Red"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bodymark_color), _("Green"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bodymark_color), _("Yellow"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bodymark_color), _("Blue"));

    button_comment_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_hide (button_comment_default);
    //gtk_fixed_put (GTK_FIXED (fixed_comment), button_comment_default, 30, 450);
    gtk_fixed_put (GTK_FIXED (fixed_comment), button_comment_default, 50, 450+50);
    gtk_widget_set_size_request (button_comment_default, 140+8, 40);
    g_signal_connect ((gpointer) button_comment_default, "clicked", G_CALLBACK (on_button_comment_default_clicked), this);
    return fixed_comment;
}

int ViewSystem::GetCommentProbeType(void)
{
    return  gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
}

void ViewSystem::ButtonSelectOneCommentClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    gboolean isSelect = FALSE;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a item before add!"), NULL);
        return;
    }

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);

    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

    if(!exam_type_name)
        return;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
    vector<ExamPara> vecItem_Comment;
    vecItem_Comment.clear();
    CreateItemList_Comment1(probe_exam, vecItem_Comment);

    int item_length(0);
    item_length = vecItem_Comment.size();
    if(!vecItem_Comment.empty())
    {
        for(int i=0; i<item_length; i++)
        {

            SysGeneralSetting sysGS;
            if(sysGS.GetLanguage() ==ZH)
            {
                if( strcmp(select_name, _(vecItem_Comment[i].name.c_str())) == 0)
                {

                    GtkTreeModel *model_tmp;
                    GtkTreeSelection *selection;
                    GtkTreeIter iter_tmp;
                    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
                    model_tmp= gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment1));
                    //高亮要插入的词条，并更新滚动条
                    iter_tmp= InsertUniqueComment(model_tmp, select_name);
                    gtk_tree_selection_select_iter(selection, &iter_tmp);

                    GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
                    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment1), path_scroll, NULL, FALSE, 1.0, 1.0);
                    gtk_tree_path_free (path_scroll);

                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _(" Item has existed. Please select item again!"), NULL);
                    return;
                }
            }
            else
            {

                if( strcmp(select_name, vecItem_Comment[i].name.c_str()) == 0)
                {

                    GtkTreeModel *model_tmp;
                    GtkTreeSelection *selection;
                    GtkTreeIter iter_tmp;
                    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
                    model_tmp= gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment1));
                    //高亮要插入的词条，并更新滚动条
                    iter_tmp= InsertUniqueComment(model_tmp, select_name);
                    gtk_tree_selection_select_iter(selection, &iter_tmp);

                    GtkTreePath *path_scroll = gtk_tree_model_get_path(model_tmp, &iter_tmp);
                    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment1), path_scroll, NULL, FALSE, 1.0, 1.0);
                    gtk_tree_path_free (path_scroll);

                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _(" Item has existed. Please select item again!"), NULL);
                    return;

                }
            }

        }
    }

    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni = &ini;
/*
    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile ini(path1);
    IniFile *ptrIni= &ini;
  */  int number;
    char SelectNum[256];
    number = ptrIni->ReadInt(probe_exam.c_str(), "Number");

    sprintf(SelectNum, "Note%d",number+1);
    ptrIni->WriteString(probe_exam.c_str(), SelectNum, select_name);
    ptrIni->WriteInt(probe_exam.c_str(), "Number", number+1);
    ptrIni->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_comment_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), new_model1);
//高亮插入的词条，并更新滚动条
    GtkTreeSelection *new_selection;
    GtkTreeIter iter_new;
    new_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
    iter_new= InsertUniqueComment(new_model1, select_name);
    gtk_tree_selection_select_iter(new_selection, &iter_new);

    GtkTreePath *path_scroll = gtk_tree_model_get_path(new_model1, &iter_new);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment1), path_scroll, NULL, TRUE, 1.0, 1.0);
    gtk_tree_path_free (path_scroll);

}

void ViewSystem::ButtonSelectAllCommentClicked(GtkButton *button)
{
    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

    if(!exam_type_name)
        return;
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
      char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini11(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini11);
    char path[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path);
    IniFile *ptrIni = &ini;

  /*  char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
    */const char* ptrSection = probe_exam.c_str();

    int Num;
    Num = ptrIni->ReadInt(ptrSection, "Number");

    vector<ExamPara> vecSelect_Comment;
    vecSelect_Comment.clear();
    CreateItemList_Comment1(ptrSection, vecSelect_Comment);

    int select_length(0);
    select_length = vecSelect_Comment.size();

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile new_ini(path1);
    IniFile *new_ptrIni= &new_ini;
    int department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
    char department_name[256];
    DepartmentName(department_name, department_index);
    int number;
    number = new_ptrIni->ReadInt(department_name, "Number");

    for(int i=1;i<=number;i++)
    {
        int writeNote=1;
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i);
        string Note_name;
        Note_name = new_ptrIni->ReadString(department_name, NoteNumber);

        if(!vecSelect_Comment.empty())
        {
            for(int j=0; j<select_length; j++)
            {
                SysGeneralSetting sysGS;
                if(sysGS.GetLanguage() ==ZH)
                {
                    if(strcmp(_(Note_name.c_str()), vecSelect_Comment[j].name.c_str()) ==0)
                    {
                        writeNote=0;
                    }
                }
                else
                {
                    if(strcmp(Note_name.c_str(), vecSelect_Comment[j].name.c_str()) ==0)
                    {
                        writeNote=0;
                    }
                }
            }
        }

        if(writeNote)
        {
            char NoteNum[256];
            sprintf(NoteNum, "Note%d", ++Num);
            SysGeneralSetting sysGS;
            if(sysGS.GetLanguage() ==ZH)
            {
                ptrIni->WriteString(ptrSection, NoteNum, _(Note_name.c_str()));
            }else
            {
                ptrIni->WriteString(ptrSection, NoteNum, Note_name.c_str());
            }
        }

    }
    ptrIni->WriteInt(ptrSection, "Number", Num);
    ptrIni->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_comment_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), new_model1);

}

void ViewSystem::ButtonDownClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a item before down!"), NULL);
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
int path_num = atoi(path_string);

    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
     vector<ExamPara> vecDelete_Comment;
    vecDelete_Comment.clear();
    CreateItemList_Comment1(probe_exam, vecDelete_Comment);

    int item_length(0);
    item_length = vecDelete_Comment.size();

    int path_total = item_length-1;
    if(path_num != path_total)
    {
        char path11[256];
        sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path11);
        ExamItem exam;
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni_comment = &ini;

        /* char path1[256];
        sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
        IniFile ini(path1);
        IniFile *ptrIni_comment= &ini;
*/
        ptrIni_comment->RemoveGroup(probe_exam.c_str());
        ptrIni_comment->SyncConfigFile();

        swap(vecDelete_Comment[path_num], vecDelete_Comment[path_num+1]);

        for(int i=0;i<item_length;i++)
        {
            char NoteNumber[256];
            sprintf(NoteNumber, "Note%d", i+1);
            ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i].name.c_str());
        }

        ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", item_length);
        ptrIni_comment->SyncConfigFile();
        //更新树
        GtkTreeModel *new_model1 = create_item_comment_model1();
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), new_model1);

        g_free(path_string);
        gtk_tree_path_next(path);
        gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment1), path, NULL, TRUE);
        //在词条下移时，更新滚动条
    /*    gdouble value = gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));
        gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)), value+18);
        gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));
     */
        if(item_length-path_num >2)
            gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment1), path, NULL, TRUE, 1.0, 1.0);//移动后的选中词条置底
        else
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));//保证在最下端的词条移动后能够正常显示
        gtk_tree_path_free (path);
    }
}

void ViewSystem::ButtonUpClicked(GtkButton *button)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a item before up!"), NULL); //请先选择待插入结点的父结点
        return;
    }
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int path_num = atoi(path_string);
    PRINTF("---path_string:%s %d\n",path_string,path_num);

    if(path_num == 0)
        return;

    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));
    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;

     vector<ExamPara> vecDelete_Comment;
     vecDelete_Comment.clear();
     CreateItemList_Comment1(probe_exam, vecDelete_Comment);

     int item_length(0);
     item_length = vecDelete_Comment.size();

 char path11[256];
        sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path11);
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni_comment = &ini;

   /*  char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile ini(path1);
    IniFile *ptrIni_comment= &ini;
*/
     ptrIni_comment->RemoveGroup(probe_exam.c_str());
     ptrIni_comment->SyncConfigFile();

     swap(vecDelete_Comment[path_num], vecDelete_Comment[path_num-1]);

     for(int i=0;i<item_length;i++)
     {
         char NoteNumber[256];
         sprintf(NoteNumber, "Note%d", i+1);
         ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i].name.c_str());
     }

     ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", item_length);
     ptrIni_comment->SyncConfigFile();

     GtkTreeModel *new_model1 = create_item_comment_model1();
     gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), new_model1);

     g_free(path_string);
     if(gtk_tree_path_prev(path))
     {
         gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment1), path, NULL, TRUE);
        //在词条上移时，更新滚动条
         /*gdouble value = gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));
           gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)), value-18);
           gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));
          */
         if(item_length-path_num >12)
             gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment1), path, NULL, TRUE, 0.0, 1.0);// 移动后选中词条置顶
         else
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));//保证在最上端的词条移动时能够正确显示
     }
     gtk_tree_path_free (path);
}

/**
 * @brief: 光标改变，隐藏注册信息
 */
void ViewSystem::TreeCursorChanged(GtkTreeView *treeview)
{
    gtk_label_set_text(GTK_LABEL(m_labelExport),"");
    gtk_label_set_text(GTK_LABEL(m_labelRegister),"");
    gtk_entry_set_text(GTK_ENTRY(m_entryRegisterKey), "");
    gtk_widget_hide(m_frameRegisterInfo);
}

/**
 * @brief: pop menu
 */
int ViewSystem::OptionOptEvent(GtkWidget *widget, GdkEvent *event)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
    bool selected = gtk_tree_selection_get_selected(selection, &model, &iter);
    if(!selected)
        return FALSE;
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    gtk_tree_path_free (path);

    //create popup-menu
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *installItem = gtk_menu_item_new_with_label(_("Install"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), installItem);
    g_signal_connect(G_OBJECT (installItem), "activate", G_CALLBACK(HandleMenuInstallActivate), this);
    gtk_widget_show(installItem);

    GtkWidget *uninstallItem = gtk_menu_item_new_with_label(_("Uninstall"));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), uninstallItem);
    g_signal_connect(G_OBJECT (uninstallItem), "activate", G_CALLBACK(HandleMenuUninstallActivate), this);
    gtk_widget_show(uninstallItem);

    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row]))
        gtk_widget_set_sensitive(installItem, false);
    else
        gtk_widget_set_sensitive(uninstallItem, false);

    if (event->type == GDK_BUTTON_PRESS ) {
        GdkEventButton *bevent = (GdkEventButton *) event;
        gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,
                bevent->button, bevent->time);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief:注册/安装选配,获取选中配件的节点
 */
void ViewSystem::MenuInstallActivate(GtkMenuItem *menuitem)
{
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeOption));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    bool selected = gtk_tree_selection_get_selected(selection, &model, &m_optionIter);

    gtk_widget_show(m_frameRegisterInfo);
}

/**
 * @brief: 注销/卸载选配 直接删除密钥文件，同时设置标识为false
 */
void ViewSystem::MenuUninstallActivate(GtkMenuItem *menuitem)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeOption));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    bool selected = gtk_tree_selection_get_selected(selection, &model, &iter);
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);

    gchar *optionName = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
            0, _(CManRegister::Optional[row].c_str()),
            1, _("Disabled(* Restart to take effect)"),
            -1);
    gtk_tree_path_free (path);
    m_powerOffFlag = true;

    ///>移除文件
    if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row]))
    {
        char filePath[256];
        sprintf(filePath, "%s/%s", OPTIONAL_DIR, CManRegister::Optional[row].c_str());
        if(remove(filePath) == -1)
            PRINTF("remove %s file error: %s\n", CManRegister::Optional[row].c_str(),  strerror(errno));
    }
}

/**
 * @brief: 更改选配类表中选配功能的状态
 */
void ViewSystem::UpdateOptionStatus(bool status)
{
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    GtkTreePath *path = gtk_tree_model_get_path(model, &m_optionIter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    if(status)
        gtk_list_store_set(GTK_LIST_STORE(model), &m_optionIter,
                0, _(CManRegister::Optional[row].c_str()),
                1, _("Installed(* Restart to take effect)"),
                -1);
    else
        gtk_list_store_set(GTK_LIST_STORE(model), &m_optionIter,
                0, _(CManRegister::Optional[row].c_str()),
                1, _("Disabled(* Restart to take effect)"),
                -1);

    gtk_tree_path_free (path);
    m_powerOffFlag = true;
}

/**
 * @brief: export license file to usb
 */
void ViewSystem::BtnExportClicked(GtkButton *button)
{
    PeripheralMan *ptr = PeripheralMan::GetInstance();
    char info1[256];
    if(!ptr->CheckUsbStorageState())
    {
        sprintf(info1,"%s",_("No USB storage found!"));
        gtk_label_set_text(GTK_LABEL(m_labelExport),info1);
        gtk_widget_show (m_labelExport);

        return;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            sprintf(info1,"%s",_("Failed to mount USB storage!"));
            gtk_label_set_text(GTK_LABEL(m_labelExport),info1);
            gtk_widget_show (m_labelExport);
            return;
        }
    }

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
    GtkTreePath *path = gtk_tree_model_get_path(model, &m_optionIter);
    char *path_string = gtk_tree_path_to_string(path);
    int row = atoi(path_string);
    gtk_tree_path_free (path);

    if(row < 0 || row >= CManRegister::MAX_OPTIONAL_NUM)
        return;
    string destFileDir = UDISK_PATH;
    bool value(false);
    if(row == 0)
    {
        if(CDCMRegister::GetMe()==NULL)
            return;
        value = CDCMRegister::GetMe()->GenerateLicenseFile(destFileDir);
    }
    else
    {
        if(CManRegister::GetInstance()==NULL)
            return;
        value = CManRegister::GetInstance()->GenerateLicenseFile(destFileDir, CManRegister::Optional[row]);
    }
    char info[256];
    if(value)
    {
        sprintf(info,"%s",_("Succeed to export file!"));
    }
    else
    {
        sprintf(info,"%s",_("Fail to export file!"));
    }
    gtk_label_set_text(GTK_LABEL(m_labelExport),info);
    gtk_widget_show (m_labelExport);

    ptr->UmountUsbStorage();
}

/**
 * @brief: 校验序列号，验证注册是否成功
 */
void ViewSystem::BtnRegisterClicked(GtkButton *button)
{
   const gchar *registerKey = gtk_entry_get_text(GTK_ENTRY(m_entryRegisterKey));
   if(CManRegister::GetInstance() == NULL)
       return;

   GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeOption));
   GtkTreePath *path = gtk_tree_model_get_path(model, &m_optionIter);
   char *path_string = gtk_tree_path_to_string(path);
   int row = atoi(path_string);
   gtk_tree_path_free (path);

   if(row < 0 || row >= CManRegister::MAX_OPTIONAL_NUM)
       return;

    bool value(false);
    if(row == 0)
    {
        value =CDCMRegister::GetMe()->CheckAuthorize(registerKey);
        CManRegister::GetInstance()->CheckAuthorize(registerKey, row);
    }
   else
       value =CManRegister::GetInstance()->CheckAuthorize(registerKey, row);

   char info[256];
   if(value)
   {
       sprintf(info,"%s",_("Succeed to register!"));
   }
   else
   {
       sprintf(info,"%s",_("Fail to register!"));
   }
   gtk_label_set_text(GTK_LABEL(m_labelRegister),info);
   gtk_widget_show (m_labelRegister);

   ///>设置选配状态
   if(!CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row]) && value)
       UpdateOptionStatus(value);
}

void ViewSystem::ButtonDeleteClicked(GtkButton *button)
{

    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment1));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a item before delete!"), NULL); //请先选择待插入结点的父结点
        return;
    }
  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);

    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

    if(!exam_type_name)
        return;

    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
    char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path1[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini(path1);
    IniFile *ptrIni_comment = &ini;

    const char* ptrSection = probe_exam.c_str();

     vector<ExamPara> vecDelete_Comment;
    vecDelete_Comment.clear();
    CreateItemList_Delete_Comment1(select_name, probe_exam, vecDelete_Comment);

    int item_length(0);
    item_length = vecDelete_Comment.size();
    string department_copy;
    department_copy= ptrIni_comment->ReadString(probe_exam.c_str(), "Department");
    ptrIni_comment->RemoveGroup(probe_exam.c_str());
    ptrIni_comment->SyncConfigFile();

    for(int i=0;i<item_length;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i+1);
        ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i].name.c_str());
    }

    ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", item_length);
       ptrIni_comment->WriteString(probe_exam.c_str(), "Department", department_copy.c_str());
    ptrIni_comment->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_comment_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), new_model1);
//设置光标，更新滚动条的值
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment1), path, NULL, TRUE);
    gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment1)));
    //gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment1), path, NULL, FALSE, 1.0, 1.0);
    gtk_tree_path_free (path);
}

void ViewSystem::ButtonDeleteSelectClicked(GtkButton *button)
{

 GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                _("Please select a item before delete!"), NULL); //请先选择待插入结点的父结点
        return;
    }
  GtkTreePath *path = gtk_tree_model_get_path(model, &iter);

    char* select_name;
    gtk_tree_model_get(model, &iter, 0, &select_name, -1);
   int  department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
   char department[256];
   DepartmentName(department, department_index);
   char path2[256];
    sprintf(path2, "%s%s", CFG_RES_PATH, DEFAULT_NOTE_FILE);
    IniFile ini(path2);
    IniFile *ptrIni_comp= &ini;
    int number;
    number = ptrIni_comp->ReadInt(department, "Number");
    vector<ExamPara> vecItemNote;
    vecItemNote.clear();
    CreateItemList_Note(department, vecItemNote, number);

    int note_item_length(0);
    note_item_length = vecItemNote.size();

    if(!vecItemNote.empty())
    {
        for(int i=0; i<note_item_length; i++)
        {
            SysGeneralSetting sysGS;
            if(sysGS.GetLanguage() ==ZH)
            {

                if( strcmp(select_name, _(vecItemNote[i].name.c_str())) == 0)
                {
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _(" Only Userdefined items can be deleted!"), NULL);
                    return;
                }
            }
            else
            {
                if( strcmp(select_name, vecItemNote[i].name.c_str()) == 0)
                {
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _(" Only Userdefined items can be deleted!"), NULL);
                    return;
                }
            }
        }
    }

     char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile new_ini(path1);
    IniFile *ptrIni_comment= &new_ini;

     vector<ExamPara> vecDeleteComment;
    vecDeleteComment.clear();
    CreateItemList_Delete_Comment(select_name, department,vecDeleteComment);

    int item_length(0);
    item_length = vecDeleteComment.size();
    ptrIni_comment->RemoveGroup(department);
    ptrIni_comment->SyncConfigFile();

    for(int i=0;i<item_length;i++)
    {
        char NoteNumber[256];
        sprintf(NoteNumber, "Note%d", i+1);
        ptrIni_comment->WriteString(department, NoteNumber, vecDeleteComment[i].name.c_str());
    }

    ptrIni_comment->WriteInt(department, "Number", item_length);
    ptrIni_comment->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_comment_model(0);
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment), new_model1);
//设置光标，更新滚动条的值
    gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_item_comment), path, NULL, TRUE);
    gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment)));
    //   gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment), path, NULL, FALSE, 1.0, 1.0);

    gtk_tree_path_free (path);

}

void ViewSystem::ButtonAddClicked(GtkButton *button)
{
    char *new_string = _("New Item");
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
   selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));

    char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile new_ini(path1);
    IniFile *ptrIni= &new_ini;
    int number;
    int department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));
    char department[256];
    DepartmentName(department, department_index);
    number = ptrIni->ReadInt(department, "Number");
    vector<ExamPara> vecItemComment;
    vecItemComment.clear();
    CreateItemList_Comment(department, vecItemComment, number);

    int item_length(0);
    item_length = vecItemComment.size();
    if(!vecItemComment.empty())
    {
        for(int i=0; i<item_length; i++)
        {

            SysGeneralSetting sysGS;
            if(sysGS.GetLanguage() ==ZH)
            {
                if( strcmp(new_string, _(vecItemComment[i].name.c_str())) == 0)
                {

                    GtkTreeIter iter_tmp = InsertUniqueComment(model, new_string);
                    gtk_tree_selection_select_iter(selection, &iter_tmp);
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _(" 'New Item' has existed. Please rename item !"), NULL);
                    return;
                }
            }
            else
            {
                if( strcmp(new_string, vecItemComment[i].name.c_str()) == 0)
                {

                    GtkTreeIter iter_tmp = InsertUniqueComment(model, new_string);
                    gtk_tree_selection_select_iter(selection, &iter_tmp);
                    ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _(" 'New Item' has existed. Please rename item !"), NULL);
                    return;
                }

            }
        }
    }
    gtk_tree_selection_get_selected(selection, &model, &iter);
    if(gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
   {

        char SelectNum[256];

        sprintf(SelectNum, "Note%d",number+1);

        ptrIni->WriteString(department, SelectNum, new_string);
        ptrIni->WriteInt(department, "Number", number+1);
        ptrIni->SyncConfigFile();

        GtkTreeIter iter_new;
        gtk_tree_store_append(GTK_TREE_STORE(model), &iter_new, NULL);
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter_new, 0, new_string, -1);
        GtkTreePath *new_path = gtk_tree_model_get_path(model, &iter_new);

        //刷新treeview
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment),  create_item_comment_model(0));
        gtk_tree_view_expand_to_path(GTK_TREE_VIEW(m_treeview_item_comment), new_path);
        //设置结点为编辑状态

        GtkTreeViewColumn *column_tree_view;
        column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeview_item_comment), 0);

        gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeview_item_comment),
                new_path,
                column_tree_view,
                m_cellrenderer_comment_text,
                TRUE);
        //更新滚动条到底部
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment), new_path, NULL, TRUE, 1.0, 1.0);

        gtk_tree_path_free (new_path);
   }
    else
    {
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        char *path_string = gtk_tree_path_to_string(path);
        int path_num = atoi(path_string);
        ptrIni->RemoveGroup(department);
        ptrIni->SyncConfigFile();
        ExamPara exampara1;
        exampara1.dept_name="";
        exampara1.name = new_string;
        exampara1.index=ExamItem::USERNAME;
        vecItemComment.insert(vecItemComment.begin()+path_num+1, exampara1);
        for(int i=0;i<item_length+1;i++)
        {
            char NoteNumber[256];
            sprintf(NoteNumber, "Note%d", i+1);
            ptrIni->WriteString(department, NoteNumber, vecItemComment[i].name.c_str());
        }

        ptrIni->WriteInt(department, "Number", item_length+1);
        ptrIni->SyncConfigFile();

        GtkTreeModel *new_model1 = create_item_comment_model(0);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment), new_model1);

        g_free(path_string);
        gtk_tree_path_next(path);
        GtkTreeViewColumn *column_tree_view;
        column_tree_view = gtk_tree_view_get_column(GTK_TREE_VIEW(m_treeview_item_comment), 0);

        gtk_tree_view_set_cursor_on_cell(GTK_TREE_VIEW(m_treeview_item_comment),
                path,
                column_tree_view,
                m_cellrenderer_comment_text,
                TRUE);

        //更新滚动条
        /*  gdouble value = gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment)));
            gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment)), value+21);
            gtk_adjustment_value_changed(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledwindow_item_comment)));
         */
        gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(m_treeview_item_comment), path, NULL, FALSE, 1.0, 1.0);
        gtk_tree_path_free (path);
    }
}

void ViewSystem::ButtonDeleteAllClicked(GtkButton *button)
{
    int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
    gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));
    if(!exam_type_name)
        return;

    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);
    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
   /* char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile ini(path);
    IniFile *ptrIni= &ini;
  */
     char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini1(path);
        string username;
        username = exam.ReadDefaultUserSelect(&ini1);
        char path1[256];
        if(strcmp(username.c_str(), "System Default") == 0)
        {
            sprintf(path1, "%s%s", CFG_RES_PATH, COMMENT_FILE);
        }
        else
        {
            sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
        }
        IniFile ini(path1);
        IniFile *ptrIni = &ini;

        const char* ptrSection = probe_exam.c_str();
        string department_copy;
        department_copy= ptrIni->ReadString(ptrSection, "Department");

        ptrIni->RemoveGroup(ptrSection);

        ptrIni->WriteInt(ptrSection, "Number", 0);
        ptrIni->WriteString(ptrSection, "Department", department_copy.c_str());
        ptrIni->SyncConfigFile();

    GtkTreeModel *new_model1 = create_item_comment_model1();
    gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_item_comment1), new_model1);

}

void ViewSystem::CellRendererRenameSelectComment(GtkCellRendererText *m_cellrenderer_comment_text, gchar *path_str, gchar *new_text)
{
    char *old_text;

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment));
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &old_text, -1);
    gtk_tree_path_free(path);

    //如果重命名前后renderertext中的文本没有改变,则返回
    if (strcmp(new_text, old_text) == 0)
    {

        return;
    }
    if (strcmp(new_text, "") == 0) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("The new item name can not be empty!"), NULL); //结点名称不能为空

        return;
    }

    int department_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_department_comment));

    if(department_index ==-1)
        return;

    char department[256];
    DepartmentName(department, department_index);
    char path2[256];
    sprintf(path2, "%s%s", CFG_RES_PATH, DEFAULT_NOTE_FILE);
    IniFile ini(path2);
    IniFile *ptrIni_comp= &ini;
    int num;
    num= ptrIni_comp->ReadInt(department, "Number");
    vector<ExamPara> vecItemNote;
    vecItemNote.clear();
    CreateItemList_Note(department, vecItemNote, num);

    int note_item_length(0);
    note_item_length = vecItemNote.size();

    if(!vecItemNote.empty())
    {
        for(int i=0; i<note_item_length; i++)
        {
            if(( strcmp(old_text, vecItemNote[i].name.c_str()) == 0)||(strcmp(old_text, _(vecItemNote[i].name.c_str())) == 0))
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _("Only Userdefined items can be renamed!"), NULL);
                return;
            }

        }
    }
      char path1[256];
    sprintf(path1, "%s%s", CFG_RES_PATH, NOTE_FILE);
    IniFile new_ini(path1);
    IniFile *ptrIni= &new_ini;
    int number;
    number = ptrIni->ReadInt(department, "Number");
    vector<ExamPara> vecItemComment;
    vecItemComment.clear();
    CreateItemList_Comment(department, vecItemComment, number);

    int item_length(0);
    item_length = vecItemComment.size();

    int Num_tmp;

    if(!vecItemComment.empty())
    {
        for(int i=0; i<item_length; i++)
        {
            if( strcmp(new_text, vecItemComment[i].name.c_str()) == 0)
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _("Fail to rename! The item name has existed!"), NULL);
                return;
            }

        }

        for(int i=0; i<item_length; i++)
        {
            if( strcmp(old_text, vecItemComment[i].name.c_str()) == 0)
            {
                Num_tmp=i+1;
            }
        }

    }
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);

    vector<ExamPara> vecDelete_Comment;
    vecDelete_Comment.clear();
    CreateItemList_Delete_Comment(old_text, department, vecDelete_Comment);

    int delete_length(0);
    delete_length = vecDelete_Comment.size();
    ptrIni->RemoveGroup(department);
    ptrIni->SyncConfigFile();

    for(int i=1;i<number;i++)
    {
        char NoteNumber[256];
        if(Num_tmp > i)
        {
            sprintf(NoteNumber, "Note%d", i);
        ptrIni->WriteString(department, NoteNumber, vecDelete_Comment[i-1].name.c_str());

        }
        else
        {
            sprintf(NoteNumber, "Note%d", i+1);
        ptrIni->WriteString(department, NoteNumber, vecDelete_Comment[i-1].name.c_str());
        }
    }
    char New_Note[256];
    sprintf(New_Note, "Note%d", Num_tmp);
    ptrIni->WriteString(department, New_Note, new_text);
    ptrIni->WriteInt(department, "Number", number);
    ptrIni->SyncConfigFile();

}

void ViewSystem::CellRendererRenameComment(GtkCellRendererText *m_cellrenderer_comment_text1, gchar *path_str, gchar *new_text)
{
    char *old_text;

    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_item_comment1));
    GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 0, &old_text, -1);
    gtk_tree_path_free(path);

    if (strcmp(new_text, old_text) == 0)
    {

        return;
    }
    if (strcmp(new_text, "") == 0) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR, _("The new item name can not be empty!"), NULL); //结点名称不能为空

        return;
    }

  int index1 = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));

  gchar* exam_type_name = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_exam_comment));

    if(!exam_type_name)
        return;

    char exam_type[256];
    ExamItem exam;
    exam.TransItemName(exam_type_name, exam_type);

    string probe_exam = PROBE_LIST[index1] + "-" +exam_type;
    vector<ExamPara> vecItem_Comment;
    vecItem_Comment.clear();
    CreateItemList_Comment1(probe_exam, vecItem_Comment);

    int item_length(0);
    item_length = vecItem_Comment.size();

    int Num_tmp;

    if(!vecItem_Comment.empty())
    {
        for(int i=0; i<item_length; i++)
        {
            if( strcmp(new_text, vecItem_Comment[i].name.c_str()) == 0)
            {
                ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO,  _("Fail to rename! The item name has existed!"), NULL);
                return;
            }

        }

        for(int i=0; i<item_length; i++)
        {
            if( strcmp(old_text, vecItem_Comment[i].name.c_str()) == 0)
            {
                Num_tmp=i+1;
            }
        }

    }
    gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, new_text, -1);

       char path11[256];
    sprintf(path11, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini1(path11);
    string username;
    username = exam.ReadDefaultUserSelect(&ini1);
    char path_comment[256];
    if(strcmp(username.c_str(), "System Default") == 0)
    {
        sprintf(path_comment, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    }
    else
    {
        sprintf(path_comment, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, username.c_str(), ".ini");
    }
    IniFile ini_comment(path_comment);
    IniFile *ptrIni_comment= &ini_comment;
    /*  char path_comment[256];

    sprintf(path_comment, "%s%s", CFG_RES_PATH, COMMENT_FILE);
    IniFile ini_comment(path_comment);
    IniFile *ptrIni_comment= &ini_comment;
 */
    int DeleteNum;
    DeleteNum = ptrIni_comment->ReadInt(probe_exam.c_str(), "Number");

    vector<ExamPara> vecDelete_Comment;
    vecDelete_Comment.clear();
    CreateItemList_Delete_Comment1(old_text, probe_exam, vecDelete_Comment);

    int delete_length(0);
    delete_length = vecDelete_Comment.size();
    ptrIni_comment->RemoveGroup(probe_exam.c_str());
    ptrIni_comment->SyncConfigFile();

    for(int i=1;i<DeleteNum;i++)
    {
        char NoteNumber[256];
        if(Num_tmp > i)
        {
            sprintf(NoteNumber, "Note%d", i);
        ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i-1].name.c_str());

        }
        else
        {
            sprintf(NoteNumber, "Note%d", i+1);
        ptrIni_comment->WriteString(probe_exam.c_str(), NoteNumber, vecDelete_Comment[i-1].name.c_str());
        }
    }
    char New_Note[256];
    sprintf(New_Note, "Note%d", Num_tmp);
    ptrIni_comment->WriteString(probe_exam.c_str(), New_Note, new_text);
    ptrIni_comment->WriteInt(probe_exam.c_str(), "Number", DeleteNum);
    ptrIni_comment->SyncConfigFile();

}

void ViewSystem::init_comment_setting(SysNoteSetting* sysNoteSetting)
{
    if (sysNoteSetting == NULL) {
        sysNoteSetting = new SysNoteSetting;
    }

    int index_font_size = sysNoteSetting->GetFontSize();
    switch (index_font_size) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_font_big), TRUE);
            break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_font_mid), TRUE);
            break;
        case 2:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_font_small), TRUE);
            break;
    }
    int index_bdmk_size = sysNoteSetting->GetBodyMarkSize();
    switch (index_bdmk_size) {
        case 2:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_big), TRUE);
            break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_mid), TRUE);
            break;
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_small), TRUE);
            break;
    }

    int index_font_color = sysNoteSetting->GetFontColor();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_comment_color), index_font_color);

    int index_bdmk_color = sysNoteSetting->GetBodyMarkColor();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_bodymark_color), index_bdmk_color);

    delete sysNoteSetting;
}

void ViewSystem::save_comment_setting(void)
{
    SysNoteSetting sysNoteSetting;
    unsigned char fontSizeIndex = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_font_big)))
        fontSizeIndex = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_font_mid)))
        fontSizeIndex = 1;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_font_small)))
        fontSizeIndex = 2;
    sysNoteSetting.SetFontSize(fontSizeIndex);

    unsigned char bdmkSizeIndex = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_big)))
        bdmkSizeIndex = 2;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_mid)))
        bdmkSizeIndex = 1;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_bodymark_small)))
        bdmkSizeIndex = 0;
    sysNoteSetting.SetBodyMarkSize(bdmkSizeIndex);

    unsigned char fontColorIndex  = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_comment_color));
    sysNoteSetting.SetFontColor(fontColorIndex);

    unsigned char bdmkColorIndex  = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_bodymark_color));
    sysNoteSetting.SetBodyMarkColor(bdmkColorIndex);
    sysNoteSetting.SyncFile();
}

GtkWidget* ViewSystem::create_note_measure(void)
{
    GtkWidget *fixed_measure;
    GtkWidget *frame_measure_line;
    GtkWidget *hbox_measure_line;
    GSList *radiobutton_ml_group = NULL;
    GSList *radiobutton_measurt_result_group = NULL;
    GtkWidget *label_measure_line;
    GtkWidget *frame_cursor;
    GtkWidget *hbox_cursor;
    GSList *radiobutton_cursor_size_group = NULL;
    GtkWidget *label_cursor;

    GtkWidget *frame_line_color;
    GtkWidget *fixed_line_color;
    GtkWidget *label_current_line;
    GtkWidget *label_confirmed_line;
    GtkWidget *label_line_color;
    GtkWidget *label_unit;

    GtkWidget *frame_autocalc_para;
    GtkWidget *fixed_autocalc_para;
    GtkWidget *label_autocalc_para;

    GtkWidget *frame_trace_method;
    GtkWidget *hbox_trace_method;
    GSList *radiobutton_trace_method_group = NULL;
    GtkWidget *label_method_trace;

    GtkWidget *frame_report_result;
    GtkWidget *hbox_report_result;
    GSList *radiobutton_report_result_group = NULL;
    GtkWidget *label_report_result;

    GtkWidget *frame_unit_line;
    GtkWidget *fixed_unit_line;
    GtkWidget *label_unit_dist;
    GtkWidget *label_unit_area;
    GtkWidget *label_unit_vol;
    GtkWidget *label_unit_time;
    GtkWidget *label_unit_vel;
    GtkWidget *label_unit_slope;
    GtkWidget *label_unit_accel;
    GtkWidget *label_unit_efw;

    //GtkWidget *hseparator_1;
    GtkWidget *button_measure_default;
    GtkWidget *label_heart_beat_cycle;

    fixed_measure = gtk_fixed_new ();
    gtk_widget_show (fixed_measure);

    frame_measure_line = gtk_frame_new (NULL);
    gtk_widget_show (frame_measure_line);
   // gtk_fixed_put (GTK_FIXED (fixed_measure), frame_measure_line, 30, 30);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_measure_line, 310, 240+140);
    gtk_widget_set_size_request (frame_measure_line, 255, 60);
    gtk_frame_set_label_align (GTK_FRAME (frame_measure_line), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_measure_line), GTK_SHADOW_IN);

    hbox_measure_line = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_measure_line);
    gtk_container_add (GTK_CONTAINER (frame_measure_line), hbox_measure_line);

    m_radiobutton_ml_on = gtk_radio_button_new_with_mnemonic (NULL, _("ON"));
    gtk_widget_show (m_radiobutton_ml_on);
    gtk_box_pack_start (GTK_BOX (hbox_measure_line), m_radiobutton_ml_on, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_ml_on), radiobutton_ml_group);
    radiobutton_ml_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_ml_on));

    m_radiobutton_ml_off = gtk_radio_button_new_with_mnemonic (NULL, _("OFF"));
    gtk_widget_show (m_radiobutton_ml_off);
    gtk_box_pack_start (GTK_BOX (hbox_measure_line), m_radiobutton_ml_off, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_ml_off), radiobutton_ml_group);
    radiobutton_ml_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_ml_off));

    label_measure_line = gtk_label_new (_("<b>Measure Line</b>"));
    gtk_widget_show (label_measure_line);
    gtk_frame_set_label_widget (GTK_FRAME (frame_measure_line), label_measure_line);
    gtk_label_set_use_markup (GTK_LABEL (label_measure_line), TRUE);

    // frame_ml_density = gtk_frame_new (NULL);
    // gtk_widget_show (frame_ml_density);
    // gtk_fixed_put (GTK_FIXED (fixed_measure), frame_ml_density, 260, 30);
    // gtk_widget_set_size_request (frame_ml_density, 200, 60);
    // gtk_frame_set_label_align (GTK_FRAME (frame_ml_density), 0.5, 0.5);
    // gtk_frame_set_shadow_type (GTK_FRAME (frame_ml_density), GTK_SHADOW_IN);
    // gtk_widget_set_sensitive (frame_ml_density, FALSE);

    // hbox_ml_density = gtk_hbox_new (TRUE, 0);
    // gtk_widget_show (hbox_ml_density);
    // gtk_container_add (GTK_CONTAINER (frame_ml_density), hbox_ml_density);

    // m_radiobutton_ml_high = gtk_radio_button_new_with_mnemonic (NULL, _("High"));
    // gtk_widget_show (m_radiobutton_ml_high);
    // gtk_box_pack_start (GTK_BOX (hbox_ml_density), m_radiobutton_ml_high, FALSE, FALSE, 0);
    // gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_ml_high), radiobutton_ml_density_group);
    // radiobutton_ml_density_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_ml_high));

    // m_radiobutton_ml_mid = gtk_radio_button_new_with_mnemonic (NULL, _("Mid"));
    // gtk_widget_show (m_radiobutton_ml_mid);
    // gtk_box_pack_start (GTK_BOX (hbox_ml_density), m_radiobutton_ml_mid, FALSE, FALSE, 0);
    // gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_ml_mid), radiobutton_ml_density_group);
    // radiobutton_ml_density_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_ml_mid));

    // m_radiobutton_ml_low = gtk_radio_button_new_with_mnemonic (NULL, _("Low"));
    // gtk_widget_show (m_radiobutton_ml_low);
    // gtk_box_pack_start (GTK_BOX (hbox_ml_density), m_radiobutton_ml_low, FALSE, FALSE, 0);
    // gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_ml_low), radiobutton_ml_density_group);
    // radiobutton_ml_density_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_ml_low));

    // label_ml_density = gtk_label_new (_("<b>Measure Line Density</b>"));
    // gtk_widget_show (label_ml_density);
    // gtk_frame_set_label_widget (GTK_FRAME (frame_ml_density), label_ml_density);
    // gtk_label_set_use_markup (GTK_LABEL (label_ml_density), TRUE);

    frame_cursor = gtk_frame_new (NULL);
    gtk_widget_show (frame_cursor);
    // gtk_fixed_put (GTK_FIXED (fixed_measure), frame_cursor, 490, 30);
   // gtk_fixed_put (GTK_FIXED (fixed_measure), frame_cursor, 260, 30);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_cursor, 600, 240+70);
    gtk_widget_set_size_request (frame_cursor, 255, 60);
    gtk_frame_set_label_align (GTK_FRAME (frame_cursor), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_cursor), GTK_SHADOW_IN);

    hbox_cursor = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_cursor);
    gtk_container_add (GTK_CONTAINER (frame_cursor), hbox_cursor);

    m_radiobutton_cursor_big = gtk_radio_button_new_with_mnemonic (NULL, _("Big"));
    gtk_widget_show (m_radiobutton_cursor_big);
    gtk_box_pack_start (GTK_BOX (hbox_cursor), m_radiobutton_cursor_big, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_cursor_big), radiobutton_cursor_size_group);
    radiobutton_cursor_size_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_cursor_big));

    m_radiobutton_cursor_mid = gtk_radio_button_new_with_mnemonic (NULL, _("Mid"));
    gtk_widget_show (m_radiobutton_cursor_mid);
    gtk_box_pack_start (GTK_BOX (hbox_cursor), m_radiobutton_cursor_mid, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_cursor_mid), radiobutton_cursor_size_group);
    radiobutton_cursor_size_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_cursor_mid));

    m_radiobutton_cursor_small = gtk_radio_button_new_with_mnemonic (NULL, _("Small"));
    gtk_widget_show (m_radiobutton_cursor_small);
    gtk_box_pack_start (GTK_BOX (hbox_cursor), m_radiobutton_cursor_small, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_cursor_small), radiobutton_cursor_size_group);
    radiobutton_cursor_size_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_cursor_small));

    label_cursor = gtk_label_new (_("<b>Cursor Size</b>"));
    gtk_widget_show (label_cursor);
    gtk_frame_set_label_widget (GTK_FRAME (frame_cursor), label_cursor);
    gtk_label_set_use_markup (GTK_LABEL (label_cursor), TRUE);

    frame_line_color = gtk_frame_new (NULL);
    gtk_widget_show (frame_line_color);
    //gtk_fixed_put (GTK_FIXED (fixed_measure), frame_line_color, 30, 110);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_line_color, 310, 170);
    gtk_widget_set_size_request (frame_line_color, 450 + 100, 60);
    gtk_frame_set_label_align (GTK_FRAME (frame_line_color), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_line_color), GTK_SHADOW_IN);

    fixed_line_color = gtk_fixed_new ();
    gtk_widget_show (fixed_line_color);
    gtk_container_add (GTK_CONTAINER (frame_line_color), fixed_line_color);

    label_current_line = gtk_label_new (_("Current Line:"));
    gtk_widget_show (label_current_line);
    gtk_fixed_put (GTK_FIXED (fixed_line_color), label_current_line, 5, 5);
    gtk_widget_set_size_request (label_current_line, 100 + 20, 30);
    gtk_misc_set_alignment (GTK_MISC (label_current_line), 0, 0.5);

    m_combobox_current_line = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_current_line);
    gtk_fixed_put (GTK_FIXED (fixed_line_color), m_combobox_current_line, 105 + 20, 5);
    gtk_widget_set_size_request (m_combobox_current_line, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_current_line), _("Red"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_current_line), _("Yellow"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_current_line), _("Green"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_current_line), _("Blue"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_current_line), _("White"));

    label_confirmed_line = gtk_label_new (_("Confirmed Line:"));
    gtk_widget_show (label_confirmed_line);
    gtk_fixed_put (GTK_FIXED (fixed_line_color), label_confirmed_line, 230 + 20, 5);
    gtk_widget_set_size_request (label_confirmed_line, 110 + 60, 30);
    gtk_misc_set_alignment (GTK_MISC (label_confirmed_line), 0, 0.5);

    m_combobox_confirmed_line = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_confirmed_line);
    gtk_fixed_put (GTK_FIXED (fixed_line_color), m_combobox_confirmed_line, 340 + 80, 5);
    gtk_widget_set_size_request (m_combobox_confirmed_line, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_confirmed_line), _("Red"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_confirmed_line), _("Yellow"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_confirmed_line), _("Green"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_confirmed_line), _("Blue"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_confirmed_line), _("White"));

    label_line_color = gtk_label_new (_("<b>Line Color</b>"));
    gtk_widget_show (label_line_color);
    gtk_frame_set_label_widget (GTK_FRAME (frame_line_color), label_line_color);
    gtk_label_set_use_markup (GTK_LABEL (label_line_color), TRUE);

    //measure result font size
    GtkWidget* frame_measure_result = gtk_frame_new (NULL);
    gtk_widget_show (frame_measure_result);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_measure_result, 600, 90+150);
    gtk_widget_set_size_request (frame_measure_result, 255, 60);
    gtk_frame_set_label_align (GTK_FRAME (frame_measure_result), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_measure_result), GTK_SHADOW_IN);

    GtkWidget* hbox_measure_result = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_measure_result);
    gtk_container_add (GTK_CONTAINER (frame_measure_result), hbox_measure_result);

    m_radiobutton_result_small = gtk_radio_button_new_with_mnemonic (NULL, _("Small"));
    gtk_widget_show (m_radiobutton_result_small);
    gtk_box_pack_start (GTK_BOX (hbox_measure_result), m_radiobutton_result_small, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_result_small), radiobutton_measurt_result_group);
    radiobutton_measurt_result_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_result_small));

    m_radiobutton_result_big = gtk_radio_button_new_with_mnemonic (NULL, _("Big"));
    gtk_widget_show (m_radiobutton_result_big);
    gtk_box_pack_start (GTK_BOX (hbox_measure_result), m_radiobutton_result_big, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_result_big), radiobutton_measurt_result_group);
    radiobutton_measurt_result_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_result_big));

    GtkWidget* label_measure_result = gtk_label_new (_("<b>Measure Result Font Size</b>"));
    gtk_widget_show (label_measure_result);
    gtk_frame_set_label_widget (GTK_FRAME (frame_measure_result), label_measure_result);
    gtk_label_set_use_markup (GTK_LABEL (label_measure_result), TRUE);

#if 0
    hseparator_1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator_1);
    gtk_fixed_put (GTK_FIXED (fixed_measure), hseparator_1, 15, 190);//10
    gtk_widget_set_size_request (hseparator_1, DEFALT_SCREEN_WIDTH - 30, 16); //775
#endif
    frame_trace_method = gtk_frame_new (NULL);
#if (defined(CT_322) || defined(CT_313))
    gtk_widget_hide (frame_trace_method);
#else
    gtk_widget_show (frame_trace_method);
#endif
  //  gtk_fixed_put (GTK_FIXED (fixed_measure), frame_trace_method, 260, 210);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_trace_method, 310, 240+70);
    gtk_widget_set_size_request (frame_trace_method, 255, 60);
    gtk_frame_set_label_align (GTK_FRAME (frame_trace_method), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_trace_method), GTK_SHADOW_IN);

    hbox_trace_method = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_trace_method);
    gtk_container_add (GTK_CONTAINER (frame_trace_method), hbox_trace_method);

    m_radiobutton_trace_point = gtk_radio_button_new_with_mnemonic (NULL, _("Point"));
    gtk_widget_show (m_radiobutton_trace_point);
    gtk_box_pack_start (GTK_BOX (hbox_trace_method), m_radiobutton_trace_point, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_trace_point), radiobutton_trace_method_group);
    radiobutton_trace_method_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_trace_point));

    m_radiobutton_trace_track = gtk_radio_button_new_with_mnemonic (NULL, _("Track"));
    gtk_widget_show (m_radiobutton_trace_track);
    gtk_box_pack_start (GTK_BOX (hbox_trace_method), m_radiobutton_trace_track, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_trace_track), radiobutton_trace_method_group);
    radiobutton_trace_method_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_trace_track));

    m_radiobutton_trace_auto = gtk_radio_button_new_with_mnemonic (NULL, _("Auto"));
    gtk_widget_show (m_radiobutton_trace_auto);
    gtk_box_pack_start (GTK_BOX (hbox_trace_method), m_radiobutton_trace_auto, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_trace_auto), radiobutton_trace_method_group);
    radiobutton_trace_method_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_trace_auto));

    label_method_trace = gtk_label_new (_("<b>Trace Method</b>"));
    gtk_widget_show (label_method_trace);
    gtk_frame_set_label_widget (GTK_FRAME (frame_trace_method), label_method_trace);
    gtk_label_set_use_markup (GTK_LABEL (label_method_trace), TRUE);

    frame_report_result = gtk_frame_new (NULL);
    gtk_widget_show (frame_report_result);
   // gtk_fixed_put (GTK_FIXED (fixed_measure), frame_report_result, 30, 210);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_report_result, 310, 90+150);
    gtk_widget_set_size_request (frame_report_result, 255, 60);
    gtk_frame_set_label_align (GTK_FRAME (frame_report_result), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_report_result), GTK_SHADOW_IN);

    hbox_report_result = gtk_hbox_new (TRUE, 0);
    gtk_widget_show (hbox_report_result);
    gtk_container_add (GTK_CONTAINER (frame_report_result), hbox_report_result);

    m_radiobutton_report_last = gtk_radio_button_new_with_mnemonic (NULL, _("Last"));
    gtk_widget_show (m_radiobutton_report_last);
    gtk_box_pack_start (GTK_BOX (hbox_report_result), m_radiobutton_report_last, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_report_last), radiobutton_report_result_group);
    radiobutton_report_result_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_report_last));

    m_radiobutton_report_average = gtk_radio_button_new_with_mnemonic (NULL, _("Average"));
    gtk_widget_show (m_radiobutton_report_average);
    gtk_box_pack_start (GTK_BOX (hbox_report_result), m_radiobutton_report_average, FALSE, FALSE, 0);
    gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_report_average), radiobutton_report_result_group);
    radiobutton_report_result_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_report_average));

    label_report_result = gtk_label_new (_("<b>Report Result Method</b>"));
    gtk_widget_show (label_report_result);
    gtk_frame_set_label_widget (GTK_FRAME (frame_report_result), label_report_result);
    gtk_label_set_use_markup (GTK_LABEL (label_report_result), TRUE);

    frame_autocalc_para = gtk_frame_new (NULL);
#if (defined(CT_313) || defined(CT_322))
    gtk_widget_hide (frame_autocalc_para);
#else
    gtk_widget_show (frame_autocalc_para);
#endif
  //  gtk_fixed_put (GTK_FIXED (fixed_measure), frame_autocalc_para, 30, 290);
    gtk_fixed_put (GTK_FIXED (fixed_measure), frame_autocalc_para, 310, 10);
    gtk_widget_set_size_request (frame_autocalc_para, 610-80+20, 150);
    gtk_frame_set_label_align (GTK_FRAME (frame_autocalc_para), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_autocalc_para), GTK_SHADOW_IN);

    fixed_autocalc_para = gtk_fixed_new ();
    gtk_widget_show (fixed_autocalc_para);
    gtk_container_add (GTK_CONTAINER (frame_autocalc_para), fixed_autocalc_para);

    m_checkbutton_autocalc_ps = gtk_check_button_new_with_mnemonic (("PS"));
    gtk_widget_show (m_checkbutton_autocalc_ps);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_ps, 20, 10);
    gtk_widget_set_size_request (m_checkbutton_autocalc_ps, 100, 30);
    gtk_widget_set_sensitive(m_checkbutton_autocalc_ps, FALSE);

    m_checkbutton_autocalc_ed = gtk_check_button_new_with_mnemonic (("ED"));
    gtk_widget_show (m_checkbutton_autocalc_ed);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_ed, 130+15, 10);
    gtk_widget_set_size_request (m_checkbutton_autocalc_ed, 100, 30);
    gtk_widget_set_sensitive(m_checkbutton_autocalc_ed, FALSE);

    m_checkbutton_autocalc_ri = gtk_check_button_new_with_mnemonic (("RI"));
    gtk_widget_show (m_checkbutton_autocalc_ri);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_ri, 240+30, 10);
    gtk_widget_set_size_request (m_checkbutton_autocalc_ri, 100, 30);

    m_checkbutton_autocalc_sd = gtk_check_button_new_with_mnemonic (("SD"));
    gtk_widget_show (m_checkbutton_autocalc_sd);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_sd, 350+45, 10);
    gtk_widget_set_size_request (m_checkbutton_autocalc_sd, 100, 30);

    m_checkbutton_autocalc_tamax = gtk_check_button_new_with_mnemonic (_("TA Max"));
    gtk_widget_show (m_checkbutton_autocalc_tamax);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_tamax, 20, 50);
    gtk_widget_set_size_request (m_checkbutton_autocalc_tamax, 100, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_autocalc_tamax), "toggled", G_CALLBACK(HandleTAmaxToggled), this);

    m_checkbutton_autocalc_pi = gtk_check_button_new_with_mnemonic (("PI"));
    gtk_widget_show (m_checkbutton_autocalc_pi);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_pi, 135+10, 50);
    gtk_widget_set_size_request (m_checkbutton_autocalc_pi, 100, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_autocalc_pi), "toggled", G_CALLBACK(HandlePIToggled), this);

    m_checkbutton_autocalc_time = gtk_check_button_new_with_mnemonic (("Time"));
    gtk_widget_show (m_checkbutton_autocalc_time);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_time, 250+20, 50);
    gtk_widget_set_size_request (m_checkbutton_autocalc_time, 100, 30);

    m_checkbutton_autocalc_hr = gtk_check_button_new_with_mnemonic (("HR"));
    gtk_widget_show (m_checkbutton_autocalc_hr);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_hr, 365+30, 50);
    gtk_widget_set_size_request (m_checkbutton_autocalc_hr, 100, 30);
    g_signal_connect(G_OBJECT(m_checkbutton_autocalc_hr), "toggled", G_CALLBACK(HandleHRToggled), this);

    m_checkbutton_autocalc_pgmax = gtk_check_button_new_with_mnemonic (_("PG Max"));
    gtk_widget_show (m_checkbutton_autocalc_pgmax);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_pgmax, 20, 90);
    gtk_widget_set_size_request (m_checkbutton_autocalc_pgmax, 100, 30);

    m_checkbutton_autocalc_pgmean = gtk_check_button_new_with_mnemonic (_("PG Mean"));
    gtk_widget_show (m_checkbutton_autocalc_pgmean);
    gtk_fixed_put (GTK_FIXED (fixed_autocalc_para), m_checkbutton_autocalc_pgmean, 135+10, 90);
    gtk_widget_set_size_request (m_checkbutton_autocalc_pgmean, 100, 30);

    label_autocalc_para = gtk_label_new (_("<b>Auto Spectrum Calculation Result</b>"));
    gtk_widget_show (label_autocalc_para);
    gtk_frame_set_label_widget (GTK_FRAME (frame_autocalc_para), label_autocalc_para);
    gtk_label_set_use_markup (GTK_LABEL (label_autocalc_para), TRUE);

    button_measure_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_measure_default);
  //  gtk_fixed_put (GTK_FIXED (fixed_measure), button_measure_default, 30, 450);
    gtk_fixed_put (GTK_FIXED (fixed_measure), button_measure_default, 50, 450);
    gtk_widget_set_size_request (button_measure_default, 140, 40);
    g_signal_connect ((gpointer) button_measure_default, "clicked", G_CALLBACK (on_button_measure_default_clicked), this);

    label_heart_beat_cycle = gtk_label_new (_("<b>Heart Beat:</b>"));
    gtk_label_set_use_markup (GTK_LABEL (label_heart_beat_cycle ), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_heart_beat_cycle ), 0, 0.5);
    gtk_fixed_put (GTK_FIXED (fixed_measure), label_heart_beat_cycle , 50, 400);
    gtk_widget_set_size_request (label_heart_beat_cycle, 100, 30);
    gtk_widget_show (label_heart_beat_cycle );
    m_combobox_heart_beat_cycle = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_heart_beat_cycle );
    gtk_fixed_put (GTK_FIXED (fixed_measure), m_combobox_heart_beat_cycle , 100 + 60, 400);
    gtk_widget_set_size_request (m_combobox_heart_beat_cycle , 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "1");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "2");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "3");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "4");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "5");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "6");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "7");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_heart_beat_cycle ), "8");

    frame_unit_line = gtk_frame_new(NULL);
    gtk_widget_show(frame_unit_line);
    gtk_fixed_put(GTK_FIXED (fixed_measure), frame_unit_line, 50, 10);
    gtk_widget_set_size_request(frame_unit_line,  240, 380-5);
    gtk_frame_set_label_align(GTK_FRAME(frame_unit_line), 0.5, 0.5);
    gtk_frame_set_shadow_type(GTK_FRAME(frame_unit_line), GTK_SHADOW_IN);

    fixed_unit_line = gtk_fixed_new();
    gtk_widget_show(fixed_unit_line);
    gtk_container_add(GTK_CONTAINER(frame_unit_line), fixed_unit_line);

    label_unit_dist = gtk_label_new(_("Distance"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_dist), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_dist), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_dist, 10, 10);
    gtk_widget_set_size_request (label_unit_dist, 100, 30);
    gtk_widget_show (label_unit_dist);
    m_combobox_unit_dist = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_dist);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_dist, 110, 10);
    gtk_widget_set_size_request (m_combobox_unit_dist, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_dist), "cm");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_dist), "mm");

    label_unit_area = gtk_label_new(_("Area"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_area), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_area), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_area, 10, 60);
    gtk_widget_set_size_request (label_unit_area, 100, 30);
    gtk_widget_show (label_unit_area);
    m_combobox_unit_area = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_area);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_area, 110, 60);
    gtk_widget_set_size_request (m_combobox_unit_area, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_area), "cm²");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_area),"mm²");

    label_unit_vol = gtk_label_new(_("Volume"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_vol), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_vol), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_vol, 10, 110);
    gtk_widget_set_size_request (label_unit_vol, 100, 30);
    gtk_widget_show (label_unit_vol);
    m_combobox_unit_vol = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_vol);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_vol, 110, 110);
    gtk_widget_set_size_request (m_combobox_unit_vol, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_vol),"cm³");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_vol), "mm³");

    label_unit_time = gtk_label_new(_("Time"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_time), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_time), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_time, 10, 160);
    gtk_widget_set_size_request (label_unit_time, 100, 30);
    gtk_widget_show (label_unit_time);
    m_combobox_unit_time = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_time);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_time, 110, 160);
    gtk_widget_set_size_request (m_combobox_unit_time, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_time), "s");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_time), "ms");

    label_unit_vel = gtk_label_new(_("Velocity"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_vel), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_vel), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_vel, 10, 210);
    gtk_widget_set_size_request (label_unit_vel, 100, 30);
    gtk_widget_show (label_unit_vel);
    m_combobox_unit_vel = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_vel);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_vel, 110, 210);
    gtk_widget_set_size_request (m_combobox_unit_vel, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_vel), "cm/s");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_vel), "mm/s");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_vel), "m/s");

   /* label_unit_slope = gtk_label_new(_("Slope"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_slope), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_slope), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_slope, 50, 260);
    gtk_widget_set_size_request (label_unit_slope, 100, 30);
    gtk_widget_show (label_unit_slope);
    m_combobox_unit_slope = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_slope);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_slope, 160, 260);
    gtk_widget_set_size_request (m_combobox_unit_slope, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_slope), "cm/s");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_slope), "mm/s");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_slope), "m/s");
*/
    label_unit_accel = gtk_label_new(_("Accel"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_accel), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_accel), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_accel, 10, 260);
    gtk_widget_set_size_request (label_unit_accel, 100, 30);
    gtk_widget_show (label_unit_accel);
    m_combobox_unit_accel = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_accel);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_accel, 110, 260);
    gtk_widget_set_size_request (m_combobox_unit_accel, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_accel), "cm/s²");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_accel), "mm/s²");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_accel), "m/s²");

    label_unit_efw = gtk_label_new(_("EFW"));
    gtk_label_set_use_markup (GTK_LABEL (label_unit_efw), TRUE);
    gtk_misc_set_alignment (GTK_MISC (label_unit_efw), 0, 0.5);
    gtk_fixed_put (GTK_FIXED(fixed_unit_line), label_unit_efw, 10, 310);
    gtk_widget_set_size_request (label_unit_efw, 100, 30);
    gtk_widget_show (label_unit_efw);
    m_combobox_unit_efw = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_unit_efw);
    gtk_fixed_put (GTK_FIXED (fixed_unit_line), m_combobox_unit_efw, 110, 310);
    gtk_widget_set_size_request (m_combobox_unit_efw, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_efw), "kg");
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_unit_efw), "g");

    label_unit = gtk_label_new (_("<b>Unit Setting</b>"));
    gtk_widget_show (label_unit);
    gtk_frame_set_label_widget (GTK_FRAME (frame_unit_line), label_unit);
    gtk_label_set_use_markup (GTK_LABEL (label_unit), TRUE);

    return fixed_measure;
}

void ViewSystem::init_measure_setting(SysMeasurementSetting* sysMeasure)
{
    if (sysMeasure == NULL)
        sysMeasure = new SysMeasurementSetting;

    int measureLine = sysMeasure->GetMeasureLineDisplay();
    switch (measureLine) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_ml_on), TRUE); break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_ml_off), TRUE); break;
    }
    // int density = sysMeasure->GetMeasureLineDensity();
    // switch (density) {
    // case 0:
    //     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_ml_high), TRUE); break;
    // case 1:
    //     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_ml_mid), TRUE); break;
    // case 2:
    //     gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_ml_low), TRUE); break;
    // }
    int cursorSize = sysMeasure->GetMeasureCursorSize();
    switch (cursorSize) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_cursor_big), TRUE); break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_cursor_mid), TRUE); break;
        case 2:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_cursor_small), TRUE); break;
    }

    int lineColor = sysMeasure->GetMeasureColorCur();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_current_line), lineColor);
    lineColor = sysMeasure->GetMeasureColorConfirm();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_confirmed_line), lineColor);

    // int traceMethod = sysMeasure->GetTraceInMeasure();
    // switch (traceMethod) {
    // case 0:
    //  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_auto), TRUE); break;
    // case 1:
    //  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_manual), TRUE); break;
    // }

    int meaResult = sysMeasure->GetMeasureResult();
    switch (meaResult) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_result_small), TRUE); break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_result_big), TRUE); break;
    }

    int traceManual = sysMeasure->GetTraceMethod();
    switch (traceManual) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_point), TRUE); break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_track), TRUE); break;
        case 2:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_trace_auto), TRUE); break;
    }
    int reportResult = sysMeasure->GetReportResult();
    switch (reportResult) {
        case 0:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_report_last), TRUE); break;
        case 1:
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_report_average), TRUE); break;
    }

    // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ps), sysMeasure->GetAutoCalcPS());
    // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ed), sysMeasure->GetAutoCalcED());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ps), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ed), TRUE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ri), sysMeasure->GetAutoCalcRI());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_sd), sysMeasure->GetAutoCalcSD());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_tamax), sysMeasure->GetAutoCalcTAmax());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pi), sysMeasure->GetAutoCalcPI());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_time), sysMeasure->GetAutoCalcTime());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_hr), sysMeasure->GetAutoCalcHR());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmax), sysMeasure->GetAutoCalcPGmax());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmean), sysMeasure->GetAutoCalcPGmean());

    int unitDist = sysMeasure->GetUnitDist();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_dist), unitDist);

    int unitArea = sysMeasure->GetUnitArea();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_area), unitArea);

    int unitVol = sysMeasure->GetUnitVol();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_vol), unitVol);

    int unitTime = sysMeasure->GetUnitTime();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_time), unitTime);

    int unitVel = sysMeasure->GetUnitVel();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_vel), unitVel);

//  int unitSlope = sysMeasure->GetUnitSlope();
//  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_slope), unitSlope);

    int unitAccel = sysMeasure->GetUnitAccel();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_accel), unitAccel);

    int unitEfw = sysMeasure->GetUnitEfw();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_unit_efw), unitEfw);

    int heartBeatCycleIndex = sysMeasure->GetHeartBeatCycle() - 1;
    if (heartBeatCycleIndex < 0)
        heartBeatCycleIndex = 0;
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_heart_beat_cycle), heartBeatCycleIndex);

    delete sysMeasure;
}

void ViewSystem::save_measure_setting(void)
{
    SysMeasurementSetting sysMeasure;
    unsigned char measureLine = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_ml_on)))
        measureLine = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_ml_off)))
        measureLine = 1;
    sysMeasure.SetMeasureLineDisplay(measureLine);

    // unsigned char density = 0;
    // if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_ml_high)))
    //     density = 0;
    // else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_ml_mid)))
    //     density = 1;
    // else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_ml_low)))
    //     density = 2;
    // sysMeasure.SetMeasureLineDensity(density);

    unsigned char cursorSize = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_big)))
        cursorSize = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_mid)))
        cursorSize = 1;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_cursor_small)))
        cursorSize = 2;
    sysMeasure.SetMeasureCursorSize(cursorSize);

    unsigned char colorIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_current_line));
    sysMeasure.SetMeasureColorCur(colorIndex);
    colorIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_confirmed_line));
    sysMeasure.SetMeasureColorConfirm(colorIndex);

    unsigned int meaResult = 0;//小字体
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_result_small)))
        meaResult = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_result_big)))
        meaResult = 1;
    sysMeasure.SetMeasureResult(meaResult);
  //  ImageArea::GetInstance()->UpdateMeaResultArea(meaResult);

    // if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_auto)))
    //  trace = 0;
    // else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_manual)))
    //  trace = 1;
    // sysMeasure.SetTraceInMeasure(trace);

    unsigned char trace = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_point)))
        trace = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_track)))
        trace = 1;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_trace_auto)))
        trace = 2;
    sysMeasure.SetTraceMethod(trace);

    unsigned char reportResult = 0;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_report_last)))
        reportResult = 0;
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_report_average)))
        reportResult = 1;
    sysMeasure.SetReportResult(reportResult);

    // sysMeasure.SetAutoCalcPS(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ps)));
    // sysMeasure.SetAutoCalcED(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ed)));
    sysMeasure.SetAutoCalcPS(true);
    sysMeasure.SetAutoCalcED(true);
    sysMeasure.SetAutoCalcRI(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_ri)));
    sysMeasure.SetAutoCalcSD(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_sd)));
    sysMeasure.SetAutoCalcTAmax(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_tamax)));
    sysMeasure.SetAutoCalcPI(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pi)));
    sysMeasure.SetAutoCalcTime(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_time)));
    sysMeasure.SetAutoCalcHR(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_hr)));
    sysMeasure.SetAutoCalcPGmax(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmax)));
    sysMeasure.SetAutoCalcPGmean(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pgmean)));

    int unitDist = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_dist));
    sysMeasure.SetUnitDist(unitDist);

    int unitArea = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_area));
    sysMeasure.SetUnitArea(unitArea);

    int unitVol = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_vol));
    sysMeasure.SetUnitVol(unitVol);

    int unitTime = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_time));
    sysMeasure.SetUnitTime(unitTime);

    int unitVel = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_vel));
    sysMeasure.SetUnitVel(unitVel);

   // int unitSlope = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_slope));
    //sysMeasure.SetUnitSlope(unitSlope);

    int unitAccel = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_accel));
    sysMeasure.SetUnitAccel(unitAccel);

    int unitEfw = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_unit_efw));
    sysMeasure.SetUnitEfw(unitEfw);

    int heartBeatCycle = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_heart_beat_cycle));
    sysMeasure.SetHeartBeatCycle(heartBeatCycle+1);

    sysMeasure.SyncFile();
    sysMeasure.UpdateTraceItemSetting();
    ImageArea::GetInstance()->UpdateMeaResultArea(meaResult);
}

void ViewSystem::GetImageNoteSelection(int &probeIndex, int &itemIndex, char* &itemName)
{
    // get probeIndex
    probeIndex = GetProbeType();

    // get itemIndex
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
    {
        itemIndex = -1;
        return ;
    }
    gtk_tree_model_get(model, &iter,
            0, &itemName,
            -1);
    gtk_tree_model_get(model, &iter,
            INDEX_COLUMN, &itemIndex,
            -1);
}

void ViewCustomOB::save_customOB(void)
{
    MeaCalcFun::SaveCustomTable();
}

GtkWidget* ViewSystem::create_note_key_config(void)
{
    GtkWidget *fixed_key_config;

    GtkWidget *fixed_key;
    // GtkWidget *m_entry_function_p1;
    // GtkWidget *m_entry_function_p2;

    GSList *radiobutton_key_p1_group = NULL;
    GSList *radiobutton_key_p2_group = NULL;
    GSList *radiobutton_key_p3_group = NULL;

    GtkWidget *label_key_p1;
    GtkWidget *label_key_p2;
    GtkWidget *label_key_p3;

    GtkWidget *frame_key_p1;
    GtkWidget *frame_key_p2;
    GtkWidget *frame_key_p3;
    GtkWidget *fixed_key_p1;
    GtkWidget *fixed_key_p2;
    GtkWidget *fixed_key_p3;

    fixed_key_config = gtk_fixed_new ();
    gtk_widget_show (fixed_key_config);

    frame_key_p1 = gtk_frame_new (NULL);
    gtk_widget_show (frame_key_p1);
    gtk_fixed_put (GTK_FIXED (fixed_key_config), frame_key_p1, 50, 30);
    gtk_widget_set_size_request (frame_key_p1, 810, 120);
    gtk_frame_set_label_align (GTK_FRAME (frame_key_p1), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_key_p1), GTK_SHADOW_IN);

    fixed_key_p1 = gtk_fixed_new ();
    gtk_widget_show (fixed_key_p1);
    gtk_container_add (GTK_CONTAINER (frame_key_p1), fixed_key_p1);
    for(int i=0;i<MAX_KEY;i++)
    {
        if(strcmp(UserDefineKey::KeyFunctionList[i].c_str(), "Print Screen") == 0)
            m_radiobutton_key_p1[i] = gtk_radio_button_new_with_mnemonic (NULL, _("Print"));
        else
            m_radiobutton_key_p1[i] = gtk_radio_button_new_with_mnemonic (NULL, _(UserDefineKey::KeyFunctionList[i].c_str()));
        gtk_widget_show (m_radiobutton_key_p1[i]);
        gtk_fixed_put (GTK_FIXED (fixed_key_p1), m_radiobutton_key_p1[i], 20+(i%5)*150, 20+(i/5)*40);
        gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_key_p1[i]), radiobutton_key_p1_group);
        radiobutton_key_p1_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_key_p1[i]));
    }
#ifdef CT_322
    label_key_p1 = gtk_label_new (_("<b>Value</b>"));
#else
    label_key_p1 = gtk_label_new (_("<b>P1</b>"));
#endif
    gtk_widget_show (label_key_p1);
    gtk_frame_set_label_widget (GTK_FRAME (frame_key_p1), label_key_p1);
    gtk_label_set_use_markup (GTK_LABEL (label_key_p1), TRUE);
    //p2
    frame_key_p2 = gtk_frame_new (NULL);
    gtk_widget_show (frame_key_p2);
    gtk_fixed_put (GTK_FIXED (fixed_key_config), frame_key_p2, 50, 170);
    gtk_widget_set_size_request (frame_key_p2, 810, 120);
    gtk_frame_set_label_align (GTK_FRAME (frame_key_p2), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_key_p2), GTK_SHADOW_IN);

    fixed_key_p2 = gtk_fixed_new ();
    gtk_widget_show (fixed_key_p2);
    gtk_container_add (GTK_CONTAINER (frame_key_p2), fixed_key_p2);
    for(int i=0;i<MAX_KEY;i++)
    {
        m_radiobutton_key_p2[i] = gtk_radio_button_new_with_mnemonic (NULL, _(UserDefineKey::KeyFunctionList[i].c_str()));
        gtk_widget_show (m_radiobutton_key_p2[i]);
        gtk_fixed_put (GTK_FIXED (fixed_key_p2), m_radiobutton_key_p2[i], 20+(i%5)*150, 20+(i/5)*40);
        gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_key_p2[i]), radiobutton_key_p2_group);
        radiobutton_key_p2_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_key_p2[i]));
    }
#ifdef CT_322
    label_key_p2 = gtk_label_new (_("<b>Gain</b>"));
#else
    label_key_p2 = gtk_label_new (_("<b>P2</b>"));
#endif
    gtk_widget_show (label_key_p2);
    gtk_frame_set_label_widget (GTK_FRAME (frame_key_p2), label_key_p2);
    gtk_label_set_use_markup (GTK_LABEL (label_key_p2), TRUE);
 //p3
#if (defined(CT_460) || defined(CT_355))
    frame_key_p3 = gtk_frame_new (NULL);
    gtk_widget_show (frame_key_p3);
    gtk_fixed_put (GTK_FIXED (fixed_key_config), frame_key_p3, 50, 310);
    gtk_widget_set_size_request (frame_key_p3, 810, 120);
    gtk_frame_set_label_align (GTK_FRAME (frame_key_p3), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_key_p3), GTK_SHADOW_IN);

    fixed_key_p3 = gtk_fixed_new ();
    gtk_widget_show (fixed_key_p3);
    gtk_container_add (GTK_CONTAINER (frame_key_p3), fixed_key_p3);
    for(int i=0;i<MAX_KEY;i++)
    {
        m_radiobutton_key_p3[i] = gtk_radio_button_new_with_mnemonic (NULL, _(UserDefineKey::KeyFunctionList[i].c_str()));
        gtk_widget_show (m_radiobutton_key_p3[i]);
        gtk_fixed_put (GTK_FIXED (fixed_key_p3), m_radiobutton_key_p3[i], 20+(i%5)*150, 20+(i/5)*40);
        gtk_radio_button_set_group (GTK_RADIO_BUTTON (m_radiobutton_key_p3[i]), radiobutton_key_p3_group);
        radiobutton_key_p3_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (m_radiobutton_key_p3[i]));
    }
    label_key_p3 = gtk_label_new (_("<b>P3</b>"));
    gtk_widget_show (label_key_p3);
    gtk_frame_set_label_widget (GTK_FRAME (frame_key_p3), label_key_p3);
    gtk_label_set_use_markup (GTK_LABEL (label_key_p3), TRUE);
#endif
    return fixed_key_config;

}

void ViewSystem::init_key_config(void)
{
    SysUserDefinedKey sysDefine;
    m_p1_func_index = sysDefine.GetFuncP1();
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_key_p1[m_p1_func_index]), TRUE);

    m_p2_func_index = sysDefine.GetFuncP2();
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_key_p2[m_p2_func_index]), TRUE);
#if (defined(CT_460) || defined(CT_355))
    m_p3_func_index = sysDefine.GetFuncP3();
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_radiobutton_key_p3[m_p3_func_index]), TRUE);
#endif
#if 0
    SysUserDefinedKey sysDefine;
    m_p1_func_index = sysDefine.GetFuncP1();
    m_p2_func_index = sysDefine.GetFuncP2();

    gtk_entry_set_text(GTK_ENTRY(m_entry_function_p1), _(UserDefineKey::KeyFunctionList[m_p1_func_index].c_str()));
    gtk_entry_set_text(GTK_ENTRY(m_entry_function_p2), _(UserDefineKey::KeyFunctionList[m_p2_func_index].c_str()));
    m_p1Selected = true;
#endif
}

void ViewSystem::save_key_config(void)
{
    SysUserDefinedKey sysDefine;
#if (defined(CT_460) || defined(CT_355))
    for(int i = 0;i < MAX_KEY;i++)
    {
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p1[i])))
            m_p1_func_index = i;
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p2[i])))
            m_p2_func_index = i;
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p3[i])))
            m_p3_func_index = i;
    }
#else
 for(int i = 0;i < MAX_KEY;i++)
    {
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p1[i])))
            m_p1_func_index = i;
        if( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_radiobutton_key_p2[i])))
            m_p2_func_index = i;
    }
#endif
    sysDefine.SetFuncP1(m_p1_func_index);
    sysDefine.SetFuncP2(m_p2_func_index);
    sysDefine.SetFuncP3(m_p3_func_index);
    sysDefine.SyncFile();
}
#if 0
    SysUserDefinedKey sysDefine;
    //printf("*********system config**********%d  %d\n",m_p1_func_index,m_p2_func_index);
    sysDefine.SetFuncP1(m_p1_func_index);
    sysDefine.SetFuncP2(m_p2_func_index);
    sysDefine.SyncFile();
#endif

GtkWidget *ViewSystem::create_note_calc(void)
{
    GtkWidget *fixed_calc;
    // GtkWidget *frame_adult_echo;
    // GtkWidget *fixed_adult_echo;
    // GtkWidget *label_adult_2d_edv;
    // GtkWidget *label_adult_2d_fs;
    // GtkWidget *label_adult_2d_lv_mass;
    // GtkWidget *label_adult_m_edv;
    // GtkWidget *label_adult_m_fs;
    // GtkWidget *label_adult_m_lv_mass;
    // GtkWidget *label_mva;
    GtkWidget *label_bsa;
    // GtkWidget *label_adult_echo;

    // GtkWidget *frame_fetus_echo;
    // GtkWidget *fixed_fetus_echo;
    // GtkWidget *label_fetus_2d_edv;
    // GtkWidget *label_fetus_2d_fs;
    // GtkWidget *label_fetus_m_edv;
    // GtkWidget *label_fetus_m_fs;
    // GtkWidget *label_fetus_echo;

    GtkWidget *frame_ob_method;
    GtkWidget *fixed_ob_method;

    GtkWidget *label_ob_hl;
    GtkWidget *label_ob_bpd;
    GtkWidget *label_ob_crl;

    GtkWidget *label_ob_gs;
    GtkWidget *label_ob_cer;
    GtkWidget *label_ob_fl;
    GtkWidget *label_ob_ac;
    GtkWidget *label_ob_apad;
    GtkWidget *label_ob_hc;
    GtkWidget *label_ob_thd;
    GtkWidget *label_ob_tad;
    GtkWidget *label_ob_ofd;
    GtkWidget *label_ob_efw;

    GtkWidget *label_ob_method;
    GtkWidget *button_ob_custom;
    GtkWidget *button_calc_default;

    fixed_calc = gtk_fixed_new ();
    gtk_widget_show (fixed_calc);

    // -*- set adult echo -*-
    // frame_adult_echo = gtk_frame_new (NULL);
    // gtk_widget_show (frame_adult_echo);
    // gtk_fixed_put (GTK_FIXED (fixed_calc), frame_adult_echo, 20, 5);
    // gtk_widget_set_size_request (frame_adult_echo, 650, 130);
    // gtk_frame_set_label_align (GTK_FRAME (frame_adult_echo), 0.5, 0.5);
    // gtk_frame_set_shadow_type (GTK_FRAME (frame_adult_echo), GTK_SHADOW_IN);

    // fixed_adult_echo = gtk_fixed_new ();
    // gtk_widget_show (fixed_adult_echo);
    // gtk_container_add (GTK_CONTAINER (frame_adult_echo), fixed_adult_echo);

    // label_adult_2d_edv = gtk_label_new (_("<b>2D</b> [EDV ESV] :"));
    // gtk_widget_show (label_adult_2d_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_adult_2d_edv, 5, 5);
    // gtk_widget_set_size_request (label_adult_2d_edv, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_2d_edv), TRUE);

    // m_combobox_adult_2d_edv = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_adult_2d_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_adult_2d_edv, 105, 5);
    // gtk_widget_set_size_request (m_combobox_adult_2d_edv, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_edv), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_edv), _("Teich"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_edv), _("A/L"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_edv), _("A2C"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_edv), _("A4C"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_edv), _("BP"));

    // label_adult_2d_fs = gtk_label_new (_("<b>2D</b> [FS] :"));
    // gtk_widget_show (label_adult_2d_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_adult_2d_fs, 5, 40);
    // gtk_widget_set_size_request (label_adult_2d_fs, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_2d_fs), TRUE);

    // m_combobox_adult_2d_fs = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_adult_2d_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_adult_2d_fs, 105, 40);
    // gtk_widget_set_size_request (m_combobox_adult_2d_fs, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_fs), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_fs), _("Teich"));

    // label_adult_2d_lv_mass = gtk_label_new (_("<b>2D</b> [LV Mass] :"));
    // gtk_widget_show (label_adult_2d_lv_mass);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_adult_2d_lv_mass, 5, 75);
    // gtk_widget_set_size_request (label_adult_2d_lv_mass, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_2d_lv_mass), TRUE);

    // m_combobox_adult_2d_lv_mass = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_adult_2d_lv_mass);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_adult_2d_lv_mass, 105, 75);
    // gtk_widget_set_size_request (m_combobox_adult_2d_lv_mass, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_lv_mass), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_2d_lv_mass), _("A/L"));

    // label_adult_m_edv = gtk_label_new (_("<b>M</b> [EDV ESV] :"));
    // gtk_widget_show (label_adult_m_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_adult_m_edv, 220, 5);
    // gtk_widget_set_size_request (label_adult_m_edv, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_m_edv), TRUE);

    // m_combobox_adult_m_edv = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_adult_m_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_adult_m_edv, 320, 5);
    // gtk_widget_set_size_request (m_combobox_adult_m_edv, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_m_edv), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_m_edv), _("Teich"));

    // label_adult_m_fs = gtk_label_new (_("<b>M</b> [FS] :"));
    // gtk_widget_show (label_adult_m_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_adult_m_fs, 220, 40);
    // gtk_widget_set_size_request (label_adult_m_fs, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_m_fs), TRUE);

    // m_combobox_adult_m_fs = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_adult_m_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_adult_m_fs, 320, 40);
    // gtk_widget_set_size_request (m_combobox_adult_m_fs, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_m_fs), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_m_fs), _("Teich"));

    // label_adult_m_lv_mass = gtk_label_new (_("<b>M</b> [LV Mass] :"));
    // gtk_widget_show (label_adult_m_lv_mass);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_adult_m_lv_mass, 220, 75);
    // gtk_widget_set_size_request (label_adult_m_lv_mass, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_m_lv_mass), TRUE);

    // m_combobox_adult_m_lv_mass = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_adult_m_lv_mass);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_adult_m_lv_mass, 320, 75);
    // gtk_widget_set_size_request (m_combobox_adult_m_lv_mass, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_m_lv_mass), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_adult_m_lv_mass), _("A/L"));

    // label_mva = gtk_label_new (_("[ MVA ] :"));
    // gtk_widget_show (label_mva);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_mva, 435, 5);
    // gtk_widget_set_size_request (label_mva, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_mva), TRUE);

    // m_combobox_mva = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_mva);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_mva, 535, 5);
    // gtk_widget_set_size_request (m_combobox_mva, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_mva), _("P1 / 2t"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_mva), _("PISA"));

    // label_bsa = gtk_label_new (_("[ BSA ] :"));
    // gtk_widget_show (label_bsa);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), label_bsa, 435, 40);
    // gtk_widget_set_size_request (label_bsa, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_bsa), TRUE);

    // m_combobox_bsa = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_bsa);
    // gtk_fixed_put (GTK_FIXED (fixed_adult_echo), m_combobox_bsa, 535, 40);
    // gtk_widget_set_size_request (m_combobox_bsa, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bsa), _("Oriental"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bsa), _("Occidental"));

    // label_adult_echo = gtk_label_new (_("<b>Adult echo</b>"));
    // gtk_widget_show (label_adult_echo);
    // gtk_frame_set_label_widget (GTK_FRAME (frame_adult_echo), label_adult_echo);
    // gtk_label_set_use_markup (GTK_LABEL (label_adult_echo), TRUE);

    // -*- set fetus echo -*-
    // frame_fetus_echo = gtk_frame_new (NULL);
    // gtk_widget_show (frame_fetus_echo);
    // gtk_fixed_put (GTK_FIXED (fixed_calc), frame_fetus_echo, 20, 150);
    // gtk_widget_set_size_request (frame_fetus_echo, 440, 100);
    // gtk_frame_set_label_align (GTK_FRAME (frame_fetus_echo), 0.5, 0.5);
    // gtk_frame_set_shadow_type (GTK_FRAME (frame_fetus_echo), GTK_SHADOW_IN);

    // fixed_fetus_echo = gtk_fixed_new ();
    // gtk_widget_show (fixed_fetus_echo);
    // gtk_container_add (GTK_CONTAINER (frame_fetus_echo), fixed_fetus_echo);

    // label_fetus_2d_edv = gtk_label_new (_("<b>2D</b> [EDV ESV] :"));
    // gtk_widget_show (label_fetus_2d_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), label_fetus_2d_edv, 5, 5);
    // gtk_widget_set_size_request (label_fetus_2d_edv, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_fetus_2d_edv), TRUE);

    // m_combobox_fetus_2d_edv = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_fetus_2d_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), m_combobox_fetus_2d_edv, 105, 5);
    // gtk_widget_set_size_request (m_combobox_fetus_2d_edv, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_2d_edv), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_2d_edv), _("Teich"));

    // label_fetus_2d_fs = gtk_label_new (_("<b>2D</b> [FS] :"));
    // gtk_widget_show (label_fetus_2d_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), label_fetus_2d_fs, 5, 40);
    // gtk_widget_set_size_request (label_fetus_2d_fs, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_fetus_2d_fs), TRUE);

    // m_combobox_fetus_2d_fs = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_fetus_2d_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), m_combobox_fetus_2d_fs, 105, 40);
    // gtk_widget_set_size_request (m_combobox_fetus_2d_fs, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_2d_fs), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_2d_fs), _("Teich"));

    // label_fetus_m_edv = gtk_label_new (_("<b>M</b> [EDV ESV] :"));
    // gtk_widget_show (label_fetus_m_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), label_fetus_m_edv, 220, 5);
    // gtk_widget_set_size_request (label_fetus_m_edv, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_fetus_m_edv), TRUE);

    // m_combobox_fetus_m_edv = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_fetus_m_edv);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), m_combobox_fetus_m_edv, 320, 5);
    // gtk_widget_set_size_request (m_combobox_fetus_m_edv, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_m_edv), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_m_edv), _("Teich"));

    // label_fetus_m_fs = gtk_label_new (_("<b>M</b> [FS] :"));
    // gtk_widget_show (label_fetus_m_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), label_fetus_m_fs, 220, 40);
    // gtk_widget_set_size_request (label_fetus_m_fs, 100, 30);
    // gtk_label_set_use_markup (GTK_LABEL (label_fetus_m_fs), TRUE);

    // m_combobox_fetus_m_fs = gtk_combo_box_new_text ();
    // gtk_widget_show (m_combobox_fetus_m_fs);
    // gtk_fixed_put (GTK_FIXED (fixed_fetus_echo), m_combobox_fetus_m_fs, 320, 40);
    // gtk_widget_set_size_request (m_combobox_fetus_m_fs, 100, 30);
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_m_fs), _("Cubed"));
    // gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_fetus_m_fs), _("Teich"));

    // label_fetus_echo = gtk_label_new (_("<b>Fetus echo</b>"));
    // gtk_widget_show (label_fetus_echo);
    // gtk_frame_set_label_widget (GTK_FRAME (frame_fetus_echo), label_fetus_echo);
    // gtk_label_set_use_markup (GTK_LABEL (label_fetus_echo), TRUE);

    frame_ob_method = gtk_frame_new (NULL);
    gtk_widget_show (frame_ob_method);
   // gtk_fixed_put (GTK_FIXED (fixed_calc), frame_ob_method, 30, 30);
    gtk_fixed_put (GTK_FIXED (fixed_calc), frame_ob_method, 50, 30);
    gtk_widget_set_size_request (frame_ob_method, 650, 200);
    gtk_frame_set_label_align (GTK_FRAME (frame_ob_method), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_ob_method), GTK_SHADOW_IN);

    fixed_ob_method = gtk_fixed_new ();
    gtk_widget_show (fixed_ob_method);
    gtk_container_add (GTK_CONTAINER (frame_ob_method), fixed_ob_method);

    m_combobox_ob_crl = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_crl);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_crl, 320, 40);
    gtk_widget_set_size_request (m_combobox_ob_crl, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_crl), _("Hadlock"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_crl), _("Robinson"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_crl), _("Hansmenn"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_crl), _("Lasser"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_crl), _("Tokyo"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_crl), _("Custom"));

    m_combobox_ob_fl = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_fl);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_fl, 105, 40);
    gtk_widget_set_size_request (m_combobox_ob_fl, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_fl), _("Hadlock"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_fl), _("Merz"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_fl), _("Jeanty"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_fl), _("Tokyo"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_fl), _("Custom"));

    m_combobox_ob_ac = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_ac);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_ac, 105, 75);
    gtk_widget_set_size_request (m_combobox_ob_ac, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ac), _("Hadlock"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ac), _("Merz"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ac), _("Lasser"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ac), _("Tokyo"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ac), _("Custom"));

    m_combobox_ob_bpd = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_bpd);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_bpd, 535, 5);
    gtk_widget_set_size_request (m_combobox_ob_bpd, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_bpd), _("Hadlock"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_bpd), _("Merz"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_bpd), _("Lasser"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_bpd), _("Rempen"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_bpd), _("Tokyo"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_bpd), _("Custom"));

    m_combobox_ob_gs = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_gs);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_gs, 535, 40);
    gtk_widget_set_size_request (m_combobox_ob_gs, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_gs), _("Hellman"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_gs), _("Rempen"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_gs), _("Tokyo"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_gs), _("Custom"));

    m_combobox_ob_hc = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_hc);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_hc, 320, 75);
    gtk_widget_set_size_request (m_combobox_ob_hc, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_hc), _("Hadlock"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_hc), _("Merz"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_hc), _("Lasser"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_hc), _("Custom"));

    m_combobox_ob_hl = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_hl);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_hl, 320, 5);
    gtk_widget_set_size_request (m_combobox_ob_hl, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_hl), _("Jeanty"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_hl), _("Custom"));

    m_combobox_ob_cer = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_cer);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_cer, 105, 5);
    gtk_widget_set_size_request (m_combobox_ob_cer, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_cer), _("Goldstein"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_cer), _("Custom"));

    label_ob_hl = gtk_label_new (_("<b>HL :</b>"));
    gtk_widget_show (label_ob_hl);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_hl, 220, 5);
    gtk_widget_set_size_request (label_ob_hl, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_hl), TRUE);

    label_ob_bpd = gtk_label_new (_("<b>BPD :</b>"));
    gtk_widget_show (label_ob_bpd);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_bpd, 435, 5);
    gtk_widget_set_size_request (label_ob_bpd, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_bpd), TRUE);

    label_ob_crl = gtk_label_new (_("<b>CRL :</b>"));
    gtk_widget_show (label_ob_crl);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_crl, 220, 40);
    gtk_widget_set_size_request (label_ob_crl, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_crl), TRUE);

    m_combobox_ob_tad = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_tad);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_tad, 535, 75);
    gtk_widget_set_size_request (m_combobox_ob_tad, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_tad), _("Merz"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_tad), _("Custom"));

    label_ob_gs = gtk_label_new (_("<b>GS :</b>"));
    gtk_widget_show (label_ob_gs);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_gs, 435, 40);
    gtk_widget_set_size_request (label_ob_gs, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_gs), TRUE);

    label_ob_cer = gtk_label_new (_("<b>CER :</b>"));
    gtk_widget_show (label_ob_cer);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_cer, 5, 5);
    gtk_widget_set_size_request (label_ob_cer, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_cer), TRUE);

    label_ob_fl = gtk_label_new (_("<b>FL :</b>"));
    gtk_widget_show (label_ob_fl);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_fl, 5, 40);
    gtk_widget_set_size_request (label_ob_fl, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_fl), TRUE);

    label_ob_ac = gtk_label_new (_("<b>AC :</b>"));
    gtk_widget_show (label_ob_ac);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_ac, 5, 75);
    gtk_widget_set_size_request (label_ob_ac, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_ac), TRUE);

    label_ob_apad = gtk_label_new (_("<b>APAD :</b>"));
    gtk_widget_show (label_ob_apad);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_apad, 5, 110);
    gtk_widget_set_size_request (label_ob_apad, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_apad), TRUE);

    m_combobox_ob_apad = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_apad);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_apad, 105, 110);
    gtk_widget_set_size_request (m_combobox_ob_apad, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_apad), _("Merz"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_apad), _("Custom"));

    label_ob_hc = gtk_label_new (_("<b>HC :</b>"));
    gtk_widget_show (label_ob_hc);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_hc, 220, 75);
    gtk_widget_set_size_request (label_ob_hc, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_hc), TRUE);

    label_ob_thd = gtk_label_new (_("<b>THD :</b>"));
    gtk_widget_show (label_ob_thd);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_thd, 220, 110);
    gtk_widget_set_size_request (label_ob_thd, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_thd), TRUE);

    m_combobox_ob_thd = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_thd);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_thd, 320, 110);
    gtk_widget_set_size_request (m_combobox_ob_thd, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_thd), _("Hansmenn"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_thd), _("Custom"));

    label_ob_tad = gtk_label_new (_("<b>TAD :</b>"));
    gtk_widget_show (label_ob_tad);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_tad, 435, 75);
    gtk_widget_set_size_request (label_ob_tad, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_tad), TRUE);

    label_ob_ofd = gtk_label_new (_("<b>OFD :</b>"));
    gtk_widget_show (label_ob_ofd);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_ofd, 435, 110);
    gtk_widget_set_size_request (label_ob_ofd, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_ofd), TRUE);

    m_combobox_ob_ofd = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_ofd);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_ofd, 535, 110);
    gtk_widget_set_size_request (m_combobox_ob_ofd, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ofd), _("Korean"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_ofd), _("Custom"));

    label_ob_efw = gtk_label_new (_("<b>EFW :</b>"));
    gtk_widget_show (label_ob_efw);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), label_ob_efw, 5, 145);
    gtk_widget_set_size_request (label_ob_efw, 100, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_efw), TRUE);

    m_combobox_ob_efw = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_efw);
    gtk_fixed_put (GTK_FIXED (fixed_ob_method), m_combobox_ob_efw, 105, 145);
    gtk_widget_set_size_request (m_combobox_ob_efw, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Hadlock1"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Hadlock2"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Hadlock3"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Hadlock4"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Shepard"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Hansmenn"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_efw), _("Tokyo"));

    label_ob_method = gtk_label_new (_("<b>OB Calc Method</b>"));
    gtk_widget_show (label_ob_method);
    gtk_frame_set_label_widget (GTK_FRAME (frame_ob_method), label_ob_method);
    gtk_label_set_use_markup (GTK_LABEL (label_ob_method), TRUE);

    button_ob_custom = gtk_button_new_with_mnemonic (_("OB Custom"));
    gtk_widget_show (button_ob_custom);
    //gtk_fixed_put (GTK_FIXED (fixed_calc), button_ob_custom, 30, 260);
    gtk_fixed_put (GTK_FIXED (fixed_calc), button_ob_custom, 80, 260);
    gtk_widget_set_size_request (button_ob_custom, 115, 30);
    g_signal_connect ((gpointer) button_ob_custom, "clicked",
            G_CALLBACK (on_button_ob_custom_clicked),
            this);

    label_bsa = gtk_label_new (_("<b>BSA :</b>"));
    gtk_widget_show (label_bsa);
    gtk_fixed_put (GTK_FIXED (fixed_calc), label_bsa, 245+20, 260);
    gtk_widget_set_size_request (label_bsa, 108, 30);
    gtk_label_set_use_markup (GTK_LABEL (label_bsa), TRUE);

    m_combobox_bsa = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_bsa);
    gtk_fixed_put (GTK_FIXED (fixed_calc), m_combobox_bsa, 345, 260);
    gtk_widget_set_size_request (m_combobox_bsa, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bsa), _("Oriental"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_bsa), _("Occidental"));

    button_calc_default = gtk_button_new_with_mnemonic (_("Default Factory"));
    gtk_widget_show (button_calc_default);
    //gtk_fixed_put (GTK_FIXED (fixed_calc), button_calc_default, 30, 450);
    gtk_fixed_put (GTK_FIXED (fixed_calc), button_calc_default, 50, 450);
    gtk_widget_set_size_request (button_calc_default, 140+8, 40);
    g_signal_connect ((gpointer) button_calc_default, "clicked", G_CALLBACK (on_button_calc_default_clicked), this);

    return fixed_calc;
}

void ViewSystem::BtnOBCustomClicked(GtkButton *button)
{
    ViewCustomOB::GetInstance()->CreateWindow(m_window);
}

void ViewSystem::init_calc_setting(SysCalculateSetting* sysCalc)
{
    if (sysCalc == NULL) {
        sysCalc = new SysCalculateSetting;
    }

    int bsa = sysCalc->GetBSAMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_bsa), bsa);

    int cer = sysCalc->GetCerMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_cer), cer);

    int hl = sysCalc->GetHlMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_hl), hl);

    int bpd = sysCalc->GetBpdMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_bpd), bpd);

    int fl = sysCalc->GetFlMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_fl), fl);

    int crl = sysCalc->GetCrlMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_crl), crl);

    int gs = sysCalc->GetGsMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_gs), gs);

    int ac = sysCalc->GetAcMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_ac), ac);

    int hc = sysCalc->GetHcMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_hc), hc);

    int tad = sysCalc->GetTadMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_tad), tad);

    int apad = sysCalc->GetApadMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_apad), apad);

    int thd = sysCalc->GetThdMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_thd), thd);

    int ofd = sysCalc->GetOfdMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_ofd), ofd);

    int efw = sysCalc->GetEfwMethod();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_efw), efw);
    delete sysCalc;
}

void ViewSystem::save_calc_setting(void)
{
    SysCalculateSetting sysCalc;

    int bsa = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_bsa));
    sysCalc.SetBSAMethod(bsa);

    int cer = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_cer));
    sysCalc.SetCerMethod(cer);

    int hl = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_hl));
    sysCalc.SetHlMethod(hl);

    int bpd = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_bpd));
    sysCalc.SetBpdMethod(bpd);

    int fl = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_fl));
    sysCalc.SetFlMethod(fl);

    int crl = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_crl));
    sysCalc.SetCrlMethod(crl);

    int gs = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_gs));
    sysCalc.SetGsMethod(gs);

    int ac = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_ac));
    sysCalc.SetAcMethod(ac);

    int hc = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_hc));
    sysCalc.SetHcMethod(hc);

    int tad = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_tad));
    sysCalc.SetTadMethod(tad);

    int apad = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_apad));
    sysCalc.SetApadMethod(apad);

    int thd = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_thd));
    sysCalc.SetThdMethod(thd);

    int ofd = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_ofd));
    sysCalc.SetOfdMethod(ofd);

    int efw = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_efw));
    sysCalc.SetEfwMethod(efw);

    sysCalc.SyncFile();
    sysCalc.UpdateOBSetting();

    g_menuCalc.UpdateEfwItem(efw);
}

GtkWidget* ViewSystem::create_note_info(void)
{
     GtkWidget *fixed_info;
#ifdef CT_3410
    GtkWidget *frame_version;
    GtkWidget *scrolledwindow_version;
    GtkWidget *label_version;
    GtkWidget *button_upgrade;
    const int X0 = 20;
    fixed_info = gtk_fixed_new ();
    gtk_widget_show (fixed_info);

    frame_version = gtk_frame_new (NULL);
    gtk_widget_show (frame_version);
    gtk_fixed_put (GTK_FIXED (fixed_info), frame_version, X0, 10);
    gtk_widget_set_size_request (frame_version, 360+150, 160);
    gtk_frame_set_label_align (GTK_FRAME (frame_version), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_version), GTK_SHADOW_IN);

    GtkWidget *fixed_system_info = gtk_fixed_new ();
    gtk_widget_show (fixed_system_info);
    gtk_container_add (GTK_CONTAINER (frame_version), fixed_system_info);

    GtkWidget *frame_container = gtk_frame_new(NULL);
    gtk_widget_show(frame_container);
    gtk_fixed_put (GTK_FIXED (fixed_system_info), frame_container, 10, 10);
    gtk_widget_set_size_request (frame_container, 360, 120);
    gtk_frame_set_label_align (GTK_FRAME (frame_container), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_container), GTK_SHADOW_IN);

    scrolledwindow_version = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_version);
    gtk_container_add (GTK_CONTAINER (frame_container), scrolledwindow_version);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow_version), 5);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_version), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_version), GTK_SHADOW_IN);

    GtkWidget *label_frame_container = gtk_label_new (_("<b>Software and FPGA Version</b>"));
    gtk_widget_show (label_frame_container);
    gtk_frame_set_label_widget (GTK_FRAME (frame_container), label_frame_container);
    gtk_label_set_use_markup (GTK_LABEL (label_frame_container), TRUE);

    m_textview_version = gtk_text_view_new ();
    gtk_widget_show (m_textview_version);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_version), m_textview_version);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (m_textview_version), FALSE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_version), GTK_WRAP_WORD);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (m_textview_version), FALSE);

    label_version = gtk_label_new (_("System Info"));
    gtk_widget_show (label_version);
    gtk_frame_set_label_widget (GTK_FRAME (frame_version), label_version);
    gtk_label_set_use_markup (GTK_LABEL (label_version), TRUE);

    button_upgrade = gtk_button_new_with_mnemonic (_("Upgrade"));
    gtk_widget_show (button_upgrade);
    gtk_fixed_put (GTK_FIXED (fixed_system_info), button_upgrade, 360+150-10-100, 60);
    gtk_widget_set_size_request (button_upgrade, 100, 35);

    g_signal_connect ((gpointer) button_upgrade, "clicked",
            G_CALLBACK (on_button_upgrade_clicked),
            this);

    ///options list
    GtkWidget *frame_option = gtk_frame_new(NULL);
    gtk_widget_show (frame_option);
    gtk_fixed_put (GTK_FIXED (fixed_info), frame_option, X0, 10+160+20);
    gtk_widget_set_size_request (frame_option, 360+150, 300);
    gtk_frame_set_label_align (GTK_FRAME (frame_option), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_option), GTK_SHADOW_IN);

    GtkWidget *scrolledwindow_option = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (frame_option), scrolledwindow_option);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow_option), 5);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_option), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_option), GTK_SHADOW_IN);

    GtkTreeModel *modelOption = GTK_TREE_MODEL(gtk_list_store_new(2,
                G_TYPE_STRING,
                G_TYPE_STRING));
    m_treeOption = gtk_tree_view_new_with_model(modelOption);
    gtk_widget_set_usize(m_treeOption, 360, 200);
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeOption), FALSE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_treeOption), TRUE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeOption)), GTK_SELECTION_SINGLE);

    char buf[10];
    char *title_list[]={"Option", "State"};
    for(int i = 0; i < 2; i++)
    {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(i));
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", i, NULL);
        GtkWidget *label = create_label(_(title_list[i]), 0, 0, g_white, NULL);
        g_object_set(G_OBJECT(column), "widget", label, "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 170, NULL);
        gtk_widget_show(label);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeOption), column);
    }

    gtk_list_store_clear(GTK_LIST_STORE(modelOption));
    GtkTreeIter iter;
    char status[50];
    for(int row=1; row<CManRegister::MAX_OPTIONAL_NUM+1; row++)
    {
        gtk_list_store_append(GTK_LIST_STORE(modelOption), &iter);///<增加一行
        if(CManRegister::GetInstance()->IsAuthorize(CManRegister::Optional[row-1]))
            strcpy(status, "Installed");
        else
            strcpy(status, "Disabled");
        gtk_list_store_set(GTK_LIST_STORE(modelOption), &iter,
                0, _(CManRegister::Optional[row-1].c_str()),
                1, _(status),
                -1);
    }
    g_signal_connect(G_OBJECT(m_treeOption), "cursor-changed", G_CALLBACK(HandleOptionCursorChanged), this);
    g_signal_connect(G_OBJECT(m_treeOption), "event-after", G_CALLBACK(HandleOptionOptEvent), this);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_option), m_treeOption);
    gtk_widget_show_all(scrolledwindow_option);

    ///>register info
    m_frameRegisterInfo = gtk_frame_new(NULL);
    gtk_widget_show (m_frameRegisterInfo);
    gtk_fixed_put (GTK_FIXED (fixed_info), m_frameRegisterInfo, X0+360+150+20, 10);
    gtk_widget_set_size_request (m_frameRegisterInfo, 330, 480);
    gtk_frame_set_label_align (GTK_FRAME (m_frameRegisterInfo), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (m_frameRegisterInfo), GTK_SHADOW_IN);

    GtkWidget *fixed_register_info = gtk_fixed_new ();
    gtk_widget_show (fixed_register_info);
    gtk_container_add (GTK_CONTAINER (m_frameRegisterInfo), fixed_register_info);

    GtkWidget *label_frame_register = gtk_label_new (_("Register"));
    gtk_widget_show (label_frame_register);
    gtk_frame_set_label_widget (GTK_FRAME (m_frameRegisterInfo), label_frame_register);
    gtk_label_set_use_markup (GTK_LABEL (label_frame_register), TRUE);

    GtkWidget *labelNote = gtk_label_new(_("Note: According to the license file,the manufacturer will generate a unique register key for every machine,you can register optional function by inputting thist register key.If register is successful,you can use the optional function.\n\nPlease insert Udisk,and click button 'Export License File'. You will find this file named 'license' in Udisk. Please send this file to manufacturer."));
    gtk_widget_show(labelNote);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), labelNote, 10, 5);
    gtk_misc_set_alignment (GTK_MISC(labelNote), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (labelNote), TRUE);
    gtk_widget_set_size_request (labelNote, 330-10-10, 200);
    gtk_label_set_line_wrap(GTK_LABEL(labelNote), true);
    gtk_label_set_line_wrap_mode(GTK_LABEL(labelNote), PANGO_WRAP_WORD_CHAR);

    const int YT = 20;
    m_labelExport = gtk_label_new ("");
    gtk_widget_show (m_labelExport);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), m_labelExport, 10, 200-10);
    gtk_label_set_use_markup (GTK_LABEL (m_labelExport), TRUE);
    gtk_widget_set_size_request (m_labelExport, 330-15, 33);
    gtk_label_set_justify(GTK_LABEL(m_labelExport), GTK_JUSTIFY_LEFT);

    GtkWidget *btnExport = gtk_button_new_with_mnemonic(_("Export License File"));
    gtk_widget_show(btnExport);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), btnExport, 330-200-10, 5+200+10+YT);
    gtk_widget_set_size_request (btnExport, 200, 35);
    g_signal_connect (G_OBJECT(btnExport), "clicked", G_CALLBACK(HandleBtnExportClicked), this);
    GtkWidget *hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), hseparator1, 8, 5+200+10+35+5+YT);
    gtk_widget_set_size_request (hseparator1, 330-16, 16);

    GtkWidget *labelKey = gtk_label_new (_("Register Key:"));
    gtk_widget_show (labelKey);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), labelKey, 10, 5+200+10+34+5+16+20+YT-10);
    gtk_label_set_use_markup (GTK_LABEL (labelKey), TRUE);

    m_entryRegisterKey = gtk_entry_new ();
    gtk_widget_show (m_entryRegisterKey);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), m_entryRegisterKey, 10+10, 5+200+10+34+5+16+20+25+YT-10);
    gtk_widget_set_size_request (m_entryRegisterKey, 330-20-10, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entryRegisterKey), 9679);

    m_labelRegister = gtk_label_new ("");
    gtk_widget_show (m_labelRegister);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), m_labelRegister, 10, 5+200+10+34+5+16+20+35+30+15+YT-20);
    gtk_label_set_use_markup (GTK_LABEL (m_labelRegister), TRUE);
    gtk_widget_set_size_request (m_labelRegister, 200, 33);

    GtkWidget *btnRegister = gtk_button_new_with_mnemonic(_("Register"));
    gtk_widget_show(btnRegister);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), btnRegister, 330-10-100, 5+200+10+34+5+16+20+35+30+15+YT-20);
    gtk_widget_set_size_request (btnRegister, 100, 35);
    g_signal_connect (G_OBJECT(btnRegister), "clicked", G_CALLBACK(HandleBtnRegisterClicked), this);

    GtkWidget *labelWarning = gtk_label_new(_("If the registeration is successful, it will take effect after the machine is rebooted."));
    gtk_widget_show(labelWarning);
    gtk_fixed_put (GTK_FIXED (fixed_register_info), labelWarning, 10, 5+200+10+34+5+16+20+35+30+15+YT+35+5-25);
    gtk_label_set_use_markup (GTK_LABEL (labelWarning), TRUE);
    gtk_widget_set_size_request (labelWarning, 330-10-10, 50);
    gtk_label_set_line_wrap(GTK_LABEL(labelWarning), true);
    gtk_label_set_line_wrap_mode(GTK_LABEL(labelWarning), PANGO_WRAP_WORD_CHAR);

    gtk_widget_hide(m_frameRegisterInfo);
#else
    GtkWidget *frame_info_model;
    GtkWidget *scrolledwindow_info_model;
    GtkWidget *label_info_model;
    GtkWidget *frame_version;
    GtkWidget *scrolledwindow_version;
    //    GtkWidget *textview_version;
    GtkWidget *label_version;
    GtkWidget *button_upgrade;

    fixed_info = gtk_fixed_new ();
    gtk_widget_show (fixed_info);
    frame_info_model = gtk_frame_new (NULL);
    gtk_widget_hide (frame_info_model);
   // gtk_fixed_put (GTK_FIXED (fixed_info), frame_info_model, 30, 30);
    gtk_fixed_put (GTK_FIXED (fixed_info), frame_info_model, 50, 30);
    gtk_widget_set_size_request (frame_info_model, 250, 120);
    gtk_frame_set_label_align (GTK_FRAME (frame_info_model), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_info_model), GTK_SHADOW_IN);

    scrolledwindow_info_model = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_hide (scrolledwindow_info_model);
    gtk_container_add (GTK_CONTAINER (frame_info_model), scrolledwindow_info_model);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow_info_model), 5);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_info_model), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_info_model), GTK_SHADOW_IN);

    label_info_model = gtk_label_new (_("<b>Model</b>"));
    gtk_widget_hide (label_info_model);
    gtk_frame_set_label_widget (GTK_FRAME (frame_info_model), label_info_model);
    gtk_label_set_use_markup (GTK_LABEL (label_info_model), TRUE);

    frame_version = gtk_frame_new (NULL);
    gtk_widget_show (frame_version);
   // gtk_fixed_put (GTK_FIXED (fixed_info), frame_version, 350, 30);
    gtk_fixed_put (GTK_FIXED (fixed_info), frame_version, 450, 30);
    gtk_widget_set_size_request (frame_version, 360, 224);
    gtk_frame_set_label_align (GTK_FRAME (frame_version), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame_version), GTK_SHADOW_IN);

    scrolledwindow_version = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_version);
    gtk_container_add (GTK_CONTAINER (frame_version), scrolledwindow_version);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow_version), 5);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_version), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_version), GTK_SHADOW_IN);

    m_textview_version = gtk_text_view_new ();
    gtk_widget_show (m_textview_version);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_version), m_textview_version);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (m_textview_version), FALSE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (m_textview_version), GTK_WRAP_WORD);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (m_textview_version), FALSE);
    //  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (m_textview_version)), _("Software version:\n\nFPGA version:"), -1);

    label_version = gtk_label_new (_("<b>Software and FPGA Version</b>"));
    gtk_widget_show (label_version);
    gtk_frame_set_label_widget (GTK_FRAME (frame_version), label_version);
    gtk_label_set_use_markup (GTK_LABEL (label_version), TRUE);

    button_upgrade = gtk_button_new_with_mnemonic (_("Upgrade"));
    gtk_widget_show (button_upgrade);
   // gtk_fixed_put (GTK_FIXED (fixed_info), button_upgrade, 350, 280);
    gtk_fixed_put (GTK_FIXED (fixed_info), button_upgrade, 450, 280);

    gtk_widget_set_size_request (button_upgrade, 120, 40);

    g_signal_connect ((gpointer) button_upgrade, "clicked",
            G_CALLBACK (on_button_upgrade_clicked),
            this);
#endif
    return fixed_info;
}

void ViewSystem::init_info_setting(void)
{
    //    SysSystemInfo sys;
    string machine, soft, fpga;

    UpgradeMan::GetInstance()->GetMachineType(machine);
    UpgradeMan::GetInstance()->GetSoftVersion(soft);
    UpgradeMan::GetInstance()->GetFpgaVersion(fpga);

    gchar *buf;
    gchar *b1 = _("Software version");
    gchar *b2 = _("FPGA version");

    if(g_authorizationOn)
    {
        int rest = CEmpAuthorization::GetRestTime();
        if (rest == CEmpAuthorization::PERMANENTDAY)
        {
            buf = g_strdup_printf("%s: %s\n%s\n\n%s: %s", b1, soft.c_str(), _("This is charging software, the period of validity is permanent."), b2, fpga.c_str());
        }
        else
        {
            buf = g_strdup_printf("%s: %s\n%s %d %s\n\n%s: %s", b1, soft.c_str(), _("This is charging software, the period of validity is"), rest, _("days."), b2, fpga.c_str());
        }
    }
    else
        buf = g_strdup_printf("%s: %s\n\n%s: %s", b1, soft.c_str(), b2, fpga.c_str());
    gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (m_textview_version)), buf, -1);
    g_free(buf);
}

GtkWidget* ViewSystem::create_common_print_treeview(void)
{
    GtkWidget *treeview;
    GtkTreeModel *model;

    GtkCellRenderer *render_brand, *render_model;
    GtkTreeViewColumn *col_brand, *col_model;

    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);

    render_brand = gtk_cell_renderer_text_new();
    col_brand = gtk_tree_view_column_new_with_attributes(_("Brand"), render_brand, "text", PRT_BRAND, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col_brand);

    render_model = gtk_cell_renderer_text_new();
    col_model = gtk_tree_view_column_new_with_attributes(_("Model"), render_model, "text", PRT_MODEL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col_model);

    model = create_common_print_model();
    if (model != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); /* destroy model automatically with view */

    return treeview;
}

GtkTreeModel* ViewSystem::create_common_print_model(void)
{
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new(CNUM_COLS, G_TYPE_STRING, G_TYPE_STRING);
    for (unsigned int i = 0; i < G_N_ELEMENTS(common_printer_data); i++) {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set(store, &iter,
                PRT_BRAND, common_printer_data[i].brand.c_str(),
                PRT_MODEL, common_printer_data[i].model.c_str(),
                -1);
    }
    return GTK_TREE_MODEL (store);
}

GtkWidget* ViewSystem::create_specific_print_treeview(void)
{
    GtkWidget *treeview;
    GtkTreeModel *model;

    GtkCellRenderer *render_default, *render_name, *render_status, *render_message;
    GtkTreeViewColumn *col_default, *col_name, *col_status, *col_message;

    treeview = gtk_tree_view_new ();
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW (treeview), FALSE);

    render_name = gtk_cell_renderer_text_new();
    col_name = gtk_tree_view_column_new_with_attributes(_("Printer"), render_name, "text", PRT_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col_name);

    render_default = gtk_cell_renderer_text_new();
    col_default = gtk_tree_view_column_new_with_attributes(_("Default"), render_default, "text", PRT_DEFAULT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col_default);

    render_status = gtk_cell_renderer_text_new();
    col_status = gtk_tree_view_column_new_with_attributes(_("Status"), render_status, "text", PRT_STATUS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col_status);

    render_message = gtk_cell_renderer_text_new();
    col_message = gtk_tree_view_column_new_with_attributes(_("Message"), render_message, "text", PRT_MESSAGE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), col_message);

    model = create_specific_print_model();
    if (model != NULL)
        gtk_tree_view_set_model (GTK_TREE_VIEW(treeview), model);
    g_object_unref (model); /* destroy model automatically with view */

    return treeview;
}

GtkTreeModel* ViewSystem::create_specific_print_model(void)
{
    GtkListStore  *store;
    GtkTreeIter    iter;

    store = gtk_list_store_new(SNUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    Printer print_data;
    if (print_data.GetPrinters()) {
        const vector<Printer::printer_attr> *vec_prt = print_data.get_printers();
        vector<Printer::printer_attr>::const_iterator iter_prt;
        for (iter_prt = vec_prt->begin(); iter_prt != vec_prt->end(); ++iter_prt) {
            string tag_default;
            if (iter_prt->isDefault)
                tag_default = "*";
            else
                tag_default = "";

            gtk_list_store_append (store, &iter);
            gtk_list_store_set(store, &iter,
                    PRT_NAME, iter_prt->name.c_str(),
                    PRT_DEFAULT, tag_default.c_str(),
                    PRT_STATUS, iter_prt->status.c_str(),
                    PRT_MESSAGE, iter_prt->message.c_str(),
                    -1);
        }
    }
    return GTK_TREE_MODEL (store);
}

GtkWidget * ViewSystem::create_key_function_treeview(const string function_list[], unsigned int size)
{
    GtkWidget *treeview;
    GtkListStore *store;
    GtkTreeIter    iter;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    store = gtk_list_store_new(1, G_TYPE_STRING);
    for (unsigned int i = 0; i < size; i++) {
        gtk_list_store_append (store, &iter);
        gtk_list_store_set(store, &iter,
                0, _(function_list[i].c_str()),
                -1);
    }
    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview), FALSE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(_("Function"), renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    return treeview;
}

void  ViewSystem::create_exam_comment_model(vector<int> index)
{
    vecExamItem_comment.clear();
    vector<int>::iterator iter_index;

    int probeIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_probe_comment));
#if 0
    ExamItem::ParaItem paraItem;
    ExamItem examItem;
    for(int i=0;i<10;i++)
    {
        examItem.ReadExamItemPara(probeIndex,i, &paraItem,NULL);
        ExamItemArray[i].name=paraItem.alias;

    }
#endif
    for (iter_index = index.begin(); iter_index != index.end(); iter_index++)
      {
          vecExamItem_comment.push_back(ExamItemArray[*iter_index].name);

      }
    CreateDefineItem_comment(probeIndex, vecExamItem_comment);

    int exam_size(0);
    exam_size = vecExamItem_comment.size();

   GtkWidget *label_exam_comment = gtk_label_new (_("Exam Type:"));
    gtk_misc_set_alignment (GTK_MISC(label_exam_comment), 0, 0.5);
    gtk_label_set_use_markup (GTK_LABEL (label_exam_comment), TRUE);
    gtk_widget_show (label_exam_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), label_exam_comment, 330-5-17, 10+10+5);
    gtk_widget_set_size_request (label_exam_comment, 100+17, 30);

    m_combobox_exam_comment = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_exam_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_exam_comment, 415, 10+10+5);
    gtk_widget_set_size_request (m_combobox_exam_comment, 120, 30);
    for (int i = 0; i < exam_size; i++) {
        gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_exam_comment), _(vecExamItem_comment[i].c_str()));
    }

    int exam_type_index=0;
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    string current_exam;
    current_exam=exam.ReadDefaultProbeDefaultItemName(&ini);
    int j=0;
    for (j = 0; j < exam_size; j++)
    {
        if(strcmp(current_exam.c_str(), vecExamItem_comment[j].c_str())==0)
        {
            exam_type_index = j;
            break;
        }
        else
            exam_type_index = 0;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_exam_comment), exam_type_index);

    g_signal_connect(m_combobox_exam_comment, "changed", G_CALLBACK(HandleExamCommentChanged), this);

    int department_index = DepartmentIndex();
    m_combobox_department_comment = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_department_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), m_combobox_department_comment, 190, 10+20+50-10);//300+310+150+10-100+30, 10+10+5);
    gtk_widget_set_size_request (m_combobox_department_comment, 120, 30);

    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Abdomen"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Urology"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Cardiac"));

    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Obstetrics"));
#ifndef VET
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Gynecology"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Small Part"));
#ifndef CT_322
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Vascular"));
#endif
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_department_comment), _("Orthopedic"));
   gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_department_comment), department_index);
    g_signal_connect(m_combobox_department_comment, "changed", G_CALLBACK(HandleDepartmentCommentChanged), this);

 scrolledwindow_item_comment = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_comment);
    gtk_fixed_put (GTK_FIXED (fixed_comment), scrolledwindow_item_comment, 90, 10+20+50-5+40-5);
    gtk_widget_set_size_request (scrolledwindow_item_comment, 120+100, 200+63+20-40+10);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_comment), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkTreeModel *model = create_item_comment_model(0);
    m_treeview_item_comment = gtk_tree_view_new_with_model(model);
    add_columns_comment(GTK_TREE_VIEW(m_treeview_item_comment));

    gtk_widget_modify_base(m_treeview_item_comment, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_comment), m_treeview_item_comment);

    GtkTreeSelection *select;
    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    g_object_set(m_cellrenderer_comment_text, "editable", TRUE, NULL);
    g_signal_connect(m_cellrenderer_comment_text, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(m_cellrenderer_comment_text, "edited", G_CALLBACK(HandleCellRendererRenameSelectComment), this);

    gtk_widget_show (m_treeview_item_comment);

 //select note item
  scrolledwindow_item_comment1 = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_item_comment1);
    gtk_fixed_put (GTK_FIXED (fixed_comment), scrolledwindow_item_comment1, 470, 10+20+50-5+40-5);//-3-2);
    gtk_widget_set_size_request (scrolledwindow_item_comment1, 120+100, 200+63+20-40+10);//+5+3+2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_item_comment1), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    GtkTreeModel *model1 = create_item_comment_model1();
    m_treeview_item_comment1 = gtk_tree_view_new_with_model(model1);
    add_columns_comment1(GTK_TREE_VIEW(m_treeview_item_comment1));

    gtk_widget_modify_base(m_treeview_item_comment1, GTK_STATE_NORMAL, g_deep);

    gtk_container_add (GTK_CONTAINER (scrolledwindow_item_comment1), m_treeview_item_comment1);

    GtkTreeSelection *select1;
    select1 = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_item_comment1));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_BROWSE);

    g_object_set(m_cellrenderer_comment_text1, "editable", TRUE, NULL);
    g_signal_connect(m_cellrenderer_comment_text1, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);
    g_signal_connect(m_cellrenderer_comment_text1, "edited", G_CALLBACK(HandleCellRendererRenameComment), this);

    gtk_widget_show (m_treeview_item_comment1);

}

GtkTreeModel* ViewSystem::create_exam_item_model(vector<int> index)
{
    vector<ExamPara> vecExamItem;
    vecExamItem.clear();
    vector<int>::iterator iter_index;
    int probe_index = 0;
    string probe_type = TopArea::GetInstance()->GetProbeType();
    for (int i = 0; i < NUM_PROBE; ++i) {
        if (probe_type == PROBE_LIST[i])
            probe_index = i;
    }

    int probeIndex = GetProbeType();
    string str_check_part("");
    TopArea::GetInstance()->GetCheckPart(str_check_part);
#if 0
    ExamItem::ParaItem paraItem;
    ExamItem examItem;
    for(int i=0;i<10;i++)
    {
        examItem.ReadExamItemPara(probeIndex,i, &paraItem,NULL);
        ExamItemArray[i].name=paraItem.alias;
        PRINTF("---name=%s\n", ExamItemArray[i].name.c_str());
    }
#endif
    for (iter_index = index.begin(); iter_index != index.end(); iter_index++)
    {
        vecExamItem.push_back(ExamItemArray[*iter_index]);
    }

    CreateDefineItem(probeIndex, vecExamItem);

    if (vecExamItem.empty())
        return NULL;

    GtkTreeIter iter;
    GtkTreeIter child_iter;
    GtkTreeStore *store = gtk_tree_store_new(N_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_INT);

    vector<ExamPara>::iterator iterItem;
    for (iterItem = vecExamItem.begin(); iterItem != vecExamItem.end(); iterItem++) {

#ifdef VET
        gtk_tree_store_append(store, &iter, NULL);

        gtk_tree_store_set(store, &iter,
                NAME_COLUMN, _(iterItem->name.c_str()),
                INDEX_COLUMN, iterItem->index,
                -1);

        if((strcmp(str_check_part.c_str(), _(iterItem->name.c_str())) == 0) && (probe_index == probeIndex))
        {
            GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
            m_str_index = gtk_tree_path_to_string (path);
            PRINTF("path111111:%s\n", m_str_index);
            gtk_tree_path_free(path);
        }
        else if(probe_index != probeIndex)
        {
            m_str_index = NULL;
        }

        goto next;

#else
        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter)) {
            bool has_node = true;
            char *strtmp;
            while (has_node) {

                gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, NAME_COLUMN, &strtmp, -1);

                if (strcmp(_(iterItem->dept_name.c_str()), strtmp) == 0) {
                    gtk_tree_store_append(store, &child_iter, &iter);
                    gtk_tree_store_set(store, &child_iter,
                            NAME_COLUMN, _(iterItem->name.c_str()),
                            INDEX_COLUMN, iterItem->index,
                            -1);
                    if((strcmp(str_check_part.c_str(), _(iterItem->name.c_str())) == 0) && (probe_index == probeIndex))
                    {
                        GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &child_iter);
                        m_str_index = gtk_tree_path_to_string (path);
                        PRINTF("path:%s\n", m_str_index);
                        gtk_tree_path_free(path);
                    }
                    else if(probe_index != probeIndex)
                    {
                        m_str_index = NULL;
                    }

                    goto next;
                } else {
                    has_node = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
                }
            }
        }
        if ((iterItem->name).empty()) {
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, _(iterItem->dept_name.c_str()),
                    INDEX_COLUMN, iterItem->index,
                    -1);
        } else {
            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter,
                    NAME_COLUMN, _(iterItem->dept_name.c_str()),
                    INDEX_COLUMN, -1,
                    -1);
            gtk_tree_store_append(store, &child_iter, &iter);
            gtk_tree_store_set(store, &child_iter,
                    NAME_COLUMN, _(iterItem->name.c_str()),
                    INDEX_COLUMN, iterItem->index,
                    -1);

          if((strcmp(str_check_part.c_str(), _(iterItem->name.c_str())) == 0) && (probe_index == probeIndex))
            {
                // printf("chekc:%s\n", str_check_part.c_str());
                GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &child_iter);
                m_str_index = gtk_tree_path_to_string (path);
                PRINTF("path:%s\n", m_str_index);
                gtk_tree_path_free(path);
            }
            else if(probe_index != probeIndex)
            {
                m_str_index = NULL;
            }

        }
#endif
next:
        continue;
    }
    return GTK_TREE_MODEL(store);

}

#ifdef VET
void ViewSystem::add_columns(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_text = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1,_("User"),m_cellrenderer_text, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}
#endif

void ViewSystem::add_columns_comment(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_comment_text = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1,_("Select Item"),m_cellrenderer_comment_text, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}
void ViewSystem::add_columns_comment1(GtkTreeView *treeview)
{
    gint col_offset;
    GtkTreeViewColumn *column;

    m_cellrenderer_comment_text1 = gtk_cell_renderer_text_new();
    col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
            -1,_("Selected Item"),m_cellrenderer_comment_text1, "text", 0,NULL);
    column = gtk_tree_view_get_column (GTK_TREE_VIEW (treeview), col_offset - 1);
    gtk_tree_view_column_set_clickable (GTK_TREE_VIEW_COLUMN (column), TRUE);
}

void ViewSystem::add_exam_item_column(GtkTreeView *treeview)
{
  //  GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
#ifdef VET
    column = gtk_tree_view_column_new_with_attributes(_("User"), renderer, "text", NAME_COLUMN, NULL);
#else
    column = gtk_tree_view_column_new_with_attributes(_("Department"), renderer, "text", NAME_COLUMN, NULL);
#endif
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

static gboolean ExitWindow(gpointer data)
{
    ViewSystem *tmp;
    tmp = (ViewSystem *)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewSystem::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);
    switch(keyValue)
    {
        case KEY_ESC:
        case KEY_SYSTEM:
            if (!m_vgaInterl)
            {
                    gtk_combo_box_set_active(GTK_COMBO_BOX(ViewSystem::GetInstance()->GetVGACombobox()), 0);
                    return;
            }
        case KEY_P1:
            g_timeout_add(700, ExitWindow, this);
            FakeEscKey();
#ifdef CT_355
            FreezeMode::GetInstance()->PressUnFreeze();
            MultiFuncUndo();//2016.11.05--solve problem that biopsy verify don't exit.
#endif
            break;
        default:
            break;
    }
}

void ViewSystem::BtnExitClicked(GtkButton *button)
{
    DestroyWindow();
}

void ViewSystem::BtnApplyClicked(GtkButton *button)
{
    //if language changed, relocalize the CustomReport.db
    SysGeneralSetting sys;
    int langage = sys.GetLanguage();
    int index_lang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_language));
    if(langage != index_lang)
    {
        InitDB();
        LocalizationDB(); // 110~120ms
    }

    apply_report_setting();//add by jhuang
    save_general_setting();
    save_option_setting();
    save_comment_setting();
    if(m_current_note_page == m_flag_notebook_tvout)
        save_tvout_setting();//改成了"Peripheral"
    //save_dicom_setting();
    save_measure_setting();
    //save_image_setting();
    save_key_config();
#ifndef VET
    save_calc_setting();
#endif
}

bool system_save = false;
void ViewSystem::BtnSaveClicked(GtkButton *button)
{
    //if language changed, relocalize the CustomReport.db
    SysGeneralSetting sys;
    int langage = sys.GetLanguage();
    int index_lang = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_language));
    if(langage != index_lang)
    {
        InitDB();
        LocalizationDB(); // 110~120ms
    }

    /*BtnSave2Clicked(NULL);//add by jhuang
    save_general_setting();
    save_option_setting();
    save_comment_setting();
    if(m_current_note_page == m_flag_notebook_tvout)
        save_tvout_setting();//改成了"Peripheral" 避免打印机总是死机问题
    //save_dicom_setting();
    save_measure_setting();
    save_key_config();
#ifndef VET
    save_calc_setting();
#endif
    DestroyWindow();
    system_save = true;*/

    BtnSave2Clicked(NULL);//add by jhuang
    save_general_setting();
    save_option_setting();
    save_comment_setting();
    if(m_current_note_page == m_flag_notebook_tvout)
        save_tvout_setting();//改成了"Peripheral" 避免打印机总是死机问题
    //save_dicom_setting();
    save_measure_setting();
    save_key_config();
#ifndef VET
    save_calc_setting();
#endif
    g_timeout_add(500, ExitWindowSystem, this);
    //DestroyWindow();
    system_save = true;
    //-----------------------------//
#ifdef CT_355
            FreezeMode::GetInstance()->PressUnFreeze();
            MultiFuncUndo();//2016.11.05--solve problem that biopsy verify don't exit.
#endif

}

void ViewSystem::UpdateHospitalandpart(int date_format, const char *hospital_name)
{
    TopArea::GetInstance()->SetDateFormat(date_format);
    g_patientInfo.UpdateTopArea();
    TopArea::GetInstance()->UpdateHospitalName(hospital_name);

    ExamItem exam;
    char str_path[256];
    sprintf(str_path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(str_path);
    bool userFlag = exam.ReadUserItemFlag(&ini);
    string userItemName = exam.ReadDefaultProbeDefaultItemName(&ini);
    if(userFlag)
    {
        TopArea::GetInstance()->UpdateCheckPart(_(userItemName.c_str()));
    }
    else
    {
        ExamItem::EItem index = ProbeSelect::GetItemIndex();
#ifdef VET
        TopArea::GetInstance()->UpdateCheckPart(_(ExamItemArray[index].name.c_str()));
#else
        TopArea::GetInstance()->UpdateCheckPart(_(examType[index]));
#endif
    }
}

#ifdef VET
void ViewSystem::UpdateExamName(int probe_index)
{
    ExamItem::ParaItem paraItem;
    ExamItem examItem;
   /* for(int i=0;i<10;i++)
    {
        examItem.ReadExamItemPara(probe_index,i, &paraItem,NULL);
        ExamItemArray[i].name=paraItem.alias;
    }*/
    ExamItem::EItem index = ProbeSelect::GetItemIndex();
    TopArea::GetInstance()->UpdateCheckPart(_(ExamItemArray[index].name.c_str()));

}
#endif

void ViewSystem::BtnGeneralDefaultClicked(GtkButton *button)
{
    SysGeneralSetting sysGeneralSetting;
    sysGeneralSetting.DefaultFactory();
    int date_format = sysGeneralSetting.GetDateFormat();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_date_format), date_format);
    int index_lang = sysGeneralSetting.GetLanguage();
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_language), index_lang);
    int screen_saver = sysGeneralSetting.GetScreenProtect();
    unsigned char index_screen_saver = 0;
    switch (screen_saver) {
        case 0: index_screen_saver = 0; break;
        case 5: index_screen_saver = 1; break;
        case 10: index_screen_saver = 2; break;
        case 20: index_screen_saver = 3; break;
        case 30: index_screen_saver = 4; break;
        case 45: index_screen_saver = 5; break;
        case 60: index_screen_saver = 6; break;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_screen_saver), index_screen_saver);
#if 0
    char printer_path[3];
    int index_printer = sysGeneralSetting.GetPrinter();
    sprintf(printer_path, "%d", index_printer);
    GtkTreePath *path = gtk_tree_path_new_from_string(printer_path);
    gtk_tree_selection_select_path(m_selected_common_printer, path);
#endif
}

void ViewSystem::BtnOptionsDefaultClicked(GtkButton *button)
{
    SysOptions *sysOpt = new SysOptions;
    sysOpt->DefaultFactory();
    init_option_setting(sysOpt);
}

void ViewSystem::BtnMeasureDefaultClicked(GtkButton *button)
{
    SysMeasurementSetting* sysMea = new SysMeasurementSetting;
    sysMea->DefaultFactory();
    init_measure_setting(sysMea);
}

void ViewSystem::BtnCalculateDefaultClicked(GtkButton *button)
{
    SysCalculateSetting* sysCalc = new SysCalculateSetting;
    sysCalc->DefaultFactory();
    init_calc_setting(sysCalc);
}

void ViewSystem::BtnCommentDefaultClicked(GtkButton *button)
{
    SysNoteSetting* sysNote = new SysNoteSetting;
    sysNote->DefaultFactory();
    init_comment_setting(sysNote);
}

// void ViewSystem::ComboNetMethodChanged(GtkComboBox *widget)
// {
//     int active = gtk_combo_box_get_active (widget);
//     if (active) {
//  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_ip), TRUE);
//  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_mask), TRUE);
//  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_gateway), TRUE);
// //   gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_dns), TRUE);
//     } else {
//  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_ip), FALSE);
//  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_mask), FALSE);
//  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_gateway), FALSE);
// //   gtk_widget_set_sensitive(GTK_WIDGET(m_entry_network_dns), FALSE);
//     }
// }

#if 0
gboolean DicomTest(gpointer data)
{
    ViewSystem *tmp;
    tmp = (ViewSystem *)data;
    tmp->DoDicomTest();
    return FALSE;
}

void ViewSystem::DoDicomTest()
{
    char info[256];
    const char *local_ip, *local_mask, *local_gateway, *local_ae, *host_ip, *host_port, *host_ae;

    local_ip = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_network_ip));
    local_mask = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_network_mask));
    local_gateway = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_network_gateway));
    local_ae = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_equipment_ae));
    host_ip = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_host_dicom));
    host_port = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_host_port));
    host_ae = (const char *)gtk_entry_get_text(GTK_ENTRY(m_entry_host_ae));

    if ((!host_ip[0])||(!host_ae[0])||(!host_port)||(!local_ae[0]))
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()), ViewDialog::ERROR, _("Please set the relevant information of network,host and equipment!"), NULL);
        return ;
    }
    CNetworkMan localIP;
    if(localIP.SetLocalIP(local_ip, local_gateway, local_mask))
    {
        CDCMMan::GetMe()->SetRemoteAE(host_ae);
        CDCMMan::GetMe()->SetRemoteIP(host_ip);
        CDCMMan::GetMe()->SetRemotePort(atoi(host_port));
        CDCMMan::GetMe()->SetLocalAE(local_ae);
        //      CDCMMan::GetMe()->SetLocalPort(atoi(local_port));

        if(CDCMMan::GetMe()->TestLink())
        {
            PRINTF("Dicom test link OK!!\n");
            sprintf(info, _("Connection test successfully!"));
        }
        else
        {
            PRINTF("Dicom test link FAIL!!\n");
            sprintf(info, _("Connection test fails!"));
        }
    }
    else
    {
        PRINTF("Fail to set the local network!\n");
        sprintf(info, _("Fail to set the local network!"));
    }

    ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
            ViewDialog::INFO,
            info,
            NULL);
}

void ViewSystem::BtnDicomTest(GtkButton *button)
{
    ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
            _("Configuring Network and Testing..."));

    g_timeout_add(500, DicomTest, this);
}

void ViewSystem::BtnDicomReset(GtkButton *button)
{
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_ip), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_mask), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_gateway), "");
    //    gtk_entry_set_text(GTK_ENTRY(m_entry_network_dns), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_dicom), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_port), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_ae), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_equipment_ae), "");
}
#endif

#if 0
void ViewSystem::EntryIPInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if ((*new_text > '9' || *new_text < '0') && (*new_text != '.'))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}
#endif

// void ViewSystem::EntryIPChanged(GtkEditable *editable)
// {
//     int position = gtk_editable_get_position(editable);
//     PRINTF("%d\n", position);
// }

#ifdef VET
void ViewSystem:: BtnEditCellChanged(GtkCellRendererText *cell, gchar *path_string, gchar *new_text)
{
    ExamItem::ParaItem paraItem;
    ExamItem examItem;
    int probeIndex = -1;
    int itemIndex = -1;
    char *itemName =NULL;
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreePath *path;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview_exam_type));
    path = gtk_tree_path_new_from_string(path_string);
    gtk_tree_model_get_iter(GTK_TREE_MODEL(model), &iter, path);
//get old_text
    char old_text[256];
    gtk_tree_model_get(model,&iter,0,&old_text,-1);

    GetImageNoteSelection(probeIndex, itemIndex, itemName);
    if (probeIndex == -1 || itemIndex == -1) return;
    if (strlen(new_text) == 0)
    {
        char default_text[20]={0};
        snprintf(default_text, 20, "User defined %d", itemIndex+1);
        char *tmp_text = g_strdup(default_text);
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, tmp_text, -1);
        g_free(tmp_text);
        //gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, g_strdup(default_text), -1);
    }
    else
    {
        //printf("%s\n", paraItem.alias);
        char *tmp_text = g_strdup(new_text);
        gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, tmp_text, -1);
        g_free(tmp_text);
        //gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0, g_strdup(new_text), -1);
    }

    gtk_tree_path_free(path);
}

void ViewSystem::BtnImageEditClicked(GtkButton *button)
{
    gtk_widget_hide(button_default_image);
    gtk_widget_hide(button_get_current);
    gtk_widget_hide(button_edit_dept);
    gtk_widget_show(button_return_dept);

    g_object_set(m_cellrenderer_text, "editable", TRUE, NULL);
//  g_signal_connect(m_cellrenderer_text, "editing_started", G_CALLBACK(on_entry_user_item_insert), this);

    g_signal_connect(m_cellrenderer_text, "edited", G_CALLBACK(on_button_edit_cell_callback), this);
}

void ViewSystem::BtnImageReturnClicked(GtkButton *button)
{
    gtk_widget_show(button_default_image);
    gtk_widget_show(button_get_current);
    gtk_widget_show(button_edit_dept);
    gtk_widget_hide(button_return_dept);

    g_object_set(m_cellrenderer_text, "editable", FALSE, NULL);
}

#endif

void ViewSystem::EntryDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text))
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}

void ViewSystem::EntryFracDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}
#if 0
bool IsUTF8(const void* pBuffer, long size)
{
    bool IsUTF8 = true;
    unsigned char* start = (unsigned char*)pBuffer;
    unsigned char* end = (unsigned char*)pBuffer + size;
    while(start < end)
    {
        if (*start < 0x80)
        {
            start++;
        }
        else if (*start < (0xC0))
        {
            IsUTF8 = false;
            break;
        }
        else if (*start < (0xE0))
        {
            if (start >= end - 1)
                break;
            if ((start[1] & (0xC0)) != 0x80)
            {
                IsUTF8 = false;
                break;
            }
            start += 2;
        }
        else if (*start < (0xF0))
        {
            if (start >= end - 2)
                break;
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
            {
                IsUTF8 = false;
                break;
            }
            start += 3;
        }
        else
        {
            IsUTF8 = false;
            break;
        }
    }
    return IsUTF8;
}
#endif

void ViewSystem::EntryHospitalInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

    if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable))) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}

void ViewSystem::BtnUpgradeClicked(GtkButton *button)
{
    UpgradeMan::GetInstance()->Upgrade();
}

void ViewSystem::BtnImageDefaultClicked(GtkButton *button)
{
    m_save_or_new_flag = 2;
    gtk_label_set_text(GTK_LABEL(m_label_check_part), (_("Clicking OK will override the previous data, whether\n to continue the operation.")));
    gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_check_part), PANGO_WRAP_WORD);
    gtk_widget_show(m_frame_new_check_part);
    gtk_widget_hide(m_entry_new_check_part);

}

void ViewSystem::image_default_setting()
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    const char *name = exam.ReadDefaultUserSelect(&ini).c_str();
    bool flag = exam.ReadUserItemFlag(&ini);

    if (((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))||(flag))
    {
       ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("Fail to default factory! Define user or New item is under using!"), NULL);

    }
    else
    {
        //delete define item
        char deletepath[256];
        sprintf(deletepath, "%s%s", CFG_RES_PATH, EXAM_ITEM_FILE);

        // remove all the custom image parameter config file
        remove(USERNAME_DB);
        FileMan f;
        f.DelDirectory(deletepath);
        f.DelDirectory(USERCONFIG_PATH);

        char itemparafile[256];
        sprintf(itemparafile, "%s%s", CFG_RES_PATH, EXAM_FILE);
        remove(itemparafile);
        char defaultfile[256];
        sprintf(defaultfile, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
        f.CopyFile(defaultfile, itemparafile);
        //itempara file of default dir
        sprintf(itemparafile, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
        remove(itemparafile);
        sprintf(defaultfile, "%s%s", CFG_RES_PATH, DEFAULT_EXAM_FILE);
        f.CopyFile(defaultfile, itemparafile);

        UserSelect::GetInstance()->create_userdefined_dir();
        char path_defined[256];
        sprintf(path_defined, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, "SystemDefault", ".ini");
        int fd = open(path_defined, O_RDWR |O_CREAT |O_TRUNC, 0666);
        close(fd);

        UserSelect::GetInstance()->create_userconfig_dir();
        UserSelect::GetInstance()->creat_username_db(USERNAME_DB);
        ClearComboBox(GTK_COMBO_BOX(m_comboboxentry_user_select));
        UserSelect::GetInstance()->read_default_username(m_comboboxentry_user_select);
        UserSelect::GetInstance()->set_active_user(m_comboboxentry_user_select, 0);
#ifdef VET
        set_image_item_sensitive(false);
        save_itemIndex(-1);

        // rebuild the exam type treeview, in order to disppear the select box in gtk
        int index = GetProbeType();
        if (index == -1)
            return;
        ExamItem examItem;
        vector<int> itemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
        GtkTreeModel *model = create_exam_item_model(itemIndex);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
#endif
    }

    CalcSetting::GetInstance()->ChangeExamBoxDelete();
    MeasureSetting::GetInstance()->ChangeExamBoxDelete();

    int probeIndex;
    probeIndex = GetProbeType();
    ChangeCommentExamBoxDelete(probeIndex);

    // comment change
    //   gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_probe_comment), 0);

    //calc change
  //  CalcSetting::GetInstance()->ChangeExamBoxToDefault();
}

void ViewSystem::BtnImageGetCurrentClicked(GtkButton *button)
{
    if (m_imageItemSensitive)
    {
        ExamItem::ParaItem paraItem;

        // get current item para
        Img2D::GetInstance()->GetCurPara(&paraItem);
        ImgPw::GetInstance()->GetCurPara(&paraItem);
        ImgCfm::GetInstance()->GetCurPara(&paraItem);
        ImgProc2D::GetInstance()->GetCurPara(&paraItem);
        ImgProcPw::GetInstance()->GetCurPara(&paraItem);
        ImgProcCfm::GetInstance()->GetCurPara(&paraItem);

        // set current item para to dialog
        SetImagePara(paraItem);
    }
}

// 获取当前值，并保存当前检查部位参数预设
void ViewSystem::BtnImageSaveClicked(GtkButton *button)
{
#ifdef VET
    GtkTreeSelection *selected_node = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));

    GtkTreeModel *model_vet;
    GtkTreeIter iter_vet;
    if (gtk_tree_selection_get_selected(selected_node, &model_vet, &iter_vet) != TRUE) {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::ERROR,
                          _("Please select one item to be save!"), NULL); //请先选择保存的结点!
        return;
    }
#endif
    gtk_label_set_text(GTK_LABEL(m_label_check_part), (_("Clicking OK will override the previous data, whether\n to continue the operation.")));
    gtk_label_set_line_wrap_mode(GTK_LABEL(m_label_check_part), PANGO_WRAP_WORD);
    gtk_widget_show(m_frame_new_check_part);
    gtk_widget_hide(m_entry_new_check_part);
    m_save_or_new_flag = 1;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if ((gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE))
    {
        GtkTreePath *path_tmp;
        if(m_str_index == NULL)
        {
#ifdef VET
            path_tmp = gtk_tree_path_new_from_string("0");
#else
            path_tmp = gtk_tree_path_new_from_string("0:0");
#endif
            gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
        }
        else
        {
            path_tmp = gtk_tree_path_new_from_string(m_str_index);
            gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
        }
    }
}

void ViewSystem::get_current_and_save_image_para(char *itemName)
{
    ExamItem::ParaItem paraItem;

    // get current item para
    Img2D::GetInstance()->GetCurPara(&paraItem);
    ImgPw::GetInstance()->GetCurPara(&paraItem);
    ImgCfm::GetInstance()->GetCurPara(&paraItem);
    ImgProc2D::GetInstance()->GetCurPara(&paraItem);
    ImgProcPw::GetInstance()->GetCurPara(&paraItem);
    ImgProcCfm::GetInstance()->GetCurPara(&paraItem);

    save_image_para(paraItem, itemName);
}

//获得当前值，新建检查部位
void ViewSystem::BtnImageNewClicked(GtkButton *button)
{
    m_save_or_new_flag = 0;
    gtk_label_set_text(GTK_LABEL(m_label_check_part), (_("Exam Type:")));
    gtk_entry_set_text(GTK_ENTRY(m_entry_new_check_part), "");
    gtk_widget_show_all(m_frame_new_check_part);
    gtk_widget_grab_focus(m_entry_new_check_part);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview_exam_type));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if ((gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE))
    {
        GtkTreePath *path_tmp;
        if(m_str_index == NULL)
        {
#ifdef VET
            path_tmp = gtk_tree_path_new_from_string("0");
#else
            path_tmp = gtk_tree_path_new_from_string("0:0");
#endif
            gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
        }
        else
        {
            path_tmp = gtk_tree_path_new_from_string(m_str_index);
            gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
            gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_treeview_exam_type), path_tmp, NULL, TRUE);
        }
    }
}

void ViewSystem::BtnImageExportToUSBClicked(GtkButton *button)
{

   /* char path[256];
    char defaultuser[256];
    sprintf(path, "%s%s", CFG_RES_PATH, EXAM_FILE);
    sprintf(defaultuser, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR,"userconfig/ItemPara.ini");

    FileMan f;
    f.CopyFile(path,defaultuser);
*/
    ConfigToUSB::GetInstance()->CreateWindow(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()));
}

void ViewSystem::BtnImageImportFromUSBClicked(GtkButton *button)
{
    PeripheralMan *ptr = PeripheralMan::GetInstance();

    if(!ptr->CheckUsbStorageState())
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                ViewDialog::ERROR,
                _("No USB storage found!"),
                NULL);
        return;
    }
    else
    {
        if(!ptr->MountUsbStorage())
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()),
                    ViewDialog::ERROR,
                    _("Failed to mount USB storage!"),
                    NULL);
            return;
        }
    }

    ConfigToHost::GetInstance()->CreateWindow(GTK_WINDOW(ViewSystem::GetInstance()->GetWindow()));
    ptr->UmountUsbStorage();

}

// delete username and file named by username
void ViewSystem::BtnDeleteUserClicked(GtkButton *button)
{
    const char* name = GetUserName();
    char path[256];

    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))
    {
        // avoid to delete a config which is in use
        char *username = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_user_select));

        char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(path);
        ExamItem exam;
        string cur_username;
        cur_username = exam.ReadDefaultUserSelect(&ini);
        if(strcmp(username,_(cur_username.c_str()))==0)
        {
            ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("User configuration being in use!"), NULL);
            return ;
        }

        UserSelect::GetInstance()->delete_username_db(USERNAME_DB, name);

        // delete file named by username
        sprintf(path, "%s%s%s", USERCONFIG_PATH, name, ".ini");
        remove(path);

        char path_define[256];
        sprintf(path_define, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, name,".ini");
        remove(path_define);

        //delete defined comment file
        char path_comment_user[256];
        sprintf(path_comment_user, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, name, ".ini");
        remove(path_comment_user);

        // append text to combo_box
        ClearComboBox(GTK_COMBO_BOX(m_comboboxentry_user_select));
        UserSelect::GetInstance()->read_default_username(m_comboboxentry_user_select);
        UserSelect::GetInstance()->read_username_db(USERNAME_DB, m_comboboxentry_user_select);

        // set System Default is the active user after delete a user
        UserSelect::GetInstance()->set_active_user(m_comboboxentry_user_select, 0);
        memset(user_configure, 0, USERCONFIG_LEN);
#ifdef VET
        sprintf(user_configure, "%s", "VetItemPara.ini");

#else
        sprintf(user_configure, "%s", "ItemPara.ini");
#endif
        // set parameter zoom disable
        set_image_item_sensitive(false);
        save_itemIndex(-1);

        // rebuild the exam type treeview, in order to disppear the select box in gtk
        int index = GetProbeType();
        if (index == -1)
            return;
        ExamItem examItem;
        vector<int> itemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
        GtkTreeModel *model = create_exam_item_model(itemIndex);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
    }
    else
    {
        ViewDialog::GetInstance()->Create(GTK_WINDOW(m_window), ViewDialog::INFO, _("System Default is protected!"), NULL);
    }

}

void ViewSystem::ComboFocusSum(GtkComboBox *box)
{
    int i;
    char buf[20];
    int indexFocOrder;
    int index = gtk_combo_box_get_active(box);

    if ((index != -1) && (index < Img2D::MAX_FOCUS))
    {
        indexFocOrder = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_focus_pos));

        ClearComboBox(GTK_COMBO_BOX(m_combobox_focus_pos));
        for (i = 0; i < Img2D::FOC_POS_INDEX_MAX[index]; i ++)
        {
            sprintf(buf, "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_focus_pos), buf);
        }

        if (indexFocOrder >= Img2D::FOC_POS_INDEX_MAX[index])
            indexFocOrder = Img2D::FOC_POS_INDEX_MAX[index] / 2;
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_focus_pos), indexFocOrder);
    }
}

void ViewSystem::ComboPwPrf(GtkComboBox *box)
{
    char buf[50];
    int i;
    int indexWf;
    int index = gtk_combo_box_get_active(box);
#if not defined(CT_322)
    if ((index != -1) && (index < ImgPw::MAX_PRF))
    {
        indexWf = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_pw_wallfilter));

        ClearComboBox(GTK_COMBO_BOX(m_combobox_pw_wallfilter));
        // wall filter
        for (i = 0; i < ImgPw::MAX_WALL_FILTER; i ++)
        {
            //sprintf(buf , "%dHz", ImgPw::WALL_FILTER_FREQ[index][i]);
            sprintf(buf , "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_pw_wallfilter), buf);
        }

        if (indexWf >= ImgPw::MAX_WALL_FILTER)
            indexWf = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_pw_wallfilter), indexWf);
    }
#endif
}

void ViewSystem::ComboCfmPrf(GtkComboBox *box)
{
    char buf[50];
    int i;
    int index = gtk_combo_box_get_active(box);

    if ((index != -1) && (index < ImgCfm::MAX_PRF_INDEX))
    {
        int indexPkt = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_sensitivity));
#if 0
        if ((indexPkt != -1) && (indexPkt < ImgCfm::MAX_PRF_INDEX))
        {
            // wall filter
            ClearComboBox(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));
            for (i = 0; i < ImgCfm::MAX_WALLFILTER_INDEX; i ++)
            {
                sprintf(buf , "%dHz", ImgCfm::GetWFDisplay(index, indexPkt, ImgCfm::WALL_FILTER_SELECT_IDNEX[indexPkt][i]));
                gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_wallfilter), buf);
            }

            int indexWF = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));
            if (indexWF >= ImgCfm::MAX_WALLFILTER_INDEX)
                indexWF = 0;
            gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter), indexWF);
        }
#endif

#if 1
        // sensitive
        ClearComboBox(GTK_COMBO_BOX(m_combobox_sensitivity));
        for (i = 0; i < ImgCfm::MAX_PKTSIZE_INDEX; i ++)
        {
            sprintf(buf , "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_sensitivity), buf);
        }

        if (indexPkt >= ImgCfm::MAX_PKTSIZE_INDEX)
            indexPkt = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_sensitivity), indexPkt);
#endif
    }
}

void ViewSystem::ComboSensitivity(GtkComboBox *box)
{
#if not defined(CT_322)
#if not defined(CT_313)
    char buf[50];
    int i;
    int indexPkt = gtk_combo_box_get_active(box);

    if ((indexPkt != -1) && (indexPkt < ImgCfm::MAX_PKTSIZE_INDEX))
    {
        //int indexPrf = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_scale_prf));
        int indexWF = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));

        // wall filter
        ClearComboBox(GTK_COMBO_BOX(m_combobox_cfm_wallfilter));
        for (i = 0; i < ImgCfm::MAX_WALLFILTER_INDEX; i ++)
        {
            //sprintf(buf , "%dHz", ImgCfm::GetWFDisplay(indexPrf, indexPkt, ImgCfm::WALL_FILTER_SELECT_IDNEX[indexPkt][i]));
            sprintf(buf , "%d", i);
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_cfm_wallfilter), buf);
        }

        if (indexWF >= ImgCfm::MAX_WALLFILTER_INDEX)
            indexWF = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_cfm_wallfilter), indexWF);
    }
#endif
#endif
}

void ViewSystem::ClearComboBox(GtkComboBox *box)
{
    GtkTreeModel *model;
    model = gtk_combo_box_get_model(box);
    gtk_list_store_clear(GTK_LIST_STORE(model));
}

void ViewSystem::ChkBtnTAmaxToggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (!press) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_pi), FALSE);
    }
}

void ViewSystem::ChkBtnPIToggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_tamax), TRUE);
    }
}

void ViewSystem::ChkBtnHRToggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_autocalc_time), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_autocalc_time), FALSE);
    }
    else
        gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_autocalc_time), TRUE);
}

#if 0
 void ViewSystem::ChkBtnTotalToggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radiobtn_total), TRUE);
        //m_display_format = 0;
    }
}
#endif
#ifdef VET
#else
void ViewSystem::RadioP1Toggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press) {
        m_p1Selected = true;
    }
}
#endif
void ViewSystem::RadioP2Toggled(GtkToggleButton *togglebutton)
{
    gboolean press = gtk_toggle_button_get_active(togglebutton);
    if (press) {
        m_p1Selected = false;
    }
}

void ViewSystem::TreeFuncChanged(GtkTreeSelection *selection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *value;

    if (gtk_tree_selection_get_selected(selection, &model, &iter) != TRUE)
        return ;

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gchar *index_str = gtk_tree_path_to_string (path);

    gtk_tree_model_get(model, &iter,
            0, &value,
            -1);
    if (m_p1Selected) {
        gtk_entry_set_text(GTK_ENTRY(m_entry_function_p1), value);
        m_p1_func_index = atoi(index_str);
    }
    else {
        gtk_entry_set_text(GTK_ENTRY(m_entry_function_p2), value);
        m_p2_func_index = atoi(index_str);
    }
    gtk_tree_path_free (path);
    free(value);
}

/********************** CustomOB ********************/

extern int cer_custom[];
extern int hl_custom[];
extern int bpd_custom[];
extern int fl_custom[];
extern int crl_custom[];
extern int gs_custom[];
extern int ac_custom[];
extern int hc_custom[];
extern int tad_custom[];
extern int apad_custom[];
extern int thd_custom[];
extern int ofd_custom[];
extern int fta_custom[];

ViewCustomOB::ViewCustomOB()
{
}

ViewCustomOB* ViewCustomOB::m_ptrInstance = NULL;

ViewCustomOB::~ViewCustomOB()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

ViewCustomOB* ViewCustomOB::GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new ViewCustomOB;

    return m_ptrInstance;
}

void ViewCustomOB::CreateWindow(GtkWidget *parent)
{
    m_obWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (m_obWindow, 840, 640);
    gtk_window_set_position (GTK_WINDOW (m_obWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (m_obWindow), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (m_obWindow), FALSE);
    gtk_window_set_title (GTK_WINDOW (m_obWindow), _("OB Custom"));
    gtk_window_set_transient_for(GTK_WINDOW(m_obWindow), GTK_WINDOW(parent));
    g_signal_connect (G_OBJECT(m_obWindow), "delete-event", G_CALLBACK(on_window_delete_event), this);

    GtkWidget *fixed = create_customOB_window();
    gtk_container_add (GTK_CONTAINER (m_obWindow), fixed);

    MeaCalcFun::InitCustomTable();
    init_customOB();

    gtk_widget_show_all(m_obWindow);

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();

    return ;
}

GtkWidget* ViewCustomOB::create_customOB_window(void)
{
    int k1, k2;
    char buf[10];
    GtkWidget *fixed;
    GtkWidget *label_gw;
    GtkWidget *label_value;
    GtkWidget *button_clear;
    GtkWidget *button_save;
    GtkWidget *button_exit;
    GtkTreeIter iter;
    GtkListStore *store;

    fixed = gtk_fixed_new ();
    gtk_widget_show(fixed);

    label_gw = create_label(_("GW:31~300days"), 0, 30, g_white, NULL);
    gtk_fixed_put (GTK_FIXED (fixed), label_gw, 20, 10);
    gtk_widget_show(label_gw);

    label_value = create_label(_("Value:0.0~500.0mm (Click to enter a value)"), 0, 30, g_white, NULL);
    gtk_fixed_put (GTK_FIXED (fixed), label_value, 180, 10);
    gtk_widget_show(label_value);

    guchar StringOBType[13][10] = { {"CER"}, {"HL"}, {"BPD"}, {"FL"}, {"CRL"},
        {"GS"}, {"AC"}, {"HC"}, {"TAD"}, {"APAD"},
        {"THD"}, {"OFD"}, {"FTA"} };
    store = gtk_list_store_new(1, G_TYPE_STRING);
    for (int i=0; i<13; i++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, StringOBType[i], -1);
    }
    m_combobox_customOB = create_combobox(150, 30, "text", GTK_TREE_MODEL(store));
    gtk_fixed_put (GTK_FIXED (fixed), m_combobox_customOB, 670, 10);
    gtk_widget_show(m_combobox_customOB);
    g_signal_connect(GTK_OBJECT(m_combobox_customOB), "changed", G_CALLBACK(changed_custom_ob), this);

    GtkWidget *sw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_usize(sw, 800, 500);
    gtk_fixed_put(GTK_FIXED(fixed), sw, 20, 50);

    m_modelCustomOB = GTK_TREE_MODEL(gtk_list_store_new(NUM_COLUMNS,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING,
                G_TYPE_STRING));
    m_treeCustomOB = gtk_tree_view_new_with_model(m_modelCustomOB);
    gtk_widget_set_usize(m_treeCustomOB, 720, 665);
    gtk_tree_view_set_enable_search (GTK_TREE_VIEW(m_treeCustomOB), FALSE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(m_treeCustomOB), TRUE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeCustomOB)), GTK_SELECTION_SINGLE);

    for(int i=0; i<NUM_COLUMNS; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(i));
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("", renderer, "text", i, NULL);
        GtkWidget *label = create_label(" ", 0, 0, g_white, NULL);
        if(i==COL_GW) {
            g_object_set(G_OBJECT(column), "widget", label, "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 80, NULL);
        } else {
            g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
            g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_custom_ob), this);
            g_signal_connect(renderer, "editing_started", G_CALLBACK(cell_editing_started_custom_ob), this);
            sprintf(buf, "%d", i);
            gtk_label_set_text(GTK_LABEL(label), buf);
            g_object_set(G_OBJECT(column), "widget", label, "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 60, NULL);
        }
        gtk_widget_show(label);
        gtk_tree_view_append_column(GTK_TREE_VIEW(m_treeCustomOB), column);
    }

    m_arrayGW = g_array_new(TRUE, TRUE, sizeof(CustomData));
    for(int i=1; i<28; i++) {
        k1 = (i+2)*10+1;
        k2 = k1 + 9;
        sprintf(buf, "%d~%d", k1, k2);
        m_data.gw = g_strdup(buf);
        m_data.gw1 = NULL;
        m_data.gw2 = NULL;
        m_data.gw3 = NULL;
        m_data.gw4 = NULL;
        m_data.gw5 = NULL;
        m_data.gw6 = NULL;
        m_data.gw7 = NULL;
        m_data.gw8 = NULL;
        m_data.gw9 = NULL;
        m_data.gw10 = NULL;
        g_array_append_vals(m_arrayGW, &m_data, 1);
    }
    gtk_list_store_clear(GTK_LIST_STORE(m_modelCustomOB));

    for(unsigned int i=0; i<m_arrayGW->len; i++) {
#if 0
        PRINTF("Append Col: ");
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw1);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw2);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw3);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw4);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw5);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw6);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw7);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw8);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw9);
        PRINTF("%s  ", g_array_index(m_arrayGW, CustomData, i).gw10);
        PRINTF("\n");
#endif
        gtk_list_store_append(GTK_LIST_STORE(m_modelCustomOB), &iter);
        gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter,
                COL_GW, g_array_index(m_arrayGW, CustomData, i).gw,
                COL_GW1, g_array_index(m_arrayGW, CustomData, i).gw1,
                COL_GW2, g_array_index(m_arrayGW, CustomData, i).gw2,
                COL_GW3, g_array_index(m_arrayGW, CustomData, i).gw3,
                COL_GW4, g_array_index(m_arrayGW, CustomData, i).gw4,
                COL_GW5, g_array_index(m_arrayGW, CustomData, i).gw5,
                COL_GW6, g_array_index(m_arrayGW, CustomData, i).gw6,
                COL_GW7, g_array_index(m_arrayGW, CustomData, i).gw7,
                COL_GW8, g_array_index(m_arrayGW, CustomData, i).gw8,
                COL_GW9, g_array_index(m_arrayGW, CustomData, i).gw9,
                COL_GW10, g_array_index(m_arrayGW, CustomData, i).gw10,
                -1);
    }

    GtkWidget *image_clear = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_clear = gtk_label_new_with_mnemonic (_("Clear"));
    button_clear = create_button_icon(label_clear, image_clear);
    gtk_fixed_put (GTK_FIXED (fixed), button_clear, 440, 560);
    g_signal_connect ((gpointer) button_clear, "clicked", G_CALLBACK (on_button_ob_custom_clear_clicked), this);

    GtkWidget *image_save = gtk_image_new_from_stock ("gtk-save", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_save = gtk_label_new_with_mnemonic (_("Save"));
    button_save = create_button_icon(label_save, image_save);
    gtk_fixed_put (GTK_FIXED (fixed), button_save, 570, 560);
    g_signal_connect ((gpointer) button_save, "clicked", G_CALLBACK (on_button_ob_custom_save_clicked), this);

    GtkWidget *image_exit = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_BUTTON);
    GtkWidget *label_exit = gtk_label_new_with_mnemonic (_("Exit"));
    button_exit = create_button_icon(label_exit, image_exit);
    gtk_fixed_put (GTK_FIXED (fixed), button_exit, 700, 560);
    g_signal_connect ((gpointer) button_exit, "clicked", G_CALLBACK (on_button_ob_custom_exit_clicked), this);

    gtk_container_add(GTK_CONTAINER(sw), m_treeCustomOB);
    gtk_widget_show_all(sw);

    return fixed;
}

gboolean ViewCustomOB::WindowDeleteEvent(GtkWidget *widget, GdkEvent *event)
{
    DestroyWindow();
    return FALSE;
}

void ViewCustomOB::DestroyWindow(void)
{
    if(GTK_IS_WIDGET(m_obWindow)) {
        g_keyInterface.Pop();
        gtk_widget_destroy(m_obWindow);
        if (g_keyInterface.Size() == 1)
            SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
        m_obWindow = NULL;
    }
}

static gboolean ExitWindowOB(gpointer data)
{
    ViewCustomOB *tmp;
    tmp = (ViewCustomOB*)data;
    tmp->DestroyWindow();
    return FALSE;
}

void ViewCustomOB::KeyEvent(unsigned char keyValue)
{
    FakeXEvent::KeyEvent(keyValue);
    switch(keyValue)
    {
        case KEY_ESC:
            g_timeout_add(100, ExitWindowOB, this);
            FakeEscKey();
            break;
        default:
            break;
    }
}

void ViewCustomOB::BtnOBCustomClearClicked(GtkButton *button)
{
    gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_customOB));
    gint *ptr = NULL;

    switch (type)
    {
        case 0://CER
            ptr = cer_custom;
            break;
        case 1://HL
            ptr = hl_custom;
            break;
        case 2://BPD
            ptr = bpd_custom;
            break;
        case 3://FL
            ptr = fl_custom;
            break;
        case 4://CRL
            ptr = crl_custom;
            break;
        case 5://GS
            ptr = gs_custom;
            break;
        case 6://AC
            ptr = ac_custom;
            break;
        case 7://HC
            ptr = hc_custom;
            break;
        case 8://TAD
            ptr = tad_custom;
            break;
        case 9://APAD
            ptr = apad_custom;
            break;
        case 10://THD
            ptr = thd_custom;
            break;
        case 11://OFD
            ptr = ofd_custom;
            break;
        case 12://FTA
            ptr = fta_custom;
            break;
    }
    for(int i=0; i<272; i++)
    {
        *ptr = 0;
        ptr++;
    }
    update_list(type);
}

void ViewCustomOB::BtnOBCustomSaveClicked(GtkButton *button)
{
    save_customOB();
}

void ViewCustomOB::BtnOBCustomExitClicked(GtkButton *button)
{
    g_keyInterface.Pop();
    gtk_widget_destroy(m_obWindow);
    if (g_keyInterface.Size() == 1)
        SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
}

void ViewCustomOB::init_customOB(void)
{
    update_list(gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_customOB)));
}

void ViewCustomOB::CustomSelectChanged(GtkComboBox *combobox)
{
    gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
    update_list(type);
}

void ViewCustomOB::CustomCellEditingStarted(GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path)
{
    if(GTK_IS_ENTRY(editable))
    {
        g_signal_connect(G_OBJECT(editable), "insert_text", G_CALLBACK(on_entry_insert_custom_ob), this);
    }
}

void add_to_table(gint type, int temp1, float temp2)
{
    float val = temp2 * 10.0;
    switch(type)
    {
        case 0://CER
            cer_custom[temp1] = val;
            break;
        case 1://HL
            hl_custom[temp1] = val;
            break;
        case 2://BPD
            bpd_custom[temp1] = val;
            break;
        case 3://FL
            fl_custom[temp1] = val;
            break;
        case 4://CRL
            crl_custom[temp1] = val;
            break;
        case 5://GS
            gs_custom[temp1] = val;
            break;
        case 6://AC
            ac_custom[temp1] = val;
            break;
        case 7://HC
            hc_custom[temp1] = val;
            break;
        case 8://TAD
            tad_custom[temp1] = val;
            break;
        case 9://APAD
            apad_custom[temp1] = val;
            break;
        case 10://THD
            thd_custom[temp1] = val;
            break;
        case 11://OFD
            ofd_custom[temp1] = val;
            break;
        case 12://FTA
            fta_custom[temp1] = val;
            break;

    }
}

void ViewCustomOB::CustomCellEdited(GtkCellRenderer *cell, gchar *path_string, gchar *new_text)
{
    GtkTreePath *path = gtk_tree_path_new_from_string(path_string);

    GtkTreeIter iter;
    gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_customOB));

    gint column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
    gtk_tree_model_get_iter(m_modelCustomOB, &iter, path);

    if(!IsNum(new_text) && strcmp(new_text, ""))
    {
        PRINTF("It's not a num!\n");
        return;
    }

    float new_value = atof(new_text);
    if (new_value < 0.0 || new_value > 500.0)
    {
        PRINTF("Out of range!\n");
        return;
    }
    gint i = gtk_tree_path_get_indices(path)[0];

    switch(column)
    {
        case COL_GW1:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw1);
                g_array_index(m_arrayGW, CustomData, i).gw1 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw1, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW2:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw2);
                g_array_index(m_arrayGW, CustomData, i).gw2 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw2, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW3:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw3);
                g_array_index(m_arrayGW, CustomData, i).gw3 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw3, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW4:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw4);
                g_array_index(m_arrayGW, CustomData, i).gw4 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw4, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW5:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw5);
                g_array_index(m_arrayGW, CustomData, i).gw5 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw5, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW6:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw6);
                g_array_index(m_arrayGW, CustomData, i).gw6 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw6, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW7:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw7);
                g_array_index(m_arrayGW, CustomData, i).gw7 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw7, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW8:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw8);
                g_array_index(m_arrayGW, CustomData, i).gw8 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw8, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW9:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw9);
                g_array_index(m_arrayGW, CustomData, i).gw9 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw9, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
        case COL_GW10:
            {
                g_free(g_array_index(m_arrayGW, CustomData, i).gw10);
                g_array_index(m_arrayGW, CustomData, i).gw10 = g_strdup(new_text);

                gtk_list_store_set(GTK_LIST_STORE(m_modelCustomOB), &iter, column,
                        g_array_index(m_arrayGW, CustomData, i).gw10, -1);
                add_to_table(type, (i*10 + column)-1, new_value);
            }
            break;
    }
}

void ViewCustomOB::update_list(int type)
{
    PRINTF("type = %d\n", type);
    switch (type)
    {
        case 0://CER
            insert_list(m_modelCustomOB, cer_custom);
            break;
        case 1://HL
            insert_list(m_modelCustomOB, hl_custom);
            break;
        case 2://BPD
            insert_list(m_modelCustomOB, bpd_custom);
            break;
        case 3://FL
            insert_list(m_modelCustomOB, fl_custom);
            break;
        case 4://CRL
            insert_list(m_modelCustomOB, crl_custom);
            break;
        case 5://GS
            insert_list(m_modelCustomOB, gs_custom);
            break;
        case 6://AC
            insert_list(m_modelCustomOB, ac_custom);
            break;
        case 7://HC
            insert_list(m_modelCustomOB, hc_custom);
            break;
        case 8://TAD
            insert_list(m_modelCustomOB, tad_custom);
            break;
        case 9://APAD
            insert_list(m_modelCustomOB, apad_custom);
            break;
        case 10://THD
            insert_list(m_modelCustomOB, thd_custom);
            break;
        case 11://OFD
            insert_list(m_modelCustomOB, ofd_custom);
            break;
        case 12://FTA
            insert_list(m_modelCustomOB, fta_custom);
            break;

    }
}

void ViewCustomOB::insert_list(GtkTreeModel *model, int table[])
{
    gint col;
    gint row;
    gchar tmp[5];
    gchar buf[10][5];

    GtkTreePath *path;
    GtkTreeIter iter;

    for(row=0; row<27; row++)
    {
        sprintf(tmp, "%d", row);
        path = gtk_tree_path_new_from_string(tmp);
        gtk_tree_model_get_iter(model, &iter, path);
        for (col=1; col<11; col++)
        {
            PRINTF("%d  ", table[10*row + col - 1]);
#if 1
            sprintf(buf[col-1], "%.1f", (float)table[10*row+(col-1)]/10);
            if (strcmp(buf[col-1], "0.0") == 0)
                strcpy(buf[col-1], "");
            gtk_list_store_set(GTK_LIST_STORE(model), &iter, col, buf[col-1], -1);
#endif
        }
    }
    PRINTF("\n");
}

void ViewCustomOB::EntryFracDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}

void ViewSystem::EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position)
{
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));
    if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable))) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }

}

void ViewSystem::UserSelectFocusOut(GtkWidget *widget, GdkEventFocus *event)
{
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(widget));

    if ((strcmp(name, "System Default") != 0) && (strcmp(name, "Умолчан системы") != 0) &&
            (strcmp(name, "系统默认") != 0) && (strcmp(name, "Domyślne Systemu") != 0)  &&
            (strcmp(name, "Par défaut du sys.") != 0) && (strcmp(name, "Systemvorgabe") != 0) && (strcmp(name, "Sistema por defecto") !=0))

    {
        UserSelect::GetInstance()->write_username(m_comboboxentry_user_select, USERNAME_DB, name);

        memset(user_configure, 0, USERCONFIG_LEN);
        sprintf(user_configure, "%s%s%s", "userconfig/",name, ".ini");
        char path[256];
        sprintf(path, "%s%s%s%s", CFG_RES_PATH, EXAM_ITEM_FILE, name, ".ini");

        int fd = open(path, O_RDWR |O_CREAT |O_TRUNC, 0666);
        close(fd);
       // set_image_item_sensitive(false);
        save_itemIndex(-1);

        //注释预设 所用
        char path1[256];
        char userselectname[256];
        sprintf(path1, "%s%s%s%s", CFG_RES_PATH, COMMENT_PATH, name, ".ini");
        sprintf(userselectname, "%s%s", CFG_RES_PATH, COMMENT_COPY );
        FileMan f;
        f.CopyFile(userselectname, path1);

        // 计算预设已选所用
        char path2[256];
        char userselectname2[256];
        sprintf(path2, "%s%s%s%s", CFG_RES_PATH, CALC_PATH, name, ".ini");
        sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_CALC_FILE);
        f.CopyFile(userselectname2, path2);

        //计算预设词库所用
        char path3[256];
        char userselectname3[256];
        sprintf(path3, "%s%s%s%s", CFG_RES_PATH, CALC_ITEM_FILE, name, ".ini");
        sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_CALC_ITEM_FILE );
        f.CopyFile(userselectname3, path3);

        // 测量预设已选所用
        sprintf(path2, "%s%s%s%s", CFG_RES_PATH, MEASURE_PATH, name, ".ini");
        sprintf(userselectname2, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_FILE);
        f.CopyFile(userselectname2, path2);

        //测量预设词库所用
        sprintf(path3, "%s%s%s%s", CFG_RES_PATH, MEASURE_ITEM_FILE, name, ".ini");
        sprintf(userselectname3, "%s%s", CFG_RES_PATH, DEFAULT_MEASURE_ITEM_FILE );
        f.CopyFile(userselectname3, path3);

        // rebuild the exam type treeview, in order to disppear the select box in gtk
        int index = GetProbeType();
        if (index == -1)
            return;
        ExamItem examItem;
        vector<int> itemIndex;
        examItem.GetItemListOfProbe((char*)PROBE_LIST[index].c_str(), ((vector<ExamItem::EItem>*) &itemIndex));
        GtkTreeModel *model = create_exam_item_model(itemIndex);
        gtk_tree_view_set_model(GTK_TREE_VIEW(m_treeview_exam_type), model);
        gtk_tree_view_expand_all(GTK_TREE_VIEW(m_treeview_exam_type));
    }
}

GtkWidget* ViewSystem::GetVGACombobox()
{
    return m_combobox_vga;
}