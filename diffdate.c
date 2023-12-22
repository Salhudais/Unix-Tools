/******************************************************************************
Title : diffdate.c
Author : Salah Alhudais
Created on : 12/19/2023
Description : Computes the difference in days between two dates
Purpose : To demonstrate how time is kept in C and the variety of functions
          used to manipulate those data structures.
Usage : diffdate date1 [date2 ]
Build with : gcc -Wall -o diffdate diffdate.c
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <locale.h>

/* Parses date struct and prints the date formated as MM DD, YYYY by dynamically allocating
   Size of buffer and later on freeing */
void print_date(const struct tm *date) {
    size_t b_size = 256;
    char *buff = malloc(b_size);
    if(buff) {
        if (strftime(buff, b_size, "%B %d, %Y", date) == 0) {
            fprintf(stderr, "Error formatting date\n");
        } else {
            printf("%s", buff);
        }
        free(buff);
    } else {
        fprintf(stderr, "Could not Allocate Memory for the given date\n");
    }
}

// Parse Date using strptime
int parse_date(const char *dstring, struct tm *date) {
    date->tm_sec = 0;
    date->tm_min = 0;
    date->tm_hour = 0;
    date->tm_isdst = -1;
    if (strptime(dstring, "%Y-%m-%d", date) == NULL) {
        fprintf(stderr, "Error: Did not format Date Appropriately: %s\n", dstring);
        return 0;
    }
    return 1;
}

/* Calculates the difference between two date struct by converting both structs to a time_t data structure
   and using difftime to compare them */
int calc_day_diff(const struct tm *d1, const struct tm *d2) {
    time_t t1 = mktime(d1);
    time_t t2 = mktime(d2);
    return difftime(t1, t2) / 86400;
}
/* Prints the number of days the two dates are apart given a variety of conditions */
void print_day_diff(int days_diff, const struct tm *d1, const struct tm *d2, int argc)  {
    print_date(d1);
    if (days_diff == 0) {
        printf(" is the same as ");
        print_date(d2);
    } else {
        printf(" is %d days %s ", abs(days_diff), days_diff > 0 ? "after" : "before");
        if (argc == 3) {
            print_date(d2);
        } else {
            printf("today");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    // Local Time
    setlocale(LC_TIME, "");
    
    //Initializing our date structs and their seconds representation
    struct tm input1, input2;
    time_t s_input1, s_input2;
    double sec_difference;
    
    // If no arguments or greater than 2 arguments
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s date1 [date2]\n", argv[0]);
        return 1;
    }
    
    // Tries to parse  the first argument and if it can't, prints error message
    if (parse_date(argv[1], &input1) == 0) {
        return 1;
    }
    
    // If there's one argument, it'll use local time, otherwise try parsing the second argument
    if (argc == 2) {
        s_input2 = time(NULL);
        localtime_r(&s_input2, &input2);
        s_input2 = mktime(&input2);
    } else {
        // == 3
        if (parse_date(argv[2], &input2) == 0) {
            return 1; 
        }
    }
    // Compute the difference in dates using difftime
    int days_difference = calc_day_diff(&input1, &input2);
    //Prints the result
    print_day_diff(days_difference, &input1, &input2, argc);
    
    return 0;

}
