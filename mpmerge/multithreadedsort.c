// Replace "Full name" and "netid" here with your name and netid

// Copyright (C) [Full name] ([netid]) 2015

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "mpsortutil.h"
#include "blocking_ring_buffer.h"

/*
A reasonably fast queue-based solution.
*/
#define MAXTHREAD (16)
#define MSG(msg) fprintf(stderr,"%s\n",msg);

// Defined in main.c
extern int verbose;
extern int nthreads;


static int nitems;  
static int *data;
static pthread_t tid[MAXTHREAD];
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;


static int compare_fn(const void *arg1, const void *arg2) {
  return (*((int*)arg1)) - (*((int*)arg2)); 
}


/** Create all of the tasks required for this mergesort.
 Only the base-case (smallest) tasks are added to the queue.
 */
void create_tasks(task_t*parent, int start, int end) { 
  task_t* task = malloc(sizeof(task_t));
  task->start = start;
  task->end = end;
  task->completed_child_tasks = 0;
  task->parent = parent;
  
  int mid = (end + start) / 2;
  int len = (end - start);
  if(len > 256) {
    create_tasks(task,  start,  mid);
    create_tasks(task,  mid, end);
  } else {
    // base case tasks are ready to run, so add them to the queue
    enqueue(task);
  }
}
void do_task(int*scratch, task_t* task);

void child_finished(int* scratch, task_t* task) {
  if(task == NULL) return;
  // Notify the parent task that a child has finished
  // If two child tasks have completed then we can
  // now merge the two merged halves back together
  // rather than enqueue this parent task,
  // we just do it immediately
  // (avoiding a potential deadlock if the queue is too small)
  
  // THIS IS A CRITICAL SECTION (WHY?)
  // How will you protect it?
  int done = ++ (task->completed_child_tasks);


  assert(done>0 && done<3);
  
  if(done == 2) {
    // don't enqueue the parent task, just do it!
    // If we did enqueu then we can hit deadlock if the queue is full
    do_task(scratch, task);
  }  
}

void do_task(int*scratch, task_t* task) {
  int start = task->start;
  int end = task->end;
  
  int midpt = (start + end)/2;
  int len = end - start;
  
  if(len <=256) {
    qsort(data +start,len,sizeof(int), compare_fn);    
  } else {
    simple_merge(MISSING PARAMETERS);    
  }
  if(verbose) 
     print_stat(data,start,end);
     
   MISSING CODE HERE - tell the parent task that we have finished
   
    
  // Did we just sort all items? If so, enqueue the poison-pill,
  // enqueue the NULL task to tell all threads to quit
  
  MISSING CODE HERE - enqueue NULL if that was the largest possible merge
  
}

void* worker_func(void* arg) {
  // Each thread has its own private scratch memory
  // used for merging
  
  MISSING CODE HERE
  Create some per-thread scratch space. But how much! What is the largest we 
  would need for the lifetime of this thread?
  
  // How does the following loop work?
  
  // Each thread goes back to the queue to get the next piece of work
  // We quit when we pull the magic null task
  
  task_t * task;
  while( (task = dequeue()) )  {
    do_task(scratch, task);
  }
  
  free(scratch);
  return NULL;
}


void my_threaded_mergesort(int* _data,int _nitems) {
  
  data = _data;
  nitems = _nitems;

  MISSING CODE
  Create nthread-1 threads that call worker_func 
  
  create_tasks(NULL,  0, nitems );

  worker_func( NULL ); // WHY?
  
  // Why i = 1??
  for(int i=1;i < nthreads; i++) { 
    MISSING CODE; WAIT FOR OTHER THREADS TO FINISH
  }
  
}