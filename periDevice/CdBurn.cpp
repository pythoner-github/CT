#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include "Def.h"
#include "gui_global.h"
#include "CdBurn.h"
#include "ViewDialog.h"
#include "ViewCD.h"
#include "PeripheralMan.h"

#define ISOPATH "/tmp/09001.iso"

#define INFO_READY "Ready to burn, please wait..."
#define INFO_MKISO "Making ISO image, please wait..."
#define INFO_BURN "Burning, please wait..."
#define INFO_FINISH "Finished!"
#define ERR_MKISO "Failed to make ISO image!"
#define ERR_BURN "Failed to burn!"
#define ERR_GETDISCINFO "Failed to get media information!\nPlease check the media correct."
#define ERR_DISCTYPE "The media type is not supported!"

#define MKISOFS_ESTIMATE "done, estimate finish"
#define MKISOFS_ERROR "mkisofs:"
#define MKISOFS_IGNORE "-follow-links does not always"
#define MKISOFS_OK "Total translation table size"
#define MKISOFS_NOSPACE "No space left on device"
#define MKISOFS_SUPPORT_ISOV2 "Set ISO9660 conformance level (1..3) or 4 for ISO9660 version 2"
#define MKISOFS_CHARSET "Known charsets are:"

CdBurn* CdBurn::m_ptrInstance = NULL;

CdBurn::CdBurn()
{
	m_cont = 1;
	m_pidMkIso = -1;
	m_pidBurn = -1;
}

CdBurn::~CdBurn()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance;
}

CdBurn* CdBurn::GetInstance()
{
	if(m_ptrInstance == NULL)
		m_ptrInstance = new CdBurn;

	return m_ptrInstance;
}

static gboolean TimeoutHintDialog(gpointer data)
{
    ViewHintDialog::GetInstance()->Create(GTK_WINDOW(ViewCD::GetInstance()->GetWindow()), _((char*)data));
    
    return FALSE;
}

static gboolean TimeoutInfoDialog(gpointer data)
{
	ViewDialog::GetInstance()->Create(GTK_WINDOW(ViewCD::GetInstance()->GetWindow()),
			ViewDialog::INFO,
			_((char*)data),
			NULL);

	return FALSE;
}

static gboolean TimeoutUpdateDialog(gpointer data)
{
	ViewDialog *Dlg = ViewDialog::GetInstance();
	if(Dlg->Exist())
		Dlg->SetText(_((char*)data));
	else
		TimeoutInfoDialog(data);

	return FALSE;
}

void CdBurn::HintDialog(guint timeout, int type, const char *info)
{
	// if(type==ViewDialog::HINT)
	// 	g_timeout_add(timeout, TimeoutHintDialog, (void*)info);
    if(type==ViewDialog::INFO)
        g_timeout_add(timeout, TimeoutInfoDialog, (void*)info);
    else if(type < 0)
        g_timeout_add(timeout, TimeoutUpdateDialog, (void*)info);
    else
        g_timeout_add(timeout, TimeoutInfoDialog, (void*)info);
}

static gboolean HandleMakeIsoCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	return CdBurn::GetInstance()->MakeIsoCallback(source, condition);
}

static gboolean HandleBurnCDCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	return CdBurn::GetInstance()->BurnCDCallback(source, condition);
}

static gboolean HandleBurnDVDCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	return CdBurn::GetInstance()->BurnDVDCallback(source, condition);
}

gboolean CdBurn::StartBurn(const char* listPath)
{
	m_cont = 1;
	m_listPath.assign(listPath);

	g_timeout_add(10, TimeoutHintDialog, (void*)_(INFO_READY));

	PeripheralMan::DiscInfo info;
	if(!PeripheralMan::GetInstance()->GetDiscInfo(&info))
	{
		PRINTF("**************Error: GetDiscInfo***************\n");
		HintDialog(500, (int)ViewDialog::INFO, _(ERR_GETDISCINFO));
		return FALSE;
	}

	if(info.disc_type.find("cd") == 0)
	{
		HintDialog(100, -1, _(INFO_MKISO));
		if(!MakeIso())
		{
			PRINTF("***********Error: MakeIso***********\n");
			HintDialog(500, (int)ViewDialog::INFO, _(ERR_MKISO));
			return FALSE;
		}
		HintDialog(100, -1, _(INFO_BURN));
		if(!BurnCD(info.device.c_str()))
		{
			PRINTF("***********Error: BurnCD************\n");
			HintDialog(500, (int)ViewDialog::INFO, _(ERR_BURN));
			return FALSE;
		}
	}
	else if(info.disc_type.find("dvd") == 0)
	{
		HintDialog(100, -1, _(INFO_BURN));
		if(!BurnDVD(info.is_blank, info.device.c_str()))
		{
			PRINTF("***********Error: BurnDVD************\n");
			HintDialog(500, (int)ViewDialog::INFO, _(ERR_BURN));
			return FALSE;
		}
	}
	else
	{
		PRINTF("Error: The disc type is not supported!\nPlease insert a cd_rom, cd_r, cd_rw, or dvd_rom, dvd_ram, dvd_r, dvd_rw, dvd_plus_r, dvd_plus_rw type disc\n");
		HintDialog(500, (int)ViewDialog::INFO, _(ERR_DISCTYPE));
		return FALSE;
	}

#if 0
	//Update media info
	if(PeripheralMan::GetInstance()->CheckMediaInfo())
		UpdateSize();
#endif

	HintDialog(500, (int)ViewDialog::INFO, _(INFO_FINISH));
	return TRUE;
}

gboolean CdBurn::MakeIso()
{
	gboolean status = FALSE;
	gint cmdstatus = 0;
	gint g_out, g_err, arg, pid;
	guint event, eventerr;
	gchar *cmdline, **cmd;
	GIOChannel *comerr, *com;
//	gint64 last_track_addr, next_writable_addr;

	if(!PeripheralMan::GetInstance()->CheckDiscState())
		return FALSE;

	PeripheralMan::DiscInfo info;
	if(!PeripheralMan::GetInstance()->GetDiscInfo(&info))
		return FALSE;

	if(info.is_blank)
		cmdline = g_strdup_printf("/usr/bin/mkisofs -r -J -o %s -graft-points -path-list %s", ISOPATH, m_listPath.c_str());
	else
	{
		PRINTF("last_track_addr = %lld, next_writable_addr = %lld\n", info.last_track_addr, info.next_writable_addr);
		if(info.last_track_addr < info.next_writable_addr && info.next_writable_addr)
			cmdline = g_strdup_printf("/usr/bin/mkisofs -r -J -o %s -C %lld,%lld -M %s -graft-points -path-list %s", 
					ISOPATH, info.last_track_addr, info.next_writable_addr, info.device.c_str(), m_listPath.c_str());
		else
			return FALSE;
	}
	PRINTF("cmdline: %s\n", cmdline);

	status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
	g_free(cmdline);
	if(status == FALSE)
	{
		PRINTF("MakeISO error: shell pares argv\n");
		return FALSE;
	}
//	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

	GError *error = NULL;
	status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
			(GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
			NULL, NULL, &pid, NULL, &g_out, &g_err, &error);
	m_pidMkIso = pid;
	g_strfreev(cmd);
	if(status == FALSE)
	{
		PRINTF("MakeISO error: %s\n", error->message);
		perror("g_spawn_async_with_pipes");
		return FALSE;
	}
	
	m_cont = 1;

	com = g_io_channel_unix_new(g_out);
	g_io_channel_set_encoding(com, NULL, NULL);
	g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
	event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleMakeIsoCallback, NULL);

	comerr = g_io_channel_unix_new(g_err);
	g_io_channel_set_encoding(comerr, NULL, NULL);
	g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
	eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleMakeIsoCallback, NULL);

	while(m_cont)
	{
		gtk_main_iteration();
	}

	/* exit mkisofs process	*/
	waitpid(pid, &cmdstatus, 0);
	if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
			|| (!(WIFEXITED(cmdstatus))))	//command exit abnormal
	{
		PRINTF("mkisofs command exit with error\n");
		return FALSE;
	}
	PRINTF("mkisofs finished\n");

	g_source_remove(eventerr);
	g_source_remove(event);

	g_io_channel_shutdown(comerr, FALSE, NULL);
	g_io_channel_unref(comerr);  
	g_io_channel_shutdown(com, FALSE, NULL);
	g_io_channel_unref(com);
	g_spawn_close_pid(pid);
	m_pidBurn = -1;

	return TRUE;
}

gboolean CdBurn::MakeIsoCallback(GIOChannel *source, GIOCondition condition)
{
	gchar *buffer = NULL, *f;
	GIOStatus status;

	if (condition == G_IO_HUP || condition == G_IO_ERR) 
	{
		//Check whether where is the process of mkiso 
		if(kill(m_pidMkIso, 0) < 0)
		{
			//if the process is not exist, return FALSE(remove the iochannel source)
			if(errno == ESRCH)
				return FALSE;
		}

		perror("MakeIso GIOChannel error:");
		m_cont = 0;
		return TRUE;
	}

	status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
	if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
	{
		return TRUE;
	}
	PRINTF("%s: %s", __FUNCTION__, buffer);

	if (strstr(buffer, MKISOFS_ESTIMATE) && buffer[3]=='.') //progress of mkisofs
	{
		buffer[3] = 0;
		f = buffer;

		while (*f==' ' || *f==9) f++;	
		PRINTF("mkisofs progress: %s\n", f);

#if 0
		gdouble pct = 0.01 * atof(f);
		gtk_progress_bar_set_fraction(progressbar2, pct);
		g_snprintf(Lsbuf, sizeof(Lsbuf)-1, "%s%%", f);
		gtk_progress_bar_set_text(Lprogressbar2, Lsbuf);
		/* avancement total */
		Ltava = (1.0 / (*Ltodo)) * ((*Ldone)-1+Lpct);
		gtk_progress_bar_set_fraction(Lprogressbar, Ltava);
		g_snprintf(Lsbuf2, sizeof(Lsbuf2)-1, "%.0f%%", Ltava*100);
		gtk_progress_bar_set_text(Lprogressbar, Lsbuf2);
#endif
	}
	else if(strstr(buffer, MKISOFS_ERROR) && !strstr(buffer, MKISOFS_IGNORE))	//error with mkisofs
	{
		m_cont = 0;
		if (!strstr(buffer, MKISOFS_NOSPACE)) 
		{
			PRINTF("Not enough free disk space to create iso image!\n");
			g_free(buffer);
			return FALSE;
		}
	}
	else if(!strncmp(buffer, MKISOFS_OK, strlen(MKISOFS_OK))) 
	{
		m_cont = 0;
		PRINTF("mkisofs progress: 100%%\n");
#if 0
		gtk_progress_bar_set_fraction(Lprogressbar2, 1);
		gtk_progress_bar_set_text(Lprogressbar2, "100%");
		Ltava = (1.0 / (*Ltodo)) * (*Ldone);
		gtk_progress_bar_set_fraction(Lprogressbar, Ltava);
		g_snprintf(Lsbuf2, sizeof(Lsbuf2)-1, "%.0f%%", Ltava*100);
		gtk_progress_bar_set_text(Lprogressbar, Lsbuf2);
#endif
	}

	g_free(buffer);
	return TRUE;
}

gboolean CdBurn::BurnCD(const char *device)
{
	gboolean status = FALSE;
	gchar *cmdline, **cmd;
	gint cmdstatus = 0;
	gint g_out, g_err, pid, arg;
	guint event, eventerr;
	GIOChannel *comerr, *com;

	cmdline = g_strdup_printf("/usr/bin/cdrecord -v dev=%s -multi -eject %s", device, ISOPATH);
	
	status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
	g_free(cmdline);
	if(status == FALSE)
	{
		PRINTF("BurnCD error: shell pares argv\n");
		return FALSE;
	}
//	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

	status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
			(GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
			NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
	m_pidBurn = pid;
	g_strfreev(cmd);
	if(status == FALSE)
	{
		PRINTF("BurnCD error: g_spawn_async_with_pipes\n");
		return FALSE;
	}
	
	m_cont = 1;

	com = g_io_channel_unix_new(g_out);
	g_io_channel_set_encoding(com, NULL, NULL);
	g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
	event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleBurnCDCallback, NULL);

	comerr = g_io_channel_unix_new(g_err);
	g_io_channel_set_encoding(comerr, NULL, NULL);
	g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
	eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleBurnCDCallback, NULL);

	while(m_cont)
	{
		gtk_main_iteration();
	}

	/* exit burncd process	*/
	waitpid(pid, &cmdstatus, 0);
	if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
			|| (!(WIFEXITED(cmdstatus))))	//command exit abnormal
	{
		PRINTF("BurnCD command exit with error\n");
		return FALSE;
	}
	PRINTF("BurnCD finished\n");

	g_source_remove(eventerr);
	g_source_remove(event);

	g_io_channel_shutdown(comerr, FALSE, NULL);
	g_io_channel_unref(comerr);  
	g_io_channel_shutdown(com, FALSE, NULL);
	g_io_channel_unref(com);
	g_spawn_close_pid(pid);
	m_pidBurn = -1;

	return TRUE;
}

gboolean CdBurn::BurnCDCallback(GIOChannel *source, GIOCondition condition)
{
	gchar *buffer = NULL;
	GIOStatus status;

	if (condition == G_IO_HUP || condition == G_IO_ERR) 
	{
		//Check whether where is the process of burn
		if(kill(m_pidBurn, 0) < 0)
		{
			//if the process is not exist, return FALSE(remove the iochannel source)
			if(errno == ESRCH)
				return FALSE;
		}

		perror("BurnCD GIOChannel error:");
		m_cont = 0;
		return TRUE;
	}

	status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
	if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
	{
		return TRUE;
	}
	PRINTF("%s: %s", __FUNCTION__, buffer);

	return TRUE;
}

gboolean CdBurn::BurnDVD(gboolean is_blank, const char *device)
{
	gboolean status = FALSE;
	gchar *cmdline, **cmd;
	gint cmdstatus = 0;
	gint g_out, g_err, pid, arg;
	guint event, eventerr;
	GIOChannel *comerr, *com;

	if(is_blank)
		cmdline = g_strdup_printf("/usr/bin/growisofs -Z %s -R -J -graft-points -path-list %s", device, m_listPath.c_str());
	else
		cmdline = g_strdup_printf("/usr/bin/growisofs -M %s -R -J -graft-points -path-list %s", device, m_listPath.c_str());

	status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
	g_free(cmdline);
	if(status == FALSE)
	{
		PRINTF("BurnDVD error: shell pares argv\n");
		return FALSE;
	}
//	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

	status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
			(GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
			NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
	m_pidBurn = pid;
	g_strfreev(cmd);
	if(status == FALSE)
	{
		PRINTF("BurnDVD error: g_spawn_async_with_pipes\n");
		return FALSE;
	}
	
	m_cont = 1;

	com = g_io_channel_unix_new(g_out);
	g_io_channel_set_encoding(com, NULL, NULL);
	g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
	event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleBurnDVDCallback, NULL);

	comerr = g_io_channel_unix_new(g_err);
	g_io_channel_set_encoding(comerr, NULL, NULL);
	g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
	eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleBurnDVDCallback, NULL);

	while(m_cont)
	{
		gtk_main_iteration();
	}

	/* exit mkisofs process	*/
	waitpid(pid, &cmdstatus, 0);
	if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
			|| (!(WIFEXITED(cmdstatus))))	//command exit abnormal
	{
		PRINTF("BurnDVD command exit with error\n");
		return FALSE;
	}
	PRINTF("BurnDVD finished\n");

	g_source_remove(eventerr);
	g_source_remove(event);

	g_io_channel_shutdown(comerr, FALSE, NULL);
	g_io_channel_unref(comerr);  
	g_io_channel_shutdown(com, FALSE, NULL);
	g_io_channel_unref(com);
	g_spawn_close_pid(pid);
	m_pidBurn = -1;

	return TRUE;
}

gboolean CdBurn::BurnDVDCallback(GIOChannel *source, GIOCondition condition)
{
	gchar *buffer = NULL;
	GIOStatus status;

	if (condition == G_IO_HUP || condition == G_IO_ERR) 
	{
		//Check whether where is the process of burn
		if(kill(m_pidBurn, 0) < 0)
		{
			//if the process is not exist, return FALSE(remove the iochannel source)
			if(errno == ESRCH)
				return FALSE;
		}

		perror("BurnDVD GIOChannel error:");
		m_cont = 0;
		return TRUE;
	}

	status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
	if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
	{
		return TRUE;
	}
	PRINTF("%s: %s", __FUNCTION__, buffer);

	return TRUE;
}

