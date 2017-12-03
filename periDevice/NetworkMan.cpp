/**
* @brief NetworkMan.cpp: interface for the CNetworkMan class.
* CNetworkMan is used to set attributes of local network.
*
* @version: 1.0
* @author: hehao
* @date: 2009-9-28
*/

#include "NetworkMan.h"
#include "Def.h"
#include <stdlib.h>
#include <assert.h>
#include "ViewSystem.h"

bool CNetworkMan::SetLocalIP(string strIP, string strGateway, string strSubnetMask)
{
#if 0
	assert(strIP != "");
	assert(strGateway != "");
	assert(strSubnetMask != "");
#endif
	//why not use g_hostname_is_ip_address to check the IP,Gateway...
	if( (strIP == "") || (strGateway == "") || (strSubnetMask == "") )
	{
		PRINTF("Error: Please check the IP,Gateway,SubnetMask!\n");
		return false;
	}

	string strCmdIP = "ifconfig eth0 " + strIP + " netmask " + strSubnetMask;
	string strCmdRT = "route add default gw " + strGateway + " dev eth0";
//	string strCmdRL = "/etc/rc.d/network restart";
	if(_system_(strCmdIP.c_str()) != -1 && _system_(strCmdRT.c_str()) != -1)
	{
		m_localNet.locIP = strIP;
		m_localNet.locGateway = strGateway;
		m_localNet.locSubnetMask = strSubnetMask;
		return true;
	}
	else
		return false;
	return true;
}

string CNetworkMan::GetLocalIP()
{
	return m_localNet.locIP;
}

string CNetworkMan::GetLocalGateway()
{
	return m_localNet.locGateway;
}

string CNetworkMan::GetLocalSubnetMask()
{
	return m_localNet.locSubnetMask;
}