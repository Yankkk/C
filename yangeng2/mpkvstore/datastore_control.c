/** @file datastore_control.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <search.h>

#include "datastore_control.h"
#include "datastore.h"

datastore_t* data = NULL;
void help_action(const void * node, const VISIT which, const int dep);

void init()
{
/*
	data = malloc(sizeof(datastore_t));
	data->size = 12289ul;
	//data->size = 1031ul;
	data->current = 0;
	data->ds = malloc(sizeof(struct _jsonreq_t *)*data->size);
	data->probe = malloc(sizeof(int) * data->size);
	size_t i;
	for(i = 0; i < data->size; i++){
		data->ds[i] = NULL;
		data->probe[i] = 0;
	}
	*/
	data = malloc(sizeof(datastore_t));
	datastore_init(data);
	
}
/*
void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
       int munmap(void *addr, size_t length);
*/

void load()
{
	 if(data == NULL){
	 	init();
	 }
	 struct stat s;
	 if(stat("data/store.txt", &s) != 0)
	 	return;
	 FILE* fd = fopen("data/store.txt", "r");
	 char * buffer = NULL;
	 size_t len = 0;
	 char * key;
	 char * value;
	 int rev;
	 char k[2] = " ";
	 ssize_t r;
	 while(r = getline(&buffer, &len, fd)>0 ){
	 	buffer[len-1] = '\0';
	 	key = strtok(buffer, k);
	 	value = strtok(NULL, k);
	 	rev = atoi(strtok(NULL, k));
	 	datastore_put(data, key, value, rev);
	 }
	 if(r <= 0){
	 	free(buffer);
	 }
	 fclose(fd);

}

/*
void twalk(const void *root, void (*action)(const void *nodep,
                                          const VISIT which,
                                          const int depth));
*/
FILE * FD;
void save()
{

	FD = fopen("data/store.txt", "w");
	//void * addr = mmap();
	/*
	size_t i;
	for(i = 0; i < data->size; i++){
		if(data->ds[i] != NULL){
			fprintf(fd, "%s %s %ld\n", data->ds[i]->key, data->ds[i]->value, data->ds[i]->rev);
		}
	}
	*/
	twalk(data->root, help_action);
	datastore_destroy(data);
	free(data);
	data = NULL;
	fclose(FD);

}

void help_action(const void * node, const VISIT which, const int dep){
	datastore_entry_t * temp = *(datastore_entry_t **) node;
	char str[1024];
	 switch (which) {
           case preorder:
           		fprintf(FD, "%s %s %ld\n", temp->key, temp->value, temp->rev);
           		/*
           	   sprintf(str, "%s %s %ld\n", temp->key, temp->value, temp->rev);
               fwrite( str, 1, strlen(str), FD);
               */
               break;
           case postorder:
              	
               //fprintf(FD, "%s %s %ld\n", temp->key, temp->value, temp->rev);
               /*
               sprintf(str, "%s %s %ld\n", temp->key, temp->value, temp->rev);
               fwrite( str, 1, strlen(str), FD);
               */
               
               break;
           case endorder:
               break;
           case leaf:
				/*           
               sprintf(str, "%s %s %ld\n", temp->key, temp->value, temp->rev);
               fwrite( str, 1, strlen(str), FD);
               */
               fprintf(FD, "%s %s %ld\n", temp->key, temp->value, temp->rev);
               break;
           }

}




jsonres_t process_request(const char * uri, jsonreq_t request)
{
    jsonres_t response;
    
    if (uri)
    {
	response.rev = 0;
	response.value = "0";
	response.success = "true";
    }
    /*
    char * key = malloc(1024);
    memcpy(key, request.key, strlen(request.key)+1);
    char * value = malloc(1024);
    memcpy(value, request.value, strlen(request.key)+1);
    */
    if(strcmp(uri, "/add") == 0){
    	long rev = datastore_put(data, request.key, request.value, 1);
		//long rev = datastore_put(data, key, value, 1);
    	if(rev == 0){
    		response.success = "KEY ALREADY EXISTS";
    	}
    	
    	//else{
    		//add(request.key, request.value, 1, data);
    		//response.success = "true";	
    		//response.value = request.value;
    		//response.rev = rev;	
    	//}
    	//datastore_entry_t *entry = dictionary_tfind(data, key);
		//printf("add: %s %s %d\n", entry->value, entry->key, entry->rev);
    }
    if(strcmp(uri, "/update") == 0){
    	long rev = datastore_update(data, request.key, request.value, request.rev);
    	//long rev = datastore_update(data, key, value, request.rev);
    	if(rev >= 1){
    		response.rev = rev;
    		//response.value = request.value;
    		
    	}
    	//datastore_entry_t *entry = dictionary_tfind(data, "0");
		//printf("add: %s %s %d\n", entry->value, entry->key, entry->rev);
    	if(rev == -2){
    		response.success = "REVISION NUMBER DOES NOT MATCH";
    	}
    	if(rev == 0){
    		response.success = "KEY DOES NOT EXIST";
    	}
    	
    }
    
 
    if(strcmp(uri, "/get") == 0){
    	long rev;
    	const char* val = datastore_get(data, request.key, &rev);
    	
    	if(val != NULL){
    		response.rev = rev;
    		//response.value = malloc(strlen(val)+1);
    		//memcpy(response.value, val, strlen(val)+1);
    		response.value = val;
    		//printf("get %s %s %d\n", data->ds[k]->value, temp->key, temp->rev);
    	}
    	else{
    		response.success = "KEY DOES NOT EXIST";
    		response.value = NULL;
    	}
    }
    
    if(strcmp(uri, "/delete") == 0){
		long temp = datastore_delete(data, request.key);
    	if(temp == 0){
    		response.success = "KEY DOES NOT EXIST";
    	}
    }
    return response;
}
