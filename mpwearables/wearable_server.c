#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <pthread.h>

#include "queue.h"

const char* TYPE1 = "heart_beat";
const char* TYPE2 = "blood_sugar";
const char* TYPE3 = "body_temp";

//the wearable server socket, which all wearables connect to
int wearable_server_fd;

//a lock for your queue sctructure... (use it)
pthread_mutex_t queue_lock_;
//a queue for all received data... 
queue_t receieved_data_;

//



typedef struct SampleData {

	char type_[50];
	int data_;

} SampleData;

int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

/**
Used to write out the statistics of a given results set (of timestamp_entry's).
To generate the result set see queue_gather(). fd is the file descriptor to
which the information is sent out. The type is the type of data that is written out
(TYPE1, TYPE2, TYPE3). results is the array of timestamp_entrys, and size is 
the size of that array. NOTE: that you should call method for every type 
(TYPE1, TYPE2, TYPE3), and then write out the infomration "\r\n" to signify that
you have finished sending out the results.
*/
void write_results(int fd, const char* type, timestamp_entry* results, int size) {
    long avg = 0;
    int i;

    char buffer[1024];
    int temp_array[size];
    sprintf(buffer, "Results for %s:\n", type);
    sprintf(buffer + strlen(buffer), "Size:%i\n", size);
    for (i = 0;i < size;i ++) {
        temp_array[i] = ((SampleData*)(results[i].data_))->data_;
        avg += ((SampleData*)(results[i].data_))->data_;
    }

    qsort(temp_array, size, sizeof(int), compare);

    if (size != 0) {
    	sprintf(buffer + strlen(buffer), "Median:%i\n", (size % 2 == 0) ?
            (temp_array[size / 2] + temp_array[size / 2 - 1]) / 2 : temp_array[size / 2]);
	} else { 
		sprintf(buffer + strlen(buffer), "Median:0\n");
	}

    sprintf(buffer + strlen(buffer), "Average:%li\n\n", (size == 0 ? 0 : avg / size));
    write(fd, buffer, strlen(buffer));
}

/**
Given an input line in the form <timestamp>:<value>:<type>, this method 
parses the infomration from the string, into the given timestamp, and
mallocs space for SampleData, and stores the type and value within
*/
void extract_key(char* line, long* timestamp, SampleData** ret) {
	*ret = malloc(sizeof(SampleData));
	sscanf(line, "%zu:%i:%[^:]%:\\.*", timestamp, &((*ret)->data_), (*ret)->type_);
}

unsigned long *data_array;
int total;
int num;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void* wearable_processor_thread(void* args) {
	//int socketfd = *((int*)args);
	int socketfd = (int)args;
	//Use a buffer of length 64!
	//TODO read data from the socket until -1 is returned by read
	
	//realloc data_array by +1
	pthread_mutex_lock(&queue_lock_);
	int current = num;
	int count=0;
	data_array=realloc(data_array,sizeof(long)*(num+1));
	num++;
	pthread_mutex_unlock(&queue_lock_);

	char buffer[64];
	while (recv(socketfd, buffer, 64,0) > 0){
		//printf(" length: %d",length);
		unsigned long timestamp;
		SampleData *ret;
		extract_key(buffer,&timestamp,&ret);
		pthread_mutex_lock(&queue_lock_);
		queue_insert(&receieved_data_,(unsigned long)timestamp,ret);
		total++;
		pthread_mutex_unlock(&queue_lock_);	
		data_array[current]=timestamp;
		// add the currnt entry to the data_array
		pthread_cond_broadcast(&cv);	
		count++;
	}
	
	close(socketfd);
	data_array[current]=-1;

	pthread_cond_broadcast(&cv);
	return NULL;
}

int time_send_data(long* data_array,int num, long final_timestamp){
	int i;
	for(i=0;i<num;i++){
		if(data_array[i]!=-1 && data_array[i]<final_timestamp){
			return 0;
		} 
	}
	return 1;
}

int selector1(void* entry){
	SampleData* ret =(SampleData*)entry;
	if(strcmp(TYPE1,ret->type_)==0){
		return 1;
	}
	return 0;
}

int selector2(void* entry){
	SampleData* ret =(SampleData*)entry;
	if(strcmp(TYPE2,ret->type_)==0){
		return 1;
	}
	return 0;
}

int selector3(void* entry){
	SampleData* ret =(SampleData*)entry;
	if(strcmp(TYPE3,ret->type_)==0){
		return 1;
	}
	return 0;
}

	
//pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;

void* user_request_thread(void* args) {
	int socketfd = *((int*)args);
	//int i, ret;
	//TODO rread data from the socket until -1 is returned by read
	//Requests will be in the form
	//<timestamp1>:<timestamp2>, then write out statiticcs for data between
	//those timestamp ranges
	//
	//before you send data, check timestamp array
	char buffer[1024];
	while(recv(socketfd,buffer,1024,0)>0){
		unsigned long start;
		unsigned long end;
		sscanf(buffer,"%lu:%lu",&start, &end);
		pthread_mutex_lock(&queue_lock_);
		while(time_send_data(data_array,num,end)!=1){
			pthread_cond_wait(&cv,&queue_lock_);
		}
		//printf("the size of data: %d\n",total);
		int size1,size2,size3;
		timestamp_entry* results1 = queue_gather(&receieved_data_,(unsigned long)start,(unsigned long)end, selector1,&size1);
		timestamp_entry* results2 = queue_gather(&receieved_data_,(unsigned long)start,(unsigned long)end, selector2,&size2);
		timestamp_entry* results3 = queue_gather(&receieved_data_,(unsigned long)start,(unsigned long)end, selector3,&size3);
		write_results(socketfd,TYPE1,results1,size1);
		write_results(socketfd,TYPE2,results2,size2);
		write_results(socketfd,TYPE3,results3,size3);
		write(socketfd,"\r\n",2);
		pthread_mutex_unlock(&queue_lock_);
		
		free(results1);
		free(results2);
		free(results3);
	}
	close(socketfd);
	return NULL;
}

//IMPLEMENT!
//given a string with the port value, set up a 
//serversocket file descriptor and return it
int open_server_socket(const char* port) {
	int s, sock_fd;
	sock_fd = socket(AF_INET,SOCK_STREAM,0);

	struct addrinfo hints, *result;
	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	s = getaddrinfo(NULL,port,&hints,&result);
	if(s!=0){
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(s));
		exit(1);	
	}
	if(sock_fd == -1){
		perror("socket");
		exit(1);
	}
	int optval = 1;
	setsockopt(sock_fd,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
	if(bind(sock_fd,result->ai_addr,result->ai_addrlen)!=0){
		perror("bind");
		exit(1);
	}
	if(listen(sock_fd,40)){
		perror("listen()");
		exit(1);
	}
	free(result);
	return sock_fd;
}

int thread_num;
pthread_t* tid;

void signal_received(int sig) {
	//TODO close server socket, free anything you dont free in main
	// free()
	close(wearable_server_fd);
	pthread_mutex_destroy(&queue_lock_);
	pthread_cond_destroy(&cv);
	free(data_array);
}

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		printf("Invalid input size\n");
		exit(EXIT_FAILURE);
	}
	
	//TODO setup sig handler for SIGINT
	signal(SIGINT,signal_received);	
	int request_server_fd = open_server_socket(argv[2]);
	wearable_server_fd = open_server_socket(argv[1]);

	pthread_t request_thread;
	int request_socket = accept(request_server_fd, NULL, NULL);
	pthread_create(&request_thread, NULL, user_request_thread, &request_socket);
	close(request_server_fd);

	queue_init(&receieved_data_);
	pthread_mutex_init(&queue_lock_, NULL);
	
	//TODO accept continous requests
	tid = malloc(1);
	data_array=malloc(1);
	while(1){
		int client_fd = accept(wearable_server_fd,NULL,NULL);
		if(client_fd ==-1){
			break;
		}
		tid = realloc(tid,sizeof(pthread_t)*(thread_num+1));
		pthread_create(&tid[thread_num],NULL,wearable_processor_thread,(void*)client_fd);
		thread_num++;
	}
	int i;
	//TODO join all threads we spawned from the wearables
	for(i=0;i<thread_num;i++){
		pthread_join(tid[i],NULL);
	}
	free(tid);
	pthread_join(request_thread, NULL);
	queue_destroy(&receieved_data_, 1);
	
	return 0;
}
