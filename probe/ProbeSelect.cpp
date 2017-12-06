#include "probe/ProbeSelect.h"
#include "imageProc/ModeStatus.h"
#include "imageControl/Knob2D.h"
#include "imageControl/KnobM.h"
#include "imageControl/KnobPw.h"
#include "imageControl/KnobCfm.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ImgProcM.h"
#include "imageProc/ImgProcPw.h"
#include "imageProc/ImgProcCfm.h"
#include "imageControl/Img2D.h"
#include "imageControl/ImgPw.h"
#include "imageControl/ImgCfm.h"
#include "imageProc/GlobalClassMan.h"
#include "imageControl/Update2D.h"
#include "probe/BiopsyLine.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/LightDef.h"
#include "keyboard/KeyValueOpr.h"
#include "imageProc/ScanMode.h"
#include "patient/PatientInfo.h"
#include "display/TopArea.h"
#include "probe/ViewProbe.h"
#include "calcPeople/MenuCalcNew.h"
#include "measure/MenuMeasure.h"
#ifdef EMP_PROJECT
#include "projectMode/ProjectCalc2D.h"
#endif
/*
 * @brief select probe and exam item
 */
#ifdef VET
ExamItem::EItem ProbeSelect::m_itemIndex = ExamItem::EItem(0);
#else
ExamItem::EItem ProbeSelect::m_itemIndex = ExamItem::ABDO_ADULT;
#endif
int ProbeSelect::m_socketIndex = 0;

extern bool g_init;
bool ProbeSelect::Execute()
{
    PRINTF("probe select\n");
    FreezeMode::GetInstance()->Freeze();

    Update2D::SetCineRemoveImg(-1);

    // get probe and item info
    if (!ProbeRead()) // no probe is found
    {
        return FALSE;
    }
#ifdef VET

    ProbeInit(1, ExamItem::EItem(0));
#else
    // display probe dialog and wait user's operation
    ProbeInit(1, ExamItem::GYN);
#endif
    return TRUE;
}
bool ProbeSelect::ProbeRead()
{
    int select = ProbeMan::MAX_SOCKET;

    // power off HV
    m_ptrProbe->ActiveHV(FALSE);

    // read probe
    m_ptrProbe->GetAllProbe(m_para);

    // get exam item list
    int i;
    for (i = 0; i < ProbeMan::MAX_SOCKET; i ++)
    {
        if (m_para[i].exist)
        {
            PRINTF("================get Probe Model correct: %s\n", m_para[i].model);
            m_e.GetItemListOfProbe(m_para[i].model, &m_itemList[i]);
            select = i;
        }
        else
        {
            m_itemList[i].clear();
           // probeItemProbe[i].clear();
        }
    }

    if (select == ProbeMan::MAX_SOCKET)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void ProbeSelect::UserItemOfProbeInit(int indexSocket, ExamItem::EItem indexItem, const char *item)
{
    // exit auto optimize if in it
    KeyAutoOptimize::AutoOff();

    // exit local zoom if in it
    KeyLocalZoom klz;
    klz.ExitLocalZoom();

    // get real probe and item parameter
    m_ptrProbe->SetProbeSocket(indexSocket);
    ProbeSocket::ProbePara curPara;
    m_ptrProbe->GetCurProbe(curPara);
    if(g_init)
    {
        char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(path);
        ExamItem exam;
        string userItemName = exam.ReadDefaultProbeDefaultItemName(&ini);
        //keeping calc and bodymark are right
        string geninitfirstitem="Adult Abdomen";
        exam.GetInitUserItemInfo(curPara.model, userItemName, geninitfirstitem);
        int probeIndex = indexSocket;
        if(probeIndex < ProbeMan::MAX_SOCKET)
        {
            for(int m = 0; m < (int)m_itemList[probeIndex].size(); m++)
            {
                PRINTF("str_firesrt_item=%s,  examlist=%s\n",geninitfirstitem.c_str(),examType[m_itemList[probeIndex][m]]);
                if(strcmp(geninitfirstitem.c_str(), examType[m_itemList[probeIndex][m]]) == 0)
                    m_itemIndex =(ExamItem::EItem)m_itemList[probeIndex][m];
            }
        }
        else
        {
            m_itemIndex = indexItem;
        }
    }
    else
    {
        //keeping calc and bodymark are right
        ExamItem exam;
        string genfirstitem="Adult Abdomen";
        exam.GetUserItemInfo(curPara.model, genfirstitem);
        //string str_fisrt_item = exam.GetFirstGenItem();
        int probeIndex = ViewProbe::GetInstance()->GetProbeIndex();
        if(probeIndex < ProbeMan::MAX_SOCKET)
        {
            for(int m = 0; m < (int)m_itemList[probeIndex].size(); m++)
            {
                PRINTF("str_firesrt_item=%s,  examlist=%s\n",genfirstitem.c_str(),examType[m_itemList[probeIndex][m]]);
                if(strcmp(genfirstitem.c_str(), examType[m_itemList[probeIndex][m]]) == 0)
                {
                    m_itemIndex =(ExamItem::EItem)m_itemList[probeIndex][m];
                    break;
                }
            }
        }
        else
        {
            m_itemIndex = indexItem;
        }
    }
    m_socketIndex = indexSocket;

    // update
    AbsUpdate2D* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();
    if (indexSocket == ProbeMan::MAX_SOCKET)
        m_ptrUpdate->ProbeType("");
    else
        m_ptrUpdate->ProbeType(curPara.model);

    ExamItem::ParaItem paraItem;
    m_ptrUpdate->UserDefineExamItem(item);
    m_e.SetUserItemOfProbe(curPara.model, (ExamItem::EItem)m_itemIndex, item);
    m_e.GetCurrentItemPara(paraItem);

    g_menuCalc.ChangeExamItem(item);
    g_menuMeasure.ChangeExamItem(item);

#ifdef EMP_PROJECT
        // init project optimization para
    ProjectCalc2D::GetInstance()->SetProjectCalc2D(GlobalClassMan::GetInstance()->GetCalc2D(curPara.model));
#endif

    // init 2D
    Img2D* ptrImg2D = Img2D::GetInstance();
    ptrImg2D->SetCalc2D( GlobalClassMan::GetInstance()->GetCalc2D(curPara.model) );
    ptrImg2D->InitProbe2D(&curPara, &paraItem);
    ImgProc2D::GetInstance()->Init(&(paraItem.d2));

    // init M
    ptrImg2D->InitProbeM(&curPara, &paraItem);
    ImgProcM::GetInstance()->Init(&(paraItem.d2));
#if not defined(EMP_322) && not defined(EMP_313)
    // init pw
    ImgPw* ptrImgPw = ImgPw::GetInstance();
    ptrImgPw->SetCalcPw( GlobalClassMan::GetInstance()->GetCalcPw(curPara.model) );
    ptrImgPw->InitProbe(&curPara, &paraItem);
    ImgProcPw::GetInstance()->Init(&(paraItem.spectrum));

    // init cfm
    ImgCfm* ptrImgCfm = ImgCfm::GetInstance();
    ptrImgCfm->SetCalcCfm( GlobalClassMan::GetInstance()->GetCalcCfm(curPara.model) );
    ptrImgCfm->InitProbe(&curPara, &paraItem);
    ImgProcCfm::GetInstance()->Init(&(paraItem.color));
#endif
    // recreate biopsy line
    BiopsyLine::GetInstance()->Create();

    // enter 2D scan mode
#if (defined(EMP_322) || defined (EMP_313))
    ScanMode::GetInstance()->DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE,LIGHT_D2);
#endif
    ScanMode::GetInstance()->Enter2D();

    // power on HV
    //if (indexSocket < ProbeMan::MAX_SOCKET) // probe is found
    m_ptrProbe->ActiveHV(TRUE);

    //enable emit
    Img2D::GetInstance()->EnableEmit();

    usleep(1000);

    Update2D::SetCineRemoveImg(3);
    // unfreeze
    FreezeMode* ptrFreeze = FreezeMode::GetInstance();
    ptrFreeze->UnFreeze();
}

//test for A60
bool ProbeSelect::OneProbeRead(int socket)
{
    int select = ProbeMan::MAX_SOCKET;

    // power off HV
    m_ptrProbe->ActiveHV(FALSE);

    // read probe
    m_ptrProbe->GetOneProbe(m_para, socket);

    // get exam item list
    int i = socket;
    if (m_para[i].exist)
    {
        PRINTF("================get Probe Model: %s\n", m_para[i].model);
        m_e.GetItemListOfProbe(m_para[i].model, &m_itemList[i]);
        select = i;
    }
    else
    {
        m_itemList[i].clear();
    }

    if (select == ProbeMan::MAX_SOCKET)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void ProbeSelect::ProbeInit(int indexSocket, ExamItem::EItem indexItem)
{
    // exit auto optimize if in it
    KeyAutoOptimize::AutoOff();

    // exit local zoom if in it
    KeyLocalZoom klz;
    klz.ExitLocalZoom();

    // begin init
    m_itemIndex = indexItem;
    m_socketIndex = indexSocket;

    // get real probe and item parameter
    m_ptrProbe->SetProbeSocket(indexSocket);
#ifdef EMP_430
    ActiveHV(TRUE);
    usleep(50000);
#endif
    ProbeSocket::ProbePara curPara;
    m_ptrProbe->GetCurProbe(curPara);

    // update
    AbsUpdate2D* m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdate2D();
    if (indexSocket == ProbeMan::MAX_SOCKET)
        m_ptrUpdate->ProbeType("");
    else
        m_ptrUpdate->ProbeType(curPara.model);
  //  m_ptrUpdate->ExamItem(indexItem);
    ExamItem::ParaItem paraItem;
    m_ptrUpdate->ExamItem(indexItem);

//  ExamItem::ParaItem paraItem;
    m_e.SetItemOfProbe(curPara.model, (ExamItem::EItem)indexItem);
    m_e.GetCurrentItemPara(paraItem);

    g_menuCalc.ChangeExamItem(m_e.ITEM_LIB[indexItem]);
    g_menuMeasure.ChangeExamItem(m_e.ITEM_LIB[indexItem]);

#ifdef EMP_PROJECT
        // init project optimization para
    ProjectCalc2D::GetInstance()->SetProjectCalc2D( GlobalClassMan::GetInstance()->GetCalc2D(curPara.model) );
#endif

    // init 2D
    Img2D* ptrImg2D = Img2D::GetInstance();
    ptrImg2D->SetCalc2D( GlobalClassMan::GetInstance()->GetCalc2D(curPara.model) );
    ptrImg2D->InitProbe2D(&curPara, &paraItem);
    ImgProc2D::GetInstance()->Init(&(paraItem.d2));

    // init M
    ptrImg2D->InitProbeM(&curPara, &paraItem);
    ImgProcM::GetInstance()->Init(&(paraItem.d2));
#ifndef EMP_322
#ifndef EMP_313
    // init pw
    ImgPw* ptrImgPw = ImgPw::GetInstance();
    ptrImgPw->SetCalcPw( GlobalClassMan::GetInstance()->GetCalcPw(curPara.model) );
    ptrImgPw->InitProbe(&curPara, &paraItem);
    ImgProcPw::GetInstance()->Init(&(paraItem.spectrum));

    // init cfm
    ImgCfm* ptrImgCfm = ImgCfm::GetInstance();
    ptrImgCfm->SetCalcCfm( GlobalClassMan::GetInstance()->GetCalcCfm(curPara.model) );
    ptrImgCfm->InitProbe(&curPara, &paraItem);
    ImgProcCfm::GetInstance()->Init(&(paraItem.color));
#endif
#endif
    // recreate biopsy line
    BiopsyLine::GetInstance()->Create();

    // enter 2D scan mode
    ScanMode::GetInstance()->DarkAllModeLight();
    g_keyInterface.CtrlLight(TRUE,LIGHT_D2);

    ScanMode::GetInstance()->Enter2D();

    //test
    m_ptrProbe->ActiveHV(TRUE);

    //enable emit
    Img2D::GetInstance()->EnableEmit();

    usleep(1000);
    // power on HV
    //if (indexSocket < ProbeMan::MAX_SOCKET) // probe is found

    Update2D::SetCineRemoveImg(3);

    // unfreeze
    FreezeMode* ptrFreeze = FreezeMode::GetInstance();
    ptrFreeze->UnFreeze();
}

void ProbeSelect::GetDefaultValue(int &socket, ExamItem::EItem &item, bool &flag)
{
    int i;
    for (i = 0; i < ProbeMan::MAX_SOCKET; i ++)
    {
        if (m_para[i].exist == TRUE)
            break;
    }
    int defaultSocket = m_ptrProbe->GetDefaultProbeSocket();

    if (defaultSocket != ProbeMan::MAX_SOCKET)
    {
        char path[256];
        sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
        IniFile ini(path);
        item = (ExamItem::EItem)(m_e.ReadDefaultProbeDefaultItem(&ini));
        socket = defaultSocket;
    }
    else
    {
        socket = i;
        if (i == ProbeMan::MAX_SOCKET)
        {
            // get default probe para
            ProbeSocket::ProbePara paraDefault;
            m_ptrProbe->GetDefaultProbe(paraDefault);
            // get default item
            item = m_e.GetDefaultItem(paraDefault.model);
        }
        else
        {
            item = m_e.GetDefaultItem(m_para[i].model);
        }
        flag = false;
    }
}

void ProbeSelect::GetDefaultItemNameandFlag(string &itemName, bool &flag)
{
    char path[256];
    sprintf(path, "%s%s", CFG_RES_PATH, STORE_DEFAULT_ITEM_PATH);
    IniFile ini(path);
    ExamItem exam;
    itemName = exam.ReadDefaultProbeDefaultItemName(&ini);
    flag = exam.ReadUserItemFlag(&ini);
}

void ProbeSelect::GetPara(ProbeSocket::ProbePara* &para, vector<ExamItem::EItem>* &item,int &maxSocket)
{
    maxSocket = ProbeMan::MAX_SOCKET;
    para  = m_para;
    item = m_itemList;
}

void ProbeSelect::ActiveHV(bool on)
{
    // control HV
    m_ptrProbe->ActiveHV(on);
}