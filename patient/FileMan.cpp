#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <errno.h>
#include <gtk/gtk.h>

#include "Def.h"
#include "FileMan.h"

/*
 * @brief check the free space in path
 *
 * @param path: the path which will be check
 * @param size: the size required
 *
 * @retval true: enough space was available for storing
 * @retval false: no enough space
 */
bool FileMan::CheckFreeSpace(const char* path, unsigned long size)
{
	struct statfs fs;
	
	if(statfs(path, &fs)==0)
	{
		if((double)fs.f_bsize*fs.f_bavail-size>0)
		{
			PRINTF("Enough space!\n");
			return TRUE;
		}
		else
		{
			PRINTF("No enought space!\n");
			return FALSE;
		}
	}
	else
	{
		PRINTF("Fail to get status of file system, the error is %s!\n", strerror(errno));
		return FALSE;
	}
}

#if 0
/*
 * @brief create directory
 *
 * @param path: the path of directory
 *
 * @retval 0: the directory is not existed, and failed to make it
 *         1: the directory is already existed
 *         2: the directroy is not existed, but success to make it
 */
int FileMan::CreateDir(const char* path)
{
	if(mkdir(path, 0755))
	{
		PRINTF("%s: the directory %s don't exist, and create it with error!\n", __FUNCTION__, dirname);
		g_free(dirname);
		return 0;
	}
	else
	{
		PRINTF("%s: the directory %s don't exist, and create it!\n", __FUNCTION__, dirname);
		g_free(dirname);
		return 2;
	}
}
#endif

/*
 * @brief Check the directory by absolute path.
 *        If not exist, create it.
 *
 * @param absPath: the absolute path of file

 * @retval 0: the directory is not existed, and failed to make it
 *         1: the directory is already existed
 *         2: the directroy is not existed, but success to make it
 */
int FileMan::CheckDir(const char* absPath)
{
	gchar *dirname;

	if(g_file_test(absPath, G_FILE_TEST_EXISTS))
	{
		if(g_file_test(absPath, G_FILE_TEST_IS_REGULAR))
		{
			dirname = g_path_get_dirname(absPath);
			PRINTF("%s: Get %s dirname %s\n", __FUNCTION__, absPath, dirname);
		}
		else
			return 1;
	}
	else
		dirname = g_strdup(absPath);

	if(mkdir(dirname, 0755))
	{
		PRINTF("%s: the directory %s don't exist, and create it with error!\n", __FUNCTION__, dirname);
		g_free(dirname);
		return 0;
	}
	else
	{
		PRINTF("%s: the directory %s don't exist, and create it!\n", __FUNCTION__, dirname);
		g_free(dirname);
		return 2;
	}

	return 1;
}

#if 0
/*
 * @brief check the file name is whether exist just for ImgMan and VideoMan
 *
 * @param id: the patient id
 * @param filename: the name of image file
 * @param filepath: the directory of storehouse
 * @param absPath(out): the absolute path to save
 *
 * @retval 0: the file is not existed
 *		   1: the file is existed
 *		   2: the directory is not exist, and error with create it
 *		   3: error with check the directory
 */
unsigned char FileMan::CheckFileName(unsigned int no, const char* filename, const char* filepath, char* absPath)
{
	char strdir[256];
	DIR* dir;

	sprintf(strdir, "%s/%d", filepath, no);
	sprintf(absPath, "%s/%s", strdir, filename);
//	PRINTF("strdir = %s\n", strdir);
//	PRINTF("abspath = %s\n", absPath);

	//check the directory
	dir = opendir(strdir);
	if(dir==NULL)
	{
		if(errno==ENOENT)
		{
			if(mkdir(strdir, 0755))
			{
				PRINTF("%s: the directory %s don't exist, and create it with error!\n", __FUNCTION__, strdir);
				return 2;
			}
			else
				PRINTF("%s: the directory %s don't exist, and create it!\n", __FUNCTION__, strdir);
		}
		else
		{
			PRINTF("%s: cannot open directory %s, error is: %s\n", __FUNCTION__, strdir, strerror(errno));
			return 3;
		}
	}
	closedir(dir);

	//check the file
	if(CheckFileName(absPath) == 0)
	{
		PRINTF("%s: %s is not existed\n", __FUNCTION__, absPath);
		return 0;
	}
	else
	{
		PRINTF("%s: %s is existed\n", __FUNCTION__, absPath);
		return 1;
	}
}
#endif

/*
 * @brief change the image file path to ini file path
 *
 * @param absPath: the image file path
 * @param absIniPath(out): the ini file path
 */
 void FileMan::GetIniFilePath(const char* absPath, char* absIniPath)
{
	int i;
	int len = strlen(absPath);
	for(i=0; i<len; i++)
	{
		if(absPath[i] == '.')
			break;
	}
	strncpy(absIniPath, absPath, i);
	absIniPath[i] = '\0';
	strcat(absIniPath, ".ini");
}

 void FileMan::GetFrmFilePath(const char* absPath, char* absFrmPath)
{
	int i;
	int len = strlen(absPath);
	for(i=0; i<len; i++)
	{
		if(absPath[i] == '.')
			break;
	}
	strncpy(absFrmPath, absPath, i);
	absFrmPath[i] = '\0';
	strcat(absFrmPath, ".frm");
}
 
/*
 * @brief check the filename which name has the same suffix
 *
 * @param filename: to be compared
 * @param suffix: the file type eg: "bmp", "jpeg", "emp", "ini"...
 *
 * @retval: return 0: the same suffix
 *			       1: means different
 *			      -1: means no suffix
 */
char FileMan::CompareSuffix(const char* filename, const char* suffix)
{
	int i;
	char strtemp[100];

	if(strlen(filename)==0 || strlen(suffix)==0)
	{
		PRINTF("(CompareSuffix) The directory or suffix is empty!\n");
		return 0;
	}

	for(i=strlen(filename)-1; i>=0; i--)
	{
		if(filename[i]=='.')
		{
			strcpy(strtemp, filename+i+1);
			if(strcmp(strtemp, suffix)==0)
				return 0;
			else
				return 1;
			break;
		}
	}
	return -1;
}

/*
 * @brief count file number which name has the same suffix
 *
 * @param directory: just as its name implies
 * @param suffix: the file type, eg: "bmp", "jpg"
 *
 * @retval: the total of the Has the same suffix document, error with 0 return
 */
int FileMan::CountFileBySuffix(const char* directory, const char* suffix)
{
	DIR* dir;
	int count=0;
	if(strlen(directory)==0 || strlen(suffix)==0)
	{
		PRINTF("(CountFileBySuffix) The directory or suffix is empty!\n");
		return 0;
	}

	dir=opendir(directory);
	if(dir==NULL)
	{
		PRINTF("(CountFileBySuffix) Fail to open directory, the error is:%s\n", strerror(errno));
		return 0;
	}

	struct dirent* ptr;
	while((ptr=readdir(dir))!=NULL)
	{
	//	if(ptr->d_type!=DT_REG)
		gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, directory, ptr->d_name, NULL);
		if(!g_file_test(tmpPath, G_FILE_TEST_IS_REGULAR))
		{
			free(tmpPath);
			continue;
		}
		int i;
		for(i=strlen(ptr->d_name)-1; i>=0; i--)
		{
			if(ptr->d_name[i]=='.')
				break;
		}
		char strtemp[10];
		strcpy(strtemp, ptr->d_name+i+1);
		if(strcmp(strtemp, suffix)==0)
			count++;
		free(tmpPath);
	}
	closedir(dir);

	return count;
}

/*
 * @brief check the file name is whether exist
 * @retval 0: the file is not existed
 *		   1: the file is existed
 */
int FileMan::CheckFileName(const char *dest)
{
	if(access(dest, F_OK) == 0)
	{
		PRINTF("%s(abs): %s is existed\n", __FUNCTION__, dest);
		return 1;
	}
	else
	{
		PRINTF("%s(abs): %s is not existed\n", __FUNCTION__, dest);
		return 0;
	}
}

/*
 * @brief Copy file to dest path by File I/O
 *  Warm: If the dest is existed, it will cover it! 
 *		  If you don't want to cover it, plese call CheckFileName to check it first!
 *
 * @param delete: whether to delete the source file, defaults is not
 */
//#include "CalcTime.h"
int FileMan::CopyFile(const char *source, const char *dest, bool del)
{
	int fds, fdd;
	char buf[4*1024*1024];
	size_t hasread = 0;

	fds = open(source, O_RDONLY | O_SYNC);
	if(fds < 0)
	{
		perror("open source error");
		return -1;
	}

	fdd = open(dest, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, 0666);
	if(fdd < 0)
	{
		perror("open dest error");
		return -1;
	}

//	CalcTime ct;
	while((hasread = read(fds, buf, sizeof(buf))) > 0)
	{
	//	ct.Begin();
		write(fdd, buf, hasread);
	//	ct.End();
	}
	close(fds);
	close(fdd);

	if(del)
	{
		PRINTF("unlink %s ", source);
		if(unlink(source))
			perror("error:");
		else
			PRINTF("\n");
	}

	return 0;
}

#if 0
#include "ViewDialog.h"
static int cond;
static double frac;
gboolean UpdateProgressBar(gpointer data)
{
	ViewDialog::GetInstance()->SetProgressBar(frac);
	cond = 0;
	return false;
}

int FileMan::CopyFileProgress(const char *source, const char *dest)
{
	int fds, fdd;
	unsigned long count, total;
	char buf[4*1024*1024];
	ssize_t hasread = 0;
	ssize_t haswrite = 0;
	struct stat st;

	if(stat(source, &st)<0)
		return -1;
	
	fds = open(source, O_RDONLY | O_SYNC);
	if(fds < 0)
	{
		perror("open source error");
		return -1;
	}

	fdd = open(dest, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, 0666);
	if(fdd < 0)
	{
		perror("open dest error");
		return -1;
	}

	frac = 0;
	while((hasread = read(fds, buf, sizeof(buf))) > 0)
	{
		while(cond)
		{
			gtk_main_iteration();
		}
		haswrite += write(fdd, buf, hasread);
		cond = 1;
		frac = (double)haswrite / st.st_size;
		PRINTF("%s writed %f\n", dest, frac);
	//	UpdateProgressBar(NULL);
		g_timeout_add(10, UpdateProgressBar, NULL);
	}
	close(fds);
	close(fdd);
	return 0;
}
#endif


#if 0
/*
 * @brief copy file to dest path by Std I/O
 */
int FileMan::CopyFile(const char *source, const char *dest)
{
	FILE *fs, *fd;
	char buff[4096];
	int n;

	fs = fopen(source, "rb");
	fd = fopen(dest, "wb");
	while(1)
	{
		if( !feof(fs) && ( n=fread( buff, sizeof(char), sizeof(buff), fs ) >=0 ) )
		{
			fwrite(buff, sizeof(char), sizeof(buff), fd);
		} 
		else if(feof(fs)) 
		{
			fclose(fd);
			fclose(fs);
			return 0;
		}
		else 
		{
			fclose(fd);
			fclose(fs);
			return -1;
		}
	}
}
#endif

/*
 * @brief copy directory to dest path
 */
int FileMan::CopyDirectory(const char* source, const char* dest)
{
	if(!copy_dir(source, dest))
	{
		PRINTF("Successed to copy directory %s to %s!\n", source, dest);
		return 0;
	}
	else
	{
		PRINTF("Failed to copy directory %s to %s!\n", source, dest);
		return -1;
	}
}

int FileMan::copy_dir(const char* source, const char* dest)
{
	DIR *dir, *dir2;
	struct dirent *ent;
	char *src = strdup(source);
	char *des = strdup(dest);

	dir = opendir(src);
	if(!dir)
	{
		perror("open source error");
		return -1;
	}
		
	dir2 = opendir(des);
	if(!dir2)
	{
		if(mkdir(des, 0755)<0)
		{
		//	perror("mkdir error:");
			PRINTF("mkdir %s error: %s\n", des, strerror(errno));
			free(src);
			free(des);
			return -1;
		}
		else
			PRINTF("Create dir %s successed!\n", des);
	}
	else
		closedir(dir2);

	while((ent = readdir(dir)) != NULL)
	{
		if(ent->d_name[0]=='.')
			continue;
	//	if(ent->d_type==DT_DIR)
		gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, src, ent->d_name, NULL);
		if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR))
		{
			char subdir[256];
			char new_dest[256];
			sprintf(subdir, "%s/%s", src, ent->d_name);
			sprintf(new_dest, "%s/%s", des, ent->d_name);

			if(copy_dir(subdir, new_dest) < 0)
			{
				free(src);
				free(des);
				free(tmpPath);
				return -1;
			}
		}
	//	else if(ent->d_type==DT_REG)
		else if(g_file_test(tmpPath, G_FILE_TEST_IS_REGULAR))
		{
		//	PRINTF("%s: %s/%s is a regular file\n", __FUNCTION__, src, ent->d_name);
			char copy_src[256];
			char copy_des[256];
			sprintf(copy_src, "%s/%s", src, ent->d_name);
			sprintf(copy_des, "%s/%s", des, ent->d_name);

			CopyFile(copy_src, copy_des);
		}
		else
		{
		//	PRINTF("%s: %s/%s is a other file\n", __FUNCTION__, src, ent->d_name);
			continue;
		}
	}
	free(src);
	free(des);

	closedir(dir);

	return 0;
}

/*
 * @brief delete directory, note that if the path is regular file or symlink but not directory, it will be deleted yet!
 */
int FileMan::DelDirectory(const char* absPath)
{
	if(!delete_dir(absPath))
	{
		PRINTF("Successed to delete directory %s!\n", absPath);
		return 0;
	}
	else
	{
		PRINTF("Failed to delete directory %s!\n", absPath);
		return -1;
	}
}

int FileMan::delete_dir(const char* absPath)
{
	DIR *dir;
	struct dirent *ent;
	char *path = strdup (absPath);

	PRINTF("%s: absPath = %s\n", __FUNCTION__, path);
	if(g_file_test(path, G_FILE_TEST_IS_REGULAR))
	{
		if(unlink(path) < 0)
		{
			PRINTF("delete file %s error: %s\n", path, strerror(errno));
			free(path);
			return -1;
		}
	}
	else if(g_file_test(path, G_FILE_TEST_IS_DIR))
	{
		dir = opendir(path);
		if(!dir)
		{
			PRINTF("opendir %s error: %s\n", path, strerror(errno));
			free(path);
			return -1;
		}

		while((ent = readdir(dir)) != NULL)
		{
			PRINTF("%s: readdir return filename %s\n", __FUNCTION__, ent->d_name);
		//	if(ent->d_name[0] == '.')
			if(strcmp(ent->d_name, ".")==0 || strcmp(ent->d_name, "..")==0)
				continue;
			//if(ent->d_type==DT_DIR)
			gchar *tmpPath = g_build_path(G_DIR_SEPARATOR_S, path, ent->d_name, NULL);
			if(g_file_test(tmpPath, G_FILE_TEST_IS_DIR))
			{
			//	char *subdir = g_build_filename(path, ent->d_name, NULL);
				if(delete_dir(tmpPath) < 0)
				{
					free(tmpPath);
					free(path);
					return -1;
				}
				free(tmpPath);
			}
			else
		//	else if(ent->d_type==DT_REG)
			{
				PRINTF("%s: %s is not directory or regular file. type=%d\n", __FUNCTION__, ent->d_name, ent->d_type);
				char *filename = g_build_filename(path, ent->d_name, NULL);
				PRINTF("%s: begin to delete file %s\n", __FUNCTION__, filename);
				if(unlink(filename) < 0)	
				{
					PRINTF("delete file %s error: %s\n", filename, strerror(errno));
					free(path);
					free(filename);
					return -1;
				}
				free(filename);
			}
		}
		closedir(dir);

		PRINTF("%s: begin to delete dir %s\n", __FUNCTION__, path);
		if(rmdir(path) < 0)
		{
			PRINTF("rmdir dir %s error: %s\n", path, strerror(errno));
			free(path);
			return -1;
		}
	}
	free(path);
	return 0;
}

static int m_cont = 0;

static gboolean HandleShellCopyFileCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	gchar *buffer = NULL;
	GIOStatus status;

	if (condition == G_IO_HUP || condition == G_IO_ERR) 
	{
		perror("ShellCopyFile GIOChannel error:");
		m_cont = 0;
		return TRUE;
	}

	status = g_io_channel_read_line(source, &buffer, NULL, NULL, NULL);
	if (!buffer || status == G_IO_STATUS_ERROR || status == G_IO_STATUS_AGAIN)
	{
		return TRUE;
	}
	PRINTF("%s: %s", __FUNCTION__, buffer);

	return TRUE;
}

int FileMan::ShellCopyFile(const char* source, const char* dest)
{
	gboolean status = FALSE;
	gchar *cmdline, **cmd;
	gint cmdstatus = 0;
	gint g_out, g_err, pid, arg;
	guint event, eventerr;
	GIOChannel *comerr, *com;

	cmdline = g_strdup_printf("/bin/cp -f %s %s", source, dest);
	PRINTF("cmd: %s\n", cmdline);
	status = g_shell_parse_argv(cmdline, &arg, &cmd, NULL);
	g_free(cmdline);
	if(status == FALSE)
	{
		PRINTF("%s error: shell pares argv\n", __FUNCTION__);
		return -1;
	}

	status = g_spawn_async_with_pipes(NULL, cmd, NULL, 
			(GSpawnFlags ) (G_SPAWN_DO_NOT_REAP_CHILD), 
			NULL, NULL, &pid, NULL, &g_out, &g_err, NULL);
	g_strfreev(cmd);
	if(status == FALSE)
	{
		PRINTF("%s error: g_spawn_async_with_pipes\n", __FUNCTION__);
		return FALSE;
	}
	
	m_cont = 1;

	com = g_io_channel_unix_new(g_out);
	g_io_channel_set_encoding(com, NULL, NULL);
	g_io_channel_set_flags(com, G_IO_FLAG_NONBLOCK, NULL);
	event = g_io_add_watch (com, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleShellCopyFileCallback, NULL);

	comerr = g_io_channel_unix_new(g_err);
	g_io_channel_set_encoding(comerr, NULL, NULL);
	g_io_channel_set_flags(comerr, G_IO_FLAG_NONBLOCK, NULL);
	eventerr = g_io_add_watch (comerr, (GIOCondition)(G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_PRI),
			HandleShellCopyFileCallback, NULL);

	while(m_cont)
	{
		gtk_main_iteration();
	}

	/* exit copyfile process	*/
	waitpid(pid, &cmdstatus, 0);
	if ((WIFEXITED(cmdstatus) && WEXITSTATUS(cmdstatus)!=0) //command exit normal with error
			|| (!(WIFEXITED(cmdstatus))))	//command exit abnormal
	{
		PRINTF("copy file command exit with error\n");
		return FALSE;
	}
	PRINTF("%s finished\n", __FUNCTION__);

	g_source_remove(eventerr);
	g_source_remove(event);

	g_io_channel_shutdown(comerr, FALSE, NULL);
	g_io_channel_unref(comerr);  
	g_io_channel_shutdown(com, FALSE, NULL);
	g_io_channel_unref(com);
	g_spawn_close_pid(pid);

	return TRUE;
}

/*
 * @brief sort the file by name
 */
bool FileMan::SortByName(const string s1, const string s2)
{
	int i;
	int len;
	char str1[255], str2[255];

	len = strlen(s1.c_str()); 
	for(i=0; i<len; i++)
	{
		if(s1.c_str()[i]=='.')
			break;
	}
	strncpy(str1, s1.c_str(), i);
	str1[i] = '\0';

	len = strlen(s2.c_str()); 
	for(i=0; i<len; i++)
	{
		if(s2.c_str()[i]=='.')
			break;
	}
	strncpy(str2, s2.c_str(), i);
	str2[i] = '\0';

	return atof(str1) > atof(str2);
//	return atoi(s1.c_str()) < atoi(s2.c_str());

}

/*
 * @brief sort the file by mtime(time of last modification)
 */
bool FileMan::SortByMTime(const string s1, const string s2)
{
	struct stat b1, b2;

	if(stat(s1.c_str(), &b1) < 0 || stat(s2.c_str(), &b2) < 0)
		return FALSE;

	return b1.st_mtime > b2.st_mtime;
}

/*
 * @brief sort the file by atime(time of last access)
 */
bool FileMan::SortByATime(const string s1, const string s2)
{
	struct stat b1, b2;

	if(stat(s1.c_str(), &b1) < 0 || stat(s2.c_str(), &b2) < 0)
		return FALSE;

	return b1.st_atime > b2.st_atime;
}

/*
 * @brief sort the file by ctime(time of last status change)
 */
bool FileMan::SortByCTime(const string s1, const string s2)
{
	struct stat b1, b2;

	if(stat(s1.c_str(), &b1) < 0 || stat(s2.c_str(), &b2) < 0)
		return FALSE;

	return b1.st_ctime > b2.st_ctime;
}
