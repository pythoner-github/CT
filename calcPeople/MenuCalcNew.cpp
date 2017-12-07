#include "calcPeople/MenuCalcNew.h"
#include "calcPeople/MeaFactoryMainSingle.h"
#include "sysMan/CalcSetting.h"
#include "sysMan/SysCalculateSetting.h"
#include "calcPeople/MeaCalcFun.h"
#include "projectMode/MenuProjectMode.h"
#include "display/MenuArea.h"
#include "sysMan/ViewSuperuser.h"

static void MeasureFunc(int type)
{
    MeaFactoryMainSingle::GetInstance()->Create((int)type);
}

static void ExpandChildMenu(int type)
{
    g_menuCalc.ExpandMenu();
}

SingleItemInfo userDef_new[MAX_USER_CALC_NUM];

GUIMenuNew g_calcAllDefaultItem[] = {
    {NULL, ABD_GALLBLAD_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, ABD_GALLBLAD_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_GALLBLAD_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_GALLBLAD_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_CBD_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_GB_WALL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_LIVER_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SPLEEN_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, ABD_SPLEEN_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SPLEEN_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SPLEEN_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_PANCREAS_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, ABD_PANCREAS_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_PANCREAS_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_PANCREAS_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_AO_DIST_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_AO_MID_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_AO_PROX_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_DIST_AO, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_CELIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_COM_HEP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_L_HEP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_R_HEP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_GDA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_IMA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SMA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SPLENIC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_PROX_IVC, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_DIST_IVC, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_L_HEP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_M_HEP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_R_HEP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_L_PORT_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_M_PORT_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_R_PORT_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SPLENIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_IMV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ABD_SMV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_BLADDER_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, UR_BLADDER_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_BLADDER_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_BLADDER_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_PV_BLADDER_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, UR_PV_BLADDER_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_PV_BLADDER_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_PV_BLADDER_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_PROSTATE_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, UR_PROSTATE_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_PROSTATE_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_PROSTATE_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_L_KID_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, UR_L_KID_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_L_KID_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_L_KID_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, UR_L_DIST_REN_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_L_MAIN_REN_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_L_REN_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, UR_L_URETER_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_L_URETER_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_R_KID_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, UR_R_KID_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_R_KID_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_R_KID_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, UR_R_DIST_REN_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_R_MAIN_REN_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_R_REN_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, UR_R_URETER_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, UR_R_URETER_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVLD_APICAL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVIDS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVPWS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVOT_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVIDD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVPWD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVAD_ENDO, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVAD_EPI, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RVIDD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RVAWD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RVOT_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_IVSD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_IVSS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LA_DIMEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AOR_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_ISTHMUS_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_ASC_AO_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_DESC_AO_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MPA_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LPA_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RPA_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_A2CD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_A2CS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_A4CD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_A4CS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVDAL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVSAL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AV_AREA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AI_RADIUS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_RADIUS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_AREA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MR_RADIUS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_RADIUS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RVAWD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RVIDD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_IVSD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVIDD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVPWD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_IVSS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVIDS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVPWS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_HR_LV, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVOT_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AOR_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LA_DIMEN_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LV_ET, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LV_PEP, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AV_CUSP_SEP, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_DE_EXC, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_DE_SLOPE, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_EF_SEP, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_EF_SLOPE, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_EPSS, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_AC_INTERVAL, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_DE_EXC, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_DE_SLOPE, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_EF_SLOPE, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_AC_INTERVAL, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LATE_DIAS_SLOPE, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_A_WAVE_AMP, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_BC_SLOPE, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RV_ET, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RV_PEP, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AV_ACC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AV_DEC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVOT_ACC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_LVOT_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AI_ACC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AI_P12T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AI_DEC_SLOPE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AI_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_AI_END_DIAS_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_VTI, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_HR_AV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_HR_MV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_P12T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_PEAK_E_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_PEAK_A_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_A_DUR, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_IVRT, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_DEC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_DEC_SLOPE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_IVCT, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_ACC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_MR_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_HR_TV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TR_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_ACC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_PEAK_E_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_TV_PEAK_A_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_HR_PV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_PV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_PV_ACC_T, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_PI_END_DIAS_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_PULM_DIAS_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_PULM_SYS_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ADULT_RVOT_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVAWD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVIDD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVSD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVIDD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVPWD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVSS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVIDS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVPWS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LV_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LV_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LV_WALL_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVS_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RV_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RV_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RV_WALL_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LA_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LA_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LA_DIMEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RA_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RA_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RPA_DIAM_DIM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVOT_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVOT_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HEART_AREA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HEART_CIRC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_THORAX_CIRC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AOR_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_ASC_AO_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_ARCH_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_ISTHMUS_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DESC_AO_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DUCT_ART_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MPA_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RPA_DIAM_VES, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LPA_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_SVC_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVC_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_AN_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_AN_CIRC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_AN_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_AN_CIRC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_PV_AN_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_AN_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVAWD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVIDD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVSD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVIDD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVPWD_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVSS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVIDS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVPWS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_LV_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LA_DIMEN_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LA_WALL_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RA_WALL_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AOR_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_ASC_AO_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_ARCH_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_ISTHMUS_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DESC_AO_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DUCT_ART_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LPA_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MPA_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RPA_DIAM_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LV_ET_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LV_PEP_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AV_CUSP_SEP_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_AV_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_DE_EXC_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_DE_SLOPE_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_EF_SLOPE_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_EF_SEP_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_EPSS_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_AC_INTERVAL_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LATE_DIAS_SLOPE_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_A_WAVE_AMP_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_BC_SLOPE_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RV_ET_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RV_PEP_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_PV_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_DE_EXC_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_DE_SLOPE_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_EF_SLOPE_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_AC_INTERVAL_MM, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_PLACENTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_UM_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_UM_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_L_UT_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_R_UT_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_L_OV_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_R_OV_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_FROAMEN_OVALE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DUCT_ART, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_L_COR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_R_COR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AOR, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_ASC_AO, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_ARCH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AO_ISTHMUS, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DESC_AO, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MPA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LPA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RPA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_PULM_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_DUCT_VEN, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVOT_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_LVOT_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AV_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AI_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AI_END_DIAS_VEL, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_AI_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_AV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_MV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_PEAK_E, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_PEAK_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_DEC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_MR_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVOT_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_RVOT_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_PV_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_PV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_PV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_PEAK_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_PEAK_E, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_ACC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_DEC_TIME, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TV_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_TR_VMAX, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HR_TV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_THORACIC_AO, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_L_RENAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_R_RENAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HEPATIC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_HEPATIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_SPLENIC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_L_MCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_R_MCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_L_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_R_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_SVC, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, FETAL_IVC, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_CER, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//CER
    {NULL, OB_CIST_MAG, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//Cisterna Magna Length小脑延髓池长度
    {NULL, OB_OOD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//OOD
    {NULL, OB_TIBIA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//Tibia
    {NULL, OB_TTD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//TTD
    {NULL, OB_IOD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//IOD
    {NULL, OB_RADIUS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//Radius
    {NULL, OB_ULNA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//Ulna
    {NULL, OB_FIBULA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//Fibula
    {NULL, OB_NUCHFOLD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, OB_UMBILICAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_MCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_MCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, OB_AFI, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, OB_AFI_LUQ, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_AFI_LLQ, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_AFI_RUQ, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_AFI_RLQ, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_CRL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_NT, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_CERV, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_SAC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_YOLK_SAC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_OV_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, OB_L_OV_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_OV_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_OV_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_OV_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, OB_R_OV_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_OV_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_OV_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RA_DIMEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RA_LEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RA_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RV_DIMEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RV_LEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RV_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_RVOT, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_IVS_2D, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LA_DIMEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LA_LEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LA_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LV_DIMEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LV_LEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LV_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_LVOT_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_HA_2D, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_HRTC_2D, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_HR, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_THC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_AOR_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_AO_ANNUL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_ASC_AO_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, OB_DESC_AO, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, OB_DESC_AO_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_DUCT_ART_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_DUCT_VEN, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_MV_ANNUL_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_TV_ANNUL_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_MPA_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, OB_THORACIC_AO, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, OB_GS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_BPD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//BPD
    {NULL, OB_AC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_HC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_HL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//Humerus
    {NULL, OB_FL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},//FL
    {NULL, OB_TAD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_APAD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_THD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_OFD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_FTA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_EAR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_ORIBIT1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_ORIBIT2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_POST_FOSSA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_MANDIBLE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_NASAL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_CLAVICLE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_M_PHALANX5, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_FOOT_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_FOOT_A, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_FOOT_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_FOOT_A, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_BLADDER_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_BLADDER_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_BLADDER_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_RENAL_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_RENAL_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_RENAL_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_RENAL_PELVIS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_RENAL_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_RENAL_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_RENAL_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_RENAL_PELVIS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_ADR_GLAND_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_ADR_GLAND_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_ADR_GLAND_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_URETER_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_URETER_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_L_LUNG_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_LUNG_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, OB_L_UTERINE_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_R_UTERINE_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, OB_PELVIS_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_PELVIS_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_PELVIS_TR, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_SCAPULA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_BLADDER_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, GYN_BLADDER_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_BLADDER_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_BLADDER_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_PV_BLADDER_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, GYN_PV_BLADDER_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_PV_BLADDER_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_PV_BLADDER_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_OV_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, GYN_L_OV_HEIGHT, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_OV_LENGTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_OV_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, GYN_L_OVARIAN_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, GYN_R_OV_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, GYN_R_OV_HEIGHT, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_OV_LENGTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_OV_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, GYN_R_OVARIAN_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, GYN_CERV_LENGTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_ENDO_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_UTERUS_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, GYN_UTERUS_HEIGHT, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_UTERUS_LENGTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_UTERUS_WIDTH, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_UTERINE_A_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, GYN_L_UTERINE_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, GYN_R_UTERINE_A_DIAM, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, GYN_R_UTERINE_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, GYN_FHR, MEA_M, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL4, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL5, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL6, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL7, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL8, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL9, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_L_FOLL10, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL4, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL5, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL6, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL7, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL8, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL9, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_R_FOLL10, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID4, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID5, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID6, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID7, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID8, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID9, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, GYN_FIBROID10, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifdef VET
    {NULL, SP_GALACT1_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_GALACT2_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_GALACT3_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_GALACT4_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_GALACT5_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE1_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE1_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE1_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE1_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE1_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE1_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE2_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE2_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE2_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE2_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE2_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE2_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE3_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE3_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE3_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE3_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE3_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE3_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE4_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE4_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE4_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE4_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE4_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE4_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE5_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE5_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE5_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE5_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE5_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE5_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#else
    {NULL, SP_L_GALACT_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_GALACT_THICK, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_L_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_L_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_L_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_L_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_L_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_L_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_R_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_R_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_R_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_R_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_BRE_R_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_BRE_R_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, SP_L_TESTIS_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_L_TESTIS_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_L_TESTIS_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_L_TESTIS_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, SP_L_TESTIS_ARTERY, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, SP_R_TESTIS_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_R_TESTIS_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_TESTIS_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_TESTIS_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, SP_R_TESTIS_ARTERY, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, SP_TES_L_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_L_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_L_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_L_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_L_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_L_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_L_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_R_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_R_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_R_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_R_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_TES_R_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TES_R_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_ISTHMUS_AP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_LONG_L_LOBE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_AP_L_LOBE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TR_L_LOBE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_LONG_R_LOBE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_AP_R_LOBE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_TR_R_LOBE, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
#ifndef CT_322
    {NULL, SP_SUPA_L_LOBE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_INFA_L_LOBE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_SUPA_R_LOBE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_INFA_R_LOBE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#endif
    {NULL, SP_THY_LESION1_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_THY_LESION1_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION1_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION1_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION2_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_THY_LESION2_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION2_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION2_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION3_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_THY_LESION3_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION3_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION3_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION4_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_THY_LESION4_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION4_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION4_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION5_VOL, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}},
    {NULL, SP_THY_LESION5_L, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION5_W, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_THY_LESION5_H, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_L_EYE_OA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_L_EYE_LENS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_L_EYE_AC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_L_EYE_ON, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_EYE_OA, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_EYE_LENS, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_EYE_AC, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, SP_R_EYE_ON, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_CCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_ICA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_LSV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_LSV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_LSV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_LSV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_LSV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_LSV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_CEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_CEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_CEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_CEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_CEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_CEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_ECA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_ECA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_BRACHIOCEPH_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_BRACHIOCEPH_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_BRACHIOCEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_BRACHIOCEPH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_BULB, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_BULB, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_VERTEBRAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_VERTEBRAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_IJV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_IJV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_SCL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_SCL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_SCL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_SCL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_SCL_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_SCL_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_SCL_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_SCL_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_CFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_CFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_CFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_CFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_COM_ILIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_COM_ILIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_COM_ILIAC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_COM_ILIAC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_ATA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_ATA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_ATA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_ATA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_ATA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_ATA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_ATV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_ATV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_ATV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_ATV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_ATV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_ATV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_GSV_CALF, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_GSV_CALF, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_GSV_CALF, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_GSV_CALF, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_GSV_CALF, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_GSV_CALF, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_GSV_THIGH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_GSV_THIGH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_GSV_THIGH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_GSV_THIGH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_GSV_THIGH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_GSV_THIGH, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_PERO_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_PERO_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_PERO_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_PERO_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_PERO_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_PERO_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_PERO_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_PERO_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_PERO_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_PERO_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_PERO_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_PERO_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_POP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_POP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_POP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_POP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_POP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_POP_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_POP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_POP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_POP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_POP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_POP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_POP_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_PTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_PTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_PTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_PTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_PTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_PTA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_PTV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_PTV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_PTV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_PTV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_PTV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_PTV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_SFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_SFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_SFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_SFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_SFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_SFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_SFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_SFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_SFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_SFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_SFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_SFV, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DOR_PEDIS, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DOR_PEDIS, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_EXT_ILIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_EXT_ILIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_EXT_ILIAC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_EXT_ILIAC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_INT_ILIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_INT_ILIAC_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_INT_ILIAC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_INT_ILIAC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_LAT_SURAL_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_LAT_SURAL_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_PFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_PFA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_SFJ, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_SFJ, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_ANTECUBE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_ANTECUBE, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_AX_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_AX_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_AX_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_AX_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_BASILIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_BASILIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_BASILIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_BASILIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_BASILIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_BASILIC_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_BRACH_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_BRACH_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_BRACH_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_BRACH_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_BRACH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_BRACH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_BRACH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_BRACH_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_RAD_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_RAD_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_RAD_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_RAD_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_RAD_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_RAD_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_RAD_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_RAD_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_RAD_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_RAD_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_RAD_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_RAD_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_ULNAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_ULNAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_ULNAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_ULNAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_ULNAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_ULNAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_DIST_ULNAR_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_DIST_ULNAR_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_MID_ULNAR_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_MID_ULNAR_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_L_PROX_ULNAR_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, VS_R_PROX_ULNAR_V, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_L_ACA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_R_ACA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_L_VERTEBRAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_R_VERTEBRAL_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_L_MCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_R_MCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_L_PCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_R_PCA, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TCD_BASILAR_A, MEA_D, TEXT_ONLY, true, {MeasureFunc}},
#ifdef VET
    {NULL, ORTHO_LT_HIP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ORTHO_RT_HIP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES1_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES1_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES1_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES1_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES2_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES2_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES2_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES2_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES3_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES3_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES3_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES3_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES4_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES4_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES4_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES4_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES5_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES5_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES5_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES5_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES6_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES6_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES6_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES6_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES7_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES7_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES7_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES7_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES8_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES8_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES8_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES8_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES9_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES9_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES9_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES9_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES10_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES10_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES10_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES10_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES11_AREA,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES11_AREA1, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES11_AREA2, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, TD_LES11_AREA3, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_DOG_GSD, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_DOG_CRL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_DOG_HD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_DOG_BD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_CAT_HD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_CAT_BD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_SHEEP_CRL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_SWINE_HL,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_SWINE_SL,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_BOVINE_KCRL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_BOVINE_WCRL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_BOVINE_HCRL, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_BOVINE_BBD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_BOVINE_BTD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_BOVINE_BUD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_EQUINE_GSD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_EQUINE_ERD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_EQUINE_ESD,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ANOB_EQUINE_EED,  MEA_2D, TEXT_ONLY, true, {MeasureFunc}}
#else
    {NULL, ORTHO_LT_HIP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, ORTHO_RT_HIP, MEA_2D, TEXT_ONLY, true, {MeasureFunc}},
    {NULL, OB_EFW_NEW, MEA_2D, TEXT_ONLY, true, {ExpandChildMenu}}
#endif
};

MenuCalcNew g_menuCalc;
int MenuCalcNew::m_fetalOrder = 0;

MenuCalcNew::MenuCalcNew()
    : m_sizeDefaultItems(0), m_examItem("AdultAbdo"), m_scanMode(ScanMode::D2), m_completeMeasureFlag(NULL), m_isExistedOBItem(false)
{
    m_sizeDefaultItems = sizeof(g_calcAllDefaultItem) / sizeof(GUIMenuNew);

    int count = 0;
    for (count = 0; count < m_sizeDefaultItems; count++)
    {
        if (g_calcAllDefaultItem[count].pf.ptrNone == ExpandChildMenu)
        {
#ifndef VET
            if(g_calcAllDefaultItem[count].etype == OB_EFW_NEW)
            {
                SysCalculateSetting sysObCalc;
                int efw = sysObCalc.GetEfwMethod();
                if(efw == EFW_HADLOCK1)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_HADLOCK2)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_HADLOCK3)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_HC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_HADLOCK4)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_HC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_SHEPARD)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                }
                else if(efw == EFW_TOKYO)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_TAD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_APAD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
            }
            else
            {
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+1]);
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+2]);
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+3]);
                if (g_calcAllDefaultItem[count].etype == OB_AFI)
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+4]);
            }
#else
            g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+1]);
            g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+2]);
            g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+3]);
            if (g_calcAllDefaultItem[count].etype == OB_AFI)
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+4]);

#endif
        }
        m_vecAllCalcItem.push_back(g_calcAllDefaultItem[count]);
    }

    UpdateAllCalcItems();
    ClearAllFlag();
}

MenuCalcNew::~MenuCalcNew()
{
    ClearUserDefineCalcItem();
    m_vecAllCalcItem.clear();
    m_vecCurExamItemCalcItem.clear();
    m_vecCurScanModeCalcItem.clear();
    if (m_completeMeasureFlag != NULL)
    {
        delete m_completeMeasureFlag;
        m_completeMeasureFlag = NULL;
    }
}

GtkWidget * MenuCalcNew::Create()
{
    m_table = gtk_table_new(18, 6, TRUE);
    gtk_table_set_row_spacing(GTK_TABLE(m_table), 0, 5);
#ifndef VET
    m_buttonFetal1 = gtk_button_new_with_label(_("Fetal 1"));
    m_buttonFetal2 = gtk_button_new_with_label(_("Fetal 2"));
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_buttonFetal1, 0, 3, 0, 1);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_buttonFetal2, 3, 6, 0, 1);
    g_signal_connect(m_buttonFetal1, "clicked", G_CALLBACK(HandleFetal1Pressed), this);
    g_signal_connect(m_buttonFetal2, "clicked", G_CALLBACK(HandleFetal2Pressed), this);
#endif
    m_scrolledWindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_scrolledWindow), GTK_SHADOW_IN);
    m_treeView = m_treeMenu.Create(m_vecCurScanModeCalcItem);

    gtk_widget_modify_base(m_treeView, GTK_STATE_NORMAL, g_deep);
    gtk_container_add (GTK_CONTAINER (m_scrolledWindow), m_treeView);
    gtk_table_attach_defaults(GTK_TABLE(m_table), m_scrolledWindow, 0, 6, 1, 18);
    gtk_widget_modify_bg(m_scrolledWindow, GTK_STATE_NORMAL, g_deep);

    return m_table;
}

bool MenuCalcNew::IsFlagExist()
{
    for (int j = 0; j < (int)m_vecAllCalcItem.size(); j++)
    {
        if(m_completeMeasureFlag[j])
            return true;
    }
    return false;
}

void MenuCalcNew::ChangeAllCalcItems()
{
    UpdateAllCalcItems();
    UpdateCurrentExamItemCalcItems(m_examItem);
    UpdateCurrentScanModeCalcItems(m_scanMode);
    m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeCalcItem);
}

void MenuCalcNew::UpdateAllCalcItems()
{
    ClearUserDefineCalcItem();
    ClearUserDefineSingleItemInfo();

    //custom
    int customBegin = USER_START - BASIC_MEA_END;
    int customEnd = CalcSetting::GetInstance()->GetCalcMaxEtype();
    int measureType = DIST_DOT;
    string calcItemName = "";
    for (int i = customBegin; i <= customEnd; i++)
    {
        CalcSetting::GetInstance()->GetCustomCalcMeasure(i, measureType, calcItemName);
        GUIMenuNew menu;
        char *pName = new char[calcItemName.size() + 1];
        strcpy(pName, calcItemName.c_str());
        menu.name = pName;
        menu.etype = i + BASIC_MEA_END;
        menu.type = TEXT_ONLY;
        menu.value = true;
        if (strlen(menu.name) != 0)
        {
            menu.mode = GetMeasureModeFromMeasureType(measureType);
            menu.pf.ptrNone = MeasureFunc;
            userDef_new[i - customBegin].meaType = measureType;
            strncpy(userDef_new[i - customBegin].title, menu.name, 40);
            userDef_new[i - customBegin].unitItem = GetUnitItemFromMeasureType(measureType);
        }
        else
        {
            menu.mode = MEA_UNKNOWN;
            menu.pf.ptrNone = NULL;
        }
        m_vecAllCalcItem.push_back(menu);
    }
}

void MenuCalcNew::UpdateEfwItem(int efw)
{
    m_vecAllCalcItem.clear();
    m_sizeDefaultItems = sizeof(g_calcAllDefaultItem) / sizeof(GUIMenuNew);

    int count = 0;
    for (count = 0; count < m_sizeDefaultItems; count++)
    {
        if (g_calcAllDefaultItem[count].pf.ptrNone == ExpandChildMenu)
        {
            g_calcAllDefaultItem[count].child.clear();;
#ifndef VET
            if(g_calcAllDefaultItem[count].etype == OB_EFW_NEW)
            {
                SysCalculateSetting sysObCalc;
                int efw = sysObCalc.GetEfwMethod();
                if(efw == EFW_HADLOCK1)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_HADLOCK2)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_HADLOCK3)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_HC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_HADLOCK4)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_HC - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
                else if(efw == EFW_SHEPARD)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_AC - BASIC_MEA_END]);
                }
                else if(efw == EFW_TOKYO)
                {
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_BPD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_TAD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_APAD - BASIC_MEA_END]);
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[OB_FL - BASIC_MEA_END]);
                }
            }
            else
            {
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+1]);
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+2]);
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+3]);
                if (g_calcAllDefaultItem[count].etype == OB_AFI)
                    g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+4]);
            }
#else
            g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+1]);
            g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+2]);
            g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+3]);
            if (g_calcAllDefaultItem[count].etype == OB_AFI)
                g_calcAllDefaultItem[count].child.push_back(g_calcAllDefaultItem[count+4]);
#endif
        }
        m_vecAllCalcItem.push_back(g_calcAllDefaultItem[count]);
    }

    ChangeAllCalcItems();
}

void MenuCalcNew::ClearAllFlag(void)
{
    if (m_completeMeasureFlag != NULL)
    {
        delete m_completeMeasureFlag;
        m_completeMeasureFlag = NULL;
    }

    m_completeMeasureFlag = new bool[m_vecAllCalcItem.size()];
    for (int j = 0; j <(int)m_vecAllCalcItem.size(); j++)
    {
        m_completeMeasureFlag[j] = false;
    }
}

void MenuCalcNew::ClearUserDefineCalcItem()
{
    for (unsigned int i = m_sizeDefaultItems; i < m_vecAllCalcItem.size(); i++)
        delete [](m_vecAllCalcItem[i].name);
    m_vecAllCalcItem.erase(m_vecAllCalcItem.begin() + m_sizeDefaultItems, m_vecAllCalcItem.end());
}

void MenuCalcNew::ClearUserDefineSingleItemInfo()
{
    for (int i = 0; i < MAX_USER_CALC_NUM; i++)
    {
        userDef_new[i].item = USER_START + i;
        userDef_new[i].meaType = MEA_TYPE_END;
        userDef_new[i].title[0] = 0;
        userDef_new[i].unitItem = UNIT_END;
        userDef_new[i].ptrCalcInfo = NULL;
    }
}

EMeasureMode MenuCalcNew::GetMeasureModeFromMeasureType(int measureType)
{
    EMeasureMode measureMode = MEA_UNKNOWN;
    switch (measureType)
    {
        case DIST_DOT:
        case DEPTH_DIST:
        case DIST_LINE:
        case LENGTH_TRACK:
        case LENGTH_DOT:
        case PERI_TRACK:
        case PERI_ELLIPSE:
        case AREA_TRACK:
        case AREA_DOT:
        case AREA_REC:
        case AREA_ELLIPSE:
        case VOL_3AXIS:
        case VOL_ELLIPSE1:
        case VOL_ELLIPSE2:
        case VOL_SPHEROID:
        case SIMPSONS:
        case AL:
        case ANGLE_3DOT:
        case ANGLE_2LINE:
        case ANGUSTY_DIST_DOT:
        case RATIO_DIST_DOT:
        case ANGUSTY_AREA:
        case RATIO_AREA:
        case RATIO_D_P:
        case RATIO_VOL:
        case RATIO_ANGLE:
        case HIP:
        case PROFILE:
        case HISTOGRAM:
        case SAC_3AXIS:
            measureMode = MEA_2D;
            break;
        case EF:
        case TIME_M:
        case VEL_M:
        case SLOPE:
        case DIST_SLOPE_M:
        case DEPTH_DIST_M:
        case HR_M:
            measureMode = MEA_M;
            break;
        case VEL_D:
        case PSED:
        case RI:
        case SD:
        case TIME_D:
        case HR_D:
        case ACCEL:
        case SLOPE_D:
        case P12T:
        case PI_D:
        case PGMAX:
        case PGMEAN:
        case INTEGRAL_TRACK:
        case MEASURE_TRACK:
        case MEASURE_TRACK_AUTO:
            measureMode = MEA_D;
            break;
        default:
            measureMode = MEA_UNKNOWN;
            break;
    }

    return measureMode;
}

int MenuCalcNew::GetUnitItemFromMeasureType(int measureType)
{
    int unitItem = UNIT_END;
    switch (measureType)
    {
        case DIST_DOT:
        case DEPTH_DIST:
        case DIST_LINE:
        case LENGTH_TRACK:
        case LENGTH_DOT:
        case PERI_TRACK:
        case PERI_ELLIPSE:
        case DEPTH_DIST_M:
        case INTEGRAL_TRACK:
            unitItem = 0;  //CM
            break;
        case VOL_SPHEROID:
            unitItem = D2_VOLUME_1DIST_MULTI;
            break;
        case RATIO_ANGLE:
            unitItem = D2_RATIO_ANGLE_MULTI;
            break;
        case SAC_3AXIS:
            unitItem = OB_SAC_MULTI;
            break;
        case PGMAX:
        case SIMPSONS:
        case AL:
        case HIP:
        case DIST_SLOPE_M:
        case PSED:
        case RI:
        case SD:
        case HR_D:
        case P12T:
            unitItem = UNIT_END + 1; //multi >UNIT_END
            break;
        case AREA_TRACK:
        case AREA_DOT:
        case AREA_REC:
        case AREA_ELLIPSE:
            unitItem = 3; //CM2
            break;
        case VOL_3AXIS:
        case VOL_ELLIPSE1:
        case VOL_ELLIPSE2:
            unitItem = 4; //CM3
            break;
        case ANGLE_3DOT:
        case ANGLE_2LINE:
            unitItem = 9; //ANG
            break;
        case ANGUSTY_DIST_DOT:
        case RATIO_DIST_DOT:
        case ANGUSTY_AREA:
        case RATIO_AREA:
        case RATIO_D_P:
        case RATIO_VOL:
        case PROFILE:
        case HISTOGRAM:
        case PI_D:
        case MEASURE_TRACK:
        case MEASURE_TRACK_AUTO:
            unitItem = 14; //NOUNIT
            break;
        case EF:
            unitItem = 12; //PERCENT
            break;
        case TIME_M:
        case TIME_D:
            unitItem = 5; //SEC
            break;
        case VEL_M:
        case SLOPE:
        case VEL_D:
            unitItem = 1; //CMS
            break;
        case HR_M:
            unitItem = 6; //BPM
            break;
        case ACCEL:
        case SLOPE_D:
            unitItem = 2; //CMS2
            break;
        case PGMEAN:
            unitItem = 8; //MMHG
            break;
        default:
            unitItem = UNIT_END;
            break;
    }

    return unitItem;
}

void MenuCalcNew::ClearAllData()
{
    if (m_completeMeasureFlag != NULL)
    {
        for (int j = 0; j < (int)m_vecAllCalcItem.size(); j++)
        {
            m_completeMeasureFlag[j] = false;
        }
    }
    UpdateCurrentScanModeCalcItems(m_scanMode);
    m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeCalcItem);
#ifndef VET
    m_fetalOrder = 0;
    gtk_button_set_label(GTK_BUTTON(m_buttonFetal1), _("Fetal 1 √"));
    gtk_button_set_label(GTK_BUTTON(m_buttonFetal2), _("Fetal 2"));
#endif
}

void MenuCalcNew::SetCompleteMeasureFlag(int etype)
{
    if (m_completeMeasureFlag != NULL && (int)m_vecAllCalcItem.size() > (etype - BASIC_MEA_END))
    {
        m_completeMeasureFlag[etype - BASIC_MEA_END] = true;
    }
}

void MenuCalcNew::ChangeExamItem(string item)
{
    m_examItem = item;
    UpdateCurrentExamItemCalcItems(m_examItem);
    UpdateCurrentScanModeCalcItems(m_scanMode);
    m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeCalcItem);
}

void MenuCalcNew::UpdateCurrentExamItemCalcItems(string item)
{
    m_vecCurExamItemCalcItem.clear();

    vector<int> vecItemCalc;
    CalcSetting::GetInstance()->GetCalcListEtype((char *)item.c_str(), vecItemCalc);

    for (int i = 0; i < (int)vecItemCalc.size(); i++)
    {
        if (vecItemCalc[i] < 0 || vecItemCalc[i] > (int)m_vecAllCalcItem.size() - 1)
            continue;

        m_vecCurExamItemCalcItem.push_back(m_vecAllCalcItem[vecItemCalc[i]]);
    }
}

void MenuCalcNew::ChangeScanMode(ScanMode::EScanMode mode)
{
    bool isChange = false;
    if (GetMeasureMode(mode) != GetMeasureMode(m_scanMode))
        isChange = true;

    m_scanMode = mode;
    if (isChange)
    {
        UpdateCurrentScanModeCalcItems(m_scanMode);
        m_treeMenu.ChangeModel(m_treeView, m_vecCurScanModeCalcItem);
    }
}

EMeasureMode MenuCalcNew::GetMeasureMode(ScanMode::EScanMode mode)
{
    EMeasureMode meaMode;
    switch (mode)
    {
        case ScanMode::D2:
        case ScanMode::M_INIT:
        case ScanMode::PW_INIT:
        case ScanMode::CW_INIT:
        case ScanMode::PWCFM_INIT:
        case ScanMode::PWPDI_INIT:
        case ScanMode::CWCFM_INIT:
        case ScanMode::CWPDI_INIT:
        case ScanMode::CFM_VS_2D:
        case ScanMode::PDI_VS_2D:
        case ScanMode::EFOV:
        case ScanMode::CFM:
        case ScanMode::PDI:
            meaMode = MEA_2D;
            break;
        case ScanMode::M:
        case ScanMode::ANATOMIC_M:
            meaMode = MEA_M;
            break;
        case ScanMode::PW:
        case ScanMode::PW_SIMULT:
        case ScanMode::CW:
        case ScanMode::PWCFM:
        case ScanMode::PWCFM_SIMULT:
        case ScanMode::PWPDI:
        case ScanMode::PWPDI_SIMULT:
        case ScanMode::CWCFM:
        case ScanMode::CWPDI:
            meaMode = MEA_D;
            break;
        default:
            meaMode = MEA_2D;
            break;
    }

    return meaMode;
}

void MenuCalcNew::UpdateCurrentScanModeCalcItems(ScanMode::EScanMode mode)
{
    EMeasureMode meaMode = GetMeasureMode(mode);

    m_vecCurScanModeCalcItem.clear();
    for (int i = 0; i < (int)m_vecCurExamItemCalcItem.size(); i++)
    {
        if (m_vecCurExamItemCalcItem[i].mode == meaMode)
        {
            m_vecCurScanModeCalcItem.push_back(m_vecCurExamItemCalcItem[i]);

            vector<GUIMenuNew>::iterator iter = m_vecCurScanModeCalcItem.end() - 1;
            UpdateRendererType(iter);
            int size = iter->child.size();
            if (size > 0)
            {
                iter = iter->child.begin();
                for (int s = 0; s < size; s++)
                {
                    UpdateRendererType(iter);
                    iter++;
                }
            }
        }
    }

    m_isExistedOBItem = false;
#ifndef VET
    for (int j = 0; j < (int)m_vecCurScanModeCalcItem.size(); j++)
    {
        if (IsOBItem(m_vecCurScanModeCalcItem[j].etype))
        {
            m_isExistedOBItem = true;
            if (m_fetalOrder == 0)
            {
                gtk_button_set_label(GTK_BUTTON(m_buttonFetal1), _("Fetal 1 √"));
                gtk_button_set_label(GTK_BUTTON(m_buttonFetal2), _("Fetal 2"));
            }
            else
            {
                gtk_button_set_label(GTK_BUTTON(m_buttonFetal1), _("Fetal 1"));
                gtk_button_set_label(GTK_BUTTON(m_buttonFetal2), _("Fetal 2 √"));
            }
            break;
        }
    }
#endif
}

void MenuCalcNew::UpdateRendererType(vector<GUIMenuNew>::iterator iter)
{
    if (m_completeMeasureFlag != NULL && (int)m_vecAllCalcItem.size() > (iter->etype - BASIC_MEA_END))
    {
        if (m_completeMeasureFlag[iter->etype - BASIC_MEA_END])
        {
            iter->type = TOGGLE;
        }
    }
}

bool MenuCalcNew::IsOBItem(int etype)
{
#ifdef VET
    if (etype >= ANOB_MEA_START && etype < ANOB_MEA_END)
    {
        return true;
    }
    else
        return false;

#else
    if (etype >= OB_MEA_START && etype < OB_MEA_END)
    {
        return true;
    }
    else
        return false;

#endif
}

bool MenuCalcNew::ShowMultiplyFetal()
{
    if (m_isExistedOBItem)
    {
        gtk_widget_show(m_buttonFetal1);
        gtk_widget_show(m_buttonFetal2);
    }
    else
    {
        gtk_widget_hide(m_buttonFetal1);
        gtk_widget_hide(m_buttonFetal2);
    }

    return m_isExistedOBItem;
}

void MenuCalcNew::UpdateLabel()
{
    m_treeMenu.Update(m_treeView, m_vecCurScanModeCalcItem);
#ifndef VET
    if (m_fetalOrder == 0)
    {
        gtk_button_set_label(GTK_BUTTON(m_buttonFetal1), _("Fetal 1 √"));
        gtk_button_set_label(GTK_BUTTON(m_buttonFetal2), _("Fetal 2"));
    }
    else
    {
        gtk_button_set_label(GTK_BUTTON(m_buttonFetal1), _("Fetal 1"));
        gtk_button_set_label(GTK_BUTTON(m_buttonFetal2), _("Fetal 2 √"));
    }
    ShowMultiplyFetal();
#endif
}

void MenuCalcNew::Show()
{
#ifdef CT_PROJECT
    if(ViewSuperuser::GetInstance()->GetProjectModeStatus() && g_menuProjectMode.ShowStatus())
    {
        MenuArea::GetInstance()->ShowProjectModeMenu();
        return;
    }
#endif
    gtk_widget_show_all(m_table);
#ifndef VET
    ShowMultiplyFetal();
#endif

    EMeasureSequenceType sequence = (EMeasureSequenceType)CalcSetting::GetInstance()->GetMeasureSequence(m_examItem.c_str());
    if (sequence == MEA_SEQ_REPEAT)
    {
        m_treeMenu.SelectFirst(m_treeView);
    }

}

void MenuCalcNew::Hide()
{
    gtk_widget_hide_all(m_table);
}
void MenuCalcNew::Fetal1Pressed(GtkButton *widget)
{
    m_fetalOrder = 0;
    gtk_button_set_label(widget, _("Fetal 1 √"));
    gtk_button_set_label(GTK_BUTTON(m_buttonFetal2), _("Fetal 2"));
}

void MenuCalcNew::Fetal2Pressed(GtkButton *widget)
{
    m_fetalOrder = 1;
    gtk_button_set_label(GTK_BUTTON(m_buttonFetal1), _("Fetal 1"));
    gtk_button_set_label(widget, _("Fetal 2 √"));
}
void MenuCalcNew::ExpandMenu()
{
    m_treeMenu.ExpandMenu(m_treeView);
}

void MenuCalcNew::SelectNext()
{
    EMeasureSequenceType sequence = (EMeasureSequenceType)CalcSetting::GetInstance()->GetMeasureSequence(m_examItem.c_str());
    if (sequence != MEA_SEQ_REPEAT)
    {
        MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);
        m_treeMenu.SelectNext(m_treeView, sequence);
    }
}

void MenuCalcNew::CompleteCurrentMeasure()
{
    int parentEtype;
    int etype = m_treeMenu.SetCompleteMeasureFlag(m_treeView, parentEtype);
    SetCompleteMeasureFlag(etype);

    if (parentEtype != -1)
        SetCompleteMeasureFlag(parentEtype);
}