#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

//+
// File:	shell.c
//
// Pupose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//				any argument must be numeric and is the exit value
//
//		if the command is not recognized an error is printed.
//-

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
int doInternalCommand(char * args[], int nargs);
int doProgram(char * args[], int nargs);
//+
// Function:	main
//
// Purpose:	The main function. Contains the read
//		eval print loop for the shell.
//
// Parameters:	(none)
//
// Returns:	integer (exit status of shell)
//-

int main() {

    char commandBuffer[CMD_BUFFSIZE];
    // note the pl......0us one, allows for an extra null
    char *args[MAXARGS+1];

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        //printf("%s",commandBuffer);

	// remove newline at end of buffer
	int cmdLen = strlen(commandBuffer);
	if (commandBuffer[cmdLen-1] == '\n'){
	    commandBuffer[cmdLen-1] = '\0';
	    cmdLen--;
            //printf("<%s>\n",commandBuffer);
	}

	// split command line into words.(Step 2)
    int nargs = splitCommandLine(commandBuffer, args, MAXARGS);

	// add a null to end of array (Step 2)
    args[nargs] = '\0';

	// TODO

	//debugging
	printf("%d\n", nargs);
	int i;
	for (i = 0; i < nargs; ++i){
	    printf("%d: %s\n",i, args[i]);
	}
	//element just past nargs
	printf("%d: %x\n",i, args[i]);

        // TODO: check if 1 or more args (Step 3)
        if (nargs > 0){
            if (!doInternalCommand(args, nargs)){
                if (!doProgram(args, nargs)){
                    fprintf(stderr, "Error: Could not find match input to command or executable");
                }
            }
        }
	// print prompt
	printf("%%> ");
	fflush(stdout);
    }
    return 0;
}

////////////////////////////// String Handling (Step 1) ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	TODO: finish description of function
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars
//		ID function if the string doesn't start with skip,
//		or skip is the null character
//-

char * skipChar(char * charPtr, char skip){
    if (skip == '\0')
        return charPtr;
    while (*charPtr == skip){
        charPtr++;
    }
    return charPtr;
}

//+
// Funtion:	splitCommandLine
//
// Purpose:	TODO: give descritption of function
//
// Parameters:
//	TODO: parametrs and purpose
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
   // TODO: Contents of function
    char *curPos = commandBuffer;
    int argNum = 0;
    do {
        curPos = skipChar(curPos, ' ');
        args[argNum] = curPos;
        argNum++;
        curPos = strchr(curPos, ' ');
        if (curPos == NULL){
            if (argNum > maxargs){
                //error print
                exit(0);
            }
            return argNum;
        }
        *curPos = '\0';
        ++curPos;
    } while (*curPos != '\0');

    return argNum;
}

////////////////////////////// External Program  (Note this is step 4, complete doeInternalCommand first!!) ///////////////////////////////////

// list of directorys to check for command.
// terminated by null value
char * path[] = {
    ".",
    "/usr/bin",
    NULL
};

//+
// Funtion:	doProgram
//
// Purpose:	TODO: add description of funciton
// Function is called when the user enters a string into the shell that is not a command
// The function searches for a matching executable to call
// Parameters:
//	TODO: add paramters and description
//
// Returns	int
//		1 = found and executed the file
//		0 = could not find and execute the file
//-

int doProgram(char * args[], int nargs){
    // find the executable    
    // TODO: add body.
    // Note this is step 4, complete doInternalCommand first!!!
    int pathLength;
    int commandLength = strlen(args[0]);
    char* pathStr;
    struct stat fileType;

    for (int i = 0; path[i] != NULL; ++i){
        pathLength = strlen(path[i]);
        pathStr = malloc(sizeof(char) * (pathLength + commandLength + 2));
        sprintf(pathStr, "%s%s%s", path[i], "/", args[0]);
        printf(pathStr);

        if (pathStr != NULL){
            if (!(stat(pathStr, &fileType))){
                if (S_ISREG(fileType.st_mode) && (fileType.st_mode & S_IXUSR)){
                    break;
                }
            }
        }
        printf("\n");
    }
    if (pathStr == NULL)
        return 0;
    
    int forkInt = fork();
    if (forkInt == -1) {
        fprintf(stderr, "Error: Problem with child process");
        return 0;
    } else if (forkInt == 0){
        execv(pathStr, args);
    } else {
        free(pathStr);
        wait(NULL);
    }


    return 1;
}

////////////////////////////// Internal Command Handling (Step 3) ///////////////////////////////////

// define command handling function pointer type
typedef void(*commandFunc)(char * args[], int nargs);

// associate a command name with a command handling function
struct cmdStruct{
   char 	* cmdName;
   commandFunc 	cmdFunc;
};

// prototypes for command handling functions
// TODO: add prototype for each comamand function
void exitFunc(char * args[], int nargs);
void pwdFunc(char * args[], int nargs);
void lsFunc(char * args[], int nargs);
void cdFunc(char * args[], int nargs);
int fileDirVisible(const struct dirent *d);
// list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.
struct cmdStruct commands[] = {
   // TODO: add entry for each command
   {"pwd", pwdFunc},
   {"cd", cdFunc},
   {"ls", lsFunc},
   {"exit", exitFunc},
   { NULL, NULL}		// terminator
};

//+
// Function:	doInternalCommand
//
// Purpose:	TODO: add description
//
// Parameters:
//	TODO: add parameter names and descriptions
//
// Returns	int
//		1 = args[0] is an internal command
//		0 = args[0] is not an internal command
//-

int doInternalCommand(char * args[], int nargs){
    for (int i = 0; commands[i].cmdName != NULL; ++i){
        if (!strcmp(args[0], commands[i].cmdName)){
            (*commands[i].cmdFunc)(args, nargs);
            return 1;
        }
    }
    return 0;
}

///////////////////////////////
// comand Handling Functions //
///////////////////////////////
// TODO: a function for each command handling function
// goes here. Also make sure a comment block prefaces
// each of the command handling functions.

void exitFunc(char * args[], int nargs){
    exit(0);
}

void lsFunc(char * args[], int nargs){
    struct dirent ** namesList;
    int numEntities;
    if (nargs == 1){
        numEntities = scandir(".", &namesList, fileDirVisible, NULL);
    } else if(nargs == 2 && !strcmp(args[1], "-a")){
        numEntities = scandir(".", &namesList, NULL, NULL);
    } else {
        fprintf(stderr, "Error: Command not formatted properly");
        return;
    }

    for (int i = 0; i < numEntities; ++i){
        printf(namesList[i]->d_name);
        printf("\n");
    }
}

int fileDirVisible(const struct dirent *dir){
    if (dir->d_name[0] == ".")
        return 0;
    return 1;
}

void cdFunc(char * args[], int nargs){
    if (nargs == 1){
        struct passwd* password = getpwuid(getuid());
        if (password == NULL){
            fprintf(stderr, "Error: Could not find User ID\n");
            return;
        } else {
            if (password->pw_dir == NULL){
                fprintf(stderr, "Error: Could not find Home Directory\n");
                return;
            } else {
                chdir(password->pw_dir);
                return;
            }
        }
    } else if (nargs == 2){
        if (args[1] == NULL){
            fprintf(stderr, "Error: Invalid argument inserted\n");
            return;
        } else {
            int result = chdir(args[1]);
            if (result != 0)
                fprintf(stderr, "Error: Could not find directory\n");
            return;
        }
    } else {
        fprintf(stderr, "Error: Command not formatted properly, format is: %%>cd <dir>\n");
    }
}

void pwdFunc(char * args[], int nargs){
    char* workingDir = getcwd(NULL, 0);
    printf(workingDir);
    free(workingDir);
}
