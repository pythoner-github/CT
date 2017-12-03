#ifndef __MAN_REGISTER_H__
#define __MAN_REGISTER_H__

#include <string>
#include <vector>
using namespace std;

class CManRegister
{
	public:
        static const int MAX_OPTIONAL_NUM = 6;
        static const string Optional[MAX_OPTIONAL_NUM];
        static const char* ValueOpt[MAX_OPTIONAL_NUM];
        static CManRegister* GetInstance();
		static void Destroy();
		static void Create(string resFileDir);

        bool GenerateLicenseFile(string destFileDir, string fileName);
		bool CheckAuthorize(string registerKey, int optionIndex);
        bool IsAuthorize(string optional);
        void SetAuthorize(string optional, bool status);

	protected:
		CManRegister(string resFileDir);
		~CManRegister();

		static CManRegister* m_ptrInstance;

	private:
        void GetFiles(const char *path, vector<string>& files);
        string GenerateRegisterKey(int optionalIndex=0);
        bool GetSCSIDiskID(unsigned char *serialNumber);
		int GetMacID(unsigned char *mac); 
        void GetValue(unsigned char *value, int optionalIndex);
        int OpenSCSIDevice(const char *dev);
		int SCSIInquiryUnitSerialNumber(int fd, unsigned char *serialNumber);
		int SCSIIO(int fd, unsigned char *cdb, unsigned char cdb_size, int xfer_dir,
            unsigned char *data, unsigned int *data_size,
            unsigned char *sense, unsigned int *sense_len);
		bool SaveRegisterKeyFile(string registerKey);
		bool ReadRegisterKeyFile(string &registerKey, string fileName);
		void GenerateLicenseCode(char *code);
		unsigned int GetRand();
		unsigned int GetSecond();

		string m_resFilePath;
		bool m_isAuthorize;
		int m_seed;
        bool m_optAuthorize[MAX_OPTIONAL_NUM];
        int m_currentIndex;
        string m_currentFileName;
};

#endif //__REGISTER_H__
