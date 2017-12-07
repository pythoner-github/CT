#ifndef NETWORK_MAN_H
#define NETWORK_MAN_H
#include <string>
using std::string;

class CNetworkMan
{
public:
    CNetworkMan(){};
    virtual ~CNetworkMan(){};

    virtual string GetLocalIP();
    virtual string GetLocalGateway();
    virtual string GetLocalSubnetMask();
    virtual bool SetLocalIP(string strIP, string strGateway, string strSubnetMask);

private:
    typedef struct tagNetworkElem
    {
        string locIP;
        string locGateway;
        string locSubnetMask;
    } NetworkElem, *PNetworkElem;

    NetworkElem m_localNet;
};

#endif  //NETWORK_MAN_H