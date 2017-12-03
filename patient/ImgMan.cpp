#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "Def.h"
#include "gui_global.h"
#include "ImgMan.h"
#include "IniFile.h"
#include "CreateJpeg.h"
#include "CreateBitmap.h"
#include "CreateEmp.h"
#include "HintArea.h"
#include "SysOptions.h"
#include "ImageArea.h"
#include "DCMMan.h"
#include "ViewArchive.h"
#include "ViewMain.h"

///> /////////////////////////////////[private variable]////////////////////////////
ImgMan* ImgMan::m_ptrInstance = NULL;
struct ImgMan::ImgItem* ImgMan::m_ptrImgItem = NULL;

///> /////////////////////////////////[public func]////////////////////////////

const string ImgMan::SECTION = "Image";

ImgMan* ImgMan::GetInstance()
{
	if(m_ptrInstance == NULL)
	{
		m_ptrInstance = new ImgMan;
	}

	return m_ptrInstance;
}

///> construct
ImgMan::ImgMan()
{
	SysOptions so;
	m_Format = so.GetImageFormat();
//	m_Storage = so.GetImageMedia();
	m_Storage = HOST;
	m_NameMode = so.GetImageAutoName();
	m_listName.clear();
}

ImgMan::~ImgMan()
{
	if (m_ptrInstance != NULL)
		delete m_ptrInstance;
}

/*
 * @brief Check and save the config file by image file
 *        If the config file can not save, the image file also can not to save 
 * @param absPath: the absolute path of the image file where to save
 * 
 * @retval 0: success to save the config file
 *         1: failed to save the config file, it is existed
 */
unsigned char ImgMan::SaveIniFile(const char *absPath)
{
	char absIniPath[256];

	GetIniFilePath(absPath, absIniPath);

	if(CheckFileName(absIniPath)==1)
	{
		char hint[255];
		sprintf(hint, "%s", _("Fail to save. The file name is already existed, please input another file name!"));
		HintArea::GetInstance()->UpdateHint(hint, 2);
		return 1;
	}

	int fd = open(absIniPath, O_CREAT, 0666);
	close(fd);
	IniFile ini(absIniPath);
	WriteConfigPara(m_ptrImgItem, &ini, SECTION);

	return 0;
}

/*
 * @brief save snap to the directory for current patient
 *
 * @param no: user id
 * @param filename: file name without suffix (eg: 200907011200)
 * @param filepath: file path (eg: STORE_PATH)
 * 
 * @retval 0: success
 *		   1: no enough space
 *		   2: filename existed
 *		   3: error with save image file
 *		   4: the directory not existed
 */
unsigned char ImgMan::SaveSnap(unsigned int no, const char* filename, const char* filepath, struct ImgItem* item)
{
	unsigned long size = 0;
	char absPath[256];
	bool ret = false;
	int total = m_listName.size();
	char hint[255];

	m_ptrImgItem = item;

	//check the disk free space
	if(!CheckFreeSpace(filepath, size))
	{
		sprintf(hint, "%s", _("Fail to save. No enough space!"));
		HintArea::GetInstance()->UpdateHint(hint, 2);
		return 1;	//no enough space
	}

	sprintf(absPath, "%s/%d/%s", filepath, no, filename);

	if(CheckDir(absPath) == 0)
		return 4;

    string s(filename);
    char imgFileNameTmp[256];
    sprintf(imgFileNameTmp,"%s",filename);
	PRINTF("m_format = %d\n", m_Format);	
	switch(m_Format)
	{
		case FRM:	//for measure
			strcat(absPath, ".frm");
			ret = SaveImageToEMP(absPath);
		//	ret = SaveImageToJPEG(absPath);
		//	strcat(absPath, ".frmjpg");	//For Test
		//	ret = SaveImageToJPEG(absPath);	//For Test
			break;
		case EMP:
            strcat(absPath, ".emp");
            strcat(imgFileNameTmp, ".emp");
            if(SaveIniFile(absPath))
                return 2;
			ret = SaveImageToEMP(absPath);
			break;
		case BMP:
			strcat(absPath, ".bmp");
            strcat(imgFileNameTmp, ".bmp");
			if(SaveIniFile(absPath))
				return 2;
			ret = SaveImageToBMP(absPath);
			break;
		case JPEG:
			strcat(absPath, ".jpg");
            strcat(imgFileNameTmp, ".jpg");
			if(SaveIniFile(absPath))
				return 2;
			ret = SaveImageToJPEG(absPath);
			break;
		default:
			strcat(absPath, ".bmp");
            strcat(imgFileNameTmp, ".bmp");
			if(SaveIniFile(absPath))
				return 2;
			ret = SaveImageToBMP(absPath);
			break;
	}
	if(!ret)
		return 3;

	if(m_Format != FRM)
	{
		string s(filename);
		if(total >= MAX_SNAP)
		{
			DeleteSnap(no, m_listName.front().c_str(), filepath);
			m_listName.pop_front();
		}
		m_listName.push_back(s);

        sprintf(hint, "%s\"%s\".", _("Success to save image. Filename is "), filename);
        HintArea::GetInstance()->UpdateHint(hint, 2);

        m_imgFileName = (string)imgFileNameTmp;
        CDCMMan::GetMe()->AddImage(GetImageElement());
    }

	return 0;
}

unsigned char ImgMan::SaveSnapForRetrieve(unsigned int no, const char* filename, const char* filepath, struct ImgItem* item)
{
	unsigned long size = 0;
	char absPath[256];
	bool ret = false;
	int total = m_listName.size();
	char hint[255];

	m_ptrImgItem = item;

	//check the disk free space
	if(!CheckFreeSpace(filepath, size))
	{
		return 1;	//no enough space
	}

	sprintf(absPath, "%s/%d/%s", filepath, no, filename);

	if(CheckDir(absPath) == 0)
		return 4;

    string s(filename);
    char imgFileNameTmp[256];
    sprintf(imgFileNameTmp,"%s",filename);
	switch(m_Format)
	{
		case FRM:	//for measure
			strcat(absPath, ".frm");
			ret = SaveImageToEMP(absPath);
			break;
		case EMP:
            strcat(absPath, ".emp");
            strcat(imgFileNameTmp, ".emp");
            if(SaveIniFile(absPath))
                return 2;
			ret = SaveImageToEMP(absPath);
			break;
		case BMP:
			strcat(absPath, ".bmp");
            strcat(imgFileNameTmp, ".bmp");
			if(SaveIniFile(absPath))
				return 2;
			ret = SaveImageToBMP(absPath);
			break;
		case JPEG:
			strcat(absPath, ".jpg");
            strcat(imgFileNameTmp, ".jpg");
			if(SaveIniFile(absPath))
				return 2;
			ret = SaveImageToJPEG(absPath);
			break;
		default:
			strcat(absPath, ".bmp");
            strcat(imgFileNameTmp, ".bmp");
			if(SaveIniFile(absPath))
				return 2;
			ret = SaveImageToBMP(absPath);
			break;
	}
	if(!ret)
		return 3;

	if(m_Format != FRM)
	{
		if(total >= MAX_SNAP)
		{
			DeleteSnap(no, m_listName.front().c_str(), filepath);
			m_listName.pop_front();
		}
        m_listName.push_back(s);
    }
    filename = imgFileNameTmp;
	return 0;
}

/*
 * @brief delete image file and ini file matched
 *
 * @param the same as SaveSnap
 *
 * @retval 0: success
 *		   1: error with CheckFileName
 *		   2: error with delete image file 
 *		   3: error with delete image ini file 
 *		   4: error with delete image frm file 
 */
unsigned char ImgMan::DeleteSnap(unsigned int no, const char* filename, const char* filepath)
{
	char absPath[256];
	char absIniPath[256];
	char absFrmPath[256];

	sprintf(absPath, "%s/%d/%s", filepath, no, filename);
	if(CheckFileName(absPath) != 1)
		return 1;

	GetIniFilePath(absPath, absIniPath);
	GetFrmFilePath(absPath, absFrmPath);
		
	if(unlink(absPath))
	{
		perror("Delete Image File Error:");
		return 2;
	}
	if(unlink(absIniPath))
	{
		perror("Delete Image Ini File Error:");
		return 3;
	}
	if(unlink(absFrmPath))
	{
		perror("Delete Image Frm File Error:");
		return 3;
	}
	
	return 0;
}

unsigned char ImgMan::DeleteSnap(const char* absPath)
{
	char absIniPath[256];
	char absFrmPath[256];

	if(CheckFileName(absPath) != 1)
		return 1;

	GetIniFilePath(absPath, absIniPath);
	GetFrmFilePath(absPath, absFrmPath);

	if(unlink(absPath))
	{
		perror("Delete Image File Error:");
		return 2;
	}
	if(unlink(absIniPath))
	{
		perror("Delete Image Ini File Error:");
		return 3;
	}
	if(unlink(absFrmPath))
	{
		perror("Delete Image Frm File Error:");
		return 4;
	}
	
	return 0;
}

/*
 * @brief load all image filename to vector by patient's id
 *
 * @param no: patient id
 * @param filepath: file path (eg: STORE_PATH)
 * @param vec(out): vector which to store the filename (only file name, path is not included)
 *
 * @retval 0: success
 *		   1: can not open the dir
 *		   2: the path is empty
 */
unsigned char ImgMan::LoadSnap(unsigned int no, const char* filepath, vector<string> *vec)
{
	char *path;
	char tmp[10];
	string str;
	const char *name;
	char NameIni[256];
	char absNameIni[256];
	GDir *dir;
	GError *err = NULL;

//	if(vec->size() != 0)
//		vec->clear();

	sprintf(tmp, "%d", no);
	path = g_build_filename(filepath, tmp, NULL);
	dir = g_dir_open(path, 0, &err);
	g_free(path);
	if(!dir)
	{
		PRINTF("%s: g_dir_open with error: %s\n", __FUNCTION__, err->message);
		return 1;
	}

	name = g_dir_read_name(dir);
	if(!name)
	{
		PRINTF("%s: the dir is null!\n", __FUNCTION__);
		g_dir_close(dir);
		return 2; 
	}

	while(name != NULL)
	{
		//PRINTF("name is %s\n", name);
		if(!CompareSuffix(name, "bmp") || !CompareSuffix(name, "emp") || !CompareSuffix(name, "jpg"))
		{
			GetIniFilePath(name, NameIni);
			sprintf(absNameIni, "%s/%d/%s", filepath, no, NameIni);
			if(CheckFileName(absNameIni) != 0)
			{
				str = name;
				vec->push_back(str);
			}
#if 1
			else
			{
				path = g_build_filename(filepath, tmp, name, NULL);
				unlink(path);
				g_free(path);
			}
#endif
		}	
		name = g_dir_read_name(dir);
	}

	g_dir_close(dir);
	return 0;
}

/*
 * @brief load all image filename to vector by path(without patient's id)
 *
 * @param filepath: file path (eg: STORE_PATH)
 * @param vec(out): vector which to store the filename (only file name, path is not included)
 *
 * @retval 0: success
 *		   1: can not open the dir
 *		   2: the path is empty
 */
unsigned char ImgMan::LoadSnap(const char* filepath, vector<string> *vec)
{
	string str;
	const char *name;
	char NameIni[256];
	char absNameIni[256];
	GDir *dir;
	GError *err = NULL;

//	if(vec->size() != 0)
//		vec->clear();

	dir = g_dir_open(filepath, 0, &err);
	if(!dir)
	{
		PRINTF("%s: g_dir_open with error: %s\n", __FUNCTION__, err->message);
		return 1;
	}

	name = g_dir_read_name(dir);
	if(!name)
	{
		PRINTF("%s: the dir is null!\n", __FUNCTION__);
		g_dir_close(dir);
		return 2; 
	}

	while(name != NULL)
	{
		//PRINTF("name is %s\n", name);
		if(!CompareSuffix(name, "bmp") || !CompareSuffix(name, "emp") || !CompareSuffix(name, "jpg"))
		{
			GetIniFilePath(name, NameIni);
			sprintf(absNameIni, "%s/%s", filepath, NameIni);
			if(CheckFileName(absNameIni) != 0)
			{
				str = name;
				vec->push_back(str);
			}
			else
			{
				unlink(filepath);
			}
		}	
		name = g_dir_read_name(dir);
	}

	g_dir_close(dir);
	return 0;
}

/*
 * @brief read snap from the specify directory by user id ,and return the image para and pixbuf
 *  (ATTN: The item->pixbuf must to free!)
 *
 * @param no: patient id
 * @param filename: file name without suffix (eg: 200907011200)
 * @param filepath: file path (eg: STORE_PATH)
 * @param item(out): ImgItem
 * 
 * @retval 0: success
 *		   1: file not existed
 *		   2: failed to read the file, or the file is damaged
 */
unsigned char ImgMan::ReadSnap(unsigned int no, const char* filename, const char* filepath, struct ImgItem* item)
{
	int i, j;
	char suffix[10];
	char absPath[256];
	char absIniPath[256];
	int len = strlen(filename);
	
	//check the file name
	sprintf(absPath, "%s/%d/%s", filepath, no, filename);
	if(CheckFileName(absPath) != 1)
		return 1;
	
	// check the suffix of filename
	for(i=0; i<len; i++)
	{
		if(filename[i]=='.')
			break;
	}
	for(j=i+1; j<len; j++)
		suffix[j-i-1] = filename[j];
	suffix[j-i-1] = '\0';

	//get config file path
	GetIniFilePath(absPath, absIniPath);
	IniFile ini(absIniPath);

	ReadConfigPara(item, &ini, SECTION);
	if(!strcmp(suffix, "frm"))
		item->height = IMG_AREA_H;

	if(!strcmp(suffix, "jpg") || !strcmp(suffix, "bmp"))
	{
		GError *err = NULL;
		item->pixbuf = gdk_pixbuf_new_from_file(absPath, &err);
		if(!item->pixbuf)
		{
			PRINTF("gdk_pixbuf_new_from_file error: %s\n", err->message);
			g_error_free(err);
			return 2;
		}
	}
	else if(!strcmp(suffix, "emp") || !strcmp(suffix, "frm"))
	{
		unsigned char buf[item->width * item->height * 3];
		if(!ReadImageEmp(absPath, buf, item->width, item->height))
			return 2;
		else
		{
			item->pixbuf = gdk_pixbuf_new_from_data(buf, 
					GDK_COLORSPACE_RGB, 
					false, 8, 
					item->width, item->height, 
					item->width*3, 
					NULL, NULL);
		}
	}

	return 0;
}

/*
 * @brief read snap from the absolute path, and return the image para and pixbuf
 *  (ATTN: The item->pixbuf must to free!)
 *
 * @param absPath: absolute path of file
 * @param item(out): ImgItem
 * 
 * @retval 0: success
 *		   1: file not existed 
 *		   2: failed to read the file, or the file is damaged
 */
unsigned char ImgMan::ReadSnap(const char* absPath, struct ImgItem* item)
{
	int i, j;
	char suffix[10];
	char absIniPath[256];
	char* basename = g_path_get_basename(absPath);

	//check the file name
	if(CheckFileName(absPath) != 1)
		return 1;
	
	// check the suffix of filename
	int len = strlen(basename);
	for(i=0; i<len; i++)
	{
		if(basename[i]=='.')
			break;
	}
	for(j=i+1; j<len; j++)
		suffix[j-i-1] = basename[j];
	suffix[j-i-1] = '\0';
//	PRINTF("suffix is %s\n", suffix);
	free(basename);

	//get config file path
	GetIniFilePath(absPath, absIniPath);
	IniFile ini(absIniPath);

	ReadConfigPara(item, &ini, SECTION);
	if(!strcmp(suffix, "frm"))
		item->height = IMG_AREA_H;

	GError *err = NULL;
	if(!strcmp(suffix, "jpg") || !strcmp(suffix, "bmp"))
	{
		item->pixbuf = gdk_pixbuf_new_from_file(absPath, &err);
		if(!item->pixbuf)
		{
			PRINTF("gdk_pixbuf_new_from_file error: %s\n", err->message);
			g_error_free(err);
			return 2;
		}
	}
	else if(!strcmp(suffix, "emp") || !strcmp(suffix, "frm"))
	{
		unsigned char buf[item->width * item->height * 3];
		if(!ReadImageEmp(absPath, buf, item->width, item->height))
			return 2;
		else
		{
			GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(buf, 
					GDK_COLORSPACE_RGB, 
					false, 8, 
					item->width, item->height, 
					item->width*3, 
					NULL, NULL);
			item->pixbuf = gdk_pixbuf_copy(pixbuf);
			g_object_unref(pixbuf);
#if 0
			if(strcmp(suffix, "emp")==0)
			{
				guchar *pixels = gdk_pixbuf_get_pixels(item->pixbuf);
				int rowstride = gdk_pixbuf_get_rowstride(item->pixbuf);
				int width = gdk_pixbuf_get_width(item->pixbuf);
				int height = gdk_pixbuf_get_height(item->pixbuf);
				PRINTF("data:              width = %d, height=%d, rowstride=%d\n", width, height, rowstride);
				guchar *p = pixels + rowstride*height;
				for(int i=0; i<rowstride*2; i++)
				{
					PRINTF("%d ", *(p-i));
					if(i==rowstride-1)
						PRINTF("\n");
				}
				PRINTF("\n");
			}
#endif
		}
	}
	return 0;
}

///> /////////////////////////////////[private func]////////////////////////////

/*
 * @brief save image data to file with EMP
 *
 * @param absPath: the absolute path to save
 */
bool ImgMan::SaveImageToEMP(const char* absPath)
{
#if 1
	FILE* file = fopen(absPath, "wb");
	if(!file)
	{
		PRINTF("(SaveImageToEMP) fail to open file, the error is %s!\n", strerror(errno));
		return false;
	}
	CreateImageEmp(m_ptrImgItem->data, file, m_ptrImgItem->width, m_ptrImgItem->height);
	fclose(file);
#endif
#if 0
	int fd = open(absPath, O_WRONLY|O_CREAT);
	write(fd, m_ptrImgItem->data, m_ptrImgItem->width*m_ptrImgItem->height*3);
	close(fd);
#endif

	return true;
}

/*
 * @brief save image data to file with bmp
 *
 * @param absPath: the absolute path to save
 */
bool ImgMan::SaveImageToBMP(const char* absPath)
{
	int fd = open(absPath, O_CREAT|O_WRONLY|O_TRUNC, 0777);
	if(fd<0)
	{
		PRINTF("(SaveImageToBMP) Fail to open file, the error is %s!\n", strerror(errno));
		return false;
	}
	CreateBitmap24(m_ptrImgItem->data, fd, m_ptrImgItem->width, m_ptrImgItem->height);
	close(fd);
	return true;
}

/*
 * @brief save image data to file  jpeg
 *
 * @param absPath: the absolute path to save
 */
bool ImgMan::SaveImageToJPEG(const char* absPath)
{
	FILE* file = fopen(absPath, "wb");
	if(!file)
	{
		PRINTF("(SaveImageToJPEG) Fail to open file, the error is %s!\n", strerror(errno));
		return false;
	}
	CreateJpeg(m_ptrImgItem->data, file, 100, m_ptrImgItem->width, m_ptrImgItem->height);
	fclose(file);
	return true;
}

/*
 * @brief write image information to ini file
 *
 * @param ptrItem: the information to be written
 * @param section: item name
 * @param ptrIni: which file will be written
 */
void ImgMan::WriteConfigPara(ImgItem *ptrItem, IniFile* ptrIni, string section)
{
	const char* ptrSection = section.c_str();

	ptrIni->WriteInt(ptrSection, "Image-Width", ptrItem->width);
	ptrIni->WriteInt(ptrSection, "Image-Height", ptrItem->height);
	ptrIni->WriteInt(ptrSection, "Image-Mode", ptrItem->para.mode);
	ptrIni->WriteInt(ptrSection, "Image-Format2D", ptrItem->para.format2D);
	ptrIni->WriteInt(ptrSection, "Image-FormatCfm", ptrItem->para.formatCfm);
	ptrIni->WriteInt(ptrSection, "Image-FormatPw", ptrItem->para.formatPw);
	ptrIni->WriteInt(ptrSection, "Image-FormatM", ptrItem->para.formatM);
	ptrIni->WriteDouble(ptrSection, "Image-Scale2D", ptrItem->para.scale2D);
	ptrIni->WriteDouble(ptrSection, "Image-Scale1", ptrItem->para.scale2DAll[0]);
	ptrIni->WriteDouble(ptrSection, "Image-Scale2", ptrItem->para.scale2DAll[1]);
	ptrIni->WriteDouble(ptrSection, "Image-Scale3", ptrItem->para.scale2DAll[2]);
	ptrIni->WriteDouble(ptrSection, "Image-Scale4", ptrItem->para.scale2DAll[3]);
	ptrIni->WriteDouble(ptrSection, "Image-ScaleMDepth", ptrItem->para.scaleMDepth);
	ptrIni->WriteDouble(ptrSection, "Image-ScaleMTime", ptrItem->para.scaleMTime);
	ptrIni->WriteDouble(ptrSection, "Image-ScalePwVel", ptrItem->para.scalePwVel);
	ptrIni->WriteDouble(ptrSection, "Image-ScalePwTime", ptrItem->para.scalePwTime);
	ptrIni->WriteInt(ptrSection, "Image-BaselineCalc", ptrItem->para.baselineCalc);
	ptrIni->SyncConfigFile();
}

/*
 * @brief read image information from ini file
 *
 * @param ptrItem: the information will be read 
 * @param section: item name
 * @param ptrIni: which file will be read 
 */
void ImgMan::ReadConfigPara(ImgItem *ptrItem, IniFile* ptrIni, string section)
{
	const char* ptrSection = section.c_str();

	ptrItem->width = ptrIni->ReadInt(ptrSection, "Image-Width");
	ptrItem->height = ptrIni->ReadInt(ptrSection, "Image-Height");
	ptrItem->para.mode = ScanMode::EScanMode(ptrIni->ReadInt(ptrSection, "Image-Mode"));
	ptrItem->para.format2D = Format2D::EFormat2D(ptrIni->ReadInt(ptrSection, "Image-Format2D"));
	ptrItem->para.formatCfm = FormatCfm::EFormatCfm(ptrIni->ReadInt(ptrSection, "Image-FormatCfm"));
	ptrItem->para.formatPw = FormatPw::EFormatPw(ptrIni->ReadInt(ptrSection, "Image-FormatPw"));
	ptrItem->para.formatM = FormatM::EFormatM(ptrIni->ReadInt(ptrSection, "Image-FormatM"));
	ptrItem->para.scale2D = ptrIni->ReadDouble(ptrSection, "Image-Scale2D");
	ptrItem->para.scale2DAll[0] = ptrIni->ReadDouble(ptrSection, "Image-Scale1");
	ptrItem->para.scale2DAll[1] = ptrIni->ReadDouble(ptrSection, "Image-Scale2");
	ptrItem->para.scale2DAll[2] = ptrIni->ReadDouble(ptrSection, "Image-Scale3");
	ptrItem->para.scale2DAll[3] = ptrIni->ReadDouble(ptrSection, "Image-Scale4");
	ptrItem->para.scaleMDepth = ptrIni->ReadDouble(ptrSection, "Image-ScaleMDepth");
	ptrItem->para.scaleMTime = ptrIni->ReadDouble(ptrSection, "Image-ScaleMTime");
	ptrItem->para.scalePwTime = ptrIni->ReadDouble(ptrSection, "Image-ScalePwTime");
	ptrItem->para.scalePwVel = ptrIni->ReadDouble(ptrSection, "Image-ScalePwVel");
	ptrItem->para.baselineCalc = ptrIni->ReadInt(ptrSection, "Image-BaselineCalc");
}

DCMIMAGEELEMENT ImgMan::GetImageElement(void)
{
    DCMIMAGEELEMENT Img;
    Img.imgHeight = m_ptrImgItem->height;
    Img.imgWidth = m_ptrImgItem->width;
    Img.imgBitCount = 24;
    Img.imgFrameNumber = 1;
    Img.imgFilename = m_imgFileName;
    Img.imgCalibrationRegion.left = IMAGE_X;
    Img.imgCalibrationRegion.top = IMAGE_Y + IMG_AREA_Y;
    Img.imgCalibrationRegion.right = IMAGE_X + IMAGE_W;
    Img.imgCalibrationRegion.bottom = IMAGE_Y +IMG_AREA_Y +IMAGE_H;
    Img.imgScale = Img2D::GetInstance()->GetScale2D()/10;
    Img.imgStudyPart = ViewArchive::GetInstance()->GetExamType();
    PRINTF("imgHeigth=%d",Img.imgHeight);
    PRINTF("imgwidth=%d",Img.imgWidth);
    PRINTF("imgbitcount=%d",Img.imgBitCount);
    PRINTF("imgframenumber=%d",Img.imgFrameNumber);
    PRINTF("imgFilename=%s",Img.imgFilename.c_str());
    PRINTF("imgcalibration.left=%d",Img.imgCalibrationRegion.left);
    PRINTF("imgcalibration.top=%d",Img.imgCalibrationRegion.top);
    PRINTF("imgcalibration.right=%d",Img.imgCalibrationRegion.right); 
    PRINTF("imgcalibration.bottom=%d",Img.imgCalibrationRegion.bottom);
    PRINTF("imgscale=%f",Img.imgScale);
    PRINTF("imgstudypart=%s",Img.imgStudyPart.c_str());
 
 return Img;
}
