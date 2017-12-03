#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <glib.h>
#include <errno.h>
#include <sys/vfs.h>
#include "Def.h"
#include "ViewIcon.h"
#include "ViewCD.h"
#include "PeripheralMan.h"
#include "Printer.h"
#include "ViewSystem.h"


PeripheralMan* PeripheralMan::m_ptrInstance = NULL;

PeripheralMan* PeripheralMan::GetInstance()
{
	if (m_ptrInstance == NULL)
		m_ptrInstance = new PeripheralMan;
	return m_ptrInstance;
}

PeripheralMan::PeripheralMan()
{
	m_dbus = NULL;
	m_ctx = NULL;

	m_cont = 1;

	m_disc.state = PLUGOUT;
	m_disc.is_mounted = FALSE;
	m_disc.is_blank = FALSE;
	m_disc.is_appendable = FALSE;
	m_disc.udi.assign("");
	m_disc.device.assign("");
	m_disc.disc_type.assign("");
	m_disc.capacity = 0;
	m_disc.data_size = 0;
	m_disc.last_track_addr = 0;
	m_disc.next_writable_addr = 0;

	m_printer.state = PLUGOUT;
	m_printer.udi.assign("");
	m_printer.device.assign("");

	m_storage.state = PLUGOUT;
	m_storage.is_mounted = FALSE;
	m_storage.udi.assign("");
	m_storage.device.assign("");
	m_storage.fstype.assign("");
        m_storage.data_size = 0;
        m_storage.capacity = 0;
}

PeripheralMan::~PeripheralMan()
{
	if (m_ptrInstance != NULL) 
		delete m_ptrInstance; 
}

bool PeripheralMan::get_property_string(char **val, LibHalContext *ctx, const char *udi, const char *key, DBusError *error)
{
	*val = libhal_device_get_property_string(ctx, udi, key, error);
	if(dbus_error_is_set(&m_error))
	{
		PRINTF("DBus Error: %s\n", m_error.message);
		LIBHAL_FREE_DBUS_ERROR(&m_error);
		return FALSE;
	}
	return TRUE;
}

bool PeripheralMan::get_property_bool(bool *val, LibHalContext *ctx, const char *udi, const char *key, DBusError *error)
{
	*val = libhal_device_get_property_bool(ctx, udi, key, error);
	if(dbus_error_is_set(&m_error))
	{
		PRINTF("DBus Error: %s\n", m_error.message);
		LIBHAL_FREE_DBUS_ERROR(&m_error);
		return FALSE;
	}
	return TRUE;
}

bool PeripheralMan::get_property_uint64(guint64 *val, LibHalContext *ctx, const char *udi, const char *key, DBusError *error)
{
	*val = libhal_device_get_property_uint64(ctx, udi, key, error);
	if(dbus_error_is_set(&m_error))
	{
		PRINTF("DBus Error: %s\n", m_error.message);
		LIBHAL_FREE_DBUS_ERROR(&m_error);
		return FALSE;
	}
	return TRUE;
}

void PeripheralMan::GetDiscSessionInfo(gint64 *last, gint64 *next)
{
	*last = m_disc.last_track_addr;
	*next = m_disc.next_writable_addr;
}

bool PeripheralMan::CheckDiscInfo()
{
	if(m_disc.state==PLUGIN && !m_disc.is_mounted)
	{
		ViewCD* ptrCD = ViewCD::GetInstance();
		if(ptrCD->GetWindow())
			ptrCD->SetBackupStatus(ViewCD::SCAN);

		if(m_disc.disc_type.find("cd") == 0)
			return CheckCdMediaInfo();
		else if(m_disc.disc_type.find("dvd") == 0)
			return CheckDvdMediaInfo();
	}
	else
		return FALSE;

	return FALSE;
}

bool PeripheralMan::CheckUSBInfo()
{

    PRINTF("m_storage.is_mounted = %d %d\n", m_storage.state, m_storage.is_mounted);
    //if(m_storage.state==PLUGIN && !m_storage.is_mounted)
    if(m_storage.state==PLUGIN && !m_storage.is_mounted)
    {
        ViewCD* ptrCD = ViewCD::GetInstance();
        if(ptrCD->GetWindow())
            ptrCD->SetBackupStatus(ViewCD::USB_SCAN);
        return TRUE;
    }
    else
        return FALSE;
    return FALSE;
}

bool PeripheralMan::GetDvdMediaInfoCallback(GIOChannel *source, GIOCondition condition)
{
    gchar *buffer = NULL, *f;
    GIOStatus status;

    if(condition == G_IO_HUP || condition == G_IO_ERR)
    {
        perror("Get DVD msinfo GIOChannel error:");
        m_cont = 0;
        return TRUE;
    }

    status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
    if(!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
    {
        return TRUE;
    }
    PRINTF("%s: %s", __FUNCTION__, buffer);

    if((f = strstr(buffer, "Legacy lead-out at:")))
    {
        for(; *f; f++)
        {
            if(*f=='=')
            {
                f++;
                break;
            }
        }
        f[strlen(f)-1] = '\0';
        m_disc.capacity = strtoull(f, NULL, 10);
        PRINTF("Dvd media capacity=%llu\n", m_disc.capacity);
    }
    else if((f = strstr(buffer, "READ CAPACITY:")))
    {
        for(; *f; f++)
        {
            if(*f=='=')
            {
                f++;
                break;
            }
        }
        f[strlen(f)-1] = '\0';
        m_disc.data_size = strtoull(f, NULL, 10);
        PRINTF("Dvd media data size=%llu\n", m_disc.data_size);
    }

    g_free(buffer);
    return TRUE;
}

gboolean HandleGetDvdMediaInfoCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    return PeripheralMan::GetInstance()->GetDvdMediaInfoCallback(source, condition);
}

bool PeripheralMan::CheckDvdMediaInfo()
{
    gboolean status = FALSE;
    gint cmdstatus = 0;
    gint g_out, g_err, arg, pid;
    guint event;
    gchar *cmdline, **cmd;
    GIOChannel *com;

    cmdline = g_strdup_printf("/usr/bin/dvd+rw-mediainfo %s", m_disc.device.c_str());
    PRINTF("cmdline: %s\n", cmdline);

    status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
    g_free(cmdline);
    if(status == FALSE)
    {
        PRINTF("%s-%d: g_shell_parse_argv error!\n", __FILE__, __LINE__);
        return FALSE;
    }
    //	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

    status = g_spawn_async_with_pipes(NULL, cmd, NULL,
            (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD),
            NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
    g_strfreev(cmd);
    if(status == FALSE)
    {
        PRINTF("%s-%d: g_spawn_async_with_pipes error!\n", __FILE__, __LINE__);
        return FALSE;
    }

    m_cont = 1;

    com = g_io_channel_unix_new(g_out);
    g_io_channel_set_encoding(com, NULL, NULL);
    g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
    event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleGetDvdMediaInfoCallback, NULL);

    while(m_cont)
    {
        gtk_main_iteration();
    }

    /* exit get msinfo process	*/
    waitpid(pid, &cmdstatus, 0);
    if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
            || (!(WIFEXITED(cmdstatus))))	//command exit abnormal
    {
        PRINTF("get dvd size command exit with error\n");
        return FALSE;
    }
    PRINTF("get dvd size finished\n");

    g_source_remove(event);
    g_io_channel_shutdown(com, FALSE, NULL);
    g_io_channel_unref(com);
    g_spawn_close_pid(pid);

    return TRUE;
}

bool PeripheralMan::GetCdMediaInfoCallback(GIOChannel *source, GIOCondition condition)
{
    gchar *buffer = NULL, *f;
    GIOStatus status;
    gboolean format = TRUE;
    gint bit = 0;
    gint comma = 0;
    gint64 last_track_addr = 0; 
    gint64 next_writable_addr = 0;

    if(condition == G_IO_HUP || condition == G_IO_ERR)
    {
        perror("Get CD msinfo GIOChannel error:");
        m_cont = 0;
        return TRUE;
    }

    status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
    if(!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
    {
        return TRUE;
    }
    PRINTF("%s: %s", __FUNCTION__, buffer);

    for(f=buffer; *f; f++)
    {
        if(isdigit(*f)){
            bit++;
        }
        else if(*f==','){
            if(comma==0){
                if(bit>0)
                    comma = bit;
            }
            else{
                format = FALSE;
                break;
            }
        }
        else if(*f=='\n'){
            break;
        }
        else{
            format = FALSE;
            break;
        }
    }
    PRINTF("comma = %d, format = %d\n", comma, format);

    if(comma && format)
    {
        char last[50], next[50];
        memcpy(last, buffer, comma);
        last[comma] = '\0';
        memcpy(next, buffer+comma+1, strlen(buffer)-comma);
        next[strlen(buffer)-comma] = '\0';
        PRINTF("last=%s, next=%s\n", last, next);

        last_track_addr = atoll(last);
        next_writable_addr = atoll(next);
        if(last_track_addr < next_writable_addr)
        {
            m_disc.last_track_addr = last_track_addr;
            m_disc.next_writable_addr = next_writable_addr;
        }
        else
        {
            m_disc.last_track_addr = 0;
            m_disc.next_writable_addr = 0;
        }
        PRINTF("m_disc: last_track_addr=%lld, next_writable_addr=%lld\n", m_disc.last_track_addr, m_disc.next_writable_addr);
        m_cont = 0;
    }
    else
    {
        m_disc.last_track_addr = 0;
        m_disc.next_writable_addr = 0;
        m_cont = 0;
    }

    g_free(buffer);
    return TRUE;
}

gboolean HandleGetCdMediaInfoCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    return PeripheralMan::GetInstance()->GetCdMediaInfoCallback(source, condition);
}

bool PeripheralMan::CheckCdMediaInfo()
{
    gboolean status = FALSE;
    gint cmdstatus = 0;
    gint g_out, g_err, arg, pid;
    guint event, eventerr;
    gchar *cmdline, **cmd;
    GIOChannel *comerr, *com;

    cmdline = g_strdup_printf("/usr/bin/cdrecord dev=%s -msinfo", m_disc.device.c_str());
    PRINTF("cmdline: %s\n", cmdline);

    status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
    g_free(cmdline);
    if(status == FALSE)
    {
        PRINTF("%s-%d: g_shell_parse_argv error!\n", __FILE__, __LINE__);
        return FALSE;
    }
    //	PRINTF("cmd: %s arg num=%d\n", cmd, arg);

    status = g_spawn_async_with_pipes(NULL, cmd, NULL,
            (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD),
            NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
    g_strfreev(cmd);
    if(status == FALSE)
    {
        PRINTF("%s-%d: g_spawn_async_with_pipes error!\n", __FILE__, __LINE__);
        return FALSE;
    }

    m_cont = 1;

    com = g_io_channel_unix_new(g_out);
    g_io_channel_set_encoding(com, NULL, NULL);
    g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
    event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleGetCdMediaInfoCallback, NULL);

    comerr = g_io_channel_unix_new(g_err);
    g_io_channel_set_encoding(comerr, NULL, NULL);
    g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
    eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
            HandleGetCdMediaInfoCallback, NULL);

    while(m_cont)
    {
        gtk_main_iteration();
    }

    /* exit get msinfo process	*/
    waitpid(pid, &cmdstatus, 0);
    if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
            || (!(WIFEXITED(cmdstatus))))	//command exit abnormal
    {
        PRINTF("get msinfo command exit with error\n");
        return FALSE;
    }
    PRINTF("get msinfo finished\n");

    g_source_remove(eventerr);
    g_source_remove(event);

    g_io_channel_shutdown(comerr, FALSE, NULL);
    g_io_channel_unref(comerr);
    g_io_channel_shutdown(com, FALSE, NULL);
    g_io_channel_unref(com);
    g_spawn_close_pid(pid);

    return TRUE;
}

void PeripheralMan::VolumeDetails(const char *udi)
{
    char *device;
    bool is_disc, is_mounted;
    string str;
    int ret;

#if 0
    //show all property for test
    libhal_device_print(m_ctx, udi, &m_error);
    if(dbus_error_is_set(&m_error))
    {
        PRINTF("DBus Error: %s\n", m_error.message);
        LIBHAL_FREE_DBUS_ERROR(&m_error);
        return;
    }
#endif

    if(!get_property_string(&device, m_ctx, udi, "block.device", &m_error))
        return;
    str.assign(device, strlen(device));

    get_property_bool(&is_disc, m_ctx, udi, "volume.is_disc", &m_error);
    get_property_bool(&is_mounted, m_ctx, udi, "volume.is_mounted", &m_error);

    if(is_disc)
    {
        PRINTF("Added Volume Disc Device: %s\n", str.c_str());

        bool is_blank, is_appendable;
        guint64 capacity, volume_size;
        char *disc_type;
        get_property_bool(&is_blank, m_ctx, udi, "volume.disc.is_blank", &m_error);
        get_property_bool(&is_appendable, m_ctx, udi, "volume.disc.is_appendable", &m_error);
        get_property_uint64(&capacity, m_ctx, udi, "volume.disc.capacity", &m_error);
        get_property_uint64(&volume_size, m_ctx, udi, "volume.size", &m_error);
        if(!get_property_string(&disc_type, m_ctx, udi, "volume.disc.type", &m_error))
            return;

        m_disc.is_mounted = is_mounted;
        m_disc.is_blank = is_blank;
        m_disc.is_appendable = is_appendable;
        m_disc.state = PLUGIN;
        m_disc.device = str;
        m_disc.udi.assign(udi, strlen(udi));
        m_disc.disc_type.assign(disc_type, strlen(disc_type));
        m_disc.capacity = capacity;
        m_disc.data_size = volume_size;
        m_disc.last_track_addr = 0;
        m_disc.next_writable_addr = 0;

        PRINTF("disc state=%d, device=%s, udi=%s, is_mounted=%d, is_blank=%d, is_appendable=%d, disc_type=%s, capacity=%llu, volume.size=%llu, last_track_addr=%lld, next_writable_addr=%lld\n", 
                m_disc.state, m_disc.device.c_str(), m_disc.udi.c_str(), m_disc.is_mounted, m_disc.is_blank, m_disc.is_appendable, m_disc.disc_type.c_str(), m_disc.capacity, m_disc.data_size, m_disc.last_track_addr, m_disc.next_writable_addr);

        ViewCD* ptrCD = ViewCD::GetInstance();
        if(ptrCD->GetWindow())
        {
            gdk_threads_enter();
            if(CheckDiscInfo())
            {
                ptrCD->UpdateSize(true);
                //	if(ptrCD->GetWindow())
                //		ptrCD->SetBackupStatus(ViewCD::SCAN);
            }
            gdk_threads_leave();
        }

        ViewIcon::GetInstance()->Cdrom(TRUE);
        free(disc_type);
    }
    else if(!is_disc)
    {
        PRINTF("Added Volume Device: %s\n", str.c_str());

        char *fstype;
        guint64 capacity=0;
        //guint64 usage=0;
        if(!get_property_string(&fstype, m_ctx, udi, "volume.fstype", &m_error))
            return;
        get_property_uint64(&capacity, m_ctx, udi, "volume.size", &m_error);

        ret = str.find("sda");
        if( ret < 0)
        {
            m_storage.is_mounted = is_mounted;
            m_storage.state = PLUGIN;
            m_storage.device = str;
            m_storage.udi.assign(udi, strlen(udi));
            m_storage.fstype.assign(fstype, strlen(fstype));
            m_storage.capacity = capacity;
            // m_storage.data_size = usage;//volume_size;
#if 0
            LibHalVolume* disk=libhal_volume_from_device_file(m_ctx, str);
            libhal_volume_free(disk);
#endif
            //printf("mounted:%d cap:%llu, \n", m_storage.is_mounted,capacity);
            ViewIcon::GetInstance()->Udisk(TRUE);
            //get_property_uint64(&usage, m_ctx, udi, "volume.usage", &m_error);
            //printf("usage:%llu, \n", usage);

            ViewCD* ptrCD = ViewCD::GetInstance();
            if(ptrCD->GetWindow())
            {
                gdk_threads_enter();
                if(CheckUSBInfo())
                {
                    MountUsbStorage();
                    ptrCD->UpdateSize(false);
                }
                gdk_threads_leave();
            }
        }
        free(fstype);
    }
    free(device);
}

void PeripheralMan::PrinterDetails(const char *udi)
{
    char *device;

    device = libhal_device_get_property_string(m_ctx, udi,
            (const char *)"printer.device", &m_error);
    if(dbus_error_is_set(&m_error))
    {
        PRINTF("DBus Error: %s\n", m_error.message);
        LIBHAL_FREE_DBUS_ERROR(&m_error);
    }

#if 0
    libhal_device_print(m_ctx, udi, &m_error);
    if(dbus_error_is_set(&m_error))
    {
        PRINTF("DBus Error: %s\n", m_error.message);
        LIBHAL_FREE_DBUS_ERROR(&m_error);
    }
#endif

    PRINTF("Added Printer Device: %s\n", device);

    m_printer.state = PLUGIN;
    m_printer.device.assign(device, strlen(device));
    m_printer.udi.assign(udi, strlen(udi));

    ViewIcon::GetInstance()->Printer(TRUE);
    //	PRINTF("printer state=%d, device=%s, udi=%s\n", m_printer.state, m_printer.device.c_str(), m_printer.udi.c_str());

    free(device);

    // load firmware
    Printer printer;
    string uri;
    if(!printer.GetUsbPrinterUri(uri))
        PRINTF("%s: Error with GetUsbPrinterUri\n", __FUNCTION__);
    printer.LoadFirmware(uri);
}

void PeripheralMan::DevAdded(LibHalContext *ctx, const char *udi)
{
    if (libhal_device_query_capability(ctx, udi, "printer", NULL)) 
    {
        PrinterDetails(udi);
    }
    else if (libhal_device_query_capability(ctx, udi, "volume", NULL)) 
    {
        VolumeDetails(udi);
    }
}

void PeripheralMan::DevRemoved(LibHalContext *ctx, const char *udi)
{
    if(!strcmp(udi, m_storage.udi.c_str()))	//USB Storage
    {
        PRINTF("Removed Usb Device: %s\n", m_storage.device.c_str());
        m_storage.state = PLUGOUT;
        m_storage.device.assign("");
        m_storage.udi.assign("");
        m_storage.fstype.assign("");
        m_storage.data_size = 0;
        m_storage.capacity = 0;
        m_storage.is_mounted = FALSE;

        ViewCD* ptrCD = ViewCD::GetInstance();
        if(ptrCD->GetWindow())
        {
            ptrCD->SetBackupStatus(ViewCD::USB_EMPTY);
            ptrCD->UpdateSize(false);
            UmountUsbStorage();
        }
        ViewIcon::GetInstance()->Udisk(FALSE);
    }
    else if(!strcmp(udi, m_printer.udi.c_str()))	//Printer
    {
        PRINTF("Removed Printer Device: %s\n", m_printer.device.c_str());
        m_printer.state = PLUGOUT;
        m_printer.device.assign("");
        m_printer.udi.assign("");
        ViewIcon::GetInstance()->Printer(FALSE);
    }
    else if(!strcmp(udi, m_disc.udi.c_str()))	//disc
    {
        PRINTF("Removed Disc Device: %s\n", m_disc.device.c_str());
        m_disc.state = PLUGOUT;
        m_disc.is_mounted = FALSE;
        m_disc.is_blank = FALSE;
        m_disc.is_appendable = FALSE;
        m_disc.disc_type.assign("");
        m_disc.device.assign("");
        m_disc.udi.assign("");
        m_disc.capacity = 0;
        m_disc.data_size = 0;
        m_disc.last_track_addr = 0;
        m_disc.next_writable_addr = 0;

        ViewCD* ptrCD = ViewCD::GetInstance();
        if(ptrCD->GetWindow())
        {
            ptrCD->SetBackupStatus(ViewCD::EMPTY);
            ptrCD->UpdateSize(true);
        }
        ViewIcon::GetInstance()->Cdrom(FALSE);
    }
}

void HandleDeviceAdded(LibHalContext *ctx, const char *udi)
{
    PeripheralMan::GetInstance()->DevAdded(ctx, udi);
}

void HandleDeviceRemoved(LibHalContext *ctx, const char *udi)
{
    PeripheralMan::GetInstance()->DevRemoved(ctx, udi);
}

void PeripheralMan::ListDevices()
{
    int i;
    int num_devices;
    char **devices;

    /* search HAL for volume devices */
    if ((devices = libhal_find_device_by_capability(m_ctx, "volume", &num_devices, &m_error)) == NULL) 
        LIBHAL_FREE_DBUS_ERROR(&m_error);
    else
    {
        /* details */
        for (i = 0;i < num_devices;i++) 
            VolumeDetails((const char *)devices[i]);
        libhal_free_string_array(devices);
    }

    /* search HAL for printer devices */
    if ((devices = libhal_find_device_by_capability(m_ctx, "printer", &num_devices, &m_error)) == NULL) 
        LIBHAL_FREE_DBUS_ERROR(&m_error);
    else
    {
        /* details */
        for (i = 0;i < num_devices;i++) 
            PrinterDetails((const char *)devices[i]);
        libhal_free_string_array(devices);
    }
}

#if 0
#include <sys/inotify.h>
static void* CheckNetCable(void* argv)
{
    int fd, wd;
    int len, index;
    char buffer[1024];
    struct inotify_event *event;
    const char *path = "/sys/class/net/eth0/carrier";
    //	const char *path = "/tmp/1";

    fd = inotify_init();
    if(fd < 0)
    {
        printf("ERROR: Failed to initialize inotify.\n");
        return 0;
    }

    wd = inotify_add_watch(fd, path, IN_CLOSE_WRITE);
    if(wd < 0)
    {
        printf("ERROR: Can't add watch for %s", path);
        return 0;
    }

    len = read(fd, buffer, 1024);
    printf("len = %d\n", len);
    while(len){
        index = 0;
        while(index < len)
        {
            event = (struct inotify_event *)(buffer+index);
            if(event->wd != wd)
                continue;
            if(event->mask & IN_CLOSE_WRITE)
            {
                printf("file %s is modified.\n", event->name);
            }
            else
            {
                printf("file %s is not modified.\n", event->name);
            }
            index += sizeof(struct inotify_event)+event->len;
            printf("index = %d\n", index);
        }
        len = read(fd, buffer, 1024);
    }

    return 1;
}
#endif

static gboolean CheckNetCable(gpointer data)
{
    char buf[10];
    int buf_len = 1;
    const char *path = "/sys/class/net/eth0/carrier";
    int fd = open(path, O_RDONLY | O_NDELAY);
    memset(buf, '\0', 10);
    int len = read(fd, buf, buf_len);
    if(len < 0)
    {
        //perror("CheckNetCable read error");
        PRINTF("CheckNetCable read error\n");
    }
    else
    {
        //PRINTF("cable carrier = %s\n", buf);
        int onoff = atoi(buf);
        ViewIcon::GetInstance()->Network(onoff);
    }
    close(fd);

    return 1;
}

bool PeripheralMan::AddWatchNetCable()
{
    g_timeout_add(2000, CheckNetCable, NULL);
#if 0
    pthread_t pidWatch;
    if(pthread_create(&pidWatch, NULL, CheckNetCable, NULL) != 0)
    {
        PRINTF("Fail to create NetCable Watch thread!");
        return FALSE;
    }
#endif
    return TRUE;
}

void PeripheralMan::AddWatch()
{	
    dbus_error_init(&m_error);

    /* set up connection to DBUS system bus */
    m_dbus = dbus_bus_get(DBUS_BUS_SYSTEM, &m_error);
    if (m_dbus == NULL) {
        PRINTF("ERROR: dbus_bus\n");
        LIBHAL_FREE_DBUS_ERROR(&m_error);
        return;
    }
    dbus_connection_setup_with_g_main (m_dbus, NULL);
    if ((m_ctx = libhal_ctx_new()) == NULL) {
        PRINTF("ERROR: libhal_ctx_new\n");
        dbus_connection_unref(m_dbus);
        return;
    }

    /* set up connection to HAL daemon */
    if (!libhal_ctx_set_dbus_connection (m_ctx, m_dbus)) {
        PRINTF("ERROR: libhal_ctx_set_dbus_connection\n");
        dbus_connection_unref(m_dbus);
        return;
    }
    if (!libhal_ctx_init(m_ctx, &m_error)) {
        PRINTF("ERROR: libhal_ctx_init\n");
        dbus_connection_unref(m_dbus);
        return;
    }

    /* set up two callbacks */ 
    libhal_ctx_set_device_added(m_ctx, HandleDeviceAdded);
    libhal_ctx_set_device_removed(m_ctx, HandleDeviceRemoved);

    if (!libhal_device_property_watch_all(m_ctx, &m_error)) {
        PRINTF("ERROR: libhal_device_property_watch_all\n");
        libhal_ctx_free(m_ctx);
        return;
    }

    /* list current volume devices */
    ListDevices();
}

void PeripheralMan::RemoveWatch()
{
    /* close connections */
    if (libhal_ctx_shutdown(m_ctx, &m_error) == FALSE)
        LIBHAL_FREE_DBUS_ERROR(&m_error);
    libhal_ctx_free(m_ctx);
    dbus_connection_unref(m_dbus);
}

bool PeripheralMan::GetDiscInfo(DiscInfo *info)
{
    if(m_disc.state == PLUGIN)
    {
        *info = m_disc;
        return TRUE;
    }
    else
    {
        PRINTF("No Disc Plug-in\n");
        return FALSE;
    }
}

bool PeripheralMan::GetUSBInfo(StorageInfo *info)
{
    if(m_storage.state == PLUGIN)
    {
        *info = m_storage;
        return TRUE;
    }
    else
    {
        PRINTF("No Disc Plug-in\n");
        return FALSE;
    }
}

bool PeripheralMan::GetPrinterDev(char* device)
{
    if(m_printer.state == PLUGIN)
    {
        //device = g_strdup(m_printer.device.c_str());
        sprintf(device, "%s", m_printer.device.c_str());
        return TRUE;
    }
    else
    {
        PRINTF("No Printer Plug-in\n");
        return FALSE;
    }
}

bool PeripheralMan::CheckUsbStorageState()
{
    if(m_storage.state == PLUGIN)
        return TRUE;
    else
    {
        PRINTF("No UsbStorage Plug-in\n");
        return FALSE;
    }
}

bool PeripheralMan::CheckDiscCondition()
{
    if(!m_cont)
        return TRUE;
    else
    {
        PRINTF("Check MediaInfo is running!\n");
        return FALSE;
    }
}

bool PeripheralMan::CheckDiscState()
{
    if(m_disc.state == PLUGIN)
        return TRUE;
    else
    {
        PRINTF("No Disc Insert\n");
        return FALSE;
    }
}

static guint64 GetPathUsedSize(const char *path)
{
    struct statfs fs;
    guint64 used_size = 0;
    if (path == NULL)
        return 0;
    if(statfs(path, &fs)==0)
    {
        used_size=(double)fs.f_bsize*(fs.f_blocks-fs.f_bavail);
    }
    //printf("used_size:%llu\n", used_size);
    return used_size;
}

bool PeripheralMan::MountUsbStorage()
{
    if(!m_storage.is_mounted)
    {
		//printf("::::::::::::::::::: fstype = %s\n", m_storage.fstype.c_str());
        //	if(mount(m_storage.device.c_str(), UDISK_PATH, m_storage.fstype.c_str(), MS_SYNCHRONOUS, NULL) < 0) //use sync flag is very slow!
        //	if(mount(m_storage.device.c_str(), UDISK_PATH, m_storage.fstype.c_str(), 0, NULL) < 0)
		char option[100];
		if(strcmp(m_storage.fstype.c_str(), "vfat")==0) //支持FAT16，FAT32
			sprintf(option, "shortname=mixed,iocharset=utf8,flush");
		else	//经测试，支持NTFS，不支持exFat，其他未测
			sprintf(option, "iocharset=utf8");
        if(mount(m_storage.device.c_str(), UDISK_PATH, m_storage.fstype.c_str(), 0, option) < 0)
        //if(mount(m_storage.device.c_str(), UDISK_PATH, m_storage.fstype.c_str(), 0, "iocharset=utf8,flush") < 0)
        {
            //printf("Mount error: %s\n", strerror(errno));
            return FALSE;
        }
        else
        {
           // printf("%s Mount Usb Device Success!\n", __FUNCTION__);
            m_storage.is_mounted = TRUE;
            m_storage.data_size = GetPathUsedSize(UDISK_PATH); ///
            return TRUE;
        }
    }
    else
    {
        m_storage.data_size = GetPathUsedSize(UDISK_PATH); ///
        return TRUE;
    }
}

bool PeripheralMan::UmountUsbStorage()
{
    if(m_storage.is_mounted)
    {
        if(umount(UDISK_PATH) < 0)
        {
            //printf("Umount error: %s\n", strerror(errno));
            return FALSE;
        }
        else
        {
            //printf("Umount Usb Device Success!\n");
            m_storage.is_mounted = FALSE;
            m_storage.data_size = 0; ///
            return TRUE;
        }
    }
    return FALSE;
}

void PeripheralMan::SwitchPrinterDriver()
{
	Printer printer;
	bool isSony = printer.IsSonyPrinter();
	if(isSony)
		LoadUsblp();
	else
		UnLoadUsblp();
}
void PeripheralMan::LoadUsblp()
{
	_system_("modprobe usblp");
	printf("%s-%s: modprobe usblp\n", __FILE__, __FUNCTION__);
}
void PeripheralMan::UnLoadUsblp()
{
	_system_("modprobe -r usblp");
	printf("%s-%s: modprobe -r usblp\n", __FILE__, __FUNCTION__);
}  
