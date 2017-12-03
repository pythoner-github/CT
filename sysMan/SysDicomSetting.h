#ifndef __SYSDICOMSETTING_H__
#define __SYSDICOMSETTING_H__

#include "IniFile.h"

class SysDicomSetting
{
public:
	
    SysDicomSetting();
    ~SysDicomSetting();

    void GetRemoteAE(std::string& remoteAE);
    void GetRemoteIP(std::string& remoteIP);
    void GetRemotePort(std::string& remotePort);
    void GetLocalAE(std::string& localAE);
    void GetLocalIP(std::string& loclaIP);
    void GetLocalGateWay(std::string& localGateway);
    void GetLocalNetMask(std::string& localNetmask);
    void GetLocalPort(std::string& localPort);

    void SetRemoteAE(const char *remoteAE);
    void SetRemoteIP(const char *remoteIP);
    void SetRemotePort(const char *remotePort);
    void SetLocalAE(const char *localAE);
    void SetLocalIP(const char *localIP);
    void SetLocalGateWay(const char *localGateway);
    void SetLocalNetMask(const char *localNetmask);
    void SetLocalPort(const char *localPort);

    void SetSendReport(bool report);
    void SetSendVideo(bool Video);
    void SetVideoFrames(int frames);
    bool GetAutoQuery(void);
    void SetAutoQuery(bool autoQuery);
    bool GetSendReport(void);
    bool GetSendVideo(void);
    int GetVideoFrames(void);

    bool GetMPPS(void);
    void SetMPPS(bool mpps);
    bool GetStorageCommitment(void);
    void SetStorageCommitment(bool storageCommit);

    void SyncFile(void);
private:
    IniFile *ptrIni;
};

#endif


