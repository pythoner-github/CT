#ifndef __IMGMAN_H__
#define __IMGMAN_H__

#include <vector>
#include <list>
#include <string>
#include "IniFile.h"
#include "FileMan.h"
#include "ScanMode.h"
#include "DCMMan.h"

using std::vector;
using std::list;
using std::string;

class ImgMan: public FileMan
{
	public:
		~ImgMan();

		enum ImgFormat{BMP, JPEG, EMP, FRM};
		enum ImgStorage{HOST, UDISK};
		enum ImgNameMode{MANUAL, AUTO};

		struct ImgItem
		{	
			///> basic info for saving to file
			//char path[256];	///< the image file name (only use for get the file path when read snap and the file format is bmp or jpeg) 
			GdkPixbuf *pixbuf;	///< output para: pixbuf when read snap, and you must free it
			unsigned char *data;	///< intput para: image data
			///> extend info for config file
			int width;	///< width of image 
			int height;	///< height of image 

			ScanMode::SpecialMeasurePara para;
		};

		static ImgMan* GetInstance();

		list<string> m_listName;

		///> general member
		void SetImgFormat(int format) {m_Format = format;}
	//	void SetImgStorage(int storage) {m_Storage = storage;}
		void SetImgNameMode(int mode) {m_NameMode = mode;}
		int GetImgFormat() {return m_Format;}
		int GetImgStorage() {return m_Storage;}
		int GetImgNameMode() {return m_NameMode;}
	//	void SetImgItem(struct ImgItem* item) {m_ptrImgItem = item;}

		unsigned char SaveSnap(unsigned int no, const char* filename, const char* filepath, struct ImgItem* item);
		unsigned char DeleteSnap(unsigned int no, const char* filename, const char* filepath);
		unsigned char DeleteSnap(const char* absPath);
		unsigned char ReadSnap(unsigned int no, const char* filename, const char*filepath, struct ImgItem* item);
		unsigned char ReadSnap(const char* absPath, struct ImgItem* item);
		unsigned char LoadSnap(unsigned int no, const char* filepath, vector<string> *vec);
        unsigned char LoadSnap(const char* filepath, vector<string> *vec);

        unsigned char SaveSnapForRetrieve(unsigned int no, const char* filename, const char* filepath, struct ImgItem* item);
        DCMIMAGEELEMENT GetImageElement(void);
    private:
		ImgMan();

		unsigned char SaveIniFile(const char *absPath);
		bool SaveImageToEMP(const char* absPath);
		bool SaveImageToBMP(const char* absPath);
		bool SaveImageToJPEG(const char* absPath);

		void WriteConfigPara(ImgItem *ptrItem, IniFile* ptrIni, string section=SECTION);
		void ReadConfigPara(ImgItem *ptrItem, IniFile* ptrIni, string section=SECTION);

		///> extern member
		static ImgMan* m_ptrInstance;
		static struct ImgItem* m_ptrImgItem;

		static const int MAX_SNAP = 64;		///< max image num
		static const string SECTION;

		int m_Format;	///< image format
		int m_Storage;	///< storage type
		int m_NameMode;	///< name mode
	//	int m_TotalSnap;	///< total image saved
	//	int m_CurSnap;		///< current num 
        string m_imgFileName;
};

#endif //__IMGMAN_H__

