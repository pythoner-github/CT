#include "base/IniFile.h"
#include "Def.h"

using std::string;

IniFile::IniFile(const char* file)
{
    GError *error = NULL;
    m_fileName = file;

    if (!g_file_test(file, G_FILE_TEST_EXISTS))
    {
        g_build_filename(file, NULL);
    }
    gchar *contents;
    gsize length;
    if (!g_file_get_contents(file, &contents, &length, &error)) {
        PRINTF("%s\n", error->message);
        g_error_free(error);
        return;
    }
    m_keyFile = g_key_file_new();
    if (!g_key_file_load_from_data(m_keyFile, contents, -1, G_KEY_FILE_NONE, &error)) {
        PRINTF("%s\n", error->message);
        g_error_free(error);
        g_key_file_free(m_keyFile);
        return;
    }
    g_free(contents);
}

IniFile::~IniFile()
{
    g_key_file_free(m_keyFile);
}

string IniFile::ReadString(const char *group, const char *key)
{
    GError *error = NULL;
    string value("");

    gchar *ret_val = g_key_file_get_string(m_keyFile, group, key, &error);
    if (ret_val != NULL)
        value = ret_val;
    if (error) {
    PRINTF("%s\n", error->message);
    g_error_free(error);
    }
    return value;
}

int IniFile::ReadInt(const char *group, const char *key)
{
    GError *error = NULL;
    int value = g_key_file_get_integer(m_keyFile, group, key, &error);
    if (error) {
    PRINTF("%s\n", error->message);
    g_error_free(error);
    }
    return value;
}

bool IniFile::ReadBool(const char *group, const char *key)
{
    GError *error = NULL;
    bool value = g_key_file_get_boolean(m_keyFile, group, key, &error);
    if (error) {
    PRINTF("%s\n", error->message);
    g_error_free(error);
    }
    return value;
}

double IniFile::ReadDouble(const char *group, const char *key)
{
    GError *error = NULL;
    double value = g_key_file_get_double(m_keyFile, group, key, &error);
    if (error) {
    PRINTF("%s\n", error->message);
    g_error_free(error);
    }
    return value;
}

vector<double> IniFile::ReadDoubleList(const char *group, const char *key)
{
    vector<double> value;
    if(!value.empty())
        value.clear();
    GError *error = NULL;
    gsize length;
    if(group == NULL || key == NULL) return value;

    double *tmp = g_key_file_get_double_list(m_keyFile, group, key, &length, &error);
    for(int i = 0; i < length; i++)
        value.push_back(tmp[i]);
    if (error) {
        PRINTF("%s\n", error->message);
        g_error_free(error);
    }
    return value;
}

vector<int> IniFile::ReadIntList(const char *group, const char *key)
{
    vector<int> value;
    if(!value.empty())
        value.clear();
    GError *error = NULL;
    gsize length;
    if(group == NULL || key == NULL) return value;

    int *tmp = g_key_file_get_integer_list(m_keyFile, group, key, &length, &error);
    for(int i = 0; i < length; i++)
        value.push_back(tmp[i]);
    if (error) {
        PRINTF("%s\n", error->message);
        g_error_free(error);
    }
    return value;
}

void IniFile::WriteString(const char *group, const char *key, const char *value)
{
    g_key_file_set_string(m_keyFile, group, key, value);
}

void IniFile::WriteBool(const char *group, const char *key, const bool value)
{
    g_key_file_set_boolean(m_keyFile, group, key, value);
}

void IniFile::WriteInt(const char *group, const char *key, const int value)
{
    g_key_file_set_integer(m_keyFile, group, key, value);
}

void IniFile::WriteDouble(const char *group, const char *key, const double value)
{
    g_key_file_set_double(m_keyFile, group, key, value);
}
#include <iostream>
using namespace std;

void IniFile::WriteDoubleList(const char *group, const char *key, double value[], int length)
{
    g_key_file_set_double_list(m_keyFile, group, key, value, length);
}

void IniFile::WriteIntList(const char *group, const char *key, int value[], int length)
{
    g_key_file_set_integer_list(m_keyFile, group, key, value, length);
}

void IniFile::RemoveString(const char *group, const char *key)
{
    GError *error = NULL;
    if (!g_key_file_remove_key(m_keyFile, group, key, &error))
    {
        PRINTF("---%s\n", error->message);
    }
}

void IniFile::RemoveGroup(const gchar *group)
{
    GError *error = NULL;
    if (!g_key_file_remove_group(m_keyFile, group, &error))
    {
        PRINTF("---%s\n", error->message);
    }
}

vector <string> IniFile::GetGroupName(void)
{
   // char tmp_l[256];
    gsize length;
    char **tmp = g_key_file_get_groups(m_keyFile, &length);
    vector<string> group_name;
    for(gsize i = 0; i < length; i++)
    {
        group_name.push_back(tmp[i]);
    }
    g_strfreev(tmp);
    return group_name;
}

vector <string> IniFile::GetGroupKey(const gchar *group)
{
    gsize length;
    GError *error = NULL;
    char  **tmp = g_key_file_get_keys(m_keyFile, group, &length,&error);
    vector<string> group_key;
    for(gsize i=0; i<length; i++)
    {
        group_key.push_back(tmp[i]);
    }
    g_strfreev(tmp);
    return group_key;
}

bool IniFile::SyncConfigFile(void)
{
    gsize length;
    GError *error = NULL;
    gchar *contents = g_key_file_to_data(m_keyFile, &length, NULL);
    if (!g_file_set_contents (m_fileName.c_str(), contents, length, &error)) {
    PRINTF("%s\n", error->message);
    g_error_free(error);
    g_free(contents);
    return false;
    }
    g_free(contents);
    return true;
}