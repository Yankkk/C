#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void Print(FILE *);

void WritetoFile(FILE *, FILE *);

int main(int argc, char* argv[]){
	//printf("%s", "my cat!");
	FILE* ff;
	if(argc < 2){
		printf("Not enough arguments!\n");
	}
	else if(argc == 2){
		ff = fopen(argv[1], "r+");
		Print(ff);
		fclose(ff);
	}
	else{
		int i;
		int result = -1;
		int index = 0;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], ">") == 0){
				result = 0;
				index = i;
			}			
		}
		if(result == 0){
			FILE * dest = fopen(argv[index+1], "w");
			for(i = 1; i < index; i++){
				FILE * ff = fopen(argv[i], "r+");
				WritetoFile(ff, dest);
			}
			fclose(dest);
		}
		else if(result == -1){
			for(i = 1; i < argc; i++){
				FILE * ff = fopen(argv[i], "r+");
				Print(ff);
				fclose(ff);
			}
		}
	}
	
	return 0;
}


void Print(FILE * ff){
	while(!feof(ff)){
		char line[2048];
		fgets(line, sizeof(line), ff);
		
		fprintf( stdout, "%s", line );
	}
}

void WritetoFile(FILE * ff, FILE * dest){
	
	while(!feof(ff)){
		char line[2048];
		fgets(line, sizeof(line), ff);
		fprintf(dest, line);
		
	}
}
