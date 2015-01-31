#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

int main(int argc, char * argv[]){
	
	FILE * ff;
	char **content = (char ** )malloc(100 * sizeof(char *));
	//job_ptr j = calloc(1, sizeof(struct job));
	ff = fopen("Fakefile.txt", "r+");
	if (argc < 2)
	{
		printf("Not enough arguements!\n");
		return 0;
	}
	//printf("%s", argv[1]);
	char name[2048];
	strcpy(name, argv[1]);
	//printf("%d", strlen(name));
	int i;
	for(i = 0; i < 100; i++){
		if(!feof(ff)){
			char line[2048];
			if(fgets(line, sizeof(line), ff)){
				//printf("%s", line);
				content[i] = (char *)malloc(2048 * sizeof(char));
				strcpy(content[i], line);
				//printf("%s", content[i]);
			}
			
		}
		else{
			break;
		}
	}
	
	
	
	//argv[1]
	int index = -1;
	
	int k;
	for(k = 0; k < 100; k ++){
		
		if(strlen(name) == (strlen(content[k]) - 2)){
			//printf("aaa");
			if(strstr(content[k], name) != NULL){
				index = k;
				printf("Found %s target\n", argv[1]);
				break;
			}
		}
	}
	if(index == -1){
		printf("Cannot build %s\n", argv[1]);
		exit(1);
	}
	
	k = index + 1;
	
	int pid = fork();
	//printf("%d", pid);
	if(pid == -1){
		exit(1);
	}
	if(pid > 0){
		int status;
		waitpid(pid, &status, 0);
		/**
		if(status < 0){
			_exit(1);
		}
		*/
		printf("%s was built successfully! \n", name);
		exit(0);
	
	}
	if(pid == 0){
		printf("Running ");
		execl("/yangeng2/ls", "/bin/ls", ".", NULL);
		
		
	}
	
	return 0;
}
