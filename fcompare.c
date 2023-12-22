/******************************************************************************
Title :       fcompare.c
Author :      Salah Alhudais
Created on :  12/13/2023
Description : Given an arbitrary amount of files, fcompare compares 
              the files based on the operation it is given.
Purpose :     Utilizing stat, the stat structure, and parsing options
              to get a better understanding of how file attributes are 
              retrieved, how symbolic links are treated, and options 
Usage :       fcompare <operation> [-rl] file file ..
Build with :  gcc -o fcompare fcompare.c
******************************************************************************/

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

/* Takes a given path and parses it, removing the initial / for readability */
void print_path(const char* path) {
  char* basename = strrchr(path, '/'); /* Finds first occurrence of '/' */
  printf("%s\n", (basename != NULL) ? basename+1 : path);  /* Skips / if present, otherwise use path */
}

/*
Takes two stat structures with the user provided operation and optional flags:
    1. Begins comparison by operation.
    2. Takes into consideration of the r -> -r flag.
    3. Accesses the designated members of the struct relevant to the operation
*/
int cmp_metadata(struct stat* a, struct stat* b, char operation, int r_flag) {
    int result;
    if (operation == 'a') {
        if (r_flag) {
            result = a->st_atime < b->st_atime;
        } else {
            result = a->st_atime > b->st_atime;
        }
    } else if (operation == 'b') {
        if (r_flag) {
            result = a->st_ctime < b->st_ctime;
        } else {
            result = a->st_ctime > b->st_ctime;
        }
    } else if (operation == 'c') {
        if (r_flag) {
            result = a->st_ctime < b->st_ctime;
        } else {
            result = a->st_ctime > b->st_ctime;
        }
    } else if (operation == 'm') {
        if (r_flag) {
            result = a->st_mtime < b->st_mtime;
        } else {
            result = a->st_mtime > b->st_mtime;
        }
    } else if (operation == 's') {
        if (r_flag) {
            result = a->st_size > b->st_size;
        } else {
            result = a->st_size < b->st_size;
        }
    } else if (operation == 'u') {
        if (r_flag) {
            result = a->st_blocks > b->st_blocks;
        } else {
            result = a->st_blocks < b->st_blocks;
        }
    } else {
        fprintf(stderr, "Error: Invalid operation specified\n");
        exit(EXIT_FAILURE);
    }
    return result;
}
int main(int argc, char** argv) {
/* Initial flags to check the operations */
  int r_flag = 0; /* Reverse flag for -r option */
  int symb_link = 0; /* Flag for -l Symbolic link option */
  char operation = '\0'; /* The type of operation given, initially set to null. */
  int operation_count = 0; /* Counting the operations to make sure it does not exceed 1 */
  int opt;
  while ((opt = getopt(argc, argv, "arlbcmus")) != -1) {
      /* Checks if the option is among a,b,c,m,s and u for operation_count */
    if(opt == 'a' || opt == 'b' || opt == 'c' || opt == 'm' || opt == 's' || opt == 'u') {
        operation_count++;
    }
    if (opt == 'r') {
        r_flag = 1;
    } else if (opt == 'l') {
        symb_link = 1;
    } else if (opt == 'a') {
        operation = 'a';
    } else if (opt == 'b') {
        operation = 'b';
    } else if (opt == 'c') {
        operation = 'c';
    } else if (opt == 'm') {
        operation = 'm';  
    } else if (opt == 's') {
        operation = 's';
    } else if (opt == 'u') {  
        operation = 'u';
    } else if (opt == '?') {
        printf("Invalid option: %c\n", optopt); /* // Invalid option option returned by getopt -> defaults to ? */
        exit(EXIT_FAILURE);
      }
  }
    /* Only one operation is allowed */
    if(operation_count > 1) {
      fprintf(stderr, "Error: Only one operation allowed\n");
      exit(EXIT_FAILURE);
    }
    /* Ensure operation is provided */
    if (operation == -1) {
        fprintf(stderr, "Error: Must specify compare operation\n");
        exit(EXIT_FAILURE);
      }
    /* Begin to process file arguments */
    int min_index = -1; /* Tracks target file argument by index */
    struct stat min_stat; /* target struct to be updated */
    for (int i = optind; i < argc; i++) { /* optind is derived from getopt and it is currently pointing to the file arguments now */
        const char* path = argv[i];
        struct stat sb;
        int res;
        // Handles Symbolic Links using lstat where it returns the information about the link itself and not the file referred
        if(symb_link != 0) {
            struct stat link_stat;
            if(lstat(path, &link_stat) == -1) {
                perror(path);
                continue;
            }
            if(S_ISLNK(link_stat.st_mode)) {
                if (stat(path, &sb) == -1) {
                    perror(path);
                    continue;
                }
            }else {
                if(lstat(path, &sb) == -1) {
                    perror(path);
                    continue;
                }
            }
        }else {
            res = stat(path, &sb);
        }
        if (res == -1) {
          perror(path);
          continue;
        }
        // Begins with min and compares new stat structures, if found less or greater depending on r_flag, update
        if (min_index == -1 || cmp_metadata(&sb, &min_stat, operation, r_flag)) {
          min_index = i;
          min_stat = sb; 
        }
    }
    if (min_index != -1) {
        print_path(argv[min_index]);
    }
    return 0;
}
