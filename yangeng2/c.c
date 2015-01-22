#include <stdio.h>


int *fun(){
	int re = 50;
	static int mo;
	return &re; //returning the addredd of a local variable
	
}
