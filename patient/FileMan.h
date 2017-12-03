#ifndef __FILEMAN_H__
#define __FILEMAN_H__

using std::string;

class FileMan
{
	public:
		int CheckDir(const char* absPath);

		int CheckFileName(const char *dest);

		int CopyFile(const char *source, const char *dest, bool del=false);
		//int CopyFileProgress(const char *source, const char *dest);

		int ShellCopyFile(const char* source, const char* dest);

		int CopyDirectory(const char* source, const char* dest);

		int DelDirectory(const char* absPath);

		bool CheckFreeSpace(const char* path, unsigned long size);

		char CompareSuffix(const char* filename, const char* suffix);

		int CountFileBySuffix(const char* directory, const char* suffix);

		//unsigned char CheckFileName(unsigned int no, const char* filename, const char* filepath, char* absPath);

		void GetIniFilePath(const char* absPath, char* absIniPath);
		void GetFrmFilePath(const char* absPath, char* absFrmPath);

		static bool SortByName(const string s1, const string s2);
		static bool SortByMTime(const string s1, const string s2);
		static bool SortByATime(const string s1, const string s2);
		static bool SortByCTime(const string s1, const string s2);
	
	private:
		int copy_dir(const char* source, const char* dest);
		int delete_dir(const char* absPath);
};

#endif
