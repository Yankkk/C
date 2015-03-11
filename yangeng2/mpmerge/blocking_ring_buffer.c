// Replace "Full name" and "netid" here with your name and netid

// Copyright (C) [Yan Geng] ([yangeng2]) 2015

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "blocking_ring_buffer.h"

task_t* queue[QUEUE_SIZE];
int in, out, count;
int in = 0, out = 0, count = 0;
int flag = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cv2 = PTHREAD_COND_INITIALIZER;

/* Adds the task to the queue. If the queue is full this call will block until space is available.
  enqueue 'NULL' when there are no more tasks to execute. */

void enqueue(task_t*task) {
	pthread_mutex_lock(&m);
	while(count == QUEUE_SIZE){
		pthread_cond_wait(&cv1, &m);
	}
	queue[(in++) & (QUEUE_SIZE-1)] = task;
	count++;
	pthread_cond_broadcast(&cv2);
	pthread_mutex_unlock(&m);
}

/* Removes the next task from the queue. If there are currently no more items, this call blocks.
 Note if the removed item is null then the null remains on the queue for all future dequeue requests.
 Thus the null task is a poison-pill and can be used to terminate all remaining waiting threads.
*/
task_t* dequeue() {
  //The special NULL (finished) stays in the queue
  // Once NULL is returned, dequeue will never block and will always immediately return NULL.
  if(flag == 1){
  	return NULL;
  }
  pthread_mutex_lock(&m);
 
  while(count == 0){
  	pthread_cond_wait(&cv2, &m);
  }
  task_t* result = queue[(out) & (QUEUE_SIZE-1)];
  if(result == NULL){
  	flag = 1;
  	pthread_cond_broadcast(&cv1);
  	pthread_mutex_unlock(&m);
  	return NULL;
  }
  out++;
  count--;
  pthread_cond_broadcast(&cv1);
  pthread_mutex_unlock(&m);
  return result;
}
