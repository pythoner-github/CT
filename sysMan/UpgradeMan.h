#ifndef __UPGRADEMAN_H__
#define __UPGRADEMAN_H__

#include <string>
#include <vector>

#include "IniFile.h"

using std::string;
using std::vector;

class UpgradeMan
{
	public:
		~UpgradeMan();
		static UpgradeMan *GetInstance();

		enum {QUESTION, PROGRESS, SUCCESS, E_NO_USB, E_REBOOT, E_MOUNT, E_COPY, E_NO_SRC, E_NO_DEST, E_OTHER};
		static const string m_info[10];

		int UpgradeFile(vector<string> src, vector<string> dest);
		void Upgrade();
		int SureToUpgrade();
		void GetMachineType(std::string& machinetype);
		void GetSoftVersion(std::string& softversion);
		void GetFpgaVersion(std::string& fpgaversion);
		void GetRebootState(gint *state);
		void GetOrderState(gint *state);
		void SetRebootState(const char *val);
		void SetOrderState(const char *val);

	private:
		UpgradeMan();
		static UpgradeMan *m_ptrInstance;
		void NewIniFile();
		IniFile *ptrIni;
		static const string SECTION;
};

#endif
