#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "imports_osal.h"

#ifdef WIN32
    #include <windows.h>

    const int  osal_libsearchdirs = 2;
    const char *osal_libsearchpath[2] = { ".\\", ".\\emulator\\" };
#else
    #include <dirent.h>
    #include <limits.h>
    #include <sys/types.h>

    const int   osal_libsearchdirs = 2;
    const char *osal_libsearchpath[2] = { "./", "./emulator/" };
#endif

osal_lib_search *osal_library_search(const char *searchpath)
{
    osal_lib_search *head = NULL, *curr = NULL;

#ifdef WIN32
    WIN32_FIND_DATA entry;
    HANDLE hDir;

    char *pchSearchPath = (char *) malloc(strlen(searchpath) + 16);
    if (pchSearchPath == NULL)
    {
        printf("Couldn't allocate memory for file search path in osal_library_search()!");
        return NULL;
    }
    sprintf(pchSearchPath, "%s\\*.dll", searchpath);
    hDir = FindFirstFile(pchSearchPath, &entry);
    free(pchSearchPath);
    if (hDir == INVALID_HANDLE_VALUE)
        return NULL;

    /* look for any shared libraries in this folder */
    do
    {
        osal_lib_search *newlib = NULL;
        /* this is a .dll file, so add it to the list */
        newlib = (osal_lib_search*)malloc(sizeof(osal_lib_search));
        if (newlib == NULL)
        {
            printf("Memory allocation error in osal_library_search()!");
            osal_free_lib_list(head);
            FindClose(hDir);
            return NULL;
        }
        if (head == NULL)
        {
            head = curr = newlib;
        }
        else
        {
            curr->next = newlib;
            curr = newlib;
        }
        /* set up the filepath and filename members */
        strncpy(curr->filepath, searchpath, PATH_MAX-2);
        curr->filepath[PATH_MAX-2] = 0;
        if (curr->filepath[strlen(curr->filepath)-1] != '\\')
            strcat(curr->filepath, "\\");
        int pathlen = (int) strlen(curr->filepath);
        curr->filename = curr->filepath + pathlen;
        strncat(curr->filepath, entry.cFileName, PATH_MAX - pathlen - 1);
        curr->filepath[PATH_MAX-1] = 0;
        /* set plugin_type and next pointer */
        curr->plugin_type = (m64p_plugin_type) 0;
        curr->next = NULL;
    } while (FindNextFile(hDir, &entry));

    FindClose(hDir);
#else
    DIR *dir;
    struct dirent *entry;
        
    #ifdef __APPLE__
        const char* suffix = ".dylib";
    #else
        const char* suffix = ".so";
    #endif

    dir = opendir(searchpath);
    if (dir == NULL)
        return NULL;

    /* look for any shared libraries in this folder */
    while ((entry = readdir(dir)) != NULL)
    {
        osal_lib_search *newlib = NULL;
        if (strcmp(entry->d_name + strlen(entry->d_name) - strlen(suffix), suffix) != 0)
            continue;
        /* this is a .so file, so add it to the list */
        newlib = (osal_lib_search*)malloc(sizeof(osal_lib_search));
        if (newlib == NULL)
        {
            printf("Memory allocation error in osal_library_search()!");
            osal_free_lib_list(head);
            closedir(dir);
            return NULL;
        }
        if (head == NULL)
        {
            head = curr = newlib;
        }
        else
        {
            curr->next = newlib;
            curr = newlib;
        }
        /* set up the filepath and filename members */
        strncpy(curr->filepath, searchpath, PATH_MAX-2);
        curr->filepath[PATH_MAX-2] = 0;
        if (curr->filepath[strlen(curr->filepath)-1] != '/')
            strcat(curr->filepath, "/");
        int pathlen = strlen(curr->filepath);
        curr->filename = curr->filepath + pathlen;
        strncat(curr->filepath, entry->d_name, PATH_MAX - pathlen - 1);
        curr->filepath[PATH_MAX-1] = 0;
        /* set plugin_type and next pointer */
        curr->plugin_type = M64PLUGIN_NULL;
        curr->next = NULL;
    }

    closedir(dir);
#endif

    return head;
}

void osal_free_lib_list(osal_lib_search *head)
{
    while (head != NULL)
    {
        osal_lib_search *next = head->next;
        free(head);
        head = next;
    }
}