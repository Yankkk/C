/** @file libmapreduce.c */
/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/epoll.h>

#include "libmapreduce.h"
#include "libds/libds.h"


static const int BUFFER_SIZE = 2048;  /**< Size of the buffer used by read_from_fd(). */

//pthread_t tid;
/**
 * Adds the key-value pair to the mapreduce data structure.  This may
 * require a reduce() operation.
 *
 * @param key
 *    The key of the key-value pair.  The key has been malloc()'d by
 *    read_from_fd() and must be free()'d by you at some point.
 * @param value
 *    The value of the key-value pair.  The value has been malloc()'d
 *    by read_from_fd() and must be free()'d by you at some point.
 * @param mr
 *    The pass-through mapreduce data structure (from read_from_fd()).
 */
static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{

	unsigned long r;
	
	if(datastore_put(mr->ds, key, value)){
		free((void*)value);
		free((void*)key);
	}	
	else{
		const char * v = datastore_get(mr->ds, key, &r);
		const char * new_v = mr->reducefunc(v, value);
		datastore_update(mr->ds, key, new_v, r);
		free((void*)v);
		free((void*)new_v);
		free((void*)key);
		free((void*)value);
	}
	return;
}


/**
 * Helper function.  Reads up to BUFFER_SIZE from a file descriptor into a
 * buffer and calls process_key_value() when for each and every key-value
 * pair that is read from the file descriptor.
 *
 * Each key-value must be in a "Key: Value" format, identical to MP1, and
 * each pair must be terminated by a newline ('\n').
 *
 * Each unique file descriptor must have a unique buffer and the buffer
 * must be of size (BUFFER_SIZE + 1).  Therefore, if you have two
 * unique file descriptors, you must have two buffers that each have
 * been malloc()'d to size (BUFFER_SIZE + 1).
 *
 * Note that read_from_fd() makes a read() call and will block if the
 * fd does not have data ready to be read.  This function is complete
 * and does not need to be modified as part of this MP.
 *
 * @param fd
 *    File descriptor to read from.
 * @param buffer
 *    A unique buffer associated with the fd.  This buffer may have
 *    a partial key-value pair between calls to read_from_fd() and
 *    must not be modified outside the context of read_from_fd().
 * @param mr
 *    Pass-through mapreduce_t structure (to process_key_value()).
 *
 * @retval 1
 *    Data was available and was read successfully.
 * @retval 0
 *    The file descriptor fd has been closed, no more data to read.
 * @retval -1
 *    The call to read() produced an error.
 */
static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)
{
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	if (bytes_read == 0)
		return 0;
	else if(bytes_read < 0)
	{
		fprintf(stderr, "error in read.\n");
		return -1;
	}

	buffer[offset + bytes_read] = '\0';

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
	buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}


void * worker_func(void * arg)
{
	mapreduce_t * mr = (mapreduce_t*)arg;
	fd_set set1, set2;
	FD_ZERO(&set1);
	FD_ZERO(&set2);
	/*
	int epoll_fd = epoll_create(mr->size);
	struct epoll_event event[mr->size];
	
	memset(event, 0, (mr->size)*sizeof(struct epoll_event));
	*/
	int remain = mr->size;
	mr->buffer = malloc(sizeof(char*)*(mr->size));
	/*
	char ** buffer;
	buffer = malloc( sizeof(char*) * (mr->size));
	*/
	int i;
	for(i = 0; i < mr->size; i++){
		FD_SET(mr->pipe[i][0], &set1);
		mr->buffer[i] = malloc(sizeof(char) * (BUFFER_SIZE+1));
		mr->buffer[i][0] = '\0';
	/*
		buffer[i] = malloc( (BUFFER_SIZE+1) * sizeof(char));	
		buffer[i][0] = '\0';
		event[i].events = EPOLLIN;
		event[i].data.fd = mr->pipe[i][0];
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, mr->pipe[i][0], &event[i]);
		*/
	}
	
	i = 0;
	while(remain>0){
		set2 = set1;
		select(FD_SETSIZE, &set2, NULL, NULL, NULL);

		for(i = 0; i < mr->size; i++){
			//struct epoll_event ev;
			//epoll_wait(epoll_fd, &ev, 1, -1);
			if(FD_ISSET(mr->pipe[i][0], &set2)){
				int k = read_from_fd(mr->pipe[i][0], mr->buffer[i], mr);
			//int k = read_from_fd(ev.data.fd, buffer[i], mr);
			//i ++;
				if(k == 0){
					close(mr->pipe[i][0]);
					FD_CLR(mr->pipe[i][0], &set1);
				/*
					close(ev.data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev.data.fd, NULL);
					*/
					remain--;
				}
			}
		}
	}
/*
	for(i = 0; i < mr->size; i++){
		free(buffer[i]);
	}
	free(buffer);
	*/
	return NULL;
}

/**
 * Initialize the mapreduce data structure, given a map and a reduce
 * function pointer.
 */
void mapreduce_init(mapreduce_t *mr, 
                    void (*mymap)(int, const char *), 
                    const char *(*myreduce)(const char *, const char *))
{	
	//mr = malloc(sizeof(mapreduce_t));
	mr->mapfunc = mymap;
	mr->reducefunc = myreduce;
	mr->pipe = NULL;
	mr->size = 0;
	mr->ds = (datastore_t *)malloc(sizeof(datastore_t));
	datastore_init(mr->ds);

}


/**
 * Starts the map() processes for each value in the values array.
 * (See the MP description for full details.)
 */
void mapreduce_map_all(mapreduce_t *mr, const char **values)		//how to free the values using mr or not?
{

	int size;
	for(size=0; values[size] != 0 ; size++);
	
	mr->size = size;
	
	mr->pipe = (int **)malloc(sizeof(int *)*size);
	
	int i;
	for(i=0; i<size; i++){
		mr->pipe[i] = (int *)malloc(sizeof(int)*2);
	}
	
	
	for(i=0; i<size; i++){
		pipe(mr->pipe[i]);
		pid_t pid;
		if((pid=fork()) == 0){ 
			close(mr->pipe[i][0]); 
			mr->mapfunc(mr->pipe[i][1], values[i]);
			exit(0);
		}
		else{
			close(mr->pipe[i][1]);
		}
		
	}
	
	/*
	pid_t pid = fork();
	if(pid > 0){
		for(i = 0; i < mr->size; i++){
			close(mr->pipe[i][1]);
			
		}
		pthread_create(&tid, NULL, (void *)worker_func, (void *)mr);
	}
	else{
		for(i = 0; i < mr->size; i++){
			close(mr->pipe[i][0]);
			mr->mapfunc(mr->pipe[i][0], values[i]);
		}
	}
	*/
	pthread_create(&mr->tid, NULL, (void *)worker_func, (void *)mr);
}


/**
 * Blocks until all the reduce() operations have been completed.
 * (See the MP description for full details.)
 */
void mapreduce_reduce_all(mapreduce_t *mr)
{
	pthread_join(mr->tid, NULL);
}


/**
 * Gets the current value for a key.
 * (See the MP description for full details.)
 */
const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{
	unsigned long r;
    return datastore_get(mr->ds,(char *)result_key, &r);
}


/**
 * Destroys the mapreduce data structure
 */
void mapreduce_destroy(mapreduce_t *mr)
{
	int i;
	for(i=0; i<mr->size; i++){
	    free(mr->pipe[i]);
	    free(mr->buffer[i]);
	}
	free(mr->pipe);
	free(mr->buffer);
	datastore_destroy(mr->ds);
	free(mr->ds);
}
