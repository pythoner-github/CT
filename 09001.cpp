#include <iostream>
#include <signal.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "Def.h"
#include "SysLog.h"
#include "Init.h"
#include "IoCtrl.h"
#include "FpgaReceive.h"
#include "ExamItem.h"
#include "ProbeMan.h"
#include "PeripheralMan.h"
#include "UpgradeMan.h"
#include "gui_func.h"
#include "UIS4DReceiveInterface.h"
#include "FpgaGeneral.h"
#include "ViewSystem.h"
#include "ManRegister.h"

using namespace std;

pthread_t pidDsc;

extern int g_shm_id0, g_shm_id1;
extern int g_sem_id0, g_sem_id1;
extern unsigned char *g_pshm_data0, *g_pshm_data1;

extern CUIS4DReceiveInterface g_uis4d_ri;

#ifdef EMP_355
bool g_printProcessFps = false;
#endif

void* ThreadDsc(void* argv)
{
	cout << "Hello! I am thread dsc\n";

	FpgaReceive fr;
	fr.UsbServiceRoutine(argv);
	
	return NULL;
}


/*
 * When recevied SIGSEGV signal, auto run gdb
 */
void dump(int signo)
{
	char buf[1024];
	char cmd[1024];
	FILE *fh;

	snprintf(buf, sizeof(buf), "/proc/%d/cmdline", getpid());
	if(!(fh = fopen(buf, "r")))
		exit(0);
	if(!fgets(buf, sizeof(buf), fh))
		exit(0);
	fclose(fh);
	if(buf[strlen(buf) - 1] == '\n')
		buf[strlen(buf) - 1] = '\0';
    snprintf(cmd, sizeof(cmd), "gdb %s %d", buf, getpid());
    _system_(cmd);

	exit(0);
}
#include "Printmain.h"
void IngPipe(int signo)
{
    int id = getpid();
    signal(SIGPIPE, SIG_IGN);
}

int main(int argc, char*argv[])
{
#ifdef EMP_355
    if (argc > 1)
    {
        if (strcmp(argv[1], "PrintProcessFps") == 0)
        {
            g_printProcessFps = true;
            printf("==================printf:%s\n", argv[1]);
        }
    }
#endif
    //signal(SIGSEGV, &dump);
    signal(SIGSEGV, SIG_IGN);
    // in order to due the GetPrinter cause broken pipe, process be killed 
    //signal(SIGPIPE, SIG_IGN);   
    signal(SIGPIPE, &IngPipe);   

    Init init;
	init.WriteLogHead();
	
	///> system init
	SysLog* sysLog = SysLog::Instance();
	(*sysLog) << "main: Sytem init!" << endl;
	init.SystemInit(argc, argv);

	if(!g_thread_supported())
		g_thread_init(NULL);
	gdk_threads_init();
	gtk_init(&argc, &argv);

	SetTheme(RC_PATH);

	ChangeKeymap();

	ASSERT(init_colors());
#ifdef EMP_3410
    CManRegister::GetInstance()->Create(OPTIONAL_DIR);
#endif

	///> init gui, and create main view
	(*sysLog) << "main: Run gui, create main view!" << endl;
	init.CreatMainWindow();
	
	init.ParaInit();

	///> probe init
	(*sysLog) << "main: Probe init!" << endl;
	init.ProbeCheck();

	///> start up DSC thread
#if 1
	(*sysLog) << "main: Start up DSC thread!" << endl;
//	pthread_t pidDsc;
	if( pthread_create(&pidDsc, NULL, ThreadDsc, NULL) != 0)
	{
		///> phread create error
		cerr << "main: Fail to create DSC thread!" << endl;
	}
#endif

    ///>IPC Control
    g_uis4d_ri.IPC4DInit();
    g_shm_id0 = g_uis4d_ri.GetSHMID(0);
    g_pshm_data0 = g_uis4d_ri.GetSHMData(0);
    g_sem_id0 = g_uis4d_ri.GetSEMID(0);
    /****************** LWF ************************/

	gdk_threads_enter();
	gtk_main();	
	gdk_threads_leave();
	
	///> exit main, and do something clear
	(*sysLog) << "main: Exit main, and do something clear!" << endl;

    g_uis4d_ri.IPC4DDestroy();

	init.ExitSystem();
}

