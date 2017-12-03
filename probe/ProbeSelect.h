#ifndef PROBE_SELECT_H
#define PROBE_SELECT_H

#include <vector>
#include "ExamItem.h"
#include "ProbeSocket.h"
#include "ProbeMan.h"

using std::vector;

class ProbeSelect
{
public:
    static ExamItem::EItem GetItemIndex(void) { return m_itemIndex; }

    ProbeSelect() { m_ptrProbe = ProbeMan::GetInstance(); }

    bool Execute(); //only for test
    bool ProbeRead();
    void UserItemOfProbeInit(int indexSocket, ExamItem::EItem indexItem, const char *item);
    bool OneProbeRead(int socket);
    void ProbeInit(int indexSocket, ExamItem::EItem indexItem);
    void GetDefaultValue(int &socket, ExamItem::EItem &item, bool &flag);
    void GetDefaultItemNameandFlag(string &itemName, bool &flag);
    void GetPara(ProbeSocket::ProbePara* &para, vector<ExamItem::EItem>* &item,int &maxSocket);
    void ActiveHV(bool on);

private:
    static ExamItem::EItem m_itemIndex;
	static int m_socketIndex;

    ProbeMan* m_ptrProbe;
    ExamItem m_e;
    ProbeSocket::ProbePara m_para[ProbeMan::MAX_SOCKET];
    vector<ExamItem::EItem> m_itemList[ProbeMan::MAX_SOCKET];
    vector<ExamItem::EItem> probeItemName[ProbeMan::MAX_SOCKET];
};
#endif

