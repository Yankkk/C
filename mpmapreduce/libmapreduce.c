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

typedef struct block{
	const char * value;
	void (*mapfunc)(int, const char *);
	int fd;

}block;


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
	
	int remain = mr->size;
	mr->buffer = malloc(sizeof(char*)*(mr->size));
	
	int i;
	for(i = 0; i < mr->size; i++){
		//FD_SET(mr->pipe[i][0], &set1);
		mr->buffer[i] = malloc(sizeof(char) * (BUFFER_SIZE+1));
		mr->buffer[i][0] = '\0';
	
	}
	
	i = 0;
	int index = 0;
	while(remain>0){
		struct epoll_event event;
		epoll_wait(mr->epoll_fd, &event, 1, -1);

		for(i = 0; i < mr->size; i++){
			if(event.data.fd == mr->event[i].data.fd){
					index = i;
					break;
			}		
		}		
		int k = read_from_fd(event.data.fd, mr->buffer[index], mr);
			
	   if(k == 0){
			epoll_ctl(mr->epoll_fd, EPOLL_CTL_DEL, event.data.fd, NULL);
			remain--;
		}
	}
	return NULL;
}

void * worker_map(void * arg){
	block * temp = (block*)arg;
	//printf("%p\n", temp->mapfunc);
	temp->mapfunc(temp->fd, temp->value);
	close(temp->fd);
	free(temp);
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
	mr->mapfunc = mymap;
	mr->reducefunc = myreduce;
	mr->pipe = NULL;
	mr->size = 0;
	mr->ds = (datastore_t *)malloc(sizeof(datastore_t));
	datastore_init(mr->ds);
	mr->epoll_fd = 0;
	mr->event = NULL;

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
	mr->epoll_fd = epoll_create(size);
	mr->event = malloc(sizeof(struct epoll_event) * size);
	memset(mr->event, 0, (mr->size)*sizeof(struct epoll_event));
	int i;
	for(i=0; i<size; i++){
		mr->pipe[i] = (int *)malloc(sizeof(int)*2);
		pipe(mr->pipe[i]);
		
		mr->event[i].events = EPOLLIN;
		mr->event[i].data.fd = mr->pipe[i][0];
		epoll_ctl(mr->epoll_fd, EPOLL_CTL_ADD, mr->pipe[i][0], &mr->event[i]);
	}
	
	mr->pid = fork();
	if(mr->pid == 0){
		pthread_t tids[size];
		for(i = 0; i < mr->size; i++){
			close(mr->pipe[i][0]);
			block * temp = malloc(sizeof(block));
			temp->value = values[i];
			temp->mapfunc = mr->mapfunc;
		
			temp->fd = mr->pipe[i][1];
			pthread_create(&tids[i], NULL, (void *)worker_map, (void *)temp);
		}
		
		for(i = 0; i < mr->size; i++){
			pthread_join(tids[i], NULL);
		}
		exit(0);
	}
	else{
		for(i = 0; i < mr->size; i++){
			close(mr->pipe[i][1]);
		}
		pthread_create(&mr->tid, NULL, (void *)worker_func, (void *)mr);
	}
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
	free(mr->event);
	datastore_destroy(mr->ds);
	free(mr->ds);
}
