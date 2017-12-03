/*
 * 2009, 深圳恩普电子技术有限公司
 *
 * @file: SysLog.cpp
 * @brief: Print system info to file log/sys.log, and this file will be save when power off 
 * and clear when power on.
 *
 * version: V1.0
 * date: 2009-5-6
 * @author: zhanglei
 */

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <string>
#include "SysLog.h"

using std::cout;
using std::endl;
using std::stringstream;
using std::ofstream;
using std::string;


///> init static variable
class SysLog* SysLog::m_ptrInstance = NULL;

///>realize funtion member of class
SysLog::SysLog()
{
	Open();
}

SysLog::~SysLog()
{
	if (is_open()) close();

	if (m_ptrInstance != NULL)
		delete (m_ptrInstance);
}

/*
 * @brief: open a file to save system info, and add some head info at the begin of file.
 */
void SysLog::Open()
{
	///> write date and time at begin of log file
	time_t at; //absolut time
	at = time (&at);

	struct tm* ct; //current time
	ct = localtime(&at);

	stringstream s;
	s.clear();
	s << "***** Log Name: Project(09001) " << endl
		<< "***** Log Time: "
		<< (ct->tm_year + 1900) << "-"
		<< (ct->tm_mon+1) << "-"
		<< ct->tm_mday << " "
		<< ct->tm_hour << ":"
		<< ct->tm_min << ":"
		<< ct->tm_sec << endl
		<< endl;

	ofstream outfile;
	outfile.open(FILENAME, ofstream::out | ofstream::trunc);
	outfile << s.str();
	outfile.close();

	///> open log file and clear all data in it
	open(FILENAME, ofstream::app);
}

class SysLog* SysLog::Instance()
{
	if (m_ptrInstance == NULL)
	{
		//class SysLog log;
		m_ptrInstance = new (class SysLog);
	}

	return (m_ptrInstance);
}
