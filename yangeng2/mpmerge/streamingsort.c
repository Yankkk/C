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

pthread_mutex_t mm = PTHREAD_MUTEX_INITIALIZER;
task_t* mqueue[QUEUE_SIZE];
int qin, qout, qcount;

int qflag = 0;
pthread_mutex_t mq = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t qcv1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t qcv2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cvt = PTHREAD_COND_INITIALIZER;

int merge = 0;

//void* worker_funcs(void* arg);

task_t* dequeues();
void enqueues(task_t * task);


void menqueue(task_t*task) {
	pthread_mutex_lock(&mq);
	while(qcount == QUEUE_SIZE){
		pthread_cond_wait(&qcv1, &mq);
	}
	mqueue[(qin++) & (QUEUE_SIZE-1)] = task;
	qcount++;
	pthread_cond_broadcast(&qcv2);
	pthread_mutex_unlock(&mq);
}


task_t* mdequeue() {
  //The special NULL (finished) stays in the queue
  // Once NULL is returned, dequeue will never block and will always immediately return NULL.
  if(qflag == 1){
  	return NULL;
  }
  pthread_mutex_lock(&mq);
 
  while(qcount == 0){
  	pthread_cond_wait(&qcv2, &mq);
  }
  task_t* result = mqueue[(qout) & (QUEUE_SIZE-1)];
  if(result == NULL){
  	qflag = 1;
  	pthread_cond_broadcast(&qcv1);
  	pthread_mutex_unlock(&mq);
  	return NULL;
  }
  qout++;
  qcount--;
  pthread_cond_broadcast(&qcv1);
  pthread_mutex_unlock(&mq);
  return result;
}


static int compare_fns(const void *arg1, const void *arg2) {
  return (*((int*)arg1)) - (*((int*)arg2)); 
}


void create_task(task_t*parent, int start, int end) { 
  task_t* task = malloc(sizeof(task_t));
  task->start = start;
  task->end = end;
  task->completed_child_tasks = 0;
  task->parent = parent;
  
 // int mid = (end + start) / 2;
  int len = (end - start);
  /*
  int mid = ((len/256)/2 + (len/256)%2) * 256;
  */
  int mid = (end+start)/2;
  int re = mid % 256;
  if(re){
  	mid = mid - re + 256;
  }
  if(len > 256) {   //
    create_task(task,  start,  mid);
    create_task(task,  mid, end);
  } else {
    enqueue(task);
  }
}
void do_tasks(int*scratch, task_t* task);

void child_finisheds(int* scratch, task_t* task) {
  if(task == NULL) return;

  pthread_mutex_lock(&mm);
  int done = ++ (task->completed_child_tasks);
  pthread_mutex_unlock(&mm);

  assert(done>0 && done<3);
  
  if(done == 2) {
    do_tasks(scratch, task);
  }  
}

void do_tasks(int*scratch, task_t* task) {
  int start = task->start;
  int end = task->end;
  
  int len = end - start;
  //int mid = ((len/256)/2 + (len/256)%2)* 256;
	int mid = (end + start)/2;
  	int re = mid % 256;
  	if(re){
  		mid = mid - re + 256;
  	}
/*
  if(len <= 512){
  	mid = (end + start)/2;
  	int re = mid % 256;
  	if(re){
  		mid = mid - re + 256;
  	}
  }
  else{
  	mid = (end + start)/2;
  	int re = mid % 512;
  	if(re){
  		mid = mid - re + 512;
  	}
  }
*/
  if(len > 256){
  	simple_merge(data, scratch, start, mid, end);
  	if(verbose) 
     print_stat(data,start,end);
  }
  
   if((len < nitems) && task->parent != NULL){
   		child_finisheds(scratch, task->parent);
   }
  if(task->parent == NULL){
  	enqueue(NULL);
  }
  free(task);
}

void* worker_funcs(void* arg) {

  task_t * task;
  while( (task = mdequeue()))  {
    	int start = task->start;
    	int end = task->end;
    	int len = task->end - task->start;
    	qsort(data +start,len,sizeof(int), compare_fns);
    	if(verbose) 
    		print_stat(data,start,end);
    	free(task);
  }
 
  pthread_mutex_lock(&mt);
  if(!merge){
  	while(!merge){
  		pthread_cond_wait(&cvt, &mt);
  	}
  }
  else{
  	pthread_cond_broadcast(&cvt);
  }
  pthread_mutex_unlock(&mt);
  
  int * scratch = malloc(nitems * sizeof(int));
  while( (task = dequeue()))  {
    do_tasks(scratch, task);
  }
  free(scratch);
  return NULL;
}




/**
 * Stream-based fast sort. The stream sort may be faster because you can start processing the data
 * before it has been fully read.
 * Before data arrives, you can pre-create threads and prepared to send output to the given ile
 * @param nthreads number of threads to use including the calling thread (these should created once and reused for multiple tasks)
 * @param verbose whether to print to standard out the number of unique values for each merged segment
 * @param outfile outoutfile_nameput FILE descriptor (either stdout or a output file)
*/
void stream_init() {
  outfile = open_outfile(outfile_name);
  // do awesome stuff
  
  int i;

  for(i = 1; i < nthreads; i++){
  	pthread_create(&tid[i], NULL, worker_funcs, NULL);
  }
  data = malloc(16777216 * sizeof(int));
}



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
  int current = nitems;
  nitems+=count;
 // data = realloc(data, sizeof(int)*nitems);
  int i;
  for(i = 0; i < count; i++){
  	data[nitems-count+i] = buffer[i];
  }
  task_t * task = malloc(sizeof(task_t));
  task->completed_child_tasks = 0;
  task->start = current;
  task->end = nitems;
  task->parent = NULL;
  menqueue(task); 
}


/**
 * All data has been delivered. Your implementation should now finish writing out sorted data and verbose info.
* The output stream will be closed for you after this call returns
*/
void stream_end() {
// do awesome stuff
// then print to outfile e.g.

	menqueue(NULL);
	merge = 1;
    int i;
  	create_task(NULL, 0, nitems);
    worker_funcs(NULL);
    for(i = 1; i < nthreads; i++){
    	pthread_join(tid[i], NULL);
    }
   for(int i = 0; i < nitems ;i++) 
     fprintf(outfile,"%d\n", data[i]);
     
   if(outfile != stdout) 
     fclose(outfile);
}
