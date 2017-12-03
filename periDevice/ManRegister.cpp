/*
 * 2015, 深圳恩普电子技术有线公司
 *
 * @file: ManRegister.cpp
 * @brief: implement for the CManRegister class, generate unique register key for each computer,
 * and it can generate license file, and judge that whether the inputing key is authorised.
 *
 *
 * @version: 1.0
 * @author: lihuamei
 * @date: 2015-11-10
 */

#include "ManRegister.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <scsi/sg.h>
#include <math.h>
#include <sys/socket.h>
#include <net/if.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "Def.h"
#include "ViewSystem.h"


#define MAXSIZE_MAC 6
#define MAXSIZE_SD 20
#define MAXSIZE_VALUE 8
#define MAXSIZE_LICENSE_LENGTH 240
#define ETHERNET_INTERFACE "eth0"
#define SCSIDISK_NAME "/dev/sda"
#define SCSI_TIMEOUT 5000 /* ms */
#define LICENSE_FILENAME "license"
#define KEY_FILENAME "registerkey"   ///>dicom
#define DIR_SEPARATOR "/"
#define DIR_SEPARATOR_CHAR '/'
#define DCMDB_DB "DCMDB.db"

const string CManRegister::Optional[MAX_OPTIONAL_NUM] =
{
    "Dicom",
    "Anatomic M", 
    "CW",         
    "eFci",      
    "eView",     
    "IMT",       
};

const char* CManRegister::ValueOpt[MAX_OPTIONAL_NUM] =
{
    "2ai3dc8e", "3ecfdf4a", "ef564a4c", "83dcb2f5", "bc072ca3", "25f8a3f1"};

CManRegister* CManRegister::m_ptrInstance = NULL;

void CManRegister::Create(string resFileDir)
{
	if(m_ptrInstance == NULL)
		m_ptrInstance = new CManRegister(resFileDir);
}

void CManRegister::Destroy()
{
	if(m_ptrInstance != NULL)
		delete m_ptrInstance, m_ptrInstance = NULL;
}

CManRegister* CManRegister::GetInstance()
{
	return m_ptrInstance;
}

CManRegister::CManRegister(string resFileDir) : m_resFilePath(""), m_isAuthorize(false)
{
    for(int i = 0; i < MAX_OPTIONAL_NUM; i++)
        m_optAuthorize[i] = false;//true;
    m_currentFileName = "";
    m_currentIndex = 0;
	if (resFileDir == "")
		m_resFilePath = resFileDir + "/mnt/disk3" + DIR_SEPARATOR;
	else if (resFileDir.at(resFileDir.length()-1) != DIR_SEPARATOR_CHAR)
		m_resFilePath = resFileDir + DIR_SEPARATOR_CHAR;
	else
		m_resFilePath = resFileDir;
	
	string registerKey = "";
    /**查找当前目录下所有文件名称
     * 读取文件内容，校验，赋值
     */
    if (access(m_resFilePath.c_str(), F_OK) < 0)
        PRINTF("The directory used to store the license is invalid!\n");

    vector<string> file_name; 
    GetFiles(m_resFilePath.c_str(), file_name);

    for(int i = 0; i < file_name.size(); i++)
    {
        PRINTF("file%d: %s\n", i, file_name[i].c_str());
        for(int j = 0; j < MAX_OPTIONAL_NUM; j++){
            if(file_name[i] == Optional[j]){
                ///>读取文件内容，比较
                if(ReadRegisterKeyFile(registerKey, file_name[i]))
                {
                    if(registerKey == GenerateRegisterKey(j))
                        m_optAuthorize[j] = true;
                }
                break;
            }
        }
    }
    ///>如果dicom注册成功，复制dcmdb数据库到./res目录下，因为dicom库关于数据库的目录就是这个
    if(m_optAuthorize[0])
    {
        char cmd[256];
        sprintf(cmd, "cp %s/%s %s", OPTIONAL_DIR, DCMDB_DB, DCMDB_DIR);
        _system_(cmd);
    }
}

CManRegister::~CManRegister()
{
    if(m_ptrInstance != NULL)
        delete m_ptrInstance;
    m_ptrInstance = NULL;
}


/*
 * @biref: 获取选配功能加密之后的序列号,并保存到license文件中
 * para destFileDir[in]: 文件路径
 */
bool CManRegister::GenerateLicenseFile(string destFileDir, string fileName)
{
	if (access(destFileDir.c_str(), F_OK) < 0)
	{
		PRINTF("The directory used to store the license is invalid!\n");
		return false;
	}
    if(fileName == "")
    {
		PRINTF("The optional fuction is invalid!\n");
		return false;
    }
    for(int j = 0; j < MAX_OPTIONAL_NUM; j++)
    {
        if(fileName == Optional[j])
            m_currentIndex = j;
    }
    m_currentFileName = fileName;

    string destFilePath = "";
	if (destFileDir.at(destFileDir.length()-1) != DIR_SEPARATOR_CHAR)
		destFilePath = destFileDir + DIR_SEPARATOR_CHAR + LICENSE_FILENAME;
	else
		destFilePath = destFileDir + LICENSE_FILENAME;

    string srcFilePath = m_resFilePath + LICENSE_FILENAME;
    PRINTF("srcFilePath:%s\n", srcFilePath.c_str());

	if (access(srcFilePath.c_str(), F_OK) == 0)
	{
		PRINTF("The license file have existed!\n");
		if (rename(srcFilePath.c_str(), destFilePath.c_str()) < 0)
		{
			PRINTF("Fail to mv license file!\n");
			return false;
		}
		return true;
	}

	int fd = open(destFilePath.c_str(), O_CREAT|O_TRUNC|O_RDWR, 00777);
	
	if (fd < 0)
	{
		PRINTF("Fail to open license file!\n");
		return false;
	}

	char code[MAXSIZE_LICENSE_LENGTH] = {0};
	GenerateLicenseCode(code);
	if (write(fd, code, MAXSIZE_LICENSE_LENGTH) < 0)
	{
		PRINTF("Fail to write license file!\n");
		return false;
	}

	close(fd);
	PRINTF("Success to generate license file!\n");
    return true;
}

/**
 * @brief: 存储密钥到相应选配文件
 * para registerKey[in]: 注册序列号
 */
bool CManRegister::SaveRegisterKeyFile(string registerKey)
{
	string filePath = m_resFilePath + m_currentFileName; //KEY_FILENAME;
	int fd = open(filePath.c_str(), O_CREAT|O_TRUNC|O_RDWR, 00777);
	
	if (fd < 0)
	{
		PRINTF("Fail to open register key file!\n");
		return false;
	}

	if (write(fd, registerKey.c_str(), registerKey.length()+1) < 0)
	{
		PRINTF("Fail to write register key file!\n");
		return false;
	}

	close(fd);
	return true;
}

bool CManRegister::ReadRegisterKeyFile(string &registerKey, string fileName)
{
	string filePath = m_resFilePath + fileName;
	int fd = open(filePath.c_str(), O_RDWR);
	
	if (fd < 0)
	{
		PRINTF("Fail to open register key file!\n");
		return false;
	}

	char key[256] = {0};
	char *pKey = key;
	int i = 0;
	while (1)
	{
		if (read(fd, pKey, 1) < 0)
		{
			PRINTF("Fail to read register key file!\n");
			return false;
		}

		if (pKey == 0 || i == 255)
			break;

		i++;
		pKey++;
	}

	registerKey = key;

	close(fd);
	return true;
}

/**
 * @brief: 序列号校验，成功则选配添加成功且保存序列号到文件
 * para registerKey[in]: 解密软件生成的序列号
 */
bool CManRegister::CheckAuthorize(string registerKey, int optionIndex)
{
    if(optionIndex >= MAX_OPTIONAL_NUM)
    {
        PRINTF("index is max MAX_OPTIONAL_NUM!\n");
        return false;
    }
    m_currentIndex = optionIndex;
    PRINTF("check inputkey:%s  generateKey[%d]:%s\n", registerKey.c_str(), m_currentIndex,  GenerateRegisterKey(m_currentIndex).c_str());
	if (registerKey != GenerateRegisterKey(m_currentIndex))
    {
        PRINTF("key is not valid!\n");
		return false;
    }

    m_currentFileName = Optional[optionIndex];
	if (!SaveRegisterKeyFile(registerKey))
	{
		PRINTF("Fail to save register key file!\n");
		return false;
	}

	return true;
}

/**
 * @brief: 判断选配是否注册, 返回状态
 * @para optional[in]: 选配功能的名称
 */
bool CManRegister::IsAuthorize(string optional)
{
    for(int i = 0; i < MAX_OPTIONAL_NUM; i++)
    {
        if(optional == Optional[i])
            return m_optAuthorize[i];
    }
    return false;
}

/**
 * @brief: 设置选配状态
 */
void CManRegister::SetAuthorize(string optional, bool status)
{
    for(int i = 0; i < MAX_OPTIONAL_NUM; i++)
    {
        if(optional == Optional[i])
            m_optAuthorize[i] = status;
    }
}

/**
 *@brief: 获取目录下文件名称
 *para *path[in]: 目录路径
 *para files[out]: 当前目录下文件名称
 */
void CManRegister::GetFiles(const char *path, vector<string>& files)
{ 
	const char *name;
	GDir *dir;
	GError *err = NULL;
    dir = g_dir_open(path, 0, &err);
    if(!dir)
    {
        PRINTF("%s: g_dir_open with error: %s\n", __FUNCTION__, err->message);
        return;
	}
    name = g_dir_read_name(dir);
    if(name == NULL)
    {
        PRINTF("%s: the dir is null!\n", __FUNCTION__);
        g_dir_close(dir);
        return; 
    }
    string str;
    while(name != NULL)
    {
        str = name;
        files.push_back(str);
        name = g_dir_read_name(dir);
    }

	g_dir_close(dir);
}

string CManRegister::GenerateRegisterKey(int optionalIndex)
{
	unsigned char scsiID[MAXSIZE_SD];
	unsigned char macID[MAXSIZE_MAC];
    unsigned char optValue[MAXSIZE_VALUE];
    memset(scsiID, 0, sizeof(scsiID));
    memset(macID, 0, sizeof(macID));
    memset(optValue, 0, MAXSIZE_VALUE);

	GetSCSIDiskID(scsiID);
	GetMacID(macID);
    GetValue(optValue, optionalIndex);

	int i;
	unsigned int segA = 0, segB = 0, segC = 0, segD = 0;
	for (i = 0; i < 20; i++)
		segA += (unsigned int)(scsiID[i] * pow((double)2, (double)i));

	for (i = 0; i < 6; i++)
		segB += (unsigned int)(macID[i] * pow((double)3, (double)i));

	for (i = 0; i < 6; i++)
		segC += (unsigned int)(macID[i] * pow((double)10, (double)i));

    for(i = 0; i < MAXSIZE_VALUE; i++)
        segD += (unsigned)(optValue[i] * pow((double)2, (double)i));

	char key[256];
    memset(key, 0, sizeof(key));
    if(optionalIndex == 0)
        sprintf(key, "%x%x%x", segA, segB, segC);
    else
        sprintf(key, "%x%x%x%x", segA, segB, segC, segD);
	PRINTF("Key is %s\n", key);

	return key;
}

bool CManRegister::GetSCSIDiskID(unsigned char *serialNumber)
{
	int fd;

    fd = OpenSCSIDevice(SCSIDISK_NAME);
    if (fd < 0)
	{
        PRINTF("Could not open SCSI device %s\n", SCSIDISK_NAME);
		return false;
    }

   if (SCSIInquiryUnitSerialNumber(fd, serialNumber) < 0)
   {
	   PRINTF("Fail to inquiry serial number!\n");
	   return false;
   }

   return true;
}

int CManRegister::GetMacID(unsigned char *mac)
{
	int sock, ret;
    struct ifreq ifr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, ETHERNET_INTERFACE);
    ret = ioctl(sock, SIOCGIFHWADDR, &ifr, sizeof(ifr));
    if (ret == 0 )
        memcpy(mac, ifr.ifr_hwaddr.sa_data, MAXSIZE_MAC);
    close(sock);
    return ret;
}

void CManRegister::GetValue(unsigned char *value, int optionalIndex)
{
    if(optionalIndex >= MAX_OPTIONAL_NUM)
        return;
    memcpy(value, ValueOpt[optionalIndex], MAXSIZE_VALUE);
}

int CManRegister::OpenSCSIDevice(const char *dev)
{
    int fd, vers;
    if ((fd = open(dev, O_RDWR)) < 0)
	{
        PRINTF("ERROR could not open device %s\n", dev);
        return -1;
    }

    if ((ioctl(fd, SG_GET_VERSION_NUM, &vers) < 0) || (vers < 30000)) 
	{
        PRINTF("/dev is not an sg device, or old sg driver\n");
        close(fd);
        return -1;
    }

    return fd;
}

int CManRegister::SCSIInquiryUnitSerialNumber(int fd, unsigned char *serialNumber)
{
    unsigned char cdb[] = {0x12, 0x01, 0x80, 0, 0, 0};
    unsigned int data_size = 0x00ff;
    unsigned char data[0x00ff] = {0};
    unsigned int sense_len = 32;
    unsigned char sense[32] = {0};
    int res, pl, i;

    cdb[3] = (data_size >> 8) & 0xff;
    cdb[4] = data_size & 0xff;
    PRINTF("INQUIRY Unit Serial Number:\n");

    res = SCSIIO(fd, cdb, sizeof(cdb), SG_DXFER_FROM_DEV, data, &data_size, sense, &sense_len);
    if (res)
	{
        PRINTF("SCSI_IO failed\n");
        return -1;
    }
    if (sense_len)
	{
		PRINTF("sense_len = 0\n");
        return -1;
    }

    /* Page Length */
    pl = data[3];

    /* Unit Serial Number */
    PRINTF("Unit Serial Number:");
    for (i = 0; i < pl; i++)
	{
		PRINTF("%c", data[i+4]);
		PRINTF("%02x ", data[i+4]);
		if (i < MAXSIZE_SD)
			serialNumber[i] = data[i+4] & 0xff;
	}
	PRINTF("\n");

    return 0;
}

int CManRegister::SCSIIO(int fd, unsigned char *cdb, unsigned char cdb_size, int xfer_dir,
            unsigned char *data, unsigned int *data_size,
            unsigned char *sense, unsigned int *sense_len)
{
    sg_io_hdr_t io_hdr;

    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';
    /* CDB */
    io_hdr.cmdp = cdb;
    io_hdr.cmd_len = cdb_size;

    /* Where to store the sense_data, if there was an error */
    io_hdr.sbp = sense;
    io_hdr.mx_sb_len = *sense_len;
    *sense_len=0;

    /* Transfer direction, either in or out. Linux does not yet
       support bidirectional SCSI transfers ?
     */
    io_hdr.dxfer_direction = xfer_dir;

    /* Where to store the DATA IN/OUT from the device and how big the
       buffer is
     */
    io_hdr.dxferp = data;
    io_hdr.dxfer_len = *data_size;

    /* SCSI timeout in ms */
    io_hdr.timeout = SCSI_TIMEOUT;

    if(ioctl(fd, SG_IO, &io_hdr) < 0){
        perror("SG_IO ioctl failed");
        return -1;
    }
    /* now for the error processing */
    if((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK){
        if(io_hdr.sb_len_wr > 0){
            *sense_len=io_hdr.sb_len_wr;
            return 0;
        }
    }
    if(io_hdr.masked_status){
        PRINTF("status=0x%x\n", io_hdr.status);
        PRINTF("masked_status=0x%x\n", io_hdr.masked_status);
        return -2;
    }
    if(io_hdr.host_status){
        PRINTF("host_status=0x%x\n", io_hdr.host_status);
        return -3;
    }
    if(io_hdr.driver_status){
        PRINTF("driver_status=0x%x\n", io_hdr.driver_status);
        return -4;
    }
    return 0;
}

/**
 * @brief: 生成加密序列号
 */
void CManRegister::GenerateLicenseCode(char *code)
{
	unsigned int randBit = 10;
	char *ptrCurPos = code;

	unsigned char scsiID[MAXSIZE_SD] = {0};
	unsigned char macID[MAXSIZE_MAC] = {0};
    unsigned char value[MAXSIZE_VALUE] = {0};

	GetSCSIDiskID(scsiID);
	GetMacID(macID);
    GetValue(value, m_currentIndex);
	
	sprintf(ptrCurPos, "%010d", GetRand());
	ptrCurPos += randBit;

	char offset = (char)(10 + GetSecond()/2);
	*ptrCurPos = offset;
	ptrCurPos++;

	int i;
	for (i = 0; i < offset; i+=randBit)
	{
		sprintf(ptrCurPos, "%010d", GetRand());
		if (offset - i < (int)randBit)
			ptrCurPos += (offset - i);
		else
			ptrCurPos += randBit;
	}

	memcpy(ptrCurPos, scsiID, MAXSIZE_SD);
	ptrCurPos += MAXSIZE_SD;

	for (i = 0; i < offset; i+=randBit)
	{
		sprintf(ptrCurPos, "%010d", GetRand());
		if (offset - i < (int)randBit)
			ptrCurPos += (offset - i);
		else
			ptrCurPos += randBit;
	}

	memcpy(ptrCurPos, macID, MAXSIZE_MAC);
	ptrCurPos += MAXSIZE_MAC;

    for (i = 0; i < offset; i+=randBit)
    {
        sprintf(ptrCurPos, "%010d", GetRand());
        if (offset - i < (int)randBit)
            ptrCurPos += (offset - i);
        else
            ptrCurPos += randBit;
    }
    memcpy(ptrCurPos, value, MAXSIZE_VALUE);
    ptrCurPos += MAXSIZE_VALUE;

    sprintf(ptrCurPos, "%010d", GetRand());
}

unsigned int CManRegister::GetRand()
{
	unsigned int randNumber;
	if(m_seed == 0)
	{
		srand((int)time(0));
		m_seed = 1;
	}
	randNumber = (unsigned int)rand();
	return randNumber;
}

unsigned int CManRegister::GetSecond()
{
	time_t getTime;
    struct tm *p;
	time(&getTime);
    p = localtime(&getTime);
	return p->tm_sec;
}

