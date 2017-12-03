#include "SysImgPreset.h"
#include "ViewSystem.h"
#include "ExamItem.h"

void ExecSetting(ExamItem::EItem itemIndex)
{
	ExamItem examItem;
	ExamItem::ParaItem paraItem;
    int probeIndex = ViewSystem::GetInstance()->GetProbeType();
    if (probeIndex != -1) {
         char *itemName="Adult Abdomen";
        PRINTF("PROBE iNDEX = %d, itemIndex = %d\n", probeIndex, itemIndex);
        examItem.ReadExamItemPara(probeIndex, (int)itemIndex, &paraItem, itemName);
        ViewSystem::GetInstance()->SetImagePara(paraItem);
    }
}
#ifdef VET
#else
void SetAdultAbdo(void)
{
    ExecSetting(ExamItem::ABDO_ADULT);
}

void SetAdultLiver(void)
{
    ExecSetting(ExamItem::ABDO_LIVER);
}

void SetKidAbdo(void)
{
    ExecSetting(ExamItem::ABDO_KID);
}

void SetEarlyPreg(void)
{
	ExecSetting(ExamItem::EARLY_PREG);
}

void SetMiddleLaterPreg(void)
{
    ExecSetting(ExamItem::LATER_PREG);
}

void SetFetusCardio(void)
{
    ExecSetting(ExamItem::CAR_FETUS);
}

void SetGyn(void)
{
    ExecSetting(ExamItem::GYN);
}

void SetKidneyUreter(void)
{
    ExecSetting(ExamItem::KIDNEY);
}

void SetBladderProstate(void)
{
    ExecSetting(ExamItem::BLADDER);
}

void SetAdultCardio(void)
{
    ExecSetting(ExamItem::CAR_ADULT);
}

void SetKidCardio(void)
{
    ExecSetting(ExamItem::CAR_KID);
}

void SetMammaryGlands(void)
{
    ExecSetting(ExamItem::GLANDS);
}

void SetThyroid(void)
{
    ExecSetting(ExamItem::THYROID);
}

void SetEyeBall(void)
{
    ExecSetting(ExamItem::EYE);
}

void SetSmallPart(void)
{
    ExecSetting(ExamItem::SMALL_PART);
}

void SetCarotid(void)
{
    ExecSetting(ExamItem::CAROTID);
}

void SetJugular(void)
{
    ExecSetting(ExamItem::JUGULAR);
}

void SetPeripheryArtery(void)
{
    ExecSetting(ExamItem::PERI_ARTERY);
}

void SetPeripheryVein(void)
{
    ExecSetting(ExamItem::PERI_VEIN);
}

void SetHipJoint(void)
{
    ExecSetting(ExamItem::HIP_JOINT);
}

void SetMeniscus(void)
{
    ExecSetting(ExamItem::MENISCUS);
}

void SetJointCavity(void)
{
    ExecSetting(ExamItem::JOINT_CAVITY);
}

void SetSpine(void)
{
    ExecSetting(ExamItem::SPINE);
}

void SetUser1(void)
{
    ExecSetting(ExamItem::USER1);
}

void SetUser2(void)
{
    ExecSetting(ExamItem::USER2);
}

void SetUser3(void)
{
    ExecSetting(ExamItem::USER3);
}

void SetUser4(void)
{
    ExecSetting(ExamItem::USER4);
}
#endif
