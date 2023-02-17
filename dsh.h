/*
 * dsh.h
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */

#define MAXBUF 256
#define HISTORY_LEN 100

// TODO: Your function prototypes below

 char** split(char *str, char *delim);
 int modeOne(char** input);
 int modeTwo(char** input);