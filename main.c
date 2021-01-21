#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>


#define MAX_COMMAND 512
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"


void execution(char*);
char **splitLine(char*);
int myExecvp(char*);
char* removeLeadingSpaces(char*);
void interactiveMode();
void batchMode(char**);

 

int main(int argc, char *argv[]){

	if(argc == 1)
		interactiveMode();
	else if(argc == 2){
		batchMode(argv);
	}else{
        fprintf(stderr, "Too much argument!\n");
        exit(EXIT_FAILURE);
    }
     
    return 0;
}

void interactiveMode(){

	printf("\e[1;1H\e[2J");

	while(1){

        printf("Shell>");

        char input[MAX_COMMAND];
		scanf(" %[^\n]", input);
        
        if(strlen(input) > MAX_COMMAND){
            fprintf(stderr, "Too long command. Max command character size is 512.\n");
            
        }
      
        else if (feof(stdin)){
            perror("Exiting shell ");
            exit(0);
        }
    
        else execution(input);
    }
}

void batchMode(char* argv[]){

    char line[MAX_COMMAND];

	FILE* fp = fopen(argv[1],"r");

	if(fp == NULL){
    	perror("Error opening file");
    	return;
	}

    printf("\nPrinting file:\n\n");

    while(fgets(line,MAX_COMMAND, fp) != NULL){
        printf("%s",line);
    }

    fseek(fp, 0, SEEK_SET);

    printf("\nExecuting:\n\n");

    while(fgets(line,MAX_COMMAND, fp) != NULL){
        execution(line);
        printf("\n");
    }


    fclose(fp);

}

void execution(char* argvk){

    int failed = 0;
    int counter = 0;

    char* tok = strtok(argvk, ";");

    tok = removeLeadingSpaces(tok);
	
    while(tok){

        if (strstr(tok, "quit") !=NULL ){
            printf("Quit command. Exiting shell.\n");
            exit(0);
        }

        pid_t pid = fork();

        if (pid <  0){
            perror("forking child process failed\n");
            exit(EXIT_FAILURE);
        }

        else if (pid == 0){
            if(myExecvp(tok)==-1)
                fprintf(stderr, "%s: command not found\n",tok);
            exit(EXIT_FAILURE);
        }

        else{
            int status;

            if (waitpid(pid, &status, 0) < 0){
                failed++;
            }

            else if (WIFEXITED(status)){
                failed++;
            }

            else if (WIFSIGNALED(status)){
                failed++;
            }

            else{
                failed++;
            }
        }
        tok = strtok(NULL, ";");
    }

    while (wait(NULL) > 0);
}


int myExecvp(char* cmd){

    char **args = splitLine(cmd);

    return execvp(*args,args);

}


char **splitLine(char *line){

    int bufsize = SHELL_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHELL_TOK_DELIM);

    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {

            bufsize += SHELL_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "shell: Allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, SHELL_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


char* removeLeadingSpaces(char* str) { 

    static char str1[99]; 
    int count = 0, j, k; 
  	

    if( str == NULL )
    	return str;

    while (str[count] == ' ') { 
        count++; 
    }

    for (j = count, k = 0; str[j] != '\0'; j++, k++) { 
        str1[k] = str[j]; 
    } 

    str1[k] = '\0'; 
  
    return str1; 
}