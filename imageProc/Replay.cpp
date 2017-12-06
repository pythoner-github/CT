#include "imageProc/Replay.h"
#include "Def.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/KnobReplay.h"
#include "display/KnobNone.h"
#include "display/ImageArea.h"
#include "display/TopArea.h"
#include "display/HintArea.h"
#include "base/CalcTime.h"
#include "imageControl/KnobEFOV.h"
#include "imageControl/Update2D.h"
#include "patient/ImgMan.h"
#include "imageProc/MenuReview.h"

// calc back
gboolean ViewCallBack(gpointer data)
{
    Replay::GetInstance()->EnterViewStatus();
    Replay::GetInstance()->ViewOneImgOnly();
    return true;
}

gboolean ReviewCallBack(gpointer data)
{
    Replay::GetInstance()->EnterReviewStatus();
    Replay::GetInstance()->ReviewOneImgOnly();
    return true;
}

gboolean EFOVReviewCallBack(gpointer data)
{
    Replay::GetInstance()->EFOVAutoReview();
    return true;
}

Replay* Replay::m_ptrInstance = NULL;
#if (defined(EMP_460) || defined(EMP_355))
const float Replay::REPLAY_SPEED[MAX_SPEED] = {0, 2.0,1.0,0.5};
#else
const float Replay::REPLAY_SPEED[MAX_SPEED] = {0.5, 1.0, 2.0};
#endif
Replay* Replay::GetInstance()
{
    if(m_ptrInstance == NULL)
        m_ptrInstance = new Replay;

    return m_ptrInstance;
}

Replay::Replay()
{
    int i;

    m_ptrUpdate = GlobalClassMan::GetInstance()->GetUpdateMix();
    m_ptrUpdate2D = GlobalClassMan::GetInstance()->GetUpdate2D();
    m_ptrUpdateFrameFreeze = DscMan::GetInstance()->GetFrameUpdateFunFreeze();
    m_ptrDsc = DscMan::GetInstance();

    m_ptrDsc->GetReplayWriteLock();
    for (i = 0; i < MAX_AREA; i ++)
    {
        m_deq[i].clear();
        m_maxCount[i] = 0;
        m_imgIndex[i] = 0;
        m_maxCountBak[i] = 0;
    }
    m_ptrDsc->ReadWriteReplayUnlock();

    m_areaIndex = 0;
    m_speedIndex = 1;
    m_fps = 20;
    m_trimLeft = 0;
    m_trimRight = 0;
    m_tagTimer = 0;
    m_viewMode = TRUE;

    m_areaIndexBak = 0;
    m_areaNumBak = 0;
    m_fpsBak = m_fps;
    m_enterCine = FALSE;;
    m_enterSnap = FALSE;

    m_discard = FALSE;

    m_efovFrameNo = 0;
    m_efovAutoReviewFlag = FALSE;
    m_tagTimerEFOV = 0;
}

Replay::~Replay()
{
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

void Replay::Init()
{
    EKnobReturn retSpeed = OK;
    if (m_speedIndex == (MAX_SPEED-1))
        retSpeed = MAX;
    else if (m_speedIndex == 0)
        retSpeed = MIN;
    else
        retSpeed = OK;
#if (defined (EMP_322) || defined(EMP_313))
    m_ptrUpdate->ReplayCtrl(FALSE);
    m_ptrUpdate->SpeedCtrl(m_speedIndex);
#else
    m_ptrUpdate->ReplayCtrl(FALSE, m_speedIndex, retSpeed);
#endif
    m_ptrUpdate->ReplayTrimLeft(m_trimLeft+1);
    m_ptrUpdate->ReplayTrimRight(m_trimRight+1);
}
/*
void Replay::ChangeReplayStatus()
{
    bool on = ModeStatus::IsReplayMode();
    m_ptrUpdate->ReplayCtrl(on);

}
*/
EKnobReturn Replay::ChangeSpeed(EKnobOper oper)
{
    bool on = ModeStatus::IsAutoReplayMode();
    if (oper == ROTATE)
    {
        if (m_speedIndex < (MAX_SPEED-1))
        {
            m_speedIndex ++;
        }
        else
        {
            m_speedIndex = 0;
        }
    }
    else if (oper == ADD)
    {
        if (m_speedIndex < (MAX_SPEED-1))
        {
            m_speedIndex ++;
        }
        else
        {
            PRINTF("replay: change speed reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (m_speedIndex > 0)
        {
            m_speedIndex --;
        }
        else
        {
            PRINTF("replay: change speed reach min value!\n");
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }
#if (defined(EMP_460) || defined(EMP_355))
    if (on&&(m_speedIndex==0))
            FreezeMode::GetInstance()->ChangeAutoReplay();
    if ((!on)&&(m_speedIndex>0))
            FreezeMode::GetInstance()->ChangeAutoReplay();
#endif

    EKnobReturn ret = OK;
    if (m_speedIndex == (MAX_SPEED-1))
        ret = MAX;
    else if (m_speedIndex == 0)
        ret = MIN;
    else
        ret = OK;

    // power off timer
    if (m_tagTimer > 0)
    {
        int interval = GetReplayTime(m_speedIndex);

        if (!g_source_remove(m_tagTimer))
        {
            PRINTF("remove timer error\n");
        }
        m_tagTimer = 0;

        // restart timer
        if (m_viewMode)
            m_tagTimer = g_timeout_add(interval, ViewCallBack, NULL);
        else
            m_tagTimer = g_timeout_add(interval, ReviewCallBack, NULL);
    }

#if (defined (EMP_322) || defined(EMP_313))
    m_ptrUpdate->ReplayCtrl(on);
    m_ptrUpdate->SpeedCtrl(m_speedIndex);
#else
    m_ptrUpdate->ReplayCtrl(on, m_speedIndex, ret);
#endif
    return (ret);
}

#if (defined(EMP_460) || defined(EMP_355))
void Replay::SetTrimLeft(EKnobOper oper)
#else
void Replay::SetTrimLeft()
#endif
{
    int size = m_deq[m_areaIndex].size();
    if (size == 0)
        return;

    m_trimLeft = m_imgIndex[m_areaIndex];

    // update view
    m_ptrUpdate->ReplayTrimLeft(m_trimLeft+1);
    m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);
}
#if (defined(EMP_460) || defined(EMP_355))
void Replay::SetTrimRight(EKnobOper oper)
#else
void Replay::SetTrimRight()
#endif
{
    int size = m_deq[m_areaIndex].size();
    if (size == 0)
        return;

    if (m_imgIndex[m_areaIndex] >  m_trimLeft)
    {
        m_trimRight= m_imgIndex[m_areaIndex];

        // update view
        m_ptrUpdate->ReplayTrimRight(m_trimRight+1);
        m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);
    }
}
#if (defined(EMP_460) || defined(EMP_355))
void Replay::ResetTrim(EKnobOper oper)
#else
void Replay::ResetTrim()
#endif
{
    int size = m_deq[m_areaIndex].size();

    m_trimLeft = 0;
    m_trimRight = size - 1;

    // update view
    m_ptrUpdate->ReplayTrimLeft(m_trimLeft+1);
    m_ptrUpdate->ReplayTrimRight(m_trimRight+1);
    m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);
}
/*
 * @brief split the quantity of replay save area(space) to "num"
 *
 * @para num number of replay area, num value must be in range{1, 2, 4}
 *
 */
bool Replay::SetAreaNum(int num)
{
    if ((num != 1) && (num != 2) && (num != 4))
    {
        PRINTF("input is out of range!\n");
        return FALSE;
    }

    ///> clear all of image saved
    ClearAll();
    m_areaNum = num;
    m_areaIndex = 0;

    ///> resize replay save area
    int i;
    switch (num)
    {
        case 1:
            ///> split area
            m_maxCount[0] = TOTAL_REPLAY;
            for (i = 1; i < MAX_AREA; i ++)
            {
                m_maxCount[i] = 0;
            }

            ///> reset count
            for (i = 0; i < MAX_AREA; i ++)
            {
                m_imgIndex[i] = 0;
            }
            break;

        case 2:
            for (i = 0; i < 2; i ++)
            {
                m_maxCount[i] = TOTAL_REPLAY / 2;
            }
            for (i = 2; i < MAX_AREA; i ++)
            {
                m_maxCount[i] = 0;
            }

            ///>reset
            for (i = 0; i < MAX_AREA; i ++)
            {
                m_imgIndex[i] = 0;
            }
            break;

        case 4:
            for (i = 0; i < MAX_AREA; i ++)
            {
                m_maxCount[i] = TOTAL_REPLAY / 4;
            }
            ///> reset
            for (i = 0; i < MAX_AREA; i ++)
            {
                m_imgIndex[i] = 0;
            }
            break;

        default:
            break;
    }

    return TRUE;
}

/*
 * @brief switch current replay area to "current", clear images in current area
 *
 * @para current current area, range{0} when area number is 1, {0,1} when area number is 2; {0,1,2,3} when area number if 4
 */
bool Replay::SwitchArea(int current, bool unfreeze)
{
    switch(m_areaNum)
    {
        case 1:
            if (current > 0)
                return FALSE;
            break;
        case 2:
            if (current > 1)
                return FALSE;
            break;
        case 4:
            if (current > 3)
                return FALSE;
            break;
    }

    m_areaIndex = current;
    if (unfreeze)
    {
        m_ptrDsc->GetReplayWriteLock();
        m_deq[m_areaIndex].clear();
        m_ptrDsc->ReadWriteReplayUnlock();
    }
    return TRUE;
}

/*
 * @brief add one image to current replay area(m_deqor)
 *
 * @para img image to be added
 */
void Replay::AddOneImg(Image img)
{
    // discard the first image after unfreeze
    if (m_discard)
    {
        PRINTF("add one img: discard\n");
        m_discard = FALSE;
        return;
    }

    // add image
    m_ptrDsc->GetReplayWriteLock();

    int size = m_deq[m_areaIndex].size();

    if (size < m_maxCount[m_areaIndex])
    {
        m_deq[m_areaIndex].push_back(img);
    }
    else if (size  == m_maxCount[m_areaIndex])
    {
        m_deq[m_areaIndex].pop_front();
        m_deq[m_areaIndex].push_back(img);
    }

    m_ptrDsc->ReadWriteReplayUnlock();
}

const Image::AutoTracePara* Replay::GetCurrentTraceData(void)
{
    int size = m_deq[m_areaIndex].size();

    if (size > 0)
    {
        const Image::AutoTracePara* ptrPara = m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetTraceData();
        PRINTF("sizeVec = %d, sizeMaxVec = %d, begin = %d, end = %d, index = %d\n", ptrPara->vecTrace.size(), ptrPara->vecMaxTrace.size(), ptrPara->cycleBegin, ptrPara->cycleEnd, m_imgIndex[m_areaIndex]);
        return ptrPara;
    }
    else
    {
        return NULL;
    }
}

const std::vector<POINT> Replay::GetInitTraceData()
{
    vector<POINT> vecAll = m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetTraceData()->vecMaxTrace;
    int size = vecAll.size();
    if(size <= 0)
        return vecAll;

    int x1 = 1;
    int x2 = size - 1;

    vector<POINT> vecResult;
    vecResult.clear();
    vector<POINT>::iterator iterBegin = vecAll.begin() + x1;
    vector<POINT>::iterator iterEnd = vecAll.begin() + x2;
    vecResult.assign(iterBegin, iterEnd);

    return vecResult;
}
/*
 * @brief get trace data between x1 and x2
 * @para x1[in] start point of auto trace, point position is according to position of pw image start
 * @para x2[in] end point of auto trace, must larger than x1
 * @retval vecResult[out] auto trace data between x1 and x2
 */
const std::vector<POINT> Replay::GetTraceData(int x1, int x2)
{
    vector<POINT> vecResult;
    vecResult.clear();
    if(m_imgIndex[m_areaIndex] <= 0) return vecResult;

    vector<POINT> vecAll = m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetTraceData()->vecMaxTrace;
    int size = vecAll.size();

    PRINTF("m_imgIndex:%d  size:%d\n", m_imgIndex[m_areaIndex], size);
    if ((x1 < x2) && (x1 < size) && (x2 < size) && (size > 0) )
    {
        vector<POINT>::iterator iterBegin = vecAll.begin() + x1;
        vector<POINT>::iterator iterEnd = vecAll.begin() + x2;
        vecResult.assign(iterBegin, iterEnd);
    }
    PRINTF("-------replay trace: size of new vector = %d\n", vecResult.size());

    return vecResult;
}

void Replay::UpdateReplayCtrl(bool on, EKnobReturn ret)
{
//printf("---updateReplayCtrl value = %d\n", on);
    EKnobReturn retSpeed = OK;
    if (m_speedIndex == (MAX_SPEED-1))
        retSpeed = MAX;
    else if (m_speedIndex == 0)
        retSpeed = MIN;
    else
        retSpeed = OK;
#if (defined (EMP_322) || defined(EMP_313))
    m_ptrUpdate->ReplayCtrl(on);
#else
    m_ptrUpdate->ReplayCtrl(on, m_speedIndex, retSpeed);
#endif
}

void Replay::ClearCurReplayData()
{
    Update2D::SetCineRemoveImg(3);

    // clear replay info
    m_ptrDsc->GetReplayWriteLock();
    m_deq[m_areaIndex].clear();
    m_ptrDsc->ReadWriteReplayUnlock();

    m_imgIndex[m_areaIndex] = 0;
    m_trimLeft = 0;
    m_trimRight = 0;
}
/*
 * @brief clear images in current replay area
 */
void Replay::PrepareForEndReplay()
{
    ClearCurReplayData();
    m_ptrUpdate->ClearReplayBar();
}

/*
 *@brief get replay info when freeze
 */
void Replay::PrepareForReplay()
{
    int size = m_deq[m_areaIndex].size();
#ifdef EMP_355
    m_speedIndex=0;
    m_ptrUpdate->ReplayCtrl(false, m_speedIndex, MIN);
    m_ptrUpdate->ReplayTrimLeft(m_trimLeft+1);
#endif
    m_trimLeft = 0;
    m_trimRight = size - 1;

    if((ViewSuperuser::GetInstance()->GetDemoStatus()) && m_viewMode)
        m_imgIndex[m_areaIndex] = m_trimLeft;
    else
        m_imgIndex[m_areaIndex] = m_trimRight;

    // m_imgIndex[m_areaIndex] = m_trimRight;

    if (ScanMode::GetInstance()->GetScanMode() != ScanMode::EFOV)
    {
        m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);
        m_ptrUpdate->ReplayTrimLeft(m_trimLeft+1);
        m_ptrUpdate->ReplayTrimRight(m_trimRight+1);
    }

    m_discard = FALSE;
}

void Replay::DisplayReplayBar()
{
    int size = m_deq[m_areaIndex].size();

    //draw
    if (size == 0)
        m_ptrUpdate->ReplayBar(0, size, 0, 0);
    else
        m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);
}

unsigned int* Replay::ReviewOneImg()
{
    //update replay mode
    FreezeMode::GetInstance()->SetReplayMode();

    return ReviewOneImgOnly();
}
unsigned int* Replay::ViewOneImg()
{
    //update replay mode
    FreezeMode::GetInstance()->SetReplayMode();

    return ViewOneImgOnly();
}

//2B下，判断当前区域是否有图像zjx
bool Replay::ReviewNoImgBB(int curB)
{
    if(curB == 1){
        int size = m_deq[0].size();
        if (size == 0)
            return TRUE;
        else
            return FALSE;
    }
    if(curB == 0){
        int size = m_deq[1].size();
        if (size == 0)
            return TRUE;
        else
            return FALSE;
    }
}

//4B下，判断当前区域是否有图像zjx
bool Replay::ReviewNoImgB4(int curB)
{
    if(curB == 4)
        return FALSE;

    int size = m_deq[curB+1].size();
    if (size == 0)
        return TRUE;
    else
        return FALSE;
}

/*
 * @brief return one image data when review(m_count[i] ~ 1)
 *
 * @retval pointer pointed to review image
 */
unsigned int* Replay::ReviewOneImgOnly()
{
#if 0
    PRINTF("review one image\n");
    PRINTF("m_maxCount[m_areaIndex] = %d\n", m_maxCount[m_areaIndex]);
    PRINTF("m_imgIndex[m_areaIndex] = %d\n", m_imgIndex[m_areaIndex]);
    PRINTF("m_areaIndex = %d\n", m_areaIndex);
    PRINTF("m_trimLeft= %d\n", m_trimLeft);
    PRINTF("m_trimRight= %d\n", m_trimRight);
#endif
    int size = m_deq[m_areaIndex].size();

    if (size == 0)
        return NULL;

    //review
        if (m_imgIndex[m_areaIndex] > m_trimLeft)
            m_imgIndex[m_areaIndex] --;
        else
            m_imgIndex[m_areaIndex] = m_trimRight;

    //draw
    m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);

    //review
    unsigned int* ptrTemp = m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetImg();
    m_ptrUpdateFrameFreeze(ptrTemp, Image::WIDTH, Image::HEIGHT);

    return ptrTemp;
}

/*
 * @brief return one image data when view(1 ~ m_count[i])
 *
 * @retval pointer pointed to view image
 */
unsigned int* Replay::ViewOneImgOnly()
{
#if 0
    PRINTF("view one image\n");
    PRINTF("m_maxCount[m_areaIndex] = %d\n", m_maxCount[m_areaIndex]);
    PRINTF("m_imgIndex[m_areaIndex] = %d\n", m_imgIndex[m_areaIndex]);
    PRINTF("m_areaIndex = %d\n", m_areaIndex);
    PRINTF("m_trimLeft= %d\n", m_trimLeft);
    PRINTF("m_trimRight= %d\n", m_trimRight);
    PRINTF("m_imgIndex[m_areaIndex] = %d\n", m_imgIndex[m_areaIndex]);
#endif
    //view
    int size = m_deq[m_areaIndex].size();
    if (size == 0)
        return NULL;

    if (m_imgIndex[m_areaIndex] < m_trimRight)
    {
        m_imgIndex[m_areaIndex] ++;
    }
    else
    {
        if((ViewSuperuser::GetInstance()->GetDemoStatus())&&(!g_menuReview.OnlyOneVideoDemo()))
        {
            g_menuReview.PlayNextVideoDemo();
            return NULL;
        }
        else
        {
            m_imgIndex[m_areaIndex] = m_trimLeft;
        }
    }

    //draw
    m_ptrUpdate->ReplayBar(m_imgIndex[m_areaIndex]+1, size, m_trimLeft, m_trimRight);

    //view
    unsigned int* ptrTemp = m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetImg();
    m_ptrUpdateFrameFreeze(ptrTemp, Image::WIDTH, Image::HEIGHT);

    return ptrTemp;
}
unsigned int* Replay::GetCurrentImg()
{
    return (unsigned int*)m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetImg();
}

/*
 * @brief get the select image for IMT measure when freeze
 */
 unsigned int* Replay::GetSelectImgForIMT()
{
    int size = m_deq[m_areaIndex].size();
    if ((size <= 1) || m_imgIndex[m_areaIndex] < 1)
        return NULL;
    return m_deq[m_areaIndex][m_imgIndex[m_areaIndex]].GetImg();
}

/*
 * @brief get the preselect image for IMT measure when freeze
 */
unsigned int* Replay::GetSelectNextImgForIMT()
{
    int size = m_deq[m_areaIndex].size();
    if (size == 0)
        return NULL;
    return m_deq[m_areaIndex][m_imgIndex[m_areaIndex]-1].GetImg();
}

/*
 * @brief get the last image for IMT measure whenever unfreeze
 */
unsigned int* Replay::GetLastImgForIMT()
{
    if (m_deq[m_areaIndex].size() < 1)
    {
        return NULL;
    }
    int index = m_deq[m_areaIndex].size() - 1;
    return (unsigned int*)m_deq[m_areaIndex][index].GetImg();
}

/*
 * @brief get the next to the last image for IMT measure.
 */
unsigned int* Replay::GetNextImgForIMT()
{
    if (m_deq[m_areaIndex].size() < 2)
        return NULL;

    int index = m_deq[m_areaIndex].size() - 1 - 1;
    return (unsigned int*)m_deq[m_areaIndex][index].GetImg();
}

/*
 * @brief get addr of cine image data
 */
bool Replay::GetCineImg(deque<Image>** ptrDeq, int &cineBegin, int &cineCount, int &cineWidth, int &cineHeight, int &fps)
{
    if (m_deq[m_areaIndex].size() == 0)
        return FALSE;

    *ptrDeq = &m_deq[m_areaIndex];
    cineBegin = m_trimLeft;
    cineCount = m_trimRight - m_trimLeft + 1;
    cineWidth = Image::WIDTH;
    cineHeight = Image::HEIGHT;
    fps = m_fps;

    return TRUE;
}

void Replay::EnterViewStatus()
{
    m_viewMode = TRUE;
}

void Replay::EnterReviewStatus()
{
    m_viewMode = FALSE;
}

void Replay::AutoReplayEnd()
{
    // time off
    if (m_tagTimer > 0)
    {
        if (!g_source_remove(m_tagTimer))
        {
           // printf("remove timer error\n");
        }
        m_tagTimer = 0;
        //printf("remove timer\n");
    }
}

bool Replay::AutoReplayStartView()
{
    if (m_deq[m_areaIndex].size() > 1)
    {
        int interval = GetReplayTime(m_speedIndex);

        // timer on
        if (m_tagTimer > 0)
        {
            if (!g_source_remove(m_tagTimer))
            {
               // printf("remove timer error\n");
            }
            m_tagTimer = 0;
        }

        m_tagTimer = g_timeout_add(interval, ViewCallBack, NULL);
        if (m_tagTimer > 0)
        {
           // printf("create timer for replay ok\n");
        }
        return TRUE;
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Replay]: No image can be replayed."), 1);
    }

    return FALSE;
}

bool Replay::AutoReplayStartReview()
{
    if (m_deq[m_areaIndex].size() > 1)
    {
        int interval = GetReplayTime(m_speedIndex);

        // timer on
        if (m_tagTimer > 0)
        {
            if (!g_source_remove(m_tagTimer))
            {
               // printf("remove timer error\n");
            }
            m_tagTimer = 0;
        }

        m_tagTimer = g_timeout_add(interval, ReviewCallBack, NULL);
        if (m_tagTimer > 0)
        {
           // printf("create timer for replay ok\n");
        }

        return TRUE;
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Replay]: No image can be replayed."), 1);
    }

    return FALSE;
}

void Replay::ClearAll()
{
    int i;

    m_ptrDsc->GetReplayWriteLock();

    for (i = 0; i < MAX_AREA; i ++)
    {
        m_deq[i].clear();
        m_imgIndex[i] = 0;
    }

    m_ptrDsc->ReadWriteReplayUnlock();

}

// cine read

/*
 * @brief add one image to current replay area(m_deqor)
 *
 * @para img image to be added
 */
void Replay::AddOneCineImg(Image img)
{
#if 0
    m_ptrDsc->GetReplayWriteLock();

    int size = m_deq[m_areaIndex].size();

    if (size < m_maxCount[m_areaIndex])
    {
        m_deq[m_areaIndex].push_back(img);
    }
    else if (size  == m_maxCount[m_areaIndex])
    {
        m_deq[m_areaIndex].pop_front();
        m_deq[m_areaIndex].push_back(img);
    }

    m_ptrDsc->ReadWriteReplayUnlock();
#endif

    AddOneImg(img);
}

/*
 * @brief enter cine read mode, you can use system replay memory temporarily. read replay data from harddisk to memory, then replay it.
 * @note  1)if in snap read mode now, must exit it first, or else replay image can not be displayed correctly
 *        2)ExitReadCine must be called when EnterReadCine is calledi(after use it).
 */
void Replay::EnterReadCine()
{
    ExitReadSnap();

    if (!m_enterCine)
    {
        m_enterCine = TRUE;
        m_discard = FALSE;

        //bak
        for (int i = 0; i < MAX_AREA; i ++)
            m_maxCountBak[i] = m_maxCount[i];
        m_areaIndexBak = m_areaIndex;
        m_areaNumBak = m_areaNum;
        m_fpsBak = m_fps;

        SetAreaNum(1);
    }

    PrepareForEndReplay();
    KnobCineCreate();
}

/*
 * @brief exit read cine mode, must call after EnterReadCine
 */
void Replay::ExitReadCine()
{
    if (m_enterCine)
    {
        m_enterCine = FALSE;
        m_discard = FALSE;
        //m_discard = TRUE; //lhm

        for (int i = 0; i < MAX_AREA; i ++)
            m_maxCount[i]= m_maxCountBak[i];
        m_areaIndex= m_areaIndexBak;
        m_areaNum= m_areaNumBak;
        m_fps = m_fpsBak;

        ClearAll();

        m_imgIndex[m_areaIndex] = 0;
        m_trimLeft = 0;
        m_trimRight = 0;
        m_ptrUpdate->ReplayTrimLeft(m_trimLeft);
        m_ptrUpdate->ReplayTrimRight(m_trimRight);

     // m_ptrUpdate->ReplayTrimLeft(m_trimLeft+1);
       // m_ptrUpdate->ReplayTrimRight(m_trimRight+1);

        m_ptrUpdate->ClearReplayBar();
    }
    KnobUndo();
}

/*
 * @brief set new frame update func(draw image on pointed draw area)
 *
 * @para drawImg[in] function pointer pointed to draw-image function
 */
void Replay::SetUpdateFrameFunc(UPDATEFRAME drawImg)
{
    m_ptrUpdateFrameFreeze = drawImg;
}

/*
 * @brief set default frame update func(draw image on draw area in main window)
 *
 */
void Replay::SetDefaultFrameFunc(void)
{
    m_ptrUpdateFrameFreeze = DscMan::GetInstance()->GetFrameUpdateFunFreeze();
}
/*
 * @brief enter snap read mode, read data from harddisk and display it in main window
 * @note in this mode, system image can not be display correctly, so ExitReadSnap must be
 *       called when EnterReadSnap is called(after used it).
 */
void Replay::EnterReadSnap()
{
    if (!m_enterSnap)
    {
        m_enterSnap = TRUE;
        BakImgForSnap();
    }
}
void Replay::ExitReadSnap()
{
    if (m_enterSnap)
    {
        m_enterSnap = FALSE;
        RestoreImgForSnap();
        ScanMode::GetInstance()->ExitSpecialMeasure();
    }
    KnobUndo();
}

bool Replay::GetReadSnapStatus()
{
    return m_enterSnap;
}

void Replay::BakImgForSnap()
{
    ImageArea* ptrImgArea = ImageArea::GetInstance();
    TopArea* ptrTopArea = TopArea::GetInstance();

    if(!ptrTopArea->GetReadImg())
        ptrTopArea->SetReadImg(TRUE);
    if(!ptrImgArea->GetReadImg())
        ptrImgArea->SetReadImg(TRUE);
}

void Replay::RestoreImgForSnap()
{
    ImageArea* ptrImgArea = ImageArea::GetInstance();
    TopArea* ptrTopArea = TopArea::GetInstance();

    if(ptrTopArea->GetReadImg())
        ptrTopArea->SetReadImg(FALSE);
    if(ptrImgArea->GetReadImg())
        ptrImgArea->SetReadImg(FALSE);
}

/*
 * @brief get real replay time
 *
 * @retval replay time interval, unit: ms
 */
int Replay::GetReplayTime(int speedIndex)
{
    if (speedIndex > (MAX_SPEED - 1))
        speedIndex = MAX_SPEED - 1;

    int time = REPLAY_SPEED[speedIndex] * (1 / (float)m_fps) * 1000;
    //printf("-------------------------------m_fps = %d, time = %dms\n", m_fps, time);

    return time;
}

void Replay::EnterEFOVReview(void)
{
    m_efovFrameNo = 0;
    m_efovAutoReviewFlag = FALSE;
    ScanMode::GetInstance()->EnterEFOVReview();
    m_ptrUpdate2D->EFOVReplay(FALSE, PRESS);
    SyncKnobEFOVReview(EFOV_SAVE, N_("Press"), ERROR);
}

void Replay::ExitEFOVReview(void)
{
    EFOVAutoReviewEnd();
    ScanMode::GetInstance()->EnterEFOVView();
}

EKnobReturn Replay::ChangeEFOVReviewDir(EKnobOper oper)
{
    EFOVAutoReviewEnd();
    int total = 0;
    if(DscMan::GetInstance()->GetDsc() != NULL)
        total = DscMan::GetInstance()->GetDsc()->GetEFOVTotalFrameNo();

    if (oper == ADD)
    {
        if (m_efovFrameNo < (total-1))
        {
            m_efovFrameNo ++;
        }
        else
        {
            PRINTF("replay: EFOV frameno reach max value!\n");
            return (MAX);
        }
    }
    else if (oper == SUB)
    {
        if (m_efovFrameNo > 0)
        {
            m_efovFrameNo --;
        }
        else
        {
            PRINTF("replay: EFOV frameNo speed reach min value!\n");
            return (MIN);
        }
    }
    else
    {
        return ERROR;
    }

    EKnobReturn ret = OK;
    if (m_efovFrameNo == (total-1))
        ret = MIN;
    else if (m_efovFrameNo == 0)
        ret = MAX;
    else
        ret = OK;

    if(DscMan::GetInstance()->GetDsc() != NULL)
        DscMan::GetInstance()->GetDsc()->SetEFOVReviewFrameNo(m_efovFrameNo);

    return ret;
}

void Replay::GetEFOVCineImg(unsigned char **data, int &size)
{
    if(DscMan::GetInstance()->GetDsc() != NULL)
        DscMan::GetInstance()->GetDsc()->SaveEFOVCine(data, size);
}

void Replay::SetEFOVCineImg(unsigned char *data, int size)
{
    m_efovFrameNo = 0;
    if(DscMan::GetInstance()->GetDsc() != NULL)
        DscMan::GetInstance()->GetDsc()->ReadEFOVCine(data, size);
}

void Replay::EFOVAutoReviewStart()
{
    if(m_efovAutoReviewFlag)
        return;

        // set replay para
        ClearCurReplayData();

    int total = 0;
    if(DscMan::GetInstance()->GetDsc() != NULL)
        total = DscMan::GetInstance()->GetDsc()->GetEFOVTotalFrameNo();

    if (total > 1)
    {
        // timer on
        if (m_tagTimerEFOV == 0)
        {
            m_tagTimerEFOV = g_timeout_add(50, EFOVReviewCallBack, NULL);
            if (m_tagTimerEFOV > 0)
            {
                m_efovAutoReviewFlag = TRUE;
                PRINTF("create timer for replay ok\n");
                //knob update
                m_ptrUpdate2D->EFOVReplay(TRUE, PRESS);
            }
        }
    }
    else
    {
        HintArea::GetInstance()->UpdateHint(_("[Replay]: No image can be replayed."), 1);
    }
    SyncKnobEFOVReview(EFOV_SAVE, N_("Press"), ERROR);
}

void Replay::EFOVAutoReviewEnd()
{
    if(!m_efovAutoReviewFlag)
        return;

        PrepareForReplay();

    // time off
    if (m_tagTimerEFOV > 0)
    {
        if (!g_source_remove(m_tagTimerEFOV))
        {
            PRINTF("remove timer error\n");
        }
        m_tagTimerEFOV = 0;
        m_efovAutoReviewFlag = FALSE;
        PRINTF("remove timer\n");
        //knob update
        m_ptrUpdate2D->EFOVReplay(FALSE, PRESS);
    }
    SyncKnobEFOVReview(EFOV_SAVE, N_("Press"), PRESS);
}

void Replay::EFOVAutoReview()
{
    int total = 0;
    if(DscMan::GetInstance()->GetDsc() != NULL)
        total = DscMan::GetInstance()->GetDsc()->GetEFOVTotalFrameNo();

    if(m_efovFrameNo < total)
    {
        if(DscMan::GetInstance()->GetDsc() != NULL)
            DscMan::GetInstance()->GetDsc()->SetEFOVReviewFrameNo(m_efovFrameNo);
        m_efovFrameNo++;
    }

    if(m_efovFrameNo >= total)
    {
        EFOVAutoReviewEnd();
        m_efovFrameNo = 0;
    }
}

void Replay::ChangeEFOVAutoReviewFlag()
{
    if(m_efovAutoReviewFlag)
        EFOVAutoReviewEnd();
    else
        EFOVAutoReviewStart();
}

bool Replay::EFOVVideoOutOfRange(void)
{
    if (m_deq[m_areaIndex].size() >= (unsigned int)TOTAL_REPLAY)
        return true;
    else
        return false;
}