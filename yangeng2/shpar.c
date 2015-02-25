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




typedef struct linkedlist{
	char data[1000];
	struct linkedlist * next;
	struct linkedlist * previous;
	//linkedlist * tail;
	
}list;

char ** parse(char *);
void execute(char **, pid_t *);
void save(list **, char *, list **);

void save(list ** lists, char * data, list ** head){
	if((*lists) == NULL){
		(*lists) = (list*)malloc(sizeof(list));
		(*lists)->previous = NULL;
		(*lists)->next = NULL;
		strcpy((*lists)->data, data);
		(*head) = (*lists);
		//printf("%s", (*head)->data);
	}
	else{
		list * temp = (list*)malloc(sizeof(list));
		temp->previous = (*lists);
		temp->next = NULL;
		strcpy(temp->data, data);
		(*lists)->next = temp;
		(*lists) = temp;
				}

}

int main(int argc, char * argv[]){
	char* buffer = (char*)malloc(2048*sizeof(char));
	char address[100];
	char ins[100];
	char commond[2048];
	char** arg = NULL;
	char pro;
	int status;
	struct rusage usage1;
	struct rusage usage2;
	struct rusage usage3;
	struct rusage usage4;
	pid_t child1 = -1;
	pid_t child2 = -1;
	pid_t child3 = -1;
	pid_t child4 = -1;
	
	list * list1 = NULL;
	list * list2 = NULL;
	list * list3 = NULL;
	list * list4 = NULL;
	list * head1 = NULL;
	list * head2 = NULL;
	list * head3 = NULL;
	list * head4 = NULL;
	int s1 = -1;
	int s2 = -1;
	int s3 = -1;
	int s4 = -1;
	
	while(1){
		fgets(buffer, 2047, stdin);
		//close(1);
		
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
			//printf("%s\n", buffer);
			commond[i] = '\0';
			//printf("%s\n", commond);
			char t[2048];
			int m;
			for(m = 0; m < strlen(commond); m++){
				if(commond[m-1] == '\n')
					break;
				t[m] = commond[m];
			}
			
			//strncpy(t, commond, i);
			t[m] = '\0';
			//printf("%s\n", t);
			arg = parse(commond);
			/*
			char ** tempptr = arg;
			while(*tempptr != NULL){
				printf("%s\n", *tempptr);
				tempptr++;
			}
			*/
			int stdout_cpy = dup(1);
			close(1);
			int files;
			if(pro == '1'){
				files = open("p1.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list1, t, &head1);
				getrusage(RUSAGE_SELF, &usage1);
				if(child1 != -1){
					kill(child1, SIGKILL);
				}
				child1 = fork();
			
				//wait3(&status, 0, &usage1);
				//execute(arg, &child1);
				if(child1 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
				
			}
			else if(pro== '2'){
				files = open("p2.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				//printf("%s\n", t);
				save(&list2, t, &head2);
				getrusage(RUSAGE_SELF, &usage2);
				if(child2 != -1){
					kill(child2, SIGKILL);
				}
				child2 = fork();
				//execute(arg, &child2);
				if(child2 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
			else if(pro== '3'){
				files = open("p3.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list3, t, &head3);
				getrusage(RUSAGE_SELF, &usage3);
				if(child3 != -1){
					kill(child3, SIGKILL);
				}
				child3 = fork();
				//execute(arg, &child3);
				if(child3 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
			else if(pro == '4'){
				files = open("p4.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list4, t, &head4);
				getrusage(RUSAGE_SELF, &usage4);
				if(child4 != -1){
					kill(child4, SIGKILL);
				}
				child4 = fork();
				//execute(arg, &child4);
				if(child4 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
			close(files);
			dup2(stdout_cpy, 1);
		}
		
		if(buffer[0] == 'w'){
			char commond[2048];
			pro = buffer[2];
			int i;
			for(i = 4; i < strlen(buffer); i++){
				commond[i-4] =buffer[i];
			}
			commond[i] = '\0';
			char t[2048];
			strcpy(t, commond);
			
			arg = parse(commond);
			int files;
			int stdout_cpy = dup(1);
			close(1);
			if(pro == '1'){
			
				files = open("p1.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list1, t, &head1);
				getrusage(RUSAGE_SELF, &usage1);
				if(child1 != -1){
					waitpid(child1, &status, 0);
				}
				child1 = fork();
				//execute(arg, &child1);
				if(child1 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
			else if(pro=='2'){
				files = open("p2.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list2, t, &head2);
				getrusage(RUSAGE_SELF, &usage2);
				if(child2 != -1){
					waitpid(child2, &status, 0);
				}
				child2 = fork();
				//execute(arg, &child2);
				if(child1 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
			else if(pro=='3'){
				files = open("p3.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list3, t, &head3);
				getrusage(RUSAGE_SELF, &usage3);
				if(child3 != -1){
					waitpid(child3, &status, 0);
				}
				child3 = fork();
				//execute(arg, &child3);
				if(child1 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
			else if(pro == '4'){
				files = open("p4.txt", O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
				save(&list4, t, &head4);
				getrusage(RUSAGE_SELF, &usage4);
				if(child4 != -1){
					waitpid(child4, &status, 0);
				}
				child4 = fork();
				//execute(arg, &child4);
				if(child1 == 0){
					execl("/bin/sh", "sh", "-c", buffer+4, (char*)0);
					perror("error\n");
				}
			}
		
		}
		
		
		if(buffer[0] == 'z'){
			if(buffer[2] == '1'){
				if(child1 > 0){
					int status = waitpid(child1, &status, WNOHANG);
					if(status == 0 && status != -1){
						kill(child1, SIGSTOP);
					}
				}
			}
			if(buffer[2] == '2'){
				if(child2 > 0){
					int status = waitpid(child2, &status, WNOHANG);
					if(status == 0 && status != -1){
						kill(child2, SIGSTOP);
					}
				}
			}
			if(buffer[2] == '3'){
				if(child3 > 0){
					int status = waitpid(child3, &status, WNOHANG);
					if(status == 0 && status != -1){
						kill(child3, SIGSTOP);
					}
				}
			}
			if(buffer[2] == '4'){
				if(child4 > 0){
					int status = waitpid(child4, &status, WNOHANG);
					if(status == 0 && status != -1){
						kill(child4, SIGSTOP);
					}
				}
			}
			
		}
		
		if(buffer[0] == 'g'){
			if(buffer[2] == '1'){
				if(child1 > 0)
					kill(child1, SIGCONT);
			
			}
			if(buffer[2] == '2'){
				if(child2 > 0){
					kill(child2, SIGCONT);
				}
			}
			if(buffer[2] == '3'){
				if(child3 > 0){
					kill(child3, SIGCONT);
				}
			}
			if(buffer[2] == '4'){
				if(child4 > 0){
					kill(child4, SIGCONT);
				}
			}
		}
		
		if(buffer[0] == 'i'){
			
			int status;
			//int s1 = waitpid(child1, &status, WNOHANG);
			if(s1 == -1 || s1 != child1){
				s1 = wait4(child1, &status, WNOHANG, &usage1);
			}
			char sta[2048];	
			//if(child1 <= 0){	
			printf("%d %d\n", child1, s1);	
			if(s1 == 0)
				strcpy(sta, "running");
			if(s1 == -1)
				strcpy(sta, "error");
			if(s1 == child1)
				sprintf(sta, "normally exit (exit value %d)", WEXITSTATUS(s1));
			//}
			if(list1 == NULL){
				printf("1 not using\n");
			}
			
			else{
				printf("1 %s %s, %d.%06ld ms user, %d.%06ld s system\n", list1->data, sta, usage1.ru_utime.tv_sec, usage1.ru_utime.tv_usec, usage2.ru_stime.tv_sec, usage1.ru_stime.tv_usec);
				//printf("1 %s %s, %d.%06lds user, %d.%06lds system\n", list1->data, sta, c, a, d, b);
			}
			
			char sta2[2048];
			//int s2 = waitpid(child2, &status, WNOHANG);	
			//int s2 = wait4(child2, &status, WNOHANG, &usage2);
			
			if(s2 == -1 || s2 != child2){
				s2 = waitpid(child2, &status, WNOHANG);
			}
			if(s2 == 0)
				strcpy(sta2, "running");
			if(s2 == -1)
				strcpy(sta2, "error");
			if(s2 == child2)
				sprintf(sta2, "normally exit (exit value %d)", WEXITSTATUS(s2));
			if(list2 == NULL){
				printf("2 not using\n");
			}
			else{
				printf("2 %s %s, %d.%06lds user, %d.%06lds system\n", list2->data, sta2, usage2.ru_utime.tv_sec, usage2.ru_utime.tv_usec, usage2.ru_stime.tv_sec, usage2.ru_stime.tv_usec);
			}
			
			char sta3[2048];
			if(s3 == -1 || s3 != child3){
				s3 = waitpid(child3, &status, WNOHANG);
			}
			
			if(s3 == 0)
				strcpy(sta3, "running");
			if(s3 == -1)
				strcpy(sta3, "error");
			if(s3 == child3)
				sprintf(sta3, "normally exit (exit value %d)", WEXITSTATUS(s3));
			if(list3 == NULL){
				printf("3 not using\n");
			}
			else{
				printf("3 %s %s, %d.%06lds user, %d.%06lds system\n", list3->data, sta3, usage3.ru_utime.tv_sec, usage3.ru_utime.tv_usec, usage3.ru_stime.tv_sec, usage3.ru_stime.tv_usec);
			}
			
			char sta4[2048];
			if(s4 == -1 || s4 != child4){
				s4 = waitpid(child4, &status, WNOHANG);
			}
			if(s4 == 0)
				strcpy(sta4, "running");
			if(s4 == -1)
				strcpy(sta4, "error");
			if(s4 == child4)
				sprintf(sta4, "normally exit (exit value %d)", WEXITSTATUS(s4));
			if(list4 == NULL){
				printf("4 not using\n");
			}
			else{
				printf("4 %s %s, %d.%06lds user, %d.%06lds system\n", list4->data, sta4, usage4.ru_stime.tv_sec, usage4.ru_utime.tv_usec, usage4.ru_stime.tv_sec, usage4.ru_stime.tv_usec);
			}
			
		}
		
		if(buffer[0] == 'l'){
			char temp[2048];
			if(buffer[2] == '1'){
					list * temp = head1;
					while(temp != NULL){
						printf("%s", temp->data);
						temp = temp->next;
					}
			}
			if(buffer[2] == '2'){
				list * temp = head2;
					//if(temp == NULL)
						//printf("NOTHING!\n");
					while(temp != NULL){
						printf("%s", temp->data);
						temp = temp->next;
					}
				
			}
			if(buffer[2] == '3'){
				list * temp = head3;
					//if(temp == NULL)
					//	printf("NOTHING!\n");
					while(temp != NULL){
						printf("%s\n", temp->data);
						temp = temp->next;
					}
				
			}
			else{
				list * temp = head4;
					//if(temp == NULL)
					//	printf("NOTHING!\n");
					while(temp != NULL){
						printf("%s", temp->data);

						temp = temp->next;
					}
			}
		}
		if(buffer[0] == 'o' && strlen(buffer) > 1){
			char num = buffer[2];
			FILE * file;
			//printf("%c\n", num);
			if(num == '1'){
			
				if(child1 > 0){
					file = fopen("p1.txt", "r");
				}
				else{
					printf("Process not created\n");
				}
			}
			else if(num == '2'){
				if(child2 > 0){
					file = fopen("p2.txt", "r");
				}
				else{
					printf("Process not created\n");
				}
			}
			else if(num == '3'){
				if(child3 > 0){
					file = fopen("p3.txt", "r");
				}
				else{
					printf("Process not created\n");
				}
			}
			else if(num == '4'){
				if(child4 > 0){
					file = fopen("p4.txt", "r");
				}
				else{
					printf("Process not created\n");
				}
			}
			else{
				printf("No such process\n");
			}
			char * line = NULL;
			size_t len = 0;
			while(getline(&line, &len, file) != -1){
				printf("%s", line);
			}
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
		p = strtok(NULL, " \"/");//		
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
			if(arg[0][0] == '.'){
				
			}
			else{
				if(execvp(arg[0], arg) < 0){
					printf("ERROR: exec failed\n");
					exit(1);
				}	
			}	
	}
}


