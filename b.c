#include <stdio.h>

#define N (5)
int main(){
	int i = N, array[N];
	for(;i >= 0; i--)
		array[i] = i; // index out of bound

	return 0;
}
