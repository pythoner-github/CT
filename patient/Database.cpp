#include "Database.h"
#include "Def.h"
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "../display/gui_func.h"
#include "gui_global.h"
#include "../sysMan/SysGeneralSetting.h"
#include "ViewSystem.h"

using std::ostringstream;
using std::string;

sqlite3 * Database::m_db = 0;

Database::Database(void)
{
    SysGeneralSetting *sgs = new SysGeneralSetting;
    if ((sgs->GetLanguage() == ZH) || (sgs->GetLanguage() == FR))
        m_langCN = true;
    else
        m_langCN = false;

    m_dateFormat = sgs->GetDateFormat();
    delete sgs;
}

Database::~Database(void)
{
}

bool Database::CreateTable(void)
{
    if (access(PATIENT_DB, F_OK)) {
	sqlite3 *db = 0;
	char *errmsg;

	if (sqlite3_open(PATIENT_DB, &db) != SQLITE_OK)
	    return false;
#ifdef VET
        const char *sql_pat_info = "CREATE TABLE patient_info(\
                                    ID VARCHAR(50) NOT NULL,\
                                    AnimalName VARCHAR(20) NOT NULL DEFAULT '',\
                                    OwnerName VARCHAR(20) NOT NULL DEFAULT '',\
                                    Species VARCHAR(5) NOT NULL DEFAULT '',\
                                    Sex VARCHAR(10) NOT NULL DEFAULT '',\
                                    BirthYear VARCHAR(10) NOT NULL DEFAULT '',\
                                    BirthMonth VARCHAR(5) NOT NULL DEFAULT '',\
                                    BirthDay VARCHAR(5) NOT NULL DEFAULT '',\
                                    Age VARCHAR(5) NOT NULL DEFAULT '',\
                                    AgeUnit VARCHAR(5) NOT NULL DEFAULT '',\
                                    Telephone VARCHAR(20) NOT NULL DEFAULT '',\
                                    Address VARCHAR(100) NOT NULL DEFAULT '',\
                                    Comment VARCHAR(2000) NOT NULL DEFAULT '',\
                                    PRIMARY KEY (ID)\
                                    )";
#else


	const char *sql_pat_info = "CREATE TABLE patient_info(\
               ID VARCHAR(50) NOT NULL,\
               LastName VARCHAR(20) NOT NULL DEFAULT '',\
               FirstName VARCHAR(20) NOT NULL DEFAULT '',\
               MidName VARCHAR(20) NOT NULL DEFAULT '',\
               Sex VARCHAR(10) NOT NULL DEFAULT '',\
               BirthYear VARCHAR(10) NOT NULL DEFAULT '',\
               BirthMonth VARCHAR(5) NOT NULL DEFAULT '',\
               BirthDay VARCHAR(5) NOT NULL DEFAULT '',\
               Age VARCHAR(5) NOT NULL DEFAULT '',\
               AgeUnit VARCHAR(5) NOT NULL DEFAULT '',\
               Telephone VARCHAR(20) NOT NULL DEFAULT '',\
               Address VARCHAR(100) NOT NULL DEFAULT '',\
               Comment VARCHAR(2000) NOT NULL DEFAULT '',\
               PRIMARY KEY (ID)\
               )";
#endif
	if (sqlite3_exec(db, sql_pat_info, 0, 0, &errmsg) != SQLITE_OK) {
	    PRINTF("Create Table Error: %s\n", sqlite3_errmsg(db));
	    sqlite3_free(errmsg);
	    return false;
	}
	const char *sql_exam_info = "CREATE TABLE exam_info(\
               PatID VARCHAR(50) NOT NULL,\
               ExamID INTEGER NOT NULL,\
               ExamType VARCHAR(20) NOT NULL DEFAULT '',\
               ExamYear VARCHAR(10) NOT NULL DEFAULT '',\
               ExamMonth VARCHAR(5) NOT NULL DEFAULT '',\
               ExamDay VARCHAR(5) NOT NULL DEFAULT '',\
               ExamHour VARCHAR(5) NOT NULL DEFAULT '',\
               ExamMinute VARCHAR(5) NOT NULL DEFAULT '',\
               ExamSecond VARCHAR(5) NOT NULL DEFAULT '',\
               Height VARCHAR(10) NOT NULL DEFAULT '',\
               Weight VARCHAR(10) NOT NULL DEFAULT '',\
               BSA VARCHAR(10) NOT NULL DEFAULT '',\
               LMPYear VARCHAR(10) NOT NULL DEFAULT '',\
               LMPMonth VARCHAR(5) NOT NULL DEFAULT '',\
               LMPDay VARCHAR(5) NOT NULL DEFAULT '',\
               OvulateYear VARCHAR(10) NOT NULL DEFAULT '',\
               OvulateMonth VARCHAR(5) NOT NULL DEFAULT '',\
               OvulateDay VARCHAR(5) NOT NULL DEFAULT '',\
               PregCount VARCHAR(5) NOT NULL DEFAULT '',\
               AbnormalPregCount VARCHAR(5) NOT NULL DEFAULT '',\
               FetusCount VARCHAR(5) NOT NULL DEFAULT '',\
               DeliveryCount VARCHAR(5) NOT NULL DEFAULT '',\
               MiscarryCount VARCHAR(5) NOT NULL DEFAULT '',\
               HR VARCHAR(10) NOT NULL DEFAULT '',\
               PressureHigh VARCHAR(10) NOT NULL DEFAULT '',\
               PressureLow VARCHAR(10) NOT NULL DEFAULT '',\
               PSA VARCHAR(10) NOT NULL DEFAULT '',\
               ExamDr VARCHAR(50) NOT NULL DEFAULT '',\
               ReportDr VARCHAR(50) NOT NULL DEFAULT '',\
               ExamComment VARCHAR(2000) NOT NULL DEFAULT '',\
               imagePath VARCHAR(50) NOT NULL DEFAULT '',\
               PRIMARY KEY (ExamID),\
               FOREIGN KEY (PatID) REFERENCES patient_info(ID)\
               )";
	if (sqlite3_exec(db, sql_exam_info, 0, 0, &errmsg) != SQLITE_OK) {
	    PRINTF("Create Table Error: %s\n", sqlite3_errmsg(db));
	    sqlite3_free(errmsg);
	    return false;
	}

	const char *sql_doctor_info = "CREATE TABLE doctor_info(\
               ID INTEGER NOT NULL,\
               Name VARCHAR(50) NOT NULL DEFAULT '',\
               PRIMARY KEY (ID)\
               )";
	if (sqlite3_exec(db, sql_doctor_info, 0, 0, &errmsg) != SQLITE_OK) {
	    PRINTF("Create Table Error: %s\n", sqlite3_errmsg(db));
	    sqlite3_free(errmsg);
	    return false;
	}
	sqlite3_close(db);
    }
    return true;
}

bool Database::OpenDB(void)
{
    if (sqlite3_open(PATIENT_DB, &m_db) != SQLITE_OK) {
	PRINTF("Database Open Error!\n");
	return false;
    }
    return true;
}

bool Database::CloseDB(void)
{
    if (sqlite3_close(m_db) != SQLITE_OK) {
	PRINTF("Database Close Error:%s\n", sqlite3_errmsg(m_db));
	return false;
    }
    m_db = 0;
    return true;
}

bool Database::ArchivePat(PatientInfo::Info &info, string &errorMsg)
{
    char *errmsg;
    ostringstream streamPat;
    ostringstream streamExam;
    ostringstream streamDoc;
    string sql;

    if (info.p.id.empty()) {
#ifdef VET
        errorMsg = _("No Animal ID.");
#else
        errorMsg = _("No Patient ID.");
#endif
	//errorMsg = _("No Patient ID.");
	return FALSE;
    }

    if (sqlite3_exec(m_db, "begin transaction", 0, 0, &errmsg) != SQLITE_OK) {
    	PRINTF("Archive Error:%s\n", sqlite3_errmsg(m_db));
	errorMsg = sqlite3_errmsg(m_db);
	sqlite3_free(errmsg);
	return FALSE;
    }

    string year, month, day;
    ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, month, day);

    if (GetPatIDExist(info.p.id.c_str()).empty()) {
#ifdef VET
        streamPat << "INSERT INTO patient_info VALUES('"
            << info.p.id << "', '"
            << info.p.animal_name << "', '"
            << info.p.owner_name << "', '"
            << info.p.species << "', '"
            << info.p.sex << "', '"
            << year <<"', '"
            << month << "', '"
            << day << "', '"
            << info.p.age << "', '"
            << info.p.ageUnit << "', '"
            << info.p.telephone << "', '"
            << info.p.address << "', '"
            << info.p.comment << "');";
#else
	streamPat << "INSERT INTO patient_info VALUES('"
		  << info.p.id << "', '"
		  << info.p.name.last << "', '"
		  << info.p.name.first << "', '"
		  << info.p.name.mid << "', '"
		  << info.p.sex << "', '"
		  << year <<"', '"
		  << month << "', '"
		  << day << "', '"
          << info.p.age << "', '"
          << info.p.ageUnit << "', '"
		  << info.p.telephone << "', '"
		  << info.p.address << "', '"
		  << info.p.comment << "');";
   #endif
 }
 else
 {
#ifdef VET
        streamPat << "UPDATE patient_info SET "
            << "AnimalName = '" << info.p.animal_name << "', "
            << "OwnerName = '" << info.p.owner_name << "', "
            << "Species = '" << info.p.species << "', "
            << "Sex = '" << info.p.sex << "', "
            << "BirthYear = '" << year <<"', "
            << "BirthMonth = '" << month << "', "
            << "BirthDay = '" << day << "', "
            << "Age = '" << info.p.age << "', "
            << "AgeUnit = '" << info.p.ageUnit << "', "
            << "Telephone = '" << info.p.telephone << "', "
            << "Address = '" << info.p.address << "', "
            << "Comment = '" << info.p.comment << "'"
            << "WHERE ID = '" << info.p.id << "';";
#else
	streamPat << "UPDATE patient_info SET "
		  << "LastName = '" << info.p.name.last << "', "
		  << "FirstName = '" << info.p.name.first << "', "
		  << "MidName = '" << info.p.name.mid << "', "
		  << "Sex = '" << info.p.sex << "', "
		  << "BirthYear = '" << year <<"', "
		  << "BirthMonth = '" << month << "', "
		  << "BirthDay = '" << day << "', "
          << "Age = '" << info.p.age << "', "
          << "AgeUnit = '" << info.p.ageUnit << "', "
		  << "Telephone = '" << info.p.telephone << "', "
		  << "Address = '" << info.p.address << "', "
		  << "Comment = '" << info.p.comment << "'"
		  << "WHERE ID = '" << info.p.id << "';";
#endif
 }

 sql = streamPat.str();
 PRINTF("sql: %s\n", sql.c_str());

 if (sqlite3_exec(m_db, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
     PRINTF("Archive Insert or Update Error:%s\n", sqlite3_errmsg(m_db));
        errorMsg = sqlite3_errmsg(m_db);
    	sqlite3_free(errmsg);
        sqlite3_exec(m_db, "rollback transaction", 0, 0, 0);
    	return FALSE;
    }

    ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, month, day);
    string line=info.e.comment;
    streamExam << "INSERT INTO exam_info VALUES('"
	       << info.p.id << "', "
	       << "NULL, '"
	       << info.e.examType << "', '"
	       << year << "', '"
	       << month << "', '"
	       << day << "', '"
	       << info.e.examTime.hour << "', '"
	       << info.e.examTime.minute << "', '"
	       << info.e.examTime.second << "', '"
	       << info.e.height << "', '"
	       << info.e.weight << "', '"
	       << info.e.BSA << "', '"
	       << info.ob.LMPDate.year << "', '"
	       << info.ob.LMPDate.month << "', '"
	       << info.ob.LMPDate.day << "', '"
	       << info.ob.OvulateDate.year << "', '"
	       << info.ob.OvulateDate.month << "', '"
	       << info.ob.OvulateDate.day << "', '"
	       << info.ob.pregCount << "', '"
	       << info.ob.abnormalPregCount << "', '"
	       << info.ob.fetusCount << "', '"
	       << info.ob.deliveryCount << "', '"
	       << info.ob.miscarryCount << "', '"
	       << info.car.HR << "', '"
	       << info.car.pressureHigh << "', '"
	       << info.car.pressureLow << "', '"
	       << info.uro.PSA << "', '"
	       << info.e.examDoctor << "', '"
	       << info.e.reportDoctor << "', '"
	      // << info.e.comment << "', '"
	       << line<< "', '"
	       << info.e.imagePath << "');";
    sql = streamExam.str();
    PRINTF("sql: %s\n", sql.c_str());

    if (sqlite3_exec(m_db, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
    	PRINTF("Archive Insert Error:%s\n", sqlite3_errmsg(m_db));
        errorMsg = sqlite3_errmsg(m_db);
    	sqlite3_free(errmsg);
        sqlite3_exec(m_db, "rollback transaction", 0, 0, 0);
    	return FALSE;
    }

    if (!GetDoctorExist(info.e.examDoctor.c_str())) {
        streamDoc << "INSERT INTO doctor_info VALUES("
                   << "NULL, '"
                   << info.e.examDoctor << "');";
        sql = streamDoc.str();
        PRINTF("sql: %s\n", sql.c_str());

        if (sqlite3_exec(m_db, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
            PRINTF("Archive Insert Error:%s\n", sqlite3_errmsg(m_db));
            errorMsg = sqlite3_errmsg(m_db);
            sqlite3_free(errmsg);
            sqlite3_exec(m_db, "rollback transaction", 0, 0, 0);
            return FALSE;
        }
    }
    if ((info.e.reportDoctor != info.e.examDoctor ) && (!GetDoctorExist(info.e.reportDoctor.c_str())))
    {
        streamDoc.str("");
        streamDoc << "INSERT INTO doctor_info VALUES("
                   << "NULL, '"
                   << info.e.reportDoctor << "');";
        sql = streamDoc.str();
        PRINTF("sql: %s\n", sql.c_str());

        if (sqlite3_exec(m_db, sql.c_str(), 0, 0, &errmsg) != SQLITE_OK) {
            PRINTF("Archive Insert Error:%s\n", sqlite3_errmsg(m_db));
            errorMsg = sqlite3_errmsg(m_db);
            sqlite3_free(errmsg);
            sqlite3_exec(m_db, "rollback transaction", 0, 0, 0);
            return FALSE;
        }
    }

    sqlite3_exec(m_db, "commit transaction", 0, 0, 0);

    ostringstream streamExamID;
    sqlite3_int64 rowid = sqlite3_last_insert_rowid(m_db);
    streamExamID << rowid;
    info.e.examNum = streamExamID.str();

    return TRUE;
}


// Sqlite3 basic invocation sequence:
// 1, sqlite3_prepare  编译sql语句
// 2, sqlite3_bind_*   使用sql参数时，用于将sql参数绑定到sql语句
// 3, sqlite3_step     用来执行编译后的sql语句
// 4, sqlite3_column_* 用于从查询的结果中获取数据
// 5, sqlite3_finalize 用来释放sqlite3_stmt对象

/*
void Database::Testdb(void)
{
    sqlite3_stmt *stmt=NULL;
    string sql = "SELECT ID, LastName, FirstName, MidName FROM patient_info ORDER BY ID";

    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	return ;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	int col_count = sqlite3_column_count(stmt);
	for (int i = 0; i < col_count; i++) {
	    PRINTF("%s  ", sqlite3_column_name(stmt, i));
	    PRINTF("%s\n", sqlite3_column_text(stmt, i));
	}
    }
    sqlite3_finalize(stmt);
}

void Database::TestBinddb(char *id)
{
    sqlite3_stmt *stmt=NULL;
    string sql = "SELECT ID, LastName, FirstName, MidName FROM patient_info WHERE ID=?";
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	return ;
    }
    if (sqlite3_bind_text(stmt, 1, id, strlen(id), SQLITE_TRANSIENT) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	sqlite3_finalize(stmt);
	return ;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	int col_count = sqlite3_column_count(stmt);
	for (int i = 0; i < col_count; i++) {
	    PRINTF("%s\n", sqlite3_column_text(stmt, i));
	}
    }
    sqlite3_finalize(stmt);
}
*/

// DESCRIPTION: 通过传入的检查号examID来填充PatientInfo::Info结构体
// RETURN: 空
void Database::GetPatExamInfo(const char *examID, PatientInfo::Info &info)
{
    sqlite3_stmt *stmt=NULL;
#ifdef VET
    string sql = "SELECT ID, AnimalName, OwnerName, Species, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Height, Weight, BSA, LMPYear, LMPMonth, LMPDay, OvulateYear, OvulateMonth, OvulateDay, PregCount, abnormalPregCount, FetusCount, DeliveryCount, MiscarryCount, HR, PressureHigh, PressureLow, PSA, ExamDr, ReportDr, ExamComment FROM patient_info, exam_info WHERE patient_info.ID = exam_info.PatID AND exam_info.ExamID = ?";
#else
    string sql = "SELECT ID, LastName, FirstName, MidName, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Height, Weight, BSA, LMPYear, LMPMonth, LMPDay, OvulateYear, OvulateMonth, OvulateDay, PregCount, abnormalPregCount, FetusCount, DeliveryCount, MiscarryCount, HR, PressureHigh, PressureLow, PSA, ExamDr, ReportDr, ExamComment FROM patient_info, exam_info WHERE patient_info.ID = exam_info.PatID AND exam_info.ExamID = ?";
#endif
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	return ;
    }
    if (sqlite3_bind_text(stmt, 1, examID, strlen(examID), SQLITE_TRANSIENT) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	sqlite3_finalize(stmt);
	return ;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int col = 0;
	if (strcmp(sqlite3_column_name(stmt, col), "ID") == 0)
	    info.p.id.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            info.p.animal_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            info.p.owner_name.assign((const char *)sqlite3_column_text(stmt, col));
           ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0)
            info.p.species = sqlite3_column_int(stmt, col);
            ++col;
#else
	if (strcmp(sqlite3_column_name(stmt, col), "LastName") == 0)
	    info.p.name.last.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "FirstName") == 0)
	    info.p.name.first.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "MidName") == 0)
	    info.p.name.mid.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#endif

	if (strcmp(sqlite3_column_name(stmt, col), "Sex") == 0)
	    info.p.sex = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthYear") == 0)
	    info.p.birthDate.year = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthMonth") == 0)
	    info.p.birthDate.month = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthDay") == 0)
	    info.p.birthDate.day = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
	    info.p.age = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
	    info.p.ageUnit = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Telephone") == 0)
	    info.p.telephone.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Address") == 0)
	    info.p.address.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Height") == 0)
	    info.e.height = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Weight") == 0)
	    info.e.weight = sqlite3_column_double(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BSA") == 0)
	    info.e.BSA = sqlite3_column_double(stmt, col);
        ++col;
// ob
	if (strcmp(sqlite3_column_name(stmt, col), "LMPYear") == 0)
	    info.ob.LMPDate.year = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "LMPMonth") == 0)
	    info.ob.LMPDate.month = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "LMPDay") == 0)
	    info.ob.LMPDate.day = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "OvulateYear") == 0)
	    info.ob.OvulateDate.year = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "OvulateMonth") == 0)
	    info.ob.OvulateDate.month = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "OvulateDay") == 0)
	    info.ob.OvulateDate.day = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "PregCount") == 0)
	    info.ob.pregCount = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "AbnormalPregCount") == 0)
	    info.ob.abnormalPregCount = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "FetusCount") == 0)
	    info.ob.fetusCount = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "DeliveryCount") == 0)
	    info.ob.deliveryCount = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "MiscarryCount") == 0)
	    info.ob.miscarryCount = sqlite3_column_int(stmt, col);
        ++col;
// car
	if (strcmp(sqlite3_column_name(stmt, col), "HR") == 0)
	    info.car.HR = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "PressureHigh") == 0)
	    info.car.pressureHigh = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "PressureLow") == 0)
	    info.car.pressureLow = sqlite3_column_int(stmt, col);
        ++col;
// uro
	if (strcmp(sqlite3_column_name(stmt, col), "PSA") == 0)
	    info.uro.PSA = sqlite3_column_int(stmt, col);
        ++col;

	if (strcmp(sqlite3_column_name(stmt, col), "ExamDr") == 0)
	    info.e.examDoctor.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;

	if (strcmp(sqlite3_column_name(stmt, col), "ReportDr") == 0)
	    info.e.reportDoctor.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "ExamComment") == 0)
	    info.e.comment.assign((const char *)sqlite3_column_text(stmt, col));
    }
    sqlite3_finalize(stmt);
}

// DESCRIPTION: 通过传入的检查号examID来填充PatientInfo::Info结构体
// RETURN: 空
void Database::GetPatInfo(const char *patID, PatientInfo::Patient &pat_info)
{
    sqlite3_stmt *stmt=NULL;
   // string sql = "SELECT ID, LastName, FirstName, MidName, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Comment FROM patient_info WHERE ID = ?";
#ifdef VET
    string sql = "SELECT ID, AnimalName, OwnerName, Species, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Comment FROM patient_info WHERE ID = ?";
#else
    string sql = "SELECT ID, LastName, FirstName, MidName, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Comment FROM patient_info WHERE ID = ?";
#endif
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	return ;
    }
    if (patID == NULL)
        return;
    if (sqlite3_bind_text(stmt, 1, patID, strlen(patID), SQLITE_TRANSIENT) != SQLITE_OK) {
        PRINTF("%s\n", sqlite3_errmsg(m_db));
        sqlite3_finalize(stmt);
        return ;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int col = 0;
	if (strcmp(sqlite3_column_name(stmt, col), "ID") == 0)
            pat_info.id.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            pat_info.animal_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            pat_info.owner_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0)
            pat_info.species = sqlite3_column_int(stmt, col);
        ++col;
#else
        if (strcmp(sqlite3_column_name(stmt, col), "LastName") == 0)
            pat_info.name.last.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "FirstName") == 0)
            pat_info.name.first.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "MidName") == 0)
            pat_info.name.mid.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#endif


	if(strcmp(sqlite3_column_name(stmt, col), "Sex") == 0)
	    pat_info.sex = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthYear") == 0)
	    pat_info.birthDate.year = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthMonth") == 0)
	    pat_info.birthDate.month = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthDay") == 0)
	    pat_info.birthDate.day = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
	    pat_info.age = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
	    pat_info.ageUnit = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Telephone") == 0)
	    pat_info.telephone.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Address") == 0)
	    pat_info.address.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "Comment") == 0)
	    pat_info.comment.assign((const char *)sqlite3_column_text(stmt, col));
    }
    sqlite3_finalize(stmt);
}

// DESCRIPTION: 查找数据库中ID号是否存在
// RETURN: 存在返回查找的string类型的ID，不存在返回空string
string Database::GetPatIDExist(const char *id)
{
    sqlite3_stmt *stmt=NULL;
    string exist_id;
    if (id == NULL)
        return false;
    string sql = "SELECT count(*) AS Count_ID, ID FROM patient_info WHERE ID=?";
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("-******%s\n", sqlite3_errmsg(m_db));
	return false;
    }
    if (sqlite3_bind_text(stmt, 1, id, strlen(id), SQLITE_TRANSIENT) != SQLITE_OK) {
	PRINTF("---------%s\n", sqlite3_errmsg(m_db));
	sqlite3_finalize(stmt);
	return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (sqlite3_column_int(stmt, 0))
	    exist_id = (const char *)sqlite3_column_text(stmt, 1);
    }
    sqlite3_finalize(stmt);
    return exist_id;
}

// DESCRIPTION: 查找数据库中是否存在医生姓名doc_name
// RETURN: 存在返回true
bool Database::GetDoctorExist(const char *doc_name)
{
    sqlite3_stmt *stmt=NULL;
    int doc_count = 0;

    if (strlen(doc_name) == 0)
        return false;

    string sql = "SELECT count(*) AS Count_doc FROM doctor_info WHERE Name=?";
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	return false;
    }
    if (sqlite3_bind_text(stmt, 1, doc_name, strlen(doc_name), SQLITE_TRANSIENT) != SQLITE_OK) {
	PRINTF("%s\n", sqlite3_errmsg(m_db));
	sqlite3_finalize(stmt);
	return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (sqlite3_column_int(stmt, 0))
	    doc_count = atoi((const char*)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);

    if (doc_count)
        return true;
    else
        return false;
}

// 国际化姓名和日期格式的Sql语句
void Database::GetFormatForSQL(string &name, string &birthdate, string &examdate)
{
    if (m_langCN)
        name = "LastName || FirstName AS Name";
    else
        name = "FirstName || MidName || LastName AS Name";
        //name = "FirstName || ' ' || MidName || ' ' || LastName AS Name"; ///为了解决档案管理下导出/入U盘文件名称bug lhm

    switch (m_dateFormat) {
    case 0:
        birthdate = "BirthYear || '-' || BirthMonth || '-' || BirthDay AS BirthDate";
        examdate = "ExamYear || '-' || ExamMonth || '-' || ExamDay AS ExamDate";
        break;
    case 1:
        birthdate = "BirthMonth || '-' || BirthDay || '-' || BirthYear AS BirthDate";
        examdate = " ExamMonth || '-' || ExamDay || '-' || ExamYear AS ExamDate";
        break;
    case 2:
        birthdate = "BirthDay || '-' || BirthMonth || '-' || BirthYear AS BirthDate";
        examdate = "ExamDay || '-' || ExamMonth || '-' || ExamYear AS ExamDate";
        break;
    }
}

bool Database::NewPatSearch(NewPatSearchTerm& term, vector<NewPatSearchResult>& result)
{
    sqlite3_stmt *stmt=NULL;
    ostringstream streamTerm;
    string sql;

    string sql_name, sql_birthdate, sql_examdate;
    GetFormatForSQL(sql_name, sql_birthdate, sql_examdate);
#ifdef VET
    streamTerm << "SELECT ID, AnimalName, OwnerName, Species, Sex, "
        << sql_birthdate << ", "
        << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
        << "FROM patient_info, exam_info "
        << "WHERE patient_info.ID = exam_info.PatID "
        << "AND patient_info.ID LIKE '" << term.id << "' "
        << "AND patient_info.AnimalName LIKE '" << term.animal_name << "' "
        << "AND patient_info.OwnerName LIKE '" << term.owner_name << "' "
        << "AND patient_info.Species LIKE '" << term.species << "' "
        << "AND patient_info.Sex LIKE '" << term.gender << "' "
        << "AND patient_info.BirthYear LIKE '" << term.birthYear << "' "
        << "AND patient_info.BirthMonth LIKE '" << term.birthMonth << "' "
        << "AND patient_info.BirthDay LIKE '" << term.birthDay << "' "
        << "AND patient_info.Age LIKE '" << term.age << "' "
        << "AND patient_info.AgeUnit LIKE '" << term.ageUnit << "' "
        << "ORDER BY ID";
#else
    streamTerm << "SELECT ID, "
           << sql_name << ", Sex, "
           << sql_birthdate << ", "
           << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
	       << "FROM patient_info, exam_info "
	       << "WHERE patient_info.ID = exam_info.PatID "
	       << "AND patient_info.ID LIKE '" << term.id << "' "
	       << "AND patient_info.LastName LIKE '" << term.name.last << "' "
 	       << "AND patient_info.FirstName LIKE '" << term.name.first << "' "
 	       << "AND patient_info.MidName LIKE '" << term.name.mid << "' "
	       << "AND patient_info.Sex LIKE '" << term.gender << "' "
 	       << "AND patient_info.BirthYear LIKE '" << term.birthYear << "' "
 	       << "AND patient_info.BirthMonth LIKE '" << term.birthMonth << "' "
 	       << "AND patient_info.BirthDay LIKE '" << term.birthDay << "' "
 	       << "AND patient_info.Age LIKE '" << term.age << "' "
 	       << "AND patient_info.AgeUnit LIKE '" << term.ageUnit << "' "
	       << "ORDER BY ID";
#endif
    sql = streamTerm.str();
    PRINTF("sql = %s\n", sql.c_str());
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
    	PRINTF("%s\n", sqlite3_errmsg(m_db));
    	return false;
    }

    result.clear();
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	NewPatSearchResult tmp;
        int col = 0;
	if (strcmp(sqlite3_column_name(stmt, col), "ID") == 0)
	    tmp.ID.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            tmp.animal_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            tmp.owner_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0) {
            int index = sqlite3_column_int(stmt, col);
            GetAnimalSpeciesString(index, tmp.species);
        }
        ++col;
#else
        if (strcmp(sqlite3_column_name(stmt, col), "Name") == 0)
            tmp.name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#endif


	if (strcmp(sqlite3_column_name(stmt, col), "Sex") == 0) {
	    int sex = sqlite3_column_int(stmt, col);
            GetSexString(sex, tmp.gender);
	}
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "BirthDate") == 0)
	    tmp.birthDate.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "ExamDate") == 0)
	{
		tmp.examDate.assign((const char *)sqlite3_column_text(stmt, col));
       ++col;
	}
	if (strcmp(sqlite3_column_name(stmt, col), "ExamID") == 0)
	    tmp.examID.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "ExamType") == 0)
	    tmp.examType.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        int age = 0;
        int ageUnit = 0;
	if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
            age = sqlite3_column_int(stmt, col);
        ++col;
	if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
            ageUnit = sqlite3_column_int(stmt, col);
        GenAgeStr(age, ageUnit, tmp.age);

	result.push_back(tmp);
    }
    sqlite3_finalize(stmt);
    return true;
}

// 搜索数据库中doctor_info表中的所有医生姓名
bool Database::DoctorSearch(vector<string> *result)
{
    sqlite3_stmt *stmt=NULL;
    string sql = "SELECT Name FROM doctor_info ORDER BY ID";
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
    	PRINTF("%s\n", sqlite3_errmsg(m_db));
    	return false;
    }
    result->clear();
    string doc_name;
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (strcmp(sqlite3_column_name(stmt, 0), "Name") == 0)
	    doc_name = (const char *)sqlite3_column_text(stmt, 0);
        if (!doc_name.empty())
            result->push_back(doc_name);
    }
    sqlite3_finalize(stmt);
    return true;
}
#ifdef VET
//添加VET 主人名名字的获取
bool Database::OwnerNameSearch(vector<string> *result)
{
    sqlite3_stmt *stmt=NULL;
    string sql = "SELECT OwnerName FROM patient_info ORDER BY ID";
    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
    	PRINTF("%s\n", sqlite3_errmsg(m_db));
    	return false;
    }
    result->clear();
    string doc_name;
    while (sqlite3_step(stmt) != SQLITE_DONE) {
	if (strcmp(sqlite3_column_name(stmt, 0), "OwnerName") == 0)
	    doc_name = (const char *)sqlite3_column_text(stmt, 0);
        if (!doc_name.empty())
            result->push_back(doc_name);
    }
    sqlite3_finalize(stmt);
    return true;
}
#endif

void StrtoStr(string &Srcstr, string &Desstr)
{
    char buf[50], bufT[50];
    int j(0),k(0);

    SysGeneralSetting sgs;
    int dateFormat = sgs.GetDateFormat();

    for(int i = 0;Srcstr[i] != '\0'; i++)
    {
        if (Srcstr[i] >= '0' && Srcstr[i] <= '9')
        {
            buf[k] = Srcstr[i];
            k++;
        }
        else
        {
            j = i;
            Srcstr[j] = Srcstr[j+1];
            j++;
        }
    }
    buf[k] = '\0';
    switch (dateFormat) {
        case 0:
            Desstr = buf;
            break;
        case 1:
            bufT[0] = buf[4];
            bufT[1] = buf[5];
            bufT[2] = buf[6];
            bufT[3] = buf[7];
            bufT[4] = buf[0];
            bufT[5] = buf[1];
            bufT[6] = buf[2];
            bufT[7] = buf[3];
            bufT[8] = '\0';
            Desstr = bufT;
            break;
        case 2:
            bufT[0] = buf[4];
            bufT[1] = buf[5];
            bufT[2] = buf[6];
            bufT[3] = buf[7];
            bufT[4] = buf[2];
            bufT[5] = buf[3];
            bufT[6] = buf[0];
            bufT[7] = buf[1];
            bufT[8] = '\0';
            Desstr = bufT;
            break;
    }
    PRINTF("********SrcStr buf  Desstr %s %s %s\n",Srcstr.c_str(),buf,Desstr.c_str());
}

bool Database::ArchiveSearch(NewPatSearchTerm& term, vector<NewPatSearchResult>& result)
{
    sqlite3_stmt *stmt=NULL;
    ostringstream streamTerm;
    string sql;
    string strDate;
    string strD;
    int iTmpSt(0),iTmpEd(0),iTmp(0);

    // get range of birthday
    string birthBegin;
    string birthEnd;
    GetBirthRange(term.age, term.ageUnit, birthBegin, birthEnd);

    // get exam date range
    string examBegin;
    string examBeginTmp;
    string year = term.examDateStartYear;
    string mon = term.examDateStartMonth;
    string day = term.examDateStartDay;
    ValidDateFormat(year, mon, day);
    GenerateDataString(year, mon, day, examBeginTmp);
    StrtoStr(examBeginTmp, examBegin);
    iTmpSt = atoi(examBegin.c_str());

    string examEnd;
    string examEndTmp;
    year = term.examDateEndYear;
    mon = term.examDateEndMonth;
    day = term.examDateEndDay;
    ValidDateFormat(year, mon, day);
    GenerateDataString(year, mon, day, examEndTmp);
    StrtoStr(examEndTmp, examEnd);
    iTmpEd = atoi(examEnd.c_str());

#if 1
    // get sql
    string sql_name, sql_birthdate, sql_examdate;
    GetFormatForSQL(sql_name, sql_birthdate, sql_examdate);

    // search
#ifdef VET
    streamTerm << "SELECT ID, AnimalName, OwnerName, Species, Sex, "
    //streamTerm << "SELECT ID, OwnerName, Species, Sex, "
        << sql_birthdate << ", "
        << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
        << "FROM patient_info, exam_info "
        << "WHERE patient_info.ID = exam_info.PatID "
        << "AND patient_info.ID LIKE '" << term.id << "' "
        << "AND patient_info.AnimalName LIKE '" << term.animal_name << "' "
        << "AND patient_info.OwnerName LIKE '" << term.owner_name << "' "
        << "AND patient_info.Species LIKE '" << term.species << "' "
        << "AND patient_info.Sex LIKE '" << term.gender << "' "
        << "AND patient_info.Age LIKE '" << term.age << "' "
        << "AND patient_info.AgeUnit LIKE '" << term.ageUnit << "' "
        //<< "AND ExamDate >= '" << examBegin << "' " << "AND ExamDate <= '" << examEnd << "' "
        << "ORDER BY ExamID";
#else
    // search
    streamTerm << "SELECT ID, "
               << sql_name << ", Sex, "
               << sql_birthdate << ", "
               << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
	       << "FROM patient_info, exam_info "
	       << "WHERE patient_info.ID = exam_info.PatID "
	       << "AND patient_info.ID LIKE '" << term.id << "' "
	       << "AND Name LIKE '" << term.totalName << "' "
	       << "AND patient_info.Sex LIKE '" << term.gender << "' "
			//<< "AND BirthDate >= '" << birthBegin << "' " << "AND BirthDate <= '" << birthEnd << "' "
           << "AND patient_info.Age LIKE '" << term.age << "' "
           << "AND patient_info.AgeUnit LIKE '" << term.ageUnit << "' "
         //<< "AND ExamDate >= '" << examBegin << "' " << "AND ExamDate <= '" << examEnd << "' " //2012.09.04
	       << "ORDER BY ID";
#endif
    sql = streamTerm.str();
    PRINTF("sql: %s\n", sql.c_str());

    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
    	PRINTF("%s\n", sqlite3_errmsg(m_db));
    	return false;
    }

    // get search result
    result.clear();
    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int col=1;
        NewPatSearchResult tmp;
        if (strcmp(sqlite3_column_name(stmt, 0), "ID") == 0)
	    tmp.ID.assign((const char *)sqlite3_column_text(stmt, 0));
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            tmp.animal_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            tmp.owner_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0) {
            int index = sqlite3_column_int(stmt, col);
            GetAnimalSpeciesString(index, tmp.species);
        }
        ++col;
#else
        if (strcmp(sqlite3_column_name(stmt, col), "Name") == 0)
            tmp.name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#endif

      if (strcmp(sqlite3_column_name(stmt, col), "Sex") == 0) {
            int sex = sqlite3_column_int(stmt, col);
            GetSexString(sex, tmp.gender);
        }

        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "BirthDate") == 0)
            tmp.birthDate = (const char*)sqlite3_column_text(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamDate") == 0)
        {
            strDate = (const char*)sqlite3_column_text(stmt, col);
            string strDateTemp(strDate);
            StrtoStr(strDateTemp,strD);
            iTmp = atoi(strD.c_str());
        }
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamID") == 0)
            tmp.examID = (const char*)sqlite3_column_text(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamType") == 0)
            tmp.examType = _((const char*)sqlite3_column_text(stmt, col));
        ++col;

        int age = 0;
        int ageUnit = 0;
        if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
            age = sqlite3_column_int(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
            ageUnit = sqlite3_column_int(stmt, col);

        if ( iTmpSt <= iTmp && iTmp <= iTmpEd)
        {
            tmp.examDate = strDate;
            GenAgeStr(age, ageUnit, tmp.age);
            result.push_back(tmp);
        }

    }
    sqlite3_finalize(stmt);
#endif
    return true;
}

bool Database::QuichSearch(NewPatSearchTerm& term, vector<NewPatSearchResult>& result)
{
    sqlite3_stmt *stmt;
    ostringstream ostream;
    string sql;
    string strDate;
    string strD;
    int iTmpSt,iTmpEd,iTmp;

    // get range of exam date
    string dateBegin;
    string dateBeginTmp;
    string year = term.examDateStartYear;
    string mon = term.examDateStartMonth;
    string day = term.examDateStartDay;
    ValidDateFormat(year, mon, day);
    GenerateDataString(year, mon, day, dateBeginTmp);
    StrtoStr(dateBeginTmp,dateBegin);
    iTmpSt = atoi(dateBegin.c_str());

    string dateEnd;
    string dateEndTmp;
    year = term.examDateEndYear;
    mon = term.examDateEndMonth;
    day = term.examDateEndDay;
    ValidDateFormat(year, mon, day);
    GenerateDataString(year, mon, day, dateEndTmp);
    StrtoStr(dateEndTmp,dateEnd);
    iTmpEd = atoi(dateEnd.c_str());

    //sql
    string sql_name, sql_birthdate, sql_examdate;
    GetFormatForSQL(sql_name, sql_birthdate, sql_examdate);
#ifdef VET
    ostream << "SELECT ID, AnimalName, OwnerName, Species, Sex, "
        << sql_birthdate << ", "
        << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
        << "FROM patient_info, exam_info "
        << "WHERE patient_info.ID = exam_info.PatId "
        //<< "AND ExamDate >= '" << dateBegin << "' AND ExamDate <= '" << dateEnd << "' "
       // << "ORDER BY ExamID";
        << "ORDER BY ID";
#else
     ostream << "SELECT ID, "
         << sql_name << ", Sex, "
         << sql_birthdate << ", "
         << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
         << "FROM patient_info, exam_info "
         << "WHERE patient_info.ID = exam_info.PatId "
         //<< "AND ExamDate >= '" << dateBegin << "' AND ExamDate <= '" << dateEnd << "' " //2012.08.21lihuamei 为了解决快速搜索在日月年的格式下搜索出现的问题
          << "ORDER BY ID";
#endif
     sql = ostream.str();
     PRINTF("sql: %s\n", sql.c_str());

    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        PRINTF("%s\n", sqlite3_errmsg(m_db));
        return false;
    }

    while(sqlite3_step(stmt) != SQLITE_DONE)
    {
        int col=1;
        NewPatSearchResult temp;
             if (strcmp(sqlite3_column_name(stmt, 0), "ID") == 0)
            temp.ID = (const char*)sqlite3_column_text(stmt, 0);
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            temp.animal_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            temp.owner_name.assign((const char *)sqlite3_column_text(stmt, col));

        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0) {
            int index = sqlite3_column_int(stmt, col);
            GetAnimalSpeciesString(index, temp.species);
        }

        ++col;
#else
        if (strcmp(sqlite3_column_name(stmt, col), "Name") == 0)
            temp.name.assign((const char *)sqlite3_column_text(stmt, col));
           // temp.name = name;
        ++col;
#endif

        if (strcmp(sqlite3_column_name(stmt, col), "Sex") == 0) {
            int sex = sqlite3_column_int(stmt, col);
            GetSexString(sex, temp.gender);
        }

        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "BirthDate") == 0)
            temp.birthDate = (const char*)sqlite3_column_text(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamDate") == 0)
        {
            strDate = (const char*)sqlite3_column_text(stmt, col);
            string strDateTemp(strDate);
            StrtoStr(strDateTemp,strD);
            iTmp = atoi(strD.c_str());
        }
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamID") == 0)
            temp.examID = (const char*)sqlite3_column_text(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamType") == 0)
            temp.examType = _((const char*)sqlite3_column_text(stmt, col));
        ++col;

        int age = 0;
        int ageUnit = 0;
        if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
            age = sqlite3_column_int(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
            ageUnit = sqlite3_column_int(stmt, col);

        if ( iTmpSt <= iTmp && iTmp <= iTmpEd)
        {
            temp.examDate = strDate;
            GenAgeStr(age, ageUnit, temp.age);
            result.push_back(temp);
        }
    }
    sqlite3_finalize(stmt);

    return true;
}

bool Database::AllInfoSearch(string examID, PatientInfo::Info &result)
{
    sqlite3_stmt *stmt;
    ostringstream ostream;
    string sql;

#ifdef VET
    ostream << "SELECT ID, AnimalName, OwnerName, Species, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Comment, "
        << "ExamID, ExamType, ExamYear, ExamMonth, ExamDay, ExamHour, ExamMinute, ExamSecond, Height, Weight, BSA, LMPYear, LMPMonth, LMPDay, "
        << "OvulateYear, OvulateMonth, OvulateDay, PregCount, AbnormalPregCount, FetusCount, DeliveryCount, MiscarryCount, HR, "
        << "PressureHigh, PressureLow, PSA, ExamDr, ReportDr, ExamComment, imagePath "
        << "FROM patient_info, exam_info "
        << "WHERE patient_info.ID = exam_info.PatId "
        << "AND ExamID = '" << examID << "' "
        << "ORDER BY ID";
#else
    ostream << "SELECT ID, LastName, FirstName, MidName, Sex, BirthYear, BirthMonth, BirthDay, Age, AgeUnit, Telephone, Address, Comment, "
            << "ExamID, ExamType, ExamYear, ExamMonth, ExamDay, ExamHour, ExamMinute, ExamSecond, Height, Weight, BSA, LMPYear, LMPMonth, LMPDay, "
            << "OvulateYear, OvulateMonth, OvulateDay, PregCount, AbnormalPregCount, FetusCount, DeliveryCount, MiscarryCount, HR, "
            << "PressureHigh, PressureLow, PSA, ExamDr, ReportDr, ExamComment, imagePath "
            << "FROM patient_info, exam_info "
            << "WHERE patient_info.ID = exam_info.PatId "
            << "AND ExamID = '" << examID << "' "
            << "ORDER BY ID";
#endif
    sql = ostream.str();
    PRINTF("sql = %s\n", sql.c_str());

    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, NULL) != SQLITE_OK) {
        PRINTF("%s\n", sqlite3_errmsg(m_db));
        return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        int col = 0;
        if (strcmp(sqlite3_column_name(stmt, col), "ID") == 0)
            result.p.id = (const char*)sqlite3_column_text(stmt, col);
        col++;
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            result.p.animal_name = (const char *)sqlite3_column_text(stmt, col);
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            result.p.owner_name = (const char *)sqlite3_column_text(stmt, col);
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0)
            result.p.species = sqlite3_column_int(stmt, col);
        ++col;
#else
        if (strcmp(sqlite3_column_name(stmt, col), "LastName") == 0)
            result.p.name.last = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "FirstName") == 0)
            result.p.name.first = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "MidName") == 0)
            result.p.name.mid = (const char*)sqlite3_column_text(stmt, col);
        col++;
#endif

        if (strcmp(sqlite3_column_name(stmt, col), "Sex") == 0)
            result.p.sex = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "BirthYear") == 0)
            result.p.birthDate.year = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "BirthMonth") == 0)
            result.p.birthDate.month = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "BirthDay") == 0)
            result.p.birthDate.day = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
        {
            result.p.age = sqlite3_column_int(stmt, col);
            //printf("result.p.age = %f\d", result.p.age);
        }
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
            result.p.ageUnit = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "Telephone") == 0)
            result.p.telephone = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "Address") == 0)
            result.p.address = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "Comment") == 0)
            result.p.comment = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamID") == 0)
            result.e.examNum = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamType") == 0)
            result.e.examType = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamYear") == 0)
            result.e.examDate.year = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamMonth") == 0)
            result.e.examDate.month = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamDay") == 0)
            result.e.examDate.day = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamHour") == 0)
            result.e.examTime.hour = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamMinute") == 0)
            result.e.examTime.minute  = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamSecond") == 0)
            result.e.examTime.second = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "Height") == 0)
            result.e.height = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "Weight") == 0)
        {
            result.e.weight = sqlite3_column_double(stmt, col);
            printf("result.p.weight = %f\d", result.e.weight);
        }
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "BSA") == 0)
            result.e.BSA = sqlite3_column_double(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "LMPYear") == 0)
            result.ob.LMPDate.year = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "LMPMonth") == 0)
            result.ob.LMPDate.month = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "LMPDay") == 0)
            result.ob.LMPDate.day = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "OvulateYear") == 0)
            result.ob.OvulateDate.year= sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "OvulateMonth") == 0)
            result.ob.OvulateDate.month = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "OvulateDay") == 0)
            result.ob.OvulateDate.day = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "PregCount") == 0)
            result.ob.pregCount = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "AbnormalPregCount") == 0)
            result.ob.abnormalPregCount = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "FetusCount") == 0)
            result.ob.fetusCount = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "DeliveryCount") == 0)
            result.ob.deliveryCount = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "MiscarryCount") == 0)
            result.ob.miscarryCount = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "HR") == 0)
            result.car.HR = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "PressureHigh") == 0)
            result.car.pressureHigh = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "PressureLow") == 0)
            result.car.pressureLow = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "PSA") == 0)
            result.uro.PSA = sqlite3_column_int(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamDr") == 0)
            result.e.examDoctor = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ReportDr") == 0)
            result.e.reportDoctor = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "ExamComment") == 0)
            result.e.comment = (const char*)sqlite3_column_text(stmt, col);
        col++;
        if (strcmp(sqlite3_column_name(stmt, col), "imagePath") == 0)
            result.e.imagePath = (const char*)sqlite3_column_text(stmt, col);
    } else {
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);

    return true;

}

bool Database::CommonSearch(string examID, NewPatSearchResult &result)
{
    sqlite3_stmt *stmt=NULL;
    ostringstream streamTerm;
    string sql;
    string sql_name, sql_birthdate, sql_examdate;
    GetFormatForSQL(sql_name, sql_birthdate, sql_examdate);
#ifdef VET
    streamTerm << "SELECT ID, AnimalName, OwnerName, Species, Sex, "
        << sql_birthdate << ", "
        << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
        << "FROM patient_info, exam_info "
        << "WHERE patient_info.ID = exam_info.PatID "
        << "AND exam_info.ExamID = '" << examID << "' "
        << "ORDER BY ID";
#else

    streamTerm << "SELECT ID, "
               << sql_name << ", Sex, "
               << sql_birthdate << ", "
               << sql_examdate << ", ExamID, ExamType, Age, AgeUnit "
	       << "FROM patient_info, exam_info "
	       << "WHERE patient_info.ID = exam_info.PatID "
 	       << "AND exam_info.ExamID = '" << examID << "' "
	       << "ORDER BY ID";
#endif
    sql = streamTerm.str();
    PRINTF("sql =%s\n", sql.c_str());

    if (sqlite3_prepare(m_db, sql.c_str(), sql.size(), &stmt, 0) != SQLITE_OK) {
    	PRINTF("%s\n", sqlite3_errmsg(m_db));
    	return false;
    }

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        int col=1;
        if (strcmp(sqlite3_column_name(stmt, 0), "ID") == 0)
            result.ID.assign((const char *)sqlite3_column_text(stmt, 0));
#ifdef VET
        if (strcmp(sqlite3_column_name(stmt, col), "AnimalName") == 0)
            result.animal_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "OwnerName") == 0)
            result.owner_name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
        if (strcmp(sqlite3_column_name(stmt, col), "Species") == 0) {
            int index = sqlite3_column_int(stmt, col);
            GetAnimalSpeciesString(index, result.species);
        }
        ++col;
#else
        if (strcmp(sqlite3_column_name(stmt, col), "Name") == 0)
            result.name.assign((const char *)sqlite3_column_text(stmt, col));
        ++col;
#endif
        if (strcmp(sqlite3_column_name(stmt, col), "Sex") == 0) {
            int sex = sqlite3_column_int(stmt, col);
            GetSexString(sex, result.gender);
        }

        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "BirthDate") == 0)
            result.birthDate = (const char*)sqlite3_column_text(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamDate") == 0)
        {
            result.examDate = (const char *)sqlite3_column_text(stmt, col);

            //strDate = (const char*)sqlite3_column_text(stmt, col);
            //string strDateTemp(strDate);
            //StrtoStr(strDateTemp,strD);
            //iTmp = atoi(strD.c_str());
        }
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamID") == 0)
            result.examID = (const char*)sqlite3_column_text(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "ExamType") == 0)
            result.examType = _((const char*)sqlite3_column_text(stmt, col));
        ++col;

        int age = 0;
        int ageUnit = 0;
        if (strcmp(sqlite3_column_name(stmt, col), "Age") == 0)
            age = sqlite3_column_int(stmt, col);
        ++col;

        if (strcmp(sqlite3_column_name(stmt, col), "AgeUnit") == 0)
            ageUnit = sqlite3_column_int(stmt, col);
            GenAgeStr(age, ageUnit, result.age);
#if 0
        if ( iTmpSt <= iTmp && iTmp <= iTmpEd)
        {
            result.examDate = strDate;
            GenAgeStr(age, ageUnit, temp.age);
            result.push_back(temp);
        }



    if (strcmp(sqlite3_column_name(stmt, 2), "Sex") == 0) {
            int sex = sqlite3_column_int(stmt, 2);
            GetSexString(sex, result.gender);
        }
        if (strcmp(sqlite3_column_name(stmt, 3), "BirthDate") == 0)
            result.birthDate.assign((const char *)sqlite3_column_text(stmt, 3));
        if (strcmp(sqlite3_column_name(stmt, 4), "ExamDate") == 0)
            result.examDate.assign((const char *)sqlite3_column_text(stmt, 4));
        if (strcmp(sqlite3_column_name(stmt, 5), "ExamID") == 0)
            result.examID.assign((const char *)sqlite3_column_text(stmt, 5));
        if (strcmp(sqlite3_column_name(stmt, 6), "ExamType") == 0)
            result.examType.assign((const char *)sqlite3_column_text(stmt, 6));

        int age = 0;
        int ageUnit = 0;
	if (strcmp(sqlite3_column_name(stmt, 7), "Age") == 0)
            age = sqlite3_column_int(stmt, 7);
	if (strcmp(sqlite3_column_name(stmt, 8), "AgeUnit") == 0)
            ageUnit = sqlite3_column_int(stmt, 8);
        GenAgeStr(age, ageUnit, result.age);
#endif
    }
    sqlite3_finalize(stmt);
    return true;
}

bool Database::DeleteRecord(string examID)
{
    string sql;
    ostringstream stream;
    char *errmsg;

    stream << "DELETE FROM exam_info "
           << "WHERE exam_info.ExamID = '" << examID << "' ";

    sql = stream.str();
    PRINTF("sql : %s\n", sql.c_str());
    if (sqlite3_exec(m_db, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK) {
        PRINTF("%s\n", sqlite3_errmsg(m_db));
        sqlite3_free(errmsg);
        return false;
    }
    return true;
}

bool Database::GetExamIDNext(string &examID)
{
    string s;
    ostringstream stream;

    if(!GetExamIDCurrent(s))
        return false;

    int i = atoi(s.c_str()) + 1;

    stream << i;

    examID = stream.str();

    return true;
}

bool Database::GetExamIDCurrent(string &examID)
{
    string sql;
    ostringstream stream;
    char **dbResult;
    int row, column;
    char *errmsg;

    stream << "SELECT MAX(ExamID) AS curExamID "
           << "FROM exam_info ";

    sql = stream.str();
    PRINTF("sql: %s\n", sql.c_str());

    if (sqlite3_get_table(m_db, sql.c_str(), &dbResult, &row, &column, &errmsg) != SQLITE_OK) {
            PRINTF("%s\n", sqlite3_errmsg(m_db));
            sqlite3_free(errmsg);
            return false;
    }

    if( (column != 1) || (row != 1) || (strcmp(dbResult[0], "curExamID") != 0) )
        return false;

    if (dbResult[1]) {
        examID = dbResult[1];
        PRINTF("Current ExamID = %s\n", examID.c_str());
    } else {
        examID = "0";
    }

    return true;
}

///>private
void Database::GetBirthRange(string age, string ageUnit, string &birthBegin, string &birthEnd)
{
	int year, mon, day;

	// get current date
	GetCurrentDate(year, mon, day);
	if (strcmp(age.c_str(), "%") == 0)
	{
		GetDateString(0, 0, 0, birthBegin);
		GetDateString(year, mon, day, birthEnd);
		return;
	}

	GDate* curDate = g_date_new_dmy((GDateDay)day, (GDateMonth)mon, (GDateYear)year);
	GDate* beginDate = g_date_new_dmy((GDateDay)day, (GDateMonth)mon, (GDateYear)year);
	GDate* endDate = g_date_new_dmy((GDateDay)day, (GDateMonth)mon, (GDateYear)year);
	int unit = atoi(ageUnit.c_str());
	int iAge = atoi(age.c_str());
	switch(unit)
	{
		case 0: //年
			g_date_subtract_years(beginDate, iAge+1);
			g_date_subtract_years(endDate, iAge);
			break;

		case 1: //月
			g_date_subtract_months(beginDate, iAge+1);
			g_date_subtract_months(endDate, iAge);
			break;

		case 2: //日
			g_date_subtract_days(beginDate, iAge+1);
			g_date_subtract_days(endDate, iAge);
			break;

		default:
			PRINTF("range of ageUnit is out of range\n");
			break;
	}

	year = (int)g_date_get_year(beginDate);
	mon = (int)g_date_get_month(beginDate);
	day = (int)g_date_get_day(beginDate);
	GetDateString(year, mon, day, birthBegin);

	year = (int)g_date_get_year(endDate);
	mon = (int)g_date_get_month(endDate);
	day = (int)g_date_get_day(endDate);
	GetDateString(year, mon, day, birthEnd);

	g_date_free(curDate);
	g_date_free(beginDate);
	g_date_free(endDate);
}
