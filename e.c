#include <stdio.h>


void fun(){
	char *p = malloc(10);
	strcpy(p, "aaaa");
	free(p);
	
	strcpy(p, "bbbb");
	
}
