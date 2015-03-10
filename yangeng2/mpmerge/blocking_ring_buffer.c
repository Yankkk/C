// Replace "Full name" and "netid" here with your name and netid

// Copyright (C) [Full name] ([netid]) 2015

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "blocking_ring_buffer.h"

task_t* queue[QUEUE_SIZE];
int in, out, count;

/* Adds the task to the queue. If the queue is full this call will block until space is available.
  enqueue 'NULL' when there are no more tasks to execute. */

void enqueue(task_t*task) {
 
}

/* Removes the next task from the queue. If there are currently no more items, this call blocks.
 Note if the removed item is null then the null remains on the queue for all future dequeue requests.
 Thus the null task is a poison-pill and can be used to terminate all remaining waiting threads.
*/
task_t* dequeue() {
  //The special NULL (finished) stays in the queue
  // Once NULL is returned, dequeue will never block and will always immediately return NULL.
  
  
  return NULL;
}