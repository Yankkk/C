#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

char ** parse(char *);
void execute(char **, pid_t *);
void writeTofile(char *, int);

/**
typedef struct linkedlist{
	char* data;
	linkedlist * next;
};
*/

int main(int argc, char * argv[]){
	char* buffer = (char*)malloc(2048*sizeof(char));
	char address[100];
	char ins[100];
	char commond[2048];
	char** arg = NULL;
	char pro;
	int status;
	char c1[2048];
	char c2[2048];
	char c3[2048];
	char c4[2048];
	
	pid_t child1 = -1;
	pid_t child2 = -1;
	pid_t child3 = -1;
	pid_t child4 = -1;
	

	while(1){
		fgets(buffer, 2047, stdin);
		
		
		if(buffer[0] == 'q'){
			if(child1 != -1){
				kill(child1, SIGKILL);
			}
			if(child2 != -1){
				kill(child2, SIGKILL);
			}
			if(child3 != -1){
				kill(child3, SIGKILL);
			}
			if(child4 != -1){
				kill(child4, SIGKILL);
			}
			exit(0);
		}
		if(buffer[0] == 'h'){
			printf("i        Print the above information\n\
o 1      Print the output of the most-recent slot 1 program\n\
l 1      Display a log (history) of commands of slot 1\n\
r 1 gcc..   Run  the command gcc in slot 1. If there is a running command in slot it is killed with SIGKILL\n\
cd ...   Change the current directory of the shell. (Future commands will use this as their current directory)\n\
w 1 gcc..   Run the command gcc in slot 1 after the current command has finished\n\
h        Display a help message (this text)\n\
z 1      Stops (pauses) the most recent slot 1 program if it is still running\n\
g 1      Continues a previously stopped program in slot 1. If there is no slot 1 stopped process this command does nothing.\n\
q        Quits.\n");
		}
		if(buffer[0] == 'c'){
			//if(strlen(buffer) >= 3){
				sscanf(buffer, "%s %s", ins, address);
				chdir(address);
				//execl("/bin/ls", "/bin/ls", ".", (char*) NULL);
			//}
			//else{
				//printf("No directory entered!\n");
			//}
		}
		
		
		if(buffer[0] == 'r'){
			char commond[2048];
			pro = buffer[2];
			
			int i;
			for(i = 4; i < strlen(buffer); i++){
				commond[i-4] =buffer[i];
			}
			commond[i] = '\0';
	
			writeTofile(commond, pro);
			arg = parse(commond);
			/*
			char ** tempptr = arg;
			while(*tempptr != NULL){
				printf("%s\n", *tempptr);
				tempptr++;
			}
			*/
			if(pro == '1'){
				strcpy(c1, commond);
				if(child1 != -1){
					kill(child1, SIGKILL);
				}
				child1 = fork();
				execute(arg, &child1);
			}
			else if(pro== '2'){
				strcpy(c2, commond);
				if(child2 != -1){
					kill(child2, SIGKILL);
				}
				child2 = fork();
				execute(arg, &child2);
			}
			else if(pro== '3'){
				strcpy(c3, commond);
				if(child3 != -1){
					kill(child3, SIGKILL);
				}
				child3 = fork();
				execute(arg, &child3);
			}
			else{
				strcpy(c4, commond);
				if(child4 != -1){
					kill(child4, SIGKILL);
				}
				child4 = fork();
				execute(arg, &child4);
			}
		}
		
		if(buffer[0] == 'w'){
			char commond[2048];
			pro = buffer[2];
			int i;
			for(i = 4; i < strlen(buffer); i++){
				commond[i-4] =buffer[i];
			}
			commond[i] = '\0';
			writeTofile(commond, pro);
			arg = parse(commond);
			if(pro == '1'){
				strcpy(c1, commond);
				if(child1 != -1){
					waitpid(child1, &status, 0);
				}
				child1 = fork();
				execute(arg, &child1);
			}
			else if(pro=='2'){
				strcpy(c2, commond);
				if(child2 != -1){
					waitpid(child2, &status, 0);
				}
				child2 = fork();
				execute(arg, &child2);
			}
			else if(pro=='3'){
				strcpy(c3, commond);
				if(child3 != -1){
					waitpid(child3, &status, 0);
				}
				child3 = fork();
				execute(arg, &child3);
			}
			else{
				strcpy(c4, commond);
				if(child4 != -1){
					waitpid(child4, &status, 0);
				}
				child4 = fork();
				execute(arg, &child4);
			}
		
		}
		
		
		if(buffer[0] == 'z'){
			if(buffer[0] == 1){
				if(child1 == 0){
					kill(child1, SIGSTOP);
				}
			}
			if(buffer[0] == 2){
				if(child2 == 0){
					kill(child2, SIGSTOP);
				}
			}
			if(buffer[0] == 3){
				if(child3 == 0){
					kill(child3, SIGSTOP);
				}
			}
			if(buffer[0] == 4){
				if(child4 == 0){
					kill(child4, SIGSTOP);
				}
			}
		}
		
		if(buffer[0] == 'g'){
			if(buffer[0] == 1){
				kill(child1, SIGCONT);
			}
			if(buffer[0] == 2){
				kill(child2, SIGCONT);
			}
			if(buffer[0] == 3){
				kill(child3, SIGCONT);
			}
			if(buffer[0] == 4){
				kill(child4, SIGCONT);
			}
		}
		
		if(buffer[0] == 'i'){
			
			int status;
			int s1 = waitpid(child1, &status, WNOHANG);
			int u = getrusage(child1, ru_utime);
			int s = getrusage(child1, ru_stime);
			char sta[2048];			
			if(s1 == 0)
				strcpy(sta, "running");
			if(s1 == -1)
				strcpy(sta, "error");
			if(s1 == child1)
				strcpy(sta, "normally exit");	
			if(strlen(c1) == 0){
				printf("1 not using\n");
			}
			else{
				printf("1 %s %s, %d user, %d system\n", c1, sta, u, s);
			}
			
			
			int s2 = waitpid(child2, &status, WNOHANG);
			u = getrusage(child2, ru_utime);
			s = getrusage(child2, ru_stime);		
			if(s2 == 0)
				strcpy(sta, "running");
			if(s2 == -1)
				strcpy(sta, "error");
			if(s2 == child2)
				strcpy(sta, "normally exit");	
			if(strlen(c2) == 0){
				printf("2 not using\n");
			}
			else{
				printf("2 %s %s, %d user, %d system\n", c2, sta, u, s);
			}
			
			int s3 = waitpid(child3, &status, WNOHANG);
			u = getrusage(child3, ru_utime);
			s = getrusage(child3, ru_stime);	
			if(s3 == 0)
				strcpy(sta, "running");
			if(s3 == -1)
				strcpy(sta, "error");
			if(s3 == child3)
				strcpy(sta, "normally exit");	
			if(strlen(c3) == 0){
				printf("3 not using\n");
			}
			else{
				printf("3 %s %s, %d user, %d system\n", c3, sta, u, s);
			}
			
			
			int s4 = waitpid(child4, &status, WNOHANG);
			u = getrusage(child4, ru_utime);
			s = getrusage(child4, ru_stime);
			if(s3 == 0)
				strcpy(sta, "running");
			if(s3 == -1)
				strcpy(sta, "error");
			if(s3 == child3)
				strcpy(sta, "normally exit");	
			if(strlen(c3) == 0){
				printf("4 not using\n");
			}
			else{
				printf("4 %s %s, %d user, %d system\n", c4, sta, u, s);
			}
		}
		
		if(buffer[0] == 'l'){
			char temp[2048];
			if(buffer[2] == '1'){
				FILE* f = fopen("child1.txt", "r+");
				while(!feof(f)){
					fgets(temp, 2047, f);
					printf("%s", temp);
				}
			
			}
			if(buffer[2] == '2'){
				FILE* f = fopen("child2.txt", "r+");
				while(!feof(f)){
					fgets(temp, 2047, f);
					printf("%s", temp);
				}
			}
			if(buffer[2] == '3'){
				FILE* f = fopen("child3.txt", "r+");
				while(!feof(f)){
					fgets(temp, 2047, f);
					printf("%s", temp);
				}
			}
			else{
				FILE* f = fopen("child4.txt", "r+");
				while(!feof(f)){
					fgets(temp, 2047, f);
					printf("%s", temp);
				}
			}
			
		}
		if(buffer[0] == 'o'){
			
		}
		
	}
}

char ** parse(char * buffer){
	
	char ** arg = NULL;
	if(buffer[strlen(buffer)-1] == '\n'){
		buffer[strlen(buffer)-1] = '\0';
	}
	//printf("%s\n", buffer);
	char * p = strtok(buffer, " \"");//
	int m = 0;
	while(p){
		arg = realloc(arg, sizeof(char *) * ++m);
		arg[m-1] = p;
		p = strtok(NULL, " \"");//		
	}
			
	arg = realloc(arg, sizeof(char*)*(m+1));
	arg[m] = NULL;
	
	return arg;
}

void execute(char ** arg, pid_t * child){
	if(*child == -1){
		printf("ERROR: not a valid child\n");
		exit(1);
	}
	if(*child > 0){
		return;
	}
	if(*child == 0){
			if(execvp(arg[0], arg) < 0){
				printf("ERROR: exec failed\n");
				exit(1);
			}		
	}
}

void writeTofile(char * buffer, int pro){
	//printf("%c", pro);
	if(pro == '1'){
		FILE * f = fopen("child1.txt", "r+");
		fprintf(f, buffer);
	}
	else if(pro == '2'){
		FILE * f = fopen("child2.txt", "r+");
		fprintf(f, buffer);
	}
	else if(pro == '3'){
		FILE * f = fopen("child3.txt", "r+");
		fprintf(f, buffer);
	}
	else{
		FILE * f = fopen("child4.txt", "r+");
		fprintf(f, buffer);
	}

}

