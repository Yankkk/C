#include <stdio.h>


int *fun(){
	int re = 50;
	static int mo;
	return &re;
	
}
