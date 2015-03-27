// Replace "Full name" and "netid" here with your name and netid

// Copyright (C) [Yan Geng] ([yangeng2]) 2015

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "mpsortutil.h"
#include "blocking_ring_buffer.h"

#define MAXTHREAD (16)
#define QUEUE_SIZE (64)

extern int nthreads, verbose; // defined in main.c
extern char * outfile_name;

static FILE * outfile;
static int* data;
static int nitems;
static int capacity;
static pthread_t tid[MAXTHREAD];




typedef struct _block {
 // int start, end;
  int * mydata;
  size_t size;
   
}block;

typedef struct _lock {
 pthread_mutex_t ms;
 pthread_cond_t cvs1;
 pthread_cond_t cvs2;  
} lock;

lock* lock1;
lock* lock2;
//void do_tasks(int*, task_t* );

block* queue_s[QUEUE_SIZE];
block* queue_m[QUEUE_SIZE];

pthread_mutex_t mm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mm2 = PTHREAD_MUTEX_INITIALIZER;

int ins[2];
int outs[2];
int counts[2];

int F;
int done;
int flags[2];

void* worker_funcs(void* arg);

block* dequeues(block ** queue, lock * locks, int i);
void enqueues(block* task, block** queue, lock * locks, int i);

static int compare_fns(const void *arg1, const void *arg2) {
  return (*((int*)arg1)) - (*((int*)arg2)); 
}



void enqueues(block* task, block** queue, lock * locks, int i) {
	pthread_mutex_lock(&locks->ms);
	while(counts[i] == QUEUE_SIZE){
		pthread_cond_wait(&locks->cvs1, &locks->ms);
	}
	queue[(ins[i]++) & (QUEUE_SIZE-1)] = task;
	counts[i]++;
	pthread_cond_broadcast(&locks->cvs2);
	pthread_mutex_unlock(&locks->ms);
}

block* dequeues(block** queue, lock * locks, int i) {
  if(flags[i] == 1){
  	return NULL;
  }
  pthread_mutex_lock(&locks->ms);
 
  while(counts[i] == 0){
  	pthread_cond_wait(&locks->cvs2, &locks->ms);
  }
  block* result = queue[(outs[i]) & (QUEUE_SIZE-1)];
  if(result == NULL){
  	flags[i] = 1;
  	pthread_cond_broadcast(&locks->cvs1);
  	pthread_mutex_unlock(&locks->ms);
  	return NULL;
  }
  outs[i]++;
  counts[i]--;
  pthread_cond_broadcast(&locks->cvs1);
  pthread_mutex_unlock(&locks->ms);
  return result;
}


/**
 * Stream-based fast sort. The stream sort may be faster because you can start processing the data
 * before it has been fully read.
 * Before data arrives, you can pre-create threads and prepared to send output to the given ile
 * @param nthreads number of threads to use including the calling thread (these should created once and reused for multiple tasks)
 * @param verbose whether to print to standard out the number of unique values for each merged segment
 * @param outfile output FILE descriptor (either stdout or a output file)
*/
void stream_init() {
  outfile = open_outfile(outfile_name);
  // do awesome stuff
  lock1 = malloc(sizeof(lock));
  pthread_mutex_init(&lock1->ms, NULL);
  pthread_cond_init(&lock1->cvs1, NULL);
  pthread_cond_init(&lock1->cvs2, NULL); 
  lock2 = malloc(sizeof(lock));
  pthread_mutex_init(&lock2->ms, NULL);
  pthread_cond_init(&lock2->cvs1, NULL);
  pthread_cond_init(&lock2->cvs2, NULL); 
  
  int i;

  for(i = 1; i < nthreads; i++){
  	pthread_create(&tid[i], NULL, worker_funcs, NULL);
  }
  worker_funcs(NULL);
  
}


void* worker_funcs(void* arg) {
  block * b;
  while( (b = dequeues(queue_s, lock1, 0) ))  {
    qsort(b->mydata, b->size, sizeof(int), compare_fns);   
  	enqueues(b, queue_m, lock2, 1); 
  	pthread_mutex_lock(&mm2);
  	done ++;
  	pthread_mutex_unlock(&mm2);
  }
  
  while(done >= 2){
  	pthread_mutex_lock(&mm2);
  	done-=2;
    pthread_mutex_unlock(&mm2);
  	block * a =  dequeues(queue_m, lock2, 1);
  	block * c =  dequeues(queue_m, lock2, 1);
  	int midpt = a->size;
  	int start = 0;
  	int end = a->size + c->size;
    block * d = malloc(sizeof(block));
    d->size = end;
    d->mydata = malloc(sizeof(block)*end);
    int i;
    for(i=0; i<a->size ; i++){
       d->mydata[i] = a->mydata[i];
       }
    for(i = a->size; i<end; i++){
    	d->mydata[i] = c->mydata[i-a->size];
    }
    int * scratch = (int*)malloc(sizeof(int)*(d->size));
  	simple_merge(d->mydata, scratch, start, midpt, end);
  	free(scratch);
  	if((d->size == nitems) && F){
  		enqueues(NULL, queue_m, lock2, 1);
  		data = d->mydata;
  		//for(i = 0, i )
  		if(verbose) 
           print_stat(data,start,end);
  	}
  	else{
  		enqueues(b, queue_m, lock2, 1);
  		pthread_mutex_lock(&mm2);
  	    done++;
        pthread_mutex_unlock(&mm2);
  	}
  }
   
  
  return NULL;
}

/*
void do_tasks(block* b) {

  int midpt = (b->size)/2;
  int len = b->size;
  int start = 0;
  int end = b->size;
  
  if(len <= 256){
  	qsort(b->mydata, len, sizeof(int), compare_fns);   
  	enqueue(b, queue_m, 1); 
  	pthread_mutex_lock(&mm2);
  	done ++;
  	pthread_mutex_unlock(&mm2);
  }
  else{
  	int * scratch = (int*)malloc(sizeof(int)*len);
  	simple_merge(b->mydata, scratch, start, midpt, end)
  	free(scratch);
  	if((len == nitems) && F){
  		enqueue(NULL, queue_m, 1);
  		for
  	}
  	else{
  		enqueue(b, queue_m, 1);
  	}
  
  }
  free(b);
}


*/

/**
 * Additional data has arrived and is ready to be processed in the buffer. 
 * This function may be called multiple times (between stream_init() and stream_en()d).
 * The buffer is re-used for new data, you will need to copy / process the data before returning from this method.
 * @param buffer pointer to the buffer. The buffer contents is only valid for the duration of the call.
 * @param count the number of items in the buffer (256 >= count > 0). This may be less than 256 for the last segment.
*/

void stream_data(int* buffer, int count) {
  // You can already start sorting before the data is fully read into memory
  // do awesome stuff
 
  block * b = malloc(sizeof(block));
  b->size = count;
  b->mydata = malloc(sizeof(int)*count);
  int i;
  for(i=0; i<count ; i++)
    b->mydata[i] = buffer[i];
    
  pthread_mutex_lock(&mm);
  nitems += count;
  pthread_mutex_unlock(&mm);
  
  enqueues(b, queue_s, lock1, 0);

}


/**
 * All data has been delivered. Your implementation should now finish writing out sorted data and verbose info.
* The output stream will be closed for you after this call returns
*/
void stream_end() {
// do awesome stuff
// then print to outfile e.g.

  enqueues(NULL, queue_s, lock1, 0);
  F = 1;
  for(int i = 1; i < nthreads; i++){
	pthread_join(tid[i], NULL);
  }

  for(int i = 0; i < nitems;i++) 
     fprintf(outfile,"%d\n", data[i]);
     
  if(outfile != stdout) 
     fclose(outfile);
    
}
