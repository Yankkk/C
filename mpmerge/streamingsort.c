// Replace "Full name" and "netid" here with your name and netid

// Copyright (C) [Yan Geng] ([yangeng2]) 2015

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "mpsortutil.h"

#define MAXTHREAD (16)

extern int nthreads, verbose; // defined in main.c
extern char * outfile_name;

static FILE * outfile;
static int* data;
static int nitems;
static int capacity;
static pthread_t tid[MAXTHREAD];

//pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
//int num = 0;

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
  /*
  int i;
  for(int i = 1; i < nthreads; i++){
  	pthread_create(&tid[i], NULL, worker_func, NULL);
  }
  worker_func(NULL);
  */
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
 /*
  memcpy(data, buffer, count*sizeof(int));
  
  if(nthreads == -1){
  	baseline_nonthreaded_mergesort(data, nitems);
  }
  else{
  	qsort(buffer,count, sizeof(int), compare_fn);
  	pthread_mutex_lock(&m);
  	num ++;
  	pthread_mutex_lock(&m);
  	if(num == 2){
  		merge();	
  	}
  }
  
  */
}


/**
 * All data has been delivered. Your implementation should now finish writing out sorted data and verbose info.
* The output stream will be closed for you after this call returns
*/
void stream_end() {
// do awesome stuff
// then print to outfile e.g.
	/*
  for(int i = 1; i < nthreads; i++){
	pthread_join(tid[i], NULL);
  }

  for(int i = 0; i < nitems;i++) 
     fprintf(outfile,"%d\n", data[i]);
     
  if(outfile != stdout) 
     fclose(outfile);
     */
}
