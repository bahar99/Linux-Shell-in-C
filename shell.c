//YOU NEED TO LINK THE LIBRARY,PUT -lreadlin IN GCC ARGUMENTS


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

// Clearing the shell using escape sequences
#define clear() printf("\033[H\033[J")

// printing during startup
void init_shell()
{
	clear();
	printf("\n\n\n\n##################"
		"##########################");
	printf("\n\n\n\t****SHELL****");
	printf("\n\n\n\n##################"
		"##########################");
	char* username = getenv("USER"); //Getting username by getenv(“USER”)
	printf("\n\n\nUSER : @%s", username);
	printf("\n");
	sleep(1);
	clear();
}

// takeing input
int takeInput(char* str)
{
	char* buffer;

	buffer = readline("\n>>> ");
	if (strlen(buffer) != 0) {
		add_history(buffer);
		strcpy(str, buffer);
		return 0;
	} else {
		return 1;
	}
}

// Printing Current Directory.
void printDirectory()
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd)); //Printing the directory by using getcwd.
	printf("\nDir: %s", cwd);
}

// executing the system command without pipe
void execArgs(char** parsed)
{

	pid_t pid = fork();

	if (pid == -1) { //forking is failed
		printf("\nFailed forking child..");
		return;
	} else if (pid == 0) { //child process
		if (execvp(parsed[0], parsed) < 0) { //Execute the command using execvp()
			printf("\nCould not execute command..");
		}
		exit(0);
	} else {
		// waiting for child to terminate
		wait(NULL);
		return;
	}
}

// executing the piped system command
void execArgsPiped(char** parsed, char** parsedpipe)
{
	// 0 is read end, 1 is write end
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0) {
		printf("\nPipe could not be initialized");
		return;
	}
	p1 = fork();
	if (p1 < 0) { //forking is failed
		printf("\nCould not fork");
		return;
	}

	if (p1 == 0) {
		// Child 1 executing..
		// It only needs to write at the write end
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execvp(parsed[0], parsed) < 0) {
			printf("\nCould not execute command 1..");
			exit(0);
		}
	} else {
		// Parent executing
		p2 = fork();

		if (p2 < 0) { //forking is failed
			printf("\nCould not fork");
			return;
		}

		// Child 2 executing..
		// It only needs to read at the read end
		if (p2 == 0) {
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(parsedpipe[0], parsedpipe) < 0) {
				printf("\nCould not execute command 2..");
				exit(0);
			}
		} else {
			// parent executing, waiting for two children
			wait(NULL);
			wait(NULL);
		}
	}
}

// Help command builtin
void Help()
{
	puts("\n***WELCOME TO MY SHELL HELP***"
		"\nList of Commands supported:"
		"\n>cd"
		"\n>ls"
		"\n>exit"
    "\n>pipe handling"
		"\n>Other general commands available in UNIX shell");

	return;
}

//execute builtin commands
int builtinCmdHandler(char** parsed)
{
	int NoOfCmds = 3, i, switchArg = 0;
	char* ListOfCmds[NoOfCmds];


	ListOfCmds[0] = "exit";
	ListOfCmds[1] = "cd";
	ListOfCmds[2] = "help";


	for (i = 0; i < NoOfCmds; i++) {
		if (strcmp(parsed[0], ListOfCmds[i]) == 0) {
			switchArg = i + 1;
			break;
		}
	}

	switch (switchArg) {
	case 1:
		printf("\n Goodbye :] \n");
		exit(0);
	case 2:
  // “cd” does not work natively using execvp, so it is a built-in command,
  // executed with chdir().
		chdir(parsed[1]);
		return 1;
	case 3:
		Help();
		return 1;
	default:
		break;
	}

	return 0;
}

// finding pipe
int parsePipe(char* str, char** strpiped)
{
	int i;
	for (i = 0; i < 2; i++) {
		strpiped[i] = strsep(&str, "|"); //Detecting pipes by using strsep(“|”).
		if (strpiped[i] == NULL)
			break;
	}

	if (strpiped[1] == NULL)
		return 0; // returns zero if no pipe is found.
	else {
		return 1;
	}
}

//parsing command words
void parseSpace(char* str, char** parsed)
{
	int i;

	for (i = 0; i < MAXLIST; i++) {
		parsed[i] = strsep(&str, " "); // It will separate words based on spaces.

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
	}
}

int processString(char* str, char** parsed, char** parsedpipe)
{

	char* piped_parts[2];
	int has_pipe= 0;

	has_pipe = parsePipe(str, piped_parts);//finding pipe

	if (has_pipe) { //if it has pipe
		parseSpace(piped_parts[0], parsed);
		parseSpace(piped_parts[1], parsedpipe);

	} else {

		parseSpace(str, parsed);
	}

	if (builtinCmdHandler(parsed))
		return 0;
	else
		return 1 + has_pipe;
}

int main()
{
	char inputString[MAXCOM], *parsedArgs[MAXLIST];
	char* parsedArgsPiped[MAXLIST];
	int execFlag = 0;
	init_shell(); //printing during startup

	while (1) {

		printDirectory();//printing the Current directory

		if (takeInput(inputString))	// takeing input
			continue;

		execFlag = processString(inputString,
		parsedArgs, parsedArgsPiped); //process the string
		// execflag returns zero if there is no command
		// or it is a builtin command,
		// 1 if it is a simple command
		// 2 if it is including a pipe.

		// executing the system command without pipe
		if (execFlag == 1)
			execArgs(parsedArgs);

    //executing the piped system command
		if (execFlag == 2)
			execArgsPiped(parsedArgs, parsedArgsPiped);
	}
	return 0;
}
