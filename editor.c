#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct job{
	
	char content[10][2048];
	FILE * ff;
	//int ff;
}*job_ptr;

void Write(job_ptr);
void append(job_ptr);
void Print(job_ptr);
void find(job_ptr);
void reverse(job_ptr);
void change(job_ptr);
void encrypt(job_ptr);
void decrypt(job_ptr);

int main(int argc, char * argv[]){
	
//	char name[2048];
//	scanf("%s", &name);
	//printf("%s", name);
	job_ptr j = calloc(1, sizeof(struct job));
	if (argc < 2)
	{
		printf("Not enough arguements!\n");
		return 0;
	}
	j->ff = fopen(argv[1], "r+");
	if (j->ff == NULL)
		j->ff = fopen(argv[1], "w");
	//mode_t mode = S_IRUSR | S_IWUSR;
	
	//FILE file = fopen(name, O_CREAT | O_TRUNC | O_RDWR, mode);
	
	int i;
	for(i = 0; i < 10; i++){
		char line[2048];
		if(fgets(line, sizeof(line), j->ff)){
			//printf("%s", line);
			strcpy(j->content[i], line);
		}
		else{
			strcpy(j->content[i], "");
		}	
	}
	fclose(j->ff);
	j->ff = fopen(argv[1], "w");
	while(1){
		char c = getchar();
		
		switch(c){
			case 'p' : Print(j); break;
			case 'a' : append(j); break;
			case 'f' : find(j); break;
			case 'w' : Write(j); break;
			case 'r' : reverse(j); break;
			case 'e' : encrypt(j); break;
			case 'd' : decrypt(j); break;
			case 'q' : fclose(j->ff); return 0;
			
		}
	}
	return 0;
}

void encrypt(job_ptr j){
	int i;
	int k;
	for(i = 0; i < 10; i++){
		if(strlen(j->content[i]) > 0){
			for(k = 0; k < strlen(j->content[i])-1; k++){
				j->content[i][k] += 3;
			}
		}
	}
}

void decrypt(job_ptr j){
	int i;
	int k;
	for(i = 0; i < 10; i++){
		if(strlen(j->content[i]) > 0){
			for(k = 0; k < strlen(j->content[i])-1; k++){
				j->content[i][k] -= 3;
			}
		}
	}

}

void reverse(job_ptr j){
	int index;
	scanf("%d", &index);
	int i = strlen(j->content[index-1]);
	int k = 0;
	char temp[2048];
	if(i > 1){
		for(i = i-2; i >= 0; i --){
			temp[k] = j->content[index-1][i];
			k++ ;
		}
	}
	temp[k] = '\n';
	temp[k+1] = '\0';
	strcpy(j->content[index-1], temp);
}

void Print(job_ptr j){
	int i;
	for(i = 0; i < 10; i++){
		if(strlen(j->content[i]) == 0){
			printf("%d\n", i+1);
		}
		else{
			printf("%d %s", i+1, j->content[i]);
		}
	}
}

void append(job_ptr j){
	int index;
	char temp[2048];
	scanf("%d %s", &index, &temp);
	/*
	scanf("%d", &index);
	//char * temp;
	//size_t len = 0;
	getline(&temp, &len, stdin);
	//printf("%s", temp);
	*/
	if(index > 10 || index <= 0)
		printf("Index Invalid");
	else{
		if(strlen(j->content[index-1]) == 0){
			strcat(j->content[index-1], temp);
			strcat(j->content[index-1], "\n");
			//printf("%d", 123);
		}
		else{
			char t[2048];
			strncpy(t, j->content[index-1], strlen(j->content[index-1])-1);
			strcpy(j->content[index-1], t);
			strcat(j->content[index-1], temp);
			strcat(j->content[index-1], "\n");
		}
	}
}

void find(job_ptr j){
	char find[2048];
	scanf("%s", &find);
	int i;
	for(i = 0; i < 10; i ++){
		if(strlen(find) <= strlen(j->content[i])){
			if(strstr(j->content[i], find) != NULL){
				printf("%d %s", i+1, j->content[i]);
			}
		}
	}
}
void Write(job_ptr j){
	int i;
	for(i = 0; i < 10; i++){
		//write(j->ff, j->content[i]);
		int l = strlen(j->content[i]);
		if(l>=1){
			//j->content[i][l] = '\0'; 
			//j->content[i][l-1] = '\n';
			
			fprintf(j->ff, j->content[i]);
			//fprintf(j->ff,"\n");
		}
		else{
			fprintf(j->ff,"\n");	
			}
		}
}
