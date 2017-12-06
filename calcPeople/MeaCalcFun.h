#ifndef __MEACALCFUN_H_
#define __MEACALCFUN_H_

#include "calcPeople/MeaResult.h"
#include "display/gui_global.h"

/***************************计算函数**********************************/
class MeaCalcFun{

public:
        static int DoNothing(double data[], double *result, int state, int item, int save);

        static int CalcGW_LMP(unsigned int LMP_Y, unsigned int LMP_M, unsigned int LMP_D);
        static int CalcGW_Ovul(unsigned int LMP_Y, unsigned int LMP_M, unsigned int LMP_D);
        static void CalcEDCB_LMP(unsigned int LMP_Y, unsigned int LMP_M, unsigned int LMP_D, char* retval);
        static void CalcEDCB_Ovul(unsigned int LMP_Y, unsigned int LMP_M, unsigned int LMP_D, char* retval);

        //产科
        static int CalcGWMan(CalcInfoTmp *calcInfo, double data[], int item, int save, int *position, const int parPosi);
        static int EDCBCalc(CalcInfoTmp *calcInfo, double data[], int item, int save, int *position, const int parPosi);
        static int EFWCalc(CalcInfoTmp *calcInfo, double data[], int item, int save, int *position, const int parPosi);

        static int SectionCalc(CalcInfoTmp *calcInfo, double data[], int item, int save, int *calcPosi, const int parPosi);
        static void InitCustomTable();
        static bool SaveCustomTable();
#ifdef VET
        static int CalcGWManVet(CalcInfoTmp *calcInfo, double data[], int item, int save, int *position, const int parPosi);
#endif
    private:
        MeaCalcFun();
        ~MeaCalcFun();

        static MeaResult *m_ptrMeaResult;

        static int CalcGWUser(const double data, int *gw, const int obTable[]);
        static int CalcGWUser(const double data, int *gw, const int obTable[],int k);
        static int CalcGW(const double data, int *gw, const int obTable[]);
        static int CalcGW(const double data, int *gw, const int obTable[],int k);
#ifdef VET
        static int CalcGWVet(const double data, int *gw, const int obTable[][2]);
#endif
        static void CalcAGW(void);
        static int EDCB(int gw, int *EDCB);
        static int EDCBFormula(const int gpLen, int gw, int *EDCB);
        static int EFWGetData(const int item, const int state, const int method, double data[]);
        static int CalcGetValue(const double currValue, double value[][MEA_MULTI], const int allItem[], const int currItem, const int itemNums, const int state);

        static int CalcGetPar(CalcInfoTmp *calcInfo, const double currValue[], double value[][MEA_MULTI], const int currItem, const int parPosi, const int state);
        static double CalcFormula(CalcInfoTmp *calcInfo, double value[][MEA_MULTI]);
        static int FindSection(int item, int *section);
        static void SetTableMaxMin(int table[]);
        static int CalcPSAD(void);

        static void CalcEDCB(unsigned int start_year, unsigned int start_month, unsigned int start_day, int dop, char* retval);
};

enum EOBGwMethod{
    GW_START, CER_START, CER_GOLDSTEIN = CER_START, CER_USER,
    HL_START, HL_JEANTY = HL_START, HL_USER,
    BPD_START, BPD_HADLOCK = BPD_START, BPD_MERZ, BPD_LASSER, BPD_REMPEN, BPD_TOKYO, BPD_USER,
    FL_START, FL_HADLOCK = FL_START, FL_MERZ, FL_JEANTY, FL_TOKYO, FL_USER,
    CRL_START, CRL_HADLOCK =CRL_START, CRL_ROBINSON, CRL_HANSMAN, CRL_LASSER, CRL_TOKYO, CRL_USER,
    GS_START, GS_HELLMAN = GS_START, GS_REMPEN, GS_TOKYO, GS_USER,
    AC_START, AC_HADLOCK = AC_START, AC_MERZ, AC_LASSER, AC_TOKYO, AC_USER,
    HC_START, HC_HADLOCK = HC_START, HC_MERZ, HC_LASSER, HC_USER,
    TAD_START, TAD_MERZ = TAD_START, TAD_USER,
    APAD_START, APAD_MERZ = APAD_START, APAD_USER,
    THD_START, THD_HANDMAN = THD_START, THD_USER,
    OFD_START, OFD_KOREAN = OFD_START, OFD_USER,
    OOD_START, OOD_JEANTY = OOD_START, OOD_USER,
    TIBIA_START, TIBIA_JEANTY = TIBIA_START, TIBIA_USER,
    TTD_START, TTD_HANSMANN = TTD_START, TTD_USER,
    ULNA_START, ULNA_JEANTY = ULNA_START, ULNA_USER,
    FTA_START, FTA_OSAKA = FTA_START, FTA_USER,

    GW_END
};

enum EOBEfwMethod{
    EFW_START, EFW_HADLOCK1=EFW_START, EFW_HADLOCK2, EFW_HADLOCK3, EFW_HADLOCK4, EFW_SHEPARD, EFW_HANSMAN, EFW_TOKYO, EFW_END
};

const titleUnitInfo VolInfo[] = {{"Diameter", CM}, {"Vol", CM3}, {"\0", NOUNIT}};//VOL_SPHEROID
const titleUnitInfo HrInfo[] = {{"Time", SEC}, {"HR", BPM}, {"\0", NOUNIT}};//HR_D
const titleUnitInfo PgmaxInfo[] = {{"Vmax", CMS}, {"PG max", MMHG}, {"\0", NOUNIT}};//PGMAX
const titleUnitInfo RiInfo[] = {{"PS", CMS}, {"ED", CMS}, {"RI", NOUNIT}, {"\0", NOUNIT}};//RI
const titleUnitInfo PsEdInfo[] = {{"PS", CMS}, {"ED", CMS}, {"\0", NOUNIT}};//PSED
const titleUnitInfo SdInfo[] = {{"PS", CMS}, {"ED", CMS}, {"RI", NOUNIT}, {"S/D", NOUNIT}, {"\0", NOUNIT}};//SD
const titleUnitInfo RationAngleInfo[] = {{"Angle1", ANG}, {"Angle2", ANG}, {"Ratio", NOUNIT}, {"\0", NOUNIT}};//RATIO_ANGLE
const titleUnitInfo SimpsonsEdvInfo[] = {{"LVAd", CM2}, {"LVLd", CM}, {"EDV", CM3}, {"\0", NOUNIT}};//SIMPSONS EDV
const titleUnitInfo SimpsonsEsvInfo[] = {{"LVAs", CM2}, {"LVLs", CM}, {"ESV", CM3}, {"\0", NOUNIT}};//SIMPSONS ESV
const titleUnitInfo AlEdvInfo[] = {{"LVAd", CM2}, {"LVLd", CM}, {"EDV", CM3}, {"\0", NOUNIT}};//AL EDV
const titleUnitInfo AlEsvInfo[] = {{"LVAs", CM2}, {"LVLs", CM}, {"ESV", CM3}, {"\0", NOUNIT}};//AL ESV
const titleUnitInfo SlopeMInfo[] = {{"Dist", CM}, {"Slope", CMS}, {"\0", NOUNIT}};//DIST_SLOPE_M
const titleUnitInfo P12tInfo[] = {{"Vmax", CMS}, {"P1/2t", SEC}, {"Slope", CMS2}, {"\0", NOUNIT}};//P12T
const titleUnitInfo HipInfo[] = {{"α", ANG}, {"β", ANG}, {"\0", NOUNIT}};//HIP
const titleUnitInfo SacInfo[] = {{"Diam1", CM}, {"Diam2", CM},{"Diam3", CM},{"mean Diam",CM}, {"\0", NOUNIT}};//Sac

/******************************Basic Measure**************************************/
#if 1
const MultiItemInfo BasicMultiInfo[] = {
    {D2_VOLUME_1DIST, D2_VOLUME_1DIST_MULTI, VOL_SPHEROID, "\0", VolInfo, NULL },
    {D_HR, D_HR_MULTI, HR_D, "\0", HrInfo, NULL },
    {D_PGMAX, D_PGMAX_MULTI, PGMAX, "\0", PgmaxInfo, NULL },
    {D_RI, D_RI_MULTI, RI, "\0", RiInfo, NULL },
    {D_SD, D_SD_MULTI, SD, "\0", SdInfo, NULL },//转义字符
    {D2_RATIO_ANGLE, D2_RATIO_ANGLE_MULTI, RATIO_ANGLE, "\0", RationAngleInfo, NULL },
};

const SingleItemInfo BasicInfo[] = {
    { D2_DIST_DOT, DIST_DOT, N_("Distance"), CM, NULL},
    { D2_DIST_LINE, DIST_LINE, N_("Dist-2Line"), CM, NULL},
    { D2_LEN_TRACK, LENGTH_TRACK, N_("Length"), CM, NULL},
    { D2_LEN_DOT, LENGTH_DOT, N_("Length-Point"), CM, NULL},
    { D2_AREA_TRACK, AREA_TRACK, N_("Area"), CM2, NULL},
    { D2_AREA_DOT, AREA_DOT, N_("Area-Point"), CM2, NULL},
    { D2_AREA_ELLIPSE, AREA_ELLIPSE, N_("Area-Ellipse"), CM2, NULL},
    { D2_AREA_RECTANGLE, AREA_REC, N_("Area-Rectangle"), CM2, NULL},
    { D2_VOLUME_3DIST, VOL_3AXIS, N_("Volume-3 Dist."), CM3, NULL},
    { D2_VOLUME_ELLIPSE, VOL_ELLIPSE1, N_("Volume"), CM3, NULL},
    { D2_VOLUME_ELLIPSE_1DIST, VOL_ELLIPSE2, N_("Volume-Ellipse 1 Dist."), CM3, NULL},
    { D2_VOLUME_1DIST, VOL_SPHEROID, N_("Volume-1 Dist."), D2_VOLUME_1DIST_MULTI, NULL},
    { D2_ANGLE_DOT, ANGLE_3DOT, N_("Angle-3 Point"), ANG, NULL},
    { D2_ANGLE_LINE, ANGLE_2LINE, N_("Angle"), ANG, NULL},
    { D2_DEPTH, DEPTH_DIST, N_("Depth"), CM, NULL},
    { M_TIME, TIME_M, N_("Time(M)"), SEC, NULL},
    { M_SLOPE, SLOPE, N_("Slope(M)"), CMS, NULL},
    { M_VEL, VEL_M, N_("Vel(M)"), CMS, NULL},
    { M_HR, HR_M, N_("HR(M)"), BPM, NULL},
    { M_DEPTH, DEPTH_DIST_M, N_("Dist(M)"), CM, NULL},
    { D_AUTO_TRACK, VEL_D, N_("Auto Trace(D)"), NOUNIT, NULL},
    { D_MANUAL_TRACK, MEASURE_TRACK, N_("Trace(D)"), NOUNIT, NULL},
    { D_VEL, VEL_D, N_("Velocity(D)"), CMS, NULL},
    { D_RI, RI, N_("RI(D)"), D_RI_MULTI, NULL},
    { D_SD, SD, N_("PS / ED(D)"), D_SD_MULTI, NULL},
    { D_TIME, TIME_D, N_("Time(D)"), SEC, NULL},
    { D_ACCEL, ACCEL, N_("Accel(D)"), CMS2, NULL},
    { D_PI, PI_D, N_("PI(D)"), NOUNIT, NULL},
    { D_HR, HR_D, N_("HR(D)"), D_HR_MULTI, NULL},
    { D_PGMAX, PGMAX, N_("PG max(D)"), D_PGMAX_MULTI, NULL},
    { D_PGMEAN, PGMEAN, N_("PG mean(D)"), MMHG, NULL},
    { D2_ANGUSTY_DIST, ANGUSTY_DIST_DOT, N_("Distance Angusty"), NOUNIT, NULL},
    { D2_RATIO_DIST, RATIO_DIST_DOT, N_("Distance Ratio"), NOUNIT, NULL},
    { D2_ANGUSTY_AREA, ANGUSTY_AREA, N_("Area Angusty"), NOUNIT, NULL},
    { D2_RATIO_AREA, RATIO_AREA, N_("Area Ratio"), NOUNIT, NULL},
    { D2_RATIO_D_P, RATIO_D_P, N_("Dist Peri Ratio"), NOUNIT, NULL},
    { D2_RATIO_VOL, RATIO_VOL, N_("Volume Ratio"), NOUNIT, NULL},
    { D2_RATIO_ANGLE, RATIO_ANGLE, N_("Angle Ratio"), D2_RATIO_ANGLE_MULTI, NULL},
    { D2_EF, EF, N_("EF(M)"), PERCENT, NULL},
    { D2_PROFILE, PROFILE, N_("Profile"), NOUNIT, NULL},
    { D2_HISTOGRAM, HISTOGRAM, N_("Histogram"), NOUNIT, NULL},
    { D2_IMT, MEA_TYPE_END, N_("IMT"),UNIT_END,  NULL},
    { D2_IMT_ABOVE, IMT_ABOVE, N_("IMT-Above"),UNIT_END, NULL},
    { D2_IMT_BELOW, IMT_BELOW, N_("IMT-Below"),UNIT_END, NULL},
};
#endif

/******************************Abdo Measure**************************************/
const CalcResultInfo AbdoGBVol = {ABD_GB_VOL, {ABD_GALLBLAD_L, ABD_GALLBLAD_W, ABD_GALLBLAD_H, NO_PAR, NO_PAR}, "GB Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AbdoSpleenVol = {ABD_CALC_SPLEEN_VOL, {ABD_SPLEEN_L, ABD_SPLEEN_W, ABD_SPLEEN_H, NO_PAR, NO_PAR}, "Spleen Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AbdoPancreasVol = {ABD_PANC_VOL, {ABD_PANCREAS_L, ABD_PANCREAS_W, ABD_PANCREAS_H, NO_PAR, NO_PAR}, "Pancreas Vol", CM3, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray AbdoGBVolCalc = {&AbdoGBVol, NULL};
const CalcInfoArray AbdoSpleenVolCalc = {&AbdoSpleenVol, NULL};
const CalcInfoArray AbdoPancreasVolCalc = {&AbdoPancreasVol, NULL};

const SingleItemInfo AbdoInfo[] = {
    //Abdo General
    { ABD_GALLBLAD_VOL, MEA_TYPE_END, N_("Gallbladder Vol"), UNIT_END, NULL},
    { ABD_GALLBLAD_L, DIST_DOT, N_("Gallbladder L"), CM, &AbdoGBVolCalc},
    { ABD_GALLBLAD_W, DIST_DOT, N_("Gallbladder W"), CM, &AbdoGBVolCalc},
    { ABD_GALLBLAD_H, DIST_DOT, N_("Gallbladder H"), CM, &AbdoGBVolCalc},
    { ABD_CBD_DIAM, DIST_DOT, N_("CBD Diam"), CM, NULL},
    { ABD_GB_WALL, DIST_DOT, N_("GB Wall"), CM, NULL},
    { ABD_LIVER_L, DIST_DOT, N_("Liver L"), CM, NULL},
    { ABD_SPLEEN_VOL, MEA_TYPE_END, N_("Spleen Vol"), UNIT_END, NULL},
    { ABD_SPLEEN_L, DIST_DOT, N_("Spleen L"), CM, &AbdoSpleenVolCalc},
    { ABD_SPLEEN_W, DIST_DOT, N_("Spleen W"), CM, &AbdoSpleenVolCalc},
    { ABD_SPLEEN_H, DIST_DOT, N_("Spleen H"), CM, &AbdoSpleenVolCalc},
    { ABD_PANCREAS_VOL, MEA_TYPE_END, N_("Pancreas Vol"), UNIT_END, NULL},
    { ABD_PANCREAS_L, DIST_DOT, N_("Pancreas L"), CM, &AbdoPancreasVolCalc},
    { ABD_PANCREAS_W, DIST_DOT, N_("Pancreas W"), CM, &AbdoPancreasVolCalc},
    { ABD_PANCREAS_H, DIST_DOT, N_("Pancreas H"), CM, &AbdoPancreasVolCalc},
    { ABD_AO_DIST_DIAM, DIST_DOT, N_("Ao Dist Diam"), CM, NULL},
    { ABD_AO_MID_DIAM, DIST_DOT, N_("Ao Mid Diam"), CM, NULL},
    { ABD_AO_PROX_DIAM, DIST_DOT, N_("Ao Prox Diam"), CM, NULL},
    //Abdo Arterial
    { ABD_DIST_AO, PSED, N_("Dist Ao"), ABD_DIST_AO_MULTI, NULL},
    { ABD_CELIAC_A, RI, N_("Celiac A"), ABD_CELIAC_A_MULTI, NULL},
    { ABD_COM_HEP_A, PSED, N_("Com Hep A"), ABD_COM_HEP_A_MULTI, NULL},
    { ABD_L_HEP_A, PSED, N_("L Hep A"), ABD_L_HEP_A_MULTI, NULL},
    { ABD_R_HEP_A, PSED, N_("R Hep A"), ABD_R_HEP_A_MULTI, NULL},
    { ABD_GDA, PSED, N_("GDA"), ABD_GDA_MULTI, NULL},
    { ABD_IMA, RI, N_("IMA"), ABD_IMA_MULTI, NULL},
    { ABD_SMA, RI, N_("SMA"), ABD_SMA_MULTI, NULL},
    { ABD_SPLENIC_A, PSED, N_("Splenic A"), ABD_SPLENIC_A_MULTI, NULL},
    //Abdo venous
    { ABD_PROX_IVC, PSED, N_("Prox IVC"), ABD_PROX_IVC_MULTI, NULL},
    { ABD_DIST_IVC, PSED, N_("Dist IVC"), ABD_DIST_IVC_MULTI, NULL},
    { ABD_L_HEP_V, PSED, N_("L Hep V"), ABD_L_HEP_V_MULTI, NULL},
    { ABD_M_HEP_V, PSED, N_("M Hep V"), ABD_M_HEP_V_MULTI, NULL},
    { ABD_R_HEP_V, PSED, N_("R Hep V"), ABD_R_HEP_V_MULTI, NULL},
    { ABD_L_PORT_V, PSED, N_("L Port V"), ABD_L_PORT_V_MULTI, NULL},
    { ABD_M_PORT_V, PSED, N_("M Port V"), ABD_M_PORT_V_MULTI, NULL},
    { ABD_R_PORT_V, PSED, N_("R Port V"), ABD_R_PORT_V_MULTI, NULL},
    { ABD_SPLENIC_V, PSED, N_("Splenic V"), ABD_SPLENIC_V_MULTI, NULL},
    { ABD_IMV, PSED, N_("IMV"), ABD_IMV_MULTI, NULL},
    { ABD_SMV, PSED, N_("SMV"), ABD_SMV_MULTI, NULL},
};

const PtrOfCalcInfo AbdoCalcInfoPtr[ABD_CALC_END - ABD_CALC_START] = {
    {ABD_GB_VOL, &AbdoGBVol},
    {ABD_CALC_SPLEEN_VOL, &AbdoSpleenVol},
    {ABD_PANC_VOL, &AbdoPancreasVol}
};

const MultiItemInfo AbdoMultiInfo[] = {
    {ABD_DIST_AO, ABD_DIST_AO_MULTI, PSED, "Dist Ao", PsEdInfo, NULL },
    {ABD_CELIAC_A, ABD_CELIAC_A_MULTI, RI, "Celiac A", RiInfo, NULL },
    {ABD_COM_HEP_A, ABD_COM_HEP_A_MULTI, PSED, "Com Hep A", PsEdInfo, NULL },
    {ABD_L_HEP_A, ABD_L_HEP_A_MULTI, PSED, "L Hep A", PsEdInfo, NULL },
    {ABD_R_HEP_A, ABD_R_HEP_A_MULTI, PSED, "R Hep A", PsEdInfo, NULL },
    {ABD_GDA, ABD_GDA_MULTI, PSED, "GDA", PsEdInfo, NULL },
    {ABD_IMA, ABD_IMA_MULTI, RI, "IMA", RiInfo, NULL },
    {ABD_SMA, ABD_SMA_MULTI, RI, "SMA", RiInfo, NULL },
    {ABD_SPLENIC_A, ABD_SPLENIC_A_MULTI, PSED, "Splenic A", PsEdInfo, NULL },
    {ABD_PROX_IVC, ABD_PROX_IVC_MULTI, PSED, "Prox IVC", PsEdInfo, NULL },
    {ABD_DIST_IVC, ABD_DIST_IVC_MULTI, PSED, "Dist IVC", PsEdInfo, NULL },
    {ABD_L_HEP_V, ABD_L_HEP_V_MULTI, PSED, "L Hep V", PsEdInfo, NULL },
    {ABD_M_HEP_V, ABD_M_HEP_V_MULTI, PSED, "M Hep V", PsEdInfo, NULL },
    {ABD_R_HEP_V, ABD_R_HEP_V_MULTI, PSED, "R Hep V", PsEdInfo, NULL },
    {ABD_L_PORT_V, ABD_L_PORT_V_MULTI, PSED, "L Port V", PsEdInfo, NULL },
    {ABD_M_PORT_V, ABD_M_PORT_V_MULTI, PSED, "M Port V", PsEdInfo, NULL },
    {ABD_R_PORT_V, ABD_R_PORT_V_MULTI, PSED, "R Port V", PsEdInfo, NULL },
    {ABD_SPLENIC_V, ABD_SPLENIC_V_MULTI, PSED, "Splenic V", PsEdInfo, NULL },
    {ABD_IMV, ABD_IMV_MULTI, PSED, "IMV", PsEdInfo, NULL },
    {ABD_SMV, ABD_SMV_MULTI, PSED, "SMV", PsEdInfo, NULL },
};

/******************************Adult Measure**************************************/
const CalcResultInfo AdultCI2DCubed = {ADULT_CI_2DCUBED, {ADULT_CO_2DCUBED, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(Cubed)", LMINM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultCI2DTeich = {ADULT_CI_2DTEICH, {ADULT_CO_2DTEICH, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(Teich)", LMINM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultCIMMCubed = {ADULT_CI_MMCUBED, {ADULT_CO_MMCUBED, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(Cubed)", LMINM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultCIMMTeich = {ADULT_CI_MMTEICH, {ADULT_CO_MMTEICH, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(Teich)", LMINM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultCIA2C = {ADULT_CI_A2C, {ADULT_CO_A2C, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(A2C)", LMINM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultCIA4C = {ADULT_CI_A4C, {ADULT_CO_A4C, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(A4C)", LMINM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultCIAL = {ADULT_CI_AL, {ADULT_CO_AL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "CI(A/L)", LMINM2, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray AdultCO2DCubedCalc = {&AdultCI2DCubed, NULL};
const CalcInfoArray AdultCO2DTeichCalc = {&AdultCI2DTeich, NULL};
const CalcInfoArray AdultCOMMCubedCalc = {&AdultCIMMCubed, NULL};
const CalcInfoArray AdultCOMMTeichCalc = {&AdultCIMMTeich, NULL};
const CalcInfoArray AdultCOA2CCalc = {&AdultCIA2C, NULL};
const CalcInfoArray AdultCOA4CCalc = {&AdultCIA4C, NULL};
const CalcInfoArray AdultCOALCalc = {&AdultCIAL, NULL};

const CalcResultInfo AdultSI2DCubed = {ADULT_SI_2DCUBED, {ADULT_SV_2DCUBED, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(Cubed)", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultSI2DTeich = {ADULT_SI_2DTEICH, {ADULT_SV_2DTEICH, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(Teich)", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultSIMMCubed = {ADULT_SI_MMCUBED, {ADULT_SV_MMCUBED, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(Cubed)", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultSIMMTeich = {ADULT_SI_MMTEICH, {ADULT_SV_MMTEICH, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(Teich)", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultSIA2C = {ADULT_SI_A2C, {ADULT_SV_A2C, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(A2C)", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultSIA4C = {ADULT_SI_A4C, {ADULT_SV_A4C, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(A4C)", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultSIAL = {ADULT_SI_AL, {ADULT_SV_AL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "SI(A/L)", NOUNIT, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo AdultCO2DCubed = {ADULT_CO_2DCUBED, {ADULT_SV_2DCUBED, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(Cubed)", LMIN, MeaCalcFun::SectionCalc, &AdultCO2DCubedCalc};
const CalcResultInfo AdultCO2DTeich = {ADULT_CO_2DTEICH, {ADULT_SV_2DTEICH, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(Teich)", LMIN, MeaCalcFun::SectionCalc, &AdultCO2DTeichCalc};
const CalcResultInfo AdultCOMMCubed = {ADULT_CO_MMCUBED, {ADULT_SV_MMCUBED, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(Cubed)", LMIN, MeaCalcFun::SectionCalc, &AdultCOMMCubedCalc};
const CalcResultInfo AdultCOMMTeich = {ADULT_CO_MMTEICH, {ADULT_SV_MMTEICH, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(Teich)", LMIN, MeaCalcFun::SectionCalc, &AdultCOMMTeichCalc};
const CalcResultInfo AdultCOA2C = {ADULT_CO_A2C, {ADULT_SV_A2C, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(A2C)", LMIN, MeaCalcFun::SectionCalc, &AdultCOA2CCalc};
const CalcResultInfo AdultCOA4C = {ADULT_CO_A4C, {ADULT_SV_A4C, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(A4C)", LMIN, MeaCalcFun::SectionCalc, &AdultCOA4CCalc};
const CalcResultInfo AdultCOAL = {ADULT_CO_AL, {ADULT_SV_AL, ADULT_HR_LV, NO_PAR, NO_PAR, NO_PAR}, "CO(A/L)", LMIN, MeaCalcFun::SectionCalc, &AdultCOALCalc};

const CalcInfoArray AdultSV2DCubedCalc = {&AdultSI2DCubed, &AdultCO2DCubed, NULL};
const CalcInfoArray AdultSV2DTeichCalc = {&AdultSI2DTeich, &AdultCO2DTeich, NULL};
const CalcInfoArray AdultSVMMCubedCalc = {&AdultSIMMCubed, &AdultCOMMCubed, NULL};
const CalcInfoArray AdultSVMMTeichCalc = {&AdultSIMMTeich, &AdultCOMMTeich, NULL};
const CalcInfoArray AdultSVA2CCalc = {&AdultSIA2C, &AdultCOA2C, NULL};
const CalcInfoArray AdultSVA4CCalc = {&AdultSIA4C, &AdultCOA4C, NULL};
const CalcInfoArray AdultSVALCalc = {&AdultSIAL, &AdultCOAL, NULL};

//const CalcResultInfo AdultEF = {ADULT_EF, {ADULT_EDV, ADULT_ESV, NO_PAR, NO_PAR, NO_PAR}, "EF", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEF2DCubed = {ADULT_EF_2DCUBED, {ADULT_EDV_2DCUBED, ADULT_ESV_2DCUBED, NO_PAR, NO_PAR, NO_PAR}, "EF(Cubed)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEF2DTeich = {ADULT_EF_2DTEICH, {ADULT_EDV_2DTEICH, ADULT_ESV_2DTEICH, NO_PAR, NO_PAR, NO_PAR}, "EF(Teich)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEFMMCubed = {ADULT_EF_MMCUBED, {ADULT_EDV_MMCUBED, ADULT_ESV_MMCUBED, NO_PAR, NO_PAR, NO_PAR}, "EF(Cubed)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEFMMTeich = {ADULT_EF_MMTEICH, {ADULT_EDV_MMTEICH, ADULT_ESV_MMTEICH, NO_PAR, NO_PAR, NO_PAR}, "EF(Teich)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEFA2C = {ADULT_EF_A2C, {ADULT_A2CD, ADULT_A2CS, NO_PAR, NO_PAR, NO_PAR}, "EF(A2C)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEFA4C = {ADULT_EF_A4C, {ADULT_A4CD, ADULT_A4CS, NO_PAR, NO_PAR, NO_PAR}, "EF(A4C)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultEFAL = {ADULT_EF_AL, {ADULT_LVDAL, ADULT_LVSAL, NO_PAR, NO_PAR, NO_PAR}, "EF(AL)", PERCENT, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo AdultSV2DCubed = {ADULT_SV_2DCUBED, {ADULT_EDV_2DCUBED, ADULT_ESV_2DCUBED, NO_PAR, NO_PAR, NO_PAR}, "SV(Cubed)", CM3, MeaCalcFun::SectionCalc, &AdultSV2DCubedCalc};
const CalcResultInfo AdultSV2DTeich = {ADULT_SV_2DTEICH, {ADULT_EDV_2DTEICH, ADULT_ESV_2DTEICH, NO_PAR, NO_PAR, NO_PAR}, "SV(Teich)", CM3, MeaCalcFun::SectionCalc, &AdultSV2DTeichCalc};
const CalcResultInfo AdultSVMMCubed = {ADULT_SV_MMCUBED, {ADULT_EDV_MMCUBED, ADULT_ESV_MMCUBED, NO_PAR, NO_PAR, NO_PAR}, "SV(Cubed)", CM3, MeaCalcFun::SectionCalc, &AdultSVMMCubedCalc};
const CalcResultInfo AdultSVMMTeich = {ADULT_SV_MMTEICH, {ADULT_EDV_MMTEICH, ADULT_ESV_MMTEICH, NO_PAR, NO_PAR, NO_PAR}, "SV(Teich)", CM3, MeaCalcFun::SectionCalc, &AdultSVMMTeichCalc};
const CalcResultInfo AdultSVA2C = {ADULT_SV_A2C, {ADULT_A2CD, ADULT_A2CS, NO_PAR, NO_PAR, NO_PAR}, "SV(A2C)", CM3, MeaCalcFun::SectionCalc, &AdultSVA2CCalc};
const CalcResultInfo AdultSVA4C = {ADULT_SV_A4C, {ADULT_A4CD, ADULT_A4CS, NO_PAR, NO_PAR, NO_PAR}, "SV(A4C)", CM3, MeaCalcFun::SectionCalc, &AdultSVA4CCalc};
const CalcResultInfo AdultSVAL = {ADULT_SV_AL, {ADULT_LVDAL, ADULT_LVSAL, NO_PAR, NO_PAR, NO_PAR}, "SV(AL)", CM3, MeaCalcFun::SectionCalc, &AdultSVALCalc};

const CalcInfoArray AdultEDV2DCubedCalc = {&AdultSV2DCubed, &AdultEF2DCubed, NULL};
const CalcInfoArray AdultEDV2DTeichCalc = {&AdultSV2DTeich, &AdultEF2DTeich, NULL};
const CalcInfoArray AdultEDVMMCubedCalc = {&AdultSVMMCubed, &AdultEFMMCubed, NULL};
const CalcInfoArray AdultEDVMMTeichCalc = {&AdultSVMMTeich, &AdultEFMMTeich, NULL};

const CalcInfoArray AdultESV2DCubedCalc = {&AdultSV2DCubed, &AdultEF2DCubed, NULL};
const CalcInfoArray AdultESV2DTeichCalc = {&AdultSV2DTeich, &AdultEF2DTeich, NULL};
const CalcInfoArray AdultESVMMCubedCalc = {&AdultSVMMCubed, &AdultEFMMCubed, NULL};
const CalcInfoArray AdultESVMMTeichCalc = {&AdultSVMMTeich, &AdultEFMMTeich, NULL};

const CalcResultInfo AdultEDV2DCubed = {ADULT_EDV_2DCUBED, {ADULT_LVIDD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDV(Cubed)", CM3, MeaCalcFun::SectionCalc, &AdultEDV2DCubedCalc};
const CalcResultInfo AdultEDV2DTeich = {ADULT_EDV_2DTEICH, {ADULT_LVIDD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDV(Teich)", CM3, MeaCalcFun::SectionCalc, &AdultEDV2DTeichCalc};
const CalcResultInfo AdultEDVMMCubed = {ADULT_EDV_MMCUBED, {ADULT_LVIDD_MM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDV(Cubed)", CM3, MeaCalcFun::SectionCalc, &AdultEDVMMCubedCalc};
const CalcResultInfo AdultEDVMMTeich = {ADULT_EDV_MMTEICH, {ADULT_LVIDD_MM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDV(Teich)", CM3, MeaCalcFun::SectionCalc, &AdultEDVMMTeichCalc};

const CalcResultInfo AdultESV2DCubed = {ADULT_ESV_2DCUBED, {ADULT_LVIDS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "ESV(Cubed)", CM3, MeaCalcFun::SectionCalc, &AdultESV2DCubedCalc};
const CalcResultInfo AdultESV2DTeich = {ADULT_ESV_2DTEICH, {ADULT_LVIDS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "ESV(Teich)", CM3, MeaCalcFun::SectionCalc, AdultESV2DTeichCalc};
const CalcResultInfo AdultESVMMCubed = {ADULT_ESV_MMCUBED, {ADULT_LVIDS_MM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "ESV(Cubed)", CM3, MeaCalcFun::SectionCalc, AdultESVMMCubedCalc};
const CalcResultInfo AdultESVMMTeich = {ADULT_ESV_MMTEICH, {ADULT_LVIDS_MM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "ESV(Teich)", CM3, MeaCalcFun::SectionCalc, AdultESVMMTeichCalc};

const CalcResultInfo AdultMVAP12t = {ADULT_MVA_P12T, {ADULT_MV_P12T, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "MVA", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultAVAVmax = {ADULT_AVA_VMAX, {ADULT_LVOT_AREA, ADULT_LVOT_VMAX, ADULT_AV_VMAX, NO_PAR, NO_PAR}, "AVA", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultMVEA = {ADULT_MV_EA_RATIO, {ADULT_MV_PEAK_E_V, ADULT_MV_PEAK_A_V, NO_PAR, NO_PAR, NO_PAR }, "MV E/A", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultPulmSD = {ADULT_PULM_SD_RATIO, {ADULT_PULM_SYS_V, ADULT_PULM_DIAS_V, NO_PAR, NO_PAR, NO_PAR }, "Pulm S/D", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultPVAVmax = {ADULT_PVA_VMAX, {ADULT_RVOT_AREA, ADULT_RVOT_VMAX, ADULT_PV_VMAX, NO_PAR, NO_PAR}, "PVA", CM2, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo AdultFS = {ADULT_FS, {ADULT_LVIDD, ADULT_LVIDS, NO_PAR, NO_PAR, NO_PAR}, "FS", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultFSMM = {ADULT_FS_MM, {ADULT_LVIDD_MM, ADULT_LVIDS_MM, NO_PAR, NO_PAR, NO_PAR}, "FS", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultIVSPer = {ADULT_IVS_PER, {ADULT_IVSD, ADULT_IVSS, NO_PAR, NO_PAR, NO_PAR}, "IVS%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultIVSPerMM = {ADULT_IVS_PER_MM, {ADULT_IVSD_MM, ADULT_IVSS_MM, NO_PAR, NO_PAR, NO_PAR}, "IVS%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultIVSLVPW = {ADULT_IVS_LVPW_RATIO, {ADULT_IVSD, ADULT_LVPWD, NO_PAR, NO_PAR, NO_PAR}, "IVS/LVPW", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultIVSLVPWMM = {ADULT_IVS_LVPW_RATIO_MM, {ADULT_IVSD_MM, ADULT_LVPWD_MM, NO_PAR, NO_PAR, NO_PAR}, "IVS/LVPW", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultLAAO = {ADULT_LA_AO_RATIO, {ADULT_LA_DIMEN, ADULT_AOR_DIAM, NO_PAR, NO_PAR, NO_PAR}, "LA/Ao", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultLVMassIndexCubed = {ADULT_LVMASS_INDEX, {ADULT_LVMASS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "LVMWI", NOUNIT, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray AdultLVMassCalc = {&AdultLVMassIndexCubed, NULL};
const CalcInfoArray AdultLVOTAreaCalc = {&AdultAVAVmax, NULL};
const CalcInfoArray AdultRVOTAreaCalc = {&AdultPVAVmax, NULL};
const CalcResultInfo AdultLVMass = {ADULT_LVMASS, {ADULT_IVSD_MM, ADULT_LVIDD_MM, ADULT_LVPWD_MM, NO_PAR, NO_PAR}, "LVMW", NOUNIT, MeaCalcFun::SectionCalc, &AdultLVMassCalc};
const CalcResultInfo AdultLVOTArea = {ADULT_LVOT_AREA, {ADULT_LVOT_DIAM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "LVOT Area", CM2, MeaCalcFun::SectionCalc, &AdultLVOTAreaCalc};
const CalcResultInfo AdultRVOTArea = {ADULT_RVOT_AREA, {ADULT_RVOT_DIAM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "RVOT Area", CM2, MeaCalcFun::SectionCalc, &AdultRVOTAreaCalc};
const CalcResultInfo AdultTVArea = {ADULT_TV_AREA, {ADULT_TV_DIAM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "TV Area", CM2, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo AdultAIPISA = {ADULT_AI_PISA, {ADULT_AI_RADIUS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "AI PISA", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultMVPISA = {ADULT_MV_PISA, {ADULT_MV_RADIUS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "MV PISA", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultMRPISA = {ADULT_MR_PISA, {ADULT_MR_RADIUS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "MR PISA", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultTVPISA = {ADULT_TV_PISA, {ADULT_TV_RADIUS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "TV PISA", CM2, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo AdultTVEA = {ADULT_TV_EA_RATIO, {ADULT_TV_PEAK_E_V, ADULT_TV_PEAK_A_V, NO_PAR, NO_PAR, NO_PAR}, "TV E/A", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultLVPWPer = {ADULT_LVPW_PER, {ADULT_LVPWS, ADULT_LVPWD, NO_PAR, NO_PAR, NO_PAR}, "LVPW%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultLVPWPerMM = {ADULT_LVPW_PER_MM, {ADULT_LVPWS_MM, ADULT_LVPWD_MM, NO_PAR, NO_PAR, NO_PAR}, "LVPW%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultLAAOMM = {ADULT_LA_AO_RATIO_MM, {ADULT_LA_DIMEN_MM, ADULT_AOR_DIAM_MM, NO_PAR, NO_PAR, NO_PAR}, "LA/Ao", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultLVPEPET = {ADULT_LV_PEP_ET_RATIO, {ADULT_LV_PEP, ADULT_LV_ET, NO_PAR, NO_PAR, NO_PAR}, "LV PEP/ET", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultRVPEPET = {ADULT_RV_PEP_ET_RATIO, {ADULT_RV_PEP, ADULT_RV_ET, NO_PAR, NO_PAR,NO_PAR }, "RV PEP/ET", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo AdultTeiIndex = {ADULT_TEI_INDEX, {ADULT_IVRT, ADULT_IVCT, ADULT_LV_ET, NO_PAR,NO_PAR }, "Tei Index", NOUNIT, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray AdultLVLDApicalCalc = {NULL};
const CalcInfoArray AdultLVIDsCalc = {&AdultESV2DCubed, &AdultESV2DTeich, &AdultFS, NULL};
const CalcInfoArray AdultLVPWsCalc = {&AdultLVPWPer, NULL};
const CalcInfoArray AdultLVOTDiamCalc = {&AdultLVOTArea, NULL};
const CalcInfoArray AdultLVIDdCalc = {&AdultEDV2DCubed, &AdultEDV2DTeich, &AdultFS, NULL};//再查，应该有第4项
const CalcInfoArray AdultLVPWdCalc = {&AdultIVSLVPW, &AdultLVPWPer, NULL};
const CalcInfoArray AdultRVOTDiamCalc = {&AdultRVOTArea, NULL};
const CalcInfoArray AdultIVSdCalc = {&AdultIVSPer, &AdultIVSLVPW, NULL};
//const CalcInfoArray AdultIVSdCalc = {&AdultIVSLVPW, &AdultIVSPer, NULL};
const CalcInfoArray AdultIVSsCalc = {&AdultIVSPer, NULL};
const CalcInfoArray AdultLADimenCalc = {&AdultLAAO, NULL};
const CalcInfoArray AdultAoRDiamCalc = {&AdultLAAO, NULL};
const CalcInfoArray AdultIVSdMMCalc = {&AdultIVSPerMM, &AdultIVSLVPWMM, &AdultLVMass, NULL};
const CalcInfoArray AdultLVIDdMMCalc = {&AdultEDVMMCubed, &AdultEDVMMTeich, &AdultFSMM, &AdultLVMass, NULL};//再查，应该有第4项
const CalcInfoArray AdultLVPWdMMCalc = {&AdultIVSLVPWMM, &AdultLVPWPerMM, &AdultLVMass, NULL};
const CalcInfoArray AdultIVSsMMCalc = {&AdultIVSPerMM, NULL};
const CalcInfoArray AdultLVIDsMMCalc = {&AdultESVMMCubed, &AdultESVMMTeich, &AdultFSMM, NULL};
const CalcInfoArray AdultLVPWsMMCalc = {&AdultLVPWPerMM, NULL};
const CalcInfoArray AdultAoRDiamMMCalc = {&AdultLAAOMM, NULL};
const CalcInfoArray AdultLADiamMMCalc = {&AdultLAAOMM, NULL};
const CalcInfoArray AdultLVETCalc = {&AdultLVPEPET, &AdultTeiIndex, NULL};
const CalcInfoArray AdultLVPEPCalc = {&AdultLVPEPET, NULL};
const CalcInfoArray AdultRVETCalc = {&AdultRVPEPET, NULL};
const CalcInfoArray AdultRVPEPCalc = {&AdultRVPEPET, NULL};
const CalcInfoArray AdultLvotVmaxCalc = {&AdultAVAVmax, NULL};
const CalcInfoArray AdultMVP12tCalc = {&AdultMVAP12t, NULL};
const CalcInfoArray AdultAvVmaxCalc = {&AdultAVAVmax, NULL};
const CalcInfoArray AdultMVPeakECalc = {&AdultMVEA, NULL};
const CalcInfoArray AdultMVPeakACalc = {&AdultMVEA, NULL};
const CalcInfoArray AdultTVDiamCalc = {&AdultTVArea, NULL};

const CalcInfoArray AdultAIRadiusCalc = {&AdultAIPISA, NULL};
const CalcInfoArray AdultMVRadiusCalc = {&AdultMVPISA, NULL};
const CalcInfoArray AdultMRRadiusCalc = {&AdultMRPISA, NULL};
const CalcInfoArray AdultTVRadiusCalc = {&AdultTVPISA, NULL};

const CalcInfoArray AdultTVPeakECalc = {&AdultTVEA, NULL};
const CalcInfoArray AdultTVPeakACalc = {&AdultTVEA, NULL};
const CalcInfoArray AdultPulmDiasCalc = {&AdultPulmSD, NULL};
const CalcInfoArray AdultPulmSysCalc = {&AdultPulmSD, NULL};
const CalcInfoArray AdultRvotVmaxCalc = {&AdultPVAVmax, NULL};
const CalcInfoArray AdultPvVmaxCalc = {&AdultPVAVmax, NULL};
const CalcInfoArray AdultIVRTCalc = {&AdultTeiIndex, NULL};
const CalcInfoArray AdultIVCTCalc = {&AdultTeiIndex, NULL};

const CalcInfoArray AdultA2CdCalc = {&AdultSVA2C, &AdultEFA2C, NULL};
const CalcInfoArray AdultA4CdCalc = {&AdultSVA4C, &AdultEFA4C, NULL};
const CalcInfoArray AdultA2CsCalc = {&AdultSVA2C, &AdultEFA2C, NULL};
const CalcInfoArray AdultA4CsCalc = {&AdultSVA4C, &AdultEFA4C, NULL};
const CalcInfoArray AdultLVdALCalc = {&AdultSVAL, &AdultEFAL, NULL};
const CalcInfoArray AdultLVsALCalc = {&AdultSVAL, &AdultEFAL, NULL};

const PtrOfCalcInfo AdultCalcInfoPtr[ADULT_CALC_END - ADULT_CALC_START] = {
    {ADULT_EDV_2DCUBED, &AdultEDV2DCubed},
    {ADULT_EDV_2DTEICH, &AdultEDV2DTeich},
    {ADULT_EDV_MMCUBED, &AdultEDVMMCubed},
    {ADULT_EDV_MMTEICH, &AdultEDVMMTeich},
    {ADULT_ESV_2DCUBED, &AdultESV2DCubed},
    {ADULT_ESV_2DTEICH, &AdultESV2DTeich},
    {ADULT_ESV_MMCUBED, &AdultESVMMCubed},
    {ADULT_ESV_MMTEICH, &AdultESVMMTeich},
    {ADULT_SV_2DCUBED, &AdultSV2DCubed},
    {ADULT_SV_2DTEICH, &AdultSV2DTeich},
    {ADULT_SV_MMCUBED, &AdultSVMMCubed},
    {ADULT_SV_MMTEICH, &AdultSVMMTeich},
    {ADULT_SV_A2C, &AdultSVA2C},
    {ADULT_SV_A4C, &AdultSVA4C},
    {ADULT_SV_AL, &AdultSVAL},
    {ADULT_SI_2DCUBED, &AdultSI2DCubed},
    {ADULT_SI_2DTEICH, &AdultSI2DTeich},
    {ADULT_SI_MMCUBED, &AdultSIMMCubed},
    {ADULT_SI_MMTEICH, &AdultSIMMTeich},
    {ADULT_SI_A2C, &AdultSIA2C},
    {ADULT_SI_A4C, &AdultSIA4C},
    {ADULT_SI_AL, &AdultSIAL},
    {ADULT_CO_2DCUBED, &AdultCO2DCubed},
    {ADULT_CO_2DTEICH, &AdultCO2DTeich},
    {ADULT_CO_MMCUBED, &AdultCOMMCubed},
    {ADULT_CO_MMTEICH, &AdultCOMMTeich},
    {ADULT_CO_A2C, &AdultCOA2C},
    {ADULT_CO_A4C, &AdultCOA4C},
    {ADULT_CO_AL, &AdultCOAL},
    {ADULT_CI_2DCUBED, &AdultCI2DCubed},
    {ADULT_CI_2DTEICH, &AdultCI2DTeich},
    {ADULT_CI_MMCUBED, &AdultCIMMCubed},
    {ADULT_CI_MMTEICH, &AdultCIMMTeich},
    {ADULT_CI_A2C, &AdultCIA2C},
    {ADULT_CI_A4C, &AdultCIA4C},
    {ADULT_CI_AL, &AdultCIAL},
    {ADULT_EF_2DCUBED, &AdultEF2DCubed},
    {ADULT_EF_2DTEICH, &AdultEF2DTeich},
    {ADULT_EF_MMCUBED, &AdultEFMMCubed},
    {ADULT_EF_MMTEICH, &AdultEFMMTeich},
    {ADULT_EF_A2C, &AdultEFA2C},
    {ADULT_EF_A4C, &AdultEFA4C},
    {ADULT_EF_AL, &AdultEFAL},
    {ADULT_FS, &AdultFS},
    {ADULT_FS_MM, &AdultFSMM},
    {ADULT_IVS_PER, &AdultIVSPer},
    {ADULT_IVS_PER_MM, &AdultIVSPerMM},
    {ADULT_IVS_LVPW_RATIO, &AdultIVSLVPW},
    {ADULT_IVS_LVPW_RATIO_MM, &AdultIVSLVPWMM},
    {ADULT_LA_AO_RATIO, &AdultLAAO},
    {ADULT_LA_AO_RATIO_MM, &AdultLAAOMM},
    {ADULT_LVMASS, &AdultLVMass},
    {ADULT_LVMASS_INDEX, &AdultLVMassIndexCubed},
    {ADULT_LVOT_AREA, &AdultLVOTArea},
    {ADULT_RVOT_AREA, &AdultRVOTArea},
    {ADULT_TV_AREA, &AdultTVArea},
    {ADULT_AI_PISA, &AdultAIPISA},
    {ADULT_MV_PISA, &AdultMVPISA},
    {ADULT_MR_PISA, &AdultMRPISA},
    {ADULT_TV_PISA, &AdultTVPISA},
    {ADULT_AVA_VMAX, &AdultAVAVmax},
    {ADULT_PVA_VMAX, &AdultPVAVmax},
    {ADULT_MVA_P12T, &AdultMVAP12t},
    {ADULT_MV_EA_RATIO, &AdultMVEA},
    {ADULT_TV_EA_RATIO, &AdultTVEA},
    {ADULT_LVPW_PER, &AdultLVPWPer},
    {ADULT_LVPW_PER_MM, &AdultLVPWPerMM},
    {ADULT_LV_PEP_ET_RATIO, &AdultLVPEPET},
    {ADULT_RV_PEP_ET_RATIO, &AdultRVPEPET},
    {ADULT_PULM_SD_RATIO, &AdultPulmSD},
    {ADULT_TEI_INDEX, &AdultTeiIndex},
};

const MultiItemInfo AdultMultiInfo[] = {
    { ADULT_A2CD, ADULT_A2CD_MULTI, SIMPSONS, "A2Cd", SimpsonsEdvInfo, &AdultA2CdCalc},
    { ADULT_A2CS, ADULT_A2CS_MULTI, SIMPSONS, "A2Cs", SimpsonsEsvInfo, &AdultA2CdCalc},
    { ADULT_A4CD, ADULT_A4CD_MULTI, SIMPSONS, "A4Cd", SimpsonsEdvInfo, &AdultA4CdCalc},
    { ADULT_A4CS, ADULT_A4CS_MULTI, SIMPSONS, "A4Cs", SimpsonsEsvInfo, &AdultA4CsCalc},
    { ADULT_LVDAL, ADULT_LVDAL_MULTI, AL, "LVd(A/L)", AlEdvInfo, &AdultLVdALCalc},
    { ADULT_LVSAL, ADULT_LVSAL_MULTI, AL, "LVs(A/L)", AlEsvInfo, &AdultLVsALCalc},
//  { ADULT_LVDAL, ADULT_LVDAL_MULTI, DIST_SLOPE_M, "LVDAL", {{"Dist", CM}, {"Slope", CMS}, {"\0", NOUNIT}, {"\0", NOUNIT}}, NULL },
//  { ADULT_LVSAL, ADULT_LVSAL_MULTI, DIST_SLOPE_M, "LVSAL", {{"Dist", CM}, {"Slope", CMS}, {"\0", NOUNIT}, {"\0", NOUNIT}}, NULL },
    { ADULT_MV_DE_EXC, ADULT_MV_DE_EXC_MULTI, DIST_SLOPE_M, "MV D-E Exc", SlopeMInfo, NULL },
    { ADULT_TV_DE_EXC, ADULT_TV_DE_EXC_MULTI, DIST_SLOPE_M, "TV D-E Exc", SlopeMInfo, NULL },
    { ADULT_AV_VMAX, ADULT_AV_VMAX_MULTI, PGMAX, "AV Vmax", PgmaxInfo, &AdultAvVmaxCalc },
    { ADULT_LVOT_VMAX, ADULT_LVOT_VMAX_MULTI, PGMAX, "LVOT Vmax", PgmaxInfo, &AdultLvotVmaxCalc },
    { ADULT_AI_P12T, ADULT_AI_P12T_MULTI, P12T, "AI P1/2t", P12tInfo, NULL },
    { ADULT_AI_VMAX, ADULT_AI_VMAX_MULTI, PGMAX, "AI Vmax", PgmaxInfo, NULL },
    { ADULT_AI_END_DIAS_V, ADULT_AI_END_DIAS_V_MULTI, PGMAX, "AI End Dias V", PgmaxInfo, NULL },
    { ADULT_HR_AV, ADULT_HR_AV_MULTI, HR_D, "HR-AV", HrInfo, NULL },
    { ADULT_HR_MV, ADULT_HR_MV_MULTI, HR_D, "HR-MV", HrInfo, NULL },
    { ADULT_MV_P12T, ADULT_MV_P12T_MULTI, P12T, "MV P1/2t", P12tInfo, &AdultMVP12tCalc},
    { ADULT_MV_PEAK_E_V, ADULT_MV_PEAK_E_V_MULTI, PGMAX, "MV Peak E", PgmaxInfo, &AdultMVPeakECalc },
    { ADULT_MV_PEAK_A_V, ADULT_MV_PEAK_A_V_MULTI, PGMAX, "MV Peak A", PgmaxInfo, &AdultMVPeakACalc },
    { ADULT_HR_TV, ADULT_HR_TV_MULTI, HR_D, "HR-TV", HrInfo, NULL },
    { ADULT_TV_PEAK_E_V, ADULT_TV_PEAK_E_V_MULTI, PGMAX, "TV Peak E", PgmaxInfo, &AdultTVPeakECalc },
    { ADULT_TV_PEAK_A_V, ADULT_TV_PEAK_A_V_MULTI, PGMAX, "TV Peak A", PgmaxInfo, &AdultTVPeakACalc },
    { ADULT_HR_PV, ADULT_HR_PV_MULTI, HR_D, "HR-PV", HrInfo, NULL },
    { ADULT_PV_VMAX, ADULT_PV_VMAX_MULTI, PGMAX, "PV Vmax", PgmaxInfo, &AdultPvVmaxCalc },
    { ADULT_PI_END_DIAS_V, ADULT_PI_END_DIAS_V_MULTI, PGMAX, "PI End Dias Vel", PgmaxInfo, NULL},
    { ADULT_PULM_DIAS_V, ADULT_PULM_DIAS_V_MULTI, PGMAX, "Pulm Dias Vel", PgmaxInfo, &AdultPulmDiasCalc},
    { ADULT_PULM_SYS_V, ADULT_PULM_SYS_V_MULTI, PGMAX, "Pulm Sys Vel", PgmaxInfo,  &AdultPulmSysCalc},
    { ADULT_RVOT_VMAX, ADULT_RVOT_VMAX_MULTI, PGMAX, "RVOT Vmax", PgmaxInfo, &AdultRvotVmaxCalc },
};

const SingleItemInfo AdultInfo[] = {
    //2D Dimensions
    {ADULT_LVLD_APICAL, DIST_DOT, N_("LVLd Apical(2D)"), CM, &AdultLVLDApicalCalc},
    {ADULT_LVIDS, DIST_DOT, N_("LVIDs(2D)"), CM, &AdultLVIDsCalc},
    {ADULT_LVPWS, DIST_DOT, N_("LVPWs(2D)"), CM, &AdultLVPWsCalc},
    {ADULT_LVOT_DIAM, DIST_DOT, N_("LVOT Diam(2D)"), CM, &AdultLVOTDiamCalc},
    {ADULT_LVIDD, DIST_DOT, N_("LVIDd(2D)"), CM, &AdultLVIDdCalc},
    {ADULT_LVPWD, DIST_DOT, N_("LVPWd(2D)"), CM, &AdultLVPWdCalc},
    {ADULT_LVAD_ENDO, AREA_TRACK, N_("LVAd SAx Endo(2D)"), CM2, NULL},
    {ADULT_LVAD_EPI, AREA_TRACK, N_("LVAd SAx Epi(2D)"), CM2, NULL},
    {ADULT_RVIDD, DIST_DOT, N_("RVIDd(2D)"), CM, NULL},
    {ADULT_RVAWD, DIST_DOT, N_("RVAWd(2D)"), CM, NULL},
    {ADULT_RVOT_DIAM, DIST_DOT, N_("RVOT Diam(2D)"), CM, &AdultRVOTDiamCalc},
    {ADULT_IVSD, DIST_DOT, N_("IVSd(2D)"), CM, &AdultIVSdCalc},
    {ADULT_IVSS, DIST_DOT, N_("IVSs(2D)"), CM, &AdultIVSsCalc},
    {ADULT_LA_DIMEN, DIST_DOT, N_("LA Dimen(2D)"), CM, &AdultLADimenCalc},
    //2D Vessels
    {ADULT_AOR_DIAM, DIST_DOT, N_("AoR Diam(2D)"), CM, &AdultAoRDiamCalc},
    {ADULT_ISTHMUS_DIAM, DIST_DOT, N_("Ao Isthmus Diam(2D)"), CM, NULL},
    {ADULT_ASC_AO_DIAM, DIST_DOT, N_("Asc Ao Diam(2D)"), CM, NULL},
    {ADULT_DESC_AO_DIAM, DIST_DOT, N_("Desc Ao Diam(2D)"), CM, NULL},
    {ADULT_MPA_DIAM, DIST_DOT, N_("MPA Diam(2D)"), CM, NULL},
    {ADULT_LPA_DIAM, DIST_DOT, N_("LPA Diam(2D)"), CM, NULL},
    {ADULT_RPA_DIAM, DIST_DOT, N_("RPA Diam(2D) "), CM, NULL},
    //2D EF & Volume
    {ADULT_A2CD, DIST_DOT, N_("A2Cd(2D)"), ADULT_A2CD_MULTI, &AdultA2CdCalc},
    {ADULT_A2CS, DIST_DOT, N_("A2Cs(2D)"), ADULT_A2CS_MULTI, &AdultA2CsCalc},
    {ADULT_A4CD, DIST_DOT, N_("A4Cd(2D)"), ADULT_A4CD_MULTI, &AdultA4CdCalc},
    {ADULT_A4CS, DIST_DOT, N_("A4Cs(2D)"), ADULT_A4CS_MULTI, &AdultA4CdCalc},
    {ADULT_LVDAL, DIST_DOT, N_("LVd(A/L)(2D)"), ADULT_LVDAL_MULTI, &AdultLVdALCalc},
    {ADULT_LVSAL, DIST_DOT, N_("LVs(A/L)(2D)"), ADULT_LVSAL_MULTI, &AdultLVdALCalc},
    //2D Valves
    {ADULT_AV_AREA, AREA_TRACK, N_("AV Area(2D)"), CM2, NULL},
    {ADULT_AI_RADIUS, DIST_DOT, N_("AI Radius(2D)"), CM, &AdultAIRadiusCalc},
    {ADULT_MV_DIAM, DIST_DOT, N_("MV Diam(2D)"), CM, NULL},
    {ADULT_MV_RADIUS, DIST_DOT, N_("MV Radius(2D)"), CM, &AdultMVRadiusCalc},
    {ADULT_MV_AREA, AREA_TRACK, N_("MV Area(Planim)(2D)"), CM2, NULL},
    {ADULT_MR_RADIUS, DIST_DOT, N_("MR Radius(2D)"), CM, &AdultMRRadiusCalc},
    {ADULT_TV_DIAM, DIST_DOT, N_("TV Diam(2D)"), CM, &AdultTVDiamCalc},
    {ADULT_TV_RADIUS, DIST_DOT, N_("TV Radius(2D)"), CM, &AdultTVRadiusCalc},
    //M Dimensions
    {ADULT_RVAWD_MM, DEPTH_DIST_M, N_("RVAWd(M)"), CM, NULL},
    {ADULT_RVIDD_MM, DEPTH_DIST_M, N_("RVIDd(M)"), CM, NULL},
    {ADULT_IVSD_MM, DEPTH_DIST_M, N_("IVSd(M)"), CM, &AdultIVSdMMCalc},
    {ADULT_LVIDD_MM, DEPTH_DIST_M, N_("LVIDd(M)"), CM, &AdultLVIDdMMCalc},
    {ADULT_LVPWD_MM, DEPTH_DIST_M, N_("LVPWd(M)"), CM, &AdultLVPWdMMCalc},
    {ADULT_IVSS_MM, DEPTH_DIST_M, N_("IVSs(M)"), CM, &AdultIVSsMMCalc},
    {ADULT_LVIDS_MM, DEPTH_DIST_M, N_("LVIDs(M)"), CM, &AdultLVIDsMMCalc},
    {ADULT_LVPWS_MM, DEPTH_DIST_M, N_("LVPWs(M)"), CM, &AdultLVPWsMMCalc},
    {ADULT_HR_LV, HR_M, N_("HR-LV(M)"), BPM, NULL},
    {ADULT_LVOT_DIAM_MM, DEPTH_DIST_M, N_("LVOT Diam(M)"), CM, NULL},
    {ADULT_AOR_DIAM_MM, DEPTH_DIST_M, N_("AoR Diam(M)"), CM, &AdultAoRDiamMMCalc},
    {ADULT_LA_DIMEN_MM, DEPTH_DIST_M, N_("LA Dimen(M)"), CM, &AdultLADiamMMCalc},
    //M Aortic Valve
    {ADULT_LV_ET, TIME_M, N_("LV ET(M)"), SEC, &AdultLVETCalc},
    {ADULT_LV_PEP, TIME_M, N_("LV PEP(M)"), SEC, &AdultLVPEPCalc},
    {ADULT_AV_CUSP_SEP, DEPTH_DIST_M, N_("AV Cusp Sep(M)"), CM, NULL},
    //M Mitral Valve
    {ADULT_MV_DE_EXC, DIST_SLOPE_M, N_("MV D-E Exc(M)"), ADULT_MV_DE_EXC_MULTI, NULL},//显示距离和斜率
    {ADULT_MV_DE_SLOPE, SLOPE, N_("MV D-E Slope(M)"), CMS, NULL},//仅显示斜率
    {ADULT_MV_EF_SEP, DEPTH_DIST_M, N_("MV E-F Sep(M)"), CM, NULL},
    {ADULT_MV_EF_SLOPE, SLOPE, N_("MV E-F Slope(M)"), CMS, NULL},//仅显示斜率
    {ADULT_MV_EPSS, DEPTH_DIST_M, N_("MV EPSS(M)"), CM, NULL},
    {ADULT_MV_AC_INTERVAL, TIME_M, N_("MV A-C Interval(M)"), SEC, NULL},
    //M Tricuspid Valve
    {ADULT_TV_DE_EXC, DIST_SLOPE_M, N_("TV D-E Exc(M)"), ADULT_TV_DE_EXC_MULTI, NULL},//显示距离和斜率
    {ADULT_TV_DE_SLOPE, SLOPE, N_("TV D-E Slope(M)"), CMS, NULL},
    {ADULT_TV_EF_SLOPE, SLOPE, N_("TV E-F Slope(M)"), CMS, NULL},
    {ADULT_TV_AC_INTERVAL, TIME_M, N_("TV A-C Interval(M)"), SEC, NULL},
    //M Pulmonic Valve
    {ADULT_LATE_DIAS_SLOPE, SLOPE, N_("Late Dias Slope(M)"), CMS, NULL},
    {ADULT_A_WAVE_AMP, DIST_DOT, N_("A Wave Amp(M)"), CM, NULL},
    {ADULT_BC_SLOPE, SLOPE, N_("B-C Slope(M)"), CMS, NULL},
    {ADULT_RV_ET, TIME_M, N_("RV ET(M)"), SEC, &AdultRVETCalc},
    {ADULT_RV_PEP, TIME_M, N_("RV PEP(M)"), SEC, &AdultRVPEPCalc},
    //D Aortic Valve
    {ADULT_AV_ACC_T, TIME_D, N_("AV Accel Time(D)"), SEC, NULL},
    {ADULT_AV_DEC_T, TIME_D, N_("AV Decel Time(D)"), SEC, NULL},
    {ADULT_AV_VMAX, PGMAX, N_("AV Vmax(D)"), ADULT_AV_VMAX_MULTI, &AdultAvVmaxCalc},
    {ADULT_LVOT_ACC_T, TIME_D, N_("LVOT Accel Time(D)"), SEC, NULL},
    {ADULT_LVOT_VMAX, PGMAX, N_("LVOT Vmax(D)"), ADULT_LVOT_VMAX_MULTI, &AdultLvotVmaxCalc},
    {ADULT_AI_ACC_T, TIME_D, N_("AI Accel Time(D)"), SEC, NULL},
    {ADULT_AI_P12T, P12T, N_("AI P1/2t(D)"), ADULT_AI_P12T_MULTI, NULL},//压力降半时间，实际上要显示Vmax, P1/2t, Slope
    {ADULT_AI_DEC_SLOPE, SLOPE_D, N_("AI Dec Slope(D)"), CMS2, NULL},
    //  {ADULT_AI_ALIAS_V, DIALOG_BOX, "AI Alias Vel", CMS, NULL},//弹出对话框，选择测量或输入
    {ADULT_AI_VMAX, PGMAX, N_("AI Vmax(D)"), ADULT_AI_VMAX_MULTI, NULL},
    {ADULT_AI_END_DIAS_V, PGMAX, N_("AI End Dias Vel(D)"), ADULT_AI_END_DIAS_V_MULTI, NULL},//测量速度和压力阶差
    {ADULT_VTI,INTEGRAL_TRACK,N_("VTI(D)"), CM, NULL},
    {ADULT_HR_AV, HR_D, N_("HR-AV(D)"), ADULT_HR_AV_MULTI, NULL},
    //D Mitral Valve
    {ADULT_HR_MV, HR_D, N_("HR-MV(D)"), ADULT_HR_MV_MULTI, NULL},
    {ADULT_MV_P12T, P12T, N_("MV P1/2t(D)"), ADULT_MV_P12T_MULTI, &AdultMVP12tCalc},//压力降半时间，实际上要显示Vmax, P1/2t, Slope
//  {ADULT_MV_ALIAS_V, VEL_D, "MV Alias Vel", CM, NULL},//弹出对话框，选择测量或输入
    {ADULT_MV_PEAK_E_V, PGMAX, N_("MV Peak E Vel(D)"), ADULT_MV_PEAK_E_V_MULTI, &AdultMVPeakECalc},//测量速度和压力阶差
    {ADULT_MV_PEAK_A_V, PGMAX, N_("MV Peak A Vel(D)"), ADULT_MV_PEAK_A_V_MULTI, &AdultMVPeakECalc},//测量速度和压力阶差
    {ADULT_MV_A_DUR, TIME_D, N_("MV A Dur(D)"), SEC, NULL},
    {ADULT_IVRT, TIME_D, N_("IVRT(D)"), SEC, &AdultIVRTCalc},
    {ADULT_MV_DEC_T, TIME_D, N_("MV Decel Time(D)"), SEC, NULL},
    {ADULT_MV_DEC_SLOPE, SLOPE_D, N_("MV Decel Slope(D)"), CMS2, NULL},
    {ADULT_IVCT, TIME_D, N_("IVCT(D)"), SEC, &AdultIVCTCalc},
    {ADULT_MV_ACC_T, TIME_D, N_("MV Accel Time(D)"), SEC, NULL},
    {ADULT_MV_VMAX, VEL_D, N_("MV Vmax(D)"), CMS, NULL},//仅速度测量
    {ADULT_MR_VMAX, VEL_D, N_("MR Vmax(D)"), CMS, NULL},//仅速度测量
//  {ADULT_MR_ALIAS_V, VEL_D, "MR Alias Vel", CM, NULL},//弹出对话框，选择测量或输入
    //D Pulmonic Valve
    {ADULT_HR_TV, HR_D, N_("HR-TV(D)"), ADULT_HR_TV_MULTI, NULL},
    {ADULT_TV_VMAX, VEL_D, N_("TV Vmax(D)"), CMS, NULL},//测量速度和压力阶差
    {ADULT_TR_VMAX, VEL_D, N_("TR Vmax(D)"), CMS, NULL},//测量速度和压力阶差
//  {ADULT_RA_PRESSURE, VEL_D, "RA Pressure", CM, NULL},//弹出对话框，选择或输入数据
    {ADULT_TV_ACC_T, TIME_D, N_("TV Accel Time(D)"), SEC, NULL},
    {ADULT_TV_PEAK_E_V, PGMAX, N_("TV Peak E Vel(D)"), ADULT_TV_PEAK_E_V_MULTI, &AdultTVPeakECalc},//测量速度和压力阶差
    {ADULT_TV_PEAK_A_V, PGMAX, N_("TV Peak A Vel(D)"), ADULT_TV_PEAK_A_V_MULTI, &AdultTVPeakACalc},//测量速度和压力阶差
    //D Tricuspid Valve
    {ADULT_HR_PV, HR_D, N_("HR-PV(D)"), ADULT_HR_PV_MULTI, NULL},
    {ADULT_PV_VMAX, PGMAX, N_("PV Vmax(D)"), ADULT_PV_VMAX_MULTI, &AdultPvVmaxCalc},//测量速度和压力阶差
    {ADULT_PV_ACC_T, TIME_D, N_("PV Accel Time(D)"), SEC, NULL},
    {ADULT_PI_END_DIAS_V, PGMAX, N_("PI End Dias Vel(D)"), ADULT_PI_END_DIAS_V_MULTI, NULL},//测量速度和压力阶差
    {ADULT_PULM_DIAS_V, PGMAX, N_("Pulm Dias Vel(D)"), ADULT_PULM_DIAS_V_MULTI, &AdultPulmDiasCalc},//测量速度和压力阶差
    {ADULT_PULM_SYS_V, PGMAX, N_("Pulm Sys Vel(D)"), ADULT_PULM_SYS_V_MULTI, &AdultPulmSysCalc},//测量速度和压力阶差
    {ADULT_RVOT_VMAX, PGMAX, N_("RVOT Vmax(D)"), ADULT_RVOT_VMAX_MULTI, &AdultRvotVmaxCalc},
};

/******************************OB Measure**************************************/
const CalcResultInfo  OBCerEDCB = {OB_CER_EDCB, {OB_CER, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray fkdakfdakOBCerGWCalc = {&OBCerEDCB, NULL};
const CalcResultInfo  OBCerGW = {OB_CER_GW, {OB_CER, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBHLEDCB = {OB_HL_EDCB, {OB_HL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBHLGWCalc = {&OBHLEDCB, NULL};
const CalcResultInfo  OBHLGW = {OB_HL_GW, {OB_HL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBBPDEDCB = {OB_BPD_EDCB, {OB_BPD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBBPDGWCalc = {&OBBPDEDCB, NULL};
const CalcResultInfo  OBBPDGW = {OB_BPD_GW, {OB_BPD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBFLEDCB = {OB_FL_EDCB, {OB_FL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBFLGWCalc = {&OBFLEDCB, NULL};
const CalcResultInfo  OBFLGW = {OB_FL_GW, {OB_FL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBCRLEDCB = {OB_CRL_EDCB, {OB_CRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBCRLGWCalc = {&OBCRLEDCB, NULL};
const CalcResultInfo  OBCRLGW = {OB_CRL_GW, {OB_CRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBGSEDCB = {OB_GS_EDCB, {OB_GS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBGSGWCalc = {&OBGSEDCB, NULL};
const CalcResultInfo  OBGSGW = {OB_GS_GW, {OB_GS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBACEDCB = {OB_AC_EDCB, {OB_AC, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBACGWCalc = {&OBACEDCB, NULL};
const CalcResultInfo  OBACGW = {OB_AC_GW, {OB_AC, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBHCEDCB = {OB_HC_EDCB, {OB_HC, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBHCGWCalc = {&OBHCEDCB, NULL};
const CalcResultInfo  OBHCGW = {OB_HC_GW, {OB_HC, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBTADEDCB = {OB_TAD_EDCB, {OB_TAD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBTADGWCalc = {&OBTADEDCB, NULL};
const CalcResultInfo  OBTADGW = {OB_TAD_GW, {OB_TAD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBAPADEDCB = {OB_APAD_EDCB, {OB_APAD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBAPADGWCalc = {&OBAPADEDCB, NULL};
const CalcResultInfo  OBAPADGW = {OB_APAD_GW, {OB_APAD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBTHDEDCB = {OB_THD_EDCB, {OB_THD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBTHDGWCalc = {&OBTHDEDCB, NULL};
const CalcResultInfo  OBTHDGW = {OB_THD_GW, {OB_THD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBOFDEDCB = {OB_OFD_EDCB, {OB_OFD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBOFDGWCalc = {&OBOFDEDCB, NULL};
const CalcResultInfo  OBOFDGW = {OB_OFD_GW, {OB_OFD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBOODEDCB = {OB_OOD_EDCB, {OB_OOD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBOODGWCalc = {&OBOODEDCB, NULL};
const CalcResultInfo  OBOODGW = {OB_OOD_GW, {OB_OOD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBTibiaEDCB = {OB_TIBIA_EDCB, {OB_TIBIA, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBTibiaGWCalc = {&OBTibiaEDCB, NULL};
const CalcResultInfo  OBTibiaGW = {OB_TIBIA_GW, {OB_TIBIA, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBTTDEDCB = {OB_TTD_EDCB, {OB_TTD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBTTDGWCalc = {&OBTTDEDCB, NULL};
const CalcResultInfo  OBTTDGW = {OB_TTD_GW, {OB_TTD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};
const CalcResultInfo  OBUlnaEDCB = {OB_ULNA_EDCB, {OB_ULNA, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//const CalcInfoArray OBUlnaGWCalc = {&OBUlnaEDCB, NULL};
const CalcResultInfo  OBUlnaGW = {OB_ULNA_GW, {OB_ULNA, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};

//const CalcResultInfo OBEfw = {OB_EFW, {OB_EFW_METH, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EFW", KG, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBEfwHadlock1 = {OB_EFW_HADLOCK1, {OB_AC, OB_FL, NO_PAR, NO_PAR, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBEfwHadlock2 = {OB_EFW_HADLOCK2, {OB_AC, OB_BPD, OB_FL, NO_PAR, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBEfwHadlock3 = {OB_EFW_HADLOCK3, {OB_AC, OB_HC, OB_FL, NO_PAR, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBEfwHadlock4 = {OB_EFW_HADLOCK4, {OB_AC, OB_BPD, OB_HC, OB_FL, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBEfwShepard = {OB_EFW_SHEPARD, {OB_BPD, OB_AC, NO_PAR, NO_PAR, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBEfwHansmenn = {OB_EFW_HANSMAN, {OB_BPD, OB_TAD, OB_FL, NO_PAR, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBEfwTokyo = {OB_EFW_TOKYO, {OB_BPD, OB_TAD, OB_APAD, OB_FL, NO_PAR}, "EFW", KG, MeaCalcFun::EFWCalc, NULL};
const CalcResultInfo OBAFI = {OB_CALC_AFI, {OB_AFI_LUQ, OB_AFI_LLQ, OB_AFI_RUQ, OB_AFI_RLQ, NO_PAR}, "AFI", CM, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBLOvVol = {OB_CALC_L_OV_VOL, {OB_L_OV_L, OB_L_OV_W, OB_L_OV_H, NO_PAR, NO_PAR}, "Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBROvVol = {OB_CALC_R_OV_VOL, {OB_R_OV_L, OB_R_OV_W, OB_R_OV_H, NO_PAR, NO_PAR}, "Vol", CM3, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo OBMeanSac = {OB_MEAN_SAC, {OB_SAC1, OB_SAC2, OB_SAC3, NO_PAR, NO_PAR}, "mean Sac", CM, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBAoPa = {OB_AO_PA_RATIO, {OB_AOR_DIAM, OB_MPA_DIAM, NO_PAR, NO_PAR, NO_PAR}, "Ao/PA", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBHrtCThrC = {OB_HRTC_THRC_RATIO, {OB_HRTC_2D, OB_THC, NO_PAR, NO_PAR, NO_PAR}, "HrtC/ThrC", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBLaAo = {OB_LA_AO_RTAIO, {OB_LA_DIMEN, OB_AOR_DIAM, NO_PAR, NO_PAR, NO_PAR}, "LA/Ao", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBLaRa = {OB_LA_RA_RATIO, {OB_LA_DIMEN, OB_RA_DIMEN, NO_PAR, NO_PAR, NO_PAR}, "LA/RA", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBLvRv = {OB_LV_RV_RATIO, {OB_LV_DIMEN, OB_RV_DIMEN, NO_PAR, NO_PAR, NO_PAR}, "LV/RV", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBHCAC = {OB_HC_AC_RATIO, {OB_HC, OB_AC, NO_PAR, NO_PAR, NO_PAR}, "HC/AC", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBFLBPD = {OB_FL_BPD_RATIO, {OB_FL, OB_BPD, NO_PAR, NO_PAR, NO_PAR}, "FL/BPD", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBFLAC = {OB_FL_AC_RATIO, {OB_FL, OB_AC, NO_PAR, NO_PAR, NO_PAR}, "FL/AC", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBBPDa = {OB_BPDA, {OB_BPD, OB_OFD, NO_PAR, NO_PAR, NO_PAR}, "BPDa", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo OBCI = {OB_CI, {OB_BPD, OB_OFD, NO_PAR, NO_PAR, NO_PAR}, "CI", NOUNIT, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo  OBFTAGW = {OB_FTA_GW, {OB_FTA, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWMan, NULL};//12.05
const CalcResultInfo  OBFTAEDCB = {OB_FTA_EDCB, {OB_FTA, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};//12.05

const CalcInfoArray OBCerCalc = {&OBCerGW, &OBCerEDCB, NULL};
const CalcInfoArray OBHLCalc = {&OBHLGW, &OBHLEDCB, NULL};
const CalcInfoArray OBBPDCalc = {&OBBPDGW, &OBBPDEDCB, &OBEfwHadlock2, &OBEfwHadlock4, &OBEfwShepard, &OBEfwHansmenn, &OBEfwTokyo,&OBFLBPD, &OBBPDa, &OBCI, NULL};
const CalcInfoArray OBFLCalc = {&OBFLGW, &OBFLEDCB, &OBEfwHadlock1, &OBEfwHadlock2, &OBEfwHadlock3, &OBEfwHadlock4, &OBEfwTokyo, &OBFLBPD, &OBFLAC, NULL};
const CalcInfoArray OBGSCalc = {&OBGSGW, &OBGSEDCB, NULL};
const CalcInfoArray OBACCalc = {&OBACGW, &OBACEDCB, &OBEfwHadlock1, &OBEfwHadlock2, &OBEfwHadlock3, &OBEfwHadlock4, &OBEfwShepard, &OBHCAC, &OBFLAC, NULL};
const CalcInfoArray OBHCCalc = {&OBHCGW, &OBHCEDCB, &OBEfwHadlock3, &OBEfwHadlock4, &OBHCAC, NULL};
const CalcInfoArray OBTADCalc = {&OBTADGW, &OBTADEDCB, &OBEfwHansmenn, &OBEfwTokyo, NULL};
const CalcInfoArray OBAPADCalc = {&OBAPADGW, &OBAPADEDCB, &OBEfwTokyo, NULL};
const CalcInfoArray OBOFDCalc = {&OBOFDGW, &OBOFDEDCB, &OBBPDa, &OBCI, NULL};
const CalcInfoArray OBTHDCalc = {&OBTHDGW, &OBTHDEDCB, NULL};
const CalcInfoArray OBOODCalc = {&OBOODGW, &OBOODEDCB, NULL};
const CalcInfoArray OBTibiaCalc = {&OBTibiaGW, &OBTibiaEDCB, NULL};
const CalcInfoArray OBTTDCalc = {&OBTTDGW, &OBTTDEDCB, NULL};
const CalcInfoArray OBUlnaCalc = {&OBUlnaGW, &OBUlnaEDCB, NULL};
const CalcInfoArray OBAFICalc = {&OBAFI, NULL};
const CalcInfoArray OBCRLCalc = {&OBCRLGW, &OBCRLEDCB, NULL};
//const CalcInfoArray OBSacCalc = {&OBMeanSac, NULL};
const CalcInfoArray OBLOvVolCalc = {&OBLOvVol, NULL};
const CalcInfoArray OBROvVolCalc = {&OBROvVol, NULL};
const CalcInfoArray OBHrtCThrCCalc = {&OBHrtCThrC, NULL};
const CalcInfoArray OBAoRDiamCalc = {&OBAoPa, &OBLaAo, NULL};
const CalcInfoArray OBMPADiamCalc = {&OBAoPa, NULL};
const CalcInfoArray OBLaDimenCalc = {&OBLaRa, &OBLaAo, NULL};
const CalcInfoArray OBRaDimenCalc = {&OBLaRa, NULL};
const CalcInfoArray OBLvDimenCalc = {&OBLvRv, NULL};
const CalcInfoArray OBRvDimenCalc = {&OBLvRv, NULL};
const CalcInfoArray OBFTACalc = {&OBFTAGW,&OBFTAEDCB, NULL}; //12.05

const PtrOfCalcInfo OBCalcInfoPtr[OB_CALC_END - OB_CALC_START] = {
    {OB_CER_GW, &OBCerGW},
    {OB_CER_EDCB, &OBCerEDCB},
    {OB_HL_GW, &OBHLGW},
    {OB_HL_EDCB, &OBHLEDCB},
    {OB_BPD_GW, &OBBPDGW},
    {OB_BPD_EDCB, &OBBPDEDCB},
    {OB_FL_GW, &OBFLGW},
    {OB_FL_EDCB, &OBFLEDCB},
    {OB_OOD_GW, &OBOODGW},
    {OB_OOD_EDCB, &OBOODEDCB},
    {OB_TIBIA_GW, &OBTibiaGW},
    {OB_TIBIA_EDCB, &OBTibiaEDCB},
    {OB_TTD_GW, &OBTTDGW},
    {OB_TTD_EDCB, &OBTTDEDCB},
    {OB_ULNA_GW, &OBUlnaGW},
    {OB_ULNA_EDCB, &OBUlnaEDCB},
    {OB_CRL_GW, &OBCRLGW},
    {OB_CRL_EDCB, &OBCRLEDCB},
    {OB_GS_GW, &OBGSGW},
    {OB_GS_EDCB, &OBGSEDCB},
    {OB_AC_GW, &OBACGW},
    {OB_AC_EDCB, &OBACEDCB},
    {OB_HC_GW, &OBHCGW},
    {OB_HC_EDCB, &OBHCEDCB},
    {OB_TAD_GW, &OBTADGW},
    {OB_TAD_EDCB, &OBTADEDCB},
    {OB_APAD_GW, &OBAPADGW},
    {OB_APAD_EDCB, &OBAPADEDCB},
    {OB_THD_GW, &OBTHDGW},
    {OB_THD_EDCB, &OBTHDEDCB},
    {OB_OFD_GW, &OBOFDGW},
    {OB_OFD_EDCB, &OBOFDEDCB},
    {OB_EFW_HADLOCK1, &OBEfwHadlock1},
    {OB_EFW_HADLOCK2, &OBEfwHadlock2},
    {OB_EFW_HADLOCK3, &OBEfwHadlock3},
    {OB_EFW_HADLOCK4, &OBEfwHadlock4},
    {OB_EFW_SHEPARD, &OBEfwShepard},
    {OB_EFW_HANSMAN, &OBEfwHansmenn},
    {OB_EFW_TOKYO, &OBEfwTokyo},
    {OB_CALC_AFI, &OBAFI},
    {OB_CALC_L_OV_VOL, &OBLOvVol},
    {OB_CALC_R_OV_VOL, &OBROvVol},
    //{OB_MEAN_SAC, &OBMeanSac},
    {OB_AO_PA_RATIO, &OBAoPa},
    {OB_HRTC_THRC_RATIO, &OBHrtCThrC},
    {OB_LA_AO_RTAIO, &OBLaAo},
    {OB_LA_RA_RATIO, &OBLaRa},
    {OB_LV_RV_RATIO, &OBLvRv},
    {OB_HC_AC_RATIO, &OBHCAC},
    {OB_FL_BPD_RATIO, &OBFLBPD},
    {OB_FL_AC_RATIO, &OBFLAC},
    {OB_BPDA, &OBBPDa},
    {OB_CI, &OBCI},
    {OB_FTA_GW, &OBFTAGW}, //12.05
    {OB_FTA_EDCB, &OBFTAEDCB}, //12.05
};

const MultiItemInfo OBMultiInfo[] = {
#ifdef EMP_322
#else
    {OB_DESC_AO, OB_DESC_AO_MULTI, RI, "Desc Ao", RiInfo, NULL },
    {OB_L_MCA, OB_L_MCA_MULTI, PSED, "L MCA", PsEdInfo, NULL },
    {OB_R_MCA, OB_R_MCA_MULTI, PSED, "R MCA", PsEdInfo, NULL },
    {OB_L_UTERINE_A, OB_L_UTERINE_A_MULTI, RI, "L Uterine A", RiInfo, NULL },
    {OB_R_UTERINE_A, OB_R_UTERINE_A_MULTI, RI, "R Uterine A", RiInfo, NULL },
    {OB_THORACIC_AO, OB_THORACIC_AO_MULTI, RI, "Thoracic Ao", RiInfo, NULL },
    {OB_UMBILICAL_A, OB_UMBILICAL_A_MULTI, RI, "Umbilical A", RiInfo, NULL },
#endif
    {OB_SAC, OB_SAC_MULTI, SAC_3AXIS, "Sac", SacInfo, NULL }
};

const SingleItemInfo OBInfo[] = {
    {OB_CER, DIST_DOT, N_("CER"), CM, &OBCerCalc},
    {OB_CIST_MAG, DIST_DOT, N_("Cist Magna"), CM, NULL},
    {OB_OOD, DIST_DOT, N_("OOD"), CM, &OBOODCalc},
    {OB_TIBIA, DIST_DOT, N_("Tibia"), CM, &OBTibiaCalc},
    {OB_TTD, DIST_DOT, N_("TTD"), CM, &OBTTDCalc},
//  {OB_HUMERUS, DIST_DOT, N_("Humerus"), CM, NULL},
    {OB_IOD, DIST_DOT, N_("IOD"), CM, NULL},
    {OB_RADIUS, DIST_DOT, N_("Radius"), CM, NULL},
    {OB_ULNA, DIST_DOT, N_("Ulna"), CM, &OBUlnaCalc},
    {OB_FIBULA, DIST_DOT, N_("Fibula"), CM, NULL},
    {OB_NUCHFOLD, DIST_DOT, N_("Nuch Fold"), CM, NULL},
#ifdef EMP_322
#else
    {OB_UMBILICAL_A, RI, N_("Umbilical A"), OB_UMBILICAL_A_MULTI, NULL},
    {OB_L_MCA, PSED, N_("L MCA"), OB_L_MCA_MULTI, NULL},
    {OB_R_MCA, PSED, N_("R MCA"), OB_R_MCA_MULTI, NULL},
#endif
    {OB_AFI, MEA_TYPE_END, N_("AFI"), UNIT_END, NULL},
    {OB_AFI_LUQ, DIST_DOT, N_("AFI1"), CM, &OBAFICalc},
    {OB_AFI_LLQ, DIST_DOT, N_("AFI2"), CM, &OBAFICalc},
    {OB_AFI_RUQ, DIST_DOT, N_("AFI3"), CM, &OBAFICalc},
    {OB_AFI_RLQ, DIST_DOT, N_("AFI4"), CM, &OBAFICalc},
//  {OB_BPP, DIST_DOT, N_("BPP"), CM, NULL},//生物物理相评分，对话框
    {OB_CRL, DIST_DOT, N_("CRL"), CM, &OBCRLCalc},
    {OB_NT, DIST_DOT, N_("NT"), CM, NULL},
    {OB_CERV, DIST_DOT, N_("Cerv Length"), CM, NULL},
    /*
    {OB_SAC1, DIST_DOT, N_("Sac Diam1"), CM, &OBSacCalc},
    {OB_SAC2, DIST_DOT, N_("Sac Diam2"), CM, &OBSacCalc},
    {OB_SAC3, DIST_DOT, N_("Sac Diam3"), CM, &OBSacCalc},
    */
    {OB_SAC, SAC_3AXIS, N_("Sac Diam"), OB_SAC_MULTI, NULL},
    {OB_YOLK_SAC, DIST_DOT, N_("Yolk Sac"), CM, NULL},
    {OB_L_OV_VOL, MEA_TYPE_END, N_("L Ov Vol"), UNIT_END, NULL},
    {OB_L_OV_H, DIST_DOT, N_("L Ov Height"), CM, &OBLOvVolCalc},
    {OB_L_OV_L, DIST_DOT, N_("L Ov Length"), CM, &OBLOvVolCalc},
    {OB_L_OV_W, DIST_DOT, N_("L Ov Width"), CM, &OBLOvVolCalc},
    {OB_R_OV_VOL, MEA_TYPE_END, N_("R Ov Vol"), UNIT_END, NULL},
    {OB_R_OV_H, DIST_DOT, N_("R Ov Height"), CM, &OBROvVolCalc},
    {OB_R_OV_L, DIST_DOT, N_("R Ov Length"), CM, &OBROvVolCalc},
    {OB_R_OV_W, DIST_DOT, N_("R Ov Width"), CM, &OBROvVolCalc},
    {OB_RA_DIMEN, DIST_DOT, N_("RA Dimension"), CM, &OBRaDimenCalc},
    {OB_RA_LEN, DIST_DOT, N_("RA Length"), CM, NULL},
    {OB_RA_WIDTH, DIST_DOT, N_("RA Width"), CM, NULL},
    {OB_RV_DIMEN, DIST_DOT, N_("RV Dimension"), CM, &OBRvDimenCalc},
    {OB_RV_LEN, DIST_DOT, N_("RV Length"), CM, NULL},
    {OB_RV_WIDTH, DIST_DOT, N_("RV Width"), CM, NULL},
    {OB_RVOT, DIST_DOT, N_("RVOT Diam"), CM, NULL},
    {OB_IVS_2D, DIST_DOT, N_("IVS(2D)"), CM, NULL},
    {OB_LA_DIMEN, DIST_DOT, N_("LA Dimension"), CM, &OBLaDimenCalc},
    {OB_LA_LEN, DIST_DOT, N_("LA Length"), CM, NULL},
    {OB_LA_WIDTH, DIST_DOT, N_("LA Width"), CM, NULL},
    {OB_LV_DIMEN, DIST_DOT, N_("LV Dimension"), CM, &OBLvDimenCalc},
    {OB_LV_LEN, DIST_DOT, N_("LV Length"), CM, NULL},
    {OB_LV_WIDTH, DIST_DOT, N_("LV Width"), CM, NULL},
    {OB_LVOT_DIAM, DIST_DOT, N_("LVOT Diam"), CM, NULL},
    {OB_HA_2D, AREA_ELLIPSE, N_("Heart Area"), CM2, &OBHrtCThrCCalc},//椭圆面积
    {OB_HRTC_2D, PERI_ELLIPSE, N_("Heart Circ"), CM, &OBHrtCThrCCalc},//椭圆周长
    {OB_HR, HR_M, N_("Heart Rate"), BPM, NULL},
    {OB_THC, PERI_ELLIPSE, N_("ThrC"), CM, &OBHrtCThrCCalc},//胸周长
    {OB_AOR_DIAM, DIST_DOT, N_("AoR Diam"), CM, &OBAoRDiamCalc},
    {OB_AO_ANNUL, DIST_DOT, N_("Ao Annul Diam"), CM, NULL},
    {OB_ASC_AO_DIAM, DIST_DOT, N_("Asc Ao Diam(2D)"), CM, NULL},
#ifdef EMP_322
#else
    {OB_DESC_AO, RI, N_("Desc Ao"), OB_DESC_AO_MULTI, NULL},
#endif
    {OB_DESC_AO_DIAM, DIST_DOT, N_("Desc Ao Diam(2D)"), CM, NULL},
    {OB_DUCT_ART_DIAM, DIST_DOT, N_("Duct Art Diam(2D)"), CM, NULL},
    {OB_DUCT_VEN, DIST_DOT, N_("Duct Ven"), CM, NULL},//还未知道是测什么
    {OB_MV_ANNUL_DIAM, DIST_DOT, N_("MV Annul Diam"), CM, NULL},
    {OB_TV_ANNUL_DIAM, DIST_DOT, N_("TV Annul Diam"), CM, NULL},
    {OB_MPA_DIAM, DIST_DOT, N_("MPA Diam(2D)"), CM, &OBMPADiamCalc},
#ifdef EMP_322
#else
    {OB_THORACIC_AO, RI, N_("Thoracic Ao"), OB_THORACIC_AO_MULTI, NULL},//胸主动脉
#endif
    {OB_GS, DIST_DOT, N_("GS"), CM, &OBGSCalc},
    {OB_BPD, DIST_DOT, N_("BPD"), CM, &OBBPDCalc},
    {OB_AC, PERI_ELLIPSE, N_("AC"), CM, &OBACCalc},//周长
    {OB_HC, PERI_ELLIPSE, N_("HC"), CM, &OBHCCalc},//周长
    {OB_HL, DIST_DOT, N_("HL"), CM, &OBHLCalc},
    {OB_FL, DIST_DOT, N_("FL"), CM,  &OBFLCalc},
    {OB_TAD, DIST_DOT,N_( "TAD"), CM, &OBTADCalc},
    {OB_APAD, DIST_DOT, N_("APAD"), CM, &OBAPADCalc},
    {OB_THD, DIST_DOT, N_("THD"), CM, &OBTHDCalc},
    {OB_OFD, DIST_DOT, N_("OFD"), CM, &OBOFDCalc},
    {OB_FTA, AREA_ELLIPSE, N_("FTA"), CM2, &OBFTACalc},//面积测量12.05
    //{OB_FTA, DIST_DOT, N_("FTA"), CM, NULL},//面积测量
    {OB_EAR, DIST_DOT, N_("Ear"), CM, NULL},
    {OB_ORIBIT1, DIST_DOT, N_("Orbit1"), CM, NULL},
    {OB_ORIBIT2, DIST_DOT, N_("Orbit2"), CM, NULL},
    {OB_POST_FOSSA, DIST_DOT, N_("Post Fossa"), CM, NULL},
    {OB_MANDIBLE, DIST_DOT, N_("Mandible"), CM, NULL},
    {OB_NASAL, DIST_DOT, N_("Nasal"), CM, NULL},
    {OB_CLAVICLE, DIST_DOT, N_("Clavicle"), CM, NULL},
    {OB_M_PHALANX5, DIST_DOT, N_("M Phalanx 5"), CM, NULL},
    {OB_L_FOOT_L, DIST_DOT, N_("L Foot L"), CM, NULL},//关于脚的测量忘了
    {OB_L_FOOT_A, ANGLE_3DOT, N_("L Foot A"), ANG, NULL},//关于脚的测量忘了
    {OB_R_FOOT_L, DIST_DOT, N_("R Foot L"), CM, NULL},//关于脚的测量忘了
    {OB_R_FOOT_A, ANGLE_3DOT, N_("R Foot A"), ANG, NULL},//关于脚的测量忘了
    {OB_BLADDER_AP, DIST_DOT, N_("Bladder AP"), CM, NULL},
    {OB_BLADDER_L, DIST_DOT, N_("Bladder L"), CM, NULL},
    {OB_BLADDER_TR, DIST_DOT, N_("Bladder Tr"), CM, NULL},
    {OB_L_RENAL_AP, DIST_DOT, N_("L Renal AP"), CM, NULL},
    {OB_L_RENAL_L, DIST_DOT, N_("L Renal L"), CM, NULL},
    {OB_L_RENAL_TR, DIST_DOT, N_("L Renal Tr"), CM, NULL},
    {OB_L_RENAL_PELVIS, DIST_DOT, N_("L Renal Pelvis"), CM, NULL},
    {OB_R_RENAL_AP, DIST_DOT, N_("R Renal AP"), CM, NULL},
    {OB_R_RENAL_L, DIST_DOT, N_("R Renal L"), CM, NULL},
    {OB_R_RENAL_TR, DIST_DOT, N_("R Renal Tr"), CM, NULL},
    {OB_R_RENAL_PELVIS, DIST_DOT, N_("R Renal Pelvis"), CM, NULL},
    {OB_ADR_GLAND_AP, DIST_DOT, N_("Adr Gland AP"), CM, NULL},
    {OB_ADR_GLAND_L, DIST_DOT, N_("Adr Gland L"), CM, NULL},
    {OB_ADR_GLAND_TR, DIST_DOT, N_("Adr Gland Tr"), CM, NULL},
    {OB_URETER_AP, DIST_DOT, N_("Ureter AP"), CM, NULL},
    {OB_URETER_TR, DIST_DOT, N_("Ureter Tr"), CM, NULL},
    {OB_L_LUNG_DIAM, DIST_DOT, N_("L Lung Diam"), CM, NULL},
    {OB_R_LUNG_DIAM, DIST_DOT, N_("R Lung Diam"), CM, NULL},
#ifdef EMP_322
#else
    {OB_L_UTERINE_A, RI, N_("L Uterine A"), OB_L_UTERINE_A_MULTI, NULL},//子宫动脉，没写清测啥
    {OB_R_UTERINE_A, RI, N_("R Uterine A"), OB_R_UTERINE_A_MULTI, NULL},//子宫动脉，没写清测啥
#endif
    {OB_PELVIS_AP, DIST_DOT, N_("Pelvis AP"), CM, NULL},
    {OB_PELVIS_L, DIST_DOT, N_("Pelvis L"), CM, NULL},
    {OB_PELVIS_TR, DIST_DOT, N_("Pelvis Tr"), CM, NULL},
    {OB_SCAPULA, DIST_DOT, N_("Scapul"), CM, NULL}
};
#ifndef VET
const SingleItemInfo EFWInfo[] = {
    { OB_EFW_NEW, MEA_TYPE_END, N_("EFW"), UNIT_END, NULL}
};
#endif

/******************************GYN Measure**************************************/
//const CalcResultInfo GYNBladderVol = {GYN_CALC_BLADDER_VOL, "Bladder Vol", CM3, MeaCalcFun::GallBladderVolCalc};
const CalcResultInfo  GYNBladderVol = {GYN_CALC_BLADDER_VOL, {GYN_BLADDER_L, GYN_BLADDER_W, GYN_BLADDER_H, NO_PAR, NO_PAR}, "Bladder Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNPVBladderVol = {GYN_CALC_PV_BLADDER_VOL, {GYN_PV_BLADDER_L, GYN_PV_BLADDER_W, GYN_PV_BLADDER_H, NO_PAR, NO_PAR}, "PV Bladder Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLOvVol = {GYN_CALC_L_OV_VOL, {GYN_L_OV_LENGTH, GYN_L_OV_WIDTH, GYN_L_OV_HEIGHT, NO_PAR, NO_PAR}, "L Ov Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNROvVol = {GYN_CALC_R_OV_VOL, {GYN_R_OV_LENGTH, GYN_R_OV_WIDTH, GYN_R_OV_HEIGHT, NO_PAR, NO_PAR}, "R Ov Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNUterusVol = {GYN_CALC_UTERUS_VOL, {GYN_UTERUS_LENGTH, GYN_UTERUS_WIDTH, GYN_UTERUS_HEIGHT, NO_PAR, NO_PAR}, "Uterus Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL1Vol = {GYN_L_FOLL1_VOL, {GYN_L_FOLL1, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL2Vol = {GYN_L_FOLL2_VOL, {GYN_L_FOLL2, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL3Vol = {GYN_L_FOLL3_VOL, {GYN_L_FOLL3, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL4Vol = {GYN_L_FOLL4_VOL, {GYN_L_FOLL4, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL5Vol = {GYN_L_FOLL5_VOL, {GYN_L_FOLL5, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL6Vol = {GYN_L_FOLL6_VOL, {GYN_L_FOLL6, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll6 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL7Vol = {GYN_L_FOLL7_VOL, {GYN_L_FOLL7, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll7 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL8Vol = {GYN_L_FOLL8_VOL, {GYN_L_FOLL8, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll8 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL9Vol = {GYN_L_FOLL9_VOL, {GYN_L_FOLL9, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll9 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNLFOLL10Vol = {GYN_L_FOLL10_VOL, {GYN_L_FOLL10, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "L Foll10 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL1Vol = {GYN_R_FOLL1_VOL, {GYN_R_FOLL1, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL2Vol = {GYN_R_FOLL2_VOL, {GYN_R_FOLL2, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL3Vol = {GYN_R_FOLL3_VOL, {GYN_R_FOLL3, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL4Vol = {GYN_R_FOLL4_VOL, {GYN_R_FOLL4, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL5Vol = {GYN_R_FOLL5_VOL, {GYN_R_FOLL5, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL6Vol = {GYN_R_FOLL6_VOL, {GYN_R_FOLL6, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll6 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL7Vol = {GYN_R_FOLL7_VOL, {GYN_R_FOLL7, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll7 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL8Vol = {GYN_R_FOLL8_VOL, {GYN_R_FOLL8, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll8 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL9Vol = {GYN_R_FOLL9_VOL, {GYN_R_FOLL9, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll9 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNRFOLL10Vol = {GYN_R_FOLL10_VOL, {GYN_R_FOLL10, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "R Foll10 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
/*
const CalcResultInfo  GYNFIBROID1Vol = {GYN_FIBROID1_VOL, {GYN_FIBROID1, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID2Vol = {GYN_FIBROID2_VOL, {GYN_FIBROID2, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID3Vol = {GYN_FIBROID3_VOL, {GYN_FIBROID3, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID4Vol = {GYN_FIBROID4_VOL, {GYN_FIBROID4, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID5Vol = {GYN_FIBROID5_VOL, {GYN_FIBROID5, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID6Vol = {GYN_FIBROID6_VOL, {GYN_FIBROID6, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid6 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID7Vol = {GYN_FIBROID7_VOL, {GYN_FIBROID7, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid7 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID8Vol = {GYN_FIBROID8_VOL, {GYN_FIBROID8, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid8 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID9Vol = {GYN_FIBROID9_VOL, {GYN_FIBROID9, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid9 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  GYNFIBROID10Vol = {GYN_FIBROID10_VOL, {GYN_FIBROID10, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "Fibroid10 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
*/

const CalcInfoArray GYNBladderCalc = {&GYNBladderVol, NULL};
const CalcInfoArray GYNPVBladderCalc = {&GYNPVBladderVol, NULL};
const CalcInfoArray GYNLOvCalc = {&GYNLOvVol, NULL};
const CalcInfoArray GYNROvCalc = {&GYNROvVol, NULL};
const CalcInfoArray GYNUterusCalc = {&GYNUterusVol, NULL};
const CalcInfoArray GYNLFOLL1Calc = {&GYNLFOLL1Vol, NULL};
const CalcInfoArray GYNLFOLL2Calc = {&GYNLFOLL2Vol, NULL};
const CalcInfoArray GYNLFOLL3Calc = {&GYNLFOLL3Vol, NULL};
const CalcInfoArray GYNLFOLL4Calc = {&GYNLFOLL4Vol, NULL};
const CalcInfoArray GYNLFOLL5Calc = {&GYNLFOLL5Vol, NULL};
const CalcInfoArray GYNLFOLL6Calc = {&GYNLFOLL6Vol, NULL};
const CalcInfoArray GYNLFOLL7Calc = {&GYNLFOLL7Vol, NULL};
const CalcInfoArray GYNLFOLL8Calc = {&GYNLFOLL8Vol, NULL};
const CalcInfoArray GYNLFOLL9Calc = {&GYNLFOLL9Vol, NULL};
const CalcInfoArray GYNLFOLL10Calc = {&GYNLFOLL10Vol, NULL};
const CalcInfoArray GYNRFOLL1Calc = {&GYNRFOLL1Vol, NULL};
const CalcInfoArray GYNRFOLL2Calc = {&GYNRFOLL2Vol, NULL};
const CalcInfoArray GYNRFOLL3Calc = {&GYNRFOLL3Vol, NULL};
const CalcInfoArray GYNRFOLL4Calc = {&GYNRFOLL4Vol, NULL};
const CalcInfoArray GYNRFOLL5Calc = {&GYNRFOLL5Vol, NULL};
const CalcInfoArray GYNRFOLL6Calc = {&GYNRFOLL6Vol, NULL};
const CalcInfoArray GYNRFOLL7Calc = {&GYNRFOLL7Vol, NULL};
const CalcInfoArray GYNRFOLL8Calc = {&GYNRFOLL8Vol, NULL};
const CalcInfoArray GYNRFOLL9Calc = {&GYNRFOLL9Vol, NULL};
const CalcInfoArray GYNRFOLL10Calc = {&GYNRFOLL10Vol, NULL};
/*
const CalcInfoArray GYNFIBROID1Calc = {&GYNFIBROID1Vol, NULL};
const CalcInfoArray GYNFIBROID2Calc = {&GYNFIBROID2Vol, NULL};
const CalcInfoArray GYNFIBROID3Calc = {&GYNFIBROID3Vol, NULL};
const CalcInfoArray GYNFIBROID4Calc = {&GYNFIBROID4Vol, NULL};
const CalcInfoArray GYNFIBROID5Calc = {&GYNFIBROID5Vol, NULL};
const CalcInfoArray GYNFIBROID6Calc = {&GYNFIBROID6Vol, NULL};
const CalcInfoArray GYNFIBROID7Calc = {&GYNFIBROID7Vol, NULL};
const CalcInfoArray GYNFIBROID8Calc = {&GYNFIBROID8Vol, NULL};
const CalcInfoArray GYNFIBROID9Calc = {&GYNFIBROID9Vol, NULL};
const CalcInfoArray GYNFIBROID10Calc = {&GYNFIBROID10Vol, NULL};
*/

const PtrOfCalcInfo GYNCalcInfoPtr[GYN_CALC_END - GYN_CALC_START] = {
    {GYN_CALC_BLADDER_VOL, &GYNBladderVol},
    {GYN_CALC_PV_BLADDER_VOL, &GYNPVBladderVol},
    {GYN_CALC_L_OV_VOL, &GYNLOvVol},
    {GYN_CALC_R_OV_VOL, &GYNROvVol},
    {GYN_CALC_UTERUS_VOL, &GYNUterusVol},
    {GYN_L_FOLL1_VOL, &GYNLFOLL1Vol},
    {GYN_L_FOLL2_VOL, &GYNLFOLL2Vol},
    {GYN_L_FOLL3_VOL, &GYNLFOLL3Vol},
    {GYN_L_FOLL4_VOL, &GYNLFOLL4Vol},
    {GYN_L_FOLL5_VOL, &GYNLFOLL5Vol},
    {GYN_L_FOLL6_VOL, &GYNLFOLL6Vol},
    {GYN_L_FOLL7_VOL, &GYNLFOLL7Vol},
    {GYN_L_FOLL8_VOL, &GYNLFOLL8Vol},
    {GYN_L_FOLL9_VOL, &GYNLFOLL9Vol},
    {GYN_L_FOLL10_VOL, &GYNLFOLL10Vol},
    {GYN_R_FOLL1_VOL, &GYNRFOLL1Vol},
    {GYN_R_FOLL2_VOL, &GYNRFOLL2Vol},
    {GYN_R_FOLL3_VOL, &GYNRFOLL3Vol},
    {GYN_R_FOLL4_VOL, &GYNRFOLL4Vol},
    {GYN_R_FOLL5_VOL, &GYNRFOLL5Vol},
    {GYN_R_FOLL6_VOL, &GYNRFOLL6Vol},
    {GYN_R_FOLL7_VOL, &GYNRFOLL7Vol},
    {GYN_R_FOLL8_VOL, &GYNRFOLL8Vol},
    {GYN_R_FOLL9_VOL, &GYNRFOLL9Vol},
    {GYN_R_FOLL10_VOL, &GYNRFOLL10Vol},
/*  {GYN_FIBROID1_VOL, &GYNFIBROID1Vol},
    {GYN_FIBROID2_VOL, &GYNFIBROID2Vol},
    {GYN_FIBROID3_VOL, &GYNFIBROID3Vol},
    {GYN_FIBROID4_VOL, &GYNFIBROID4Vol},
    {GYN_FIBROID5_VOL, &GYNFIBROID5Vol},
    {GYN_FIBROID6_VOL, &GYNFIBROID6Vol},
    {GYN_FIBROID7_VOL, &GYNFIBROID7Vol},
    {GYN_FIBROID8_VOL, &GYNFIBROID8Vol},
    {GYN_FIBROID9_VOL, &GYNFIBROID9Vol},
    {GYN_FIBROID10_VOL, &GYNFIBROID10Vol}
*/
};

const MultiItemInfo GYNMultiInfo[] = {
#ifdef EMP_322
#else
    {GYN_L_OVARIAN_A, GYN_L_OVARIAN_A_MULTI, SD, "L OV A", SdInfo, NULL },
    {GYN_R_OVARIAN_A, GYN_R_OVARIAN_A_MULTI, SD, "R OV A", SdInfo, NULL },
    {GYN_L_UTERINE_A, GYN_L_UTERINE_A_MULTI, SD, "L Ut A", SdInfo, NULL },
    {GYN_R_UTERINE_A, GYN_R_UTERINE_A_MULTI, SD, "R Ut A", SdInfo, NULL },
#endif
};

const SingleItemInfo GYNInfo[] = {
    {GYN_BLADDER_VOL, MEA_TYPE_END, N_("Bladder Vol(Gyn)"), UNIT_END, NULL},
    {GYN_BLADDER_H, DIST_DOT, N_("Bladder H"), CM, &GYNBladderCalc},
    {GYN_BLADDER_L, DIST_DOT, N_("Bladder L"), CM, &GYNBladderCalc},
    {GYN_BLADDER_W, DIST_DOT, N_("Bladder W"), CM, &GYNBladderCalc},
    {GYN_PV_BLADDER_VOL, MEA_TYPE_END, N_("PV Bladder Vol(Gyn)"), UNIT_END, NULL},
    {GYN_PV_BLADDER_H, DIST_DOT, N_("PV Bladder H"), CM, &GYNPVBladderCalc},
    {GYN_PV_BLADDER_L, DIST_DOT, N_("PV Bladder L"), CM, &GYNPVBladderCalc},
    {GYN_PV_BLADDER_W, DIST_DOT, N_("PV Bladder W"), CM, &GYNPVBladderCalc},
    {GYN_L_OV_VOL, MEA_TYPE_END, N_("L Ov Vol"), UNIT_END, NULL},
    {GYN_L_OV_HEIGHT, DIST_DOT, N_("L Ov Height"), CM, &GYNLOvCalc},
    {GYN_L_OV_LENGTH, DIST_DOT, N_("L Ov Length"), CM, &GYNLOvCalc},
    {GYN_L_OV_WIDTH, DIST_DOT, N_("L Ov Width"), CM, &GYNLOvCalc},
#ifdef EMP_322
#else
    {GYN_L_OVARIAN_A, SD, N_("L Ovarian A"), GYN_L_OVARIAN_A_MULTI, NULL},
#endif
    {GYN_R_OV_VOL, MEA_TYPE_END, N_("R Ov Vol"), UNIT_END, NULL},
    {GYN_R_OV_HEIGHT, DIST_DOT, N_("R Ov Height"), CM, &GYNROvCalc},
    {GYN_R_OV_LENGTH, DIST_DOT, N_("R Ov Length"), CM, &GYNROvCalc},
    {GYN_R_OV_WIDTH, DIST_DOT, N_("R Ov Width"), CM, &GYNROvCalc},
#ifdef EMP_322
#else
    {GYN_R_OVARIAN_A, SD, N_("R Ovarian A"), GYN_R_OVARIAN_A_MULTI, NULL},
#endif
    {GYN_CERV_LENGTH, DIST_DOT, N_("Cerv Length"), CM, NULL},
    {GYN_ENDO_THICK, DIST_DOT, N_("Endo Thick"), CM, NULL},
    {GYN_UTERUS_VOL, MEA_TYPE_END, N_("Uterus Vol"), UNIT_END, NULL},
    {GYN_UTERUS_HEIGHT, DIST_DOT, N_("Uterus Height"), CM, &GYNUterusCalc},
    {GYN_UTERUS_LENGTH, DIST_DOT, N_("Uterus Length"), CM, &GYNUterusCalc},
    {GYN_UTERUS_WIDTH, DIST_DOT, N_("Uterus Width"), CM, &GYNUterusCalc},
    {GYN_L_UTERINE_A_DIAM, DIST_DOT, N_("L Uterine A Diam"), CM, NULL},
#ifdef EMP_322
#else
    {GYN_L_UTERINE_A, SD, N_("L Uterine A"), GYN_L_UTERINE_A_MULTI, NULL},
#endif
    {GYN_R_UTERINE_A_DIAM, DIST_DOT, N_("R Uterine A Diam"), CM, NULL},
#ifdef EMP_322
#else
    {GYN_R_UTERINE_A, SD, N_("R Uterine A"), GYN_R_UTERINE_A_MULTI, NULL},
#endif
    {GYN_FHR, HR_M, N_("FHR"), BPM, NULL},
    {GYN_L_FOLL1, DIST_DOT, N_("L Foll1"), CM, &GYNLFOLL1Calc},
    {GYN_L_FOLL2, DIST_DOT, N_("L Foll2"), CM, &GYNLFOLL2Calc},
    {GYN_L_FOLL3, DIST_DOT, N_("L Foll3"), CM, &GYNLFOLL3Calc},
    {GYN_L_FOLL4, DIST_DOT, N_("L Foll4"), CM, &GYNLFOLL4Calc},
    {GYN_L_FOLL5, DIST_DOT, N_("L Foll5"), CM, &GYNLFOLL5Calc},
    {GYN_L_FOLL6, DIST_DOT, N_("L Foll6"), CM, &GYNLFOLL6Calc},
    {GYN_L_FOLL7, DIST_DOT, N_("L Foll7"), CM, &GYNLFOLL7Calc},
    {GYN_L_FOLL8, DIST_DOT, N_("L Foll8"), CM, &GYNLFOLL8Calc},
    {GYN_L_FOLL9, DIST_DOT, N_("L Foll9"), CM, &GYNLFOLL9Calc},
    {GYN_L_FOLL10, DIST_DOT, N_("L Foll10"), CM, &GYNLFOLL10Calc},
    {GYN_R_FOLL1, DIST_DOT, N_("R Foll1"), CM, &GYNRFOLL1Calc},
    {GYN_R_FOLL2, DIST_DOT, N_("R Foll2"), CM, &GYNRFOLL2Calc},
    {GYN_R_FOLL3, DIST_DOT, N_("R Foll3"), CM, &GYNRFOLL3Calc},
    {GYN_R_FOLL4, DIST_DOT, N_("R Foll4"), CM, &GYNRFOLL4Calc},
    {GYN_R_FOLL5, DIST_DOT, N_("R Foll5"), CM, &GYNRFOLL5Calc},
    {GYN_R_FOLL6, DIST_DOT, N_("R Foll6"), CM, &GYNRFOLL6Calc},
    {GYN_R_FOLL7, DIST_DOT, N_("R Foll7"), CM, &GYNRFOLL7Calc},
    {GYN_R_FOLL8, DIST_DOT, N_("R Foll8"), CM, &GYNRFOLL8Calc},
    {GYN_R_FOLL9, DIST_DOT, N_("R Foll9"), CM, &GYNRFOLL9Calc},
    {GYN_R_FOLL10, DIST_DOT, N_("R Foll10"), CM, &GYNRFOLL10Calc},
    {GYN_FIBROID1, DIST_DOT, N_("Fibroid1"), CM, NULL},
    {GYN_FIBROID2, DIST_DOT, N_("Fibroid2"), CM, NULL},
    {GYN_FIBROID3, DIST_DOT, N_("Fibroid3"), CM, NULL},
    {GYN_FIBROID4, DIST_DOT, N_("Fibroid4"), CM, NULL},
    {GYN_FIBROID5, DIST_DOT, N_("Fibroid5"), CM, NULL},
    {GYN_FIBROID6, DIST_DOT, N_("Fibroid6"), CM, NULL},
    {GYN_FIBROID7, DIST_DOT, N_("Fibroid7"), CM, NULL},
    {GYN_FIBROID8, DIST_DOT, N_("Fibroid8"), CM, NULL},
    {GYN_FIBROID9, DIST_DOT, N_("Fibroid9"), CM, NULL},
    {GYN_FIBROID10, DIST_DOT, N_("Fibroid10"), CM, NULL},
};

/******************************UR Measure**************************************/
const CalcResultInfo  URBladderVol = {UR_CALC_BLADDER_VOL, {UR_BLADDER_L, UR_BLADDER_W, UR_BLADDER_H, NO_PAR, NO_PAR}, "Bladder Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  URPVBladderVol = {UR_CALC_PV_BLADDER_VOL, {UR_PV_BLADDER_L, UR_PV_BLADDER_W, UR_PV_BLADDER_H, NO_PAR, NO_PAR}, "PV Bladder Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  URProstateVol = {UR_CALC_PROSTATE_VOL, {UR_PROSTATE_L, UR_PROSTATE_W, UR_PROSTATE_H, NO_PAR, NO_PAR}, "Prostate Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  URLKidVol = {UR_CALC_L_KID_VOL, {UR_L_KID_L, UR_L_KID_W, UR_L_KID_H, NO_PAR, NO_PAR}, "L KID Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo  URRKidVol = {UR_CALC_R_KID_VOL, {UR_R_KID_L, UR_R_KID_W, UR_R_KID_H, NO_PAR, NO_PAR}, "R KID Vol", CM3, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray URBladderVolCalc = {&URBladderVol, NULL};
const CalcInfoArray URPVBladderVolCalc = {&URPVBladderVol, NULL};
const CalcInfoArray URProstateVolCalc = {&URProstateVol, NULL};
const CalcInfoArray URLKidVolCalc = {&URLKidVol, NULL};
const CalcInfoArray URRKidVolCalc = {&URRKidVol, NULL};

const PtrOfCalcInfo URCalcInfoPtr[UR_CALC_END - UR_CALC_START] = {
    {UR_CALC_BLADDER_VOL, &URBladderVol},
    {UR_CALC_PV_BLADDER_VOL, &URPVBladderVol},
    {UR_CALC_PROSTATE_VOL, &URProstateVol},
    {UR_CALC_L_KID_VOL, &URLKidVol},
    {UR_CALC_R_KID_VOL, &URRKidVol}
};

const MultiItemInfo URMultiInfo[] = {
#ifdef EMP_322
#else
    { UR_L_DIST_REN_A, UR_L_DIST_REN_A_MULTI, RI, "L Dist Ren A", RiInfo, NULL },
    { UR_L_MAIN_REN_A, UR_L_MAIN_REN_A_MULTI, RI, "L Main Ren A", RiInfo, NULL },
    { UR_L_REN_V, UR_L_REN_V_MULTI, RI, "L Ren V", RiInfo, NULL },
    { UR_R_DIST_REN_A, UR_R_DIST_REN_A_MULTI, RI, "R Dist Ren A", RiInfo, NULL },
    { UR_R_MAIN_REN_A, UR_R_MAIN_REN_A_MULTI, RI, "R Main Ren A", RiInfo, NULL },
    { UR_R_REN_V, UR_R_REN_V_MULTI, RI, "R Ren V", RiInfo, NULL },
#endif
};

const SingleItemInfo URInfo[] = {
    {UR_BLADDER_VOL, MEA_TYPE_END, N_("Bladder Vol"), UNIT_END, NULL},
    {UR_BLADDER_H, DIST_DOT, N_("Bladder H"), CM, &URBladderVolCalc},
    {UR_BLADDER_L, DIST_DOT, N_("Bladder L"), CM, &URBladderVolCalc},
    {UR_BLADDER_W, DIST_DOT, N_("Bladder W"), CM, &URBladderVolCalc},
    {UR_PV_BLADDER_VOL, MEA_TYPE_END, N_("PV Bladder Vol"), UNIT_END, NULL},
    {UR_PV_BLADDER_H, DIST_DOT, N_("PV Bladder H"), CM, &URPVBladderVolCalc},
    {UR_PV_BLADDER_L, DIST_DOT, N_("PV Bladder L"), CM, &URPVBladderVolCalc},
    {UR_PV_BLADDER_W, DIST_DOT, N_("PV Bladder W"), CM, &URPVBladderVolCalc},
    {UR_PROSTATE_VOL, MEA_TYPE_END, N_("Prostate Vol"), UNIT_END, NULL},
    {UR_PROSTATE_H, DIST_DOT, N_("Prostate H"), CM, &URProstateVolCalc},
    {UR_PROSTATE_L, DIST_DOT, N_("Prostate L"), CM, &URProstateVolCalc},
    {UR_PROSTATE_W, DIST_DOT, N_("Prostate W"), CM, &URProstateVolCalc},
    {UR_L_KID_VOL, MEA_TYPE_END, N_("L Kid Vol"), UNIT_END, NULL},
    {UR_L_KID_H, DIST_DOT, N_("L Kid H"), CM, &URLKidVolCalc},
    {UR_L_KID_L, DIST_DOT, N_("L Kid L"), CM, &URLKidVolCalc},
    {UR_L_KID_W, DIST_DOT, N_("L Kid W"), CM, &URLKidVolCalc},
#ifdef EMP_322
#else
    {UR_L_DIST_REN_A, RI, N_("L Dist Ren A"), UR_L_DIST_REN_A_MULTI, NULL},
    {UR_L_MAIN_REN_A, RI, N_("L Main Ren A"), UR_L_MAIN_REN_A_MULTI, NULL},
    {UR_L_REN_V, RI, N_("L Ren V"), UR_L_REN_V_MULTI, NULL},
#endif
    {UR_L_URETER_AP, DIST_DOT, N_("L Ureter AP"), CM, NULL},
    {UR_L_URETER_TR, DIST_DOT, N_("L Ureter Tr"), CM, NULL},
    {UR_R_KID_VOL, MEA_TYPE_END, N_("R Kid Vol"), UNIT_END, NULL},
    {UR_R_KID_H, DIST_DOT, N_("R Kid H"), CM, &URRKidVolCalc},
    {UR_R_KID_L, DIST_DOT, N_("R Kid L"), CM, &URRKidVolCalc},
    {UR_R_KID_W, DIST_DOT, N_("R Kid W"), CM, &URRKidVolCalc},
#ifdef EMP_322
#else
    {UR_R_DIST_REN_A, RI, N_("R Dist Ren A"), UR_R_DIST_REN_A_MULTI, NULL},
    {UR_R_MAIN_REN_A, RI, N_("R Main Ren A"), UR_R_MAIN_REN_A_MULTI, NULL},
    {UR_R_REN_V, RI, N_("R Ren V"), UR_R_REN_V_MULTI, NULL},
#endif
    {UR_R_URETER_AP, DIST_DOT, N_("R Ureter AP"), CM, NULL},
    {UR_R_URETER_TR, DIST_DOT, N_("R Ureter Tr"), CM, NULL},
};
#ifndef VET
/******************************SP Measure**************************************/
const CalcResultInfo SPLBreLesion1Vol = {SP_BRE_L_LESION1_VOL_C, {SP_BRE_L_LESION1_L, SP_BRE_L_LESION1_W, SP_BRE_L_LESION1_H, NO_PAR, NO_PAR}, "L Lesion1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLBreLesion2Vol = {SP_BRE_L_LESION2_VOL_C, {SP_BRE_L_LESION2_L, SP_BRE_L_LESION2_W, SP_BRE_L_LESION2_H, NO_PAR, NO_PAR}, "L Lesion2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLBreLesion3Vol = {SP_BRE_L_LESION3_VOL_C, {SP_BRE_L_LESION3_L, SP_BRE_L_LESION3_W, SP_BRE_L_LESION3_H, NO_PAR, NO_PAR}, "L Lesion3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLBreLesion4Vol = {SP_BRE_L_LESION4_VOL_C, {SP_BRE_L_LESION4_L, SP_BRE_L_LESION4_W, SP_BRE_L_LESION4_H, NO_PAR, NO_PAR}, "L Lesion4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLBreLesion5Vol = {SP_BRE_L_LESION5_VOL_C, {SP_BRE_L_LESION5_L, SP_BRE_L_LESION5_W, SP_BRE_L_LESION5_H, NO_PAR, NO_PAR}, "L Lesion5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRBreLesion1Vol = {SP_BRE_R_LESION1_VOL_C, {SP_BRE_R_LESION1_L, SP_BRE_R_LESION1_W, SP_BRE_R_LESION1_H, NO_PAR, NO_PAR}, "R Lesion1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRBreLesion2Vol = {SP_BRE_R_LESION2_VOL_C, {SP_BRE_R_LESION2_L, SP_BRE_R_LESION2_W, SP_BRE_R_LESION2_H, NO_PAR, NO_PAR}, "R Lesion2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRBreLesion3Vol = {SP_BRE_R_LESION3_VOL_C, {SP_BRE_R_LESION3_L, SP_BRE_R_LESION3_W, SP_BRE_R_LESION3_H, NO_PAR, NO_PAR}, "R Lesion3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRBreLesion4Vol = {SP_BRE_R_LESION4_VOL_C, {SP_BRE_R_LESION4_L, SP_BRE_R_LESION4_W, SP_BRE_R_LESION4_H, NO_PAR, NO_PAR}, "R Lesion4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRBreLesion5Vol = {SP_BRE_R_LESION5_VOL_C, {SP_BRE_R_LESION5_L, SP_BRE_R_LESION5_W, SP_BRE_R_LESION5_H, NO_PAR, NO_PAR}, "R Lesion5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
#endif

#ifdef VET //Ve

const CalcResultInfo SPBre1Lesion1Vol = {SP_BRE1_LESION1_VOL_C, {SP_BRE1_LESION1_L, SP_BRE1_LESION1_W, SP_BRE1_LESION1_H, NO_PAR, NO_PAR}, "Bre1 Le1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre1Lesion2Vol = {SP_BRE1_LESION2_VOL_C, {SP_BRE1_LESION2_L, SP_BRE1_LESION2_W, SP_BRE1_LESION2_H, NO_PAR, NO_PAR}, "Bre1 Le2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre1Lesion3Vol = {SP_BRE1_LESION3_VOL_C, {SP_BRE1_LESION3_L, SP_BRE1_LESION3_W, SP_BRE1_LESION3_H, NO_PAR, NO_PAR}, "Bre1 Le3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre1Lesion4Vol = {SP_BRE1_LESION4_VOL_C, {SP_BRE1_LESION4_L, SP_BRE1_LESION4_W, SP_BRE1_LESION4_H, NO_PAR, NO_PAR}, "Bre1 Le4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre1Lesion5Vol = {SP_BRE1_LESION5_VOL_C, {SP_BRE1_LESION5_L, SP_BRE1_LESION5_W, SP_BRE1_LESION5_H, NO_PAR, NO_PAR}, "Bre1 Le5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre2Lesion1Vol = {SP_BRE2_LESION1_VOL_C, {SP_BRE2_LESION1_L, SP_BRE2_LESION1_W, SP_BRE2_LESION1_H, NO_PAR, NO_PAR}, "Bre2 Le1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre2Lesion2Vol = {SP_BRE2_LESION2_VOL_C, {SP_BRE2_LESION2_L, SP_BRE2_LESION2_W, SP_BRE2_LESION2_H, NO_PAR, NO_PAR}, "Bre2 Le2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre2Lesion3Vol = {SP_BRE2_LESION3_VOL_C, {SP_BRE2_LESION3_L, SP_BRE2_LESION3_W, SP_BRE2_LESION3_H, NO_PAR, NO_PAR}, "Bre2 Le3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre2Lesion4Vol = {SP_BRE2_LESION4_VOL_C, {SP_BRE2_LESION4_L, SP_BRE2_LESION4_W, SP_BRE2_LESION4_H, NO_PAR, NO_PAR}, "Bre2 Le4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre2Lesion5Vol = {SP_BRE2_LESION5_VOL_C, {SP_BRE2_LESION5_L, SP_BRE2_LESION5_W, SP_BRE2_LESION5_H, NO_PAR, NO_PAR}, "Bre2 Le5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre3Lesion1Vol = {SP_BRE3_LESION1_VOL_C, {SP_BRE3_LESION1_L, SP_BRE3_LESION1_W, SP_BRE3_LESION1_H, NO_PAR, NO_PAR}, "Bre3 Le1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre3Lesion2Vol = {SP_BRE3_LESION2_VOL_C, {SP_BRE3_LESION2_L, SP_BRE3_LESION2_W, SP_BRE3_LESION2_H, NO_PAR, NO_PAR}, "Bre3 Le2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre3Lesion3Vol = {SP_BRE3_LESION3_VOL_C, {SP_BRE3_LESION3_L, SP_BRE3_LESION3_W, SP_BRE3_LESION3_H, NO_PAR, NO_PAR}, "Bre3 Le3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre3Lesion4Vol = {SP_BRE3_LESION4_VOL_C, {SP_BRE3_LESION4_L, SP_BRE3_LESION4_W, SP_BRE3_LESION4_H, NO_PAR, NO_PAR}, "Bre3 Le4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre3Lesion5Vol = {SP_BRE3_LESION5_VOL_C, {SP_BRE3_LESION5_L, SP_BRE3_LESION5_W, SP_BRE3_LESION5_H, NO_PAR, NO_PAR}, "Bre3 Le5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre4Lesion1Vol = {SP_BRE4_LESION1_VOL_C, {SP_BRE4_LESION1_L, SP_BRE4_LESION1_W, SP_BRE4_LESION1_H, NO_PAR, NO_PAR}, "Bre4 Le1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre4Lesion2Vol = {SP_BRE4_LESION2_VOL_C, {SP_BRE4_LESION2_L, SP_BRE4_LESION2_W, SP_BRE4_LESION2_H, NO_PAR, NO_PAR}, "Bre4 Le2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre4Lesion3Vol = {SP_BRE4_LESION3_VOL_C, {SP_BRE4_LESION3_L, SP_BRE4_LESION3_W, SP_BRE4_LESION3_H, NO_PAR, NO_PAR}, "Bre4 Le3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre4Lesion4Vol = {SP_BRE4_LESION4_VOL_C, {SP_BRE4_LESION4_L, SP_BRE4_LESION4_W, SP_BRE4_LESION4_H, NO_PAR, NO_PAR}, "Bre4 Le4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre4Lesion5Vol = {SP_BRE4_LESION5_VOL_C, {SP_BRE4_LESION5_L, SP_BRE4_LESION5_W, SP_BRE4_LESION5_H, NO_PAR, NO_PAR}, "Bre4 Le5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre5Lesion1Vol = {SP_BRE5_LESION1_VOL_C, {SP_BRE5_LESION1_L, SP_BRE5_LESION1_W, SP_BRE5_LESION1_H, NO_PAR, NO_PAR}, "Bre5 Le1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre5Lesion2Vol = {SP_BRE5_LESION2_VOL_C, {SP_BRE5_LESION2_L, SP_BRE5_LESION2_W, SP_BRE5_LESION2_H, NO_PAR, NO_PAR}, "Bre5 Le2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre5Lesion3Vol = {SP_BRE5_LESION3_VOL_C, {SP_BRE5_LESION3_L, SP_BRE5_LESION3_W, SP_BRE5_LESION3_H, NO_PAR, NO_PAR}, "Bre5 Le3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre5Lesion4Vol = {SP_BRE5_LESION4_VOL_C, {SP_BRE5_LESION4_L, SP_BRE5_LESION4_W, SP_BRE5_LESION4_H, NO_PAR, NO_PAR}, "Bre5 Le4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPBre5Lesion5Vol = {SP_BRE5_LESION5_VOL_C, {SP_BRE5_LESION5_L, SP_BRE5_LESION5_W, SP_BRE5_LESION5_H, NO_PAR, NO_PAR}, "Bre5 Le5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
#endif
const CalcResultInfo SPLTesVol = {SP_L_TESTIS_VOL_C, {SP_L_TESTIS_L, SP_L_TESTIS_W, SP_L_TESTIS_H, NO_PAR, NO_PAR}, "L Tes Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRTesVol = {SP_R_TESTIS_VOL_C, {SP_R_TESTIS_L, SP_R_TESTIS_W, SP_R_TESTIS_H, NO_PAR, NO_PAR}, "R Tes Vol", CM3, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo SPLTesLesion1Vol = {SP_TES_L_LESION1_VOL_C, {SP_TES_L_LESION1_L, SP_TES_L_LESION1_W, SP_TES_L_LESION1_H, NO_PAR, NO_PAR}, "L Lesion1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLTesLesion2Vol = {SP_TES_L_LESION2_VOL_C, {SP_TES_L_LESION2_L, SP_TES_L_LESION2_W, SP_TES_L_LESION2_H, NO_PAR, NO_PAR}, "L Lesion2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLTesLesion3Vol = {SP_TES_L_LESION3_VOL_C, {SP_TES_L_LESION3_L, SP_TES_L_LESION3_W, SP_TES_L_LESION3_H, NO_PAR, NO_PAR}, "L Lesion3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLTesLesion4Vol = {SP_TES_L_LESION4_VOL_C, {SP_TES_L_LESION4_L, SP_TES_L_LESION4_W, SP_TES_L_LESION4_H, NO_PAR, NO_PAR}, "L Lesion4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPLTesLesion5Vol = {SP_TES_L_LESION5_VOL_C, {SP_TES_L_LESION5_L, SP_TES_L_LESION5_W, SP_TES_L_LESION5_H, NO_PAR, NO_PAR}, "L Lesion5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRTesLesion1Vol = {SP_TES_R_LESION1_VOL_C, {SP_TES_R_LESION1_L, SP_TES_R_LESION1_W, SP_TES_R_LESION1_H, NO_PAR, NO_PAR}, "R Lesion1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRTesLesion2Vol = {SP_TES_R_LESION2_VOL_C, {SP_TES_R_LESION2_L, SP_TES_R_LESION2_W, SP_TES_R_LESION2_H, NO_PAR, NO_PAR}, "R Lesion2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRTesLesion3Vol = {SP_TES_R_LESION3_VOL_C, {SP_TES_R_LESION3_L, SP_TES_R_LESION3_W, SP_TES_R_LESION3_H, NO_PAR, NO_PAR}, "R Lesion3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRTesLesion4Vol = {SP_TES_R_LESION4_VOL_C, {SP_TES_R_LESION4_L, SP_TES_R_LESION4_W, SP_TES_R_LESION4_H, NO_PAR, NO_PAR}, "R Lesion4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPRTesLesion5Vol = {SP_TES_R_LESION5_VOL_C, {SP_TES_R_LESION5_L, SP_TES_R_LESION5_W, SP_TES_R_LESION5_H, NO_PAR, NO_PAR}, "R Lesion5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};

const CalcResultInfo SPThyLesion1Vol = {SP_THY_LESION1_VOL_C, {SP_THY_LESION1_L, SP_THY_LESION1_W, SP_THY_LESION1_H, NO_PAR, NO_PAR}, "Lesion1 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPThyLesion2Vol = {SP_THY_LESION2_VOL_C, {SP_THY_LESION2_L, SP_THY_LESION2_W, SP_THY_LESION2_H, NO_PAR, NO_PAR}, "Lesion2 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPThyLesion3Vol = {SP_THY_LESION3_VOL_C, {SP_THY_LESION3_L, SP_THY_LESION3_W, SP_THY_LESION3_H, NO_PAR, NO_PAR}, "Lesion3 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPThyLesion4Vol = {SP_THY_LESION4_VOL_C, {SP_THY_LESION4_L, SP_THY_LESION4_W, SP_THY_LESION4_H, NO_PAR, NO_PAR}, "Lesion4 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo SPThyLesion5Vol = {SP_THY_LESION5_VOL_C, {SP_THY_LESION5_L, SP_THY_LESION5_W, SP_THY_LESION5_H, NO_PAR, NO_PAR}, "Lesion5 Vol", CM3, MeaCalcFun::SectionCalc, NULL};
#ifdef VET
#else
const CalcInfoArray SPLBreLesion1VolCalc = {&SPLBreLesion1Vol, NULL};
const CalcInfoArray SPLBreLesion2VolCalc = {&SPLBreLesion2Vol, NULL};
const CalcInfoArray SPLBreLesion3VolCalc = {&SPLBreLesion3Vol, NULL};
const CalcInfoArray SPLBreLesion4VolCalc = {&SPLBreLesion4Vol, NULL};
const CalcInfoArray SPLBreLesion5VolCalc = {&SPLBreLesion5Vol, NULL};
const CalcInfoArray SPRBreLesion1VolCalc = {&SPRBreLesion1Vol, NULL};
const CalcInfoArray SPRBreLesion2VolCalc = {&SPRBreLesion2Vol, NULL};
const CalcInfoArray SPRBreLesion3VolCalc = {&SPRBreLesion3Vol, NULL};
const CalcInfoArray SPRBreLesion4VolCalc = {&SPRBreLesion4Vol, NULL};
const CalcInfoArray SPRBreLesion5VolCalc = {&SPRBreLesion5Vol, NULL};
#endif
#ifdef VET //Vet

const CalcInfoArray SPBre1Lesion1VolCalc = {&SPBre1Lesion1Vol, NULL};
const CalcInfoArray SPBre1Lesion2VolCalc = {&SPBre1Lesion2Vol, NULL};
const CalcInfoArray SPBre1Lesion3VolCalc = {&SPBre1Lesion3Vol, NULL};
const CalcInfoArray SPBre1Lesion4VolCalc = {&SPBre1Lesion4Vol, NULL};
const CalcInfoArray SPBre1Lesion5VolCalc = {&SPBre1Lesion5Vol, NULL};
const CalcInfoArray SPBre2Lesion1VolCalc = {&SPBre2Lesion1Vol, NULL};
const CalcInfoArray SPBre2Lesion2VolCalc = {&SPBre2Lesion2Vol, NULL};
const CalcInfoArray SPBre2Lesion3VolCalc = {&SPBre2Lesion3Vol, NULL};
const CalcInfoArray SPBre2Lesion4VolCalc = {&SPBre2Lesion4Vol, NULL};
const CalcInfoArray SPBre2Lesion5VolCalc = {&SPBre2Lesion5Vol, NULL};
const CalcInfoArray SPBre3Lesion1VolCalc = {&SPBre3Lesion1Vol, NULL};
const CalcInfoArray SPBre3Lesion2VolCalc = {&SPBre3Lesion2Vol, NULL};
const CalcInfoArray SPBre3Lesion3VolCalc = {&SPBre3Lesion3Vol, NULL};
const CalcInfoArray SPBre3Lesion4VolCalc = {&SPBre3Lesion4Vol, NULL};
const CalcInfoArray SPBre3Lesion5VolCalc = {&SPBre3Lesion5Vol, NULL};
const CalcInfoArray SPBre4Lesion1VolCalc = {&SPBre4Lesion1Vol, NULL};
const CalcInfoArray SPBre4Lesion2VolCalc = {&SPBre4Lesion2Vol, NULL};
const CalcInfoArray SPBre4Lesion3VolCalc = {&SPBre4Lesion3Vol, NULL};
const CalcInfoArray SPBre4Lesion4VolCalc = {&SPBre4Lesion4Vol, NULL};
const CalcInfoArray SPBre4Lesion5VolCalc = {&SPBre4Lesion5Vol, NULL};
const CalcInfoArray SPBre5Lesion1VolCalc = {&SPBre5Lesion1Vol, NULL};
const CalcInfoArray SPBre5Lesion2VolCalc = {&SPBre5Lesion2Vol, NULL};
const CalcInfoArray SPBre5Lesion3VolCalc = {&SPBre5Lesion3Vol, NULL};
const CalcInfoArray SPBre5Lesion4VolCalc = {&SPBre5Lesion4Vol, NULL};
const CalcInfoArray SPBre5Lesion5VolCalc = {&SPBre5Lesion5Vol, NULL};
#endif
const CalcInfoArray SPLTesVolCalc = {&SPLTesVol, NULL};
const CalcInfoArray SPRTesVolCalc = {&SPRTesVol, NULL};
const CalcInfoArray SPLTesLesion1VolCalc = {&SPLTesLesion1Vol, NULL};
const CalcInfoArray SPLTesLesion2VolCalc = {&SPLTesLesion2Vol, NULL};
const CalcInfoArray SPLTesLesion3VolCalc = {&SPLTesLesion3Vol, NULL};
const CalcInfoArray SPLTesLesion4VolCalc = {&SPLTesLesion4Vol, NULL};
const CalcInfoArray SPLTesLesion5VolCalc = {&SPLTesLesion5Vol, NULL};
const CalcInfoArray SPRTesLesion1VolCalc = {&SPRTesLesion1Vol, NULL};
const CalcInfoArray SPRTesLesion2VolCalc = {&SPRTesLesion2Vol, NULL};
const CalcInfoArray SPRTesLesion3VolCalc = {&SPRTesLesion3Vol, NULL};
const CalcInfoArray SPRTesLesion4VolCalc = {&SPRTesLesion4Vol, NULL};
const CalcInfoArray SPRTesLesion5VolCalc = {&SPRTesLesion5Vol, NULL};
const CalcInfoArray SPThyLesion1VolCalc = {&SPThyLesion1Vol, NULL};
const CalcInfoArray SPThyLesion2VolCalc = {&SPThyLesion2Vol, NULL};
const CalcInfoArray SPThyLesion3VolCalc = {&SPThyLesion3Vol, NULL};
const CalcInfoArray SPThyLesion4VolCalc = {&SPThyLesion4Vol, NULL};
const CalcInfoArray SPThyLesion5VolCalc = {&SPThyLesion5Vol, NULL};
#ifndef VET
const PtrOfCalcInfo SPCalcInfoPtr[SP_CALC_END - SP_CALC_START] = {
    {SP_BRE_L_LESION1_VOL_C, &SPLBreLesion1Vol},
    {SP_BRE_L_LESION2_VOL_C, &SPLBreLesion2Vol},
    {SP_BRE_L_LESION3_VOL_C, &SPLBreLesion3Vol},
    {SP_BRE_L_LESION4_VOL_C, &SPLBreLesion4Vol},
    {SP_BRE_L_LESION5_VOL_C, &SPLBreLesion5Vol},
    {SP_BRE_R_LESION1_VOL_C, &SPRBreLesion1Vol},
    {SP_BRE_R_LESION2_VOL_C, &SPRBreLesion2Vol},
    {SP_BRE_R_LESION3_VOL_C, &SPRBreLesion3Vol},
    {SP_BRE_R_LESION4_VOL_C, &SPRBreLesion4Vol},
    {SP_BRE_R_LESION5_VOL_C, &SPRBreLesion5Vol},
    {SP_L_TESTIS_VOL_C, &SPLTesVol},
    {SP_R_TESTIS_VOL_C, &SPRTesVol},
    {SP_TES_L_LESION1_VOL_C, &SPLTesLesion1Vol},
    {SP_TES_L_LESION2_VOL_C, &SPLTesLesion2Vol},
    {SP_TES_L_LESION3_VOL_C, &SPLTesLesion3Vol},
    {SP_TES_L_LESION4_VOL_C, &SPLTesLesion4Vol},
    {SP_TES_L_LESION5_VOL_C, &SPLTesLesion5Vol},
    {SP_TES_R_LESION1_VOL_C, &SPRTesLesion1Vol},
    {SP_TES_R_LESION2_VOL_C, &SPRTesLesion2Vol},
    {SP_TES_R_LESION3_VOL_C, &SPRTesLesion3Vol},
    {SP_TES_R_LESION4_VOL_C, &SPRTesLesion4Vol},
    {SP_TES_R_LESION5_VOL_C, &SPRTesLesion5Vol},
    {SP_THY_LESION1_VOL_C, &SPThyLesion1Vol},
    {SP_THY_LESION2_VOL_C, &SPThyLesion2Vol},
    {SP_THY_LESION3_VOL_C, &SPThyLesion3Vol},
    {SP_THY_LESION4_VOL_C, &SPThyLesion4Vol},
    {SP_THY_LESION5_VOL_C, &SPThyLesion5Vol},
};
#endif  //Vet
#ifdef VET
const PtrOfCalcInfo SPCalcInfoPtr[SP_CALC_END - SP_CALC_START] = {
    {SP_BRE1_LESION1_VOL_C, &SPBre1Lesion1Vol},
    {SP_BRE1_LESION2_VOL_C, &SPBre1Lesion2Vol},
    {SP_BRE1_LESION3_VOL_C, &SPBre1Lesion3Vol},
    {SP_BRE1_LESION4_VOL_C, &SPBre1Lesion4Vol},
    {SP_BRE1_LESION5_VOL_C, &SPBre1Lesion5Vol},
    {SP_BRE2_LESION1_VOL_C, &SPBre2Lesion1Vol},
    {SP_BRE2_LESION2_VOL_C, &SPBre2Lesion2Vol},
    {SP_BRE2_LESION3_VOL_C, &SPBre2Lesion3Vol},
    {SP_BRE2_LESION4_VOL_C, &SPBre2Lesion4Vol},
    {SP_BRE2_LESION5_VOL_C, &SPBre2Lesion5Vol},
    {SP_BRE3_LESION1_VOL_C, &SPBre3Lesion1Vol},
    {SP_BRE3_LESION2_VOL_C, &SPBre3Lesion2Vol},
    {SP_BRE3_LESION3_VOL_C, &SPBre3Lesion3Vol},
    {SP_BRE3_LESION4_VOL_C, &SPBre3Lesion4Vol},
    {SP_BRE3_LESION5_VOL_C, &SPBre3Lesion5Vol},
    {SP_BRE4_LESION1_VOL_C, &SPBre4Lesion1Vol},
    {SP_BRE4_LESION2_VOL_C, &SPBre4Lesion2Vol},
    {SP_BRE4_LESION3_VOL_C, &SPBre4Lesion3Vol},
    {SP_BRE4_LESION4_VOL_C, &SPBre4Lesion4Vol},
    {SP_BRE4_LESION5_VOL_C, &SPBre4Lesion5Vol},
    {SP_BRE5_LESION1_VOL_C, &SPBre5Lesion1Vol},
    {SP_BRE5_LESION2_VOL_C, &SPBre5Lesion2Vol},
    {SP_BRE5_LESION3_VOL_C, &SPBre5Lesion3Vol},
    {SP_BRE5_LESION4_VOL_C, &SPBre5Lesion4Vol},
    {SP_BRE5_LESION5_VOL_C, &SPBre5Lesion5Vol},
    {SP_L_TESTIS_VOL_C, &SPLTesVol},
    {SP_R_TESTIS_VOL_C, &SPRTesVol},
    {SP_TES_L_LESION1_VOL_C, &SPLTesLesion1Vol},
    {SP_TES_L_LESION2_VOL_C, &SPLTesLesion2Vol},
    {SP_TES_L_LESION3_VOL_C, &SPLTesLesion3Vol},
    {SP_TES_L_LESION4_VOL_C, &SPLTesLesion4Vol},
    {SP_TES_L_LESION5_VOL_C, &SPLTesLesion5Vol},
    {SP_TES_R_LESION1_VOL_C, &SPRTesLesion1Vol},
    {SP_TES_R_LESION2_VOL_C, &SPRTesLesion2Vol},
    {SP_TES_R_LESION3_VOL_C, &SPRTesLesion3Vol},
    {SP_TES_R_LESION4_VOL_C, &SPRTesLesion4Vol},
    {SP_TES_R_LESION5_VOL_C, &SPRTesLesion5Vol},
    {SP_THY_LESION1_VOL_C, &SPThyLesion1Vol},
    {SP_THY_LESION2_VOL_C, &SPThyLesion2Vol},
    {SP_THY_LESION3_VOL_C, &SPThyLesion3Vol},
    {SP_THY_LESION4_VOL_C, &SPThyLesion4Vol},
    {SP_THY_LESION5_VOL_C, &SPThyLesion5Vol},
};
#endif
const MultiItemInfo SPMultiInfo[] = {
#ifdef EMP_322
#else
    { SP_L_TESTIS_ARTERY, SP_L_TESTIS_ARTERY_MULTI, SD, "L Testis A", SdInfo, NULL },
    { SP_R_TESTIS_ARTERY, SP_R_TESTIS_ARTERY_MULTI, SD, "R Testis A", SdInfo, NULL },
    { SP_SUPA_L_LOBE, SP_SUPA_L_LOBE_MULTI, SD, "Supa L Lobe", SdInfo, NULL },
    { SP_INFA_L_LOBE, SP_INFA_L_LOBE_MULTI, SD, "Infa L Lobe", SdInfo, NULL },
    { SP_SUPA_R_LOBE, SP_SUPA_R_LOBE_MULTI, SD, "Supa R Lobe", SdInfo, NULL },
    { SP_INFA_R_LOBE, SP_INFA_R_LOBE_MULTI, SD, "Infa R Lobe", SdInfo, NULL },
#endif
};
#ifndef VET

const SingleItemInfo SPInfo[] = {
    {SP_L_GALACT_THICK, DIST_DOT, N_("L Galact Thick"), CM, NULL},
    {SP_R_GALACT_THICK, DIST_DOT, N_("R Galact Thick"), CM, NULL},
    {SP_BRE_L_LESION1_VOL, MEA_TYPE_END, N_("Breast L Lesion1 Vol"), UNIT_END, NULL},
    {SP_BRE_L_LESION1_L, DIST_DOT, N_("Breast L Lesion1 L"), CM, &SPLBreLesion1VolCalc},
    {SP_BRE_L_LESION1_W, DIST_DOT, N_("Breast L Lesion1 W"), CM, &SPLBreLesion1VolCalc},
    {SP_BRE_L_LESION1_H, DIST_DOT, N_("Breast L Lesion1 H"), CM, &SPLBreLesion1VolCalc},
    {SP_BRE_L_LESION2_VOL, MEA_TYPE_END, N_("Breast L Lesion2 Vol"), UNIT_END, NULL},
    {SP_BRE_L_LESION2_L, DIST_DOT, N_("Breast L Lesion2 L"), CM, &SPLBreLesion2VolCalc},
    {SP_BRE_L_LESION2_W, DIST_DOT, N_("Breast L Lesion2 W"), CM, &SPLBreLesion2VolCalc},
    {SP_BRE_L_LESION2_H, DIST_DOT, N_("Breast L Lesion2 H"), CM, &SPLBreLesion2VolCalc},
    {SP_BRE_L_LESION3_VOL, MEA_TYPE_END, N_("Breast L Lesion3 Vol"), UNIT_END, NULL},
    {SP_BRE_L_LESION3_L, DIST_DOT, N_("Breast L Lesion3 L"), CM, &SPLBreLesion3VolCalc},
    {SP_BRE_L_LESION3_W, DIST_DOT, N_("Breast L Lesion3 W"), CM, &SPLBreLesion3VolCalc},
    {SP_BRE_L_LESION3_H, DIST_DOT, N_("Breast L Lesion3 H"), CM, &SPLBreLesion3VolCalc},
    {SP_BRE_L_LESION4_VOL, MEA_TYPE_END, N_("Breast L Lesion4 Vol"), UNIT_END, NULL},
    {SP_BRE_L_LESION4_L, DIST_DOT, N_("Breast L Lesion4 L"), CM, &SPLBreLesion4VolCalc},
    {SP_BRE_L_LESION4_W, DIST_DOT, N_("Breast L Lesion4 W"), CM, &SPLBreLesion4VolCalc},
    {SP_BRE_L_LESION4_H, DIST_DOT, N_("Breast L Lesion4 H"), CM, &SPLBreLesion4VolCalc},
    {SP_BRE_L_LESION5_VOL, MEA_TYPE_END,N_( "Breast L Lesion5 Vol"), UNIT_END, NULL},
    {SP_BRE_L_LESION5_L, DIST_DOT, N_("Breast L Lesion5 L"), CM, &SPLBreLesion5VolCalc},
    {SP_BRE_L_LESION5_W, DIST_DOT, N_("Breast L Lesion5 W"), CM, &SPLBreLesion5VolCalc},
    {SP_BRE_L_LESION5_H, DIST_DOT, N_("Breast L Lesion5 H"), CM, &SPLBreLesion5VolCalc},
    {SP_BRE_R_LESION1_VOL, MEA_TYPE_END, N_("Breast R Lesion1 Vol"), UNIT_END, NULL},
    {SP_BRE_R_LESION1_L, DIST_DOT, N_("Breast R Lesion1 L"), CM, &SPRBreLesion1VolCalc},
    {SP_BRE_R_LESION1_W, DIST_DOT, N_("Breast R Lesion1 W"), CM, &SPRBreLesion1VolCalc},
    {SP_BRE_R_LESION1_H, DIST_DOT, N_("Breast R Lesion1 H"), CM, &SPRBreLesion1VolCalc},
    {SP_BRE_R_LESION2_VOL, MEA_TYPE_END, N_("Breast R Lesion2 Vol"), UNIT_END, NULL},
    {SP_BRE_R_LESION2_L, DIST_DOT, N_("Breast R Lesion2 L"), CM, &SPRBreLesion2VolCalc},
    {SP_BRE_R_LESION2_W, DIST_DOT, N_("Breast R Lesion2 W"), CM, &SPRBreLesion2VolCalc},
    {SP_BRE_R_LESION2_H, DIST_DOT, N_("Breast R Lesion2 H"), CM, &SPRBreLesion2VolCalc},
    {SP_BRE_R_LESION3_VOL, MEA_TYPE_END, N_("Breast R Lesion3 Vol"), UNIT_END, NULL},
    {SP_BRE_R_LESION3_L, DIST_DOT, N_("Breast R Lesion3 L"), CM, &SPRBreLesion3VolCalc},
    {SP_BRE_R_LESION3_W, DIST_DOT, N_("Breast R Lesion3 W"), CM, &SPRBreLesion3VolCalc},
    {SP_BRE_R_LESION3_H, DIST_DOT, N_("Breast R Lesion3 H"), CM, &SPRBreLesion3VolCalc},
    {SP_BRE_R_LESION4_VOL, MEA_TYPE_END, N_("Breast R Lesion4 Vol"), UNIT_END, NULL},
    {SP_BRE_R_LESION4_L, DIST_DOT, N_("Breast R Lesion4 L"), CM, &SPRBreLesion4VolCalc},
    {SP_BRE_R_LESION4_W, DIST_DOT, N_("Breast R Lesion4 W"), CM, &SPRBreLesion4VolCalc},
    {SP_BRE_R_LESION4_H, DIST_DOT, N_("Breast R Lesion4 H"), CM, &SPRBreLesion4VolCalc},
    {SP_BRE_R_LESION5_VOL, MEA_TYPE_END, N_("Breast R Lesion5 Vol"), UNIT_END, NULL},
    {SP_BRE_R_LESION5_L, DIST_DOT, N_("Breast R Lesion5 L"), CM, &SPRBreLesion5VolCalc},
    {SP_BRE_R_LESION5_W, DIST_DOT, N_("Breast R Lesion5 W"), CM, &SPRBreLesion5VolCalc},
    {SP_BRE_R_LESION5_H, DIST_DOT, N_("Breast R Lesion5 H"), CM, &SPRBreLesion5VolCalc},
    {SP_L_TESTIS_VOL, MEA_TYPE_END,N_( "L Testis Vol"), UNIT_END, NULL},
    {SP_L_TESTIS_L, DIST_DOT, N_("L Testis L"), CM, &SPLTesVolCalc},
    {SP_L_TESTIS_W, DIST_DOT, N_("L Testis W"), CM, &SPLTesVolCalc},
    {SP_L_TESTIS_H, DIST_DOT, N_("L Testis H"), CM, &SPLTesVolCalc},
#ifdef EMP_322
#else
    {SP_L_TESTIS_ARTERY, SD, N_("L Testis Artery"), SP_L_TESTIS_ARTERY_MULTI, NULL},
#endif
    {SP_R_TESTIS_VOL, MEA_TYPE_END, N_("R Testis Vol"), UNIT_END, NULL},
    {SP_R_TESTIS_L, DIST_DOT, N_("R Testis L"), CM, &SPRTesVolCalc},
    {SP_R_TESTIS_W, DIST_DOT, N_("R Testis W"), CM, &SPRTesVolCalc},
    {SP_R_TESTIS_H, DIST_DOT, N_("R Testis H"), CM, &SPRTesVolCalc},
#ifdef EMP_322
#else
    {SP_R_TESTIS_ARTERY, SD, N_("R Testis Artery"), SP_R_TESTIS_ARTERY_MULTI, NULL},
#endif
    {SP_TES_L_LESION1_VOL, MEA_TYPE_END, N_("Testis L Lesion1 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION1_L, DIST_DOT, N_("Testis L Lesion1 L"), CM, &SPLTesLesion1VolCalc},
    {SP_TES_L_LESION1_W, DIST_DOT, N_("Testis L Lesion1 W"), CM, &SPLTesLesion1VolCalc},
    {SP_TES_L_LESION1_H, DIST_DOT, N_("Testis L Lesion1 H"), CM, &SPLTesLesion1VolCalc},
    {SP_TES_L_LESION2_VOL, MEA_TYPE_END, N_("Testis L Lesion2 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION2_L, DIST_DOT, N_("Testis L Lesion2 L"), CM, &SPLTesLesion2VolCalc},
    {SP_TES_L_LESION2_W, DIST_DOT, N_("Testis L Lesion2 W"), CM, &SPLTesLesion2VolCalc},
    {SP_TES_L_LESION2_H, DIST_DOT, N_("Testis L Lesion2 H"), CM, &SPLTesLesion2VolCalc},
    {SP_TES_L_LESION3_VOL, MEA_TYPE_END, N_("Testis L Lesion3 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION3_L, DIST_DOT, N_("Testis L Lesion3 L"), CM, &SPLTesLesion3VolCalc},
    {SP_TES_L_LESION3_W, DIST_DOT, N_("Testis L Lesion3 W"), CM, &SPLTesLesion3VolCalc},
    {SP_TES_L_LESION3_H, DIST_DOT, N_("Testis L Lesion3 H"), CM, &SPLTesLesion3VolCalc},
    {SP_TES_L_LESION4_VOL, MEA_TYPE_END, N_("Testis L Lesion4 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION4_L, DIST_DOT, N_("Testis L Lesion4 L"), CM, &SPLTesLesion4VolCalc},
    {SP_TES_L_LESION4_W, DIST_DOT, N_("Testis L Lesion4 W"), CM, &SPLTesLesion4VolCalc},
    {SP_TES_L_LESION4_H, DIST_DOT, N_("Testis L Lesion4 H"), CM, &SPLTesLesion4VolCalc},
    {SP_TES_L_LESION5_VOL, MEA_TYPE_END, N_("Testis L Lesion5 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION5_L, DIST_DOT, N_("Testis L Lesion5 L"), CM, &SPLTesLesion5VolCalc},
    {SP_TES_L_LESION5_W, DIST_DOT, N_("Testis L Lesion5 W"), CM, &SPLTesLesion5VolCalc},
    {SP_TES_L_LESION5_H, DIST_DOT, N_("Testis L Lesion5 H"), CM, &SPLTesLesion5VolCalc},
    {SP_TES_R_LESION1_VOL, MEA_TYPE_END, N_("Testis R Lesion1 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION1_L, DIST_DOT, N_("Testis R Lesion1 L"), CM, &SPRTesLesion1VolCalc},
    {SP_TES_R_LESION1_W, DIST_DOT, N_("Testis R Lesion1 W"), CM, &SPRTesLesion1VolCalc},
    {SP_TES_R_LESION1_H, DIST_DOT, N_("Testis R Lesion1 H"), CM, &SPRTesLesion1VolCalc},
    {SP_TES_R_LESION2_VOL, MEA_TYPE_END, N_("Testis R Lesion2 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION2_L, DIST_DOT, N_("Testis R Lesion2 L"), CM, &SPRTesLesion2VolCalc},
    {SP_TES_R_LESION2_W, DIST_DOT, N_("Testis R Lesion2 W"), CM, &SPRTesLesion2VolCalc},
    {SP_TES_R_LESION2_H, DIST_DOT, N_("Testis R Lesion2 H"), CM, &SPRTesLesion2VolCalc},
    {SP_TES_R_LESION3_VOL, MEA_TYPE_END, N_("Testis R Lesion3 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION3_L, DIST_DOT, N_("Testis R Lesion3 L"), CM, &SPRTesLesion3VolCalc},
    {SP_TES_R_LESION3_W, DIST_DOT, N_("Testis R Lesion3 W"), CM, &SPRTesLesion3VolCalc},
    {SP_TES_R_LESION3_H, DIST_DOT, N_("Testis R Lesion3 H"), CM, &SPRTesLesion3VolCalc},
    {SP_TES_R_LESION4_VOL, MEA_TYPE_END, N_("Testis R Lesion4 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION4_L, DIST_DOT, N_("Testis R Lesion4 L"), CM, &SPRTesLesion4VolCalc},
    {SP_TES_R_LESION4_W, DIST_DOT, N_("Testis R Lesion4 W"), CM, &SPRTesLesion4VolCalc},
    {SP_TES_R_LESION4_H, DIST_DOT, N_("Testis R Lesion4 H"), CM, &SPRTesLesion4VolCalc},
    {SP_TES_R_LESION5_VOL, MEA_TYPE_END, N_("Testis R Lesion5 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION5_L, DIST_DOT, N_("Testis R Lesion5 L"), CM, &SPRTesLesion5VolCalc},
    {SP_TES_R_LESION5_W, DIST_DOT, N_("Testis R Lesion5 W"), CM, &SPRTesLesion5VolCalc},
    {SP_TES_R_LESION5_H, DIST_DOT, N_("Testis R Lesion5 H"), CM, &SPRTesLesion5VolCalc},
    {SP_ISTHMUS_AP, DIST_DOT, N_("Isthmus AP"), CM, NULL},
    {SP_LONG_L_LOBE, DIST_DOT, N_("Long L Lobe"), CM, NULL},
    {SP_AP_L_LOBE, DIST_DOT, N_("AP L Lobe"), CM, NULL},
    {SP_TR_L_LOBE, DIST_DOT, N_("Tr L Lobe"), CM, NULL},
    {SP_LONG_R_LOBE, DIST_DOT, N_("Long R Lobe"), CM, NULL},
    {SP_AP_R_LOBE, DIST_DOT, N_("AP R Lobe"), CM, NULL},
    {SP_TR_R_LOBE, DIST_DOT, N_("Tr R Lobe"), CM, NULL},
#ifdef EMP_322
#else
    {SP_SUPA_L_LOBE, SD, N_("Supa L Lobe"), SP_SUPA_L_LOBE_MULTI, NULL},
    {SP_INFA_L_LOBE, SD, N_("Infa L Lobe"), SP_INFA_L_LOBE_MULTI, NULL},
    {SP_SUPA_R_LOBE, SD, N_("Supa R Lobe"), SP_SUPA_R_LOBE_MULTI, NULL},
    {SP_INFA_R_LOBE, SD, N_("Infa R Lobe"), SP_INFA_R_LOBE_MULTI, NULL},
#endif
    {SP_THY_LESION1_VOL, MEA_TYPE_END, N_("Thyroid Lesion1 Vol"), UNIT_END, NULL},
    {SP_THY_LESION1_L, DIST_DOT, N_("Thyroid Lesion1 L"), CM, &SPThyLesion1VolCalc},
    {SP_THY_LESION1_W, DIST_DOT, N_("Thyroid Lesion1 W"), CM, &SPThyLesion1VolCalc},
    {SP_THY_LESION1_H, DIST_DOT, N_("Thyroid Lesion1 H"), CM, &SPThyLesion1VolCalc},
    {SP_THY_LESION2_VOL, MEA_TYPE_END, N_("Thyroid Lesion2 Vol"), UNIT_END, NULL},
    {SP_THY_LESION2_L, DIST_DOT, N_("Thyroid Lesion2 L"), CM, &SPThyLesion2VolCalc},
    {SP_THY_LESION2_W, DIST_DOT, N_("Thyroid Lesion2 W"), CM, &SPThyLesion2VolCalc},
    {SP_THY_LESION2_H, DIST_DOT, N_("Thyroid Lesion2 H"), CM, &SPThyLesion2VolCalc},
    {SP_THY_LESION3_VOL, MEA_TYPE_END, N_("Thyroid Lesion3 Vol"), UNIT_END, NULL},
    {SP_THY_LESION3_L, DIST_DOT, N_("Thyroid Lesion3 L"), CM, &SPThyLesion3VolCalc},
    {SP_THY_LESION3_W, DIST_DOT, N_("Thyroid Lesion3 W"), CM, &SPThyLesion3VolCalc},
    {SP_THY_LESION3_H, DIST_DOT, N_("Thyroid Lesion3 H"), CM, &SPThyLesion3VolCalc},
    {SP_THY_LESION4_VOL, MEA_TYPE_END, N_("Thyroid Lesion4 Vol"), UNIT_END, NULL},
    {SP_THY_LESION4_L, DIST_DOT, N_("Thyroid Lesion4 L"), CM, &SPThyLesion4VolCalc},
    {SP_THY_LESION4_W, DIST_DOT, N_("Thyroid Lesion4 W"), CM, &SPThyLesion4VolCalc},
    {SP_THY_LESION4_H, DIST_DOT, N_("Thyroid Lesion4 H"), CM, &SPThyLesion4VolCalc},
    {SP_THY_LESION5_VOL, MEA_TYPE_END, N_("Thyroid Lesion5 Vol"), UNIT_END, NULL},
    {SP_THY_LESION5_L, DIST_DOT, N_("Thyroid Lesion5 L"), CM, &SPThyLesion5VolCalc},
    {SP_THY_LESION5_W, DIST_DOT, N_("Thyroid Lesion5 W"), CM, &SPThyLesion5VolCalc},
    {SP_THY_LESION5_H, DIST_DOT, N_("Thyroid Lesion5 H"), CM, &SPThyLesion5VolCalc},
    {SP_L_EYE_OA, DIST_DOT, N_("L Eye OA"), CM, NULL},
    {SP_L_EYE_LENS, DIST_DOT, N_("L Eye Lens"), CM, NULL},
    {SP_L_EYE_AC, DIST_DOT, N_("L Eye AC"), CM, NULL},
    {SP_L_EYE_ON, DIST_DOT, N_("L Eye ON"), CM, NULL},
    {SP_R_EYE_OA, DIST_DOT, N_("R Eye OA"), CM, NULL},
    {SP_R_EYE_LENS, DIST_DOT, N_("R Eye Lens"), CM, NULL},
    {SP_R_EYE_AC, DIST_DOT, N_("R Eye AC"), CM, NULL},
    {SP_R_EYE_ON, DIST_DOT, N_("R Eye ON"), CM, NULL},
};
#else
const SingleItemInfo SPInfo[] = {
    {SP_GALACT1_THICK, DIST_DOT, N_("Galact1 Thick"), CM, NULL},
    {SP_GALACT2_THICK, DIST_DOT, N_("Galact2 Thick"), CM, NULL},
    {SP_GALACT3_THICK, DIST_DOT, N_("Galact3 Thick"), CM, NULL},
    {SP_GALACT4_THICK, DIST_DOT, N_("Galact4 Thick"), CM, NULL},
    {SP_GALACT5_THICK, DIST_DOT, N_("Galact5 Thick"), CM, NULL},
    {SP_BRE1_LESION1_VOL, MEA_TYPE_END, N_("Bre1 Le1 Vol"), UNIT_END, NULL},
    {SP_BRE1_LESION1_L, DIST_DOT, N_("Bre1 Le1 L"), CM, &SPBre1Lesion1VolCalc},
    {SP_BRE1_LESION1_W, DIST_DOT, N_("Bre1 Le1 W"), CM, &SPBre1Lesion1VolCalc},
    {SP_BRE1_LESION1_H, DIST_DOT, N_("Bre1 Le1 H"), CM, &SPBre1Lesion1VolCalc},
    {SP_BRE1_LESION2_VOL, MEA_TYPE_END, N_("Bre1 Le2 Vol"), UNIT_END, NULL},
    {SP_BRE1_LESION2_L, DIST_DOT, N_("Bre1 Le2 L"), CM, &SPBre1Lesion2VolCalc},
    {SP_BRE1_LESION2_W, DIST_DOT, N_("Bre1 Le2 W"), CM, &SPBre1Lesion2VolCalc},
    {SP_BRE1_LESION2_H, DIST_DOT, N_("Bre1 Le2 H"), CM, &SPBre1Lesion2VolCalc},
    {SP_BRE1_LESION3_VOL, MEA_TYPE_END, N_("Bre1 Le3 Vol"), UNIT_END, NULL},
    {SP_BRE1_LESION3_L, DIST_DOT, N_("Bre1 Le3 L"), CM, &SPBre1Lesion3VolCalc},
    {SP_BRE1_LESION3_W, DIST_DOT, N_("Bre1 Le3 W"), CM, &SPBre1Lesion3VolCalc},
    {SP_BRE1_LESION3_H, DIST_DOT, N_("Bre1 Le3 H"), CM, &SPBre1Lesion3VolCalc},
    {SP_BRE1_LESION4_VOL, MEA_TYPE_END, N_("Bre1 Le4 Vol"), UNIT_END, NULL},
    {SP_BRE1_LESION4_L, DIST_DOT, N_("Bre1 Le4 L"), CM, &SPBre1Lesion4VolCalc},
    {SP_BRE1_LESION4_W, DIST_DOT, N_("Bre1 Le4 W"), CM, &SPBre1Lesion4VolCalc},
    {SP_BRE1_LESION4_H, DIST_DOT, N_("Bre1 Le4 H"), CM, &SPBre1Lesion4VolCalc},
    {SP_BRE1_LESION5_VOL, MEA_TYPE_END, N_("Bre1 Le5 Vol"), UNIT_END, NULL},
    {SP_BRE1_LESION5_L, DIST_DOT, N_("Bre1 Le5 L"), CM, &SPBre1Lesion5VolCalc},
    {SP_BRE1_LESION5_W, DIST_DOT, N_("Bre1 Le5 W"), CM, &SPBre1Lesion5VolCalc},
    {SP_BRE1_LESION5_H, DIST_DOT, N_("Bre1 Le5 H"), CM, &SPBre1Lesion5VolCalc},
    {SP_BRE2_LESION1_VOL, MEA_TYPE_END, N_("Bre2 Le1 Vol"), UNIT_END, NULL},
    {SP_BRE2_LESION1_L, DIST_DOT, N_("Bre2 Le1 L"), CM, &SPBre2Lesion1VolCalc},
    {SP_BRE2_LESION1_W, DIST_DOT, N_("Bre2 Le1 W"), CM, &SPBre2Lesion1VolCalc},
    {SP_BRE2_LESION1_H, DIST_DOT, N_("Bre2 Le1 H"), CM, &SPBre2Lesion1VolCalc},
    {SP_BRE2_LESION2_VOL, MEA_TYPE_END, N_("Bre2 Le2 Vol"), UNIT_END, NULL},
    {SP_BRE2_LESION2_L, DIST_DOT, N_("Bre2 Le2 L"), CM, &SPBre2Lesion2VolCalc},
    {SP_BRE2_LESION2_W, DIST_DOT, N_("Bre2 Le2 W"), CM, &SPBre2Lesion2VolCalc},
    {SP_BRE2_LESION2_H, DIST_DOT, N_("Bre2 Le2 H"), CM, &SPBre2Lesion2VolCalc},
    {SP_BRE2_LESION3_VOL, MEA_TYPE_END, N_("Bre2 Le3 Vol"), UNIT_END, NULL},
    {SP_BRE2_LESION3_L, DIST_DOT, N_("Bre2 Le3 L"), CM, &SPBre2Lesion3VolCalc},
    {SP_BRE2_LESION3_W, DIST_DOT, N_("Bre2 Le3 W"), CM, &SPBre2Lesion3VolCalc},
    {SP_BRE2_LESION3_H, DIST_DOT, N_("Bre2 Le3 H"), CM, &SPBre2Lesion3VolCalc},
    {SP_BRE2_LESION4_VOL, MEA_TYPE_END, N_("Bre2 Le4 Vol"), UNIT_END, NULL},
    {SP_BRE2_LESION4_L, DIST_DOT, N_("Bre2 Le4 L"), CM, &SPBre2Lesion4VolCalc},
    {SP_BRE2_LESION4_W, DIST_DOT, N_("Bre2 Le4 W"), CM, &SPBre2Lesion4VolCalc},
    {SP_BRE2_LESION4_H, DIST_DOT, N_("Bre2 Le4 H"), CM, &SPBre2Lesion4VolCalc},
    {SP_BRE2_LESION5_VOL, MEA_TYPE_END, N_("Bre2 Le5 Vol"), UNIT_END, NULL},
    {SP_BRE2_LESION5_L, DIST_DOT, N_("Bre2 Le5 L"), CM, &SPBre2Lesion5VolCalc},
    {SP_BRE2_LESION5_W, DIST_DOT, N_("Bre2 Le5 W"), CM, &SPBre2Lesion5VolCalc},
    {SP_BRE2_LESION5_H, DIST_DOT, N_("Bre2 Le5 H"), CM, &SPBre2Lesion5VolCalc},
    {SP_BRE3_LESION1_VOL, MEA_TYPE_END, N_("Bre3 Le1 Vol"), UNIT_END, NULL},
    {SP_BRE3_LESION1_L, DIST_DOT, N_("Bre3 Le1 L"), CM, &SPBre3Lesion1VolCalc},
    {SP_BRE3_LESION1_W, DIST_DOT, N_("Bre3 Le1 W"), CM, &SPBre3Lesion1VolCalc},
    {SP_BRE3_LESION1_H, DIST_DOT, N_("Bre3 Le1 H"), CM, &SPBre3Lesion1VolCalc},
    {SP_BRE3_LESION2_VOL, MEA_TYPE_END, N_("Bre3 Le2 Vol"), UNIT_END, NULL},
    {SP_BRE3_LESION2_L, DIST_DOT, N_("Bre3 Le2 L"), CM, &SPBre3Lesion2VolCalc},
    {SP_BRE3_LESION2_W, DIST_DOT, N_("Bre3 Le2 W"), CM, &SPBre3Lesion2VolCalc},
    {SP_BRE3_LESION2_H, DIST_DOT, N_("Bre3 Le2 H"), CM, &SPBre3Lesion2VolCalc},
    {SP_BRE3_LESION3_VOL, MEA_TYPE_END, N_("Bre3 Le3 Vol"), UNIT_END, NULL},
    {SP_BRE3_LESION3_L, DIST_DOT, N_("Bre3 Le3 L"), CM, &SPBre3Lesion3VolCalc},
    {SP_BRE3_LESION3_W, DIST_DOT, N_("Bre3 Le3 W"), CM, &SPBre3Lesion3VolCalc},
    {SP_BRE3_LESION3_H, DIST_DOT, N_("Bre3 Le3 H"), CM, &SPBre3Lesion3VolCalc},
    {SP_BRE3_LESION4_VOL, MEA_TYPE_END, N_("Bre3 Le4 Vol"), UNIT_END, NULL},
    {SP_BRE3_LESION4_L, DIST_DOT, N_("Bre3 Le4 L"), CM, &SPBre3Lesion4VolCalc},
    {SP_BRE3_LESION4_W, DIST_DOT, N_("Bre3 Le4 W"), CM, &SPBre3Lesion4VolCalc},
    {SP_BRE3_LESION4_H, DIST_DOT, N_("Bre3 Le4 H"), CM, &SPBre3Lesion4VolCalc},
    {SP_BRE3_LESION5_VOL, MEA_TYPE_END, N_("Bre3 Le5 Vol"), UNIT_END, NULL},
    {SP_BRE3_LESION5_L, DIST_DOT, N_("Bre3 Le5 L"), CM, &SPBre3Lesion5VolCalc},
    {SP_BRE3_LESION5_W, DIST_DOT, N_("Bre3 Le5 W"), CM, &SPBre3Lesion5VolCalc},
    {SP_BRE3_LESION5_H, DIST_DOT, N_("Bre3 Le5 H"), CM, &SPBre3Lesion5VolCalc},
    {SP_BRE4_LESION1_VOL, MEA_TYPE_END, N_("Bre4 Le1 Vol"), UNIT_END, NULL},
    {SP_BRE4_LESION1_L, DIST_DOT, N_("Bre4 Le1 L"), CM, &SPBre4Lesion1VolCalc},
    {SP_BRE4_LESION1_W, DIST_DOT, N_("Bre4 Le1 W"), CM, &SPBre4Lesion1VolCalc},
    {SP_BRE4_LESION1_H, DIST_DOT, N_("Bre4 Le1 H"), CM, &SPBre4Lesion1VolCalc},
    {SP_BRE4_LESION2_VOL, MEA_TYPE_END, N_("Bre4 Le2 Vol"), UNIT_END, NULL},
    {SP_BRE4_LESION2_L, DIST_DOT, N_("Bre4 Le2 L"), CM, &SPBre4Lesion2VolCalc},
    {SP_BRE4_LESION2_W, DIST_DOT, N_("Bre4 Le2 W"), CM, &SPBre4Lesion2VolCalc},
    {SP_BRE4_LESION2_H, DIST_DOT, N_("Bre4 Le2 H"), CM, &SPBre4Lesion2VolCalc},
    {SP_BRE4_LESION3_VOL, MEA_TYPE_END, N_("Bre4 Le3 Vol"), UNIT_END, NULL},
    {SP_BRE4_LESION3_L, DIST_DOT, N_("Bre4 Le3 L"), CM, &SPBre4Lesion3VolCalc},
    {SP_BRE4_LESION3_W, DIST_DOT, N_("Bre4 Le3 W"), CM, &SPBre4Lesion3VolCalc},
    {SP_BRE4_LESION3_H, DIST_DOT, N_("Bre4 Le3 H"), CM, &SPBre4Lesion3VolCalc},
    {SP_BRE4_LESION4_VOL, MEA_TYPE_END, N_("Bre4 Le4 Vol"), UNIT_END, NULL},
    {SP_BRE4_LESION4_L, DIST_DOT, N_("Bre4 Le4 L"), CM, &SPBre4Lesion4VolCalc},
    {SP_BRE4_LESION4_W, DIST_DOT, N_("Bre4 Le4 W"), CM, &SPBre4Lesion4VolCalc},
    {SP_BRE4_LESION4_H, DIST_DOT, N_("Bre4 Le4 H"), CM, &SPBre4Lesion4VolCalc},
    {SP_BRE4_LESION5_VOL, MEA_TYPE_END, N_("Bre4 Le5 Vol"), UNIT_END, NULL},
    {SP_BRE4_LESION5_L, DIST_DOT, N_("Bre4 Le5 L"), CM, &SPBre4Lesion5VolCalc},
    {SP_BRE4_LESION5_W, DIST_DOT, N_("Bre4 Le5 W"), CM, &SPBre4Lesion5VolCalc},
    {SP_BRE4_LESION5_H, DIST_DOT, N_("Bre4 Le5 H"), CM, &SPBre4Lesion5VolCalc},
    {SP_BRE5_LESION1_VOL, MEA_TYPE_END, N_("Bre5 Le1 Vol"), UNIT_END, NULL},
    {SP_BRE5_LESION1_L, DIST_DOT, N_("Bre5 Le1 L"), CM, &SPBre5Lesion1VolCalc},
    {SP_BRE5_LESION1_W, DIST_DOT, N_("Bre5 Le1 W"), CM, &SPBre5Lesion1VolCalc},
    {SP_BRE5_LESION1_H, DIST_DOT, N_("Bre5 Le1 H"), CM, &SPBre5Lesion1VolCalc},
    {SP_BRE5_LESION2_VOL, MEA_TYPE_END, N_("Bre5 Le2 Vol"), UNIT_END, NULL},
    {SP_BRE5_LESION2_L, DIST_DOT, N_("Bre5 Le2 L"), CM, &SPBre5Lesion2VolCalc},
    {SP_BRE5_LESION2_W, DIST_DOT, N_("Bre5 Le2 W"), CM, &SPBre5Lesion2VolCalc},
    {SP_BRE5_LESION2_H, DIST_DOT, N_("Bre5 Le2 H"), CM, &SPBre5Lesion2VolCalc},
    {SP_BRE5_LESION3_VOL, MEA_TYPE_END, N_("Bre5 Le3 Vol"), UNIT_END, NULL},
    {SP_BRE5_LESION3_L, DIST_DOT, N_("Bre5 Le3 L"), CM, &SPBre5Lesion3VolCalc},
    {SP_BRE5_LESION3_W, DIST_DOT, N_("Bre5 Le3 W"), CM, &SPBre5Lesion3VolCalc},
    {SP_BRE5_LESION3_H, DIST_DOT, N_("Bre5 Le3 H"), CM, &SPBre5Lesion3VolCalc},
    {SP_BRE5_LESION4_VOL, MEA_TYPE_END, N_("Bre5 Le4 Vol"), UNIT_END, NULL},
    {SP_BRE5_LESION4_L, DIST_DOT, N_("Bre5 Le4 L"), CM, &SPBre5Lesion4VolCalc},
    {SP_BRE5_LESION4_W, DIST_DOT, N_("Bre5 Le4 W"), CM, &SPBre5Lesion4VolCalc},
    {SP_BRE5_LESION4_H, DIST_DOT, N_("Bre5 Le4 H"), CM, &SPBre5Lesion4VolCalc},
    {SP_BRE5_LESION5_VOL, MEA_TYPE_END, N_("Bre5 Le5 Vol"), UNIT_END, NULL},
    {SP_BRE5_LESION5_L, DIST_DOT, N_("Bre5 Le5 L"), CM, &SPBre5Lesion5VolCalc},
    {SP_BRE5_LESION5_W, DIST_DOT, N_("Bre5 Le5 W"), CM, &SPBre5Lesion5VolCalc},
    {SP_BRE5_LESION5_H, DIST_DOT, N_("Bre5 Le5 H"), CM, &SPBre5Lesion5VolCalc},
    {SP_L_TESTIS_VOL, MEA_TYPE_END,N_( "L Testis Vol"), UNIT_END, NULL},
    {SP_L_TESTIS_L, DIST_DOT, N_("L Testis L"), CM, &SPLTesVolCalc},
    {SP_L_TESTIS_W, DIST_DOT, N_("L Testis W"), CM, &SPLTesVolCalc},
    {SP_L_TESTIS_H, DIST_DOT, N_("L Testis H"), CM, &SPLTesVolCalc},
    {SP_L_TESTIS_ARTERY, SD, N_("L Testis Artery"), SP_L_TESTIS_ARTERY_MULTI, NULL},
    {SP_R_TESTIS_VOL, MEA_TYPE_END, N_("R Testis Vol"), UNIT_END, NULL},
    {SP_R_TESTIS_L, DIST_DOT, N_("R Testis L"), CM, &SPRTesVolCalc},
    {SP_R_TESTIS_W, DIST_DOT, N_("R Testis W"), CM, &SPRTesVolCalc},
    {SP_R_TESTIS_H, DIST_DOT, N_("R Testis H"), CM, &SPRTesVolCalc},
    {SP_R_TESTIS_ARTERY, SD, N_("R Testis Artery"), SP_R_TESTIS_ARTERY_MULTI, NULL},
    {SP_TES_L_LESION1_VOL, MEA_TYPE_END, N_("Testis L Lesion1 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION1_L, DIST_DOT, N_("Testis L Lesion1 L"), CM, &SPLTesLesion1VolCalc},
    {SP_TES_L_LESION1_W, DIST_DOT, N_("Testis L Lesion1 W"), CM, &SPLTesLesion1VolCalc},
    {SP_TES_L_LESION1_H, DIST_DOT, N_("Testis L Lesion1 H"), CM, &SPLTesLesion1VolCalc},
    {SP_TES_L_LESION2_VOL, MEA_TYPE_END, N_("Testis L Lesion2 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION2_L, DIST_DOT, N_("Testis L Lesion2 L"), CM, &SPLTesLesion2VolCalc},
    {SP_TES_L_LESION2_W, DIST_DOT, N_("Testis L Lesion2 W"), CM, &SPLTesLesion2VolCalc},
    {SP_TES_L_LESION2_H, DIST_DOT, N_("Testis L Lesion2 H"), CM, &SPLTesLesion2VolCalc},
    {SP_TES_L_LESION3_VOL, MEA_TYPE_END, N_("Testis L Lesion3 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION3_L, DIST_DOT, N_("Testis L Lesion3 L"), CM, &SPLTesLesion3VolCalc},
    {SP_TES_L_LESION3_W, DIST_DOT, N_("Testis L Lesion3 W"), CM, &SPLTesLesion3VolCalc},
    {SP_TES_L_LESION3_H, DIST_DOT, N_("Testis L Lesion3 H"), CM, &SPLTesLesion3VolCalc},
    {SP_TES_L_LESION4_VOL, MEA_TYPE_END, N_("Testis L Lesion4 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION4_L, DIST_DOT, N_("Testis L Lesion4 L"), CM, &SPLTesLesion4VolCalc},
    {SP_TES_L_LESION4_W, DIST_DOT, N_("Testis L Lesion4 W"), CM, &SPLTesLesion4VolCalc},
    {SP_TES_L_LESION4_H, DIST_DOT, N_("Testis L Lesion4 H"), CM, &SPLTesLesion4VolCalc},
    {SP_TES_L_LESION5_VOL, MEA_TYPE_END, N_("Testis L Lesion5 Vol"), UNIT_END, NULL},
    {SP_TES_L_LESION5_L, DIST_DOT, N_("Testis L Lesion5 L"), CM, &SPLTesLesion5VolCalc},
    {SP_TES_L_LESION5_W, DIST_DOT, N_("Testis L Lesion5 W"), CM, &SPLTesLesion5VolCalc},
    {SP_TES_L_LESION5_H, DIST_DOT, N_("Testis L Lesion5 H"), CM, &SPLTesLesion5VolCalc},
    {SP_TES_R_LESION1_VOL, MEA_TYPE_END, N_("Testis R Lesion1 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION1_L, DIST_DOT, N_("Testis R Lesion1 L"), CM, &SPRTesLesion1VolCalc},
    {SP_TES_R_LESION1_W, DIST_DOT, N_("Testis R Lesion1 W"), CM, &SPRTesLesion1VolCalc},
    {SP_TES_R_LESION1_H, DIST_DOT, N_("Testis R Lesion1 H"), CM, &SPRTesLesion1VolCalc},
    {SP_TES_R_LESION2_VOL, MEA_TYPE_END, N_("Testis R Lesion2 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION2_L, DIST_DOT, N_("Testis R Lesion2 L"), CM, &SPRTesLesion2VolCalc},
    {SP_TES_R_LESION2_W, DIST_DOT, N_("Testis R Lesion2 W"), CM, &SPRTesLesion2VolCalc},
    {SP_TES_R_LESION2_H, DIST_DOT, N_("Testis R Lesion2 H"), CM, &SPRTesLesion2VolCalc},
    {SP_TES_R_LESION3_VOL, MEA_TYPE_END, N_("Testis R Lesion3 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION3_L, DIST_DOT, N_("Testis R Lesion3 L"), CM, &SPRTesLesion3VolCalc},
    {SP_TES_R_LESION3_W, DIST_DOT, N_("Testis R Lesion3 W"), CM, &SPRTesLesion3VolCalc},
    {SP_TES_R_LESION3_H, DIST_DOT, N_("Testis R Lesion3 H"), CM, &SPRTesLesion3VolCalc},
    {SP_TES_R_LESION4_VOL, MEA_TYPE_END, N_("Testis R Lesion4 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION4_L, DIST_DOT, N_("Testis R Lesion4 L"), CM, &SPRTesLesion4VolCalc},
    {SP_TES_R_LESION4_W, DIST_DOT, N_("Testis R Lesion4 W"), CM, &SPRTesLesion4VolCalc},
    {SP_TES_R_LESION4_H, DIST_DOT, N_("Testis R Lesion4 H"), CM, &SPRTesLesion4VolCalc},
    {SP_TES_R_LESION5_VOL, MEA_TYPE_END, N_("Testis R Lesion5 Vol"), UNIT_END, NULL},
    {SP_TES_R_LESION5_L, DIST_DOT, N_("Testis R Lesion5 L"), CM, &SPRTesLesion5VolCalc},
    {SP_TES_R_LESION5_W, DIST_DOT, N_("Testis R Lesion5 W"), CM, &SPRTesLesion5VolCalc},
    {SP_TES_R_LESION5_H, DIST_DOT, N_("Testis R Lesion5 H"), CM, &SPRTesLesion5VolCalc},
    {SP_ISTHMUS_AP, DIST_DOT, N_("Isthmus AP"), CM, NULL},
    {SP_LONG_L_LOBE, DIST_DOT, N_("Long L Lobe"), CM, NULL},
    {SP_AP_L_LOBE, DIST_DOT, N_("AP L Lobe"), CM, NULL},
    {SP_TR_L_LOBE, DIST_DOT, N_("Tr L Lobe"), CM, NULL},
    {SP_LONG_R_LOBE, DIST_DOT, N_("Long R Lobe"), CM, NULL},
    {SP_AP_R_LOBE, DIST_DOT, N_("AP R Lobe"), CM, NULL},
    {SP_TR_R_LOBE, DIST_DOT, N_("Tr R Lobe"), CM, NULL},
    {SP_SUPA_L_LOBE, SD, N_("Supa L Lobe"), SP_SUPA_L_LOBE_MULTI, NULL},
    {SP_INFA_L_LOBE, SD, N_("Infa L Lobe"), SP_INFA_L_LOBE_MULTI, NULL},
    {SP_SUPA_R_LOBE, SD, N_("Supa R Lobe"), SP_SUPA_R_LOBE_MULTI, NULL},
    {SP_INFA_R_LOBE, SD, N_("Infa R Lobe"), SP_INFA_R_LOBE_MULTI, NULL},
    {SP_THY_LESION1_VOL, MEA_TYPE_END, N_("Thyroid Lesion1 Vol"), UNIT_END, NULL},
    {SP_THY_LESION1_L, DIST_DOT, N_("Thyroid Lesion1 L"), CM, &SPThyLesion1VolCalc},
    {SP_THY_LESION1_W, DIST_DOT, N_("Thyroid Lesion1 W"), CM, &SPThyLesion1VolCalc},
    {SP_THY_LESION1_H, DIST_DOT, N_("Thyroid Lesion1 H"), CM, &SPThyLesion1VolCalc},
    {SP_THY_LESION2_VOL, MEA_TYPE_END, N_("Thyroid Lesion2 Vol"), UNIT_END, NULL},
    {SP_THY_LESION2_L, DIST_DOT, N_("Thyroid Lesion2 L"), CM, &SPThyLesion2VolCalc},
    {SP_THY_LESION2_W, DIST_DOT, N_("Thyroid Lesion2 W"), CM, &SPThyLesion2VolCalc},
    {SP_THY_LESION2_H, DIST_DOT, N_("Thyroid Lesion2 H"), CM, &SPThyLesion2VolCalc},
    {SP_THY_LESION3_VOL, MEA_TYPE_END, N_("Thyroid Lesion3 Vol"), UNIT_END, NULL},
    {SP_THY_LESION3_L, DIST_DOT, N_("Thyroid Lesion3 L"), CM, &SPThyLesion3VolCalc},
    {SP_THY_LESION3_W, DIST_DOT, N_("Thyroid Lesion3 W"), CM, &SPThyLesion3VolCalc},
    {SP_THY_LESION3_H, DIST_DOT, N_("Thyroid Lesion3 H"), CM, &SPThyLesion3VolCalc},
    {SP_THY_LESION4_VOL, MEA_TYPE_END, N_("Thyroid Lesion4 Vol"), UNIT_END, NULL},
    {SP_THY_LESION4_L, DIST_DOT, N_("Thyroid Lesion4 L"), CM, &SPThyLesion4VolCalc},
    {SP_THY_LESION4_W, DIST_DOT, N_("Thyroid Lesion4 W"), CM, &SPThyLesion4VolCalc},
    {SP_THY_LESION4_H, DIST_DOT, N_("Thyroid Lesion4 H"), CM, &SPThyLesion4VolCalc},
    {SP_THY_LESION5_VOL, MEA_TYPE_END, N_("Thyroid Lesion5 Vol"), UNIT_END, NULL},
    {SP_THY_LESION5_L, DIST_DOT, N_("Thyroid Lesion5 L"), CM, &SPThyLesion5VolCalc},
    {SP_THY_LESION5_W, DIST_DOT, N_("Thyroid Lesion5 W"), CM, &SPThyLesion5VolCalc},
    {SP_THY_LESION5_H, DIST_DOT, N_("Thyroid Lesion5 H"), CM, &SPThyLesion5VolCalc},
    {SP_L_EYE_OA, DIST_DOT, N_("L Eye OA"), CM, NULL},
    {SP_L_EYE_LENS, DIST_DOT, N_("L Eye Lens"), CM, NULL},
    {SP_L_EYE_AC, DIST_DOT, N_("L Eye AC"), CM, NULL},
    {SP_L_EYE_ON, DIST_DOT, N_("L Eye ON"), CM, NULL},
    {SP_R_EYE_OA, DIST_DOT, N_("R Eye OA"), CM, NULL},
    {SP_R_EYE_LENS, DIST_DOT, N_("R Eye Lens"), CM, NULL},
    {SP_R_EYE_AC, DIST_DOT, N_("R Eye AC"), CM, NULL},
    {SP_R_EYE_ON, DIST_DOT, N_("R Eye ON"), CM, NULL},

};
#endif

/******************************Fetal Measure**************************************/
const CalcResultInfo FetalCO2DTeich = {FETAL_CO_2DTEICH, {FETAL_SV_2DTEICH, FETAL_HR_LV_MM, NO_PAR, NO_PAR, NO_PAR}, "CO(Teich)", LMIN, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalCOMMTeich = {FETAL_CO_MMTEICH, {FETAL_SV_MMTEICH, FETAL_HR_LV_MM, NO_PAR, NO_PAR, NO_PAR}, "CO(Teich)", LMIN, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray FetalSV2DTeichCalc = {&FetalCO2DTeich, NULL};
const CalcInfoArray FetalSVMMTeichCalc = {&FetalCOMMTeich, NULL};

const CalcResultInfo FetalSV2DTeich = {FETAL_SV_2DTEICH, {FETAL_EDV_2DTEICH, FETAL_ESV_2DTEICH, NO_PAR, NO_PAR, NO_PAR}, "SV(Teich)", CM3, MeaCalcFun::SectionCalc, &FetalSV2DTeichCalc};
const CalcResultInfo FetalSVMMTeich = {FETAL_SV_MMTEICH, {FETAL_EDV_MMTEICH, FETAL_ESV_MMTEICH, NO_PAR, NO_PAR, NO_PAR}, "SV(Teich)", CM3, MeaCalcFun::SectionCalc, &FetalSVMMTeichCalc};

const CalcResultInfo FetalEF2DTeich = {FETAL_EF_2DTEICH, {FETAL_EDV_2DTEICH, FETAL_ESV_2DTEICH, NO_PAR, NO_PAR, NO_PAR}, "EF(Teich)", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalEFMMTeich = {FETAL_EF_MMTEICH, {FETAL_EDV_MMTEICH, FETAL_ESV_MMTEICH, NO_PAR, NO_PAR, NO_PAR}, "EF(Teich)", PERCENT, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray FetalEDV2DTeichCalc = {&FetalSV2DTeich, &FetalEF2DTeich, NULL};
const CalcInfoArray FetalEDVMMTeichCalc = {&FetalSVMMTeich, &FetalEFMMTeich, NULL};

const CalcInfoArray FetalESV2DTeichCalc = {&FetalSV2DTeich, &FetalEF2DTeich, NULL};
const CalcInfoArray FetalESVMMTeichCalc = {&FetalSVMMTeich, &FetalEFMMTeich, NULL};

const CalcResultInfo FetalAoPa = {FETAL_AO_PA_RATIO, {FETAL_AOR_DIAM, FETAL_MPA_DIAM, NO_PAR, NO_PAR, NO_PAR}, "Ao/PA", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalEDV2DTeich = {FETAL_EDV_2DTEICH, {FETAL_LVIDD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDV", CM3, MeaCalcFun::SectionCalc, &FetalEDV2DTeichCalc};
const CalcResultInfo FetalEDVMMTeich = {FETAL_EDV_MMTEICH, {FETAL_LVIDD_MM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDV", CM3, MeaCalcFun::SectionCalc, &FetalEDVMMTeichCalc};
const CalcResultInfo FetalESV2DTeich = {FETAL_ESV_2DTEICH, {FETAL_LVIDS, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "ESV", CM3, MeaCalcFun::SectionCalc, &FetalESV2DTeichCalc};
const CalcResultInfo FetalESVMMTeich = {FETAL_ESV_MMTEICH, {FETAL_LVIDS_MM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "ESV", CM3, MeaCalcFun::SectionCalc, &FetalESVMMTeichCalc};
//const CalcResultInfo FetalEF2DTeich = {FETAL_EF_2DTEICH, {FETAL_EDV_2DTEICH, FETAL_ESV_2DTEICH, NO_PAR, NO_PAR, NO_PAR}, "EF", NOUNIT, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo FetalEFMMTeich = {FETAL_EF_MMTEICH, {FETAL_EDV_MMTEICH, FETAL_ESV_MMTEICH, NO_PAR, NO_PAR, NO_PAR}, "EF", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalFS2DTeich = {FETAL_FS_2DTEICH, {FETAL_LVIDD, FETAL_LVIDS, NO_PAR, NO_PAR, NO_PAR}, "FS", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalFSMMTeich = {FETAL_FS_MMTEICH, {FETAL_LVIDD_MM, FETAL_LVIDS_MM, NO_PAR, NO_PAR, NO_PAR}, "FS", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalHrtCThrC = {FETAL_HRTC_THRC_RATIO, {FETAL_HEART_CIRC, FETAL_THORAX_CIRC, NO_PAR, NO_PAR, NO_PAR}, "HrtC/ThrC", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalIVSPer = {FETAL_IVS_PER, {FETAL_IVSD, FETAL_IVSS, NO_PAR, NO_PAR, NO_PAR}, "IVS%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalIVSPerMM = {FETAL_IVS_PER_MM, {FETAL_IVSD_MM, FETAL_IVSS_MM, NO_PAR, NO_PAR, NO_PAR}, "IVS%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalIVSLVPW = {FETAL_IVS_LVPW_RATIO, {FETAL_IVSD, FETAL_LVPWD, NO_PAR, NO_PAR, NO_PAR}, "IVS/LVPW", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLAAO = {FETAL_LA_AO_RATIO, {FETAL_LA_W, FETAL_AOR_DIAM, NO_PAR, NO_PAR, NO_PAR}, "LA/Ao", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLAAOMM = {FETAL_LA_AO_RATIO_MM, {FETAL_LA_DIMEN_MM, FETAL_AOR_DIAM_MM, NO_PAR, NO_PAR, NO_PAR}, "LA/Ao", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLVRV = {FETAL_LV_RV_RATIO, {FETAL_LV_W, FETAL_RV_W, NO_PAR, NO_PAR, NO_PAR}, "LV/RV", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLARA = {FETAL_LA_RA_RATIO, {FETAL_LA_W, FETAL_RA_W, NO_PAR, NO_PAR, NO_PAR}, "LA/RA", NOUNIT, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo FetalLVEF = {FETAL_LV_EF, {, , , , }, "LV EF", NOUNIT, NULL};
//const CalcResultInfo FetalLVMass = {FETAL_LV_MASS, {, , , , }, "LV Mass", NOUNIT, NULL};
const CalcResultInfo FetalLVMass = {FETAL_LV_MASS, {FETAL_IVSD_MM, FETAL_LVIDD_MM, FETAL_LVPWD_MM, NO_PAR, NO_PAR}, "LV Mass", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLVPEPET = {FETAL_LV_PEP_ET_RATIO, {FETAL_LV_PEP_MM, FETAL_LV_ET_MM, NO_PAR, NO_PAR, NO_PAR}, "LV PEP/ET", NOUNIT, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo FetalLVRV = {FETAL_LV_RV_RATIO, {, , , , }, "LV/RV", NOUNIT, NULL};
//const CalcResultInfo FetalLVETc = {FETAL_LVETC, {, , , , }, "LVETc", NOUNIT, NULL};
const CalcResultInfo FetalLVETc = {FETAL_LVETC, {FETAL_LV_ET_MM, FETAL_HR_AV_MM, NO_PAR, NO_PAR, NO_PAR}, "LV ETc", SEC, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLVOTArea = {FETAL_LVOT_AREA, {FETAL_LVOT_DIAM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "LVOT Area", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLVPEPc = {FETAL_LVPEPC, {FETAL_LV_PEP_MM, FETAL_HR_AV_MM, NO_PAR, NO_PAR, NO_PAR}, "LVPEPc", SEC, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLVPWPer = {FETAL_LVPW_PER, {FETAL_LVPWS, FETAL_LVPWD, NO_PAR, NO_PAR, NO_PAR}, "LVPW%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalLVPWPerMM = {FETAL_LVPW_PER_MM, {FETAL_LVPWS_MM, FETAL_LVPWD_MM, NO_PAR, NO_PAR, NO_PAR}, "LVPW%", PERCENT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalIvsLvpwMM = {FETAL_IVS_LVPW_MM, {FETAL_IVSD_MM, FETAL_LVPWD_MM, NO_PAR, NO_PAR, NO_PAR}, "IVS/LVPW", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalMeanVCF = {FETAL_MEAN_VCF, {FETAL_LVIDD_MM, FETAL_LVIDS_MM, FETAL_LV_ET_MM, NO_PAR, NO_PAR}, "mean VCF", CIRCS, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalMeanVCFc = {FETAL_MEAN_VCFC, {FETAL_LVIDD_MM, FETAL_LVIDS_MM, FETAL_LV_ET_MM, FETAL_HR_AV_MM, NO_PAR}, "mean VCFc", CIRCS, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo FetalQpQs = {FETAL_QP_QS_RATIO, {, , , , }, "Qp/Qs", NOUNIT, NULL};
//const CalcResultInfo FetalRVEF = {FETAL_RV_EF, {, , , , }, "RV EF", NOUNIT, NULL};
const CalcResultInfo FetalRVPEPET = {FETAL_RV_PEP_ET_RATIO, {FETAL_RV_PEP_MM, FETAL_RV_ET_MM, NO_PAR, NO_PAR, NO_PAR}, "RV PEP/ET", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalRVOTArea = {FETAL_RVOT_AREA, {FETAL_RVOT_DIAM, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "RVOT Area", CM2, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo FetalRVPEPc = {FETAL_RVPEPC, {FETAL_RV_PEP_MM, FETAL_HR_PV_MM, NO_PAR, NO_PAR, NO_PAR}, "RVPEPc", SEC, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo FetalSV = {FETAL_SV, {, , , , }, "SV", NOUNIT, NULL};
const CalcResultInfo FetalMVEA = {FETAL_MV_EA_RATIO, {FETAL_MV_PEAK_E, FETAL_MV_PEAK_A, NO_PAR, NO_PAR, NO_PAR}, "MV E/A", NOUNIT, MeaCalcFun::SectionCalc, NULL};
//const CalcResultInfo FetalThrc = {FETAL_THRC, {, , , , }, "Thrc", NOUNIT, NULL};
const CalcResultInfo FetalTVEA = {FETAL_TV_EA_RATIO, {FETAL_TV_PEAK_E, FETAL_TV_PEAK_A, NO_PAR, NO_PAR, NO_PAR}, "TV E/A", NOUNIT, MeaCalcFun::SectionCalc, NULL};

//const CalcInfoArray FetalLVIDd2DCalc = {&FetalEDV2DTeich, &FetalFS2DTeich, &FetalEF2DTeich, NULL};
const CalcInfoArray FetalLVIDd2DCalc = {&FetalEDV2DTeich, &FetalFS2DTeich, NULL};
const CalcInfoArray FetalLVPWdCalc = {&FetalLVPWPer, &FetalIVSLVPW, NULL};
//const CalcInfoArray FetalLVIDs2DCalc = {&FetalESV2DTeich, &FetalFS2DTeich, &FetalEF2DTeich, NULL};
const CalcInfoArray FetalLVIDs2DCalc = {&FetalESV2DTeich, &FetalFS2DTeich, NULL};
const CalcInfoArray FetalLVPWsCalc = {&FetalLVPWPer, NULL};
const CalcInfoArray FetalIVSd2DCalc = {&FetalIVSPer, &FetalIVSLVPW, NULL};
const CalcInfoArray FetalIVSs2DCalc = {&FetalIVSPer, NULL};
const CalcInfoArray FetalLVRVCalc = {&FetalLVRV, NULL};
const CalcInfoArray FetalLARACalc = {&FetalLARA, NULL};
const CalcInfoArray FetalLaCalc = {&FetalLAAO, &FetalLARA, NULL};
const CalcInfoArray FetalLaMMCalc = {&FetalLAAOMM, NULL};
const CalcInfoArray FetalAoRCalc = {&FetalAoPa, &FetalLAAO, NULL};
const CalcInfoArray FetalAoRMMCalc = {&FetalLAAOMM, NULL};
const CalcInfoArray FetalMPACalc = {&FetalAoPa, NULL};
const CalcInfoArray FetalLvotCalc = {&FetalLVOTArea, NULL};
const CalcInfoArray FetalRvotCalc = {&FetalRVOTArea, NULL};
const CalcInfoArray FetalHrCThrCCalc = {&FetalHrtCThrC, NULL};
//const CalcInfoArray FetalLVIDdMCalc = {&FetalEDVMMTeich, &FetalFSMMTeich, &FetalEFMMTeich, &FetalMeanVCF, &FetalMeanVCFc, &FetalLVMass, NULL};
//const CalcInfoArray FetalLVIDsMCalc = {&FetalESVMMTeich, &FetalFSMMTeich, &FetalEFMMTeich, &FetalMeanVCF, &FetalMeanVCFc, NULL};
const CalcInfoArray FetalLVIDdMCalc = {&FetalEDVMMTeich, &FetalFSMMTeich, &FetalMeanVCF, &FetalMeanVCFc, &FetalLVMass, NULL};
const CalcInfoArray FetalLVIDsMCalc = {&FetalESVMMTeich, &FetalFSMMTeich, &FetalMeanVCF, &FetalMeanVCFc, NULL};
const CalcInfoArray FetalIVSdMCalc = {&FetalIVSPerMM, &FetalIvsLvpwMM, &FetalLVMass, NULL};
const CalcInfoArray FetalIVSsMCalc = {&FetalIVSPerMM, NULL};
const CalcInfoArray FetalLVPWdMCalc = {&FetalLVPWPerMM, &FetalIvsLvpwMM, &FetalLVMass, NULL};
const CalcInfoArray FetalLVPWsMCalc = {&FetalLVPWPerMM, NULL};
const CalcInfoArray FetalLVETCalc = {&FetalLVETc, &FetalLVPEPET, &FetalMeanVCF, &FetalMeanVCFc, NULL};
const CalcInfoArray FetalLVPEPCalc = {&FetalLVPEPET, &FetalLVPEPc, NULL};
const CalcInfoArray FetalHRAVCalc = {&FetalLVETc, &FetalLVPEPc, &FetalMeanVCFc, NULL};
const CalcInfoArray FetalRVETCalc = {&FetalRVPEPET, NULL};
const CalcInfoArray FetalRVPEPCalc = {&FetalRVPEPc, &FetalRVPEPET, NULL};
const CalcInfoArray FetalHRPVCalc = {&FetalRVPEPc, NULL};
const CalcInfoArray FetalMVEACalc = {&FetalMVEA, NULL};
const CalcInfoArray FetalTVEACalc = {&FetalTVEA, NULL};

const PtrOfCalcInfo FetalCalcInfoPtr[FETAL_CALC_END - FETAL_CALC_START] = {
    {FETAL_AO_PA_RATIO, &FetalAoPa},
    {FETAL_CO_2DTEICH, &FetalCO2DTeich},
    {FETAL_CO_MMTEICH, &FetalCOMMTeich},
    {FETAL_EDV_2DTEICH, &FetalEDV2DTeich},
    {FETAL_EDV_MMTEICH, &FetalEDVMMTeich},
    {FETAL_ESV_2DTEICH, &FetalESV2DTeich},
    {FETAL_ESV_MMTEICH, &FetalESVMMTeich},
    {FETAL_EF_2DTEICH, &FetalEF2DTeich},
    {FETAL_EF_MMTEICH, &FetalEFMMTeich},
    {FETAL_FS_2DTEICH, &FetalFS2DTeich},
    {FETAL_FS_MMTEICH, &FetalFSMMTeich},
    {FETAL_HRTC_THRC_RATIO, &FetalHrtCThrC},
    {FETAL_IVS_PER, &FetalIVSPer},
    {FETAL_IVS_PER_MM, &FetalIVSPerMM},
    {FETAL_IVS_LVPW_RATIO, &FetalIVSLVPW},
    {FETAL_LA_AO_RATIO, &FetalLAAO},
    {FETAL_LA_AO_RATIO_MM, &FetalLAAOMM},
    {FETAL_LA_RA_RATIO, &FetalLARA},
    {FETAL_LV_MASS, &FetalLVMass},
    {FETAL_LV_PEP_ET_RATIO, &FetalLVPEPET},
    {FETAL_LV_RV_RATIO, &FetalLVRV},
    {FETAL_LVETC, &FetalLVETc},
    {FETAL_LVOT_AREA, &FetalLVOTArea},
    {FETAL_LVPEPC, &FetalLVPEPc},
    {FETAL_LVPW_PER, &FetalLVPWPer},
    {FETAL_LVPW_PER_MM, &FetalLVPWPerMM},
    {FETAL_IVS_LVPW_MM, &FetalIvsLvpwMM},
    {FETAL_MEAN_VCF, &FetalMeanVCF},
    {FETAL_MEAN_VCFC, &FetalMeanVCFc},
    {FETAL_RV_PEP_ET_RATIO,  &FetalRVPEPET},
    {FETAL_RVOT_AREA, &FetalRVOTArea},
    {FETAL_RVPEPC, &FetalRVPEPc},
    {FETAL_SV_2DTEICH, &FetalSV2DTeich},
    {FETAL_SV_MMTEICH, &FetalSVMMTeich},
    {FETAL_MV_EA_RATIO, &FetalMVEA},
    {FETAL_TV_EA_RATIO, &FetalTVEA},
};

const SingleItemInfo FetalInfo[] = {
    //Fetal 2D Dimen
    {FETAL_RVAWD, DIST_DOT, N_("RVAWd(2D)"), CM, NULL},
    {FETAL_RVIDD, DIST_DOT,N_("RVIDd(2D)"), CM, NULL},
    {FETAL_IVSD, DIST_DOT,N_("IVSd(2D)"), CM, &FetalIVSd2DCalc},
    {FETAL_LVIDD, DIST_DOT,N_("LVIDd(2D)"), CM, &FetalLVIDd2DCalc },
    {FETAL_LVPWD, DIST_DOT,N_("LVPWd(2D)"), CM, &FetalLVPWdCalc },
    {FETAL_IVSS, DIST_DOT,N_("IVSs(2D)"), CM, &FetalIVSs2DCalc},
    {FETAL_LVIDS, DIST_DOT,N_("LVIDs(2D)"), CM, &FetalLVIDs2DCalc },
    {FETAL_LVPWS, DIST_DOT,N_("LVPWs(2D)"), CM, &FetalLVPWsCalc },
    {FETAL_LV_L, DIST_DOT,N_("LV Length(2D)"), CM, NULL},
    {FETAL_LV_W, DIST_DOT,N_("LV Width(2D)"), CM, &FetalLVRVCalc},
    {FETAL_LV_WALL_W, DIST_DOT,N_("LV Wall Width(2D)"), CM, NULL},
    {FETAL_IVS_W, DIST_DOT, N_("IVS Width(2D)"), CM, NULL},
    {FETAL_RV_L, DIST_DOT, N_("RV Length(2D)"), CM, NULL},
    {FETAL_RV_W, DIST_DOT, N_("RV Width(2D)"), CM, &FetalLVRVCalc},
    {FETAL_RV_WALL_W, DIST_DOT, N_("RV Wall Width(2D)"), CM, NULL},
    {FETAL_LA_L, DIST_DOT, N_("LA Length(2D)"), CM, NULL},
    {FETAL_LA_W, DIST_DOT, N_("LA Width(2D)"), CM, &FetalLaCalc},
//  {FETAL_LA_DIMEN, DIST_DOT, N_("LA Dimen"), CM, &FetalLaCalc},
    {FETAL_LA_DIMEN, DIST_DOT, N_("LA Dimen(2D)"), CM, NULL},
    {FETAL_RA_L, DIST_DOT, N_("RA Length(2D)"), CM, NULL},
    {FETAL_RA_W, DIST_DOT, N_("RA Width(2D)"), CM, &FetalLARACalc},
    {FETAL_RPA_DIAM_DIM, DIST_DOT, N_("RPA Diam(2D)"), CM, NULL},
    {FETAL_LVOT_DIAM, DIST_DOT, N_("LVOT Diam(2D)"), CM, &FetalLvotCalc},
    {FETAL_RVOT_DIAM, DIST_DOT, N_("RVOT Diam(2D)"), CM, &FetalRvotCalc},
    {FETAL_HEART_AREA, AREA_ELLIPSE, N_("Heart Area(2D)"), CM2, NULL},//椭圆法面积
    {FETAL_HEART_CIRC, PERI_ELLIPSE, N_("Heart Circ(2D)"), CM, &FetalHrCThrCCalc},//椭圆法周长
    {FETAL_THORAX_CIRC, PERI_ELLIPSE, N_("Thorax Circ(2D)"), CM, &FetalHrCThrCCalc},//椭圆法周长
    //Fetal 2D Vessels
    {FETAL_AOR_DIAM, DIST_DOT, N_("AoR Diam(2D)"), CM, &FetalAoRCalc },
    {FETAL_ASC_AO_DIAM, DIST_DOT, N_("Asc Ao Diam(2D)"), CM, NULL},
    {FETAL_AO_ARCH_DIAM, DIST_DOT, N_("Ao Arch Diam(2D)"), CM, NULL},
    {FETAL_AO_ISTHMUS_DIAM, DIST_DOT, N_("Ao Isthmus Diam(2D)"), CM, NULL},
    {FETAL_DESC_AO_DIAM, DIST_DOT, N_("Desc Ao Diam(2D)"), CM, NULL},
    {FETAL_DUCT_ART_DIAM, DIST_DOT, N_("Duct Art Diam(2D)"), CM, NULL},
    {FETAL_MPA_DIAM, DIST_DOT, N_("MPA Diam(2D)"), CM, &FetalMPACalc },
    {FETAL_RPA_DIAM_VES, DIST_DOT, N_("RPA Diam(2D-Ves)"), CM, NULL},
    {FETAL_LPA_DIAM, DIST_DOT, N_("LPA Diam(2D)"), CM, NULL},
    {FETAL_SVC_DIAM, DIST_DOT, N_("SVC Diam(2D)"), CM, NULL},
    {FETAL_IVC_DIAM, DIST_DOT, N_("IVC Diam(2D)"), CM, NULL},
    //Fetal 2D Valves
    {FETAL_AO_AN_DIAM, DIST_DOT, N_("Ao Annul Diam(2D)"), CM, NULL},
    {FETAL_AO_AN_CIRC, PERI_TRACK, N_("Ao Annul Circ(2D)"), CM, NULL},//轨迹法测周长
    {FETAL_MV_AN_DIAM, DIST_DOT, N_("MV Annul Diam(2D)"), CM, NULL},
    {FETAL_MV_AN_CIRC, PERI_TRACK, N_("MV Annul Circ(2D)"), CM, NULL},//轨迹法测周长
    {FETAL_PV_AN_DIAM, DIST_DOT, N_("PV Annul Diam(2D)"), CM, NULL},
    {FETAL_TV_AN_DIAM, DIST_DOT, N_("TV Annul Diam(2D)"), CM, NULL},
    //Fetal M Dimen
    {FETAL_RVAWD_MM, DEPTH_DIST_M, N_("RVAWd(M)"), CM, NULL},
    {FETAL_RVIDD_MM, DEPTH_DIST_M, N_("RVIDd(M)"), CM, NULL},
    {FETAL_IVSD_MM, DEPTH_DIST_M, N_("IVSd(M)"), CM, &FetalIVSdMCalc},
    {FETAL_LVIDD_MM, DEPTH_DIST_M, N_("LVIDd(M)"), CM, &FetalLVIDdMCalc },//MeanVCFC可以再考虑，其有一个参数LVETc也是计算出来的
    {FETAL_LVPWD_MM, DEPTH_DIST_M, N_("LVPWd(M)"), CM, &FetalLVPWdMCalc},
    {FETAL_IVSS_MM, DEPTH_DIST_M, N_("IVSs(M)"), CM, &FetalIVSsMCalc},
    {FETAL_LVIDS_MM, DEPTH_DIST_M, N_("LVIDs(M)"), CM, &FetalLVIDsMCalc},//MeanVCFC可以再考虑，其有一个参数LVETc也是计算出来的
    {FETAL_LVPWS_MM, DEPTH_DIST_M, N_("LVPWs(M)"), CM, &FetalLVPWsMCalc},
    {FETAL_HR_LV_MM, HR_M, N_("HR-LV(M)"), BPM, NULL},
    {FETAL_LA_DIMEN_MM, DEPTH_DIST_M, N_("LA Dimen(M)"), CM, &FetalLaMMCalc},
    {FETAL_LA_WALL_DIAM_MM, DEPTH_DIST_M, N_("LA Wall Diam(M)"), CM,  NULL},
    {FETAL_RA_WALL_DIAM_MM, DEPTH_DIST_M, N_("RA Wall Diam(M)"), CM, NULL},
    //Fetal M Vessels
    {FETAL_AOR_DIAM_MM, DEPTH_DIST_M, N_("AoR Diam(M)"), CM, &FetalAoRMMCalc},
    {FETAL_ASC_AO_DIAM_MM, DEPTH_DIST_M, N_("Asc Ao Diam(M)"), CM, NULL},
    {FETAL_AO_ARCH_DIAM_MM, DEPTH_DIST_M, N_("Ao Arch Diam(M)"), CM,  NULL},
    {FETAL_AO_ISTHMUS_DIAM_MM, DEPTH_DIST_M, N_("Ao Isthmus Diam(M)"), CM, NULL},
    {FETAL_DESC_AO_DIAM_MM, DEPTH_DIST_M, N_("Desc Ao Diam(M)"), CM, NULL},
    {FETAL_DUCT_ART_DIAM_MM, DEPTH_DIST_M, N_("Duct Art Diam(M)"), CM, NULL},
    {FETAL_LPA_DIAM_MM, DEPTH_DIST_M, N_("LPA Diam(M)"), CM, NULL},
    {FETAL_MPA_DIAM_MM, DEPTH_DIST_M, N_("MPA Diam(M)"), CM, NULL},
    {FETAL_RPA_DIAM_MM, DEPTH_DIST_M, N_("RPA Diam(M)"), CM, NULL},
    //Fetal M AV and MV
    {FETAL_LV_ET_MM, TIME_M, N_("LV ET(M)"), SEC, &FetalLVETCalc },
    {FETAL_LV_PEP_MM, TIME_M, N_("LV PEP(M)"), SEC, &FetalLVPEPCalc },
    {FETAL_AV_CUSP_SEP_MM, DEPTH_DIST_M, N_("AV Cusp Sep(M)"), CM, NULL },
    {FETAL_HR_AV_MM, HR_M, N_("HR-AV(M)"), BPM, &FetalHRAVCalc },
    {FETAL_MV_DE_EXC_MM, DEPTH_DIST_M, N_("MV D-E Exc(M)"), CM, NULL},
    {FETAL_MV_DE_SLOPE_MM, SLOPE, N_("MV D-E Slope(M)"), CMS, NULL},//仅显示斜率
    {FETAL_MV_EF_SLOPE_MM, SLOPE, N_("MV E-F Slope(M)"), CMS, NULL},//仅显示斜率
    {FETAL_MV_EF_SEP_MM, DEPTH_DIST_M, N_("MV E-F Sep(M)"), CM, NULL},
    {FETAL_MV_EPSS_MM, DEPTH_DIST_M, N_("MV EPSS(M)"), CM, NULL},
    {FETAL_MV_AC_INTERVAL_MM, TIME_M, N_("MV A-C Interval(M)"), SEC, NULL},
    //Fetal M PV and TV
    {FETAL_LATE_DIAS_SLOPE_MM, SLOPE, N_("Late Dias Slope(M)"), CMS, NULL},//仅显示斜率
    {FETAL_A_WAVE_AMP_MM, DEPTH_DIST_M, N_("A Wave Amp(M)"), CM,  NULL },
    {FETAL_BC_SLOPE_MM, SLOPE, N_("B-C Slope(M)"), CMS, NULL},//仅显示斜率
    {FETAL_RV_ET_MM, TIME_M, N_("RV ET(M)"), SEC, &FetalRVETCalc},
    {FETAL_RV_PEP_MM, TIME_M, N_("RV PEP(M)"), SEC, &FetalRVPEPCalc },
    {FETAL_HR_PV_MM, HR_M, N_("HR-PV(M)"), BPM, &FetalHRPVCalc },
    {FETAL_TV_DE_EXC_MM, DIST_SLOPE_M, N_("TV D-E Exc(M)"), FETAL_TV_DE_EXC_MULTI, NULL},//显示距离和斜率
    {FETAL_TV_DE_SLOPE_MM, SLOPE, N_("TV D-E Slope(M)"), CMS, NULL},//仅显示斜率
    {FETAL_TV_EF_SLOPE_MM, SLOPE, N_("TV E-F Slope(M)"), CMS, NULL},//仅显示斜率
    {FETAL_TV_AC_INTERVAL_MM, TIME_M, N_("TV A-C Interval(M)"), SEC, NULL},
    //Fetal D Utero and Placenta
    {FETAL_PLACENTA, SD, N_("Placenta(D)"), FETAL_PLACENTA_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_UM_A, SD, N_("Umbilical A(D)"), FETAL_UM_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_UM_V, VEL_D, N_("Umbilical V(D)"), CMS, NULL},//仅测速度
    {FETAL_L_UT_A, SD, N_("L Uterine A(D)"), FETAL_L_UT_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_R_UT_A, SD, N_("R Uterine A(D)"), FETAL_R_UT_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_L_OV_A, SD, N_("L Ovarian A(D)"), FETAL_L_OV_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_R_OV_A, SD, N_("R Ovarian A(D)"), FETAL_R_OV_A_MULTI, NULL},//测PS, ED, RI, S/D
    //Fetal D Vessels
    {FETAL_FROAMEN_OVALE, PGMAX, N_("Foramen Ovale(D)"), FETAL_FROAMEN_OVALE_MULTI, NULL},
    {FETAL_DUCT_ART, SD, N_("Duct Art(D)"), FETAL_DUCT_ART_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_L_COR_A, PGMAX, N_("L Coronary A(D)"), FETAL_L_COR_A_MULTI, NULL},
    {FETAL_R_COR_A, PGMAX, N_("R Coronary A(D)"), FETAL_R_COR_A_MULTI, NULL},
    {FETAL_AOR, PGMAX, N_("AoR(D)"), FETAL_AOR_MULTI, NULL},
    {FETAL_ASC_AO, PGMAX, N_("Asc Ao(D)"), FETAL_ASC_AO_MULTI, NULL},
    {FETAL_AO_ARCH, PGMAX, N_("Ao Arch(D)"), FETAL_AO_ARCH_MULTI, NULL},
    {FETAL_AO_ISTHMUS, PGMAX, N_("Ao Isthmus(D)"), FETAL_AO_ISTHMUS_MULTI, NULL},
    {FETAL_DESC_AO, PGMAX, N_("Desc Ao(D)"), FETAL_DESC_AO_MULTI, NULL},
    {FETAL_MPA, PGMAX, N_("MPA(D)"), FETAL_MPA_MULTI, NULL},
    {FETAL_LPA, PGMAX, N_("LPA(D)"), FETAL_LPA_MULTI, NULL},
    {FETAL_RPA, PGMAX, N_("RPA(D)"), FETAL_RPA_MULTI, NULL},
//  {FETAL_PULM_V, PGMAX, N_("Pulm V"), FETAL_PULM_V_MULTI, NULL},//仅速度测量
    {FETAL_PULM_V, VEL_D, N_("Pulm V(D)"), CMS, NULL},//仅速度测量
    {FETAL_DUCT_VEN, SD, N_("Duct Ven(D)"), FETAL_DUCT_VEN_MULTI, NULL},//测PS, ED, RI, S/D
    //Fetal D AV and MV
    {FETAL_LVOT_ACC_TIME, TIME_D, N_("LVOT Acc Time(D)"), SEC, NULL},
    {FETAL_LVOT_VMAX, PGMAX, N_("LVOT Vmax(D)"), FETAL_LVOT_VMAX_MULTI, NULL},
    {FETAL_AV_ACC_TIME, TIME_D, N_("AV Acc Time(D)"), SEC, NULL},
    {FETAL_AV_VMAX, PGMAX, N_("AV Vmax(D)"), FETAL_AV_VMAX_MULTI, NULL},
    {FETAL_AI_ACC_TIME, TIME_D, N_("AI Acc Time(D)"), SEC, NULL},
    {FETAL_AI_END_DIAS_VEL, PGMAX, N_("AI End Dias Vel(D)"), FETAL_AI_END_DIAS_VEL_MULTI, NULL},
    {FETAL_AI_VMAX, PGMAX, N_("AI Vmax(D)"), FETAL_AI_VMAX_MULTI, NULL},
    {FETAL_HR_AV, HR_D, N_("HR-AV(D)"), FETAL_HR_AV_MULTI, NULL},
    {FETAL_HR_MV, HR_D, N_("HR-MV(D)"), FETAL_HR_MV_MULTI, NULL},
    {FETAL_MV_PEAK_E, VEL_D, N_("MV Peak E(D)"), CMS, &FetalMVEACalc},
    {FETAL_MV_PEAK_A, VEL_D, N_("MV Peak A(D)"), CMS, &FetalMVEACalc},
    {FETAL_MV_ACC_TIME, TIME_D, N_("MV Acc Time(D)"), SEC, NULL},
    {FETAL_MV_DEC_TIME, TIME_D, N_("MV Dec Time(D)"), SEC, NULL},
    {FETAL_MV_VMAX, PGMAX, N_("MV Vmax(D)"), FETAL_MV_VMAX_MULTI,  NULL},
    {FETAL_MR_VMAX, PGMAX, N_("MR Vmax(D)"), FETAL_MR_VMAX_MULTI, NULL},
    //Fetal D PV and TV
    {FETAL_RVOT_ACC_TIME, TIME_D, N_("RVOT Acc Time(D)"), SEC, NULL},
    {FETAL_RVOT_VMAX, PGMAX, N_("RVOT Vmax(D)"), FETAL_RVOT_VMAX_MULTI, NULL},
    {FETAL_PV_ACC_TIME, TIME_D, N_("PV Acc Time(D)"), SEC, NULL},
    {FETAL_PV_VMAX, PGMAX, N_("PV Vmax(D)"), FETAL_PV_VMAX_MULTI, NULL},
    {FETAL_HR_PV, HR_D, N_("HR-PV(D)"), FETAL_HR_PV_MULTI, NULL},
    {FETAL_TV_PEAK_A, VEL_D, N_("TV Peak A(D)"), CMS, &FetalTVEACalc},
    {FETAL_TV_PEAK_E, VEL_D, N_("TV Peak E(D)"), CMS, &FetalTVEACalc},
    {FETAL_TV_ACC_TIME, TIME_D, N_("TV Acc Time(D)"), SEC, NULL},
    {FETAL_TV_DEC_TIME, TIME_D, N_("TV Dec Time(D)"), SEC, NULL},
    {FETAL_TV_VMAX, PGMAX, N_("TV Vmax(D)"), FETAL_TV_VMAX_MULTI, NULL},
    {FETAL_TR_VMAX, PGMAX, N_("TR Vmax(D)"), FETAL_TR_VMAX_MULTI, NULL},
    {FETAL_HR_TV, HR_D, N_("HR-TV(D)"), FETAL_HR_TV_MULTI, NULL},
    //Fetal D Peripheral Vasc
    {FETAL_THORACIC_AO, SD, N_("Thoracic Ao(D)"), FETAL_THORACIC_AO_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_L_RENAL_A, SD, N_("L Renal A(D)"), FETAL_L_RENAL_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_R_RENAL_A, SD, N_("R Renal A(D)"), FETAL_R_RENAL_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_HEPATIC_A, SD, N_("Hepatic A(D)"), FETAL_HEPATIC_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_HEPATIC_V, SD, N_("Hepatic V(D)"), FETAL_HEPATIC_V_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_SPLENIC_A, SD, N_("Splenic A(D)"), FETAL_SPLENIC_A_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_L_MCA, SD, N_("L MCA(D)"), FETAL_L_MCA_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_R_MCA, SD, N_("R MCA(D)"), FETAL_R_MCA_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_L_ICA, SD, N_("L ICA(D)"), FETAL_L_ICA_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_R_ICA, SD, N_("R ICA(D)"), FETAL_R_ICA_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_SVC, SD, N_("SVC(D)"), FETAL_SVC_MULTI, NULL},//测PS, ED, RI, S/D
    {FETAL_IVC, SD, N_("IVC(D)"), FETAL_IVC_MULTI, NULL}//测PS, ED, RI, S/D
};

const MultiItemInfo FetalMultiInfo[] = {
    { FETAL_TV_DE_EXC_MM, FETAL_TV_DE_EXC_MULTI, DIST_SLOPE_M, "TV D-E Exc", SlopeMInfo, NULL },
    { FETAL_PLACENTA, FETAL_PLACENTA_MULTI, SD, "Placenta", SdInfo, NULL },
    { FETAL_UM_A, FETAL_UM_A_MULTI, SD, "Umbilical A", SdInfo, NULL },
    { FETAL_L_UT_A, FETAL_L_UT_A_MULTI, SD, "L Uterine A", SdInfo, NULL },
    { FETAL_R_UT_A, FETAL_R_UT_A_MULTI, SD, "R Uterine A", SdInfo, NULL },
    { FETAL_L_OV_A, FETAL_L_OV_A_MULTI, SD, "L Ovarian A", SdInfo, NULL },
    { FETAL_R_OV_A, FETAL_R_OV_A_MULTI, SD, "R Ovarian A", SdInfo, NULL },
    { FETAL_FROAMEN_OVALE, FETAL_FROAMEN_OVALE_MULTI, PGMAX, "Foramen Ovale", PgmaxInfo, NULL },
    { FETAL_DUCT_ART, FETAL_DUCT_ART_MULTI, SD, "Duct Art", SdInfo, NULL },
    { FETAL_L_COR_A, FETAL_L_COR_A_MULTI, PGMAX, "L Coronary A", PgmaxInfo, NULL },
    { FETAL_R_COR_A, FETAL_R_COR_A_MULTI, PGMAX, "R Corenary A", PgmaxInfo, NULL },
    { FETAL_AOR, FETAL_AOR_MULTI, PGMAX, "AoR", PgmaxInfo, NULL },
    { FETAL_ASC_AO, FETAL_ASC_AO_MULTI, PGMAX, "Asc Ao", PgmaxInfo, NULL },
    { FETAL_AO_ARCH, FETAL_AO_ARCH_MULTI, PGMAX, "Ao Arch", PgmaxInfo, NULL },
    { FETAL_AO_ISTHMUS, FETAL_AO_ISTHMUS_MULTI, PGMAX, "Ao Isthmus", PgmaxInfo, NULL },
    { FETAL_DESC_AO, FETAL_DESC_AO_MULTI, PGMAX, "Desc Ao", PgmaxInfo, NULL },
    { FETAL_MPA, FETAL_MPA_MULTI, PGMAX, "MPA", PgmaxInfo, NULL },
    { FETAL_LPA, FETAL_LPA_MULTI, PGMAX, "LPA", PgmaxInfo, NULL },
    { FETAL_RPA, FETAL_RPA_MULTI, PGMAX, "RPA", PgmaxInfo, NULL },
//  { FETAL_PULM_V, FETAL_PULM_V_MULTI, PGMAX, "Pulm V", {{"Vmax", CMS}, {"PG max", MMHG}, {"\0", NOUNIT}, {"\0", NOUNIT}}, NULL },
    { FETAL_DUCT_VEN, FETAL_DUCT_VEN_MULTI, SD, "Duct Ven", SdInfo, NULL },
    { FETAL_LVOT_VMAX, FETAL_LVOT_VMAX_MULTI, PGMAX, "LVOT Vmax", PgmaxInfo, NULL },
    { FETAL_AV_VMAX, FETAL_AV_VMAX_MULTI, PGMAX, "AV Vmax", PgmaxInfo, NULL },
    { FETAL_AI_END_DIAS_VEL, FETAL_AI_END_DIAS_VEL_MULTI, PGMAX, "AI End Dias Vel", PgmaxInfo, NULL },
    { FETAL_AI_VMAX, FETAL_AI_VMAX_MULTI, PGMAX, "AI Vmax", PgmaxInfo, NULL },
    { FETAL_HR_AV, FETAL_HR_AV_MULTI, HR_D, "HR-AV", HrInfo, NULL },
    { FETAL_HR_MV, FETAL_HR_MV_MULTI, HR_D, "HR-MV", HrInfo, NULL },
    { FETAL_MV_VMAX, FETAL_MV_VMAX_MULTI, PGMAX, "MV Vmax", PgmaxInfo, NULL },
    { FETAL_MR_VMAX, FETAL_MR_VMAX_MULTI, PGMAX, "MR Vmax", PgmaxInfo, NULL },
    { FETAL_RVOT_VMAX, FETAL_RVOT_VMAX_MULTI, PGMAX, "RVOT Vmax", PgmaxInfo, NULL },
    { FETAL_PV_VMAX, FETAL_PV_VMAX_MULTI, PGMAX, "PV Vmax", PgmaxInfo, NULL },
    { FETAL_HR_PV, FETAL_HR_PV_MULTI, HR_D, "HR-PV", HrInfo, NULL },
    { FETAL_TV_VMAX, FETAL_TV_VMAX_MULTI, PGMAX, "TV Vmax", PgmaxInfo, NULL },
    { FETAL_TR_VMAX, FETAL_TR_VMAX_MULTI, PGMAX, "TR Vmax", PgmaxInfo, NULL },
    { FETAL_HR_TV, FETAL_HR_TV_MULTI, HR_D, "HR-TV", HrInfo, NULL },
    { FETAL_THORACIC_AO, FETAL_THORACIC_AO_MULTI, SD, "Thoracic Ao", SdInfo, NULL },
    { FETAL_L_RENAL_A, FETAL_L_RENAL_A_MULTI, SD, "L Renal A", SdInfo, NULL },
    { FETAL_R_RENAL_A, FETAL_R_RENAL_A_MULTI, SD, "R Renal A", SdInfo, NULL },
    { FETAL_HEPATIC_A, FETAL_HEPATIC_A_MULTI, SD, "Hepatic A", SdInfo, NULL },
    { FETAL_HEPATIC_V, FETAL_HEPATIC_V_MULTI, SD, "Hepatic V", SdInfo, NULL },
    { FETAL_SPLENIC_A, FETAL_SPLENIC_A_MULTI, SD, "Splenic A", SdInfo, NULL },
    { FETAL_L_MCA, FETAL_L_MCA_MULTI, SD, "L MCA", SdInfo, NULL },
    { FETAL_R_MCA, FETAL_R_MCA_MULTI, SD, "R MCA", SdInfo, NULL },
    { FETAL_L_ICA, FETAL_L_ICA_MULTI, SD, "L ICA", SdInfo, NULL },
    { FETAL_R_ICA, FETAL_R_ICA_MULTI, SD, "R ICA", SdInfo, NULL },
    { FETAL_SVC, FETAL_SVC_MULTI, SD, "SVC", SdInfo, NULL },
    { FETAL_IVC, FETAL_IVC_MULTI, SD, "IVC", SdInfo, NULL },
};

/******************************VS Measure**************************************/
//const CalcResultInfo VSIcaCca = {VS_ICA_CCA_RATIO, "ICA/CCA", NOUNIT, MeaCalcFun::ICACCACalc};

const CalcResultInfo VSLIcaCcaRatio = {VS_L_ICA_CCA_RATIO, {VS_L_ICA, VS_L_CCA, NO_PAR, NO_PAR, NO_PAR}, "L ICA/CCA", NOUNIT, MeaCalcFun::SectionCalc, NULL};
const CalcResultInfo VSRIcaCcaRatio = {VS_R_ICA_CCA_RATIO, {VS_R_ICA, VS_R_CCA, NO_PAR, NO_PAR, NO_PAR}, "R ICA/CCA", NOUNIT, MeaCalcFun::SectionCalc, NULL};

const CalcInfoArray VSLIcaCcaCalc = {&VSLIcaCcaRatio, NULL};
const CalcInfoArray VSRIcaCcaCalc = {&VSRIcaCcaRatio, NULL};

const PtrOfCalcInfo VSCalcInfoPtr[VS_CALC_END - VS_CALC_START] = {
    {VS_L_ICA_CCA_RATIO, &VSLIcaCcaRatio},
    {VS_R_ICA_CCA_RATIO, &VSRIcaCcaRatio},
};

const SingleItemInfo VSInfo[] = {
    {VS_L_CCA, PSED, N_("L CCA"), VS_L_CCA_MULTI, &VSLIcaCcaCalc},
    {VS_R_CCA, PSED, N_("R CCA"), VS_R_CCA_MULTI, &VSRIcaCcaCalc},
    {VS_L_DIST_CCA, PSED, N_("L Dist CCA"), VS_L_DIST_CCA_MULTI, NULL},
    {VS_R_DIST_CCA, PSED, N_("R Dist CCA"), VS_R_DIST_CCA_MULTI, NULL},
    {VS_L_MID_CCA, PSED, N_("L Mid CCA"), VS_L_MID_CCA_MULTI, NULL},
    {VS_R_MID_CCA, PSED, N_("R Mid CCA"), VS_R_MID_CCA_MULTI, NULL},
    {VS_L_PROX_CCA, PSED, N_("L Prox CCA"), VS_L_PROX_CCA_MULTI, NULL},
    {VS_R_PROX_CCA, PSED, N_("R Prox CCA"), VS_R_PROX_CCA_MULTI, NULL},
    {VS_L_ICA, PSED, N_("L ICA"), VS_L_ICA_MULTI, &VSLIcaCcaCalc},
    {VS_R_ICA, PSED, N_("R ICA"), VS_R_ICA_MULTI, &VSRIcaCcaCalc},
    {VS_L_DIST_ICA, PSED, N_("L Dist ICA"), VS_L_DIST_ICA_MULTI, NULL},
    {VS_R_DIST_ICA, PSED, N_("R Dist ICA"), VS_R_DIST_ICA_MULTI, NULL},
    {VS_L_MID_ICA, PSED, N_("L Mid ICA"), VS_L_MID_ICA_MULTI, NULL},
    {VS_R_MID_ICA, PSED, N_("R Mid ICA"), VS_R_MID_ICA_MULTI, NULL},
    {VS_L_PROX_ICA, PSED, N_("L Prox ICA"), VS_L_PROX_ICA_MULTI, NULL},
    {VS_R_PROX_ICA, PSED, N_("R Prox ICA"), VS_R_PROX_ICA_MULTI, NULL},
    {VS_L_DIST_LSV, PSED, N_("L Dist LSV"), VS_L_DIST_LSV_MULTI, NULL},
    {VS_R_DIST_LSV, PSED, N_("R Dist LSV"), VS_R_DIST_LSV_MULTI, NULL},
    {VS_L_MID_LSV, PSED, N_("L Mid LSV"), VS_L_MID_LSV_MULTI, NULL},
    {VS_R_MID_LSV, PSED, N_("R Mid LSV"), VS_R_MID_LSV_MULTI, NULL},
    {VS_L_PROX_LSV, PSED, N_("L Prox LSV"), VS_L_PROX_LSV_MULTI, NULL},
    {VS_R_PROX_LSV, PSED, N_("R Prox LSV"), VS_R_PROX_LSV_MULTI, NULL},
    {VS_L_DIST_CEPH_V, PSED, N_("L Dist Ceph V"), VS_L_DIST_CEPH_V_MULTI, NULL},
    {VS_R_DIST_CEPH_V, PSED, N_("R Dist Ceph V"), VS_R_DIST_CEPH_V_MULTI, NULL},
    {VS_L_MID_CEPH_V, PSED, N_("L Mid Ceph V"), VS_L_MID_CEPH_V_MULTI, NULL},
    {VS_R_MID_CEPH_V, PSED, N_("R Mid Ceph V"), VS_R_MID_CEPH_V_MULTI, NULL},
    {VS_L_PROX_CEPH_V, PSED, N_("L Prox Ceph V"), VS_L_PROX_CEPH_V_MULTI, NULL},
    {VS_R_PROX_CEPH_V, PSED, N_("R Prox Ceph V"), VS_R_PROX_CEPH_V_MULTI, NULL},
    {VS_L_PROX_ECA, PSED, N_("L Prox ECA"), VS_L_PROX_ECA_MULTI, NULL},
    {VS_R_PROX_ECA, PSED, N_("R Prox ECA"), VS_R_PROX_ECA_MULTI, NULL},
    {VS_L_BRACHIOCEPH_A, PSED, N_("L Brachioceph A"), VS_L_BRACHIOCEPH_A_MULTI, NULL},
    {VS_R_BRACHIOCEPH_A, PSED, N_("R Brachioceph A"), VS_R_BRACHIOCEPH_A_MULTI, NULL},
    {VS_L_BRACHIOCEPH_V, PSED, N_("L Brachioceph V"), VS_L_BRACHIOCEPH_V_MULTI, NULL},
    {VS_R_BRACHIOCEPH_V, PSED, N_("R Brachioceph V"), VS_R_BRACHIOCEPH_V_MULTI, NULL},
    {VS_L_BULB, PSED, N_("L Bulb"), VS_L_BULB_MULTI, NULL},
    {VS_R_BULB, PSED, N_("R Bulb"), VS_R_BULB_MULTI, NULL},
    {VS_L_VERTEBRAL_A, PSED, N_("L Vertebral A"), VS_L_VERTEBRAL_A_MULTI, NULL},
    {VS_R_VERTEBRAL_A, PSED, N_("R Vertebral A"), VS_R_VERTEBRAL_A_MULTI, NULL},
    {VS_L_IJV, PSED, N_("L IJV"), VS_L_IJV_MULTI, NULL},
    {VS_R_IJV, PSED, N_("R IJV"), VS_R_IJV_MULTI, NULL},
    {VS_L_MID_SCL_A, PSED, N_("L Mid SCL A"), VS_L_MID_SCL_A_MULTI, NULL},
    {VS_R_MID_SCL_A, PSED, N_("R Mid SCL A"), VS_R_MID_SCL_A_MULTI, NULL},
    {VS_L_PROX_SCL_A, PSED, N_("L Prox SCL A"), VS_L_PROX_SCL_A_MULTI, NULL},
    {VS_R_PROX_SCL_A, PSED, N_("R Prox SCL A"), VS_R_PROX_SCL_A_MULTI, NULL},
    {VS_L_MID_SCL_V, PSED, N_("L Mid SCL V"), VS_L_MID_SCL_V_MULTI, NULL},
    {VS_R_MID_SCL_V, PSED, N_("R Mid SCL V"), VS_R_MID_SCL_V_MULTI, NULL},
    {VS_L_PROX_SCL_V, PSED, N_("L Prox SCL V"), VS_L_PROX_SCL_V_MULTI, NULL},
    {VS_R_PROX_SCL_V, PSED, N_("R Prox SCL V"), VS_R_PROX_SCL_V_MULTI, NULL},
    {VS_L_CFA, RI, N_("L CFA"), VS_L_CFA_MULTI, NULL},
    {VS_R_CFA, RI, N_("R CFA"), VS_R_CFA_MULTI, NULL},
    {VS_L_CFV, PSED, N_("L CFV"), VS_L_CFV_MULTI, NULL},
    {VS_R_CFV, PSED, N_("R CFV"), VS_R_CFV_MULTI, NULL},
    {VS_L_COM_ILIAC_A, RI, N_("L Com Iliac A"), VS_L_COM_ILIAC_A_MULTI, NULL},
    {VS_R_COM_ILIAC_A, RI, N_("R Com Iliac A"), VS_R_COM_ILIAC_A_MULTI, NULL},
    {VS_L_COM_ILIAC_V, PSED, N_("L Com Iliac V"), VS_L_COM_ILIAC_V_MULTI, NULL},
    {VS_R_COM_ILIAC_V, PSED, N_("R Com Iliac V"), VS_R_COM_ILIAC_V_MULTI, NULL},
    {VS_L_DIST_ATA, RI, N_("L Dist ATA"), VS_L_DIST_ATA_MULTI, NULL},
    {VS_R_DIST_ATA, RI, N_("R Dist ATA"), VS_R_DIST_ATA_MULTI, NULL},
    {VS_L_MID_ATA, RI, N_("L Mid ATA"), VS_L_MID_ATA_MULTI, NULL},
    {VS_R_MID_ATA, RI, N_("R Mid ATA"), VS_R_MID_ATA_MULTI, NULL},
    {VS_L_PROX_ATA, RI, N_("L Prox ATA"), VS_L_PROX_ATA_MULTI, NULL},
    {VS_R_PROX_ATA, RI, N_("R Prox ATA"), VS_R_PROX_ATA_MULTI, NULL},
    {VS_L_DIST_ATV, PSED, N_("L Dist ATV"), VS_L_DIST_ATV_MULTI, NULL},
    {VS_R_DIST_ATV, PSED, N_("R Dist ATV"), VS_R_DIST_ATV_MULTI, NULL},
    {VS_L_MID_ATV, PSED, N_("L Mid ATV"), VS_L_MID_ATV_MULTI, NULL},
    {VS_R_MID_ATV, PSED, N_("R Mid ATV"), VS_R_MID_ATV_MULTI, NULL},
    {VS_L_PROX_ATV, PSED, N_("L Prox ATV"), VS_L_PROX_ATV_MULTI, NULL},
    {VS_R_PROX_ATV, PSED, N_("R Prox ATV"), VS_R_PROX_ATV_MULTI, NULL},
    {VS_L_DIST_GSV_CALF, PSED, N_("L Dist GSV Calf"), VS_L_DIST_GSV_CALF_MULTI, NULL},
    {VS_R_DIST_GSV_CALF, PSED, N_("R Dist GSV Calf"), VS_R_DIST_GSV_CALF_MULTI, NULL},
    {VS_L_MID_GSV_CALF, PSED, N_("L Mid GSV Calf"), VS_L_MID_GSV_CALF_MULTI, NULL},
    {VS_R_MID_GSV_CALF, PSED, N_("R Mid GSV Calf"), VS_R_MID_GSV_CALF_MULTI, NULL},
    {VS_L_PROX_GSV_CALF, PSED, N_("L Prox GSV Calf"), VS_L_PROX_GSV_CALF_MULTI, NULL},
    {VS_R_PROX_GSV_CALF, PSED, N_("R Prox GSV Calf"), VS_R_PROX_GSV_CALF_MULTI, NULL},
    {VS_L_DIST_GSV_THIGH, PSED, N_("L Dist GSV Thigh"), VS_L_DIST_GSV_THIGH_MULTI, NULL},
    {VS_R_DIST_GSV_THIGH, PSED, N_("R Dist GSV Thigh"), VS_R_DIST_GSV_THIGH_MULTI, NULL},
    {VS_L_MID_GSV_THIGH, PSED, N_("L Mid GSV Thigh"), VS_L_MID_GSV_THIGH_MULTI, NULL},
    {VS_R_MID_GSV_THIGH, PSED, N_("R Mid GSV Thigh"), VS_R_MID_GSV_THIGH_MULTI, NULL},
    {VS_L_PROX_GSV_THIGH, PSED, N_("L Prox GSV Thigh"), VS_L_PROX_GSV_THIGH_MULTI, NULL},
    {VS_R_PROX_GSV_THIGH, PSED, N_("R Prox GSV Thigh"), VS_R_PROX_GSV_THIGH_MULTI, NULL},
    {VS_L_DIST_PERO_A, RI, N_("L Dist Pero A"), VS_L_DIST_PERO_A_MULTI, NULL},
    {VS_R_DIST_PERO_A, RI, N_("R Dist Pero A"), VS_R_DIST_PERO_A_MULTI, NULL},
    {VS_L_MID_PERO_A, RI, N_("L Mid Pero A"), VS_L_MID_PERO_A_MULTI, NULL},
    {VS_R_MID_PERO_A, RI, N_("R Mid Pero A"), VS_R_MID_PERO_A_MULTI, NULL},
    {VS_L_PROX_PERO_A, RI, N_("L Prox Pero A"), VS_L_PROX_PERO_A_MULTI, NULL},
    {VS_R_PROX_PERO_A, RI, N_("R Prox Pero A"), VS_R_PROX_PERO_A_MULTI, NULL},
    {VS_L_DIST_PERO_V, PSED, N_("L Dist Pero V"), VS_L_DIST_PERO_V_MULTI, NULL},
    {VS_R_DIST_PERO_V, PSED, N_("R Dist Pero V"), VS_R_DIST_PERO_V_MULTI, NULL},
    {VS_L_MID_PERO_V, PSED, N_("L Mid Pero V"), VS_L_MID_PERO_V_MULTI, NULL},
    {VS_R_MID_PERO_V, PSED, N_("R Mid Pero V"), VS_R_MID_PERO_V_MULTI, NULL},
    {VS_L_PROX_PERO_V, PSED, N_("L Prox Pero V"), VS_L_PROX_PERO_V_MULTI, NULL},
    {VS_R_PROX_PERO_V, PSED, N_("R Prox Pero V"), VS_R_PROX_PERO_V_MULTI, NULL},
    {VS_L_DIST_POP_A, RI, N_("L Dist Pop A"), VS_L_DIST_POP_A_MULTI, NULL},
    {VS_R_DIST_POP_A, RI, N_("R Dist Pop A"), VS_R_DIST_POP_A_MULTI, NULL},
    {VS_L_MID_POP_A, RI, N_("L Mid Pop A"), VS_L_MID_POP_A_MULTI, NULL},
    {VS_R_MID_POP_A, RI, N_("R Mid Pop A"), VS_R_MID_POP_A_MULTI, NULL},
    {VS_L_PROX_POP_A, RI, N_("L Prox Pop A"), VS_L_PROX_POP_A_MULTI, NULL},
    {VS_R_PROX_POP_A, RI, N_("R Prox Pop A"), VS_R_PROX_POP_A_MULTI, NULL},
    {VS_L_DIST_POP_V, PSED, N_("L Dist Pop V"), VS_L_DIST_POP_V_MULTI, NULL},
    {VS_R_DIST_POP_V, PSED, N_("R Dist Pop V"), VS_R_DIST_POP_V_MULTI, NULL},
    {VS_L_MID_POP_V, PSED, N_("L Mid Pop V"), VS_L_MID_POP_V_MULTI, NULL},
    {VS_R_MID_POP_V, PSED, N_("R Mid Pop V"), VS_R_MID_POP_V_MULTI, NULL},
    {VS_L_PROX_POP_V, PSED, N_("L Prox Pop V"), VS_L_PROX_POP_V_MULTI, NULL},
    {VS_R_PROX_POP_V, PSED, N_("R Prox Pop V"), VS_R_PROX_POP_V_MULTI, NULL},
    {VS_L_DIST_PTA, RI, N_("L Dist PTA"), VS_L_DIST_PTA_MULTI, NULL},
    {VS_R_DIST_PTA, RI, N_("R Dist PTA"), VS_R_DIST_PTA_MULTI, NULL},
    {VS_L_MID_PTA, RI, N_("L Mid PTA"), VS_L_MID_PTA_MULTI, NULL},
    {VS_R_MID_PTA, RI, N_("R Mid PTA"), VS_R_MID_PTA_MULTI, NULL},
    {VS_L_PROX_PTA, RI, N_("L Prox PTA"), VS_L_PROX_PTA_MULTI, NULL},
    {VS_R_PROX_PTA, RI, N_("R Prox PTA"), VS_R_PROX_PTA_MULTI, NULL},
    {VS_L_DIST_PTV, PSED, N_("L Dist PTV"), VS_L_DIST_PTV_MULTI, NULL},
    {VS_R_DIST_PTV, PSED, N_("R Dist PTV"), VS_R_DIST_PTV_MULTI, NULL},
    {VS_L_MID_PTV, PSED, N_("L Mid PTV"), VS_L_MID_PTV_MULTI, NULL},
    {VS_R_MID_PTV, PSED, N_("R Mid PTV"), VS_R_MID_PTV_MULTI, NULL},
    {VS_L_PROX_PTV, PSED, N_("L Prox PTV"), VS_L_PROX_PTV_MULTI, NULL},
    {VS_R_PROX_PTV, PSED, N_("R Prox PTV"), VS_R_PROX_PTV_MULTI, NULL},
    {VS_L_DIST_SFA, RI, N_("L Dist SFA"), VS_L_DIST_SFA_MULTI, NULL},
    {VS_R_DIST_SFA, RI, N_("R Dist SFA"), VS_R_DIST_SFA_MULTI, NULL},
    {VS_L_MID_SFA, RI, N_("L Mid SFA"), VS_L_MID_SFA_MULTI, NULL},
    {VS_R_MID_SFA, RI, N_("R Mid SFA"), VS_R_MID_SFA_MULTI, NULL},
    {VS_L_PROX_SFA, RI, N_("L Prox SFA"), VS_L_PROX_SFA_MULTI, NULL},
    {VS_R_PROX_SFA, RI, N_("R Prox SFA"), VS_R_PROX_SFA_MULTI, NULL},
    {VS_L_DIST_SFV, PSED, N_("L Dist SFV"), VS_L_DIST_SFV_MULTI, NULL},
    {VS_R_DIST_SFV, PSED, N_("R Dist SFV"), VS_R_DIST_SFV_MULTI, NULL},
    {VS_L_MID_SFV, PSED, N_("L Mid SFV"), VS_L_MID_SFV_MULTI, NULL},
    {VS_R_MID_SFV, PSED, N_("R Mid SFV"), VS_R_MID_SFV_MULTI, NULL},
    {VS_L_PROX_SFV, PSED, N_("L Prox SFV"), VS_L_PROX_SFV_MULTI, NULL},
    {VS_R_PROX_SFV, PSED, N_("R Prox SFV"), VS_R_PROX_SFV_MULTI, NULL},
    {VS_L_DOR_PEDIS, RI, N_("L Dor Pedis"), VS_L_DOR_PEDIS_MULTI, NULL},
    {VS_R_DOR_PEDIS, RI, N_("R Dor Pedis"), VS_R_DOR_PEDIS_MULTI, NULL},
    {VS_L_EXT_ILIAC_A, RI, N_("L Ext Iliac A"), VS_L_EXT_ILIAC_A_MULTI, NULL},
    {VS_R_EXT_ILIAC_A, RI, N_("R Ext Iliac A"), VS_R_EXT_ILIAC_A_MULTI, NULL},
    {VS_L_EXT_ILIAC_V, PSED, N_("L Ext Iliac V"), VS_L_EXT_ILIAC_V_MULTI, NULL},
    {VS_R_EXT_ILIAC_V, PSED, N_("R Ext Iliac V"), VS_R_EXT_ILIAC_V_MULTI, NULL},
    {VS_L_INT_ILIAC_A, RI, N_("L Int Iliac A"), VS_L_INT_ILIAC_A_MULTI, NULL},
    {VS_R_INT_ILIAC_A, RI, N_("R Int Iliac A"), VS_R_INT_ILIAC_A_MULTI, NULL},
    {VS_L_INT_ILIAC_V, PSED, N_("L Int Iliac V"), VS_L_INT_ILIAC_V_MULTI, NULL},
    {VS_R_INT_ILIAC_V, PSED, N_("R Int Iliac V"), VS_R_INT_ILIAC_V_MULTI, NULL},
    {VS_L_LAT_SURAL_V, PSED, N_("L Lat Sural V"), VS_L_LAT_SURAL_V_MULTI, NULL},
    {VS_R_LAT_SURAL_V, PSED, N_("R Lat Sural V"), VS_R_LAT_SURAL_V_MULTI, NULL},
    {VS_L_PROX_PFA, RI, N_("L Prox PFA"), VS_L_PROX_PFA_MULTI, NULL},
    {VS_R_PROX_PFA, RI, N_("R Prox PFA"), VS_R_PROX_PFA_MULTI, NULL},
    {VS_L_SFJ, PSED, N_("L SFJ"), VS_L_SFJ_MULTI, NULL},
    {VS_R_SFJ, PSED, N_("R SFJ"), VS_R_SFJ_MULTI, NULL},
    {VS_L_ANTECUBE, RI, N_("L Antecube"), VS_L_ANTECUBE_MULTI, NULL},
    {VS_R_ANTECUBE, RI, N_("R Antecube"), VS_R_ANTECUBE_MULTI, NULL},
    {VS_L_AX_A, RI, N_("L Ax A"), VS_L_AX_A_MULTI, NULL},
    {VS_R_AX_A, RI, N_("R Ax A"), VS_R_AX_A_MULTI, NULL},
    {VS_L_AX_V, PSED, N_("L Ax V"), VS_L_AX_V_MULTI, NULL},
    {VS_R_AX_V, PSED, N_("R Ax V"), VS_R_AX_V_MULTI, NULL},
    {VS_L_DIST_BASILIC_V, PSED, N_("L Dist Basilic V"), VS_L_DIST_BASILIC_V_MULTI, NULL},
    {VS_R_DIST_BASILIC_V, PSED, N_("R Dist Basilic V"), VS_R_DIST_BASILIC_V_MULTI, NULL},
    {VS_L_MID_BASILIC_V, PSED, N_("L Mid Basilic V"), VS_L_MID_BASILIC_V_MULTI, NULL},
    {VS_R_MID_BASILIC_V, PSED, N_("R Mid Basilic V"), VS_R_MID_BASILIC_V_MULTI, NULL},
    {VS_L_PROX_BASILIC_V, PSED, N_("L Prox Basilic V"), VS_L_PROX_BASILIC_V_MULTI, NULL},
    {VS_R_PROX_BASILIC_V, PSED, N_("R Prox Basilic V"), VS_R_PROX_BASILIC_V_MULTI, NULL},
    {VS_L_DIST_BRACH_A, RI, N_("L Dist Brach A"), VS_L_DIST_BRACH_A_MULTI, NULL},
    {VS_R_DIST_BRACH_A, RI, N_("R Dist Brach A"), VS_R_DIST_BRACH_A_MULTI, NULL},
    {VS_L_PROX_BRACH_A, RI, N_("L Prox Brach A"), VS_L_PROX_BRACH_A_MULTI, NULL},
    {VS_R_PROX_BRACH_A, RI, N_("R Prox Brach A"), VS_R_PROX_BRACH_A_MULTI, NULL},
    {VS_L_DIST_BRACH_V, RI, N_("L Dist Brach V"), VS_L_DIST_BRACH_V_MULTI, NULL},
    {VS_R_DIST_BRACH_V, RI, N_("R Dist Brach V"), VS_R_DIST_BRACH_V_MULTI, NULL},
    {VS_L_PROX_BRACH_V, RI, N_("L Prox Brach V"), VS_L_PROX_BRACH_V_MULTI, NULL},
    {VS_R_PROX_BRACH_V, RI, N_("R Prox Brach V"), VS_R_PROX_BRACH_V_MULTI, NULL},
    {VS_L_DIST_RAD_A, RI, N_("L Dist Rad A"), VS_L_DIST_RAD_A_MULTI, NULL},
    {VS_R_DIST_RAD_A, RI, N_("R Dist Rad A"), VS_R_DIST_RAD_A_MULTI, NULL},
    {VS_L_MID_RAD_A, RI, N_("L Mid Rad A"), VS_L_MID_RAD_A_MULTI, NULL},
    {VS_R_MID_RAD_A, RI, N_("R Mid Rad A"), VS_R_MID_RAD_A_MULTI, NULL},
    {VS_L_PROX_RAD_A, RI, N_("L Prox Rad A"), VS_L_PROX_RAD_A_MULTI, NULL},
    {VS_R_PROX_RAD_A, RI, N_("R Prox Rad A"), VS_R_PROX_RAD_A_MULTI, NULL},
    {VS_L_DIST_RAD_V, PSED, N_("L Dist Rad V"), VS_L_DIST_RAD_V_MULTI, NULL},
    {VS_R_DIST_RAD_V, PSED, N_("R Dist Rad V"), VS_R_DIST_RAD_V_MULTI, NULL},
    {VS_L_MID_RAD_V, PSED, N_("L Mid Rad V"), VS_L_MID_RAD_V_MULTI, NULL},
    {VS_R_MID_RAD_V, PSED, N_("R Mid Rad V"), VS_R_MID_RAD_V_MULTI, NULL},
    {VS_L_PROX_RAD_V, PSED, N_("L Prox Rad V"), VS_L_PROX_RAD_V_MULTI, NULL},
    {VS_R_PROX_RAD_V, PSED, N_("R Prox Rad V"), VS_R_PROX_RAD_V_MULTI, NULL},
    {VS_L_DIST_ULNAR_A, RI, N_("L Dist Ulnar A"), VS_L_DIST_ULNAR_A_MULTI, NULL},
    {VS_R_DIST_ULNAR_A, RI, N_("R Dist Ulnar A"), VS_R_DIST_ULNAR_A_MULTI, NULL},
    {VS_L_MID_ULNAR_A, RI, N_("L Mid Ulnar A"), VS_L_MID_ULNAR_A_MULTI, NULL},
    {VS_R_MID_ULNAR_A, RI, N_("R Mid Ulnar A"), VS_R_MID_ULNAR_A_MULTI, NULL},
    {VS_L_PROX_ULNAR_A, RI, N_("L Prox Ulnar A"), VS_L_PROX_ULNAR_A_MULTI, NULL},
    {VS_R_PROX_ULNAR_A, RI, N_("R Prox Ulnar A"), VS_R_PROX_ULNAR_A_MULTI, NULL},
    {VS_L_DIST_ULNAR_V, PSED, N_("L Dist Ulnar V"), VS_L_DIST_ULNAR_V_MULTI, NULL},
    {VS_R_DIST_ULNAR_V, PSED, N_("R Dist Ulnar V"), VS_R_DIST_ULNAR_V_MULTI, NULL},
    {VS_L_MID_ULNAR_V, PSED, N_("L Mid Ulnar V"), VS_L_MID_ULNAR_V_MULTI, NULL},
    {VS_R_MID_ULNAR_V, PSED, N_("R Mid Ulnar V"), VS_R_MID_ULNAR_V_MULTI, NULL},
    {VS_L_PROX_ULNAR_V, PSED, N_("L Prox Ulnar V"), VS_L_PROX_ULNAR_V_MULTI, NULL},
    {VS_R_PROX_ULNAR_V, PSED, N_("R Prox Ulnar V"), VS_R_PROX_ULNAR_V_MULTI, NULL},
//  {VS_L_MID_SCL_A, PSED, "L Mid SCL A", VS_L_MID_SCL_A_MULTI, NULL},
//  {VS_R_MID_SCL_A, PSED, "R Mid SCL A", VS_R_MID_SCL_A_MULTI, NULL},
//  {VS_L_PROX_SCL_A, PSED, "L Prox SCL A", VS_L_PROX_SCL_A_MULTI, NULL},
//  {VS_R_PROX_SCL_A, PSED, "R Prox SCL A", VS_R_PROX_SCL_A_MULTI, NULL},
//  {VS_L_MID_SCL_V, PSED, "L Mid SCL V", VS_L_MID_SCL_V_MULTI, NULL},
//  {VS_R_MID_SCL_V, PSED, "R Mid SCL V", VS_R_MID_SCL_V_MULTI, NULL},
//  {VS_L_PROX_SCL_V, PSED, "L Prox SCL V", VS_L_PROX_SCL_V_MULTI, NULL},
//  {VS_R_PROX_SCL_V, PSED, "R Prox SCL V", VS_R_PROX_SCL_V_MULTI, NULL},
};

/*
 *VS和TCD是以有多项测量的测量项为主的科别，在VSMultiInfo和TCDMultiInfo中，如果某项是单项测量，
 *则titleUnit[MEA_MULTI-1].unitItem存储此单项测量的枚举号，程序根据此枚举号寻找它在VSInfo中的位置
 * */
const MultiItemInfo VSMultiInfo[] = {
    {VS_L_CCA, VS_L_CCA_MULTI, PSED, "L CCA", PsEdInfo, &VSLIcaCcaCalc},
    {VS_R_CCA, VS_R_CCA_MULTI, PSED, "R CCA", PsEdInfo, &VSRIcaCcaCalc},
    {VS_L_DIST_CCA, VS_L_DIST_CCA_MULTI, PSED, "L Dist CCA", PsEdInfo, NULL},
    {VS_R_DIST_CCA, VS_R_DIST_CCA_MULTI, PSED, "R Dist CCA", PsEdInfo, NULL},
    {VS_L_MID_CCA, VS_L_MID_CCA_MULTI, PSED, "L Mid CCA", PsEdInfo, NULL},
    {VS_R_MID_CCA, VS_R_MID_CCA_MULTI, PSED, "R Mid CCA", PsEdInfo, NULL},
    {VS_L_PROX_CCA, VS_L_PROX_CCA_MULTI, PSED, "L Prox CCA", PsEdInfo, NULL},
    {VS_R_PROX_CCA, VS_R_PROX_CCA_MULTI, PSED, "R Prox CCA", PsEdInfo, NULL},
    {VS_L_ICA, VS_L_ICA_MULTI, PSED, "L ICA", PsEdInfo,  &VSLIcaCcaCalc},  //所有ICA的测量文档上写的都是Vel测量，实际上动脉不应该只测一个速度，在纸质资料上漏写了，所以误认为只有速度测量
    {VS_R_ICA, VS_R_ICA_MULTI, PSED, "R ICA", PsEdInfo, &VSRIcaCcaCalc},
    {VS_L_DIST_ICA, VS_L_DIST_ICA_MULTI, PSED, "L Dist ICA", PsEdInfo, NULL},
    {VS_R_DIST_ICA, VS_R_DIST_ICA_MULTI, PSED, "R Dist ICA", PsEdInfo, NULL},
    {VS_L_MID_ICA, VS_L_MID_ICA_MULTI, PSED, "L Mid ICA", PsEdInfo, NULL},
    {VS_R_MID_ICA, VS_R_MID_ICA_MULTI, PSED, "R Mid ICA", PsEdInfo, NULL},
    {VS_L_PROX_ICA, VS_L_PROX_ICA_MULTI, PSED, "L Prox ICA", PsEdInfo, NULL},
    {VS_R_PROX_ICA, VS_R_PROX_ICA_MULTI, PSED, "R Prox ICA", PsEdInfo, NULL},
    {VS_L_DIST_LSV, VS_L_DIST_LSV_MULTI, PSED, "L Dist LSV", PsEdInfo, NULL},
    {VS_R_DIST_LSV, VS_R_DIST_LSV_MULTI, PSED, "R Dist LSV", PsEdInfo, NULL},
    {VS_L_MID_LSV, VS_L_MID_LSV_MULTI, PSED, "L Mid LSV", PsEdInfo, NULL},
    {VS_R_MID_LSV, VS_R_MID_LSV_MULTI, PSED, "R Mid LSV", PsEdInfo, NULL},
    {VS_L_PROX_LSV, VS_L_PROX_LSV_MULTI, PSED, "L Prox LSV", PsEdInfo, NULL},
    {VS_R_PROX_LSV, VS_R_PROX_LSV_MULTI, PSED, "R Prox LSV", PsEdInfo, NULL},
    {VS_L_DIST_CEPH_V, VS_L_DIST_CEPH_V_MULTI, PSED, "L Dist Ceph V", PsEdInfo, NULL},
    {VS_R_DIST_CEPH_V, VS_R_DIST_CEPH_V_MULTI, PSED, "R Dist Ceph V", PsEdInfo, NULL},
    {VS_L_MID_CEPH_V, VS_L_MID_CEPH_V_MULTI, PSED, "L Mid Ceph V", PsEdInfo, NULL},
    {VS_R_MID_CEPH_V, VS_R_MID_CEPH_V_MULTI, PSED, "R Mid Ceph V", PsEdInfo, NULL},
    {VS_L_PROX_CEPH_V, VS_L_PROX_CEPH_V_MULTI, PSED, "L Prox Ceph V", PsEdInfo, NULL},
    {VS_R_PROX_CEPH_V, VS_R_PROX_CEPH_V_MULTI, PSED, "R Prox Ceph V", PsEdInfo, NULL},
    {VS_L_PROX_ECA, VS_L_PROX_ECA_MULTI, PSED, "L Prox ECA", PsEdInfo, NULL},
    {VS_R_PROX_ECA, VS_R_PROX_ECA_MULTI, PSED, "R Prox ECA", PsEdInfo, NULL},
    {VS_L_BRACHIOCEPH_A, VS_L_BRACHIOCEPH_A_MULTI, PSED, "L Brachioceph A", PsEdInfo, NULL},
    {VS_R_BRACHIOCEPH_A, VS_R_BRACHIOCEPH_A_MULTI, PSED, "R Brachioceph A", PsEdInfo, NULL},
    {VS_L_BRACHIOCEPH_V, VS_L_BRACHIOCEPH_V_MULTI, PSED, "L Brachioceph V", PsEdInfo, NULL},
    {VS_R_BRACHIOCEPH_V, VS_R_BRACHIOCEPH_V_MULTI, PSED, "R Brachioceph V", PsEdInfo, NULL},
    {VS_L_BULB, VS_L_BULB_MULTI, PSED, "L Bulb", PsEdInfo, NULL},
    {VS_R_BULB, VS_R_BULB_MULTI, PSED, "R Bulb", PsEdInfo, NULL},
    {VS_L_VERTEBRAL_A, VS_L_VERTEBRAL_A_MULTI, PSED, "L Vertebral A", PsEdInfo, NULL},
    {VS_R_VERTEBRAL_A, VS_R_VERTEBRAL_A_MULTI, PSED, "R Vertebral A", PsEdInfo, NULL},
    {VS_L_IJV, VS_L_IJV_MULTI, PSED, "L IJV", PsEdInfo, NULL},
    {VS_R_IJV, VS_R_IJV_MULTI, PSED, "R IJV", PsEdInfo, NULL},
    {VS_L_CFA, VS_L_CFA_MULTI, RI, "L CFA", RiInfo, NULL},
    {VS_R_CFA, VS_R_CFA_MULTI, RI, "R CFA", RiInfo, NULL},
    {VS_L_CFV, VS_L_CFV_MULTI, PSED, "L CFV", PsEdInfo, NULL},
    {VS_R_CFV, VS_R_CFV_MULTI, PSED, "R CFV", PsEdInfo, NULL},
    {VS_L_COM_ILIAC_A, VS_L_COM_ILIAC_A_MULTI, RI, "L Com Iliac A", RiInfo, NULL},
    {VS_R_COM_ILIAC_A, VS_R_COM_ILIAC_A_MULTI, RI, "R Com Iliac A", RiInfo, NULL},
    {VS_L_COM_ILIAC_V, VS_L_COM_ILIAC_V_MULTI, PSED, "L Com Iliac V", PsEdInfo, NULL},
    {VS_R_COM_ILIAC_V, VS_R_COM_ILIAC_V_MULTI, PSED, "R Com Iliac V", PsEdInfo, NULL},
    {VS_L_DIST_ATA, VS_L_DIST_ATA_MULTI, RI, "L Dist ATA", RiInfo, NULL},
    {VS_R_DIST_ATA, VS_R_DIST_ATA_MULTI, RI, "R Dist ATA", RiInfo, NULL},
    {VS_L_MID_ATA, VS_L_MID_ATA_MULTI, RI, "L Mid ATA", RiInfo, NULL},
    {VS_R_MID_ATA, VS_R_MID_ATA_MULTI, RI, "R Mid ATA", RiInfo, NULL},
    {VS_L_PROX_ATA, VS_L_PROX_ATA_MULTI, RI, "L Prox ATA", RiInfo, NULL},
    {VS_R_PROX_ATA, VS_R_PROX_ATA_MULTI, RI, "R Prox ATA", RiInfo, NULL},
    {VS_L_DIST_ATV, VS_L_DIST_ATV_MULTI, PSED, "L Dist ATV", PsEdInfo, NULL},
    {VS_R_DIST_ATV, VS_R_DIST_ATV_MULTI, PSED, "R Dist ATV", PsEdInfo, NULL},
    {VS_L_MID_ATV, VS_L_MID_ATV_MULTI, PSED, "L Mid ATV", PsEdInfo, NULL},
    {VS_R_MID_ATV, VS_R_MID_ATV_MULTI, PSED, "R Mid ATV", PsEdInfo, NULL},
    {VS_L_PROX_ATV, VS_L_PROX_ATV_MULTI, PSED, "L Prox ATV", PsEdInfo, NULL},
    {VS_R_PROX_ATV, VS_R_PROX_ATV_MULTI, PSED, "R Prox ATV", PsEdInfo, NULL},
    {VS_L_DIST_GSV_CALF, VS_L_DIST_GSV_CALF_MULTI, PSED, "L Dist GSV Calf", PsEdInfo, NULL},
    {VS_R_DIST_GSV_CALF, VS_R_DIST_GSV_CALF_MULTI, PSED, "R Dist GSV Calf", PsEdInfo, NULL},
    {VS_L_MID_GSV_CALF, VS_L_MID_GSV_CALF_MULTI, PSED, "L Mid GSV Calf", PsEdInfo, NULL},
    {VS_R_MID_GSV_CALF, VS_R_MID_GSV_CALF_MULTI, PSED, "R Mid GSV Calf", PsEdInfo, NULL},
    {VS_L_PROX_GSV_CALF, VS_L_PROX_GSV_CALF_MULTI, PSED, "L Prox GSV Calf", PsEdInfo, NULL},
    {VS_R_PROX_GSV_CALF, VS_R_PROX_GSV_CALF_MULTI, PSED, "R Prox GSV Calf", PsEdInfo, NULL},
    {VS_L_DIST_GSV_THIGH, VS_L_DIST_GSV_THIGH_MULTI, PSED, "L Dist GSV Thigh", PsEdInfo, NULL},
    {VS_R_DIST_GSV_THIGH, VS_R_DIST_GSV_THIGH_MULTI, PSED, "R Dist GSV Thigh", PsEdInfo, NULL},
    {VS_L_MID_GSV_THIGH, VS_L_MID_GSV_THIGH_MULTI, PSED, "L Mid GSV Thigh", PsEdInfo, NULL},
    {VS_R_MID_GSV_THIGH, VS_R_MID_GSV_THIGH_MULTI, PSED, "R Mid GSV Thigh", PsEdInfo, NULL},
    {VS_L_PROX_GSV_THIGH, VS_L_PROX_GSV_THIGH_MULTI, PSED, "L Prox GSV Thigh", PsEdInfo, NULL},
    {VS_R_PROX_GSV_THIGH, VS_R_PROX_GSV_THIGH_MULTI, PSED, "R Prox GSV Thigh", PsEdInfo, NULL},
    {VS_L_DIST_PERO_A, VS_L_DIST_PERO_A_MULTI, RI, "L Dist Pero A", RiInfo, NULL},
    {VS_R_DIST_PERO_A, VS_R_DIST_PERO_A_MULTI, RI, "R Dist Pero A", RiInfo, NULL},
    {VS_L_MID_PERO_A, VS_L_MID_PERO_A_MULTI, RI, "L Mid Pero A", RiInfo, NULL},
    {VS_R_MID_PERO_A, VS_R_MID_PERO_A_MULTI, RI, "R Mid Pero A", RiInfo, NULL},
    {VS_L_PROX_PERO_A, VS_L_PROX_PERO_A_MULTI, RI, "L Prox Pero A", RiInfo, NULL},
    {VS_R_PROX_PERO_A, VS_R_PROX_PERO_A_MULTI, RI, "R Prox Pero A", RiInfo, NULL},
    {VS_L_DIST_PERO_V, VS_L_DIST_PERO_V_MULTI, PSED, "L Dist Pero V", PsEdInfo, NULL},
    {VS_R_DIST_PERO_V, VS_R_DIST_PERO_V_MULTI, PSED, "R Dist Pero V", PsEdInfo, NULL},
    {VS_L_MID_PERO_V, VS_L_MID_PERO_V_MULTI, PSED, "L Mid Pero V", PsEdInfo, NULL},
    {VS_R_MID_PERO_V, VS_R_MID_PERO_V_MULTI, PSED, "R Mid Pero V", PsEdInfo, NULL},
    {VS_L_PROX_PERO_V, VS_L_PROX_PERO_V_MULTI, PSED, "L Prox Pero V", PsEdInfo, NULL},
    {VS_R_PROX_PERO_V, VS_R_PROX_PERO_V_MULTI, PSED, "R Prox Pero V", PsEdInfo, NULL},
    {VS_L_DIST_POP_A, VS_L_DIST_POP_A_MULTI, RI, "L Dist Pop A", RiInfo, NULL},
    {VS_R_DIST_POP_A, VS_R_DIST_POP_A_MULTI, RI, "R Dist Pop A", RiInfo, NULL},
    {VS_L_MID_POP_A, VS_L_MID_POP_A_MULTI, RI, "L Mid Pop A", RiInfo, NULL},
    {VS_R_MID_POP_A, VS_R_MID_POP_A_MULTI, RI, "R Mid Pop A", RiInfo, NULL},
    {VS_L_PROX_POP_A, VS_L_PROX_POP_A_MULTI, RI, "L Prox Pop A", RiInfo, NULL},
    {VS_R_PROX_POP_A, VS_R_PROX_POP_A_MULTI, RI, "R Prox Pop A", RiInfo, NULL},
    {VS_L_DIST_POP_V, VS_L_DIST_POP_V_MULTI, PSED, "L Dist Pop V", PsEdInfo, NULL},
    {VS_R_DIST_POP_V, VS_R_DIST_POP_V_MULTI, PSED, "R Dist Pop V", PsEdInfo, NULL},
    {VS_L_MID_POP_V, VS_L_MID_POP_V_MULTI, PSED, "L Mid Pop V", PsEdInfo, NULL},
    {VS_R_MID_POP_V, VS_R_MID_POP_V_MULTI, PSED, "R Mid Pop V", PsEdInfo, NULL},
    {VS_L_PROX_POP_V, VS_L_PROX_POP_V_MULTI, PSED, "L Prox Pop V", PsEdInfo, NULL},
    {VS_R_PROX_POP_V, VS_R_PROX_POP_V_MULTI, PSED, "R Prox Pop V", PsEdInfo, NULL},
    {VS_L_DIST_PTA, VS_L_DIST_PTA_MULTI, RI, "L Dist PTA", RiInfo, NULL},
    {VS_R_DIST_PTA, VS_R_DIST_PTA_MULTI, RI, "R Dist PTA", RiInfo, NULL},
    {VS_L_MID_PTA, VS_L_MID_PTA_MULTI, RI, "L Mid PTA", RiInfo, NULL},
    {VS_R_MID_PTA, VS_R_MID_PTA_MULTI, RI, "R Mid PTA", RiInfo, NULL},
    {VS_L_PROX_PTA, VS_L_PROX_PTA_MULTI, RI, "L Prox PTA", RiInfo, NULL},
    {VS_R_PROX_PTA, VS_R_PROX_PTA_MULTI, RI, "R Prox PTA", RiInfo, NULL},
    {VS_L_DIST_PTV, VS_L_DIST_PTV_MULTI, PSED, "L Dist PTV", PsEdInfo, NULL},
    {VS_R_DIST_PTV, VS_R_DIST_PTV_MULTI, PSED, "R Dist PTV", PsEdInfo, NULL},
    {VS_L_MID_PTV, VS_L_MID_PTV_MULTI, PSED, "L Mid PTV", PsEdInfo, NULL},
    {VS_R_MID_PTV, VS_R_MID_PTV_MULTI, PSED, "R Mid PTV", PsEdInfo, NULL},
    {VS_L_PROX_PTV, VS_L_PROX_PTV_MULTI, PSED, "L Prox PTV", PsEdInfo, NULL},
    {VS_R_PROX_PTV, VS_R_PROX_PTV_MULTI, PSED, "R Prox PTV", PsEdInfo, NULL},
    {VS_L_DIST_SFA, VS_L_DIST_SFA_MULTI, RI, "L Dist SFA", RiInfo, NULL},
    {VS_R_DIST_SFA, VS_R_DIST_SFA_MULTI, RI, "R Dist SFA", RiInfo, NULL},
    {VS_L_MID_SFA, VS_L_MID_SFA_MULTI, RI, "L Mid SFA", RiInfo, NULL},
    {VS_R_MID_SFA, VS_R_MID_SFA_MULTI, RI, "R Mid SFA", RiInfo, NULL},
    {VS_L_PROX_SFA, VS_L_PROX_SFA_MULTI, RI, "L Prox SFA", RiInfo, NULL},
    {VS_R_PROX_SFA, VS_R_PROX_SFA_MULTI, RI, "R Prox SFA", RiInfo, NULL},
    {VS_L_DIST_SFV, VS_L_DIST_SFV_MULTI, PSED, "L Dist SFV", PsEdInfo, NULL},
    {VS_R_DIST_SFV, VS_R_DIST_SFV_MULTI, PSED, "R Dist SFV", PsEdInfo, NULL},
    {VS_L_MID_SFV, VS_L_MID_SFV_MULTI, PSED, "L Mid SFV", PsEdInfo, NULL},
    {VS_R_MID_SFV, VS_R_MID_SFV_MULTI, PSED, "R Mid SFV", PsEdInfo, NULL},
    {VS_L_PROX_SFV, VS_L_PROX_SFV_MULTI, PSED, "L Prox SFV", PsEdInfo, NULL},
    {VS_R_PROX_SFV, VS_R_PROX_SFV_MULTI, PSED, "R Prox SFV", PsEdInfo, NULL},
    {VS_L_DOR_PEDIS, VS_L_DOR_PEDIS_MULTI, RI, "L Dor Pedis", RiInfo, NULL},
    {VS_R_DOR_PEDIS, VS_R_DOR_PEDIS_MULTI, RI, "R Dor Pedis", RiInfo, NULL},
    {VS_L_EXT_ILIAC_A, VS_L_EXT_ILIAC_A_MULTI, RI, "L Ext Iliac A", RiInfo, NULL},
    {VS_R_EXT_ILIAC_A, VS_R_EXT_ILIAC_A_MULTI, RI, "R Ext Iliac A", RiInfo, NULL},
    {VS_L_EXT_ILIAC_V, VS_L_EXT_ILIAC_V_MULTI, PSED, "L Ext Iliac V", PsEdInfo, NULL},
    {VS_R_EXT_ILIAC_V, VS_R_EXT_ILIAC_V_MULTI, PSED, "R Ext Iliac V", PsEdInfo, NULL},
    {VS_L_INT_ILIAC_A, VS_L_INT_ILIAC_A_MULTI, RI, "L Int Iliac A", RiInfo, NULL},
    {VS_R_INT_ILIAC_A, VS_R_INT_ILIAC_A_MULTI, RI, "R Int Iliac A", RiInfo, NULL},
    {VS_L_INT_ILIAC_V, VS_L_INT_ILIAC_V_MULTI, PSED, "L Int Iliac V", PsEdInfo, NULL},
    {VS_R_INT_ILIAC_V, VS_R_INT_ILIAC_V_MULTI, PSED, "R Int Iliac V", PsEdInfo, NULL},
    {VS_L_LAT_SURAL_V, VS_L_LAT_SURAL_V_MULTI, PSED, "L Lat Sural V", PsEdInfo, NULL},
    {VS_R_LAT_SURAL_V, VS_R_LAT_SURAL_V_MULTI, PSED, "R Lat Sural V", PsEdInfo, NULL},
    {VS_L_PROX_PFA, VS_L_PROX_PFA_MULTI, RI, "L Prox PFA", RiInfo, NULL},
    {VS_R_PROX_PFA, VS_R_PROX_PFA_MULTI, RI, "R Prox PFA", RiInfo, NULL},
    {VS_L_SFJ, VS_L_SFJ_MULTI, PSED, "L SFJ", PsEdInfo, NULL},
    {VS_R_SFJ, VS_R_SFJ_MULTI, PSED, "R SFJ", PsEdInfo, NULL},
    {VS_L_ANTECUBE, VS_L_ANTECUBE_MULTI, RI, "L Antecube", RiInfo, NULL},
    {VS_R_ANTECUBE, VS_R_ANTECUBE_MULTI, RI, "R Antecube", RiInfo, NULL},
    {VS_L_AX_A, VS_L_AX_A_MULTI, RI, "L Ax A", RiInfo, NULL},
    {VS_R_AX_A, VS_R_AX_A_MULTI, RI, "R Ax A", RiInfo, NULL},
    {VS_L_AX_V, VS_L_AX_V_MULTI, PSED, "L Ax V", PsEdInfo, NULL},
    {VS_R_AX_V, VS_R_AX_V_MULTI, PSED, "R Ax V", PsEdInfo, NULL},
    {VS_L_DIST_BASILIC_V, VS_L_DIST_BASILIC_V_MULTI, PSED, "L Dist Basilic V", PsEdInfo, NULL},
    {VS_R_DIST_BASILIC_V, VS_R_DIST_BASILIC_V_MULTI, PSED, "R Dist Basilic V", PsEdInfo, NULL},
    {VS_L_MID_BASILIC_V, VS_L_MID_BASILIC_V_MULTI, PSED, "L Mid Basilic V", PsEdInfo, NULL},
    {VS_R_MID_BASILIC_V, VS_R_MID_BASILIC_V_MULTI, PSED, "R Mid Basilic V", PsEdInfo, NULL},
    {VS_L_PROX_BASILIC_V, VS_L_PROX_BASILIC_V_MULTI, PSED, "L Prox Basilic V", PsEdInfo, NULL},
    {VS_R_PROX_BASILIC_V, VS_R_PROX_BASILIC_V_MULTI, PSED, "R Prox Basilic V", PsEdInfo, NULL},
    {VS_L_DIST_BRACH_A, VS_L_DIST_BRACH_A_MULTI, RI, "L Dist Brach A", RiInfo, NULL},
    {VS_R_DIST_BRACH_A, VS_R_DIST_BRACH_A_MULTI, RI, "R Dist Brach A", RiInfo, NULL},
    {VS_L_PROX_BRACH_A, VS_L_PROX_BRACH_A_MULTI, RI, "L Prox Brach A", RiInfo, NULL},
    {VS_R_PROX_BRACH_A, VS_R_PROX_BRACH_A_MULTI, RI, "R Prox Brach A", RiInfo, NULL},
    {VS_L_DIST_BRACH_V, VS_L_DIST_BRACH_V_MULTI, RI, "L Dist Brach V", RiInfo, NULL},
    {VS_R_DIST_BRACH_V, VS_R_DIST_BRACH_V_MULTI, RI, "R Dist Brach V", RiInfo, NULL},
    {VS_L_PROX_BRACH_V, VS_L_PROX_BRACH_V_MULTI, RI, "L Prox Brach V", RiInfo, NULL},
    {VS_R_PROX_BRACH_V, VS_R_PROX_BRACH_V_MULTI, RI, "R Prox Brach V", RiInfo, NULL},
    {VS_L_DIST_RAD_A, VS_L_DIST_RAD_A_MULTI, RI, "L Dist Rad A", RiInfo, NULL},
    {VS_R_DIST_RAD_A, VS_R_DIST_RAD_A_MULTI, RI, "R Dist Rad A", RiInfo, NULL},
    {VS_L_MID_RAD_A, VS_L_MID_RAD_A_MULTI, RI, "L Mid Rad A", RiInfo, NULL},
    {VS_R_MID_RAD_A, VS_R_MID_RAD_A_MULTI, RI, "R Mid Rad A", RiInfo, NULL},
    {VS_L_PROX_RAD_A, VS_L_PROX_RAD_A_MULTI, RI, "L Prox Rad A", RiInfo, NULL},
    {VS_R_PROX_RAD_A, VS_R_PROX_RAD_A_MULTI, RI, "R Prox Rad A", RiInfo, NULL},
    {VS_L_DIST_RAD_V, VS_L_DIST_RAD_V_MULTI, PSED, "L Dist Rad V", PsEdInfo, NULL},
    {VS_R_DIST_RAD_V, VS_R_DIST_RAD_V_MULTI, PSED, "R Dist Rad V", PsEdInfo, NULL},
    {VS_L_MID_RAD_V, VS_L_MID_RAD_V_MULTI, PSED, "L Mid Rad V", PsEdInfo, NULL},
    {VS_R_MID_RAD_V, VS_R_MID_RAD_V_MULTI, PSED, "R Mid Rad V", PsEdInfo, NULL},
    {VS_L_PROX_RAD_V, VS_L_PROX_RAD_V_MULTI, PSED, "L Prox Rad V", PsEdInfo, NULL},
    {VS_R_PROX_RAD_V, VS_R_PROX_RAD_V_MULTI, PSED, "R Prox Rad V", PsEdInfo, NULL},
    {VS_L_DIST_ULNAR_A, VS_L_DIST_ULNAR_A_MULTI, RI, "L Dist Ulnar A", RiInfo, NULL},
    {VS_R_DIST_ULNAR_A, VS_R_DIST_ULNAR_A_MULTI, RI, "R Dist Ulnar A", RiInfo, NULL},
    {VS_L_MID_ULNAR_A, VS_L_MID_ULNAR_A_MULTI, RI, "L Mid Ulnar A", RiInfo, NULL},
    {VS_R_MID_ULNAR_A, VS_R_MID_ULNAR_A_MULTI, RI, "R Mid Ulnar A", RiInfo, NULL},
    {VS_L_PROX_ULNAR_A, VS_L_PROX_ULNAR_A_MULTI, RI, "L Prox Ulnar A", RiInfo, NULL},
    {VS_R_PROX_ULNAR_A, VS_R_PROX_ULNAR_A_MULTI, RI, "R Prox Ulnar A", RiInfo, NULL},
    {VS_L_DIST_ULNAR_V, VS_L_DIST_ULNAR_V_MULTI, PSED, "L Dist Ulnar V", PsEdInfo, NULL},
    {VS_R_DIST_ULNAR_V, VS_R_DIST_ULNAR_V_MULTI, PSED, "R Dist Ulnar V", PsEdInfo, NULL},
    {VS_L_MID_ULNAR_V, VS_L_MID_ULNAR_V_MULTI, PSED, "L Mid Ulnar V", PsEdInfo, NULL},
    {VS_R_MID_ULNAR_V, VS_R_MID_ULNAR_V_MULTI, PSED, "R Mid Ulnar V", PsEdInfo, NULL},
    {VS_L_PROX_ULNAR_V, VS_L_PROX_ULNAR_V_MULTI, PSED, "L Prox Ulnar V", PsEdInfo, NULL},
    {VS_R_PROX_ULNAR_V, VS_R_PROX_ULNAR_V_MULTI, PSED, "R Prox Ulnar V", PsEdInfo, NULL},
    {VS_L_MID_SCL_A, VS_L_MID_SCL_A_MULTI, PSED, "L Mid SCL A", PsEdInfo, NULL},
    {VS_R_MID_SCL_A, VS_R_MID_SCL_A_MULTI, PSED, "R Mid SCL A", PsEdInfo, NULL},
    {VS_L_PROX_SCL_A, VS_L_PROX_SCL_A_MULTI, PSED, "L Prox SCL A", PsEdInfo, NULL},
    {VS_R_PROX_SCL_A, VS_R_PROX_SCL_A_MULTI, PSED, "R Prox SCL A", PsEdInfo, NULL},
    {VS_L_MID_SCL_V, VS_L_MID_SCL_V_MULTI, PSED, "L Mid SCL V", PsEdInfo, NULL},
    {VS_R_MID_SCL_V, VS_R_MID_SCL_V_MULTI, PSED, "R Mid SCL V", PsEdInfo, NULL},
    {VS_L_PROX_SCL_V, VS_L_PROX_SCL_V_MULTI, PSED, "L Prox SCL V", PsEdInfo, NULL},
    {VS_R_PROX_SCL_V, VS_R_PROX_SCL_V_MULTI, PSED, "R Prox SCL V", PsEdInfo, NULL},
};

/******************************TCD Measure*************************/
const SingleItemInfo TCDInfo[] = {
    {TCD_L_ACA, SD, N_("L ACA"), TCD_L_ACA_MULTI, NULL},
    {TCD_R_ACA, SD, N_("R ACA"), TCD_R_ACA_MULTI, NULL},
    {TCD_L_VERTEBRAL_A, SD, N_("L Vertebral A"), TCD_L_VERTEBRAL_A_MULTI, NULL},
    {TCD_R_VERTEBRAL_A, SD, N_("R Vertebral A"), TCD_R_VERTEBRAL_A_MULTI, NULL},
    {TCD_L_MCA, SD, N_("L MCA"), TCD_L_MCA_MULTI, NULL},
    {TCD_R_MCA, SD, N_("R MCA"), TCD_R_MCA_MULTI, NULL},
    {TCD_L_PCA, SD, N_("L PCA"), TCD_L_PCA_MULTI, NULL},
    {TCD_R_PCA, SD, N_("R PCA"), TCD_R_PCA_MULTI, NULL},
    {TCD_BASILAR_A, SD, N_("Basilar A"), TCD_BASILAR_A_MULTI, NULL},
};

const MultiItemInfo TCDMultiInfo[] = {
    {TCD_L_ACA, TCD_L_ACA_MULTI, SD, "L ACA", SdInfo, NULL},
    {TCD_R_ACA, TCD_R_ACA_MULTI, SD, "R ACA", SdInfo, NULL},
    {TCD_L_VERTEBRAL_A, TCD_L_VERTEBRAL_A_MULTI, SD, "L Vertebral A", SdInfo, NULL},
    {TCD_R_VERTEBRAL_A, TCD_R_VERTEBRAL_A_MULTI, SD, "R Vertebral A", SdInfo, NULL},
    {TCD_L_MCA, TCD_L_MCA_MULTI, SD, "L MCA", SdInfo, NULL},
    {TCD_R_MCA, TCD_R_MCA_MULTI, SD, "R MCA", SdInfo, NULL},
    {TCD_L_PCA, TCD_L_PCA_MULTI, SD, "L PCA", SdInfo, NULL},
    {TCD_R_PCA, TCD_R_PCA_MULTI, SD, "R PCA", SdInfo, NULL},
    {TCD_BASILAR_A, TCD_BASILAR_A_MULTI, SD, "Basilar A", SdInfo, NULL},
};

/******************************Ortho Measure*************************/
const SingleItemInfo OrthoInfo[] = {
    {ORTHO_LT_HIP, HIP, N_("L Hip"), ORTHO_LT_HIP_MULTI, NULL},
    {ORTHO_RT_HIP, HIP, N_("R Hip"), ORTHO_RT_HIP_MULTI, NULL},
};

const MultiItemInfo OrthoMultiInfo[] = {
    {ORTHO_LT_HIP, ORTHO_LT_HIP_MULTI, HIP, "L Hip", HipInfo, NULL},
    {ORTHO_RT_HIP, ORTHO_RT_HIP_MULTI, HIP, "R Hip", HipInfo, NULL},
};

#ifdef VET
/*
const SingleItemInfo TDInfo[] = {
    {TD_LESION1_AREA, AREA_TRACK,  N_("Lesion 1"),  TD_LESION1_AREA_MULTI,  NULL},
    {TD_LESION2_AREA, AREA_TRACK,  N_("Lesion 2"),  TD_LESION2_AREA_MULTI,  NULL},
    {TD_LESION3_AREA, AREA_TRACK,  N_("Lesion 3"),  TD_LESION3_AREA_MULTI,  NULL},
    {TD_LESION4_AREA, AREA_TRACK,  N_("Lesion 4"),  TD_LESION4_AREA_MULTI,  NULL},
    {TD_LESION5_AREA, AREA_TRACK,  N_("Lesion 5"),  TD_LESION5_AREA_MULTI,  NULL},
    {TD_LESION6_AREA, AREA_TRACK,  N_("Lesion 6"),  TD_LESION6_AREA_MULTI,  NULL},
    {TD_LESION7_AREA, AREA_TRACK,  N_("Lesion 7"),  TD_LESION7_AREA_MULTI,  NULL},
    {TD_LESION8_AREA, AREA_TRACK,  N_("Lesion 8"),  TD_LESION8_AREA_MULTI,  NULL},
    {TD_LESION9_AREA, AREA_TRACK,  N_("Lesion 9"),  TD_LESION9_AREA_MULTI,  NULL},
    {TD_LESION10_AREA, AREA_TRACK, N_("Lesion 10"), TD_LESION10_AREA_MULTI, NULL},
};
*/
const SingleItemInfo TDInfo[] = {
//Lesion1
    {TD_LES1_AREA,  AREA_TRACK, N_("Lesion1 Area"),   CM2, NULL},
    {TD_LES1_AREA1, AREA_TRACK, N_("Lesion1 Area 1"), CM2, NULL},
    {TD_LES1_AREA2, AREA_TRACK, N_("Lesion1 Area 2"), CM2, NULL},
    {TD_LES1_AREA3, AREA_TRACK, N_("Lesion1 Area 3"), CM2, NULL},
//Lesion2
    {TD_LES2_AREA,  AREA_TRACK, N_("Lesion2 Area"),   CM2, NULL},
    {TD_LES2_AREA1, AREA_TRACK, N_("Lesion2 Area 1"), CM2, NULL},
    {TD_LES2_AREA2, AREA_TRACK, N_("Lesion2 Area 2"), CM2, NULL},
    {TD_LES2_AREA3, AREA_TRACK, N_("Lesion2 Area 3"), CM2, NULL},
//Lesion3
    {TD_LES3_AREA,  AREA_TRACK, N_("Lesion3 Area"),   CM2, NULL},
    {TD_LES3_AREA1, AREA_TRACK, N_("Lesion3 Area 1"), CM2, NULL},
    {TD_LES3_AREA2, AREA_TRACK, N_("Lesion3 Area 2"), CM2, NULL},
    {TD_LES3_AREA3, AREA_TRACK, N_("Lesion3 Area 3"), CM2, NULL},
//Lesion4
    {TD_LES4_AREA,  AREA_TRACK, N_("Lesion4 Area"),   CM2, NULL},
    {TD_LES4_AREA1, AREA_TRACK, N_("Lesion4 Area 1"), CM2, NULL},
    {TD_LES4_AREA2, AREA_TRACK, N_("Lesion4 Area 2"), CM2, NULL},
    {TD_LES4_AREA3, AREA_TRACK, N_("Lesion4 Area 3"), CM2, NULL},
//Lesion5
    {TD_LES5_AREA,  AREA_TRACK, N_("Lesion5 Area"),   CM2, NULL},
    {TD_LES5_AREA1, AREA_TRACK, N_("Lesion5 Area 1"), CM2, NULL},
    {TD_LES5_AREA2, AREA_TRACK, N_("Lesion5 Area 2"), CM2, NULL},
    {TD_LES5_AREA3, AREA_TRACK, N_("Lesion5 Area 3"), CM2, NULL},
//Lesion6
    {TD_LES6_AREA,  AREA_TRACK, N_("Lesion6 Area"),   CM2, NULL},
    {TD_LES6_AREA1, AREA_TRACK, N_("Lesion6 Area 1"), CM2, NULL},
    {TD_LES6_AREA2, AREA_TRACK, N_("Lesion6 Area 2"), CM2, NULL},
    {TD_LES6_AREA3, AREA_TRACK, N_("Lesion6 Area 3"), CM2, NULL},
//Lesion7
    {TD_LES7_AREA,  AREA_TRACK, N_("Lesion7 Area"),   CM2, NULL},
    {TD_LES7_AREA1, AREA_TRACK, N_("Lesion7 Area 1"), CM2, NULL},
    {TD_LES7_AREA2, AREA_TRACK, N_("Lesion7 Area 2"), CM2, NULL},
    {TD_LES7_AREA3, AREA_TRACK, N_("Lesion7 Area 3"), CM2, NULL},
//Lesion8
    {TD_LES8_AREA,  AREA_TRACK, N_("Lesion8 Area"),   CM2, NULL},
    {TD_LES8_AREA1, AREA_TRACK, N_("Lesion8 Area 1"), CM2, NULL},
    {TD_LES8_AREA2, AREA_TRACK, N_("Lesion8 Area 2"), CM2, NULL},
    {TD_LES8_AREA3, AREA_TRACK, N_("Lesion8 Area 3"), CM2, NULL},
//Lesion9
    {TD_LES9_AREA,  AREA_TRACK, N_("Lesion9 Area"),   CM2, NULL},
    {TD_LES9_AREA1, AREA_TRACK, N_("Lesion9 Area 1"), CM2, NULL},
    {TD_LES9_AREA2, AREA_TRACK, N_("Lesion9 Area 2"), CM2, NULL},
    {TD_LES9_AREA3, AREA_TRACK, N_("Lesion9 Area 3"), CM2, NULL},
//Lesion10
    {TD_LES10_AREA,  AREA_TRACK, N_("Lesion10 Area"),   CM2, NULL},
    {TD_LES10_AREA1, AREA_TRACK, N_("Lesion10 Area 1"), CM2, NULL},
    {TD_LES10_AREA2, AREA_TRACK, N_("Lesion10 Area 2"), CM2, NULL},
    {TD_LES10_AREA3, AREA_TRACK, N_("Lesion10 Area 3"), CM2, NULL},
//Lesion11
    {TD_LES11_AREA,  AREA_TRACK, N_("Lesion11 Area"),   CM2, NULL},
    {TD_LES11_AREA1, AREA_TRACK, N_("Lesion11 Area 1"), CM2, NULL},
    {TD_LES11_AREA2, AREA_TRACK, N_("Lesion11 Area 2"), CM2, NULL},
    {TD_LES11_AREA3, AREA_TRACK, N_("Lesion11 Area 3"), CM2, NULL},

};
const MultiItemInfo TDMultiInfo[] = {
    /*
    {TD_AREA_RATIO1,  TD_LESION1_AREA_MULTI,  AREA_TRACK, "Lesion 1",  AreaInfo, NULL},
    {TD_AREA_RATIO2,  TD_LESION2_AREA_MULTI,  AREA_TRACK, "Lesion 2",  AreaInfo, NULL},
    {TD_AREA_RATIO3,  TD_LESION3_AREA_MULTI,  AREA_TRACK, "Lesion 3",  AreaInfo, NULL},
    {TD_AREA_RATIO4,  TD_LESION4_AREA_MULTI,  AREA_TRACK, "Lesion 4",  AreaInfo, NULL},
    {TD_AREA_RATIO5,  TD_LESION5_AREA_MULTI,  AREA_TRACK, "Lesion 5",  AreaInfo, NULL},
    {TD_AREA_RATIO6,  TD_LESION6_AREA_MULTI,  AREA_TRACK, "Lesion 6",  AreaInfo, NULL},
    {TD_AREA_RATIO7,  TD_LESION7_AREA_MULTI,  AREA_TRACK, "Lesion 7",  AreaInfo, NULL},
    {TD_AREA_RATIO8,  TD_LESION8_AREA_MULTI,  AREA_TRACK, "Lesion 8",  AreaInfo, NULL},
    {TD_AREA_RATIO9,  TD_LESION9_AREA_MULTI,  AREA_TRACK, "Lesion 9",  AreaInfo, NULL},
    {TD_AREA_RATIO10, TD_LESION10_AREA_MULTI, AREA_TRACK, "Lesion 10", AreaInfo, NULL},
    {TD_AREA_RATIO11, TD_LESION11_AREA_MULTI, AREA_TRACK, "Lesion 11", AreaInfo, NULL},
    */
};
//添加 动物超 产科
//hlx 1.11

const CalcResultInfo AnOBDogGSDGW = {ANOB_DOG_GSD_GW, {ANOB_DOG_GSD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBDogGSDEDCB = {ANOB_DOG_GSD_EDCB, {ANOB_DOG_GSD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBDogCRLGW = {ANOB_DOG_CRL_GW, {ANOB_DOG_CRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBDogCRLEDCB = {ANOB_DOG_CRL_EDCB, {ANOB_DOG_CRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBDogHDGW = {ANOB_DOG_HD_GW, {ANOB_DOG_HD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet,NULL};
const CalcResultInfo AnOBDogHDEDCB = {ANOB_DOG_HD_EDCB, {ANOB_DOG_HD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT,  MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBDogBDGW = {ANOB_DOG_BD_GW, {ANOB_DOG_BD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet,NULL};
const CalcResultInfo AnOBDogBDEDCB = {ANOB_DOG_BD_EDCB, {ANOB_DOG_BD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT,  MeaCalcFun::EDCBCalc, NULL};

//Cat
const CalcResultInfo AnOBCatHDGW = {ANOB_CAT_HD_GW, {ANOB_CAT_HD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet,  NULL};
const CalcResultInfo AnOBCatBDGW = {ANOB_CAT_BD_GW, {ANOB_CAT_BD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet,  NULL};
const CalcResultInfo AnOBCatHDEDCB = {ANOB_CAT_HD_EDCB, {ANOB_CAT_HD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBCatBDEDCB = {ANOB_CAT_BD_EDCB, {ANOB_CAT_BD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT,  MeaCalcFun::EDCBCalc, NULL};
//sheep
const CalcResultInfo AnOBSheepCRLGW = {ANOB_SHEEP_CRL_GW, {ANOB_SHEEP_CRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBSheepCRLEDCB = {ANOB_SHEEP_CRL_EDCB, {ANOB_SHEEP_CRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
//swine
const CalcResultInfo AnOBSwineHLGW = {ANOB_SWINE_HL_GW, {ANOB_SWINE_HL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT,  MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBSwineHLEDCB = {ANOB_SWINE_HL_EDCB, {ANOB_SWINE_HL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc,  NULL};
const CalcResultInfo AnOBSwineSLGW = {ANOB_SWINE_SL_GW, {ANOB_SWINE_SL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT,  MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBSwineSLEDCB = {ANOB_SWINE_SL_EDCB, {ANOB_SWINE_SL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc,  NULL};
//Bovine
const CalcResultInfo AnOBBovineKCRLGW = {ANOB_BOVINE_KCRL_GW, {ANOB_BOVINE_KCRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBBovineKCRLEDCB = {ANOB_BOVINE_KCRL_EDCB, {ANOB_BOVINE_KCRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT,  MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBBovineWCRLGW = {ANOB_BOVINE_WCRL_GW, {ANOB_BOVINE_WCRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet,  NULL};
const CalcResultInfo AnOBBovineWCRLEDCB = {ANOB_BOVINE_WCRL_EDCB, {ANOB_BOVINE_WCRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc,  NULL};
const CalcResultInfo AnOBBovineHCRLGW = {ANOB_BOVINE_HCRL_GW, {ANOB_BOVINE_HCRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet,  NULL};
const CalcResultInfo AnOBBovineHCRLEDCB = {ANOB_BOVINE_HCRL_EDCB, {ANOB_BOVINE_HCRL, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc,  NULL};
const CalcResultInfo AnOBBovineBBDGW = {ANOB_BOVINE_BBD_GW, {ANOB_BOVINE_BBD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBBovineBBDEDCB = {ANOB_BOVINE_BBD_EDCB, {ANOB_BOVINE_BBD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBBovineBTDGW = {ANOB_BOVINE_BTD_GW, {ANOB_BOVINE_BTD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBBovineBTDEDCB = {ANOB_BOVINE_BTD_EDCB, {ANOB_BOVINE_BTD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBBovineBUDGW = {ANOB_BOVINE_BUD_GW, {ANOB_BOVINE_BUD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBBovineBUDEDCB = {ANOB_BOVINE_BUD_EDCB, {ANOB_BOVINE_BUD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc,  NULL};
//Equine
const CalcResultInfo AnOBEquineGSDGW = {ANOB_EQUINE_GSD_GW, {ANOB_EQUINE_GSD,NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBEquineGSDEDCB = {ANOB_EQUINE_GSD_EDCB, {ANOB_EQUINE_GSD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc,  NULL};
const CalcResultInfo AnOBEquineERDGW = {ANOB_EQUINE_ERD_GW, {ANOB_EQUINE_ERD,NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBEquineERDEDCB = {ANOB_EQUINE_ERD_EDCB, {ANOB_EQUINE_ERD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBEquineESDGW = {ANOB_EQUINE_ESD_GW, {ANOB_EQUINE_ESD,NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBEquineESDEDCB = {ANOB_EQUINE_ESD_EDCB, {ANOB_EQUINE_ESD, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};
const CalcResultInfo AnOBEquineEEDGW = {ANOB_EQUINE_EED_GW, {ANOB_EQUINE_EED,NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "GW", NOUNIT, MeaCalcFun::CalcGWManVet, NULL};
const CalcResultInfo AnOBEquineEEDEDCB = {ANOB_EQUINE_EED_EDCB, {ANOB_EQUINE_EED, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, "EDD", NOUNIT, MeaCalcFun::EDCBCalc, NULL};

const CalcInfoArray AnOBDogGSDCalc = {&AnOBDogGSDGW,  &AnOBDogGSDEDCB, NULL};
const CalcInfoArray AnOBDogCRLCalc = {&AnOBDogCRLGW,  &AnOBDogCRLEDCB, NULL};
const CalcInfoArray AnOBDogHDCalc  = {&AnOBDogHDGW,   &AnOBDogHDEDCB,  NULL};
const CalcInfoArray AnOBDogBDCalc  = {&AnOBDogBDGW,   &AnOBDogBDEDCB,  NULL};
const CalcInfoArray AnOBCatHDCalc  = {&AnOBCatHDGW,   &AnOBCatHDEDCB,  NULL};
const CalcInfoArray AnOBCatBDCalc  = {&AnOBCatBDGW,   &AnOBCatBDEDCB,  NULL};
const CalcInfoArray AnOBSheepCRLCalc  = {&AnOBSheepCRLGW,  &AnOBSheepCRLEDCB, NULL};
const CalcInfoArray AnOBSwineHLCalc = {&AnOBSwineHLGW, &AnOBSwineHLEDCB, NULL};
const CalcInfoArray AnOBSwineSLCalc = {&AnOBSwineSLGW, &AnOBSwineSLEDCB, NULL};
const CalcInfoArray AnOBBovineKCRLCalc = {&AnOBBovineKCRLGW, &AnOBBovineKCRLEDCB, NULL};
const CalcInfoArray AnOBBovineWCRLCalc = {&AnOBBovineWCRLGW, &AnOBBovineWCRLEDCB, NULL};
const CalcInfoArray AnOBBovineHCRLCalc = {&AnOBBovineHCRLGW, &AnOBBovineHCRLEDCB, NULL};
const CalcInfoArray AnOBBovineBBDCalc  = {&AnOBBovineBBDGW, &AnOBBovineBBDEDCB, NULL};
const CalcInfoArray AnOBBovineBTDCalc  = {&AnOBBovineBTDGW, &AnOBBovineBTDEDCB, NULL};
const CalcInfoArray AnOBBovineBUDCalc  = {&AnOBBovineBUDGW, &AnOBBovineBUDEDCB, NULL};
const CalcInfoArray AnOBEquineGSDCalc  = {&AnOBEquineGSDGW, &AnOBEquineGSDEDCB, NULL};
const CalcInfoArray AnOBEquineERDCalc  = {&AnOBEquineERDGW, &AnOBEquineERDEDCB, NULL};
const CalcInfoArray AnOBEquineESDCalc  = {&AnOBEquineESDGW, &AnOBEquineESDEDCB, NULL};
const CalcInfoArray AnOBEquineEEDCalc  = {&AnOBEquineEEDGW, &AnOBEquineEEDEDCB, NULL};

//hlx 12.28
const PtrOfCalcInfo AnOBCalcInfoPtr[ANOB_CALC_END - ANOB_CALC_START] = {
    {ANOB_DOG_GSD_GW, &AnOBDogGSDGW},
    {ANOB_DOG_GSD_EDCB, &AnOBDogGSDEDCB},
    {ANOB_DOG_CRL_GW, &AnOBDogCRLGW},
    {ANOB_DOG_CRL_EDCB, &AnOBDogCRLEDCB},
    {ANOB_DOG_HD_GW, &AnOBDogHDGW},
    {ANOB_DOG_HD_EDCB, &AnOBDogHDEDCB},
    {ANOB_DOG_BD_GW, &AnOBDogBDGW},
    {ANOB_DOG_BD_EDCB, &AnOBDogBDEDCB},

    {ANOB_CAT_HD_GW, &AnOBCatHDGW},
    {ANOB_CAT_HD_EDCB, &AnOBCatHDEDCB},
    {ANOB_CAT_BD_GW, &AnOBCatBDGW},
    {ANOB_CAT_BD_EDCB, &AnOBCatBDEDCB},

    {ANOB_SHEEP_CRL_GW, &AnOBSheepCRLGW},
    {ANOB_SHEEP_CRL_EDCB, &AnOBSheepCRLEDCB},

    {ANOB_SWINE_SL_GW, &AnOBSwineSLGW},
    {ANOB_SWINE_SL_EDCB, &AnOBSwineSLEDCB},
    {ANOB_SWINE_HL_GW, &AnOBSwineHLGW},
    {ANOB_SWINE_HL_EDCB, &AnOBSwineHLEDCB},

    {ANOB_BOVINE_KCRL_GW, &AnOBBovineKCRLGW},
    {ANOB_BOVINE_KCRL_EDCB, &AnOBBovineKCRLEDCB},
    {ANOB_BOVINE_WCRL_GW, &AnOBBovineWCRLGW},
    {ANOB_BOVINE_WCRL_EDCB, &AnOBBovineWCRLEDCB},
    {ANOB_BOVINE_HCRL_GW, &AnOBBovineHCRLGW},
    {ANOB_BOVINE_HCRL_EDCB, &AnOBBovineHCRLEDCB},
    {ANOB_BOVINE_BBD_GW, &AnOBBovineBBDGW},
    {ANOB_BOVINE_BBD_EDCB, &AnOBBovineBBDEDCB},
    {ANOB_BOVINE_BTD_GW, &AnOBBovineBTDGW},
    {ANOB_BOVINE_BTD_EDCB, &AnOBBovineBTDEDCB},
    {ANOB_BOVINE_BUD_GW, &AnOBBovineBUDGW},
    {ANOB_BOVINE_BUD_EDCB, &AnOBBovineBUDEDCB},

    {ANOB_EQUINE_GSD_GW, &AnOBEquineGSDGW},
    {ANOB_EQUINE_GSD_EDCB, &AnOBEquineGSDEDCB},
    {ANOB_EQUINE_ERD_GW, &AnOBEquineERDGW},
    {ANOB_EQUINE_ERD_EDCB, &AnOBEquineERDEDCB},
    {ANOB_EQUINE_ESD_GW, &AnOBEquineESDGW},
    {ANOB_EQUINE_ESD_EDCB, &AnOBEquineESDEDCB},
    {ANOB_EQUINE_EED_GW, &AnOBEquineEEDGW},
    {ANOB_EQUINE_EED_EDCB, &AnOBEquineEEDEDCB},
};

//添加 动物超 产科
//hlx 12.23
const SingleItemInfo AnOBInfo[] = {
    //dog
    {ANOB_DOG_GSD, DIST_DOT, N_("Dog GSD"), CM, &AnOBDogGSDCalc},
    {ANOB_DOG_CRL, DIST_DOT, N_("Dog CRL"), CM, &AnOBDogCRLCalc},
    {ANOB_DOG_HD,  DIST_DOT, N_("Dog HD"),  CM, &AnOBDogHDCalc},
    {ANOB_DOG_BD,  DIST_DOT, N_("Dog BD"),  CM, &AnOBDogBDCalc},
    //cat
    {ANOB_CAT_HD,  DIST_DOT, N_("Cat HD"),  CM, &AnOBCatHDCalc},
    {ANOB_CAT_BD,  DIST_DOT, N_("Cat BD"),  CM, &AnOBCatBDCalc},
    //sheep
    {ANOB_SHEEP_CRL, DIST_DOT, N_("Sheep CRL"), CM, &AnOBSheepCRLCalc},
    //swine
    {ANOB_SWINE_HL,  DIST_DOT, N_("Swine HL"),  CM, &AnOBSwineHLCalc},
    {ANOB_SWINE_SL,  DIST_DOT, N_("Swine SL"),  CM, &AnOBSwineSLCalc},
    //bovine
    {ANOB_BOVINE_KCRL, DIST_DOT, N_("Bovine K-CRL"), CM, &AnOBBovineKCRLCalc},
    {ANOB_BOVINE_WCRL, DIST_DOT, N_("Bovine W-CRL"), CM, &AnOBBovineWCRLCalc},
    {ANOB_BOVINE_HCRL, DIST_DOT, N_("Bovine H-CRL"), CM, &AnOBBovineHCRLCalc},
    {ANOB_BOVINE_BBD,  DIST_DOT, N_("Bovine BBD"),   CM, &AnOBBovineBBDCalc},
    {ANOB_BOVINE_BTD,  DIST_DOT, N_("Bovine BTD"),   CM, &AnOBBovineBTDCalc},
    {ANOB_BOVINE_BUD,  DIST_DOT, N_("Bovine BUD"),   CM, &AnOBBovineBUDCalc},
    //equine
    {ANOB_EQUINE_GSD,  DIST_DOT, N_("Equine GSD"),   CM, &AnOBEquineGSDCalc},
    {ANOB_EQUINE_ERD,  DIST_DOT, N_("Equine ERD"),   CM, &AnOBEquineERDCalc},
    {ANOB_EQUINE_ESD,  DIST_DOT, N_("Equine ESD"),   CM, &AnOBEquineESDCalc},
    {ANOB_EQUINE_EED,  DIST_DOT, N_("Equine EED"),   CM, &AnOBEquineEEDCalc}
};

const MultiItemInfo AnOBMultiInfo[] = {
};
#endif
#if 1
/******************************有多种计算方法的计算项**************************************/
//const CalcMultiMethod MethodCalc[] ={
//  {ADULT_EDV, {ADULT_EDV_2DCUBED, ADULT_EDV_2DTEICH, ADULT_EDV_MMCUBED, ADULT_EDV_MMTEICH, NO_PAR, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, {&AdultEDV2DCubed, &AdultEDV2DTeich, &AdultEDVMMCubed, &AdultEDVMMTeich, NULL, NULL, NULL, NULL, NULL}},
//  {ADULT_ESV, {ADULT_ESV_2DCUBED, ADULT_ESV_2DTEICH, ADULT_ESV_MMCUBED, ADULT_ESV_MMTEICH, NO_PAR, NO_PAR, NO_PAR, NO_PAR, NO_PAR}, {&AdultESV2DCubed, &AdultESV2DTeich, &AdultESVMMCubed, &AdultESVMMTeich, NULL, NULL, NULL, NULL, NULL}},
//  {OB_EFW, {OB_EFW_HADLOCK1, OB_EFW_HADLOCK2, OB_EFW_HADLOCK3, OB_EFW_HADLOCK4, OB_EFW_SHEPARD, OB_EFW_HANSMAN, OB_EFW_TOKYO, NO_PAR, NO_PAR}, {&OBEfwHadlock1, &OBEfwHadlock2, &OBEfwHadlock3, &OBEfwHadlock4, &OBEfwShepard, &OBEfwHansmenn, &OBEfwTokyo, NULL, NULL}},
//};
#endif

#endif