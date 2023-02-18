/*
 * main.c
 *
 *  Created on: Mar 17 2017
 *      Author: david
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "dsh.h"


#define MAXBUF 256
#define HISTORY_LEN 100

int main(int argc, char **argv)
{
	char* input = (char*) malloc((MAXBUF) * sizeof(char*));
	strcpy(input, ""); //initialize input so no valgrind error

	while(strcmp(input, "exit") != 0){ //keep getting user input until exit is pressed
		printf("dsh> ");
		//accept user input
		fgets(input, MAXBUF, stdin);
		
		for (int i = 0; i < strlen(input); i++){
			if (input[0] == '\n' && strlen(input) == 1){ //if user types in an enter for the first time
				printf("dsh> "); //re-prompt the user
				fgets(input, MAXBUF, stdin); //get the user input
			} 
			else if (input[i] == '\n'){ //if there is another enter somewhere along the way...
				input[i] = '\0';
			}
		}
		
		char** splitInput = split(input, " "); //split the input on the spaces

		//handles built in commands:
		if (strcmp(splitInput[0], "cd") == 0){ //cd
			if (splitInput[1] == NULL){ //if user does cd without passing in a directory
				chdir(getenv("HOME"));
			}
			else{
				chdir(splitInput[1]); //changes directory to the one passed in
			}
			
		}
		else if (strcmp(splitInput[0], "pwd") == 0){ //pwd
			char buffer[MAXBUF]; //space for current working directory
			getcwd(buffer, sizeof(buffer)); //get the cwd
			printf("%s \n", buffer); //print out cwd
		}
		
		if (input[0] == '/'){ //if input starts with a "/", then execute mode 1
			modeOne(splitInput);
		}
		else if (strcmp(input, "exit") != 0 && strcmp(input, "pwd") != 0 && strcmp(input, "cd") != 0) { //execute mode 2 
			modeTwo(splitInput); 
		}

		for (int i = 0; splitInput[i] != NULL; i++) { //free splitInput
        	char* currPointer = splitInput[i]; //get each pointer in terms
			free(currPointer);//free each pointer
			currPointer = NULL; //set to null for defensive programming
    	}
		free(splitInput); //frees splitInput
		splitInput = NULL; //defensive programming
	}

	free(input); //frees input
	input = NULL; //defensive programming

	return 0;
}
