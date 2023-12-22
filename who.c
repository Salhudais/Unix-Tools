/******************************************************************************
Title :       who.c
Author :      Salah Alhudais
Created on :  12/22/2023
Description : Accesses the utmp Linux Structure to return current user
Usage :       ./who
Build with :  gcc -o who who.c
******************************************************************************/
#include <stdio.h>
#include <utmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h> 
#include <time.h>

#define SHOWHOST

void show_info(struct utmp *utbufp) {
    if(utbufp -> ut_type != USER_PROCESS) {
        return;
    } 
    printf("%-8.8s", utbufp -> ut_name);/* Log name */
    printf(" ");
    printf("%-8.8s", utbufp -> ut_line); /* tty */
    printf(" ");
    time_t val = (time_t)(utbufp->ut_time);
    char * cp = ctime(&val);
    printf("%12.12s", cp+4);
    printf(" ");
#ifdef SHOWHOST
   if(utbufp->ut_host[0] != '\0') {
      printf("  (%s)\n", utbufp-> ut_host);
   }
#endif
    printf("\n");
    
}

int main() {
    struct utmp current_record;
    int utmpfd;
    int reclen = sizeof(current_record);
    if( (utmpfd = open(UTMP_FILE, O_RDONLY)) == -1) {
        perror(UTMP_FILE);
        exit(1);
    }
    while(read(utmpfd, &current_record, reclen) == reclen) {
        show_info(&current_record);
    }
    close(utmpfd);
    
    return 0;
}
