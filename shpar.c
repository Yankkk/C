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
void save(list *, char *, list *);

void save(list * lists, char * data, list * head){
	if(lists == NULL){
		lists = (list*)malloc(sizeof(list));
		lists->previous = NULL;
		lists->next = NULL;
		strcpy(lists->data, data);
		head = lists;
		printf("%s", head->data);
	}
	else{
		list * temp = (list*)malloc(sizeof(list));
		temp->previous = lists;
		temp->next = NULL;
		strcpy(temp->data, data);
		lists->next = temp;
		lists = temp;
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
	char c1[2048];
	char c2[2048];
	char c3[2048];
	char c4[2048];
	
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
			char t[2048];
			strcpy(t, commond);
			
			arg = parse(commond);
			/*
			char ** tempptr = arg;
			while(*tempptr != NULL){
				printf("%s\n", *tempptr);
				tempptr++;
			}
			*/
			if(pro == '1'){
				
				if(list1 == NULL){
					list1 = (list*)malloc(sizeof(list));
					list1->previous = NULL;
					list1->next = NULL;
					strcpy(list1->data, t);
					head1 = list1;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list1;
					temp->next = NULL;
					strcpy(temp->data, t);
					list1->next = temp;
					list1 = temp;
				}
			
				if(child1 != -1){
					kill(child1, SIGKILL);
				}
				child1 = fork();
				execute(arg, &child1);
			}
			else if(pro== '2'){
				if(list2 == NULL){
					list2 = (list*)malloc(sizeof(list));
					list2->previous = NULL;
					list2->next = NULL;
					strcpy(list2->data, t);
					head2 = list2;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list2;
					temp->next = NULL;
					strcpy(temp->data, t);
					list2->next = temp;
					list2 = temp;
				}
				if(child2 != -1){
					kill(child2, SIGKILL);
				}
				child2 = fork();
				execute(arg, &child2);
			}
			else if(pro== '3'){
				if(list3 == NULL){
					list3 = (list*)malloc(sizeof(list));
					list3->previous = NULL;
					list3->next = NULL;
					strcpy(list3->data, t);
					head3 = list3;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list3;
					temp->next = NULL;
					strcpy(temp->data, t);
					list3->next = temp;
					list3 = temp;
				}
			
				if(child3 != -1){
					kill(child3, SIGKILL);
				}
				child3 = fork();
				execute(arg, &child3);
			}
			else{
				if(list4 == NULL){
					list4 = (list*)malloc(sizeof(list));
					list4->previous = NULL;
					list4->next = NULL;
					strcpy(list4->data, t);
					head4 = list4;
					printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list4;
					temp->next = NULL;
					strcpy(temp->data, t);
					list4->next = temp;
					list4 = temp;
				}
				
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
			char t[2048];
			strcpy(t, commond);
			
			arg = parse(commond);
			if(pro == '1'){
				if(list1 == NULL){
					list1 = (list*)malloc(sizeof(list));
					list1->previous = NULL;
					list1->next = NULL;
					strcpy(list1->data, t);
					head1 = list1;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list1;
					temp->next = NULL;
					strcpy(temp->data, t);
					list1->next = temp;
					list1 = temp;
				}
				if(child1 != -1){
					waitpid(child1, &status, 0);
				}
				child1 = fork();
				execute(arg, &child1);
			}
			else if(pro=='2'){
				if(list2 == NULL){
					list2 = (list*)malloc(sizeof(list));
					list2->previous = NULL;
					list2->next = NULL;
					strcpy(list2->data, t);
					head2 = list2;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list2;
					temp->next = NULL;
					strcpy(temp->data, t);
					list2->next = temp;
					list2 = temp;
				}
				if(child2 != -1){
					waitpid(child2, &status, 0);
				}
				child2 = fork();
				execute(arg, &child2);
			}
			else if(pro=='3'){
				if(list3 == NULL){
					list3 = (list*)malloc(sizeof(list));
					list3->previous = NULL;
					list3->next = NULL;
					strcpy(list3->data, t);
					head3 = list3;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list3;
					temp->next = NULL;
					strcpy(temp->data, t);
					list3->next = temp;
					list3 = temp;
				}
				if(child3 != -1){
					waitpid(child3, &status, 0);
				}
				child3 = fork();
				execute(arg, &child3);
			}
			else{
				if(list4 == NULL){
					list4 = (list*)malloc(sizeof(list));
					list4->previous = NULL;
					list4->next = NULL;
					strcpy(list4->data, t);
					head4 = list4;
					//printf("%s", head1->data);
				}
				else{
					list * temp = (list*)malloc(sizeof(list));
					temp->previous = list4;
					temp->next = NULL;
					strcpy(temp->data, t);
					list4->next = temp;
					list4 = temp;
				}
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
				printf("1 %s %s, %d user, %d system\n", list1->data, sta, u, s);
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
				printf("2 %s %s, %d user, %d system\n", list2->data, sta, u, s);
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
				printf("3 %s %s, %d user, %d system\n", list3->data, sta, u, s);
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
				printf("4 %s %s, %d user, %d system\n", list4->data, sta, u, s);
			}
		}
		
		if(buffer[0] == 'l'){
			char temp[2048];
			if(buffer[2] == '1'){
				list * temp = head1;
				while(temp != NULL){
					printf("%s", temp->data);
					//printf("yeah!\n");
					temp = temp->next;
				}
			
			}
			
			if(buffer[2] == '2'){
				list * temp = head2;
				while(temp != NULL){
					printf("%s", temp->data);
					//printf("yeah!\n");
					temp = temp->next;
				}
			}
			if(buffer[2] == '3'){
				list * temp = head3;
				while(temp != NULL){
					printf("%s", temp->data);
					//printf("yeah!\n");
					temp = temp->next;
				}
			}
			else{
				list * temp = head4;
				while(temp != NULL){
					printf("%s", temp->data);
					//printf("yeah!\n");
					temp = temp->next;
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


