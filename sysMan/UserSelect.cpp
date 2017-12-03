#include <unistd.h>  
#include <stdio.h>
#include <gtk/gtk.h>
#include "UserSelect.h"
#include "ViewSystem.h"
#include "FileMan.h"
#include <stdio.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <glib/gi18n.h> // define the label of "_" in this head file


const char* listname[] = {};
#define LIST_COUNT (sizeof(listname)/sizeof(listname[0]))

UserSelect* UserSelect::m_ptrInstance = NULL; 

UserSelect::~UserSelect()
{
    if (m_ptrInstance != NULL)
    {
        delete m_ptrInstance;
        m_ptrInstance = NULL;
    }
}

UserSelect::UserSelect()
{
    active_user_id = 0;
  // strcpy(cur_username,"System Default"); 
}

UserSelect* UserSelect:: GetInstance()
{
    if (m_ptrInstance == NULL)
        m_ptrInstance = new UserSelect;

    return m_ptrInstance;
}

void UserSelect::save_active_user_id(int num)
{
    active_user_id = num;
}

void UserSelect::save_cur_username(char *username)
{
    memset(cur_username, 0, sizeof(cur_username));
    memcpy(cur_username, username, sizeof(cur_username));
}

int UserSelect::get_active_user(void)
{
    return active_user_id;
}

void UserSelect::set_active_user(GtkWidget * widget, gint num)
{
    gtk_combo_box_set_active(GTK_COMBO_BOX(widget), num);

} 

int UserSelect::compare_with_current_user(char *username)
{
    return strcmp(cur_username, username);
}

void UserSelect::create_exportUSB_dir(const char *tmp_name)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s%s%s%s", G_DIR_SEPARATOR_S, UDISK_DATA_PATH, "/", tmp_name);
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}

void UserSelect::create_import_tmp_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s", CALC_TMP_DATA_PATH );
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }
}

void UserSelect::create_udisk_data_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s", UDISK_DATA_PATH );
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}

void UserSelect::create_userconfig_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s", USERCONFIG_PATH );
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}

void UserSelect::create_userdefined_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s%s", CFG_RES_PATH, EXAM_ITEM_FILE);
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            // PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
            //printf("g_file_make_directory error: %s\n", err_mkdir->message);
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}

void UserSelect::create_commentdefined_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s%s", CFG_RES_PATH, COMMENT_PATH);
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            // PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
            //printf("g_file_make_directory error: %s\n", err_mkdir->message);
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}


void UserSelect::create_usercalcdefined_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s%s", CFG_RES_PATH, CALC_ITEM_FILE);
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            //printf("g_file_make_directory error: %s\n", err_mkdir->message);
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}

/*
void UserSelect::create_userdefault_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s%s", CFG_RES_PATH, EXAM_FILE_OTHER);
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            // PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
            //printf("g_file_make_directory error: %s\n", err_mkdir->message);
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}
*/
bool UserSelect::creat_username_db(const char *db_dbname)
{
    if(access(db_dbname, F_OK)) 
    {
        sqlite3 *db = 0;
        int max_count = LIST_COUNT;
        if (sqlite3_open(db_dbname, &db) != SQLITE_OK) 
        {
            PRINTF("Open Database Error!\n");
            return false;
        }
        char *errmsg = NULL;
        char buffer[1024*10], buf[1024];
        memset(buffer, 0, sizeof(buffer));
        //create table
        strcpy(buffer, "BEGIN TRANSACTION;\nCREATE TABLE userlist(username);\nCREATE TABLE userlist_cp(username);\n");

        for(int i = 0; i < max_count; i++)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "INSERT INTO \"userlist\" VALUES('%s');\n", listname[i]);
            strcat(buffer, buf);
        }
        strcat(buffer, "COMMIT;\n");
        if (sqlite3_exec(db, buffer, 0, 0, &errmsg) != SQLITE_OK) 
        {
            PRINTF("Init Database Error: %s\n", sqlite3_errmsg(db));
            sqlite3_free(errmsg);
            return false;
        }

        sqlite3_close(db);
    }
    return true;
}

void UserSelect::read_default_username(GtkWidget* widget)
{

    gtk_combo_box_append_text (GTK_COMBO_BOX (widget), _("System Default"));
}

bool UserSelect::read_username_db(const char *db_dbname,  GtkWidget* widget)
{
    char buf[1024];
    sqlite3 *db = 0;
    int t = 0;

    if (sqlite3_open(db_dbname, &db) != SQLITE_OK) 
    {
        PRINTF("Open Database Error!\n");
        return false; 
    }

    sqlite3_stmt *stmt_f1 = NULL;
    sprintf(buf, "SELECT DISTINCT username FROM userlist;");

    if(sqlite3_prepare(db, buf, strlen(buf), &stmt_f1, 0) != SQLITE_OK) 
    {
        PRINTF("DISTINCT :%s\n", sqlite3_errmsg(db));
        return false;
    }
    {
        int count = 0;
        while(sqlite3_step(stmt_f1) != SQLITE_DONE)
        {
            const char *name = (const char *)sqlite3_column_text(stmt_f1, 0);
            PRINTF("name:%s\n", name);
            if (strcmp(name, "") == 0) continue;
            gtk_combo_box_append_text (GTK_COMBO_BOX (widget), name);
            count++;
            if (strcmp(name, cur_username) == 0)
            {  
                save_active_user_id(count);
            }
        }
    }   
    sqlite3_finalize(stmt_f1);    
    sqlite3_close(db);

    return true;
}

bool UserSelect::insert_username_db(const char *db_dbname, const char *record)
{
    sqlite3 *db = 0;
    char *errmsg = NULL;
    char buf[1024];


    if (sqlite3_open(db_dbname, &db) != SQLITE_OK) 
    {
        PRINTF("Open Database Error!\n");
        return false;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "INSERT INTO \"userlist\" VALUES('%s');\n", record);

    if (sqlite3_exec(db, buf, 0, 0, &errmsg) != SQLITE_OK) 
    {
        PRINTF("insert Database Error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg);
        return false;
    }

    sqlite3_close(db);
    return true;
}

bool UserSelect::delete_username_db(const char *db_dbname, const char *record)
{
    sqlite3 *db = 0;
    char *errmsg = NULL;
    char buf[1024];

    if (sqlite3_open(db_dbname, &db) != SQLITE_OK) 
    {
        PRINTF("Open Database Error!\n");
        return false;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "DELETE FROM userlist WHERE username = '%s';\n", record);

    if (sqlite3_exec(db, buf, 0, 0, &errmsg) != SQLITE_OK) 
    {
        PRINTF("delete Database Error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(errmsg);
        return false;
    }

    sqlite3_close(db);
    return true;
}

bool UserSelect::username_unique_db(const char *db_dbname, const char *record)
{
    bool result = false;
    sqlite3_stmt *stmt = NULL;
    char buf[1024];
    sqlite3 *db = 0;

    if (sqlite3_open(db_dbname, &db) != SQLITE_OK) 
    {
        PRINTF("Open Database Error!\n");
        return false;
    }

    sprintf(buf, "SELECT COUNT(*) AS num_result FROM userlist WHERE username LIKE '%s';\n", record);
    if (sqlite3_prepare(db, buf, strlen(buf), &stmt, 0) != SQLITE_OK) 
    {
        PRINTF("SELECT ERROR:%s!\n", sqlite3_errmsg(db));
        return false;
    }
    while (sqlite3_step(stmt) != SQLITE_DONE) 
    {
        if (strcmp(sqlite3_column_name(stmt, 0), "num_result") == 0)
            result = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    if (result)
        return false;
    else
        return true;
}

void UserSelect::write_username(GtkWidget* widget, const char *db_dbname, const char *record)
{
    if (username_unique_db(db_dbname, record))
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX (widget), record);
        insert_username_db(db_dbname, record);
        char path[256];
        sprintf(path, "%s%s%s", USERCONFIG_PATH, record, ".ini");
        FileMan f;
        char srcpath[50]={0};
#ifdef VET
        sprintf(srcpath, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "VetDefaultItemPara.ini");
#else
        sprintf(srcpath, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "DefaultItemPara.ini");
#endif
        //f.CopyFile("./res/config/ItemPara.ini", path);
        f.CopyFile(srcpath, path);

        memset(user_configure, 0, USERCONFIG_LEN);
        sprintf(user_configure, "%s%s%s", "userconfig/", record, ".ini");
    }

}
#if 0             
/*********************************use regular file to record username ago**************************
 * *******************************in order to delete username change to user db*******************/
void UserSelect::open_username(void)
{
    //printf("open username\n");
    if(access(USER_PATH, F_OK))        
        fp_user = fopen(USER_PATH, "w+");
    else
        fp_user = fopen(USER_PATH, "a+");

    if (fp_user == NULL)
        perror("open error");

    setvbuf(fp_user, NULL, _IONBF, 0);



}


void UserSelect::create_list(void)
{
    char name[50];
    fseek(fp_user, 0, SEEK_SET);

    // create a head note
    Head = new namelist;
    Head->next = NULL;

    while(!feof(fp_user))
    {
        memset(name, 0, sizeof(name));
        fgets(name, 50, fp_user);
        // delete the '\n'
        int i;
        for (i =0; i < 50; i++)
        {
            if (name[i] == '\n')
            {
                name[i] = '\0';
                break;
            }
        }

        AddNametoList(name);
    }

}

void UserSelect::close_username(void)
{
    fclose(fp_user);
}

void UserSelect::read_default_username(GtkWidget* widget)
{
    gtk_combo_box_append_text (GTK_COMBO_BOX (widget), _("System Default"));
}

void UserSelect::read_username(GtkWidget* widget)
{
    char name[50];
    fseek(fp_user, 0, SEEK_SET);

    while(!feof(fp_user))
    {
        memset(name, 0, sizeof(name));
        fgets(name, 50, fp_user);

        // delete the '\n'
        int i;
        for (i =0; i < 50; i++)
        {
            if (name[i] == '\n')
            {
                name[i] = '\0';
                break;
            }
        }

        if (*name != '\0' && *name != '\n') 
            gtk_combo_box_append_text (GTK_COMBO_BOX (widget), name);
    }

}

void UserSelect::create_userconfig_dir(void)
{
    // make directory if not exist
    char dirpath[256];
    sprintf(dirpath, "%s", USERCONFIG_PATH );
    GFile *dir = g_file_new_for_path(dirpath);
    GError *err_mkdir = NULL;
    if(!g_file_make_directory_with_parents(dir, NULL, &err_mkdir))
    {
        if(err_mkdir->code!=G_IO_ERROR_EXISTS)
        {
            // PRINTF("g_file_make_directory error: %s\n", err_mkdir->message);
            //printf("g_file_make_directory error: %s\n", err_mkdir->message);
            g_error_free(err_mkdir);
            g_object_unref(dir);
        }
    }

}

void UserSelect::write_username(GtkWidget* widget, const char *name)
{
    if (CompareName(name) == 0)
    { 
        gtk_combo_box_append_text(GTK_COMBO_BOX (widget), name);
        fprintf(fp_user, "%s\n", name);
        char path[256];
        sprintf(path, "%s%s%s", USERCONFIG_PATH, name, ".ini");
        FileMan f;
        char srcpath[50]={0};
#ifdef VET
        sprintf(srcpath, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "VetDefaultItemPara.ini");
#else
        sprintf(srcpath, "%s%s%s", CFG_RES_PATH, EXAM_FILE_DIR, "DefaultItemPara.ini");
#endif
        //f.CopyFile("./res/config/ItemPara.ini", path);
        f.CopyFile(srcpath, path);

        memset(user_configure, 0, USERCONFIG_LEN);
        sprintf(user_configure, "%s%s%s", "userconfig/", name, ".ini");
        AddNametoList(name);
        //count_node_num();
    }
}

// record the last input order in order to sync the probe user_select and system user_select
void UserSelect::count_node_num(void)
{
    nmlst p;
    int count = -1;
    p = Head->next;
    while (p != NULL)
    {
        count++;
        p = p->next;
    }

    active_user_id = count;
    // printf("active_user_id:%d\n", active_user);
}

int  UserSelect::CompareName(const char *name)
{
    nmlst P, r;
    r = Head->next;

    while(r != NULL)
    {
        P = r;

        //printf("local P->name:%s, name:%s\n",(P->name), name);
        if(strcmp(P->name, name) == 0)
            return -1;	 		
        r = P->next;
    }

    return 0;

}

int  UserSelect::AddNametoList(const char *name)
{
    nmlst node, p;

    p = Head;
    while (p->next != NULL)
    {
        p = p->next;
    }
    node = new namelist;
    strcpy(node->name, name);
    node->next = NULL;
    p->next = node;

    return 0;

}

void UserSelect::AddNametoFile(const char *name)
{
    fprintf(fp_user, "%s\n", name);
}
#endif
