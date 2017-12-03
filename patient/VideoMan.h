#ifndef __VIDEOMAN_H__
#define __VIDEOMAN_H__

#include <deque>
#include <vector>
#include <list>
#include <string>
#include "Image.h"
#include "IniFile.h"
#include "FileMan.h"
#include "DCMMan.h"

using std::deque;
using std::vector;
using std::list;
using std::string;

class VideoMan: public FileMan
{
	public:
		///>
		~VideoMan();

		enum VideoFormat{AVI, CINE};
		enum VideoStorage{HOST, UDISK};
		enum VideoNameMode{MANUAL, AUTO};

		struct VideoItem
		{
			///> basic info for saving to file
			char path[256];	///< the video file name (only use for get the file path when read video and the file format is emp or avi) 
			VideoFormat format;	///< the type of the video (output)
			unsigned char *data;	///< frame data (output)

			deque<Image> *deq;	///< each frames in deque (input)
			int begin;	///< the start frame in deque
			int frames;	///< total number of frames
			int frame_rate;	///< the frame rate when playing, eg: 4
			///> extend info for config file
			int width;	///< width of frame 
			int height;	///< height of frame 
		//	unsigned int depthL;	///< if BB mode, express the probe depth of left image, if B mode, express the current probe depth
		//	unsigned int depthR;	///< if BB mode, express the probe depth of right image, if B mode, no effect
		};

		static VideoMan* GetInstance();

		list<string> m_listName;

		///> general member
		void SetVideoFormat(int format) {m_Format = format;}
	//	void SetVideoStorage(int storage) {m_Storage = storage;}
		void SetVideoNameMode(int mode) {m_NameMode = mode;}
		int GetVideoFormat() {return m_Format;}
		int GetVideoStorage() {return m_Storage;}
		int GetVideoNameMode() {return m_NameMode;}
	//	void SetVideoItem(struct VideoItem* item) {m_ptrVideoItem = item;}

		char SaveVideo(unsigned int no, const char* filename, const char* filepath, struct VideoItem* item);
		char DeleteVideo(unsigned int no, const char* filename, const char* filepath);
		char DeleteVideo(const char* absPath);
		char ReadVideoInfo(unsigned int no, const char* filename, const char*filepath, struct VideoItem* item);
		char ReadVideoInfo(const char *absPath, struct VideoItem* item);
		bool ReadVideoData(FILE *fs, unsigned char* buf, int tyep, int spec_frame, unsigned int width, unsigned int height);
        char LoadVideo(unsigned int no, const char* filepath, vector<string> *vec);
        char LoadVideo(const char* filepath, vector<string> *vec);

		char SaveEFOVVideo(unsigned int no, const char *filename, const char *filepath);
		bool SaveVideoToEFOVCine(const char* absPath);
		char DeleteEFOVVideo(unsigned int no, const char* filename, const char* filepath);
       static void SaveReplay(uint8_t *buf, int width, int height);
        char SaveVideoForRetrieve(unsigned int no, const char* filename, const char* filepath, struct VideoItem* item);
	private:
		VideoMan();

		unsigned char SaveIniFile(const char *absPath);
		bool SaveVideoToCine(const char*absPath);
		bool SaveVideoToAVI(const char* absPath);

        bool SaveVideoToCineForRetrieve(const char*absPath);
		bool SaveVideoToAVIForRetrieve(const char* absPath);


		void WriteConfigPara(VideoItem *ptrItem, string section, IniFile* ptrIni);
		void ReadConfigPara(VideoItem *ptrItem, string section, IniFile* ptrIni);

		///> extern member
		static VideoMan* m_ptrInstance;
		static struct VideoItem* m_ptrVideoItem;

		static const int MAX_VIDEO = 64;		///< max video num
		static const string SECTION;

		int m_Format;
		int m_Storage;	
		int m_NameMode;	
        string m_videoFileName;

        DCMIMAGEELEMENT GetImageElement(void);
};

#endif //__VIDEOMAN_H__

