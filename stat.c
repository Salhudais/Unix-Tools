/******************************************************************************
Title :       stat.c
Author :      Salah Alhudais
Created on :  12/22/2023
Description : Given any file argument or no argument at all, returns 
              file attributes.
Usage :       ./stat file file ...
Build with :  gcc -o stat.c stat
******************************************************************************/

#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>

#define BUFFERSIZE 4096
#define COPYMODE 0644

void do_ls(char[]);
void dostat(char *, char *);
void show_file_info(char *, struct stat *);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);

void mode_to_letters(int mode, char str[]) {
    strcpy(str, "----------"); // Default
    if(S_ISDIR(mode) ) str[0] = 'd';
    if(S_ISCHR(mode) ) str[0] = 'c';
    if(S_ISBLK(mode) ) str[0] = 'b';
    
    if(mode & S_IRUSR) str[1] = 'r';
    if(mode & S_IWUSR) str[2] = 'w';
    if(mode & S_IXUSR) str[3] = 'x';
    
    if(mode & S_IRGRP) str[4] = 'r';
    if(mode & S_IWGRP) str[5] = 'w';
    if(mode & S_IXGRP) str[6] = 'x';
    
    if(mode & S_IROTH) str[7] = 'r';
    if(mode & S_IWOTH) str[8] = 'w';
    if(mode & S_IXOTH) str[9] = 'x';
}

int main(int ac, char *av[]) {
    if ( ac == 1) {
        do_ls(".");
    }else {
        while(--ac) {
            printf("%s:\n", *++av);
            do_ls(*av);
        }
    }
}
void do_ls(char dirname[]) {
    DIR *dir_ptr;
    struct dirent *direntp;
    if((dir_ptr = opendir(dirname)) == NULL) {
        fprintf(stderr, "ls1: cannot open %s\n", dirname);
    }else {
        while((direntp = readdir(dir_ptr)) != NULL) {
            dostat(dirname, direntp->d_name);
        }
        closedir(dir_ptr);
    }
}
void dostat(char *dirname, char *filename) {
    struct stat info;
    char fullpath[4096];
    
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, filename);
    if(stat(fullpath, &info) == -1) {
        perror(fullpath);
    }else {
        show_file_info(filename, &info);
    }
}
void show_file_info(char *filename, struct stat *info_p) {
    char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
    void mode_to_letters();
    char modestr[11];
    
    mode_to_letters(info_p->st_mode, modestr);
    
    printf("%s", modestr);
    printf("%4ld ", info_p->st_nlink);
    printf("%-8s ", uid_to_name(info_p->st_uid));
    printf("%-8s ", gid_to_name(info_p->st_gid));
    printf("%8ld ", (long)info_p->st_size);
    printf("%.12s ", 4+ctime(&info_p->st_mtime));
    printf("%s\n", filename);
}

char *uid_to_name(uid_t uid) {
    struct passwd* getpwuid(), *pw_ptr;
    static char numstr[10];
    if((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr,"%d", uid);
        return numstr;
    }else{
        return pw_ptr->pw_name;
    }
}

char *gid_to_name(gid_t gid) {
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];
    if((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    }else{
        return grp_ptr->gr_name;
    }
}
