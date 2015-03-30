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



//void* worker_funcs(void* arg);

//block* dequeues(block ** queue, lock * locks, int i);
//void enqueues(block* task, block** queue, lock * locks, int i);

static int compare_fns(const void *arg1, const void *arg2) {
  return (*((int*)arg1)) - (*((int*)arg2)); 
}



void create_task(task_t*parent, int start, int end) { 
  task_t* task = malloc(sizeof(task_t));
  task->start = start;
  task->end = end;
  task->completed_child_tasks = 0;
  task->parent = parent;
  
  int mid = (end + start) / 2;
  int len = (end - start);
  if(len > 256) {
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
  
  int midpt = (start + end)/2;
  int len = end - start;
  
  if(len <=256) {
    qsort(data +start,len,sizeof(int), compare_fns);    
  } else {
    simple_merge(data, scratch, start, midpt, end);    
  }
  if(verbose) 
     print_stat(data,start,end);
     
   if(len < nitems){
   		child_finisheds(scratch, task->parent);
   }
    free(task);

  if(len == nitems){
  	enqueue(NULL);
  }
}

void* worker_funcs(void* arg) {

  int * scratch = (int *)malloc(1);
  task_t * task;

  while( (task = dequeue()))  {
  	scratch = (int *)realloc(scratch, sizeof(int)*nitems);
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
  data = malloc(1);
  
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
  nitems+=count;
  data = realloc(data, sizeof(int)*nitems);
  int i;
  for(i = 0; i < count; i++){
  	data[nitems-count+i] = buffer[i];
  }
}


/**
 * All data has been delivered. Your implementation should now finish writing out sorted data and verbose info.
* The output stream will be closed for you after this call returns
*/
void stream_end() {
// do awesome stuff
// then print to outfile e.g.

	create_task(NULL, 0, nitems);
    worker_funcs(NULL);
    int i;
    for(i = 1; i < nthreads; i++){
    	pthread_join(tid[i], NULL);
    }
    //printf("%d\n", data[0]);
    //print_result(outfile_name, data, nitems);
   
   for(int i = 0; i < nitems ;i++) 
     fprintf(outfile,"%d\n", data[i]);
     
   if(outfile != stdout) 
     fclose(outfile);
   // free(data);
}
