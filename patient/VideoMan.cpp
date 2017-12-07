#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <dirent.h>
#include <dirent.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "Def.h"
#include "display/gui_global.h"
#include "imageProc/Replay.h"
#include "patient/VideoMan.h"
#include "base/IniFile.h"
#include "patient/CreateAvi.h"
#include "patient/CreateEmp.h"
#include "display/HintArea.h"
#include "sysMan/SysOptions.h"
#include "display/ImageArea.h"
#include "patient/ViewArchive.h"
#include "periDevice/DCMMan.h"
#include "imageControl/Img2D.h"
#include "ViewMain.h"
#include "patient/AviEncDecH264.h"
///> /////////////////////////////////[private variable]////////////////////////////
VideoMan* VideoMan::m_ptrInstance = NULL;
struct VideoMan::VideoItem* VideoMan::m_ptrVideoItem = NULL;

///> /////////////////////////////////[public func]////////////////////////////

const string VideoMan::SECTION = "Video";

VideoMan* VideoMan::GetInstance()
{
    if(m_ptrInstance == NULL)
    {
        m_ptrInstance = new VideoMan;
    }

    return m_ptrInstance;
}

///> construct
VideoMan::VideoMan()
{
    SysOptions so;
    m_Format = so.GetCineFormat();
//  m_Storage = so.GetCineMedia();
    m_Storage = HOST;
    m_NameMode = so.GetCineAutoName();
    m_listName.clear();
}
VideoMan::~VideoMan()
{

}

/*
 * @brief Check and save the config file by video file
 *        If the config file can not save, the video file also can not to save
 * @param absPath: the absolute path of the video file where to save
 *
 * @retval 0: success to save the config file
 *         1: failed to save the config file, it is existed
 */
unsigned char VideoMan::SaveIniFile(const char *absPath)
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
    WriteConfigPara(m_ptrVideoItem, SECTION, &ini);

    return 0;
}
/*
 * @brief save snap to the directory for current patient
 *  (ATTN: you must call SetImgItem first, then call SaveSnap )
 *
 * @param no: user id
 * @param filename: file name without suffix (eg: 200907011200)
 * @param filepath: file path (eg: STORE_PATH)
 *
 * @retval 0: success
 *         1: no enough space
 *         2: filename existed
 *         3: error with save video file
 *         4: the directory not existed
 */
char VideoMan::SaveVideo(unsigned int no, const char* filename, const char* filepath, struct VideoItem* item)
{
    unsigned long size = 0;
    char absPath[256];
    bool ret = false;
    int total = m_listName.size();
    char hint[255];

    m_ptrVideoItem = item;

    //check the disk free space
    if(!CheckFreeSpace(filepath, size))
    {
        sprintf(hint, "%s", _("Fail to save. No enough space!"));
        HintArea::GetInstance()->UpdateHint(hint, 2);
        return 1;   //no enough space
    }

    sprintf(absPath, "%s/%d/%s", filepath, no, filename);

    if(CheckDir(absPath) == 0)
        return 4;

    char videoFileNameTmp[256];
    sprintf(videoFileNameTmp,"%s",filename);

    switch(m_Format)
    {
        case CINE:
            strcat(absPath, ".cine");
            strcat(videoFileNameTmp, ".cine");
            if(SaveIniFile(absPath))
                return 2;
            ret = SaveVideoToCine(absPath);
            break;
        case AVI:
            strcat(absPath, ".avi");
            strcat(videoFileNameTmp, ".avi");
            if(SaveIniFile(absPath))
                return 2;
            ret = SaveVideoToAVI(absPath);
            break;
        default:
            strcat(absPath, ".avi");
            strcat(videoFileNameTmp, ".avi");
            if(SaveIniFile(absPath))
                return 2;
            ret = SaveVideoToAVI(absPath);
            break;
    }
    if(!ret)
        return 3;

    string s(filename);
    if(total >= MAX_VIDEO)
    {
        DeleteVideo(no, m_listName.front().c_str(), filepath);
        m_listName.pop_front();
    }
    m_listName.push_back(s);

    sprintf(hint, "%s\"%s\".", _("Success to save cine. Filename is "), filename);
    HintArea::GetInstance()->UpdateHint(hint, 2);

    m_videoFileName = (string)videoFileNameTmp;
    CDCMMan::GetMe()->AddImage(GetImageElement());
    return 0;
}

char VideoMan::SaveVideoForRetrieve(unsigned int no, const char* filename, const char* filepath, struct VideoItem* item)
{
    unsigned long size = 0;
    char absPath[256];
    bool ret = false;
    char hint[255];

    m_ptrVideoItem = item;

    //check the disk free space
    if(!CheckFreeSpace(filepath, size))
    {
        sprintf(hint, "%s", _("Fail to save. No enough space!"));
        HintArea::GetInstance()->UpdateHint(hint, 2);
        return 1;   //no enough space
    }

    sprintf(absPath, "%s/%d/%s", filepath, no, filename);

    if(CheckDir(absPath) == 0)
        return 4;

    switch(m_Format)
    {
        case CINE:
            strcat(absPath, ".cine");
            if(SaveIniFile(absPath))
                return 2;
            ret = SaveVideoToCineForRetrieve(absPath);
            break;
        case AVI:
            strcat(absPath, ".avi");
            if(SaveIniFile(absPath))
                return 2;
            ret = SaveVideoToAVIForRetrieve(absPath);
            break;
        default:
            strcat(absPath, ".avi");
            if(SaveIniFile(absPath))
                return 2;
            ret = SaveVideoToAVIForRetrieve(absPath);
            break;
    }
    if(!ret)
        return 3;

    return 0;
}

/*
 * @brief delete image file and ini file matched
 *
 * @param the same as SaveSnap
 *
 * @retval 0: success
 *         1: error with CheckFileName
 *         2: error with delete image file
 *         3: error with delete image ini file
 */
char VideoMan::DeleteVideo(unsigned int no, const char* filename, const char* filepath)
{
    char absPath[256];
    char absIniPath[256];

    sprintf(absPath, "%s/%d/%s", filepath, no, filename);
    if(CheckFileName(absPath) != 1)
        return 1;
    GetIniFilePath(absPath, absIniPath);

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

    return 0;
}

char VideoMan::DeleteVideo(const char* absPath)
{
    char absIniPath[256];

    if(CheckFileName(absPath) != 1)
        return 1;

    GetIniFilePath(absPath, absIniPath);

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

    return 0;
}
/*
 * @brief load all video filename to vector by patient's id
 *
 * @param no: patient id
 * @param filepath: file path (eg: STORE_PATH)
 * @param vec(out): vector which to store the filename
 *
 * @retval 0: success
 *         1: can not open the dir
 *         2: the path is empty
 */
char VideoMan::LoadVideo(unsigned int no, const char* filepath, vector<string> *vec)
{
    char *path;
    char tmp[10];
    string str;
    const char *name;
    char NameIni[256];
    char absNameIni[256];
    GDir *dir;

   /* if(vec->size() != 0)
    {
        vec->clear();
    }
*/
    sprintf(tmp, "%d", no);
    path = g_build_filename(filepath, tmp, NULL);
    dir = g_dir_open(path, 0, NULL);
    g_free(path);
    if(!dir)
    {
        PRINTF("Error: Cannot open the dir '%s'\n", path);
        return 1;
    }

    name = g_dir_read_name(dir);
    if(!name)
    {
        PRINTF("Error: The dir '%s' is empty!\n", path);
        g_dir_close(dir);
        return 2;
    }

    while(name != NULL)
    {
        if(!CompareSuffix(name, "avi") || !CompareSuffix(name, "cine"))
        {
            GetIniFilePath(name, NameIni);
            sprintf(absNameIni, "%s/%d/%s", filepath, no, NameIni);
            if(CheckFileName(absNameIni) != 0)
            {
                str = name;
                vec->push_back(str);
            }
            else
            {
                path = g_build_filename(filepath, tmp, name, NULL);
                unlink(path);
                g_free(path);
            }
        }

        name = g_dir_read_name(dir);
    }

    g_dir_close(dir);
    return 0;
}

/*
 * @brief load all video filename to vector by path(without patient's id)
 *
 * @param filepath: file path (eg: STORE_PATH)
 * @param vec(out): vector which to store the filename
 *
 * @retval 0: success
 *         1: can not open the dir
 *         2: the path is empty
 */
char VideoMan::LoadVideo(const char* filepath, vector<string> *vec)
{
    string str;
    const char *name;
    char NameIni[256];
    char absNameIni[256];
    GDir *dir;
    GError *err = NULL;

//  if(vec->size() != 0)
//      vec->clear();

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
        //  PRINTF("name is %s\n", name);
        if(!CompareSuffix(name, "avi") || !CompareSuffix(name, "cine"))
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
 * @brief read video info(absolute path, the first frame data, format, frames, frame_rate, width, height, ) by the specify directory by user id ,and return the image para
 *  (ATTN: If success return, you must to free the memory of the item->data pointer)
 *
 * @param no: user id
 * @param filename: file name without suffix (eg: 200907011200)
 * @param filepath: file path (eg: STORE_PATH)
 * @param item(out): ImgItem
 *
 * @retval 0: success
 *         1: file not existed
 *         2: file format is wrong
 *         3: error with read the first frame data
 */
char VideoMan::ReadVideoInfo(unsigned int no, const char* filename, const char* filepath, struct VideoItem* item)
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

    strcpy(item->path, absPath);

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
    ReadConfigPara(item, SECTION, &ini);

    if(!strcmp(suffix, "avi"))
        item->format= AVI;
    else if(!strcmp(suffix, "cine"))
        item->format= CINE;
    else
        return 2;

    if (item->format == AVI)
    {
        unsigned char *buf = (unsigned char*)malloc(item->width * item->height * 3);
        if (buf != NULL)
        {
            if (!ReadAviData(item->path, buf, 0, item->width, item->height, NULL))
            {
                PRINTF("ReadAviData Error!\n");
                free(buf);
                return 3;
            }
            else
            {
                item->data = buf;
            }

        }
        else
        {
            return 3;
        }
    }
    else
    {
        FILE* fs = fopen(item->path, "rb");
        if(fs==NULL)
        {
            perror("fopen error!");
            return 3;
        }
        unsigned char *buf = (unsigned char*)malloc(item->width * item->height * 3);
        if(buf != NULL)
        {
            if(!ReadVideoData(fs, buf, item->format, 0, item->width, item->height))
            {
                PRINTF("ReadVideoData Error!\n");
                free(buf);
                fclose(fs);
                return 3;
            }
            else
                item->data = buf;
        }
        else
        {
            fclose(fs);
            return 3;
        }
        fclose(fs);
    }
    return 0;
}

char VideoMan::ReadVideoInfo(const char *absPath, struct VideoItem* item)
{
    int i, j;
    char suffix[10];
    char absIniPath[256];
    int len = strlen(absPath);

    //check the file name
    if(CheckFileName(absPath) != 1)
        return 1;

    strcpy(item->path, absPath);

    // check the suffix of filename
    for(i=0; i<len; i++)
    {
        if(absPath[i]=='.')
            break;
    }
    for(j=i+1; j<len; j++)
        suffix[j-i-1] = absPath[j];
    suffix[j-i-1] = '\0';

    //get config file path
    GetIniFilePath(absPath, absIniPath);
    IniFile ini(absIniPath);
    ReadConfigPara(item, SECTION, &ini);

    if(!strcmp(suffix, "avi"))
        item->format= AVI;
    else if(!strcmp(suffix, "cine"))
        item->format= CINE;
    else
        return 2;

    if (item->format == AVI)
    {
        unsigned char *buf = (unsigned char*)malloc(item->width * item->height * 3);
        if (buf != NULL)
        {
            if (!ReadAviData(item->path, buf, 0, item->width, item->height, NULL))
            {
                PRINTF("ReadAviData Error!\n");
                free(buf);
                return 3;
            }
            else
            {
                item->data = buf;
            }

        }
        else
        {
            return 3;
        }
    }
    else
    {

        FILE* fs = fopen(item->path, "rb");
        if(fs==NULL)
        {
            perror("fopen error!");
            return 3;
        }
        unsigned char *buf = (unsigned char*)malloc(item->width * item->height * 3);
        if(buf != NULL)
        {
            if(!ReadVideoData(fs, buf, item->format, 0, item->width, item->height))
            {
                PRINTF("ReadVideoData Error!\n");
                free(buf);
                fclose(fs);
                return 3;
            }
            else
                item->data = buf;
        }
        else
        {
            fclose(fs);
            return 3;
        }
        fclose(fs);
    }
    return 0;
}

/*
 * @brief read the appointed frame data to the buffer
 */
bool VideoMan::ReadVideoData(FILE *fs,unsigned char* buf, int format, int spec_frame, unsigned int width, unsigned int height)
{
    if(format == AVI)
    {
       PRINTF("read the avi\n");
        if(!ReadAvi(fs, buf, spec_frame, width, height))
            return false;
    }
    else if(format == CINE)
    {
       PRINTF("read the cine\n");
        if(!ReadCineEmp(fs, buf, spec_frame, width, height))
            return false;
    }
    return true;
}

///> /////////////////////////////////[private func]////////////////////////////

/*
 * @brief save image data to file with CINE
 *
 * @param absPath: the absolute path to save
 */
bool VideoMan::SaveVideoToCine(const char* absPath)
{
    PRINTF("%s: absPath=%s\n", __FUNCTION__, absPath);

    FILE* file = fopen(absPath, "wb");
    if(!file)
    {
        PRINTF("(SaveVideoToCine) Fail to open file, the error is %s!\n", strerror(errno));
        return false;
    }

    unsigned char* data[m_ptrVideoItem->frames];
    int i, j;
    for(j=0, i=m_ptrVideoItem->begin; i<(m_ptrVideoItem->begin + m_ptrVideoItem->frames); i++, j++)
    {
        data[j] = (unsigned char *)(m_ptrVideoItem->deq->at(i).GetImg());
    }
    CreateCineEmp(data, file, m_ptrVideoItem->frames, m_ptrVideoItem->width, m_ptrVideoItem->height);
        PRINTF("save videotocine frames = %d\n",m_ptrVideoItem->frames);
    fclose(file);
    return true;
}

bool VideoMan::SaveVideoToCineForRetrieve(const char* absPath)
{

    FILE* file = fopen(absPath, "wb");
    if(!file)
    {
        PRINTF("(SaveVideoToCine) Fail to open file, the error is %s!\n", strerror(errno));
        return false;
    }

    unsigned char* data[m_ptrVideoItem->frames];
    int i, j;
    for(j=0, i=m_ptrVideoItem->begin; i<(m_ptrVideoItem->begin + m_ptrVideoItem->frames); i++, j++)
    {
        data[j] = (unsigned char *)(m_ptrVideoItem->deq->at(i).GetImg());
    }
    CreateCineEmp(data, file, m_ptrVideoItem->frames, m_ptrVideoItem->width, m_ptrVideoItem->height);
    fclose(file);
    return true;
}

/*
 * @brief save image data to file  jpeg
 *
 * @param absPath: the absolute path to save
 */
bool VideoMan::SaveVideoToAVI(const char* absPath)
{
    PRINTF("absPath=%s\n",absPath);

    FILE* file = fopen(absPath, "wb");
    if(!file)
    {
        PRINTF("(SaveVideoToAVI) Fail to open file, the error is %s!\n", strerror(errno));
        return false;
    }

#if 0
    unsigned char* data[m_ptrVideoItem->deq->size()];
    int i, j;
    for(j=0, i=m_ptrVideoItem->begin; i< m_ptrVideoItem->deq->size(); i++, j++)
    {
        data[j] = (unsigned char *)(m_ptrVideoItem->deq->at(i).GetImg());
    //  PRINTF("frame%d=0x%x\n", j, data[i]);
    }
#endif
    unsigned char* data[m_ptrVideoItem->frames];
    int i, j;
    for(j=0, i=m_ptrVideoItem->begin; i<(m_ptrVideoItem->begin + m_ptrVideoItem->frames); i++, j++)
    {
        data[j] = (unsigned char *)(m_ptrVideoItem->deq->at(i).GetImg());
    }
    //CreateAvi(data, file, m_ptrVideoItem->width, m_ptrVideoItem->height, m_ptrVideoItem->frames, m_ptrVideoItem->frame_rate);//2012.05.26
    //CreateAvi(data, file, m_ptrVideoItem->width, m_ptrVideoItem->height, m_ptrVideoItem->frames, m_ptrVideoItem->frame_rate);
    CreateAviEncode(data, file, m_ptrVideoItem->width, m_ptrVideoItem->height, m_ptrVideoItem->frames, m_ptrVideoItem->frame_rate);

    fclose(file);
    return true;
}

bool VideoMan::SaveVideoToAVIForRetrieve(const char* absPath)
{

    FILE* file = fopen(absPath, "wb");
    if(!file)
    {
        PRINTF("(SaveVideoToAVI) Fail to open file, the error is %s!\n", strerror(errno));
        return false;
    }

    unsigned char* data[m_ptrVideoItem->frames];
    int i, j;
    for(j=0, i=m_ptrVideoItem->begin; i<(m_ptrVideoItem->begin + m_ptrVideoItem->frames); i++, j++)
    {
        data[j] = (unsigned char *)(m_ptrVideoItem->deq->at(i).GetImg());
    }
//  CreateAvi(data, file, m_ptrVideoItem->width, m_ptrVideoItem->height, m_ptrVideoItem->frames, m_ptrVideoItem->frame_rate);
    CreateAviEncode(data, file, m_ptrVideoItem->width, m_ptrVideoItem->height, m_ptrVideoItem->frames, m_ptrVideoItem->frame_rate);

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
void VideoMan::WriteConfigPara(VideoItem *ptrItem, string section, IniFile* ptrIni)
{
    const char* ptrSection = section.c_str();

    ptrIni->WriteInt(ptrSection, "Video-Width", ptrItem->width);
    ptrIni->WriteInt(ptrSection, "Video-Height", ptrItem->height);
    ptrIni->WriteInt(ptrSection, "Video-Frames", ptrItem->frames);
    ptrIni->WriteInt(ptrSection, "Video-FrameRate", ptrItem->frame_rate);
    ptrIni->SyncConfigFile();
#if 0
    ptrIni->WriteString(ptrSection, "Image-DepthL", ptrItem->depthL);
    ptrIni->WriteString(ptrSection, "Image-DepthR", ptrItem->depthR);
#endif
}

/*
 * @brief read image information from ini file
 *
 * @param ptrItem: the information will be read
 * @param section: item name
 * @param ptrIni: which file will be read
 */
void VideoMan::ReadConfigPara(VideoItem *ptrItem, string section, IniFile* ptrIni)
{
    const char* ptrSection = section.c_str();

    ptrItem->width = ptrIni->ReadInt(ptrSection, "Video-Width");
    ptrItem->height = ptrIni->ReadInt(ptrSection, "Video-Height");
    ptrItem->frames = ptrIni->ReadInt(ptrSection, "Video-Frames");
    ptrItem->frame_rate = ptrIni->ReadInt(ptrSection, "Video-FrameRate");
#if 0
    ptrItem->depthL = ptrIni->ReadInt(ptrSection, "Image-DepthL");
    ptrItem->depthR = ptrIni->ReadInt(ptrSection, "Image-DepthR");
#endif
}

char VideoMan::SaveEFOVVideo(unsigned int no, const char *filename, const char *filepath)
{
    unsigned long size = 0;
    char absPath[256];
    bool ret = false;
    int total = m_listName.size();
    char hint[255];

    //check the disk free space
    if(!CheckFreeSpace(filepath, size))
    {
        sprintf(hint, "%s", _("Fail to save. No enough space!"));
        HintArea::GetInstance()->UpdateHint(hint, 2);
        return 1;   //no enough space
    }

        //test
    sprintf(absPath, "%s/%d/%s", filepath, no, "EFOV");
    //sprintf(absPath, "%s/%d/%s", filepath, no, filename);

    if(CheckDir(absPath) == 0)
        return 4;

    strcat(absPath, ".efov");
//  if(SaveIniFile(absPath))
//      return 2;
    ret = SaveVideoToEFOVCine(absPath);
    if(!ret)
        return 3;

    string s(filename);
    if(total >= MAX_VIDEO)
    {
        DeleteEFOVVideo(no, m_listName.front().c_str(), filepath);
        m_listName.pop_front();
    }
    m_listName.push_back(s);

    sprintf(hint, "%s\"%s\".", _("Success to save eView cine. Filename is "), filename);
    HintArea::GetInstance()->UpdateHint(hint, 2);

    return 0;
}

bool VideoMan::SaveVideoToEFOVCine(const char* absPath)
{
    unsigned char *data;
    int size;
    Replay::GetInstance()->GetEFOVCineImg(&data, size);

    FILE* file = fopen(absPath, "wb");
    if(!file)
    {
        PRINTF("(SaveVideoToCine) Fail to open file, the error is %s!\n", strerror(errno));
        return false;
    }
    if(fwrite(data, sizeof(unsigned char), size, file) != (size_t)size)
    {
        perror("Failed to write video data, error:");
        return false;
    }

    fclose(file);
    return true;
}

char VideoMan::DeleteEFOVVideo(unsigned int no, const char* filename, const char* filepath)
{
    char absPath[256];
    char absIniPath[256];

    sprintf(absPath, "%s/%d/%s", filepath, no, filename);
    if(CheckFileName(absPath) != 1)
        return 1;
    GetIniFilePath(absPath, absIniPath);

    if(unlink(absPath))
    {
        perror("Delete Image File Error:");
        return 2;
    }
    return 0;
}

DCMIMAGEELEMENT VideoMan::GetImageElement(void)
{
    DCMIMAGEELEMENT Img;
    Img.imgHeight = m_ptrVideoItem->height;
    Img.imgWidth = m_ptrVideoItem->width;
    Img.imgBitCount = 24;
    Img.imgFrameNumber = m_ptrVideoItem->frames;
    Img.imgFilename = m_videoFileName;
    Img.imgCalibrationRegion.left = IMAGE_X;
    Img.imgCalibrationRegion.top = IMAGE_Y + IMG_AREA_Y;
    Img.imgCalibrationRegion.right = IMAGE_X + IMAGE_W;
    Img.imgCalibrationRegion.bottom = IMAGE_Y +IMG_AREA_Y +IMAGE_H;
    Img.imgScale = Img2D::GetInstance()->GetScale2D()/10;
    Img.imgStudyPart = ViewArchive::GetInstance()->GetExamType();

    return Img;
}

void VideoMan::SaveReplay(uint8_t *buf, int width, int height)
{
    // add image to replay memory
    Image::AutoTracePara para;
    Image image((unsigned int*)buf, para);
    Replay::GetInstance()->AddOneCineImg(image);
}