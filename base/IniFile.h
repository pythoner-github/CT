#ifndef INI_FILE_H
#define INI_FILE_H

#include <glib.h>
#include <string>
#include <vector>
using namespace std;
class IniFile
{
public:
    IniFile(const char* file);
    ~IniFile();

    std::string ReadString(const char *group, const char *key);
    int ReadInt(const char *group, const char *key);
    bool ReadBool(const char *group, const char *key);
    double ReadDouble(const char *group, const char *key);
    vector<double> ReadDoubleList(const char *group, const char *key);
    vector<int> ReadIntList(const char *group, const char *key);

    void WriteString(const char *group, const char *key, const char *value);
    void WriteBool(const char *group, const char *key, const bool value);
    void WriteInt(const char *group, const char *key, const int value);
    void WriteDouble(const char *group, const char *key, const double value);
    void WriteDoubleList(const char *group, const char *key, double value[], int length);
    void WriteIntList(const char *group, const char *key, int value[], int length);

    void RemoveString(const char *group, const char *key);
    void RemoveGroup(const gchar *group);
  vector <string> GetItemGroup(void);

    vector <string> GetGroupName(void);
    vector <string>  GetGroupKey(const char *group);

    bool SyncConfigFile(void);
private:
    GKeyFile *m_keyFile;
    std::string m_fileName;
};

#endif