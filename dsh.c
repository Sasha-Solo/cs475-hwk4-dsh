/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>

#define CAP 256

// TODO: Your function definitions (declarations in dsh.h)

/**
* Splits given string on delimiters
* @param str string to be split up
* @param delim delimiters for the string to be split on
* @result an array of substrings split on the delimiters 
*/ 
 char** split(char *str, char *delim){
    int numTokens = 0;
    //count number of instances of delim
    for (int i = 0; i < strlen(str); i++){
        if (*(str + i) == *delim){ //go through str and incrmement where it points to and dereference the whole thing to get its content
            numTokens++;
        }
    }

    //get space on heap 
    char** splitString = (char**) malloc((numTokens + 2) * sizeof(char*));

    // this loops through each array element and instantiates
    // an array of capacity CAP
    for (int i = 0; i < (numTokens + 1); i++) {
        splitString[i] = (char*) malloc(CAP * sizeof(char));
    }

    int j = 0;
    //split the string on the delims
    char* token = strtok(str, delim);

    while (token != NULL){
        strcpy(splitString[j], token);
        j++;
		token = strtok(NULL, delim);
	}
    
    splitString[j] = NULL;

    return splitString;
 }

/**
* Handles user input when user types in full path of execution
* @param input full path name given by user
* @result an int 0 or 1 representing if method failed or succeeded
*/ 
 int modeOne(char** input){

    //counts number of args:
    int count = 0;
    for (int i = 0; input[i] != NULL; i++){
        count++;
    }

    int andSymbol = 0; //to keep track of &
    if (strcmp(input[count-1], "&") == 0){
        andSymbol = 1;
    }
   
    if (access(input[0], F_OK | X_OK) == 0){ //if file exists and is executable, proceed with mode one
        //get space on heap for array of strings
        char** arg = (char**) malloc(CAP * sizeof(char*));
    
        // this loops through each array element and instantiates
        // an array of capacity CAP
        for (int i = 0; i < count; i++) {
            arg[i] = (char*) malloc(CAP * sizeof(char));
        }

        int i = 0;
        //get command arguments and put into an array of strings
        for (i = 0; input[i] != NULL; i++){
            strcpy(arg[i], input[i]); //put input args into the array of strings
        }
        arg[i] = NULL;

        pid_t pid;
        pid = fork(); //creates child process
        
        if (pid == 0){ //check if we are in child process
            execv(arg[0], arg); //call execv

            //free up space
             for (int i = 0; arg[i] != NULL; i++) {
                char* currPointer = arg[i]; //get each pointer in terms
		        free(currPointer);//free each pointer
		        currPointer = NULL; //set to null for defensive programming
            }
	        free(arg);
            arg = NULL;

        }
        else{ //we are in parent
            if (andSymbol == 0){
                wait(NULL);
            }
    
            //free up space
            for (int i = 0; arg[i] != NULL; i++) {
                char* currPointer = arg[i]; //get each pointer in terms
		        free(currPointer);//free each pointer
		        currPointer = NULL; //set to null for defensive programming
            }
	        free(arg);
            arg = NULL;
        }
        return 1;
    }
    else{
        printf("File doesn't exist or is not executable!\n"); // Alert user 
        return 1; //return to reprompt the user
    }

    return 0;
 }

/**
* Handles user input when user does not type in the full path name
* @param input user input 
* @result an int 0 or 1 representing if method failed or succeeded
*/ 
 int modeTwo(char** input){

    //get the current working directory
    char cwd[MAXBUF]; //space for current working directory
	getcwd(cwd, sizeof(cwd)); //get the cwd
	strcat(cwd, "/"); //concat slash to cwd

    //concatenate input to cwd
    strcat(cwd, input[0]);

    //counts number of args in input
    int count = 0;
    for (int i = 0; input[i] != NULL; i++){
        count++;
    }

    if (access(cwd, F_OK | X_OK) == 0){ //check if the file is in the cwd
        modeOne(input); //if found, execute mode 1
        return 1;
    } 
    else{ //if the file is not in the cwd...
        
        //malloc and make a copy of getenv(PATH)
        char* path = getenv("PATH"); //get the paths inside PATH env variable //DO I NEED TO FREE
        char* pathCopy = (char*) malloc((strlen(path) + 1) * sizeof(char));
        strcpy(pathCopy, path);
        char** splitPath = split(pathCopy, ":");

        //concatenate user input to end of each individual path that we want to check
        for (int i = 0; splitPath[i] != NULL; i++){
            strcat(splitPath[i], "/");
            strcat(splitPath[i], input[0]); 
        }

        //search for the file elsewhere
        for (int i = 0; splitPath[i] != NULL; i++){
            if (access(splitPath[i], F_OK | X_OK) == 0){ //check if the file is in any of the paths
                strcpy(input[0], splitPath[i]);
                modeOne(input); //if found, execute mode 1

                free(pathCopy); //free pathCopy
                pathCopy = NULL;

                //free splitPath:
                for (int i = 0; splitPath[i] != NULL; i++) { //free splitPath
        	        char* currPointer = splitPath[i]; //get each pointer in terms
			        free(currPointer);//free each pointer
			        currPointer = NULL; //set to null for defensive programming
    	        }
                free(splitPath);
                splitPath = NULL;

                return 1;
            } 
        }

        printf("ERROR: Command not found\n");

        //free pathCopy twice bc returned and used twice
        free(pathCopy); //free pathCopy
        pathCopy = NULL;
        
        //free splitPath:
        for (int i = 0; splitPath[i] != NULL; i++) { //free splitPath
        	char* currPointer = splitPath[i]; //get each pointer in terms
			free(currPointer);//free each pointer
			currPointer = NULL; //set to null for defensive programming
    	}
        free(splitPath);
        splitPath = NULL;

        return 1; //return to reprompt the user
    }

    return 0;
 }

 