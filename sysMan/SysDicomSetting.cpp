#include <string>

#include "Def.h"
#include "sysMan/SysDicomSetting.h"
#include "base/IniFile.h"

using std::string;

SysDicomSetting::SysDicomSetting()
{
    char path[256];
    sprintf(path, "%s/%s", CFG_RES_PATH, SYS_SETTING_FILE);
    ptrIni = new IniFile(path);
}

SysDicomSetting::~SysDicomSetting()
{
    if (ptrIni != NULL) {
    delete ptrIni;
    ptrIni = NULL;
    }
}

void SysDicomSetting::GetRemoteAE(string& remoteAE)
{
    remoteAE = ptrIni->ReadString("DicomSetting", "RemoteAE");
}

void SysDicomSetting::GetRemoteIP(string& remoteIP)
{
    remoteIP  = ptrIni->ReadString("DicomSetting", "RemoteIP");
}

void SysDicomSetting::GetRemotePort(string& remotePort)
{
    remotePort = ptrIni->ReadString("DicomSetting", "RemotePort");
}

void SysDicomSetting::GetLocalAE(string& localAE)
{
    localAE = ptrIni->ReadString("DicomSetting", "LocalAE");
}

void SysDicomSetting::GetLocalIP(string& localIP)
{
    localIP = ptrIni->ReadString("DicomSetting", "LocalIP");
}

void SysDicomSetting::GetLocalGateWay(string& localGateway)
{
    localGateway = ptrIni->ReadString("DicomSetting", "LocalGateWay");
}

void SysDicomSetting::GetLocalNetMask(string& localNetmask)
{
    localNetmask = ptrIni->ReadString("DicomSetting", "LocalNetMask");
}

void SysDicomSetting::GetLocalPort(string& localPort)
{
    localPort = ptrIni->ReadString("DicomSetting", "LocalPort");
}

void SysDicomSetting::SetRemoteAE(const char* remoteAE)
{
    ptrIni->WriteString("DicomSetting", "RemoteAE", remoteAE);
}

void SysDicomSetting::SetRemoteIP(const char* remoteIP)
{
    ptrIni->WriteString("DicomSetting", "RemoteIP", remoteIP);
}

void SysDicomSetting::SetRemotePort(const char* remotePort)
{
    ptrIni->WriteString("DicomSetting", "RemotePort", remotePort);
}

void SysDicomSetting::SetLocalAE(const char* localAE)
{
    ptrIni->WriteString("DicomSetting", "LocalAE", localAE);
}

void SysDicomSetting::SetLocalIP(const char* localIP)
{
    ptrIni->WriteString("DicomSetting", "LocalIP", localIP);
}

void SysDicomSetting::SetLocalGateWay(const char* localGateway)
{
    ptrIni->WriteString("DicomSetting", "LocalGateWay", localGateway);
}

void SysDicomSetting::SetLocalNetMask(const char* localNetmask)
{
    ptrIni->WriteString("DicomSetting", "LocalNetMask", localNetmask);
}

void SysDicomSetting::SetLocalPort(const char* localPort)
{
    ptrIni->WriteString("DicomSetting", "LocalPort", localPort);
}

bool SysDicomSetting::GetAutoQuery(void)
{
    return ptrIni->ReadBool("DicomSetting", "AutoQuery");

}
void SysDicomSetting::SetSendReport(bool report)
{
    ptrIni->WriteBool("DicomSetting", "SendReport", report);
}

void SysDicomSetting::SetVideoFrames(int frames)
{
    ptrIni->WriteInt("DicomSetting", "VideoFrames", frames);
}

void SysDicomSetting::SetSendVideo(bool video)
{
    ptrIni->WriteBool("DicomSetting", "SendVideo", video);
}

void SysDicomSetting::SetAutoQuery(bool autoQuery)
{
    ptrIni->WriteBool("DicomSetting", "AutoQuery", autoQuery);
}

bool SysDicomSetting::GetMPPS(void)
{
    return ptrIni->ReadBool("DicomSetting", "MPPS");
}

void SysDicomSetting::SetMPPS(bool mpps)
{
    ptrIni->WriteBool("DicomSetting", "MPPS", mpps);
}

bool SysDicomSetting::GetStorageCommitment(void)
{
    return ptrIni->ReadBool("DicomSetting", "StorageCommitment");
}

void SysDicomSetting::SetStorageCommitment(bool storageCommit)
{
    ptrIni->WriteBool("DicomSetting", "StorageCommitment", storageCommit);
}

bool SysDicomSetting::GetSendReport(void)
{
    return ptrIni->ReadBool("DicomSetting", "SendReport");
}

bool SysDicomSetting::GetSendVideo(void)
{
    return ptrIni->ReadBool("DicomSetting", "SendVideo");
}

int SysDicomSetting::GetVideoFrames(void)
{
    return ptrIni->ReadInt("DicomSetting", "VideoFrames");
}

void SysDicomSetting::SyncFile(void)
{
    ptrIni->SyncConfigFile();
}