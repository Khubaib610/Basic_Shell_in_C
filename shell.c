#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define max_size 64
static int count = 0;

void parse(char *line, char **argv){
	while (*line != '\0') /* if not the end of line ....... */
	{
	while (*line == ' ' || *line == '\t' || *line == '\n')
	*line++ = '\0'; /* replace white spaces with 0 */
	*argv++ = line; /* save the argument position */
	while (*line != '\0' && *line != ' ' &&
	*line != '\t' && *line != '\n')
	line++;
	}
	*argv = '\0';
}

void execute(char **argv){
	pid_t pid;
	int status;
	if ((pid = fork()) < 0) /* fork a child process */
	{
	printf("*** ERROR: forking child process failed\n");
	exit(1);
	}
	else if (pid == 0) { /* for the child process: */
	if (execvp(*argv, argv) < 0) /* execute the command */
	{
	printf("*** ERROR: exec failed\n");
	exit(1);
	}
	}
	else /* for the parent: */
	{
	while (wait(&status) != pid) ; /* wait for completion */
	}
}

struct History{
	int num, index;
	char cmd[200];  //char array to store a whole commad
};

void storeHistory(char **argv, struct History *history, int size){
	 
	int index = 0;
	
	for(int j=0; j<200; j++)
	history[count%size].cmd[j] = '\0';
	
	//Assigning values to char array
	for(int i=0; argv[i] != NULL; i++){
	char *ptr = argv[i];
	char array[50];
	int j;
	for(j=0; ptr[j] != '\0'; j++)
	array[j] = ptr[j];
	array[j] = '\0';
	
	for(j=0; array[j] != '\0'; j++){
	history[count%size].cmd[index] = array[j];
	index++;
	}
	history[count%size].cmd[index++] = ' ';	
	
	}
	
	//Assigning value to num
	history[count%size].num = count;
	history[count%size].index = count%size;
	count++;
	
}

void displayHistory(struct History *history){
	for(int i=0; i<10; i++){
	    if(history[i].index == i){
		printf("%d. ", history[i].num);
		printf("%s\n", history[i].cmd);
	    }
	}
	
}

//Returns the index of the most recent command
int getRecentIndex(struct History *history, int size){
for(int i=0; i<size; i++){
    if(count-2 == history[i].num)
    return i;
}
}

// Checks & Returns value of "!n" where n is any integer
int getval(char *arg, struct History *history, int size){
   if(arg[0] == '!' && isdigit(arg[1])){
      char arr[3];
      int num;
      
      for(int i=0; i<3; i++)
          arr[i] = arg[i+1];
      
      num = atoi(arr);
      
      for(int i=0; i<size; i++){
          if(num == history[i].num)
      	    return i;
      }
      
   
   } 
    return -1;
}

void main(void){
	char line[1024];
	char *argv[64];
	int index, size = 10;
	struct History history[size]; 
	
	
	while(1){
		printf("SHELL :) -> ");
		fgets(line, 1024, stdin);
		printf("\n");
		parse(line, argv);
		if(strcmp(argv[0], "exit") == 0) exit(0);
		//arr = argv; int i = 0;
		
	   storeHistory(argv, history, size);
	   if(strcmp(argv[0],"history")==0||strcmp(argv[0],"History") == 0)
	   displayHistory(history);
	   printf("\n");
	
	   index = getval(argv[0], history, size);
	   if(index != -1){
	   parse(history[index].cmd, argv);
	   execute(argv);
	   continue;
	}
	
	
	if(strcmp(argv[0], "!!") == 0){
	   int index = getRecentIndex(history, size);
	   parse(history[index].cmd, argv);
	   execute(argv);
	   continue;
	}
	
	execute(argv);
	}
	
}

