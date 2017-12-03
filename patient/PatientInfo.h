#ifndef PATIENT_INFO_H
#define PATIENT_INFO_H

#include <string>

using std::string;

class PatientInfo
{
	public:
		PatientInfo();
		~PatientInfo();

		struct Name
		{
			string last;
			string first;
			string mid;
		};
		struct Date
		{
			int year;
			int month;
			int day;
		};
		struct Time
		{
			int hour;
			int minute;
			int second;
		};
#ifdef VET
        struct Patient
        {
            string id;
            string animal_name;
            string owner_name;
            int species;
            int sex;
            int age;
            int ageUnit;
            Date birthDate;
            string telephone;
            string address;
            string comment;

        
        };
#else
		struct Patient
		{
			string id;
			Name name;
			int sex; ///< 0: Female; 1: Male 2: Other
			int age; 
			int ageUnit; ///< 0: year; 1-month; 2-day
			Date birthDate;
			string telephone;
			string address;
			string comment;
		};
#endif 
		struct ExamGeneral
		{
			string examNum;
			string examType;
			Date examDate;
			Time examTime;
			int height; ///< unit:cm
			double weight; ///< unit: kg
			double BSA; ///< body serface area, unit:m2
			string examDoctor;
			string reportDoctor;
			string comment; ///< exam comment
			string imagePath;
		};
		struct ObExam
		{
			Date LMPDate;
			Date OvulateDate;
			int pregCount; ///< count of pregnancy normally
			int abnormalPregCount; ///< count of pregnancy abmormally
			int fetusCount;
			int deliveryCount;
			int miscarryCount;
		};
		struct CarExam
		{
			int HR; ///< heart rate, unit:b/min
			int pressureHigh; ///< unit: 10*mmHg
			int pressureLow; ///< unit: 10*mmHg
		};
		struct UroExam
		{
			int PSA; ///< prostate specific antigen
		};
		struct Info
		{
			Patient p;
			ExamGeneral e;
			ObExam ob;
			CarExam car;
			UroExam uro;
		};

		void GetInfo(Info &info);
		void SetInfo(Info info);
		void SetInfoQA(Info info);
		void ClearRecord();
		void ClearExam();
		void ClearAll();

		void UpdateTopArea(void);
		void SetExamType(const std::string &examtype) { m_info.e.examType = examtype; }

		bool ArchivePatientInfo(string &errmsg);
		void ArchiveImg(); 
		void ArchiveReport(); 
		bool GetExist(void) { return m_exist; }

        void UpdatePatInfoToTopArea(Name name ,string sex,string age,string id);

	private:
		Info m_info;
		bool m_exist;
        
        void InitComment();
		void InitPatient();
		void InitExam();
		void InitExamGen();
		void InitExamOb();
		void InitExamCar();
		void InitExamUro();
		void ClearExamInfo();
		void ClearPatientInfo();
};

extern PatientInfo g_patientInfo;
#endif
