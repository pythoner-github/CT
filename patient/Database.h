#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sqlite3.h>
#include "PatientInfo.h"
#include <vector>

#define PATIENT_DB "./res/patient.db"

using std::vector;

class Database{
public:
    Database();
    ~Database();
   //enum RetType {SQLOK, DBERR, NOID};
#ifdef VET
    struct NewPatSearchTerm{
	string id;
	string animal_name;
	string owner_name;
	string species;
	string gender;
	string birthYear;
	string birthMonth;
	string birthDay;
	string age; //range(0-999) sample 10
	string ageUnit; //range(0,1,2 means Y/M/D), sample 0 
	string examDateStartYear;
	string examDateStartMonth;
	string examDateStartDay;
	string examDateEndYear;
	string examDateEndMonth;
	string examDateEndDay;
    };
#else
    //enum RetType {SQLOK, DBERR, NOID};
    struct NewPatSearchTerm{
	string id;
	PatientInfo::Name name;
	string totalName;
	string gender;
	string birthYear;
	string birthMonth;
	string birthDay;
	string age; //range(0-999) sample 10
	string ageUnit; //range(0,1,2 means Y/M/D), sample 0 
	string examDateStartYear;
	string examDateStartMonth;
	string examDateStartDay;
	string examDateEndYear;
	string examDateEndMonth;
	string examDateEndDay;
    };
#endif

#ifdef VET
    struct NewPatSearchResult{
	string ID;
	string animal_name;
	string owner_name;
	string species;
	string gender;
	string birthDate;
	string age; // 010Y/M/D
	string examDate;
	string examID;
	string examType;
	string examDoctor; // to be finshed 
    };
#else
    struct NewPatSearchResult{
	string ID;
	string name;
	string gender;
	string birthDate;
	string age; // 010Y/M/D
	string examDate;
	string examID;
	string examType;
	string examDoctor; // to be finshed 
    };
#endif
    bool OpenDB(void);
    bool CloseDB(void);

    // void Testdb(void);
    // void TestBinddb(char *id);

    bool CreateTable(void);
    bool ArchivePat(PatientInfo::Info &info, std::string &errorMsg);
    string GetPatIDExist(const char *id);
    bool GetDoctorExist(const char *doc_name);
    bool DoctorSearch(vector<std::string> *result);
#ifdef VET
    bool OwnerNameSearch(vector<string> *result);
#endif 
    void GetPatInfo(const char *patID, PatientInfo::Patient &pat_info);
    void GetPatExamInfo(const char *examID, PatientInfo::Info &info);
    bool NewPatSearch(NewPatSearchTerm& term, vector<NewPatSearchResult>& result);
    bool ArchiveSearch(NewPatSearchTerm& term, vector<NewPatSearchResult>& result);
    bool QuichSearch(NewPatSearchTerm& term, vector<NewPatSearchResult>& result);
    bool AllInfoSearch(string examID, PatientInfo::Info &result);
    bool CommonSearch(string examID, NewPatSearchResult &result);
    bool DeleteRecord(string examID);
    bool GetExamIDCurrent(string &examID);
    bool GetExamIDNext(string &examID);
private:
    static sqlite3* m_db;
    void GetBirthRange(string age, string ageUnit, string &begin, string &end);
    void GetFormatForSQL(std::string &name, std::string &birthdate, std::string &examdate);
    
    bool m_langCN; 
    int m_dateFormat;
// call back function
};

#endif
