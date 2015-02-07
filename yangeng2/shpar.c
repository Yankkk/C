#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

int main(int argc, char * argv[]){
	char buffer[2048];
	char* address;
	char* ins;
	
	while(1){
		fgets(buffer, sizeof(buffer), stdin);
		if(buffer[0] == 'q'){
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
			sscanf(buffer, "%s %s", ins, address);
			chdir(address);
		}
		if(buffer[0] == 'r'){
		}
		if(buffer[0] == 'w'){
		
		}
		if(buffer[0] == 'i'){
		
		}

	}
}
