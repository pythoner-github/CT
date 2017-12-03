#ifndef __PERIPHERAL_MAN_H__
#define __PERIPHERAL_MAN_H__

#include <string>

#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <libhal.h>

using std::string;

class PeripheralMan
{
	public:
		~PeripheralMan();
		static PeripheralMan* GetInstance();

		enum {PLUGIN, PLUGOUT};

		struct DiscInfo
		{
			bool state;
			bool is_mounted;
			bool is_blank;
			bool is_appendable;
			string disc_type;
			string udi;
			string device;
			guint64 capacity;	//total size (byte)
			guint64 data_size;	// used size (byte)
			gint64 last_track_addr;	//only for burn cd (block)
			gint64 next_writable_addr;	//only for burn cd (block)
		};

		struct PrinterInfo
		{
			bool state;
			string udi;
			string device;
		};

		struct StorageInfo
		{
			bool state;
			bool is_mounted;
			string udi;
			string device;
			string fstype;
			guint64 capacity;	//total size (byte)
			guint64 data_size;	// used size (byte)
		};

		bool AddWatchNetCable();

		void AddWatch();
		void RemoveWatch();

		bool CheckDiscCondition(); 
		bool CheckDiscState(); 
		bool CheckDiscInfo();
		bool CheckUSBInfo();
		bool GetDiscInfo(DiscInfo *info);
		bool GetUSBInfo(StorageInfo *info);
		void GetDiscSessionInfo(gint64 *last, gint64 *next);

		bool GetPrinterDev(char* device); 

		bool CheckUsbStorageState();
		bool MountUsbStorage();
		bool UmountUsbStorage();
		void SwitchPrinterDriver();
		void LoadUsblp();
		void UnLoadUsblp();

		//private callback 
		void DevAdded(LibHalContext *ctx, const char *udi);
		void DevRemoved(LibHalContext *ctx, const char *udi);
		bool GetCdMediaInfoCallback(GIOChannel *source, GIOCondition condition);
		bool GetDvdMediaInfoCallback(GIOChannel *source, GIOCondition condition);
	private:
		PeripheralMan();
		static PeripheralMan* m_ptrInstance;

		void VolumeDetails(const char *udi);
		void PrinterDetails(const char *udi);
		void DiscDetails(const char *udi);
		void ListDevices();

		bool get_property_string(char **val, LibHalContext *ctx, const char *udi, const char *key, DBusError *error);
		bool get_property_bool(bool *val, LibHalContext *ctx, const char *udi, const char *key, DBusError *error);
		bool get_property_uint64(guint64 *val, LibHalContext *ctx, const char *udi, const char *key, DBusError *error);

		bool CheckCdMediaInfo();
		bool CheckDvdMediaInfo();

		DBusError m_error;
		DBusConnection *m_dbus;
		LibHalContext *m_ctx;

		gint m_cont;
		DiscInfo m_disc;
		PrinterInfo m_printer;
		StorageInfo m_storage;

#if 0
		bool m_state;
		bool m_mounted;
		string m_udi;
		string m_device;
		string m_fstype;
#endif
};

#endif
